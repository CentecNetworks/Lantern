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
 * @brief This file implements netdev APIs
 */

#include "ofp_api.h"
#include "ofp_netdev_api.h"
#include "adpt_port.h"
#include "adpt_message.h"

#include "hal_port.h"

VLOG_DEFINE_THIS_MODULE(ofp_netdev_api);

/**
 * Get netdev statistics
 * @param ifname                interface name
 * @param if_stats              interface statistics
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_get_port_stats(const char * ifname, ofp_if_stats_t* p_if_stats)
{
    uint16_ofp gport;
    ofp_interface_type_t if_type;

    OFP_PTR_CHECK(ifname);
    OFP_LOG_DEBUG_FUNC();
    OFP_LOG_DEBUG("ifname = %s\n", ifname);

    OFP_ERROR_RETURN(adpt_port_get_port_type_by_name(ifname, &if_type));

    if (if_type != OFP_INTERFACE_TYPE_PHYSICAL)
    {
        return OFP_ERR_INVALID_PARAM;
    }

    OFP_ERROR_RETURN(adpt_port_get_gport_by_name(ifname, &gport));

    OFP_ERROR_RETURN(hal_port_get_mac_stats(gport, p_if_stats));

    return OFP_ERR_SUCCESS;
}

/**
 * Reset port mac stats
 * @param ofp_port          ofp port
 * @param port_dir          port direction
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_netdev_reset_port_stats(const char * ifname)
{
    uint16_ofp gport;
    ofp_interface_type_t if_type;

    OFP_PTR_CHECK(ifname);
    
    OFP_LOG_DEBUG_FUNC();
    OFP_LOG_DEBUG("ifname = %s\n", ifname);

    OFP_ERROR_RETURN(adpt_port_get_port_type_by_name(ifname, &if_type));
    
    if (if_type != OFP_INTERFACE_TYPE_PHYSICAL)
    {
        return OFP_ERR_INVALID_PARAM;
    }

    OFP_ERROR_RETURN(adpt_port_get_gport_by_name(ifname, &gport));
    
    OFP_ERROR_RETURN(hal_port_reset_mac_stats(gport));
    
    return OFP_ERR_SUCCESS;
}

/**
 * Set interface feature: advertised
 * @param ifname                interface name
 * @param advertised            advertised feature
 * @return  OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_set_port_advertised(const char * ifname, uint32_ofp advertised)
{
    OFP_PTR_CHECK(ifname);

    OFP_LOG_DEBUG_FUNC();
    OFP_LOG_DEBUG("ifname = %s\n", ifname);
    
    if (0 == strcmp(ifname, DEFAULT_CPU_PORT_INTERFACE_NAME) ||
        0 == strcmp(ifname, DEFAULT_BRIDGE_INTERFACE_NAME))
    {
        /**TODO to be handled */
        return OFP_ERR_SUCCESS;
    }
    OFP_ERROR_RETURN(adpt_port_set_of_fea_advertised(ifname, advertised));

    return OFP_ERR_SUCCESS;
}

int32_ofp
ofp_netdev_get_port_speed(const char * ifname, uint32_ofp *speed)
{
    uint16_ofp gport;
    ofp_interface_type_t if_type;
    OFP_PTR_CHECK(ifname);
    OFP_LOG_DEBUG_FUNC();
    OFP_LOG_DEBUG("ifname = %s\n", ifname);

    OFP_ERROR_RETURN(adpt_port_get_port_type_by_name(ifname, &if_type));

    if (if_type != OFP_INTERFACE_TYPE_PHYSICAL)
    {
        return OFP_ERR_INVALID_PARAM;
    }

    OFP_ERROR_RETURN(adpt_port_get_gport_by_name(ifname, &gport));

    OFP_ERROR_RETURN(hal_port_get_autonego_speed(gport, speed));

    return OFP_ERR_SUCCESS;
}

