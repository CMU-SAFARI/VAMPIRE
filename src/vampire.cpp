/*

VAMPIRE.CPP

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#include <algorithm>
#include "vampire.h"
#include "command.h"

// Constructor
Vampire::Vampire() {
    init_lambdas();
    init_latencies();
}

// Destructor
Vampire::~Vampire() {
    free_structures[int(traceType)]();

    delete configs;
    delete parser;

    delete traceFilename;
    delete configFilename;
    delete dramSpecFilename;

    delete dramSpec;
    delete dramStruct;
    delete statistics;
    delete equations;

    delete dist;
}

/*
 * Initializes all the lambda arrays declared in src/vampire.h, these lambda arrays are arrays of functions for different
 * tasks like calculating # of set/toggle bits & initializing data structures. Each element of these arrays corresponds to
 * one of the traceType, thus only one of the element of each array is called during runtime.
 */
void Vampire::init_lambdas(){
    /*****************************************************************************************/
    /* Create lambdas which allocates memory for all the data structures used by a TraceType */
    /*****************************************************************************************/
    init_structures[int(TraceType::WR)] = [this] () -> void {
        unsigned long channel, rank, bank, row, col;
        memory = (DRAMdata *****) malloc(sizeof(class DRAMdata ****) * configs->getNumChannels());

        for (channel = 0; channel < configs->getNumChannels(); channel++){
            memory[channel] = (DRAMdata ****) malloc(sizeof(class DRAMdata ***) * configs->getNumRanks());
        }
        for (channel = 0; channel < configs->getNumChannels(); channel++){
            for (rank = 0; rank < configs->getNumRanks(); rank++){
                memory[channel][rank] = (DRAMdata ***) malloc(sizeof(class DRAMdata **) * configs->getNumBanks());
            }
        }
        for (channel = 0; channel < configs->getNumChannels(); channel++){
            for (rank = 0; rank < configs->getNumRanks(); rank++){
                for (bank = 0; bank < configs->getNumBanks(); bank++){
                    memory[channel][rank][bank] = (DRAMdata **) malloc(sizeof(class DRAMdata *) * configs->getNumRows());
                }
            }
        }
        for (channel = 0; channel < configs->getNumChannels(); channel++){
            for (rank = 0; rank < configs->getNumRanks(); rank++){
                for (bank = 0; bank < configs->getNumBanks(); bank++){
                    for (row = 0; row < configs->getNumRows(); row++){
                        memory[channel][rank][bank][row] = (DRAMdata *) malloc(sizeof(class DRAMdata) * configs->getNumCols());
                    }
                }
            }
        }
        for (channel = 0; channel < configs->getNumChannels(); channel++){
            for (rank = 0; rank < configs->getNumRanks(); rank++){
                for (bank = 0; bank < configs->getNumBanks(); bank++){
                    for (row = 0; row < configs->getNumRows(); row++){
                        for (col = 0; col < configs->getNumCols(); col++){
                            memory[channel][rank][bank][row][col].init_values();
                        }
                    }
                }
            }
        }
    };
    init_structures[int(TraceType::RD_WR)] = [this] () -> void {
        return;
    };
    init_structures[int(TraceType::DIST)] = [this] () -> void {
        /* NOTE: These arrays are described in the default configuration file (config/default.cfg) */

        /***********************
        * Set bit handling     *
        ***********************/
        // Initialize the set bit array
        numOfSetBits = new std::vector<unsigned short>((unsigned long)(NUM_OF_BITS+1)*this->configs->getSetBitArrSizeMult());

        if (!configs->contains(DIST_SET_S)) {
            std::cout << RED
                 << "Error: No distribution of set bits supplied or config file missing."
                 << RESET << std::endl
                 << "Use 'vampire --help' for list of options."
                 << std::endl;
            exit(EX_CONFIG);
        }

        int setBitArrPtr = 0;
        // Standard probability checks for each of the elements of the setBitsDist array; P(a[i]) <= 1
        // and Count(a) < NUM_OF_BITS+1. Where 'a' is the array of probability distribution read from the config file
        for (unsigned short i = 0; i < configs->getSetBitDist()->size(); i++) {
            msg::error(configs->getSetBitDist()->operator[](i) > 1, "Probability cannot be greater than 1");
            msg::error((int)(setBitArrPtr-1) > (int)numOfSetBits->size(), "Distribution array bound check failed");

            auto elementsWithSetBitI = (int)(configs->getSetBitDist()->operator[](i) * this->configs->getSetBitArrSizeMult()*(NUM_OF_BITS+1));
            for (int j = 0; j < elementsWithSetBitI; j++, setBitArrPtr++) {
                numOfSetBits->operator[]((unsigned long)setBitArrPtr) = i;
            }
        }

        // Fill in with random values for rest of the elements and print a warning
        if (setBitArrPtr < (NUM_OF_BITS+1)*NUM_OF_SET_BIT_ARR_MULT) {
            msg::warning("Sum of probabilities for setBitArr is not 1, rest of the "
                         + std::to_string((NUM_OF_BITS+1)*NUM_OF_SET_BIT_ARR_MULT - setBitArrPtr)
                         + " values will be initialized randomly.");

            for (; setBitArrPtr < (NUM_OF_BITS+1)*NUM_OF_SET_BIT_ARR_MULT; setBitArrPtr++) {
                numOfSetBits->operator[]((unsigned long)setBitArrPtr) = (unsigned short)(rand() % (NUM_OF_BITS));
            }
        }

        /***********************
         * Toggle bit handling *
         ***********************/
        // NOTE: Working similar to set bit array
        numOfToggleBits = new std::vector<unsigned short>((unsigned long)(NUM_OF_BITS+1)*this->configs->getToggleArrSizeMult());

        if (!configs->contains(DIST_SET_S)) {
            msg::error("Error: No distribution of toggle bits supplied or config file missing. Use 'vampire --help' for list of options.");
        }

        int toggleBitArrPtr = 0;

        for (unsigned short i = 0; i < configs->getToggleDist()->size(); i++) {
            msg::error(configs->getSetBitDist()->operator[](i) > 1, "setBitArr: Probability cannot be greater than 1");
            msg::error(toggleBitArrPtr-1 > (NUM_OF_BITS+1)*NUM_OF_SET_BIT_ARR_MULT, "Distribution array bound check failed");

            auto elementsWithSetBitI = (int)(configs->getToggleDist()->operator[](i) * this->configs->getToggleArrSizeMult()*(NUM_OF_BITS+1));
            for (int j = 0; j < elementsWithSetBitI; j++, toggleBitArrPtr++) {
                numOfToggleBits->operator[]((unsigned long)toggleBitArrPtr) = i;
            }
        }

        // Fill in with random values for rest of the elements and print a warning
        if (toggleBitArrPtr < (NUM_OF_BITS+1)*TOGGLE_BIT_ARR_MULT) {
            msg::warning("Sum of probabilities for toggleBitArr is not 1, rest of the "
                       + std::to_string((NUM_OF_BITS+1)*TOGGLE_BIT_ARR_MULT - toggleBitArrPtr)
                       + " values will be initialized randomly.");

            for (; toggleBitArrPtr < (NUM_OF_BITS+1)*TOGGLE_BIT_ARR_MULT; toggleBitArrPtr++) {
                numOfToggleBits->operator[]((unsigned long)toggleBitArrPtr) = (unsigned short)(rand() % (NUM_OF_BITS));
            }
        }
    };
    init_structures[int(TraceType::MEAN)] = [this] () -> void {

    };

    /*************************************************************************/
    /* Initialize function to calculate # of set bits depending on traceType */
    /*************************************************************************/
    getSetBits[int(TraceType::MEAN)] = [this] (unsigned int data[16]) -> unsigned int {
        static_cast<void>(data); // Avoids compiler warning -Wunused-parameter

        return (unsigned int) configs->getAvgNumSetBits();
    };
    getSetBits[int(TraceType::DIST)] = [this] (unsigned int data[16]) -> unsigned int {
        static_cast<void>(data); // Avoids compiler warning -Wunused-parameter

        // Pick the value from the array based on a rnd number, probability of a
        // value being picked is proportional to its frequency
        msg::warning(RAND_MAX > rand()%((NUM_OF_BITS+1) * configs->getSetBitArrSizeMult()),
                   "Maximum random number that can be generated exceeds the length of the array used for internal representation of setBitArr,"
                   " consider reducing the value of '" + AVG_SET_BITS_S + "'");
        unsigned int result = numOfSetBits->operator[]((unsigned long)(rand()%((NUM_OF_BITS+1) * configs->getSetBitArrSizeMult())));
        dist->operator[](result)++;
        return result;
    };
    getSetBits[int(TraceType::WR)] = [this] (unsigned int data[16]) -> unsigned int {
        unsigned int count = 0;

        for (int fourbytes = 0; fourbytes < 16; fourbytes++){
            count += __builtin_popcount(data[fourbytes]);
        }
        return count;
    };
    getSetBits[int(TraceType::RD_WR)] = [this] (unsigned int data[16]) -> unsigned int {
        return getSetBits[int(TraceType::WR)](data); // Same as
    };
    
    /****************************************************************************/
    /* Initialize function to calculate # of toggle bits depending on traceType */
    /****************************************************************************/
    getToggleBits[int(TraceType::WR)] = [this] (unsigned int new_data[16], unsigned int old_data[16])
            -> unsigned int {
        unsigned int toggle_count = 0;

        for(int fourbytes = 0; fourbytes < 16; fourbytes++){
            toggle_count += __builtin_popcount(old_data[fourbytes] ^ new_data[fourbytes]);
        }
        return toggle_count;
    };
    getToggleBits[int(TraceType::RD_WR)] = [this] (unsigned int new_data[16], unsigned int old_data[16])
            -> unsigned int {
        return getToggleBits[int(TraceType::WR)](new_data, old_data);
    };
    getToggleBits[int(TraceType::MEAN)] = [this] (unsigned int new_data[16], unsigned int old_data[16])
            -> unsigned int {
        static_cast<void>(new_data); // Avoids compiler warning -Wunused-parameter
        static_cast<void>(old_data); // Avoids compiler warning -Wunused-parameter

        return (unsigned int) configs->getAvgNumToggleBits();
    };
    getToggleBits[int(TraceType::DIST)] = [this] (unsigned int new_data[16], unsigned int old_data[16])
            -> unsigned int {
        static_cast<void>(new_data); // Avoids compiler warning -Wunused-parameter
        static_cast<void>(old_data); // Avoids compiler warning -Wunused-parameter

        // Pick the value from the array based on a rnd number, probability of a
        // value being picked is proportional to its frequency
        return numOfToggleBits->operator[](rand()%((NUM_OF_BITS+1) * configs->getSetBitArrSizeMult()));
    };

    /***********************************************************************************/
    /* Initializes the lambdas which frees up data structures created by init_struct() */
    /***********************************************************************************/
    free_structures[int(TraceType::WR)] = [this] () {
        unsigned long channel, rank, bank, row;

        for (channel = 0; channel < configs->getNumChannels(); channel++){
            for (rank = 0; rank < configs->getNumRanks(); rank++){
                for (bank = 0; bank < configs->getNumBanks(); bank++){
                    for (row = 0; row < configs->getNumRows(); row++){
                        free(memory[channel][rank][bank][row]);
                    }
                    free(memory[channel][rank][bank]);
                }
                free(memory[channel][rank]);
            }
            free(memory[channel]);
        }
        free(memory);
    };
    free_structures[int(TraceType::RD_WR)] = [this] () {
        return;
    };
    free_structures[int(TraceType::MEAN)] = [this] () {
        return;
    };
    free_structures[int(TraceType::DIST)] = [this] () {
        delete numOfSetBits;
        delete numOfToggleBits;
    };
}

