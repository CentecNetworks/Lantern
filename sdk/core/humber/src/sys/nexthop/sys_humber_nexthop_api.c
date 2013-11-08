 /**
  @file sys_humber_nexthop_api.c

  @date 2009-11-11

  @version v2.0

  The file contains all nexthop API function for upper layer module
 */
/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_linklist.h"
#include "ctc_nexthop.h"
#include "sys_humber_chip.h"
#include "sys_humber_opf.h"
#include "sys_humber_vlan.h"

#include "sys_humber_nexthop_api.h"
#include "sys_humber_nexthop.h"
#include "sys_humber_queue_api.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/
#define SYS_NH_INIT_CHECK \
    {\
        if(sys_nexthop_initialized == FALSE) return CTC_E_NOT_INIT;\
    }

struct sys_hbnh_brguc_node_info_s
{
    uint32 used_flag;

    uint32 nhid_brguc;
    uint32 dsfwd_offset_brguc[CTC_MAX_LOCAL_CHIP_NUM];

    uint32 nhid_brguc_untagged;
    uint32 dsfwd_offset_brguc_untagged[CTC_MAX_LOCAL_CHIP_NUM];

    uint32 nhid_bypass;
    uint32 dsfwd_offset_bypass[CTC_MAX_LOCAL_CHIP_NUM];

    uint32 nhid_ram_pkt;
    uint32 dsfwd_offset_ram_packet[CTC_MAX_LOCAL_CHIP_NUM];

    uint32 nhid_srv_queue;
    uint32 dsfwd_offset_srv_queue[CTC_MAX_LOCAL_CHIP_NUM];
};
typedef struct sys_hbnh_brguc_node_info_s sys_hbnh_brguc_node_info_t;

struct sys_humber_brguc_info_s
{
    ctc_vector_t * brguc_vector;/*Maintains ucast bridge DB(no egress vlan trans)*/
    kal_mutex_t*  brguc_mutex;
};
typedef struct sys_humber_brguc_info_s sys_humber_brguc_info_t;

struct sys_humber_nh_api_master_s
{
    sys_humber_brguc_info_t brguc_info;
    uint32 max_external_nhid;
};
typedef struct sys_humber_nh_api_master_s sys_humber_nh_api_master_t;

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/
static sys_humber_nh_api_master_t * p_nh_api_master = NULL;
static bool sys_nexthop_initialized = FALSE;

static int32 _sys_humber_misc_nh_create(uint32 nhid, sys_humber_nh_type_t nh_param_type);



/****************************************************************************
 *
* Function
*
*****************************************************************************/
/**
 @brief This function is to initialize nexthop API data,
 */
int32
sys_humber_nh_api_init(ctc_nh_global_cfg_t * nh_cfg)
{

    uint32   vec_block_num  =0;
    uint32   ret = 0;
    if(sys_nexthop_initialized)
        return CTC_E_NONE;


    CTC_ERROR_RETURN(sys_humber_nh_init(nh_cfg));

     p_nh_api_master = (sys_humber_nh_api_master_t *)mem_malloc(MEM_NEXTHOP_MODULE, (sizeof(sys_humber_nh_api_master_t)));
     kal_memset(p_nh_api_master,0,(sizeof(sys_humber_nh_api_master_t)));
    /*1. Bridge Ucast Data init*/
    if(NULL != p_nh_api_master)
    {
        SYS_NH_CREAT_LOCK(p_nh_api_master->brguc_info.brguc_mutex);
    }
    else
    {
        free(p_nh_api_master);
        p_nh_api_master = NULL;
        return CTC_E_NO_MEMORY;
    }

    vec_block_num  = (256*31 + 128) / 64;  /*31*256 (physical port ) + 128 (linkagg) */
    p_nh_api_master->brguc_info.brguc_vector = ctc_vector_init(vec_block_num, 64);
    if(NULL ==  p_nh_api_master->brguc_info.brguc_vector)
    {
        free(p_nh_api_master);
        p_nh_api_master = NULL;
        return CTC_E_NO_MEMORY;
    }

    ret = sys_humer_nh_get_max_external_nhid(&p_nh_api_master->max_external_nhid);

    if(ret)
    {
        free(p_nh_api_master);
        p_nh_api_master = NULL;
        return ret;
    }
    sys_nexthop_initialized = TRUE;

    /*2. Create default nexthop*/
    CTC_ERROR_RETURN(_sys_humber_misc_nh_create(SYS_HUMBER_NH_RESOLVED_NHID_FOR_DROP, SYS_HUMBER_NH_TYPE_DROP));
    CTC_ERROR_RETURN(_sys_humber_misc_nh_create(SYS_HUMBER_NH_RESOLVED_NHID_FOR_TOCPU, SYS_HUMBER_NH_TYPE_TOCPU));



    return CTC_E_NONE;
}

/**
 @brief This function is to create normal bridge nexthop in different types

 @param[in] gport, global port id

 @param[in] nh_type, nexthop type

 @return CTC_E_XXX
 */
int32
_sys_humber_brguc_nh_create_by_type(uint16 gport ,sys_nh_param_brguc_sub_type_t nh_type)
{
    sys_nh_param_brguc_t brguc_nh;
    sys_hbnh_brguc_node_info_t *p_brguc_node;
    bool is_add = FALSE;

    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("gport = %d , nh_type = %d\n", gport,nh_type);

    kal_memset(&brguc_nh, 0, sizeof(sys_nh_param_brguc_t));
    brguc_nh.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_BRGUC;
    brguc_nh.hdr.nhid = SYS_HUMBER_NH_INVALID_NHID;
    brguc_nh.hdr.is_internal_nh = TRUE;
    brguc_nh.nh_sub_type = nh_type;
    brguc_nh.gport = gport;

    CTC_ERROR_RETURN(sys_humber_nh_api_create((sys_nh_param_com_t*)(&brguc_nh)));

    p_brguc_node = ctc_vector_get(p_nh_api_master->brguc_info.brguc_vector, gport);
    if( NULL == p_brguc_node )
    {
        p_brguc_node = (sys_hbnh_brguc_node_info_t*)mem_malloc(MEM_NEXTHOP_MODULE, sizeof(sys_hbnh_brguc_node_info_t));
        if(NULL == p_brguc_node)
        {
            return CTC_E_NO_MEMORY;
        }
        kal_memset(p_brguc_node,0,sizeof(sys_hbnh_brguc_node_info_t));
        is_add = TRUE;
    }

    if( nh_type == SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC )
    {
        p_brguc_node->nhid_brguc = brguc_nh.hdr.nhid;
        p_brguc_node->used_flag |= SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC;
        kal_memcpy(p_brguc_node->dsfwd_offset_brguc, brguc_nh.hdr.dsfwd_offset,
            sizeof(sys_nh_offset_array_t));
    }

    if( nh_type == SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED )
    {
        p_brguc_node->nhid_brguc_untagged = brguc_nh.hdr.nhid;
        p_brguc_node->used_flag |= SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED;
        kal_memcpy(p_brguc_node->dsfwd_offset_brguc_untagged, brguc_nh.hdr.dsfwd_offset,
            sizeof(sys_nh_offset_array_t));
    }

    if( nh_type == SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS )
    {
        p_brguc_node->nhid_bypass = brguc_nh.hdr.nhid;
        p_brguc_node->used_flag |= SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS;
        kal_memcpy(p_brguc_node->dsfwd_offset_bypass, brguc_nh.hdr.dsfwd_offset,
            sizeof(sys_nh_offset_array_t));
    }

    if( nh_type == SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU )
    {
        p_brguc_node->nhid_ram_pkt = brguc_nh.hdr.nhid;
        p_brguc_node->used_flag |= SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU;
        kal_memcpy(p_brguc_node->dsfwd_offset_ram_packet, brguc_nh.hdr.dsfwd_offset,
            sizeof(sys_nh_offset_array_t));
    }

    if( nh_type == SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE )
    {
        p_brguc_node->nhid_srv_queue = brguc_nh.hdr.nhid;
        p_brguc_node->used_flag |= SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE;
        kal_memcpy(p_brguc_node->dsfwd_offset_srv_queue, brguc_nh.hdr.dsfwd_offset,
            sizeof(sys_nh_offset_array_t));
    }

    if(is_add)
    {
        if(FALSE == ctc_vector_add(p_nh_api_master->brguc_info.brguc_vector,gport,p_brguc_node))
        {
            if( (p_brguc_node->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC) != 0 )
            {
                sys_humber_nh_api_delete(p_brguc_node->nhid_brguc, SYS_HUMBER_NH_TYPE_BRGUC);
                p_brguc_node->used_flag -= SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC;
            }

            if( (p_brguc_node->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED) != 0 )
            {
                sys_humber_nh_api_delete(p_brguc_node->nhid_brguc_untagged, SYS_HUMBER_NH_TYPE_BRGUC);
                p_brguc_node->used_flag -= SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED;
            }

            if( (p_brguc_node->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS) != 0 )
            {
                sys_humber_nh_api_delete(p_brguc_node->nhid_bypass, SYS_HUMBER_NH_TYPE_BRGUC);
                p_brguc_node->used_flag -= SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS;
            }

            if( (p_brguc_node->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU) != 0 )
            {
                sys_humber_nh_api_delete(p_brguc_node->nhid_ram_pkt, SYS_HUMBER_NH_TYPE_BRGUC);
                p_brguc_node->used_flag -= SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU;
            }

            if( (p_brguc_node->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE) != 0 )
            {
                sys_humber_nh_api_delete(p_brguc_node->nhid_srv_queue, SYS_HUMBER_NH_TYPE_BRGUC);
                p_brguc_node->used_flag -= SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE;
            }

            mem_free(p_brguc_node);
            return CTC_E_NO_MEMORY;
        }
    }
    return CTC_E_NONE;
}

