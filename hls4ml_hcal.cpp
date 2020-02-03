#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "ap_utils.h"
#include "hls4ml_hcal.h"
#include "ereg_v1.h"

#define N_INPUTS_OVER_2 5




void ingress(
        galapagos_interface * in, 
        hls::stream<result_t > & to_run
        )
{
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis register both port=in
#pragma HLS INTERFACE axis register both port=to_run

    for(int i=0; i<N_INPUTS_OVER_2; i++){
#pragma HLS PIPELINE II=1
        input_t data;
        galapagos_packet gp;
        gp = in->read();
        data(31,0) = gp.data(31,0);
        to_run.write(data);
        data(31,0) = gp.data(63,32);
        to_run.write(data);
        data(31,0) = gp.data(63,32);
        to_run.write(data);
    }

    input_t data;
    galapagos_packet gp;
    gp = in->read();
    data(31,0) = gp.data(31,0);
    to_run.write(data);




}

void run(
        galapagos_interface * in, 
        //hls::stream<input_t> & from_ingress, // Read-Only Vector
        hls::stream<result_t > & to_egress
        )
{
#pragma HLS INTERFACE ap_ctrl_none port=return
//#pragma HLS INTERFACE axis register both port=from_ingress
#pragma HLS INTERFACE axis register both port=in
#pragma HLS INTERFACE axis register both port=to_egress




    input_t in_buf[N_INPUTS];
#pragma HLS ARRAY_PARTITION variable=in_buf complete dim=1
    result_t out_buf[N_OUTPUTS];
#pragma HLS ARRAY_PARTITION variable=out_buf complete dim=1

    galapagos_packet gp;
    gp = in->read();
    in_buf[0] = gp.data(31,0);
    in_buf[1] = gp.data(63,32);
#if PACKET_DATA_LENGTH == 512
    in_buf[2] = gp.data(95,64);
    in_buf[3] = gp.data(127,96);
    in_buf[4] = gp.data(159,128);
    in_buf[5] = gp.data(191,160);
    in_buf[6] = gp.data(223,192);
    in_buf[7] = gp.data(255,224);
    in_buf[8] = gp.data(287,256);
    in_buf[9] = gp.data(287,256);
    in_buf[10] = gp.data(319,288);
#endif

    ereg_v1(in_buf,out_buf);
    for (int i=0; i<N_OUTPUTS; i++)
#pragma HLS PIPELINE II=1
        to_egress.write(out_buf[i]);


}

void egress(
        const int flits_per_packet,
        const ap_uint<1> output_compress,
        short id,
        hls::stream<result_t >  & from_run,
        galapagos_interface * out
        )
{
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis register both port=from_run
#pragma HLS INTERFACE axis register both port=out
#pragma HLS PIPELINE II=1

    static int i=0;

    int size = flits_per_packet;

//#define OUTPUT_COMPRESS

//#ifdef OUTPUT_COMPRESS
//
    if(!from_run.empty()){
        galapagos_packet gp;
        gp.user = size;
        result_t res = from_run.read();
        gp.data(31,0) = res(31,0);
        if(output_compress){
            res = from_run.read();
            gp.data(63, 32) = res(31,0);
            res = from_run.read();
#if PACKET_DATA_LENGTH == 512
            gp.data(95, 64) = res(31,0);
            res = from_run.read();
            gp.data(127, 96) = res(31,0);
            res = from_run.read();
            gp.data(159, 128) = res(31,0);
            res = from_run.read();
            gp.data(191, 160) = res(31,0);
            res = from_run.read();
            gp.data(223, 192) = res(31,0);
            res = from_run.read();
            gp.data(255, 224) = res(31,0);
            res = from_run.read();
            gp.data(287, 256) = res(31,0);
            res = from_run.read();
            gp.data(319, 288) = res(31,0);
            res = from_run.read();
            gp.data(351, 320) = res(31,0);
            res = from_run.read();
            gp.data(383, 352) = res(31,0);
            res = from_run.read();
            gp.data(415, 384) = res(31,0);
            res = from_run.read();
            gp.data(447, 416) = res(31,0);
            res = from_run.read();
            gp.data(479, 448) = res(31,0);
            res = from_run.read();
            gp.data(479, 448) = res(31,0);
            res = from_run.read();
            gp.data(511, 480) = res(31,0);
#endif
        }
        gp.keep = KEEP_ALL;
        gp.dest = id-1;
        gp.id = id;
        if (i == (size) - 1){
            gp.last = 1;
            i=0;
        }
        else{
            gp.last = 0;
            i++;
        }
        out->write(gp);


    }   
}

