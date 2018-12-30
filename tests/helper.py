# helper.py

# VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
# https://github.com/CMU-SAFARI/VAMPIRE

# Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zurich
# Released under the MIT License

import os.path
import subprocess
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


def vampire(trace_f, config="", vendor="A", csv_f="", data_model="RD_WR", parser="ASCII", dramSpec=None):
    if config == "":
        config = VAMPIRE_CFG

    if csv_f == "":
        vampire_cmd = "%s -f %s -c %s -v %s -d %s -p %s" \
                      % (VAMPIRE_PATH, trace_f, config, vendor, data_model, parser)
    else:
        vampire_cmd = "%s -f %s -c %s -v %s -csv %s -d %s -p %s" \
                      % (VAMPIRE_PATH, trace_f, config, vendor, csv_f, data_model, parser)

    if not dramSpec == None:
        vampire_cmd += " -dramSpec %s" % dramSpec
    exec_shell(vampire_cmd)

def convert_trace(in_f, out_f, data_model):
    trace_conv_cmd = "%s/tests/vampireAsciiToBin.py -i %s -o %s -d %s" \
          % (VAMPIRE_DIR, in_f, out_f, data_model)
    exec_shell(trace_conv_cmd)

def setup():
    global SCRIPT_DIR, error_str, VAMPIRE_PATH, VAMPIRE_DIR, VAMPIRE_CFG

    SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
    VAMPIRE_DIR = os.path.dirname(SCRIPT_DIR)
    VAMPIRE_CFG = VAMPIRE_DIR + "/configs/default.cfg"
    VAMPIRE_PATH = VAMPIRE_DIR + "/vampire"
    
    if not os.path.isfile(VAMPIRE_PATH):
        print "Unable to find VAMPIRE binary at %s, exiting..." % VAMPIRE_PATH
        exit(1)

# Raises exception if every csv in the csv_arr is not identical to others else returns True
def compare_csv(csv_arr):
    first_csv = csv_arr[1]
    first_csv_rows = [row for row in first_csv]

    for csv in csv_arr[1:]:
        row_iter = 0
        for row in csv:
            if not first_csv_rows[row_iter] == row:
                raise ValueError("CSV match failed")
            row_iter += 1
    return True

def check_for_nan(csv_arr):
    for csv in csv_arr:
        row_iter = 0
        for row in csv:
            if row[0] == "total energy" and (row[1] == "-nan" or row[1] == "nan"):
                return False
    return True
