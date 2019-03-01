/*

EQUATIONS.CPP

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#include "helper.h"
#include "equations.h"
#include "dramStruct.h"

/********************************************************************/
/********************* DATA DEPENDENCY AND TOGGLE *******************/
/********************************************************************/
void Equations::init_struct_var() {
    /* Initialize RD current values for the 8 banks */
    rdStructVarCurrent[int(VendorType::A)].assign({
            241.6258f/241.6258f, 243.7769f/241.6258f, 255.7692f/241.6258f, 259.8341f/241.6258f,
            243.5455f/241.6258f, 246.2226f/241.6258f, 256.0077f/241.6258f, 260.2505f/241.6258f
    });
    rdStructVarCurrent[int(VendorType::B)].assign({
            222.4698f/222.4698f, 228.8632f/222.4698f, 225.5057f/222.4698f, 226.3366f/222.4698f,
            227.9259f/222.4698f, 225.7385f/222.4698f, 225.361f/222.4698f, 231.3116f/222.4698f
    });
    rdStructVarCurrent[int(VendorType::C)].assign({
            222.4698f/222.4698f, 228.8632f/222.4698f, 225.5057f/222.4698f, 226.3366f/222.4698f,
            227.9259f/222.4698f, 225.7385f/222.4698f, 225.3615f/222.4698f, 231.3116f/222.4698f
    });

    actStructVarCurrent[int(VendorType::A)] = [this] (int row) -> float {
        unsigned int n1 = __builtin_popcount(row); /* # of 1s */
        return (float)(-0.0024*pow(n1,3) + 0.0333*pow(n1,2) + 0.3106*n1 + 48.034);
    };
    actStructVarCurrent[int(VendorType::B)] = [this] (int row) -> float {
        unsigned int n1 = __builtin_popcount(row); /* # of 1s */
        return (float)(0.0084*pow(n1,3) -0.0499*pow(n1,2) + 0.1724*n1 + 41.994);
    };
    actStructVarCurrent[int(VendorType::C)] = [this] (int row) -> float {
        unsigned int n1 = __builtin_popcount(row); /* # of 1s */
        return (float)(0.0002*pow(n1,5) -0.0104*pow(n1,4) + 0.1582*pow(n1,3) -0.9256*pow(n1,2) + 2.0187*n1 + 37.527);
    };
}

