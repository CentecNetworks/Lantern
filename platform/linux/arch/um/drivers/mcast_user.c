/*
 * user-mode-linux networking multicast transport
 * Copyright (C) 2001 - 2007 Jeff Dike (jdike@{addtoit,linux.intel}.com)
 * Copyright (C) 2001 by Harald Welte <laforge@gnumonks.org>
 *
 * based on the existing uml-networking code, which is
 * Copyright (C) 2001 Lennert Buytenhek (buytenh@gnu.org) and
 * James Leu (jleu@mindspring.net).
 * Copyright (C) 2001 by various other people who didn't put their name here.
 *
 * Licensed under the GPL.
 *
 */

#include <unistd.h>
#include <errno.h>
#include <net/if.h>
#include <netinet/in.h>
#include "kern_constants.h"
#include "mcast.h"
#include "net_user.h"
#include "um_malloc.h"
#include "user.h"
#include "os.h"

extern void *memcpy(void *, const void *, size_t);
static struct sockaddr_in *new_addr(char *addr, unsigned short port)
{
	struct sockaddr_in *sin;

	sin = uml_kmalloc(sizeof(struct sockaddr_in), UM_GFP_KERNEL);
	if (sin == NULL) {
		printk(UM_KERN_ERR "new_addr: allocation of sockaddr_in "
		       "failed\n");
		return NULL;
	}
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = in_aton(addr);
	sin->sin_port = htons(port);
	return sin;
}

static int mcast_user_init(void *data, void *dev)
{
	struct mcast_data *pri = data;

	pri->mcast_addr = new_addr(pri->addr, pri->port);
	pri->dev = dev;
    pri->tx_fd = -1;
	return 0;
}

static void mcast_remove(void *data)
{
	struct mcast_data *pri = data;

	kfree(pri->mcast_addr);
	pri->mcast_addr = NULL;
}

static int mcast_open(void *data)
{
	struct mcast_data *pri = data;
	struct sockaddr_in *sin = pri->mcast_addr;
	struct ip_mreq mreq;
	int fd, yes = 1, err = -EINVAL;
    int sklen;

	if ((sin->sin_addr.s_addr == 0) || (sin->sin_port == 0))
		goto out;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (fd < 0) {
		err = -errno;
		printk(UM_KERN_ERR "mcast_open : data socket failed, "
		       "errno = %d\n", errno);
		goto out;
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
		err = -errno;
		printk(UM_KERN_ERR "mcast_open: SO_REUSEADDR failed, "
		       "errno = %d\n", errno);
		goto out_close;
	}

	/* set ttl according to config */
	if (setsockopt(fd, SOL_IP, IP_MULTICAST_TTL, &pri->ttl,
		       sizeof(pri->ttl)) < 0) {
		err = -errno;
		printk(UM_KERN_ERR "mcast_open: IP_MULTICAST_TTL failed, "
		       "error = %d\n", errno);
		goto out_close;
	}

	/* set LOOP, so data does get fed back to local sockets */
	if (setsockopt(fd, SOL_IP, IP_MULTICAST_LOOP, &yes, sizeof(yes)) < 0) {
		err = -errno;
		printk(UM_KERN_ERR "mcast_open: IP_MULTICAST_LOOP failed, "
		       "error = %d\n", errno);
		goto out_close;
	}

	/* bind socket to mcast address */
	if (bind(fd, (struct sockaddr *) sin, sizeof(*sin)) < 0) {
		err = -errno;
		printk(UM_KERN_ERR "mcast_open : data bind failed, "
		       "errno = %d\n", errno);
		goto out_close;
	}

	/* subscribe to the multicast group */
	mreq.imr_multiaddr.s_addr = sin->sin_addr.s_addr;
	mreq.imr_interface.s_addr = 0;
	if (setsockopt(fd, SOL_IP, IP_ADD_MEMBERSHIP,
		       &mreq, sizeof(mreq)) < 0) {
		err = -errno;
		printk(UM_KERN_ERR "mcast_open: IP_ADD_MEMBERSHIP failed, "
		       "error = %d\n", errno);
		printk(UM_KERN_ERR "There appears not to be a multicast-"
		       "capable network interface on the host.\n");
		printk(UM_KERN_ERR "eth0 should be configured in order to use "
		       "the multicast transport.\n");
		goto out_close;
	}
    pri->tx_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (pri->tx_fd < 0)
    {
        err = -errno;
        printk("mcast_open: socket() failed: error = %d\n", errno);
        goto out_close;
    }

    setsockopt(pri->tx_fd, SOL_IP, IP_MULTICAST_TTL, &pri->ttl, sizeof(pri->ttl));

    if (connect(pri->tx_fd, (struct sockaddr *)sin, sizeof(*sin)) < 0)
    {
        err = -errno;
        printk("mcast_open: connect() failed: error = %d\n", errno);
        os_close_file(pri->tx_fd);
        pri->tx_fd = -1;
        goto out_close;
    }

    sklen = sizeof(pri->local_addr);
    if (getsockname(pri->tx_fd, (struct sockaddr *)&pri->local_addr, &sklen) < 0)
    {
        err = -errno;
        printk("mcast_open: getsockname() failed: error = %d\n", errno);
        os_close_file(pri->tx_fd);
        pri->tx_fd = -1;
        goto out_close;
    }

    pri->fd = fd;
	return fd;

 out_close:
	close(fd);
 out:
	return err;
}

