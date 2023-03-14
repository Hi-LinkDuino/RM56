/* Copyright (c) 2021-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LWIP_HDR_DISTRIBUTED_NET_CORE_H
#define LWIP_HDR_DISTRIBUTED_NET_CORE_H

#include "lwip/opt.h"

#if LWIP_ENABLE_DISTRIBUTED_NET

#include "lwip/distributed_net/distributed_net.h"
#include "lwip/sockets.h"

#define DNS_PORT 53

#define CHECK_PARA(exp, err) \
  do { \
    if (!(exp)) { \
      set_errno(err); \
      return -1; \
    } \
  } while (0)

#define IS_LOCAL_UDP_SERVER_ADDR(addr) \
  ((addr)->sin_addr.s_addr == ipaddr_addr(LOCAL_SERVER_IP) && (addr)->sin_port == ntohs(get_local_udp_server_port()))

#define IS_DNS_PORT(addr) (ntohs((addr).sin_port) == DNS_PORT)

typedef struct tcp_connect_data {
  char dest_addr[IP4_MAX_ADDR_LEN];
  u32_t dest_port;
} tcp_connect_data;

int distributed_net_connect(int sock, const struct sockaddr *addr, socklen_t addr_len);

int distributed_net_close(int sock);

#if LWIP_USE_GET_HOST_BY_NAME_EXTERNAL
ssize_t distributed_net_sendto(int sock, const void *buf, size_t buf_len, int flags, const struct sockaddr *addr,
                               socklen_t addr_len);

#if LWIP_DISTRIBUTED_NET_ENABLE_SENDMSG
ssize_t distributed_net_sendmsg(int sock, const struct msghdr *hdr, int flags);
#endif

ssize_t distributed_net_recvfrom(int sock, void *buf, size_t buf_len, int flags, struct sockaddr *from,
                                 socklen_t *from_len);
#endif /* LWIP_USE_GET_HOST_BY_NAME_EXTERNAL */

#endif /* LWIP_ENABLE_DISTRIBUTED_NET */

#endif /* LWIP_HDR_DISTRIBUTED_NET_CORE_H */
