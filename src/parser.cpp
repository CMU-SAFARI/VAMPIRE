/*

PARSER.H

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#include <assert.h>
#include "parser.h"

/******************/
/* Class : Parser */
/******************/
Parser::Parser() {
    DELIM = ",";
}
Parser::~Parser() {
    file->close();
    delete file;
}
std::vector<std::string> Parser::splitStrAt(const std::string& str, const std::string& delim) {
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

bool verify_cmd(MappedAdd &add, CommandType &cmdType) {
    return true; // TODO: implement this
}

std::string Parser::getFilename() {
    return filename;
}

TraceType Parser::getTraceType() const {
    return traceType;
}

void Parser::setTraceType(TraceType traceType) {
    Parser::traceType = traceType;
}

/***********************/
/*  Class : BinParser  */
/***********************/
void BinParser::setFilename(std::string filename) {
    this->filename = filename;


    file = new std::ifstream(filename, std::ifstream::binary);

    if (file->bad())
        msg::error("Bad trace file");

    msg::info("Using trace file: `" + filename + "'");

    if (!file->is_open())
        msg::error("Unable to open trace file `" + filename + "'.");

    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    this->fileSize = in.tellg();
}

void BinParser::parse_data(uint32_t data[16]) {
    msg::error("Not implemented");
}

/* Parses a single command from the trace file
 *
 * Command format (`<>' represents a 64 bit word):
 *
 * 1. <Timestamp(64bits)><(zero padding), CommandType(3bit), channel(2bits), rank(2bits), bank(3bits), row(16bits), col(7bits)>
 * 2. <Timestamp(64bits)><(zero padding), CommandType(3bit), channel(2bits), rank(2bits), bank(3bits), row(16bits), col(7bits)><data0><data1>...<data7>
 */

bool BinParser::parse(bool &wasDataRead, Command &cmd) {
    dbgstream << ", tellg: " << file->tellg();

    // Return false if file is completely read
    if (file->tellg() == fileSize)
        return false;

    if (file->tellg() == -1)
        msg::error("Incorrect format used for the trace file.");

    auto *time = new uint64_t(0);
    auto time_a = new char[sizeof(time)];
    file->read(time_a, sizeof(time));

    cmd.issueTime = *(uint64_t*)(time_a);

    auto bytes_read_c = new char[sizeof(uint64_t)];

    file->read(bytes_read_c, sizeof(bytes_read_c));

    auto bytes_read = *((uint64_t*)bytes_read_c);

    cmd.add.col = bytes_read & 0x7F;
    bytes_read = bytes_read >> 7;

    cmd.add.row     = bytes_read & 0xFFFF;
    bytes_read = bytes_read >> 16;

    cmd.add.bank    = bytes_read & 0x7;
    bytes_read = bytes_read >> 3;

    cmd.add.rank    = bytes_read & 0x3;
    bytes_read = bytes_read >> 2;

    cmd.add.channel = bytes_read & 0x3;
    bytes_read = bytes_read >> 2;

    // Use rest of bits in the bytes_read for identifying cmd.type
    switch (bytes_read & 0b111) {
        case (0b000):
            cmd.type = CommandType::RD;
            break;
        case (0b001):
            cmd.type = CommandType::WR;
            break;
        case (0b010):
            cmd.type = CommandType::ACT;
            break;
        case (0b011):
            cmd.type = CommandType::PRE;
            break;
        case (0b100):
            cmd.type = CommandType::RDA;
            break;
        case (0b101):
            cmd.type = CommandType::WRA;
            break;
        default:
            std::stringstream errorStr;
            errorStr << "Unknown command Type at: " << file->tellg() << " byte, cmd.issueTime: " << cmd.issueTime << std::endl;
            msg::error(errorStr.str());
    }

    // Reset cmd.data
    for (int i = 0;i < 16; i++)
        cmd.data[i] = 0;

    bool isIOCmd = cmd.type == CommandType::WR || cmd.type == CommandType::RD;
    bool isWriteCmd = cmd.type == CommandType::WR;

    if ((isWriteCmd && traceType == TraceType::WR) || (isIOCmd && traceType == TraceType::RD_WR)){ /* Also get the data to be written */
        char data_c[sizeof(uint32_t) * 16]; // 64 bytes
        file->read(data_c, sizeof(uint32_t)*16);
        auto data = (uint32_t*)data_c;

        for (int i = 0; i < 16; i++) {
            cmd.data[i] = data[i];
        }

        dbgstream << std::endl << "dataread: " << std::hex;
        for (int i = 0; i < 16; i++) {
            dbgstream << data[i];
        }
        dbgstream << std::dec << std::endl;

        wasDataRead = true;

    } else {
        wasDataRead = false;
    }

    delete[] bytes_read_c;
    delete[] time_a;
    delete time;
    return true;
}

/***********************/
/* Class : AsciiParser */
/***********************/
void AsciiParser::setFilename(std::string filename) {
    this->filename = filename;


    file = new std::ifstream(filename);

    if (file->bad())
        msg::error("Bad trace file");

    msg::info("Using trace file: `" + filename + "'");

    if (!file->is_open())
        msg::error("Unable to open trace file `" + filename + "'.");
}

void AsciiParser::parse_data(uint32_t data[16]) {
    assert(false && "Function is not implemented yet");
}

/* Command format:
 * 1. <timestamp>,<command>,<bank>
 * 2. <timestamp>,<command>,<bank>,<data>
 * 3. <timestamp>,ACT,<bank>,<rank>
 */
// TODO: Make parsing simpler by parsing each command independently
bool AsciiParser::parse(bool &wasDataRead, Command &cmd) {
    std::string line;
    bool result = false;
    if (std::getline(*file, line)) {
        result = true;
    } else {
        return false;
    }

    auto tokens = splitStrAt(line, DELIM);
    uint64_t count = 0;
    wasDataRead = false;
    for (auto token : tokens) {
        switch (count++) {
            case (0): // 1st token: Command time
                cmd.issueTime = std::stoul(line);
                break;
            case (1): { // 2nd token: CommandType
                int cmdIter = 0;
                // Iterate over all possible command types to find a match
                for (auto i = static_cast<CommandType>(cmdIter); cmdIter < int(CommandType::MAX); cmdIter++) {
                    // Is this a match?
                    if (commandString[static_cast<int>(static_cast<CommandType>(cmdIter))]
                                .compare(token) == 0) {
                        // Yes, set the command type
                        cmd.type = static_cast<CommandType>(cmdIter);
                        break;
                    }
                }
                break;
            }
            case (2): {
                cmd.add.bank = std::stoul(token);
                break;
            }
            case (3):{
                bool isRdCmd = cmd.type == CommandType::RD || cmd.type == CommandType::RDA;
                bool isWrCmd = cmd.type == CommandType::WR || cmd.type == CommandType::WRA;
                bool isIoCmd = isRdCmd || isWrCmd;
                if (isIoCmd) {
                    cmd.add.col = static_cast<uint64_t>(std::stoul(token));
                } else if (cmd.type == CommandType::ACT) {
                    cmd.add.row = std::stoul(token);
                    count = (uint64_t)-1;
                } else {
                    msg::error("Trace element: `" + line + "' contains illegal token(s)");
                }
                break;
            }
            case (4): {
                // TODO fix this:
                bool isRdCmd = cmd.type == CommandType::RD || cmd.type == CommandType::RDA;
                bool isWrCmd = cmd.type == CommandType::WR || cmd.type == CommandType::WRA;
                bool isIoCmd = isRdCmd || isWrCmd;
                bool isTraceRdWR = traceType == TraceType::RD_WR;
                bool isTraceWr = traceType == TraceType::WR;

                // True if this token is required as the data input
                bool readData = isIoCmd && isTraceRdWR
                                || isWrCmd && isTraceWr;

                if (readData){
                    // Reads data in hexadecimal without the '0x' or any other suffix
                    // Error if data string is not of length
                    //   burstLength*sizeof(unsigned int)*(number of hex bits in a byte)
                    msg::error(token.length() != 8 * sizeof(unsigned long) * 2,
                               "Data size for command at time `" + std::to_string(cmd.issueTime) +
                               "' is not of length " + std::to_string(8 * sizeof(unsigned long) * 2));

                    // Extract the data from the string in chunks of 32 bits
                    for (int dataElementCount = 0; dataElementCount < 16; dataElementCount++) {
                        unsigned int dataElementLengthInHex = sizeof(uint32_t) * 2;
                        auto substring = "0x" + token.substr(dataElementLengthInHex * dataElementCount, dataElementLengthInHex);
                        cmd.data[dataElementCount] = static_cast<uint32_t>(std::stoul(substring, nullptr, 16));
                    }
                    wasDataRead = true;
                } else {
                    msg::error("Trace element: `" + line + "' contains illegal token(s)");
                }
                break;
            }
            default: {
                msg::error("Illegal command trace format, error at `" + line + "'. Please read help.");
            }
        }
    }

    msg::error(cmd.type == CommandType::PRE &&  tokens.size() != 3,
               "PRE command at time: " + std::to_string(cmd.issueTime) + " doesn't have enough parameters.");
    msg::error((cmd.type == CommandType::RD || cmd.type == CommandType::RDA || cmd.type == CommandType::WR || cmd.type == CommandType::WRA) &&  tokens.size() < 4,
               "I/O command at time: " + std::to_string(cmd.issueTime) + " doesn't have enough parameters.");
    msg::error(cmd.type == CommandType::ACT &&  tokens.size() != 4,
               "ACT command at time: " + std::to_string(cmd.issueTime) + " doesn't have enough parameters.");

    verify_cmd(cmd.add, cmd.type);

    dbgstream << line << std::endl;
    dbgstream << "result:: " << result << std::endl;

    return result;
}


