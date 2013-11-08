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
**********************************************************************/
#include <linux/kernel.h>
#include <linux/ethtool.h>
#include <linux/phy.h>
#include <linux/net_tstamp.h>

#include <net/dst.h>

#include <asm/octeon/octeon.h>
#include <asm/octeon/cvmx-helper.h>
#include <asm/octeon/cvmx-helper-board.h>
#include <asm/octeon/cvmx-srio.h>

#include <asm/octeon/cvmx-smix-defs.h>
#include <asm/octeon/cvmx-pko-defs.h>
#include <asm/octeon/cvmx-gmxx-defs.h>

#include "ethernet-defines.h"
#include "octeon-ethernet.h"
#include "ethernet-mdio.h"
#include "ethernet-util.h"


static void cvm_oct_get_drvinfo(struct net_device *dev,
				struct ethtool_drvinfo *info)
{
	strcpy(info->driver, "octeon-ethernet");
	strcpy(info->version, OCTEON_ETHERNET_VERSION);
	strcpy(info->bus_info, "Builtin");
}

static int cvm_oct_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct octeon_ethernet *priv = netdev_priv(dev);

	if (priv->phydev)
		return phy_ethtool_gset(priv->phydev, cmd);

	return -EINVAL;
}

static int cvm_oct_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct octeon_ethernet *priv = netdev_priv(dev);

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (priv->phydev)
		return phy_ethtool_sset(priv->phydev, cmd);

	return -EINVAL;
}

static int cvm_oct_nway_reset(struct net_device *dev)
{
	struct octeon_ethernet *priv = netdev_priv(dev);

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (priv->phydev)
		return phy_start_aneg(priv->phydev);

	return -EINVAL;
}

const struct ethtool_ops cvm_oct_ethtool_ops = {
	.get_drvinfo = cvm_oct_get_drvinfo,
	.get_settings = cvm_oct_get_settings,
	.set_settings = cvm_oct_set_settings,
	.nway_reset = cvm_oct_nway_reset,
	.get_link = ethtool_op_get_link,
	.get_sg = ethtool_op_get_sg,
	.get_tx_csum = ethtool_op_get_tx_csum,
};

/**
 * cvm_oct_ioctl_hwtstamp - IOCTL support for timestamping
 * @dev:    Device to change
 * @rq:     the request
 * @cmd:    the command
 *
 * Returns Zero on success
 */
static int cvm_oct_ioctl_hwtstamp(struct net_device *dev,
	struct ifreq *rq, int cmd)
{
	struct octeon_ethernet *priv = netdev_priv(dev);
	struct hwtstamp_config config;
	union cvmx_mio_ptp_clock_cfg ptp;
	int have_hw_timestamps = 0;

	if (copy_from_user(&config, rq->ifr_data, sizeof(config)))
		return -EFAULT;

	if (config.flags) /* reserved for future extensions */
		return -EINVAL;

	/* Check the status of hardware for tiemstamps */
	if (OCTEON_IS_MODEL(OCTEON_CN6XXX)) {
		/* Write TX timestamp into word 4 */
		cvmx_write_csr(CVMX_PKO_REG_TIMESTAMP, 4);
		/* Get the current state of the PTP clock */
		ptp.u64 = octeon_read_ptp_csr(CVMX_MIO_PTP_CLOCK_CFG);
		if (!ptp.s.ext_clk_en) {
			/*
			 * The clock has not been configured to use an
			 * external source.  Program it to use the main clock
			 * reference.
			 */
			unsigned long long clock_comp = (NSEC_PER_SEC << 32) /
				octeon_get_io_clock_rate();
			if (!ptp.s.ptp_en)
				cvmx_write_csr(CVMX_MIO_PTP_CLOCK_COMP, clock_comp);
			pr_info("PTP Clock: Using sclk reference at %lld Hz\n",
				(NSEC_PER_SEC << 32) / clock_comp);
		} else {
			/* The clock is already programmed to use a GPIO */
			unsigned long long clock_comp = octeon_read_ptp_csr(
				CVMX_MIO_PTP_CLOCK_COMP);
			pr_info("PTP Clock: Using GPIO %d at %lld Hz\n",
				ptp.s.ext_clk_in,
				(NSEC_PER_SEC << 32) / clock_comp);
		}

		/* Enable the clock if it wasn't done already */
		if (!ptp.s.ptp_en) {
			ptp.s.ptp_en = 1;
			cvmx_write_csr(CVMX_MIO_PTP_CLOCK_CFG, ptp.u64);
		}
		have_hw_timestamps = 1;
		/* Only the first two interfaces support hardware timestamps */
		if (priv->port >= 32)
			have_hw_timestamps = 0;
	}

