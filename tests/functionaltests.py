#!/usr/bin/env python2

import subprocess
import os.path
import glob
import csv
import sys

SCRIPT_DIR = ""
VAMPIRE_DIR = ""
VAMPIRE_CFG = ""

def exec_shell(cmd, get_output=False, stdout="/dev/null"):
    if (get_output):
        try:
            return (0,subprocess.check_output(cmd.split()))
        except subprocess.CalledProcessError, e:
            return (e.returncode, e.message)
    else:
        returncode = subprocess.call(cmd.split(), stdout=open(stdout))
        return (returncode, "")

def vampire(trace_f, config="", vendor="A", csv_f="", data_model="RD_WR", parser="ASCII"):
    if config == "":
        config = VAMPIRE_CFG
    
    if csv_f == "":
        vampire_cmd = "%s -f %s -c %s -v %s -d %s -p %s" \
                      % (VAMPIRE_PATH, trace_f, config, vendor, data_model, parser)
    else:
        vampire_cmd = "%s -f %s -c %s -v %s -csv %s -d %s -p %s" \
                      % (VAMPIRE_PATH, trace_f, config, vendor, csv_f, data_model, parser)
    exec_shell(vampire_cmd)
    
def setup():
    global SCRIPT_DIR, error_str, VAMPIRE_PATH, VAMPIRE_DIR, VAMPIRE_CFG

    SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
    VAMPIRE_DIR = os.path.dirname(SCRIPT_DIR)
    VAMPIRE_CFG = VAMPIRE_DIR + "/configs/default.cfg"
    VAMPIRE_PATH = VAMPIRE_DIR + "/vampire"
    
    if not os.path.isfile(VAMPIRE_PATH):
        print "Unable to find vamire binary at %s, exiting..." % VAMPIRE_PATH
        exit(1)

def compare_csv(csv_arr):
    first_csv = csv_arr[1]
    first_csv_rows = [row for row in first_csv]
    
    for csv in csv_arr[1:]:
        row_iter = 0
        for row in csv:
            if not first_csv_rows[row_iter] == row:
                raise ValueError("CSV match failed")
            row_iter += 1

def run_test_autoprecharge():
    TEST_FILE_PREFIX = VAMPIRE_DIR + "/tests/traces"
    test_pairs = [["0.0", "0.1"],["1.0", "1.1"]]
    tests_status = [0]*len(test_pairs)

    test_pair_id = 0
    for test_pair in test_pairs:
        temp_result_f = []
        for test_id in test_pair:
            # Find the test trace for the test_id
            test_glob = TEST_FILE_PREFIX + "/" + test_id + "*"
            file = glob.glob(test_glob)

            # Only one test should match to a test id
            if (len(file)==1):
                file = file[0]
            else:
                raise ValueError("More than one match for test code: " + str(test_id))

            # Construct output csv file name
            csv_f = file + ".csv"

            # Execute VAMPIRE
            try:
                vampire(file, csv_f=csv_f)
            except:
                tests_status[test_pair_id] = 1
                
            temp_result_f.append(csv_f)

        # Compare csv files
        try:
            compare_csv([csv.reader(open(f), delimiter=',') for f in temp_result_f])
        except:
            tests_status[test_pair_id] = 1

        test_result = ["passed", "failed"][tests_status[test_pair_id]]
        print "[test_autoprecharge]: Test " + str(test_pair[0].split(".")[0]) + " " + test_result

        # Delete temporary files
        [os.remove(temp_result) for temp_result in temp_result_f]

    print "\n[test_autoprecharge]: Result:"
    pass_count = 0

    for status in tests_status:
        sys.stdout.write(["o","x"][status])
        if status == 0:
            pass_count += 1

    print "\n"
    print "[test_autoprecharge]: %d test completed, %d%% passed" %(len(tests_status), pass_count/len(tests_status)*100)

    if (pass_count == len(tests_status)):
        return (0, len(tests_status), pass_count)
    else:
        return (1, len(tests_status), pass_count)
        
def main():
    setup()
    (result, _, _) = run_test_autoprecharge()
    return result
main()
