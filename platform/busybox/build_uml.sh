#!/bin/sh

OUTDIR=../../../out/$2/build.busybox.i686.$1

if [ "$3" == "clean" ]; then
    if [ -d $OUTDIR ]; then
        make O=$OUTDIR distclean ARCH=i386
    fi
    exit 0
fi

mkdir -p $OUTDIR
cp Makefile_ppc Makefile
if [ -f $OUTDIR/.config ]; then
    make $COMPILE_SPEED O=$OUTDIR ARCH=i386 CROSS_COMPILE=
else
    make O=$OUTDIR distclean ARCH=i386
    cp config.ppc $OUTDIR/.config
    make O=$OUTDIR oldconfig ARCH=i386 >/dev/null
    make $COMPILE_SPEED O=$OUTDIR ARCH=i386 CROSS_COMPILE=
fi
exit 0
