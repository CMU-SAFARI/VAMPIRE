/*

MAKE_SAMPLE_TRACE.CPP

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

/*
 * tools/make_sample_trace.cpp: Generates random traces in different formats for VAMPIRE
 *
 * Usage:
 * 		./traceGen [number-of-requests]
 *
 * TraceGen generates binary traces of the following formats:
 * 1.   RD_WR, RD_WR_t
 * 2.   DIST, DIST_t
 * 3.   WR, WR_t
 *
 * `t' variants of the traces also include timing information of each request
 *
 * Note: 	1.  Current version of VAMPIRE only accepts the traces with the timing information.
 * 			    Traces with un-timed requests are for debugging only.
 *
 *			2.  Trace format is explained in `src/parser.cpp'
 */

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>    
#include <vector>
#include <algorithm>
using namespace std;

#define TIME_INCR_LIMIT 100
#define REQ_RANGE       0b101
#define NUM_OPS         10000
#define NUM_CHANNELS    1
#define NUM_RANKS       1
#define NUM_BANKS       8
#define NUM_ROWS        32768
#define NUM_COLS        128
//#define DEBUG         1

int main(int argc, char *argv[]){
    srand(time(NULL));
  
    FILE *fp_rd_wr, *fp_dist, *fp_wr, *fp_rd_wr_t, *fp_dist_t, *fp_wr_t;
    unsigned long time = 0;
    uint64_t bytes_write;
    unsigned int data[16] = {0};
    unsigned int data_old[16] = {0};
    uint64_t req, channel, rank, bank, row, col, rowHit;
    uint64_t prev_channel, prev_rank, prev_bank, prev_row;
  
    int num_ops = (argc > 1) ? atoi(argv[1]) : 10000;
    std::vector<unsigned int> setBitCount(16*sizeof(unsigned int)*8+1, 0);
    std::vector<unsigned int> toggleBitCount(16*sizeof(unsigned int)*8+1, 0);

    long unsigned int totalOcc = 0;

    prev_channel = -1;
    prev_rank    = -1;
    prev_bank    = -1;
    prev_row     = -1;
  
    fp_rd_wr = fopen("trace_rd_wr.bin", "wb");
    fp_dist = fopen("trace_dist.bin", "wb");
    fp_wr = fopen("trace_wr.bin", "wb");
  
    fp_rd_wr_t = fopen("trace_rd_wr_t.bin", "wb");
    fp_dist_t = fopen("trace_dist_t.bin", "wb");
    fp_wr_t = fopen("trace_wr_t.bin", "wb");
  
    unsigned int *memory = new unsigned int[NUM_CHANNELS*NUM_RANKS*NUM_BANKS*NUM_ROWS*NUM_COLS*16]();

    for (int i = 0; i < num_ops; i++){
	time    = 20 + time + (unsigned long)(rand()%(TIME_INCR_LIMIT-20));
	/*
	  0b000 = RD
	  0b001 = WR
	  0b010 = ACT
	  0b011 = PRE
	  0b100 = RDA
	  0b101 = WRA
	*/
	req     = rand() % REQ_RANGE; 
	channel = rand() % NUM_CHANNELS;
	rank    = rand() % NUM_RANKS;
	bank    = rand() % NUM_BANKS;
	row     = rand() % NUM_ROWS;
	col     = rand() % NUM_COLS;
	rowHit  = 0;

	if (rand() % 2 < 1)
	    row = 63;
    
	if (rand() % 2 < 1)
	    bank = 7;

	int numOf1s = 0;
	int numOfFlips = 0;
    
	unsigned int _0_dat[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	// If request is WRITE and it has to be included or is READ,
	// include its data in dist and mean calculations
	if (req == 0 | req == 1) {
	    for(int iter = 0; iter < 16; iter++){
		data_old[iter] = data[iter];
		if (req == 1) {
		    data[iter] = rand();

		    memory[channel*NUM_RANKS*NUM_BANKS*NUM_ROWS*NUM_COLS*16
			   + rank*NUM_BANKS*NUM_ROWS*NUM_COLS*16
			   + bank*NUM_ROWS*NUM_COLS*16
			   + row*NUM_COLS*16
			   + col*16
			   + iter] = data[iter];
		} else {
		    data[iter] = memory[channel*NUM_RANKS*NUM_BANKS*NUM_ROWS*NUM_COLS*16
					+ rank*NUM_BANKS*NUM_ROWS*NUM_COLS*16
					+ bank*NUM_ROWS*NUM_COLS*16
					+ row*NUM_COLS*16
					+ col*16
					+ iter];
		}
		numOfFlips += __builtin_popcount(data_old[iter] ^ data[iter]);
		numOf1s += __builtin_popcount(data[iter]);
	    }
      
	    setBitCount[numOf1s]++;
	    toggleBitCount[numOfFlips]++;
	    totalOcc++;
	}
    
	bytes_write = (req<<30)| (channel<<28) | (rank<<26) | (bank<<23) | (row<<7) | (col<<0);

	// Write time of each request as 64bit number (unsigned long)
	fwrite(&time, sizeof(uint64_t), 1, fp_wr_t);
	fwrite(&time, sizeof(uint64_t), 1, fp_dist_t);
	fwrite(&time, sizeof(uint64_t), 1, fp_rd_wr_t);

	// Write rest of the request information 
	fwrite(&bytes_write, sizeof(uint64_t), 1, fp_rd_wr);
	fwrite(&bytes_write, sizeof(uint64_t), 1, fp_wr);
	fwrite(&bytes_write, sizeof(uint64_t), 1, fp_dist);
    
	fwrite(&bytes_write, sizeof(uint64_t), 1, fp_rd_wr_t);
	fwrite(&bytes_write, sizeof(uint64_t), 1, fp_wr_t);
	fwrite(&bytes_write, sizeof(uint64_t), 1, fp_dist_t);

	if (req == 1){
	    fwrite(&data[0], sizeof(unsigned int), 16, fp_wr);
	    fwrite(&data[0], sizeof(unsigned int), 16, fp_wr_t);
	}

	if (req == 1 || req == 0) {
	    fwrite(&data[0], sizeof(unsigned int), 16, fp_rd_wr);
	    fwrite(&data[0], sizeof(unsigned int), 16, fp_rd_wr_t);
	}
	
	prev_channel = channel;
	prev_rank    = rank;
	prev_bank    = bank;
	prev_row     = row;

    
#ifdef DEBUG
	std::cout
	  << "time: " << std::setw(4) << time << std::setw(0)
	  << ", req: " << req
	  << ", channel: " << channel
	  << ", rank: " << rank
	  << ", bank: " << bank
	  << ", row: " << std::setw(5) << row << std::setw(0)
	  << ", col: " << std::setw(4) << col << std::setw(0)
	  << ", bytes_write(" << sizeof(bytes_write) << "): " << std::hex << std::setw(8) << bytes_write << std::dec
	  << ", data: 0x";

	std::cout << std::hex;
	for (int i =0 ; i < 16; i++) {
	    std::cout << data[i];
	}
	std::cout << std::dec << std::endl;
#endif 
    }  
    std::vector<float> setBitDist(sizeof(unsigned int)*8*16+1, 0); 
    std::vector<float> toggleBitDist(sizeof(unsigned int)*8*16+1, 0); 

    int setBitWeightedMean = 0;
    int toggleBitWeightedMean = 0;

    auto normalizeVal = std::bind2nd(std::divides<float>(), totalOcc);
    std::transform(setBitCount.begin(), setBitCount.end(), setBitDist.begin(), normalizeVal);
    std::transform(toggleBitCount.begin(), toggleBitCount.end(), toggleBitDist.begin(), normalizeVal);

    // Calculated weighted mean
    for (int count = 0; count < setBitDist.size(); count++) {
	setBitWeightedMean += setBitDist[count] * count;
	toggleBitWeightedMean += toggleBitDist[count] * count;
    }

    std::cout << "# Data for the config file: " << std::endl;
    std::cout << "avgNumSetBits = " << setBitWeightedMean << std::endl << std::endl;
    std::cout << "avgNumToggleBits = " << toggleBitWeightedMean << std::endl << std::endl;
  
    std::cout << "setBitsDist = ";
    for_each(setBitDist.begin(), setBitDist.end(), [&] (float &a) {std::cout << a << " ";});

    std::cout << std::endl << std::endl;

    std::cout << "toggleDist = ";
    for_each(toggleBitDist.begin(), toggleBitDist.end(), [&] (float &a) {std::cout << a << " ";});
  
    std::cout << std::endl;
    fclose(fp_wr);
    fclose(fp_rd_wr);  
    fclose(fp_dist);
  
    fclose(fp_wr_t);
    fclose(fp_rd_wr_t);  
    fclose(fp_dist_t);
    return 1;
}
