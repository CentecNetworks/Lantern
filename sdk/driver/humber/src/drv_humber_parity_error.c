/**
 @file drv_humber_parity_error.c

 @date 2011-08-03

 @version v5.1

 The file contains all parity error fatal interrupt resume realization
*/

#include "drv_chip_io.h"
#include "drv_common.h"
#include "drv_error.h"
#include "drv_humber_parity_error.h"


extern tables_t drv_tbls_list[MAX_TBL_NUM];

#define DRV_PER_SRAM_ENTRY_SIZE             16
#define DRV_SRAM_HASH_KEY_BASE              0x0d200000
#define DRV_SRAM_ASSOCIATE_DATA_BASE        0x041c0000
#define DRV_SRAM_VLAN_BASE                  0x0c480000
#define DRV_EXT_SRAM_ASSOCIATE_DATA_BASE    0x05000000

#define PARITY_LOCK \
    if (g_mem_map_ctl->parity_mutex) kal_mutex_lock(g_mem_map_ctl->parity_mutex)
#define PARITY_UNLOCK \
    if (g_mem_map_ctl->parity_mutex) kal_mutex_unlock(g_mem_map_ctl->parity_mutex)

/* memory mapping table */
struct mem_mapping_table_s
{
    uint32 start_addr[MAX_LOCAL_CHIP_NUM];      /*start address of one memory mapping table*/
    uint32 size[MAX_LOCAL_CHIP_NUM];
};
typedef struct mem_mapping_table_s mem_mapping_table_t;

/* memory mapping control */
struct mem_mapping_control_s
{
    mem_mapping_table_t mem_map_table[DRV_MEM_MAPPING_TYPE_MAX];
    uint8 is_hash_48ksize;
    uint8 is_ext_sram_en;
    uint8 is_mpls_store_ext_sram;
    uint8 is_dyntbl_store_ext_sram;
    uint8 is_metnh_store_ext_sram;
    uint8 rsv[3];
    kal_mutex_t* parity_mutex;
};
typedef struct mem_mapping_control_s mem_mapping_control_t;

static mem_mapping_control_t* g_mem_map_ctl = NULL;


