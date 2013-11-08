/**
 *  Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "sal.h"

extern int kal_ioctl(int cmd, void *arg);

/*****************************************************************************
 * defines
 *****************************************************************************/
#define FLASH_BLK_SIZE                  (128 * 1024)
#define FLASH_SIZE_PER_BAY              (2 * 1024 * 1024)

#define MAC_ID_MAXCNT                   3
#define PORT_ID_MAXCNT                  8

#define KLOGCTL_DISABLE                 6
#define KLOGCTL_ENABLE                  7
#define KLOGCTL_SET_LEVEL               8


/*****************************************************************************
 * typedef
 *****************************************************************************/
enum cmd_type_e
{
    /* PLX9056 */
    cmd_write_plx9056pci,
    cmd_read_plx9056pci,
    cmd_write_plx9056local,
    cmd_read_plx9056local,

    /* Flash */
    cmd_write_flash,
    cmd_read_flash,
    /* Diag */
    cmd_read_flashstatus,
    cmd_write_flashcmd,
    cmd_read_flashcmd,
    cmd_write_flash16,
    cmd_read_flash16,
    cmd_erase_flash16,

	/* SRAM */
	cmd_write_sram,
	cmd_read_sram,

    /* MAC */
    cmd_write_mac,
    cmd_read_mac,

    /* PHY */
    cmd_write_phy,
    cmd_read_phy,

    /* Floodgate */
    cmd_write_floodgate,
    cmd_read_floodgate,
    cmd_diag_write_floodgate_data,
    cmd_diag_read_floodgate_data,
    cmd_diag_write_floodgate_ctrl,
    cmd_diag_read_floodgate_ctrl,

    /* Bay */
    cmd_write_bay,
    cmd_read_bay,
    cmd_do_test,
    cmd_show_testresult,

    /* PCI device */
    cmd_write_pcidev_mem,
    cmd_read_pcidev_mem,
    cmd_write_pcidev_pci,
    cmd_read_pcidev_pci,

    /* Misc */
    reset_tcam_nl,
    reset_tcam_idt,
    reset_dp83816,
    reset_gd82551,
    reset_bp_time,
    reset_bp_logic,
    reset_bay_time,
    reset_bay_logic,
    reset_mac,
    reset_phy,

    /* Debug */
    enable_debug,
    disable_debug,

    cmd_type_maxcnt
};
typedef enum cmd_type_e	cmd_type_t;


/* PCI device */
struct cmdpara_pcidev_s
{
    usrctrl_offset_t reg_offset;
    usrctrl_value_t  value;
};
typedef struct cmdpara_pcidev_s cmdpara_pcidev_t;


/* floodgate FGPA */
struct cmdpara_floodgate_diag_s
{
    usrctrl_value_t value;
};
typedef struct cmdpara_floodgate_diag_s cmdpara_floodgate_diag_t;

struct cmdpara_floodgate_s
{
    usrctrl_name_t  name;
    usrctrl_value_t value;
};
typedef struct cmdpara_floodgate_s cmdpara_floodgate_t;


/* flash */
struct cmdpara_flash_s
{
    usrctrl_id_t   bay_id;
    usrctrl_addr_t p_user_data;
    usrctrl_size_t len;
};
typedef struct cmdpara_flash_s cmdpara_flash_t;

struct cmdpara_flash16_s
{
    usrctrl_offset_t  reg_offset;
    usrctrl_value_t   value;
    usrctrl_value_t   diag;
};
typedef struct cmdpara_flash16_s cmdpara_flash16_t;

/* sram */
struct cmdpara_sram_s
{
    usrctrl_id_t   fpga_id;
    usrctrl_addr_t p_user_data;
    usrctrl_size_t len;
};
typedef struct cmdpara_sram_s cmdpara_sram_t;

/* bay */
struct cmdpara_bay_s
{
    usrctrl_id_t     bay_id;
	usrctrl_id_t	 fpga_id;
    usrctrl_offset_t reg_addr;
    usrctrl_value_t  value;
};
typedef struct cmdpara_bay_s cmdpara_bay_t;

struct cmdpara_testbay_s
{
    usrctrl_id_t    src_id;
    usrctrl_id_t    dst_id;
    usrctrl_value_t result;

    union
    {
        usrctrl_value_t  data;
        usrctrl_value_t  long_cnt;
    } para;
};
typedef struct cmdpara_testbay_s cmdpara_testbay_t;


/* mac */
struct cmdpara_mac_s
{
    usrctrl_id_t    mac_id;
    usrctrl_id_t    port_id;
    usrctrl_name_t  reg_name;
    usrctrl_value_t value;
};
typedef struct cmdpara_mac_s cmdpara_mac_t;


/* phy */
struct cmdpara_phy_s
{
    usrctrl_id_t    phy_id;
    usrctrl_name_t  reg_name;
    usrctrl_value_t value;
};
typedef struct cmdpara_phy_s cmdpara_phy_t;

