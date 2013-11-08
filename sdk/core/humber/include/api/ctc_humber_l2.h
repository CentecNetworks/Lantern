/**
 @file ctc_humber_l2.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-10-28

 @version v2.0

  This file contains all the L2Uc/L2MC/Default entry APIs
*/


#ifndef _CTC_HUMBER_L2_H
#define _CTC_HUMBER_L2_H

#include "kal.h"
#include "ctc_l2.h"

/**********************************************************************************
                      Define API function interfaces
***********************************************************************************/
/**
 @addtogroup fdb FDB
 @{
*/

/**
 @brief Init fdb module

 @param[in]  l2 fdb global config information   flush_fdb_cnt_per_loop  default_entry_rsv_num

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2_fdb_init(void* l2_fdb_global_cfg);



/**
 @brief This function is to add a fdb entry

  @param[in] l2_addr       device-independent L2 unicast address

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2_add_fdb(ctc_l2_addr_t* l2_addr);

/**
 @brief This function is to remove a fdb entry

 @param[in] l2_addr      device-independent L2 unicast address

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2_remove_fdb(ctc_l2_addr_t* l2_addr);

/**
 @brief  This function is to delete fdb entry by index

 @param[in] index  fdb node index

 @param[out] l2_addr      device-independent L2 unicast address

 @return SDK_E_XXX

*/
extern int32
ctc_humber_l2_get_fdb_by_index(uint32 index,ctc_l2_addr_t* l2_addr);

/**
 @brief  This function is to delete fdb entry by index

 @param[in] index  fdb node index

 @return SDK_E_XXX

*/
extern int32
ctc_humber_l2_remove_fdb_by_index(uint32 index);


/**
 @brief This function is to add a fdb entry with nexthop ID

 @param[in] l2_addr        device-independent L2 unicast address,when packet will edited by nh_id,l2_addr.gport is invalid.

 @param[in] nhp_id         the specified nhp_id

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2_add_fdb_with_nexthop(ctc_l2_addr_t* l2_addr, uint32 nhp_id);

/**
 @brief This function is to get fdb count according to specified query condition

 @param[in] pQuery        query condition

 @return CTC_E_XXX

*/

extern int32
ctc_humber_l2_get_fdb_count(ctc_l2_fdb_query_t* pQuery);

/**
 @brief This function is to query fdb enery according to specified query condition

 @param[in] pQuery           query condition

 @param[in,out] query_rst  query results

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2_get_fdb_entry(ctc_l2_fdb_query_t* pQuery, ctc_l2_fdb_query_rst_t* query_rst);

/**
 @brief This function is to flush fdb entry by specified type(based on mac, port, fid) and specified flag(static,dynamic,all)

 @param[in] pFlush      flush FDB entries data structure

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2_fdb_flush(ctc_l2_fdb_flush_t* pFlush);


/**@} end of @addtogroup fdb FDB*/


/**
 @addtogroup l2mcast L2Mcast
 @{
*/

/**
 @brief This function is to create a layer2 multicast group

 @param[in] l2mc_addr     l2MC  data structure

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2mcast_add_addr(ctc_l2_mcast_addr_t* l2mc_addr);

/**
 @brief This function is to remove a layer2 multicast group

 @param[in] l2mc_addr     l2MC  data structure

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2mcast_remove_addr(ctc_l2_mcast_addr_t* l2mc_addr);

/**
 @brief This function is to add a member port into a existed multicast group

 @param[in] l2mc_addr     l2MC  data structure

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2mcast_add_member(ctc_l2_mcast_addr_t* l2mc_addr);

/**
 @brief This function is to remove a member port into a existed multicast group

 @param[in] l2mc_addr     l2MC  data structure
 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2mcast_remove_member(ctc_l2_mcast_addr_t* l2mc_addr);


/**
 @brief This function is to add a default entry

 @param[in] l2dflt_addr   point to ctc_l2dflt_addr_t

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2_add_default_entry(ctc_l2dflt_addr_t* l2dflt_addr);


/**
 @brief remove default entry

 @param[in] l2dflt_addr   point to ctc_l2dflt_addr_t

 @return CTC_E_XXX
*/
extern int32
ctc_humber_l2_remove_default_entry(ctc_l2dflt_addr_t* l2dflt_addr);



/**
 @brief add a port into default entry

 @param[in] l2dflt_addr   point to ctc_l2dflt_addr_t

 @return CTC_E_XXX
*/
extern int32
ctc_humber_l2_add_port_to_default_entry(ctc_l2dflt_addr_t* l2dflt_addr);


/**
 @brief remove a port from default entry

 @param[in] l2dflt_addr   point to ctc_l2dflt_addr_t

 @return CTC_E_XXX
*/
extern int32
ctc_humber_l2_remove_port_from_default_entry(ctc_l2dflt_addr_t* l2dflt_addr);


/**
 @brief add a port into default entry

 @param[in] l2dflt_addr   point to ctc_l2dflt_addr_t

 @return CTC_E_XXX
*/
extern int32
ctc_humber_l2_set_default_entry_features(ctc_l2dflt_addr_t* l2dflt_addr);


/**
 @brief add a port into default entry

 @param[in] l2dflt_addr   point to ctc_l2dflt_addr_t

 @return CTC_E_XXX
*/
extern int32
ctc_humber_l2_get_default_entry_features(ctc_l2dflt_addr_t* l2dflt_addr);


/**@} end of @addtogroup l2mcast L2Mcast*/



#endif /*end of _CTC_HUMBER_L2_H*/