void Vampire::init_latencies() {
/*    // RD, WR, ACT, PRE
    latency[int(VendorType::A)].assign({
            25.0, 25.0, 15.0, 15.0 //ns
    });
    latency[int(VendorType::B)].assign({
            25.0, 25.0, 15.0, 15.0 //ns
    });
    latency[int(VendorType::C)].assign({
            23.125, 23.125, 13.125, 13.125 //ns
    });*/
}

/* Intializes all the objects used in VAMPIRE class. Called after parsing config file and command line parameters */
int Vampire::set_values(){
    dist = new std::vector<int>(NUM_OF_BITS+1);
    this->configs = new Config(*configFilename); // Parse the config file

    /* Init IO_buffer */
    IO_buffer.init_values();
    
    /* Initialize parser to parse the trace file */
    if (parserType == ParserType::ASCII)
        parser = new AsciiParser();
    else if (parserType == ParserType::BINARY)
        parser = new BinParser();
    else
        msg::error("Unkonwn parser type.");

    if (traceFilename == nullptr) {
        msg::error("No trace found, please specify a trace file. See 'vampire --help' for more details.");
    }

    parser->setFilename(*traceFilename);
    parser->setTraceType(traceType);

    /* Initialize all the vendor specific info */
    switch (int(vendorType)) {
        case (int(VendorType::A)): {
            dramSpec = new DramSpec_A();
            break;
        }
        case (int(VendorType::B)): {
            dramSpec = new DramSpec_B();
            break;
        }
        case (int(VendorType::C)): {
            dramSpec = new DramSpec_C();
            break;
        }
        case (int(VendorType::Cust)) : {
            if (dramSpecFilename == nullptr) {
                msg::error("No dramSpec file specified with Cust Vender, see vampire --help for more details.");
            }
            dramSpec = new DramSpec_Cust(*dramSpecFilename);
        }
    }

    /* Initialize all the statistics */
    statistics = new Statistics(configs->structCount, this->csvFilename);
    equations = new Equations(*statistics, *dramSpec, *configs, vendorType, traceType, &memory, structVar);
    dramStruct = new DramStruct();
    return 0;
}

