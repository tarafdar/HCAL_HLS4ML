include $(GALAPAGOS_PATH)/include.mk
middleware_path = $(GALAPAGOS_PATH)/middleware
hls_path = $(middleware_path)/hls
python_path = $(middleware_path)/python

SRC_LIB=$(GALAPAGOS_PATH)/middleware/libGalapagos
CXXFLAGS = -DCPU -O2 -std=c++17 -pthread -isystem $(XILINX_VIVADO)/include -I$(GALAPAGOS_PATH)/middleware/include -I$(SRC_LIB) -I$(GALAPAGOS_PATH)/util/spdlog/include -I../nnet_utils

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

hls:
	vivado_hls generate_hls.tcl

middleware:
	python3.5 ${python_path}/globalFPGAParser.py --logicalFile=${LOGICALFILE} \
		--mapFile=${MAPFILE} --projectName=${PROJECTNAME}
	chmod +x $(GALAPAGOS_PATH)/projects/$(PROJECTNAME)/createCluster.sh

hlsmiddleware:
	 $(MAKE) -C $(hls_path)


cpu_send.exe: ereg_v1.o hls4ml_hcal.o cpu_send.cpp
	$(CXX) $(CXXFLAGS) -I$(SRC_LIB) -I$(GALAPAGOS_PATH)/util/argparse -DCPU -DLOG_LEVEL=0 $^ -o $@ $(BOOST_LDFLAGS)  

cpu_compute.exe: ereg_v1.o hls4ml_hcal.o cpu_compute.cpp
	$(CXX) $(CXXFLAGS) -I$(SRC_LIB) -I$(GALAPAGOS_PATH)/util/argparse -DCPU -DLOG_LEVEL=0 $^ -o $@ $(BOOST_LDFLAGS)  

cpu_node.exe: ereg_v1.o hls4ml_hcal.o cpu_node.cpp
	$(CXX) $(CXXFLAGS) -I$(SRC_LIB) -I$(GALAPAGOS_PATH)/util/argparse -DCPU -DLOG_LEVEL=0 $^ -o $@ $(BOOST_LDFLAGS)

clean:
	rm -rf $(GALAPAGOS_PATH)/hlsBuild/$(GALAPAGOS_BOARD_NAME)/ip/hls4ml_hcal
	rm -rf *.exe  *.txt *.o
