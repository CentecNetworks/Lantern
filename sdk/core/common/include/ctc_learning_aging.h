/**
 @file ctc_learning_aging.h

 @date 2010-2-25

 @version v2.0

---file comments----
*/

#ifndef CTC_LEARNING_AGING_H_
#define CTC_LEARNING_AGING_H_

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_const.h"

/**
 @defgroup learning_aging  LEARNING_AGING
 @{
*/

#define CTC_LEARNING_CACHE_MAX_INDEX          16     /**< learning cache size */
#define CTC_AGING_FIFO_DEPTH                           16     /**< aging fifo size */
#define MAX_CTC_ENTRY_NUM_IN_AGING_INDEX  16    /**< a aging entry  contains  the max num of  MAC entries */

/**
 @brief enum type about aging status
*/
struct ctc_aging_status_s
{
    uint16    aging_valid_bitmap;  /**< if set, corresponding  entry need  be aged*/
    uint16    reserved;            /**< Reserved */
};
typedef struct ctc_aging_status_s ctc_aging_status_t;

/**
 @brief learning cache entry node struct
*/
struct ctc_learning_cache_entry_s
{
    uint32 mac_sa_0to31;       /**< MacSa lower 32 bits */

    uint32 cmac_sa_0to31;      /**< CmacSa Lower 32 bits */

    uint16 mac_sa_32to47;      /**< MacSa upper 16 bits */
    uint16 cmac_sa_32to47;     /**< CMacSa upper 16 bits */

	uint16 logic_port;         /**< logic port */
	uint8  rsv[2];			   /**< Reserved */

    uint16 cvlan_id;           /**< Cvlan ID */
    uint16 svlan_id;           /**< Svlan ID */

    uint16 mapped_vlan_id;     /**< Mapped Vlan ID */
    uint16 global_src_port;    /**< Learnt source port number */

    uint8 is_logic_port;       /**< If set, indicate that CMacSa[47:0]
                                    carries {35'd0, vplsSrcPort[12:0]} */
    uint8 is_ether_oam;        /**< If set, indicate packet is Ethernet OAM */
    uint8 ether_oam_md_level;  /**< Ethernet OAM MD Levl */
    uint8 reserved;            /**< Reserved */
};
typedef struct ctc_learning_cache_entry_s ctc_learning_cache_entry_t;

/**
 @brief struct type to store the aging cache content
*/
struct ctc_learning_cache_s
{
    ctc_learning_cache_entry_t learning_entry[CTC_LEARNING_CACHE_MAX_INDEX];    /**< Learning cache entry */
    uint32 entry_num;                                                           /**< Learning cache entry num */
};
typedef struct ctc_learning_cache_s ctc_learning_cache_t;

/**
 @brief struct type to store the aging fifo content
*/
struct ctc_aging_fifo_info_s
{
    uint32 aging_index_array[CTC_AGING_FIFO_DEPTH]; /**< Aging FIFO index */
    uint32 aging_base;      /**< Aging base */
    uint8  fifo_idx_num;    /**< FIFO index num */
};
typedef struct ctc_aging_fifo_info_s ctc_aging_fifo_info_t;


/**
 @brief define Learning action
*/
enum ctc_learning_action_e
{
     CTC_LEARNING_ACTION_ALWAYS_CPU = 1,          /**< MAC Learning by CPU*/
     CTC_LEARNING_ACTION_CACHE_FULL_TO_CPU,   /**< MAC Learning by cache ,but cache full send to cpu*/
     CTC_LEARNING_ACTION_CACHE_ONLY,                 /**< MAC Learning by cache*/
     CTC_LEARNING_ACTION_DONLEARNING                 /**< Dont  Learning */
};
typedef enum ctc_learning_action_e ctc_learning_action_t;


/**
 @brief define Learning action and learning cache threshold
*/
struct ctc_learning_action_info_s
{
    ctc_learning_action_t action;	    /**< define Learning action*/
    uint8    cache_threshold;          /**< learning cache_threshold*/
    uint8    rsv1;
    uint16   rsv2;

};
typedef struct ctc_learning_action_info_s ctc_learning_action_info_t;


/**
 @brief define MAC Aging property associated type data struct
*/
enum ctc_aging_prop_e
{
    CTC_AGING_PROP_FIFO_THRESHOLD  =  1,    /**< aging fifo threshold  ; value: 0x0--0xFF*/
    CTC_AGING_PROP_INTERVAL ,               /**< aging interval  ; value :0x0--0xFFFFFFFF*/
    CTC_AGING_PROP_STOP_SCAN_TIMER_EXPIRED ,/**< when aing timer is expired,if set , timer will stop scan ; value: TRUE or FALSE */
    CTC_AGING_PROP_AGING_SCAN_EN ,          /**< aging timer  ; value TRUE FALSE */

    CTC_AGING_PROP_MAX
};
typedef enum ctc_aging_prop_e ctc_aging_prop_t;


/**@} end of @defgroup   learning_aging  LEARNING_AGING*/

#endif


