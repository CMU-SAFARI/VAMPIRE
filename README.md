# VAMPIRE

VAMPIRE (Variation-Aware model of Memory Power Informed by Real Experiments) is an open-source DRAM power model 
based on an extensive experimental characterization of the power consumption of real DRAM modules.
VAMPIRE can operate on any trace that is generated in the [DRAMPower format](http://www.drampower.info),
and is compatible with widely-used memory simulators such as
[Ramulator](https://github.com/CMU-SAFARI/ramulator/) and
[DRAMSim2](https://github.com/umd-memsys/DRAMSim2/).

*Current version: 1.1.0 (released March 1, 2019)*


## Key Features

VAMPIRE includes the following major features that are not included in prior DRAM power modeling tools:

- typical-case power consumption values (as opposed to worst-case power consumption)
- data-dependent power consumption behavior
- the impact structural variation within a module on power consumption
- power consumption models based on extensive data collected from DRAM modules by three major vendors


For more information, please read our paper:
>S. Ghose, A. G. Yağlıkçı, R. Gupta, D. Lee, K. Kudrolli, W. X. Liu, H. Hassan, K. K. Chang, N. Chatterjee, A. Agrawal, M. O'Connor, O. Mutlu.
>"[**What Your DRAM Power Models Are Not Telling You: Lessons from a Detailed Experimental Study**](http://users.ece.cmu.edu/~saugatag/papers/18sigmetrics_vampire.pdf)".
>In _Proceedings of the ACM International Conference on Measurement and Modeling of Computer Systems (SIGMETRICS)_, June 2018.
>To be published in _Proceedings of the ACM on Measurement and Analysis of Computing Systems (POMACS)_, Vol. 2, No. 3, December 2018.


## Using VAMPIRE

### Building the VAMPIRE Executable
```shell
make -j
```

*Note: VAMPIRE currently requires a C++11 compiler (e.g., `clang++`, `g++-5`).*

### Running tests
VAMPIRE includes some inbuilt test to verify functional correctness, however, please note that these tests do not cover 100% of the functionalities.  
```shell
make tests
```

On a successful run `make` will exit will code 0
### Generating an Input Trace File

VAMPIRE requires an input file that contains a trace (i.e., list) of DDR3 DRAM commands.  This can take one of two formats:

1. Binary
2. ASCII (compatible with [DRAMPower trace format](http://www.drampower.info))

#### Binary Trace Format

```
<64-bit timestamp of current cycle><31 bits of zero padding><command type (3 bits)><channel ID (2 bits)><rank ID (2 bits)><bank ID (3 bits)><row ID (16 bits)><column number (7 bits)>[<64-byte data value; optional, for reads and writes only>]
```

Valid values for the command type (in binary) are:
1. 000: RD
2. 001: WR
3. 010: ACT
4. 011: PRE
5. 100: RDA
6. 101: WRA

#### ASCII Trace Format

```
<timestamp (i.e., current cycle)>,<command>,<bank>[,row/col][,<data>]
```

**NOTES:**  
1. `ACT` commands require a row number while I/O commands (`RD`, `RDA`, `WR` and `WRA`) require a column number.
1. Certain data dependency models require data for corresponding I/O command (e.g. for `WR` and `WRA` with WR data dependency model).

#### Generating Random Binary Test Traces Using `traceGen`
```shell
./traceGen 10000
```

This generates a number of trace files with 10000 random requests in each trace file.  
  
### Running VAMPIRE

```shell
./vampire -f <file_name> -c <config_file> -d {MEAN|DIST|WR|RD_WR} -p {BINARY|ASCII} [-v {A|B|C|Cust}] [-s] [-dramSpec <dramspec_file>]
```

#### Required Command-Line Arguments

```
   -f <trace_filename>                 Trace file to parse
   -c <config_filename>                Configuration file
   -d {MEAN|DIST|WR|RD_WR}             Data dependency model
   -p {BINARY|ASCII}                   Parsing format to use for trace file
```

#### Optional Command-Line Arguments

```
   -v {A|B|C|Cust}                     Vendor to perform power calculations for (default: A; see the paper for more information). 
                                       Cust vendor requires an additional dramspec file, specified using -dramSpec option.
   -dramSpec <dramspec_file>           Specifies DRAM specifications for calculations, required for Cust vendor type (An example in dramSpec/example.cfg).
   -s                                  Enables structural variation modeling (default: disabled).
   -csv <csv_filename>                 Specifies filename for VAMPIRE to write stats as csv to. If the file exists, it is overwritten else a new file is 
                                       created.               
```
#### Data Dependency Models
1. __MEAN__:
   VAMPIRE assumes that all read and write requests consume a mean energy value.
   The energy of each read/write request is based on the mean number of ones in a 64-byte cache line for your application.
   The mean number of ones is read from the configuration file.
   The input trace should *not* contain any data values for read or write requests.
2. __DIST__:
   VAMPIRE assumes that the energy consumed by the read and write requests follows a distribution based on the number of ones in all cache lines.
   The energy of each read/write request is selected to fit the distribution of cache lines based on the number of ones in each 64-byte line.
   The distribution is read from the configuration file.
   The input trace should *not* contain any data values for read or write requests.   
3. __WR__:
   VAMPIRE models the energy consumed by reads and writes based on the actual data value used by the application.
   In this model, VAMPIRE allocates a memory data block, where the current data value of each line is stored in the simulator.
   The data value for each write request is read directly from the trace file, and the data value for each read request is determined from the memory data block.
   The input trace should contain data values for write requests, but not for read requests.
   (Note: this model requires a large amount of RAM on your system to model all of the memory data.)
4. __RD_WR__:
   VAMPIRE models the energy consumed by reads and writes based on the actual data value used by the application.
   In this model, VAMPIRE does *not* need to allocate a memory data block.
   The data value for each read/write request is read directly from the trace file.
   The input trace should contain data values for both read and write requests.
   (Note: unlike the WR model, this model does not consume a large amount of RAM.)

#### Example
The following command uses the RD_WR data dependency model, a compatible RD_WR binary trace generated by traceGen, the default configuration file (`configs/default.cfg`), and Vendor B to estimate energy.

```shell
./vampire -f ./trace_rd_wr_t.bin -v B -c configs/default.cfg -d RD_WR
```

### Piping commands to VAMPIRE
VAMPIRE supports reading trace commands from pipe (`/dev/stdin`) instead of a trace file, allowing VAMPIRE to process the commands as they are fed. To read input from pipe use `/dev/stdin` as the input trace file (option `-f`) and pipe the commands to VAMPIRE binary.

e.g.  
```shell
cat sample_vampire_trace.trace | ./vampire -f /dev/stdin [options]
```

## Summary of VAMPIRE Execution Flow
A short description of the execution flow of VAMPIRE through the source code is available [here](src/README.md).


## Maintainers

VAMPIRE is maintained by the SAFARI Research Group at Carnegie Mellon University and ETH Zürich.

Contributors include:

- Raghav Gupta
- Suyash Mahar
- Sumit Kumar Yadav
- Saugata Ghose
- A. Giray Yağlıkçı

VAMPIRE is released under the MIT License.


