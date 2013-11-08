/**
 @file ctc_l3if.h

 @author Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2010-1-7

 @version v2.0

This file contains all L3 interface related data structure, enum, macro and proto.

*/
#ifndef _CTC_L3_IF
#define _CTC_L3_IF


/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_const.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/

/**
 @defgroup l3if L3if
 @{
*/

/*-----------------------L3if id allocation ---------------------
Humber :
 Vlan L3if,Phy L3if and sub L3if share 1023 L3 interface.
 |0...............................................................................................1022|
 |Phy and sub l3if ID(0~511)  ->                      <-(0~1022) vlan l3if ID   |

 vlan l3if             range:  0~1022
 Phy and sub l3if range:   0~511
 Greatbelt:
  Vlan L3if,Phy L3if and sub L3if share 1023 L3 interface ,and the range from 1~1023
-----------------------------------------------------------*/

#define MIN_CTC_L3IF_ID                     0       /**<  Min value of layer 3 interface ID */
#define MAX_CTC_L3IF_ID                     1022    /**<  Max value of layer 3 interface ID */
#define MAX_CTC_L3IF_ID_FOR_PHY_SUB_IF      511     /**<  Max value of Phy-if and sub-if  ID*/

#define CTC_L3IF_INVALID_L3IF_ID            0x3FF   /**<  Invalid value of layer 3 interface ID */

#define MAX_CTC_L3IF_VMAC_MAC_INDEX         3       /**<  Max index of layer 3 interface virtual router-mac */
#define MAX_CTC_L3IF_MCAST_TTL_THRESHOLD    256     /**<  Max TTL threshold of layer 3 interface multicast */
#define MAX_CTC_L3IF_MTU_SIZE               16*1024 /**<  Max MTU size of layer 3 interface */

/**
 @brief  Define the prefix type of  l3 interface 40bits Router MAC
*/
enum ctc_l3if_route_mac_type_e
{
    CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE0 ,               /**< Type 0*/
    CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE1 ,               /**< Type 1*/
    CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_RSV_ROUTER_MAC , /**< Type 2,will be reserved for per-system router mac*/
    CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID ,        /**< Non-support type*/
    MAX_CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE
};
typedef enum ctc_l3if_route_mac_type_e  ctc_l3if_route_mac_type_t;

/**
 @brief  Define the type of l3if
*/
enum ctc_l3if_type_e
{
    CTC_L3IF_TYPE_PHY_IF ,   /**< Physical IP interfaces*/
    CTC_L3IF_TYPE_SUB_IF ,   /**< Sub-interface*/
    CTC_L3IF_TYPE_VLAN_IF,   /**< Logical IP Vlan interfaces*/
    MAX_L3IF_TYPE_NUM
};
typedef enum ctc_l3if_type_e  ctc_l3if_type_t;

/**
 @brief  Define the lookup operation for of IPSA lookup
*/
enum ctc_l3if_ipsa_lkup_type_e
{
    CTC_L3IF_IPSA_LKUP_TYPE_NONE, /**< Don't do IPSA lookup*/
    CTC_L3IF_IPSA_LKUP_TYPE_RPF,  /**< IPSA lookup operation used as RPF check */
    CTC_L3IF_IPSA_LKUP_TYPE_NAT,  /**< IPSA lookup operation used as NAT */
    CTC_L3IF_IPSA_LKUP_TYPE_PBR,  /**< IPSA lookup operation used as PBR*/
    MAX_CTC_L3IF_IPSA_LKUP_TYPE
};
typedef enum ctc_l3if_ipsa_lkup_type_e ctc_l3if_ipsa_lkup_type_t;


