 /**
  @file ctc_internal_port.h

  @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

  @date 2010-03-29

  @version v2.0

    This file contains all internal port related data structure, enum, macro and proto.
 */


 #ifndef _CTC_INTERNAL_PORT_H
 #define _CTC_INTERNAL_PORT_H
/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/

/****************************************************************
*
* Defines and Macros
*
****************************************************************/
 /**
  @defgroup internal_port INTERNAL_PORT
  @{
 */

/**
 @brief  Define internal port usage type
*/
enum ctc_internal_port_type_e
{
    CTC_INTERNAL_PORT_TYPE_ILOOP,    /**< I-Loopback port */
    CTC_INTERNAL_PORT_TYPE_ELOOP,    /**< E-Loopback port */
    CTC_INTERNAL_PORT_TYPE_DISCARD,  /**< Dicard port */
    CTC_INTERNAL_PORT_TYPE_FWD,      /**< Forwarding port*/
};
typedef enum ctc_internal_port_type_e ctc_internal_port_type_t;


/**
 @brief Internal port assign parameters.
*/
struct ctc_internal_port_assign_para_s
{
    ctc_internal_port_type_t type;  /**< internal port type */
    uint8  gchip;           /**< global chipId */
    uint8  inter_port;      /**< the internal port which is assigned or setted, range 64-XX */
    uint16 fwd_gport;       /**< this is the dest port of FWD type */
};
typedef struct ctc_internal_port_assign_para_s ctc_internal_port_assign_para_t;

/**@} end of @defgroup internal_port INTERNAL_PORT  */


#endif


