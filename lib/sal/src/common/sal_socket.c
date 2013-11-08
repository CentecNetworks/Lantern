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

#include "sal.h"
#include "sal_socket.h"

int32
sal_sock_set_nonblocking (sal_sock_handle_t sock, int32 state)
{
#ifndef HAVE_IPNET
  int val;

  val = fcntl (sock, F_GETFL, 0);
  if (SAL_SOCK_ERROR != val)
    {
      fcntl (sock, F_SETFL, (state ? val | O_NONBLOCK : val & (~O_NONBLOCK)));
      return RESULT_OK;
    }
  else
    {
      return errno;
    }
#else
  int ret;
  int on;

  if (state == PAL_TRUE)
    on = 1;
  else
    on = 0;

  ret = ioctl (sock, IP_FIONBIO, (void *) &on);
  if (ret < 0)
    return RESULT_ERROR;

  return RESULT_OK;
#endif /* HAVE_IPNET */
}

#ifdef HAVE_IPNET
sal_sock_handle_t
sal_sock_accept (sal_sock_handle_t sock,
                 struct sal_sockaddr *addr,
                 size_t *addrlen)
{
  struct sockaddr taddr;
  int ret;

  ret = -1;

  if ((addr == NULL) || (addrlen == NULL))
    return ret;

  memset (&taddr, 0, sizeof (struct sockaddr));

  ret = accept (sock, (struct sockaddr *) &taddr, addrlen);
  if (sock < 0)
    return ret;

  /* Adjust sa_len, sa_family. */
  addr->sa_family = taddr.sa_family;
  if (addr->sa_family == AF_INET)
    addr->sa_len = sizeof (struct Ip_sockaddr_in);
#ifdef HAVE_IPV6
  else if (addr->sa_family == AF_INET6)
    addr->sa_len = sizeof (struct Ip_sockaddr_in6);
#endif /* HAVE_IPV6 */
  else
    addr->sa_len = sizeof (struct Ip_sockaddr);

  /* Copy sa_data. */
  memcpy (addr->sa_data, taddr.sa_data, addr->sa_len - (sizeof (addr->sa_family) + sizeof (addr->sa_len)));

  return ret;
}

#endif /* HAVE_IPNET */