/**
 @brief get table Id
*/
static int32
_drv_humber_get_tblid_from_mapping_type(mem_mapping_type_e type)
{
    uint32 table_id = 0;

    switch(type)
    {
        case DRV_MEM_MAPPING_DS_PHY_PORT:
            table_id = DS_PHY_PORT;
            break;
        case DRV_MEM_MAPPING_DS_ROUTER_MAC:
            table_id = DS_ROUTER_MAC;
            break;
        case DRV_MEM_MAPPING_DS_VRF:
            table_id = DS_VRF;
            break;
        case DRV_MEM_MAPPING_DS_SRC_INTERFACE:
            table_id = DS_SRC_INTERFACE;
            break;
        case DRV_MEM_MAPPING_DS_PROTOCOL_VLAN:
            table_id = DS_PROTOCOL_VLAN;
            break;
        case DRV_MEM_MAPPING_DS_SRC_PORT:
            table_id = DS_SRC_PORT;
            break;
        case DRV_MEM_MAPPING_DS_PHY_PORT_EXT:
            table_id = DS_PHY_PORT_EXT;
            break;
        case DRV_MEM_MAPPING_DS_MPLS_CTL:
            table_id = DS_MPLS_CTL;
            break;
        case DRV_MEM_MAPPING_IPE_DS_PBB_MAC_TABLE:
            table_id = IPE_DS_PBB_MAC_TABLE;
            break;
        case DRV_MEM_MAPPING_DS_BIDI_PIM_GROUP_TABLE:
            table_id = DS_BIDI_PIM_GROUP_TABLE;
            break;
        case DRV_MEM_MAPPING_IPE_CLASSIFICATION_COS_MAP_TABLE:
            table_id = IPE_CLASSIFICATION_COS_MAP_TABLE;
            break;
        case DRV_MEM_MAPPING_IPE_CLASSIFICATION_DSCP_MAP_TABLE:
            table_id = IPE_CLASSIFICATION_DSCP_MAP_TABLE;
            break;
        case DRV_MEM_MAPPING_IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE:
            table_id = IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE;
            break;
        case DRV_MEM_MAPPING_APSBRIDGETABLE:
            table_id = APSBRIDGETABLE;
            break;
        case DRV_MEM_MAPPING_APSSELECTTABLE:
            table_id = APSSELECTTABLE;
            break;
        case DRV_MEM_MAPPING_FWDEXTTABLE:
            table_id = FWDEXTTABLE;
            break;
        case DRV_MEM_MAPPING_SEQUENCENUMBERTABLE:
            table_id = SEQUENCENUMBERTABLE;
            break;
        case DRV_MEM_MAPPING_DS_DEST_PHY_PORT:
            table_id = DS_DEST_PHY_PORT;
            break;
        case DRV_MEM_MAPPING_EPE_HEADER_EDIT_SGMAC_PRIORITY_MAP_MEM:
            table_id = EPE_HEADER_EDIT_SGMAC_PRIORITY_MAP_MEM;
            break;
        case DRV_MEM_MAPPING_EPE_HDR_EDIT_L2_EDIT_LOOPBACK_RAM:
            table_id = EPE_HDR_EDIT_L2_EDIT_LOOPBACK_RAM;
            break;
        case DRV_MEM_MAPPING_DS_L3_EDIT_TUNNEL_V6_IP:
            table_id = DS_L3_EDIT_TUNNEL_V6_IP;
            break;
        case DRV_MEM_MAPPING_DS_L3_EDIT_TUNNEL_V4_IP_SA:
            table_id = DS_L3_EDIT_TUNNEL_V4_IP_SA;
            break;
        case DRV_MEM_MAPPING_EPE_EDIT_PRIORITY_MAP_TABLE:
            table_id = EPE_EDIT_PRIORITY_MAP_TABLE;
            break;
        case DRV_MEM_MAPPING_DS_DEST_PORT:
            table_id = DS_DEST_PORT;
            break;
        case DRV_MEM_MAPPING_DS_DEST_INTERFACE:
            table_id = DS_DEST_INTERFACE;
            break;
        case DRV_MEM_MAPPING_DS_VPLS_PORT:
            table_id = DS_VPLS_PORT;
            break;
        case DRV_MEM_MAPPING_DS_APS_BRIDGE_MCAST:
            table_id = DS_APS_BRIDGE_MCAST;
            break;
        case DRV_MEM_MAPPING_DS_LINK_AGG_BLOCK_MASK:
            table_id = DS_LINK_AGG_BLOCK_MASK;
            break;
        case DRV_MEM_MAPPING_DS_LINK_AGG_BITMAP:
            table_id = DS_LINK_AGG_BITMAP;
            break;
        case DRV_MEM_MAPPING_DS_LINK_AGGREAGATION_GROUP:
            table_id = DS_LINK_AGGREAGATION_GROUP;
            break;
        case DRV_MEM_MAPPING_DS_VLAN:
            table_id = DS_VLAN;
            break;
        case DRV_MEM_MAPPING_DS_VLAN_STATUS:
            table_id = DS_VLAN_STATUS;
            break;
        case DRV_MEM_MAPPING_DS_QUEUE_DROP_PROFILE_ID:
            table_id = DS_QUEUE_DROP_PROFILE_ID;
            break;
        case DRV_MEM_MAPPING_DSQ_MGR_EGRESS_RESRC_THRESHOLD:
            table_id = DSQ_MGR_EGRESS_RESRC_THRESHOLD;
            break;
        case DRV_MEM_MAPPING_DS_HEAD_HASH_MOD:
            table_id = DS_HEAD_HASH_MOD;
            break;
        case DRV_MEM_MAPPING_DS_LINK_AGG_MEMBER_NUM:
            table_id = DS_LINK_AGG_MEMBER_NUM;
            break;
        case DRV_MEM_MAPPING_DS_LINK_AGGREGATION:
            table_id = DS_LINK_AGGREGATION;
            break;
        case DRV_MEM_MAPPING_DS_QUEUE_DROP_PROFILE:
            table_id = DS_QUEUE_DROP_PROFILE;
            break;
        case DRV_MEM_MAPPING_DS_QUEUE_NUM_GEN_CTL:
            table_id = DS_QUEUE_NUM_GEN_CTL;
            break;
        case DRV_MEM_MAPPING_DS_QUEUE_IPG_INDEX:
            table_id = DS_QUEUE_IPG_INDEX;
            break;
        case DRV_MEM_MAPPING_DS_SERVICE_QUEUE_HASH_KEY:
            table_id = DS_SERVICE_QUEUE_HASH_KEY;
            break;
        case DRV_MEM_MAPPING_DS_SERVICE_QUEUE:
            table_id = DS_SERVICE_QUEUE;
            break;
        case DRV_MEM_MAPPING_DS_SGMAC_MAP:
            table_id = DS_SGMAC_MAP;
            break;
        case DRV_MEM_MAPPING_DS_QUEUE_MAP:
            table_id = DS_QUEUE_MAP;
            break;
        case DRV_MEM_MAPPING_DS_ETH_OAM_HASH_KEY0:
            table_id = DS_ETH_OAM_HASH_KEY0;
            break;
        case DRV_MEM_MAPPING_DS_ETH_OAM_HASH_KEY1:
            table_id = DS_ETH_OAM_HASH_KEY1;
            break;
        case DRV_MEM_MAPPING_DS_MEP_CHAN_TABLE:
            table_id = DS_MEP_CHAN_TABLE;
            break;
        case DRV_MEM_MAPPING_OAM_DS_PORT_PROPERTY:
            table_id = OAM_DS_PORT_PROPERTY;
            break;
        case DRV_MEM_MAPPING_DS_ETH_MEP:
            table_id = DS_ETH_MEP;
            break;
        case DRV_MEM_MAPPING_OAM_DS_MA:
            table_id = OAM_DS_MA;
            break;
        case DRV_MEM_MAPPING_OAM_DS_MA_NAME:
            table_id = OAM_DS_MA_NAME;
            break;
        case DRV_MEM_MAPPING_OAM_DS_ICC:
            table_id = OAM_DS_ICC;
            break;
        case DRV_MEM_MAPPING_INT_16K_SRAM:
        case DRV_MEM_MAPPING_INT_48K_SRAM:
        case DRV_MEM_MAPPING_INT_96K_SRAM:
        case DRV_MEM_MAPPING_EXT_256K_SRAM:
            table_id = MAX_TBL_NUM;
            break;
        default:
            table_id = MAX_TBL_NUM;
            break;
    }

    return table_id;
}

