/**
 @file ctc_humber_nexthop.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-11-18

 @version v2.0

  This file contains  the nexthop APIs for customer
*/


#ifndef _CTC_HUMBER_NH_H
#define _CTC_HUMBER_NH_H

#include "kal.h"
#include "ctc_stats.h"

/**********************************************************************************
                      Define API function interfaces
***********************************************************************************/
/**
 @addtogroup nexthop NEXTHOP
 @{
*/

/**
 @brief SDK nexthop module initilize

 @param[in] nh_cfg  nexthop module global config

 @return CTC_E_XXX

*/
extern int32
ctc_humber_nexthop_init(ctc_nh_global_cfg_t* nh_cfg);



/**********************************************************************************
                      Define l2 ucast nexthop functions
***********************************************************************************/
/**
 @brief This function is to create normal ucast bridge nexthop entry

 @param[in] gport   global port id

 @param[in] nh_type   bridge unicast nexthop sub type to create

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2_create_ucast_nh(uint16 gport ,  ctc_nh_param_brguc_sub_type_t nh_type);

/**
 @brief This function is to delete normal ucast bridge nexthop entry

 @param[in] gport   global port id

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2_delete_ucast_nh(uint16 gport);


/**********************************************************************************
                      Define ipuc nexthop functions
***********************************************************************************/
/**
 @brief Create IPUC nexthop

 @param[in] nhid nexthop ID to be created

 @param[in] p_nh_param nexthop parameter used to create this ipuc nexthop

 @return CTC_E_XXX

*/
extern int32
ctc_humber_ipuc_nh_create(uint32 nhid, ctc_ip_nh_param_t* p_nh_param);

/**
 @brief Remove IPUC nexthop

 @param[in] nhid Nexthop ID to be removed

 @return CTC_E_XXX

*/
extern int32
ctc_humber_ipuc_nh_remove(uint32 nhid);

/**
 @brief Update IPUC nexthop

 @param[in] nhid nexthop ID to be updated

 @param[in] p_nh_param nexthop parameter used to update this ipuc nexthop

 @return CTC_E_XXX

*/
extern int32
ctc_humber_ipuc_nh_update(uint32 nhid, ctc_ip_nh_param_t* p_nh_param);


/**********************************************************************************
                      Define mpls nexthop functions
***********************************************************************************/

/**
 @brief Create a mpls nexthop

 @param[in] nhid nexthop ID to be created

 @param[in] p_nh_param   nexthop parameter used to create this nexthop

 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_nh_create(uint32 nhid, ctc_mpls_nexthop_param_t* p_nh_param);

/**
 @brief Remove mpls nexthop

 @param[in] nhid nexthop ID to be removed

 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_nh_remove(uint32 nhid);

/**
 @brief Update a mpls unresolved nexthop to forwarded mpls push nexthop

 @param[in] nhid nexthop ID to be updated

 @param[in] p_nh_param nexthop parameter used to update this nexthop

 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_nh_update(uint32 nhid, ctc_mpls_nexthop_param_t* p_nh_param);

/**********************************************************************************
                      Define iloop nexthop functions
***********************************************************************************/
/**
 @brief Create a ipe loopback nexthop

 @param[in] nhid nexthop ID to be created

 @param[in] p_nh_param loopback nexthop parameters

 @return CTC_E_XXX

*/
extern int32
ctc_humber_iloop_nh_create(uint32 nhid, ctc_loopback_nexthop_param_t* p_nh_param);

/**
 @brief Remove ipe loopback nexthop

 @param[in] nhid nexthop ID to be removed

 @return CTC_E_XXX

*/
extern int32
ctc_humber_iloop_nh_remove(uint32 nhid);

/**********************************************************************************
                      Define rspan(remote mirror) nexthop functions
***********************************************************************************/
/**
 @brief Create a rspan(remote mirror) nexthop

 @param[in] nhid nexthop ID to be created

 @param[in] dsnh_offset dsNexthop offset used by this mpls nexthop

 @param[in] p_nh_param remote mirror nexthop parameters

 @return CTC_E_XXX

*/
extern int32
ctc_humber_rspan_nh_create(uint32 nhid, uint32 dsnh_offset,ctc_rspan_nexthop_param_t* p_nh_param);

/**
 @brief Remove rspan(remote mirror) nexthop

 @param[in] nhid nexthop ID to be removed

 @return CTC_E_XXX

*/
extern int32
ctc_humber_rspan_nh_remove(uint32 nhid);


