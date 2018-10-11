## Simplified execution flow of VAMPIRE
VAMPIRE uses `main.cpp` as the entry point, rest of the steps are:  
* `main.cpp::main()`:  
  * `main.cpp::parse_args()`: All the arguments passed to the binary are parsed here.  
  * `vampire.cpp::set_values()`: Trace parser and spec values are initialized based on arguments  
  * `vampire.cpp::init_structures[traceType]()`: Calls one of the functions from the function array `init_structures` depending on the value of `traceType`. Each function in `init_structure` corresponds to one of the traceType.  
  * `vampire.cpp::Vampire::estimate()`: Goes through each command in the trace  
    * `vampire.cpp::Vampire::service_request()`: Adds energy of each request read from the trace to the total energy.  
      * `vampire.cpp::` `getSetBits[int(traceType)]`/`getToggleBits[int(traceType)]`: Array of functions similar to `init_structure`, but returns # of set bits/# of toggle bits in a cahce line based on the value of `traceType`.
