#include <linux/init.h>

__INITDATA
    .globl humber_demo_lattice_bin_start
humber_demo_lattice_bin_start:
    .incbin "humberdemo.bit.gz" 
    .globl humber_demo_lattice_bin_end
humber_demo_lattice_bin_end:

    .globl seoul_g24eu_xilinx_bin_start
seoul_g24eu_xilinx_bin_start:
    .incbin "g24eu.bit.gz" 
    .globl seoul_g24eu_xilinx_bin_end
seoul_g24eu_xilinx_bin_end:

