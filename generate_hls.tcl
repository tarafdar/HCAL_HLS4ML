set src_path_root . 
set part_name  xcku115-flva1517-2-e

open_project hls4ml_hcal
set_top hls4ml_hcal
open_solution "solution1"
set_part ${part_name}
add_files $src_path_root/ereg_v1.cpp
add_files $src_path_root/hls4ml_hcal.cpp -cflags "-I $src_path_root -I $src_path_root/weights -I $src_path_root/nnet_utils"
create_clock -period 250MHz -name default
config_interface -expose_global
csynth_design
export_design -format ip_catalog
close_project

quit