/* 9056 */
struct cmdpara_plx9056_s
{
    usrctrl_name_t  reg_name;
    usrctrl_value_t value;
};
typedef struct cmdpara_plx9056_s cmdpara_plx9056_t;


struct cmdpara_sysctrl_s
{
    usrctrl_value_t value;
};
typedef struct cmdpara_sysctrl_s cmdpara_sysctrl_t;


/*****************************************************************************
 * macros
 *****************************************************************************/
#define CMDPARA_ENCODE_FLASH(bay_id_1, p_user_data_1, len_1, para_flash) \
        para_flash.bay_id = bay_id_1;                                    \
        para_flash.p_user_data = p_user_data_1;                          \
        para_flash.len = len_1

#define CMDPARA_DECODE_FLASH(bay_id_1, p_user_data_1, len_1, para_flash) \
        bay_id_1 = para_flash.bay_id;                                    \
        p_user_data_1 = para_flash.p_user_data;                          \
        len_1 = para_flash.len

#define CMDPARA_ENCODE_FLOODGATE(name_1, value_1, para_fg)               \
        memcpy(para_fg.name, name_1, NAME_SIZE);                         \
        para_fg.value = value_1

#define CMDPARA_DECODE_FLOODGATE(name_1, value_1, para_fg)               \
        memcpy(name_1, para_fg.name, NAME_SIZE);                         \
        value_1 = para_fg.value

#define CMDPARA_ENCODE_BAY(bay_id_1, fpga_id_1, reg_addr_1, value_1, para_bay)\
        para_bay.bay_id	= bay_id_1;                                      \
		para_bay.fpga_id = fpga_id_1;				 \
        para_bay.reg_addr = reg_addr_1;                                  \
        para_bay.value = value_1

#define CMDPARA_DECODE_BAY(bay_id_1, fpga_id_1, reg_addr_1, value_1, para_bay)\
        bay_id_1 = para_bay.bay_id;                                      \
		fpga_id_1 = para_bay.fpga_id_1;				 \
        reg_addr_1 = para_bay.reg_addr;                                  \
        value_1 = para_bay.value

#define CMDPARA_ENCODE_MAC(mac_id_1, port_id_1, reg_name_1, value_1, para_mac)\
        para_mac.mac_id = mac_id_1;                                      \
        para_mac.port_id = port_id_1;                                    \
        para_mac.value = value_1;                                        \
        memcpy(para_mac.reg_name, reg_name_1, NAME_SIZE)

#define CMDPARA_DECODE_MAC(mac_id_1, port_id_1, reg_name_1, value_1, para_mac)\
        mac_id_1 = para_mac.mac_id;                                      \
        port_id_1 = para_mac.port_id;                                    \
        value_1 = para_mac.value;                                        \
        memcpy(reg_name_1, para_mac.reg_name, NAME_SIZE)

#define CMDPARA_ENCODE_PHY(phy_id_1, reg_name_1, value_1, para_phy)      \
        para_phy.phy_id = phy_id_1;                                      \
        para_phy.value = value_1;                                        \
        memcpy(para_phy.reg_name, reg_name_1, NAME_SIZE)

#define CMDPARA_DECODE_PHY(phy_id_1, reg_name_1, value_1, para_phy)      \
        phy_id_1 = para_phy.mac_id;                                      \
        value_1 = para_phy.value;                                        \
        memcpy(reg_name_1, para_phy.reg_name, NAME_SIZE)

#define CMDPARA_ENCODE_PLX9056(reg_name_1, value_1, para_plx9056)        \
        para_plx9056.value = value_1;                                    \
        memcpy(para_plx9056.reg_name, reg_name_1, NAME_SIZE)

#define CMDPARA_DECODE_PLX9056(reg_name_1, value_1, para_plx9056)        \
        value_1 = para_plx9056.value;                                    \
        memcpy(reg_name_1, para_plx9056.reg_name, NAME_SIZE)

#define CMDPARA_ENCODE_SYSCTRL(value_1, para_sysctrl)                    \
        para_sysctrl.value = value_1

#define CMDPARA_DECODE_SYSCTRL(value_1, para_sysctrl)                    \
        value_1 = para_sysctrl.value

#define CMDPARA_ENCODE_PCIDEV(reg_offset_1, value_1, para_pcidev)	\
        para_pcidev.reg_offset = reg_offset_1;                          \
        para_pcidev.value = value_1

#define CMDPARA_DECODE_PCIDEV(reg_offset_1, value_1, para_pcidev)	\
        reg_offset_1 = para_pcidev.reg_offset;                          \
        value_1     = para_pcidev.value

