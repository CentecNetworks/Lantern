/*
 * MPC8272 ADS board support
 *
 * Copyright 2007 Freescale Semiconductor, Inc.
 * Author: Scott Wood <scottwood@freescale.com>
 *
 * Based on code by Vitaly Bordug <vbordug@ru.mvista.com>
 * Copyright (c) 2006 MontaVista Software, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/fsl_devices.h>
#include <linux/of_platform.h>
#include <linux/io.h>

#include <asm/cpm2.h>
#include <asm/udbg.h>
#include <asm/machdep.h>
#include <asm/time.h>

#include <platforms/82xx/pq2.h>

#include <sysdev/fsl_soc.h>
#include <sysdev/cpm2_pic.h>
#ifdef CONFIG_CTC8247
#include <linux/zlib.h>
#include "ctc8247.h"
#endif
#include <asm/delay.h>

#include "pq2.h"

#ifdef CONFIG_CTC8247

static char __initdata scratch[46912];  /* scratch space for gunzip, from zlib_inflate_workspacesize() */
static char __initdata fpga_loadbuf[524288]; /* the uncompressed fpga image store buf 512K*/
extern unsigned char humber_demo_lattice_bin_start;
extern unsigned char humber_demo_lattice_bin_end;
extern unsigned char seoul_g24eu_xilinx_bin_start;
extern unsigned char seoul_g24eu_xilinx_bin_end;


//volatile u32* epld_logic_addr;
void __iomem *epld_logic_addr;
#define SYS_EPLD_SIZE   0x00001000 /* 4K bytes */

/*from seoul G24EU epld spec*/
#define EPLD_ACCESS_MODE 0x8
#define EPLD_SPI_SWITCH 0x9

#define SEOUL_G24EU_HUMBER_RST_OFFSET 0xB

#define TRUE 1
#define FALSE 0
#define LATTICE_PROGRAM 0x100                     /*PC23*/
#define LATTICE_INIT      0x400                  /*PD21*/
#define LATTICE_DONE 0x1000             /*PD19*/
#define LATTICE_CCLK   0x2000                     /*PD18*/
#define LATTICE_DIN 0x800                       /*PD20*/
#define LATTICE_RST_IN        0x80                     /*PC24*/
#define XILINX_G24EU_PROGRAM 0x100                     /*PC23*/
#define XILINX_G24EU_INIT      0x400                  /*PD21*/
#define XILINX_G24EU_DONE 0x1000             /*PD19*/
#define XILINX_G24EU_CCLK   0x2000                     /*PD18*/
#define XILINX_G24EU_DIN 0x800                       /*PD20*/
#define XILINX_G24EU_RST_IN        0x80                     /*PC24*/

#define HEAD_CRC	2
#define EXTRA_FIELD	4
#define ORIG_NAME	8
#define COMMENT		0x10
#define RESERVED	0xe0

#define SPI_CLK_BIT      0x2000                  /*PD18*/
#define SPI_DO_BIT       0x800                  /*PD20*/
#define SPI_DI_BIT       0x1000                   /*PD19*/
#define SPI_CS_BIT       0x100                   /*PC23*/

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
	(cpm2_immr->im_ioport.iop_pdirc) |= SPI_CS_BIT;
	(cpm2_immr->im_ioport.iop_pdatc) |= SPI_CS_BIT;
    (cpm2_immr->im_ioport.iop_pparc) &= (~SPI_CS_BIT);

    (cpm2_immr->im_ioport.iop_pdird) |= SPI_CLK_BIT;
	(cpm2_immr->im_ioport.iop_pdatd) &= (~SPI_CLK_BIT);


    (cpm2_immr->im_ioport.iop_pdird) |= SPI_DO_BIT;

    (cpm2_immr->im_ioport.iop_pdird) &= (~SPI_DI_BIT);
    return 0;
}

int spi_cs(int assert_cs)
{
	if(assert_cs)
	{
		(cpm2_immr->im_ioport.iop_pdatc) |= SPI_CS_BIT;
	}
	else
	{
		(cpm2_immr->im_ioport.iop_pdatc) &= (~SPI_CS_BIT);
	}
	return 0;
}

