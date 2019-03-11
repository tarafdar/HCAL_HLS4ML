#include "hls4ml_hcal.h"
#include "ereg_v1.h"

void hls4ml_hcal(
        data32_t in[STREAMSIZE*N_INPUTS], // Read-Only Vector
        data32_t out[STREAMSIZE*N_OUTPUTS]       // Output Result
        )
{
#pragma HLS INTERFACE axis port=in bundle=control
#pragma HLS INTERFACE axis port=out bundle=control
//#pragma HLS INTERFACE s_axilite port=in   bundle=control
//#pragma HLS INTERFACE s_axilite port=out  bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control


    input_t in_buf[STREAMSIZE][N_INPUTS];
    result_t out_buf[STREAMSIZE][N_OUTPUTS];
    #pragma HLS ARRAY_PARTITION variable=in_buf complete dim=2
    #pragma HLS ARRAY_PARTITION variable=out_buf complete dim=2 

    for (int i = 0; i < STREAMSIZE; i++) {
#pragma HLS dataflow
        for (int j = 0; j < N_INPUTS; j++) {
#pragma HLS PIPELINE II=1
            in_buf[i][j] = (input_t)in[i*N_INPUTS+j];
        }
    }
    
    for (int i = 0; i < STREAMSIZE; i++) {
#pragma HLS PIPELINE II=1
        hls4ml: ereg_v1(in_buf[i],out_buf[i]);
    }

    for (int i = 0; i < STREAMSIZE; i++) {
#pragma HLS dataflow
        for (int j = 0; j < N_OUTPUTS; j++) {
#pragma HLS PIPELINE II=1
            out[i*N_OUTPUTS+j] = (data32_t)out_buf[i][j];
        }
    }

}
