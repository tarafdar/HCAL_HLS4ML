
#include <string>
#include <math.h>
#include <thread>
#include <chrono>
#include <string>
#include "galapagos_net_tcp.hpp"
#include "galapagos_node.hpp"
#include "hls4ml_send.h"


#define NUM_KERNELS 3

std::shared_ptr<spdlog::logger> my_logger;
typedef ap_uint<64> T;
#define GALAPAGOS_PORT 7


int main(int argc, const char** argv){

    my_logger = spdlog::basic_logger_mt("basic_logger", "send_log.txt"); 
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
    std::string source_ip_str("10.1.2.155");
    std::string dest_ip_str("10.1.2.156");
    kern_info.push_back(source_ip_str);
    kern_info.push_back(dest_ip_str);
    
    galapagos::net::tcp <T> my_tcp(
                    GALAPAGOS_PORT, 
                    kern_info, 
                    source_ip_str, 
                    my_logger
                    );
    std::vector < galapagos::external_driver<T> * > ext_drivers;
    ext_drivers.push_back(&my_tcp);



    galapagos::node <T> node(kern_info, source_ip_str, ext_drivers, my_logger);
    node.add_kernel(0, kern_send);

    node.start();
    node.end();

}
