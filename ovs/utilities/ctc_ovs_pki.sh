#!/bin/bash
#
# Copyright (C) 2004-2012 Centec Networks. All rights reserved.
#
# Author: Yi Zhao 2012-04-25

OVS_PKI_IN=ovs-pki.in
OVS_PKI_SCRIPT=ovs-pki

#source $TOP_DIR/build/Bash.env.inc
#set -o errexit

PKIDIR=/mnt/flash/pki
LOGDIR=/mnt/flash

sed -e "s,[@]PKIDIR[@],$PKIDIR,g" -e "s,[@]LOGDIR[@],$LOGDIR,g" $OVSROOT/utilities/$OVS_PKI_IN > $OVSROOT/utilities/$OVS_PKI_SCRIPT  
chmod +x $OVSROOT/utilities/$OVS_PKI_SCRIPT
