
/****************************************************************************
* $Id$
*  kernel message general log api implementation
*
* Copyright (C) 2011 Centec Networks Inc.  All rights reserved.
*
* Modify History:
* Revision      : R0.01
* Author        : zhoucheng
* Date          : 2007-11-09 14:12
* Reason        : First Create.
****************************************************************************/


/*******************************************************************************
 *
 * Header files
 *
 ******************************************************************************/

#include "kal.h"
#include "genlog.h"

/*******************************************************************************
 * Name:    klog_format
 * Purpose: format message
 * Input:
 *   eModule: module id
 *   eSeverity: security level
 *   alrm_log: is this a alarm log?
 *   fmt: message content
 * Output:
 * Return:
 * Note:
 ******************************************************************************/
    /*
     * module name pair
     * notice: please keep the order with E_MODULE!!
     */
   static  S_MODULE_NAME_PAIR gModuleNameArr[] =
    {
        {M_MOD_UNKNOWN, M_MNAME_UNKNOWN},
        {M_MOD_STP, M_MNAME_STP},
        {M_MOD_MSTP, M_MNAME_MSTP},
        {M_MOD_VLANCLS, M_MNAME_VLANCLS},
        {M_MOD_PVLAN, M_MNAME_PVLAN},
        {M_MOD_SLINKAGG, M_MNAME_SLINKAGG},
        {M_MOD_LACP, M_MNAME_LACP},
        {M_MOD_FDB, M_MNAME_FDB},
        {M_MOD_LEARNING, M_MNAME_LEARNING},
        {M_MOD_AGING, M_MNAME_AGING},
        {M_MOD_QINQ, M_MNAME_QINQ},
        {M_MOD_IGMPSNOOPING, M_MNAME_IGMPSNOOPING},
        {M_MOD_STORMCTRL, M_MNAME_STORMCTRL},
        {M_MOD_VLANTRANS, M_MNAME_VLANTRANS},
        {M_MOD_UCAST4, M_MNAME_UCAST4},
        {M_MOD_L3SUCAST4, M_MNAME_L3SUCAST4},
        {M_MOD_PBR4, M_MNAME_PBR4},
        {M_MOD_OSPF4, M_MNAME_OSPF4},
        {M_MOD_RIP4, M_MNAME_RIP4},
        {M_MOD_MCAST4, M_MNAME_MCAST4},
        {M_MOD_PIMSM4, M_MNAME_PIMSM4},
        {M_MOD_IGMP4, M_MNAME_IGMP4},
        {M_MOD_UCAST6, M_MNAME_UCAST6},
        {M_MOD_L3SUCAST6, M_MNAME_L3SUCAST6},
        {M_MOD_PBR6, M_MNAME_PBR6},
        {M_MOD_OSPF6, M_MNAME_OSPF6},
        {M_MOD_RIPNG6, M_MNAME_RIPNG6},
        {M_MOD_IPINIP, M_MNAME_IPINIP},
        {M_MOD_IPV6INV4, M_MNAME_IPV6INV4},
        {M_MOD_ISATAP, M_MNAME_ISATAP},
        {M_MOD_IPV6TOIPV4, M_MNAME_IPV6TOIPV4},
        {M_MOD_GRE, M_MNAME_GRE},
        {M_MOD_ACL, M_MNAME_ACL},
        {M_MOD_MACACL, M_MNAME_MACACL},
        {M_MOD_ACL4, M_MNAME_ACL4},
        {M_MOD_ACL6, M_MNAME_ACL6},
        {M_MOD_L2ACL4, M_MNAME_L2ACL4},
        {M_MOD_L2ACL6, M_MNAME_L2ACL6},
        {M_MOD_L3ACL4, M_MNAME_L3ACL4},
        {M_MOD_L3ACL6, M_MNAME_L3ACL6},
        {M_MOD_ARP, M_MNAME_ARP},
        {M_MOD_IP4, M_MNAME_IP4},
        {M_MOD_ICMP4, M_MNAME_ICMP4},
        {M_MOD_NDP, M_MNAME_NDP},
        {M_MOD_IP6, M_MNAME_IP6},
        {M_MOD_ICMP6, M_MNAME_ICMP6},
        {M_MOD_MIRR, M_MNAME_MIRR},
        {M_MOD_INTMIRR, M_MNAME_INTMIRR},
        {M_MOD_FLOWMIRR, M_MNAME_FLOWMIRR},
        {M_MOD_REMTMIRR, M_MNAME_REMTMIRR},
        {M_MOD_MPLS, M_MNAME_MPLS},
        {M_MOD_DPS, M_MNAME_DPS},
        {M_MOD_LDP, M_MNAME_LDP},
        {M_MOD_QOS, M_MNAME_QOS},
        {M_MOD_POLICING, M_MNAME_POLICING},
        {M_MOD_SCHEDULING, M_MNAME_SCHEDULING},
        {M_MOD_SHAPING, M_MNAME_SHAPING},
        {M_MOD_CONGAVOID, M_MNAME_CONGAVOID},
        {M_MOD_AQOS, M_MNAME_AQOS},
        {M_MOD_HA, M_MNAME_HA},
        {M_MOD_VRRP, M_MNAME_VRRP},
        {M_MOD_CARP, M_MNAME_CARP},
        {M_MOD_VPN, M_MNAME_VPN},
        {M_MOD_VPLS, M_MNAME_VPLS},
        {M_MOD_VPWS, M_MNAME_VPWS},
        {M_MOD_L3VPN, M_MNAME_L3VPN},
        {M_MOD_NAT, M_MNAME_NAT},
        {M_MOD_NAPT, M_MNAME_NAPT},
        {M_MOD_SECURITY, M_MNAME_SECURITY},
        {M_MOD_8021X, M_MNAME_8021X},
        {M_MOD_8021XSEC, M_MNAME_8021XSEC},
        {M_MOD_PORTSEC, M_MNAME_PORTSEC},
        {M_MOD_DHCPSNOOPING, M_MNAME_DHCPSNOOPING},
        {M_MOD_ARPSNOOPING, M_MNAME_ARPSNOOPING},
        {M_MOD_IPSRCGUARD, M_MNAME_IPSRCGUARD},
        {M_MOD_USERADMI, M_MNAME_USERADMI},
        {M_MOD_LOG, M_MNAME_LOG},
        {M_MOD_DHCPRLY, M_MNAME_DHCPRLY},
        {M_MOD_DHCPSRV, M_MNAME_DHCPSRV},
        {M_MOD_DHCPCLI, M_MNAME_DHCPCLI},
        {M_MOD_AAA, M_MNAME_AAA},
        {M_MOD_SNMPSRV, M_MNAME_SNMPSRV},
        {M_MOD_SNMPCLI, M_MNAME_SNMPCLI},
        {M_MOD_SNMPTRAP, M_MNAME_SNMPTRAP},
        {M_MOD_SSHCLI, M_MNAME_SSHCLI},
        {M_MOD_SSHSRV, M_MNAME_SSHSRV},
        {M_MOD_TFTPCLI, M_MNAME_TFTPCLI},
        {M_MOD_FTPCLI, M_MNAME_FTPCLI},
        {M_MOD_TELNETCLI, M_MNAME_TELNETCLI},
        {M_MOD_TELNETSRV, M_MNAME_TELNETSRV},
        {M_MOD_HTTP, M_MNAME_HTTP},
        {M_MOD_HTTPS, M_MNAME_HTTPS},
        {M_MOD_MAX, M_MNAME_MAX}
    };