static void mcast_close(int fd, void *data)
{
	struct ip_mreq mreq;
	struct mcast_data *pri = data;
	struct sockaddr_in *sin = pri->mcast_addr;
    os_close_file(pri->tx_fd);
    pri->tx_fd = -1;

	mreq.imr_multiaddr.s_addr = sin->sin_addr.s_addr;
	mreq.imr_interface.s_addr = 0;
	if (setsockopt(fd, SOL_IP, IP_DROP_MEMBERSHIP,
		       &mreq, sizeof(mreq)) < 0) {
		printk(UM_KERN_ERR "mcast_open: IP_DROP_MEMBERSHIP failed, "
		       "error = %d\n", errno);
	}

	close(fd);
}

int mcast_user_write(int fd, void *buf, int len, struct mcast_data *pri)
{
	struct sockaddr_in *data_addr = pri->mcast_addr;

	return net_sendto(pri->tx_fd, buf, len, data_addr, sizeof(*data_addr));
}
static int mcast_change_config(void *data, void *ifr)
{
	struct mcast_data *pri = data;
	struct sockaddr_in *xsin = pri->mcast_addr;
	struct sockaddr_in *sin = (struct sockaddr_in *)&((struct ifreq *)ifr)->ifr_addr;
	struct ip_mreq mreq;
	int fd, tx_fd, yes = 1, err = -EINVAL;
    int sklen;

    if (!((struct ifreq *)ifr)->ifr_name[0])
    {
        /* The device is not open */
        xsin->sin_addr = sin->sin_addr;
        xsin->sin_port = sin->sin_port;

        return 0;
    }

	if ((sin->sin_addr.s_addr == 0) || (sin->sin_port == 0))
		goto out;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (fd < 0){
		err = -errno;
		printk("mcast_open : data socket failed, errno = %d\n", 
		       errno);
		goto out;
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
		err = -errno;
		printk("mcast_open: SO_REUSEADDR failed, errno = %d\n",
			errno);
		goto out_close;
	}

	/* set ttl according to config */
	if (setsockopt(fd, SOL_IP, IP_MULTICAST_TTL, &pri->ttl,
		       sizeof(pri->ttl)) < 0) {
		err = -errno;
		printk("mcast_open: IP_MULTICAST_TTL failed, error = %d\n",
			errno);
		goto out_close;
	}

	/* set LOOP, so data does get fed back to local sockets */
	if (setsockopt(fd, SOL_IP, IP_MULTICAST_LOOP, &yes, sizeof(yes)) < 0) {
		err = -errno;
		printk("mcast_open: IP_MULTICAST_LOOP failed, error = %d\n",
			errno);
		goto out_close;
	}

	/* bind socket to mcast address */
	if (bind(fd, (struct sockaddr *) sin, sizeof(*sin)) < 0) {
		err = -errno;
		printk("mcast_open : data bind failed, errno = %d\n", errno);
		goto out_close;
	}		
	
	/* subscribe to the multicast group */
	mreq.imr_multiaddr.s_addr = sin->sin_addr.s_addr;
	mreq.imr_interface.s_addr = 0;
	if (setsockopt(fd, SOL_IP, IP_ADD_MEMBERSHIP, 
		       &mreq, sizeof(mreq)) < 0) {
		err = -errno;
		printk("mcast_open: IP_ADD_MEMBERSHIP failed, error = %d\n",
			errno);
		printk("There appears not to be a multicast-capable network "
		       "interface on the host.\n");
		printk("eth0 should be configured in order to use the "
		       "multicast transport.\n");
		goto out_close;
	}

    tx_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (tx_fd < 0)
    {
        err = -errno;
        printk("mcast_open: socket() failed: error = %d\n", errno);
        goto out_close;
    }

    setsockopt(tx_fd, SOL_IP, IP_MULTICAST_TTL, &pri->ttl, sizeof(pri->ttl));

    if (connect(tx_fd, (struct sockaddr *)sin, sizeof(*sin)) < 0)
    {
        err = -errno;
        printk("mcast_open: connect() failed: error = %d\n", errno);
        os_close_file(tx_fd);
        tx_fd = -1;
        goto out_close;
    }

    sklen = sizeof(pri->local_addr);
    if (getsockname(tx_fd, (struct sockaddr *)&pri->local_addr, &sklen) < 0)
    {
        err = -errno;
        printk("mcast_open: getsockname() failed: error = %d\n", errno);
        os_close_file(tx_fd);
        tx_fd = -1;
        goto out_close;
    }


    /* Close old file descriptors */
    if (pri->tx_fd >= 0)
    {
        os_close_file(pri->tx_fd);
        pri->tx_fd = -1;
    }

    if (pri->fd >= 0)
    {
	    mreq.imr_multiaddr.s_addr = xsin->sin_addr.s_addr;
	    mreq.imr_interface.s_addr = 0;
	    if (setsockopt(pri->fd, SOL_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
		    printk("mcast_open: IP_DROP_MEMBERSHIP failed, error = %d\n", errno);

	    os_close_file(pri->fd);
    }

    memcpy(xsin, sin, sizeof(struct sockaddr_in));
    pri->tx_fd = tx_fd;
    pri->fd = fd;
	return fd;

 out_close:
	os_close_file(fd);
 out:
	return err;
}

const struct net_user_info mcast_user_info = {
	.init		= mcast_user_init,
	.open		= mcast_open,
	.close	 	= mcast_close,
	.remove	 	= mcast_remove,
	.add_address	= NULL,
	.delete_address = NULL,
	.change_config = mcast_change_config,
	.mtu		= ETH_MAX_PACKET,
	.max_packet	= ETH_MAX_PACKET + ETH_HEADER_OTHER,
};