/* Function to find and add energy consumed by a request. Returns 0 if sucessful.
   Else returns -1*/
int Vampire::service_request(int encoded, Command cmd) {
    msg::error(cmd.finishTime == 0, "Finish time is 0.");

    uint32_t fourbytes;

    Helper::verify_add(cmd.type, cmd.add, *configs);

    auto &cmdLength             =  dramSpec->getCmdLength();
    auto &cmdCurrent            =  dramSpec->getCmdCurrent();
    auto &memClkSpeed           =  dramSpec->memClkSpeed;

    auto &totalReadEnergy       = *statistics->totalReadEnergy;
    auto &totalWriteEnergy      = *statistics->totalWriteEnergy;
    auto &totalActCmdEnergy     = *statistics->totalActCmdEnergy;
    auto &totalPreCmdEnergy     = *statistics->totalPreCmdEnergy;
    auto &totalActiveEnergy     = *statistics->totalActiveStandbyEnergy;
    auto &totalPrechargeEnergy  = *statistics->totalPrechargeStandbyEnergy;

    auto &totalActStandbyCycles = *statistics->totalActStandbyCycles;
    auto &totalPreStandbyCycles = *statistics->totalPreStandbyCycles;

    auto &vdd                   = dramSpec->vdd;

    auto cmdLengthInCycles = [&] (CommandType cmdType) -> uint64_t {
        return static_cast<uint64_t>(
                cmdLength[static_cast<uint64_t>(cmdType)]*memClkSpeed*0.001+0.5); // +0.5 for rounding off
    };

    this->currentTime = cmd.issueTime;

    /* Calculate # of cycles for idle state for the current bank */
    bool allBankPrecharged = true;

    auto timeDiff = (currentTime > lastStandbyEnergyEvalTime) ? currentTime - lastStandbyEnergyEvalTime : 0;
    /* Cycle through all the banks to check if anyone of them was open */
    for (uint64_t i = 0; i < dramStruct->banks->size(); i++) {
        auto &bank = dramStruct->banks->operator[](i);
        auto &bankState = dramStruct->bankStates->operator[](i);

        if (timeDiff && dramStruct->bankStates->operator[](i) == State::OPEN)
            allBankPrecharged = false;
    }

    dbgstream
            << "currentTime: " << currentTime
            << ", timeDiff: " << timeDiff << std::endl;

    if (allBankPrecharged) {
        totalPrechargeEnergy += timeDiff*dramSpec->preStandbyEnergy;
        totalPreStandbyCycles += timeDiff;
    } else {
        totalActiveEnergy += timeDiff*dramSpec->actStandbyEnergy;
        totalActStandbyCycles += timeDiff;
    }

    this->lastStandbyEnergyEvalTime = this->currentTime;

#ifdef GLOBAL_DEBUG
    std::stringstream ss;
    ss << "Request: " << commandString[int(cmd.type)] << " issueTime: " << cmd.issueTime <<  " Bank: " << cmd.add.bank << " Row: " << cmd.add.row << " Col: " << cmd.add.col;
    msg::info(ss.str());
#endif

    /* Calculate everything for the next request */
    // TODO: Refactor - split into multiple methods
    switch (int(cmd.type)) {
        case (int(CommandType::RD)): {
            msg::warning(dramStruct->bankStates->operator[](cmd.add.bank) != State::OPEN,
                       "RD/RDA called on closed bank at time: " + std::to_string(cmd.issueTime));

            /* Gets the # of set and toggle bits using the lambda corresponding to the current traceType */
            auto numOfSetBits = getSetBits[int(traceType)](cmd.data);
            auto numOfToggleBits = getToggleBits[int(traceType)](cmd.data, IO_buffer.data);

            // Correct row address of the command since it isn't read from the trace for a PRE
            cmd.add.row = dramStruct->banks->operator[](cmd.add.bank)->actRowNum;

            double_t cmdRdEnergy = equations->calc_rd_wr_energy(
                    CommandType::RD,
                    cmd.add,
                    cmd.data,
                    numOfSetBits,
                    numOfToggleBits,
                    IO_buffer,
                    encodingType
            ) - dramSpec->actStandbyEnergy * dramSpec->cmdLengthInCycles(CommandType::RD);

            dbgstream
                    << ", cmdLengthInCycles: " << dramSpec->cmdLengthInCycles(CommandType::RD)
                    << ", cmdRdEnergy: " << equations->calc_rd_wr_energy(CommandType::RD, cmd.add, cmd.data, numOfSetBits, numOfToggleBits, IO_buffer, encodingType)
                    << ", cmdRdEnergy - bgEnergy" << cmdRdEnergy
                    << ", RDCmdLength: " << dramSpec->cmdLengthInCycles(CommandType::RD)
                    << ", cmdLength: " << cmdLength[static_cast<uint64_t>(CommandType::RD)]
                    << ", memClkSpeed: " << dramSpec->memClkSpeed;


            cmdRdEnergy /= 2; // Current model uses the standard IDD4 loops which have RD/WR energies of two command
            if (structVar == StructVar::YES)
                totalReadEnergy += cmdRdEnergy * equations->struct_var_power_adjustment(CommandType::RD, cmd.add);
            else
                totalReadEnergy += cmdRdEnergy;

            /* Update IO_buffer/Memory's state */
            if (traceType == TraceType::WR) {
                for (fourbytes = 0; fourbytes < 16; fourbytes++) {
                    cmd.data[fourbytes]       = memory[cmd.add.channel][cmd.add.rank][cmd.add.bank][cmd.add.row][cmd.add.col].data[fourbytes];
                    IO_buffer.data[fourbytes] = memory[cmd.add.channel][cmd.add.rank][cmd.add.bank][cmd.add.row][cmd.add.col].data[fourbytes];
                    encoded                   = memory[cmd.add.channel][cmd.add.rank][cmd.add.bank][cmd.add.row][cmd.add.col].encoded;
                }
            } else if (traceType == TraceType::RD_WR) {
                for (fourbytes = 0; fourbytes < 16; fourbytes++) {
                    IO_buffer.data[fourbytes] = cmd.data[fourbytes];
                }
            }

            IO_buffer.prevAdd.bank = cmd.add.bank;
            IO_buffer.prevAdd.col = cmd.add.col;

            dramStruct->banks->operator[](cmd.add.bank)->cmdEndTime = currentTime + cmdLengthInCycles(cmd.type);
            break;
        }
        case (int(CommandType::WR)): {
            bool isBankClosed = dramStruct->bankStates->operator[](cmd.add.bank) != State::OPEN;
            msg::warning(isBankClosed, "WR/WRA called on closed bank");

            /* Gets the # of set and toggle bits using the lambda corresponding to the current traceType */
            auto numOfSetBits = getSetBits[int(traceType)](cmd.data);
            auto numOfToggleBits = getToggleBits[int(traceType)](cmd.data, IO_buffer.data);

            auto cmdWriteEnergy = equations->calc_rd_wr_energy(
                    CommandType::WR,
                    cmd.add,
                    cmd.data,
                    numOfSetBits,
                    numOfToggleBits,
                    IO_buffer,
                    encodingType
            ) - dramSpec->actStandbyEnergy * dramSpec->getCmdLength()[int(CommandType::WR)];
            cmdWriteEnergy /= 2; // Current model uses the standard IDD4 loops which have RD/WR energies of two command

            if (structVar == StructVar::YES) {
                totalWriteEnergy += cmdWriteEnergy * equations->struct_var_power_adjustment(CommandType::WR, cmd.add);
            } else {
                totalWriteEnergy += cmdWriteEnergy;
            }

            /* Update Memory and buffer*/
            if (traceType == TraceType::WR) {
                for (fourbytes = 0; fourbytes < 16; fourbytes++) {
                    memory[cmd.add.channel][cmd.add.rank][cmd.add.bank][cmd.add.row][cmd.add.col].data[fourbytes] = cmd.data[fourbytes];
                    memory[cmd.add.channel][cmd.add.rank][cmd.add.bank][cmd.add.row][cmd.add.col].encoded = encoded;
                    IO_buffer.data[fourbytes] = cmd.data[fourbytes];
                }
            } else if (traceType == TraceType::RD_WR) {
                for (fourbytes = 0; fourbytes < 16; fourbytes++) {
                    IO_buffer.data[fourbytes] = cmd.data[fourbytes];
                }
            }

            IO_buffer.prevAdd.bank = cmd.add.bank;
            IO_buffer.prevAdd.col = cmd.add.col;

            dramStruct->banks->operator[](cmd.add.bank)->cmdEndTime = currentTime + cmdLengthInCycles(cmd.type);

            if (cmd.type == CommandType::WRA) {
                dramStruct->bankStates->operator[](cmd.add.bank) =  State::CLOSE;
            } else {
                dramStruct->bankStates->operator[](cmd.add.bank) = State::OPEN;
            }
            break;
        }
        case (int(CommandType::ACT)):
            dramStruct->bankStates->operator[](cmd.add.bank) = State::OPEN;
            dramStruct->banks->operator[](cmd.add.bank)->cmdEndTime = this->currentTime + cmdLengthInCycles(cmd.type);
            dramStruct->banks->operator[](cmd.add.bank)->actRowNum = cmd.add.row;

            if (structVar == StructVar::NO) {
                totalActCmdEnergy += dramSpec->actCmdEnergy;
            } else {
	            totalActCmdEnergy += dramSpec->actCmdEnergy;
            }
            break;
        case (int(CommandType::PRE)):
            dramStruct->bankStates->operator[](cmd.add.bank) = State::CLOSE;
            dramStruct->banks->operator[](cmd.add.bank)->cmdEndTime = this->currentTime + cmdLengthInCycles(cmd.type);

            // Correct row address of the command since it isn't read from the trace for a PRE
            cmd.add.row = dramStruct->banks->operator[](cmd.add.bank)->actRowNum;

            if (structVar == StructVar::NO) {
	            totalPreCmdEnergy += dramSpec->preCmdEnergy;
            } else {
	            totalPreCmdEnergy += dramSpec->preCmdEnergy;
            }
            break;
        case (int(CommandType::PREA)):
        case (int(CommandType::REF)):
        case (int(CommandType::REFB)):
        case (int(CommandType::PDN_F_ACT)):
        case (int(CommandType::PDN_F_PRE)):
        case (int(CommandType::PDN_S_PRE)):
        case (int(CommandType::SREN)):
        case (int(CommandType::SREX)):
        default:
            msg::error("Not implemented yet");
    }

    /* Update relevant stats */
    statistics->cmdCycles->operator[]((uint64_t)(cmd.type)) += cmdLengthInCycles(cmd.type);

    dbgstream << ", cmdLengthInCycles: " << cmdLengthInCycles(cmd.type);
    return 0;
}

