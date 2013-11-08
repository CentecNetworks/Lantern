/**
 @file sys_humber_qos_class.h

 @date 2009-11-30

 @version v2.0

 The file defines macro, data structure, and function for qos classification component
*/

#ifndef _SYS_HUMBER_QOS_CLASS_H_
#define _SYS_HUMBER_QOS_CLASS_H_


/*********************************************************************
  *
  * macro definition
  *
  *********************************************************************/

#define SYS_QOS_CLASS_DBG_INFO(FMT, ...) \
    { \
        CTC_DEBUG_OUT_INFO(qos, class, QOS_CLASS_SYS, FMT, ##__VA_ARGS__); \
    }

#define SYS_QOS_CLASS_DBG_FUNC() \
    { \
        CTC_DEBUG_OUT_FUNC(qos, class, QOS_CLASS_SYS); \
    }


/*********************************************************************
  *
  * data structure definition
  *
  *********************************************************************/

/**
 @brief set cos -> priority + color mapping table for the given domain
*/
extern int32
sys_humber_qos_set_igs_cos_map_table(uint8 domain, uint8 cos, uint8 cfi, uint8 priority, ctc_qos_color_t color);


/**
 @brief set ip precedence -> priority + color mapping table for the given domain
*/
extern int32
sys_humber_qos_set_igs_ip_prec_map_table(uint8 domain, uint8 ip_prec, uint8 priority, ctc_qos_color_t color);


/**
 @brief set dscp -> priority + color mapping table for the given domain
*/
extern int32
sys_humber_qos_set_igs_dscp_map_table(uint8 domain, uint8 dscp, uint8 priority, ctc_qos_color_t color);


/**
 @brief set mpls exp -> priority + color mapping table for the given domain
*/
extern int32
sys_humber_qos_set_igs_exp_map_table(uint8 domain, uint8 exp, uint8 priority, ctc_qos_color_t color);


/**
 @brief set priority + color -> cos mapping table for the given domain
*/
extern int32
sys_humber_qos_set_egs_cos_map_table(uint8 domain, uint8 priority, ctc_qos_color_t color, uint8 cos, uint8 cfi);


/**
 @brief set priority + color -> dscp mapping table for the given domain
*/
extern int32
sys_humber_qos_set_egs_dscp_map_table(uint8 domain, uint8 priority, ctc_qos_color_t color, uint8 dscp);


/**
 @brief set priority + color -> mpls exp mapping table for the given domain
*/
extern int32
sys_humber_qos_set_egs_exp_map_table(uint8 domain, uint8 priority, ctc_qos_color_t color, uint8 exp);


/**
 @brief init qos classification mapping tables
*/
extern int32
sys_humber_qos_class_init(void);

#endif
