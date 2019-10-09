set galapagos_path $::env(GALAPAGOS_PATH)
set board_name $::env(GALAPAGOS_BOARD_NAME)
set part_name $::env(GALAPAGOS_PART)
set src_path_root [pwd] 

cd $galapagos_path/hlsBuild/${board_name}/ip

open_project ingress 
set_top ingress
open_solution "solution1"
config_rtl -prefix "hls4ml_" 
set_part ${part_name}
add_files $src_path_root/ereg_v1.cpp
add_files $src_path_root/hls4ml_hcal.cpp -cflags "-I $galapagos_path/middleware/include -I $src_path_root -I $src_path_root/weights -I $src_path_root/nnet_utils -DSIM"
create_clock -period 156.25MHz -name default
config_interface -expose_global
csynth_design
export_design -format ip_catalog
close_project

open_project run 
set_top run
open_solution "solution1"
config_rtl -prefix "hls4ml_" 
set_part ${part_name}
add_files $src_path_root/ereg_v1.cpp
add_files $src_path_root/hls4ml_hcal.cpp -cflags "-I $galapagos_path/middleware/include -I $src_path_root -I $src_path_root/weights -I $src_path_root/nnet_utils -DSIM"
create_clock -period 156.25MHz -name default
config_interface -expose_global
csynth_design
export_design -format ip_catalog
close_project

open_project egress 
set_top egress
open_solution "solution1"
config_rtl -prefix "hls4ml_" 
set_part ${part_name}
add_files $src_path_root/ereg_v1.cpp
add_files $src_path_root/hls4ml_hcal.cpp -cflags "-I $galapagos_path/middleware/include -I $src_path_root -I $src_path_root/weights -I $src_path_root/nnet_utils -DSIM"
create_clock -period 156.25MHz -name default
config_interface -expose_global
csynth_design
export_design -format ip_catalog
close_project

quit
