/*

CONFIG.CPP

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#include <iterator>
#include <sstream>
#include <algorithm>
#include "config.h"
#include "consts.h"
#include "helper.h"

Config::Config() {
    setBitDist.assign({});
    toggleDist.assign({});
}

Config::Config(const std::string& fname) : Config() {
    parse(fname);
}

void Config::parse(const std::string& fname) {
    std::ifstream file(fname);

    if (!file.good()) {
        msg::error("Unable to locate the specified config file, see 'vampire --help'.");
    }

    std::string line;
    while (getline(file, line)) {
        std::string origLine = line; // Store the orginal line for warning msg

        char delim[] = " \t=";
        std::vector<std::string> tokens;

        bool lineParsed = false; // Identifies lines with unknown keys

        while (true) {
            size_t start = line.find_first_not_of(delim);
            if (start == std::string::npos)
                break;

            size_t end = line.find_first_of(delim, start);
            if (end == std::string::npos) {
                tokens.push_back(line.substr(start));
                break;
            }

            tokens.push_back(line.substr(start, end - start));
            line = line.substr(end);
        }

        // empty line
        if (tokens.empty())
            continue;

        // comment line
        if (tokens[0][0] == '#')
            continue;

        options[tokens[0]] = tokens[1];

        // Command line overridable options
        if (tokens[0] == VENDOR_STR) {
            vendor = atoi(tokens[1].c_str());
            lineParsed = true;
        } else if (tokens[0] == TRACE_TYPE_STR) {
            lineParsed = true;
            traceType = atoi(tokens[1].c_str());
        } else if (tokens[0] == ENCODING_TYPE_STR) {
            lineParsed = true;
            encodingType = atoi(tokens[1].c_str());
        }

        // For structural configuration
        if (tokens[0] == NUM_CHAN_S) {
            lineParsed = true;
            structCount[int(Level::CHANNEL)] = std::stoul(tokens[1].c_str());
        } else if (tokens[0] == NUM_RANKS_S) {
            lineParsed = true;
            structCount[int(Level::RANK)]    = std::stoul(tokens[1].c_str());
        } else if (tokens[0] == NUM_BANKS_S) {
            lineParsed = true;
            structCount[int(Level::BANK)]    = std::stoul(tokens[1].c_str());
        } else if (tokens[0] == NUM_ROWS_S) {
            lineParsed = true;
            structCount[int(Level::ROW)]     = std::stoul(tokens[1].c_str());
        } else if (tokens[0] == NUM_COLS_S) {
            lineParsed = true;
            structCount[int(Level::COLUMN)]  = std::stoul(tokens[1].c_str());
        }

        // For TraceType::DIST
        if (tokens[0] == DIST_SET_S) {
            // Convert std::vector<std::string> to std::vector<float>
            assert(tokens.size() == (NUM_OF_BITS+1+1) && "Distribution should have exactly 513 values.");

            setBitDist.resize(tokens.size()-1); // -1 for the key
            std::transform(tokens.begin()+1, tokens.end(), setBitDist.begin(), [](const std::string& val)
            {
                return std::stof(val);
            });
            lineParsed = true;
        } else if (tokens[0] == DIST_TOG_S) {
            // Convert std::vector<std::string> to std::vector<float>
            assert(tokens.size() == (NUM_OF_BITS+1+1) && "Distribution should have exactly 513 values.");

            toggleDist.resize(tokens.size()-1); // -1 for the key
            std::transform(tokens.begin()+1, tokens.end(), toggleDist.begin(), [](const std::string& val)
            {
                return std::stof(val);
            });
            lineParsed = true;
        } else if (tokens[0] == DIST_SET_MULT_S) {
            setBitArrSizeMult = atoi(tokens[1].c_str());
            lineParsed = true;
        } else if (tokens[0] == DIST_TOG_MULT_S) {
            toggleArrSizeMult = atoi(tokens[1].c_str());
            lineParsed = true;
        }

        // For TraceType::RATIO
        if (tokens[0] == AVG_SET_BITS_S) {
            avgNumSetBits = atoi(tokens[1].c_str());
            lineParsed = true;
        } else if (tokens[0] == AVG_TOGGLE_BITS_S) {
            avgNumToggleBits = atoi(tokens[1].c_str());
            lineParsed = true;
        }

        if (!lineParsed) {
            msg::warning("Config: Line `" + origLine + "' contains unknown key, please refer to configs/default.cfg for all valid options.");
        }
    }
    file.close();
}

std::string Config::operator [] (const std::string& name) const {
    if (options.find(name) != options.end()) {
        return (options.find(name))->second;
    } else {
        return "";
    }
}

bool Config::contains(const std::string& name) const {
    if (options.find(name) != options.end()) {
        return true;
    } else {
        return false;
    }
}

void Config::add (const std::string& name, const std::string& value) {
    if (!contains(name)) {
        options.insert(make_pair(name, value));
    } else {
        msg::error(FUNCTION_STR + ": Options[" + name + "] already set.");
    }
}

/* Other general purpose functions and operators */
std::string Config::getValue(const std::string &name) {
    return this->operator[](name);
}