/**
 * Get interface features
 * @param ifname interface name
 * @param current current
 * @param advertised advertised
 * @param supported supported
 * @param peer peer
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_get_port_features(const char * ifname, uint32_ofp *p_current, 
    uint32_ofp *p_advertised, uint32_ofp *p_supported, uint32_ofp *p_peer)
{
    adpt_port_of_fea_info_t fea_info;

    OFP_PTR_CHECK(ifname);
    OFP_PTR_CHECK(p_current);
    OFP_PTR_CHECK(p_advertised);
    OFP_PTR_CHECK(p_supported);
    OFP_PTR_CHECK(p_peer);

    OFP_LOG_DEBUG_FUNC();
    OFP_LOG_DEBUG("ifname = %s\n", ifname);

    if (0 == strcmp(ifname, DEFAULT_CPU_PORT_INTERFACE_NAME) ||
        0 == strcmp(ifname, DEFAULT_BRIDGE_INTERFACE_NAME))
    {
        /**TODO to be handled */
        return OFP_ERR_SUCCESS;
    }

    OFP_ERROR_RETURN(adpt_port_get_of_fea_info(ifname, &fea_info));
    *p_current    = fea_info.current;
    *p_advertised = fea_info.advertised;
    *p_supported  = fea_info.supported;
    *p_peer       = fea_info.peer;

    return OFP_ERR_SUCCESS;
}

/**
 * Get interface features
 * @param ifname interface name
 * @param supported supported
 * @return OFP_SUCCESS, OFP_FAIL
 */
int32_ofp
ofp_netdev_get_port_supported(const char * ifname, uint32_ofp* p_supported)
{
    adpt_port_of_fea_info_t fea_info;

    OFP_PTR_CHECK(ifname);
    OFP_PTR_CHECK(p_supported);

    OFP_LOG_DEBUG_FUNC();
    OFP_LOG_DEBUG("ifname = %s\n", ifname);

    if (0 == strcmp(ifname, DEFAULT_CPU_PORT_INTERFACE_NAME) ||
        0 == strcmp(ifname, DEFAULT_BRIDGE_INTERFACE_NAME))
    {
        /**TODO to be handled */
        return OFP_ERR_SUCCESS;
    }

    OFP_ERROR_RETURN(adpt_port_get_of_fea_info(ifname, &fea_info));
    *p_supported  = fea_info.supported;

    return OFP_ERR_SUCCESS;
}


/**
 * Get interface link status
 * @param ifname interface name
 * @param carrier carrier
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_get_port_carrier(const char * ifname, uint8_ofp* p_carrier)
{
    adpt_port_of_fea_info_t fea_info;
    OFP_PTR_CHECK(ifname);

    OFP_LOG_DEBUG_FUNC();
    OFP_LOG_DEBUG("ifname = %s\n", ifname);

    /* eth1 is CPU port to receive the packet from Centec chip, 
       it is not a physical interface, workaround is applied*/
    if (0 == strcmp(ifname, DEFAULT_CPU_PORT_INTERFACE_NAME))
    {
        *p_carrier = 1;
        return OFP_ERR_SUCCESS;
    }

    OFP_ERROR_RETURN(adpt_port_get_of_fea_info(ifname, &fea_info));
    *p_carrier  = fea_info.carrier;

    return OFP_ERR_SUCCESS;
}