#define CHECK_FD(fd)                    if (fd < 0) return E_FD
#define CHECK_PTR(ptr)                  if (!ptr) return E_PTR
#define CHECK_MAC_ID(mac_id)            do{;} while(0)
#define CHECK_BAY_ID(bay_id)            do{;} while(0)
#define CHECK_PHY_ID(phy_id)            do{;} while(0)
#define CHECK_FPGA_ID(fpga_id)          do{;} while(0)
#define CHECK_REG_NAME(reg_name)        do{;} while(0)
#define CHECK_FILE_NAME(filename)       do{;} while(0)
#define DEAL_ERR_CODE(err)              do{;} while(0)

#define DO_CMD(cmd, p_para) ctckal_usrctrl_do_cmd(cmd, p_para)

#define IOCTL_FILE_FD                   usrctrl_fd


/*****************************************************************************
 * global variables
 *****************************************************************************/
static int32 usrctrl_inited = 1;
static int32 usrctrl_fd  = 0;
#if 0 /* XXX _KAL_LINUX_UM */
static char *usrctrl_fn = "/dev/ctc_allctrl";
#endif

static int32 ctckal_usrctrl_do_cmd(uint32 cmd, uint32 p_para);


/*****************************************************************************
 * static functions
 *****************************************************************************/
static int32
ctckal_usrctrl_do_cmd(uint32 cmd, uint32 p_para)
{
    CHECK_FD(IOCTL_FILE_FD);
    CHECK_PTR(p_para);

    return kal_ioctl(cmd, (void *)p_para);
}

/*****************************************************************************
 * exported functions
 *****************************************************************************/
int32
ctckal_usrctrl_init(void)
{
    if (usrctrl_inited)
        return OP_SUCCESS;

#if 0 /* XXX _KAL_LINUX_UM */
    usrctrl_fd = open(usrctrl_fn, O_RDWR);
    if (usrctrl_fd < 0)
    {
        printf("\nwarning: no hardware device /dev/ctc_allctrl\n");
        return -1;
    }
#endif

    usrctrl_fd = 0;
    usrctrl_inited = 1;
    return OP_SUCCESS;
}

/*
 * Floodgate FPGA
 */
int32
ctckal_usrctrl_write_pcidev_mem(uint32 reg_offset, uint32 value)
{
    int32 ret;
    cmdpara_pcidev_t cmdpara_pcidev;

    if (!usrctrl_inited)
        return -1;

    cmdpara_pcidev.reg_offset = reg_offset;
    cmdpara_pcidev.value = value;
    ret = DO_CMD(cmd_write_pcidev_mem, (uint32)&cmdpara_pcidev);

    return ret;
}

int32
ctckal_usrctrl_read_pcidev_mem(uint32 reg_offset, void * p_value)
{
    int32 ret;
    cmdpara_pcidev_t cmdpara_pcidev;

    if (!usrctrl_inited)
        return -1;

    cmdpara_pcidev.reg_offset = reg_offset;
    ret = DO_CMD(cmd_read_pcidev_mem, (uint32)&cmdpara_pcidev);
    *(uint32 *)p_value = cmdpara_pcidev.value;

    return ret;
}

int32
ctckal_usrctrl_write_pcidev_pci(uint32 reg_offset, uint32 value)
{
    int32 ret;
    cmdpara_pcidev_t cmdpara_pcidev;

    if (!usrctrl_inited)
        return -1;

    cmdpara_pcidev.reg_offset = reg_offset;
    cmdpara_pcidev.value = value;
    ret = DO_CMD(cmd_write_pcidev_pci, (uint32)&cmdpara_pcidev);

    return ret;
}

int32
ctckal_usrctrl_read_pcidev_pci(uint32 reg_offset, void * p_value)
{
    int32 ret;
    cmdpara_pcidev_t cmdpara_pcidev;

    if (!usrctrl_inited)
        return -1;

    cmdpara_pcidev.reg_offset = reg_offset;
    ret = DO_CMD(cmd_read_pcidev_pci, (uint32)&cmdpara_pcidev);
    *(uint32 *)p_value = cmdpara_pcidev.value;

    return ret;
}

int32
ctckal_usrctrl_diag_write_floodgate_data(uint32 value)
{
    int32 ret;
    cmdpara_floodgate_diag_t cmdpara_floodgate_diag;

    if (!usrctrl_inited)
        return -1;

    cmdpara_floodgate_diag.value = value;
    ret = DO_CMD(cmd_diag_write_floodgate_data,
                 (uint32)&cmdpara_floodgate_diag);

    return ret;
}

int32
ctckal_usrctrl_diag_read_floodgate_data(uint32 p_value)
{
    int32 ret;
    cmdpara_floodgate_diag_t cmdpara_floodgate_diag;

    if (!usrctrl_inited)
        return -1;

    ret = DO_CMD(cmd_diag_read_floodgate_data,
                 (uint32)&cmdpara_floodgate_diag);
    *(uint32 *)p_value = cmdpara_floodgate_diag.value;

    return ret;
}

