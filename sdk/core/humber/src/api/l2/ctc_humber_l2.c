/**
 @file ctc_humber_l2.c

 @date 2009-11-2

 @version v2.0


*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_humber_l2.h"
#include "sys_humber_l2_fdb.h"
#include "sys_humber_nexthop_api.h"
/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/
/****************************************************************************
 *
* Function
*
*****************************************************************************/

/**
 @brief init fdb module

 @param[]

 @return CTC_E_XXX

*/
int32
ctc_humber_l2_fdb_init(void* l2_fdb_global_cfg )
{
    CTC_ERROR_RETURN(sys_humber_l2_fdb_init(l2_fdb_global_cfg));
    return CTC_E_NONE;
}


/**
 @brief    Ddd a fdb entry

 @param[in] l2_addr        sys layer maintain the data from the ctc layer

 @return CTC_E_XXX

*/
int32
ctc_humber_l2_add_fdb(ctc_l2_addr_t* l2_addr)
{
    CTC_ERROR_RETURN(sys_humber_l2_add_fdb(l2_addr));
    return CTC_E_NONE;
}

/**
 @brief Delete a fdb entry

 @param[in] l2_addr         sys layer maintain the data from the ctc layer

 @return CTC_E_XXX

*/
int32
ctc_humber_l2_remove_fdb(ctc_l2_addr_t* l2_addr )
{
    CTC_ERROR_RETURN(sys_humber_l2_remove_fdb(l2_addr));
    return CTC_E_NONE;
}

/**
 @brief  This function is to delete fdb entry by index

 @param[in] index  fdb node index

 @param[out] l2_addr      device-independent L2 unicast address

 @return SDK_E_XXX

*/
extern int32
ctc_humber_l2_get_fdb_by_index(uint32 index,ctc_l2_addr_t* l2_addr)
{
    CTC_ERROR_RETURN(sys_humber_l2_get_fdb_by_index(index, l2_addr));
    return CTC_E_NONE;
}

/**
 @brief Delete a fdb entry by index

 @param[in] index    fdb node index

 @return CTC_E_XXX

*/
int32
ctc_humber_l2_remove_fdb_by_index(uint32 index)
{
    CTC_ERROR_RETURN(sys_humber_l2_remove_fdb_by_index(index));
    return CTC_E_NONE;
}

/**
 @brief Add a fdb entry with nhp

 @param[in] l2_addr         sys layer maintain the data from the ctc layer

 @param[in] nhp_id           the specified nhp_id

 @return CTC_E_XXX

*/
int32
ctc_humber_l2_add_fdb_with_nexthop(ctc_l2_addr_t* l2_addr, uint32 nhp_id)
{
    CTC_ERROR_RETURN(sys_humber_l2_add_fdb_with_nexthop(l2_addr, nhp_id));
    return CTC_E_NONE;
}


/**
 @brief Query fdb count according to specified query condition

 @param[in] pQuery                query condition

 @return CTC_E_XXX

*/
int32
ctc_humber_l2_get_fdb_count(ctc_l2_fdb_query_t* pQuery)
{
    CTC_ERROR_RETURN(sys_humber_l2_get_fdb_count(pQuery));
    return CTC_E_NONE;
}

/**
 @brief Query fdb enery according to specified query condition

 @param[in] pQuery                 query condition

 @param[in/out] query_rst      query results

 @return CTC_E_XXX

*/
int32
ctc_humber_l2_get_fdb_entry(ctc_l2_fdb_query_t* pQuery, ctc_l2_fdb_query_rst_t* query_rst)
{
  CTC_ERROR_RETURN(sys_humber_l2_get_fdb_entry( pQuery,query_rst));
   return CTC_E_NONE;
}

/**
 @brief Flush fdb entry by specified flush type

 @param[in] pFlush  flush type

 @return CTC_E_XXX

*/
int32
ctc_humber_l2_fdb_flush(ctc_l2_fdb_flush_t* pFlush)
{
    CTC_ERROR_RETURN(sys_humber_l2_fdb_flush(pFlush));
    return CTC_E_NONE;
}