std::vector<float> *Config::getValueVectorF(const std::string &inputStr) {
    std::stringstream ss(inputStr);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> vstrings(begin, end);

    std::vector<float> *result = new std::vector<float>(vstrings.size());

    std::transform(vstrings.begin(), vstrings.end(), back_inserter(*result),
              [] (std::string const &element) {
                  return stof(element);
              }
    );

    return result;
}

template<typename T>
T Config::getValue
        (const std::string &name, std::function<T (std::string)> cast_f) {
    return cast_f(name);
}

/***********************/
/* Getters and setters */
const std::map<std::string, std::string> &Config::getOptions() const {
    return options;
}

void Config::setOptions(const std::map<std::string, std::string> &options) {
    Config::options = options;
}

unsigned long Config::getNumChannels() const {
    return structCount[int(Level::CHANNEL)];
}

void Config::setNumChannels(unsigned long numChannels) {
    Config::structCount[int(Level::CHANNEL)] = numChannels;
}

unsigned long Config::getNumRanks() const {
    return structCount[int(Level::RANK)];
}

void Config::setNumRanks(unsigned long numRanks) {
    Config:: structCount[int(Level::RANK)] = numRanks;
}

unsigned long Config::getNumBanks() const {
    return  structCount[int(Level::BANK)];
}

void Config::setNumBanks(unsigned long numBanks) {
    Config::structCount[int(Level::BANK)] = numBanks;
}

unsigned long Config::getNumRows() const {
    return structCount[int(Level::ROW)];
}

void Config::setNumRows(unsigned long numRows) {
    Config::structCount[int(Level::ROW)] = numRows;
}

unsigned long Config::getNumCols() const {
    return structCount[int(Level::COLUMN)];
}

void Config::setNumCols(unsigned long numCols) {
    Config::structCount[int(Level::COLUMN)] = numCols;
}

int Config::getSetBitArrSizeMult() const {
    return setBitArrSizeMult;
}

void Config::setSetBitArrSizeMult(int setBitArrSizeMult) {
    Config::setBitArrSizeMult = setBitArrSizeMult;
}

int Config::getToggleArrSizeMult() const {
    return toggleArrSizeMult;
}

void Config::setToggleArrSizeMult(int toggleArrSizeMult) {
    Config::toggleArrSizeMult = toggleArrSizeMult;
}

std::vector<float> *Config::getSetBitDist() {
    return &setBitDist;
}

void Config::setSetBitDist(std::vector<float> *setBitDist) {
    Config::setBitDist = (*setBitDist);
}

std::vector<float> *Config::getToggleDist() {
    return &toggleDist;
}

void Config::setToggleDist(std::vector<float> *toggleDist) {
    Config::toggleDist = (*toggleDist);
}

int Config::getAvgNumSetBits() const {
    return avgNumSetBits;
}

void Config::setAvgNumSetBits(int avgNumSetBits) {
    Config::avgNumSetBits = avgNumSetBits;
}

int Config::getAvgNumToggleBits() const {
    return avgNumToggleBits;
}

void Config::setAvgNumToggleBits(int avgNumToggleBits) {
    Config::avgNumToggleBits = avgNumToggleBits;
}

const std::string &Config::getVendor() const {
    return vendor;
}

void Config::setVendor(const std::string &vendor) {
    Config::vendor = vendor;
}

const std::string &Config::getTraceType() const {
    return traceType;
}

void Config::setTraceType(const std::string &traceType) {
    Config::traceType = traceType;
}

const std::string &Config::getStructVarType() const {
    return structVarType;
}

void Config::setStructVarType(const std::string &structVarType) {
    Config::structVarType = structVarType;
}

const std::string &Config::getEncodingType() const {
    return encodingType;
}

void Config::setEncodingType(const std::string &encodingType) {
    Config::encodingType = encodingType;
}
