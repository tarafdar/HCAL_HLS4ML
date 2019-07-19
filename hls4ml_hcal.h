#ifndef HLS4ML_HCAL_H_
#define HLS4ML_HCAL_H_

#include "parameters.h"
#include "defines.h"
#include "packet.h"

void kern_send(galapagos_stream * in, galapagos_stream * out);
void kern_recv(galapagos_stream * in, galapagos_stream * out);
void hls4ml_hcal(galapagos_stream * in, galapagos_stream * out);


#endif
