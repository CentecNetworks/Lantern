/**
 @file sys_humber_aclqos_policer.c

 @date 2009-10-16

 @version v2.0

*/

/****************************************************************************
  *
  * Header Files
  *
  ****************************************************************************/
#include "ctc_error.h"
#include "ctc_const.h"
#include "ctc_macro.h"
#include "ctc_aclqos.h"
#include "ctc_debug.h"

#include "sys_humber_chip.h"
#include "sys_humber_qos_class.h"
#include "sys_humber_port.h"

#include "drv_io.h"
#include "drv_enum.h"
#include "drv_humber.h"

/****************************************************************************
  *
  * Defines and Macros
  *
  ****************************************************************************/

#define SYS_QOS_CLASS_DOMAIN_MAX 7
#define SYS_QOS_CLASS_PRIORITY_MAX 63
#define SYS_QOS_CLASS_COS_MAX 7
#define SYS_QOS_CLASS_CFI_MAX 1
#define SYS_QOS_CLASS_IP_PREC_MAX 7
#define SYS_QOS_CLASS_DSCP_MAX 63
#define SYS_QOS_CLASS_EXP_MAX 7

/****************************************************************************
  *
  * Functions
  *
  ****************************************************************************/

/**
 @brief set cos -> priority + color mapping table for the given domain
*/
int32
sys_humber_qos_set_igs_cos_map_table(uint8 domain, uint8 cos, uint8 cfi, uint8 priority, ctc_qos_color_t color)
{
    uint32 field_id_priority, field_id_color;
    uint32 offset;
    uint32 cmd;
    uint32 tmp;
    uint8 lchip, lchip_num;

    SYS_QOS_CLASS_DBG_FUNC();

    CTC_MAX_VALUE_CHECK(domain, SYS_QOS_CLASS_DOMAIN_MAX);
    CTC_MAX_VALUE_CHECK(cos, SYS_QOS_CLASS_COS_MAX);
    CTC_MAX_VALUE_CHECK(priority, SYS_QOS_CLASS_PRIORITY_MAX);

    if (0 == cfi)
    {
        field_id_priority = (0 == cos % 2) ? IPE_CLASSIFICATION_COS_MAP_TABLE_COS_PRIORITY0 :
                                                                           IPE_CLASSIFICATION_COS_MAP_TABLE_COS_PRIORITY2;
        field_id_color = (0 == cos % 2) ? IPE_CLASSIFICATION_COS_MAP_TABLE_COS_COLOR0 :
                                                                      IPE_CLASSIFICATION_COS_MAP_TABLE_COS_COLOR2;
    }
    else
    {
        field_id_priority = (0 == cos % 2) ? IPE_CLASSIFICATION_COS_MAP_TABLE_COS_PRIORITY1 :
                                                                           IPE_CLASSIFICATION_COS_MAP_TABLE_COS_PRIORITY3;
        field_id_color = (0 == cos % 2) ? IPE_CLASSIFICATION_COS_MAP_TABLE_COS_COLOR1 :
                                                                      IPE_CLASSIFICATION_COS_MAP_TABLE_COS_COLOR3;
    }

    /* each domain has 16 entries, and each entry includes 2 {cos, cfi} fields */
    offset = (domain << 2) | (cos >> 1);

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        cmd = DRV_IOW(IOC_TABLE, IPE_CLASSIFICATION_COS_MAP_TABLE, field_id_priority);
        tmp = priority;
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, &tmp));

        cmd = DRV_IOW(IOC_TABLE, IPE_CLASSIFICATION_COS_MAP_TABLE, field_id_color);
        tmp = color;
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, &tmp));
    }

    SYS_QOS_CLASS_DBG_INFO("cos -> priority + color map, domain = %d, cos = %d, cfi = %d, priority = %d, color = %d\n",
                              domain, cos, cfi, priority, color);

    return CTC_E_NONE;
}


