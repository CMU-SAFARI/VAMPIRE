/*

DRAMSPEC.CPP

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#include <iostream>
#include "dramSpec.h"

DramSpec::DramSpec() {
    cmdLength_ptr.reset(new std::vector<double_t>(8));
    cmdCurrent_ptr.reset(new std::vector<double_t>(8));
}

std::vector<double_t> &DramSpec::getCmdLength() {
    return *this->cmdLength_ptr;
}

std::vector<double_t> &DramSpec::getCmdCurrent() {
    return *this->cmdCurrent_ptr;
}

DramSpec_A::DramSpec_A() : DramSpec() {
    auto &cmdLength = *cmdLength_ptr;
    auto &cmdCurrent = *cmdCurrent_ptr;

    cmdLength.resize(static_cast<uint64_t>(CommandType::MAX));
    cmdCurrent.resize(static_cast<uint64_t>(CommandType::MAX));
    for (int i = 0; i < int(CommandType::MAX); i++) {
        getCmdLength()[i] = 0;
        cmdCurrent[i] = 0;
    }

    vdd = 1.35f;

    /* Latencies (ns) */
    cmdLength[int(CommandType::ACT)] = 15.0;
    cmdLength[int(CommandType::PRE)] = 15.0;
    cmdLength[int(CommandType::RD)] = 25.0;
    cmdLength[int(CommandType::WR)] = 25.0;

    /* Currents (mA) */
    cmdCurrent[int(CommandType::ACT)] = 68.9f;
    cmdCurrent[int(CommandType::PRE)] = 68.9f;

    /* Pre-calculated energies (pJ/1cycle = pJ/2.5ns) */
    actCmdEnergy = 1114.961;
    preCmdEnergy = 1114.961;

    actStandbyEnergy = 129.3598;
    preStandbyEnergy = 119.0111;
}

DramSpec_B::DramSpec_B() : DramSpec() {
    auto &cmdLength = *cmdLength_ptr;
    auto &cmdCurrent = *cmdCurrent_ptr;

    cmdLength.resize(static_cast<uint64_t>(CommandType::MAX));
    cmdCurrent.resize(static_cast<uint64_t>(CommandType::MAX));
    for (int i = 0; i < int(CommandType::MAX); i++) {
        cmdLength[i] = 0;
        cmdCurrent[i] = 0;
    }

    vdd = 1.35f;

    /* Latencies (ns) */
    cmdLength[int(CommandType::ACT)] = 15.0;
    cmdLength[int(CommandType::PRE)] = 15.0;
    cmdLength[int(CommandType::RD)] = 25.0;
    cmdLength[int(CommandType::WR)] = 25.0;

    /* Currents (mA) */
    cmdCurrent[int(CommandType::ACT)] = 69.8f;
    cmdCurrent[int(CommandType::PRE)] = 69.8f;

    /* Pre-calculated energies (pJ/1cycle = pJ/2.5ns) */
    actCmdEnergy = 739.2047;
    preCmdEnergy = 739.2047;

    actStandbyEnergy = 161.6844;
    preStandbyEnergy = 181.3352;
}

DramSpec_C::DramSpec_C() : DramSpec() {
    auto &cmdLength = *cmdLength_ptr;
    auto &cmdCurrent = *cmdCurrent_ptr;

    cmdLength.resize(static_cast<uint64_t>(CommandType::MAX));
    cmdCurrent.resize(static_cast<uint64_t>(CommandType::MAX));
    for (int i = 0; i < int(CommandType::MAX); i++) {
        cmdLength[i] = 0;
        cmdCurrent[i] = 0;
    }

    /* Latencies (ns) */
    cmdLength[int(CommandType::ACT)] = 13.125;
    cmdLength[int(CommandType::PRE)] = 13.125;
    cmdLength[int(CommandType::RD)] = 23.125;
    cmdLength[int(CommandType::WR)] = 23.125;

    vdd = 1.35f;

    /* Currents (mA) */
    cmdCurrent[int(CommandType::ACT)] = 58.6f;
    cmdCurrent[int(CommandType::PRE)] = 58.6f;

    /* Pre-calculated energies (pJ/1cycle = pJ/2.5ns) */
    actCmdEnergy = 812.3848;
    preCmdEnergy = 812.3848;

    actStandbyEnergy = 112.6297;
    preStandbyEnergy = 122.75881;
}

