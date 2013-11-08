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

#ifndef _GEN_LOG_H
#define _GEN_LOG_H

#include "sal.h"
#ifndef _CTC_OF_
/* #include "ctclib_thread.h" */
#else
#include "ctclib_thread.h"
#endif /* !_CTC_OF_ */

#if 0
#define HAVE_GENLOG_DEBUG
#endif

/*
 * arguments to setlogmask.
 */
#define	LOG_MASK(pri)	(1 << (pri))		/* mask for one priority */
#define	LOG_UPTO(pri)	((1 << ((pri)+1)) - 1)	/* all priorities through pri */


#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERR		3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */


#define	LOG_PRIMASK	0x07	/* mask to extract priority part (internal) */
#define	LOG_PRI(p)	((p) & LOG_PRIMASK) /* extract priority */
#define	LOG_MAKEPRI(fac, pri)	(((fac) << 3) | (pri))


/* facility codes */
#define	LOG_KERN	(0<<3)	/* kernel messages */
#define	LOG_USER	(1<<3)	/* random user-level messages */
#define	LOG_MAIL	(2<<3)	/* mail system */
#define	LOG_DAEMON	(3<<3)	/* system daemons */
#define	LOG_AUTH	(4<<3)	/* security/authorization messages */
#define	LOG_SYSLOG	(5<<3)	/* messages generated internally by syslogd */
#define	LOG_LPR		(6<<3)	/* line printer subsystem */
#define	LOG_NEWS	(7<<3)	/* network news subsystem */
#define	LOG_UUCP	(8<<3)	/* UUCP subsystem */
#define	LOG_CRON	(9<<3)	/* clock daemon */
#define	LOG_AUTHPRIV	(10<<3)	/* security/authorization messages (private) */
#define	LOG_FTP		(11<<3)	/* ftp daemon */

/* other codes through 15 reserved for system use */
#define	LOG_LOCAL0	(16<<3)	/* reserved for local use */
#define	LOG_LOCAL1	(17<<3)	/* reserved for local use */
#define	LOG_LOCAL2	(18<<3)	/* reserved for local use */
#define	LOG_LOCAL3	(19<<3)	/* reserved for local use */
#define	LOG_LOCAL4	(20<<3)	/* reserved for local use */
#define	LOG_LOCAL5	(21<<3)	/* reserved for local use */
#define	LOG_LOCAL6	(22<<3)	/* reserved for local use */
#define	LOG_LOCAL7	(23<<3)	/* reserved for local use */

#define	LOG_NFACILITIES	24	/* current number of facilities */
#define	LOG_FACMASK	0x03f8	/* mask to extract facility part */
#define	LOG_FAC(p)	(((p) & LOG_FACMASK) >> 3) /* facility of pri */

/*
 * Option flags for openlog.
 * LOG_ODELAY no longer does anything.
 * LOG_NDELAY is the inverse of what it used to be.
 */
#define LOG_PID     0x01    /* log the pid with each message */
#define LOG_CONS    0x02    /* log on the console if errors in sending */
#define LOG_ODELAY  0x04    /* delay open until first syslog() (default) */
#define LOG_NDELAY  0x08    /* don't delay open */
#define LOG_NOWAIT  0x10    /* don't wait for console forks: DEPRECATED */
#define LOG_PERROR  0x20    /* log to stderr as well */

#define LOG_FUNC_NAME_MAXLEN  128

/*defination for logging interval*/
#define GLB_MAX_LOGGING_INTERVAL 120

/*
 * Macros for module
 * Notice: please keep the order with gModuleNameArr
 */
