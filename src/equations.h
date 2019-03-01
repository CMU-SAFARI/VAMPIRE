/*

EQUATIONS.H

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#ifndef VAMPIRE_EQUATIONS_H
#define VAMPIRE_EQUATIONS_H

#include "dramSpec.h"
#include "statistics.h"
#include "helper.h"
#include "config.h"
#include "dramStruct.h"

#include <memory>

class Equations {
protected:
    Statistics statistics;
    DramSpec dramSpec;
    Config configs;
    TraceType traceType;
    VendorType vendorType;
    DRAMdata ******memory;
    StructVar structVar;

    /* Lookup values for current variation based on structural variations */
    std::vector<float> rdStructVarCurrent[int(VendorType::MAX)];                // For 8 banks
    std::function<float(int)> actStructVarCurrent[int(VendorType::MAX)]; // For current based on LR model

public:
    Equations(Statistics &statistics, DramSpec &dramSpec, Config &configs, VendorType &vendorType,
                  TraceType &traceType, DRAMdata ******memory, StructVar structVar);
    ~Equations() = default;
    double_t calc_rd_wr_energy(CommandType request, MappedAdd add, unsigned int *data, uint32_t numSetBits,
                               uint32_t numToggleBits, IO_data IO_buffer, EncodingType encodingType);
    void init_struct_var();
    void calculate_results(uint64_t &endTime, DramStruct &dramStruct);
    double_t struct_var_power_adjustment(CommandType request, MappedAdd add);
    float lookup_current_slope(CommandType, MappedAdd oldAdd, MappedAdd newAdd);
    float lookup_current_intercept(CommandType, MappedAdd oldAdd, MappedAdd newAdd);
    float lookup_toggle_current_slope   (CommandType, MappedAdd &newAdd, MappedAdd &oldAdd);

};

#endif //VAMPIRE_EQUATIONS_H