/**
 * Set port status modified
 * @param[in]  ifname                interface name
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_netdev_set_port_modified(const char * ifname)
{

    OFP_LOG_DEBUG_FUNC();

    OFP_ERROR_RETURN(adpt_port_set_port_modified(ifname, true));

    return OFP_ERR_SUCCESS;
}

/**
 * Get if port is modified
 * @param[in]  ifname                interface name
 * @param[out] p_is_modified         pointer to true/false
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_netdev_get_port_modified(const char * ifname, bool* p_is_modified)
{
    adpt_port_status_info_t p_status_info;

    OFP_PTR_CHECK(p_is_modified);
    memset(&p_status_info, 0, sizeof(adpt_port_status_info_t));

    OFP_LOG_DEBUG_FUNC();

    OFP_ERROR_RETURN(adpt_port_get_port_status(ifname, &p_status_info));
    *p_is_modified = p_status_info.modified;

    return OFP_ERR_SUCCESS;
}

/**
 * Clear the port modified status
 * @param[in]  ifname                interface name
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_netdev_clear_port_modified(const char * ifname)
{
    OFP_LOG_DEBUG_FUNC();

    OFP_ERROR_RETURN(adpt_port_set_port_modified(ifname, false));

    return OFP_ERR_SUCCESS;
}

/**
 * Get if any port is modified
 * @param[out] ifname                interface name that modified
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_netdev_get_any_port_modified(char** ifname)
{
    OFP_LOG_DEBUG_FUNC();

    OFP_ERROR_RETURN(adpt_port_get_any_port_modified(ifname));

    return OFP_ERR_SUCCESS;
}

/**
 * Get ethernet address
 * @param netdev_name           netdev name
 * @param ea                    ethernet address
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_get_etheraddr(const char *ifname, uint8_ofp ea[OFP_ETH_ADDR_LEN])
{
    struct ifreq ifr;
    int hwaddr_family;
    int sock = -1;

    OFP_PTR_CHECK(ifname);
    memset(&ifr, 0, sizeof ifr);

    OFP_LOG_DEBUG_FUNC();
    OFP_LOG_DEBUG("ifname = %s\n", ifname);

    ovs_strzcpy(ifr.ifr_name, ifname, sizeof ifr.ifr_name);
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        return OFP_ERR_FAIL;
    }
    
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0)
    {
        close(sock);
        sock = -1;
        return OFP_ERR_FAIL;
    }
    
    hwaddr_family = ifr.ifr_hwaddr.sa_family;
    if (hwaddr_family != AF_UNSPEC && hwaddr_family != ARPHRD_ETHER)
    {
        VLOG_WARN("%s device has unknown hardware address family %d", ifname, hwaddr_family);
    }
    memcpy(ea, ifr.ifr_hwaddr.sa_data, OFP_ETH_ADDR_LEN);
    close(sock);
    sock = -1;
    
    return OFP_ERR_SUCCESS;
}

/**
 * Get netdev flag
 * @param netdev_name           netdev name
 * @param flags                 flags
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
static uint32_ofp
ofp_netdev_get_flag(const char * netdev_name, int* flags)
{
    struct ifreq ifr;
    int ret = 0;
    
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        return OFP_ERR_FAIL;
    }
    
    memset(&ifr, 0, sizeof ifr);
    ovs_strzcpy(ifr.ifr_name, netdev_name, sizeof ifr.ifr_name);
    ret = ioctl(sock, SIOCGIFFLAGS, &ifr);
    if (ret)
    {
        close(sock);
        sock = -1;
        return OFP_ERR_FAIL;
    }
    
    *flags = ifr.ifr_flags;
    close(sock);
    sock = -1;

    return OFP_ERR_SUCCESS;
}

/**
 * Set netdev flag
 * @param netdev_name           netdev name
 * @param on_flag               on flag
 * @param off_flag              off flag
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
static uint32_ofp
ofp_netdev_set_flag(const char * ifname, int on_flag, int off_flag)
{
    /* Sockets used for ioctl operations. */
    struct ifreq ifr;
    int old_flags = 0;
    int ret = 0;
    int sock = 0;

    OFP_ERROR_RETURN(ofp_netdev_get_flag(ifname, &old_flags));
    memset(&ifr, 0, sizeof ifr);
    ovs_strzcpy(ifr.ifr_name, ifname, sizeof ifr.ifr_name);
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        return OFP_ERR_FAIL;
    }

    ifr.ifr_flags = (old_flags & ~off_flag) | on_flag;
    
    ret = ioctl(sock, SIOCSIFFLAGS, &ifr);
    if (ret)
    {
        close(sock);
        sock = -1;
        return OFP_ERR_FAIL;
    }
    close(sock);
    sock = -1;
    
    return OFP_ERR_SUCCESS;
}