/**
 @brief set ip precedence -> priority + color mapping table for the given domain
*/
int32
sys_humber_qos_set_igs_ip_prec_map_table(uint8 domain, uint8 ip_prec, uint8 priority, ctc_qos_color_t color)
{
    uint32 field_id_priority = 0, field_id_color = 0;
    uint32 offset;
    uint32 cmd;
    uint32 tmp;
    uint8 lchip, lchip_num;

    SYS_QOS_CLASS_DBG_FUNC();

    CTC_MAX_VALUE_CHECK(domain, SYS_QOS_CLASS_DOMAIN_MAX);
    CTC_MAX_VALUE_CHECK(ip_prec, SYS_QOS_CLASS_IP_PREC_MAX);
    CTC_MAX_VALUE_CHECK(priority, SYS_QOS_CLASS_PRIORITY_MAX);

    switch(ip_prec % 4)
    {
        case 0:
            field_id_priority = IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE_PRE_PRIORITY0;
            field_id_color = IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE_PRE_COLOR0;
            break;

        case 1:
            field_id_priority = IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE_PRE_PRIORITY1;
            field_id_color = IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE_PRE_COLOR1;
            break;

        case 2:
            field_id_priority = IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE_PRE_PRIORITY2;
            field_id_color = IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE_PRE_COLOR2;
            break;

        case 3:
            field_id_priority = IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE_PRE_PRIORITY3;
            field_id_color = IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE_PRE_COLOR3;
            break;
    }

    /* each domain has 2 entries, and each entry includes 4 ip-prec fields */
    offset = (domain << 1) | (ip_prec >> 2);

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        cmd = DRV_IOW(IOC_TABLE, IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE, field_id_priority);
        tmp = priority;
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, &tmp));

        cmd = DRV_IOW(IOC_TABLE, IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE, field_id_color);
        tmp = color;
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, &tmp));
    }

    SYS_QOS_CLASS_DBG_INFO("ip-prec -> priority + color map, domain = %d, ip_prec = %d, priority = %d, color = %d\n",
                               domain, ip_prec, priority, color);

    return CTC_E_NONE;
}


/**
 @brief set dscp -> priority + color mapping table for the given domain
*/
int32
sys_humber_qos_set_igs_dscp_map_table(uint8 domain, uint8 dscp, uint8 priority, ctc_qos_color_t color)
{
    uint32 field_id_priority = 0, field_id_color = 0;
    uint32 offset;
    uint32 cmd;
    uint32 tmp;
    uint8 lchip, lchip_num;

    SYS_QOS_CLASS_DBG_FUNC();

    CTC_MAX_VALUE_CHECK(domain, SYS_QOS_CLASS_DOMAIN_MAX);
    CTC_MAX_VALUE_CHECK(dscp, SYS_QOS_CLASS_DSCP_MAX);
    CTC_MAX_VALUE_CHECK(priority, SYS_QOS_CLASS_PRIORITY_MAX);

    switch(dscp % 4)
    {
        case 0:
            field_id_priority = IPE_CLASSIFICATION_DSCP_MAP_TABLE_DSCP_PRIORITY0;
            field_id_color = IPE_CLASSIFICATION_DSCP_MAP_TABLE_DSCP_COLOR0;
            break;

        case 1:
            field_id_priority = IPE_CLASSIFICATION_DSCP_MAP_TABLE_DSCP_PRIORITY1;
            field_id_color = IPE_CLASSIFICATION_DSCP_MAP_TABLE_DSCP_COLOR1;
            break;

        case 2:
            field_id_priority = IPE_CLASSIFICATION_DSCP_MAP_TABLE_DSCP_PRIORITY2;
            field_id_color = IPE_CLASSIFICATION_DSCP_MAP_TABLE_DSCP_COLOR2;
            break;

        case 3:
            field_id_priority = IPE_CLASSIFICATION_DSCP_MAP_TABLE_DSCP_PRIORITY3;
            field_id_color = IPE_CLASSIFICATION_DSCP_MAP_TABLE_DSCP_COLOR3;
            break;
    }

    /* each domain has 16 entries, and each entry includes 4 dscp fields */
    offset = (domain << 4) | (dscp >> 2);

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        cmd = DRV_IOW(IOC_TABLE, IPE_CLASSIFICATION_DSCP_MAP_TABLE, field_id_priority);
        tmp = priority;
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, &tmp));

        cmd = DRV_IOW(IOC_TABLE, IPE_CLASSIFICATION_DSCP_MAP_TABLE, field_id_color);
        tmp = color;
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, &tmp));
    }

    SYS_QOS_CLASS_DBG_INFO("dscp -> priority + color map, domain = %d, dscp = %d, priority = %d, color = %d\n",
                               domain, dscp, priority, color);

    return CTC_E_NONE;
}