	/* Require hardware if ALLOW_TIMESTAMPS_WITHOUT_HARDWARE=0 */
	if (!ALLOW_TIMESTAMPS_WITHOUT_HARDWARE && !have_hw_timestamps)
		return -EINVAL;

	switch (config.tx_type) {
		case HWTSTAMP_TX_OFF:
			priv->flags &= ~(OCTEON_ETHERNET_FLAG_TX_TIMESTAMP_SW |
					 OCTEON_ETHERNET_FLAG_TX_TIMESTAMP_HW);
			break;
		case HWTSTAMP_TX_ON:
			priv->flags |= (have_hw_timestamps) ?
				OCTEON_ETHERNET_FLAG_TX_TIMESTAMP_HW :
				OCTEON_ETHERNET_FLAG_TX_TIMESTAMP_SW;
			break;
		default:
			return -ERANGE;
	}

	switch (config.rx_filter) {
		case HWTSTAMP_FILTER_NONE:
			priv->flags &= ~(OCTEON_ETHERNET_FLAG_RX_TIMESTAMP_HW |
					 OCTEON_ETHERNET_FLAG_RX_TIMESTAMP_SW);
			if (have_hw_timestamps) {
				int interface = INTERFACE(priv->port);
				int index = INDEX(priv->port);
				union cvmx_gmxx_rxx_frm_ctl gmxx_rxx_frm_ctl;
				union cvmx_pip_prt_cfgx pip_prt_cfgx;

				gmxx_rxx_frm_ctl.u64 = cvmx_read_csr(CVMX_GMXX_RXX_FRM_CTL(index, interface));
				gmxx_rxx_frm_ctl.s.ptp_mode = 0;
				cvmx_write_csr(CVMX_GMXX_RXX_FRM_CTL(index, interface), gmxx_rxx_frm_ctl.u64);

				pip_prt_cfgx.u64 = cvmx_read_csr(CVMX_PIP_PRT_CFGX(priv->port));
				pip_prt_cfgx.s.skip = 0;
				cvmx_write_csr(CVMX_PIP_PRT_CFGX(priv->port), pip_prt_cfgx.u64);
			}
			break;
		case HWTSTAMP_FILTER_ALL:
		case HWTSTAMP_FILTER_SOME:
		case HWTSTAMP_FILTER_PTP_V1_L4_EVENT:
		case HWTSTAMP_FILTER_PTP_V1_L4_SYNC:
		case HWTSTAMP_FILTER_PTP_V1_L4_DELAY_REQ:
		case HWTSTAMP_FILTER_PTP_V2_L4_EVENT:
		case HWTSTAMP_FILTER_PTP_V2_L4_SYNC:
		case HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ:
		case HWTSTAMP_FILTER_PTP_V2_L2_EVENT:
		case HWTSTAMP_FILTER_PTP_V2_L2_SYNC:
		case HWTSTAMP_FILTER_PTP_V2_L2_DELAY_REQ:
		case HWTSTAMP_FILTER_PTP_V2_EVENT:
		case HWTSTAMP_FILTER_PTP_V2_SYNC:
		case HWTSTAMP_FILTER_PTP_V2_DELAY_REQ:
			priv->flags |= (have_hw_timestamps) ?
				OCTEON_ETHERNET_FLAG_RX_TIMESTAMP_HW :
				OCTEON_ETHERNET_FLAG_RX_TIMESTAMP_SW;
			config.rx_filter = HWTSTAMP_FILTER_ALL;
			if (have_hw_timestamps) {
				int interface = INTERFACE(priv->port);
				int index = INDEX(priv->port);
				union cvmx_gmxx_rxx_frm_ctl gmxx_rxx_frm_ctl;
				union cvmx_pip_prt_cfgx pip_prt_cfgx;

				gmxx_rxx_frm_ctl.u64 = cvmx_read_csr(CVMX_GMXX_RXX_FRM_CTL(index, interface));
				gmxx_rxx_frm_ctl.s.ptp_mode = 1;
				cvmx_write_csr(CVMX_GMXX_RXX_FRM_CTL(index, interface), gmxx_rxx_frm_ctl.u64);

				pip_prt_cfgx.u64 = cvmx_read_csr(CVMX_PIP_PRT_CFGX(priv->port));
				pip_prt_cfgx.s.skip = 8;
				cvmx_write_csr(CVMX_PIP_PRT_CFGX(priv->port), pip_prt_cfgx.u64);
			}
			break;
		default:
			return -ERANGE;
	}

