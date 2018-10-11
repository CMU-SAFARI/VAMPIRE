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

#ifdef DEBUG
#define debug_nl__(color, val) \
std::cout << (color) << (val) << RESET << std::endl;

#define debug__(color, val) \
std::cout << (color) << (val) << RESET;

#define debug_nl_only__() \
std::cout << std::endl;
#else
#define debug_nl__(val)
#define debug__(val)
#define debug_nl_only_()
#endif

namespace Helper {
    int verify_add(CommandType request, MappedAdd reqAdd, Config &configs);
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