int spi_clk(int assert_clk)
{
	if(assert_clk)
	{
		(cpm2_immr->im_ioport.iop_pdatd) |= SPI_CLK_BIT;
	}
	else
	{
		(cpm2_immr->im_ioport.iop_pdatd) &= (~SPI_CLK_BIT);
	}
	return 0;
}

int spi_out(int assert_out)
{
	if(assert_out)
	{
		(cpm2_immr->im_ioport.iop_pdatd) |= SPI_DO_BIT;
	}
	else
	{
		(cpm2_immr->im_ioport.iop_pdatd) &= (~SPI_DO_BIT);
	}
	return 0;
}

int spi_in(void)
{
	if((cpm2_immr->im_ioport.iop_pdatd) & SPI_DI_BIT)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	return 0;
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
    ret += ds3104_write( 0x20, 0xc3 );
    ret += ds3104_write( 0x21, 0xc3 );
    ret += ds3104_write( 0x22, 0xc3 );
    ret += ds3104_write( 0x23, 0xc3 );

    ret += ds3104_write( 0x4b, 0x0 );
    ret += ds3104_write( 0x18, 0x11 );
    ret += ds3104_write( 0x19, 0x21 );
    ret += ds3104_write( 0x1a, 0x0 );
    ret += ds3104_write( 0x1b, 0x0 );
    ret += ds3104_write( 0x1c, 0x0 );

    ret += ds3104_write( 0x4b, 0x10 );
    ret += ds3104_write( 0x18, 0x0 );
    ret += ds3104_write( 0x19, 0x0 );
    ret += ds3104_write( 0x1a, 0x0 );
    ret += ds3104_write( 0x1b, 0x0 );
    ret += ds3104_write( 0x1c, 0x0 );

    ret += ds3104_write( 0x4b, 0x0 );

    ret += ds3104_write( 0x65, 0x49 );
    ret += ds3104_write( 0x60, 0x04 );

    ret += ds3104_write( 0x4f, 0x01 );
    ret += ds3104_write( 0x61, 0x0 );

    ret += ds3104_write( 0x4f, 0x01 );
    ret += ds3104_write( 0x62, 0x0 );
    ret += ds3104_write( 0x63, 0xc0 );
    ret += ds3104_write( 0x7a, 0x0 );

    ret += ds3104_write( 0x41, 0x3f );
    ret += ds3104_write( 0x42, 0x00 );

    ret += ds3104_write( 0x69, 0x0e );
    ret += ds3104_write( 0x73, 0x22 );
    ret += ds3104_write( 0x74, 0xe5 );
    return ret;
}



#define EPLD_SPI_READ  0x1
#define EPLD_SPI_WRITE 0x0

int epld_read(unsigned short addr, unsigned int* value)
{
    unsigned short new_addr = 0;
    new_addr = (addr) | (EPLD_SPI_READ<<7);
    return spi_read(new_addr , value, EPLD_ADDR_LEN, EPLD_DATA_LEN);
}

int epld_write(unsigned short addr, unsigned int value)
{
    unsigned short new_addr = 0;
    new_addr = (addr)| (EPLD_SPI_WRITE<<7);
    return spi_write(new_addr , value, EPLD_ADDR_LEN, EPLD_DATA_LEN);
}


int pci_init(unsigned char offset)
{
    int ret = 0;
    unsigned int val;

    ret += epld_read(offset,&val);
    /*release dll*/
    val = (val & 0xe8);
    ret += epld_write(offset,val);
    udelay(10);

    /*release jtag*/
    val = (val | 0x2);
    ret += epld_write(offset,val);
    udelay(1000);
    /*reset jtag*/
    val = (val & 0xfd);
    ret += epld_write(offset,val);
    udelay(10000);

    /*release global*/
    val = (val | 0x4);
    ret += epld_write(offset,val);
    udelay(1000);

    /*reset glob*/
    val = (val & 0xfb);
    ret += epld_write(offset,val);
    udelay(10000);

    /*release pci*/
    val = (val | 0x1);
    ret += epld_write(offset,val);
    udelay(1000);
    return ret;
}