	if (copy_to_user(rq->ifr_data, &config, sizeof(config)))
		return -EFAULT;

	return 0;
}

/**
 * cvm_oct_ioctl - IOCTL support for PHY control
 * @dev:    Device to change
 * @rq:     the request
 * @cmd:    the command
 *
 * Returns Zero on success
 */
int cvm_oct_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	struct octeon_ethernet *priv = netdev_priv(dev);
	cvmx_srio_tx_message_header_t tx_header;
	int ivalue;

	switch (cmd) {
	case CAVIUM_NET_IOCTL_SETPRIO:
		ivalue = rq->ifr_ifru.ifru_ivalue;
		if ((ivalue >= 0) && (ivalue < 4)) {
			tx_header.u64 = priv->srio_tx_header;
			tx_header.s.prio = ivalue;
			priv->srio_tx_header = tx_header.u64;
			return 0;
		}
		return -EINVAL;

	case CAVIUM_NET_IOCTL_GETPRIO:
		tx_header.u64 = priv->srio_tx_header;
		rq->ifr_ifru.ifru_ivalue = tx_header.s.prio;
		return 0;

	case CAVIUM_NET_IOCTL_SETIDSIZE:
		ivalue = rq->ifr_ifru.ifru_ivalue;
		if ((ivalue >= 0) && (ivalue < 2)) {
			tx_header.u64 = priv->srio_tx_header;
			tx_header.s.tt = ivalue;
			priv->srio_tx_header = tx_header.u64;
			return 0;
		}
		return -EINVAL;

	case CAVIUM_NET_IOCTL_GETIDSIZE:
		tx_header.u64 = priv->srio_tx_header;
		rq->ifr_ifru.ifru_ivalue = tx_header.s.tt;
		return 0;

	case CAVIUM_NET_IOCTL_SETSRCID:
		ivalue = rq->ifr_ifru.ifru_ivalue;
		if ((ivalue >= 0) && (ivalue < 2)) {
			tx_header.u64 = priv->srio_tx_header;
			tx_header.s.sis = ivalue;
			priv->srio_tx_header = tx_header.u64;
			return 0;
		}
		return -EINVAL;

	case CAVIUM_NET_IOCTL_GETSRCID:
		tx_header.u64 = priv->srio_tx_header;
		rq->ifr_ifru.ifru_ivalue = tx_header.s.sis;
		return 0;

	case CAVIUM_NET_IOCTL_SETLETTER:
		ivalue = rq->ifr_ifru.ifru_ivalue;
		if ((ivalue >= -1) && (ivalue < 4)) {
			tx_header.u64 = priv->srio_tx_header;
			tx_header.s.lns = (ivalue == -1);
			if (tx_header.s.lns)
				tx_header.s.letter = 0;
			else
				tx_header.s.letter = ivalue;
			priv->srio_tx_header = tx_header.u64;
			return 0;
		}
		return -EINVAL;

	case CAVIUM_NET_IOCTL_GETLETTER:
		tx_header.u64 = priv->srio_tx_header;
		if (tx_header.s.lns)
			rq->ifr_ifru.ifru_ivalue = -1;
		else
			rq->ifr_ifru.ifru_ivalue = tx_header.s.letter;
		return 0;

	case SIOCSHWTSTAMP:
		return cvm_oct_ioctl_hwtstamp(dev, rq, cmd);

	default:
		if (priv->phydev)
			return phy_mii_ioctl(priv->phydev, if_mii(rq), cmd);
		return -EINVAL;
	}
}

