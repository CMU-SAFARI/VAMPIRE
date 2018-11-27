/*

DRAMSPEC.H

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#ifndef VAMPIRE_DRAMSPEC_H
#define VAMPIRE_DRAMSPEC_H


#include <cstdint>
#include <vector>
#include <cmath>
#include <memory>
#include "consts.h"
#include "helper.h"

class DramSpec {
private:
protected:
public:
    DramSpec();
    ~DramSpec() = default;

    std::shared_ptr<std::vector<double_t>> cmdLength_ptr;
    std::shared_ptr<std::vector<double_t>> cmdCurrent_ptr;

    std::vector<double_t> &getCmdLength();
    std::vector<double_t> &getCmdCurrent();

    uint64_t cmdLengthInCycles(CommandType cmdType) {
        return static_cast<uint64_t>(
                round((*cmdLength_ptr)[static_cast<uint64_t>(cmdType)]*memClkSpeed*0.001));
    };
 // +0.5 for rounding off

    double_t vdd;
    double_t memClkSpeed = 800/2; // = busSpeed/2 (MHz)

    double_t actCmdEnergy = 0;
    double_t preCmdEnergy = 0;
    double_t actStandbyEnergy = 0;
    double_t preStandbyEnergy = 0;
};

class DramSpec_A : public DramSpec {
private:
protected:
public:
    using DramSpec::DramSpec;
    DramSpec_A();
    ~DramSpec_A() = default;
};

class DramSpec_B : public DramSpec {
private:
protected:
public:
    DramSpec_B();
    ~DramSpec_B() = default;
};

class DramSpec_C : public DramSpec {
private:
protected:
public:
    DramSpec_C();
    ~DramSpec_C() = default;
};

// For reading specifications from a file
class DramSpec_Cust : public DramSpec {
private:
protected:
public:
    DramSpec_Cust(const std::string &fname);
    ~DramSpec_Cust() = default;
};

#endif //VAMPIRE_DRAMSPEC_H