/* Applies encoding to the data if it is read from the trace file */
void Vampire::apply_encoding(CommandType &req, unsigned int *data, int &encoding) {
    encoding = 0;

    dbgstream << "before data: 0x";
    for (int i = 0; i < 16; i++) {
        dbgstream << std::hex << data[i] << std::dec;
    }
    dbgstream << std::endl;

    switch (int(traceType)) {
        case (int(TraceType::RD_WR)):
            if (req == CommandType::RD) { /* Gets the cmd.data to be written */
                encoding = check_for_encoding(data); /* Check if encoding is possible */

                if (encoding)
                    apply_encoding(data);

            }
            // NOTE: Control falls through
        case (int(TraceType::WR)):
            if (req == CommandType::WR) { /* Also get the data to be written */
                encoding = check_for_encoding(data); /* Check if encoding is possible */

                if (encoding)
                    apply_encoding(data);
            }
            break;
        case (int(TraceType::MEAN)):
        case (int(TraceType::DIST)):
            break;
        default:
            msg::error(false, "Undefined Vampire::TraceType");
    }

    dbgstream << "after data: 0x";
    for (int i = 0; i < 16; i++) {
        dbgstream << std::hex << data[i] << std::dec;
    }
    dbgstream << std::endl;
}

/* Update the stats for the given command's type if the cond is true */
void update_command_count(bool cond, Statistics &statistics, Command cmd) {
    if (cond)
        statistics.cmdCount->operator[]((uint64_t)(cmd.type))++;
}

