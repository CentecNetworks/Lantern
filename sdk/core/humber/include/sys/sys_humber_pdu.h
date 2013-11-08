/**
 @file sys_humber_pdu.h

 @date 2009-12-31

 @version v2.0

---file comments----
*/


#ifndef _SYS_HUMBER_PDU_H
#define _SYS_HUMBER_PDU_H


#include "kal.h"
#include "ctc_linklist.h"
#include "ctc_const.h"
#include "ctc_pdu.h"
#include "ctc_parser.h"

/**********************************************************************************
                        Defines and Macros
***********************************************************************************/
#define  SYS_L2PDU_PER_PORT_ACTION_INDEX_RSV_MACDA_TO_CPU    15
#define MIN_SYS_L2PDU_PER_PORT_ACTION_INDEX  0
#define MAX_SYS_L2PDU_PER_PORT_ACTION_INDEX  14

#define SYS_L3PDU_PER_L3IF_ACTION_INDEX_RSV_IPDA_TO_CPU     15
#define SYS_L3PDU_PER_L3IF_ACTION_INDEX_RSV_ARP_TO_CPU      13
#define SYS_L3PDU_PER_L3IF_ACTION_INDEX_RSV_DHCP_TO_CPU    14
#define MIN_SYS_L3PDU_PER_L3IF_ACTION_INDEX  0
#define MAX_SYS_L3PDU_PER_L3IF_ACTION_INDEX  15

/**
 @brief  Classify layer2 pdu based on macda,macda-low24 bit, layer2 header protocol
*/
extern int32
sys_humber_l2pdu_classify_l2pdu(ctc_pdu_l2pdu_type_t l2pdu_type, uint8 index,
                                                                           ctc_pdu_l2pdu_key_t* key);
/**
 @brief  Set layer2 pdu global property
*/
extern int32
sys_humber_l2pdu_set_global_action(ctc_pdu_l2pdu_type_t l2pdu_type,uint8 index,
                                                                                  ctc_pdu_global_l2pdu_action_t* action);

extern int32
sys_humber_l2pdu_get_global_action(ctc_pdu_l2pdu_type_t l2pdu_type,uint8 index,
                                                                                  ctc_pdu_global_l2pdu_action_t* action);
extern int32
sys_humber_l2pdu_get_classified_key(ctc_pdu_l2pdu_type_t l2pdu_type, uint8 index,
                                                                           ctc_pdu_l2pdu_key_t* key);
extern int32
sys_humber_l3pdu_get_classified_key(ctc_pdu_l3pdu_type_t l3pdu_type, uint8 index,
                                                                           ctc_pdu_l3pdu_key_t* key);
extern int32
sys_humber_l3pdu_get_global_action(ctc_pdu_l3pdu_type_t l3pdu_type,uint8 index,
                                                                                  ctc_pdu_global_l3pdu_action_t* action);

/**
 @brief  Per port control layer2 pdu action
*/
extern int32
sys_humber_l2pdu_set_port_action(uint16 gport, uint8 action_index,
                                                                             ctc_pdu_port_l2pdu_action_t action);
extern int32
sys_humber_l2pdu_get_port_action(uint16 gport, uint8 action_index,
                                                                             ctc_pdu_port_l2pdu_action_t *action);
/**
 @brief  Classify layer3 pdu based on layer3 header protocol, layer4 dest port
*/
extern int32
sys_humber_l3pdu_classify_l3pdu(ctc_pdu_l3pdu_type_t l3pdu_type, uint8 index,
                                                                           ctc_pdu_l3pdu_key_t* key);
/**
 @brief  Set layer3 pdu global property
*/
extern int32
sys_humber_l3pdu_set_global_action(ctc_pdu_l3pdu_type_t l3pdu_type,uint8 index,
                                                                                  ctc_pdu_global_l3pdu_action_t* action);
/**
 @brief  Per layer3 interface control layer3 pdu action
*/
extern int32
sys_humber_l3pdu_set_l3if_action(uint16 l3ifid, uint8 action_index,
                                                                           ctc_pdu_l3if_l3pdu_action_t action);
extern int32
sys_humber_l3pdu_get_l3if_action(uint16 l3ifid, uint8 action_index,
                                                                           ctc_pdu_l3if_l3pdu_action_t *action);
/**
 @brief init pdu module
*/
extern int32
sys_humber_pdu_init(void);

#endif

