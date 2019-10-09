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


void ingress(
        galapagos_interface * in, 
        hls::stream<result_t > & to_run
        )
{
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis register both port=in
#pragma HLS INTERFACE axis register both port=to_run
#pragma HLS PIPELINE II=1

    if(!in->empty()){
        input_t data;
        galapagos_packet gp;
        gp = in->read();
        data(31,0) = gp.data(31,0);
        to_run.write(data);
    }

}

void run(
        hls::stream<input_t> & from_ingress, // Read-Only Vector
        hls::stream<result_t > & to_egress
        )
{
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis register both port=from_ingress
#pragma HLS INTERFACE axis register both port=to_egress

    input_t in_buf[N_INPUTS];
#pragma HLS ARRAY_PARTITION variable=in_buf complete dim=1
    result_t out_buf[N_OUTPUTS];
#pragma HLS ARRAY_PARTITION variable=out_buf complete dim=1

    galapagos_packet gp;
    for (int i = 0; i < N_INPUTS; i++) {
#pragma HLS PIPELINE II=1
        in_buf[i] = from_ingress.read();
    }

    ereg_v1(in_buf,out_buf);
    for (int i=0; i<N_OUTPUTS; i++)
#pragma HLS PIPELINE II=1
        to_egress.write(out_buf[i]);


}

void egress(
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

    if(!from_run.empty()){
        galapagos_packet gp;
        gp.user = N_OUTPUTS * STREAMSIZE;
        result_t res = from_run.read();
        gp.data(31,0) = res(31,0);
        gp.data(63, 32) = 0;
        gp.keep = 0xff;
        gp.dest = id-1;
        gp.id = id;
        if (i == N_OUTPUTS * STREAMSIZE - 1){
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

//FINAL RECV RUNNING IN SOFTWARE
void kern_recv(short id, galapagos_stream * in, galapagos_stream  * out)
{
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis register both port=out
#pragma HLS INTERFACE axis register both port=in
    
    galapagos_stream_packet gp;
    short dest;
    
    dest = gp.dest + 1;
    gp.id = id;
    ap_uint <1> last = 0;
    
    int num_flits = STREAMSIZE*N_OUTPUTS;
    for(int j=0; j<num_flits; j++){
    //while(!last){
        gp = in->read();
        std::cout << "DATA IS " << gp.data << std::endl << std::flush;
        last = gp.last;
        out->write(gp);

    }
}


void hls4ml_hcal(
        short id,
        galapagos_interface *in, // Read-Only Vector
        galapagos_interface *out       // Output Result
        )
{
#pragma HLS DATAFLOW
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis register both port=out
#pragma HLS INTERFACE axis register both port=in

    galapagos_interface to_ingress;

    hls::stream<result_t> res;
    galapagos_packet gp;

    //    serialize(
    //            in,
    //            to_ingress
    //            );

    ingress(
            in, // Read-Only Vector
            res
           );

    egress(
            id,
            res, // Read-Only Vector
            out
          );


}
