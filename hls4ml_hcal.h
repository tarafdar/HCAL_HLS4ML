#ifndef HLS4ML_HCAL_H_
#define HLS4ML_HCAL_H_

#include "parameters.h"

void hls4ml_hcal(
        data32_t in[STREAMSIZE*N_INPUTS], // Read-Only Vector
        data32_t out[STREAMSIZE*N_OUTPUTS]       // Output Result
        );

#endif