/**
 @brief This function is to create normal bridge nexthop

 @param[in] gport, global port id

 @param[in] nh_type, nexthop type

 @return CTC_E_XXX
 */
int32
sys_humber_brguc_nh_create(uint16 gport ,ctc_nh_param_brguc_sub_type_t nh_type )
{
    sys_hbnh_brguc_node_info_t* p_brguc_node;
    bool enable = 0;

    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("gport = %d , nh_type = %d\n", gport,nh_type);

    CTC_ERROR_RETURN(sys_humber_get_service_queue_enable(&enable));

    SYS_NH_INIT_CHECK;
    CTC_GLOBAL_PORT_CHECK(gport);
    SYS_NH_LOCK(p_nh_api_master->brguc_info.brguc_mutex);

    p_brguc_node = ctc_vector_get(p_nh_api_master->brguc_info.brguc_vector, gport);
    if (NULL != p_brguc_node)
    {
                SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
                return CTC_E_ENTRY_EXIST;
    }

    if( ((nh_type & CTC_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE) != 0) || (nh_type == CTC_NH_PARAM_BRGUC_SUB_TYPE_NONE )  )
    {
        if(enable)
        {
            CTC_ERROR_RETURN_WITH_UNLOCK(_sys_humber_brguc_nh_create_by_type(gport,SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE)
                ,p_nh_api_master->brguc_info.brguc_mutex);
        }
    }

    if( ((nh_type & CTC_NH_PARAM_BRGUC_SUB_TYPE_BASIC) != 0) || (nh_type == CTC_NH_PARAM_BRGUC_SUB_TYPE_NONE ) )
        CTC_ERROR_RETURN_WITH_UNLOCK(_sys_humber_brguc_nh_create_by_type(gport,SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC)
            ,p_nh_api_master->brguc_info.brguc_mutex);

    if( ((nh_type & CTC_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED) != 0) || (nh_type == CTC_NH_PARAM_BRGUC_SUB_TYPE_NONE )  )
        CTC_ERROR_RETURN_WITH_UNLOCK(_sys_humber_brguc_nh_create_by_type(gport,SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED)
            ,p_nh_api_master->brguc_info.brguc_mutex);

    if( ((nh_type & CTC_NH_PARAM_BRGUC_SUB_TYPE_BYPASS) != 0) || (nh_type == CTC_NH_PARAM_BRGUC_SUB_TYPE_NONE )  )
        CTC_ERROR_RETURN_WITH_UNLOCK(_sys_humber_brguc_nh_create_by_type(gport,SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS)
            ,p_nh_api_master->brguc_info.brguc_mutex);

    if( ((nh_type & CTC_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU) != 0) || (nh_type == CTC_NH_PARAM_BRGUC_SUB_TYPE_NONE )  )
        CTC_ERROR_RETURN_WITH_UNLOCK(_sys_humber_brguc_nh_create_by_type(gport,SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU)
            ,p_nh_api_master->brguc_info.brguc_mutex);

    SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);

    return CTC_E_NONE;
}

/**
 @brief This function is to delete normal bridge nexthop

 @param[in] gport, global port id

 @param[in] nh_type, nexthop type

 @return CTC_E_XXX
 */
int32
sys_humber_brguc_nh_delete(uint16 gport)
{

    sys_hbnh_brguc_node_info_t* p_brguc_node;
    int32 ret = 0;

    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("gport = %d\n", gport);

    SYS_NH_INIT_CHECK;
    CTC_GLOBAL_PORT_CHECK(gport);

    SYS_NH_LOCK(p_nh_api_master->brguc_info.brguc_mutex);


   p_brguc_node = ctc_vector_get(p_nh_api_master->brguc_info.brguc_vector, gport);
    if (NULL == p_brguc_node)
    {
        SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
        return CTC_E_NH_NOT_EXIST;
    }

    if( (p_brguc_node->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC) != 0 )
    {
        ret = (sys_humber_nh_api_delete(p_brguc_node->nhid_brguc, SYS_HUMBER_NH_TYPE_BRGUC));
        p_brguc_node->nhid_brguc = SYS_HUMBER_NH_INVALID_NHID;
        kal_memset(p_brguc_node->dsfwd_offset_brguc, 0, sizeof(sys_nh_offset_array_t));
        p_brguc_node->used_flag -= SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC;
    }

    if( ((p_brguc_node->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED) != 0) )
    {
        ret = ret ? ret : (sys_humber_nh_api_delete(p_brguc_node->nhid_brguc_untagged, SYS_HUMBER_NH_TYPE_BRGUC));
        p_brguc_node->nhid_brguc_untagged = SYS_HUMBER_NH_INVALID_NHID;
        kal_memset(p_brguc_node->dsfwd_offset_brguc_untagged, 0, sizeof(sys_nh_offset_array_t));
        p_brguc_node->used_flag -= SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED;
    }

    if( ((p_brguc_node->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS) != 0) )
    {
        ret = ret ? ret : (sys_humber_nh_api_delete(p_brguc_node->nhid_bypass, SYS_HUMBER_NH_TYPE_BRGUC));
        p_brguc_node->nhid_brguc_untagged = SYS_HUMBER_NH_INVALID_NHID;
        kal_memset(p_brguc_node->dsfwd_offset_bypass, 0, sizeof(sys_nh_offset_array_t));
        p_brguc_node->used_flag -= SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS;
    }

    if( ((p_brguc_node->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU) != 0) )
    {
        ret = ret ? ret : (sys_humber_nh_api_delete(p_brguc_node->nhid_ram_pkt, SYS_HUMBER_NH_TYPE_BRGUC));
        p_brguc_node->nhid_brguc_untagged = SYS_HUMBER_NH_INVALID_NHID;
        kal_memset(p_brguc_node->dsfwd_offset_ram_packet, 0, sizeof(sys_nh_offset_array_t));
        p_brguc_node->used_flag -= SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU;
    }

    if( ((p_brguc_node->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE) != 0) )
    {
        ret = ret ? ret : (sys_humber_nh_api_delete(p_brguc_node->nhid_srv_queue, SYS_HUMBER_NH_TYPE_BRGUC));
        p_brguc_node->nhid_brguc_untagged = SYS_HUMBER_NH_INVALID_NHID;
        kal_memset(p_brguc_node->dsfwd_offset_srv_queue, 0, sizeof(sys_nh_offset_array_t));
        p_brguc_node->used_flag -= SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE;
    }

    if( p_brguc_node->used_flag == 0 )
    {
        ctc_vector_del(p_nh_api_master->brguc_info.brguc_vector,gport);
        mem_free(p_brguc_node);
    }

     SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
    return ret;
}