/**
 * Set netdev mtu
 * @param ifname                netdev name
 * @param mtu                   MTU
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_set_mtu(const char * ifname, uint32_ofp mtu)
{
    int ret = 0;
    struct ifreq ifr;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        return OFP_ERR_FAIL;
    }

    memset(&ifr, 0, sizeof ifr);
    ovs_strzcpy(ifr.ifr_name, ifname, sizeof ifr.ifr_name);
    ifr.ifr_mtu = mtu;

    ret = ioctl(sock, SIOCSIFMTU, &ifr);
    if (ret)
    {
        close(sock);
        sock = -1;
        return OFP_ERR_FAIL;
    }
    close(sock);
    sock = -1;

    return OFP_ERR_SUCCESS;
}

/**
 * Enable device, only for physical interface
 * @param ifname                interface name
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
static int32_ofp
ofp_netdev_enable_device(const char * ifname)
{
    int flag = 0;

    OFP_ERROR_RETURN(ofp_netdev_get_flag(ifname, &flag));
    if (flag & IFF_UP)
    {
        return OFP_ERR_SUCCESS;
    }

    OFP_ERROR_RETURN(ofp_netdev_set_flag(ifname, IFF_UP, (int)NULL));

    return OFP_ERR_SUCCESS;
}

/**
 * Disable device, only for physical interface
 * @param ifname                interface name
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
static int32_ofp
ofp_netdev_disable_device(const char * ifname)
{
    int flag = 0;

    OFP_ERROR_RETURN(ofp_netdev_get_flag(ifname, &flag));
    if (!(flag & IFF_UP))
    {
        return OFP_ERR_SUCCESS;
    }

    /* shut down physical interface */
    OFP_ERROR_RETURN(ofp_netdev_set_flag(ifname, (int)NULL, IFF_UP));

    return OFP_ERR_SUCCESS;
}

/**
 * Enable interface
 * @param ifname                netdev interface name
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_enable_interface(const char * ifname)
{
    uint16_ofp gport;
    uint16_ofp ofport;
    int flag = 0;
#ifdef _OFP_UML_
    adpt_notify_link_status_req_t req;
    uint32_ofp port_no;
    uint32_ofp slot_no;
#endif

    OFP_LOG_DEBUG_FUNC();
    OFP_LOG_DEBUG("ifname = %s\n", ifname);

    /* 1. check if interface is enabled already */
    OFP_ERROR_RETURN(ofp_netdev_get_flag(ifname, &flag));
    if (flag & IFF_UP)
    {
        OFP_ERROR_RETURN(ofp_netdev_set_port_modified(ifname));
        return OFP_ERR_SUCCESS;
    }

    /* 2. Set device enable in kernel */
    OFP_ERROR_RETURN(ofp_netdev_enable_device(ifname));
    OFP_ERROR_RETURN(adpt_port_get_gport_by_name(ifname, &gport));

    /* 3. Set PHY enable */
#ifndef _OFP_UML_
    OFP_ERROR_RETURN(hal_port_set_phy_enable(gport, true));
#else
    memset(&req, 0, sizeof(req));
    if (2 != sscanf(ifname, "eth-%d-%d", &slot_no, &port_no))
    {
        OFP_LOG_ERROR("can not enable interface %s\n", ifname);
        return OFP_ERR_SUCCESS;
    }

    req.port_no = port_no;
    req.slot_no = slot_no;
    req.enable = TRUE;
    LCM2ADPT_MSG_SEND(ADPT_MSG_TYPE_NOTIFY_LINK, &req);
#endif
    OFP_LOG_DEBUG("gport = %d\n", gport);

    /* 4. Notify ovs */
    OFP_ERROR_RETURN(ofp_netdev_set_port_modified(ifname));

    /* 5. log */
    OFP_ERROR_RETURN(adpt_port_get_ofport_by_name(ifname, &ofport));
    OFP_LOG_INFO("port: %u (interface: %s) state change to up", ofport, ifname);
    
    return OFP_ERR_SUCCESS;

}

