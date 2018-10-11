//
// Created by suyash on 7/10/18.
//

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