/**
 @brief This function is to get dsfwd offset

 @param[in] gport, global port id

 @param[out] offset_array, dsfwd offset array

 @return CTC_E_XXX
 */
int32
sys_humber_brguc_get_dsfwd_offset(uint16 gport, uint8 untagged,sys_nh_offset_array_t offset_array)
{
    sys_hbnh_brguc_node_info_t *p_brguc_db = NULL;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("gport = %d\n", gport);

     SYS_NH_INIT_CHECK;
    CTC_GLOBAL_PORT_CHECK(gport);

    SYS_NH_LOCK(p_nh_api_master->brguc_info.brguc_mutex);

     p_brguc_db = ctc_vector_get(p_nh_api_master->brguc_info.brguc_vector, gport);
    if (NULL == p_brguc_db)
    {
        SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
        return CTC_E_NH_NOT_EXIST;
    }
    if(untagged)
    {
        if( 0 == (p_brguc_db->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED) )
        {
            SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
            return CTC_E_NH_NOT_EXIST;
        }
        kal_memcpy(offset_array, p_brguc_db->dsfwd_offset_brguc_untagged, sizeof(sys_nh_offset_array_t));

    }
    else
    {
        if( 0 == (p_brguc_db->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC) )
        {
            SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
            return CTC_E_NH_NOT_EXIST;
        }
         kal_memcpy(offset_array, p_brguc_db->dsfwd_offset_brguc, sizeof(sys_nh_offset_array_t));


    }

    SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);

    return CTC_E_NONE;
}

/**
 @brief This function is to get dsfwd offset

 @param[in] gport, global port id

 @param[out] offset_array, dsfwd offset array

 @return CTC_E_XXX
 */
int32
sys_humber_bypass_get_dsfwd_offset(uint16 gport, sys_nh_offset_array_t offset_array)
{
    sys_hbnh_brguc_node_info_t *p_brguc_db = NULL;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("gport = %d\n", gport);

     SYS_NH_INIT_CHECK;
    CTC_GLOBAL_PORT_CHECK(gport);
    SYS_NH_LOCK(p_nh_api_master->brguc_info.brguc_mutex);

     p_brguc_db = ctc_vector_get(p_nh_api_master->brguc_info.brguc_vector, gport);
    if (NULL == p_brguc_db)
    {
        SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
        return CTC_E_NH_NOT_EXIST;
    }
    if( 0 == (p_brguc_db->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS) )
    {
        SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
        return CTC_E_NH_NOT_EXIST;
    }
    kal_memcpy(offset_array, p_brguc_db->dsfwd_offset_bypass, sizeof(sys_nh_offset_array_t));

    SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);

    return CTC_E_NONE;
}

/**
 @brief This function is to get dsfwd offset,it can be used to send raw protocol to cpu( or oam engine). (802.1ag/Y1731 LBM/DMM/LMM)

 @param[in] gport, global port id

 @param[out] offset_array, dsfwd offset array

 @return CTC_E_XXX
 */
int32
sys_humber_rawpkt_elog_cpu_get_dsfwd_offset(uint16 gport, sys_nh_offset_array_t offset_array)
{
     sys_hbnh_brguc_node_info_t *p_brguc_db = NULL;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("gport = %d\n", gport);

    SYS_NH_INIT_CHECK;
    CTC_GLOBAL_PORT_CHECK(gport);
    SYS_NH_LOCK(p_nh_api_master->brguc_info.brguc_mutex);

    p_brguc_db = ctc_vector_get(p_nh_api_master->brguc_info.brguc_vector, gport);
    if (NULL == p_brguc_db)
    {
        SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
        return CTC_E_NH_NOT_EXIST;
    }

    if( 0 == (p_brguc_db->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU) )
    {
        SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
        return CTC_E_NH_NOT_EXIST;
    }
    kal_memcpy(offset_array, p_brguc_db->dsfwd_offset_ram_packet, sizeof(sys_nh_offset_array_t));

    SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);

    return CTC_E_NONE;
}


/**
 @brief This function is to get dsfwd offset,it can be used to enqueue by service id

 @param[in] gport, global port id

 @param[out] offset_array, dsfwd offset array

 @return CTC_E_XXX
 */
int32
sys_humber_srv_queue_get_dsfwd_offset(uint16 gport, sys_nh_offset_array_t offset_array)
{
     sys_hbnh_brguc_node_info_t *p_brguc_db = NULL;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("gport = %d\n", gport);

    SYS_NH_INIT_CHECK;
    CTC_GLOBAL_PORT_CHECK(gport);
    SYS_NH_LOCK(p_nh_api_master->brguc_info.brguc_mutex);

    p_brguc_db = ctc_vector_get(p_nh_api_master->brguc_info.brguc_vector, gport);
    if (NULL == p_brguc_db)
    {
        SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
        return CTC_E_NH_NOT_EXIST;
    }

    if( 0 == (p_brguc_db->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE) )
    {
        SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
        return CTC_E_NH_NOT_EXIST;
    }
    kal_memcpy(offset_array, p_brguc_db->dsfwd_offset_srv_queue, sizeof(sys_nh_offset_array_t));

    SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);

    return CTC_E_NONE;
}



