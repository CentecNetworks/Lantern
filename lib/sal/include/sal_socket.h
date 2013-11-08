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

#ifndef __SAL_SOCKET_H__
#define __SAL_SOCKET_H__


#define SAL_SOCK_ERROR ((sal_sock_handle_t)-1)

/* The socket handle type. */
typedef int sal_sock_handle_t;

/*
  This is the type used to specify socket flags.  It must be compatible with
  the flags specified above (and it must also be an integer class so the flags
  given above can be ORed together when appropriate).
*/
typedef unsigned short int sal_sock_flags_t;

/*
  Length of things in socket structures.
*/
typedef size_t sal_sock_len_t;

/*
  IPNET specific defines.
*/
#ifndef HAVE_IPNET

/*
** The following constants should be used for the second parameter of `shutdown'
*/
#define SAL_SHUT_RD	SHUT_RD /* No more receptions.  */
#define SAL_SHUT_WR	SHUT_WR /* No more transmissions.  */
#define SAL_SHUT_RDWR	SHUT_RDWR /* No more receptions or transmissions.  */

/*
  Interface flags.  Any unsupported ones must be set to zero.
*/
#define IFF_OACTIVE	0x00000000
#define IFF_SIMPLEX	0x00000000
#define IFF_LINK0	0x00000000
#define IFF_LINK1	0x00000000
#define IFF_LINK2	0x00000000

/*
  The socket address structure and its parts.
*/
#define sal_sockaddr sockaddr

/*
  The socket address structure for IPv4
*/
#define sal_sockaddr_in4 sockaddr_in

/*
  The socket address structure for IPv6
*/
#ifdef HAVE_IPV6
#define sal_sockaddr_in6 sockaddr_in6
#define sal_ipv6_mreq ipv6_mreq
#endif /* HAVE_IPV6 */

/*
  IP definitions
*/
#define SAL_IPPROTO_IP                  IPPROTO_IP
#define SAL_IPPROTO_RAW                 IPPROTO_RAW
#define SAL_IPPROTO_IPIP                IPPROTO_IPIP
#define SAL_IPPROTO_GRE                 IPPROTO_GRE

#ifdef HAVE_IPV6
#define SAL_IPPROTO_IPV6                IPPROTO_IPV6
#endif /* HAVE_IPV6 */

#define SAL_IP_DROP_MEMBERSHIP          IP_DROP_MEMBERSHIP
#define SAL_IP_ADD_MEMBERSHIP           IP_ADD_MEMBERSHIP
#define SAL_IP_TOS                      IP_TOS
#define SAL_IP_RECVIF                   IP_RECVIF
#define SAL_IP_PKTINFO                  IP_PKTINFO
#define SAL_IP_HDRINCL                  IP_HDRINCL
#define SAL_IP_TTL                      IP_TTL
#define SAL_IP_MULTICAST_LOOP           IP_MULTICAST_LOOP
#define SAL_IP_MULTICAST_IF             IP_MULTICAST_IF
#define SAL_IP_MULTICAST_TTL            IP_MULTICAST_TTL
#define SAL_IP_RECVDSTADDR              IP_RECVDSTADDR
#define SAL_IP_ROUTER_ALERT             IP_ROUTER_ALERT

#define SAL_MRT_INIT                    MRT_INIT
#define SAL_MRT_DONE                    MRT_DONE
#define SAL_MRT_PIM                     MRT_PIM
#define SAL_MRT_ASSERT                  MRT_ASSERT
#define SAL_MRT_ADD_MFC                 MRT_ADD_MFC
#define SAL_MRT_DEL_MFC                 MRT_DEL_MFC
#define SAL_MRT_ADD_VIF                 MRT_ADD_VIF
#define SAL_MRT_DEL_VIF                 MRT_DEL_VIF

