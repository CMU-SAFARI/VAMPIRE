/*

DRAMSPEC.CPP

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#include <iostream>
#include "dramSpec.h"
#include "helper.h"

DramSpec::DramSpec() {
    cmdLength_ptr.reset(new std::vector<double_t>(8));
    cmdCurrent_ptr.reset(new std::vector<double_t>(8));
}

std::vector<double_t> &DramSpec::getCmdLength() {
    return *this->cmdLength_ptr;
}

std::vector<double_t> &DramSpec::getCmdCurrent() {
    return *this->cmdCurrent_ptr;
}

DramSpec_A::DramSpec_A() : DramSpec() {
    auto &cmdLength = *cmdLength_ptr;
    auto &cmdCurrent = *cmdCurrent_ptr;

    cmdLength.resize(static_cast<uint64_t>(CommandType::MAX));
    cmdCurrent.resize(static_cast<uint64_t>(CommandType::MAX));
    for (int i = 0; i < int(CommandType::MAX); i++) {
        getCmdLength()[i] = 0;
        cmdCurrent[i] = 0;
    }

    vdd = 1.35f;

    /* Latencies (ns) */
    cmdLength[int(CommandType::ACT)] = 15.0;
    cmdLength[int(CommandType::PRE)] = 15.0;
    cmdLength[int(CommandType::RD)] = 25.0;
    cmdLength[int(CommandType::WR)] = 25.0;

    /* Currents (mA) */
    cmdCurrent[int(CommandType::ACT)] = 68.9f;
    cmdCurrent[int(CommandType::PRE)] = 68.9f;

    /* Pre-calculated energies (pJ/1cycle = pJ/2.5ns) */
    actCmdEnergy = 1114.961;
    preCmdEnergy = 1114.961;

    actStandbyEnergy = 129.3598;
    preStandbyEnergy = 119.0111;
}

DramSpec_B::DramSpec_B() : DramSpec() {
    auto &cmdLength = *cmdLength_ptr;
    auto &cmdCurrent = *cmdCurrent_ptr;

    cmdLength.resize(static_cast<uint64_t>(CommandType::MAX));
    cmdCurrent.resize(static_cast<uint64_t>(CommandType::MAX));
    for (int i = 0; i < int(CommandType::MAX); i++) {
        cmdLength[i] = 0;
        cmdCurrent[i] = 0;
    }

    vdd = 1.35f;

    /* Latencies (ns) */
    cmdLength[int(CommandType::ACT)] = 15.0;
    cmdLength[int(CommandType::PRE)] = 15.0;
    cmdLength[int(CommandType::RD)] = 25.0;
    cmdLength[int(CommandType::WR)] = 25.0;

    /* Currents (mA) */
    cmdCurrent[int(CommandType::ACT)] = 69.8f;
    cmdCurrent[int(CommandType::PRE)] = 69.8f;

    /* Pre-calculated energies (pJ/1cycle = pJ/2.5ns) */
    actCmdEnergy = 739.2047;
    preCmdEnergy = 739.2047;

    actStandbyEnergy = 161.6844;
    preStandbyEnergy = 181.3352;
}

DramSpec_C::DramSpec_C() : DramSpec() {
    auto &cmdLength = *cmdLength_ptr;
    auto &cmdCurrent = *cmdCurrent_ptr;

    cmdLength.resize(static_cast<uint64_t>(CommandType::MAX));
    cmdCurrent.resize(static_cast<uint64_t>(CommandType::MAX));
    for (int i = 0; i < int(CommandType::MAX); i++) {
        cmdLength[i] = 0;
        cmdCurrent[i] = 0;
    }

    /* Latencies (ns) */
    cmdLength[int(CommandType::ACT)] = 13.125;
    cmdLength[int(CommandType::PRE)] = 13.125;
    cmdLength[int(CommandType::RD)] = 23.125;
    cmdLength[int(CommandType::WR)] = 23.125;

    vdd = 1.35f;

    /* Currents (mA) */
    cmdCurrent[int(CommandType::ACT)] = 58.6f;
    cmdCurrent[int(CommandType::PRE)] = 58.6f;

    /* Pre-calculated energies (pJ/1cycle = pJ/2.5ns) */
    actCmdEnergy = 812.3848;
    preCmdEnergy = 812.3848;

    actStandbyEnergy = 112.6297;
    preStandbyEnergy = 122.75881;
}