int32
ctckal_usrctrl_diag_write_floodgate_ctrl(uint32 value)
{
    int32 ret;
    cmdpara_floodgate_diag_t cmdpara_floodgate_diag;

    if (!usrctrl_inited)
        return -1;

    cmdpara_floodgate_diag.value = value;
    ret = DO_CMD(cmd_diag_write_floodgate_ctrl,
                 (uint32)&cmdpara_floodgate_diag);

    return ret;
}

int32
ctckal_usrctrl_diag_read_floodgate_ctrl(uint32 p_value)
{
    int32 ret;
    cmdpara_floodgate_diag_t cmdpara_floodgate_diag;

    if (!usrctrl_inited)
        return -1;

    ret = DO_CMD(cmd_diag_read_floodgate_ctrl,
                 (uint32)&cmdpara_floodgate_diag);
    *(uint32 *)p_value = cmdpara_floodgate_diag.value;

    return ret;
}

int32
ctckal_usrctrl_write_floodgate(usrctrl_name_t reg_name, uint32 value)
{
    int32 ret;
    cmdpara_floodgate_t cmdpara_floodgate;

    if (!usrctrl_inited)
        return -1;

    CMDPARA_ENCODE_FLOODGATE(reg_name, value, cmdpara_floodgate);

	#if DEBUG_PRINT_OUT
    printk("\n ctckal_usrctrl_write_floodgate(), %s, value = 0x%08lx \n",
           reg_name, value);
	#endif

    ret = DO_CMD(cmd_write_floodgate, (uint32)&cmdpara_floodgate);

    return ret;
}

int32
ctckal_usrctrl_read_floodgate(usrctrl_name_t reg_name, uint32 p_value)
{
    int32 ret;
    cmdpara_floodgate_t cmdpara_floodgate;

    if (!usrctrl_inited)
        return -1;

    CMDPARA_ENCODE_FLOODGATE(reg_name, 0x0, cmdpara_floodgate);
    ret = DO_CMD(cmd_read_floodgate, (uint32)&cmdpara_floodgate);
    *(uint32 *)p_value = cmdpara_floodgate.value;

	#if DEBUG_PRINT_OUT
	printk("\n ctckal_usrctrl_read_floodgate(), %s, value = 0x%08lx \n",
               reg_name, cmdpara_floodgate.value);
	#endif

    return ret;
}

#if 0
/*
 * Bay
 */
int32
ctckal_usrctrl_write_fpga(uint32 bay_id, uint32 fpga_id, uint32 reg_offset,
                         uint32 value)
{
    int32 ret;
    cmdpara_bay_t cmdpara_bay;

    if (!usrctrl_inited)
        return -1;

    CMDPARA_ENCODE_BAY(0x80000000|bay_id, fpga_id, reg_offset, value, cmdpara_bay);
    ret = DO_CMD(cmd_write_bay, (uint32)&cmdpara_bay);

    return ret;
}


int32
ctckal_usrctrl_read_fpga(uint32 bay_id, uint32 fpga_id, uint32 reg_offset,
                        uint32 p_value)
{
    int32 ret;
    cmdpara_bay_t cmdpara_bay;

    if (!usrctrl_inited)
        return -1;

    CMDPARA_ENCODE_BAY(0x80000000|bay_id, fpga_id, reg_offset, 0x0, cmdpara_bay);
    ret = DO_CMD(cmd_read_bay, (uint32)&cmdpara_bay);
    *(uint32 *)p_value = cmdpara_bay.value;

    return ret;
}

int32
ctckal_usrctrl_write_bay(uint32 bay_id, uint32 fpga_id, uint32 reg_offset,
                         uint32 value)
{
    int32 ret;
    cmdpara_bay_t cmdpara_bay;

    if (!usrctrl_inited)
        return -1;

    CMDPARA_ENCODE_BAY(bay_id, fpga_id, reg_offset, value, cmdpara_bay);
    ret = DO_CMD(cmd_write_bay, (uint32)&cmdpara_bay);

    return ret;
}

int32
ctckal_usrctrl_read_bay(uint32 bay_id, uint32 fpga_id, uint32 reg_offset,
                        uint32 p_value)
{
    int32 ret;
    cmdpara_bay_t cmdpara_bay;

    if (!usrctrl_inited)
        return -1;

    CMDPARA_ENCODE_BAY(bay_id, fpga_id, reg_offset, 0x0, cmdpara_bay);
    ret = DO_CMD(cmd_read_bay, (uint32)&cmdpara_bay);
    *(uint32 *)p_value = cmdpara_bay.value;

    return ret;
}
#else
int kal_write_bay(uint32 bay_id, uint32 fpga_id, uint32 reg_offset, uint32 value);
int kal_read_bay(uint32 bay_id, uint32 fpga_id, uint32 reg_offset, uint32 p_value);