#ifdef HAVE_IPV6
#define SAL_IPV6_CHECKSUM               IPV6_CHECKSUM
#define SAL_IPV6_PKTINFO                IPV6_PKTINFO
#define SAL_IPV6_RECVPKTINFO            IPV6_RECVPKTINFO
#define SAL_IPV6_HOPOPTS                IPV6_HOPOPTS
#define SAL_IPV6_MULTICAST_HOPS         IPV6_MULTICAST_HOPS
#define SAL_IPV6_UNICAST_HOPS           IPV6_UNICAST_HOPS
#define SAL_IPV6_RECVHOPLIMIT           IPV6_RECVHOPLIMIT
#define SAL_IPV6_HOPLIMIT               IPV6_HOPLIMIT
#define SAL_IPV6_MULTICAST_LOOP         IPV6_MULTICAST_LOOP
#define SAL_IPV6_MULTICAST_IF           IPV6_MULTICAST_IF
#ifdef IPV6_JOIN_GROUP
#define SAL_IPV6_JOIN_GROUP             IPV6_JOIN_GROUP
#else
#define SAL_IPV6_ADD_MEMBERSHIP         IPV6_ADD_MEMBERSHIP
#endif /* ! IPV6_JOIN_GROUP */
#ifdef IPV6_LEAVE_GROUP
#define SAL_IPV6_LEAVE_GROUP            IPV6_LEAVE_GROUP
#else
#define SAL_IPV6_DROP_MEMBERSHIP        IPV6_DROP_MEMBERSHIP
#endif /* ! IPV6_LEAVE_GROUP */

#define SAL_IP_MRT6_INIT                0 /* Undefined for Linux native. */
#define SAL_IP_MRT6_DONE                0 /* Undefined for Linux native. */
#define SAL_IP_MRT6_PIM                 0 /* Undefined for Linux native. */
#define SAL_IP_MRT6_ADD_MFC             0 /* Undefined for Linux native. */
#define SAL_IP_MRT6_DEL_MFC             0 /* Undefined for Linux native. */
#define SAL_IP_MRT6_ADD_MIF             0 /* Undefined for Linux native. */
#define SAL_IP_MRT6_DEL_MIF             0 /* Undefined for Linux native. */

#define SAL_IPPROTO_ICMPV6              IPPROTO_ICMPV6

#endif /* HAVE_IPV6 */

/*
  IPv4 Packet information
*/
#define sal_in4_pktinfo in_pktinfo

/*
  IPv6 Packet information
*/
#ifdef HAVE_IPV6
#define sal_in6_pktinfo in6_pktinfo
#endif /* HAVE_IPV6 */

#ifdef HAVE_IPV6
/*
** ICMPv6 filter
*/
#define sal_icmp6_filter	icmp6_filter

/*
  ICMPv6 filter set options
*/
#define SAL_ICMP6_FILTER             ICMP6_FILTER
#define SAL_ICMP6_FILTER_SETBLOCKALL ICMP6_FILTER_SETBLOCKALL
#define SAL_ICMP6_FILTER_SETPASS ICMP6_FILTER_SETPASS

#endif /* HAVE_IPV6 */

/*
  The socket address for <dl>?
*/
#define sal_sockaddr_dl sockaddr_dl

/*
  The socket address structure for a UNIX socket
*/
#define sal_sockaddr_un sockaddr_un

/*
  The link-layer socket structure
*/
#define sal_sockaddr_ll sockaddr_ll

/*
  IP Multicast request.
*/
#define sal_ip_mreqn ip_mreqn

/*
  Select structures.  This will probably vary somewhat if the system does not
  use integers as socket handles.

  Linux : Okay, so it cheats a little and touches stuff within the handle
  from outside the sal_socket module, but it hides the effort.
*/
#define sal_sock_set_t                  fd_set
#define SAL_SOCKSET_SIZE                FD_SETSIZE
#define SAL_SOCK_HANDLESET_ISSET(h,s)   FD_ISSET(h,s)
#define SAL_SOCK_HANDLESET_SET(h,s)     FD_SET(h,s)
#define SAL_SOCK_HANDLESET_CLR(h,s)     FD_CLR(h,s)