int32
 sys_humber_brguc_get_nhid(uint16 gport, sys_nh_brguc_nhid_info_t* p_brguc_nhid_info)
{
    sys_hbnh_brguc_node_info_t *p_brguc_db = NULL;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("gport = %d\n", gport);

    SYS_NH_INIT_CHECK;
    CTC_GLOBAL_PORT_CHECK(gport);

    SYS_NH_LOCK(p_nh_api_master->brguc_info.brguc_mutex);

    p_brguc_db = ctc_vector_get(p_nh_api_master->brguc_info.brguc_vector, gport);
    if (NULL == p_brguc_db)
    {
        SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
        return CTC_E_NH_NOT_EXIST;
    }

    if((p_brguc_nhid_info->nh_type_flag == SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC)
        || (p_brguc_nhid_info->nh_type_flag == SYS_NH_PARAM_BRGUC_SUB_TYPE_NONE) )
    {
        if( (p_brguc_db->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC) != 0)
        {
            p_brguc_nhid_info->brguc_nhid = p_brguc_db->nhid_brguc;
        }
        else
        {
            p_brguc_nhid_info->brguc_nhid = SYS_HUMBER_NH_INVALID_NHID;
            SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
            return CTC_E_NH_NOT_EXIST;
        }
    }

    if((p_brguc_nhid_info->nh_type_flag == SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS)
        || (p_brguc_nhid_info->nh_type_flag == SYS_NH_PARAM_BRGUC_SUB_TYPE_NONE) )
    {
        if( (p_brguc_db->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS) != 0)
        {
            p_brguc_nhid_info->bypass_nhid = p_brguc_db->nhid_bypass;
        }
        else
        {
            p_brguc_nhid_info->bypass_nhid = SYS_HUMBER_NH_INVALID_NHID;
            SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
            return CTC_E_NH_NOT_EXIST;
        }
    }

    if((p_brguc_nhid_info->nh_type_flag == SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU)
        || (p_brguc_nhid_info->nh_type_flag == SYS_NH_PARAM_BRGUC_SUB_TYPE_NONE) )
    {
        if( (p_brguc_db->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU) != 0)
        {
            p_brguc_nhid_info->ram_pkt_nhid = p_brguc_db->nhid_ram_pkt;
        }
        else
        {
            p_brguc_nhid_info->ram_pkt_nhid = SYS_HUMBER_NH_INVALID_NHID;
            SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
            return CTC_E_NH_NOT_EXIST;
        }
    }


    if((p_brguc_nhid_info->nh_type_flag == SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED)
        || (p_brguc_nhid_info->nh_type_flag == SYS_NH_PARAM_BRGUC_SUB_TYPE_NONE) )
    {
        if( (p_brguc_db->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED) != 0)
        {
            p_brguc_nhid_info->brguc_untagged_nhid = p_brguc_db->nhid_brguc_untagged;
        }
        else
        {
            p_brguc_nhid_info->brguc_untagged_nhid = SYS_HUMBER_NH_INVALID_NHID;
            SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
            return CTC_E_NH_NOT_EXIST;
        }
    }

    if((p_brguc_nhid_info->nh_type_flag == SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE)
        || (p_brguc_nhid_info->nh_type_flag == SYS_NH_PARAM_BRGUC_SUB_TYPE_NONE) )
    {
        if( (p_brguc_db->used_flag & SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE) != 0)
        {
            p_brguc_nhid_info->srv_queue_nhid = p_brguc_db->nhid_srv_queue;
        }
        else
        {
            p_brguc_nhid_info->srv_queue_nhid = SYS_HUMBER_NH_INVALID_NHID;
            SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);
            return CTC_E_NH_NOT_EXIST;
        }
    }

    SYS_NH_UNLOCK(p_nh_api_master->brguc_info.brguc_mutex);

    return CTC_E_NONE;
}

int32
sys_humber_l2_get_ucast_nh(uint16 gport, ctc_nh_param_brguc_sub_type_t nh_type, uint32* nhid)
{
    sys_nh_brguc_nhid_info_t brguc_nhid_info;

    kal_memset(&brguc_nhid_info, 0, sizeof(brguc_nhid_info));

    switch(nh_type)
    {
        case CTC_NH_PARAM_BRGUC_SUB_TYPE_BASIC:
            brguc_nhid_info.nh_type_flag = SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC;
            break;
        case CTC_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED:
            brguc_nhid_info.nh_type_flag = SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED;
            break;
        case CTC_NH_PARAM_BRGUC_SUB_TYPE_BYPASS:
            brguc_nhid_info.nh_type_flag = SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS;
            break;
        case CTC_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU:
            brguc_nhid_info.nh_type_flag = SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU;
            break;
        case CTC_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE:
            brguc_nhid_info.nh_type_flag = SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE;
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }

    CTC_ERROR_RETURN(sys_humber_brguc_get_nhid(gport, &brguc_nhid_info));

    switch(nh_type)
    {
        case CTC_NH_PARAM_BRGUC_SUB_TYPE_BASIC:
            *nhid = brguc_nhid_info.brguc_nhid;
            break;
        case CTC_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED:
            *nhid = brguc_nhid_info.brguc_untagged_nhid;
            break;
        case CTC_NH_PARAM_BRGUC_SUB_TYPE_BYPASS:
            *nhid = brguc_nhid_info.bypass_nhid;
            break;
        case CTC_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU:
            *nhid = brguc_nhid_info.ram_pkt_nhid;
            break;
        case CTC_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE:
            *nhid = brguc_nhid_info.srv_queue_nhid;
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

/**
 @brief This function is to create egress vlan edit nexthop

 @param[in] nhid, nexthop id of this nexthop

 @param[in] gport, global port id

 @param[in] p_vlan_info, vlan edit information

 @param[in] dsnh_offset, nexthop offset of this nexthop

 @return CTC_E_XXX
 */
int32
sys_humber_egress_vlan_edit_nh_create(uint32 nhid, uint16 gport,
            ctc_vlan_egress_edit_info_t *p_vlan_info, uint32 dsnh_offset)
{
    sys_nh_param_brguc_t nh_brg;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d, gport = %d, dsnh_offset = %d",
        nhid, gport, dsnh_offset);

    SYS_NH_INIT_CHECK;
    CTC_GLOBAL_PORT_CHECK(gport);
    SYS_NHID_EXTERNAL_VALID_CHECK(nhid);
    CTC_PTR_VALID_CHECK(p_vlan_info);
    if(CTC_FLAG_ISSET(p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_CVID_VALID))
    {
        CTC_VLAN_RANGE_CHECK(p_vlan_info->output_cvid);
    }
    if(CTC_FLAG_ISSET(p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_SVID_VALID))
    {
        CTC_VLAN_RANGE_CHECK(p_vlan_info->output_svid);
    }
    CTC_ERROR_RETURN(sys_humber_nh_check_max_glb_nh_sram_offset(dsnh_offset));
    kal_memset(&nh_brg, 0, sizeof(sys_nh_param_brguc_t));

    nh_brg.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_BRGUC;
    nh_brg.hdr.is_internal_nh = FALSE;
    nh_brg.hdr.nhid = nhid;
    nh_brg.nh_sub_type = SYS_NH_PARAM_BRGUC_SUB_TYPE_VLAN_EDIT;
    nh_brg.gport = gport;
    nh_brg.p_vlan_edit_info = p_vlan_info;
    nh_brg.dsnh_offset = dsnh_offset;

    CTC_ERROR_RETURN(sys_humber_nh_api_create((sys_nh_param_com_t*)(&nh_brg)));

    return CTC_E_NONE;
}

/**
 @brief This function is to delete egress vlan edit nexthop

 @param[in] nhid, nexthop id of this nexthop

 @return CTC_E_XXX
 */
int32
sys_humber_egress_vlan_edit_nh_delete(uint32 nhid)
{
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d\n", nhid);

    SYS_NH_INIT_CHECK;
    SYS_NHID_EXTERNAL_VALID_CHECK(nhid);
    CTC_ERROR_RETURN(sys_humber_nh_api_delete(nhid, SYS_HUMBER_NH_TYPE_BRGUC));
    return CTC_E_NONE;
}

/**
 @brief This function is to create aps egress vlan edit nexthop

 @param[in] nhid, nexthop id of this nexthop

 @param[in] gport, global port id

 @param[in] p_vlan_info, vlan edit information

 @param[in] dsnh_offset, nexthop offset of this nexthop

 @return CTC_E_XXX
 */
int32
sys_humber_aps_egress_vlan_edit_nh_create(uint32 nhid,
            uint32 dsnh_offset, uint16 aps_bridge_id,
            ctc_vlan_egress_edit_info_t *p_vlan_info_working_path,
            ctc_vlan_egress_edit_info_t *p_vlan_info_protection_path)
{
    sys_nh_param_brguc_t nh_brg;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d, apsBridgeId = %d, dsnh_offset = %d",
        nhid, aps_bridge_id, dsnh_offset);

    /*CTC_GLOBAL_PORT_CHECK(gport); Check APS bridgeId ?*/
    SYS_NH_INIT_CHECK;
    SYS_NHID_EXTERNAL_VALID_CHECK(nhid);
    CTC_PTR_VALID_CHECK(p_vlan_info_working_path);
    CTC_PTR_VALID_CHECK(p_vlan_info_protection_path);
    if(CTC_FLAG_ISSET(p_vlan_info_working_path->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_CVID_VALID))
    {
        CTC_VLAN_RANGE_CHECK(p_vlan_info_working_path->output_cvid);
    }
    if(CTC_FLAG_ISSET(p_vlan_info_working_path->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_SVID_VALID))
    {
        CTC_VLAN_RANGE_CHECK(p_vlan_info_working_path->output_svid);
    }
    if(CTC_FLAG_ISSET(p_vlan_info_protection_path->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_CVID_VALID))
    {
        CTC_VLAN_RANGE_CHECK(p_vlan_info_protection_path->output_cvid);
    }
    if(CTC_FLAG_ISSET(p_vlan_info_protection_path->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_SVID_VALID))
    {
        CTC_VLAN_RANGE_CHECK(p_vlan_info_protection_path->output_svid);
    }
    CTC_ERROR_RETURN(sys_humber_nh_check_max_glb_nh_sram_offset(dsnh_offset + 1));/*Aps nh will consume 2 continues offsets*/
    kal_memset(&nh_brg, 0, sizeof(sys_nh_param_brguc_t));

    nh_brg.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_BRGUC;
    nh_brg.hdr.is_internal_nh = FALSE;
    nh_brg.hdr.nhid = nhid;
    nh_brg.nh_sub_type = SYS_NH_PARAM_BRGUC_SUB_TYPE_APS_VLAN_EDIT;
    nh_brg.gport = aps_bridge_id;
    nh_brg.p_vlan_edit_info = p_vlan_info_working_path;
    nh_brg.p_vlan_edit_info_prot_path = p_vlan_info_protection_path;
    nh_brg.dsnh_offset = dsnh_offset;

    CTC_ERROR_RETURN(sys_humber_nh_api_create((sys_nh_param_com_t*)(&nh_brg)));

    return CTC_E_NONE;
}

