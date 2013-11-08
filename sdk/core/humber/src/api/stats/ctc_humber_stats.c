/**
 @file ctc_humber_stats.c

 @date 2009-12-22

 @version v2.0

*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_error.h"
#include "ctc_humber_stats.h"
#include "sys_humber_stats.h"
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
 @brief Init statistics table and register

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_init(void* stats_global_cfg)
{
    CTC_ERROR_RETURN(sys_humber_stats_init());

    return CTC_E_NONE;
}


/**
 @brief Set Mac base stats property

 @param[in] gport  global port of the system
 @param[in] mac_stats_prop_type  the property type to set
 @param[in] prop_data  the property value to set

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_set_mac_stats_cfg(uint16 gport, ctc_mac_stats_prop_type_t mac_stats_prop_type, ctc_mac_stats_property_t prop_data)
{
    switch (mac_stats_prop_type)
    {
        case CTC_STATS_PACKET_LENGTH_MTU1:
            CTC_ERROR_RETURN(sys_humber_stats_set_mac_packet_length_mtu1(gport, prop_data.data.length));
            break;
        case CTC_STATS_PACKET_LENGTH_MTU2:
            CTC_ERROR_RETURN(sys_humber_stats_set_mac_packet_length_mtu2(gport, prop_data.data.length));
            break;
        case CTC_STATS_DOT1Q_SUBTRACT_BYTES:
            CTC_ERROR_RETURN(sys_humber_stats_set_mac_dot1q_subtract_bytes(gport, prop_data.data.byte));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}

/**
 @brief Get Mac base stats property

 @param[in] gport  global port of the system
 @param[in] mac_stats_prop_type  the property type to get
 @param[out] p_prop_data  the property value to get

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_get_mac_stats_cfg(uint16 gport, ctc_mac_stats_prop_type_t mac_stats_prop_type, ctc_mac_stats_property_t* p_prop_data)
{
    switch (mac_stats_prop_type)
    {
        case CTC_STATS_PACKET_LENGTH_MTU1:
            CTC_ERROR_RETURN(sys_humber_stats_get_mac_packet_length_mtu1(gport, &(p_prop_data->data.length)));
            break;
        case CTC_STATS_PACKET_LENGTH_MTU2:
            CTC_ERROR_RETURN(sys_humber_stats_get_mac_packet_length_mtu2(gport, &(p_prop_data->data.length)));
            break;
        case CTC_STATS_DOT1Q_SUBTRACT_BYTES:
            CTC_ERROR_RETURN(sys_humber_stats_get_mac_dot1q_subtract_bytes(gport, &(p_prop_data->data.byte)));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}

/**
 @brief Get Mac base stats

 @param[in] gport  global port of the system
 @param[in] dir  MAC base stats direction
 @param[out] p_stats  MAC base stats value

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_get_mac_stats(uint16 gport, ctc_mac_stats_dir_t dir, ctc_mac_stats_t* p_stats)
{

    switch(dir)
    {
        case CTC_STATS_MAC_STATS_RX:
            CTC_ERROR_RETURN(sys_humber_stats_get_mac_rx_stats(gport, &(p_stats->stats.rx_stats)));
            break;
        case CTC_STATS_MAC_STATS_TX:
            CTC_ERROR_RETURN(sys_humber_stats_get_mac_tx_stats(gport, &(p_stats->stats.tx_stats)));
            break;
        default :
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}

/**
 @brief Reset Mac base stats

 @param[in] gport  global port of the system
 @param[in] dir  MAC base stats direction

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_reset_mac_stats(uint16 gport, ctc_mac_stats_dir_t dir)
{
    switch(dir)
    {
        case CTC_STATS_MAC_STATS_RX:
            CTC_ERROR_RETURN(sys_humber_stats_reset_mac_rx_stats(gport));
            break;
        case CTC_STATS_MAC_STATS_TX:
            CTC_ERROR_RETURN(sys_humber_stats_reset_mac_tx_stats(gport));
            break;
        default :
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}

/**
 @brief Set Port base stats option

 @param[in] dir  direction
 @param[in] type  port stats option type

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_set_port_stats_cfg(ctc_direction_t dir, ctc_stats_port_stats_option_type_t type)
{
    switch(dir)
    {
        case CTC_INGRESS:
            CTC_ERROR_RETURN(sys_humber_stats_set_igs_port_stats_option(type));
            break;
        case CTC_EGRESS:
            CTC_ERROR_RETURN(sys_humber_stats_set_egs_port_stats_option(type));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}

/**
 @brief Get Port base stats option

 @param[in] dir  direction
 @param[out] p_type  port stats option type

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_get_port_stats_cfg(ctc_direction_t dir, ctc_stats_port_stats_option_type_t* p_type)
{
    switch(dir)
    {
        case CTC_INGRESS:
            CTC_ERROR_RETURN(sys_humber_stats_get_igs_port_stats_option(p_type));
            break;
        case CTC_EGRESS:
            CTC_ERROR_RETURN(sys_humber_stats_get_egs_port_stats_option(p_type));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}


/**
 @brief Get Port base stats

 @param[in] gport  global port of the system
 @param[in] dir  direction
 @param[out] p_stats    port base stats

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_get_port_stats(uint16 gport, ctc_direction_t dir, ctc_stats_port_t* p_stats)
{
    switch(dir)
    {
        case CTC_INGRESS:
            CTC_ERROR_RETURN(sys_humber_stats_get_igs_port_stats(gport, p_stats));
            break;
        case CTC_EGRESS:
            CTC_ERROR_RETURN(sys_humber_stats_get_egs_port_stats(gport, p_stats));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}

/**
 @brief Reset Port base stats

 @param[in] gport  global port of the system
 @param[in] dir  direction

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_reset_port_stats(uint16 gport, ctc_direction_t dir)
{
    switch(dir)
    {
        case CTC_INGRESS:
            CTC_ERROR_RETURN(sys_humber_stats_reset_igs_port_stats(gport));
            break;
        case CTC_EGRESS:
            CTC_ERROR_RETURN(sys_humber_stats_reset_egs_port_stats(gport));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}

/**
 @brief Get Global Forwarding stats

 @param[in] dir  direction
 @param[out] p_stats    global fwd stats value

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_get_global_fwd_stats(ctc_direction_t dir, ctc_stats_global_fwd_t* p_stats)
{
    switch(dir)
    {
        case CTC_INGRESS:
            CTC_ERROR_RETURN(sys_humber_stats_get_igs_global_fwd_stats(&(p_stats->stats.igs_stats)));
            break;

        case CTC_EGRESS:
            CTC_ERROR_RETURN(sys_humber_stats_get_egs_global_fwd_stats(&(p_stats->stats.egs_stats)));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}

/**
 @brief Reset Global Forwarding stats

 @param[in] dir  direction

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_reset_global_fwd_stats(ctc_direction_t dir)
{
   switch(dir)
    {
        case CTC_INGRESS:
            CTC_ERROR_RETURN(sys_humber_stats_reset_igs_global_fwd_stats());
            break;

        case CTC_EGRESS:
            CTC_ERROR_RETURN(sys_humber_stats_reset_egs_global_fwd_stats());
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}

/**
 @brief Set port log discard stats enable

 @param[in] enable    a boolean value denote log port discard stats is enable

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_set_drop_packet_stats_enable(bool enable)
{
    CTC_ERROR_RETURN(sys_humber_stats_set_port_log_discard_stats_enable(enable));
    return CTC_E_NONE;
}

/**
 @brief Get port log discard stats enable

 @param[in] p_enable    a boolean value denote log port discard stats is enable

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_get_drop_packet_stats_enable(bool* p_enable)
{
    CTC_ERROR_RETURN(sys_humber_stats_get_port_log_discard_stats_enable(p_enable));
    return CTC_E_NONE;
}


/**
 @brief Get port log stats

 @param[in] gport  global port of the system
 @param[in] dir  direction
 @param[out] p_stats    port log stats

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_get_log_option(uint16 gport ,ctc_direction_t dir, ctc_stats_basic_t* p_stats)
{
    switch(dir)
    {
        case CTC_INGRESS:
            CTC_ERROR_RETURN(sys_humber_stats_get_igs_port_log_stats(gport, p_stats));
            break;

        case CTC_EGRESS:
            CTC_ERROR_RETURN(sys_humber_stats_get_egs_port_log_stats(gport, p_stats));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}

/**
 @brief Reset port log stats

 @param[in] gport  global port of the system
 @param[in] dir  direction

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_reset_log_option(uint16 gport ,ctc_direction_t dir)
{
    switch(dir)
    {
        case CTC_INGRESS:
            CTC_ERROR_RETURN(sys_humber_stats_reset_igs_port_log_stats(gport));
            break;

        case CTC_EGRESS:
            CTC_ERROR_RETURN(sys_humber_stats_reset_egs_port_log_stats(gport));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}


/**
 @brief Set stats general property

 @param[in] stats_param  stats type to set
 @param[in] stats_prop   the property value to set

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_set_global_cfg(ctc_stats_property_param_t stats_param, ctc_stats_property_t stats_prop)
{
    switch(stats_param.prop_type)
    {
        case CTC_STATS_PROPERTY_SATURATE:
            CTC_ERROR_RETURN(sys_humber_stats_set_saturate_en(stats_param.stats_type, stats_prop.data.enable));
            break;

        case CTC_STATS_PROPERTY_HOLD:
            CTC_ERROR_RETURN(sys_humber_stats_set_hold_en(stats_param.stats_type, stats_prop.data.enable));
            break;

        case CTC_STATS_PROPERTY_CLEAR_AFTER_READ:
            CTC_ERROR_RETURN(sys_humber_stats_set_clear_after_read_en(stats_param.stats_type, stats_prop.data.enable));
            break;

        case CTC_STATS_PROPERTY_PKT_CNT_THREASHOLD:
            CTC_ERROR_RETURN(sys_humber_stats_set_pkt_cnt_threshold(stats_prop.data.threshold_16byte));
            break;

        case CTC_STATS_PROPERTY_BYTE_CNT_THREASHOLD:
            CTC_ERROR_RETURN(sys_humber_stats_set_byte_cnt_threshold(stats_prop.data.threshold_16byte));
            break;

        case CTC_STATS_PROPERTY_FIFO_DEPTH_THREASHOLD:
            CTC_ERROR_RETURN(sys_humber_stats_set_fifo_depth_threshold(stats_prop.data.threshold_8byte));
            break;

        default :
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}


/**
 @brief Get stats general property

 @param[in] stats_param  stats type to get
 @param[in] p_stats_prop   the property value to get

 @return CTC_E_XXX

*/
int32
ctc_humber_stats_get_global_cfg(ctc_stats_property_param_t stats_param, ctc_stats_property_t* p_stats_prop)
{
    switch (stats_param.prop_type)
    {
        case CTC_STATS_PROPERTY_SATURATE:
            CTC_ERROR_RETURN(sys_humber_stats_get_saturate_en(stats_param.stats_type, &(p_stats_prop->data.enable)));
            break;
        case CTC_STATS_PROPERTY_HOLD:
            CTC_ERROR_RETURN(sys_humber_stats_get_hold_en(stats_param.stats_type, &(p_stats_prop->data.enable)));
            break;
        case CTC_STATS_PROPERTY_CLEAR_AFTER_READ:
            CTC_ERROR_RETURN(sys_humber_stats_get_clear_after_read_en(stats_param.stats_type, &(p_stats_prop->data.enable)));
            break;
        case CTC_STATS_PROPERTY_PKT_CNT_THREASHOLD:
            CTC_ERROR_RETURN(sys_humber_stats_get_pkt_cnt_threshold(&(p_stats_prop->data.threshold_16byte)));
            break;
        case CTC_STATS_PROPERTY_BYTE_CNT_THREASHOLD:
            CTC_ERROR_RETURN(sys_humber_stats_get_byte_cnt_threshold(&(p_stats_prop->data.threshold_16byte)));
            break;
        case CTC_STATS_PROPERTY_FIFO_DEPTH_THREASHOLD:
            CTC_ERROR_RETURN(sys_humber_stats_get_fifo_depth_threshold(&(p_stats_prop->data.threshold_8byte)));
            break;

        default :
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}


int32
ctc_humber_stats_get_fwd_stats_enable(ctc_stats_fwd_type_t fwd_stats_type, bool* p_enable)
{
    CTC_ERROR_RETURN(sys_humber_stats_get_fwd_stats_enable(fwd_stats_type, p_enable));

    return CTC_E_NONE;
}

int32
ctc_humber_stats_intr_callback_func(uint8* gchip)
{
    CTC_ERROR_RETURN(sys_humber_stats_intr_callback_func(gchip));

    return CTC_E_NONE;
}


