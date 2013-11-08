/**
 @file drv_chip_io.c

 @date 2010-07-23

 @version v5.1

 The file contains all driver I/O interface realization
*/
#include "drv_model_io.h"
//#include "sram_model.h"
#include "drv_io.h"
#include "drv_humber.h"
#include "drv_chip_io.h"
#include "drv_cfg.h"
#include "drv_enum.h"
#include "drv_io.h"
#include "drv_common.h"
#include "drv_error.h"
#include "drv_tbl_reg.h"
#include "drv_humber_parity_error.h"


extern tables_t drv_tbls_list[MAX_TBL_NUM];
extern registers_t drv_regs_list[MAX_REG_NUM];

#define DRV_EACH_TCAM_ENTRY_SW_SIM_BYTES 16
#define DRV_TIME_OUT  1000    /* Time out setting */

/* Cpu access Tcam control operation type */
enum tcam_operation_type_t
{
    CPU_ACCESS_REQ_READ_REG = 0,
    CPU_ACCESS_REQ_READ_X = 1,
    CPU_ACCESS_REQ_READ_Y = 2,
    CPU_ACCESS_REQ_WRITE_REG = 4,
    CPU_ACCESS_REQ_WRITE = 5,
    CPU_ACCESS_REQ_INVALID_ENTRY = 9,
};
typedef enum tcam_operation_type_t tcam_op_tp_e;

/* Tcam memory type */
enum tcam_mem_type
{
    DRV_INT_TCAM_RECORD_DATA = 0,
    DRV_INT_TCAM_RECORD_MASK = 1,
    DRV_INT_TCAM_RECORD_REG = 2,
    DRV_EXT_TCAM_RECORD_DATA = 3,
    DRV_EXT_TCAM_RECORD_MASK = 4,
    DRV_EXT_TCAM_RECORD_REG = 5,
};
typedef enum tcam_mem_type tcam_mem_type_e;

/* Tcam/Tbl/Reg I/O operation mutex definition */
static kal_mutex_t* p_tcam_mutex[MAX_LOCAL_CHIP_NUM];
static kal_mutex_t *p_tbl_mutex[MAX_LOCAL_CHIP_NUM];
static kal_mutex_t *p_reg_mutex[MAX_LOCAL_CHIP_NUM];

#define TCAM_LOCK(chip_id)     kal_mutex_lock(p_tcam_mutex[chip_id])
#define TCAM_UNLOCK(chip_id)   kal_mutex_unlock(p_tcam_mutex[chip_id])
#define TBL_LOCK(chip_id)          kal_mutex_lock(p_tbl_mutex[chip_id])
#define TBL_UNLOCK(chip_id)        kal_mutex_unlock(p_tbl_mutex[chip_id])
#define REG_LOCK(chip_id)          kal_mutex_lock(p_reg_mutex[chip_id])
#define REG_UNLOCK(chip_id)        kal_mutex_unlock(p_reg_mutex[chip_id])

/* Each hash key and the corresponding action tb's
   position infomation in hash ram and sram */
struct hash_tblbase_info_s
{
    uint32 key_bucket_base;
    uint32 hash_action_tbl_pos;
    uint32 action_tbl_offset;
};
typedef struct hash_tblbase_info_s hash_tblbase_info_t;

/**
 @brief Real sram indirect write operation I/O
*/
static int32
_drv_chip_indirect_sram_tbl_write(uint8 chip_id, reg_id_t access_reg_id,
                             uint16 accessing_flag_fid, void* access_data)
{
    uint32 cmd, time_out, writting;

  /*  DRV_PTR_VALID_CHECK(wr_data);*/
    DRV_PTR_VALID_CHECK(access_data);
    writting = TRUE;
    time_out = 0;

    cmd = DRV_IOW(IOC_REG, access_reg_id, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, access_data));

    while (writting)
    {
        cmd = DRV_IOR(IOC_REG, access_reg_id, accessing_flag_fid);
        DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &writting));
        if ((time_out++) > DRV_TIME_OUT)
        {
            return DRV_E_TIME_OUT;
        }
    }

    return DRV_E_NONE;
}

/**
 @brief Real sram indirect read operation I/O
*/
static int32
_drv_chip_indirect_sram_tbl_read(uint8 chip_id, reg_id_t access_reg_id,
                            uint16 accessing_flag_fid, void* access_data)
{
    uint32 cmd, time_out, read_finish;

    read_finish = FALSE;
    time_out = 0;
  /*  DRV_PTR_VALID_CHECK(rd_data);*/
    DRV_PTR_VALID_CHECK(access_data);

    cmd = DRV_IOW(IOC_REG, access_reg_id, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, access_data));

    while (!read_finish)
    {
        cmd = DRV_IOR(IOC_REG, access_reg_id, accessing_flag_fid);
        DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &read_finish));
        if ((time_out ++) > DRV_TIME_OUT)
        {
            return DRV_E_TIME_OUT;
        }
    }

    return DRV_E_NONE;
}

/**
 @brief Real sram indirect operation I/O
*/
int32
drv_chip_indirect_sram_tbl_ioctl(uint8 chip_id, uint32 index,
                                uint32 cmd, void* val)
{
    int32 action;
    tbl_id_t tbl_id;
    uint16 field_id;
    uint32 data_entry[MAX_ENTRY_WORD] = {0};
    uint32 cpu_req, read_valid;
    policing_ds_policer_access_t policer_access;
    policing_ds_policer_rd00_t policer_data0;
    policing_ds_policer_rd01_t policer_data1;
    policing_ds_policer_rd02_t policer_data2;
    policing_ds_policer_rd03_t policer_data3;
    statistics_table_qdr_access_t stats_access;
    statistics_table_qdr_wr00_t stats_data0;
    statistics_table_qdr_wr01_t stats_data1;
    statistics_table_qdr_wr02_t stats_data2;
    statistics_table_qdr_wr03_t stats_data3;
    tables_t* tbl_ptr = NULL;

    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_PTR_VALID_CHECK(val);

    kal_memset(data_entry, 0, sizeof(data_entry));
    kal_memset(&policer_access, 0, sizeof(policing_ds_policer_access_t));
    kal_memset(&policer_data0, 0, sizeof(policing_ds_policer_rd00_t));
    kal_memset(&policer_data1, 0, sizeof(policing_ds_policer_rd01_t));
    kal_memset(&policer_data2, 0, sizeof(policing_ds_policer_rd02_t));
    kal_memset(&policer_data3, 0, sizeof(policing_ds_policer_rd03_t));
    kal_memset(&stats_access, 0, sizeof(statistics_table_qdr_access_t ));
    kal_memset(&stats_data0, 0, sizeof(statistics_table_qdr_wr00_t));
    kal_memset(&stats_data1, 0, sizeof(statistics_table_qdr_wr01_t));
    kal_memset(&stats_data2, 0, sizeof(statistics_table_qdr_wr02_t));
    kal_memset(&stats_data3, 0, sizeof(statistics_table_qdr_wr03_t));

    /*Here we only support full entry operation*/
    field_id = DRV_IOC_FIELDID(cmd);
    if (DRV_ENTRY_FLAG != field_id )
    {
        return DRV_E_INVALID_FLD;
    }

    cpu_req = TRUE;
    read_valid = FALSE;
    action = DRV_IOC_OP(cmd);
    tbl_id = DRV_IOC_MEMID(cmd);

    DRV_TBL_ID_VALID_CHECK(tbl_id);

    tbl_ptr = DRV_TBL_GET_INFOPTR(tbl_id);

    if (DRV_IOC_WRITE == action)
    {
        DRV_IF_ERROR_RETURN(drv_sram_tbl_ds_to_entry(tbl_id, val, data_entry));

        switch (tbl_id)
        {
            case DS_POLICER:
                policer_data0.policer_rd_data0 = data_entry[0] & 0xf;
                policer_data1.policer_rd_data1 = data_entry[1];
                policer_data2.policer_rd_data2 = data_entry[2] & 0xf;
                policer_data3.policer_rd_data3 = data_entry[3];
                policer_access.cpu_index = index;
                policer_access.cpu_req = 1;
                policer_access.cpu_req_type = 0; /* write operation */

                cmd = DRV_IOW(IOC_REG, POLICING_DS_POLICER_WR00, DRV_ENTRY_FLAG);
                DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &policer_data0));
                cmd = DRV_IOW(IOC_REG, POLICING_DS_POLICER_WR01, DRV_ENTRY_FLAG);
                DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &policer_data1));
                cmd = DRV_IOW(IOC_REG, POLICING_DS_POLICER_WR02, DRV_ENTRY_FLAG);
                DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &policer_data2));
                cmd = DRV_IOW(IOC_REG, POLICING_DS_POLICER_WR03, DRV_ENTRY_FLAG);
                DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &policer_data3));
                DRV_IF_ERROR_RETURN(_drv_chip_indirect_sram_tbl_write(chip_id,
                                        POLICING_DS_POLICER_ACCESS,
                                        POLICING_DS_POLICER_ACCESS_CPU_REQ,
                                        &policer_access));
                break;

            case DS_FORWARDING_STATS:
                stats_data0.table_qdr_wr_data0 = data_entry[0] & 0xf;
                stats_data1.table_qdr_wr_data1 = data_entry[1];
                stats_data2.table_qdr_wr_data2 = data_entry[2] & 0xf;
                stats_data3.table_qdr_wr_data3 = data_entry[3];
                stats_access.cpu_index = index;
                stats_access.cpu_req = 1;
                stats_access.cpu_req_type = 0; /* write operation */

                cmd = DRV_IOW(IOC_REG, STATISTICS_TABLE_QDR_WR00, DRV_ENTRY_FLAG);
                DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &stats_data0));
                cmd = DRV_IOW(IOC_REG, STATISTICS_TABLE_QDR_WR01, DRV_ENTRY_FLAG);
                DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &stats_data1));
                cmd = DRV_IOW(IOC_REG, STATISTICS_TABLE_QDR_WR02, DRV_ENTRY_FLAG);
                DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &stats_data2));
                cmd = DRV_IOW(IOC_REG, STATISTICS_TABLE_QDR_WR03, DRV_ENTRY_FLAG);
                DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &stats_data3));
                DRV_IF_ERROR_RETURN(_drv_chip_indirect_sram_tbl_write(chip_id,
                                        STATISTICS_TABLE_QDR_ACCESS,
                                        STATISTICS_TABLE_QDR_ACCESS_CPU_REQ,
                                        &stats_access));
                break;

            default :
                return DRV_E_INVALID_TBL;
        }
    }
    else if (DRV_IOC_READ == action)
    {
        switch (tbl_id)
        {
            case DS_POLICER:
                policer_access.cpu_index = index;
                policer_access.cpu_req = 1;
                policer_access.cpu_req_type = 1; /* read operation */
                DRV_IF_ERROR_RETURN(_drv_chip_indirect_sram_tbl_read(chip_id,
                                            POLICING_DS_POLICER_ACCESS,
                                            STATISTICS_TABLE_QDR_ACCESS_CPU_READ_DATA_VALID,
                                            &policer_access));

                cmd = DRV_IOR(IOC_REG, POLICING_DS_POLICER_RD00, DRV_ENTRY_FLAG);
                DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &policer_data0));
                cmd = DRV_IOR(IOC_REG, POLICING_DS_POLICER_RD01, DRV_ENTRY_FLAG);
                DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &policer_data1));
                cmd = DRV_IOR(IOC_REG, POLICING_DS_POLICER_RD02, DRV_ENTRY_FLAG);
                DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &policer_data2));
                cmd = DRV_IOR(IOC_REG, POLICING_DS_POLICER_RD03, DRV_ENTRY_FLAG);
                DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &policer_data3));

                data_entry[0] = policer_data0.policer_rd_data0;
                data_entry[1] = policer_data1.policer_rd_data1;
                data_entry[2] = policer_data2.policer_rd_data2;
                data_entry[3] = policer_data3.policer_rd_data3;
                break;

            case DS_FORWARDING_STATS:
                stats_access.cpu_index = index;
                stats_access.cpu_req = 1;
                stats_access.cpu_req_type = 1; /* read operation */
                DRV_IF_ERROR_RETURN(_drv_chip_indirect_sram_tbl_read(chip_id,
                                            STATISTICS_TABLE_QDR_ACCESS,
                                            STATISTICS_TABLE_QDR_ACCESS_CPU_READ_DATA_VALID,
                                            &stats_access));

                cmd = DRV_IOR(IOC_REG, STATISTICS_TABLE_QDR_RD00, DRV_ENTRY_FLAG);
                DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &stats_data0));
                cmd = DRV_IOR(IOC_REG, STATISTICS_TABLE_QDR_RD01, DRV_ENTRY_FLAG);
                DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &stats_data1));
                cmd = DRV_IOR(IOC_REG, STATISTICS_TABLE_QDR_RD02, DRV_ENTRY_FLAG);
                DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &stats_data2));
                cmd = DRV_IOR(IOC_REG, STATISTICS_TABLE_QDR_RD03, DRV_ENTRY_FLAG);
                DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &stats_data3));

                data_entry[0] = stats_data0.table_qdr_wr_data0;
                data_entry[1] = stats_data1.table_qdr_wr_data1;
                data_entry[2] = stats_data2.table_qdr_wr_data2;
                data_entry[3] = stats_data3.table_qdr_wr_data3;
                break;

            default :
                return DRV_E_INVALID_TBL;
        }

        DRV_IF_ERROR_RETURN(drv_sram_tbl_entry_to_ds( tbl_id, (uint32 *)data_entry, val));
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }

    return DRV_E_NONE;
}