int32 ctckal_usrctrl_write_fpga(uint32 bay_id, uint32 fpga_id, uint32 reg_offset, uint32 value)
{
    if (!usrctrl_inited)
        return -1;

    return kal_write_bay(0x80000000|bay_id, fpga_id, reg_offset, value);
}


int32 ctckal_usrctrl_read_fpga(uint32 bay_id, uint32 fpga_id, uint32 reg_offset, uint32 p_value)
{
    if (!usrctrl_inited)
        return -1;

    return kal_read_bay(0x80000000|bay_id, fpga_id, reg_offset, p_value);
}

int32 ctckal_usrctrl_write_bay(uint32 bay_id, uint32 fpga_id, uint32 reg_offset, uint32 value)
{
    if (!usrctrl_inited)
        return -1;

    return kal_write_bay(bay_id, fpga_id, reg_offset, value);
}

int32 ctckal_usrctrl_read_bay(uint32 bay_id, uint32 fpga_id, uint32 reg_offset, uint32 p_value)
{
    if (!usrctrl_inited)
        return -1;

    return kal_read_bay(bay_id, fpga_id, reg_offset, p_value);
}
#endif

int32
dump_pkt_ptr(uint32 start_ptr, uint32 num)
{
#define NEXT_PKT_ADDR_OFFSET (0x0c)
#define TBL_QDR_ADDR (0x05000000)

	uint32 first_pkt_addr, cur_pkt_addr, next_pkt_ptr;
	uint32 tmp;
	int32 i;
	int32 ret;

	/* 1. current ptr */
	printk(" %x", start_ptr);

	if(num == 1){
	    printk("\n");
		return 0;
	};

	/* 2. deal with more */
	first_pkt_addr = TBL_QDR_ADDR + (start_ptr << 4);
	cur_pkt_addr = first_pkt_addr;

	for(i = 1; i < num; i ++){
		ret = ctckal_usrctrl_read_bay(0, 0, cur_pkt_addr,
		                            (uint32)(&tmp));
		ret = ctckal_usrctrl_read_bay(0, 0, cur_pkt_addr + NEXT_PKT_ADDR_OFFSET,
		                            (uint32)(&next_pkt_ptr));
		#if 0
		printk("\n next pkt addr = %lx\n", cur_pkt_addr + NEXT_PKT_ADDR_OFFSET);
		#endif

		printk("->%x", next_pkt_ptr);
		cur_pkt_addr = TBL_QDR_ADDR + (next_pkt_ptr << 4);
	};

	return 0;
};

int32
ctckal_usrctrl_do_test(uint32 src_id, uint32 dst_id, uint32 data)
{
    int32 ret;
    uint32 result;
    cmdpara_testbay_t cmdpara_testbay;

    if (!usrctrl_inited)
        return -1;

#if DEBUG_PRINT_OUT
    printk("\nctckal_usrctrl_do_test(): "
           "src_id = %ld, dst_id = %ld, data = 0x%08lx \n",
           src_id, dst_id, data);
#endif

    cmdpara_testbay.src_id = src_id;
    cmdpara_testbay.dst_id = dst_id;
    cmdpara_testbay.para.data = data;

    ret = DO_CMD(cmd_do_test, (uint32)&cmdpara_testbay);
    result = cmdpara_testbay.result;

    if (0 == result)
    {
    	printk("  [OK]");
    }
    else
    {
    	printk("  [--Failed--]");
    }

    return ret;
}

int32
ctckal_usrctrl_show_testresult(uint32 src_id, uint32 dst_id, uint32 long_cnt)
{
    int32 ret;
    cmdpara_testbay_t cmdpara_testbay;

    if (!usrctrl_inited)
        return -1;

    cmdpara_testbay.src_id = src_id;
    cmdpara_testbay.dst_id = dst_id;
    cmdpara_testbay.para.long_cnt = long_cnt;

    ret = DO_CMD(cmd_show_testresult, (uint32)&cmdpara_testbay);

    printk("\nctckal_usrctrl_show_testresult(): "
           "dst_id = %d, long_cnt = %d, result = %d\n",
           dst_id, long_cnt, cmdpara_testbay.result);

    return ret;
}

#if 0 /* XXX _KAL_LINUX_UM */
/*
 * Flash
 */
