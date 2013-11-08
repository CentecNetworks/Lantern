#!/bin/bash

########################################################################
# Copyright (C) 2004-2013 Centec Networks. All rights reserved.
########################################################################

_V330_TOOLCHAIN_PATH=
_V330_GLIBC_REL_PATH=mips-linux-gnu/libc/lib
_V330_CROSSCOMPILE_PREFIX=mips-linux-gnu-
_V330_COMPILE_JOBS=1
_V330_DEBUG=n
_V330_VER=r
_V330_OPEN_SOURCE=y
CUR_ABS_PATH=`pwd`
_V330_TOP_PATH=$(dirname $CUR_ABS_PATH)
_V330_OUT_PATH=$_V330_TOP_PATH/out
_V330_OVS_PACKAGE=$_V330_OUT_PATH/open_vswitch.tar.gz
_OVS_PKG_TMP_TOP_DIR=$_V330_OUT_PATH/ovs_package
_OVS_TMP_DIR=$_OVS_PKG_TMP_TOP_DIR/open_vswitch

error() {
    echo "Error: $1."
    exit 1
}

usage() {
    cat <<EOF 
Usage: $0 :
     -p path, specify absolute path for toolchain
     -g path, specify glibc path relative to the toolchain path (must be 32bit glibc, default is mips-linux-gnu/libc/lib)
     -c prefix, specify crosscompile prefix(default is mips-linux-gnu-)
     -j number, specify compile speed
     -d , _V330_DEBUG version 
EOF
}

is_number()
{
    if ! [[ $1 =~ ^[0-9]+$ ]] ; then
    return 1;
    fi

    return 0
}

while getopts "p:c:j:d" opt; do
    case $opt in
        p ) _V330_TOOLCHAIN_PATH=$OPTARG ;;
    g ) _V330_GLIBC_REL_PATH=$OPTARG ;;
        c ) _V330_CROSSCOMPILE_PREFIX=$OPTARG ;;
    j ) _V330_COMPILE_JOBS=$OPTARG 
        if ! is_number $_V330_COMPILE_JOBS ; then
        usage
        error "compile jobs must be an integer"
        fi
        ;;
    d ) _V330_DEBUG=y ;;
    \? ) usage
        exit 1 ;;
    esac
done

if [ -z $_V330_TOOLCHAIN_PATH ]; then
    usage
    error "must have toolchain path specified";
fi

if [ $_V330_DEBUG == 'y' ]; then
    _V330_VER='d'
fi

echo "-------------------------------------------------------------------------"
echo "toolchain path: $_V330_TOOLCHAIN_PATH"
echo "glibc path: $_V330_TOOLCHAIN_PATH/$_V330_GLIBC_REL_PATH"
echo "crosscompile_prefix: $_V330_CROSSCOMPILE_PREFIX"
echo "debug: $_V330_DEBUG"
echo "compile jobs: $_V330_COMPILE_JOBS"
echo "final image directory: $_V330_TOP_PATH/out"
echo "-------------------------------------------------------------------------"

# Export gcc/gdb path
echo "Generate Makefile.env.inc under $_V330_TOP_PATH/system"
cat > $_V330_TOP_PATH/system/Makefile.env.inc <<EOF
# Export environment to makefile
export PATH=$_V330_TOOLCHAIN_PATH/bin:$PATH
export _V330_TOOLCHAIN_PATH=$_V330_TOOLCHAIN_PATH
export _V330_GLIBC_PATH=$_V330_TOOLCHAIN_PATH/$_V330_GLIBC_REL_PATH
export _V330_CROSSCOMPILE_PREFIX=$_V330_CROSSCOMPILE_PREFIX
export _V330_DEBUG=$_V330_DEBUG
export _V330_COMPILE_JOBS=$_V330_COMPILE_JOBS
export _V330_TOP_PATH=$_V330_TOP_PATH
export _V330_OPEN_SOURCE=$_V330_OPEN_SOURCE

export _OVS_BIN_DIR=$_V330_OUT_PATH/build.octeon.$_V330_VER/bin.linux-board
export _OVS_SRC_DIR=$_V330_TOP_PATH/system/ovs
export _OVS_CFG_DIR=$_V330_TOP_PATH/system/build/cfg
export _OVS_DEBIAN_CFG_DIR=$_V330_TOP_PATH/system/debian/config
export _OVS_PREBUILT=$_V330_TOP_PATH/system/prebuilt
export _OVS_PACKAGE=$_V330_OVS_PACKAGE
export _OVS_PKG_TMP_TOP_DIR=$_OVS_PKG_TMP_TOP_DIR
export _OVS_TMP_DIR=$_OVS_PKG_TMP_TOP_DIR/open_vswitch
export _OVS_PKG_TMP_DIR=$_OVS_TMP_DIR/ovs_package
export _OVS_SCRIPT_TMP_DIR=$_OVS_TMP_DIR/ovs_script
EOF
