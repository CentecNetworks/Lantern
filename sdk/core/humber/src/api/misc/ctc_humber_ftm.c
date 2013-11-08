/**
 @file ctc_humber_alloc.c

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-11-11

 @version v2.0

 memory alloc init
*/

#include "kal.h"
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_ftm.h"
#include "ctc_debug.h"
#include "sys_humber_ftm.h"

/**
 @brief
 @return CTC_E_XXX

*/
int32
ctc_humber_ftm_show_alloc_info(void)
{
    CTC_ERROR_RETURN(sys_humber_show_alloc_info());

    return CTC_E_NONE;
}

/**
 @brief

 @param[in] ctc_profile_info  allocation profile information

 @return CTC_E_XXX

*/
int32
ctc_humber_ftm_set_default_profile(ctc_ftm_profile_info_t *ctc_profile_info)
{
    ctc_ftm_key_info_t *int_tcam_info;
    uint16                        index = 0;
    CTC_PTR_VALID_CHECK(ctc_profile_info);

    /* set hardware information*/
    ctc_profile_info->table_size[CTC_FTM_TBL_TYPE_ASS_DYN_SRAM]     = 112 * 1024;
    ctc_profile_info->table_size[CTC_FTM_TBL_TYPE_HASH]             = 48 * 1024;
    ctc_profile_info->table_size[CTC_FTM_TBL_TYPE_MPLS]             = 2 * 1024;
    ctc_profile_info->table_size[CTC_FTM_TBL_TYPE_GLB_MET]          = 2 * 1024;
    ctc_profile_info->table_size[CTC_FTM_TBL_TYPE_GLB_NH]           = 2 * 1024;
    ctc_profile_info->table_size[CTC_FTM_TBL_TYPE_LOCAL_NH]         = 2 * 1024;
    ctc_profile_info->table_size[CTC_FTM_TBL_TYPE_L2EDIT]           = 2 * 1024;
    ctc_profile_info->table_size[CTC_FTM_TBL_TYPE_L3EDIT]           = 2 * 1024;
    ctc_profile_info->table_size[CTC_FTM_TBL_TYPE_IP_TUNNEL]         = 0;

    ctc_profile_info->table_size[CTC_FTM_TBL_TYPE_EXT_TCAM_TYPE]    = CTC_FTM_TCAM_TYPE_NL9K;
    ctc_profile_info->table_size[CTC_FTM_TBL_TYPE_EXT_TCAM_SIZE]    = 256 * 1024;
    ctc_profile_info->table_size[CTC_FTM_TBL_TYPE_EXT_SRAM_SIZE]    = 0;

    ctc_profile_info->table_size[CTC_FTM_TBL_TYPE_FLOW_PLOCIER]     = 4*1024;
    ctc_profile_info->table_size[CTC_FTM_TBL_TYPE_FWD_STATS]        = 4*1024;

    /* set lookup behavior */
    CTC_SET_FLAG(ctc_profile_info->flag, CTC_FTM_FLAG_ACL_QOS_DUAL_LKUP);
    CTC_SET_FLAG(ctc_profile_info->flag, CTC_FTM_FLAG_IPUC_SA_EN);
    CTC_UNSET_FLAG(ctc_profile_info->flag, CTC_FTM_FLAG_MPLS_IN_EXT_SRAM);
    CTC_UNSET_FLAG(ctc_profile_info->flag, CTC_FTM_FLAG_MET_IN_EXT_SRAM);
    CTC_UNSET_FLAG(ctc_profile_info->flag, CTC_FTM_FLAG_NH_IN_EXT_SRAM);
    CTC_UNSET_FLAG(ctc_profile_info->flag, CTC_FTM_FLAG_L2EDIT_IN_EXT_SRAM);
    CTC_UNSET_FLAG(ctc_profile_info->flag, CTC_FTM_FLAG_L3EDIT_IN_EXT_SRAM);
    /*external QDR*/
    CTC_UNSET_FLAG(ctc_profile_info->flag, CTC_FTM_FLAG_EXT_QDR_EN);

    int_tcam_info   = ctc_profile_info->key_info;

    /* Init profile info to be zero */
    kal_memset(int_tcam_info, 0, MAX_CTC_FTM_KEY_TYPE_SIZE * sizeof(ctc_ftm_key_info_t));

    /* Init Interl TCAM DsMacKey, DsIpv4UcastKey */
    /* Beaware that DsIpv6UcastKey needs 384 entries at least */

    /****** Internal Tcam Allocation Schedule ******/
    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_ACL_IPV6;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_320_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 256;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_ACL_MAC_IPV4;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_320_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 512;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_IPV4_UCAST;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_80_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 1024;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_IPV4_MCAST;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_160_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 128;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_IPV4_NAT;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_80_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 256;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_IPV4_PBR;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_160_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 128;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_IPV6_UCAST;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_160_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 512;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_USERID_MAC;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_80_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 1024;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_USERID_VLAN;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_80_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 1024;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_IPV6_MCAST;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_320_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 256;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_IPV6_NAT;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_320_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 0;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_IPV6_PBR;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_320_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 0;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_FIB;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_80_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 4096;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_QOS_IPV6;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_320_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 128;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_QOS_MAC_IPV4;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_320_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 256;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_USERID_IPV4;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_320_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 256;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_USERID_IPV6;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_320_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 128;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_OAM;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_80_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_INT_TCAM;
    ctc_profile_info->key_info[index].max_key_index = 256;
    index ++;

    /* Init Hash DsMacKey, DsIpv4UcastKey */
    /* size of DsMacHashKey actually is 72bits per entry */
    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_FIB;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_80_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_HASH;
    ctc_profile_info->key_info[index].max_key_index = 32 * 1024;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_IPV4_UCAST;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_80_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_HASH;
    ctc_profile_info->key_info[index].max_key_index = 8 * 1024;
    index ++;

    ctc_profile_info->key_info[index].key_id = CTC_FTM_KEY_TYPE_IPV6_UCAST;
    ctc_profile_info->key_info[index].key_size = CTC_FTM_KEY_SIZE_160_BIT;
    ctc_profile_info->key_info[index].key_media = CTC_FTM_HASH;
    ctc_profile_info->key_info[index].max_key_index = 4 * 1024;
    index ++;

    ctc_profile_info->key_info_size = index;

    return CTC_E_NONE;
}

/**
 @brief

 @param[in] ctc_profile_info  allocation profile information

 @return CTC_E_XXX

*/
int32
ctc_humber_ftm_mem_alloc(ctc_ftm_profile_info_t *ctc_profile_info)
{

    CTC_ERROR_RETURN(sys_humber_mem_alloc(ctc_profile_info));

    return CTC_E_NONE;

}