/**
 @brief get mapping type
*/
static uint8
_drv_humber_get_mapping_type_from_tblid(tbl_id_t tbl_id)
{
    uint8 type = DRV_MEM_MAPPING_TYPE_MAX;
    uint32 data_base = 0;

    switch(tbl_id)
    {
        case DS_PHY_PORT:
            type = DRV_MEM_MAPPING_DS_PHY_PORT;
            break;
        case DS_ROUTER_MAC:
            type = DRV_MEM_MAPPING_DS_ROUTER_MAC;
            break;
        case DS_VRF:
            type = DRV_MEM_MAPPING_DS_VRF;
            break;
        case DS_SRC_INTERFACE:
            type = DRV_MEM_MAPPING_DS_SRC_INTERFACE;
            break;
        case DS_PROTOCOL_VLAN:
            type = DRV_MEM_MAPPING_DS_PROTOCOL_VLAN;
            break;
        case DS_SRC_PORT:
            type = DRV_MEM_MAPPING_DS_SRC_PORT;
            break;
        case DS_PHY_PORT_EXT:
            type = DRV_MEM_MAPPING_DS_PHY_PORT_EXT;
            break;
        case DS_MPLS_CTL:
            type = DRV_MEM_MAPPING_DS_MPLS_CTL;
            break;
        case IPE_DS_PBB_MAC_TABLE:
            type = DRV_MEM_MAPPING_IPE_DS_PBB_MAC_TABLE;
            break;
        case DS_BIDI_PIM_GROUP_TABLE:
            type = DRV_MEM_MAPPING_DS_BIDI_PIM_GROUP_TABLE;
            break;
        case IPE_CLASSIFICATION_COS_MAP_TABLE:
            type = DRV_MEM_MAPPING_IPE_CLASSIFICATION_COS_MAP_TABLE;
            break;
        case IPE_CLASSIFICATION_DSCP_MAP_TABLE:
            type = DRV_MEM_MAPPING_IPE_CLASSIFICATION_DSCP_MAP_TABLE;
            break;
        case IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE:
            type = DRV_MEM_MAPPING_IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE;
            break;
        case APSBRIDGETABLE:
            type = DRV_MEM_MAPPING_APSBRIDGETABLE;
            break;
        case APSSELECTTABLE:
            type = DRV_MEM_MAPPING_APSSELECTTABLE;
            break;
        case FWDEXTTABLE:
            type = DRV_MEM_MAPPING_FWDEXTTABLE;
            break;
        case SEQUENCENUMBERTABLE:
            type = DRV_MEM_MAPPING_SEQUENCENUMBERTABLE;
            break;
        case DS_DEST_PHY_PORT:
            type = DRV_MEM_MAPPING_DS_DEST_PHY_PORT;
            break;
        case EPE_HEADER_EDIT_SGMAC_PRIORITY_MAP_MEM:
            type = DRV_MEM_MAPPING_EPE_HEADER_EDIT_SGMAC_PRIORITY_MAP_MEM;
            break;
        case EPE_HDR_EDIT_L2_EDIT_LOOPBACK_RAM:
            type = DRV_MEM_MAPPING_EPE_HDR_EDIT_L2_EDIT_LOOPBACK_RAM;
            break;
        case DS_L3_EDIT_TUNNEL_V6_IP:
            type = DRV_MEM_MAPPING_DS_L3_EDIT_TUNNEL_V6_IP;
            break;
        case DS_L3_EDIT_TUNNEL_V4_IP_SA:
            type = DRV_MEM_MAPPING_DS_L3_EDIT_TUNNEL_V4_IP_SA;
            break;
        case EPE_EDIT_PRIORITY_MAP_TABLE:
            type = DRV_MEM_MAPPING_EPE_EDIT_PRIORITY_MAP_TABLE;
            break;
        case DS_DEST_PORT:
            type = DRV_MEM_MAPPING_DS_DEST_PORT;
            break;
        case DS_DEST_INTERFACE:
            type = DRV_MEM_MAPPING_DS_DEST_INTERFACE;
            break;
        case DS_VPLS_PORT:
            type = DRV_MEM_MAPPING_DS_VPLS_PORT;
            break;
        case DS_APS_BRIDGE_MCAST:
            type = DRV_MEM_MAPPING_DS_APS_BRIDGE_MCAST;
            break;
        case DS_LINK_AGG_BLOCK_MASK:
            type = DRV_MEM_MAPPING_DS_LINK_AGG_BLOCK_MASK;
            break;
        case DS_LINK_AGG_BITMAP:
            type = DRV_MEM_MAPPING_DS_LINK_AGG_BITMAP;
            break;
        case DS_LINK_AGGREAGATION_GROUP:
            type = DRV_MEM_MAPPING_DS_LINK_AGGREAGATION_GROUP;
            break;
        case DS_VLAN:
            type = DRV_MEM_MAPPING_DS_VLAN;
            break;
        case DS_VLAN_STATUS:
            type = DRV_MEM_MAPPING_DS_VLAN_STATUS;
            break;
        case DS_QUEUE_DROP_PROFILE_ID:
            type = DRV_MEM_MAPPING_DS_QUEUE_DROP_PROFILE_ID;
            break;
        case DSQ_MGR_EGRESS_RESRC_THRESHOLD:
            type = DRV_MEM_MAPPING_DSQ_MGR_EGRESS_RESRC_THRESHOLD;
            break;
        case DS_HEAD_HASH_MOD:
            type = DRV_MEM_MAPPING_DS_HEAD_HASH_MOD;
            break;
        case DS_LINK_AGG_MEMBER_NUM:
            type = DRV_MEM_MAPPING_DS_LINK_AGG_MEMBER_NUM;
            break;
        case DS_LINK_AGGREGATION:
            type = DRV_MEM_MAPPING_DS_LINK_AGGREGATION;
            break;
        case DS_QUEUE_DROP_PROFILE:
            type = DRV_MEM_MAPPING_DS_QUEUE_DROP_PROFILE;
            break;
        case DS_QUEUE_NUM_GEN_CTL:
            type = DRV_MEM_MAPPING_DS_QUEUE_NUM_GEN_CTL;
            break;
        case DS_QUEUE_IPG_INDEX:
            type = DRV_MEM_MAPPING_DS_QUEUE_IPG_INDEX;
            break;
        case DS_SERVICE_QUEUE_HASH_KEY:
            type = DRV_MEM_MAPPING_DS_SERVICE_QUEUE_HASH_KEY;
            break;
        case DS_SERVICE_QUEUE:
            type = DRV_MEM_MAPPING_DS_SERVICE_QUEUE;
            break;
        case DS_SGMAC_MAP:
            type = DRV_MEM_MAPPING_DS_SGMAC_MAP;
            break;
        case DS_QUEUE_MAP:
            type = DRV_MEM_MAPPING_DS_QUEUE_MAP;
            break;
        case DS_ETH_OAM_HASH_KEY0:
            type = DRV_MEM_MAPPING_DS_ETH_OAM_HASH_KEY0;
            break;
        case DS_ETH_OAM_HASH_KEY1:
            type = DRV_MEM_MAPPING_DS_ETH_OAM_HASH_KEY1;
            break;
        case DS_MEP_CHAN_TABLE:
            type = DRV_MEM_MAPPING_DS_MEP_CHAN_TABLE;
            break;
        case OAM_DS_PORT_PROPERTY:
            type = DRV_MEM_MAPPING_OAM_DS_PORT_PROPERTY;
            break;
        case DS_ETH_MEP:
            type = DRV_MEM_MAPPING_DS_ETH_MEP;
            break;
        case OAM_DS_MA:
            type = DRV_MEM_MAPPING_OAM_DS_MA;
            break;
        case OAM_DS_MA_NAME:
            type = DRV_MEM_MAPPING_OAM_DS_MA_NAME;
            break;
        case OAM_DS_ICC:
            type = DRV_MEM_MAPPING_OAM_DS_ICC;
            break;
        case DS_MAC_HASH_KEY0:
        case DS_IPV4_UCAST_HASH_KEY0:
        case DS_IPV6_UCAST_HASH_KEY0:
            if(g_mem_map_ctl->is_hash_48ksize)
            {
                type = DRV_MEM_MAPPING_INT_48K_SRAM;
            }
            else
            {
                type = DRV_MEM_MAPPING_INT_96K_SRAM;
            }
            break;
        default:    /*process dynamic table*/
            if((tbl_id < DS_MAC_ACL) || (tbl_id > DS_ETH_OAM_RMEP_HASH_KEY1))
            {
                type = DRV_MEM_MAPPING_TYPE_MAX;
            }
            else
            {
                data_base = DRV_TBL_GET_INFO(tbl_id).hw_data_base;

                if(g_mem_map_ctl->is_hash_48ksize
                    && (data_base >= DRV_SRAM_ASSOCIATE_DATA_BASE)
                    && (data_base < DRV_SRAM_ASSOCIATE_DATA_BASE + (16+96)*1024*16))
                {
                    type = DRV_MEM_MAPPING_INT_16K_SRAM;
                }
                else if((data_base >= DRV_SRAM_ASSOCIATE_DATA_BASE)
                    && (data_base < DRV_SRAM_ASSOCIATE_DATA_BASE + (16+48)*1024*16))
                {
                    type = DRV_MEM_MAPPING_INT_16K_SRAM;
                }
                else if((data_base >= DRV_EXT_SRAM_ASSOCIATE_DATA_BASE)
                    && (data_base < DRV_EXT_SRAM_ASSOCIATE_DATA_BASE + 256*1024*16))
                {
                    type = DRV_MEM_MAPPING_EXT_256K_SRAM;
                }
                else
                {
                    type = DRV_MEM_MAPPING_TYPE_MAX;
                }
            }
            break;
    }

    return type;
}

