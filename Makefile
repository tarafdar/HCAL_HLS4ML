include $(GALAPAGOS_PATH)/include.mk
middleware_path = $(GALAPAGOS_PATH)/middleware
hls_path = $(middleware_path)/hls
python_path = $(middleware_path)/python

SRC_LIB=$(GALAPAGOS_PATH)/middleware/CPP_lib/Galapagos_lib
CXXFLAGS = -DCPU -O2 -std=c++17 -pthread -isystem $(XILINX_VIVADO)/include -I$(GALAPAGOS_PATH)/middleware/include -I$(GALAPAGOS_PATH)/middleware/libGalapagos -I$(GALAPAGOS_PATH)/util/spdlog/include -I$(SRC_LIB) -I../nnet_utils

LDFLAGS = -lpthread

BOOST_LDFLAGS=-lboost_thread -lboost_system $(LDFLAGS) 

MIDDLEWARE_DIR= $(shell pwd)/middlewareInput
CONFIG_DIR = conf_oneFPGA
LOGICALFILE= $(MIDDLEWARE_DIR)/$(CONFIG_DIR)/logical.xml
MAPFILE= $(MIDDLEWARE_DIR)/$(CONFIG_DIR)/map.xml
PROJECTNAME = test

all: hls4ml_hcal.o

hls4ml_hcal.o: hls4ml_hcal.cpp
	$(CXX) $(CXXFLAGS) -c hls4ml_hcal.cpp -o $@ $(BOOST_LDFLAGS)

ereg_v1.o: ereg_v1.cpp
	$(CXX) $(CXXFLAGS) -c ereg_v1.cpp -o $@ $(BOOST_LDFLAGS)


hls_ip:
	vivado_hls generate_ip.tcl

hls_send:
	vivado_hls generate_send.tcl

hls: hls_ip hls_send

middleware:
	python3.5 ${python_path}/globalFPGAParser.py --logicalFile=${LOGICALFILE} \
		--mapFile=${MAPFILE} --projectName=${PROJECTNAME}
	chmod +x $(GALAPAGOS_PATH)/projects/$(PROJECTNAME)/createCluster.sh

hlsmiddleware:
	 $(MAKE) -C $(hls_path)


cpu_send.exe: kern_send.cpp cpu_send.cpp
	$(CXX) $(CXXFLAGS) -I$(SRC_LIB) -I$(GALAPAGOS_PATH)/util/argparse -DCPU -DLOG_LEVEL=0 $^ -o $@ $(BOOST_LDFLAGS)  

mb_cpu_send.exe: mb.cpp mb_cpu_send.cpp
	$(CXX) $(CXXFLAGS) -I$(SRC_LIB) -I$(GALAPAGOS_PATH)/util/argparse -DCPU -DLOG_LEVEL=0 $^ -o $@ $(BOOST_LDFLAGS)  

mb_fpga_send.exe: mb.cpp mb_fpga_send.cpp
	$(CXX) $(CXXFLAGS) -I$(SRC_LIB) -I$(GALAPAGOS_PATH)/util/argparse -DCPU -DLOG_LEVEL=2 $^ -o $@ $(BOOST_LDFLAGS)  

cpu_node.exe: ereg_v1.o hls4ml_hcal.o cpu_node.cpp 
	$(CXX) $(CXXFLAGS) -I$(SRC_LIB) -I$(GALAPAGOS_PATH)/util/argparse $^ -o $@ $(BOOST_LDFLAGS)  

clean:
	rm -rf *.o myproject_prj
	rm -rf *.exe
	rm -rf *.txt
