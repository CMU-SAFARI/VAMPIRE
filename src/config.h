/*

CONFIG.H

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#ifndef __CONFIG_H
#define __CONFIG_H

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <cassert>
#include <functional>

#include "consts.h"

class Config {
private:
    std::map<std::string, std::string> options;
    std::string vendor        = "A";
    std::string traceType     = "DIST";
    std::string structVarType = "";
    std::string encodingType  = "NONE";

    /* For TraceType::DIST */
    int setBitArrSizeMult = 1000;
    int toggleArrSizeMult = 1000;

    std::vector<float> setBitDist;
    std::vector<float> toggleDist;

    /* For TraceType::RATIO */
    int avgNumSetBits    = 0;
    int avgNumToggleBits = 0;

public:
    Config();
    explicit Config(const std::string& fname);
    ~Config() = default;

    /* Counts of different structures */
    uint64_t structCount[int(Level::MAX)];

    /* Parses the config file defined */
    void parse(const std::string& fname);

    std::string operator[](const std::string& name) const;

    std::string getValue(const std::string &name);
    template <typename T>
    T getValue (const std::string &name, std::function<T (std::string)> cast_f);
    std::vector<float> *getValueVectorF(const std::string &inputStr);

    bool contains(const std::string& name) const;
    void add (const std::string& name, const std::string& value);

    /* Getters and setters */
    const std::map<std::string, std::string> &getOptions() const;
    void setOptions(const std::map<std::string, std::string> &options);

    unsigned long getNumChannels() const;
    void setNumChannels(unsigned long numChannels);

    unsigned long getNumRanks() const;
    void setNumRanks(unsigned long numRanks);

    unsigned long getNumBanks() const;
    void setNumBanks(unsigned long numBanks);

    unsigned long getNumRows() const;
    void setNumRows(unsigned long numRows);

    unsigned long getNumCols() const;
    void setNumCols(unsigned long numCols);

    int getSetBitArrSizeMult() const;
    void setSetBitArrSizeMult(int setBitArrSizeMult);

    int getToggleArrSizeMult() const;
    void setToggleArrSizeMult(int toggleArrSizeMult);

    std::vector<float> *getSetBitDist();
    void setSetBitDist(std::vector<float> *setBitDist);

    std::vector<float> *getToggleDist();
    void setToggleDist(std::vector<float> *toggleDist);

    int getAvgNumSetBits() const;
    void setAvgNumSetBits(int avgNumSetBits);

    int getAvgNumToggleBits() const;
    void setAvgNumToggleBits(int avgNumToggleBits);

    const std::string &getVendor() const;
    void setVendor(const std::string &vendor);

    const std::string &getTraceType() const;
    void setTraceType(const std::string &traceType);

    const std::string &getStructVarType() const;
    void setStructVarType(const std::string &structVarType);

    const std::string &getEncodingType() const;
    void setEncodingType(const std::string &encodingType);
};

#endif /* _CONFIG_H */