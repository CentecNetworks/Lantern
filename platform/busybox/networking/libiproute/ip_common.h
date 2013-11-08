/* vi: set sw=4 ts=4: */
#ifndef _IP_COMMON_H
#define _IP_COMMON_H 1

#include "libbb.h"
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#if !defined IFA_RTA
#include <linux/if_addr.h>
#endif
#if !defined IFLA_RTA
#include <linux/if_link.h>
#endif

#undef RTA_MAX
#undef IFA_MAX
#undef IFLA_MAX
#define RTA_MAX 16
#define IFLA_VRF 18
#define IFLA_MAX IFLA_VRF
#define IFA_MAX 8
#define RTA_VRF 15
#if 0 /*m by zhuj at 2010-11-27 for _NEW_VERSION_LINUX_*/
#define RTM_NEWVRF  68
#define RTM_DELVRF  69
#define RTM_GETVRF  70
#else
#define IFLA_NS_ID 21  
#define RTM_NEWVRF  88
#define RTM_DELVRF  89
#define RTM_GETVRF  90
#endif
struct vrfmsg
{
    unsigned char	   va_family;
	unsigned char	   va_vrf; /* VRF id */
	unsigned char	   vrf_name[16]; 
};

extern void ip_parse_common_args(int *argcp, char ***argvp);
extern int print_neigh(struct sockaddr_nl *who, struct nlmsghdr *n, void *arg);
extern int ipaddr_list_or_flush(int argc, char **argv, int flush);
extern int iproute_monitor(int argc, char **argv);
extern void iplink_usage(void) ATTRIBUTE_NORETURN;
extern void ipneigh_reset_filter(void);

extern int do_ipaddr(int argc, char **argv);
extern int do_iproute(int argc, char **argv);
extern int do_iprule(int argc, char **argv);
extern int do_ipneigh(int argc, char **argv);
extern int do_iptunnel(int argc, char **argv);
extern int do_iplink(int argc, char **argv);
extern int do_ipmonitor(int argc, char **argv);
extern int do_multiaddr(int argc, char **argv);
extern int do_multiroute(int argc, char **argv);
int do_ipvrf(int argc, char **argv);
#endif /* ip_common.h */