float Equations::lookup_toggle_current_slope(CommandType request, MappedAdd &newAdd, MappedAdd &oldAdd) {

    float current_slope = 0.0;

    if (vendorType == VendorType::A){

        if ((newAdd.bank != oldAdd.bank) && (newAdd.col== oldAdd.col)){ /* Bank Interleaving */
            if (request == CommandType::RD) {current_slope = 0.160;}
            else {current_slope = 0.180;}
        }
        else if ((newAdd.bank != oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Bank and Col Interleaving */
            if (request == CommandType::RD) {current_slope = 0.160;}
            else {current_slope = 0.180;}
        }
        else if ((newAdd.bank == oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Col Interleaving */
            if (request == CommandType::RD) {current_slope = 0.431;}
            else {current_slope = 0.712;}
        }
        else{ /* No Interleaving */
            current_slope = 0;
        }
    }

    else if (vendorType == VendorType::B){

        if ((newAdd.bank != oldAdd.bank) && (newAdd.col== oldAdd.col)){ /* Bank Interleaving */
            if (request == CommandType::RD) {current_slope = 0.291;}
            else {current_slope = 0.062;}
        }
        else if ((newAdd.bank != oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Bank and Col Interleaving */
            if (request == CommandType::RD) {current_slope = 0.291;}
            else {current_slope = 0.062;}
        }
        else if ((newAdd.bank == oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Col Interleaving */
            if (request == CommandType::RD) {current_slope = 0.548;}
            else {current_slope = 0.204;}
        }
        else{ /* No Interleaving */
            if (request == CommandType::RD) {current_slope = 0;}
            else {current_slope = 0;}
        }
    }

    else if (vendorType == VendorType::C){

        if ((newAdd.bank != oldAdd.bank) && (newAdd.col== oldAdd.col)){ /* Bank Interleaving */
            if (request == CommandType::RD) {current_slope = 0.364;}
            else {current_slope = 0.072;}
        }
        else if ((newAdd.bank != oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Bank and Col Interleaving */
            if (request == CommandType::RD) {current_slope = 0.364;}
            else {current_slope = 0.072;}
        }
        else if ((newAdd.bank == oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Col Interleaving */
            if (request == CommandType::RD) {current_slope = 0.693;}
            else {current_slope = 0.259;}
        }
        else{ /* No Interleaving */
            current_slope = 0;
        }
    }

    return current_slope;
}


float Equations::lookup_current_intercept(CommandType request, MappedAdd oldAdd, MappedAdd newAdd) {

    float current_intercept;

    if (vendorType == VendorType::A){

        if ((newAdd.bank != oldAdd.bank) && (newAdd.col== oldAdd.col)){ /* Bank Interleaving */
            if (request == CommandType::RD) {current_intercept = 287.235;}
            else {current_intercept = 732.500;}
        }
        else if ((newAdd.bank != oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Bank and Col Interleaving */
            if (request == CommandType::RD) {current_intercept = 277.128;}
            else {current_intercept = 735.149;}
        }
        else if ((newAdd.bank == oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Col Interleaving */
            if (request == CommandType::RD) {current_intercept = 246.440;}
            else {current_intercept = 728.750;}
        }
        else{ /* No Interleaving */
            if (request == CommandType::RD) {current_intercept = 250.879;}
            else {current_intercept = 687.178;}
        }
    }

    else if (vendorType == VendorType::B){

        if ((newAdd.bank != oldAdd.bank) && (newAdd.col== oldAdd.col)){ /* Bank Interleaving */
            if (request == CommandType::RD) {current_intercept = 228.142;}
            else {current_intercept = 617.553;}
        }
        else if ((newAdd.bank != oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Bank and Col Interleaving */
            if (request == CommandType::RD) {current_intercept = 223.613;}
            else {current_intercept = 618.0;}
        }
        else if ((newAdd.bank == oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Col Interleaving */
            if (request == CommandType::RD) {current_intercept = 217.420;}
            else {current_intercept = 664.403;}
        }
        else{ /* No Interleaving */
            if (request == CommandType::RD) {current_intercept = 226.689;}
            else {current_intercept = 645.515;}
        }
    }

    else { /* (vendorType == VendorType::C */

        if ((newAdd.bank != oldAdd.bank) && (newAdd.col== oldAdd.col)){ /* Bank Interleaving */
            if (request == CommandType::RD) {current_intercept = 289.992;}
            else {current_intercept = 501.901;}
        }
        else if ((newAdd.bank != oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Bank and Col Interleaving */
            if (request == CommandType::RD) {current_intercept = 266.511;}
            else {current_intercept = 520.791;}
        }
        else if ((newAdd.bank == oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Col Interleaving */
            if (request == CommandType::RD) {current_intercept = 234.417;}
            else {current_intercept = 565.860;}
        }
        else{ /* No Interleaving */
            if (request == CommandType::RD) {current_intercept = 222.114;}
            else {current_intercept = 540.974;}
        }
    }

    return current_intercept;
}

// TODO move this and all other similar functions to dramspec
float Equations::lookup_current_slope(CommandType request, MappedAdd oldAdd, MappedAdd newAdd) {
    float current_slope;

    if (vendorType == VendorType::A){

        if ((newAdd.bank != oldAdd.bank) && (newAdd.col== oldAdd.col)){ /* Bank Interleaving */
            if (request == CommandType::RD) {current_slope =  5.035;}
            else {current_slope = -5.079f;}
        }
        else if ((newAdd.bank != oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Bank and Col Interleaving */
            if (request == CommandType::RD) {current_slope =  5.219;}
            else {current_slope = -5.082f;}
        }
        else if ((newAdd.bank == oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Col Interleaving */
            if (request == CommandType::RD) {current_slope =  6.558;}
            else {current_slope = -5.056f;}
        }
        else{ /* No Interleaving */
            if (request == CommandType::RD) {current_slope =  6.679;}
            else {current_slope = -4.820f;}
        }
    }

    else if (vendorType == VendorType::B){

        if ((newAdd.bank != oldAdd.bank) && (newAdd.col== oldAdd.col)){ /* Bank Interleaving */
            if (request == CommandType::RD) {current_slope =  4.358;}
            else {current_slope = -4.517f;}
        }
        else if ((newAdd.bank != oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Bank and Col Interleaving */
            if (request == CommandType::RD) {current_slope =  4.302;}
            else {current_slope = -4.521f;}
        }
        else if ((newAdd.bank == oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Col Interleaving */
            if (request == CommandType::RD) {current_slope =  4.342;}
            else {current_slope = -4.810f;}
        }
        else{ /* No Interleaving */
            if (request == CommandType::RD) {current_slope =  4.400;}
            else {current_slope = -4.613f;}
        }
    }

    else { /* vendorType == VendorType::C */

        if ((newAdd.bank != oldAdd.bank) && (newAdd.col== oldAdd.col)){ /* Bank Interleaving */
            if (request == CommandType::RD) {current_slope =  3.360;}
            else {current_slope = -3.516f;}
        }
        else if ((newAdd.bank != oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Bank and Col Interleaving */
            if (request == CommandType::RD) {current_slope =  3.877;}
            else {current_slope = -3.661f;}
        }
        else if ((newAdd.bank == oldAdd.bank) && (newAdd.col!= oldAdd.col)){ /* Col Interleaving */
            if (request == CommandType::RD) {current_slope =  4.321;}
            else {current_slope = -4.012f;}
        }
        else{ /* No Interleaving */
            if (request == CommandType::RD) {current_slope =  4.155;}
            else {current_slope = -3.807f;}
        }
    }

    return current_slope;
}

double_t Equations::calc_rd_wr_energy(CommandType request, MappedAdd add, unsigned int *data, uint32_t numSetBits,
                                      uint32_t numToggleBits, IO_data IO_buffer, EncodingType encodingType) {
    double_t result = 0.0;
    auto channel = add.channel, rank = add.rank, bank = add.bank, row = add.row, col = add.col;
    float current_slope, current_intercept, current_toggle_slope;

    //keep data read from file instead of memory structure if traceType is RD_WR
    if (request == CommandType::RD && traceType == TraceType::WR){
        for (int fourbytes = 0; fourbytes < 16; fourbytes++){
            data[fourbytes]  = (*memory)[channel][rank][bank][row][col].data[fourbytes];
        }
    }

    current_intercept    = lookup_current_intercept(request, add, IO_buffer.prevAdd);
    current_slope        = lookup_current_slope(request, add, IO_buffer.prevAdd);
    current_toggle_slope = lookup_toggle_current_slope(request, IO_buffer.prevAdd, add);

    dbgstream
            << "current_intercept: " << current_intercept
            << ", current_slope: " << current_slope
            << ", current_toggle_slope :" << current_toggle_slope;

    if ((request == CommandType::WR) && (encodingType == EncodingType::CUSTOM_ADV))
        numSetBits = 512 - numSetBits;

    if (request == CommandType::RD || request == CommandType::WR) {
        double_t current = ((numSetBits / 8.0f) * current_slope
                            + current_intercept
                            + (numToggleBits / 8.0f) * current_toggle_slope);

        result = current * dramSpec.getCmdLength()[int(request)];
    } else
        msg::error(FUNCTION_STR + ": Request is not an IO type");

    assert(result != 0);
    return result;
}

/* Lookup power numbers based on structural variation for given request.
   Find energy and total request time */
double_t Equations::struct_var_power_adjustment(CommandType request, MappedAdd add) {
    switch (int(request)) {
        case (int(CommandType::RD)):
            return rdStructVarCurrent[int(vendorType)][add.bank];

        case (int(CommandType::ACT)):
        case (int(CommandType::PRE)):
            return actStructVarCurrent[int(vendorType)](add.row);
        default:
            return 1;
    }
}

/********************/
/* Class: Equations */
/********************/
Equations::Equations(Statistics &statistics, DramSpec &dramSpec, Config &configs, VendorType &vendorType,
                     TraceType &tracetype, DRAMdata ******memory, StructVar structVar)
        : statistics(statistics), dramSpec(dramSpec), configs(configs), vendorType(vendorType),
          traceType(tracetype), memory(memory), structVar(structVar){
    this->init_struct_var();
}