/**
  @brief   Define l3if property flags
*/
enum ctc_l3if_property_e
{
  CTC_L3IF_PROP_ROUTE_EN,               /**< Enable IP Routing */
  CTC_L3IF_PROP_PBR_LABEL,              /**< Set Policy-based routing label*/
  CTC_L3IF_PROP_NAT_IFTYPE ,            /**< The type of NAT,0~external,0- internal*/
  CTC_L3IF_PROP_ROUTE_ALL_PKT,          /**< If set,all packets are routed*/
  CTC_L3IF_PROP_IPV4_UCAST ,            /**< If set,IpV4 Ucast Routing will be enabled*/
  CTC_L3IF_PROP_IPV4_MCAST,             /**< If set,IpV4 Mcast Routing will be enabled*/
  CTC_L3IF_PROP_IPV4_MCAST_RPF ,        /**< If set,IpV4 Mcast RPF will be enabled*/
  CTC_L3IF_PROP_IPV4_SA_TYPE,           /**< SA lkup operation for IPV4 Lookup */
  CTC_L3IF_PROP_IPV6_UCAST,             /**< If set,IpV6 Ucast Routing will be enabled*/
  CTC_L3IF_PROP_IPV6_MCAST ,            /**< If set,IpV6 Mcast Routing will be enabled*/
  CTC_L3IF_PROP_IPV6_MCAST_RPF,         /**< If set,IpV6 Mcast Routing will be enabled*/
  CTC_L3IF_PROP_IPV6_SA_TYPE ,          /**< SA lkup operation for IPV6 Lookup */
  CTC_L3IF_PROP_VRF_ID ,                /**< The ID for virtual route forward table */
  CTC_L3IF_PROP_MTU_EN ,                /**< MTU check is enabled on the egress interface*/
  CTC_L3IF_PROP_MTU_SIZE ,              /**< MTU size of the egress interface*/
  CTC_L3IF_PROP_MTU_EXCEPTION_EN,       /**< MTU Exception enable for packet to cpu to do fragment */
  CTC_L3IF_PROP_TANSMIT_EN,             /**< Transmit is enabled for EPE  */
  CTC_L3IF_PROP_RECEIVE_EN,             /**< Receive is enabled for IPE */
  CTC_L3IF_PROP_EGS_MAC_SA_PREFIX_TYPE, /**< The prefix type of router MAC,to be used in the MAC SA for routed packets*/
  CTC_L3IF_PROP_EGS_MAC_SA_LOW_8BITS,   /**< Low 8 bits for the MAC SA for routed packets*/
  CTC_L3IF_PROP_EGS_MCAST_TTL_THRESHOLD,/**< Multicast TTL threshold */
  CTC_L3IF_PROP_MPLS_EN,                /**< Enable MPLS */
  CTC_L3IF_PROP_MPLS_LABEL_SPACE,       /**< Set MPLS label space */
  CTC_L3IF_PROP_ARP_EXCEPTION_TYPE,     /**< Set arp exception type */
  CTC_L3IF_PROP_DHCP_EXCEPTION_TYPE,    /**< DHCP exception type */
  CTC_L3IF_PROP_VRF_EN,                 /**< Enable VRF lookup */
  CTC_L3IF_PROP_IGMP_SNOOPING_EN,       /**< Enable IGMP snooping */
  MAX_CTC_L3IF_PROP_NUM
};
typedef enum ctc_l3if_property_e  ctc_l3if_property_t;

/**
  @brief  Define L3if structure
 */
struct ctc_l3if_s
{
  uint8   l3if_type;   /**< The type of l3interface , CTC_L3IF_TYPE_XXX */
  uint16  gport;       /**< Global logic port ID */
  uint16  vlan_id;     /**< Vlan Id */
};
typedef struct ctc_l3if_s  ctc_l3if_t;


/**
  @brief   Define L3if  VMAC structure
 */
struct ctc_l3if_vmac_s
{
  uint8   low_8bits_mac_index;   /**< The index of VMAC entry,0~3*/
  uint8   prefix_type;           /**< ctc_l3if_route_mac_type_t */
  uint8   low_8bits_mac;         /**< The id of L3 ingress interface */
};
typedef struct ctc_l3if_vmac_s  ctc_l3if_vmac_t;


#endif
/**@} end of @defgroup  l3if L3if */

