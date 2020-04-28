set part_name "xczu9eg-ffvb1156-2-e"
set project_name "zcu102"

open_project -reset $project_name

set IDIR "-I../src/beh -I../src/lib -I../src/inc -I../src/encoder -I../src/dataproc -I../src/top"
set LDFLAGS "--verbose --std=c++0x"

# source files
set SRCS {}
lappend SRCS "../src/lib/common.c"
lappend SRCS "../src/top/top.c"
lappend SRCS "../src/beh/DUFT_ap_ctrl_chain.c"
lappend SRCS "../src/beh/DUT.c"
lappend SRCS "../src/encoder/encoder.c"
lappend SRCS "../src/dataproc/dataproc.c"

# Set the top-level function to be top from "../src/top/top.c"
set_top top

for {set i 0} {$i < [llength $SRCS]} {incr i} {
  add_files [lindex $SRCS $i] -cflags $IDIR
}

add_files -blackbox ../src/blackbox/DUFT_bkb.json

# test bench
set TBS {}
lappend TBS "../src/test/tb_top.c"
lappend TBS "../src/test/directed_test.c"

for {set i 0} {$i < [llength $TBS]} {incr i} {
  add_files -tb [lindex $TBS $i] -cflags $IDIR
}

# start simulation
open_solution "solution1"
set_part $part_name

create_clock -period 10

#csim_design -ldflags $LDFLAGS

csynth_design

# CHOOSE EITHER VCD FILE GENERATION (1st option) or COSIM EXECUTION (2nd option)
cosim_design -ldflags $LDFLAGS -trace_level all

exit
