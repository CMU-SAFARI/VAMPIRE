/*

DRAMSTRUCT.H

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#ifndef VAMPIRE_STRUCT_H
#define VAMPIRE_STRUCT_H


#include <vector>
#include <algorithm>
#include <cmath>

#include "consts.h"
#include "config.h"

class Node {
public:
    explicit Node(Level level);
    explicit Node(Level level, unsigned long childCount);
    explicit Node(unsigned long id, Level level);
    explicit Node(unsigned long id, Level level, std::vector<Node*> *children);

    /* Getters and setters */
    unsigned long id = 0;
    std::vector<unsigned long> *openedRows;

    unsigned long getId() const;
    void setId(unsigned long id);

    std::vector<unsigned long> *getOpenedRows() const;
    void setOpenedRows(std::vector<unsigned long> *openedRows);

    Level getLevel() const;
    void setLevel(Level level);

    std::vector<Node *> *getChildren() const;
    void setChildren(std::vector<Node *> *children);

    Level level = Level::CHANNEL;
    std::vector<Node*> *children; // Points to a vector of children of this node, nullPtr if leaf
};

class DramStruct {
public:
    DramStruct();
    ~DramStruct();
    std::vector<Node*> *structure;
    bool isBankActive(uint64_t &bankNum);
    int init_struct_at_level(Node *node, int level, uint64_t *structCount);
    class Bank {
    public:
        Bank() = default;
        Bank(uint64_t cmdStartTime, uint64_t cmdEndTime) : cmdEndTime(cmdEndTime), cmdStartTime(cmdStartTime) {};
        uint64_t cmdStartTime = 0ul;
        uint64_t cmdEndTime = 0ul;
        uint64_t actRowNum = 0ul;
    };

    std::vector<Bank*> *banks;
    std::vector<State> *bankStates;

};

static const long DEFAULT_STRUCT_COUNT[int(Level::MAX)] = {
        // CHAN, RANK, BANK, ROW, COL
        1, 1, 8, 32768, 128
};

#endif // VAMPIRE_STRUCT_H
