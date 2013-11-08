#!/bin/bash

########################################################################
# Copyright (C) 2004-2013 Centec Networks. All rights reserved.
########################################################################

source Makefile.env.inc 

#Package Open vSwitch
echo "Building Open vSwitch package: $_OVS_PACKAGE"

if [ ! -d $_OVS_BIN_DIR ]; then
    echo "Unable to retrieve ovs binaries, please check if you have built ovs correctly"
fi

ovs_package_bin="$_OVS_BIN_DIR/ovs-vswitchd $_OVS_BIN_DIR/ovs-vsctl $_OVS_BIN_DIR/ovs-ofctl $_OVS_BIN_DIR/ovs-appctl \
    $_OVS_BIN_DIR/ovsdb-tool $_OVS_BIN_DIR/ovsdb-server $_OVS_SRC_DIR/utilities/ovs-pki"
ovs_package_ctc_bin="$_OVS_PREBUILT/bin/board_type_detect"
ovs_package_ctc_datapath_profile="$_OVS_CFG_DIR/etc/datapath_profile/datapath_cfg.txt \
    $_OVS_CFG_DIR/etc/datapath_profile/e330_24t_V3.txt $_OVS_CFG_DIR/etc/datapath_profile/e330_48s_ddr275m.txt \
    $_OVS_CFG_DIR/etc/datapath_profile/e330_48s_V3.txt $_OVS_CFG_DIR/etc/datapath_profile/e330_48t_V3.txt $_OVS_CFG_DIR/etc/datapath_profile/e330_48t.txt"
ovs_package_data="$_OVS_SRC_DIR/vswitchd/vswitch.ovsschema"
ovs_package_list="${ovs_package_bin} ${ovs_package_ctc_bin} ${ovs_package_ctc_profile} \
    ${ovs_package_data}"

for package_file in ${ovs_package_list}; do
    if [ ! -f ${package_file} ]; then 
        echo "Unable to retrieve file: ${package_file}, please check if you have built ovs correctly"
        exit 1
    fi
done

rm -rf $_OVS_PKG_TMP_TOP_DIR
mkdir -p $_OVS_TMP_DIR
mkdir -p $_OVS_PKG_TMP_DIR
mkdir -p $_OVS_SCRIPT_TMP_DIR

mkdir -p $_OVS_PKG_TMP_DIR/usr/local/openvswitch/bin
mkdir -p $_OVS_PKG_TMP_DIR/usr/local/openvswitch/data
mkdir -p $_OVS_PKG_TMP_DIR/usr/local/openvswitch/etc
mkdir -p $_OVS_PKG_TMP_DIR/usr/local/openvswitch/etc/db
mkdir -p $_OVS_PKG_TMP_DIR/usr/local/openvswitch/var
mkdir -p $_OVS_PKG_TMP_DIR/etc/datapath_profile

for package_file in ${ovs_package_bin}; do
    cp -rf ${package_file} $_OVS_PKG_TMP_DIR/usr/local/openvswitch/bin
    chmod +x ${package_file}
done

for package_file in ${ovs_package_ctc_bin}; do
    cp -rf ${package_file} $_OVS_PKG_TMP_DIR/usr/local/openvswitch/bin
    chmod +x ${package_file}
done

for package_file in ${ovs_package_data}; do
    cp -rf ${package_file} $_OVS_PKG_TMP_DIR/usr/local/openvswitch/data
done

for package_file in ${ovs_package_ctc_datapath_profile}; do
    cp -rf ${package_file} $_OVS_PKG_TMP_DIR/etc/datapath_profile
done

cp -rf $_OVS_CFG_DIR/conf_v330.db $_OVS_PKG_TMP_DIR/usr/local/openvswitch/etc/db/conf.db
cp -rf $_OVS_CFG_DIR/mem_profile.cfg $_OVS_PKG_TMP_DIR/etc/mem_profile.cfg
cp -rf $_OVS_DEBIAN_CFG_DIR/openvswitch $_OVS_SCRIPT_TMP_DIR
cp -rf $_OVS_DEBIAN_CFG_DIR/install_ovs.sh $_OVS_TMP_DIR

chmod 777 $_OVS_SCRIPT_TMP_DIR/openvswitch
chmod 777 $_OVS_TMP_DIR/install_ovs.sh

rm -rf $_OVS_PACKAGE
tar czvf $_OVS_PACKAGE -C ${_OVS_PKG_TMP_TOP_DIR} . > /dev/null 2>&1
