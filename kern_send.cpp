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
typedef ap_uint<PACKET_DATA_LENGTH> T;
#endif

void kern_send(short id, 
        galapagos_interface * in, 
        galapagos_interface  * out
        )
{
//#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis register both port=out
#pragma HLS INTERFACE axis register both port=in
    


#if PACKET_DATA_LENGTH == 64
    
    //6.3 ms latency
    //input at 123 mb/s
    int N=132;
    int S=STREAMSIZE*8;

    //6.2 ms latency
    //input at 126 mb/s
    //int N=176;
    //int S=STREAMSIZE*6;

    //10.8 ms latency
    //input at 74 mb/s
    //int N=264;
    //int S=STREAMSIZE*4;

    //15.4 ms latency
    //input at 50.5 mb/s
    //int N=352;
    //int S=STREAMSIZE*3;

    //17.8 ms latency
    //input at 43.74 mb/s
    //int N=528;
    //int S=STREAMSIZE*2;
    
    //33.4 ms latency 
    //input at 22.7 mb/s
    //int N=1056;
    //int S=STREAMSIZE;


#else // PACKET_DATA_LENGTH==64

    //int N=1056;
    //int N=2112;
    
    
    int N=512;
    int S=STREAMSIZE*4;
    //int NUM_STREAMS=32;
#endif
    


    int k=0;
    for (int i=0; i<N; i++){
#ifndef CPU
        galapagos_packet gp;
#else
        T data_array[S];
#endif
        for(int j=0; j<S; j++){

#ifndef CPU
            gp.data(31,0) = k++; //packs data to lower half of flit
            gp.data(63,32) = k++;
            gp.data(95,64) = k++;
#if PACKET_DATA_LENGTH == 512
            gp.data(127,96) = k++;
            gp.data(159,128) = k++;
            gp.data(191,160) = k++;
            gp.data(223,192) = k++;
            gp.data(255,224) = k++;
            gp.data(287,256) = k++;
            gp.data(287,256) = k++;
            gp.data(319,288) = k++;
            gp.data(351,320) = 0;
            gp.data(383,352) = 0;
            gp.data(415,384) = 0;
            gp.data(415,384) = 0;
            gp.data(447,416) = 0;
            gp.data(479,448) = 0;
#endif //PACKET_DATA_LENGTH == 512
            gp.dest = id+1;
            gp.id = id;
            if(j==(S -1))
                gp.last = 1;
            out->write(gp);
#endif
#ifdef CPU //ndef CPU
            data_array[j](31,0) = k++; //packs data to lower half of flit
            data_array[j](63,32) = k++;
            data_array[j](95,64) = k++;
#if PACKET_DATA_LENGTH == 512
            data_array[j](127,96) = k++;
            data_array[j](159,128) = k++;
            data_array[j](191,160) = k++;
            data_array[j](223,192) = k++;
            data_array[j](255,224) = k++;
            data_array[j](287,256) = k++;
            data_array[j](287,256) = k++;
            data_array[j](319,288) = k++;
            data_array[j](351,320) = 0;
            data_array[j](383,352) = 0;
            data_array[j](415,384) = 0;
            data_array[j](415,384) = 0;
            data_array[j](447,416) = 0;
            data_array[j](479,448) = 0;
            data_array[j](511,480) = 0;
#endif //PACKET_DATA_LENGTH ==512
#endif // else // CPU

        }
#ifdef CPU
	    out->packet_write((char *)data_array, S, id+1, id);
#endif
    
    }

}
