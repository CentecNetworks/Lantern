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

#ifndef __CTCLIB_SYS_CMD_H__
#define __CTCLIB_SYS_CMD_H__

typedef int (*sal_sys_cmd_clone_fn)(void*);

int ctclib_reconstruct_system_cmd_exec_str(void* sys_cmd);
int32 ctclib_reconstruct_system_cmd_chld_clone(sal_sys_cmd_clone_fn func,char *cmdstring);

#endif