/*
  I/O vector for scatter/gather operations
*/
#define sal_iovec iovec

/*
  Message header structure
*/
#define sal_msghdr msghdr
#define SAL_MSG_PEEK                    MSG_PEEK
#define SAL_MSG_DONTROUTE               MSG_DONTROUTE

/*
 POSIX 1003.1g - ancillary data object information
*/
#define sal_cmsghdr	cmsghdr

/* Addrinfo structure.  */
#define sal_addrinfo addrinfo

/* Servent structure.  */
#define sal_servent servent

/* Hostent structure.  */
#define sal_hostent hostent

#else  /* HAVE_IPNET */

/*
** The following constants should be used for the second parameter of `shutdown'
*/
#define SAL_SHUT_RD	IP_SHUT_RD /* No more receptions.  */
#define SAL_SHUT_WR	IP_SHUT_WR /* No more transmissions.  */
#define SAL_SHUT_RDWR	IP_SHUT_RDWR /* No more receptions or transmissions.  */

/*
  Interface flags.  Any unsupported ones must be set to zero.
*/
#define IFF_OACTIVE	0x00000000
#define IFF_SIMPLEX	0x00000000
#define IFF_LINK0	0x00000000
#define IFF_LINK1	0x00000000
#define IFF_LINK2	0x00000000

/*
  The socket address structure and its parts.
*/
#define sal_sockaddr Ip_sockaddr

/*
  The socket address structure for IPv4
*/
#define sal_sockaddr_in4 Ip_sockaddr_in

/*
  The socket address structure for IPv6
*/
#ifdef HAVE_IPV6
#define sal_sockaddr_in6 Ip_sockaddr_in6
#define sal_ipv6_mreq        Ip_ipv6_mreq
#endif /* HAVE_IPV6 */

/*
  IP definitions
*/
#define SAL_IPPROTO_IP                  IP_IPPROTO_IP
#define SAL_IPPROTO_RAW                 IP_IPPROTO_RAW
#define SAL_IPPROTO_IPIP                IP_IPPROTO_IPIP
#define SAL_IPPROTO_GRE                 IP_IPPROTO_GRE
#ifdef HAVE_IPV6
#define SAL_IPPROTO_IPV6                IP_IPPROTO_IPV6
#endif /* HAVE_IPV6 */

#define SAL_IP_DROP_MEMBERSHIP          IP_IP_DROP_MEMBERSHIP
#define SAL_IP_ADD_MEMBERSHIP           IP_IP_ADD_MEMBERSHIP
#define SAL_IP_TOS                      IP_IP_TOS
#define SAL_IP_RECVIF                   IP_IP_RECVIF
#define SAL_IP_PKTINFO                  IP_IP_PKTINFO
#define SAL_IP_HDRINCL                  IP_IP_HDRINCL
#define SAL_IP_TTL                      IP_IP_TTL
#define SAL_IP_MULTICAST_LOOP           IP_IP_MULTICAST_LOOP
#define SAL_IP_MULTICAST_IF             IP_IP_MULTICAST_IF
#define SAL_IP_MULTICAST_TTL            IP_IP_MULTICAST_TTL
#define SAL_IP_RECVDSTADDR              IP_IP_RECVDSTADDR
#define SAL_IP_ROUTER_ALERT             IP_IP_ROUTER_ALERT

#define SAL_MRT_INIT                    IP_MRT_INIT
#define SAL_MRT_DONE                    IP_MRT_DONE
#define SAL_MRT_PIM                     IP_MRT_PIM
#define SAL_MRT_ASSERT                  IP_MRT_ASSERT
#define SAL_MRT_ADD_MFC                 IP_MRT_ADD_MFC
#define SAL_MRT_DEL_MFC                 IP_MRT_DEL_MFC
#define SAL_MRT_ADD_VIF                 IP_MRT_ADD_VIF
#define SAL_MRT_DEL_VIF                 IP_MRT_DEL_VIF

