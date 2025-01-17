
#include <string>
#include <math.h>
#include <thread>
#include <chrono>
#include <string>
#include "galapagos_node.hpp"
#include "hls4ml_hcal.h"


std::shared_ptr<spdlog::logger> my_logger;
typedef ap_uint<64> T;


int main(int argc, const char** argv){

    my_logger = spdlog::basic_logger_mt("basic_logger", "node_log.txt"); 
#if LOG_LEVEL==0
    spdlog::set_level(spdlog::level::off); // Set global log level to off
#elif LOG_LEVEL==1
    spdlog::set_level(spdlog::level::info); // Set global log level to info
#elif LOG_LEVEL==2
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
#endif
    spdlog::flush_every(std::chrono::seconds(2));
    my_logger->info("Starting Send");
    std::vector <std::string> kern_info;
    kern_info.push_back(std::string("10.0.0.1"));
    kern_info.push_back(std::string("10.0.0.1"));
    

    galapagos::node<ap_uint <64> > node(kern_info, std::string("10.0.0.1"), std::vector<galapagos::external_driver <ap_uint<64> > * >(), my_logger);
    node.add_kernel(0, kern_send);
    node.add_kernel(1, hls4ml_hcal);

    node.start();
    node.end();

}