/**
 @brief Real sram direct write operation I/O
*/
int32
drv_chip_write_sram_entry(uint8 chip_id, uint32 addr,
                                uint32* data, int32 len)
{
    int32 i, length;
    int32 dlen;

    length = len;

    DRV_PTR_VALID_CHECK(data);

    /* the length must be 4*n(n=1,2,3...) */
    if (length & 0x3)
    {
        return DRV_E_ILLEGAL_LENGTH;
    }

    length = (length >> 2);
    
    /* the entry is 4dowrd alignment */
    if (0x0 == (length & 0x3))
    {
        dlen = (length >> 2);
        for (i = 0; i < dlen; i++)
        {
            DRV_IF_ERROR_RETURN(ctckal_usrctrl_write_bay_4w(chip_id, 0, addr, (uint32)&data[i*4]));
            addr += 16;
        }
    }
    else
    {

        for (i = 0; i < length; i++)
        {
            DRV_IF_ERROR_RETURN(dal_usrctrl_write_chip(chip_id, addr, data[i]));
            addr += 4;
        }
    }
    return DRV_E_NONE;
}

/**
 @brief Real sram direct read operation I/O
*/
int32
drv_chip_read_sram_entry(uint8 chip_id, uint32 addr, uint32* data, int32 len)
{
    int32 i, length;
    int32 dlen;

    length = len;

    DRV_PTR_VALID_CHECK(data);

    /* the length must be 4*n(n=1,2,3...) */
    if (length & 0x3)
    {
        return DRV_E_ILLEGAL_LENGTH;
    }

    length = (length >> 2);

    /* the entry is 4dowrd alignment */
    if (0x0 == (length & 0x3))
    {
        dlen = (length >> 2);
        for (i = 0; i < dlen; i++)
        {
            DRV_IF_ERROR_RETURN(ctckal_usrctrl_read_bay_4w(chip_id, 0, addr, (uint32)&data[i*4]));
            addr += 16;
        }
    }
    else
    {
        for (i = 0; i < length; i++)
        {
            DRV_IF_ERROR_RETURN(dal_usrctrl_read_chip(chip_id, addr, (uint32)&data[i]));
            addr += 4;
        }
    }

    return DRV_E_NONE;
}

/**
 @brief Real embeded tcam read operation I/O
*/
static int32
_drv_chip_read_int_tcam_entry(uint8 chip_id, uint32 index,
                             uint32* data, tcam_mem_type_e type)
{
    tcam_ctl_int_access_t access;
    uint32 time_out = 0;
    tcam_ctl_int_cpu_rd_data_t tcam_data;
    uint32 cmd;
    int32 ret = DRV_E_NONE;

    DRV_PTR_VALID_CHECK(data);

    if (index >= DRV_INT_TCAM_MAX_ENTRY_NUM)
    {
        DRV_DBG_INFO("\nEmbeded Tcam's index range is 0~(16k-1)\n");
        return DRV_E_INVALID_TCAM_INDEX;
    }

    TCAM_LOCK(chip_id);

    kal_memset(&access, 0, sizeof(access));
    kal_memset(&tcam_data, 0, sizeof(tcam_data));

    access.cpu_index = index;
    access.cpu_req = TRUE;
    if (DRV_INT_TCAM_RECORD_MASK == type)
    {
        access.cpu_req_type = CPU_ACCESS_REQ_READ_Y;
    }
    else if (DRV_INT_TCAM_RECORD_DATA == type)
    {
        access.cpu_req_type = CPU_ACCESS_REQ_READ_X;
    }
    else
    {
        DRV_DBG_INFO("\nEmbeded Tcam Memory Type is useless!\n");
        TCAM_UNLOCK(chip_id);
        return DRV_E_INVALID_TCAM_TYPE;
    }

    cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_ACCESS, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &access);
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }


    while (TRUE)
    {
        cmd = DRV_IOR(IOC_REG, TCAM_CTL_INT_ACCESS, DRV_ENTRY_FLAG);
        ret = drv_reg_ioctl(chip_id, 0, cmd, &access);
        if (ret < DRV_E_NONE)
        {
            TCAM_UNLOCK(chip_id);
            return ret;
        }

        if (access.cpu_read_data_valid)
        {
            break;
        }

        if ((time_out ++) > DRV_TIME_OUT)
        {
            TCAM_UNLOCK(chip_id);
            return DRV_E_TIME_OUT;
        }
    }

    cmd = DRV_IOR(IOC_REG, TCAM_CTL_INT_CPU_RD_DATA, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &tcam_data);
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }

    data[1] = tcam_data.tcam_read_data0;
    data[2] = tcam_data.tcam_read_data1;
    data[3] = tcam_data.tcam_read_data2;

    access.cpu_read_data_valid = FALSE;

    cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_ACCESS, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &access);
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }

    TCAM_UNLOCK(chip_id);

    return DRV_E_NONE;
}

/**
 @brief Real embeded tcam write operation I/O (data&mask write)
*/
static int32
_drv_chip_write_int_tcam_data_mask(uint8 chip_id, uint32 index,
                                   uint32* data, uint32* mask)
{
    tcam_ctl_int_access_t access;
    tcam_ctl_int_cpu_wr_data_t tcam_data;
    tcam_ctl_int_cpu_wr_mask_t tcam_mask;
    uint32 cmd;
    int32 ret = DRV_E_NONE;

    DRV_PTR_VALID_CHECK(data);
    DRV_PTR_VALID_CHECK(mask);

    if (index >= DRV_INT_TCAM_MAX_ENTRY_NUM)
    {
        DRV_DBG_INFO("\nEmbeded Tcam's index range is 0~(16k-1)\n");
        return DRV_E_INVALID_TCAM_INDEX;
    }

    TCAM_LOCK(chip_id);

    /* TCAM_LOCK(chip_id);*/
    kal_memset(&access, 0, sizeof(access));
    kal_memset(&tcam_data, 0, sizeof(tcam_data));
    kal_memset(&tcam_mask, 0, sizeof(tcam_mask));

    /* Attention that we MUST write the mask before writing corresponding data!! */
    /* because the real store in TCAM is X/Y , and the TCAM access write data and mask at the same time, */
    /* so we should write mask at first, then tcam data. */
    /* X = ~data & mask ; Y = data & mask */

    tcam_data.tcam_write_data00 = data[1] & 0xFFFF;
    tcam_data.tcam_write_data01 = data[2];
    tcam_data.tcam_write_data02 = data[3];

    tcam_mask.tcam_write_data10 = mask[1] & 0xFFFF;
    tcam_mask.tcam_write_data11 = mask[2];
    tcam_mask.tcam_write_data12 = mask[3];


    cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_CPU_WR_MASK, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &tcam_mask);
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }

    cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_CPU_WR_DATA, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &tcam_data);
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }

    access.cpu_index = index;
    access.cpu_req_type = CPU_ACCESS_REQ_WRITE;
    access.cpu_req = TRUE;

    cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_ACCESS, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &access);
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }

    TCAM_UNLOCK(chip_id);

    return DRV_E_NONE;
}

/**
 @brief Real embeded tcam remove operation I/O
*/
static int32
_drv_chip_remove_int_tcam_entry(uint8 chip_id, uint32 tcam_index)
{
    tcam_ctl_int_access_t access;
    uint32 value;
    int32 ret = DRV_E_NONE;

    kal_memset(&access, 0, sizeof(access));
    access.cpu_index = tcam_index;
    access.cpu_req = TRUE;
    access.cpu_req_type = CPU_ACCESS_REQ_INVALID_ENTRY;
    value = *(uint32*)&access;

    TCAM_LOCK(chip_id);
    ret = (dal_usrctrl_write_chip(chip_id,
                                    TCAM_CTL_INT_ACCESS_OFFSET,
                                    value));
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }

    TCAM_UNLOCK(chip_id);

    return DRV_E_NONE;
}

