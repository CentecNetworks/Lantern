/**
 @file ctc_humber_mirror.c

 @date 2009-10-21

 @version v2.0

 The file contains all mirror APIs for customers
*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/

#include "ctc_error.h"
#include "ctc_humber_mirror.h"
#include "sys_humber_mirror.h"

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
 @brief This function is to initialize the mirror module
 */
int32 ctc_humber_mirror_init(void* mirror_global_cfg)
{
    CTC_ERROR_RETURN(sys_humber_mirror_init());

    return CTC_E_NONE;
}

/**
 @brief This function is to set mirror enable on port
*/
int32
ctc_humber_mirror_set_port_enable(uint16 gport, ctc_direction_t dir, uint8 session_id)
{
    CTC_ERROR_RETURN(sys_humber_mirror_set_port_enable(gport, dir, session_id));

    return CTC_E_NONE;
}

/**
 @brief This function is to set mirror disable on port
*/
int32
ctc_humber_mirror_set_port_disable(uint16 gport, ctc_direction_t dir)
{
    CTC_ERROR_RETURN(sys_humber_mirror_set_port_disable(gport, dir));

    return CTC_E_NONE;
}

/**
 @brief This function is to get the information of port mirror
*/
int32
ctc_humber_mirror_get_port_info(uint16 gport, ctc_direction_t dir, bool *enable, uint8 *session_id)
{
    CTC_ERROR_RETURN(sys_humber_mirror_get_port_info(gport, dir, enable, session_id));

    return CTC_E_NONE;
}

/**
 @brief This function is to set enable mirror on vlan
*/
int32
ctc_humber_mirror_set_vlan_enable(uint16 vlan_id, ctc_direction_t dir, uint8 session_id)
{
    CTC_ERROR_RETURN(sys_humber_mirror_set_vlan_enable(vlan_id, dir, session_id));

    return CTC_E_NONE;
}

/**
 @brief This function is to set vlan disable to mirror
*/
int32
ctc_humber_mirror_set_vlan_disable(uint16 vlan_id, ctc_direction_t dir)
{
    CTC_ERROR_RETURN(sys_humber_mirror_set_vlan_disable(vlan_id, dir));

    return CTC_E_NONE;
}

/**
 @brief This function is to get the information of vlan mirror
*/
int32
ctc_humber_mirror_get_vlan_info(uint16 vlan_id, ctc_direction_t dir, bool *enable, uint8 *session_id)
{
    CTC_ERROR_RETURN(sys_humber_mirror_get_vlan_info(vlan_id, dir, enable, session_id));

    return CTC_E_NONE;
}

/**
 @brief This function is to set local mirror destination port
*/
int32
ctc_humber_mirror_set_dest(ctc_mirror_dest_t *mirror)
{
    CTC_ERROR_RETURN(sys_humber_mirror_set_dest(mirror));

    return CTC_E_NONE;
}

/**
 @brief This function is to set remote mirror destination port
*/
int32
ctc_humber_mirror_rspan_set_dest(ctc_mirror_dest_t *mirror, uint32 nh_id)
{
    CTC_ERROR_RETURN(sys_humber_mirror_rspan_set_dest(mirror, nh_id));

    return CTC_E_NONE;
}

/**
 @brief Discard some special mirrored packet if enable
*/
int32
ctc_humber_mirror_rspan_escape_en(bool enable)
{
    CTC_ERROR_RETURN(sys_humber_mirror_rspan_escape_en(enable));

    return CTC_E_NONE;
}

/**
 @brief Config mac info indicat the mirrored packet is special
*/
int32
ctc_humber_mirror_rspan_escape_mac(ctc_mirror_rspan_escape_t escape)
{
    CTC_ERROR_RETURN(sys_humber_mirror_rspan_escape_mac(escape));

    return CTC_E_NONE;
}


/**
 @brief This function is to remove mirror destination port
*/
int32
ctc_humber_mirror_unset_dest(ctc_mirror_dest_t *mirror)
{
    CTC_ERROR_RETURN(sys_humber_mirror_unset_dest(mirror));

    return CTC_E_NONE;
}

/**
 @brief This function is to set that whether packet is mirrored when it has been droped
*/
int32
ctc_humber_mirror_set_mirror_discard(ctc_direction_t dir, ctc_mirror_discard_t discard_flag, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_mirror_set_mirror_discard(dir, discard_flag, enable));

    return CTC_E_NONE;
}

/**
 @brief This function is to get that whether packet is mirrored when it has been droped
*/
int32
ctc_humber_mirror_get_mirror_discard(ctc_direction_t dir, ctc_mirror_discard_t discard_flag, bool* enable)
{
    CTC_ERROR_RETURN(sys_humber_mirror_get_mirror_discard(dir, discard_flag, enable));
    return CTC_E_NONE;
}


