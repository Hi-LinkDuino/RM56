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

#ifndef LWIP_HDR_DISTRIBUTED_NET_H
#define LWIP_HDR_DISTRIBUTED_NET_H

#include "lwip/opt.h"

#if LWIP_ENABLE_DISTRIBUTED_NET

#include "lwip/sockets.h"

#define IP4_MAX_ADDR_LEN 16 /* strlen(255.255.255.255) + 1 */

#define LOCAL_SERVER_IP "127.0.0.1"

#define SOCKET_TO_INDEX(socket) ((socket)-LWIP_SOCKET_OFFSET)

#define INDEX_TO_SOCKET(index) ((index) + LWIP_SOCKET_OFFSET)

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define INIT_SOCK_ADDR(addr, ip_string, port) \
  do { \
    (addr)->sin_family = AF_INET; \
    (addr)->sin_port = lwip_htons(port); \
    (addr)->sin_addr.s_addr = ipaddr_addr(ip_string); \
  } while (0)

#define SET_MSG_ADDR(hdr, addr, addr_len) \
  do { \
    ((struct msghdr *)(hdr))->msg_name = (void *)(addr); \
    ((struct msghdr *)(hdr))->msg_namelen = (addr_len); \
  } while (0)

void set_distributed_net_socket(int sock);

void reset_distributed_net_socket(int sock);

u16_t get_local_tcp_server_port(void);

u16_t get_local_udp_server_port(void);

u8_t is_distributed_net_enabled(void);

int enable_distributed_net(u16_t tcp_port, u16_t udp_port);

int disable_distributed_net(void);

#endif /* LWIP_ENABLE_DISTRIBUTED_NET */

#endif /* LWIP_HDR_DISTRIBUTED_NET_H */
