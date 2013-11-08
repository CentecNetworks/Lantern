#!/bin/sh
set -o errexit
set -o pipefail

PATH=$PATH:/sw/pub/release/compile/tools/bin:/sw/pub/release/compile/tools/xtools.ppc-linux/bin

export PATH
print_usage_and_exit()
{
    echo "Usage: build_ctc8247.sh <obj_dir> <r|d> [linecard]"
    exit 1
}

if [ -z $1 ]; then
    print_usage_and_exit
fi

KDIR_PPC=$1

if [ ! -d $1 ]; then
    mkdir -p $1
fi

cp usr/*.bit.gz $1/
if [ ! -f Makefile ]; then
    touch Makefile
fi

if [ $2 == "r" ]; then
    msg=`comm -3 Makefile.r Makefile`
    if [ "$msg" != "" ]; then
        cp Makefile.r Makefile
    fi
    cp arch/powerpc/configs/ctc8247_defconfig.r arch/powerpc/configs/ctc8247_defconfig 
fi

if [ $2 == "d" ]; then
    msg=`comm -3 Makefile.d Makefile`
    if [ "$msg" != "" ]; then
        cp Makefile.d Makefile
    fi
    cp arch/powerpc/configs/ctc8247_defconfig.d arch/powerpc/configs/ctc8247_defconfig
fi
export ARCH=powerpc
export CROSS_COMPILE=ppc-linux-

#if [ "$3" == "linecard" ]; then
#    make LC=1 O=$1 distclean
#    make LC=1 O=$1 ctc8247_defconfig >/dev/null
#    make LC=1 O=$1 uImage
#    make LC=1 O=$1 modules
#else
if [ -f $KDIR_PPC/.config ]; then
    cd $KDIR_PPC
    make $COMPILE_SPEED O=$1 uImage
    make $COMPILE_SPEED O=$1 modules
#added by jcao for bug 14811    
    make $COMPILE_SPEED O=$1 ctc8247.dtb
else
    make O=$1 distclean
    make O=$1 ctc8247_defconfig >/dev/null
    make $COMPILE_SPEED O=$1 uImage
    make $COMPILE_SPEED O=$1 modules
#added by jcao for bug 14811    
    make $COMPILE_SPEED O=$1 ctc8247.dtb
fi
#fi