/**
 @brief mapping datapath init data
*/
static int32
_drv_humber_mem_mapping_datapath(uint8 chip_id)
{
    DRV_CHIP_ID_VALID_CHECK(chip_id);

    /*DsMplsCtlTable -- datapath init all to 0, no need to mapping here*/
    /*IpeClassificationCosMapTable -- datapath init all to 0, no need to mapping here*/
    /*DsFwdExtTable -- datapath init all to 0, no need to mapping here*/
    /*DsSequenceNumberTable -- datapath init all to 0, no need to mapping here*/
    /*EpeHdrEditL2EditLoopbackRam -- datapath init all to 0, no need to mapping here*/
    /*DsL3EditTunnelV6Ip -- datapath init all to 0, no need to mapping here*/
    /*DsL3EditTunnelV4IpSa -- datapath init all to 0, no need to mapping here*/
    /*EPEEditPriorityMapTable -- datapath init all to 0, no need to mapping here*/
    /*DsVplsPort -- datapath init all to 0, no need to mapping here*/
    /*DsLinkAggreagationGroup -- datapath init all to 0, no need to mapping here*/
    /*DsQueDropProf -- datapath init all to 0, no need to mapping here*/
    /*DsQueNumGenCtl -- datapath init all to 0, no need to mapping here*/
    /*DsServiceQueue -- datapath init all to 0, no need to mapping here*/
    /*DsSgmacMap -- datapath init all to 0, no need to mapping here*/

    /*notice: if any of above table is modified in datapath, here should do mapping*/

    return DRV_E_NONE;
}

