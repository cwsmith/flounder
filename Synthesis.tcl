open_project hls 
open_solution solution 
set_part xcku5p-sfvb784-3-e 
add_files -cflags "-DHLSLIB_SYNTHESIS -std=c++11 -I./hlslib/include" "pl_graph_ops.cc" 
set_top pl_rgraph_invert
create_clock -period 300MHz -name default
csynth_design
exit
