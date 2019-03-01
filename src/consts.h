/*

CONSTS.H

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#ifndef VAMPIRE_CONSTS_H
#define VAMPIRE_CONSTS_H

#include <string>

//#define DEBUG

#define UNMAPPED_ADD_SIZE 32
static const std::string RESET    = "\033[0m";
static const std::string RED      = "\033[1m\033[31m";
static const std::string GREEN    = "\033[1m\033[32m";
static const std::string YELLOW   = "\033[1m\033[33m";
static const std::string BLUE     = "\033[1m\033[34m";
static const std::string MAGENTA  = "\033[1m\033[35m";
static const std::string CYAN     = "\033[1m\033[36m";
static const std::string WHITE    = "\033[1m\033[37m";
static const std::string DARKBLUE = "\033[1m\033[40m";

#define NUM_ENERGY_BINS  4

#define STATIC_POWER     5
#define REFRESH_POWER    5
#define LATENCY_REFRESH 10
#define LATENCY_STATIC   5
static const int BURST_LENGTH = 8; // TODO: make more generalized and read from config

/* Macros */
#define FUNCTION_STR std::string(__FUNCTION__)
#define FILE_STR     std::string(__FILE__)
#define LINE_STR     std::to_string(__LINE__)
#define DBG_STR      std::string(FILE_STR + ":" + LINE_STR + "::" + FUNCTION_STR)

/***********************************/
/* Enums for handling *everything* */
/***********************************/
enum class CommandType   {
    ACT, PRE, PREA,
    RD, WR, RDA, WRA,
    REF, REFB,
    PDN_F_ACT, PDN_F_PRE, PDN_S_PRE,
    SREN, SREX,
    MAX
};

// NOTE: On updating any one of the following enums, update their corresponding string array
enum class EncodingType     {NONE, BDI, CUSTOM, CUSTOM_ADV, MAX};
enum class VendorType       {A, B, C, Cust, MAX};
enum class StructVar        {NO, YES, MAX};
enum class TraceType        {MEAN, DIST, WR, RD_WR, MAX};
enum class Level            {CHANNEL, RANK, BANK, ROW, COLUMN, MAX};
enum class State            {OPEN, CLOSE, MAX};
enum class CmdInterleaving  {BANK, NONE, MAX};                          // Interleaving for RD/WR commands
enum class ParserType       {BINARY, ASCII, MAX};

const std::string commandString[int(CommandType::MAX)] = {
        "ACT", "PRE", "PREA",
        "RD", "WR", "RDA", "WRA",
        "REF", "REFB",
        "PDF_F_ACT", "PDF_F_PRE", "PDN_S_PRE",
        "SREN", "SREX"
};
const std::string encodingString[]      = {"NONE", "BDI", "CUSTOM", "CUSTOM_ADV"};
const std::string vendorString[]        = {"A", "B", "C", "Cust"};
const std::string structVarString[]     = {"NO", "YES"};
const std::string traceTypeString[]     = {"MEAN", "DIST", "WR", "RD_WR"};
const std::string parserTypeString[]    = {"BINARY", "ASCII"};

/* Constant values for parsing config file */
static const std::string VENDOR_STR = "vendor";
static const std::string TRACE_TYPE_STR = "traceType";
static const std::string ENCODING_TYPE_STR = "encodingType";

static const std::string DIST_SET_S = "setBitsDist";
static const std::string DIST_TOG_S = "toggleDist";

static const std::string DIST_SET_MULT_S = "setBitArrMult";
static const std::string DIST_TOG_MULT_S = "toggleBitArrMult";

static const std::string AVG_SET_S = "avgNumSetBits";
static const std::string AVG_TOG_S = "avgNumToggleBits";

static const std::string NUM_CHAN_S = "numChannels";
static const std::string NUM_RANKS_S = "numRanks";
static const std::string NUM_BANKS_S = "numBanks";
static const std::string NUM_ROWS_S = "numRows";
static const std::string NUM_COLS_S = "numCols";

static const std::string AVG_SET_BITS_S = "avgNumSetBits";
static const std::string AVG_TOGGLE_BITS_S = "avgNumToggleBits";

static const Level transLevel[int(Level::MAX)] = {
        Level::CHANNEL, Level::RANK, Level::BANK, Level::ROW, Level::COLUMN
};

// Array to indicate level of a state for which it is valid
static const Level STATE_LEVEL[int(State::MAX)] = {
        Level::ROW, Level::ROW
};

/* General purpose constants */
static const int NUM_OF_BITS = 512;

#endif //VAMPIRE_CONSTS_H