/**
 @brief The function write table data to a mapping memory location
*/
void
drv_humber_mem_mapping_write(uint8 chip_id, tbl_id_t tbl_id, uint32 start_data_addr, uint32* data, int32 len)
{
    uint32 data_base = 0;
    uint32 length = 0;
    uint32 start_addr = 0;
    uint32 mem_mapping_addr = 0;
    uint8 type;
    uint8 i = 0;

    if(!g_mem_map_ctl)
    {
        return;
    }

    length = len;

    type = _drv_humber_get_mapping_type_from_tblid(tbl_id);
    if(DRV_MEM_MAPPING_TYPE_MAX == type)
    {
        return;
    }

    start_addr = g_mem_map_ctl->mem_map_table[type].start_addr[chip_id];

    if(DRV_MEM_MAPPING_INT_16K_SRAM == type)    /*should merge 16K with 96K or 48K*/
    {
        mem_mapping_addr = start_data_addr - DRV_SRAM_ASSOCIATE_DATA_BASE + start_addr;
    }
    else if(DRV_MEM_MAPPING_EXT_256K_SRAM == type)
    {
        mem_mapping_addr = start_data_addr - DRV_EXT_SRAM_ASSOCIATE_DATA_BASE + start_addr;
    }
    else if((DRV_MEM_MAPPING_INT_48K_SRAM == type) || (DRV_MEM_MAPPING_INT_96K_SRAM == type))   /*for hash*/
    {
        mem_mapping_addr = start_data_addr - DRV_SRAM_HASH_KEY_BASE + start_addr;
    }
    else
    {
        data_base = DRV_TBL_GET_INFO(tbl_id).hw_data_base;
        mem_mapping_addr = start_data_addr - data_base + start_addr;
    }

    length = (length >> 2);

    PARITY_LOCK;

    for (i = 0; i < length; i++)
    {
        *((uint32 *)(mem_mapping_addr + i*4)) = data[i];
    }

    PARITY_UNLOCK;
}