typedef enum
{
    M_MOD_UNKNOWN = 0,
    M_MOD_STP, /* For STP protocol */
    M_MOD_MSTP, /* For MSTP protocol */
    M_MOD_VLANCLS, /* For VLAN classification */
    M_MOD_PVLAN, /* For private VLAN */
    M_MOD_SLINKAGG, /* For static link aggregation */
    M_MOD_LACP, /* For LACP protocol */
    M_MOD_FDB, /* For FDB */
    M_MOD_LEARNING, /* For MAC learning */
    M_MOD_AGING, /* For MAC aging */
    M_MOD_QINQ, /* For 802.1ad */
    M_MOD_IGMPSNOOPING, /* For IGMP snooping */
    M_MOD_STORMCTRL, /* For Storm control */
    M_MOD_VLANTRANS, /* For VLAN translation */
    M_MOD_UCAST4, /* For general IPv4 unicast */
    M_MOD_L3SUCAST4, /* For IPv4 L3 static unicast routing */
    M_MOD_PBR4, /* For IPv4 PBR */
    M_MOD_OSPF4, /* For IPv4 OSPF */
    M_MOD_RIP4, /* For IPv4 RIP */
    M_MOD_MCAST4, /* For general IPv4 multicast */
    M_MOD_PIMSM4, /* For IPv4 PIM-SM */
    M_MOD_IGMP, /* For IPv4 IGMP */
    M_MOD_UCAST6, /* For general IPv6 unicast */
    M_MOD_L3SUCAST6, /* For IPv6 L3 static unicast routing */
    M_MOD_PBR6, /* For IPv6 PBR */
    M_MOD_OSPF6, /* For IPv6 OSPF */
    M_MOD_RIPNG6, /* For IPv6 RIPng */
    M_MOD_IPINIP, /* For IP in IP */
    M_MOD_IPV6INV4, /* For IPv6 in IPv4 */
    M_MOD_ISATAP, /* For ISATAP */
    M_MOD_IPV6TOIPV4, /* For IPv6 to IPv4 */
    M_MOD_GRE, /* For GRE */
    M_MOD_ACL, /* For general ACL */
    M_MOD_MACACL, /* For MAC ACL */
    M_MOD_ACL4, /* For IPv4 ACL */
    M_MOD_ACL6, /* For IPv6 ACL */
    M_MOD_L2ACL4, /* For IPv4 L2 ACL */
    M_MOD_L2ACL6, /* For IPv6 L2 ACL */
    M_MOD_L3ACL4, /* For IPv4 L3 ACL */
    M_MOD_L3ACL6, /* For IPv6 L3 ACL */
    M_MOD_ARP, /* For ARP */
    M_MOD_IP4, /* For IPv4 */
    M_MOD_ICMP4, /* For ICMPv4 */
    M_MOD_NDP, /* For IPv6 NDP */
    M_MOD_IP6, /* For IPv6 */
    M_MOD_ICMP6, /* For ICMPv6 */
    M_MOD_MIRR, /* For general mirror */
    M_MOD_INTMIRR, /* For interface mirror */
    M_MOD_FLOWMIRR, /* For flow based mirror */
    M_MOD_REMTMIRR, /* For remote mirror */
    M_MOD_MPLS, /* For general MPLS */
    M_MOD_DPS, /* For MPLS Data Path Stuff */
    M_MOD_LDP, /* For MPLS LDP */
    M_MOD_QOS, /* For general QoS */
    M_MOD_SFLOW, /* For general SFLOW */
    M_MOD_POLICING, /* For traffic policing */
    M_MOD_SCHEDULING, /* For scheduling */
    M_MOD_SHAPING, /* For traffic shaping */
    M_MOD_CONGAVOID, /* For Congestion Avoidance */
    M_MOD_AQOS, /* For advanced QoS */
    M_MOD_HA, /* For general high availability */
    M_MOD_VRRP, /* For VRRP */
    M_MOD_CARP, /* For CARP */
    M_MOD_VPN, /* For general VPN */
    M_MOD_VPLS, /* For VPN VPLS */
    M_MOD_VPWS, /* For VPN VPWS */
    M_MOD_L3VPN, /* For L3 VPN */
    M_MOD_NAT, /* For NAT */
    M_MOD_NAPT, /* For NAPT */
    M_MOD_SECURITY, /* For general security */
    M_MOD_8021X, /* For 802.1x */
    M_MOD_8021XSEC, /* For 802.1x security */
    M_MOD_PORTSEC, /* For Port security */
    M_MOD_VLANSEC, /* For Vlan security */
    M_MOD_DHCPSNOOPING, /* For DHCP snooping */
    M_MOD_ARPINSP, /* For ARP Inspection */
    M_MOD_IPSRCGUARD, /* For IP Source Guard */
    M_MOD_USERADMI, /* For User admin */
    M_MOD_LOG, /* For Log system */
    M_MOD_DHCPRLY, /* For DHCP relay */
    M_MOD_DHCPSRV, /* For DHCP server */
    M_MOD_DHCPCLI, /* For DHCP client */
    M_MOD_AAA, /* For AAA */
    M_MOD_SNMPSRV, /* For SNMP server */
    M_MOD_SNMPCLI, /* For SNMP client */
    M_MOD_SNMPTRAP, /* For SNMP trap */
    M_MOD_SSHCLI, /* For SSH client */
    M_MOD_SSHSRV, /* For SSH server */
    M_MOD_CRYPTO, /* For crypto (rsa etc) */
    M_MOD_TFTPCLI, /* For TFTP client */
    M_MOD_FTPCLI, /* For FTP client */
    M_MOD_TELNETCLI, /* For Telnet client */
    M_MOD_TELNETSRV, /* For Telnet server */
    M_MOD_HTTP, /* For HTTP */
    M_MOD_HTTPS, /* For HTTPS */
    M_MOD_RIB, /*For RIB*/
    M_MOD_IMISH, /* For imish */
    M_MOD_VLAN, /* For VLAN */
    M_MOD_GVRP, /* For GVRP */
    M_MOD_GARP, /* For GARP */
    M_MOD_GMRP, /* For GMRP */
    M_MOD_CFM, /* For CFM */
    M_MOD_EFM, /* For EFM */
    M_MOD_LLDP, /* For LLDP */
    M_MOD_TCAM_ALLOC, /*for STM */
    M_MOD_BRIDGE, /* For BRIDGE */
    M_MOD_ERPS, /* For ERPS */
    M_MOD_CHSM, /*For chsm */
    M_MOD_INF, /* For interface */
    M_MOD_IPSOUR, /* For IP source guard */
    M_MOD_LIB, /* For common lib */
    M_MOD_NSMCLI, /* For nsm client */
    M_MOD_LINE, /* For line */
    M_MOD_CSPF, /* For cspf */
    M_MOD_IMI, /* For imi */
    M_MOD_VIP, /* For Virtual IP */
    M_MOD_IPSEC, /* For IP Security */
    M_MOD_SYSTEM, /*For commom system*/
    M_MOD_MCAST6, /*For IPv6 Multicast*/
    M_MOD_VR, /*For Virtual Router*/
    M_MOD_RMM, /*For Remote Monitor*/
    M_MOD_RMAP, /*For Route Map*/
    M_MOD_BGP,/*For BGP*/
    M_MOD_DVMRP,/*For DVMRP*/
    M_MOD_ISIS,/*For ISIS*/
    M_MOD_NSM,/*For NSM*/
    M_MOD_ONM,/*For ONM*/
    M_MOD_PDM,/*For PIM-DM*/
    M_MOD_PIMSM6,/*For PIMSM6*/
    M_MOD_PIMPKTGEN,/*For PIMPKTGEN*/
    M_MOD_RMON,/*For RMON*/
    M_MOD_RSVP,/*For RSVP*/
    M_MOD_ERRDIS,/*For Error Disable*/
    M_MOD_BHM,/* For BHM*/

    M_MOD_CHSMDRVPHY,
    M_MOD_CHSMDRV1,
    M_MOD_CHSMDRV2,
    M_MOD_CHSMDRVL2,
    M_MOD_CHSMDRVEPLD,
    M_MOD_CHSMDRVLED,
    M_MOD_CHSMDRVSERIAL,
    M_MOD_CHSMDRVMISC,
    M_MOD_CHSMMGFIBER,
    M_MOD_CHSMMGSENSOR,
    M_MOD_CHSMMGLED,
    M_MOD_CHSMMGLCM,
    M_MOD_CHSMMGLCMKEP,
    M_MOD_CHSMHACONSY,
    M_MOD_TRACK,
    M_MOD_IPSLA, /* For ipsla */
    M_MOD_DHCPRLY6, /* For DHCPv6 relay */
    M_MOD_DHCPSNOOPING6, /* For DHCPv6 snooping */
    M_MOD_HAGT,  /* For hagt */
    M_MOD_LCM,   /* For linecard*/
    M_MOD_HSRV, /*For hal server*/
    M_MOD_SDK,   /*For sdk log*/
    M_MOD_HAL,   /*For hal */
    M_MOD_DRV,   /*For driver */
    M_MOD_PTP,   /*For PTP */
    M_MOD_SSM,   /*For SSM */
    M_MOD_LCSH,   /*For PTP */
    M_MOD_SMLK, /*For smart link */
    M_MOD_PETH,  /*For user mode PETH */
    M_MOD_L2PING,/*For L2 ping */
    M_MOD_BFD,
    M_MOD_MONITORLINK,
    M_MOD_UDLD,
    M_MOD_TPOAM,
    M_MOD_PM,
    M_MOD_ALARM,
    M_MOD_VOICEVLAN,
    M_MOD_G8031, /*For G.8031 ethernet protection.*/
    M_MOD_G8032, /* For G.8032 ethernet ring protection */
    M_MOD_G8131,
    M_MOD_DHCLIENT, /* for smartconfig */
    M_MOD_SCFG, /* for smartconfig */
    M_MOD_TIME_RANGE,
    M_MOD_OVS,
    M_MOD_VTYSH,
    M_MOD_MAX /* Max module number */
} E_MODULE;

