/**
 *  Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file
 * @brief This file defines tempfile macros
 */

#ifndef __OFP_TEMPFILE_H__
#define __OFP_TEMPFILE_H__

/******************************************************************************
* Header Files 
******************************************************************************/

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/

#define SHOW_CONTROLLER_TEMP_FILE  "/tmp/show_controller.txt"

#ifdef _GLB_UML_SYSTEM_
#define OFP_TOOL_VSCTL "/centec_switch/sbin/ovs-vsctl"
#define OFP_TOOL_OFCTL "/centec_switch/sbin/ovs-ofctl"
#define OFP_TOOL_APPCTL "/centec_switch/sbin/ovs-appctl"
#define OFP_TOOL_OVSDB_TOOL "/centec_switch/sbin/ovsdb-tool"
#define OFP_OVS_USER_CONFDB_PATH "/mnt/flash/lib/conf.db"
#define OFP_OVS_IMAGE_CONFDB_PATH "/mnt/flash/openvswitch/etc/db/conf_v330.db"
#define OFP_OVS_USER_BACKUP_CONFDB_DIR "/mnt/flash/lib/backup"
#define OFP_SYNC_DB_DONE_FILE "/tmp/ofp_sync_db_done"
#else
#define OFP_TOOL_VSCTL "/usr/local/openvswitch/bin/ovs-vsctl"
#define OFP_TOOL_OFCTL "/usr/local/openvswitch/bin/ovs-ofctl"
#define OFP_TOOL_APPCTL "/usr/local/openvswitch/bin/ovs-appctl"
#define OFP_TOOL_OVSDB_TOOL "/usr/local/openvswitch/bin/ovsdb-tool"
#define OFP_OVS_USER_CONFDB_PATH "/usr/local/openvswitch/etc/db/conf.db"
#define OFP_OVS_IMAGE_CONFDB_PATH "/usr/local/openvswitch/etc/db/conf_v330.db"
#define OFP_OVS_USER_BACKUP_CONFDB_DIR "/mnt/flash/lib/backup"
#define OFP_SYNC_DB_DONE_FILE "/tmp/ofp_sync_db_done"
#endif

#ifdef _GLB_UML_SYSTEM_
#define OVS_TOOL_OFCTL "/centec_switch/sbin/ovs-ofctl"
#define OVS_TOOL_VSCTL "/centec_switch/sbin/ovs-vsctl"
#else
#define OVS_TOOL_OFCTL "/usr/local/openvswitch/bin/ovs-ofctl"
#define OVS_TOOL_VSCTL "/usr/local/openvswitch/bin/ovs-vsctl"
#endif

#endif /* !__OFP_TEMPFILE_H__ */
