#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include <complex>
#include "ap_int.h"
#include "ap_fixed.h"
#include "nnet_utils/nnet_layer.h"
#include "nnet_utils/nnet_conv.h"
#include "nnet_utils/nnet_activation.h"
#include "nnet_utils/nnet_common.h"

#ifdef SINGLE_PACKET
#define NUM_PACKETS 1
#define STREAMSIZE  16
#else //SINGLE_PACKET
#ifdef SINGLE_INPUT
#define NUM_PACKETS 1
#define STREAMSIZE 1
#else //SINGLE_INPUT
#ifdef SIM
#define NUM_PACKETS 1024 
#define STREAMSIZE 16
#else // SIM
#ifdef DOUBLE_INPUT 
#define NUM_PACKETS 1 
#define STREAMSIZE 2
#else //DOUBLE_INPUT
#ifdef INPUT_4
#define NUM_PACKETS 1 
#define STREAMSIZE 4
#else // INPUT_4
#ifdef INPUT_8
#define NUM_PACKETS 1 
#define STREAMSIZE 8
#else //INPUT_8
#ifdef INPUT_16
#define NUM_PACKETS 1 
#define STREAMSIZE 16 
#else // INPUT_16
#ifdef INPUT_30
#define NUM_PACKETS 1 
#define STREAMSIZE  30 
#endif
#endif //INPUT_16
#endif //INPUT_8
#endif // INPUT_4 
#endif //DOUBLE_INPUT
#endif //SIM
#endif //SINGLE_INPUT
#endif //SINGLE_PACKET else


#ifndef SINGLE_PACKET 
#ifndef SINGLE_INPUT 
#ifndef SIM
#ifndef DOUBLE_INPUT 
#ifndef INPUT_4 
#ifndef INPUT_8 
#ifndef INPUT_16 
#ifndef INPUT_30 
#define NUM_PACKETS 1024
#define STREAMSIZE 16 
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif


//hls-fpga-machine-learning insert numbers
typedef ap_fixed<32,14> accum_default_t;
typedef ap_fixed<32,14> weight_default_t;
typedef ap_fixed<32,14> bias_default_t;
typedef ap_fixed<32,14> input_t;
typedef ap_fixed<32,14> result_t;
#define N_INPUTS 11
#define N_LAYER_1 15
#define N_LAYER_2 5
#define N_OUTPUTS 1
#define N_INPUTS_PADDED 12
#define N_OUTPUTS_PADDED 2

typedef ap_fixed<32,14> data32_t;

//hls-fpga-machine-learning insert layer-precision
typedef ap_fixed<32,14> layer1_t;
typedef ap_fixed<32,14> layer2_t;

//hls-fpga-machine-learning insert layer-config
struct config1 : nnet::layer_config {
        static const unsigned n_in = N_INPUTS;
        static const unsigned n_out = N_LAYER_1;
        static const unsigned io_type = nnet::io_parallel;
        static const unsigned reuse_factor = 1;
        static const unsigned n_zeros = 0;
        static const bool store_weights_in_bram = false;
        typedef accum_default_t accum_t;
        typedef bias_default_t bias_t;
        typedef weight_default_t weight_t;
        };
struct linear_config1 : nnet::activ_config {
        static const unsigned n_in = N_LAYER_1;
        static const unsigned table_size = 1024;
        static const unsigned io_type = nnet::io_parallel;
        };
struct config2 : nnet::layer_config {
        static const unsigned n_in = N_LAYER_1;
        static const unsigned n_out = N_LAYER_2;
        static const unsigned io_type = nnet::io_parallel;
        static const unsigned reuse_factor = 1;
        static const unsigned n_zeros = 0;
        static const bool store_weights_in_bram = false;
        typedef accum_default_t accum_t;
        typedef bias_default_t bias_t;
        typedef weight_default_t weight_t;
        };
struct linear_config2 : nnet::activ_config {
        static const unsigned n_in = N_LAYER_2;
        static const unsigned table_size = 1024;
        static const unsigned io_type = nnet::io_parallel;
        };
struct config3 : nnet::layer_config {
        static const unsigned n_in = N_LAYER_2;
        static const unsigned n_out = N_OUTPUTS;
        static const unsigned io_type = nnet::io_parallel;
        static const unsigned reuse_factor = 1;
        static const unsigned n_zeros = 0;
        static const bool store_weights_in_bram = false;
        typedef accum_default_t accum_t;
        typedef bias_default_t bias_t;
        typedef weight_default_t weight_t;
        };
struct linear_config3 : nnet::activ_config {
        static const unsigned n_in = N_OUTPUTS;
        static const unsigned table_size = 1024;
        static const unsigned io_type = nnet::io_parallel;
        };

#endif 