static void
klog_format(E_MODULE eModule, E_SEVERITY eSeverity, char alrm_log, const char *fmt)
{
    char *p;
    int cnt;
    int tbuf_left, prlen = 0;
    int pri = (int) eSeverity;
    const char * log_tag = NULL;

#define	TBUF_LEN	1024
    char tbuf[TBUF_LEN] = { 0 };

    if (!(eModule > M_MOD_UNKNOWN && eModule < M_MOD_MAX &&
          eSeverity >= E_EMERGENCY && eSeverity <= E_DEBUG) || NULL == fmt)
    {
        return;
    }

#define	INTERNALLOG	LOG_ERR|LOG_CONS|LOG_PERROR|LOG_PID
    /* Check for invalid bits. */
    if (pri & ~(LOG_PRIMASK | LOG_FACMASK))
    {
        pri &= LOG_PRIMASK | LOG_FACMASK;
    }
    if (!LOG_MASK(LOG_PRI(pri)))
    {
        return;
    }

    p = tbuf;
    tbuf_left = TBUF_LEN;

#define	DEC()	\
     { \
         if (prlen < 0)          \
         { \
             prlen = 0;      \
         } \
         if (prlen >= tbuf_left)     \
         { \
             prlen = tbuf_left - 1;  \
         } \
         p += prlen;         \
         tbuf_left -= prlen;    \
     }

    prlen = snprintf(p, tbuf_left, "<%d>", pri);
    DEC();

    log_tag = (const char *) gModuleNameArr[eModule].pszName;

   if (log_tag != NULL )
    {
        prlen = snprintf(p, tbuf_left, "%s: ", log_tag);
        DEC();
    }

    if (strlen(fmt) > 0)
    {
        prlen = snprintf(p, tbuf_left, fmt);
        DEC();
    }

    /* add alarm log prefix */
    if (1 == alrm_log)
    {
        prlen = snprintf(p, tbuf_left, ALRM_LOG_PREFIX);
        DEC();
    }

    prlen = snprintf(p, tbuf_left, "\n");
    DEC();

    cnt = p - tbuf;
    printk(tbuf);
}

/*******************************************************************************
 * Name:    log_sys
 * Purpose: add one system log
 * Input:
 *   eModule: module id
 *   eSeverity: severity level
 *   fmt: message content
 * Output:
 * Return: N/A
 * Note: log kernel message
 ******************************************************************************/
void
log_sys(E_MODULE eModule, E_SEVERITY eSeverity, const char *fmt, ...)
{
    va_list ap;
    char buf[0x100] = {'\0'};

    va_start(ap, fmt);
    vsnprintf(buf, 0xFF, fmt, ap);
    va_end(ap);

    klog_format(eModule, eSeverity, 0, buf);
}

/*******************************************************************************
 * Name:    log_sys
 * Purpose: add one system log
 * Input:
 *   eModule: module id
 *   eSeverity: severity level
 *   fmt: message content
 * Output:
 * Return: N/A
 * Note: log kernel message
 ******************************************************************************/
void
log_alrm(E_MODULE eModule, E_SEVERITY eSeverity, const char *fmt, ...)
{
    va_list ap;
    char buf[0x100] = {'\0'};

    va_start(ap, fmt);
    vsnprintf(buf, 0xFF, fmt, ap);
    va_end(ap);

    klog_format(eModule, eSeverity, 1, buf);
}
