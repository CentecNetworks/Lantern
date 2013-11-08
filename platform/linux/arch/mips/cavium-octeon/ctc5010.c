/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2004-2007 Cavium Networks
 * Copyright (C) 2008 Wind River Systems
 */
#include <linux/init.h>
#include <linux/console.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/serial.h>
#include <linux/smp.h>
#include <linux/types.h>
#include <linux/string.h>	/* for memset */
#include <linux/tty.h>
#include <linux/time.h>
#include <linux/platform_device.h>
#include <linux/serial_core.h>
#include <linux/serial_8250.h>
#ifdef CONFIG_BLK_DEV_INITRD
#include <linux/initrd.h>
#endif

#include <asm/processor.h>
#include <asm/reboot.h>
#include <asm/smp-ops.h>
#include <asm/system.h>
#include <asm/irq_cpu.h>
#include <asm/mipsregs.h>
#include <asm/bootinfo.h>
#include <asm/sections.h>
#include <asm/time.h>

#include <asm/octeon/octeon.h>
#include <asm/octeon/octeon-boot-info.h>

#include <asm/octeon/cvmx-clock.h>
#include <asm/octeon/cvmx-error.h>
#include <asm/octeon/cvmx-debug.h>
#include <asm/octeon/cvmx-pcie.h>

#include <linux/zlib.h>
#include <asm/octeon/ctc5010.h>

void __iomem *ctc_epld_logic_addr;
ctc_board_t ctc_bd_info;
EXPORT_SYMBOL(ctc_bd_info);

#define CTC_SYS_EPLD_SIZE   0x00001000 /* 4K bytes */
#define CTC_SYS_MAP_SIZE    0x00001000 /* 4K bytes */    

static char __initdata scratch[46912];  /* scratch space for gunzip, from zlib_inflate_workspacesize() */ 
static char __initdata fpga_loadbuf[524288]; /* the uncompressed fpga image store buf 512K*/
extern unsigned char humber_demo_lattice_bin_start;
extern unsigned char humber_demo_lattice_bin_end;
extern unsigned char seoul_g24eu_xilinx_bin_start;
extern unsigned char seoul_g24eu_xilinx_bin_end;

#define TRUE 1
#define FALSE 0
#define LATTICE_PROGRAM 22                     /*PC23*/
#define LATTICE_INIT      21                  /*PD21*/
#define LATTICE_DONE 19             /*PD19*/
#define LATTICE_CCLK   18                     /*PD18*/
#define LATTICE_DIN 20                       /*PD20*/
#define LATTICE_RST_IN        23                     /*PC24*/

#define XILINX_G24EU_PROGRAM 22                     /*PC23*/
#define XILINX_G24EU_INIT      21                  /*PD21*/
#define XILINX_G24EU_DONE 19             /*PD19*/
#define XILINX_G24EU_CCLK   18                     /*PD18*/
#define XILINX_G24EU_DIN 20                       /*PD20*/
#define XILINX_G24EU_RST_IN        23                     /*PC24*/

#define HEAD_CRC	2
#define EXTRA_FIELD	4
#define ORIG_NAME	8
#define COMMENT		0x10
#define RESERVED	0xe0

#define SPI_CLK_BIT      18                  /*PD18*/
#define SPI_DO_BIT       20                  /*PD20*/
#define SPI_DI_BIT       19                   /*PD19*/
#define SPI_CS_BIT       22                   /*PC23*/

#define SI_MASK  0x1
#define BIT_WIDTH_8  8

#define EPLD_ADDR_LEN  0x1
#define EPLD_DATA_LEN  0x1

#define AD9517_DATA_LENTH 1    /* data length (byte)*/  
#define AD9517_ADDRESS_LENTH 2  /* address length (byte)*/

#define DS3104_DATA_LENTH 1    /* data length (byte)*/  
#define DS3104_ADDRESS_LENTH 2  /* address length (byte)*/

static void spi_gpio_delay(void)
{ 
    udelay(1);
}

int gpio_init(void)
{
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(SPI_CS_BIT), 0x1);
       
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(SPI_CLK_BIT), 0x1);
        
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(SPI_DO_BIT), 0x1);
    
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(SPI_DI_BIT), 0x0);
    return 0;        
}

int spi_cs(int assert_cs)
{
    u64 mask = 0;

    mask = 0x1<<SPI_CS_BIT;
	if(assert_cs)
	{
		cvmx_write_csr(CVMX_GPIO_TX_SET, mask);
	}
	else
	{
		cvmx_write_csr(CVMX_GPIO_TX_CLR, mask);
	}

	return 0;
}

int spi_clk(int assert_clk)
{
    u64 mask = 0;

    mask = 0x1<<SPI_CLK_BIT;
	if(assert_clk)
	{
		cvmx_write_csr(CVMX_GPIO_TX_SET, mask);
	}
	else
	{
		cvmx_write_csr(CVMX_GPIO_TX_CLR, mask);
	}
	return 0;
}

int spi_out(int assert_out)
{
    u64 mask = 0;

    mask = 0x1<<SPI_DO_BIT;
	if(assert_out)
	{
		cvmx_write_csr(CVMX_GPIO_TX_SET, mask);
	}
	else
	{
		cvmx_write_csr(CVMX_GPIO_TX_CLR, mask);
	}
	return 0;
}

