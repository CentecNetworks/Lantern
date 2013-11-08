/**
 @file sys_humber_linkagg.h

 @date 2009-10-19

 @version v2.0

 The file define APIs and types use in sys layer
*/
#ifndef _SYS_HUMBER_LINKAGG_H
#define _SYS_HUMBER_LINKAGG_H
/***************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_const.h"
#include "ctc_vector.h"
/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/
#define SYS_LINKAGG_BLOCK_NUM 16

#define LINKAGG_LOCK \
    if (p_linkagg_master->p_linkagg_mutex) kal_mutex_lock(p_linkagg_master->p_linkagg_mutex)
#define LINKAGG_UNLOCK \
    if (p_linkagg_master->p_linkagg_mutex) kal_mutex_unlock(p_linkagg_master->p_linkagg_mutex)

struct sys_linkagg_port_s
{
    uint8 valid;
    uint16 gport;
};

typedef struct sys_linkagg_port_s sys_linkagg_port_t;

struct sys_linkagg_s
{
    uint8 tid;
    uint8 port_cnt;
    uint16 resv;
    sys_linkagg_port_t port[CTC_MAX_LINKAGG_MEMBER_PORT];
};

typedef struct sys_linkagg_s sys_linkagg_t;

struct sys_linkagg_master_s
{
    ctc_vector_t *p_linkagg_vector;
    kal_mutex_t *p_linkagg_mutex;
};
typedef struct sys_linkagg_master_s sys_linkagg_master_t;

#define SYS_TID_VALID_CHECK(tid) \
    if (tid >= CTC_MAX_LINKAGG_GROUP_NUM) {return CTC_E_INVALID_TID;}


#define SYS_LINKAGG_DEBUG_INFO(FMT, ...) \
            { \
                CTC_DEBUG_OUT_INFO(linkagg, linkagg, LINKAGG_SYS, FMT, ##__VA_ARGS__);\
            }

#define SYS_LINKAGG_DEBUG_FUNC() \
            {\
                CTC_DEBUG_OUT_FUNC(linkagg, linkagg, LINKAGG_SYS);\
            }

/***************************************************************
 *
 *  Functions
 *
 ***************************************************************/
extern int32
sys_humber_linkagg_init(void);

extern int32
sys_humber_linkagg_create(uint8 tid);

extern int32
sys_humber_linkagg_remove(uint8 tid);

extern int32
sys_humber_linkagg_add_port(uint8 tid, uint16 gport);

extern int32
sys_humber_linkagg_remove_port(uint8 tid, uint16 gport);

extern int32
sys_humber_linkagg_get_1st_local_port(uint8 tid, uint16* p_gport,uint8 *local_cnt);

extern int32
sys_humber_linkagg_show_ports(uint8 tid, uint16 *p_gports, uint8 *cnt);

#endif
