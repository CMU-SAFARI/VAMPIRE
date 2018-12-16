/*

HELPER.H

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#ifndef __VAMPIRE_HELPER__
#define __VAMPIRE_HELPER__

#include <bits/stdc++.h>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string.h>

#include "consts.h"
#include "address.h"
#include "config.h"
#include "globalDebug.h"

namespace Helper {
    int verify_add(CommandType request, MappedAdd reqAdd, Config &configs);
    /* Results a vector of strings split at token */
    std::vector<std::string> splitStr(const std::string str, const char token);

    /* Finds location of an element in a vector, returns -1 if not found */
    template <typename T>
    int findInVect(const std::vector<T> vect, const T toFind) {
        int loc = -1, it = 0;
        for (auto elem : commandString) {
            if (elem == toFind) {
                loc = it;
                break;
            }
            it++;
        }
        return it;
    }

    /* Finds location of an element in an array, returns -1 if not found */
    template <typename T>
    int findInArr(const T *arr, const T toFind, const int len) {
        int loc = -1;
        for (int i = 0; i < len; i++) {
            if (arr[i] == toFind) {
                loc = i;
                break;
            }
        }
        return loc;
    }
}

class IO_data{
public:
    unsigned int data[16];
    MappedAdd prevAdd;
    void init_values(void);
};


class DRAMdata{
public:
    unsigned int data[16];
    unsigned int encoded; /* Whether the data stored is in encoded form*/
    void init_values(void);
};

unsigned int toggle(unsigned int[], unsigned int[]);
float percentSetBits(unsigned int[]);
unsigned int noSetBits(unsigned int[]);

class msg {
private:
    bool isErrorStreamFlushed = true;
public:
    // Get current date/time, format is YYYY-MM-DD.HH:mm:ss
    static const std::string currentDateTime();

    // Print an error message and exit and its variants
    static void error(std::string msg);
    static void error(std::string msg, int status);
    static void error(bool cond, std::string msg);
    static void error(bool cond, std::string msg, int status);

    static void info(std::string msg);

    static void warning(std::string msg);
    static void warning(bool cond, std::string msg);
};

#endif //__VAMPIRE_HELPER__