/**
 @brief This function is to delete egress vlan edit nexthop

 @param[in] nhid, nexthop id of this nexthop

 @return CTC_E_XXX
 */
int32
sys_humber_aps_egress_vlan_edit_nh_delete(uint32 nhid)
{
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d\n", nhid);

    SYS_NH_INIT_CHECK;
    SYS_NHID_EXTERNAL_VALID_CHECK(nhid);
    CTC_ERROR_RETURN(sys_humber_nh_api_delete(nhid, SYS_HUMBER_NH_TYPE_BRGUC));



    return CTC_E_NONE;
}

extern int32
sys_humber_flex_nh_create(uint32 nhid, ctc_flex_nh_param_t* p_nh_param)
{
   sys_nh_param_flex_t nh_flex;

    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d, flag = %d, vlan_id = %d",
               nhid,p_nh_param->flag, p_nh_param->vlan_id);

    SYS_NH_INIT_CHECK;
    CTC_PTR_VALID_CHECK(p_nh_param);
    SYS_NHID_EXTERNAL_VALID_CHECK(nhid);

    kal_memset(&nh_flex, 0, sizeof(sys_nh_param_flex_t));

    nh_flex.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_FLEX;
    nh_flex.hdr.is_internal_nh = FALSE;
    nh_flex.hdr.nhid = nhid;
    kal_memcpy(&nh_flex.nh_flex_param,p_nh_param,sizeof(ctc_flex_nh_param_t));
    CTC_ERROR_RETURN(sys_humber_nh_api_create((sys_nh_param_com_t*)(&nh_flex)));

     return CTC_E_NONE;

}
extern int32
sys_humber_flex_nh_remove(uint32 nhid)
{
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d\n", nhid);

    SYS_NH_INIT_CHECK;
    SYS_NHID_EXTERNAL_VALID_CHECK(nhid);
    CTC_ERROR_RETURN(sys_humber_nh_api_delete(nhid, SYS_HUMBER_NH_TYPE_FLEX));

    return CTC_E_NONE;

}

/**
 @brief This function is to create ipuc nexthop

 @param[in] nhid, nexthop ID

 @param[in] p_member_list, member list to be added to this nexthop

 @return CTC_E_XXX
 */
