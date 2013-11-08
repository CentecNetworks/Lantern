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
 */

#ifndef __GLB_TEMPFILE_DEFINE_H__
#define __GLB_TEMPFILE_DEFINE_H__

/*Hardware related temporary file define*/
#define GLB_CHASSIS_TYPE_FILE         "/tmp/ctcos_chassis_type"
#define GLB_BOARD_INFO_FILE           "/tmp/ctcos_board_info"

#define GLB_HAL_HSRV_SOCKET_PATH               "/tmp/.shal_msg"
#define GLB_HAL_HSRV_HI_PRIO_SOCKET_PATH       "/tmp/.shal_hi_prio_msg"

#define GLB_CHSM_LCM_SOCK_PATH          "/tmp/.chsm_lcm_ctrl"

#define GLB_LCM_LCSH_SOCK_PATH          "/tmp/.lcm_lcsh_ctrl"

/*Global hsrv done file define*/
#define GLB_HSRV_DONE_FILE       "/tmp/glb_hsrv_done"
#define GLB_SDK_DONE_FILE       "/tmp/glb_sdk_done"

/*file for dump*/
#define GLB_DUMP_FILE   "/tmp/dumpfile"

/*file for peth dump*/
#define GLB_PETH_DUMP_FILE "/tmp/.glb_peth_dump_file"

/*file for IPC cli parameter*/
#define GLB_CLI_SHOW_OUT_FILE "/tmp/ipc_cli_show_out"
#define GLB_PARA_DUMP_FILE "/tmp/ipc_para_dump"

/*file for EPLD update*/
#define GLB_UPDATE_EPLD_FLAG        "/mnt/flash/glb_update_epld"
#define GLB_UPDATE_EPLD_DONE        "/tmp/glb_update_epld_done"
#define GLB_UPDATE_EPLD_ERASE_DONE  "/tmp/glb_update_epld_erase_done"
#define GLB_UPDATE_EPLD_PROG_DONE        "/tmp/glb_update_epld_prog_done"

#define GLB_PETH_IPC_SOCKET_PATH               "/tmp/.peth_ipc_msg"
/*file for bootrom update*/
#define GLB_UPDATE_BOOTROM_DONE        "/tmp/glb_update_bootrom_done"
#ifdef BOOTUP_DIAG
#define DIAG_STR_BOOTUP_LOG_PATH    "/tmp/bootup_diagnostic.log"
#endif

#ifdef HAVE_SMARTCFG
#define GLB_SMARTCFG_FILE               "/tmp/glb_scfg_file"
#endif /* !HAVE_SMARTCFG */

#define GLB_SYS_MAC_FILE               "/tmp/glb_sysmac_file"
#define GLB_SERIALNO_FILE               "/tmp/glb_serialno_file"

#define GLB_POE_PORT_CFG_DONE_NAME "/tmp/glb_poe_port_cfg_done"
#define GLB_POE_SYS_CFG_DONE_NAME "/tmp/glb_poe_sys_cfg_done"

#endif /*!__GLB_TEMPFILE_DEFINE_H__*/