/**
 * Disable interface
 * @param ifname                netdev interface name
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_disable_interface(const char * ifname)
{
    uint16_ofp gport;
    uint16_ofp ofport;
    int flag;
#ifdef _OFP_UML_
    adpt_notify_link_status_req_t req;
    uint32_ofp port_no;
    uint32_ofp slot_no;
#endif

    OFP_LOG_DEBUG_FUNC();
    OFP_LOG_DEBUG("ifname = %s\n", ifname);

    /* 1. check if interface is disabled already */
    OFP_ERROR_RETURN(ofp_netdev_get_flag(ifname, &flag));
    if (!(flag & IFF_UP))
    {
        OFP_ERROR_RETURN(ofp_netdev_set_port_modified(ifname));
        return OFP_ERR_SUCCESS;
    }

    /* 2. Set device disable in kernel */
    OFP_ERROR_RETURN(ofp_netdev_disable_device(ifname));
    OFP_ERROR_RETURN(adpt_port_get_gport_by_name(ifname, &gport));
    
    /* 3. Set PHY disable */
#ifndef _OFP_UML_
    OFP_ERROR_RETURN(hal_port_set_phy_enable(gport, false));
#else
    memset(&req, 0, sizeof(req));
    if (2 != sscanf(ifname, "eth-%d-%d", &slot_no, &port_no))
    {
        OFP_LOG_ERROR("can disable interface %s\n", ifname);
        return OFP_ERR_SUCCESS;
    }

    req.port_no = port_no;
    req.slot_no = slot_no;
    req.enable = FALSE;

    LCM2ADPT_MSG_SEND(ADPT_MSG_TYPE_NOTIFY_LINK, &req);
#endif
    OFP_LOG_DEBUG("gport = %d\n", gport);
    
    /* 4. Notify ovs */
    OFP_ERROR_RETURN(ofp_netdev_set_port_modified(ifname));

    /* 5. log */
    OFP_ERROR_RETURN(adpt_port_get_ofport_by_name(ifname, &ofport));
    OFP_LOG_INFO("port: %u (interface: %s) state change to down", ofport, ifname);
    
    return OFP_ERR_SUCCESS;
}

/**
 * is a valid eth name, eg. "eth-0-1"
 * @param eth_name              eth name
 * @return true or false
 */
bool
ofp_netdev_name_is_phy_port(const char * ifname)
{
    uint32_ofp slot = 0xffffffff;
    uint32_ofp port = 0xffffffff;
    int32_ofp  ret;
    int32_ofp  i;
    int32_ofp non_digital_count = 0;

    /* eth-x-x, eth-x-xx*/
    if (strlen(ifname) != 7 && strlen(ifname) != 8)
    {
        return false;
    }

    if (ifname[0] != 'e' || ifname[1] != 't'  || ifname[2] != 'h' || ifname[3] != '-' || ifname[5] != '-')
    {
        return false;
    }

    for (i = 0; i < strlen(ifname); i++)
    {
        if (0 == isdigit(ifname[i])) /* if it is not a digital number */
        {
            non_digital_count++;
        }
    }
    if (non_digital_count != 5)
    {
        return false;
    }

    ret = sscanf(ifname, "eth-%1u-%2u", &slot, &port);
    if (ret != 2 || slot >= 0xff || port >= 0xff)
    {
        return false;
    }

    return true;
}

/**
 * Check whether the name is a valid gre tunnel name
 *  valid name should be greX ~ greXXX, and X is between 1 and OFP_TUNNEL_PORT_NO_NUM
 * @param netdev_name           netdev name
 * @return true or false
 */