/* define all module name strings */
#define M_MNAME_UNKNOWN "UNKNOWN"
#define M_MNAME_STP "STP"
#define M_MNAME_MSTP "MSTP"
#define M_MNAME_VLANCLS "VLANCLS"
#define M_MNAME_PVLAN "PVLAN"
#define M_MNAME_SLINKAGG "SLINKAGG"
#define M_MNAME_LACP "LACP"
#define M_MNAME_FDB "FDB"
#define M_MNAME_LEARNING "LEARNING"
#define M_MNAME_AGING "AGING"
#define M_MNAME_QINQ "QINQ"
#define M_MNAME_IGMPSNOOPING "IGMPSNOOPING"
#define M_MNAME_STORMCTRL "STORMCTRL"
#define M_MNAME_VLANTRANS "VLANTRANS"
#define M_MNAME_UCAST4 "UCAST4"
#define M_MNAME_L3SUCAST4 "L3SUCAST4"
#define M_MNAME_PBR4 "PBR4"
#define M_MNAME_OSPF4 "OSPF4"
#define M_MNAME_RIP4 "RIP4"
#define M_MNAME_MCAST4 "MCAST4"
#define M_MNAME_PIMSM4 "PIMSM4"
#define M_MNAME_IGMP "IGMP"
#define M_MNAME_UCAST6 "UCAST6"
#define M_MNAME_L3SUCAST6 "L3SUCAST6"
#define M_MNAME_PBR6 "PBR6"
#define M_MNAME_OSPF6 "OSPF6"
#define M_MNAME_RIPNG6 "RIPNG6"
#define M_MNAME_IPINIP "IPINIP"
#define M_MNAME_IPV6INV4 "IPV6INV4"
#define M_MNAME_ISATAP "ISATAP"
#define M_MNAME_IPV6TOIPV4 "IPV6TOIPV4"
#define M_MNAME_GRE "GRE"
#define M_MNAME_ACL "ACL"
#define M_MNAME_MACACL "MACACL"
#define M_MNAME_ACL4 "ACL4"
#define M_MNAME_ACL6 "ACL6"
#define M_MNAME_L2ACL4 "L2ACL4"
#define M_MNAME_L2ACL6 "L2ACL6"
#define M_MNAME_L3ACL4 "L3ACL4"
#define M_MNAME_L3ACL6 "L3ACL6"
#define M_MNAME_ARP "ARP"
#define M_MNAME_IP4 "IP4"
#define M_MNAME_ICMP4 "ICMP4"
#define M_MNAME_NDP "NDP"
#define M_MNAME_IP6 "IP6"
#define M_MNAME_ICMP6 "ICMP6"
#define M_MNAME_MIRR "MIRR"
#define M_MNAME_INTMIRR "INTMIRR"
#define M_MNAME_FLOWMIRR "FLOWMIRR"
#define M_MNAME_REMTMIRR "REMTMIRR"
#define M_MNAME_MPLS "MPLS"
#define M_MNAME_DPS "DPS"
#define M_MNAME_LDP "LDP"
#define M_MNAME_QOS "QOS"
#define M_MNAME_SFLOW "SFLOW"
#define M_MNAME_POLICING "POLICING"
#define M_MNAME_SCHEDULING "SCHEDULING"
#define M_MNAME_SHAPING "SHAPING"
#define M_MNAME_CONGAVOID "CONGAVOID"
#define M_MNAME_AQOS "AQOS"
#define M_MNAME_HA "HA"
#define M_MNAME_VRRP "VRRP"
#define M_MNAME_CARP "CARP"
#define M_MNAME_VPN "VPN"
#define M_MNAME_VPLS "VPLS"
#define M_MNAME_VPWS "VPWS"
#define M_MNAME_L3VPN "L3VPN"
#define M_MNAME_NAT "NAT"
#define M_MNAME_NAPT "NAPT"
#define M_MNAME_SECURITY "SECURITY"
#define M_MNAME_8021X "8021X"
#define M_MNAME_8021XSEC "8021XSEC"
#define M_MNAME_PORTSEC "PORTSEC"
#define M_MNAME_VLANSEC "VLANSEC"
#define M_MNAME_DHCPSNOOPING "DHCPSNOOPING"
#define M_MNAME_ARPINSP "ARPINSP"
#define M_MNAME_IPSRCGUARD "IPSRCGUARD"
#define M_MNAME_USERADMI "USERADMI"
#define M_MNAME_LOG "LOG"
#define M_MNAME_DHCPRLY "DHCPRLY"
#define M_MNAME_DHCPSRV "DHCPSRV"
#define M_MNAME_DHCPCLI "DHCPCLI"
#define M_MNAME_AAA "AAA"
#define M_MNAME_SNMPSRV "SNMPAGENT"
#define M_MNAME_SNMPCLI "SNMPCLI"
#define M_MNAME_SNMPTRAP "SNMPTRAP"
#define M_MNAME_SSHCLI "SSHCLI"
#define M_MNAME_SSHSRV "SSHSRV"
#define M_MNAME_CRYPTO "CRYPTO"
#define M_MNAME_TFTPCLI "TFTPCLI"
#define M_MNAME_FTPCLI "FTPCLI"
#define M_MNAME_TELNETCLI "TELNETCLI"
#define M_MNAME_TELNETSRV "TELNETSRV"
#define M_MNAME_HTTP "HTTP"
#define M_MNAME_HTTPS "HTTPS"
#define M_MNAME_RIB "RIB"
#define M_MNAME_IMISH "IMISH"
#define M_MNAME_VLAN "VLAN"
#define M_MNAME_GVRP "GVRP"
#define M_MNAME_GARP "GARP"
#define M_MNAME_GMRP "GMRP"
#define M_MNAME_CFM "CFM"
#define M_MNAME_EFM "EFM"
#define M_MNAME_LLDP "LLDP"
#define M_MNAME_TCAM_ALLOC "STM"
#define M_MNAME_BRIDGE "BRIDGE"
#define M_MNAME_ERPS "ERPS"
#define M_MNAME_CHSM "CHSM"
#define M_MNAME_INF "INTERFACE"
#define M_MNAME_IPSOUR "IPSOURCEGUARD"
#define M_MNAME_LIB "LIBRARY"
#define M_MNAME_NSMCLI "NSMCLIENT"
#define M_MNAME_LINE "LINE"
#define M_MNAME_CSPF "CSPF"
#define M_MNAME_IMI "IMI"
#define M_MNAME_VIP "VIP"
#define M_MNAME_IPSEC "IPSEC"
#define M_MNAME_SYSTEM "SYSTEM"
#define M_MNAME_MCAST6 "IPv6 Multicast"
#define M_MNAME_VR "Virtual Router"
#define M_MNAME_RMM "Remote Monitor"
#define M_MNAME_RMAP "Route Map"
#define M_MNAME_BGP "BGP"
#define M_MNAME_DVMRP "DVMRP"
#define M_MNAME_ISIS "ISIS"
#define M_MNAME_NSM "NSM"
#define M_MNAME_ONM "ONM"
#define M_MNAME_PDM "PIM-DM"
#define M_MNAME_PIMSM6 "PIMSM6"
#define M_MNAME_PIMPKTGEN "PIMPKTGEN"
#define M_MNAME_RMON "RMON"
#define M_MNAME_RSVP "RSVP"
#define M_MNAME_ERRDIS "ErrDisable"
#define M_MNAME_BHM "BHM"


