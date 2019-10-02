set galapagos_path $::env(GALAPAGOS_PATH)
set project_name test
set impl_path $galapagos_path:/projects/$project_name/$project_name.runs/impl_1

open_hw
connect_hw_server
open_hw_target
current_hw_device [get_hw_devices xcku115_0]
refresh_hw_device [lindex [get_hw_devices xcku115_0] 0]

set_property PROGRAM.FILE {$impl_path/shellTop.bit} [get_hw_devices xcku115_0]
set_property PROBES.FILE {$impl_path/shellTop.ltx} [get_hw_devices xcku115_0]
set_property FULL_PROBES.FILE {$impl_path/shellTop.ltx} [get_hw_devices xcku115_0]
current_hw_device [get_hw_devices xcku115_0]
refresh_hw_device [lindex [get_hw_devices xcku115_0] 0]
program_hw_devices [get_hw_devices xcku115_0]
refresh_hw_device [lindex [get_hw_devices xcku115_0] 0]