int spi_in(void)
{
    u64 val;
    u64 mask;

    val = cvmx_read_csr(CVMX_GPIO_RX_DAT);
    mask = 0x1<<SPI_DI_BIT;
    val = val ^ mask;
    if(val&mask)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

int spi_read(unsigned short s_addr, unsigned int* value, unsigned int alen, unsigned int dlen)
{
	unsigned int i;
	unsigned short addr;

    *value = 0x0;
	addr = s_addr;
	spi_cs(0);
	for(i=0; i<alen * BIT_WIDTH_8; i++)
	{
		spi_clk(0);
		spi_gpio_delay();
		if(addr&(SI_MASK<<(alen*BIT_WIDTH_8-1)))
		{
			spi_out(1);
            spi_gpio_delay();
		}
		else
		{
			spi_out(0);
            spi_gpio_delay();
		}
		spi_clk(1);
		spi_gpio_delay();
		addr <<= 1;
	}
	for(i=0; i<dlen*BIT_WIDTH_8; i++)
	{
		spi_clk(0);
		spi_gpio_delay();
		*value <<=1;		
		if(spi_in())
		{
			*value |= 1;
            spi_gpio_delay();
		}
		else
		{
			*value &= (~0x1);
            spi_gpio_delay();
		}
		spi_clk(1);
		spi_gpio_delay( );
	}
	spi_cs(1);
	return 0;
}

int spi_write(unsigned short s_addr, unsigned int s_value, unsigned int alen, unsigned int dlen)
{
	unsigned int i, value;
	unsigned short addr;
	
	addr = s_addr;
	value = s_value;
	
	spi_cs(0);
	for(i=0; i<alen*BIT_WIDTH_8; i++)
	{
		spi_clk(0);
		spi_gpio_delay( );
		if(addr & (SI_MASK<<(alen*BIT_WIDTH_8-1)))
		{
			spi_out(1);
            spi_gpio_delay();
		}
		else
		{
			spi_out(0);
            spi_gpio_delay();
		}
		spi_clk(1);
		spi_gpio_delay();
		addr <<= 1;
	}
	for(i=0; i<dlen*BIT_WIDTH_8; i++)
	{
		spi_clk(0);
		spi_gpio_delay();
		if(value&(SI_MASK<<(dlen*BIT_WIDTH_8-1)))
		{
			spi_out(1);
            spi_gpio_delay();
		}
		else
		{
			spi_out(0);
            spi_gpio_delay();
		}
		spi_clk(1);
		spi_gpio_delay();
		value <<= 1;
	}
	spi_cs(1);
	return 0;
}

typedef enum
{
    /*
            OUT0    OUT1    OUT2    OUT3    OUT4    OUT5    OUT6    OUT7
    TYPE_0  62.5    NO      62.5    NO      NO      NO      25      25    
    TYPE_1  156.25  156.25  156.25  NO      62.5    62.5    NO      50
    TYPE_2  156.25  NO      NO      NO      50      NO      25      NO
    TYPE_3  156.25  NO      156.25  156.25  62.5    62.5    NO      50
    TYPE_4  156.25  NO      62.5    NO      25      25      25      25
    
    */
    E_AD9517_CLOCK_TYPE_0 = 0,        
    E_AD9517_CLOCK_TYPE_1 ,          
    E_AD9517_CLOCK_TYPE_2 ,          
    E_AD9517_CLOCK_TYPE_3 ,   
    E_AD9517_CLOCK_TYPE_4  ,
    E_AD9517_CLOCK_TYPE_5  ,/*for seoul g24eu*/
} ad9517_clock_type_e;

#define AD_VCO_CAL_FINISHED 0x6
#define AD_DIGITAL_LOCK     0x0

#define AD9517_SPI_READ  0x1
#define AD9517_SPI_WRITE 0x0
#define AD9517_SPI_WIDTH_1 0x0

int ad9517_read(unsigned short addr, unsigned int* value)
{
    unsigned short new_addr = 0;
    new_addr = (addr) | (AD9517_SPI_WIDTH_1<<13) | (AD9517_SPI_READ<<15);//from AD9517 DATASHEET 
    return spi_read(new_addr , value, AD9517_ADDRESS_LENTH, AD9517_DATA_LENTH);
}

int ad9517_write(unsigned short addr, unsigned int value)
{
    unsigned short new_addr = 0;
    new_addr = (addr) | (AD9517_SPI_WIDTH_1<<13) | (AD9517_SPI_WRITE<<15);//from AD9517 DATASHEET
    return spi_write(new_addr , value, AD9517_ADDRESS_LENTH, AD9517_DATA_LENTH);
}
int
ad9517_dev_init(unsigned int clock_type)
{
    int ret = 0;
    unsigned int val = 0;
    unsigned char mask = 0;
    unsigned char time = 10;
    
    gpio_init();
    
    mask = (0x1<<AD_VCO_CAL_FINISHED)|(0x1<<AD_DIGITAL_LOCK);

    ret += ad9517_write( 0x0, 0x99); 
    ret += ad9517_write( 0x10, 0x7c); 
    ret += ad9517_write( 0x11, 0x14); 
    ret += ad9517_write( 0x13, 0x10 ); 
    ret += ad9517_write( 0x14, 0x3e); 
    ret += ad9517_write( 0x16, 0x06); 
    ret += ad9517_write( 0x18, 0x7); 
    ret += ad9517_write( 0x1b, 0xe3); 
    ret += ad9517_write( 0x1c, 0x2); 

    if(clock_type == E_AD9517_CLOCK_TYPE_5)
    {
        ret += ad9517_write( 0xf0, 0xc); 
    }
    
    if((clock_type == E_AD9517_CLOCK_TYPE_1) 
        || (clock_type == E_AD9517_CLOCK_TYPE_3))
    {
        ret += ad9517_write( 0xf1, 0x8); 
    }
    else if(clock_type == E_AD9517_CLOCK_TYPE_5)
    {
        ret += ad9517_write( 0xf1, 0xc); 
    }

     if((clock_type == E_AD9517_CLOCK_TYPE_3) || (clock_type == E_AD9517_CLOCK_TYPE_5))
    {
        ret += ad9517_write( 0xf5, 0x8); 
    }
          
    ret += ad9517_write( 0x1e0, 0x0); 
    ret += ad9517_write( 0x1e1, 0x2 ); 
    
    if(clock_type == E_AD9517_CLOCK_TYPE_0)
    {
        ret += ad9517_write( 0x140, 0x5b); 
    }
    else if((clock_type == E_AD9517_CLOCK_TYPE_1) 
        || (clock_type == E_AD9517_CLOCK_TYPE_3) 
        || (clock_type == E_AD9517_CLOCK_TYPE_4)
        || (clock_type == E_AD9517_CLOCK_TYPE_5))
    {
        ret += ad9517_write( 0x140, 0x5a); 
    }
    else if(clock_type == E_AD9517_CLOCK_TYPE_2)
    {
        ret += ad9517_write( 0x140, 0x4a); 
    }

    if(clock_type == E_AD9517_CLOCK_TYPE_0)
    {
        ret += ad9517_write( 0x141, 0x5b); 
    }
    else if((clock_type == E_AD9517_CLOCK_TYPE_1) 
        || (clock_type == E_AD9517_CLOCK_TYPE_3) 
        || (clock_type == E_AD9517_CLOCK_TYPE_4))
    {
        ret += ad9517_write( 0x141, 0x5a); 
    }
    else if(clock_type == E_AD9517_CLOCK_TYPE_5)
    {
        ret += ad9517_write( 0x141, 0x4a); 
    }
    
    if((clock_type == E_AD9517_CLOCK_TYPE_0) 
       || (clock_type == E_AD9517_CLOCK_TYPE_4)
       || (clock_type == E_AD9517_CLOCK_TYPE_5))
    {
        ret += ad9517_write( 0x142, 0x5a);     
    }
    else if((clock_type == E_AD9517_CLOCK_TYPE_1) || (clock_type == E_AD9517_CLOCK_TYPE_3))
    {
        ret += ad9517_write( 0x142, 0x4b);     
    }
    else if(clock_type == E_AD9517_CLOCK_TYPE_2)
    {
        ret += ad9517_write( 0x142, 0x43);     
    }

    if((clock_type == E_AD9517_CLOCK_TYPE_0) || (clock_type == E_AD9517_CLOCK_TYPE_4))
    {
        ret += ad9517_write( 0x143, 0x4a); 
    }
    else if((clock_type == E_AD9517_CLOCK_TYPE_1) || (clock_type == E_AD9517_CLOCK_TYPE_3))
    {
        ret += ad9517_write( 0x143, 0x5a); 
    }
    else if(clock_type == E_AD9517_CLOCK_TYPE_5)
    {
        ret += ad9517_write( 0x143, 0x5b); 
    }
              
    if(clock_type == E_AD9517_CLOCK_TYPE_0)
    {
        ret += ad9517_write( 0x190, 0x99); 
    }
    else if((clock_type == E_AD9517_CLOCK_TYPE_1) 
        || (clock_type == E_AD9517_CLOCK_TYPE_2) 
        || (clock_type == E_AD9517_CLOCK_TYPE_3)
        || (clock_type == E_AD9517_CLOCK_TYPE_4)
        || (clock_type == E_AD9517_CLOCK_TYPE_5))
    {
        ret += ad9517_write( 0x190, 0x33); 
    }
        
    ret += ad9517_write( 0x191, 0x0 ); 

    if((clock_type == E_AD9517_CLOCK_TYPE_0) 
        || (clock_type == E_AD9517_CLOCK_TYPE_4)
        || (clock_type == E_AD9517_CLOCK_TYPE_5))
    {
        ret += ad9517_write( 0x196, 0x99);     
    }
    else if((clock_type == E_AD9517_CLOCK_TYPE_1) || (clock_type == E_AD9517_CLOCK_TYPE_3))
    {
        ret += ad9517_write( 0x196, 0x33);     
    }

    if((clock_type == E_AD9517_CLOCK_TYPE_1) || (clock_type == E_AD9517_CLOCK_TYPE_3))
    {
        ret += ad9517_write( 0x199, 0x99);     
    }
    else if(clock_type == E_AD9517_CLOCK_TYPE_2)
    {
        ret += ad9517_write( 0x199, 0xbc);     
    }
    else if((clock_type == E_AD9517_CLOCK_TYPE_4) || (clock_type == E_AD9517_CLOCK_TYPE_5))
    {
        ret += ad9517_write( 0x199, 0xcb);     
    }

    if((clock_type == E_AD9517_CLOCK_TYPE_4) || (clock_type == E_AD9517_CLOCK_TYPE_5))
    {
        ret += ad9517_write( 0x19b, 0x0);     
    }

    if((clock_type != E_AD9517_CLOCK_TYPE_5)&&(clock_type != E_AD9517_CLOCK_TYPE_4))
    {
        ret += ad9517_write( 0x19c, 0x20 );     
    }
    
    
    if(clock_type == E_AD9517_CLOCK_TYPE_0)
    {
        ret += ad9517_write( 0x19e, 0x21);      
    }
    else if((clock_type == E_AD9517_CLOCK_TYPE_1) 
        || (clock_type == E_AD9517_CLOCK_TYPE_3) 
        || (clock_type == E_AD9517_CLOCK_TYPE_4))
    {
        ret += ad9517_write( 0x19e, 0xcb);      
    }
    else if(clock_type == E_AD9517_CLOCK_TYPE_5)
    {
        ret += ad9517_write( 0x19e, 0x44);      
    }

    if(clock_type == E_AD9517_CLOCK_TYPE_0)
    {
        ret += ad9517_write( 0x1a0, 0x44);    
    } 
    else if((clock_type == E_AD9517_CLOCK_TYPE_4) ||(clock_type == E_AD9517_CLOCK_TYPE_5))
    {
        ret += ad9517_write( 0x1a0, 0x0);     
    }
    
    if((clock_type == E_AD9517_CLOCK_TYPE_1) 
        || (clock_type == E_AD9517_CLOCK_TYPE_3)
        || (clock_type == E_AD9517_CLOCK_TYPE_5))
    {
        ret += ad9517_write( 0x1a1, 0x20);    
    } 
    
    ret += ad9517_write( 0x232, 0x1 ); 
    ret += ad9517_write( 0x18, 0x6); 
    ret += ad9517_write( 0x232, 0x1 ); 
    ret += ad9517_write( 0x18, 0x7); 
    ret += ad9517_write( 0x232, 0x1 ); 
    udelay(10);
    ret += ad9517_read(0x1f, &val); 
    while((mask != (val&mask))&&(--time))
    {        
        udelay(10);
        ret += ad9517_read(0x1f, &val); 
    }
    if(!time)
        return time;
    
    return ret;
}


#define DS3104_SPI_READ  0x1
#define DS3104_SPI_WRITE 0x0
/*1 = burst access, 0 = single access*/
#define DS3104_SPI_ACCESS_MODE 0x0

int ds3104_read(unsigned short addr, unsigned int* value)
{
    unsigned short new_addr = 0;
    new_addr = (addr<<1) | (DS3104_SPI_ACCESS_MODE) | (DS3104_SPI_READ<<15);
    return spi_read(new_addr , value, DS3104_ADDRESS_LENTH, DS3104_DATA_LENTH);
}

int ds3104_write(unsigned short addr, unsigned int value)
{
    unsigned short new_addr = 0;
    new_addr = (addr<<1) | (DS3104_SPI_ACCESS_MODE) | (DS3104_SPI_WRITE<<15);
    return spi_write(new_addr , value, DS3104_ADDRESS_LENTH, DS3104_DATA_LENTH);
}

int
ds3104_config(void)
{
    int ret = 0;
    
    gpio_init();
    spi_gpio_delay();
    
    ret += ds3104_write( 0x1ff, 0x0 ); 
    
    ret += ds3104_write( 0x1a8, 0x3 );
    ret += ds3104_write( 0x1ac, 0x2 );
    
    ret += ds3104_write( 0x1cc, 0x1 ); 
    
    ret += ds3104_write( 0x180, 0xc );
    ret += ds3104_write( 0x181, 0x7b );    
    ret += ds3104_write( 0x182, 0x0 );
    ret += ds3104_write( 0x183, 0x0 );
    ret += ds3104_write( 0x184, 0x0 );    

    ret += ds3104_write( 0x180, 0x8 );
    ret += ds3104_write( 0x181, 0xdf );    
    ret += ds3104_write( 0x182, 0x03 );
    ret += ds3104_write( 0x183, 0x0 );
    ret += ds3104_write( 0x184, 0x0 );    
    
    ret += ds3104_write( 0x180, 0x0 );
    ret += ds3104_write( 0x180, 0xc );
    ret += ds3104_write( 0x181, 0x79 );    
    ret += ds3104_write( 0x182, 0x0 );
    ret += ds3104_write( 0x183, 0x0 );
    ret += ds3104_write( 0x184, 0x0 );    

    
    ret += ds3104_write( 0x180, 0x8 );
    ret += ds3104_write( 0x181, 0xf5 );    
    ret += ds3104_write( 0x182, 0x0 );
    ret += ds3104_write( 0x183, 0x0 );
    ret += ds3104_write( 0x184, 0x0 );    
    
    
    ret += ds3104_write( 0x180, 0xd );
    ret += ds3104_write( 0x181, 0xad );    
    ret += ds3104_write( 0x182, 0x02 );
    ret += ds3104_write( 0x183, 0x0 );
    ret += ds3104_write( 0x184, 0x0 );  

    
    ret += ds3104_write( 0x180, 0x9 );
    ret += ds3104_write( 0x181, 0xd5 );    
    ret += ds3104_write( 0x182, 0x03 );
    ret += ds3104_write( 0x183, 0x16 );
    ret += ds3104_write( 0x184, 0x0 ); 

    
    ret += ds3104_write( 0x180, 0xd );
    ret += ds3104_write( 0x181, 0xdb );    
    ret += ds3104_write( 0x182, 0x02 );
    ret += ds3104_write( 0x183, 0x0 );
    ret += ds3104_write( 0x184, 0x0 );   

    
    ret += ds3104_write( 0x180, 0x9 );
    ret += ds3104_write( 0x181, 0xcd );    
    ret += ds3104_write( 0x182, 0xa1 );
    ret += ds3104_write( 0x183, 0x1 );
    ret += ds3104_write( 0x184, 0x0 );    

    
    ret += ds3104_write( 0x180, 0xd );
    ret += ds3104_write( 0x181, 0xdf );    
    ret += ds3104_write( 0x182, 0x02 );
    ret += ds3104_write( 0x183, 0x0 );
    ret += ds3104_write( 0x184, 0x0 );    

    ret += ds3104_write( 0x180, 0x9 );
    ret += ds3104_write( 0x181, 0xce );    
    ret += ds3104_write( 0x182, 0x03 );
    ret += ds3104_write( 0x183, 0x16 );
    ret += ds3104_write( 0x184, 0x0 );    
    /*58*/
    ret += ds3104_write( 0x180, 0xd );
    ret += ds3104_write( 0x181, 0xb7 );    
    ret += ds3104_write( 0x182, 0x03 );
    ret += ds3104_write( 0x183, 0x0 );
    ret += ds3104_write( 0x184, 0x0 );    

    ret += ds3104_write( 0x180, 0x9 );
    ret += ds3104_write( 0x181, 0xce );    
    ret += ds3104_write( 0x182, 0x51 );
    ret += ds3104_write( 0x183, 0x01 );
    ret += ds3104_write( 0x184, 0x0 );    

    ret += ds3104_write( 0x180, 0xd );
    ret += ds3104_write( 0x181, 0xc2 );    
    ret += ds3104_write( 0x182, 0x03 );
    ret += ds3104_write( 0x183, 0x0 );
    ret += ds3104_write( 0x184, 0x0 );    

    ret += ds3104_write( 0x180, 0x9 );
    ret += ds3104_write( 0x181, 0xce );    
    ret += ds3104_write( 0x182, 0x51 );
    ret += ds3104_write( 0x183, 0x4 );
    ret += ds3104_write( 0x184, 0x0 );    

    ret += ds3104_write( 0x180, 0xd );
    ret += ds3104_write( 0x181, 0xc9 );    
    ret += ds3104_write( 0x182, 0x03 );
    ret += ds3104_write( 0x183, 0x0 );
    ret += ds3104_write( 0x184, 0x0 );    

    ret += ds3104_write( 0x180, 0x9 );
    ret += ds3104_write( 0x181, 0xcf );    
    ret += ds3104_write( 0x182, 0xa1 );
    ret += ds3104_write( 0x183, 0x4 );
    ret += ds3104_write( 0x184, 0x0 );    

    ret += ds3104_write( 0x180, 0xd );
    ret += ds3104_write( 0x181, 0xcc );    
    ret += ds3104_write( 0x182, 0x03 );
    ret += ds3104_write( 0x183, 0x0 );
    ret += ds3104_write( 0x184, 0x0 );    
    
    ret += ds3104_write( 0x180, 0x9 );
    ret += ds3104_write( 0x181, 0xd0 );    
    ret += ds3104_write( 0x182, 0xa1 );
    ret += ds3104_write( 0x183, 0x4 );
    ret += ds3104_write( 0x184, 0x0 );  

    ret += ds3104_write( 0x180, 0xd );
    ret += ds3104_write( 0x181, 0xce );    
    ret += ds3104_write( 0x182, 0x03 );
    ret += ds3104_write( 0x183, 0x0 );
    ret += ds3104_write( 0x184, 0x0 );   

    ret += ds3104_write( 0x180, 0x9 );
    ret += ds3104_write( 0x181, 0xcd );    
    ret += ds3104_write( 0x182, 0x01 );
    ret += ds3104_write( 0x183, 0x4 );
    ret += ds3104_write( 0x184, 0x0 );    

    /*106*/
    ret += ds3104_write( 0x181, 0xe );    
    ret += ds3104_write( 0x182, 0x5f );
    ret += ds3104_write( 0x183, 0xa1 );
    ret += ds3104_write( 0x184, 0x0 );   

    ret += ds3104_write( 0x181, 0xd1 );    
    ret += ds3104_write( 0x182, 0x51 );
    ret += ds3104_write( 0x183, 0xa4 );
    ret += ds3104_write( 0x184, 0x0 );   

    ret += ds3104_write( 0x181, 0xd1 );    
    ret += ds3104_write( 0x182, 0x51 );
    ret += ds3104_write( 0x183, 0x1 );
    ret += ds3104_write( 0x184, 0x0 );   

    ret += ds3104_write( 0x181, 0xf0 );    
    ret += ds3104_write( 0x182, 0x11 );
    ret += ds3104_write( 0x183, 0x8 );
    ret += ds3104_write( 0x184, 0x0 );   
    
    /*122*/
    ret += ds3104_write( 0x181, 0xc1 );    
    ret += ds3104_write( 0x182, 0x1 );
    ret += ds3104_write( 0x183, 0x4 );
    ret += ds3104_write( 0x184, 0x0 );   

    /*126*/
    ret += ds3104_write( 0x181, 0xe0 );    
    ret += ds3104_write( 0x182, 0x2 );
    ret += ds3104_write( 0x183, 0x16 );
    ret += ds3104_write( 0x184, 0x0 );   

    /*130*/
    ret += ds3104_write( 0x181, 0x94 );    
    ret += ds3104_write( 0x182, 0xf );
    ret += ds3104_write( 0x183, 0x1 );
    ret += ds3104_write( 0x184, 0x0 );   

    /*134*/
    ret += ds3104_write( 0x181, 0x3 );    
    ret += ds3104_write( 0x182, 0x0 );
    ret += ds3104_write( 0x183, 0xb );
    ret += ds3104_write( 0x184, 0x0 );   

    /*138*/
    ret += ds3104_write( 0x181, 0x0 );    
    ret += ds3104_write( 0x182, 0xa0 );
    ret += ds3104_write( 0x183, 0x45 );
    ret += ds3104_write( 0x184, 0x0 );   

    /*142*/
    ret += ds3104_write( 0x181, 0xd1 );    
    ret += ds3104_write( 0x182, 0xa1 );
    ret += ds3104_write( 0x183, 0x44 );
    ret += ds3104_write( 0x184, 0x0 ); 

    /*146*/
    ret += ds3104_write( 0x181, 0xcd );    
    ret += ds3104_write( 0x182, 0xa1 );
    ret += ds3104_write( 0x183, 0x44 );
    ret += ds3104_write( 0x184, 0x0 ); 

    /*150*/
    ret += ds3104_write( 0x181, 0xae );    
    ret += ds3104_write( 0x182, 0x2 );
    ret += ds3104_write( 0x183, 0x16 );
    ret += ds3104_write( 0x184, 0x0 );  

    /*154*/
    ret += ds3104_write( 0x180, 0xd ); 
    ret += ds3104_write( 0x181, 0x35 );    
    ret += ds3104_write( 0x182, 0x7 );
    ret += ds3104_write( 0x183, 0x00 );
    ret += ds3104_write( 0x184, 0x0 );   

     /*159*/
    ret += ds3104_write( 0x180, 0x9 ); 
    ret += ds3104_write( 0x181, 0x0 );    
    ret += ds3104_write( 0x182, 0xff );
    ret += ds3104_write( 0x183, 0xa5 );
    ret += ds3104_write( 0x184, 0xe );   

    /*164*/
    ret += ds3104_write( 0x181, 0x0 );    
    ret += ds3104_write( 0x182, 0x7f );
    ret += ds3104_write( 0x183, 0xa9 );
    ret += ds3104_write( 0x184, 0x3 ); 

    /*168*/
    ret += ds3104_write( 0x181, 0x0 );    
    ret += ds3104_write( 0x182, 0xff );
    ret += ds3104_write( 0x183, 0x52 );
    ret += ds3104_write( 0x184, 0x7 ); 

    /*172*/
    ret += ds3104_write( 0x181, 0x0 );    
    ret += ds3104_write( 0x182, 0x3f );
    ret += ds3104_write( 0x183, 0x7e );
    ret += ds3104_write( 0x184, 0x4c ); 

    /*176*/
    ret += ds3104_write( 0x181, 0x0 );    
    ret += ds3104_write( 0x182, 0xf0 );
    ret += ds3104_write( 0x183, 0x2 );
    ret += ds3104_write( 0x184, 0x00 ); 

    /*180*/
    ret += ds3104_write( 0x181, 0x0 );    
    ret += ds3104_write( 0x182, 0xed );
    ret += ds3104_write( 0x183, 0x36 );
    ret += ds3104_write( 0x184, 0x00 ); 

    /*184*/
    ret += ds3104_write( 0x181, 0x0 );    
    ret += ds3104_write( 0x182, 0xb4 );
    ret += ds3104_write( 0x183, 0xdb );
    ret += ds3104_write( 0x184, 0x0 ); 

    /*188*/
    ret += ds3104_write( 0x181, 0x0 );    
    ret += ds3104_write( 0x182, 0x55 );
    ret += ds3104_write( 0x183, 0x55 );
    ret += ds3104_write( 0x184, 0x5 ); 

    /*192*/
    ret += ds3104_write( 0x181, 0x0 );    
    ret += ds3104_write( 0x182, 0x44 );
    ret += ds3104_write( 0x183, 0x44 );
    ret += ds3104_write( 0x184, 0x0 ); 

    /*196*/
    ret += ds3104_write( 0x181, 0x0 );    
    ret += ds3104_write( 0x182, 0xfe );
    ret += ds3104_write( 0x183, 0xff );
    ret += ds3104_write( 0x184, 0x3f ); 

    /*200*/
    ret += ds3104_write( 0x181, 0x0 );    
    ret += ds3104_write( 0x182, 0xc8 );
    ret += ds3104_write( 0x183, 0x8f );
    ret += ds3104_write( 0x184, 0x9 ); 

    /*204*/
    ret += ds3104_write( 0x181, 0x0 );    
    ret += ds3104_write( 0x182, 0x11 );
    ret += ds3104_write( 0x183, 0x00 );
    ret += ds3104_write( 0x184, 0x00 ); 

    /*208*/
    ret += ds3104_write( 0x180, 0x0 );    

    /*customer config register*/

    ret += ds3104_write( 0x22, 0xc1 );    
    ret += ds3104_write( 0x23, 0xc3 );
    ret += ds3104_write( 0x24, 0x3 );

    
    ret += ds3104_write( 0x27, 0x0 );
    ret += ds3104_write( 0x28, 0x0 );

    
    ret += ds3104_write( 0x4b, 0x0 );    
    ret += ds3104_write( 0x18, 0x0 );
    ret += ds3104_write( 0x19, 0x21 );        
    ret += ds3104_write( 0x1a, 0x3 );
    ret += ds3104_write( 0x1b, 0x40 );
    ret += ds3104_write( 0x1c, 0x5 );  

    ret += ds3104_write( 0x4b, 0x10 );    
    ret += ds3104_write( 0x18, 0x0 );
    ret += ds3104_write( 0x19, 0x0 );        
    ret += ds3104_write( 0x1a, 0x0 );
    ret += ds3104_write( 0x1b, 0x0 );
    ret += ds3104_write( 0x1c, 0x0 );

    ret += ds3104_write( 0x4b, 0x0 );    
    
    ret += ds3104_write( 0x65, 0x49 );   
    ret += ds3104_write( 0x60, 0x24 );
    
    ret += ds3104_write( 0x4f, 0x01 );
    ret += ds3104_write( 0x61, 0x6 );
    
    ret += ds3104_write( 0x4f, 0x01 );    
    ret += ds3104_write( 0x62, 0x0 );
    ret += ds3104_write( 0x63, 0xc0 );
    ret += ds3104_write( 0x7a, 0x0 );

    ret += ds3104_write( 0x38, 0x9f );   
    ret += ds3104_write( 0x39, 0x88 );  

    ret += ds3104_write( 0x3c, 0xff );   
    ret += ds3104_write( 0x3d, 0xa3 );  

    ret += ds3104_write( 0x41, 0xff );
    ret += ds3104_write( 0x42, 0x3f );
    return ret;
}

int xilinx_fpga_program_fn(int assert_program)
{
    u64 mask = 0;

    mask = 0x1<<XILINX_G24EU_PROGRAM;
	if(assert_program)
	{
		cvmx_write_csr(CVMX_GPIO_TX_CLR, mask);
	}
	else
	{		
        cvmx_write_csr(CVMX_GPIO_TX_SET, mask);
	}
	return 0;
}

int xilinx_fpga_init_fn( void)
{    
    u64 val;
    u64 mask;

    val = cvmx_read_csr(CVMX_GPIO_RX_DAT);
    mask = 0x1<<XILINX_G24EU_INIT;
    val = val ^ mask;
    if(val&mask)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int xilinx_fpga_clk_fn( int assert_clk)
{       
    u64 mask = 0;

    mask = 0x1<<XILINX_G24EU_CCLK;
	if(assert_clk)
	{
		cvmx_write_csr(CVMX_GPIO_TX_SET, mask);
	}
	else
	{
		cvmx_write_csr(CVMX_GPIO_TX_CLR, mask);
	}
	return 0;
}

int xilinx_fpga_data_fn( int assert_data)
{ 
    u64 mask = 0;

    mask = 0x1<<XILINX_G24EU_DIN;
	if(assert_data)
	{
		cvmx_write_csr(CVMX_GPIO_TX_SET, mask);
	}
	else
	{
		cvmx_write_csr(CVMX_GPIO_TX_CLR, mask);
	}
	return 0;
}

int xilinx_fpga_done_fn( void)
{
    u64 val;
    u64 mask;

    val = cvmx_read_csr(CVMX_GPIO_RX_DAT);
    mask = 0x1<<XILINX_G24EU_DONE;
    val = val ^ mask;
    if(val&mask)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int xilinx_fpga_gpio_init(void)
{
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(XILINX_G24EU_DIN), 0x1);
    
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(XILINX_G24EU_CCLK), 0x1);
    
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(XILINX_G24EU_DONE), 0x0);
    
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(XILINX_G24EU_INIT), 0x0);
    
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(XILINX_G24EU_PROGRAM), 0x1);
    
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(XILINX_G24EU_RST_IN), 0x1);
    return 0;
}

