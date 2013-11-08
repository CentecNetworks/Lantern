/**
 * Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
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
 * @brief This file is the header file for netdev-ctc-vport.c
 */

#ifndef NETDEV_CTC_VPORT_H
#define NETDEV_CTC_VPORT_H 1

#include <stdbool.h>
#include "netdev-provider.h"
#include "ofp_api.h"

struct netdev;
struct netdev_stats;

int
netdev_vport_get_tunnel_info(struct netdev_dev *dev, ofp_tunnel_info_t *tunnel_info);
void
register_netdev_live_vport_class(void);

#endif
