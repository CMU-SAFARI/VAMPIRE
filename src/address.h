/*

ADDRESS.H

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#ifndef VAMPIRE_ADDRESS_H
#define VAMPIRE_ADDRESS_H

#include <bitset>
#include <iostream>
#include "consts.h"

class MappedAdd {
public:
    MappedAdd(const MappedAdd &obj);
    unsigned long channel = 0ul, rank = 0ul, bank = 0ul, row = 0ul, col = 0ul;
    MappedAdd() = default;
    MappedAdd(unsigned long channel, unsigned long rank, unsigned long bank, unsigned long row, unsigned long col);
    void reset();
};

std::ostream& operator<<(std::ostream& ostr, MappedAdd& add);
class UnmappedAdd {
public:
    std::bitset<UNMAPPED_ADD_SIZE> *add;

    UnmappedAdd(std::bitset<UNMAPPED_ADD_SIZE> *add);
    virtual ~UnmappedAdd();
};


#endif //VAMPIRE_ADDRESS_H