/**
 @brief Add an entry in the multicast table

 @param[in] l2mc_addr     L2 multicast address

 @return CTC_E_XXX

*/
int32
ctc_humber_l2mcast_add_addr(ctc_l2_mcast_addr_t* l2mc_addr)
{
    CTC_ERROR_RETURN(sys_humber_l2mcast_add_addr(l2mc_addr));
    return CTC_E_NONE;
}

/**
 @brief Remove an entry in the multicast table

 @param[in] l2mc_addr     L2 multicast address

 @return CTC_E_XXX

*/
extern  int32
ctc_humber_l2mcast_remove_addr(ctc_l2_mcast_addr_t* l2mc_addr)
{
    CTC_ERROR_RETURN(sys_humber_l2mcast_remove_addr(l2mc_addr));
    return CTC_E_NONE;
}

/**
 @brief Add a given port/port list to  a existed multicast group

 @param[in] l2mc_addr     L2 multicast address

 @return CTC_E_XXX

*/
int32
ctc_humber_l2mcast_add_member(ctc_l2_mcast_addr_t* l2mc_addr)
{
    CTC_ERROR_RETURN(sys_humber_l2mcast_add_member(l2mc_addr));
    return CTC_E_NONE;
}

/**
 @brief Remove a given port/port list to  a existed multicast group

 @param[in] l2mc_addr     L2 multicast address

 @return CTC_E_XXX

*/
int32
ctc_humber_l2mcast_remove_member(ctc_l2_mcast_addr_t* l2mc_addr)
{
    CTC_ERROR_RETURN(sys_humber_l2mcast_remove_member(l2mc_addr));
    return CTC_E_NONE;

}

/**
 @brief add a default entry

 @param[in] l2dflt_addr   point to ctc_l2dflt_addr_t

 @return CTC_E_XXX

*/
int32
ctc_humber_l2_add_default_entry(ctc_l2dflt_addr_t* l2dflt_addr)
{
    CTC_ERROR_RETURN(sys_humber_l2_add_default_entry(l2dflt_addr));
    return CTC_E_NONE;
}

/**
 @brief remove default entry

 @param[in] l2dflt_addr   point to ctc_l2dflt_addr_t

 @return CTC_E_XXX
*/
int32
ctc_humber_l2_remove_default_entry(ctc_l2dflt_addr_t* l2dflt_addr)
{
    CTC_ERROR_RETURN(sys_humber_l2_remove_default_entry(l2dflt_addr));
    return CTC_E_NONE;
}

/**
 @brief add a port into default entry

 @param[in] l2dflt_addr   point to ctc_l2dflt_addr_t

 @return CTC_E_XXX
*/
int32
ctc_humber_l2_add_port_to_default_entry(ctc_l2dflt_addr_t* l2dflt_addr)
{
    CTC_ERROR_RETURN(sys_humber_l2_add_port_to_default_entry(l2dflt_addr));
    return CTC_E_NONE;
}

/**
 @brief remove a port from default entry

 @param[in] l2dflt_addr   point to ctc_l2dflt_addr_t

 @return CTC_E_XXX
*/
int32
ctc_humber_l2_remove_port_from_default_entry(ctc_l2dflt_addr_t* l2dflt_addr)
{
    CTC_ERROR_RETURN(sys_humber_l2_remove_port_from_default_entry(l2dflt_addr));
    return CTC_E_NONE;
}

/**
 @brief add a port into default entry

 @param[in] l2dflt_addr   point to ctc_l2dflt_addr_t

 @return CTC_E_XXX
*/
int32
ctc_humber_l2_set_default_entry_features(ctc_l2dflt_addr_t* l2dflt_addr)
{
    CTC_ERROR_RETURN(sys_humber_l2_set_default_entry_features(l2dflt_addr));
    return CTC_E_NONE;
}

/**
 @brief add a port into default entry

 @param[in] l2dflt_addr   point to ctc_l2dflt_addr_t

 @return CTC_E_XXX
*/
int32
ctc_humber_l2_get_default_entry_features(ctc_l2dflt_addr_t* l2dflt_addr)
{
    CTC_ERROR_RETURN(sys_humber_l2_get_default_entry_features(l2dflt_addr));
    return CTC_E_NONE;
}


