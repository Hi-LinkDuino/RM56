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

#ifndef LWIP_HDR_UDP_TRANSMIT_H
#define LWIP_HDR_UDP_TRANSMIT_H

#include "lwip/opt.h"

#if LWIP_ENABLE_DISTRIBUTED_NET

typedef struct udp_data {
  char dest_addr[IP4_MAX_ADDR_LEN];
  u32_t dest_port;
  char payload[];
} udp_data;

#if LWIP_USE_GET_HOST_BY_NAME_EXTERNAL

#include "lwip/distributed_net/distributed_net.h"
#include "lwip/sockets.h"

#define MAX_UDP_PAYLOAD_LEN 1024

#define MAX_IOV_NUM 32

#define UDP_PAYLOAD_LEN(send_len) ((send_len) - (ssize_t)sizeof(udp_data))

ssize_t udp_transmit_sendto(int sock, const void *buf, size_t buf_len, const struct sockaddr_in *dest_addr);

ssize_t udp_transmit_sendmsg(int sock, const struct msghdr *hdr);

#endif /* LWIP_USE_GET_HOST_BY_NAME_EXTERNAL */

#endif /* LWIP_ENABLE_DISTRIBUTED_NET */

#endif /* LWIP_HDR_UDP_TRANSMIT_H */
