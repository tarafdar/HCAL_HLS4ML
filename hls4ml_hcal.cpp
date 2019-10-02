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


//#define SINGLE_PACKET
//#define SINGLE_INPUT

#include "hls4ml_hcal.h"
#include "ereg_v1.h"

#define MAX_FLITS_TRANS 170


#ifdef CPU
extern std::shared_ptr<spdlog::logger> my_logger;
typedef ap_uint<64> T;
#endif

void kern_send(short id, galapagos_interface * in, galapagos_interface  * out)
{
#pragma HLS INTERFACE ap_ctrl_none port=return
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

    #include "inputs.h"
#ifdef CPU
    auto start = std::chrono::high_resolution_clock::now();
    T data_array[(NUM_PACKETS * STREAMSIZE * (k_limit/2))];
    for (int i = 0; i < NUM_PACKETS; i++) {
        for(int j=0; j<STREAMSIZE; j++){
            int array_index = i *STREAMSIZE*(k_limit/2) + j*(k_limit/2);
            for(int k=0; k<k_limit; k+=2){
                int input_index = i *STREAMSIZE*N_INPUTS + j*N_INPUTS + k;
                my_logger->debug("HLS4ML Kern Send about to write input_index:{0:d} and array_index{1:d}, i:{2:d}, j:{3:d}, k:{4:d}", input_index, array_index, i, j, k);
                my_logger->flush();
                (data_array[array_index])(31,0) = input_vals[input_index];
                if(k < (k_limit - 2 )){
                    (data_array[array_index])(63,32) = input_vals[input_index+1];
                }
                else{
                    (data_array[array_index])(63,32) = 0;
                }
                array_index++;
            }
        }
	    out->packet_write((char *)data_array + i*STREAMSIZE*(k_limit/2)*sizeof(T), STREAMSIZE*(k_limit/2)*sizeof(T), id+1, id);
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
    for (int i = 0; i < NUM_PACKETS; i++) {
        for(int j=0; j<STREAMSIZE; j++){
            for(int k=0; k<k_limit; k+=2){
                (gp.data)(31,0) = input_vals[i *STREAMSIZE*N_INPUTS + j*N_INPUTS + k];
                gp.id = id;
                gp.dest = id+1;
                if(k < (k_limit - 2 )){
                    (gp.data)(63,32) = input_vals[i *STREAMSIZE*N_INPUTS + j*N_INPUTS + k + 1];
                }
                else{
                    (gp.data)(63,32) = 0;
                }
                gp.last = ((j == (STREAMSIZE-1)) && ((k+2) >= (N_INPUTS)));
                out->write(gp);
            }
        }
    }
    for(int i=0; i<NUM_PACKETS*STREAMSIZE*N_OUTPUTS; i++){
        gp = in->read();
    }
#endif
}



void hls4ml_hcal(
        short id,
        galapagos_interface *in, // Read-Only         
        galapagos_interface *out       // Output Result
        //result_t * out_test,
        //input_t * in_test
        )
{
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis register both port=out
#pragma HLS INTERFACE axis register both port=in
    
    galapagos_packet gp;

    ap_uint <1> last = 0;

    input_t in_buf[NUM_PACKETS][STREAMSIZE][N_INPUTS];
    result_t out_buf[NUM_PACKETS][STREAMSIZE][N_OUTPUTS];
    #pragma HLS ARRAY_PARTITION variable=in_buf complete dim=2
    #pragma HLS ARRAY_PARTITION variable=out_buf complete dim=2 

    input_t * in_ptr = (input_t *)in_buf;

#ifdef CPU
    my_logger->debug("Before receive in_buf, NUM_PACKETS:{0:d}, STREAMSIZE{1:d}, N_INPUTS{2:d}", NUM_PACKETS, STREAMSIZE, N_INPUTS);
#endif
    
#ifndef CPU
    while(1){
#endif

    int k_limit;
    if(N_INPUTS %2 == 0)
        k_limit = N_INPUTS;
    else
        k_limit = N_INPUTS + 1;


    for (int i = 0; i < NUM_PACKETS; i++) {
        for(int j=0; j<STREAMSIZE; j++){
            for(int k=0; k<k_limit; k+=2){
                gp = in->read();
#ifdef CPU
                my_logger->debug("Read in {0:x}, from {1:x}", gp.data, gp.dest);
#endif 
                in_buf[i][j][k] = (gp.data)(31,0);
                if(k < (k_limit - 2 )){
                    in_buf[i][j][k+1] = (gp.data)(63,32);
                }
            }
        }
    }
#ifdef CPU 
    my_logger->debug("About to print in buf");
    for(int i =0; i<NUM_PACKETS; i++){
        for(int j=0; j<STREAMSIZE; j++){
            for(int k=0; k<N_INPUTS; k++){
                my_logger->debug("in_buf of [{0:d}][{1:d}][{2:d}] is {3:x}", i, j, k, (int)in_buf[i][j][k]);
            }
        }
    }
#endif



    short dest = gp.id;
   
    for (int i = 0; i < NUM_PACKETS; i++) {
        for(int j=0; j<STREAMSIZE; j++){
            #pragma HLS PIPELINE
            hls4ml: ereg_v1(in_buf[i][j],out_buf[i][j]);
        }
    }

    result_t * out_ptr = (result_t *)out_buf;
    int curr_index=0;
    //writing: for(int i =0; i<NUM_PACKETS * STREAMSIZE * N_OUTPUTS; i+=2){
#ifdef CPU 
    my_logger->debug("Before writing out, NUM_PACKETS:{0:d}, STREAMSIZE{1:d}, N_OUTPUTS{2:d}", NUM_PACKETS, STREAMSIZE, N_OUTPUTS);
#endif
    for (int i = 0; i < NUM_PACKETS; i++) {
        for(int j=0; j<STREAMSIZE; j++){
            for(int k=0; k<N_OUTPUTS; k+=2){
                gp.data = 0;
                (gp.data)(31,0) = ((data32_t)(out_buf[i][j][k]))(31,0);
                if(k < (N_OUTPUTS - 2 )){
                    (gp.data)(63,32) = ((data32_t)(out_buf[i][j][k+1]))(31,0);
                }
                gp.last = ((j == (STREAMSIZE-1)) && ((k+2) >= (N_OUTPUTS)));
                #ifdef CPU 
                    my_logger->debug("Last {0:d}, j:{1:d}, k:{2:d}", gp.last, j, k);
                #endif
                
                gp.dest = id -1;
                gp.id = id;
                out->write(gp);
            }
        }
    }
#ifdef CPU 
    my_logger->debug("After writing out");
#endif

#ifndef CPU 
    }
#endif

}
