/*

MAPPING.CPP

VAMPIRE: Variation-Aware model of Memory Power Informed by Real Experiments
https://github.com/CMU-SAFARI/VAMPIRE

Copyright (c) SAFARI Research Group at Carnegie Mellon University and ETH Zürich
Released under the MIT License

*/

#include "vampire.h"
#include "helper.h"


/* Check if chosen encoding is possible on given data */
int Vampire::check_for_encoding(unsigned int data[16]){

    if (encodingType == EncodingType::BDI){

        unsigned int base = data[0];
        unsigned int delta;
        unsigned int delta_extend;

        int encoding = 1; /* flag to check whether compression is possible */

        /* The 0th element is the base */
        for (int fourbytes = 1; fourbytes < 16; fourbytes++){
            delta = data[fourbytes] - base;
            delta_extend = (delta >> 8) & 0xFFFFFF;

            if ((delta_extend != 0x0) && (delta_extend != 0xFFFFFF)){
                encoding = 0;
            }
        }
        return encoding;
    }

    else if (encodingType == EncodingType::CUSTOM) {
        return 1;
    }
    else if (encodingType == EncodingType::CUSTOM_ADV){
        return 1;
    }

    return 0;

}


void Vampire::make_encoding_table(void){
  
    FILE * fp;
    unsigned int count = 0;
    unsigned int byte, index1, index2;
    unsigned int bytes_read;
    unsigned int key1, value1, key2, value2;
  
    fp = fopen("encoding.bin", "rb");
  
        while (fread((&bytes_read), sizeof(unsigned int), 1, fp) != 0){
        byte = count/256;
    
        key1   = bytes_read & 0xFF;
        value1 = (bytes_read >> 8)  & 0xFF;
        key2   = (bytes_read >> 16) & 0xFF;
        value2 = (bytes_read >> 24) & 0xFF;

        index1 = (byte << 8) | key1;
        index2 = (byte << 8) | key2;

        encoding_table[index1] = value1;
        encoding_table[index2] = value2;
    
        //cout << hex << key1 << ":" << value1 << ":"<< dec << index1 << endl;
        //cout << hex << key2 << ":" << value2 << ":"<< dec << index2 << endl;
        count += 2;
    
    }
}

unsigned int * Vampire::apply_encoding(unsigned int data[16]){
 
  
    if (encodingType == EncodingType::BDI){

        /*

     The base is set to the first 4-byte value (V0).
     The remaining values (V1..V15) are used to calcuated difference values from the base.                                                                                                                
     Resulting difference values (delta1...delta15) are then checked to see whether                                                                                                                       
     their first 3 bytes are all zeros or ones (1 byte sign extension check).                                                                                                                            
     If so the cache line can be compressed.                                                                                                                                                             

     Compressed line will have the following format:                                                                                                                                                       

     1. The first 4 bytes will be the base. 
     2. All other 4 byte values will be 0x80_00_00_delta or 0x00_00_00_delta.                                                                                                                              

     0x80_00_00_delta implies value - base = 0xFF_FF_FF_delta 
     0x00_00_00_delta implies value - base = 0x00_00_00_delta                    
  
     Here the 31st bit is used to indicate whether the delta value had leading zeros or ones
    */
               
    
        unsigned int base = data[0];
        unsigned int delta;
        unsigned int delta_extend;

        /* The 0th element is the base */
        for (int fourbytes = 1; fourbytes < 16; fourbytes++){
            delta = data[fourbytes] - base;
      
            delta_extend = (delta >> 8) & 0xFFFFFF;
            delta        = delta & 0xFF;

            if (delta_extend == 0xFFFFFF){
                data[fourbytes] = delta | 0x80000000;
            }
            else if (delta_extend == 0x0){
                data[fourbytes] = delta;
            }
        }

        return &(data[0]);
    
    }


    else if ((encodingType == EncodingType::CUSTOM) || (encodingType == EncodingType::CUSTOM_ADV)){
        unsigned int byte0, byte1, byte2, byte3, encoded_data;
        unsigned int map_byte0, map_byte1, map_byte2, map_byte3;
    
        for (int fourbytes = 0; fourbytes < 16; fourbytes++){

            byte0 = (data[fourbytes])       & 0xFF;
            byte1 = (data[fourbytes] >> 8)  & 0xFF;
            byte2 = (data[fourbytes] >> 16) & 0xFF;
            byte3 = (data[fourbytes] >> 24) & 0xFF;

            map_byte0 = encoding_table[(0 << 8) | byte0];
            map_byte1 = encoding_table[(1 << 8) | byte1];
            map_byte2 = encoding_table[(2 << 8) | byte2];
            map_byte3 = encoding_table[(3 << 8) | byte3];

            encoded_data = (map_byte3 << 24) | (map_byte2 << 16) | (map_byte1 << 8) | (map_byte0);

            data[fourbytes] = encoded_data;
        }

        return &(data[0]);

    }

  
    else{ /* DO NOTHING */
        return &(data[0]);
    }

}