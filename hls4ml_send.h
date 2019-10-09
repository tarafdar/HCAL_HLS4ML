#ifndef HLS4ML_SEND_H_
#define HLS4ML_SEND_H_

#include "parameters.h"


#ifdef CPU
#include "galapagos_interface.hpp"
#else
#include "galapagos_packet.h"
#endif
//#include "defines.h"
//#include "packet.h"

void kern_send(short id, galapagos_interface * in, galapagos_interface * out);


#endif
