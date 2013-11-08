/**
 @file sys_humber_learning_aging.h

 @date 2010-3-16

 @version v2.0

---file comments----
*/


#ifndef SYS_HUMBER_LEARNING_AGING_H_
#define SYS_HUMBER_LEARNING_AGING_H_

#include "kal.h"
#include "ctc_humber_learning_aging.h"

/***************************************
 Humber Learning module's sys interfaces
*****************************************/
/**
 @brief set learning action and cache threshold
*/
int32
sys_humber_set_learning_action(ctc_learning_action_info_t* p_learning_action);

/**
 @brief get learning action and cache threshold
*/
int32
sys_humber_get_learning_action(ctc_learning_action_info_t* p_learning_action);


/**
 @brief get learning cache entry valid status bitmap
*/
int32
sys_humber_learning_get_cache_entry_valid_bitmap(uint8 lchip, uint16* entry_vld_bitmap);


/**
 @brief read learning cache entry
*/
int32
sys_humber_learning_read_learning_cache(uint8 lchip, uint16 entry_vld_bitmap, ctc_learning_cache_t* l2_lc);
/**
 @brief Clear learning cache entry
*/
int32
sys_humber_learning_clear_learning_cache(uint8 lchip, uint16 entry_vld_bitmap);


/***************************************
 Humber Aging module's sys interfaces
*****************************************/

/**
 @brief The function is to set chip's aging cache threshold
*/
extern int32
sys_humber_aging_set_fifo_threshold(uint8 threshold);


/**
 @brief The function is to get chip's aging cache threshold
*/
extern int32
sys_humber_aging_get_fifo_threshold(uint8* threshold);


/**
 @brief The function is to set chip's aging interval
*/
extern int32
sys_humber_aging_set_aging_interval(uint32 age_seconds);

/**
 @brief The function is to set chip's aging interval
*/
extern int32
sys_humber_aging_get_aging_interval(uint32 *age_seconds);


/**
 @brief The function is to set chip's stop_scan_timer_expired
*/
extern int32
sys_humber_aging_set_stop_scan_timer_expired(bool enable);


/**
 @brief The function is to get chip's stop_scan_timer_expired
*/
extern int32
sys_humber_aging_get_stop_scan_timer_expired(bool* enable);



/**
 @brief The function is to set chip's aging scan
*/
extern int32
sys_humber_aging_set_aging_scan_en(bool enable);


/**
 @brief The function is to get chip's aging scan
*/
extern int32
sys_humber_aging_get_aging_scan_en(bool* enable);


/**
 @brief The function is to set chip's aging status(flag entry aging status)
*/
extern int32
sys_humber_aging_set_aging_status(uint8 lchip, uint32 entry_index, bool enable);

/**
 @brief The function is to set chip's aging status(flag entry aging status)
*/
extern int32
sys_humber_learning_set_learning_en(bool enable);

/**
 @brief The function is to get chip's aging status
*/
extern int32
sys_humber_aging_get_aging_index_status(uint8 lchip, uint32 fifo_ptr, ctc_aging_status_t* age_status);


/**
 @brief The function is to read chip's aging fifo,
        and remove FDB table according to Aging FIFO.
*/
extern int32
sys_humber_aging_read_aging_fifo(uint8 lchip, ctc_aging_fifo_info_t* fifo_info_ptr);


/**
 @brief The function will init learning and aging module
*/
extern int32
sys_humber_learning_aging_init(void);

#endif