#define M_MNAME_CHSMDRVPHY "CHSMDRVPHY"
#define M_MNAME_CHSMDRV1 "CHSMDRV1"
#define M_MNAME_CHSMDRV2 "CHSMDRV2"
#define M_MNAME_CHSMDRVL2 "CHSMDRVL2"
#define M_MNAME_CHSMDRVEPLD "CHSMDRVEPLD"
#define M_MNAME_CHSMDRVLED "CHSMDRVLED"
#define M_MNAME_CHSMDRVSERIAL "CHSMDRVSERIAL"
#define M_MNAME_CHSMDRVMISC "CHSMDRVMISC"
#define M_MNAME_CHSMMGFIBER "CHSMMGFIBER"
#define M_MNAME_CHSMMGSENSOR "CHSMMGSENSOR"
#define M_MNAME_CHSMMGLED "CHSMMGLED"
#define M_MNAME_CHSMMGLCM "CHSMMGLCM"
#define M_MNAME_CHSMMGLCMKEP "CHSMMGLCMKEP"
#define M_MNAME_CHSMHACONSY "CHSMHACONSY"

#define M_MNAME_TRACK "TRACK"  /*for track*/
#define M_MNAME_IPSLA "IPSLA"  /*for ipsla*/
#define M_MNAME_DHCPRLY6 "DHCV6RLY"
#define M_MNAME_DHCPSNOOPING6 "DHCV6SNP"
#define M_MNAME_HAGT "HalAgent"  /* For hagt */
#define M_MNAME_LCM "LCM"   /* For linecard */
#define M_MNAME_HSRV "HalServer"/*For hal server*/
#define M_MNAME_SDK   "SDK"/*For sdk log*/
#define M_MNAME_HAL   "HalTalk"/*For Hal talk*/
#define M_MNAME_DRV   "Driver"/*For Driver */
#define M_MNAME_PTP   "PTP" /*For PTP */

