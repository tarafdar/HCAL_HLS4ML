#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef CPU
#include <chrono>
#endif

#include "parameters.h"
#include "hls4ml_send.h"

#ifdef CPU
extern std::shared_ptr<spdlog::logger> my_logger;
typedef ap_uint<64> T;
#endif

void kern_send(short id, 
        galapagos_interface * in, 
        galapagos_interface  * out
        )
{
//#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis register both port=out
#pragma HLS INTERFACE axis register both port=in
    
    int num_flits = STREAMSIZE*N_INPUTS;
    int k_limit;
    if(N_INPUTS %2 == 0)
        k_limit = N_INPUTS;
    else
        k_limit = N_INPUTS + 1;
    
    galapagos_packet gp;
    gp.id = id;

#if 0 
    #include "inputs.h"
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<T> data_array(NUM_PACKETS * STREAMSIZE * (k_limit/2));
//    T data_array[(NUM_PACKETS * STREAMSIZE * (k_limit/2))];
    int local_sent_num = 0;
    for (int i = 0; i < NUM_PACKETS; i++) {
        for(int j=0; j<STREAMSIZE; j++){
            int array_index = i *STREAMSIZE*N_INPUTS + j*N_INPUTS;
            for(int k=0; k<N_INPUTS; k++){
                //(data_array[array_index])(31,0) = input_vals[input_index];
                (data_array[array_index])(31,0) = local_sent_num;
                array_index++;
                local_sent_num++;
            }
        }
	    out->packet_write((char *)data_array.data() + i*STREAMSIZE*(k_limit/2)*sizeof(T), STREAMSIZE*(k_limit/2)*sizeof(T), id+1, id);
    }
    auto send = std::chrono::high_resolution_clock::now();
	size_t _size, size_read;
	short _dest;
	short _id;

    size_read = 0;
    std::vector <T *> out_vec;
    std::vector <size_t> out_size;
    do{
        out_vec.push_back((ap_uint<64> *)in->packet_read(&_size, &_dest, &_id));
        out_size.push_back(_size);
        size_read += _size;
    }while(size_read<STREAMSIZE*N_OUTPUTS*NUM_PACKETS*sizeof(T));
    
    auto recv = std::chrono::high_resolution_clock::now();
    
    int num = 0;
    for(int i=0; i<out_vec.size(); i++){
        for(int j=0; j<out_size[i]/sizeof(T); j++){
            my_logger->info("out[{0:d}]:{1:x}", num, out_vec[i][j]);
            num++;
        }
        free(out_vec[i]);
    }
    std::cout << std::endl << std::endl;
    std::cout << "Send/prep time:  " << ((std::chrono::duration<double>)(send - start)).count() << " s" << std::endl;
    std::cout << "   HLS4ML time:  " << ((std::chrono::duration<double>)(recv - send)).count() << " s" << std::endl;
#else
    #include "inputs_sim.h"
    int local_sent_num = 0;
   
    for (int i = 0; i < NUM_PACKETS; i++) {
        for(int j=0; j<STREAMSIZE; j++){
            for(int k=0; k<N_INPUTS; k++){
#pragma HLS PIPELINE II=1
                //gp.user = STREAMSIZE*N_INPUTS; //writes size (optimizes to reduce buffering, if not here then bridge buffers to calculate size)
                gp.user = N_INPUTS*STREAMSIZE; //writes size (optimizes to reduce buffering, if not here then bridge buffers to calculate size)
                //(gp.data)(31,0) = input_vals[j*N_INPUTS + k]; //packs data to lower half of flit
                (gp.data)(31,0) = j*N_INPUTS + k; //packs data to lower half of flit
                (gp.data)(63,32) = 0;
                gp.id = id;  
                gp.dest = id+1;
                gp.keep = 0xff;
                gp.last = ((j==(STREAMSIZE - 1) && k == (N_INPUTS - 1)));
//                gp.last = ( k == (N_INPUTS - 1));
                out->write(gp);
                local_sent_num++;
            }
        }
    }
    for(int i=0; i<NUM_PACKETS*STREAMSIZE*N_OUTPUTS; i++){
        gp = in->read();
    }
#endif
}



