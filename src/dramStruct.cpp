/*

DRAMSTRUCT.H

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#include "dramStruct.h"

/**********************/
/* Node class methods */
/**********************/
/* Constructors */
Node::Node(Level level) : level(level) {};
Node::Node(Level level, unsigned long childCount): level(level) {
    if (level == Level::BANK)
        openedRows = new std::vector<unsigned long>(childCount);
}
Node::Node(unsigned long id, Level level) : id(id), level(level) {}
Node::Node(unsigned long id, Level level, std::vector<Node*> *children) : id(id), level(level), children(children) {}

unsigned long Node::getId() const {
    return id;
}

void Node::setId(unsigned long id) {
    Node::id = id;
}

std::vector<unsigned long> *Node::getOpenedRows() const {
    return openedRows;
}

void Node::setOpenedRows(std::vector<unsigned long> *openedRows) {
    Node::openedRows = openedRows;
}

Level Node::getLevel() const {
    return level;
}

void Node::setLevel(Level level) {
    Node::level = level;
}

std::vector<Node *> *Node::getChildren() const {
    return children;
}

void Node::setChildren(std::vector<Node *> *children) {
    Node::children = children;
}


/**************************/
/* Vampire class methods */
/**************************/

int DramStruct::init_struct_at_level(Node *node, int level, uint64_t *structCount) {
    // Should the recursion continue?
    if (level <= int(Level::BANK)) {
        node->children = new std::vector<Node*>(structCount[level]);

        // Initialize each of the child recursively
        for (unsigned long child = 0; child < structCount[level]; child++) {
            node->children->operator[](child) = new Node(transLevel[level]);
            int success = init_struct_at_level(
                    node->children->operator[](child),
                    level+1,
                    structCount);

            if (success)
                assert(false && "Failed to initialize DRAM structure.");
        }
    }
    return 0;
}

/*********************/
/* Class: DramStruct */
/*********************/

bool DramStruct::isBankActive(uint64_t &bankNum) {
    return true; // Todo: Implement this
}

DramStruct::DramStruct() {
    this->bankStates = new std::vector<State>(8, State::CLOSE);
    this->banks = new std::vector<Bank*>(8);
    for (auto &bank : *banks) {
        bank = new Bank(0, 0);
    }
}

DramStruct::~DramStruct() {
    for (auto bank : *banks) {
        delete bank;
    }
    delete banks;
    delete bankStates;
}