int xilinx_fpga_rst_fn(int assert_rst)
{
    u64 mask = 0;

    mask = 0x1<<XILINX_G24EU_RST_IN;
	if(assert_rst)
	{
		cvmx_write_csr(CVMX_GPIO_TX_CLR, mask);
	}
	else
	{		
        cvmx_write_csr(CVMX_GPIO_TX_SET, mask);
	}
	return 0;
}

static int __init xilinx_fpga_load(unsigned char *starting_address, unsigned int bsize)
{
    unsigned int bytecount=0;
    unsigned char val,i;
	
    xilinx_fpga_gpio_init();
    printk(KERN_NOTICE"loading fpga image\n");
    /* Assert Program */
    xilinx_fpga_program_fn(TRUE);
    udelay(100);		
    if (!xilinx_fpga_init_fn())
    {
        /*add for bug 7538 start*/
        xilinx_fpga_program_fn(FALSE);
        /*add for bug 7538 end*/
        printk(KERN_NOTICE"init is not asserted\n");
	 return -1;
     }
     udelay(100);		
     /* Deassert Program */
     xilinx_fpga_program_fn(FALSE);
     udelay(2000);		
     if (xilinx_fpga_init_fn())
     {
         printk(KERN_NOTICE"init is not be released\n");
	  return -1;
      }

      /* Load the data */
      while (bytecount < bsize) 	
      {
	    val=0;
	    val = *(starting_address+bytecount);
	    bytecount++;
	    i = 8;
	    do 
	    {
	         /* Deassert the clock */
		  xilinx_fpga_clk_fn (FALSE);
		  /* Write data */
		  xilinx_fpga_data_fn (val & 0x80);
		  /* Assert the clock */
		  xilinx_fpga_clk_fn (TRUE);
		  val <<= 1;
		  i --;
	    } 
	    while (i > 0);
	}	
	udelay(200);
	if (!xilinx_fpga_done_fn())
	{
	    printk(KERN_NOTICE"done is still deasserted\n");
	    return -1;
	}	
	printk (KERN_NOTICE"Done.\n");
	//udelay(1000);
	udelay(100);
    xilinx_fpga_rst_fn(1);
    udelay(10);
    xilinx_fpga_rst_fn(0);
	printk (KERN_NOTICE"Reset FPGA.\n");

    return 0;
}

