#!/bin/bash
set -o errexit
set -o pipefail

#export PATH
print_usage_and_exit()
{
    echo "Usage: build_octeon.sh <obj_dir> <r|d> [linecard]"
    exit 1
}

if [ -z $1 ]; then
    print_usage_and_exit
fi

KDIR_MIPS=$1

if [ ! -d $1 ]; then
    mkdir -p $1
fi

cp usr/*.bit.gz $1/
if [ ! -f Makefile ]; then
    touch Makefile
fi

if [ $2 == "r" ]; then
    #msg=`comm -3 Makefile.r Makefile`
    #if [ "$msg" != "" ]; then
        cp Makefile.r Makefile
    #fi
    cp arch/mips/configs/ctc_octeon_defconfig.r arch/mips/configs/ctc_octeon_defconfig 
fi

if [ $2 == "d" ]; then
    #msg=`comm -3 Makefile.d Makefile`
    #if [ "$msg" != "" ]; then
        cp Makefile.d Makefile
    #fi
    cp arch/mips/configs/ctc_octeon_defconfig.d arch/mips/configs/ctc_octeon_defconfig 
fi
export ARCH=mips
# For test
#export CROSS_COMPILE=mips64-octeon-linux-gnu-
export CROSS_COMPILE=mips-linux-gnu-

#if [ "$3" == "linecard" ]; then
#    make LC=1 O=$1 distclean
#    make LC=1 O=$1 ctc8247_defconfig >/dev/null
#    make LC=1 O=$1 uImage
#    make LC=1 O=$1 modules
#else
if [ -f $KDIR_MIPS/.config ]; then
    cd $KDIR_MIPS
    make $COMPILE_SPEED O=$1 vmlinux.64 modules
else
    make O=$1 distclean
    make O=$1 ctc_octeon_defconfig >/dev/null
    make $COMPILE_SPEED O=$1 vmlinux.64 modules
fi
#fi
