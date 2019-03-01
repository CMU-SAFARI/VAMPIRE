#!/usr/bin/env python2

# test_cust_vendor.py

# VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
# https://github.com/CMU-SAFARI/VAMPIRE

# Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zurich
# Released under the MIT License

import glob
import csv
import sys
import os
import helper as hp


def test_cust_vendor_vendor():
    TEST_FILE_PREFIX = hp.VAMPIRE_DIR + "/tests/traces/common.rand.trace"
    DRAM_SPEC_FILE = hp.VAMPIRE_DIR + "/tests/configs/cust_vendor/vendorC.cfg"


    file = TEST_FILE_PREFIX
    csv_cust_f = TEST_FILE_PREFIX + ".cust.csv"
    csv_c_f = TEST_FILE_PREFIX + ".c.csv"

    hp.vampire(trace_f=file, csv_f=csv_cust_f, vendor="Cust", dramSpec=DRAM_SPEC_FILE)
    hp.vampire(trace_f=file, csv_f=csv_c_f, vendor="C")

    print "[test_cust_vendor]:",
    if hp.compare_csv([csv.reader(open(f), delimiter=',') for f in [csv_c_f, csv_cust_f]]):
        print "passed"
    else:
        print "failed"

def main():
    hp.setup()
    test_cust_vendor_vendor()

main()