int lattice_fpga_program_fn(int assert_program)
{
    u64 mask = 0;

    mask = 0x1<<LATTICE_PROGRAM;
	if(assert_program)
	{
		cvmx_write_csr(CVMX_GPIO_TX_CLR, mask);
	}
	else
	{		
        cvmx_write_csr(CVMX_GPIO_TX_SET, mask);
	}
	return 0;
}
/*when program drive low, init output low to start init; when program driver high, and finish init, init output high.*/
int lattice_fpga_init_fn( void)
{    
    u64 val;
    u64 mask;

    val = cvmx_read_csr(CVMX_GPIO_RX_DAT);
    mask = 0x1<<LATTICE_INIT;
    val = val ^ mask;
    if(val&mask)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int lattice_fpga_clk_fn( int assert_clk)
{       
    u64 mask = 0;

    mask = 0x1<<LATTICE_CCLK;
	if(assert_clk)
	{
		cvmx_write_csr(CVMX_GPIO_TX_SET, mask);
	}
	else
	{
		cvmx_write_csr(CVMX_GPIO_TX_CLR, mask);
	}
	return 0;
}

int lattice_fpga_data_fn( int assert_data)
{ 
    u64 mask = 0;

    mask = 0x1<<LATTICE_DIN;
	if(assert_data)
	{
		cvmx_write_csr(CVMX_GPIO_TX_SET, mask);
	}
	else
	{
		cvmx_write_csr(CVMX_GPIO_TX_CLR, mask);
	}
	return 0;
}

/*done == 1, mean have finished, return 1; done == 0, haven't finished, return 0*/
int lattice_fpga_done_fn( void)
{
    u64 val;
    u64 mask;

    val = cvmx_read_csr(CVMX_GPIO_RX_DAT);
    mask = 0x1<<LATTICE_DONE;
    val = val ^ mask;
    if(val&mask)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int lattice_fpga_gpio_init(void)
{    
    /*For cavium CPU, GPIO lower 16 should use CVMX_GPIO_BIT_CFGX
                    GPIO upper 16 should use CVMX_GPIO_XBIT_CFGX; */
    cvmx_write_csr(CVMX_GPIO_XBIT_CFGX(LATTICE_DIN), 0x1);

    cvmx_write_csr(CVMX_GPIO_XBIT_CFGX(LATTICE_CCLK), 0x1);
    
    cvmx_write_csr(CVMX_GPIO_XBIT_CFGX(LATTICE_DONE), 0x0);
    
    cvmx_write_csr(CVMX_GPIO_XBIT_CFGX(LATTICE_INIT), 0x0);
    
    cvmx_write_csr(CVMX_GPIO_XBIT_CFGX(LATTICE_PROGRAM), 0x1);
    
    cvmx_write_csr(CVMX_GPIO_XBIT_CFGX(LATTICE_RST_IN), 0x1);
   
    return 0;
}

int lattice_fpga_rst_fn(int assert_rst)
{
    u64 mask = 0;

    mask = 0x1<<LATTICE_RST_IN;
	if(assert_rst)
	{
		cvmx_write_csr(CVMX_GPIO_TX_CLR, mask);
	}
	else
	{		
        cvmx_write_csr(CVMX_GPIO_TX_SET, mask);
	}
	return 0;
}

static int __init lattice_fpga_load(unsigned char *starting_address, unsigned int bsize)
{
	unsigned int bytecount=0;
	unsigned char *p_data, val;
    int decompress_len;
    /* for gzip decompress */
    z_stream s;
    int r, i, flags;

    /* ship gzip header */
    i =10;
    flags = starting_address[3]; 
    if(starting_address[2] != Z_DEFLATED || (flags & RESERVED) != 0) {
        printk(KERN_ERR"fpga bad gzipped data\n\r");
        return 1;
    }
    if ((flags & EXTRA_FIELD) != 0)
        i = 12 + starting_address[10] + (starting_address[11] << 8);
    if ((flags & ORIG_NAME) != 0)
        while(starting_address[i++] != 0)
            ;
    if ((flags & COMMENT) != 0)
        while (starting_address[i++] != 0)
            ;
    if ((flags & HEAD_CRC) != 0)
        i += 2;
    if (i >= bsize) {
        printk(KERN_ERR"fpga gunzip: ran out of data in header\n\r");
        return 1;
    }

    if (zlib_inflate_workspacesize() > sizeof(scratch)) {
        printk(KERN_ERR"fpga gunzip needs more mem\n");
        return 1;
    }
    memset(&s, 0, sizeof(s));
    s.workspace = scratch;
    r = zlib_inflateInit2(&s, -MAX_WBITS);
    if (r != Z_OK) {
        printk(KERN_ERR"inflateInit2 returned %d\n\r", r);
        return 1;
    }
    s.next_in = starting_address + i;
    s.avail_in = bsize - i;
    s.next_out = fpga_loadbuf;
    s.avail_out = 524288;
    r = zlib_inflate(&s, Z_FULL_FLUSH);
    if (r != Z_OK && r != Z_STREAM_END) {
        printk(KERN_ERR"inflate returned %d msg: %s\n\r", r, s.msg);
        return 1;
    }
    decompress_len = s.next_out - (unsigned char *)fpga_loadbuf;
    zlib_inflateEnd(&s);
    printk(KERN_NOTICE"decompress length of the fpga image %d\n", decompress_len);
    
	lattice_fpga_gpio_init();
	printk(KERN_NOTICE"loading fpga image\n");
	/* Assert Program */
	lattice_fpga_program_fn(TRUE);
	udelay(500);		
	if (!lattice_fpga_init_fn())
	{
		printk(KERN_NOTICE"init is not asserted\n");
		return -1;
	}
	udelay(500);		
	/* Deassert Program */
	lattice_fpga_program_fn(FALSE);
	udelay(500);		
	if (lattice_fpga_init_fn())
	{
		printk(KERN_NOTICE"init is not be released\n");
		return -1;
	}
	p_data = fpga_loadbuf;
	/* Load the data */
	while (bytecount < decompress_len) 
	{
		val=0;
		val = *(p_data+bytecount);
		bytecount++;
		i = 8;
		do 
		{
			/* Deassert the clock */
			lattice_fpga_clk_fn (FALSE);
			/* Write data */
			lattice_fpga_data_fn (val & 0x80);
			/* Assert the clock */
			lattice_fpga_clk_fn (TRUE);
			val <<= 1;
			i --;
		} 
		while (i > 0);
	}	

	udelay(1000);
	if (!lattice_fpga_done_fn())
	{
		printk(KERN_NOTICE"done is still deasserted\n");
		return -1;
	}	
	printk (KERN_NOTICE"Done.\n");
	udelay(1000);
    lattice_fpga_rst_fn(1);
    udelay(10);
    lattice_fpga_rst_fn(0);
	printk (KERN_NOTICE"Reset FPGA.\n");

    return 0;
}

extern int octeon_ctc_flash_odd_usage;

#define OCTEON_CTC_SWAP16(x) \
    ((short)( \
    (((short)(x) & (short) 0x00ffU) << 8) | \
    (((short)(x) & (short) 0xff00U) >> 8) ))


int e330_48t_gpio_init(void)
{
    /* Configure GPIO 4-6 interrupt source, low level sensitive */
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(4), 0x6);
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(5), 0x6);
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(6), 0x6);
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(7), 0x6);
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(8), 0x6);
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(9), 0x6);

    cvmx_write_csr(CVMX_GPIO_BOOT_ENA, 0xc00);
    
    return 0;
}

