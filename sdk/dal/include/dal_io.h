#ifndef __LINUX_DAL_H__
#define __LINUX_DAL_H__

#include "kal.h"

/*
 * error codes
 */
enum err_code_e
{
    err_op_success,
    err_invalid_ptr,
    err_invalid_chip_id,
    err_invalid_file_name,
    err_invalid_size,
    err_invalid_other_para,
    err_failed_openfile,
    err_failed_readfile,
    err_failed_writefile,
    err_notinited_fd,

    err_maxcnt
};

#define OP_SUCCESS	err_op_success
#define E_PTR		err_invalid_ptr
#define E_CHIPID    err_invalid_chip_id
#define E_FNAME		err_invalid_file_name
#define E_SIZE		err_invalid_size
#define E_OPARA		err_invalid_other_para
#define E_FILEO		err_failed_openfile
#define E_FILER		err_failed_readfile
#define E_FILEW		err_failed_writefile
#define E_FD		err_notinited_fd



typedef struct dal_op_s{
    int32  (*pci_read)(uint8 chip_id, uint32 offset);
    int32   (*pci_write)(uint8 chip_id, uint32 offset, uint32 value);
} dal_op_t;

int32 dal_usrctrl_init(dal_op_t *dal_op);
int32 dal_usrctrl_write_chip(uint8 chip_id, uint32 offset, uint32 value);
int32 dal_usrctrl_read_chip(uint8 chip_id, uint32 offset, uint32 p_value);
int32 ctckal_usrctrl_read_bay_4w(uint32 chip_id, uint32 fpga_id, uint32 reg_offset, uint32 p_value);
int32 ctckal_usrctrl_write_bay_4w(uint32 chip_id, uint32 fpga_id, uint32 reg_offset, uint32 p_value);
int32 ctckal_usrctrl_read_bay_4w(uint32 chip_id, uint32 fpga_id, uint32 reg_offset, uint32 p_value);
int32 ctckal_usrctrl_write_bay_4w(uint32 chip_id, uint32 fpga_id, uint32 reg_offset, uint32 p_value);
#endif