#define M_MNAME_SSM   "SSM"
#define M_MANME_SMLK "SmartLink"/*For smart link*/
#define M_MNAME_LCSH   "LCSH" /*For lcsh */
#define M_MNAME_PETH  "PETH"  /*For user mode PETH*/
#define M_MNAME_L2PING  "L2PING"  /*For L2 ping*/
#define M_MNAME_BFD   "OAMD"
#define M_MNAME_MONITORLINK   "Monitor Link"
#define M_MNAME_UDLD  "UDLD"
#define M_MNAME_TPOAM   "TPOAM"
#define M_MNAME_PM  "Performance Monitor"
#define M_MNAME_ALARM  "Alarm"
#define M_MNAME_DHCLIENT  "DHCLIENT"
#define M_MNAME_SCFG  "SCFGD"
#define M_MNAME_VOICEVLAN  "VoiceVlan"
#define M_MNAME_G8031  "G8031"
#define M_MNAME_G8032  "G8032"
#define M_MNAME_G8131  "G8131"
#define M_MNAME_TIME_RANGE  "TimeRange"
#define M_MNAME_OVS "ovs-vswitchd"
#define M_MNAME_VTYSH "vtysh"

#define M_MNAME_MAX "UNKNOWN"

typedef enum
{
  M_LOG_FREQ_INTVAL_1 = 1,/*1 seconds*/
  M_LOG_FREQ_INTVAL_2 = 30,/*30 seconds*/
  M_LOG_FREQ_INTVAL_3 = 1800,/*30 Minutes*/
  M_LOG_FREQ_INTVAL_4 = 3600,/*1 hours*/
}E_LOG_RATE_LIMIT_INTVAL;

