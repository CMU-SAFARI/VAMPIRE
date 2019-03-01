/*

MAIN.CPP

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#include "main.h"

template <typename T>
T get_param(const std::string *refEnumArr, const std::string paramStr, std::string paramName) {
    for (int member = 0; member < int(T::MAX); member++) {
        if (!strcmp(paramStr.c_str(), refEnumArr[member].c_str()))
            return (T)member;
    }

    msg::error("'" + paramStr + "' is not a valid option for parameter '" + paramName + "'");
}

void print_help() {
    const char *helpText =
            "usage:\n"
            "   vampire -f <trace_file_name> -c <config_file> -d {RD_WR|WR|MEAN|DIST} -p {BINARY|ASCII} [-v {A|B|C|Cust}] [-dramSpec <dramSpec_file>] [-s]\n"
            "\n"
            "options: \n"
            "   -f <trace_file_name>                Trace file to parse\n"
            "   -c <config_file>                    Config file\n"
            "   -d {MEAN|DIST|RD_WR|WR}             Data dependency model\n"
            "   -p {BINARY|ASCII}                   Specifies parser to be used (Note: Current traceGen only generates binary traces)\n"
            "   -v {A|B|C|Cust}                     Specifies vendor for calculations, default: A. Cust vendor requires an additional dramspec file, specified using -dramSpec option.\n"
            "   -dramSpec <dramspec_file>           Specifies DRAM specifications for calculations, required for Cust vendor type\n"
            // "   -e {BDI|CUSTOM|CUSTOM_MAX|NONE}     Specifies encoding, default: NONE\n"
            "   -s                                  Enables structural variations\n"
            "   -csv <csv_filename>                 Specifies filename for VAMPIRE to write stats as csv to. If the file exists, it is overwritten else a new file is\n"
            "                                       created.\n";

    std::cout << helpText;
}

void parse_args(int argc, char *argv[], Vampire &dram) {
    if (argc < 2) {
        print_help();
        exit(0);
    }

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_help();
            exit(0);
        }
    }

    for (int i = 1; i <argc; i++){
        if (strcmp(argv[i], "-f") == 0) {
            msg::error(argc <= i+1, "Option '-f': Trace file not specified.");
            dram.traceFilename = new std::string(argv[i+1]);
        }

        if (strcmp(argv[i], "-v") == 0){
            msg::error(argc <= i+1, "Option '-v': Vendor not specified.");
            msg::info("Using vendor: " + std::string(argv[i+1]));
            dram.vendorType = get_param<VendorType>(vendorString, argv[i + 1], "Vendor");
        }

        if (strcmp(argv[i], "-s") == 0){
            msg::info("Structural variation is now ON.");
            dram.structVar = StructVar::YES;
        }

        /*
        if (strcmp(argv[i], "-e") == 0){
            msg::error(argc <= i+1, "Option '-e': Encoding not specified.");
            msg::info("Using encodingType: " + std::string(argv[i+1]));
            dram.encodingType = get_param<EncodingType>(encodingString, argv[i + 1], "Encoding");
        }
        */

        if (strcmp(argv[i], "-d") == 0) {
            msg::error(argc <= i+1, "Option '-d': Data dependency model not specified.");
            msg::info("Using data dependency model (traceType): " + std::string(argv[i+1]));
            dram.traceType = get_param<TraceType>(traceTypeString, argv[i + 1], "TraceType");
        }

        if (strcmp(argv[i], "-c") == 0) {
            msg::error(argc <= i+1, "Option '-c': Config file not specified.");
            msg::info("Config file: " + std::string(argv[i+1]));
            dram.configFilename = new std::string(argv[i+1]);
        }

        if (strcmp(argv[i], "-p") == 0) {
            msg::error(argc <= i+1, "Option '-p': Parser type not specified.");
            msg::info("Using parserType: " + std::string(argv[i+1]));
            dram.parserType = get_param<ParserType>(parserTypeString, argv[i + 1], "ParserType");
        }

        if (strcmp(argv[i], "-dramSpec") == 0) {
            msg::error(argc <= i+1, "Option '-dramSpec': DramSpec file not specified.");
            msg::info("dramSpec file: " + std::string(argv[i+1]));
            dram.dramSpecFilename = new std::string(argv[i+1]);
        }

        if (strcmp(argv[i], "-csv") == 0) {
            msg::error(argc <= i+1, "Option '-csv': csv ouput filename not specified.");
            msg::info("Output csv file: " + std::string(argv[i+1]));
            dram.csvFilename = new std::string(argv[i+1]);
        }
    }
}

int main(int argc, char * argv[]){
    Vampire dram;
    FILE * fp;

    dram.vendorType = VendorType::A;
    dram.encodingType = EncodingType::NONE;
    dram.structVar = StructVar::NO;
    dram.traceType = TraceType::WR;

    parse_args(argc, argv, dram);
    dram.set_values();
    dram.init_structures[int(dram.traceType)]();

    if ((dram.encodingType == EncodingType::CUSTOM) || (dram.encodingType == EncodingType::CUSTOM_ADV)){
        fp = fopen("encoding.bin", "rb");
        if (fp == NULL){
            std::cout << RED << "CANNOT USE CUSTOM ENCODING.";
            std::cout << " encoding.bin not found. Run tools/make_encoding.py" << RESET << std::endl;
            dram.free_memory();
            return 0;
        }
        fclose(fp);
        dram.make_encoding_table();
    }

    if (dram.estimate() == -1){
      msg::error("Something went wrong, :-(");
    }
    return 0;
}
