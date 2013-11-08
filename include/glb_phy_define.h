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

#ifndef __GLB_PHY_DEFINE_H__
#define __GLB_PHY_DEFINE_H__

/*need to update pal_phy_type_t at same time*/
enum glb_phy_type_e
{
    GLB_PHY_TYPE_UNKNOWN = 0x0,
    GLB_PHY_TYPE_10BASE_T,
    GLB_PHY_TYPE_100BASE_TX,
    GLB_PHY_TYPE_BASE_PX,
    GLB_PHY_TYPE_BASE_BX10,
    GLB_PHY_TYPE_100BASE_FX,
    GLB_PHY_TYPE_100BASE_LX,
    GLB_PHY_TYPE_1000BASE_T,
    GLB_PHY_TYPE_1000BASE_CX,
    GLB_PHY_TYPE_1000BASE_LX,
    GLB_PHY_TYPE_1000BASE_SX,
    GLB_PHY_TYPE_10GBASE_ER,
    GLB_PHY_TYPE_10GBASE_LRM,
    GLB_PHY_TYPE_10GBASE_LR,
    GLB_PHY_TYPE_10GBASE_SR,
    GLB_PHY_TYPE_10GBASE_PASSIVE_COPPER,
    GLB_PHY_TYPE_10GBASE_ACTIVE_COPPER,
    GLB_PHY_TYPE_MAX,
};
typedef enum glb_phy_type_e glb_phy_type_t;

enum glb_port_type_e
{
    GLB_PORT_TYPE_AUTO,
    GLB_PORT_TYPE_RJ45,
    GLB_PORT_TYPE_SFP,
    GLB_PORT_TYPE_XFP,
    GLB_PORT_TYPE_SFP_PLUS,
    GLB_PORT_TYPE_MAX
};
typedef enum glb_port_type_e glb_port_type_t;

/*duplex*/
enum glb_port_duplex_e
{
    GLB_DUPLEX_AUTO,
    GLB_DUPLEX_HALF,
    GLB_DUPLEX_FULL,
    GLB_DUPLEX_MAX
};
typedef enum glb_port_duplex_e glb_port_duplex_t;

/* speed*/
enum glb_port_speed_e
{
    GLB_SPEED_AUTO,
    GLB_SPEED_10M,
    GLB_SPEED_100M,
    GLB_SPEED_1G,
    GLB_SPEED_10G,
    GLB_SPEED_MAX
};
typedef enum glb_port_speed_e glb_port_speed_t;

/* Loopback*/
enum glb_lb_phy_e
{
    GLB_LB_NONE,
    GLB_LB_PHY_FAR,
    GLB_LB_PHY_NEAR,
    GLB_LB_MAX
};
typedef enum glb_lb_phy_e glb_lb_phy_t;

/* flow control */
enum glb_flowctrl_direction
{
    GLB_FLOWCTRL_RECV = 0,
    GLB_FLOWCTRL_SEND
};

enum glb_port_flowctrl_direct_ability_e
{
    GLB_FLOWCTRL_DISABLE = 0,
    GLB_FLOWCTRL_ENABLE,
    GLB_FLOWCTRL_MAX
};
typedef enum glb_port_flowctrl_direct_ability_e glb_port_flowctrl_direct_ability_t;

struct glb_port_flowctrl_s
{
    glb_port_flowctrl_direct_ability_t send;
    glb_port_flowctrl_direct_ability_t recv;
};
typedef struct glb_port_flowctrl_s glb_port_flowctrl_t;

struct glb_phy_state_s
{
    uint32 link_up;    /* if non-zero, link is up; otherwise link is down */
    glb_port_duplex_t duplex;     /* if PAL_PHY_DUPLEX_AUTO, auto-neg not completed */
    glb_port_speed_t speed;      /* if PAL_PHY_SPEED_AUTO, auto-neg not completed */
    glb_port_flowctrl_t  flowctrl; /* flow control */
    glb_port_type_t linkup_media; /* media type:auto, rj45, sfp, xfp, sfp+*/
};
typedef struct glb_phy_state_s glb_phy_state_t;

struct glb_port_cfg_s
{
    uint32             enable;
    glb_lb_phy_t       loopback;
    glb_port_duplex_t  duplex;
    glb_port_speed_t   speed;
    glb_port_flowctrl_t  flowctrl;
    glb_port_type_t    media; /* media type:auto, rj45, sfp, xfp, sfp+*/
    uint32               syncE_enable;
};
typedef struct glb_port_cfg_s glb_port_cfg_t;

/*link*/
enum glb_port_link_e
{
    GLB_LINK_DOWN = 0,
    GLB_LINK_UP,
    GLB_LINK_MAX
};
typedef enum glb_port_link_e glb_port_link_t;

#ifdef _CTC_OF_
enum glb_conf_if_opcode_e
{
    CONF_IF_SPEED,
    CONF_IF_DUPLEX,
    CONF_IF_MEDIA,
    CONF_IF_LB,
    CONF_IF_ENABLE,
    CONF_IF_MAX
};
typedef enum glb_conf_if_opcode_e glb_conf_if_opcode_t;

struct glb_conf_if_s {
    glb_conf_if_opcode_t opcode;
    uint32 value;
};
typedef struct glb_conf_if_s glb_conf_if_t;
#endif

#endif