/**
 @brief Real external tcam read operation I/O
*/
static int32
_drv_chip_read_ext_tcam_entry(uint8 chip_id, uint32 index,
                             uint32* data, tcam_mem_type_e type)
{
    tcam_ctl_ext_access_t access;
    uint32 time_out = 0;
    tcam_ctl_ext_read_data_t tcam_data;
    uint32 cmd;
    int32 ret = DRV_E_NONE;

    DRV_PTR_VALID_CHECK(data);

    if (type != DRV_EXT_TCAM_RECORD_REG)
    {
        if (index >= DRV_EXT_TCAM_MAX_ENTRY_NUM)
        {
            DRV_DBG_INFO("\nExternal Tcam's index range is 0~(256k-1)\n");
            return DRV_E_INVALID_TCAM_INDEX;
        }
    }

    TCAM_LOCK(chip_id);

    kal_memset(&access, 0, sizeof(access));

    access.cpu_index = index;
    access.cpu_req = TRUE;

    if (DRV_EXT_TCAM_RECORD_MASK == type)
    {
        access.cpu_req_type = CPU_ACCESS_REQ_READ_Y;
    }
    else if (DRV_EXT_TCAM_RECORD_DATA == type)
    {
        access.cpu_req_type = CPU_ACCESS_REQ_READ_X;
    }
    else if (DRV_EXT_TCAM_RECORD_REG == type)
    {
        access.cpu_req_type = CPU_ACCESS_REQ_READ_REG;
    }
    else
    {
        DRV_DBG_INFO("\nEXT Tcam type is useless!\n");
        TCAM_UNLOCK(chip_id);
        return DRV_E_INVALID_TCAM_TYPE;
    }

    cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_ACCESS, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &access);
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }

    while (TRUE)
    {
        cmd = DRV_IOR(IOC_REG, TCAM_CTL_EXT_ACCESS, DRV_ENTRY_FLAG);
        ret = drv_reg_ioctl(chip_id, 0, cmd, &access);
        if (ret < DRV_E_NONE)
        {
            TCAM_UNLOCK(chip_id);
            return ret;
        }

        if (access.cpu_read_data_valid)
        {
            break;
        }

        if ((time_out++) > DRV_TIME_OUT)
        {
            TCAM_UNLOCK(chip_id);
            return DRV_E_TIME_OUT;
        }
    }

    cmd = DRV_IOR(IOC_REG, TCAM_CTL_EXT_READ_DATA, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &tcam_data);
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }

    if (type != DRV_EXT_TCAM_RECORD_REG)
    {
        data[1] = tcam_data.tcam_read_data0;
        data[2] = tcam_data.tcam_read_data1;
        data[3] = tcam_data.tcam_read_data2;
    }
    else
    {
        data[2] = tcam_data.tcam_read_data0;
        data[1] = tcam_data.tcam_read_data1;
        data[0] = tcam_data.tcam_read_data2;
    }

    access.cpu_read_data_valid = FALSE;

    cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_ACCESS, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &access);
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }

    TCAM_UNLOCK(chip_id);

    return DRV_E_NONE;
}

/**
 @brief Real external tcam write operation I/O (data&mask write)
*/
static int32
_drv_chip_write_ext_tcam_data_mask(uint8 chip_id, uint32 index,
                                   uint32* data, uint32* mask)
{
    tcam_ctl_ext_access_t access;
    tcam_ctl_ext_write_data_t tcam_data;
    tcam_ctl_ext_write_mask_t tcam_mask;
    uint32 cmd;
    int32 ret = DRV_E_NONE;

    DRV_PTR_VALID_CHECK(data);
    DRV_PTR_VALID_CHECK(mask);

    if (index >= DRV_EXT_TCAM_MAX_ENTRY_NUM)
    {
        DRV_DBG_INFO("\nExternal Tcam's index range is 0~(256k-1)\n");
        return DRV_E_INVALID_TCAM_INDEX;
    }

    TCAM_LOCK(chip_id);
    kal_memset(&access, 0, sizeof(access));
    kal_memset(&tcam_data, 0, sizeof(tcam_data));
    kal_memset(&tcam_mask, 0, sizeof(tcam_mask));

    /* Attention that we MUST write the mask before writing corresponding data!! */
    /* because the real store in TCAM is X/Y , and the TCAM access write data and mask at the same time, */
    /* so we should write mask at first, then tcam data. */
    /* X = ~data & mask ; Y = data & mask */

    tcam_data.tcam_write_data00 = data[1] & 0xFFFF;
    tcam_data.tcam_write_data01 = data[2];
    tcam_data.tcam_write_data02 = data[3];

    tcam_mask.tcam_write_data10 = (~mask[1]) & 0xFFFF;
    tcam_mask.tcam_write_data11 = ~mask[2];
    tcam_mask.tcam_write_data12 = ~mask[3];


    cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_WRITE_MASK, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &tcam_mask);
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }

    cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_WRITE_DATA, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &tcam_data);
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }

    access.cpu_index = index;
    access.cpu_req_type = CPU_ACCESS_REQ_WRITE;
    access.cpu_req = TRUE;

    cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_ACCESS, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &access);
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }

    TCAM_UNLOCK(chip_id);

    return DRV_E_NONE;
}


/**
 @brief Real external tcam register entry write interface
*/
static int32
_drv_chip_write_ext_tcam_reg_entry(uint8 chip_id, uint32 index, uint32 *data)
{
    tcam_ctl_ext_access_t access;
    tcam_ctl_ext_write_data_t tcam_data;
    tcam_ctl_ext_write_mask_t tcam_mask;
    uint32 cmd = 0;
    int32 ret = DRV_E_NONE;

    DRV_PTR_VALID_CHECK(data);

    TCAM_LOCK(chip_id);

    kal_memset(&tcam_mask, 0, sizeof(tcam_mask));
    tcam_mask.tcam_write_data10 = 0;
    tcam_mask.tcam_write_data11 = 0;
    tcam_mask.tcam_write_data12 = 0;
    cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_WRITE_MASK, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &tcam_mask);
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }

    kal_memset(&tcam_data, 0, sizeof(tcam_data));
    tcam_data.tcam_write_data00 = data[2] & 0xFFFF;
    tcam_data.tcam_write_data01 = data[1];
    tcam_data.tcam_write_data02 = data[0];
    cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_WRITE_DATA, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &tcam_data);
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }

    kal_memset(&access, 0, sizeof(access));
    access.cpu_index = index;
    access.cpu_req_type = CPU_ACCESS_REQ_WRITE_REG;
    access.cpu_req = TRUE;
    cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_ACCESS, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &access);
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }

    TCAM_UNLOCK(chip_id);

    return DRV_E_NONE;
}

/**
 @brief Real external tcam remove operation I/O
*/
static int32
_drv_chip_remove_ext_tcam_entry(uint8 chip_id, uint32 tcam_index)
{
    tcam_ctl_ext_access_t access;
    uint32 value;
    int32 ret = DRV_E_NONE;

    kal_memset(&access, 0, sizeof(access));
    access.cpu_index = tcam_index;
    access.cpu_req = TRUE;
    access.cpu_req_type = CPU_ACCESS_REQ_INVALID_ENTRY;
    value = *(uint32*)&access;

    TCAM_LOCK(chip_id);
    ret = dal_usrctrl_write_chip(chip_id,
                                   TCAM_CTL_EXT_ACCESS_OFFSET,
                                   value);
    if (ret < DRV_E_NONE)
    {
        TCAM_UNLOCK(chip_id);
        return ret;
    }

    TCAM_UNLOCK(chip_id);

    return DRV_E_NONE;
}


/**
 @brief Init embeded and external tcam mutex, use the same mutex
*/
int32
drv_chip_tcam_mutex_init(uint8 chip_id)
{
    int32 ret;

    DRV_CHIP_ID_VALID_CHECK(chip_id);

    ret = kal_mutex_create(&p_tcam_mutex[chip_id]);
    if (ret || (!p_tcam_mutex[chip_id]))
    {
        return DRV_E_FAIL_CREATE_MUTEX;
    }

    return DRV_E_NONE;
}

int32
drv_chip_tbl_mutex_init(uint8 chip_id)
{
    int32 ret;

    DRV_CHIP_ID_VALID_CHECK(chip_id);

    ret = kal_mutex_create(&p_tbl_mutex[chip_id]);
    if (ret || (!p_tbl_mutex[chip_id]))
    {
        return DRV_E_FAIL_CREATE_MUTEX;
    }

    return DRV_E_NONE;
}

int32
drv_chip_reg_mutex_init(uint8 chip_id)
{
    int32 ret;

    DRV_CHIP_ID_VALID_CHECK(chip_id);

    ret = kal_mutex_create(&p_reg_mutex[chip_id]);
    if (ret || (!p_reg_mutex[chip_id]))
    {
        return DRV_E_FAIL_CREATE_MUTEX;
    }

    return DRV_E_NONE;
}

static int32
_drv_chip_ibm_tcam_bist_lookup(uint32 chip_id, uint32* key, uint32 aclqos_en, uint32 key_size, uint32* index)
{
    uint32 time_out = 0;
    uint32 cmd, entries, enumber, bist_done = FALSE;
    tcam_ctl_int_bist_ctl_t bist_control;
    tcam_ctl_int_cpu_request_mem_t bist_request;
    tcam_ctl_int_cpu_result_mem_t bist_result;

    /*initial all the tcam ctrl ram and reg*/
    kal_memset(&bist_control, 0, sizeof(bist_control));
    kal_memset(&bist_request, 0, sizeof(bist_request));
    kal_memset(&bist_result, 0, sizeof(bist_result));

    cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_BIST_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &bist_control));

    for (entries = 0; entries < 64 ; entries++)
    {
        cmd = DRV_IOW(IOC_TABLE, TCAM_CTL_INT_CPU_REQUEST_MEM, DRV_ENTRY_FLAG);
        DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, entries, cmd, &bist_request));
    }

    for (entries = 0; entries < 128 ; entries++)
    {
        cmd = DRV_IOW(IOC_TABLE, TCAM_CTL_INT_CPU_RESULT_MEM, DRV_ENTRY_FLAG);
        DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, entries, cmd, &bist_result));
    }

    /* calculate entries number */
    if(key_size < 2)
    {
        enumber = 1;
    }
    else if (key_size < 3)
    {
        enumber = 2;
    }
    else
    {
        enumber = 4;
    }

    /* write key to request ram */
    for (entries = 0; entries < enumber; entries++)
    {
        kal_memset(&bist_request, 0, sizeof(bist_request));
        if (0 == key_size)
        {
            bist_request.key31_to0 = key[3];
            bist_request.key63_to32 = key[2];
            bist_request.key79_to64 = key[1] & 0xFFFF;
        }
        else
        {
            bist_request.key31_to0 = key[entries * 8 + 7];
            bist_request.key63_to32 = key[entries * 8 + 6];
            bist_request.key79_to64 = key[entries * 8 + 5] & 0xFFFF;

            bist_request.key111_to80 = key[entries * 8 + 3];
            bist_request.key143_to112 = key[entries * 8 + 2];
            bist_request.key159_to144 = key[entries * 8 + 1] & 0xFFFF;
        /*  bist_request.key_valid = TRUE;
            bist_request.key_size = enumber;
            bist_request.key_cmd = (aclqos_en? TRUE : FALSE)&0x1;*/
        }
        bist_request.key_valid = TRUE;
        bist_request.key_size = key_size;
        bist_request.key_cmd = (aclqos_en? TRUE : FALSE)&0x1;

        cmd = DRV_IOW(IOC_TABLE, TCAM_CTL_INT_CPU_REQUEST_MEM, DRV_ENTRY_FLAG);
        DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, entries, cmd, &bist_request));
    }

    /* write bist eanble and bist once */
    bist_control.cfg_bist_en = TRUE;
    bist_control.cfg_bist_once = TRUE;
    bist_control.cfg_bist_entries = enumber -1;	/* enumber - 1 for case of 320bits key */

    cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_BIST_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &bist_control));

    /* bist done */
    while (TRUE)
    {
        cmd = DRV_IOR(IOC_REG, TCAM_CTL_INT_BIST_POINTERS, TCAM_CTL_INT_BIST_POINTERS_CFG_BIST_RESULT_DONE_ONCE);
        DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &bist_done));

        if (bist_done)
        {
            break;
        }

        if((time_out ++) > DRV_TIME_OUT)
        {
            return DRV_E_TIME_OUT;
        }
    }

    /* read resutl ram */
    cmd = DRV_IOR(IOC_TABLE, TCAM_CTL_INT_CPU_RESULT_MEM, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, (enumber -1)+64, cmd, &bist_result));
    if (aclqos_en == 1)
    {
        index[0] = bist_result.index_valid;
        index[1] = bist_result.index_acl;
        index[2] = bist_result.index_qos;
        DRV_DBG_INFO("\nTcam hit, Qos index1 = 0x%x; Acl index2 = 0x%0x !\n", index[1], index[2]);
    }
    else
    {
        index[0] = bist_result.index_valid;
        index[1] = bist_result.index_qos;
        index[2] = bist_result.index_acl;
        DRV_DBG_INFO("\nTcam hit, index1 = 0x%x; index2 = 0x%0x !\n", index[1], index[2]);
    }

    /* Disable bist */
    kal_memset(&bist_control, 0, sizeof(bist_control));
    cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_BIST_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &bist_control));

    return DRV_E_NONE;
}