/**
 * cvm_oct_set_carrier - common wrapper of netif_carrier_{on,off}
 *
 * @priv: Device struct.
 * @link_info: Current state.
 */
void cvm_oct_set_carrier(struct octeon_ethernet *priv, cvmx_helper_link_info_t link_info)
{
	if (link_info.s.link_up) {
		if (!netif_carrier_ok(priv->netdev))
			netif_carrier_on(priv->netdev);
		if (priv->num_tx_queues)
			DEBUGPRINT("%s: %u Mbps %s duplex, port %2d, queue %2d\n",
				   priv->netdev->name, link_info.s.speed,
				   (link_info.s.full_duplex) ? "Full" : "Half",
				   priv->port, priv->tx_queue[0].queue);
		else
			DEBUGPRINT("%s: %u Mbps %s duplex, port %2d, POW\n",
				   priv->netdev->name, link_info.s.speed,
				   (link_info.s.full_duplex) ? "Full" : "Half",
				   priv->port);
	} else {
		if (netif_carrier_ok(priv->netdev))
			netif_carrier_off(priv->netdev);
		DEBUGPRINT("%s: Link down\n", priv->netdev->name);
	}
}

static void cvm_oct_adjust_link(struct net_device *dev)
{
	struct octeon_ethernet *priv = netdev_priv(dev);
	cvmx_helper_link_info_t link_info;

	if (priv->last_link != priv->phydev->link) {
		priv->last_link = priv->phydev->link;
		link_info.u64 = 0;
		link_info.s.link_up = priv->last_link ? 1 : 0;
		link_info.s.full_duplex = priv->phydev->duplex ? 1 : 0;
		link_info.s.speed = priv->phydev->speed;
		cvmx_helper_link_set(priv->port, link_info);
		cvm_oct_set_carrier(priv, link_info);
	}
}


/**
 * cvm_oct_phy_setup_device - setup the PHY
 *
 * @dev:    Device to setup
 *
 * Returns Zero on success, negative on failure
 */
int cvm_oct_phy_setup_device(struct net_device *dev)
{
	struct octeon_ethernet *priv = netdev_priv(dev);

	int phy_addr = cvmx_helper_board_get_mii_address(priv->port);
	if (phy_addr != -1) {
		char phy_id[20];

		if (phy_addr & 0x100)
			snprintf(phy_id, sizeof(phy_id), PHY_ID_FMT, "1", phy_addr & 0xff);
		else
			snprintf(phy_id, sizeof(phy_id), PHY_ID_FMT, "0", phy_addr);

		priv->phydev = phy_connect(dev, phy_id, cvm_oct_adjust_link, 0,
					   PHY_INTERFACE_MODE_GMII);

		if (IS_ERR(priv->phydev)) {
			priv->phydev = NULL;
			return -1;
		}
		priv->last_link = 0;
		phy_start_aneg(priv->phydev);
	}
	return 0;
}