#ifdef HAVE_IPV6
#define SAL_IPV6_CHECKSUM               IP_IPV6_CHECKSUM
#define SAL_IPV6_PKTINFO                IP_IPV6_RECVPKTINFO
#define SAL_IPV6_RECVPKTINFO            IP_IPV6_RECVPKTINFO
#define SAL_IPV6_HOPOPTS                IP_IPV6_HOPOPTS
#define SAL_IPV6_MULTICAST_HOPS         IP_IPV6_MULTICAST_HOPS
#define SAL_IPV6_UNICAST_HOPS           IP_IPV6_UNICAST_HOPS
#define SAL_IPV6_RECVHOPLIMIT           IP_IPV6_RECVHOPLIMIT
#define SAL_IPV6_HOPLIMIT               IP_IPV6_HOPLIMIT
#define SAL_IPV6_MULTICAST_LOOP         IP_IPV6_MULTICAST_LOOP
#define SAL_IPV6_MULTICAST_IF           IP_IPV6_MULTICAST_IF
#ifdef IPV6_JOIN_GROUP
#define SAL_IPV6_JOIN_GROUP             IP_IPV6_JOIN_GROUP
#else
#define SAL_IPV6_ADD_MEMBERSHIP         IP_IPV6_ADD_MEMBERSHIP
#endif /* ! IPV6_JOIN_GROUP */
#ifdef IPV6_LEAVE_GROUP
#define SAL_IPV6_LEAVE_GROUP            IP_IPV6_LEAVE_GROUP
#else
#define SAL_IPV6_DROP_MEMBERSHIP        IP_IPV6_DROP_MEMBERSHIP
#endif /* ! IPV6_LEAVE_GROUP */

#define SAL_IP_MRT6_INIT                IP_MRT6_INIT
#define SAL_IP_MRT6_DONE                IP_MRT6_DONE
#define SAL_IP_MRT6_PIM                 IP_MRT6_PIM
#define SAL_IP_MRT6_ADD_MFC             IP_MRT6_ADD_MFC
#define SAL_IP_MRT6_DEL_MFC             IP_MRT6_DEL_MFC
#define SAL_IP_MRT6_ADD_MIF             IP_MRT6_ADD_MIF
#define SAL_IP_MRT6_DEL_MIF             IP_MRT6_DEL_MIF

#define SAL_IPPROTO_ICMPV6              IP_IPPROTO_ICMPV6

#endif /* HAVE_IPV6 */

/*
  IPv4 Packet information
*/
#define sal_in4_pktinfo in_pktinfo

/*
  IPv6 Packet information
*/
#ifdef HAVE_IPV6
#define sal_in6_pktinfo Ip_in6_pktinfo
#endif /* HAVE_IPV6 */

#ifdef HAVE_IPV6
/*
** ICMPv6 filter
*/
#define sal_icmp6_filter	Ip_icmp6_filter

/*
  ICMPv6 filter set options
*/
#define SAL_ICMP6_FILTER             IP_ICMP6_FILTER
#define SAL_ICMP6_FILTER_SETBLOCKALL(filterp)           \
  memset ((filterp), 0, sizeof(struct Ip_icmp6_filter))
#define SAL_ICMP6_FILTER_SETPASS     IP_ICMP6_FILTER_SETPASS

#endif /* HAVE_IPV6 */

/*
  The socket address for <dl>?
*/
#define sal_sockaddr_dl sockaddr_dl

/*
  The socket address structure for a UNIX socket
*/
#define sal_sockaddr_un sockaddr_un

/*
  The link-layer socket structure
*/
#define sal_sockaddr_ll sockaddr_ll

/*
  IP Multicast request.
*/
#define sal_ip_mreqn Ip_ip_mreqn