/**
 @brief Embeded IBM Tcam lkp process
  Only use for the show forwarding tools
*/
int32 drv_chip_ibm_tcam_lookup(uint8 chip_id, uint8* key, int32 keysize, bool dual_lkp, uint32 * result_index0, uint32 * result_index1)
{
    int32 ret = DRV_E_NONE;
    uint8 seg_num = 0;
    uint32 index[3] = {0};
    uint8 key_size = 0;

    *result_index0 = 0;
    *result_index1 = 0;

    seg_num = keysize/128;

    if (128 == keysize)
    {
        key_size = 0;
    }
    else if (256 == keysize)
    {
        key_size = 1;
    }
    else if (512 == keysize)
    {
        key_size = 2;
    }

    /* will be updated later */
    ret = _drv_chip_ibm_tcam_bist_lookup(chip_id, (uint32 *)key, dual_lkp, key_size, index);
    if (ret != DRV_E_NONE)
    {
        DRV_DBG_INFO("CPU lookup error return!\n");
        return ret;
    }

    if (0 == index[0])
    {
        *result_index0 = 0xffffffff;
        *result_index1 = 0xffffffff;
    }
    else
    {
        *result_index0 = index[1];
        *result_index1 = index[2];
    }

    return ret;
}

static int32
_drv_chip_nl9k_tcam_bist_lookup(uint32 chip_id, uint32* key, uint32 aclqos_en,
                                                                 uint32 key_size, uint32 ltr_id, uint32* index)
{
    uint32 time_out = 0;
    uint32 cmd, entries, enumber, bist_done = FALSE;
    tcam_ctl_ext_bist_ctl_t bist_control;
    tcam_ctl_ext_bist_request_mem_t bist_request;
    tcam_ctl_ext_bist_result_mem_t bist_result;
    uint32 set_bist_en = 0;

    kal_memset(&bist_control, 0, sizeof(bist_control));
    kal_memset(&bist_request, 0, sizeof(bist_request));
    kal_memset(&bist_result, 0, sizeof(bist_result));

    cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_BIST_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &bist_control));

    for (entries = 0; entries < 64 ; entries++)
    {
        cmd = DRV_IOW(IOC_TABLE, TCAM_CTL_EXT_BIST_REQUEST_MEM, DRV_ENTRY_FLAG);
        DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, entries, cmd, &bist_request));
    }

    for (entries = 0; entries < 128 ; entries++)
    {
        cmd = DRV_IOW(IOC_TABLE, TCAM_CTL_EXT_BIST_RESULT_MEM, DRV_ENTRY_FLAG);
        DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, entries, cmd, &bist_result));
    }

    /* calculate entries number */
    if (key_size < 2)
    {
        enumber = 1;
    }
    else if (key_size < 3)
    {
        enumber = 2;
    }
    else
    {
        enumber = 4;
    }

    /* write key to request ram */
    for (entries = 0; entries < enumber; entries++)
    {
            kal_memset(&bist_request, 0, sizeof(bist_request));
            if (0 == key_size)
            {
                bist_request.key31_to0 = key[3];
                bist_request.key63_to32 = key[2];
                bist_request.key79_to64 = key[1] & 0xFFFF;
            }
            else
            {
                bist_request.key31_to0 = key[entries * 8 + 7];
                bist_request.key63_to32 = key[entries * 8 + 6];
                bist_request.key79_to64 = key[entries * 8 + 5] & 0xFFFF;

                bist_request.key111_to80 = key[entries * 8 + 3];
                bist_request.key143_to112 = key[entries * 8 + 2];
                bist_request.key159_to144 = key[entries * 8 + 1] & 0xFFFF;
            }
            bist_request.key_valid = TRUE;
            bist_request.key_size = key_size;
            bist_request.key_cmd = ltr_id;

            cmd = DRV_IOW(IOC_TABLE, TCAM_CTL_EXT_BIST_REQUEST_MEM, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, entries, cmd, &bist_request));
    }

    /* write bist eanble and bist once */
  /*  bist_control.cfg_bist_en = TRUE;*/
    bist_control.cfg_bist_once = TRUE;
    bist_control.cfg_bist_entries = enumber -1; /* enumber - 1 for case of 320bits key */
    bist_control.cfg_bist_expect_latency = 0x25;

    cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_BIST_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &bist_control));

    set_bist_en = 1;
    cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_BIST_CTL, TCAM_CTL_EXT_BIST_CTL_CFG_BIST_EN);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &set_bist_en));

    /* bist done */
    while (TRUE)
    {
        cmd = DRV_IOR(IOC_REG, TCAM_CTL_EXT_BIST_POINTERS, TCAM_CTL_EXT_BIST_POINTERS_CFG_BIST_RESULT_DONE_ONCE);
        DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &bist_done));

        if (bist_done)
        {
            break;
        }

        if((time_out ++) > DRV_TIME_OUT)
        {
            return DRV_E_TIME_OUT;
        }
    }

    set_bist_en = 0;
    cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_BIST_CTL, TCAM_CTL_EXT_BIST_CTL_CFG_BIST_EN);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &set_bist_en));

    /* read resutl ram */
    cmd = DRV_IOR(IOC_TABLE, TCAM_CTL_EXT_BIST_RESULT_MEM, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, 0, cmd, &bist_result));
    if (aclqos_en == 1)
    {
        index[0] = bist_result.index_valid;
        index[1] = bist_result.index0;
        index[2] = bist_result.index1;
        DRV_DBG_INFO("\nTcam hit, Qos index1 = 0x%x; Acl index2 = 0x%0x !\n", index[1], index[2]);
    }
    else
    {
        index[0] = bist_result.index_valid;
        index[1] = bist_result.index1;
        index[2] = bist_result.index0;
        DRV_DBG_INFO("\nTcam hit, index1 = 0x%x; index2 = 0x%0x !\n", index[1], index[2]);
    }

    return DRV_E_NONE;
}

/**
 @brief external NL9K Tcam lkp process
  Only use for the show forwarding tools
*/
int32 drv_chip_nl9k_tcam_lookup(uint8 chip_id, uint8* key, int32 keysize, bool dual_lkp,
                                          uint32 ltr_id, uint32* result_index0, uint32* result_index1)
{
    int32 ret = DRV_E_NONE;
    uint8 seg_num = 0;
    uint32 index[3] = {0};
    uint8 key_size = 0;

    *result_index0 = 0;
    *result_index1 = 0;

    seg_num = keysize/128;

    if (128 == keysize)
    {
        key_size = 0;
    }
    else if (256 == keysize)
    {
        key_size = 1;
    }
    else if (512 == keysize)
    {
        key_size = 2;
    }
    else
    {
        key_size = 3;
    }
    /* will be updated later */
    ret = _drv_chip_nl9k_tcam_bist_lookup(chip_id, (uint32 *)key, dual_lkp, key_size, ltr_id, index);
    if (ret != DRV_E_NONE)
    {
        DRV_DBG_INFO("CPU lookup error return!\n");
        return ret;
    }

    if (0 == index[0])
    {
        *result_index0 = 0xffffffff;
        *result_index1 = 0xffffffff;
    }
    else
    {
        *result_index0 = index[1];
        *result_index1 = index[2];
    }

    return ret;
}


/**
 @brief The function write data to a sram memory location for parity error
*/
int32
drv_chip_write_sram_for_parity_error(uint8 chip_id, uint32 addr, uint32* data, int32 len)
{
    int32 ret;

    DRV_PTR_VALID_CHECK(data);
    DRV_CHIP_ID_VALID_CHECK(chip_id);

    TBL_LOCK(chip_id);

    ret = drv_chip_write_sram_entry(chip_id, addr, data, len);
    if (ret < 0)
    {
        TBL_UNLOCK(chip_id);
        return ret;
    }

    TBL_UNLOCK(chip_id);

    return DRV_E_NONE;
}


/**
 @brief The function read data from a sram memory location for parity error
*/
int32
drv_chip_sram_read_for_parity_error(uint8 chip_id, uint32 addr, uint32* data, int32 len)
{
    int32 ret;

    DRV_PTR_VALID_CHECK(data);
    DRV_CHIP_ID_VALID_CHECK(chip_id);

    TBL_LOCK(chip_id);

    ret = drv_chip_read_sram_entry(chip_id, addr, data, len);
    if (ret < 0)
    {
        TBL_UNLOCK(chip_id);
        return ret;
    }

    TBL_UNLOCK(chip_id);

    return DRV_E_NONE;
}


/**
 @brief The function write table data to a sram memory location
*/
int32
drv_chip_sram_tbl_write(uint8 chip_id, tbl_id_t tbl_id, uint32 index, uint32* data)
{
    #define DYN_SRAM_ENTRY_BYTE 16
    uint32 data_base, start_data_addr, entry_size;
    uint32 max_index_num;
    int32 ret;

    DRV_PTR_VALID_CHECK(data);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_TBL_ID_VALID_CHECK(tbl_id);

    /* check if the index num exceed the max index num of the tbl */
    data_base = DRV_TBL_GET_INFO(tbl_id).hw_data_base;
    entry_size = DRV_TBL_ENTRY_SIZE(tbl_id);
    max_index_num = DRV_TBL_MAX_INDEX(tbl_id);

    if (DRV_SRAM_IS_NEXTHOP_SHARE_TBL(tbl_id)
        || DRV_SRAM_IS_L2EDIT_SHARE_TBL(tbl_id)
        || DRV_SRAM_IS_L3EDIT_SHARE_TBL(tbl_id))
    {
        max_index_num = max_index_num * (entry_size / DYN_SRAM_ENTRY_BYTE);
        start_data_addr = data_base + DYN_SRAM_ENTRY_BYTE * index;
    }
    else
    {
        start_data_addr = data_base + index * entry_size;
    }

    if (max_index_num <= index)
    {
        DRV_DBG_INFO("\nERROR (drv_write_sram_tbl): chip-0x%x, tbl-0x%x, index-0x%x exceeds the max_index-0x%x.\n", chip_id, tbl_id, index, max_index_num);
        return DRV_E_INVALID_TBL;
    }

    TBL_LOCK(chip_id);

    ret = drv_chip_write_sram_entry(chip_id, start_data_addr, data, entry_size);
    if (ret < 0)
    {
        TBL_UNLOCK(chip_id);
        return ret;
    }

    drv_humber_mem_mapping_write(chip_id, tbl_id, start_data_addr, data, entry_size);

    TBL_UNLOCK(chip_id);

    return DRV_E_NONE;
}

