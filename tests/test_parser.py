#!/usr/bin/env python2


# tests_parser.py

# VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
# https://github.com/CMU-SAFARI/VAMPIRE

# Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zurich
# Released under the MIT License

import glob
import csv
import sys
import os
import helper as hp


def test_parser_vendor(vendor="A"):
    TEST_FILE_PREFIX = hp.VAMPIRE_DIR + "/tests/traces/parser"
    test_pairs = [["0"]]
    DATA_MODELS = ["rd_wr", "wr", "dist", "mean"]
    PARSERS = ["ASCII", "BINARY"]

    tests_status = [0] * len(test_pairs) * len(DATA_MODELS)

    for data_model in DATA_MODELS:
        test_pair_id = 0
        for test_pair in test_pairs:
            temp_result_f = []
            for test_id in test_pair:
                # Find the test trace for the test_id
                test_glob = TEST_FILE_PREFIX + "/" + test_id + "*-" + data_model + "*"
                file = glob.glob(test_glob)

                # Only one test should match to a test id
                if len(file) == 1:
                    file = file[0]
                else:
                    raise ValueError("Each test should have exactly one trace file: " + str(test_id))

                # Construct output csv file name
                csv_f_ascii = file + "ascii.csv"
                csv_f_binary = file + "binary.csv"

                bin_file = file + ".bin"
                # Execute VAMPIRE
                try:
                    hp.convert_trace(file, bin_file, data_model=data_model.upper())
                    hp.vampire(file, csv_f=csv_f_ascii, vendor=vendor, data_model=data_model.upper(), parser="ASCII")
                    hp.vampire(bin_file, csv_f=csv_f_binary, vendor=vendor, data_model=data_model.upper(), parser="BINARY")
                except:
                    print "Execution failed"
                    tests_status[test_pair_id] = 1

                temp_result_f.append((csv_f_ascii, csv_f_binary))

            # Compare csv files
            for pair in temp_result_f:
                csv_arr = [csv.reader(open(f), delimiter=',') for f in pair]
                if not hp.check_for_nan(csv_arr=csv_arr):
                    print "Comparison failed"
                    tests_status[test_pair_id] = 1

        test_result = ["passed", "failed"][tests_status[test_pair_id]]
        print "[test_parser]: Test " + str(test_pair[0].split(".")[0]) + data_model + " " + test_result

        # Delete temporary files
        try:
            for temp_result in temp_result_f:
                os.remove(temp_result[0])
                os.remove(temp_result[1])
            # Find all the binaries generated to delete
            test_glob = TEST_FILE_PREFIX + "/" + "*.bin"
            bin_files = glob.glob(test_glob)
            for bin_f in bin_files:
                os.remove(bin_f)

        except OSError:
            # Assume csv creation was failed, TODO: improve this
            pass

        # print "[test_parser]: Result:"
        pass_count = 0

    print "[test_parser]: Result:"
    for status in tests_status:
        sys.stdout.write(["o", "x"][status])
        if status == 0:
            pass_count += 1

    print ""
    print "[test_parser]: %d test completed, %d%% passed" \
          % (len(tests_status), pass_count / len(tests_status) * 100)

    if (pass_count == len(tests_status)):
        return (0, len(tests_status), pass_count)
    else:
        return (1, len(tests_status), pass_count)


def main():
    hp.setup()
    VENDORS = ["A", "B", "C"]
    for vendor in VENDORS:
        print "Test for vendor %s" % vendor
        (result, _, _) = test_parser_vendor(vendor=vendor)
    return result


main()
