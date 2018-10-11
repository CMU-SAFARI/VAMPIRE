/*

MAIN.H

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#ifndef VAMPIRE_MAIN_H
#define VAMPIRE_MAIN_H

#include "vampire.h"

#include <cstring>

template <typename T>
T get_param(const std::string *refEnumArr, const std::string paramStr, std::string paramName);
void parse_args(int argc, char *argv[], Vampire &dram);
int main(int argc, char * argv[]);

#endif //VAMPIRE_MAIN_H