/**
 @brief set mpls exp -> priority + color mapping table for the given domain
*/
int32
sys_humber_qos_set_igs_exp_map_table(uint8 domain, uint8 exp, uint8 priority, ctc_qos_color_t color)
{
    uint32 field_id_priority = 0, field_id_color = 0;
    uint32 offset;
    uint32 cmd;
    uint32 tmp;
    uint8 lchip, lchip_num;

    SYS_QOS_CLASS_DBG_FUNC();

    CTC_MAX_VALUE_CHECK(domain, SYS_QOS_CLASS_DOMAIN_MAX);
    CTC_MAX_VALUE_CHECK(exp, SYS_QOS_CLASS_EXP_MAX);
    CTC_MAX_VALUE_CHECK(priority, SYS_QOS_CLASS_PRIORITY_MAX);

    switch(exp % 4)
    {
        case 0:
            field_id_priority = IPE_MPLS_EXP_MAP_TABLE_MPLS_PRIORITY0;
            field_id_color = IPE_MPLS_EXP_MAP_TABLE_MPLS_COLOR0;
            break;

        case 1:
            field_id_priority = IPE_MPLS_EXP_MAP_TABLE_MPLS_PRIORITY1;
            field_id_color = IPE_MPLS_EXP_MAP_TABLE_MPLS_COLOR1;
            break;

        case 2:
            field_id_priority = IPE_MPLS_EXP_MAP_TABLE_MPLS_PRIORITY2;
            field_id_color = IPE_MPLS_EXP_MAP_TABLE_MPLS_COLOR2;
            break;

        case 3:
            field_id_priority = IPE_MPLS_EXP_MAP_TABLE_MPLS_PRIORITY3;
            field_id_color = IPE_MPLS_EXP_MAP_TABLE_MPLS_COLOR3;
            break;
    }

    /* each domain has 2 entries, and each entry includes 4 mpls exp fields */
    offset = (domain << 1) | (exp >> 2);

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        tmp = priority;
        cmd = DRV_IOW(IOC_TABLE, IPE_MPLS_EXP_MAP_TABLE, field_id_priority);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, &tmp));

        tmp = color;
        cmd = DRV_IOW(IOC_TABLE, IPE_MPLS_EXP_MAP_TABLE, field_id_color);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, &tmp));
    }

    SYS_QOS_CLASS_DBG_INFO("exp -> priority + color map, domain = %d, exp = %d, priority = %d, color = %d\n",
                               domain, exp, priority, color);

    return CTC_E_NONE;
}


/**
 @brief set priority + color -> cos mapping table for the given domain
*/
int32
sys_humber_qos_set_egs_cos_map_table(uint8 domain, uint8 priority, ctc_qos_color_t color, uint8 cos, uint8 cfi)
{
    uint32 field_id_cos, field_id_cfi;
    uint32 offset;
    uint32 cmd_cos, cmd_cfi;
    uint32 tmp;
    uint8 lchip, lchip_num;

    SYS_QOS_CLASS_DBG_FUNC();

    CTC_MAX_VALUE_CHECK(domain, SYS_QOS_CLASS_DOMAIN_MAX);
    CTC_MAX_VALUE_CHECK(priority, SYS_QOS_CLASS_PRIORITY_MAX);
    CTC_MAX_VALUE_CHECK(cos, SYS_QOS_CLASS_COS_MAX);

    offset = (domain << 8) | (priority << 2) | color;

    field_id_cos = EPE_EDIT_PRIORITY_MAP_TABLE_MAPPED_COS;
    field_id_cfi = EPE_EDIT_PRIORITY_MAP_TABLE_MAPPED_CFI;

    cmd_cos = DRV_IOW(IOC_TABLE, EPE_EDIT_PRIORITY_MAP_TABLE, field_id_cos);
    cmd_cfi = DRV_IOW(IOC_TABLE, EPE_EDIT_PRIORITY_MAP_TABLE, field_id_cfi);

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        tmp = cos;
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd_cos, &tmp));

        tmp = cfi;
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd_cfi, &tmp));
    }

    SYS_QOS_CLASS_DBG_INFO("priority + color -> cos map, domain = %d, priority = %d, color = %d, cos = %d, cfi = %d\n",
                               domain, priority, color, cos, cfi);

    return CTC_E_NONE;
}


/**
 @brief set priority + color -> dscp mapping table for the given domain
*/
int32
sys_humber_qos_set_egs_dscp_map_table(uint8 domain, uint8 priority, ctc_qos_color_t color, uint8 dscp)
{
    uint32 field_id;
    uint32 offset;
    uint32 cmd;
    uint32 tmp;
    uint8 lchip, lchip_num;

    SYS_QOS_CLASS_DBG_FUNC();

    CTC_MAX_VALUE_CHECK(domain, SYS_QOS_CLASS_DOMAIN_MAX);
    CTC_MAX_VALUE_CHECK(priority, SYS_QOS_CLASS_PRIORITY_MAX);
    CTC_MAX_VALUE_CHECK(dscp, SYS_QOS_CLASS_DSCP_MAX);

    offset = (domain << 8) | (priority << 2) | color;

    field_id = EPE_EDIT_PRIORITY_MAP_TABLE_MAPPED_DSCP;

    cmd = DRV_IOW(IOC_TABLE, EPE_EDIT_PRIORITY_MAP_TABLE, field_id);
    tmp = dscp;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, &tmp));
    }

    SYS_QOS_CLASS_DBG_INFO("priority + color -> dscp map, domain = %d, priority = %d, color = %d, dscp = %d\n",
                               domain, priority, color, dscp);

    return CTC_E_NONE;
}

