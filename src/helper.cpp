/*

HELPER.CPP

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#include "helper.h"
#include "config.h"

/* Calculates percentage of set bits in data */
float percentSetBits(unsigned int data[16])
{
  unsigned int count = 0;
  float percentage;

  for (int fourbytes = 0; fourbytes < 16; fourbytes++){
    count += __builtin_popcount(data[fourbytes]);  
  }
  
  percentage = ((float)(count)/(16*32.0f))*100.0f;
  return percentage;
}

namespace Helper {
    int verify_add(CommandType request, MappedAdd reqAdd, Config &configs) {
        std::stringstream ss("");
        if (reqAdd.channel >= configs.getNumChannels()) {
            ss << "At channel #: " << reqAdd.channel;
            ss << ", request verification failed. Channel Invalid";
            msg::error(ss.str());
            return -1;
        }
        if (reqAdd.rank >= configs.getNumRanks()) {
            ss << "At rank #: " << reqAdd.rank;
            ss << ", request verification failed. Rank Invalid";
            msg::error(ss.str());
            return -1;
        }
        if (reqAdd.bank >= configs.getNumBanks()) {
            ss << "At bank #: " << reqAdd.bank;
            ss << ", request verification failed. Bank Invalid";
            msg::error(ss.str());
            return -1;
        }
        if (reqAdd.row >= configs.getNumRows()) {
            ss << "At row #: " << reqAdd.row;
            ss << ", request verification failed. Row Invalid";
            msg::error(ss.str());
            return -1;
        }
        if (reqAdd.col >= configs.getNumCols()) {
            ss << "At col #: " << reqAdd.col;
            ss << ", request verification failed. Col Invalid";
            msg::error(ss.str());
            return -1;
        }
        return 0;
    }

    std::vector<std::string> splitStr(const std::string str, const char token) {
        std::vector<std::string> result;
        std::istringstream f(str);

        std::string temp;
        while (getline(f, temp, token)) {
            result.push_back(temp);
        }
        return result;
    }
}

/*******************/
/* namespace : msg */
/*******************/
// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string msg::currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

// Print error message and exit and its variants
void msg::error(bool cond, std::string msg, int status) {
    if (cond) {
        std::cout << "[" << currentDateTime() << "] ";
        std::cout << RED << "Error: " << msg << RESET << std::endl;
        exit(status);
    }
}
void msg::error(bool cond, std::string msg) {
    error(cond, msg, 1);
}
void msg::error(std::string msg) {
    error(true, msg, 1);
}
void msg::error(std::string msg, int status) {
    error(true, msg, status);
}

void msg::info(std::string msg) {
    std::cout << "[" << currentDateTime() << "] ";
    std::cout << BLUE << msg << RESET << std::endl;
}

void msg::warning(bool cond, std::string msg) {
    if (cond) {
        warning(msg);
    }
}
void msg::warning(std::string msg) {
    std::cout << "[" << currentDateTime() << "] ";
    std::cout << MAGENTA << "Warning: " << msg << RESET << std::endl;
}


/* Initialising IO_data */
void IO_data::init_values(){
    for(int fourbytes = 0; fourbytes < 16; fourbytes++){
        data[fourbytes] = 0;
    }
    prevAdd.bank = 0;
    prevAdd.col  = 0;
}


/* Initiliasing DRAMdata */
void DRAMdata::init_values(){
    for(int fourbytes = 0; fourbytes < 16; fourbytes++){
        data[fourbytes] = 0;
    }
    encoded = 0;
}
