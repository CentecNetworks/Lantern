/**
 @file drv_parity_error.h

 @date 2011-08-03

 @version v5.1

 The file contains all parity error fatal interrupt resume realization
*/

#ifndef _DRV_HUMBER_PARITY_ERROR_H
#define _DRV_HUMBER_PARITY_ERROR_H


#include "kal.h"

#define DRV_PARITY_ERROR_MAX_ENTRY_WORD     8

/* memory mapping type */
enum mem_mapping_type
{
    DRV_MEM_MAPPING_DS_PHY_PORT,
    DRV_MEM_MAPPING_DS_ROUTER_MAC,
    DRV_MEM_MAPPING_DS_VRF,
    DRV_MEM_MAPPING_DS_SRC_INTERFACE,
    DRV_MEM_MAPPING_DS_PROTOCOL_VLAN,
    DRV_MEM_MAPPING_DS_SRC_PORT,
    DRV_MEM_MAPPING_DS_PHY_PORT_EXT,
    DRV_MEM_MAPPING_DS_MPLS_CTL,
    DRV_MEM_MAPPING_IPE_DS_PBB_MAC_TABLE,
    DRV_MEM_MAPPING_DS_BIDI_PIM_GROUP_TABLE,
    DRV_MEM_MAPPING_IPE_CLASSIFICATION_COS_MAP_TABLE,
    DRV_MEM_MAPPING_IPE_CLASSIFICATION_DSCP_MAP_TABLE,
    DRV_MEM_MAPPING_IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE,
    DRV_MEM_MAPPING_APSBRIDGETABLE,
    DRV_MEM_MAPPING_APSSELECTTABLE,
    DRV_MEM_MAPPING_FWDEXTTABLE,
    DRV_MEM_MAPPING_SEQUENCENUMBERTABLE,
    DRV_MEM_MAPPING_DS_DEST_PHY_PORT,
    DRV_MEM_MAPPING_EPE_HEADER_EDIT_SGMAC_PRIORITY_MAP_MEM,
    DRV_MEM_MAPPING_EPE_HDR_EDIT_L2_EDIT_LOOPBACK_RAM,
    DRV_MEM_MAPPING_DS_L3_EDIT_TUNNEL_V6_IP,
    DRV_MEM_MAPPING_DS_L3_EDIT_TUNNEL_V4_IP_SA,
    DRV_MEM_MAPPING_EPE_EDIT_PRIORITY_MAP_TABLE,
    DRV_MEM_MAPPING_DS_DEST_PORT,
    DRV_MEM_MAPPING_DS_DEST_INTERFACE,
    DRV_MEM_MAPPING_DS_VPLS_PORT,
    DRV_MEM_MAPPING_DS_APS_BRIDGE_MCAST,
    DRV_MEM_MAPPING_DS_LINK_AGG_BLOCK_MASK,
    DRV_MEM_MAPPING_DS_LINK_AGG_BITMAP,
    DRV_MEM_MAPPING_DS_LINK_AGGREAGATION_GROUP,
    DRV_MEM_MAPPING_DS_VLAN,
    DRV_MEM_MAPPING_DS_VLAN_STATUS,
    DRV_MEM_MAPPING_DS_QUEUE_DROP_PROFILE_ID,
    DRV_MEM_MAPPING_DSQ_MGR_EGRESS_RESRC_THRESHOLD,
    DRV_MEM_MAPPING_DS_HEAD_HASH_MOD,
    DRV_MEM_MAPPING_DS_LINK_AGG_MEMBER_NUM,
    DRV_MEM_MAPPING_DS_LINK_AGGREGATION,
    DRV_MEM_MAPPING_DS_QUEUE_DROP_PROFILE,
    DRV_MEM_MAPPING_DS_QUEUE_NUM_GEN_CTL,
    DRV_MEM_MAPPING_DS_QUEUE_IPG_INDEX,
    DRV_MEM_MAPPING_DS_SERVICE_QUEUE_HASH_KEY,
    DRV_MEM_MAPPING_DS_SERVICE_QUEUE,
    DRV_MEM_MAPPING_DS_SGMAC_MAP,
    DRV_MEM_MAPPING_DS_QUEUE_MAP,
    DRV_MEM_MAPPING_DS_ETH_OAM_HASH_KEY0,
    DRV_MEM_MAPPING_DS_ETH_OAM_HASH_KEY1,
    DRV_MEM_MAPPING_DS_MEP_CHAN_TABLE,
    DRV_MEM_MAPPING_OAM_DS_PORT_PROPERTY,
    DRV_MEM_MAPPING_DS_ETH_MEP,
    DRV_MEM_MAPPING_OAM_DS_MA,
    DRV_MEM_MAPPING_OAM_DS_MA_NAME,
    DRV_MEM_MAPPING_OAM_DS_ICC,
    DRV_MEM_MAPPING_INT_16K_SRAM,
    DRV_MEM_MAPPING_INT_48K_SRAM,
    DRV_MEM_MAPPING_INT_96K_SRAM,
    DRV_MEM_MAPPING_EXT_256K_SRAM,

    DRV_MEM_MAPPING_TYPE_MAX
};
typedef enum mem_mapping_type mem_mapping_type_e;

struct drv_chip_parity_error_init_s
{
    uint8 chip_id;
    uint8 is_hash_48ksize;
    uint8 rsv[2];
};
typedef struct drv_chip_parity_error_init_s drv_chip_parity_error_init_t;


struct drv_chip_parity_error_info_s
{
    uint32  asic_addr;
    uint32  asic_data[DRV_PARITY_ERROR_MAX_ENTRY_WORD];
    uint32  mapping_addr;
    uint32  mapping_data[DRV_PARITY_ERROR_MAX_ENTRY_WORD];
    uint8   is_same;
    uint8   rsv[3];
};
typedef struct drv_chip_parity_error_info_s drv_chip_parity_error_info_t;

/**
 @brief The function write table data to a mapping memory location
*/
extern void
drv_humber_mem_mapping_write(uint8 chip_id, tbl_id_t tbl_id, uint32 start_data_addr, uint32* data, int32 len);

/**
 @brief The function process the parity error which can be resume
*/
extern int32
drv_humber_parity_error_handle(uint8 chip_id, tbl_id_t tbl_id,
                               mem_mapping_type_e sram_type, uint32 parity_fail_addr,
                               drv_chip_parity_error_info_t* info);

/**
 @brief Init mapping memory
*/
extern int32
drv_humber_mem_mapping_init(drv_chip_parity_error_init_t* p_parity_error);

#endif