int normal_gpio_init(void)
{
    /* Configure GPIO 4-6 interrupt source, low level sensitive */
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(4), 0x6);
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(5), 0x6);
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(6), 0x6);
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(7), 0x6);
    /* For B330, POE irq uses GPIO 8, in other boards, GPIO 8 is reserved. */
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(8), 0x6);
    /* Cfg GPIO 17 to High and GPIO 18 to Low for upgrade EPLD control*/
    cvmx_write_csr(CVMX_GPIO_XBIT_CFGX(17), 0x1);
    cvmx_write_csr(CVMX_GPIO_TX_SET, 1<<17);
    cvmx_write_csr(CVMX_GPIO_XBIT_CFGX(18), 0x1);
    cvmx_write_csr(CVMX_GPIO_TX_CLR, 1<<18);
    return 0;
}

int b330_48t_gpio_init(void)
{
    /* Configure GPIO 4-6 interrupt source, low level sensitive */
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(4), 0x6);
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(5), 0x6);
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(6), 0x6);
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(7), 0x6);
    /* For B330, POE irq uses GPIO 8, in other boards, GPIO 8 is reserved. */
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(8), 0x6);

    cvmx_write_csr(CVMX_GPIO_BOOT_ENA, 0xe00);   
    return 0;
}

void board_info_dump(void)
{    
#if 1
    printk(KERN_INFO "BoardInfo S:T:V:F:D=[%d:%d:%d:%d:%d]\n",ctc_bd_info.board_series,
    ctc_bd_info.board_type, ctc_bd_info.board_ver, ctc_bd_info.flash_type, ctc_bd_info.ddr_type);
#else
    printk(KERN_INFO "board series: %d!\n",ctc_bd_info.board_series); 
    printk(KERN_INFO "board type: %d!\n",ctc_bd_info.board_type); 
    printk(KERN_INFO "board version: %d!\n",ctc_bd_info.board_ver); 
    printk(KERN_INFO "flash type: %d!\n",ctc_bd_info.flash_type); 
    printk(KERN_INFO "ddr type: %d!\n",ctc_bd_info.ddr_type); 
    printk(KERN_INFO "chip number: %d!\n",ctc_bd_info.asic_chip_num);
    printk(KERN_INFO "chip 0 normal irq: %d!\n",ctc_bd_info.asic0_normal_irq);
    printk(KERN_INFO "chip 0 fatal irq: %d!\n",ctc_bd_info.asic0_fatal_irq);
    printk(KERN_INFO "chip 1 normal irq: %d!\n",ctc_bd_info.asic1_normal_irq);
    printk(KERN_INFO "chip 1 fatal irq: %d!\n",ctc_bd_info.asic1_fatal_irq);
    printk(KERN_INFO "normal irq 0 number: %d!\n",ctc_bd_info.normal_irq_0);
    printk(KERN_INFO "normal irq 1 number: %d!\n",ctc_bd_info.normal_irq_1);
    printk(KERN_INFO "normal interrupt 0 offset: %d!\n",ctc_bd_info.normal_int0_offset);
    printk(KERN_INFO "normal interrupt 0 mask offset: %d!\n",ctc_bd_info.normal_int0_mask_offset);
    printk(KERN_INFO "normal interrupt 1 offset: %d!\n",ctc_bd_info.normal_int1_offset);
    printk(KERN_INFO "normal interrupt 1 mask offset: %d!\n",ctc_bd_info.normal_int1_mask_offset);
    printk(KERN_INFO "foam support: %d!\n",ctc_bd_info.foam_support);
    printk(KERN_INFO "foam fpga normal irq number: %d!\n",ctc_bd_info.foam_normal_irq);
    printk(KERN_INFO "foam fpga fatal irq number: %d!\n",ctc_bd_info.foam_fatal_irq);
    printk(KERN_INFO "ctrlfpga support: %d!\n",ctc_bd_info.ctrlfpga_support);
    printk(KERN_INFO "watch dog support: %d!\n",ctc_bd_info.watchdog_support);
    printk(KERN_INFO "watch dog en gpio: %d!\n",ctc_bd_info.watchdog_en_gpio);
    printk(KERN_INFO "watch dog feed gpio: %d!\n",ctc_bd_info.watchdog_feed_gpio);
    printk(KERN_INFO "normal irq count: %d!\n",ctc_bd_info.normal_irq_count);
#endif
}

