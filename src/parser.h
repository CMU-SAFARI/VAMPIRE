/*

PARSER.H

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#ifndef VAMPIRE_PARSER_H
#define VAMPIRE_PARSER_H


#include <cstdio>
#include <functional>
#include <vector>

#include "consts.h"
#include "address.h"
#include "command.h"


class Parser {
private:
protected:
    std::string DELIM;
    std::string filename = "";
    std::ifstream *file;
    TraceType traceType;
    uint64_t bytes_read;
public:
    Parser();
    ~Parser();
    virtual void setFilename(std::string filename) = 0;
    virtual std::string getFilename();

    TraceType getTraceType() const;
    void setTraceType(TraceType traceType);

    bool verifyCmd(MappedAdd &add, CommandType &cmdType);

    virtual void parse_data(uint32_t data[16]) = 0;
    virtual bool parse(bool &wasDataRead, Command &cmd) = 0;
    static std::vector<std::string> splitStrAt(const std::string& str, const std::string& delim);

    bool verify_request(MappedAdd &add, CommandType &cmdType);
};

class BinParser : public Parser {
private:
    int64_t fileSize = 0;
public:
    void setFilename(std::string filename);
    void parse_data(uint32_t data[16]) override;
    bool parse(bool &wasDataRead, Command &cmd) override;
};

class AsciiParser : public Parser {
private:
public:
    void setFilename(std::string filename);
    void parse_data(uint32_t data[16]) override;
    bool parse(bool &wasDataRead, Command &cmd) override;
};

#endif //VAMPIRE_PARSER_H
