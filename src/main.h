//
// Created by suyash on 7/11/18.
//

#ifndef VAMPIRE_MAIN_H
#define VAMPIRE_MAIN_H

#include "vampire.h"

#include <cstring>

template <typename T>
T get_param(const std::string *refEnumArr, const std::string paramStr, std::string paramName);
void parse_args(int argc, char *argv[], Vampire &dram);
int main(int argc, char * argv[]);

#endif //VAMPIRE_MAIN_H