DramSpec_Cust::DramSpec_Cust(const std::string &fname) : DramSpec() {
    /* Init everything to default value */
    auto &cmdLength = *cmdLength_ptr;
    auto &cmdCurrent = *cmdCurrent_ptr;

    cmdLength.resize(static_cast<uint64_t>(CommandType::MAX));
    cmdCurrent.resize(static_cast<uint64_t>(CommandType::MAX));
    for (int i = 0; i < int(CommandType::MAX); i++) {
        cmdLength[i] = 0;
        cmdCurrent[i] = 0;
    }

    /* Start parsing the dramSpec file */
    std::ifstream file(fname);

    if (!file.good()) {
        msg::warning("Unable to locate the specified DramSpec file, see 'vampire --help'.");
    }

    std::string line;
    while (getline(file, line)) {
        std::string origLine = line;

        char delim[] = " \t=";
        std::vector<std::string> tokens;

        bool lineParsed = false;

        while (true) {
            size_t start = line.find_first_not_of(delim);
            if (start == std::string::npos)
                break;
            size_t end = line.find_first_of(delim, start);
            if (end == std::string::npos) {
                tokens.push_back(line.substr(start));
                break;
            }

            tokens.push_back(line.substr(start, end-start));
            line = line.substr(end);
        }

        /* empty line */
        if (tokens.empty())
            continue;

        /* comment line */
        if (tokens[0][0] == '#')
            continue;

        if (tokens[0] == "vdd") {
            vdd = std::stod(tokens[1]);
            lineParsed = true;
        }

        // Parse cmdLength
        std::string cmdLengthPrefix = "cmdLength";
        /* To find if cmdLength is a prefix of tokens[1] */
        auto cmdLengthMatch = std::mismatch(cmdLengthPrefix.begin(), cmdLengthPrefix.end(), tokens[0].begin());

        if (cmdLengthMatch.first == cmdLengthPrefix.end()) {
            /* Found cmdLengthPrefix as the prefix of tokens[0] */
            std::vector<std::string> splitToken = Helper::splitStr(tokens[0], '.');

            int cmdLoc = Helper::findInArr<std::string>(&commandString[0], splitToken[1], int(CommandType::MAX));

            if (cmdLoc != -1){
                cmdLength[int(static_cast<CommandType>(cmdLoc))] = std::stod(tokens[1]);
            } else {
                msg::error("DramSpec: Unable to parse `" + origLine + "', `" + splitToken[1] + "' is not a valid CommandType.");
            }
            lineParsed = true;
        }

        // Parse cmdLength
        std::string cmdCurrentPrefix = "cmdCurrent";
        /* To find if cmdLength is a prefix of tokens[1] */
        auto cmdCurrentMatch = std::mismatch(cmdCurrentPrefix.begin(), cmdCurrentPrefix.end(), tokens[0].begin());

        if (cmdCurrentMatch.first == cmdCurrentPrefix.end()) {
            /* Found cmdCurrentPrefix as the prefix of tokens[0] */
            std::vector<std::string> splitToken = Helper::splitStr(tokens[0], '.');

            int cmdLoc = Helper::findInArr<std::string>(&commandString[0], splitToken[1], int(CommandType::MAX));

            if (cmdLoc != -1){
                cmdCurrent[int(static_cast<CommandType>(cmdLoc))] = std::stod(tokens[1]);
            } else {
                msg::error("DramSpec: Unable to parse `" + origLine + "', `" + splitToken[1] + "' is not a valid CommandType.");
            }
            lineParsed = true;
        }

        if (tokens[0] == "actCmdEnergy") {
            actCmdEnergy = std::stod(tokens[1]);
            lineParsed = true;
        }

        if (tokens[0] == "preCmdEnergy") {
            preCmdEnergy = std::stod(tokens[1]);
            lineParsed = true;
        }

        if (tokens[0] == "actStandbyEnergy") {
            actStandbyEnergy = std::stod(tokens[1]);
            lineParsed = true;
        }

        if (tokens[0] == "preStandbyEnergy") {
            preStandbyEnergy = std::stod(tokens[1]);
            lineParsed = true;
        }

        if (!lineParsed) {
            msg::warning("DramSpec: Unable to parse `" + origLine + "', unknown token found.");
        }
    }
}