int xilinx_fpga_program_fn(int assert_program)
{
    if (assert_program)
        (cpm2_immr->im_ioport.iop_pdatc) &= (~XILINX_G24EU_PROGRAM);
    else
        (cpm2_immr->im_ioport.iop_pdatc) |= XILINX_G24EU_PROGRAM;

	return 0;
}

int xilinx_fpga_init_fn( void)
{
    return(!((cpm2_immr->im_ioport.iop_pdatd) & XILINX_G24EU_INIT));

}

int xilinx_fpga_clk_fn( int assert_clk)
{
    if (assert_clk)
        (cpm2_immr->im_ioport.iop_pdatd) |= XILINX_G24EU_CCLK;
    else
        (cpm2_immr->im_ioport.iop_pdatd) &= (~XILINX_G24EU_CCLK);

	return 0;
}

int xilinx_fpga_data_fn( int assert_data)
{
    if (assert_data)
        (cpm2_immr->im_ioport.iop_pdatd) |= XILINX_G24EU_DIN;
    else
        (cpm2_immr->im_ioport.iop_pdatd) &= ~XILINX_G24EU_DIN;

	return 0;
}

int xilinx_fpga_done_fn( void)
{
    return((cpm2_immr->im_ioport.iop_pdatd) & XILINX_G24EU_DONE);

}

int xilinx_fpga_gpio_init(void)
{
    (cpm2_immr->im_ioport.iop_pdatd) |= XILINX_G24EU_DIN;
    (cpm2_immr->im_ioport.iop_pdird) |= XILINX_G24EU_DIN;
    (cpm2_immr->im_ioport.iop_pdatd) &= ~XILINX_G24EU_CCLK;
    (cpm2_immr->im_ioport.iop_pdird) |= XILINX_G24EU_CCLK;
    (cpm2_immr->im_ioport.iop_pdird) &= ~XILINX_G24EU_DONE;
    (cpm2_immr->im_ioport.iop_pdird) &= ~XILINX_G24EU_INIT;
    (cpm2_immr->im_ioport.iop_pdatc) |= XILINX_G24EU_PROGRAM;
    (cpm2_immr->im_ioport.iop_pdirc) |= XILINX_G24EU_PROGRAM;
    cpm2_immr->im_ioport.iop_pparc &= ~XILINX_G24EU_RST_IN;
    (cpm2_immr->im_ioport.iop_pdatc) |= XILINX_G24EU_RST_IN;
    (cpm2_immr->im_ioport.iop_pdirc) |= XILINX_G24EU_RST_IN;

    return 0;
}

int xilinx_fpga_rst_fn(int assert_rst)
{
    if (assert_rst)
        (cpm2_immr->im_ioport.iop_pdatc) &= (~XILINX_G24EU_RST_IN);
    else
        (cpm2_immr->im_ioport.iop_pdatc) |= XILINX_G24EU_RST_IN;

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
    if (assert_program)
        (cpm2_immr->im_ioport.iop_pdatc) &= (~LATTICE_PROGRAM);
    else
        (cpm2_immr->im_ioport.iop_pdatc) |= LATTICE_PROGRAM;

	return 0;
}

int lattice_fpga_init_fn( void)
{
    return(!((cpm2_immr->im_ioport.iop_pdatd) & LATTICE_INIT));
}

int lattice_fpga_clk_fn( int assert_clk)
{
    if (assert_clk)
        (cpm2_immr->im_ioport.iop_pdatd) |= LATTICE_CCLK;
    else
        (cpm2_immr->im_ioport.iop_pdatd) &= (~LATTICE_CCLK);

	return 0;
}

int lattice_fpga_data_fn( int assert_data)
{
    if (assert_data)
        (cpm2_immr->im_ioport.iop_pdatd) |= LATTICE_DIN;
    else
        (cpm2_immr->im_ioport.iop_pdatd) &= ~LATTICE_DIN;

	return 0;
}

int lattice_fpga_done_fn( void)
{
    return((cpm2_immr->im_ioport.iop_pdatd) & LATTICE_DONE);
}