/**
 @brief The function read table data from a sram memory location
*/
int32
drv_chip_sram_tbl_read(uint8 chip_id, tbl_id_t tbl_id, uint32 index, uint32* data)
{
    #define DYN_SRAM_ENTRY_BYTE 16
    uint32 data_base, start_data_addr, entry_size;
    uint32 max_index_num;
    int32 ret;

    DRV_PTR_VALID_CHECK(data);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_TBL_ID_VALID_CHECK(tbl_id);

    /* check if the index num exceed the max index num of the tbl */
    max_index_num = DRV_TBL_MAX_INDEX(tbl_id);
    data_base = DRV_TBL_GET_INFO(tbl_id).hw_data_base;
    entry_size = DRV_TBL_ENTRY_SIZE(tbl_id);

    /* According to index value to get the table 's index address in sw */
    if (DRV_SRAM_IS_NEXTHOP_SHARE_TBL(tbl_id)
        || DRV_SRAM_IS_L2EDIT_SHARE_TBL(tbl_id)
        || DRV_SRAM_IS_L3EDIT_SHARE_TBL(tbl_id))
    {
        max_index_num = max_index_num * (entry_size / DYN_SRAM_ENTRY_BYTE);
        start_data_addr = data_base + DYN_SRAM_ENTRY_BYTE * index;
    }
    else
    {
        start_data_addr = data_base + index * entry_size;
    }

    if (max_index_num <= index)
    {
        DRV_DBG_INFO("\nERROR (drv_read_sram_tbl): chip-0x%x, tbl-0x%x, index-0x%x exceeds the max_index-0x%x.\n", chip_id, tbl_id, index, max_index_num);
        return DRV_E_INVALID_TBL;
    }

    TBL_LOCK(chip_id);

    ret = drv_chip_read_sram_entry(chip_id, start_data_addr, data, entry_size);
    if (ret < 0)
    {
        TBL_UNLOCK(chip_id);
        return ret;
    }

    TBL_UNLOCK(chip_id);

    return DRV_E_NONE;
}

/**
 @brief The function write register data to a sram memory location
*/
int32
drv_chip_sram_reg_write(uint8 chip_id, reg_id_t reg_id, uint32 index, uint32* data)
{
    int32 bytes;
    uint32 addr;
    uint32 max_index_num;
    int32 ret;

    DRV_PTR_VALID_CHECK(data);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_REG_ID_VALID_CHECK(reg_id);

    /* check if the index num exceed the max index num of the reg */
    max_index_num = DRV_REG_MAX_INDEX(reg_id);
    if (max_index_num <= index)
    {
        DRV_DBG_INFO("\nERROR (drv_write_sram_reg): chip-0x%x, reg-0x%x, index-0x%x exceeds the max_index-0x%x.\n", chip_id, reg_id, index, max_index_num);
        return DRV_E_INVALID_REG;
    }

    bytes = DRV_REG_ENTRY_SIZE(reg_id);
    addr  =  DRV_REG_GET_INFO(reg_id).hw_data_base + index * bytes;

    REG_LOCK(chip_id);

    ret = drv_chip_write_sram_entry(chip_id, addr, data, bytes);
    if (ret < 0)
    {
        REG_UNLOCK(chip_id);
        return ret;
    }

    REG_UNLOCK(chip_id);

    return DRV_E_NONE;
}

/**
 @brief The function write register data to a sram memory location
*/
int32
drv_chip_sram_reg_word_write(uint8 chip_id, reg_id_t reg_id, uint32 index, uint32 word_offset, uint32* data)
{
    int32 bytes;
    uint32 addr;
    uint32 max_index_num;
    int32 ret;

    DRV_PTR_VALID_CHECK(data);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_REG_ID_VALID_CHECK(reg_id);

    /* check if the index num exceed the max index num of the reg */
    max_index_num = DRV_REG_MAX_INDEX(reg_id);
    if (max_index_num <= index)
    {
        DRV_DBG_INFO("\nERROR (drv_write_sram_reg): chip-0x%x, reg-0x%x, index-0x%x exceeds the max_index-0x%x.\n", chip_id, reg_id, index, max_index_num);
        return DRV_E_INVALID_REG;
    }

    bytes = DRV_REG_ENTRY_SIZE(reg_id);
    addr  =  DRV_REG_GET_INFO(reg_id).hw_data_base + index * bytes + word_offset * sizeof(uint32);

    REG_LOCK(chip_id);

    ret = drv_chip_write_sram_entry(chip_id, addr, &data[word_offset], sizeof(uint32));
    if (ret < 0)
    {
        REG_UNLOCK(chip_id);
        return ret;
    }

    REG_UNLOCK(chip_id);

    return DRV_E_NONE;
}

/**
 @brief The function write register data to a sram memory location
*/
int32
drv_chip_sram_reg_read(uint8 chip_id, reg_id_t reg_id, uint32 index, uint32* data)
{
    int32 bytes;
    uint32 addr;
    uint32 max_index_num;
    int32 ret;

    DRV_PTR_VALID_CHECK(data);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_REG_ID_VALID_CHECK(reg_id);

    /* check if the index num exceed the max index num of the reg */
    max_index_num = DRV_REG_MAX_INDEX(reg_id);
    if (max_index_num <= index)
    {
        DRV_DBG_INFO("\nERROR (drv_read_sram_reg): chip-0x%x, reg-0x%x, index-0x%x exceeds the max_index-0x%x.\n", chip_id, reg_id, index, max_index_num);
        return DRV_E_INVALID_REG;
    }

    bytes = DRV_REG_ENTRY_SIZE(reg_id);
    addr  = DRV_REG_GET_INFO(reg_id).hw_data_base + index * bytes;

    REG_LOCK(chip_id);

    ret = drv_chip_read_sram_entry(chip_id, addr, data, bytes);
    if (ret < 0)
    {
        REG_UNLOCK(chip_id);
        return ret;
    }

    REG_UNLOCK(chip_id);

    return DRV_E_NONE;
}


/**
 @brief write tcam interface (include operate model and real tcam)
*/
int32
drv_chip_tcam_tbl_write(uint8 chip_id, tbl_id_t tbl_id, uint32 index, tbl_entry_t* entry)
{
    #define TCAM_MIN_UNIT_BYTES 16
    uint32* mask = NULL;
    uint32* data = NULL;
    tables_t* tbl = &drv_tbls_list[tbl_id];
    uint32 hw_database_addr = tbl->hw_data_base;
    uint32 hw_maskbase_addr;
    uint32 entry_size = tbl->entry_size;
    uint32 key_size = tbl->key_size;
    uint32 tcam_asic_data_base, tcam_asic_mask_base;
    uint32 entry_num_each_idx, entry_idx, offset;
    uint32 index_by_tcam;

    /* int32 ret = FALSE; */
    bool in_external_tcam = FALSE;
    uint32 max_int_tcam_data_base_tmp;
    uint32 max_ext_tcam_data_base_tmp;

    DRV_PTR_VALID_CHECK(entry);
    DRV_PTR_VALID_CHECK(entry->data_entry);
    DRV_PTR_VALID_CHECK(entry->mask_entry);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_TBL_ID_VALID_CHECK(tbl_id);

    max_int_tcam_data_base_tmp = DRV_INT_TCAM_DATA_ASIC_BASE
                             + (DRV_INT_TCAM_MAX_ENTRY_NUM * DRV_EACH_TCAM_ENTRY_SW_SIM_BYTES);
    max_ext_tcam_data_base_tmp = DRV_EXT_TCAM_DATA_ASIC_BASE
                             + (DRV_EXT_TCAM_MAX_ENTRY_NUM * DRV_EACH_TCAM_ENTRY_SW_SIM_BYTES);
    hw_maskbase_addr = tbl->hw_mask_base;

    if ((hw_database_addr >= DRV_INT_TCAM_DATA_ASIC_BASE)
        && (hw_database_addr < max_int_tcam_data_base_tmp))
    {
        tcam_asic_data_base = DRV_INT_TCAM_DATA_ASIC_BASE;
        tcam_asic_mask_base = DRV_INT_TCAM_MASK_ASIC_BASE;
        index_by_tcam = (hw_database_addr - DRV_INT_TCAM_DATA_ASIC_BASE + (index * key_size))/TCAM_MIN_UNIT_BYTES;
        in_external_tcam = FALSE;
    }
    else if ((hw_database_addr >= DRV_EXT_TCAM_DATA_ASIC_BASE)
        && (hw_database_addr < max_ext_tcam_data_base_tmp))
    {
        tcam_asic_data_base = DRV_EXT_TCAM_DATA_ASIC_BASE;
        tcam_asic_mask_base = DRV_EXT_TCAM_MASK_ASIC_BASE;
        index_by_tcam = (hw_database_addr - DRV_EXT_TCAM_DATA_ASIC_BASE + (index * key_size))/TCAM_MIN_UNIT_BYTES;
        in_external_tcam = TRUE;
    }
    else
    {
        DRV_DBG_INFO("\nTcam key id 0x%x 's database is not correct, please check it!\n", tbl_id);
        return DRV_E_TCAM_KEY_DATA_ADDRESS;
    }

    /* the key's each index includes 80bit entry number */
    entry_num_each_idx = key_size/16;
    offset = (entry_size - key_size) / 4;

    data = entry->data_entry + offset;
    mask = entry->mask_entry + offset;

    for (entry_idx = 0; entry_idx < entry_num_each_idx; ++entry_idx)
    {
        /* write real tcam address */
        if (!in_external_tcam)
        {
            DRV_IF_ERROR_RETURN(_drv_chip_write_int_tcam_data_mask(chip_id,
                                                            index_by_tcam,
                                                            data + entry_idx*4,
                                                            mask + entry_idx*4));
        }
        else
        {
            DRV_IF_ERROR_RETURN(_drv_chip_write_ext_tcam_data_mask(chip_id,
                                             index_by_tcam,
                                             data + (entry_num_each_idx -1 - entry_idx)*4,
                                             mask + (entry_num_each_idx -1 - entry_idx)*4));
        }

        index_by_tcam++;
    }

    return DRV_E_NONE;
}

/**
 @brief convert NL9k external tcam content from X/Y format to data/mask format
*/
static int32
_drv_chip_convert_ext_tcam_dump_content(uint8 chip_id, uint32 *data, uint32 *mask)
{
    #define TCAM_ENTRY_WIDTH 80
    uint32 bit_pos = 0;
    uint32 index = 0, bit_offset = 0;

    /* for NL9k, mask bit = 0 denote compare, self-define the return mask bit = 1
       denote compare to unify the embeded and external tcam read interface's return value;

       X    Y    Read_Data    Read_Mask   Return_Data   Return_Mask
       0    0       X            1            0            0
       0    1       0            0            0            1
       1    0       1            0            1            1
       1    1       X            X            0            0
       X=1, Y=1: No Write but read.  */

    /* data[1] -- [64,80]; data[2] -- [32,63]; data[3] -- [0,31] */
    for (bit_pos = 0; bit_pos < TCAM_ENTRY_WIDTH; bit_pos++)
    {
        index = 3 - bit_pos / 32;
        bit_offset = bit_pos % 32;

        if ((!IS_BIT_SET(data[index], bit_offset))
            && IS_BIT_SET(mask[index], bit_offset))    /* X = 0; Y = 1 */
        {
            continue;                            /* Return_data = 0; Return_Mask = 1 */
        }
        else if (IS_BIT_SET(data[index], bit_offset)
            && (!IS_BIT_SET(mask[index], bit_offset))) /* X = 1; Y = 0 */
        {
            SET_BIT(mask[index], bit_offset);    /* Return_data = 1; Return_Mask = 1 */
        }
        else if ((!IS_BIT_SET(data[index], bit_offset))
            && (!IS_BIT_SET(mask[index], bit_offset))) /* X = 0; Y = 0 */
        {
            continue;                           /* Return_data = X(not cared); Return_Mask = 0 */
            /* Here data = X = 0, self-defined */
        }
        else                                           /* X = 1; Y = 1 */
        {
            /* Here data = 0, mask = 0, self-defined */
            CLEAR_BIT(data[index], bit_offset); /* Return_data = 0; Return_Mask = 0 */
            CLEAR_BIT(mask[index], bit_offset);
            DRV_DBG_INFO("ERROR! Read un-set Tcam entry!\n");
        }
    }

    return DRV_E_NONE;
}