static int octeon_ctc_bd_info_detect(ctc_board_t *bd_info)
{
    void __iomem *ctc_board_cs6;
    void __iomem *ctc_board_cs7;
    u16 value = 0;

    /* Get board series and type. support 256M flash, bug21815 */
    if(ctc_bd_info.bootbus_updated)
    {
        ctc_board_cs6 = ioremap(GLB_BOARD_SERIES_NEW_BASE, CTC_SYS_MAP_SIZE);    
    }
    else
    {
        ctc_board_cs6 = ioremap(GLB_BOARD_SERIES_BASE, CTC_SYS_MAP_SIZE);    
    }
    if (ctc_board_cs6 == NULL)    
    {   
        printk(KERN_ERR "Can't map board series space!\n");
        return -1;    
    }

    value = ioread16(ctc_board_cs6);
    value = OCTEON_CTC_SWAP16(value);

    bd_info->board_series = value&0xff;
    bd_info->board_type = (value>>0x8)&0xff;

    iounmap(ctc_board_cs6);
    
    /* Get hardware version,flash type and dram type */
    if(ctc_bd_info.bootbus_updated)
    {
        ctc_board_cs7 = ioremap(GLB_BOARD_VER_NEW_BASE, CTC_SYS_MAP_SIZE);    
    }
    else
    {
        ctc_board_cs7 = ioremap(GLB_BOARD_VER_BASE, CTC_SYS_MAP_SIZE);    
    }
    if (ctc_board_cs7 == NULL)    
    {   
        printk(KERN_ERR "Can't map hw version space!\n");
        return -1;    
    }

    value = ioread16(ctc_board_cs7);
    value = OCTEON_CTC_SWAP16(value);

    bd_info->board_ver = (value >> 4) & 0xf;
    bd_info->flash_type = (value >> 8) & 0xf;
    bd_info->ddr_type = (value >> 12) & 0xf;

    iounmap(ctc_board_cs7);

    return 0;
    
}

