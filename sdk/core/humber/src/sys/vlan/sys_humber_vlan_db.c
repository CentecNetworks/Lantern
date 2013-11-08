/**
 @file sys_humber_vlan_db.c

 @date 2009-10-17

 @version v2.0

 The fiel deal with DB of vlan
*/
/***************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_debug.h"
#include "ctc_error.h"
#include "ctc_vector.h"
#include "ctc_vlan.h"

#include "sys_humber_vlan_db.h"
#include "sys_humber_vlan.h"
#include "sys_humber_chip.h"
#include "sys_humber_ftm.h"

#include "drv_enum.h"
/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/
static ctc_vector_t *p_vlan_db_vector = NULL;
/***************************************************************
 *
 *  Functions
 *
 ***************************************************************/

/**
 @brief init the db of vlan
*/
int32
sys_humber_vlan_db_init(void)
{
    uint32 vlan_entry_num = 0;

    if (NULL != p_vlan_db_vector)
    {
        return CTC_E_NONE;
    }

    sys_alloc_get_table_entry_num(DS_VLAN, &vlan_entry_num);

    p_vlan_db_vector = ctc_vector_init(SYS_MAX_VLAN_BLOCK_NUM, (vlan_entry_num / SYS_MAX_VLAN_BLOCK_NUM));

    if (NULL == p_vlan_db_vector)
    {
        SYS_VLAN_DEBUG_INFO("Alloc vlan db fail, no resource");

        return CTC_E_NO_MEMORY;
    }

    ctc_vector_reserve(p_vlan_db_vector, 1);

    return CTC_E_NONE;
}

/**
 @brief de init the db of vlan
*/
/*
int32
sys_humber_vlan_db_deinit(void)
{
    if (NULL == p_vlan_db_vector)
    {
        return CTC_E_NONE;
    }

    if (p_vlan_db_vector)
    {
        ctc_vector_release(p_vlan_db_vector);
        mem_free(p_vlan_db_vector);
    }

    return CTC_E_NONE;
}
*/
/**
 @brief enable a vlan from the db
*/
int32
sys_humber_vlan_db_add_vlan(uint16 vlan_ptr)
{
    sys_vlan_node_t *p_vlan_node = NULL;

    if (NULL == p_vlan_db_vector)
    {
        return CTC_E_NOT_INIT;
    }

    p_vlan_node = ctc_vector_get(p_vlan_db_vector, vlan_ptr);

    if (NULL != p_vlan_node)
    {
        return CTC_E_VLAN_EXIST;
    }

    p_vlan_node =
    (sys_vlan_node_t *)mem_malloc(MEM_VLAN_MODULE, sizeof(sys_vlan_node_t));

    if (NULL == p_vlan_node)
    {
        return CTC_E_NO_MEMORY;
    }

    p_vlan_node->port_bitmap =
    (uint8 *)mem_malloc(MEM_VLAN_MODULE, sizeof(uint8) * CTC_PORT_BITMAP_IN_BYTE);

    if (NULL == p_vlan_node->port_bitmap)
    {
        mem_free(p_vlan_node);

        return CTC_E_NO_MEMORY;
    }

    kal_memset(p_vlan_node->port_bitmap, 0, sizeof(uint8) * CTC_PORT_BITMAP_IN_BYTE);

    if (FALSE == ctc_vector_add(p_vlan_db_vector, vlan_ptr, (void *)p_vlan_node))
    {
       mem_free(p_vlan_node->port_bitmap);
       mem_free(p_vlan_node);

       return CTC_E_NO_MEMORY;
    }

    return CTC_E_NONE;
}

/**
 @brief disable the vlan from the db
*/
int32
sys_humber_vlan_db_remove_vlan(uint16 vlan_ptr)
{
    sys_vlan_node_t *p_vlan_node = NULL;

    if (NULL == p_vlan_db_vector)
    {
        return CTC_E_NOT_INIT;
    }

    p_vlan_node = ctc_vector_del(p_vlan_db_vector, vlan_ptr);

    if (NULL == p_vlan_node)
    {
        return CTC_E_VLAN_NOT_CREATE;
    }

    if (p_vlan_node->port_bitmap)
    {
        mem_free(p_vlan_node->port_bitmap);
    }
    if (p_vlan_node)
    {
        mem_free(p_vlan_node);
    }

    return CTC_E_NONE;
}