/**
 @brief convert IBM embeded tcam content from X/Y format to data/mask format
*/
static int32
_drv_chip_convert_int_tcam_dump_content(uint8 chip_id, uint32 *data, uint32 *mask)
{
    #define TCAM_ENTRY_WIDTH 80
    uint32 bit_pos = 0;
    uint32 index = 0, bit_offset = 0;

    /* for IBM embeded tcam, mask bit = 1 denote compare,
       self-define the return mask bit = 1 denote compare
       to unify the embeded and external tcam read interface's return value;

       X    Y    Read_Data    Read_Mask   Return_Data   Return_Mask
       0    0       X            0            0            0
       0    1       1            1            1            1
       1    0       0            1            0            1
       1    1       X            X            0            0
       X=1, Y=1: No Write but read.  */

    /* data[1] -- [64,80]; data[2] -- [32,63]; data[3] -- [0,31] */
    for (bit_pos = 0; bit_pos < TCAM_ENTRY_WIDTH; bit_pos++)
    {
        index = 3 - bit_pos / 32;
        bit_offset = bit_pos % 32;

        if ((!IS_BIT_SET(data[index], bit_offset))
            && IS_BIT_SET(mask[index], bit_offset))    /* X = 0; Y = 1 */
        {
            SET_BIT(data[index], bit_offset);      /* Return_Data = 1; Return_Mask = 1 */
        }
        else if (IS_BIT_SET(data[index], bit_offset)
            && (!IS_BIT_SET(mask[index], bit_offset))) /* X = 1; Y = 0 */
        {
            CLEAR_BIT(data[index], bit_offset);    /* Return_Data = 0; Return_Mask = 1 */
            SET_BIT(mask[index], bit_offset);
        }
        else if ((!IS_BIT_SET(data[index], bit_offset))
            && (!IS_BIT_SET(mask[index], bit_offset))) /* X = 0; Y = 0 */
        {
            continue;                              /* Return_Data = X(not cared); Return_Mask = 0 */
            /* Here data = X = 0, self-defined */
        }
        else                                           /* X = 1; Y = 1 */
        {
            /* Here data = 0, mask = 0, self-defined */
            CLEAR_BIT(data[index], bit_offset);    /* Return_Data = 0; Return_Mask = 0 */
            CLEAR_BIT(mask[index], bit_offset);
            DRV_DBG_INFO("ERROR! Read un-set Tcam entry!\n");
        }
    }

    return DRV_E_NONE;
}



/**
 @brief read tcam interface (include operate model and real tcam)
*/
int32
drv_chip_tcam_tbl_read(uint8 chip_id, tbl_id_t tbl_id, uint32 index, tbl_entry_t *entry)
{
    #define TCAM_MIN_UNIT_BYTES 16
    uint32* mask = NULL;
    uint32* data = NULL;
    tables_t* tbl = &drv_tbls_list[tbl_id];
    uint32 hw_database_addr = tbl->hw_data_base;
    uint32 hw_maskbase_addr;
    uint32 entry_size = tbl->entry_size;
    uint32 key_size = tbl->key_size;
    uint32 tcam_asic_data_base, tcam_asic_mask_base;
    uint32 entry_num_each_idx, entry_idx, offset;
    bool in_external_tcam = FALSE;
    /* bool ret = FALSE; */
    uint32 index_by_tcam;
    uint32 max_int_tcam_data_base_tmp;
    uint32 max_ext_tcam_data_base_tmp;

    DRV_PTR_VALID_CHECK(entry);
    DRV_PTR_VALID_CHECK(entry->data_entry);
    DRV_PTR_VALID_CHECK(entry->mask_entry);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_TBL_ID_VALID_CHECK(tbl_id);

    max_int_tcam_data_base_tmp = DRV_INT_TCAM_DATA_ASIC_BASE
                             + (DRV_INT_TCAM_MAX_ENTRY_NUM * DRV_EACH_TCAM_ENTRY_SW_SIM_BYTES);
    max_ext_tcam_data_base_tmp = DRV_EXT_TCAM_DATA_ASIC_BASE
                             + (DRV_EXT_TCAM_MAX_ENTRY_NUM * DRV_EACH_TCAM_ENTRY_SW_SIM_BYTES);
    hw_maskbase_addr = tbl->hw_mask_base;

    if ((hw_database_addr >= DRV_INT_TCAM_DATA_ASIC_BASE)
        && (hw_database_addr < max_int_tcam_data_base_tmp))
    {
        tcam_asic_data_base = DRV_INT_TCAM_DATA_ASIC_BASE;
        tcam_asic_mask_base = DRV_INT_TCAM_MASK_ASIC_BASE;
        index_by_tcam = (hw_database_addr - DRV_INT_TCAM_DATA_ASIC_BASE + (index * key_size))/TCAM_MIN_UNIT_BYTES;
        in_external_tcam = FALSE;
    }
    else if ((hw_database_addr >= DRV_EXT_TCAM_DATA_ASIC_BASE)
            && (hw_database_addr < max_ext_tcam_data_base_tmp))
    {
        tcam_asic_data_base = DRV_EXT_TCAM_DATA_ASIC_BASE;
        tcam_asic_mask_base = DRV_EXT_TCAM_MASK_ASIC_BASE;
        index_by_tcam = (hw_database_addr - DRV_EXT_TCAM_DATA_ASIC_BASE + (index * key_size))/TCAM_MIN_UNIT_BYTES;
        in_external_tcam = TRUE;
    }
    else
    {
        DRV_DBG_INFO("\nTcam key id 0x%x 's database is not correct, please check it!\n", tbl_id);
        return DRV_E_TCAM_KEY_DATA_ADDRESS;
    }

    /* the key's each index includes 80bit entry number */
    entry_num_each_idx = key_size/16;
    offset = (entry_size - key_size) / 4;

    data = entry->data_entry + offset;
    mask = entry->mask_entry + offset;

    for (entry_idx = 0; entry_idx < entry_num_each_idx; ++entry_idx)
    {
        /* read real tcam address */
        if (!in_external_tcam)
        {
            DRV_IF_ERROR_RETURN(_drv_chip_read_int_tcam_entry(chip_id, index_by_tcam, mask + entry_idx*4, DRV_INT_TCAM_RECORD_MASK));
            DRV_IF_ERROR_RETURN(_drv_chip_read_int_tcam_entry(chip_id, index_by_tcam, data + entry_idx*4, DRV_INT_TCAM_RECORD_DATA));
            DRV_IF_ERROR_RETURN(_drv_chip_convert_int_tcam_dump_content(chip_id, data + entry_idx*4, mask + entry_idx*4));
        }
        else
        {
            DRV_IF_ERROR_RETURN(_drv_chip_read_ext_tcam_entry(chip_id, index_by_tcam, mask + entry_idx*4, DRV_EXT_TCAM_RECORD_MASK));
            DRV_IF_ERROR_RETURN(_drv_chip_read_ext_tcam_entry(chip_id, index_by_tcam, data + entry_idx*4, DRV_EXT_TCAM_RECORD_DATA));
            DRV_IF_ERROR_RETURN(_drv_chip_convert_ext_tcam_dump_content(chip_id, data + entry_idx*4, mask + entry_idx*4));
        }

        index_by_tcam++;
	}

	return DRV_E_NONE;
}

/**
 @brief remove tcam entry interface (include operate model and real tcam)
*/
int32
drv_chip_tcam_tbl_remove(uint8 chip_id, tbl_id_t tbl_id, uint32 index)
{
    #define TCAM_MIN_UNIT_BYTES 16
    tables_t* tbl = NULL;
    uint32 tcam_asic_data_base, tcam_asic_mask_base;
    uint32 entry_idx, entry_num_each_idx;
    uint32 hw_database_addr, key_size;
    uint32 index_by_tcam;
    bool in_external_tcam = FALSE;
    /* bool ret = FALSE; */
    uint32 max_int_tcam_data_base_tmp;
    uint32 max_ext_tcam_data_base_tmp;

    DRV_CHIP_ID_VALID_CHECK(chip_id);

    if (!DRV_SRAM_IS_TCAM_KEY(tbl_id))
    {
        DRV_DBG_INFO("\nERROR! INVALID Tcam key TblID! TblID: %d, file:%s line:%d function:%s\n",tbl_id,__FILE__,__LINE__,__FUNCTION__);\
        return DRV_E_INVALID_TBL;
    }

    tbl = &drv_tbls_list[tbl_id];
    hw_database_addr = tbl->hw_data_base;

    max_int_tcam_data_base_tmp = DRV_INT_TCAM_DATA_ASIC_BASE
                             + (DRV_INT_TCAM_MAX_ENTRY_NUM * DRV_EACH_TCAM_ENTRY_SW_SIM_BYTES);
    max_ext_tcam_data_base_tmp = DRV_EXT_TCAM_DATA_ASIC_BASE
                             + (DRV_EXT_TCAM_MAX_ENTRY_NUM * DRV_EACH_TCAM_ENTRY_SW_SIM_BYTES);
    key_size = tbl->key_size;

    if ((hw_database_addr >= DRV_INT_TCAM_DATA_ASIC_BASE)
        && (hw_database_addr < max_int_tcam_data_base_tmp))
    {
        tcam_asic_data_base = DRV_INT_TCAM_DATA_ASIC_BASE;
        tcam_asic_mask_base = DRV_INT_TCAM_MASK_ASIC_BASE;
        index_by_tcam = (hw_database_addr - DRV_INT_TCAM_DATA_ASIC_BASE + (index * key_size))/TCAM_MIN_UNIT_BYTES;
        in_external_tcam = FALSE;
    }
    else if ((hw_database_addr >= DRV_EXT_TCAM_DATA_ASIC_BASE)
        && (hw_database_addr < max_ext_tcam_data_base_tmp))
    {
        tcam_asic_data_base = DRV_EXT_TCAM_DATA_ASIC_BASE;
        tcam_asic_mask_base = DRV_EXT_TCAM_MASK_ASIC_BASE;
        index_by_tcam = (hw_database_addr - DRV_EXT_TCAM_DATA_ASIC_BASE + (index * key_size))/TCAM_MIN_UNIT_BYTES;
        in_external_tcam = TRUE;
    }
    else
    {
        DRV_DBG_INFO("\nTcam key id 0x%x 's database is not correct, please check it!\n", tbl_id);
        return DRV_E_TCAM_KEY_DATA_ADDRESS;
    }

    /* the key's each index includes 80bit entry number */
    entry_num_each_idx = key_size/16;

    /* according to keysize to clear tcam wbit(one bit each 80 bits tcam entry)*/
    for (entry_idx = 0; entry_idx < entry_num_each_idx; ++entry_idx)
    {
        /* remove tcam entry on emulation board */
        if (!in_external_tcam)
        {
            DRV_IF_ERROR_RETURN(_drv_chip_remove_int_tcam_entry(chip_id, index_by_tcam));
        }
        else
        {
            DRV_IF_ERROR_RETURN(_drv_chip_remove_ext_tcam_entry(chip_id, index_by_tcam));
        }

        index_by_tcam++;
    }
    return DRV_E_NONE;
}