int lattice_fpga_gpio_init(void)
{
    (cpm2_immr->im_ioport.iop_pdatd) |= LATTICE_DIN;
    (cpm2_immr->im_ioport.iop_pdird) |= LATTICE_DIN;
    (cpm2_immr->im_ioport.iop_pdatd) &= ~LATTICE_CCLK;
    (cpm2_immr->im_ioport.iop_pdird) |= LATTICE_CCLK;
    (cpm2_immr->im_ioport.iop_pdird) &= ~LATTICE_DONE;
    (cpm2_immr->im_ioport.iop_pdird) &= ~LATTICE_INIT;
    (cpm2_immr->im_ioport.iop_pdatc) |= LATTICE_PROGRAM;
    (cpm2_immr->im_ioport.iop_pdirc) |= LATTICE_PROGRAM;
    cpm2_immr->im_ioport.iop_pparc &= ~LATTICE_RST_IN;
    (cpm2_immr->im_ioport.iop_pdatc) |= LATTICE_RST_IN;
    (cpm2_immr->im_ioport.iop_pdirc) |= LATTICE_RST_IN;

    return 0;
}

int lattice_fpga_rst_fn(int assert_rst)
{
    if (assert_rst)
        (cpm2_immr->im_ioport.iop_pdatc) &= (~LATTICE_RST_IN);
    else
        (cpm2_immr->im_ioport.iop_pdatc) |= LATTICE_RST_IN;

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
    printk(KERN_NOTICE"FPGA image decompress addr:%x size: %d\n", (unsigned int)starting_address, bsize);
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
#if 0
jqiu_mod
int
ctc_is_linecard(void)
{
#if 0
    unsigned char dev_cfg;
    unsigned char hw_cfg;

#ifdef CONFIG_CTC8247
       bd_t *binfo = (bd_t *)__res;
	memcpy(&ctc_board, &binfo->board, sizeof(ctc_board_t));
#endif

    dev_cfg = ctc_board.dev_cfg;
    hw_cfg = ctc_board.hw_cfg;
    product_series = (dev_cfg>>4)&0xf;

    if(0x2 == product_series)            /*e800*/
    {
        ctc_board.board_type = ((hw_cfg>>4)&0xf) | (product_series*(0x1<<8));
        switch (ctc_board.board_type)
        {
            case E800_SUPER:
                return 0;
            case E800_LC_24S:
            case E800_LC_2X8S:
                return 1;
            default:
                return 1;
        }
    }
    else if(0x6 == product_series)      /*e810*/
    {
        ctc_board.board_type = ((hw_cfg>>4)&0xf) | (product_series*(0x1<<8));
        switch (ctc_board.board_type)
        {
            case E810_SUPER:
                return 0;
            case E810_LC_24T:
            case E810_LC_24S:
            case E810_LC_2X8ST:
                return 1;
            default:
                return 1;
        }
    }
#endif
    return 0;
}
#endif
#endif

static void __init mpc8272_ads_pic_init(void)
{
	struct device_node *np = of_find_compatible_node(NULL, NULL,
	                                                 "fsl,cpm2-pic");
	if (!np) {
		printk(KERN_ERR "PIC init: can not find fsl,cpm2-pic node\n");
		return;
	}

	cpm2_pic_init(np);
    cpm2_pic_init2();

	of_node_put(np);

	/* Initialize stuff for the 82xx CPLD IC and install demux  */
	//pq2ads_pci_init_irq();
}

struct cpm_pin {
	int port, pin, flags;
};

static struct cpm_pin mpc8272_ads_pins[] = {
	/* FCC1 */
	{0, 14, CPM_PIN_INPUT | CPM_PIN_PRIMARY},
	{0, 15, CPM_PIN_INPUT | CPM_PIN_PRIMARY},
	{0, 16, CPM_PIN_INPUT | CPM_PIN_PRIMARY},
	{0, 17, CPM_PIN_INPUT | CPM_PIN_PRIMARY},
	{0, 18, CPM_PIN_OUTPUT | CPM_PIN_PRIMARY},
	{0, 19, CPM_PIN_OUTPUT | CPM_PIN_PRIMARY},
	{0, 20, CPM_PIN_OUTPUT | CPM_PIN_PRIMARY},
	{0, 21, CPM_PIN_OUTPUT | CPM_PIN_PRIMARY},
	{0, 26, CPM_PIN_INPUT | CPM_PIN_SECONDARY},
	{0, 27, CPM_PIN_INPUT | CPM_PIN_SECONDARY},
	{0, 28, CPM_PIN_OUTPUT | CPM_PIN_SECONDARY},
	{0, 29, CPM_PIN_OUTPUT | CPM_PIN_SECONDARY},
	{0, 30, CPM_PIN_INPUT | CPM_PIN_SECONDARY},
	{0, 31, CPM_PIN_INPUT | CPM_PIN_SECONDARY},
	{2, 21, CPM_PIN_INPUT | CPM_PIN_PRIMARY},
	{2, 22, CPM_PIN_INPUT | CPM_PIN_PRIMARY},

	/* FCC2 */
	{1, 18, CPM_PIN_INPUT | CPM_PIN_PRIMARY},
	{1, 19, CPM_PIN_INPUT | CPM_PIN_PRIMARY},
	{1, 20, CPM_PIN_INPUT | CPM_PIN_PRIMARY},
	{1, 21, CPM_PIN_INPUT | CPM_PIN_PRIMARY},
	{1, 22, CPM_PIN_OUTPUT | CPM_PIN_PRIMARY},
	{1, 23, CPM_PIN_OUTPUT | CPM_PIN_PRIMARY},
	{1, 24, CPM_PIN_OUTPUT | CPM_PIN_PRIMARY},
	{1, 25, CPM_PIN_OUTPUT | CPM_PIN_PRIMARY},
	{1, 26, CPM_PIN_INPUT | CPM_PIN_PRIMARY},
	{1, 27, CPM_PIN_INPUT | CPM_PIN_PRIMARY},
	{1, 28, CPM_PIN_INPUT | CPM_PIN_PRIMARY},
	{1, 29, CPM_PIN_OUTPUT | CPM_PIN_SECONDARY},
	{1, 30, CPM_PIN_INPUT | CPM_PIN_PRIMARY},
	{1, 31, CPM_PIN_OUTPUT | CPM_PIN_PRIMARY},
	{2, 16, CPM_PIN_INPUT | CPM_PIN_PRIMARY},
	{2, 17, CPM_PIN_INPUT | CPM_PIN_PRIMARY},

	/* I2C */
	{3, 14, CPM_PIN_INPUT | CPM_PIN_SECONDARY | CPM_PIN_OPENDRAIN},
	{3, 15, CPM_PIN_INPUT | CPM_PIN_SECONDARY | CPM_PIN_OPENDRAIN},
};

static void __init init_ioports(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(mpc8272_ads_pins); i++) {
		struct cpm_pin *pin = &mpc8272_ads_pins[i];
		cpm2_set_pin(pin->port, pin->pin, pin->flags);
	}
#if 0
	cpm2_clk_setup(CPM_CLK_SCC1, CPM_BRG1, CPM_CLK_RX);
	cpm2_clk_setup(CPM_CLK_SCC1, CPM_BRG1, CPM_CLK_TX);
	cpm2_clk_setup(CPM_CLK_SCC3, CPM_CLK8, CPM_CLK_RX);
	cpm2_clk_setup(CPM_CLK_SCC3, CPM_CLK8, CPM_CLK_TX);
	cpm2_clk_setup(CPM_CLK_SCC4, CPM_BRG4, CPM_CLK_RX);
	cpm2_clk_setup(CPM_CLK_SCC4, CPM_BRG4, CPM_CLK_TX);
#endif
	cpm2_clk_setup(CPM_CLK_FCC1, CPM_CLK11, CPM_CLK_RX);
	cpm2_clk_setup(CPM_CLK_FCC1, CPM_CLK10, CPM_CLK_TX);
	cpm2_clk_setup(CPM_CLK_FCC2, CPM_CLK15, CPM_CLK_RX);
	cpm2_clk_setup(CPM_CLK_FCC2, CPM_CLK16, CPM_CLK_TX);
}