/**
 @brief The function process the parity error which can be resume
*/
int32
drv_humber_parity_error_handle(uint8 chip_id, tbl_id_t tbl_id,
                               mem_mapping_type_e sram_type, uint32 parity_fail_addr,
                               drv_chip_parity_error_info_t* info)
{
    #define DYN_SRAM_ENTRY_BYTE 16
    uint32 data_base = 0, start_data_addr = 0, entry_size = 0;
    uint32 start_addr = 0;
    uint32 mem_mapping_addr = 0;
    uint32 data[8];
    uint32 * mappingData = NULL;
    uint8 tmp_maping_type = 0;
    uint8 flag = 0;
    uint8 i = 0;

    if(!g_mem_map_ctl)
    {
        return DRV_E_NONE;
    }

    for(i=0; i<8; i++)
    {
        data[i] = 0;
    }

    if(MAX_TBL_NUM == tbl_id)
    {
        /*process dynamic sram*/
        switch(sram_type)
        {
            case DRV_MEM_MAPPING_INT_16K_SRAM:
                data_base = DRV_SRAM_ASSOCIATE_DATA_BASE;
                entry_size = DRV_PER_SRAM_ENTRY_SIZE;
                start_data_addr = data_base + parity_fail_addr * entry_size;    /*asic sram addr*/

                start_addr = g_mem_map_ctl->mem_map_table[sram_type].start_addr[chip_id];
                mem_mapping_addr = start_addr + parity_fail_addr * entry_size;  /*mapping memory addr*/

                DRV_IF_ERROR_RETURN(drv_chip_sram_read_for_parity_error(chip_id, start_data_addr, data, entry_size));
                mappingData = (uint32 *)mem_mapping_addr;
                break;
            case DRV_MEM_MAPPING_INT_48K_SRAM:
                if(g_mem_map_ctl->is_hash_48ksize)  /*process hash*/
                {
                    data_base = DRV_SRAM_HASH_KEY_BASE;
                    entry_size = DRV_PER_SRAM_ENTRY_SIZE;
                    start_data_addr = data_base + parity_fail_addr * entry_size;    /*asic sram addr*/

                    start_addr = g_mem_map_ctl->mem_map_table[sram_type].start_addr[chip_id];
                    mem_mapping_addr = start_addr + parity_fail_addr * entry_size;  /*mapping memory addr*/

                    DRV_IF_ERROR_RETURN(drv_chip_sram_read_for_parity_error(chip_id, start_data_addr, data, entry_size));
                    mappingData = (uint32 *)mem_mapping_addr;
                }
                else
                {
                    data_base = DRV_SRAM_ASSOCIATE_DATA_BASE + 16*1024*DRV_PER_SRAM_ENTRY_SIZE;
                    entry_size = DRV_PER_SRAM_ENTRY_SIZE;
                    start_data_addr = data_base + parity_fail_addr * entry_size;    /*asic sram addr*/

                    start_addr = g_mem_map_ctl->mem_map_table[sram_type].start_addr[chip_id];
                    mem_mapping_addr = start_addr + parity_fail_addr * entry_size;  /*mapping memory addr*/

                    DRV_IF_ERROR_RETURN(drv_chip_sram_read_for_parity_error(chip_id, start_data_addr, data, entry_size));
                    mappingData = (uint32 *)mem_mapping_addr;
                }
                break;
            case DRV_MEM_MAPPING_INT_96K_SRAM:
                if(g_mem_map_ctl->is_hash_48ksize)
                {
                    data_base = DRV_SRAM_ASSOCIATE_DATA_BASE + 16*1024*DRV_PER_SRAM_ENTRY_SIZE;
                    entry_size = DRV_PER_SRAM_ENTRY_SIZE;
                    start_data_addr = data_base + parity_fail_addr * entry_size;    /*asic sram addr*/

                    start_addr = g_mem_map_ctl->mem_map_table[sram_type].start_addr[chip_id];
                    mem_mapping_addr = start_addr + parity_fail_addr * entry_size;  /*mapping memory addr*/

                    DRV_IF_ERROR_RETURN(drv_chip_sram_read_for_parity_error(chip_id, start_data_addr, data, entry_size));
                    mappingData = (uint32 *)mem_mapping_addr;
                }
                else    /*process hash*/
                {
                    data_base = DRV_SRAM_HASH_KEY_BASE;
                    entry_size = DRV_PER_SRAM_ENTRY_SIZE;
                    start_data_addr = data_base + parity_fail_addr * entry_size;    /*asic sram addr*/

                    start_addr = g_mem_map_ctl->mem_map_table[sram_type].start_addr[chip_id];
                    mem_mapping_addr = start_addr + parity_fail_addr * entry_size;  /*mapping memory addr*/

                    DRV_IF_ERROR_RETURN(drv_chip_sram_read_for_parity_error(chip_id, start_data_addr, data, entry_size));
                    mappingData = (uint32 *)mem_mapping_addr;
                }
                break;
            case DRV_MEM_MAPPING_EXT_256K_SRAM:
                data_base = DRV_EXT_SRAM_ASSOCIATE_DATA_BASE;
                entry_size = DRV_PER_SRAM_ENTRY_SIZE;
                start_data_addr = data_base + parity_fail_addr * entry_size;    /*asic sram addr*/

                start_addr = g_mem_map_ctl->mem_map_table[sram_type].start_addr[chip_id];
                mem_mapping_addr = start_addr + parity_fail_addr * entry_size;  /*mapping memory addr*/

                DRV_IF_ERROR_RETURN(drv_chip_sram_read_for_parity_error(chip_id, start_data_addr, data, entry_size));
                mappingData = (uint32 *)mem_mapping_addr;
                break;
            default:
                break;
        }
    }
    else
    {
        /*process static sram*/
        data_base = DRV_TBL_GET_INFO(tbl_id).hw_data_base;
        entry_size = DRV_TBL_ENTRY_SIZE(tbl_id);

        start_data_addr = data_base + parity_fail_addr * entry_size;

        tmp_maping_type = _drv_humber_get_mapping_type_from_tblid(tbl_id);
        if(tmp_maping_type != DRV_MEM_MAPPING_TYPE_MAX)
        {
            start_addr = g_mem_map_ctl->mem_map_table[tmp_maping_type].start_addr[chip_id];
            mem_mapping_addr = start_addr + parity_fail_addr * entry_size;

            DRV_IF_ERROR_RETURN(drv_chip_sram_read_for_parity_error(chip_id, start_data_addr, data, entry_size));
            mappingData = (uint32 *)mem_mapping_addr;
        }
    }

    if(NULL == mappingData)
    {
        return DRV_E_INVALID_PARAM;
    }

    /*compare memory*/
    PARITY_LOCK;

    for(i=0; i<(entry_size/4); i++)
    {
        /*log*/
        info->asic_addr = start_data_addr;
        info->asic_data[i] = data[i];
        info->mapping_addr = mem_mapping_addr;
        info->mapping_data[i] = mappingData[i];

        if(mappingData[i] != data[i])
        {
            flag = 1;
        }
    }

    PARITY_UNLOCK;

    if(1 == flag)
    {
        /*resume*/
        DRV_IF_ERROR_RETURN(drv_chip_write_sram_for_parity_error(chip_id, start_data_addr, mappingData, entry_size));
    }
    else
    {
        info->is_same = TRUE;
    }

    return DRV_E_NONE;
}