int32
drv_chip_ext_tcam_reg_write(uint8 chip_id, reg_id_t reg_id,
                            uint32 index, void* ds)
{
    #define DRV_BSR_NUM_EACH_LTR    2
    #define DRV_PSR_NUM_EACH_LTR    4
    #define DRV_KCR_NUM_EACH_LTR    1

    registers_t* reg = &drv_regs_list[reg_id];
    uint32 entry_num = 0;
    uint32 i = 0;
    uint32 entry_offset = 0;
    uint32 entry[MAX_ENTRY_NUMBER] = {0};

    DRV_PTR_VALID_CHECK(ds);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_REG_ID_VALID_CHECK(reg_id);

    /* Tcam_reg[0] and Tcam_reg[1]'s offset is 0x10 entry(each entry has 80bits) */
    if ((NL9K_CTL_REG_BCR == reg_id)||(NL9K_CTL_REG_BMR0 == reg_id)||(NL9K_CTL_REG_BMR1 == reg_id))
    {
        entry_offset = (reg->hw_data_base - TCAM_EXT_REG_RAM_OFFSET)/16 + index;
    }
    else
    {
        entry_offset = (reg->hw_data_base - TCAM_EXT_REG_RAM_OFFSET)/16 + (index<<4);
    }

    DRV_IF_ERROR_RETURN(drv_sram_reg_ds_to_entry(reg_id, ds, entry));

    if (reg->hw_data_base >= TCAM_EXT_REG_RAM_OFFSET)
    {
        if (NL9K_CTL_REG_LTR == reg_id)
        {
            entry_num = DRV_BSR_NUM_EACH_LTR + DRV_PSR_NUM_EACH_LTR + DRV_KCR_NUM_EACH_LTR;
        }
        else
        {
            entry_num = 1;
        }

        for (i = 0; i < entry_num; i++)
        {
            DRV_IF_ERROR_RETURN(_drv_chip_write_ext_tcam_reg_entry(chip_id, entry_offset + i, entry + i*4));
        }
    }
    else
    {
        DRV_DBG_INFO("Not External Tcam Register, register ID = %d\n", reg_id);
        return DRV_E_INVALID_REG;
    }

    return DRV_E_NONE;
}

int32
drv_chip_ext_tcam_reg_read(uint8 chip_id, reg_id_t reg_id, uint32 index, void* ds)
{
    #define DRV_BSR_NUM_EACH_LTR    2
    #define DRV_PSR_NUM_EACH_LTR    4
    #define DRV_KCR_NUM_EACH_LTR    1

    registers_t* reg = &drv_regs_list[reg_id];
    uint32 entry_num = 0;
    uint32 i = 0;
    uint32 entry_offset = 0;
    uint32 entry[MAX_ENTRY_NUMBER] = {0};

    DRV_PTR_VALID_CHECK(ds);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_REG_ID_VALID_CHECK(reg_id);
    if (NL9K_CTL_REG_BCR == reg_id)
    {
        entry_offset = (reg->hw_data_base - TCAM_EXT_REG_RAM_OFFSET)/16 + index;
    }
    else
    {
        entry_offset = (reg->hw_data_base - TCAM_EXT_REG_RAM_OFFSET)/16 + (index<<4);
    }

    if (reg->hw_data_base >= TCAM_EXT_REG_RAM_OFFSET)
    {
        if (NL9K_CTL_REG_LTR == reg_id)
        {
            entry_num = DRV_BSR_NUM_EACH_LTR + DRV_PSR_NUM_EACH_LTR + DRV_KCR_NUM_EACH_LTR;
        }
        else
        {
            entry_num = 1;
        }

        for (i = 0; i < entry_num; i++)
        {
            DRV_IF_ERROR_RETURN(_drv_chip_read_ext_tcam_entry(chip_id, entry_offset + i, entry + i*4, DRV_EXT_TCAM_RECORD_REG));
        }
    }
    else
    {
        DRV_DBG_INFO("Not External Tcam Register, register ID = %d\n", reg_id);
        return DRV_E_INVALID_REG;
    }

    DRV_IF_ERROR_RETURN(drv_sram_reg_entry_to_ds(reg_id, entry, ds));

    return DRV_E_NONE;
}


static int32
_drv_chip_hash_tblid_get_tblbase_info(uint32 hash_tableid,
                                   hash_tblbase_info_t *base_info)
{
    uint32 cmd;
    hash_ds_ctl_lookup_ctl_t hash_ctl;
    ipe_hash_lookup_result_ctl_t hash_lkp_rst_ctl;

    kal_memset(&hash_ctl, 0, sizeof(hash_ctl));
    cmd = DRV_IOR(IOC_REG, HASH_DS_CTL_LOOKUP_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &hash_ctl));

    kal_memset(&hash_lkp_rst_ctl, 0, sizeof(hash_lkp_rst_ctl));
    cmd = DRV_IOR(IOC_REG, IPE_HASH_LOOKUP_RESULT_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &hash_lkp_rst_ctl));

    switch (hash_tableid)
    {
        case DS_MAC_HASH_KEY0:
            /* hash key's bucket offset, each bucket have 4 hash entry(each entry is 72 bits)*/
            base_info->key_bucket_base = hash_ctl.mac_da_table_base;
            /* if set, tableBase is hash action table offset, else is tcam action tbl's offset */
            base_info->hash_action_tbl_pos = hash_lkp_rst_ctl.mac_da_lookup_table_base_pos;
            /* if pos=1, is hash action table's offset, and tcam action table is front */
            base_info->action_tbl_offset = hash_lkp_rst_ctl.mac_da_lookup_table_base;
            break;
        case DS_IPV4_UCAST_HASH_KEY0:
            base_info->key_bucket_base = hash_ctl.ipv4_ucast_table_base;
            base_info->hash_action_tbl_pos = hash_lkp_rst_ctl.ipv4_ucast_lookup_table_base_pos;
            base_info->action_tbl_offset = hash_lkp_rst_ctl.ipv4_ucast_lookup_table_base;
            break;
        case DS_IPV4_MCAST_HASH_KEY0:
            base_info->key_bucket_base = hash_ctl.ipv4_mcast_table_base;
            base_info->hash_action_tbl_pos = hash_lkp_rst_ctl.ipv4_mcast_lookup_table_base_pos;
            base_info->action_tbl_offset = hash_lkp_rst_ctl.ipv4_mcast_lookup_table_base;
            break;
        case DS_IPV6_UCAST_HASH_KEY0:
            base_info->key_bucket_base = hash_ctl.ipv6_ucast_table_base;
            base_info->hash_action_tbl_pos = hash_lkp_rst_ctl.ipv6_ucast_lookup_table_base_pos;
            base_info->action_tbl_offset = hash_lkp_rst_ctl.ipv6_ucast_lookup_table_base;
            break;
        case DS_IPV6_MCAST_HASH_KEY0:
            base_info->key_bucket_base = hash_ctl.ipv6_mcast_table_base;
            base_info->hash_action_tbl_pos = hash_lkp_rst_ctl.ipv6_mcast_lookup_table_base_pos;
            base_info->action_tbl_offset = hash_lkp_rst_ctl.ipv6_mcast_lookup_table_base;
            break;
        /* need to consider OAM hash key ??? */
        default:
            break;
    }

    return DRV_E_NONE;
}

/**
 @brief add hash entry after lkp operation on real chip
*/
int32
drv_chip_hash_key_add_entry(uint8 chip_id, void *add_para)
{
    uint32 cmd;
    uint32 trigger = 1;
    uint32 write_bit = 1;
    uint32 time_out = 0;
    int32 ret = DRV_E_NONE;

    /* check para */
    DRV_CHIP_ID_VALID_CHECK(chip_id);

    TBL_LOCK(chip_id);
    write_bit = 1;
    cmd = DRV_IOW(IOC_REG, HASH_DS_CTL_CPU_KEY_REQ, HASH_DS_CTL_CPU_KEY_REQ_CPU_KEY_REQ_WR);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &write_bit);
    if (ret < DRV_E_NONE)
    {
        TBL_UNLOCK(chip_id);
        return ret;
    }

    trigger = TRUE;
    cmd = DRV_IOW(IOC_REG, HASH_DS_CTL_CPU_KEY_REQ, HASH_DS_CTL_CPU_KEY_REQ_CPU_KEY_REQ_VALID);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &trigger);
    if (ret < DRV_E_NONE)
    {
        TBL_UNLOCK(chip_id);
        return ret;
    }

    /* waitting the trigger = 0 */
    while (trigger)
    {
        cmd = DRV_IOR(IOC_REG, HASH_DS_CTL_CPU_KEY_REQ, HASH_DS_CTL_CPU_KEY_REQ_CPU_KEY_REQ_VALID);
        ret = drv_reg_ioctl(chip_id, 0, cmd, &trigger);
        if (ret < DRV_E_NONE)
        {
            TBL_UNLOCK(chip_id);
            return ret;
        }

        /* Check Time Out */
        if ((time_out++) > DRV_TIME_OUT)
        {
            TBL_UNLOCK(chip_id);
            return DRV_E_TIME_OUT;
        }
    }

    TBL_UNLOCK(chip_id);

    return ret;
}

/**
 @brief delete hash entry according to detailed key value on real chip
*/
int32
drv_chip_hash_key_del_entry_by_key(uint8 chip_id, void* del_para)
{
    /* TBD */

    /* process parity error */

    return DRV_E_NONE;
}

