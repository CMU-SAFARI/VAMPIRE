#!/usr/bin/env python2

# tests_vendor.py

# VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
# https://github.com/CMU-SAFARI/VAMPIRE

# Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zurich
# Released under the MIT License


import glob
import csv
import sys
import os
import helper as hp

def test_vendor():
    TEST_FILE_PREFIX = hp.VAMPIRE_DIR + "/tests/traces/vendors"
    test_pairs = [["0"]]
    VENDORS=["A", "B", "C"]
    tests_status = [0]*len(test_pairs)*len(VENDORS)

    vendor_iter = 0
    for vendor in VENDORS:
        test_pair_id = 0
        for test_pair in test_pairs:
            temp_result_f = []
            for test_id in test_pair:
                # Find the test trace for the test_id
                test_glob = TEST_FILE_PREFIX + "/" + test_id + "*"
                file = glob.glob(test_glob)

                # Only one test should match to a test id
                if len(file)==1:
                    file = file[0]
                else:
                    raise ValueError("Each test should have exactly one trace file: " + str(test_id))

                # Construct output csv file name
                csv_f = file + ".csv"

                # Execute VAMPIRE
                try:
                    hp.vampire(file, csv_f=csv_f, vendor=vendor)
                except:
                    print "Execution failed"
                    tests_status[test_pair_id] = 1

                temp_result_f.append(csv_f)

            # Compare csv files
            if not hp.check_for_nan(csv_arr=[csv.reader(open(f), delimiter=',') for f in temp_result_f]):
                print "Comparison failed"
                tests_status[test_pair_id] = 1

        test_result = ["passed", "failed"][tests_status[test_pair_id]]
        print "[test_vendor]: Test " + str(test_pair[0].split(".")[0]) + vendor.upper() + " " + test_result

        # Delete temporary files
        try:
            [os.remove(temp_result) for temp_result in temp_result_f]
        except OSError:
            # Assume csv creation was failed, TODO: improve this
            pass

        # print "[test_vendor]: Result:"
        pass_count = 0

    print "[test_vendor]: Result:"
    for status in tests_status:
        sys.stdout.write(["o","x"][status])
        if status == 0:
            pass_count += 1

    print ""
    print "[test_autoprecharge]: %d test completed, %d%% passed" %(len(tests_status), pass_count/len(tests_status)*100)

    if (pass_count == len(tests_status)):
        return (0, len(tests_status), pass_count)
    else:
        return (1, len(tests_status), pass_count)
        
def main():
    hp.setup()
    (result, _, _) = test_vendor()
    return result
main()
