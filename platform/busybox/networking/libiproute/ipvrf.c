/*
 * ipvrf.c		"ip vrf".
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 * Authors:	James R. Leu <jleu@mindspring.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

#include "ip_common.h"  /* #include "libbb.h" is inside */
#include "rt_names.h"
#include "utils.h"

static void usage(void) __attribute__((noreturn));

static void usage(void)
{
	fprintf(stderr, "Usage: ip vrf [ add | remove ] VRF_NUMBER\n");
	fprintf(stderr, "       ip vrf show\n");
	exit(-1);
}

int print_vrf(struct sockaddr_nl *who, struct nlmsghdr *n, void *arg)
{
	FILE *fp = (FILE*)arg;
	int len = n->nlmsg_len;
	struct vrfmsg *r = NLMSG_DATA(n);

	if (n->nlmsg_type != RTM_NEWVRF) {
		fprintf(stderr, "Not a VRF\n");
		return 0;
	}

	len -= NLMSG_LENGTH(sizeof(*r));
	if (len < 0) {
		fprintf(stderr, "Wrong len %d\n", len);
		return -1;
	}

	if (n->nlmsg_type == RTM_NEWVRF)
		fprintf(fp, "vrf %d\n", r->va_vrf);

	fflush(fp);

	return 0;
}

int do_vrf_show(void)
{
	struct rtnl_handle rth;
	struct vrfmsg r;

	memset(&r, 0, sizeof(r));

	r.va_family = PF_INET;

	xrtnl_open(&rth);

	if (rtnl_dump_request(&rth, RTM_GETVRF, &r, sizeof(r)) < 0) {
		perror("Cannot send dump request");
		exit(1);
		}

	if (xrtnl_dump_filter(&rth, print_vrf, stdout) < 0) {
		fprintf(stderr, "Dump terminated\n");
		exit(1);
	}

	return (0);
}

int ipvrf_modify(int cmd, int vrf, char *name)
{
	struct rtnl_handle rth;
	struct {
		struct nlmsghdr         n;
		struct vrfmsg           r;
		} req;

	if(vrf == 0) {
		fprintf(stderr, "Cannot add or remove VRF ID=0\n");
		exit(1);
	}
	memset(&req, 0, sizeof(req));

	req.n.nlmsg_type = cmd;
	req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct vrfmsg));
	req.n.nlmsg_flags = NLM_F_REQUEST;

	req.r.va_family = AF_INET;
	req.r.va_vrf = vrf;
	strcpy(req.r.vrf_name,name);

	if (cmd == RTM_NEWVRF) {
		req.n.nlmsg_flags |= NLM_F_CREATE|NLM_F_EXCL;
	}

	xrtnl_open(&rth);

	if (rtnl_talk(&rth, &req.n, 0, 0, NULL, NULL, NULL) < 0)
		return 2;
	return (0);
}

int do_ipvrf(int argc, char **argv)
{
	int vrf = 0;

	if (argc > 0) {
		if (matches(*argv, "show") == 0)
			return do_vrf_show();
		if (matches(*argv, "help") == 0)
			usage();
		if (matches(*argv, "add") == 0) {
			NEXT_ARG();
			if (get_integer(&vrf, *argv, 0))
				invarg("Invalid \"vrf\" value\n", *argv);
			return ipvrf_modify(RTM_NEWVRF, vrf, argv[1]);
			}
		if (matches(*argv, "remove") == 0 ||
			matches(*argv, "delete") == 0 ||
			matches(*argv, "del") == 0) {
			NEXT_ARG();
			if (get_integer(&vrf, *argv, 0))
				invarg("Invalid \"vrf\" value\n", *argv);
			return ipvrf_modify(RTM_DELVRF, vrf, argv[1]);
			}
	} else
	return do_vrf_show();

	fprintf(stderr, "Command \"%s\" is unknown, try \"ip vrf help\".\n", *argv);
	exit(-1);
}


