#! /usr/bin/env python2

# vampireAsciiToBin.py

# VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
# https://github.com/CMU-SAFARI/VAMPIRE

# Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zurich
# Released under the MIT License

# Description: Converts VAMPIRE's ASCII trace to binary trace format

import sys
import struct
import getopt

USAGE = "Usage: ./vampireAsciiToBin -i=input -o <output_filename> -d (RD_WR|WR|DIST|MEAN)"
COMMANDS = {
    'RD' : 0b000,
    'WR' : 0b001,
    'ACT': 0b010,
    'PRE': 0b011,
    'RDA': 0b100,
    'WRA': 0b101
}
IO_CMDS = ["RD", "RDA", "WR", "WRA"]
WRITE_CMDS = ["WR", "WRA"]
READ_CMDS = ["RD", "RDA"]
DATA_MODELS = ["RD_WR","WR","DIST","MEAN"]

in_f = None
out_f = None
data_model = None

def chunkstring(string, length):
    return [string[0+i:length+i] for i in range(0, len(string), length)]

def error(msg, cond=True, print_help=False):
    if (cond):
        print "Error: " + msg
        if (print_help):
            print USAGE
        sys.exit(1)
        
def parse_args(args):
    global in_f, out_f, data_model
    try:
        opts, args = getopt.getopt(args, 'i:o:d:h')
    except getopt.GetoptError:
        error("Parameter parse error, exiting...", print_help=True)    
    
    for opt in opts:
        if opt[0] == "-i":
            in_f = open(opt[1])
        elif opt[0] == "-o":
            out_f = open(opt[1], "wb")
        elif opt[0] == "-h":
            print USAGE
            sys.exit(0)
        elif opt[0] == "-d":
            data_model = opt[1]

    if (in_f == None):
        error("No input file found.", print_help=True)
    if (out_f == None):
        error("No output file found.", print_help=True)
    if (data_model == None):
        error("No data model specified.", print_help=True)
    if (data_model not in DATA_MODELS):
        error("Unknown data model.", print_help=True)
        
def main():
    parse_args(sys.argv[1:])
    for line in in_f:
        split = line.strip().split(',')

        time = split[0]
        cmd = split[1]
        bank = split[2]

        time_b = long(time)
        cmd_b  = long(COMMANDS[cmd])

        chan_b = long(0)
        rank_b = long(0)
        bank_b = long(bank)
        row_b  = long(0)
        col_b  = long(0)
        
        data_b = [int(0)]*(128*4/32)

        has_data = (data_model == "WR" and cmd in WRITE_CMDS) or (data_model == "RD_WR" and cmd in IO_CMDS)
        if cmd in IO_CMDS:
            col_b = long(split[3])

            if (has_data):
                data_str = chunkstring(split[4], 8)            
                chunk_iter = 0
                for chunk in data_str:
                    data_b[chunk_iter] = int(data_str[chunk_iter], 16)
                    chunk_iter += 1
        elif cmd == "ACT":
            row_b = long(split[3])
        elif cmd not in COMMANDS:
            print "Unknown command: " + cmd
            exit(1)

        add_b = col_b | (row_b<<7) | (bank_b<<23) | (rank_b<<26) | (chan_b<<28) | (cmd_b<<30)

        # Write the bits to the file
        out_f.write(struct.pack('=q', time_b))
        out_f.write(struct.pack('=q', add_b))

        if (has_data):
            for chunk in data_b:
                out_f.write(struct.pack('=l', chunk))

    
if __name__ == "__main__":
    main()
    sys.exit(0)
