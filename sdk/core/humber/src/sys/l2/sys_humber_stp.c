/**
 @file

 @date 2009-10-20

 @version v2.0

*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_debug.h"
#include "drv_humber_lib.h"
#include "ctc_l2.h"
#include "sys_humber_chip.h"
#include "sys_humber_stp.h"
#include "sys_humber_vlan.h"


/**
 @brief initialize the stp module
*/

/**
 @brief   the function is to clear all instances belonged to one port
*/

int32
sys_humber_stp_clear_all_inst_state( uint16 gport)
{
    uint32 index   = 0;
    uint32 state   = 0;
    uint32 cmd    =0;
    uint32 loop    = 0;
    uint8 shift     = 0;
    int32 ret        = 0;
    uint8 lchip     = 0;
    uint8 lport     = 0;

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);


   shift = SYS_HUMBER_STP_STATE_SHIFT;
   index = (lport << shift);

    cmd = DRV_IOW( IOC_TABLE, STP_STATE_RAM, DRV_ENTRY_FLAG);
    for (loop = 0; loop < SYS_STP_REG_SIZE; loop++, index++)
    {
        ret = drv_tbl_ioctl(lchip, index, cmd, &state);
    }

    return CTC_E_NONE;
}



/**
     @brief the function is to set stp id (MSTI instance) for vlan
*/

int32
sys_humber_stp_set_vlan_stpid(uint16 vlan_id,  uint8 stpid)
{
    sys_vlan_info_t vlan_info;

    SYS_STP_ID_VALID_CHECK(stpid);

    SYS_STP_DBG_FUNC();
    SYS_STP_DBG_INFO( "vlan_id :%d  stpid:%d \n", vlan_id, stpid);
    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_set_stpid(&vlan_info, stpid));
     return CTC_E_NONE;

}

/**
     @brief the function is to set stp id (MSTI instance) for vlan
*/

int32
sys_humber_stp_get_vlan_stpid(uint16 vlan_id,  uint8 *stpid)
{
    sys_vlan_info_t vlan_info;

    CTC_PTR_VALID_CHECK(stpid);

    SYS_STP_DBG_FUNC();
    SYS_STP_DBG_INFO("vlan_id :%d\n", vlan_id);
    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_get_stpid(&vlan_info, stpid));
    return CTC_E_NONE;
}

/**
 @brief    the function is to update the state
*/
int32
sys_humber_stp_set_state(uint16 gport,  uint8 stpid, uint8 state)
{
    uint8 lchip         = 0;
    uint8 lport         = 0;
    uint8 shift         = 0;
    uint32 index        = 0;
    uint32 cmd          = 0;
    uint32 field_val    = 0;
    uint32 field_id     = STP_STATE_RAM_STP_STATE0;

    SYS_STP_DBG_INFO("Set gport:%d stp id:%d state:%d\n", gport, stpid, state);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    SYS_STP_ID_VALID_CHECK(stpid);

    shift = SYS_HUMBER_STP_STATE_SHIFT;
    index = (lport << shift) + ((stpid >> 4) & 0x7);

    field_id += stpid & 0xF;
    cmd = DRV_IOW( IOC_TABLE, STP_STATE_RAM, field_id);

    field_val = state & 0x3;
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &field_val));

    return CTC_E_NONE;
}

/**
 @brief the function is to get the instance of the port
*/
int32
sys_humber_stp_get_state(uint16 gport, uint8 stpid, uint8* state)
{
    uint8 shift         = 0;
    uint8 lchip         = 0;
    uint8 lport         = 0;
    uint32 cmd          = 0;
    uint32 index        = 0;
    uint32 field_val    = 0;
    uint32 field_id     = STP_STATE_RAM_STP_STATE0;

    SYS_STP_DBG_INFO("Get gport%d stp id:%d state\n", gport, stpid);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    SYS_STP_ID_VALID_CHECK(stpid);
    CTC_PTR_VALID_CHECK(state);

    shift = SYS_HUMBER_STP_STATE_SHIFT;
    index = (lport << shift) + ((stpid >> 4) & 0x7);
    field_id += stpid & 0xF;

    cmd = DRV_IOR( IOC_TABLE, STP_STATE_RAM, field_id);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &field_val));

    *state = field_val & 0x3;

    return CTC_E_NONE;
}