struct log_message_rate_limit_s
{
  uint16 log_type;
  uint16 log_interval;
  int next_sec;
  int suppress_cnt;
};
typedef struct log_message_rate_limit_s log_message_rate_limit_t;

/* alarm log prefix */
#define ALRM_LOG_PREFIX "-~ALRM~-"

/* module name pair struct */
typedef struct
{
    E_MODULE eNo; /* module no */
    const char *pszName; /* name */
} S_MODULE_NAME_PAIR;

/* Message structure. */
struct log_message_s
{
  int key;
  char *str;
};
typedef struct log_message_s log_message_t;
#define LOG_MSG_SIZE sizeof(log_message_t)

/*
 * Severity type
 */
typedef enum
{
    E_EMERGENCY = 0,
    E_ALERT = 1,
    E_CRITICAL = 2,
    E_ERROR = 3,
    E_WARNING = 4,
    E_NOTICE = 5,
    E_INFORMATIONAL = 6,
    E_DEBUG = 7,
    E_MAX
} E_SEVERITY;

/*
 * Alarm level type
 */
typedef enum
{
    HIGH = 0,
    MIDDEN= 1,
    LOWER = 2,
    MINOR = 3,
} E_ALRM_LEVEL;

/*
 * Operating type
 */
typedef enum
{
    OPRT_SUCCESS = 0,
    OPRT_FAIL = 1,
} E_OPRT_RLT;

