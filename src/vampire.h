/*

VAMPIRE.H

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#ifndef __DRAM_ENERGY__
#define __DRAM_ENERGY__

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <functional>
#include <iostream>
#include <iomanip>
#include <vector>

#include "sysexits.h"

#include "config.h"
#include "consts.h"
#include "dramSpec.h"
#include "dramStruct.h"
#include "equations.h"
#include "helper.h"
#include "parser.h"
#include "statistics.h"
#include "command.h"
#include "globalDebug.h"

class Vampire {

public:
    Vampire();
    ~Vampire();

    EncodingType   encodingType;      /* OPTIONS ARE NONE, BDI, CUSTOM, CUSTOM_ADV */
    VendorType     vendorType;        /* OPTIONS ARE A, B, C */
    StructVar      structVar;         /* OPTIONS ARE NO, YES */
    TraceType      traceType;         /* OPTIONS ARE RATIO, DIST, WR, RD_WR */
    ParserType     parserType;        /* OPTIONS ARE BINARY, ASCII*/

    std::string *traceFilename = nullptr;
    std::string *configFilename = nullptr;

    std::string *dramSpecFilename = nullptr;
    std::string *csvFilename = nullptr;

    Config *configs;
    Parser *parser;
    DramSpec *dramSpec = nullptr;

    DramStruct *dramStruct;                         // Stores the state of different elements of a DRAM
    Statistics *statistics;
    Equations *equations;
    Command lastCommandIssued;
    Command lastPendingCommandIssued;

    uint64_t lastStandbyEnergyEvalTime = 0ul; // Keeps track of the time when the last standby time evaluation took place
protected:
    /* Constants */
    const int NUM_OF_SET_BIT_ARR_MULT = 1000; // (NUM_OF_BITS) X this_variable is size of the array used for
                                              // determining the number of 1s in case of TraceType::DIST
    const int TOGGLE_BIT_ARR_MULT = 1000;     // Similar to NUM_OF_SET_BIT_ARR_MULT, instead used for

                                              // the toggle bit array
    /* Variables */

    uint64_t currentTime = 0ul;

    void init_lambdas();
    void init_latencies();

    /* Stores latency of each operation */
    //std::vector<float> latency[int(VendorType::MAX)];

    /* Calculates # of set bits in data */
    std::function<unsigned int(unsigned int[16])> getSetBits[int(TraceType::MAX)];
    /* Calculates # of bits toggled between old_data and new_data */
    std::function<unsigned int(unsigned int[16], unsigned int[16])> getToggleBits[int(TraceType::MAX)];

    IO_data IO_buffer;
    DRAMdata ***** memory;

    /*** Variables for TraceType::DIST ***/
    std::vector<unsigned short> *numOfSetBits;    // Array for finding number of set btis in case
                                             // of a probability distribution (TraceType::DIST)

    std::vector<unsigned short> *numOfToggleBits; // Array for finding number of toggled bits in case
                                             // of a probability distribution (TraceType::DIST)

    /* ENCODING TABLE FOR CUSTOM ENCODING */
    unsigned int encoding_table[1024];
public:
    std::vector<int> *dist;

    int set_values                      ();
    int service_request(int encoded, Command cmd);
    void free_memory                    (void);
    void apply_encoding                 (CommandType &req, unsigned int *data, int &encoding);
    int  estimate                       (void);
    int check_for_encoding              (unsigned int[]);
    unsigned int * apply_encoding       (unsigned int[]);

    void make_encoding_table            (void); // Custom Encoding

    /* Initializes data structures associated with each type of trace */
    std::function<void(void)> init_structures[int(TraceType::MAX)];
    /* Frees all the data structures initialized by the init lambdas */
    std::function<void(void)> free_structures[int(TraceType::MAX)];
};

#endif //__DRAM_ENERGY__
