#ifndef __DAL_H__
#define __DAL_H__

#define HUMBER_PCI_READ_ADDR  0x0
#define HUMBER_PCI_READ_DATA  0xc
#define HUMBER_PCI_WRITE_ADDR 0x8
#define HUMBER_PCI_WRITE_DATA 0x4
#define HUMBER_PCI_STATUS     0x10

#define HUMBER_PCI_STATUS_IN_PROCESS      31
#define HUMBER_PCI_STATUS_BAD_PARITY      5
#define HUMBER_PCI_STATUS_CPU_ACCESS_ERR  4
#define HUMBER_PCI_STATUS_READ_CMD        3
#define HUMBER_PCI_STATUS_REGISTER_ERR    1
#define HUMBER_PCI_STATUS_REGISTER_ACK    0

#define HUMBER_PCI_ACCESS_TIMEOUT 0x6400

#define LINUX_DAL_NAME          "asic_allctrl"  /* "linux_dal" */
#define LINUX_DAL_DEV_NAME      "/dev/" LINUX_DAL_NAME
#define LINUX_DAL_DEV_MAJOR     99

enum cmd_type_e
{
    CMD_WRITE_CHIP,
    CMD_READ_CHIP,

    CMD_TYPE_MAX
};
typedef enum cmd_type_e cmd_type_t;


struct cmdpara_chip_s
{
    uint32 chip_id;     /*tmp should be uint8*/
    uint32 fpga_id;     /*tmp add*/
    uint32 reg_addr;
    uint32 value;
};
typedef struct cmdpara_chip_s cmdpara_chip_t;



#endif