/**********************************************************************************
                      Define ECMP nexthop functions
***********************************************************************************/
/**
 @brief Create a ECMP nexthop

 @param[in] pdata Create data

 @return CTC_E_XXX

*/
extern int32
ctc_humber_ecmp_nh_create(ctc_nh_ecmp_creat_data_t* pdata);

/**
 @brief Delete a ECMP nexthop

 @param[in] nhid nexthop ID of ECMP to be removed

 @return CTC_E_XXX

*/
extern int32
ctc_humber_ecmp_nh_delete(uint32 nhid);

/**
 @brief Update a ECMP nexthop

 @param[in] pdata Update data

 @return CTC_E_XXX

*/
extern int32
ctc_humber_ecmp_nh_update(ctc_nh_ecmp_update_data_t* pdata);

/**********************************************************************************
                      Define advanced vlan/APS  nexthop functions
***********************************************************************************/

/**
 @brief Get ucast nhid by type

 @param[in] gport global port of the system

 @param[in] nh_type the type of ucast nexthop

 @param[in] nhid nexthop ID to get

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2_get_ucast_nh(uint16 gport, ctc_nh_param_brguc_sub_type_t nh_type, uint32* nhid);

/**
 @brief The function is to create Egress Vlan Editing nexthop or APS Egress Vlan Editing nexthop

 @param[in] nhid nexthop ID to be created

 @param[in] p_nh_param  nexthop parameter used to create this nexthop

 @return CTC_E_XXX

*/
extern int32
ctc_humber_egress_vlan_edit_create(uint32 nhid, ctc_vlan_edit_nh_param_t* p_nh_param);

/**
 @brief The function is to remove Egress Vlan Editing nexthop or APS Egress Vlan Editing nexthop

 @param[in] nhid            Egress vlan Editing nexthop id or APS Egress vlan Editing nexthop id

 @return CTC_E_XXX

*/
extern int32
ctc_humber_egress_vlan_edit_remove(uint32 nhid);


/**
 @brief The function is to create flexible nexthop

 @param[in] nhid nexthop ID to be created

 @param[in] p_nh_param  nexthop parameter used to create this nexthop

 @return CTC_E_XXX

*/
extern int32
ctc_humber_flex_nh_create(uint32 nhid, ctc_flex_nh_param_t* p_nh_param);

/**
 @brief The function is to remove Eflexible nexthop

 @param[in] nhid     nexthop ID to be created

 @return CTC_E_XXX

*/
extern int32
ctc_humber_flex_nh_remove(uint32 nhid);

/**********************************************************************************
                      Define stats functions in nexthop
***********************************************************************************/

/**
 @brief The function is to add stats

 @param[in] nhid            stats nexthop id

 @return CTC_E_XXX

 */
extern int32
ctc_humber_nh_add_stats(uint32 nhid);

/**
 @brief The function is to delete stats

 @param[in] nhid            stats nexthop id

 @return CTC_E_XXX

 */
extern int32
ctc_humber_nh_del_stats(uint32 nhid);

/**
 @brief The function is to get stats

 @param[in] nhid            stats nexthop id

 @param[out] p_stats        stats data to be get

 @return CTC_E_XXX

 */
extern int32
ctc_humber_nh_get_stats(uint32 nhid, ctc_stats_basic_t* p_stats);

/**
 @brief The function is to reset stats

 @param[in] nhid            stats nexthop id

 @return CTC_E_XXX

 */
extern int32
ctc_humber_nh_reset_stats(uint32 nhid);

/**
 @brief This function is to create mcast nexthop

 @param[in] nhid   nexthop ID to be created

 @param[in] p_nh_mcast_group   nexthop parameter used to create this mcast nexthop

 @return CTC_E_XXX
 */
extern int32
ctc_humber_mcast_nh_create(uint32 nhid, ctc_mcast_nh_param_group_t* p_nh_mcast_group);

/**
 @brief This function is to delete mcast nexthop

 @param[in] nhid   nexthopid

 @return CTC_E_XXX
 */
extern int32
ctc_humber_mcast_nh_delete(uint32 nhid);

/**
 @brief This function is to update mcast nexthop

 @param[in] nhid   nexthop ID

 @param[in] p_nh_mcast_group,  nexthop parameter used to add/remove  mcast member

 @return CTC_E_XXX
 */
extern int32
ctc_humber_mcast_nh_update(uint32 nhid, ctc_mcast_nh_param_group_t* p_nh_mcast_group);
/**@} end of @addtogroup nexthop NEXTHOP*/

#endif /*_CTC_HUMBER_NH_H*/