/*
 * Alarm log trapping type
 */
typedef enum
{
    ALRM_TRAP_DISABLE=0,
    ALRM_TRAP_ENABLE
} E_TRAP_FLAG;

typedef enum
{
    LOG_RATE_LIMIT_TYPE_DEF = 0, /*no limit*/
    LOG_RATE_LIMIT_TYPE_LIB_EXC_MEM_THRESHOLD,
    LOG_RATE_LIMIT_TYPE_ERPS_1,
    LOG_RATE_LIMIT_TYPE_ERPS_2,
    LOG_RATE_LIMIT_TYPE_IPV4,
    LOG_RATE_LIMIT_TYPE_IPV6,
    LOG_RATE_LIMIT_TYPE_NDP,
    LOG_RATE_LIMIT_TYPE_OSPF_MEMCHECK_1,
    LOG_RATE_LIMIT_TYPE_OSPF_MEMCHECK_2,
    LOG_RATE_LIMIT_TYPE_OSPF_ROUTECHECK_1,
    LOG_RATE_LIMIT_TYPE_OSPF_ROUTECHECK_2,
    LOG_RATE_LIMIT_TYPE_OSPF_IGNORE_STATE_2,
    LOG_RATE_LIMIT_TYPE_ARP_NO_BUF_SPACE,
    LOG_RATE_LIMIT_TYPE_ARP_DUP_IP_ADDR,
    LOG_RATE_LIMIT_TYPE_ARP_DISCARD_PKT,
    LOG_RATE_LIMIT_TYPE_L2MCAST_RESOURCE_FULL,
    LOG_RATE_LIMIT_TYPE_IGMP_GROUP_FULL,
    LOG_RATE_LIMIT_TYPE_MRT_FULL,
    LOG_RATE_LIMIT_TYPE_IGMP_DECODE_WARN,
    LOG_RATE_LIMIT_TYPE_DHCP_SERVER_GROUP,
    LOG_RATE_LIMIT_TYPE_DHCP_SNOOPING_IPSG_LIMIT,
    LOG_RATE_LIMIT_TYPE_MAX
} nsm_log_rate_limit_t;

typedef unsigned int  CMD_LEVEL;

/* Open&Close the log module for communication with syslog-ng daemon */
extern void genlog_open(void);
extern void genlog_close(void);

/* Add one system log */
extern void log_sys(E_MODULE eModule, E_SEVERITY eSeverity, const char *fmt, ...);

/* Add one operating log */
extern void log_oper(E_MODULE eModule, E_SEVERITY eSeverity, CMD_LEVEL cl, E_OPRT_RLT rlt, const char *fmt, ...);

/* Add one device user log */
extern void log_diag(E_MODULE eModule, E_SEVERITY eSeverity, const char *fmt, ...);

/* Add one device alarm log */
extern void log_alarm(E_MODULE eModule, E_ALRM_LEVEL eSeverity, const char *fmt, ...);

/* Get the access client ip address and user name */
extern void log_init_line(const char * ip_addr, const char * usr_name);

extern void log_set_nonblocking(int set);

extern void log_sys_rate_limit(E_MODULE eModule, E_SEVERITY eSeverity, uint16 eFreq, const char *fmt, ...);

extern void log_diag_rate_limit(E_MODULE eModule, E_SEVERITY eSeverity, uint16 rateLimitType, const char *fmt, ...);

extern log_message_rate_limit_t *log_get_rate_limit_arr(void);
#endif  /* !_GEN_LOG_H */