int32
ctckal_usrctrl_download_fpga_image(usrctrl_name_t img_file_name, uint32 bay_id)
{
    int32 ret, img_size;
    int32 img_fd;
    struct stat img_stat;
    char *buf;
    cmdpara_flash_t cmdpara_flash;

    if (!usrctrl_inited)
        return -1;

    CHECK_PTR(img_file_name);
    CHECK_FILE_NAME(img_file_name);
    CHECK_BAY_ID(bay_id);

    img_fd = open(img_file_name, O_RDONLY);
    CHECK_FD(img_fd);

    ret = fstat(img_fd, &img_stat);
    DEAL_ERR_CODE(ret);

    img_size = img_stat.st_size;
    buf = malloc(img_size);
    CHECK_PTR(buf);
    read(img_fd, buf, img_size);

    CMDPARA_ENCODE_FLASH(bay_id, (uint32)buf, img_size, cmdpara_flash);
    ret = DO_CMD(cmd_write_flash, (uint32)&cmdpara_flash);

    free(buf);
    close(img_fd);

    return ret;
}


int32
ctckal_usrctrl_upload_fpga_image(usrctrl_name_t img_file_name, uint32 bay_id)
{
    int32 ret;
    char *buf;
    cmdpara_flash_t cmdpara_flash;
    int32 img_fd;

    if (!usrctrl_inited)
        return -1;

    CHECK_PTR(img_file_name);
    CHECK_FILE_NAME(img_file_name);
    CHECK_BAY_ID(bay_id);

	umask(0);

    img_fd = open(img_file_name, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP | S_IROTH|S_IWOTH);

    if (img_fd < 0)
    {
        printf("\nctckal_usrctrl_upload_fpga_image(), img_fd < 0!!!\n");
    }

    CHECK_FD(img_fd);

    buf = malloc(FLASH_SIZE_PER_BAY * 2);
    memset(buf, '6', FLASH_SIZE_PER_BAY * 2);
    CHECK_PTR(buf);

    CMDPARA_ENCODE_FLASH(bay_id, (uint32)buf, FLASH_SIZE_PER_BAY * 2,
                         cmdpara_flash);
    ret = DO_CMD(cmd_read_flash, (uint32)&cmdpara_flash);

    write(img_fd, buf, FLASH_SIZE_PER_BAY * 2);
    free(buf);
    close(img_fd);

    return ret;
}
#endif

int32
ctckal_usrctrl_read_flashstatus(uint32 reg_offset, uint32 p_value)
{
    int32 ret;
    cmdpara_flash16_t cmdpara_flash16;

    if (!usrctrl_inited)
        return -1;

    cmdpara_flash16.reg_offset = reg_offset;

    ret = DO_CMD(cmd_read_flashstatus, (uint32)(&cmdpara_flash16));
    *(uint32 *)p_value = cmdpara_flash16.value;

    return ret;
}

int32
ctckal_usrctrl_write_flashcmd(uint32 cmd_index, uint32 reg_offset,
                              uint32 value)
{
    int32 ret;
    cmdpara_flash16_t cmdpara_flash16;
    if (!usrctrl_inited)
        return -1;

    cmdpara_flash16.reg_offset = reg_offset;
    cmdpara_flash16.diag = cmd_index;
    cmdpara_flash16.value = value;

    ret = DO_CMD(cmd_write_flashcmd, (uint32)(&cmdpara_flash16));

    return ret;
}

int32
ctckal_usrctrl_read_flashcmd(uint32 cmd_index, uint32 reg_offset,
                             uint32 p_value)
{
    int32 ret;
    cmdpara_flash16_t cmdpara_flash16;
    if (!usrctrl_inited)
        return -1;

    cmdpara_flash16.reg_offset = reg_offset;
    cmdpara_flash16.diag = cmd_index;

    ret = DO_CMD(cmd_read_flashcmd, (uint32)(&cmdpara_flash16));
	*(uint32 *)p_value = cmdpara_flash16.value;

    return ret;
}

int32
ctckal_usrctrl_write_flash16(uint32 reg_offset, uint32 value)
{
    int32 ret;
    cmdpara_flash16_t cmdpara_flash16;

    if (!usrctrl_inited)
        return -1;

    cmdpara_flash16.reg_offset = reg_offset;
    cmdpara_flash16.value = value;

    ret = DO_CMD(cmd_write_flash16, (uint32)(&cmdpara_flash16));

    return ret;
}

int32
ctckal_usrctrl_read_flash16(uint32 reg_offset, uint32 p_value)
{
    int32 ret;
    cmdpara_flash16_t cmdpara_flash16;

    if (!usrctrl_inited)
        return -1;

    cmdpara_flash16.reg_offset = reg_offset;

    ret = DO_CMD(cmd_read_flash16, (uint32)(&cmdpara_flash16));
    *(uint32 *)p_value = cmdpara_flash16.value;

    return ret;
}

int32
ctckal_usrctrl_erase_flash16(uint32 reg_offset)
{
    int32 ret;
    cmdpara_flash16_t cmdpara_flash16;

    if (!usrctrl_inited)
        return -1;

    cmdpara_flash16.reg_offset = reg_offset;

    ret = DO_CMD(cmd_erase_flash16, (uint32)(&cmdpara_flash16));

    return ret;
}

/*
 * MAC
 */
