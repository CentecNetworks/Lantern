/**
 @file ctc_const.h

 @date 2009-10-27

 @version v2.0

The file define  all CTC SDK module's  common Macros and constant.
*/


#ifndef _CTC_CONST_H
#define _CTC_CONST_H
#include "ctc_mix.h"
/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_macro.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/

/**
 @defgroup const    CONST
 @{
*/
#define CTC_ETH_ADDR_LEN                6           /**< Ethernet mac address length (unit: byte) */
#define CTC_IPV6_ADDR_LEN               4           /**< IPv6 address length (unit: byte) */
#define CTC_MAX_ECPN                    8           /**< Maximum equal cost path number */
#define CTC_LOCAL_PORT_LENGTH           8           /**< Local port id length(unit: bit) */
#define CTC_LOCAL_PORT_MASK             0xFF        /**< Local port mask */
#define CTC_MAC_NUM_PER_MACRO           8
#define CTC_LINKAGGID_MASK              0x1F        /**< Linkagg ID mask */
#define CTC_LINKAGG_CHIPID              0x1F        /**< Linkagg port's global chip ID */
#define CTC_INVALID_CHIPID              0xFF        /**< Invalid chip id */
#define CTC_MAX_GPORT_ID                0x1F7F      /**< Max global port Id */
/* no need any more in GB linkagg */
#define CTC_MAX_LINKAGG_MEMBER_PORT     16          /**< static load balance group_num:128,mem_num per group:8 */
#define CTC_MAX_VLAN_ID                 4095        /**< Max vlan id */
#define CTC_MIN_VLAN_ID                 1           /**< Min vlan id */
#define CTC_MAX_FID_ID                  16383       /**< Max fid id */
#define CTC_MIN_FID_ID                  0           /**< Min fid id */
#define CTC_MAX_VPLS_PORT_ID            8191        /**< Max vpls port id */
#define CTC_MIN_VRFID_ID                0           /**< Min vrfid id */
#define BITS_NUM_OF_BYTE                8           /**< Bits num of byte */
#define BITS_NUM_OF_WORD                32          /**< Bits num of word */
#define BITS_SHIFT_OF_WORD              5           /**< Bits shift of word */
#define BITS_MASK_OF_WORD               0x1F        /**< Bits mask of word */

#define CTC_MAX_GCHIP_CHIP_ID           0x1D        /**< Max global chip id */
#define CTC_MAX_UINT16_VALUE            0xFFFF      /**< Max unsigned 16bit value */
#define CTC_MAX_UINT32_VALUE            0xFFFFFFFF  /**< Max unsigned 32bit value */
#define CTC_MAX_UINT8_VALUE             0xFF        /**< Max unsigned 8bit value */
#define CTC_MAX_PORT_PER_LC             (CTC_MAX_LOCAL_CHIP_NUM * MAX_PORT_NUM_PER_CHIP)   /**< Max port num per line card */
#define CTC_PORT_BITMAP_IN_BYTE         ((CTC_MAX_PORT_PER_LC + 7) / BITS_NUM_OF_BYTE)     /**< port bitmap */
#define CTC_IPV6_ADDR_LEN_IN_BYTE       16          /**< IPv6 address length in byte */
#define CTC_IPV4_ADDR_LEN_IN_BYTE       4           /**< IPv4 address length in byte */
#define CTC_IPV6_ADDR_LEN_IN_BIT        128         /**< IPv6 address length in bit */
#define CTC_IPV4_ADDR_LEN_IN_BIT        32          /**< IPv4 address length in bit */
#define CTC_MAX_APS_GROUP_NUM           2048        /**< Max aps group num */

#define CTC_MAX_IPV4_VRFID              65534       /**< Max IPv4 vrfid */
#define CTC_MAX_IPV6_VRFID              4094        /**< Max IPv6 vrfid */

#define CTC_IPV6_ADDR_STR_LEN           44          /**< IPv6 address string length */

#define CTC_INVLD_NH_ID                 0xFFFFFFFF  /**< Invalid nexthop Id */
#define CTC_MPLS_SPACE_NUMBER           256         /**< Mpls space number */
#define CTC_MPLS_LABEL_NUM_PROCESS      3           /**< Mpls label num */
#define CTC_MPLS_OUT_SQE_NUM            64          /**< Mpls out sqe num */


#define CTC_DEFAULT_IPG                 0x14    /**< Default ipg */
#define CTC_MAX_CHIP_NUM                32

/**
 @brief define direction
*/
enum ctc_direction_e
{
    CTC_INGRESS,        /**< Ingress direction */
    CTC_EGRESS,         /**< Egress direction */
    CTC_BOTH_DIRECTION  /**< Both Ingress and Egress direction */
};
typedef enum ctc_direction_e ctc_direction_t;

/**
 @brief define ip version
*/
enum ctc_ip_ver_e
{
    CTC_IP_VER_4,   /**< IP version 4 */
    CTC_IP_VER_6,   /**< IP version 6 */
    MAX_CTC_IP_VER
};
typedef enum ctc_ip_ver_e ctc_ip_ver_t;

#ifndef comm_addr_defined
#define comm_addr_defined
/* !End of added */
typedef uint8 mac_addr_t[CTC_ETH_ADDR_LEN];

typedef uint32 ip_addr_t;
typedef uint32 ipv6_addr_t[CTC_IPV6_ADDR_LEN];

#endif /* ! comm_addr_defined */
/* !End of added */

/**@} end of @defgroup  const CONST */


#endif /* _CTC_CONST_H*/

