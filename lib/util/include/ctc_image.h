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

#ifndef __CTC_IMAGE_H__
#define __CTC_IMAGE_H__

/*
 * Operating System Codes
 */
#define CTC_IH_OS_INVALID      0    /* Invalid OS */
#define CTC_IH_OS_OPENBSD      1    /* OpenBSD */
#define CTC_IH_OS_NETBSD       2    /* NetBSD */
#define CTC_IH_OS_FREEBSD      3    /* FreeBSD */
#define CTC_IH_OS_4_4BSD       4    /* 4.4BSD */
#define CTC_IH_OS_LINUX        5    /* Linux */
#define CTC_IH_OS_SVR4         6    /* SVR4  */
#define CTC_IH_OS_ESIX         7    /* Esix  */
#define CTC_IH_OS_SOLARIS      8    /* Solaris */
#define CTC_IH_OS_IRIX         9    /* Irix  */
#define CTC_IH_OS_SCO          10    /* SCO  */
#define CTC_IH_OS_DELL         11    /* Dell  */
#define CTC_IH_OS_NCR          12    /* NCR  */
#define CTC_IH_OS_LYNXOS       13    /* LynxOS */
#define CTC_IH_OS_VXWORKS      14    /* VxWorks */
#define CTC_IH_OS_PSOS         15    /* pSOS  */
#define CTC_IH_OS_QNX          16    /* QNX  */
#define CTC_IH_OS_U_BOOT       17    /* Firmware */
#define CTC_IH_OS_RTEMS        18    /* RTEMS */
#define CTC_IH_OS_ARTOS        19    /* ARTOS */
#define CTC_IH_OS_UNITY        20    /* Unity OS */

/*
 * CPU Architecture Codes (supported by Linux)
 */
#define CTC_IH_CPU_INVALID     0    /* Invalid CPU */
#define CTC_IH_CPU_ALPHA       1    /* Alpha */
#define CTC_IH_CPU_ARM         2    /* ARM  */
#define CTC_IH_CPU_I386        3    /* Intel x86 */
#define CTC_IH_CPU_IA64        4    /* IA64  */
#define CTC_IH_CPU_MIPS        5    /* MIPS  */
#define CTC_IH_CPU_MIPS64      6    /* MIPS     64 Bit */
#define CTC_IH_CPU_PPC         7    /* PowerPC */
#define CTC_IH_CPU_S390        8    /* IBM S390 */
#define CTC_IH_CPU_SH          9    /* SuperH */
#define CTC_IH_CPU_SPARC       10    /* Sparc */
#define CTC_IH_CPU_SPARC64     11    /* Sparc 64 Bit */
#define CTC_IH_CPU_M68K        12    /* M68K  */
#define CTC_IH_CPU_NIOS        13    /* Nios-32 */
#define CTC_IH_CPU_MICROBLAZE  14    /* MicroBlaze   */
#define CTC_IH_CPU_NIOS2       15    /* Nios-II */
#define CTC_IH_CPU_BLACKFIN    16    /* Blackfin */
#define CTC_IH_CPU_AVR32       17    /* AVR32 */

/*
 * Image Types
 *
 * "Standalone Programs" are directly runnable in the environment
 *    provided by U-Boot; it is expected that (if they behave
 *    well) you can continue to work in U-Boot after return from
 *    the Standalone Program.
 * "OS Kernel Images" are usually images of some Embedded OS which
 *    will take over control completely. Usually these programs
 *    will install their own set of exception handlers, device
 *    drivers, set up the MMU, etc. - this means, that you cannot
 *    expect to re-enter U-Boot except by resetting the CPU.
 * "RAMDisk Images" are more or less just data blocks, and their
 *    parameters (address, size) are passed to an OS kernel that is
 *    being started.
 * "Multi-File Images" contain several images, typically an OS
 *    (Linux) kernel image and one or more data images like
 *    RAMDisks. This construct is useful for instance when you want
 *    to boot over the network using BOOTP etc., where the boot
 *    server provides just a single image file, but you want to get
 *    for instance an OS kernel and a RAMDisk image.
 *
 *    "Multi-File Images" start with a list of image sizes, each
 *    image size (in bytes) specified by an "uint32_t" in network
 *    byte order. This list is terminated by an "(uint32_t)0".
 *    Immediately after the terminating 0 follow the images, one by
 *    one, all aligned on "uint32_t" boundaries (size rounded up to
 *    a multiple of 4 bytes - except for the last file).
 *
 * "Firmware Images" are binary images containing firmware (like
 *    U-Boot or FPGA images) which usually will be programmed to
 *    flash memory.
 *
 * "Script files" are command sequences that will be executed by
 *    U-Boot's command interpreter; this feature is especially
 *    useful when you configure U-Boot to use a real shell (hush)
 *    as command interpreter (=> Shell Scripts).
 */

#define CTC_IH_TYPE_INVALID        0    /* Invalid Image  */
#define CTC_IH_TYPE_STANDALONE     1    /* Standalone Program  */
#define CTC_IH_TYPE_KERNEL         2    /* OS Kernel Image  */
#define CTC_IH_TYPE_RAMDISK        3    /* RAMDisk Image  */
#define CTC_IH_TYPE_MULTI          4    /* Multi-File Image  */
#define CTC_IH_TYPE_FIRMWARE       5    /* Firmware Image  */
#define CTC_IH_TYPE_SCRIPT         6    /* Script file   */
#define CTC_IH_TYPE_FILESYSTEM     7    /* Filesystem Image (any type) */
#define CTC_IH_TYPE_FLATDT         8    /* Binary Flat Device Tree Blob */

/*
 * Compression Types
 */
#define CTC_IH_COMP_NONE        0    /*  No     Compression Used */
#define CTC_IH_COMP_GZIP        1    /* gzip     Compression Used */
#define CTC_IH_COMP_BZIP2       2    /* bzip2 Compression Used */

#define CTC_IH_MAGIC       0x27051956    /* Image Magic Number  */
#define CTC_IH_NMLEN       32    /* Image Name Length  */

/*
 * all data in network byte order (aka natural aka bigendian)
 */
typedef struct ctc_image_header
{
    uint32_t    ih_magic;   /* Image Header Magic Number */
    uint32_t    ih_hcrc;    /* Image Header CRC Checksum */
    uint32_t    ih_time;    /* Image Creation Timestamp */
    uint32_t    ih_size;    /* Image Data Size  */
    uint32_t    ih_load;    /* Data     Load  Address  */
    uint32_t    ih_ep;      /* Entry Point Address  */
    uint32_t    ih_dcrc;    /* Image Data CRC Checksum */
    uint8_t     ih_os;      /* Operating System  */
    uint8_t     ih_arch;    /* CPU architecture  */
    uint8_t     ih_type;    /* Image Type   */
    uint8_t     ih_comp;    /* Compression Type  */
    char     ih_name[CTC_IH_NMLEN];    /* Image Name  */
} ctc_image_header_t;

#endif    /* __CTC_IMAGE_H__ */