bool
ofp_netdev_name_is_gre_port(const char * ifname)
{
    int i = 0;
    uint32_ofp gre_port = OFP_INVALID_GPORT;

    if (ifname == NULL)
    {
        return false;
    }
    /* greX ~ greXXX */
    if (kal_strlen(ifname) < 4 || kal_strlen(ifname) > 6)
    {
        return false;
    }
    if (kal_strncmp(ifname, "gre", 3) != 0)
    {
        return false;
    }
    /* The first digit should not be zero */
    if (*(ifname + 3) == '0')
    {
        return false;
    }
    for (i = 3; i < kal_strlen(ifname); i++)
    {
        if (!isdigit(*(ifname+i)))
        {
            return false;
        }
    }
    if (sscanf(ifname, "gre%3u", &gre_port) != 1)
    {
        return false;
    }
    if (gre_port < 1 || gre_port > OFP_TUNNEL_PORT_NO_NUM)
    {
        return false;
    }
    return true;
}

/**
 * Check if a port number is a tunnel port
 * @param port_no               port number
 * @param check_exist           whether check the port is created
 * @return true, false
 */
bool
ofp_netdev_ofport_is_gre_port(uint16_ofp ofport, bool check_exist)
{
    ofp_interface_type_t type = OFP_INTERFACE_TYPE_UNKNOWN;
    int32_ofp ret;
    
    if (ofport <= OFP_TUNNEL_PORT_NO_BASE ||
        ofport >= OFP_TUNNEL_PORT_NO_MAX)
    {
        return false;
    }

    if (!check_exist)
    {
        return true;
    }

    ret = adpt_port_get_port_type_by_ofport(ofport, &type);
    if (ret || type != OFP_INTERFACE_TYPE_GRE)
    {
        return false;
    }
    
    return true;
}

/**
 * Check if a port number is a physical port
 * @param port_no               port number
 * @param check_exist           whether check the port is created
 * @return true, false
 */
bool
ofp_netdev_ofport_is_phy_port(uint16_ofp ofport, bool check_exist)
{
    ofp_interface_type_t type = OFP_INTERFACE_TYPE_UNKNOWN;
    int32_ofp ret;

    if (ofport <= OFP_PHYSICAL_PORT_NO_BASE ||
        ofport >= OFP_PHYSICAL_PORT_NO_MAX)
    {
        return false;
    }

    if (!check_exist)
    {
        return true;
    }

    ret = adpt_port_get_port_type_by_ofport(ofport, &type);
    if (ret || type != OFP_INTERFACE_TYPE_PHYSICAL)
    {
        return false;
    }

    return true;
}

/**
 * Get gre ofport number by name
 *  valid name should be greX ~ greXXX, and X is between 1 and OFP_TUNNEL_PORT_NO_NUM
 * @param[in]  netdev_name              netdev name
 * @param[out] p_ofport                 pointer of ofport
 * @return port number
 */
uint16_ofp 
ofp_netdev_assign_gre_ofport(const char * ifname, uint16_ofp* p_ofport)
{
    uint32_ofp gre_port = OFP_INVALID_OFP_PORT;

    if (!ofp_netdev_name_is_gre_port(ifname))
    {
        return OFP_ERR_INVALID_PARAM;
    }
    if (1 != sscanf(ifname, "gre%3u", &gre_port))
    {
        return OFP_ERR_INVALID_PARAM;
    }
    *p_ofport = OFP_TUNNEL_PORT_NO_BASE + gre_port;

    return OFP_ERR_SUCCESS;
}

/**
 * Get physical ofport number by name
 *  valid name should be eth-x-xx
 * @param[in]  netdev_name              netdev name
 * @param[out] p_ofport                 pointer of ofport
 * @return port number
 */
uint16_ofp 
ofp_netdev_assign_phy_ofport(const char * ifname, uint16_ofp* p_ofport)
{
    uint32_ofp slot_no = 0, port_no = 0;

    if (!ofp_netdev_name_is_phy_port(ifname))
    {
        return OFP_ERR_INVALID_PARAM;
    }
    if (2 != sscanf(ifname, "eth-%u-%u", &slot_no, &port_no))
    {
        return OFP_ERR_INVALID_PARAM;
    }
    *p_ofport = OFP_PHYSICAL_PORT_NO_BASE + port_no + slot_no * MAX_PHY_PORT_NUM;
    
    return OFP_ERR_SUCCESS;
}