/**
 @brief whether the vlan is enable
*/
bool
sys_humber_vlan_db_lookup(uint16 vlan_ptr)
{
    sys_vlan_node_t *p_vlan_node = NULL;

    p_vlan_node = ctc_vector_get(p_vlan_db_vector, vlan_ptr);

    if (NULL != p_vlan_node)
    {
        return TRUE;
    }

    return FALSE;
}

/**
 @brief judge whether a port is member of vlan
*/
bool
sys_humber_vlan_db_has_port(uint8 chip_id, uint16 vlan_ptr, uint8 lport)
{
    sys_vlan_node_t *p_vlan_node = NULL;
    int32 byte_offset = 0;
    int32 bit_offset = 0;

    p_vlan_node = ctc_vector_get(p_vlan_db_vector, vlan_ptr);

    if (NULL == p_vlan_node)
    {
        return FALSE;
    }

    byte_offset = ((chip_id << CTC_LOCAL_PORT_LENGTH) + lport) / BITS_NUM_OF_BYTE;
    bit_offset = ((chip_id << CTC_LOCAL_PORT_LENGTH) + lport) % BITS_NUM_OF_BYTE;

    if ((p_vlan_node->port_bitmap[byte_offset] >> bit_offset)& 0x1)
    {
        return TRUE;
    }

    return FALSE;
}

/**
 @brief add a member port to vlan db
*/
int32
sys_humber_vlan_db_add_port(uint8 chip_id, uint16 vlan_ptr, uint8 lport)
{
    sys_vlan_node_t *p_vlan_node = NULL;
    int32 byte_offset = 0;
    int32 bit_offset = 0;

    if (NULL == p_vlan_db_vector)
    {
        return CTC_E_NOT_INIT;
    }

    p_vlan_node = ctc_vector_get(p_vlan_db_vector, vlan_ptr);

    if (NULL == p_vlan_node)
    {
        return CTC_E_INVALID_VLAN_ID;
    }

    byte_offset = ((chip_id << CTC_LOCAL_PORT_LENGTH) + lport) / BITS_NUM_OF_BYTE;
    bit_offset = ((chip_id << CTC_LOCAL_PORT_LENGTH) + lport) % BITS_NUM_OF_BYTE;

    p_vlan_node->port_bitmap[byte_offset] |= (1 << bit_offset);

    return CTC_E_NONE;

}

/**
 @brief The function is to get member ports of vlan
*/
int32
sys_humber_vlan_db_get_ports(uint16 vlan_ptr, ctc_port_bitmap_t *port_bitmap)
{
    sys_vlan_node_t *p_vlan_node = NULL;

    if (NULL == p_vlan_db_vector)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(port_bitmap);

    p_vlan_node = ctc_vector_get(p_vlan_db_vector, vlan_ptr);

    if (NULL == p_vlan_node)
    {
        return CTC_E_INVALID_VLAN_ID;
    }

    kal_memcpy(port_bitmap, p_vlan_node->port_bitmap, sizeof(uint8) * CTC_PORT_BITMAP_IN_BYTE);

    return CTC_E_NONE;
}

/**
 @brief delete a member port to vlan db
*/
int32
sys_humber_vlan_db_remove_port(uint8 chip_id, uint16 vlan_ptr, uint8 lport)
{
    sys_vlan_node_t *p_vlan_node = NULL;
    int32 byte_offset = 0;
    int32 bit_offset = 0;
    uint8 mask = 0x1;

    if (NULL == p_vlan_db_vector)
    {
        return CTC_E_NOT_INIT;
    }

    p_vlan_node = ctc_vector_get(p_vlan_db_vector, vlan_ptr);

    if (NULL == p_vlan_node)
    {
        return CTC_E_INVALID_VLAN_ID;
    }

    byte_offset = ((chip_id << CTC_LOCAL_PORT_LENGTH) + lport) / BITS_NUM_OF_BYTE;
    bit_offset = ((chip_id << CTC_LOCAL_PORT_LENGTH) + lport) % BITS_NUM_OF_BYTE;

    p_vlan_node->port_bitmap[byte_offset] &= ~(mask << bit_offset);

    return CTC_E_NONE;
}





