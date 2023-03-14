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

#include "lwip/opt.h"

#if LWIP_ENABLE_DISTRIBUTED_NET

#include "lwip/distributed_net/distributed_net_utils.h"

typedef struct no_proxy_network_segment {
  const char *ip;
  const in_addr_t mask_len;
} no_proxy_network_segment;

static const no_proxy_network_segment g_special_network_segment[] = {
    {"127.0.0.0", 8},
    {"192.0.0.0", 29},
    {"192.0.2.0", 24},
    {"192.168.0.0", 16},
    {"192.18.0.0", 15},
    {"192.51.100.0", 24},
    {"0.0.0.0", 8},
    {"100.64.0.0", 10},
    {"169.254.0.0", 16},
    {"172.16.0.0", 12},
    {"203.0.113.0", 24},
    {"224.0.0.0", 4},
    {"240.0.0.0", 4},
    {"10.0.0.0", 8},
    {"255.255.255.255", 32},
};

static in_addr_t mask_len_to_mask(u32_t mask_len)
{
  if (mask_len > 32) {
    return UINT_MAX;
  }
  u8_t num[4] = {0};
  for (int i = 0; i < 4; ++i) {
    u32_t len = (mask_len > 8 ? 8 : mask_len);
    if (len > 0) {
      u8_t byte = 0x80;// binary 1000|0000
      u8_t bit = 0x80; // binary 1000|0000
      for (u32_t j = 0; j < len - 1; ++j) {
        byte = (bit >> 1U) | byte;
        bit >>= 1U;
      }
      num[i] = byte;
    }
    if (mask_len < 8) {
      break;
    }
    mask_len -= 8;
  }
  int mask = 0;
  (void)memcpy_s(&mask, sizeof(int), num, sizeof(int));
  return mask;
}

u8_t is_no_proxy_network_segment(const struct sockaddr_in *addr_in)
{
  for (int i = 0; i < sizeof(g_special_network_segment) / sizeof(g_special_network_segment[0]); ++i) {
    ip4_addr_t ip4_ip = {addr_in->sin_addr.s_addr};
    ip4_addr_t ip4_net_ip = {ipaddr_addr(g_special_network_segment[i].ip)};
    ip4_addr_t ip4_mask = {mask_len_to_mask(g_special_network_segment[i].mask_len)};

    if (ip4_addr_netcmp(&ip4_ip, &ip4_net_ip, &ip4_mask)) {
      return 1;
    }
  }
  return 0;
}

#endif /* LWIP_ENABLE_DISTRIBUTED_NET */