static void __init mpc8272_ads_setup_arch(void)
{
#ifdef CONFIG_CTC8247
    unsigned char cpu_on_motherboard;
    unsigned int epld_ver = 0;
    unsigned int hw_ver = 0;
    unsigned int board_type = 0;
    unsigned int product_series = 0;
    unsigned int value = 0;
    unsigned int value1 = 0;
    int ret = 0;
#endif
#if 0
	struct device_node *np;
	__be32 __iomem *bcsr;
#endif

	if (ppc_md.progress)
		ppc_md.progress("mpc8272_ads_setup_arch()", 0);

	cpm2_reset();
#ifdef CONFIG_CTC8247

    /*SYS_EPLD_SIZE from uboot ctc8247.h*/
    epld_logic_addr = ioremap(0x60000000, SYS_EPLD_SIZE);
    if (epld_logic_addr == NULL)
    {
        printk(KERN_ERR "Cannot map epld\n");
		return;
    }

    cpu_on_motherboard = ioread8(epld_logic_addr);
    if(cpu_on_motherboard)
    {
        /*TODO*/
    }
    else
    {
        if((ioread8(epld_logic_addr + EPLD_ACCESS_MODE)) == 1)
        {
            /*access board epld through SPI GPIO*/

            /*switch cpu epld to board epld */
            iowrite8(0x5, epld_logic_addr + EPLD_SPI_SWITCH) ;
            spi_gpio_delay();
            iowrite8(0x1, epld_logic_addr + 0xa) ;
            iowrite8(0x0, epld_logic_addr + 0xa) ;

            gpio_init();
            spi_gpio_delay();

            epld_read(0x1, &board_type);
            board_type = (board_type>>4)&0xf;

            epld_read(0x3, &product_series);
            product_series = (product_series>>4)&0xf;

            epld_read(0x2, &epld_ver);
            epld_read(0x1, &hw_ver);

            printk(KERN_INFO "product series:0x%x, board_type:0x%x\n",product_series,board_type);

            if(product_series == GLB_BOARD_SERIES_SEOUL)
            {
                switch (board_type)
                {
                    case BOARD_SEOUL_G24EU:
                        printk(KERN_INFO "board type: SEOUL G24EU BOARD!\n");

                        /*modified by jcao for bug 14885*/
                        ret += epld_read(SEOUL_G24EU_HUMBER_RST_OFFSET,&value);
                        /* reset oam fpga */
                        value = (value & 0x3f);
                        ret += epld_write(SEOUL_G24EU_HUMBER_RST_OFFSET,value);

                         /*download OAM fpga */
                        iowrite8(0x0, epld_logic_addr + EPLD_SPI_SWITCH) ;
                         spi_gpio_delay();
                        iowrite8(0x1, epld_logic_addr + 0xa) ;
                        iowrite8(0x0, epld_logic_addr + 0xa) ;
                        xilinx_fpga_load(&seoul_g24eu_xilinx_bin_start,
                                    (&seoul_g24eu_xilinx_bin_end - &seoul_g24eu_xilinx_bin_start));

                        /*switch to board epld and release ad9517 &ds3104*/
                        iowrite8(0x5, epld_logic_addr + EPLD_SPI_SWITCH) ;
                        spi_gpio_delay();
                        iowrite8(0x1, epld_logic_addr + 0xa) ;
                        iowrite8(0x0, epld_logic_addr + 0xa) ;

                        epld_write(0x9, 0xa);

                        /*switch to ds3104 */
                        iowrite8(0x2, epld_logic_addr + EPLD_SPI_SWITCH) ;
                        spi_gpio_delay();
                        iowrite8(0x1, epld_logic_addr + 0xa) ;
                        iowrite8(0x0, epld_logic_addr + 0xa) ;
                        ret = ds3104_config();
                        if (ret)
                        {
                            printk(KERN_ERR "config ds3104 fail!\n");
                    		return;
                        }
                        ds3104_read(0,&value);
                        ds3104_read(1,&value1);
                        printk(KERN_INFO "config ds3104 successful(0x%x:0x%x)!\n",value,value1);
                        /*switch to ad9517 */
                        iowrite8(0x3, epld_logic_addr + EPLD_SPI_SWITCH) ;
                        spi_gpio_delay();
                        iowrite8(0x1, epld_logic_addr + 0xa) ;
                        iowrite8(0x0, epld_logic_addr + 0xa) ;
                        ret = ad9517_dev_init(E_AD9517_CLOCK_TYPE_5);
                        if (ret)
                        {
                            printk(KERN_ERR "config ad9517 fail!\n");
                    		return;
                        }
                        ad9517_read(0,&value);
                        printk(KERN_INFO "config ad9517 successful(0x%x)!\n",value);
                        /*switch to board epld */
                        iowrite8(0x5, epld_logic_addr + EPLD_SPI_SWITCH) ;
                        spi_gpio_delay();
                        iowrite8(0x1, epld_logic_addr + 0xa) ;
                        iowrite8(0x0, epld_logic_addr + 0xa) ;
                        ret = pci_init(SEOUL_G24EU_HUMBER_RST_OFFSET);
                        if (ret)
                        {
                            printk(KERN_ERR "Init pci fail!\n");
                    		return;
                        }
                        printk(KERN_INFO "init pci successful!\n");
                        break;
                    default:
                        break;
                }
            }
        }
        else /*access board epld through I2C*/
        {
            printk(KERN_INFO "board type: HUMBER DEMO BOARD!\n");
            /*TODO*/
            lattice_fpga_load(&humber_demo_lattice_bin_start,
                (&humber_demo_lattice_bin_end - &humber_demo_lattice_bin_start));
        }
    }

#endif
#ifndef CONFIG_CTC8247
	np = of_find_compatible_node(NULL, NULL, "fsl,mpc8272ads-bcsr");
	if (!np) {
		printk(KERN_ERR "No bcsr in device tree\n");
		return;
	}
	bcsr = of_iomap(np, 0);
	of_node_put(np);
	if (!bcsr) {
		printk(KERN_ERR "Cannot map BCSR registers\n");
		return;
	}

#define BCSR1_FETHIEN		0x08000000
#define BCSR1_FETH_RST		0x04000000
#define BCSR1_RS232_EN1		0x02000000
#define BCSR1_RS232_EN2		0x01000000
#define BCSR3_USB_nEN		0x80000000
#define BCSR3_FETHIEN2		0x10000000
#define BCSR3_FETH2_RST		0x08000000

	clrbits32(&bcsr[1], BCSR1_RS232_EN1 | BCSR1_RS232_EN2 | BCSR1_FETHIEN);
	setbits32(&bcsr[1], BCSR1_FETH_RST);

	clrbits32(&bcsr[3], BCSR3_FETHIEN2);
	setbits32(&bcsr[3], BCSR3_FETH2_RST);

	clrbits32(&bcsr[3], BCSR3_USB_nEN);

	iounmap(bcsr);
#endif
	init_ioports();
	pq2_init_pci();
	if (ppc_md.progress)
		ppc_md.progress("mpc8272_ads_setup_arch(), finish", 0);
}

static struct of_device_id __initdata of_bus_ids[] = {
	{ .name = "soc", },
	{ .name = "cpm", },
	{ .name = "localbus", },
	{},
};

static int __init declare_of_platform_devices(void)
{
	/* Publish the QE devices */
	of_platform_bus_probe(NULL, of_bus_ids, NULL);
	return 0;
}
machine_device_initcall(mpc8272_ads, declare_of_platform_devices);

/*
 * Called very early, device-tree isn't unflattened
 */
static int __init mpc8272_ads_probe(void)
{
	unsigned long root = of_get_flat_dt_root();
	return of_flat_dt_is_compatible(root, "fsl,mpc8272ads");
}

define_machine(mpc8272_ads)
{
	.name = "Freescale MPC8272 ADS",
	.probe = mpc8272_ads_probe,
	.setup_arch = mpc8272_ads_setup_arch,
	.init_IRQ = mpc8272_ads_pic_init,
	.get_irq = cpm2_get_irq,
	.calibrate_decr = generic_calibrate_decr,
	.restart = pq2_restart,
	.progress = udbg_progress,
};