/**
 @brief set priority + color -> mpls exp mapping table for the given domain
*/
int32
sys_humber_qos_set_egs_exp_map_table(uint8 domain, uint8 priority, ctc_qos_color_t color, uint8 exp)
{
    uint32 field_id;
    uint32 offset;
    uint32 cmd;
    uint32 tmp;
    uint8 lchip, lchip_num;

    SYS_QOS_CLASS_DBG_FUNC();

    CTC_MAX_VALUE_CHECK(domain, SYS_QOS_CLASS_DOMAIN_MAX);
    CTC_MAX_VALUE_CHECK(priority, SYS_QOS_CLASS_PRIORITY_MAX);
    CTC_MAX_VALUE_CHECK(exp, SYS_QOS_CLASS_EXP_MAX);

    offset = (domain << 8) | (priority << 2) | color;

    field_id = EPE_EDIT_PRIORITY_MAP_TABLE_MAPPED_EXP;

    cmd = DRV_IOW(IOC_TABLE, EPE_EDIT_PRIORITY_MAP_TABLE, field_id);
    tmp = exp;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, &tmp));
    }

    SYS_QOS_CLASS_DBG_INFO("priority + color -> exp map, domain = %d, priority = %d, color = %d, exp = %d\n",
                               domain, priority, color, exp);

    return CTC_E_NONE;
}


/**
 @brief init qos classification mapping tables
*/
int32
sys_humber_qos_class_init(void)
{
    ctc_qos_color_t color;
    uint8 priority, value, domain, lchip, gchip, lchip_num, lport;
    uint16 gport;

    for (domain = 0; domain < 8; domain++)
    {
        /* init ingress cos + cfi -> priority + color mapping table */
        for (value = 0; value < 8; value++)
        {
            priority = value * 8;
            CTC_ERROR_RETURN(sys_humber_qos_set_igs_cos_map_table(domain, value, 0, priority, CTC_QOS_COLOR_GREEN));
            CTC_ERROR_RETURN(sys_humber_qos_set_igs_cos_map_table(domain, value, 1, priority, CTC_QOS_COLOR_GREEN));
        }

        /* init ingress ip prec -> priority + color mapping table */
        for (value = 0; value < 8; value++)
        {
            priority = value * 8;
            CTC_ERROR_RETURN(sys_humber_qos_set_igs_ip_prec_map_table(domain, value, priority, CTC_QOS_COLOR_GREEN));
        }

        /* init ingress dscp -> priority + color mapping table */
        for (value = 0; value < 64; value++)
        {
            priority = value;
            CTC_ERROR_RETURN(sys_humber_qos_set_igs_dscp_map_table(domain, value, priority, CTC_QOS_COLOR_GREEN));
        }

        /* init ingress mpls exp -> priority + color mapping table */
        for (value = 0; value < 8; value++)
        {
            priority = value * 8;
            CTC_ERROR_RETURN(sys_humber_qos_set_igs_exp_map_table(domain, value, priority, CTC_QOS_COLOR_GREEN));
        }

        for (priority = 0; priority < 64; priority++)
        {
            for (color = CTC_QOS_COLOR_NONE; color < MAX_CTC_QOS_COLOR; color++)
            {
                /* init egress priority + color -> cos mapping table */
                value = priority / 8;
                CTC_ERROR_RETURN(sys_humber_qos_set_egs_cos_map_table(domain, priority, color, value, 0));

                /* init egress priority + color -> dscp mapping table */
                value = priority;
                CTC_ERROR_RETURN(sys_humber_qos_set_egs_dscp_map_table(domain, priority, color, value));

                /* init egress priority + color -> exp mapping table */
                value = priority / 8;
                CTC_ERROR_RETURN(sys_humber_qos_set_egs_exp_map_table(domain, priority, color, value));
            }
        }
    }

    lchip_num = sys_humber_get_local_chip_num();

    for(lchip=0; lchip<lchip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
    {
        for(lport=0; ; lport++)
        {
            CTC_ERROR_RETURN(sys_humber_get_gchip_id(lchip, &gchip));
            gport = (gchip<<CTC_LOCAL_PORT_LENGTH) | lport;

            CTC_ERROR_RETURN(sys_humber_port_set_qos_domain(gport, CTC_BOTH_DIRECTION, 0));

            if(MAX_PORT_NUM_PER_CHIP-1 == lport)
            {
                break;
            }
        }
    }

    return CTC_E_NONE;
}




