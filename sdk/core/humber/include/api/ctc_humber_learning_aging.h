/**
 @file ctc_humber_learning_aging.h

 @date 2010-3-16

 @version v2.0

---file comments----
*/

#ifndef CTC_HUMBER_LEARNING_AGING_H_
#define CTC_HUMBER_LEARNING_AGING_H_

#include "kal.h"
#include "ctc_learning_aging.h"

/**
 @addtogroup learning_aging  LEARNING_AGING
 @{
*/


/***************************************
 Learning Module's HUMBER API Interfaces
*****************************************/
/**
 @brief This function is to set the learning action and cache threshold

 @param[in] p_learning_action    learning action and cache threshold

 @return CTC_E_XXX
*/
extern int32
ctc_humber_set_learning_action(ctc_learning_action_info_t* p_learning_action);

/**
 @brief This function is to get the learning action and cache threshold

 @param[out] p_learning_action    learning action and cache threshold

 @return CTC_E_XXX
*/
extern int32
ctc_humber_get_learning_action(ctc_learning_action_info_t* p_learning_action);


/**
 @brief This function is to get the learning cache entry valid info

 @param[in] lchip                      local chip id
 @param[in] entry_vld_bitmap    learning cache entry valid bit

 @return CTC_E_XXX
*/
extern int32
ctc_humber_learning_get_cache_entry_valid_bitmap(uint8 lchip, uint16* entry_vld_bitmap);


/**
 @brief This function is read learning cache data, learning isr founction

 @param[in] lchip             local chip id
 @param[in]                      entry_vld_bitmap    16 learning cache entry valid bit
  @param[in]                    l2_lc               use store the cache contents
 @return CTC_E_XXX
*/
extern int32
ctc_humber_learning_read_learning_cache(uint8 lchip, uint16 entry_vld_bitmap, ctc_learning_cache_t* l2_lc);



/**
 @brief This function is read learning cache data, learning isr founction

 @param[in] lchip             local chip id
 @param[in]                     entry_vld_bitmap    16 learning cache entry valid bit
 @return CTC_E_XXX
*/
extern int32
ctc_humber_learning_clear_learning_cache(uint8 lchip, uint16 entry_vld_bitmap);


/***************************************
 Aging Module's HUMBER API Interfaces
*****************************************/


/**
 @brief This function is to set the aging properties

 @param[in] aging_prop    aging action type
 @param[in] value         aging action value

 @return CTC_E_XXX
*/
extern int32
ctc_humber_aging_set_property(ctc_aging_prop_t aging_prop, uint32 value);

/**
 @brief This function is to get the aging properties

 @param[in] aging_prop    aging action type
 @param[in] value         aging action value

 @return CTC_E_XXX
*/
extern int32
ctc_humber_aging_get_property(ctc_aging_prop_t aging_prop, uint32* value);


/**
 @brief This function is to get the aging fifo age status

 @param[in]   lchip               local chip id
 @param[in]    aging_index   aging table index
 @param[out]  age_status     the age status of aging entry
 @return CTC_E_XXX
*/
extern int32
ctc_humber_aging_get_aging_status(uint8 lchip, uint32 aging_index, ctc_aging_status_t* age_status);


/**
 @brief This function is to read aging fifo data, aging isr function

 @param[in]     lchip                local chip id
 @param[out]   fifo_info         use to store those read aging fifo data
 @return CTC_E_XXX
*/
extern int32
ctc_humber_aging_read_aging_fifo(uint8 lchip, ctc_aging_fifo_info_t* fifo_info);

/**
 @brief This function is to set learning function

 @param[in]     lchip                local chip id
 @param[out]   fifo_info         use to store those read aging fifo data
 @return CTC_E_XXX
*/
extern int32
ctc_humber_learning_set_learning_en(bool enable);

/**
 @brief This function is to initialize the learning and aging modules
 @return CTC_E_XXX
*/
extern int32
ctc_humber_learning_aging_init(void* global_cfg);

/**@} end of @addtogroup learning_aging  LEARNING_AGING*/

#endif

