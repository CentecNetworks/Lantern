#!/bin/sh
set -o errexit
set -o pipefail

print_usage_and_exit()
{
    echo "Usage: build_uml.sh <obj_dir>"
    exit 1
}

if [ -z $1 ]; then
    print_usage_and_exit
fi

if [ ! -d $1 ]; then
    mkdir -p $1
fi

KDIR_UML=$1

if [ ! -f Makefile ]; then
    touch Makefile
fi

msg=`comm -3 Makefile.r Makefile`
if [ "$msg" != "" ]; then
    cp -f Makefile.r Makefile
fi

if [ "$2" == "clean" ]; then
    make O=$1 distclean
else
    if [ -f $KDIR_UML/.config ]; then
        cd $KDIR_UML
        make $COMPILE_SPEED ARCH=um SUBARCH=i386 CROSS_COMPILE= linux
    else
        make O=$KDIR_UML ARCH=um SUBARCH=i386 distclean
        make O=$KDIR_UML ARCH=um SUBARCH=i386 uml_defconfig >/dev/null
        make $COMPILE_SPEED O=$KDIR_UML ARCH=um SUBARCH=i386 linux
        make $COMPILE_SPEED O=$KDIR_UML ARCH=um SUBARCH=i386 modules
    fi
fi
