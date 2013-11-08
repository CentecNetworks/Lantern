/**
 @file ctc_chip.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-11-30

 @version v2.0

   This file contains all chip related data structure, enum, macro and proto.
*/

#ifndef _CTC_CHIP_H
#define _CTC_CHIP_H

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/

#include "kal.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/

/****************************************************************
*
* Data Structures
*
****************************************************************/

/**
 @defgroup chip CHIP
 @{
*/

/**
 @brief define chip type
*/
enum ctc_chip_type_e
{
    CTC_CHIP_HUMBER,    /**< indicate this is a humber chip */
    MAX_CTC_CHIP_TYPE
};
typedef enum ctc_chip_type_e ctc_chip_type_t;

/**
 @brief define chip IO type
*/
enum ctc_chip_io_type_e
{
    CTC_CHIP_IO_SUP,    /**< chip io type is sup */
    CTC_CHIP_IO_PCI,    /**< chip io type is pci */
    MAX_CTC_CHIP_IO_TYPE
};
typedef enum ctc_chip_io_type_e ctc_chip_io_type_t;

/**
 @brief define chip IO type
*/
enum ctc_chip_datapath_type_e
{
    CTC_CHIP_DATAPATH_48G_4SG,  /**< indicate this chip use 48 GMAC + 4 SGMAC */
    CTC_CHIP_DATAPATH_4XG_4SG,  /**< indicate this chip use 4 XMAC + 4 SGMAC */
    CTC_CHIP_DATAPATH_100G,     /**< indicate this chip use  ???*/
    MAX_CTC_CHIP_DATAPATH
};
typedef enum ctc_chip_datapath_type_e ctc_chip_datapath_type_t;

/**
 @brief define chip reset type
*/
enum ctc_chip_reset_type_e
{
    CTC_CHIP_RESET_TCAM_CORE,   /**< reset tcam core */
    CTC_CHIP_RESET_TCAM_POWER,  /**< reset tcam power */
    CTC_CHIP_RESET_GLB,         /**< reset glb */
    CTC_CHIP_RESET_DLLOCK,      /**< reset dllock */
    CTC_CHIP_RESET_PCI,         /**< reset pci */
    CTC_CHIP_RESET_SUP,         /**< reset sup */
    MAX_CTC_CHIP_RESET_RESET_TYPE
};
typedef enum ctc_chip_reset_type_e ctc_chip_reset_type_t;

/**
 @brief define cpumac speed
*/
enum ctc_cpumac_speed_e
{
    CTC_CPUMAC_SPEED_MODE_100M, /**< indicate cpumac's speed is 100M  */
    CTC_CPUMAC_SPEED_MODE_1G,   /**< indicate cpumac's speed is 1G  */
    CTC_CPUMAC_SPEED_MODE_MAX
};
typedef enum ctc_cpumac_speed_e ctc_cpumac_speed_t;

/**
 @brief define chip datapath
*/
struct ctc_chip_datapath_s
{
    uint32  chip_id;                    /**< chip id */
    ctc_chip_type_t chip_type;          /**< chip type */
    ctc_chip_io_type_t chip_io_type;    /**< chip io type */
    ctc_cpumac_speed_t cpumac_speed;    /**< cpumac speed */
    ctc_chip_datapath_type_t  datapath; /**< datapath type */
    uint8 ptp_quanta;                   /**< ptp quanta */
};
typedef struct ctc_chip_datapath_s ctc_chip_datapath_t;

/**
 @brief define chip global configure
*/
struct ctc_chip_global_cfg_s
{
#define MAX_CTC_CPU_MACDA_NUM       8               /**< define cpu macda number */
    uint8  lchip;                                   /**< local chip id */
    uint8  rsv0;
    mac_addr_t cpu_mac_sa;                          /**< cpu mac source address */
    mac_addr_t cpu_mac_da[MAX_CTC_CPU_MACDA_NUM];   /**< cpu mac destination address */

};
typedef struct ctc_chip_global_cfg_s ctc_chip_global_cfg_t;


typedef int32 (*ctc_chip_reset_cb)(uint8 type, uint32 flag);

/**@} end of @defgroup chip  */

#endif