static int e330_48t_version1_fixup(void)
{
    u8 led_ctl = 0;
    u8 daught_card;
    
    ctc_bd_info.asic_chip_num= 1;
    
    ctc_bd_info.asic0_normal_irq = ASIC_NORMAL_IRQ;
    ctc_bd_info.asic0_fatal_irq = ASIC_FATAL_IRQ;

    daught_card = ioread8(ctc_epld_logic_addr + CTC_DAUGHTER_CARD_TYPE);
    /*fix bug 15050, 15185, 15444. jqiu 2011-06-17, 2011-07-13*/
    /*if extend card is present and card is not 1G card,EPLD spec: 0 mean present, 1 mean absent*/
    if(((daught_card & 0x1)==0) &&(((daught_card & 0x30)>>4)!=3))
    {
        ctc_bd_info.normal_irq_count = 2;
    }
    else
    {
        ctc_bd_info.normal_irq_count = 1;
    }
    ctc_bd_info.normal_irq_0 = 48; /*gpio 8*/
    ctc_bd_info.normal_irq_1 = 49; /*gpio 9*/

    ctc_bd_info.normal_int0_offset = CTC_NORMAL_INT0_OFFSET;
    ctc_bd_info.normal_int1_offset = CTC_NORMAL_INT1_OFFSET;
    ctc_bd_info.normal_int0_mask_offset = CTC_NORMAL_INT0_MASK_OFFSET;
    ctc_bd_info.normal_int1_mask_offset = CTC_NORMAL_INT1_MASK_OFFSET;
    
    ctc_bd_info.watchdog_support = 0;
    ctc_bd_info.watchdog_en_gpio = 14;
    ctc_bd_info.watchdog_feed_gpio = 13;
    
    /* hardware limition,It use two 32M bytes flash, but each flash can only use 16M bytes 
       Only the E330 48T first hardware version should set this flag
    */
    octeon_ctc_flash_odd_usage = 1;
    
    /* system led, amber blinkingSlow (period is 1 second) */
   led_ctl = ioread8(ctc_epld_logic_addr + CTC_EPLD_LED_OFFSET);
   led_ctl &= ~0xf;
   led_ctl |= 0x9;
   iowrite8(led_ctl, ctc_epld_logic_addr + CTC_EPLD_LED_OFFSET);

   e330_48t_gpio_init();

   return 0;

}
/*used for cavium cpu based e330-48T and E330-48S*/
static int e330_48_normal_board_fixup(void)
{
    u8 led_ctl = 0;
    u8 daught_card, card_ver, card_swt;

    ctc_bd_info.asic_chip_num= 1;
    
    ctc_bd_info.asic0_normal_irq = ASIC_NORMAL_IRQ;
    ctc_bd_info.asic0_fatal_irq = ASIC_FATAL_IRQ;    
        
    daught_card = ioread8(ctc_epld_logic_addr + CTC_DAUGHTER_CARD_TYPE);
    /* Bug19028. For V4.0 board, and daught card is new version, must cfg switch first, then read daught-card type*/
    /* daughter card is present and mother board is V4.0, need check card version*/
    if(((daught_card & 0x1)==0)&&(ctc_bd_info.board_ver >= 0x2))
    {
        card_ver = ioread8(ctc_epld_logic_addr + CTC_DAUGHTER_CARD_VER);
        /* daughter card version is 2*/
        if((card_ver&0xc0)==0x80)
        {
            /*then cfg switch flag*/
            card_swt = ioread8(ctc_epld_logic_addr + CTC_DAUGHTER_CARD_SWT);
            card_swt = card_swt | 0x10;
            iowrite8(card_swt, ctc_epld_logic_addr + CTC_DAUGHTER_CARD_SWT);
            daught_card = ioread8(ctc_epld_logic_addr + CTC_DAUGHTER_CARD_TYPE);
        }
    }
    /*fix bug 15050, 15185, 15444. jqiu 2011-06-17, 2011-07-13*/
    /*if extend card is present and card is not 1G card. EPLD spec: 0 mean present, 1 mean absent*/
    if(((daught_card & 0x1)==0) &&(((daught_card & 0x30)>>4)!=3))
    {
        ctc_bd_info.normal_irq_count = 2;
    }
    else
    {
        ctc_bd_info.normal_irq_count = 1;
    }
    ctc_bd_info.normal_irq_0 = CTC_HW_NORMAL_IRQ_0;
    ctc_bd_info.normal_irq_1 = CTC_HW_NORMAL_IRQ_1;

    ctc_bd_info.normal_int0_offset = CTC_NORMAL_INT0_OFFSET;
    ctc_bd_info.normal_int1_offset = CTC_NORMAL_INT1_OFFSET;
    ctc_bd_info.normal_int0_mask_offset = CTC_NORMAL_INT0_MASK_OFFSET;
    ctc_bd_info.normal_int1_mask_offset = CTC_NORMAL_INT1_MASK_OFFSET;
    
    ctc_bd_info.watchdog_support = 1;
    ctc_bd_info.watchdog_en_gpio = WATCHDOG_EN_GPIO;
    ctc_bd_info.watchdog_feed_gpio = WATCHDOG_FEED_GPIO;
    
    /* system led, amber blinkingSlow (period is 1 second) */
    led_ctl = ioread8(ctc_epld_logic_addr + CTC_EPLD_LED_OFFSET);
    led_ctl &= ~0xf;
    led_ctl |= 0x9;
    iowrite8(led_ctl, ctc_epld_logic_addr + 0x5);

    normal_gpio_init();
    return 0;
}

static int 
b330_48t_fixup(void)
{
    u8 led_ctl = 0;
    u8 poe_present;
    u8 epld_val = 0xff;
    u8 daught_card;

    ctc_bd_info.asic_chip_num= 1;
    
    ctc_bd_info.asic0_normal_irq = ASIC_NORMAL_IRQ;
    ctc_bd_info.asic0_fatal_irq = ASIC_FATAL_IRQ;    

    daught_card = ioread8(ctc_epld_logic_addr + CTC_DAUGHTER_CARD_TYPE);

    /* For B330, if extend card is present, card can't be 1G card. EPLD spec: 0 mean present, 1 mean absent */
    if((daught_card & 0x1)==0)
    {
        ctc_bd_info.normal_irq_count = 2;
    }
    else
    {
        ctc_bd_info.normal_irq_count = 1;
    }
    ctc_bd_info.normal_irq_0 = CTC_HW_NORMAL_IRQ_0;
    ctc_bd_info.normal_irq_1 = CTC_HW_NORMAL_IRQ_1;

    ctc_bd_info.normal_int0_offset = CTC_NORMAL_INT0_OFFSET;
    ctc_bd_info.normal_int1_offset = CTC_NORMAL_INT1_OFFSET;
    ctc_bd_info.normal_int0_mask_offset = CTC_NORMAL_INT0_MASK_OFFSET;
    ctc_bd_info.normal_int1_mask_offset = CTC_NORMAL_INT1_MASK_OFFSET;
    
    ctc_bd_info.watchdog_support = 1;
    ctc_bd_info.watchdog_en_gpio = WATCHDOG_EN_GPIO;
    ctc_bd_info.watchdog_feed_gpio = WATCHDOG_FEED_GPIO;

    /* B330 add POE feature */
    epld_val = ioread8(ctc_epld_logic_addr + CTC_EPLD_POE_PRESENT_OFFSET);
    poe_present = epld_val & 0x1;
    if(poe_present == 0)
    {
        ctc_bd_info.poe_support = 0;
        ctc_bd_info.poe_irq = CTC_HW_POE_IRQ;
        ctc_bd_info.poe_irq_offset = CTC_POE_INT_OFFSET;
        ctc_bd_info.poe_irq_mask_offset = CTC_POE_INT_MASK_OFFSET;
    }
    else
    {
        ctc_bd_info.poe_support = 0;
    }

    /* system led, amber blinkingSlow (period is 1 second) */
    led_ctl = ioread8(ctc_epld_logic_addr + CTC_EPLD_LED_OFFSET);
    led_ctl &= ~0xf;
    led_ctl |= 0x9;
    iowrite8(led_ctl, ctc_epld_logic_addr + CTC_EPLD_LED_OFFSET);

    b330_48t_gpio_init();
    return 0;
}

static int humber_demo_fixup(void)
{   
    ctc_bd_info.asic_chip_num= 1;
    
    ctc_bd_info.ctrlfpga_support = 1;

    ctc_bd_info.asic_chip_num= 1;
    ctc_bd_info.asic0_normal_irq = HUMBER_DEMO_ASIC_NORMAL_IRQ;
    ctc_bd_info.asic0_fatal_irq = HUMBER_DEMO_ASIC_FATAL_IRQ;
    ctc_bd_info.normal_irq_count = 1;    
    ctc_bd_info.normal_irq_0 = HUMBER_DEMO_CTC_HW_NORMAL_IRQ_0;
    ctc_bd_info.normal_int0_offset = CTC_CTLFPGA_NORMAL_INT_OFFSET;
    ctc_bd_info.normal_int0_mask_offset = CTC_CTLFPGA_NORMAL_INT_MASK_OFFSET;

    lattice_fpga_load(&humber_demo_lattice_bin_start, 
        (&humber_demo_lattice_bin_end - &humber_demo_lattice_bin_start));
    normal_gpio_init();
    return 0;
}

static int seoul_g24eu_fixup(void)
{
    /* For seoul g24eu, only can get 8 bit board series for hardware limition */
    
    int ret = 0;
    u8 value = 0;
    u32 temp1 = 0;
    u32 temp2 = 0;

    ctc_bd_info.asic_chip_num= 1;
    
    ctc_bd_info.asic0_normal_irq = ASIC_NORMAL_IRQ;
    ctc_bd_info.asic0_fatal_irq = ASIC_FATAL_IRQ;
    
    ctc_bd_info.foam_support= 1;
    ctc_bd_info.foam_normal_irq = SEOUL_FOAM_NORMAL_IRQ;

    ctc_bd_info.normal_irq_count = 2;    
    ctc_bd_info.normal_irq_0 = SEOUL_CTC_HW_NORMAL_IRQ_0;
    ctc_bd_info.normal_irq_1 = SEOUL_CTC_HW_NORMAL_IRQ_0;

    ctc_bd_info.normal_int0_offset = CTC_NORMAL_INT0_OFFSET;
    ctc_bd_info.normal_int1_offset = CTC_NORMAL_INT1_OFFSET;
    ctc_bd_info.normal_int0_mask_offset = CTC_NORMAL_INT0_MASK_OFFSET;
    ctc_bd_info.normal_int1_mask_offset = CTC_NORMAL_INT1_MASK_OFFSET;
    
    /*interrupt gpio pin init*/
    normal_gpio_init();
    
    /*modified by jcao for bug 14885*/
    value = ioread8(ctc_epld_logic_addr + SEOUL_G24EU_RST_OFFSET); 
    /* reset oam fpga */
    value = (value & 0x3f);
    iowrite8(value, ctc_epld_logic_addr + SEOUL_G24EU_RST_OFFSET); 
    
     /*download OAM fpga */
    iowrite8(0x0, ctc_epld_logic_addr + EPLD_GPIO_MODE_SWITCH) ; 
    spi_gpio_delay(); 
    iowrite8(0x1, ctc_epld_logic_addr + EPLD_GPIO_ENABLE_REG) ; 
    iowrite8(0x0, ctc_epld_logic_addr + EPLD_GPIO_ENABLE_REG) ;  
    xilinx_fpga_load(&seoul_g24eu_xilinx_bin_start, 
                (&seoul_g24eu_xilinx_bin_end - &seoul_g24eu_xilinx_bin_start));
    
    /*release ad9517 &ds3104*/ 
    iowrite8(0x28, ctc_epld_logic_addr + 0xe); 
    
    /*switch to ds3104 */
    iowrite8(0x2, ctc_epld_logic_addr + EPLD_GPIO_MODE_SWITCH) ;  
    spi_gpio_delay(); 
    iowrite8(0x1, ctc_epld_logic_addr + EPLD_GPIO_ENABLE_REG) ; 
    iowrite8(0x0, ctc_epld_logic_addr + EPLD_GPIO_ENABLE_REG) ;  
    ret = ds3104_config();
    if (ret)
    {
        printk(KERN_ERR "config ds3104 fail!\n");
		return -1;
    }
    ds3104_read(0,&temp1);
    ds3104_read(1,&temp2);
    printk(KERN_INFO "config ds3104 successful(0x%x:0x%x)!\n",temp1,temp2); 
    
    /*switch to ad9517 */                       
    iowrite8(0x3, ctc_epld_logic_addr + EPLD_GPIO_MODE_SWITCH) ; 
    spi_gpio_delay(); 
    iowrite8(0x1, ctc_epld_logic_addr + EPLD_GPIO_ENABLE_REG) ; 
    iowrite8(0x0, ctc_epld_logic_addr + EPLD_GPIO_ENABLE_REG) ;  
    ret = ad9517_dev_init(E_AD9517_CLOCK_TYPE_5);
    if (ret)
    {
        printk(KERN_ERR "config ad9517 fail!\n");
		return -1;
    }
    ad9517_read(0,&temp1);
    printk(KERN_INFO "config ad9517 successful(0x%x)!\n",temp1); 

    return 0;
}

