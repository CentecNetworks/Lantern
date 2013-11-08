#!/bin/sh

#OUTDIR=$PLAT_TOP_DIR/../../out/pizza_box/build.busybox.$1
OUTDIR=$3

mkdir -p $OUTDIR
cp Makefile_ppc Makefile
make O=$OUTDIR distclean ARCH=ppc
cp config.ppc $OUTDIR/.config
make $COMPILE_SPEED O=$OUTDIR oldconfig ARCH=ppc
make $COMPILE_SPEED O=$OUTDIR ARCH=ppc CROSS_COMPILE=ppc-linux-