int32
ctckal_usrctrl_write_mac(uint32 mac_id, uint32 port_id, usrctrl_name_t reg_name,
                      uint32 value)
{
    int32 ret;
    cmdpara_mac_t cmdpara_mac;

    if (!usrctrl_inited)
        return -1;

    CMDPARA_ENCODE_MAC(mac_id, port_id, reg_name, value, cmdpara_mac);
    ret = DO_CMD(cmd_write_mac, (uint32)&cmdpara_mac);

    return ret;
}

int32
ctckal_usrctrl_read_mac(uint32 mac_id, uint32 port_id, usrctrl_name_t reg_name,
                     uint32 p_value)
{
    int32 ret;
    cmdpara_mac_t cmdpara_mac;

    if (!usrctrl_inited)
        return -1;

    CMDPARA_ENCODE_MAC(mac_id, port_id, reg_name, 0x0, cmdpara_mac);
    ret = DO_CMD(cmd_read_mac, (uint32)&cmdpara_mac);
    *(uint32 *)p_value = cmdpara_mac.value;

	#if DEBUG_PRINT_OUT
	printk("\n ctckal_usrctrl_read_mac(), value = 0x%08lx, mac_id = 0x%08lx \n", cmdpara_mac.value, cmdpara_mac.mac_id);
	#endif

    return ret;
}

/*
 * PHY
 */
int32
ctckal_usrctrl_write_phy(uint32 phy_id, usrctrl_name_t reg_name, uint32 value)
{
    int32 ret;
    cmdpara_phy_t cmdpara_phy;

    if (!usrctrl_inited)
        return -1;

    CMDPARA_ENCODE_PHY(phy_id, reg_name, value, cmdpara_phy);
    ret = DO_CMD(cmd_write_phy, (uint32)&cmdpara_phy);

    return ret;
}

int32
ctckal_usrctrl_read_phy(uint32 phy_id, usrctrl_name_t reg_name, uint32 p_value)
{
    int32 ret;
    cmdpara_phy_t cmdpara_phy;

    if (!usrctrl_inited)
        return -1;

    CMDPARA_ENCODE_PHY(phy_id, reg_name, 0x0, cmdpara_phy);
    ret = DO_CMD(cmd_read_phy, (uint32)&cmdpara_phy);
    *(uint32 *)p_value = cmdpara_phy.value;

    return ret;
}


/*
 * PLX9056
 */
int32
ctckal_usrctrl_write_plx9056_pci(usrctrl_name_t reg_name, uint32 value)
{
    int32 ret;
    cmdpara_plx9056_t cmdpara_plx9056;

    if (!usrctrl_inited)
        return -1;

    CMDPARA_ENCODE_PLX9056(reg_name, value, cmdpara_plx9056);
    ret = DO_CMD(cmd_write_plx9056pci, (uint32)&cmdpara_plx9056);

    return ret;
}

int32
ctckal_usrctrl_read_plx9056_pci(usrctrl_name_t reg_name, uint32 p_value)
{
    int32 ret;
    cmdpara_plx9056_t cmdpara_plx9056;

    if (!usrctrl_inited)
        return -1;

    CMDPARA_ENCODE_PLX9056(reg_name, 0x0, cmdpara_plx9056);
    ret = DO_CMD(cmd_read_plx9056pci, (uint32)&cmdpara_plx9056);
    *(uint32 *)p_value = cmdpara_plx9056.value;

    return ret;
}

int32
ctckal_usrctrl_write_plx9056_local(usrctrl_name_t reg_name, uint32 value)
{
    int32 ret;
    cmdpara_plx9056_t cmdpara_plx9056;

    if (!usrctrl_inited)
        return -1;

    CMDPARA_ENCODE_PLX9056(reg_name, value, cmdpara_plx9056);

    ret = DO_CMD(cmd_write_plx9056local, (uint32)&cmdpara_plx9056);

    return ret;
}

int32
ctckal_usrctrl_read_plx9056_local(usrctrl_name_t reg_name, uint32 p_value)
{
    int32 ret;
    cmdpara_plx9056_t cmdpara_plx9056;

    if (!usrctrl_inited)
        return -1;

    CMDPARA_ENCODE_PLX9056(reg_name, 0x0, cmdpara_plx9056);

    ret = DO_CMD(cmd_read_plx9056local, (uint32)&cmdpara_plx9056);

    *(uint32 *)p_value = cmdpara_plx9056.value;

    return ret;
}

/*
 * Debug
 */
int32
ctckal_usrctrl_enable_debug(void)
{
#if 0 /* XXX _KAL_LINUX_UM */
    char dummy_buf[16];
#endif

    if (!usrctrl_inited)
        return -1;

#if 0 /* XXX _KAL_LINUX_UM */
    klogctl(KLOGCTL_ENABLE, dummy_buf, sizeof(dummy_buf));
    klogctl(KLOGCTL_SET_LEVEL, dummy_buf, 9);
#endif

    return OP_SUCCESS;
}