int32
sys_humber_ipuc_nh_create(uint32 nhid, uint32 dsnh_offset, ctc_nh_oif_info_t*p_oif,
                          mac_addr_t mac, bool is_unrov)
{
    sys_nh_param_ipuc_t nh_param;
    SYS_NH_DBG_FUNC();

    SYS_NH_INIT_CHECK;
    SYS_NHID_EXTERNAL_VALID_CHECK(nhid);
    if(!is_unrov)
    {
        CTC_PTR_VALID_CHECK(p_oif);
        CTC_GLOBAL_PORT_CHECK(p_oif->gport);
        CTC_MAX_VALUE_CHECK(p_oif->vid, CTC_MAX_VLAN_ID);
    }
    CTC_ERROR_RETURN(sys_humber_nh_check_max_glb_nh_sram_offset(dsnh_offset));
    SYS_NH_DBG_INFO("nhid = %d, dsnhoffset = %d, gport = %d, vid = %d, if_type = %d, mac = %02x:%02x:%02x:%02x:%02x:%02x\n",
        nhid, dsnh_offset, p_oif->gport, p_oif->vid, p_oif->oif_type, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    kal_memset(&nh_param, 0, sizeof(sys_nh_param_ipuc_t));
    nh_param.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_IPUC;
    nh_param.hdr.is_internal_nh = FALSE;
    nh_param.hdr.nhid = nhid;
    nh_param.dsnh_offset = dsnh_offset;
    kal_memcpy(&nh_param.oif, p_oif, sizeof(ctc_nh_oif_info_t));
    nh_param.is_unrov_nh = is_unrov;
    kal_memcpy(nh_param.mac, mac, sizeof(mac_addr_t));

    CTC_ERROR_RETURN(sys_humber_nh_api_create((sys_nh_param_com_t*)(&nh_param)));

    return CTC_E_NONE;
}

/**
 @brief This function is to remove member from multicast bridge nexthop

 @param[in] nhid, nexthop id

 @return CTC_E_XXX
 */
int32
sys_humber_ipuc_nh_delete(uint32 nhid)
{
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d\n", nhid);

    SYS_NH_INIT_CHECK;
    CTC_ERROR_RETURN(sys_humber_nh_api_delete(nhid, SYS_HUMBER_NH_TYPE_IPUC));
    return CTC_E_NONE;
}

/**
 @brief This function is to update ipuc nexthop

 @param[in] nhid, nexthop id
 @param[in] vid, vlan  id
 @param[in] gport, global destination port
 @param[in] mac, nexthop mac address
 @param[in] update_type, nexthop update type

 @return CTC_E_XXX
 */
int32
sys_humber_ipuc_nh_update(uint32 nhid, ctc_nh_oif_info_t *p_oif, mac_addr_t mac,
                          sys_nh_entry_change_type_t update_type)
{
    sys_nh_param_ipuc_t nh_param;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d\n", nhid);

    SYS_NH_INIT_CHECK;
    CTC_PTR_VALID_CHECK(p_oif);
    kal_memset(&nh_param, 0, sizeof(sys_nh_param_ipuc_t));
    nh_param.hdr.nhid = nhid;
    nh_param.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_IPUC;
    nh_param.change_type = update_type;
    kal_memcpy(&nh_param.oif, p_oif, sizeof(ctc_nh_oif_info_t));
    kal_memcpy(&nh_param.mac, mac, sizeof(mac_addr_t));

    CTC_ERROR_RETURN(sys_humber_nh_api_update(nhid, (sys_nh_param_com_t*)(&nh_param)));
    return CTC_E_NONE;
}

/**
 @brief This function is to create mcast nexthop

 @param[in] groupid, basic met offset of this multicast nexthop

 @param[in] p_nh_mcast_group,  group and member infomation this multicast nexthop

 @return CTC_E_XXX
 */

int32
sys_humber_mcast_nh_create(uint32 groupid, sys_nh_param_mcast_group_t  *p_nh_mcast_group)
{
    sys_nh_param_mcast_t nh_mcast;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("groupid = %d\n", groupid);

    /*Sanity Check*/
    SYS_NH_INIT_CHECK;
    CTC_PTR_VALID_CHECK(p_nh_mcast_group);

    CTC_ERROR_RETURN(sys_humber_nh_check_max_glb_met_sram_offset(groupid));
    SYS_LOCAL_CHIPID_CHECK(p_nh_mcast_group->mem_info.lchip);

    kal_memset(&nh_mcast, 0, sizeof(sys_nh_param_mcast_t));
    nh_mcast.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_MCAST;
    if (p_nh_mcast_group->is_nhid_valid)
    {
        nh_mcast.hdr.nhid = p_nh_mcast_group->nhid;
        nh_mcast.hdr.is_internal_nh = FALSE;
    }
    else
    {
        nh_mcast.hdr.nhid = SYS_HUMBER_NH_INVALID_NHID;
        nh_mcast.hdr.is_internal_nh = TRUE;
    }
    nh_mcast.hdr.stats_valid = p_nh_mcast_group->is_stats_enable;
    nh_mcast.groupid = groupid;
    nh_mcast.p_member = &(p_nh_mcast_group->mem_info);
    nh_mcast.opcode = p_nh_mcast_group->opcode;
    CTC_ERROR_RETURN(sys_humber_nh_api_create((sys_nh_param_com_t*)(&nh_mcast)));
    kal_memcpy(p_nh_mcast_group->fwd_offset, nh_mcast.hdr.dsfwd_offset, sizeof(sys_nh_offset_array_t));
    kal_memcpy(p_nh_mcast_group->stats_ptr, nh_mcast.hdr.stats_ptr, sizeof(sys_nh_u16_array_t));
    p_nh_mcast_group->nhid = nh_mcast.hdr.nhid;

    return CTC_E_NONE;
}


/**
 @brief This function is to update mcast nexthop

 @param[in] nhid,  nexthopid

 @return CTC_E_XXX
 */

int32
sys_humber_mcast_nh_delete(uint32 nhid)
{
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d\n", nhid);

    SYS_NH_INIT_CHECK;
    CTC_ERROR_RETURN(sys_humber_nh_api_delete(nhid, SYS_HUMBER_NH_TYPE_MCAST));
    return CTC_E_NONE;
}


/**
 @brief This function is to update mcast nexthop

 @param[in] p_nh_mcast_group,  group and member infomation this multicast nexthop

 @return CTC_E_XXX
 */

int32
sys_humber_mcast_nh_update(sys_nh_param_mcast_group_t *p_nh_mcast_group)
{
    sys_nh_param_mcast_t nh_mcast;
    SYS_NH_DBG_FUNC();

    SYS_NH_INIT_CHECK;
    CTC_PTR_VALID_CHECK(p_nh_mcast_group);
    SYS_LOCAL_CHIPID_CHECK(p_nh_mcast_group->mem_info.lchip);

    SYS_NH_DBG_INFO("nhid = %d\n", p_nh_mcast_group->nhid);

    kal_memset(&nh_mcast, 0, sizeof(sys_nh_param_mcast_t));
    nh_mcast.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_MCAST;
    nh_mcast.p_member = &(p_nh_mcast_group->mem_info);
    nh_mcast.hdr.nhid = p_nh_mcast_group->nhid;
    nh_mcast.opcode = p_nh_mcast_group->opcode;

    CTC_ERROR_RETURN(sys_humber_nh_api_update(p_nh_mcast_group->nhid,
        (sys_nh_param_com_t*)(&nh_mcast)));

    return CTC_E_NONE;
}


/**
 @brief This function is to create mpls nexthop

 @param[in] nhid  nexthop ID

 @param[in] dsnh_offset  DsNexthop offset used by this nexthop

 @param[in] p_nh_param  Nexthop param used to create this nexthop

 @return CTC_E_XXX
 */
int32
sys_humber_mpls_nh_create(uint32 nhid, uint32 dsnh_offset,
                          bool use_nh8w, bool is_unrov,
                          sys_nh_mpls_param_t *p_nh_param,
                          sys_nh_mpls_param_t *p_nh_param_protection)
{
    sys_nh_param_mpls_t nh_param;
    SYS_NH_DBG_FUNC();

    SYS_NH_INIT_CHECK;
    SYS_NHID_EXTERNAL_VALID_CHECK(nhid);
    CTC_PTR_VALID_CHECK(p_nh_param);
    CTC_ERROR_RETURN(sys_humber_nh_check_max_glb_nh_sram_offset(dsnh_offset));

    SYS_NH_DBG_INFO("nhid = %d, dsnhoffset = %d\n", nhid, dsnh_offset);
    kal_memset(&nh_param, 0, sizeof(sys_nh_param_mpls_t));
    nh_param.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_MPLS;
    nh_param.hdr.is_internal_nh = FALSE;
    nh_param.hdr.nhid = nhid;
    nh_param.dsnh_offset = dsnh_offset;
    nh_param.p_mpls_nh_param = p_nh_param;
    nh_param.p_mpls_nh_param_protection = p_nh_param_protection;
    nh_param.use_dsnh8w = use_nh8w;
    nh_param.is_unrov = is_unrov;

    CTC_ERROR_RETURN(sys_humber_nh_api_create((sys_nh_param_com_t*)(&nh_param)));

    return CTC_E_NONE;
}

/**
 @brief This function is to remove mpls nexthop by nexthop id

 @param[in] nhid nexthop id

 @return CTC_E_XXX
 */
int32
sys_humber_mpls_nh_delete(uint32 nhid)
{
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d\n", nhid);

    SYS_NH_INIT_CHECK;
    CTC_ERROR_RETURN(sys_humber_nh_api_delete(nhid, SYS_HUMBER_NH_TYPE_MPLS));
    return CTC_E_NONE;
}

/**
 @brief This function is to update mpls nexthop

 @param[in] nhid, nexthop id

 @param[in] p_nh_param nexthop paramter

 @return CTC_E_XXX
 */
int32
sys_humber_mpls_nh_update(uint32 nhid, sys_nh_mpls_param_t *p_nh_param,
                                    sys_nh_mpls_param_t *p_nh_param_protection,
                                    sys_nh_entry_change_type_t change_type)
{
    sys_nh_param_mpls_t nh_param;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d\n", nhid);

    SYS_NH_INIT_CHECK;
    kal_memset(&nh_param, 0, sizeof(sys_nh_param_mpls_t));
    nh_param.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_MPLS;
    nh_param.hdr.nhid = nhid;
    nh_param.p_mpls_nh_param = p_nh_param;
    nh_param.p_mpls_nh_param_protection = p_nh_param_protection;
    nh_param.change_type = change_type;
    CTC_ERROR_RETURN(sys_humber_nh_api_update(nhid, (sys_nh_param_com_t*)(&nh_param)));

    return CTC_E_NONE;
}

/**
 @brief This function is to create mist nexthop, include drop nh, tocpu nh and unresolve nh

 @param[in] nhid, nexthop id

 @param[in] nh_param_type, misc nexthop type

 @return CTC_E_XXX
 */
static int32
_sys_humber_misc_nh_create(uint32 nhid, sys_humber_nh_type_t nh_param_type)
{
    sys_nh_param_special_t nh_para;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d\n", nhid);


    switch(nh_param_type)
    {
        case SYS_HUMBER_NH_TYPE_DROP:
        case SYS_HUMBER_NH_TYPE_TOCPU:
        case SYS_HUMBER_NH_TYPE_UNROV:
            break;

        default:
            CTC_ERROR_RETURN(CTC_E_INVALID_PARAM);
    }
    kal_memset(&nh_para, 0, sizeof(sys_nh_param_special_t));
    nh_para.hdr.nhid = nhid;
    nh_para.hdr.nh_param_type = nh_param_type;
    nh_para.hdr.is_internal_nh = FALSE;

    CTC_ERROR_RETURN(sys_humber_nh_api_create((sys_nh_param_com_t*)(&nh_para)));
    return CTC_E_NONE;
}

/**
 @brief This function is to create ecmp nexthop

 @param[in] nh_ptr, nexthop data pointer

 @param[in] p_member_list, member list to be added to this nexthop

 @return CTC_E_XXX
 */
int32
sys_humber_ecmp_nh_create(sys_nh_param_ecmp_api_t* p_nh_param, uint32 *p_nhid)
{
    sys_nh_param_ecmp_t nh_param;
    SYS_NH_DBG_FUNC();

    SYS_NH_INIT_CHECK;
    CTC_PTR_VALID_CHECK(p_nh_param);
    CTC_PTR_VALID_CHECK(p_nhid);
    kal_memset(&nh_param, 0, sizeof(sys_nh_param_ecmp_t));

    nh_param.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_ECMP;
    nh_param.hdr.is_internal_nh = TRUE;
    nh_param.p_ecmp_param = p_nh_param;
    nh_param.hdr.nhid = *p_nhid;

    CTC_ERROR_RETURN(sys_humber_nh_api_create((sys_nh_param_com_t*)(&nh_param)));
    *p_nhid = nh_param.hdr.nhid;

    return CTC_E_NONE;
}

int32
sys_humber_ecmp_group_create(ctc_nh_ecmp_creat_data_t* pdata)
{
    sys_nh_param_ecmp_api_t nh_param;
    sys_nh_info_com_t* p_nhinfo;
    uint8 i;

    CTC_PTR_VALID_CHECK(pdata);

    if(pdata->nh_num < 2)
    {
        return CTC_E_INVALID_PARAM;
    }

    for(i = 0; i < pdata->nh_num; i++)
    {
        SYS_NHID_EXTERNAL_VALID_CHECK(pdata->nhid[i]);
        CTC_ERROR_RETURN(sys_humber_nh_get_nhinfo_by_nhid(pdata->nhid[i], &p_nhinfo));
    }

    pdata->ecmp_nhid = pdata->nhid[0];

    kal_memset(&nh_param, 0, sizeof(sys_nh_param_ecmp_api_t));
    nh_param.ref_nh_id = pdata->nhid[1];
    nh_param.oif_need = TRUE;
    CTC_ERROR_RETURN(sys_humber_ecmp_nh_create(&nh_param, &pdata->ecmp_nhid));

    nh_param.change_type = SYS_NH_CHANGE_TYPE_ECMP_ADD_ITEM;
    for(i = 2; i < pdata->nh_num; i++)
    {
        nh_param.ref_nh_id = pdata->nhid[i];
        CTC_ERROR_RETURN(sys_humber_ecmp_nh_update(pdata->ecmp_nhid, &nh_param));
    }

    return CTC_E_NONE;
}

int32
sys_humber_ecmp_nh_delete(uint32 nhid)
{
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d\n", nhid);

    SYS_NH_INIT_CHECK;
    CTC_ERROR_RETURN(sys_humber_nh_api_delete(nhid, SYS_HUMBER_NH_TYPE_ECMP));
    return CTC_E_NONE;
}

int32
sys_humber_ecmp_group_delete(uint32 nhid)
{
    if(nhid < p_nh_api_master->max_external_nhid)
    {
        return CTC_E_INVALID_NHID;
    }

    CTC_ERROR_RETURN(sys_humber_ecmp_nh_delete(nhid));

    return CTC_E_NONE;
}

int32
sys_humber_ecmp_nh_update(uint32 nhid, sys_nh_param_ecmp_api_t* p_nh_param)
{
    sys_nh_param_ecmp_t nh_param;

    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d\n", nhid);

    SYS_NH_INIT_CHECK;
    kal_memset(&nh_param, 0, sizeof(sys_nh_param_ecmp_t));
    nh_param.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_ECMP;
    nh_param.hdr.nhid = nhid;
    nh_param.p_ecmp_param = p_nh_param;

    CTC_ERROR_RETURN(sys_humber_nh_api_update(nhid, (sys_nh_param_com_t*)(&nh_param)));

    return CTC_E_NONE;
}

int32
sys_humber_ecmp_group_update(ctc_nh_ecmp_update_data_t* pdata)
{
    sys_nh_param_ecmp_api_t nh_param;

    CTC_PTR_VALID_CHECK(pdata);
    SYS_NHID_EXTERNAL_VALID_CHECK(pdata->nhid);
    if(pdata->ecmp_nhid < p_nh_api_master->max_external_nhid)
    {
        return CTC_E_INVALID_NHID;
    }

    kal_memset(&nh_param, 0, sizeof(sys_nh_param_ecmp_api_t));
    nh_param.ref_nh_id = pdata->nhid;
    nh_param.oif_need = TRUE;
    if(CTC_NH_ECMP_ADD_MEMBER == pdata->op_type)
    {
        nh_param.change_type = SYS_NH_CHANGE_TYPE_ECMP_ADD_ITEM;
    }
    else
    {
        nh_param.change_type = SYS_NH_CHANGE_TYPE_ECMP_REMOVE_ITEM;
    }

    CTC_ERROR_RETURN(sys_humber_ecmp_nh_update(pdata->ecmp_nhid, &nh_param));

    return CTC_E_NONE;
}

/**
 @brief This function is to create IPE Loopback nexthop
 */
int32
sys_humber_iloop_nh_create(uint32 nhid, ctc_loopback_nexthop_param_t* p_nh_param)
{
    sys_nh_param_iloop_t nh_param;
    SYS_NH_DBG_FUNC();

    SYS_NH_INIT_CHECK;
    CTC_PTR_VALID_CHECK(p_nh_param);
    SYS_NHID_EXTERNAL_VALID_CHECK(nhid);
    kal_memset(&nh_param, 0, sizeof(sys_nh_param_iloop_t));

    nh_param.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_ILOOP;
    nh_param.hdr.is_internal_nh = FALSE;
    nh_param.p_iloop_param = p_nh_param;
    nh_param.hdr.nhid = nhid;

    CTC_ERROR_RETURN(sys_humber_nh_api_create((sys_nh_param_com_t*)(&nh_param)));

    return CTC_E_NONE;
}

int32
sys_humber_iloop_nh_delete(uint32 nhid)
{
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d\n", nhid);

    SYS_NH_INIT_CHECK;
    CTC_ERROR_RETURN(sys_humber_nh_api_delete(nhid, SYS_HUMBER_NH_TYPE_ILOOP));
    return CTC_E_NONE;
}


/**
 @brief This function is to create rspan  nexthop
 */
int32
sys_humber_rspan_nh_create(uint32 nhid, uint32 dsnh_offset,ctc_rspan_nexthop_param_t* p_nh_param)
{
    sys_nh_param_rspan_t nh_param;
    SYS_NH_DBG_FUNC();

    SYS_NH_INIT_CHECK;
    CTC_PTR_VALID_CHECK(p_nh_param);
    SYS_NHID_EXTERNAL_VALID_CHECK(nhid);
    kal_memset(&nh_param, 0, sizeof(sys_nh_param_rspan_t));

    nh_param.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_RSPAN;
    nh_param.hdr.is_internal_nh = FALSE;
    nh_param.p_rspan_param = p_nh_param;
    nh_param.hdr.nhid = nhid;
    nh_param.dsnh_offset = dsnh_offset;

    CTC_ERROR_RETURN(sys_humber_nh_api_create((sys_nh_param_com_t*)(&nh_param)));

    return CTC_E_NONE;
}

int32
sys_humber_rspan_nh_delete(uint32 nhid)
{
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d\n", nhid);

    SYS_NH_INIT_CHECK;
    CTC_ERROR_RETURN(sys_humber_nh_api_delete(nhid, SYS_HUMBER_NH_TYPE_RSPAN));
    return CTC_E_NONE;
}

int32
sys_humber_downmep_nh_create(uint16 vlan_id, uint32* p_nhid)
{
    sys_nh_param_downmep_t nh_downmep;
    sys_vlan_info_t vlan_info;
    uint16 vlan_ptr = 0;

    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("vlan_id = %d\n", vlan_id);

    /*Sanity Check*/
     SYS_NH_INIT_CHECK;
    CTC_PTR_VALID_CHECK(p_nhid);

    kal_memset(&vlan_info,0,sizeof(sys_vlan_info_t));
    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    sys_humber_vlan_get_vlan_ptr(&vlan_info, &vlan_ptr);

    kal_memset(&nh_downmep, 0, sizeof(sys_nh_param_downmep_t));
    nh_downmep.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_DOWNMEP;
    nh_downmep.hdr.is_internal_nh = TRUE;
    nh_downmep.dest_vlan_ptr = vlan_ptr;

    CTC_ERROR_RETURN(sys_humber_nh_api_create((sys_nh_param_com_t*)(&nh_downmep)));
    *p_nhid = nh_downmep.hdr.nhid;

    return CTC_E_NONE;
}

/**
 @brief This function is to delete upmep nexthop

 @param[in] nhid, nexthop id to be removed

 @return CTC_E_XXX
 */
int32
sys_humber_downmep_nh_delete(uint32 nhid)
{
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d\n", nhid);

    SYS_NH_INIT_CHECK;
    CTC_ERROR_RETURN(sys_humber_nh_api_delete(nhid, SYS_HUMBER_NH_TYPE_DOWNMEP));

    return CTC_E_NONE;
}

int32
sys_humber_nh_test_crscnt_ref_nhid(uint16 cc_gport, uint32 ref_nhid)
{
    uint8 gchip, lchip;
    sys_nh_offset_array_t offset_array;

    gchip = (cc_gport >> 8) & 0x1F;
    if(!sys_humber_chip_is_local(gchip, &lchip))
        return CTC_E_CHIP_IS_REMOTE;

    CTC_ERROR_RETURN(sys_humber_nh_get_dsfwd_offset(ref_nhid, offset_array));
    CTC_ERROR_RETURN(sys_humber_nh_debug_cp_dsfwd(lchip, (cc_gport & 0xFF), offset_array[lchip]));

    return CTC_E_NONE;
}

int32
sys_humber_nh_test_crscnt_ref_brguc(uint16 cc_gport, uint32 ref_gport)
{

    uint8 gchip, lchip;
    sys_nh_offset_array_t offset_array;

    gchip = (cc_gport >> 8) & 0x1F;
    if(!sys_humber_chip_is_local(gchip, &lchip))
        return CTC_E_CHIP_IS_REMOTE;

    CTC_ERROR_RETURN(sys_humber_brguc_get_dsfwd_offset(ref_gport, 0,offset_array));
    CTC_ERROR_RETURN(sys_humber_nh_debug_cp_dsfwd(lchip, (cc_gport & 0xFF), offset_array[lchip]));
    return CTC_E_NONE;
}

int32
sys_humber_nh_add_stats(uint32 nhid)
{
    SYS_NH_INIT_CHECK;

    CTC_ERROR_RETURN(sys_humber_nh_add_stats_action(nhid));
    return CTC_E_NONE;
}

int32
sys_humber_nh_del_stats(uint32 nhid)
 {
     SYS_NH_INIT_CHECK;

     CTC_ERROR_RETURN(sys_humber_nh_del_stats_action(nhid));
     return CTC_E_NONE;
 }

int32
sys_humber_nh_get_stats(uint32 nhid, ctc_stats_basic_t* p_stats)
{
    SYS_NH_INIT_CHECK;

    CTC_PTR_VALID_CHECK(p_stats);

    CTC_ERROR_RETURN(sys_humber_nh_get_stats_result(nhid, p_stats));
    return CTC_E_NONE;
}

int32
sys_humber_nh_reset_stats(uint32 nhid)
{
    SYS_NH_INIT_CHECK;

    CTC_ERROR_RETURN(sys_humber_nh_reset_stats_result(nhid));
    return CTC_E_NONE;
}



int32
sys_humber_ip_tunnel_nh_create(uint32 nhid, uint32 dsnh_offset, ctc_ip_nh_param_t* p_nh_param)
{

    sys_nh_param_ip_tunnel_t nh_param;
    SYS_NH_DBG_FUNC();

    SYS_NH_INIT_CHECK;
    SYS_NHID_EXTERNAL_VALID_CHECK(nhid);

    CTC_PTR_VALID_CHECK(p_nh_param);
    CTC_GLOBAL_PORT_CHECK(p_nh_param->oif.gport);

    CTC_ERROR_RETURN(sys_humber_nh_check_max_glb_nh_sram_offset(dsnh_offset));
    SYS_NH_DBG_INFO("nhid = %d, dsnhoffset = %d, gport = %d\n",
        nhid, dsnh_offset, p_nh_param->oif.gport);

    kal_memset(&nh_param, 0, sizeof(nh_param));
    nh_param.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_IP_TUNNEL;
    nh_param.hdr.is_internal_nh = FALSE;
    nh_param.hdr.nhid = nhid;
    nh_param.dsnh_offset = dsnh_offset;
    kal_memcpy(&nh_param.ip_nh_param, p_nh_param, sizeof(ctc_ip_nh_param_t));

    CTC_ERROR_RETURN(sys_humber_nh_api_create((sys_nh_param_com_t*)(&nh_param)));

    return CTC_E_NONE;
}

int32
sys_humber_ip_tunnel_nh_delete(uint32 nhid)
{
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d\n", nhid);

    SYS_NH_INIT_CHECK;
    CTC_ERROR_RETURN(sys_humber_nh_api_delete(nhid, SYS_HUMBER_NH_TYPE_IP_TUNNEL));
    return CTC_E_NONE;
}


int32
sys_humber_ip_tunnel_nh_update(uint32 nhid, ctc_ip_nh_param_t* p_nh_param,
                          sys_nh_entry_change_type_t change_type)
{
    sys_nh_param_ip_tunnel_t nh_param;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("nhid = %d\n", nhid);

    SYS_NH_INIT_CHECK;
    kal_memset(&nh_param, 0, sizeof(sys_nh_param_ip_tunnel_t));
    nh_param.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_IP_TUNNEL;
    nh_param.hdr.nhid = nhid;
    kal_memcpy(&nh_param.ip_nh_param, p_nh_param, sizeof(ctc_ip_nh_param_t));
    nh_param.change_type = change_type;
    CTC_ERROR_RETURN(sys_humber_nh_api_update(nhid, (sys_nh_param_com_t*)(&nh_param)));

    return CTC_E_NONE;
}


