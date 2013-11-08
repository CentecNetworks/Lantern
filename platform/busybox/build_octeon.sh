#!/bin/sh

#OUTDIR=$PLAT_TOP_DIR/../../out/pizza_box/build.busybox.$1
OUTDIR=$3

mkdir -p $OUTDIR
cp Makefile_octeon Makefile
make O=$OUTDIR distclean ARCH=mips
cp config.octeon $OUTDIR/.config
make $COMPILE_SPEED O=$OUTDIR oldconfig ARCH=mips
make $COMPILE_SPEED O=$OUTDIR ARCH=mips CROSS_COMPILE=mips-linux-gnu-