/*
  Select structures.  This will probably vary somewhat if the system does not
  use integers as socket handles.

  Linux : Okay, so it cheats a little and touches stuff within the handle
  from outside the sal_socket module, but it hides the effort.
*/
#define sal_sock_set_t                  Ip_fd_set
#define SAL_SOCKSET_SIZE                IP_FD_SETSIZE
#define SAL_SOCK_HANDLESET_ISSET(h,s)   IP_FD_ISSET(h,s)
#define SAL_SOCK_HANDLESET_SET(h,s)     IP_FD_SET(h,s)
#define SAL_SOCK_HANDLESET_CLR(h,s)     IP_FD_CLR(h,s)

/*
  I/O vector for scatter/gather operations
*/
#define sal_iovec iovec

/*
  Message header structure
*/
#define sal_msghdr msghdr
#define SAL_MSG_PEEK                    IP_MSG_PEEK
#define SAL_MSG_DONTROUTE               IP_MSG_DONTROUTE

/*
   POSIX 1003.1g - ancillary data object information
*/
#define sal_cmsghdr	cmsghdr

/* Addrinfo structure.  */
#define sal_addrinfo Ip_addrinfo

/* Servent structure.  */
#define sal_servent servent

/* Hostent structure.  */
#define sal_hostent hostent

#endif /* HAVE_IPNET */

/*
  Functions
*/

#undef sal_sock
#define sal_sock(w,x,y,z) socket(x,y,z)

#ifndef HAVE_IPNET
#undef sal_sock_accept
#define sal_sock_accept(x,y,z) accept(x,y,z)

#undef sal_sock_bind
#define sal_sock_bind bind

#undef sal_sock_sendto
#define sal_sock_sendto sendto

#undef sal_sock_recvfrom
#define sal_sock_recvfrom recvfrom

#undef sal_sock_connect
#define sal_sock_connect connect

#undef sal_sock_select
#define sal_sock_select select

#ifdef _CENTEC_ /* add by Tang Gang, 2008-12-09 */
#undef sal_sock_poll
#define sal_sock_poll poll
#endif /* _CENTEC_ */

#undef sal_sock_getaddrinfo
#define sal_sock_getaddrinfo getaddrinfo

#undef sal_sock_freeaddrinfo
#define sal_sock_freeaddrinfo freeaddrinfo

#endif /* ! HAVE_IPNET */

#undef sal_sock_close
#define sal_sock_close(y,z) close(z)

#undef sal_sock_listen
#define sal_sock_listen listen

#undef sal_sock_read
#define sal_sock_read read

#undef sal_sock_readvec
#define sal_sock_readvec readv

#undef sal_sock_write
#define sal_sock_write write

#undef sal_sock_writevec
#define sal_sock_writevec writev

#undef sal_sock_send
#define sal_sock_send send

#undef sal_sock_sendmsg
#define sal_sock_sendmsg sendmsg

#undef sal_sock_recv
#define sal_sock_recv recv

#undef sal_sock_recvmsg
#define sal_sock_recvmsg recvmsg

#undef sal_sock_shutdown
#define sal_sock_shutdown shutdown

#ifndef HAVE_IPNET
#undef sal_sock_getname
#define sal_sock_getname getsockname
#endif /* ! HAVE_IPNET */

#ifndef HAVE_IPNET
#undef sal_sock_getpeer
#define sal_sock_getpeer getpeername
#endif /* ! HAVE_IPNET */

#undef sal_getservbyname
#define sal_getservbyname getservbyname

#undef sal_gethostbyname
#define sal_gethostbyname gethostbyname

#ifdef _CENTEC_
#undef sal_gethostbyname2
#define sal_gethostbyname2 gethostbyname2
#endif
#undef sal_freehostbyname
#define sal_freehostbyname(x)

#undef sal_ntoh32
#undef sal_hton32
#undef sal_ntoh16
#undef sal_hton16
#define sal_ntoh32 ntohl
#define sal_hton32 htonl
#define sal_ntoh16 ntohs
#define sal_hton16 htons


/*use IFF_NONPRESENT flag to indicate that the line card is not present */
#define IFF_NONPRESENT 0x4

int32
sal_sock_set_nonblocking (sal_sock_handle_t sock, int32 state);
#endif /* __SAL_SOCKET_H__ */