/**
 @brief The function write table data to a mapping memory location
*/
void
drv_humber_show_parity_error_mapping_memory(uint32 tbl_id, uint32 index)
{
    uint32 data_base = 0, entry_size = 0, max_index_num = 0;
    uint32 length = 0;
    uint32 start_addr = 0;
    uint32 mem_mapping_addr = 0;
    uint8 type;
    uint8 chip_id = 0, chip_num = 1;
    uint8 i = 0;

    if(!g_mem_map_ctl)
    {
        DRV_DBG_INFO("\nERROR: parity error moudle is not enable.\n");
        return;
    }

    type = _drv_humber_get_mapping_type_from_tblid(tbl_id);
    if(DRV_MEM_MAPPING_TYPE_MAX == type)
    {
        DRV_DBG_INFO("\nERROR : table %d is no need to do parity error memory mapping.\n", tbl_id);
        return;
    }

    data_base = DRV_TBL_GET_INFO(tbl_id).hw_data_base;
    entry_size = DRV_TBL_ENTRY_SIZE(tbl_id);
    max_index_num = DRV_TBL_MAX_INDEX(tbl_id);
    drv_get_chipnum(&chip_num);

    if(index >= max_index_num)
    {
        DRV_DBG_INFO("\nERROR : table %d index %d is too large.\n", tbl_id, index);
        return;
    }

    PARITY_LOCK;

    for(chip_id = 0; chip_id < chip_num; chip_id ++)
    {
        start_addr = g_mem_map_ctl->mem_map_table[type].start_addr[chip_id];

        if(DRV_MEM_MAPPING_INT_16K_SRAM == type)    /*should merge 16K with 96K or 48K*/
        {
            mem_mapping_addr = data_base + index * entry_size - DRV_SRAM_ASSOCIATE_DATA_BASE + start_addr;
        }
        else if(DRV_MEM_MAPPING_EXT_256K_SRAM == type)
        {
            mem_mapping_addr = data_base + index * entry_size - DRV_EXT_SRAM_ASSOCIATE_DATA_BASE + start_addr;
        }
        else if((DRV_MEM_MAPPING_INT_48K_SRAM == type) || (DRV_MEM_MAPPING_INT_96K_SRAM == type))   /*for hash*/
        {
            mem_mapping_addr = data_base + index * entry_size - DRV_SRAM_HASH_KEY_BASE + start_addr;
        }
        else
        {
            mem_mapping_addr = start_addr + index * entry_size;
        }

        DRV_DBG_INFO("\nlchip %d table %d index %d parity error mapping memory:\n", chip_id, tbl_id, index);
        length = (entry_size >> 2);

        for (i = 0; i < length; i++)
        {
            DRV_DBG_INFO("  0x%.8x", *((uint32 *)(mem_mapping_addr + i*4)));
        }
        DRV_DBG_INFO("\n");
    }

    PARITY_UNLOCK;
}