static int normal_board_fixup(void)
{
    u8 led_ctl = 0;

    ctc_bd_info.asic_chip_num= 1;
    
    ctc_bd_info.asic0_normal_irq = ASIC_NORMAL_IRQ;
    ctc_bd_info.asic0_fatal_irq = ASIC_FATAL_IRQ;    

    ctc_bd_info.normal_irq_count = 2;
    ctc_bd_info.normal_irq_0 = CTC_HW_NORMAL_IRQ_0;
    ctc_bd_info.normal_irq_1 = CTC_HW_NORMAL_IRQ_1;

    ctc_bd_info.normal_int0_offset = CTC_NORMAL_INT0_OFFSET;
    ctc_bd_info.normal_int1_offset = CTC_NORMAL_INT1_OFFSET;
    ctc_bd_info.normal_int0_mask_offset = CTC_NORMAL_INT0_MASK_OFFSET;
    ctc_bd_info.normal_int1_mask_offset = CTC_NORMAL_INT1_MASK_OFFSET;
    
    ctc_bd_info.watchdog_support = 1;
    ctc_bd_info.watchdog_en_gpio = WATCHDOG_EN_GPIO;
    ctc_bd_info.watchdog_feed_gpio = WATCHDOG_FEED_GPIO;
    
    /* system led, amber blinkingSlow (period is 1 second) */
    led_ctl = ioread8(ctc_epld_logic_addr + CTC_EPLD_LED_OFFSET);
    led_ctl &= ~0xf;
    led_ctl |= 0x9;
    iowrite8(led_ctl, ctc_epld_logic_addr + 0x5);

    normal_gpio_init();
    return 0;
}

void asic_pci_init(u8 chip_id)
{       
    unsigned int val;
    val = ioread8(ctc_epld_logic_addr + ASIC_RESET_OFFSET +chip_id); 
#if 0        
    /*release dll*/
    val = (val & 0xe8);    
    iowrite8(val, ctc_epld_logic_addr + ASIC_RESET_OFFSET+chip_id); 
    udelay(10);
 
    /*release jtag*/
    val = (val | 0x1);
    iowrite8(val, ctc_epld_logic_addr + ASIC_RESET_OFFSET+chip_id); 
    udelay(1000);
    /*reset jtag*/
    val = (val & 0xfe);
    iowrite8(val, ctc_epld_logic_addr + ASIC_RESET_OFFSET+chip_id); 
    udelay(10000);
#else
    /*config every reset pin to default value.*/
    val = ((val & 0xe0) | 0x8);
    iowrite8(val, ctc_epld_logic_addr + ASIC_RESET_OFFSET+chip_id); 
#endif

    /*release global*/
    val = (val | 0x10);
    iowrite8(val, ctc_epld_logic_addr + ASIC_RESET_OFFSET+chip_id); 
    udelay(1000);
    
    /*reset glob*/
    val = (val & 0xef);
    iowrite8(val, ctc_epld_logic_addr + ASIC_RESET_OFFSET+chip_id); 
    udelay(10000);

    /*release pci*/
    val = (val | 0x2);
    iowrite8(val, ctc_epld_logic_addr + ASIC_RESET_OFFSET+chip_id); 
    udelay(1000);
    return ;
}
/*support 256M flash, bug21815 */
void ctc_bootbus_flag_updated(void)
{
    if(NULL != strstr(arcs_cmdline, "UPD_BUS"))
    {
        ctc_bd_info.bootbus_updated = 1;
    }
}

int ctc_boards_fixup(void)
{            
    int ret = 0;
    u8 chip_id = 0;
    memset(&ctc_bd_info, 0, sizeof(ctc_board_t));

    ctc_bootbus_flag_updated();
    ret = octeon_ctc_bd_info_detect(&ctc_bd_info);
    if (ret == -1)    
    {   
        printk(KERN_ERR "board info detect fail!\n");
        return -1;    
    }
        
    /*Fix bug 15622. support cavium CPU card on humberDemo board.*/
    /*This is special for humber demo board. board type 0xfc is just use for humberdemo */
    /* For humber demo, only can get 8 bit board type for hardware limition */
    if(BOARD_DEMO_HUMBER_MAX == ctc_bd_info.board_type)
    {
        ctc_bd_info.board_series = GLB_BOARD_SERIES_HUMBER_DEMO;
        ctc_bd_info.board_type = BOARD_DEMO_HUMBER;
        ctc_bd_info.board_ver = BOARD_VERSION_2;
    }
    /*HumberDemo */
    if(ctc_bd_info.board_series != GLB_BOARD_SERIES_HUMBER_DEMO)
    {
        /* Get epld logic address */
        if(ctc_bd_info.bootbus_updated)
        {
            ctc_bd_info.epld_logic_addr = ioremap(GLB_EPLD_NEW_BASE, CTC_SYS_EPLD_SIZE);
        }
        else
        {
            ctc_bd_info.epld_logic_addr = ioremap(GLB_EPLD_BASE, CTC_SYS_EPLD_SIZE);    
        }
        if (ctc_bd_info.epld_logic_addr == NULL)
        {   
            printk(KERN_ERR "Can't map EPLD space!\n");
            return -1;    
        }
        ctc_epld_logic_addr = ctc_bd_info.epld_logic_addr;
    }
    /*for some special board, we do some special action*/
    switch(ctc_bd_info.board_series)
    {
        case GLB_BOARD_SERIES_SEOUL:
            ret = seoul_g24eu_fixup(); 
            break;
        case GLB_BOARD_SERIES_HUMBER_DEMO:
            ret = humber_demo_fixup();
            break;
        case GLB_BOARD_SERIES_E330:
            if((BOARD_E330_48T == ctc_bd_info.board_type)&&(BOARD_VERSION_0 == ctc_bd_info.board_ver))
                ret = e330_48t_version1_fixup();
            else
                ret = e330_48_normal_board_fixup();
            break;
        case GLB_BOARD_SERIES_B330:
            ret = b330_48t_fixup();        
            break;
        default:
            ret = normal_board_fixup();
            break;
    }
    if (ret == -1)    
    {   
        printk(KERN_ERR "Series %d board %d fixup fail!\n",ctc_bd_info.board_series, ctc_bd_info.board_type);
        return -1;    
    }

    /*asic chip pci init*/    
    if(GLB_BOARD_SERIES_HUMBER_DEMO != ctc_bd_info.board_series)
    {   
        /*for humber demo board, asic pci init did by hardware and so we do nothing*/
        for(chip_id = 0;chip_id < ctc_bd_info.asic_chip_num;chip_id++)
        {
            asic_pci_init(chip_id);    
        }                            
    }
    
    board_info_dump();
    
    return 0;
}

/*For cpu phy, address 0 is broadcast addr, So shouldn't use this addr, all boards use phy addr should start from 1;
  But E330-48T use address 0, and it just has 1 phy, so address 0 can work well.This func is used for process phy.*/
int ctc_board_is_e330_48T(void)
{
    if((GLB_BOARD_SERIES_E330 == ctc_bd_info.board_series)&&
          ((BOARD_E330_48T == ctc_bd_info.board_type)||(BOARD_E330_24T == ctc_bd_info.board_type)))
        return 1;
    else
        return 0;
}