int32
ctckal_usrctrl_disable_debug(void)
{
#if 0 /* XXX _KAL_LINUX_UM */
    char dummy_buf[4];
#endif

    if (!usrctrl_inited)
        return -1;

#if 0 /* XXX _KAL_LINUX_UM */
    klogctl(KLOGCTL_DISABLE, dummy_buf, sizeof(dummy_buf));
#endif

    return OP_SUCCESS;
}

/*
 * Misc
 */
int32
ctckal_usrctrl_reset_tcam_nl(void)
{
    if (!usrctrl_inited)
        return -1;

   /* TBD */
    return OP_SUCCESS;
}

int32
ctckal_usrctrl_reset_tcam_idt(void)
{
    if (!usrctrl_inited)
        return -1;

	/* TBD */
	return OP_SUCCESS;
}

int32
ctckal_usrctrl_reset_dp83816(void)
{
    if (!usrctrl_inited)
        return -1;

    /* TBD */
    return OP_SUCCESS;
}

int32
ctckal_usrctrl_reset_gd82551(void)
{
    if (!usrctrl_inited)
        return -1;

    /* TBD */
    return OP_SUCCESS;
}

int32
ctckal_usrctrl_reset_bp_time(void)
{
    if (!usrctrl_inited)
        return -1;

    /* TBD */
    return OP_SUCCESS;
}

int32
ctckal_usrctrl_reset_bp_logic(void)
{
    if (!usrctrl_inited)
        return -1;

    /* TBD */
    return OP_SUCCESS;
}

int32
ctckal_usrctrl_reset_bay_time(void)
{
    if (!usrctrl_inited)
        return -1;

    /* TBD */
    return OP_SUCCESS;
}

int32
ctckal_usrctrl_reset_bay_logic(void)
{
    if (!usrctrl_inited)
        return -1;

    /* TBD */
    return OP_SUCCESS;
}

int32
ctckal_usrctrl_reset_mac(uint32 mac_id)
{
    if (!usrctrl_inited)
        return -1;

    /* TBD */
    return OP_SUCCESS;
}

int32
ctckal_usrctrl_reset_phy(uint32 phy_id)
{
    if (!usrctrl_inited)
        return -1;

    /* TBD */
    return OP_SUCCESS;
}

#if 0 /* XXX _KAL_LINUX_UM */
/*
 * Flash
 */
int32
ctckal_usrctrl_download_sram_image(usrctrl_name_t img_file_name,
                                   uint32 fpga_id)
{
    int32 ret, img_size;
    int32 img_fd;
    struct stat img_stat;
    char *buf;
    cmdpara_sram_t cmdpara_sram;

    if (!usrctrl_inited)
        return -1;

    CHECK_PTR(img_file_name);
    CHECK_FILE_NAME(img_file_name);
    CHECK_FPGA_ID(bay_id);

    img_fd = open(img_file_name, O_RDONLY);
    CHECK_FD(img_fd);

    ret = fstat(img_fd, &img_stat);
    DEAL_ERR_CODE(ret);

    img_size = img_stat.st_size;
    buf = malloc(img_size);
    CHECK_PTR(buf);
    read(img_fd, buf, img_size);

    cmdpara_sram.fpga_id = fpga_id;
    cmdpara_sram.p_user_data = (uint32)buf;
    cmdpara_sram.len = img_size;

    ret = DO_CMD(cmd_write_sram, (uint32)&cmdpara_sram);

    free(buf);
    close(img_fd);

    return ret;
}


int32
ctckal_usrctrl_upload_sram_image(usrctrl_name_t img_file_name, uint32 len,
                                 uint32 fpga_id)
{
    int32 ret;
    char *buf;
    cmdpara_sram_t cmdpara_sram;
    int32 img_fd;

    if (!usrctrl_inited)
        return -1;

    CHECK_PTR(img_file_name);
    CHECK_FILE_NAME(img_file_name);
    CHECK_BAY_ID(bay_id);

    img_fd = open(img_file_name, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);

    if (img_fd < 0)
    {
        printf("\n ctckal_usrctrl_upload_sram_image(), img_fd < 0!!! \n");
    }

    CHECK_FD(img_fd);

    buf = malloc(len);
    CHECK_PTR(buf);

    cmdpara_sram.fpga_id = fpga_id;
    cmdpara_sram.p_user_data = (uint32)buf;
    cmdpara_sram.len = len;

    ret = DO_CMD(cmd_read_sram, (uint32)&cmdpara_sram);

    write(img_fd, buf, len);
    free(buf);
    close(img_fd);

    return ret;
}
#endif

EXPORT_SYMBOL(ctckal_usrctrl_read_bay);
EXPORT_SYMBOL(ctckal_usrctrl_write_bay);
