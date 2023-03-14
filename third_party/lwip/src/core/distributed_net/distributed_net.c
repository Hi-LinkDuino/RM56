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

#include "lwip/distributed_net/distributed_net.h"
#include "lwip/priv/sockets_priv.h"

static sys_mutex_t g_mutex = {0};

static u8_t g_is_distributed_net_enabled = 0;

static u16_t g_local_tcp_server_port = 0;

static u16_t g_local_udp_server_port = 0;

static u8_t g_is_distributed_net_socket[NUM_SOCKETS] = {0};

#if LWIP_DISTRIBUTED_NET_TRY_CONNECT
static int try_connect_to_local_tcp_server(u16_t tcp_port)
{
  struct sockaddr_in addr = {0};
  (void)memset_s(&addr, sizeof(struct sockaddr_in), 0, sizeof(struct sockaddr_in));
  INIT_SOCK_ADDR(&addr, LOCAL_SERVER_IP, tcp_port);

  int sock = lwip_socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    return -1;
  }

  int ret = lwip_connect_internal(sock, (struct sockaddr *)&addr, sizeof(addr));
  (void)lwip_close_internal(sock);
  return ret;
}
#endif /* LWIP_DISTRIBUTED_NET_TRY_CONNECT */

void set_distributed_net_socket(int sock)
{
  int index = SOCKET_TO_INDEX(sock);
  if (index >= 0 && index < NUM_SOCKETS) {
    sys_mutex_lock(&g_mutex);
    g_is_distributed_net_socket[index] = 1;
    sys_mutex_unlock(&g_mutex);
  }
}

void reset_distributed_net_socket(int sock)
{
  int index = SOCKET_TO_INDEX(sock);
  if (index >= 0 && index < NUM_SOCKETS) {
    sys_mutex_lock(&g_mutex);
    g_is_distributed_net_socket[index] = 0;
    sys_mutex_unlock(&g_mutex);
  }
}

u16_t get_local_tcp_server_port(void)
{
  sys_mutex_lock(&g_mutex);
  u16_t ret = g_local_tcp_server_port;
  sys_mutex_unlock(&g_mutex);
  return ret;
}

u16_t get_local_udp_server_port(void)
{
  sys_mutex_lock(&g_mutex);
  u16_t ret = g_local_udp_server_port;
  sys_mutex_unlock(&g_mutex);
  return ret;
}

u8_t is_distributed_net_enabled(void)
{
  sys_mutex_lock(&g_mutex);
  u8_t ret = g_is_distributed_net_enabled;
  sys_mutex_unlock(&g_mutex);
  return ret;
}

int enable_distributed_net(u16_t tcp_port, u16_t udp_port)
{
  LWIP_DEBUGF(SOCKETS_DEBUG, ("enable distributed_net"));
  if (is_distributed_net_enabled()) {
    if (get_local_tcp_server_port() == tcp_port && get_local_udp_server_port() == udp_port) {
      return 0;
    }
    set_errno(EINVAL);
    return -1;
  }

#if LWIP_DISTRIBUTED_NET_TRY_CONNECT
  if (try_connect_to_local_tcp_server(tcp_port) < 0) {
    return -1;
  }
#endif /* LWIP_DISTRIBUTED_NET_TRY_CONNECT */

  sys_mutex_lock(&g_mutex);
  g_is_distributed_net_enabled = 1;
  g_local_tcp_server_port = tcp_port;
  g_local_udp_server_port = udp_port;
  sys_mutex_unlock(&g_mutex);
  return 0;
}

int disable_distributed_net(void)
{
  LWIP_DEBUGF(SOCKETS_DEBUG, ("disable distributed_net"));
  sys_mutex_lock(&g_mutex);
  for (int i = 0; i < NUM_SOCKETS; ++i) {
    if (g_is_distributed_net_socket[i]) {
      (void)lwip_close_internal(INDEX_TO_SOCKET(i));
    }
  }
  g_local_tcp_server_port = 0;
  g_local_udp_server_port = 0;
  g_is_distributed_net_enabled = 0;
  (void)memset_s(g_is_distributed_net_socket, sizeof(g_is_distributed_net_socket), 0,
                 sizeof(g_is_distributed_net_socket));
  sys_mutex_unlock(&g_mutex);
  return 0;
}

#endif /* LWIP_ENABLE_DISTRIBUTED_NET */