/**
 @file ctc_humber_learning_aging.c

 @date 2010-3-16

 @version v2.0

---file comments----
*/

/****************************************************************************
*
* Header Files
*
****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"

#include "ctc_learning_aging.h"
#include "sys_humber_learning_aging.h"


/***************************************
 Learning Module's HUMBER API Interfaces
*****************************************/

extern int32
ctc_humber_set_learning_action(ctc_learning_action_info_t* p_learning_action)
{
    CTC_ERROR_RETURN(sys_humber_set_learning_action(p_learning_action));
    return CTC_E_NONE;
}


extern int32
ctc_humber_get_learning_action(ctc_learning_action_info_t* p_learning_action)
{
    CTC_ERROR_RETURN(sys_humber_get_learning_action(p_learning_action));
    return CTC_E_NONE;
}

 int32
ctc_humber_learning_get_cache_entry_valid_bitmap(uint8 lchip, uint16* entry_vld_bitmap)
{
    CTC_ERROR_RETURN(sys_humber_learning_get_cache_entry_valid_bitmap(lchip, entry_vld_bitmap));
    return CTC_E_NONE;
}


extern int32
ctc_humber_learning_clear_learning_cache(uint8 lchip, uint16 entry_vld_bitmap)
{
    CTC_ERROR_RETURN(sys_humber_learning_clear_learning_cache(lchip, entry_vld_bitmap));
    return CTC_E_NONE;
}

 int32
ctc_humber_learning_read_learning_cache(uint8 lchip,
                                        uint16 entry_vld_bitmap,
                                        ctc_learning_cache_t* l2_lc)
{
    CTC_ERROR_RETURN(sys_humber_learning_read_learning_cache(lchip, entry_vld_bitmap, l2_lc));
    return CTC_E_NONE;
}


/***************************************
 Aging Module's HUMBER API Interfaces
*****************************************/


int32
ctc_humber_aging_set_property(ctc_aging_prop_t aging_prop, uint32 value)
{
    uint8 value_8 = 0;
    uint32 value_32 = 0;
    bool value_b = FALSE;

    switch( aging_prop )
    {
        case CTC_AGING_PROP_FIFO_THRESHOLD :
            value_8 = value & 0xFF;
            CTC_ERROR_RETURN(sys_humber_aging_set_fifo_threshold(value_8));
            break;
        case  CTC_AGING_PROP_INTERVAL :
            value_32 = value ;
            CTC_ERROR_RETURN(sys_humber_aging_set_aging_interval(value_32));
            break;
        case CTC_AGING_PROP_STOP_SCAN_TIMER_EXPIRED :
            if( 0 != value )
            {
                value_b = TRUE;
            }
            CTC_ERROR_RETURN(sys_humber_aging_set_stop_scan_timer_expired(value_b));
            break;
        case CTC_AGING_PROP_AGING_SCAN_EN :
            if( 0 != value )
            {
                value_b = TRUE;
            }
            CTC_ERROR_RETURN(sys_humber_aging_set_aging_scan_en(value_b));
            break;
         default :
            break;

    }
    return CTC_E_NONE;
}


int32
ctc_humber_aging_get_property(ctc_aging_prop_t aging_prop, uint32* value)
{
    switch( aging_prop )
    {
        case CTC_AGING_PROP_FIFO_THRESHOLD :
            CTC_ERROR_RETURN(sys_humber_aging_get_fifo_threshold((uint8*)value));
            break;
        case  CTC_AGING_PROP_INTERVAL :
            CTC_ERROR_RETURN(sys_humber_aging_get_aging_interval(value));
            break;
        case CTC_AGING_PROP_STOP_SCAN_TIMER_EXPIRED :
            CTC_ERROR_RETURN(sys_humber_aging_get_stop_scan_timer_expired((bool *)value));
            break;
        case CTC_AGING_PROP_AGING_SCAN_EN :
            CTC_ERROR_RETURN(sys_humber_aging_get_aging_scan_en((bool *)value));
            break;
         default :
            break;

    }
    return CTC_E_NONE;
}

int32
ctc_humber_aging_get_aging_status(uint8 lchip, uint32 aging_index, ctc_aging_status_t* age_status)
{
    CTC_ERROR_RETURN(sys_humber_aging_get_aging_index_status(lchip, aging_index, age_status));
    return CTC_E_NONE;
}

int32
ctc_humber_aging_read_aging_fifo(uint8 lchip,
                                 ctc_aging_fifo_info_t* fifo_info_ptr)
{
    CTC_ERROR_RETURN(sys_humber_aging_read_aging_fifo(lchip, fifo_info_ptr));
    return CTC_E_NONE;
}

int32
ctc_humber_learning_set_learning_en(bool enable)
{
    CTC_ERROR_RETURN(sys_humber_learning_set_learning_en(enable));
    return CTC_E_NONE;
}

int32
ctc_humber_learning_aging_init(void* global_cfg)
{
    CTC_ERROR_RETURN(sys_humber_learning_aging_init());
    return CTC_E_NONE;
}