/* Gets commands from the trace file using parser->parse() and estimates their energy using service_request() */
int Vampire::estimate(){
    int encoding = 0;

    /* Variables for passing data from parsers */
    Command cmd;
    bool wasDataRead;

    std::queue<Command> pendingQueue; // Queue to service the PRE commands generated by RDA/WRA
    volatile bool parseSuccessful = parser->parse(wasDataRead, cmd);
    update_command_count(parseSuccessful, *statistics, cmd);

    while (parseSuccessful || pendingQueue.size() > 0){
        // Check if the first pending command is ready to be serviced
        if (pendingQueue.size() > 0 ) {
            if (!(parseSuccessful) || (pendingQueue.front().issueTime <= cmd.issueTime)) {
                auto &pendingCmd = pendingQueue.front();
                pendingQueue.pop();
                std::cout << std::endl << std::endl << std::endl << "Command issue time" << pendingCmd.issueTime << std::endl;
                pendingCmd.finishTime = pendingCmd.issueTime + dramSpec->cmdLengthInCycles(pendingCmd.type);
                service_request(0, pendingCmd);
                lastPendingCommandIssued = pendingCmd;
            }
            dbgstream << ", pending queue size: " << pendingQueue.size();
        }

        if (parseSuccessful) {
            if ((wasDataRead) && (encodingType != EncodingType::NONE))
                apply_encoding(cmd.type, cmd.data, encoding);

            // Convert auto pre-charge commands to their open page counterparts and add the pre-charge to the pending queue
            if (cmd.type == CommandType::RDA) {
                auto pendingCmdIssueTime = cmd.issueTime + dramSpec->cmdLengthInCycles(CommandType::RD);
                dbgstream << "pending cmd a: " << pendingCmdIssueTime << std::endl;
                pendingQueue.push(Command(CommandType::PRE, cmd.add,
                                          pendingCmdIssueTime, 0,
                                          cmd.data));
                cmd.type = CommandType::RD;
            } else if (cmd.type == CommandType::WRA) {
                auto pendingCmdIssueTime = cmd.issueTime + dramSpec->cmdLengthInCycles(CommandType::WR);
                pendingQueue.push(Command(CommandType::PRE, cmd.add,
                                          pendingCmdIssueTime, 0,
                                          cmd.data));
                cmd.type = CommandType::WR;
            }

            cmd.finishTime = cmd.issueTime + dramSpec->cmdLengthInCycles(cmd.type);

            dbgstream << cmd << ", length: " << dramSpec->cmdLengthInCycles(cmd.type) << std::endl;

            service_request(0, cmd);
            cmd.add.reset();
            parseSuccessful = parser->parse(wasDataRead, cmd);
            update_command_count(parseSuccessful, *statistics, cmd);
            lastCommandIssued = cmd;
        }
    }

    dbgstream << "Last issued command: " << lastCommandIssued
              << "lastStandbyEnergyEvalTime: " << lastStandbyEnergyEvalTime
              << std::endl;

    // Choose the finish time among last cmd and last pending to decide standby evaluation time
    uint64_t lastCmdFinishTime = std::max(lastCommandIssued.finishTime, lastPendingCommandIssued.finishTime);

    if (lastStandbyEnergyEvalTime < lastCmdFinishTime) {

        auto &cmdLength             =  dramSpec->getCmdLength();
        auto &cmdCurrent            =  dramSpec->getCmdCurrent();
        auto &memClkSpeed           =  dramSpec->memClkSpeed;

        auto &totalReadEnergy       = *statistics->totalReadEnergy;
        auto &totalWriteEnergy      = *statistics->totalWriteEnergy;
        auto &totalActCmdEnergy     = *statistics->totalActCmdEnergy;
        auto &totalPreCmdEnergy     = *statistics->totalPreCmdEnergy;
        auto &totalActiveEnergy     = *statistics->totalActiveStandbyEnergy;
        auto &totalPrechargeEnergy  = *statistics->totalPrechargeStandbyEnergy;

        auto &totalActStandbyCycles = *statistics->totalActStandbyCycles;
        auto &totalPreStandbyCycles = *statistics->totalPreStandbyCycles;

        auto &vdd                   = dramSpec->vdd;

        bool allBankPrecharged = true;

        // Check DRAM's state for bg energy calculation
        auto timeDiff = lastCmdFinishTime - lastStandbyEnergyEvalTime;
        for (uint64_t i = 0; i < dramStruct->banks->size(); i++) {
            auto &bank = dramStruct->banks->operator[](i);
            auto &bankState = dramStruct->bankStates->operator[](i);

            if (timeDiff && dramStruct->bankStates->operator[](i) == State::OPEN)
                allBankPrecharged = false;
        }

        dbgstream << "currentTime: " << currentTime << std::endl;
        dbgstream << "timeDiff: " << timeDiff << std::endl;

        // Add corresponding background energy to the stats
        if (allBankPrecharged) {
            totalPrechargeEnergy += timeDiff*dramSpec->preStandbyEnergy;
            totalPreStandbyCycles += timeDiff;
        } else {
            totalActiveEnergy += timeDiff*dramSpec->actStandbyEnergy;
            totalActStandbyCycles += timeDiff;
        }

    }
    // Find the end time of the command which finished last among all the other commands
    auto lastCmdEndTime = 0ul;
    for (auto bank : *dramStruct->banks) {
        lastCmdEndTime = lastCmdEndTime > bank->cmdEndTime ? lastCmdEndTime : bank->cmdEndTime;
    }

    statistics->calculateTotal(*dramSpec, lastCmdEndTime);

    statistics->print_stats();

    if (this->csvFilename != nullptr) {
        statistics->write_csv(csvFilename);
    }

    return 0;
}

/* Frees everything that was ever dynamically allocated */
void Vampire::free_memory() {
    // Call to lambda for freeing TraceType specific allocations
    free_structures[int(traceType)]();

    // Free rest of the allocated variables
    free(configs);
}
