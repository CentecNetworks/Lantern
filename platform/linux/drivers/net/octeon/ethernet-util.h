/**********************************************************************
 * Author: Cavium Networks
 *
 * Contact: support@caviumnetworks.com
 * This file is part of the OCTEON SDK
 *
 * Copyright (c) 2003-2007 Cavium Networks
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, Version 2, as
 * published by the Free Software Foundation.
 *
 * This file is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * or visit http://www.gnu.org/licenses/.
 *
 * This file may also be available under a different license from Cavium.
 * Contact Cavium Networks for more information
*********************************************************************/

#include <asm/octeon/cvmx-pip.h>

#define DEBUGPRINT(format, ...) do { if (printk_ratelimit()) 		\
					printk(format, ##__VA_ARGS__);	\
				} while (0)

/**
 * cvm_oct_get_buffer_ptr - convert packet data address to pointer
 * @packet_ptr: Packet data hardware address
 *
 * Returns Packet buffer pointer
 */
static inline void *cvm_oct_get_buffer_ptr(union cvmx_buf_ptr packet_ptr)
{
	return cvmx_phys_to_ptr(((packet_ptr.s.addr >> 7) - packet_ptr.s.back)
				<< 7);
}

/**
 * INTERFACE - convert IPD port to locgical interface
 * @ipd_port: Port to check
 *
 * Returns Logical interface
 */
static inline int INTERFACE(int ipd_port)
{
	int interface;

        /* Non existant interface for POW0 */
	if (ipd_port == CVMX_PIP_NUM_INPUT_PORTS)
		return cvmx_helper_get_interface_num(ipd_port-1) + 1;

	interface = cvmx_helper_get_interface_num(ipd_port);
	
	if (interface == -1)
		panic("Illegal ipd_port %d passed to INTERFACE\n", ipd_port);

	return interface;
}

/**
 * INDEX - convert IPD/PKO port number to the port's interface index
 * @ipd_port: Port to check
 *
 * Returns Index into interface port list
 */
static inline int INDEX(int ipd_port)
{
	if (ipd_port == CVMX_PIP_NUM_INPUT_PORTS)
		return 0;

	return cvmx_helper_get_interface_index_num(ipd_port);
}
