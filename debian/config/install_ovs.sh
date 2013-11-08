#!/bin/bash

########################################################################
# Copyright (C) 2004-2013 Centec Networks. All rights reserved.
########################################################################

if [ ! -d ./ovs_package  ]; then
    echo "The Open vSwitch package is corrupted, missing directory: ovs_package" 
    exit
fi

echo "Installing Open vSwitch package ..."
cp -rf ./ovs_package/* /
cp -rf ./ovs_script/openvswitch /etc/init.d/

/sbin/insserv -f openvswitch
echo "Done"
echo ""