/**
 @brief Init mapping memory
*/
int32
drv_humber_mem_mapping_init(drv_chip_parity_error_init_t* p_parity_error)
{
    uint8 i = 0;
    uint8 chip_id = 0;
    uint32 size = 0;
    uint32 start_addr = 0;
    uint32 table_id = 0;
    int32 ret = DRV_E_NONE;

    chip_id = p_parity_error->chip_id;

    if(!g_mem_map_ctl)
    {
        g_mem_map_ctl = mem_malloc(MEM_SYSTEM_MODULE, sizeof(mem_mapping_control_t));
        if (!g_mem_map_ctl)
        {
            return DRV_E_NO_MEMORY;
        }
        kal_memset((uint8*)g_mem_map_ctl, 0, sizeof(*g_mem_map_ctl));
    }

    ret = kal_mutex_create(&(g_mem_map_ctl->parity_mutex));
    if (ret || !(g_mem_map_ctl->parity_mutex))
    {
        return ret;
    }

    g_mem_map_ctl->is_hash_48ksize = p_parity_error->is_hash_48ksize;

    for(i = 0; i < DRV_MEM_MAPPING_TYPE_MAX; i ++)
    {
        table_id = _drv_humber_get_tblid_from_mapping_type(i);
        if(MAX_TBL_NUM != table_id)
        {
            size = drv_tbls_list[table_id].max_index_num * drv_tbls_list[table_id].entry_size;
            start_addr = (uint32)mem_malloc(MEM_SYSTEM_MODULE, size);
            if (!start_addr)
            {
                return DRV_E_NO_MEMORY;
            }
            kal_memset((uint8*)start_addr, 0, size);

            g_mem_map_ctl->mem_map_table[i].start_addr[chip_id] = start_addr;
            g_mem_map_ctl->mem_map_table[i].size[chip_id] = size;
        }
    }

    if(g_mem_map_ctl->is_hash_48ksize)
    {
        /*malloc 16K+96K=112K*/
        size = (16 + 96) * 1024 * DRV_PER_SRAM_ENTRY_SIZE;
        start_addr = (uint32)mem_malloc(MEM_SYSTEM_MODULE, size);
        if (!start_addr)
        {
            return DRV_E_NO_MEMORY;
        }
        kal_memset((uint8*)start_addr, 0, size);

        /*DRV_MEM_MAPPING_INT_16K_SRAM*/
        g_mem_map_ctl->mem_map_table[DRV_MEM_MAPPING_INT_16K_SRAM].start_addr[chip_id] = start_addr;
        g_mem_map_ctl->mem_map_table[DRV_MEM_MAPPING_INT_16K_SRAM].size[chip_id] = 16 * 1024 * DRV_PER_SRAM_ENTRY_SIZE;

        /*DRV_MEM_MAPPING_INT_96K_SRAM*/
        g_mem_map_ctl->mem_map_table[DRV_MEM_MAPPING_INT_96K_SRAM].start_addr[chip_id] =
            start_addr + 16 * 1024 * DRV_PER_SRAM_ENTRY_SIZE;
        g_mem_map_ctl->mem_map_table[DRV_MEM_MAPPING_INT_96K_SRAM].size[chip_id] = 96 * 1024 * DRV_PER_SRAM_ENTRY_SIZE;

        /*DRV_MEM_MAPPING_INT_48K_SRAM*/
        size = 48 * 1024 * DRV_PER_SRAM_ENTRY_SIZE;
        start_addr = (uint32)mem_malloc(MEM_SYSTEM_MODULE, size);
        if (!start_addr)
        {
            return DRV_E_NO_MEMORY;
        }
        kal_memset((uint8*)start_addr, 0xFF, size);     /*for hash, should init to 0xFF*/

        g_mem_map_ctl->mem_map_table[DRV_MEM_MAPPING_INT_48K_SRAM].start_addr[chip_id] = start_addr;
        g_mem_map_ctl->mem_map_table[DRV_MEM_MAPPING_INT_48K_SRAM].size[chip_id] = size;
    }
    else
    {
        /*malloc 16K+48K=64K*/
        size = (16 + 48) * 1024 * DRV_PER_SRAM_ENTRY_SIZE;
        start_addr = (uint32)mem_malloc(MEM_SYSTEM_MODULE, size);
        if (!start_addr)
        {
            return DRV_E_NO_MEMORY;
        }
        kal_memset((uint8*)start_addr, 0, size);

        /*DRV_MEM_MAPPING_INT_16K_SRAM*/
        g_mem_map_ctl->mem_map_table[DRV_MEM_MAPPING_INT_16K_SRAM].start_addr[chip_id] = start_addr;
        g_mem_map_ctl->mem_map_table[DRV_MEM_MAPPING_INT_16K_SRAM].size[chip_id] = 16 * 1024 * DRV_PER_SRAM_ENTRY_SIZE;

        /*DRV_MEM_MAPPING_INT_48K_SRAM*/
        g_mem_map_ctl->mem_map_table[DRV_MEM_MAPPING_INT_48K_SRAM].start_addr[chip_id] =
            start_addr + 16 * 1024 * DRV_PER_SRAM_ENTRY_SIZE;
        g_mem_map_ctl->mem_map_table[DRV_MEM_MAPPING_INT_48K_SRAM].size[chip_id] = 48 * 1024 * DRV_PER_SRAM_ENTRY_SIZE;

        /*DRV_MEM_MAPPING_INT_96K_SRAM*/
        size = 96 * 1024 * DRV_PER_SRAM_ENTRY_SIZE;
        start_addr = (uint32)mem_malloc(MEM_SYSTEM_MODULE, size);
        if (!start_addr)
        {
            return DRV_E_NO_MEMORY;
        }
        kal_memset((uint8*)start_addr, 0xFF, size);     /*for hash, should init to 0xFF*/

        g_mem_map_ctl->mem_map_table[DRV_MEM_MAPPING_INT_96K_SRAM].start_addr[chip_id] = start_addr;
        g_mem_map_ctl->mem_map_table[DRV_MEM_MAPPING_INT_96K_SRAM].size[chip_id] = size;
    }

    /*DRV_MEM_MAPPING_EXT_256K_SRAM*/
    if(g_mem_map_ctl->is_ext_sram_en)
    {
        size = 256 * 1024 * DRV_PER_SRAM_ENTRY_SIZE;
        start_addr = (uint32)mem_malloc(MEM_SYSTEM_MODULE, size);
        if (!start_addr)
        {
            return DRV_E_NO_MEMORY;
        }
        kal_memset((uint8*)start_addr, 0, size);

        g_mem_map_ctl->mem_map_table[DRV_MEM_MAPPING_EXT_256K_SRAM].start_addr[chip_id] = start_addr;
        g_mem_map_ctl->mem_map_table[DRV_MEM_MAPPING_EXT_256K_SRAM].size[chip_id] = size;
    }

    DRV_IF_ERROR_RETURN(_drv_humber_mem_mapping_datapath(chip_id));

    return DRV_E_NONE;
}
