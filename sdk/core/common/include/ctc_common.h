/**
 @file ctc_common.h

 @date 2010-6-10

 @version v2.0

The file define all CTC SDK module's common function and APIs.
*/


#ifndef _CTC_COMMON_H
#define _CTC_COMMON_H


/*
  define forwarded packet to cpu nexthopptr
   combine:
        |3bit    |      6bit    |        6bit      | 3bit   |   = 18 bit
        [modid][exception index][exception sub index|[cpu mac]
 In humber:
        |3bit    |      6bit    |      5bit  |       4 bit     |    = 18 bit
        [modid][exception index][ Reserved][exception sub index/cpu mac]
       1) if cpu destination  is single cpu , the low 4 bits indicate exception sub index;
       2) if cpu destination  is multiple cpu ,the low 3 bits indicate cpu mac index;
 In Greatbelt
        |3bit    |      6bit    |      5bit  |       4 bit     |    = 18 bit
        [modid][exception index][ Reserved][exception sub index]

*/
#define CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(modid,exp_idx,low_9bits)  ((modid) << 15 | (exp_idx) <<9 |(low_9bits))


/**
  @brief  define  forwarded packet  to cpu  module id

*/
enum ctc_cpu_pkttocpu_mod_e
{
    CTC_CPU_PKTTOCPU_MOD_NORMAL_EXCP = 0,  /**< normal exception */
    CTC_CPU_PKTTOCPU_MOD_FATAL_EXCP,       /**< fatal exception */
    CTC_CPU_PKTTOCPU_MOD_OAM_EXCP,         /**< oam exception */
    CTC_CPU_PKTTOCPU_MOD_NORMAL_FWD_CPU,   /**< normal forward to cpu*/
    CTC_CPU_PKTTOCPU_MOD_ELOOP_FWD_CPU,    /**< do eloop and forward to cpu*/
    CTC_CPU_PKTTOCPU_MOD_RSV1,             /**< reserved for future*/
    CTC_CPU_PKTTOCPU_MOD_RSV2,             /**< reserved for future*/
    CTC_CPU_PKTTOCPU_MOD_RSV3,             /**< reserved for future*/
    MAX_YS_CPU_PKTTOCPU_MOD_FLAG,
};
typedef enum ctc_cpu_pkttocpu_mod_e ctc_cpu_pkttocpu_mod_t;

#endif /* _CTC_COMMON_H*/