/**
 @brief delete hash entry according to hash index on real chip
*/
int32
drv_chip_hash_key_del_entry_by_index(uint8 chip_id, void *del_para)
{
    uint32 cmd;
    hash_ds_ctl_cpu_key_req_t hash_req;
    uint32 trigger = 1;
    uint32 time_out = 0;
    uint8 index = 0;
    int32 ret = DRV_E_NONE;
    uint32 tbl_id;
    uint32 tbl_idx;
    /* (tbl_id, tbl_idx) -> hash_idx (hash ram's index, unit:72 bits) */
    uint32 hash_idx = 0;

    /* Hash table size */
    uint32 table_size = 0;

    /* Use to store hashkey and the action table's position base info */
    hash_tblbase_info_t hashtbl_pos_info;

    hash_del_para_s *para = NULL;

    /* check para */
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_PTR_VALID_CHECK(del_para);

    para = (hash_del_para_s *)del_para;
    tbl_id = para->table_id;
    tbl_idx = para->table_index;
    table_size = drv_tbls_list[tbl_id].entry_size;

    /* according to hash tablid to get hashkey table and action table's position info */
    kal_memset(&hashtbl_pos_info, 0, sizeof(hashtbl_pos_info));
    DRV_IF_ERROR_RETURN(_drv_chip_hash_tblid_get_tblbase_info(tbl_id, &hashtbl_pos_info));

    /* according to the action table's index to get the hash key's index in the total hash ram */
    if (16 == table_size)     /* hash key size is 72 bits */
    {
        /* Each bucket is 288bits, hash sram's unit is 72bits, so shift 10 bits */
        hash_idx = (hashtbl_pos_info.key_bucket_base<<10) + tbl_idx;
    }
    else if (32 == table_size)  /* hash key size is 144 bits */
    {
        /* Each bucket is 288bits, hash sram's unit is 72bits, so shift 10 bits */
        hash_idx = (hashtbl_pos_info.key_bucket_base<<10) + tbl_idx*2;
    }
    else
    {
        DRV_DBG_INFO("%%ERROR! When Do Hash I/O operation, find invalid hash table size!\n");
        DRV_DBG_INFO("%%Invalid Hash table size = %d Bytes\n", table_size);
        return DRV_E_INVALID_HASH_TABLE_SIZE;
    }

    TBL_LOCK(chip_id);
    for(index = 0; index < (table_size/16); index++ )
    {
        kal_memset(&hash_req, 0, sizeof(hash_ds_ctl_cpu_key_req_t));
        hash_req.cpu_ip31_to0 = hash_idx + index;
        hash_req.cpu_key_req_del = 1;

        cmd = DRV_IOW(IOC_REG, HASH_DS_CTL_CPU_KEY_REQ, DRV_ENTRY_FLAG);
        ret = drv_reg_ioctl(chip_id, 0, cmd, &hash_req);
        if (ret < DRV_E_NONE)
        {
            TBL_UNLOCK(chip_id);
            return ret;
        }

        trigger = TRUE;
        cmd = DRV_IOW(IOC_REG, HASH_DS_CTL_CPU_KEY_REQ, HASH_DS_CTL_CPU_KEY_REQ_CPU_KEY_REQ_VALID);
        ret = drv_reg_ioctl(chip_id, 0, cmd, &trigger);
        if (ret < DRV_E_NONE)
        {
            TBL_UNLOCK(chip_id);
            return ret;
        }

        /* waitting the trigger = 0 */
        while (trigger)
        {
            cmd = DRV_IOR(IOC_REG, HASH_DS_CTL_CPU_KEY_REQ, HASH_DS_CTL_CPU_KEY_REQ_CPU_KEY_REQ_VALID);
            ret = drv_reg_ioctl(chip_id, 0, cmd, &trigger);
            if (ret < DRV_E_NONE)
            {
                TBL_UNLOCK(chip_id);
                return ret;
            }

            /* Check Time Out */
            if ((time_out++) > DRV_TIME_OUT)
            {
                TBL_UNLOCK(chip_id);
                return DRV_E_TIME_OUT;
            }
        }
    }
    TBL_UNLOCK(chip_id);

    return ret;
}


/**
 @brief Hash lookup I/O control API on real chip
*/
int32
drv_chip_hash_lookup(uint8 chip_id,
                      uint32* key,
                      hash_ds_ctl_cpu_key_status_t* hash_cpu_status,
                      cpu_req_hash_key_type_e cpu_hashkey_type)
{
    ds_mac_hash_key0_t* mac_key = NULL;
    ds_ipv4_ucast_hash_key0_t* v4uc_key = NULL;
    ds_ipv4_mcast_hash_key0_t* v4mc_key = NULL;
    ds_ipv6_ucast_hash_key0_t* v6uc_key = NULL;
    ds_ipv6_mcast_hash_key0_t* v6mc_key = NULL;
    uint32 cmd;
    hash_ds_ctl_cpu_key_req_t hash_lkp_cpu_req;
    uint32 time_out = 0;
    uint32 trigger = TRUE;
    int32 ret = DRV_E_NONE;

    if (cpu_hashkey_type >= CPU_HASH_KEY_TYPE_RESERVED0)
    {
        return DRV_E_INVALID_PARAM;
    }

    DRV_PTR_VALID_CHECK(key);
    DRV_PTR_VALID_CHECK(hash_cpu_status);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    hash_cpu_status->cpu_key_hit = FALSE;
    hash_cpu_status->cpu_lu_index = DRV_HASH_INVALID_INDEX;

    TBL_LOCK(chip_id);
    kal_memset(&hash_lkp_cpu_req, 0, sizeof(hash_ds_ctl_cpu_key_req_t));
    hash_lkp_cpu_req.cpu_key_type = cpu_hashkey_type;
    switch (cpu_hashkey_type)
    {
        case CPU_HASH_KEY_TYPE_MAC_DA:
        case CPU_HASH_KEY_TYPE_MAC_SA:
          /*  table_id = DS_MAC_HASH_KEY0;          mac hash key */
            if (0 == DRV_TBL_MAX_INDEX(DS_MAC_HASH_KEY0))
            {
                TBL_UNLOCK(chip_id);
                return DRV_E_NONE;
            }

            mac_key = (ds_mac_hash_key0_t *)key;
            hash_lkp_cpu_req.cpu_ip31_to0 = mac_key->mapped_macl;
            hash_lkp_cpu_req.cpu_ip63_to32 = mac_key->mapped_mach;
            hash_lkp_cpu_req.cpu_ip95_to64 = 0;
            hash_lkp_cpu_req.cpu_ip127_to96 = 0;
            hash_lkp_cpu_req.cpu_vrf_id = mac_key->mapped_vlanid;
            break;
        case CPU_HASH_KEY_TYPE_IPV4_UC:
           /*  table_id = DS_IPV4_UCAST_HASH_KEY0;  ipv4uc hash key */
            if (0 == DRV_TBL_MAX_INDEX(DS_IPV4_UCAST_HASH_KEY0))
            {
                TBL_UNLOCK(chip_id);
                return DRV_E_NONE;
            }
            v4uc_key = (ds_ipv4_ucast_hash_key0_t *)key;
            hash_lkp_cpu_req.cpu_ip31_to0 = v4uc_key->key_mapped_ip;
            hash_lkp_cpu_req.cpu_ip63_to32 = 0;
            hash_lkp_cpu_req.cpu_ip95_to64 = 0;
            hash_lkp_cpu_req.cpu_ip127_to96 = 0;
            hash_lkp_cpu_req.cpu_vrf_id = v4uc_key->key_vrfid;
            break;
        case CPU_HASH_KEY_TYPE_IPV4_MC:
         /*   table_id = DS_IPV4_MCAST_HASH_KEY0; ipv4mc hash key */
            if (0 == DRV_TBL_MAX_INDEX(DS_IPV4_MCAST_HASH_KEY0))
            {
                TBL_UNLOCK(chip_id);
                return DRV_E_NONE;
            }
            v4mc_key = (ds_ipv4_mcast_hash_key0_t *)key;
            hash_lkp_cpu_req.cpu_ip31_to0 = v4mc_key->key_mapped_ip;
            hash_lkp_cpu_req.cpu_ip63_to32 = 0;
            hash_lkp_cpu_req.cpu_ip95_to64 = 0;
            hash_lkp_cpu_req.cpu_ip127_to96 = 0;
            hash_lkp_cpu_req.cpu_vrf_id = v4mc_key->key_vrfid;
            break;
        case CPU_HASH_KEY_TYPE_IPV6_UC:
          /*  table_id = DS_IPV6_UCAST_HASH_KEY0;  ipv6uc hash key */
            if (0 == DRV_TBL_MAX_INDEX(DS_IPV6_UCAST_HASH_KEY0))
            {
                TBL_UNLOCK(chip_id);
                return DRV_E_NONE;
            }
            v6uc_key = (ds_ipv6_ucast_hash_key0_t *)key;
            hash_lkp_cpu_req.cpu_ip31_to0 = v6uc_key->key_ipda0;
            hash_lkp_cpu_req.cpu_ip63_to32 = v6uc_key->key_ipda1;
            hash_lkp_cpu_req.cpu_ip95_to64 = v6uc_key->key_ipda2;
            hash_lkp_cpu_req.cpu_ip127_to96 = v6uc_key->key_ipda3;
            hash_lkp_cpu_req.cpu_vrf_id = (v6uc_key->vrfid3<<9)
                                          | (v6uc_key->vrfid2<<6)
                                          | (v6uc_key->vrfid1<<3)
                                          | v6uc_key->vrfid0;
            break;
        case CPU_HASH_KEY_TYPE_IPV6_MC:
           /* table_id = DS_IPV6_MCAST_HASH_KEY0;  ipv6mc hash key */
            if (0 == DRV_TBL_MAX_INDEX(DS_IPV6_MCAST_HASH_KEY0))
            {
                TBL_UNLOCK(chip_id);
                return DRV_E_NONE;
            }
            v6mc_key = (ds_ipv6_mcast_hash_key0_t *)key;
            hash_lkp_cpu_req.cpu_ip31_to0 = v6mc_key->key_ipda0;
            hash_lkp_cpu_req.cpu_ip63_to32 = v6mc_key->key_ipda1;
            hash_lkp_cpu_req.cpu_ip95_to64 = v6mc_key->key_ipda2;
            hash_lkp_cpu_req.cpu_ip127_to96 = v6mc_key->key_ipda3;
            hash_lkp_cpu_req.cpu_vrf_id = (v6mc_key->vrfid3<<9)
                                          | (v6mc_key->vrfid2<<6)
                                          | (v6mc_key->vrfid1<<3)
                                          | v6mc_key->vrfid0;
            break;
        default:
            break;
    }
    hash_lkp_cpu_req.cpu_key_req_lu = TRUE;  /* Do hash lookup operation */

    /* set key and key type */
    cmd = DRV_IOW(IOC_REG, HASH_DS_CTL_CPU_KEY_REQ, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &hash_lkp_cpu_req);
    if (ret < DRV_E_NONE)
    {
       TBL_UNLOCK(chip_id);
       return ret;
    }

    /* set the trigger field */
    cmd = DRV_IOW(IOC_REG, HASH_DS_CTL_CPU_KEY_REQ, HASH_DS_CTL_CPU_KEY_REQ_CPU_KEY_REQ_VALID);
    ret = drv_reg_ioctl(chip_id, 0, cmd, &trigger);
    if (ret < DRV_E_NONE)
    {
       TBL_UNLOCK(chip_id);
       return ret;
    }

    while (trigger) /* wait the trigger = 0 */
    {
        cmd = DRV_IOR(IOC_REG, HASH_DS_CTL_CPU_KEY_REQ, HASH_DS_CTL_CPU_KEY_REQ_CPU_KEY_REQ_VALID);
        ret = drv_reg_ioctl(chip_id, 0, cmd, &trigger);
        if (ret < DRV_E_NONE)
        {
           TBL_UNLOCK(chip_id);
           return ret;
        }

        if ((time_out++) > DRV_TIME_OUT)   /* Check Time Out */
        {
            TBL_UNLOCK(chip_id);
            return DRV_E_TIME_OUT;
        }
    }

    /* get hash lkp result */
    /* Note:
       if cpu_key_hit = 1, cpu_lu_index is the hitted hash key absolute index,
       if cpu_key_hit = 0 & cpu_lu_index = 0x1FFFF, conflict, no hit and hash two bucket is full,
       if cpu_key_hit = 0 & cpu_lu_index != 0x1FFFF, no hit and return a idle index,
    */
    cmd = DRV_IOR(IOC_REG, HASH_DS_CTL_CPU_KEY_STATUS, DRV_ENTRY_FLAG);
    ret = (drv_reg_ioctl(chip_id, 0, cmd, hash_cpu_status));
    if (ret < DRV_E_NONE)
    {
       TBL_UNLOCK(chip_id);
       return ret;
    }

    TBL_UNLOCK(chip_id);

    return DRV_E_NONE;
}


