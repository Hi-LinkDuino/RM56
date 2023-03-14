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

#include "lwip/distributed_net/distributed_net_core.h"
#include "lwip/distributed_net/distributed_net_utils.h"
#include "lwip/distributed_net/udp_transmit.h"
#include "lwip/priv/sockets_priv.h"

int distributed_net_connect(int sock, const struct sockaddr *addr, socklen_t addr_len)
{
  CHECK_PARA(SOCKET_TO_INDEX(sock) >= 0 && SOCKET_TO_INDEX(sock) < NUM_SOCKETS, EBADF);
  CHECK_PARA(addr != NULL, EINVAL);
  CHECK_PARA(addr_len > 0, EINVAL);

  struct sockaddr_in addr_in = {0};
  (void)memset_s(&addr_in, sizeof(addr_in), 0, sizeof(addr_in));
  (void)memcpy_s(&addr_in, sizeof(addr_in), addr, MIN(sizeof(addr_in), addr_len));
  if (is_no_proxy_network_segment(&addr_in)) {
    return lwip_connect_internal(sock, addr, addr_len);
  }

  (void)memset_s(&addr_in, sizeof(addr_in), 0, sizeof(addr_in));
  INIT_SOCK_ADDR(&addr_in, LOCAL_SERVER_IP, get_local_tcp_server_port());
  if (lwip_connect_internal(sock, (struct sockaddr *)&addr_in, sizeof(addr_in)) < 0) {
    if (get_errno() != EINPROGRESS) {
      return -1;
    }
  }
  set_distributed_net_socket(sock);

  (void)memset_s(&addr_in, sizeof(addr_in), 0, sizeof(addr_in));
  (void)memcpy_s(&addr_in, sizeof(addr_in), addr, MIN(sizeof(addr_in), addr_len));

  tcp_connect_data data = {0};
  (void)memset_s(&data, sizeof(data), 0, sizeof(data));
  (void)strcpy_s(data.dest_addr, sizeof(data.dest_addr), inet_ntoa(addr_in.sin_addr));
  data.dest_port = ntohs(addr_in.sin_port);

  if (lwip_send(sock, &data, sizeof(data), 0) < 0) {
    reset_distributed_net_socket(sock);
    return -1;
  }
  return 0;
}

int distributed_net_close(int sock)
{
  CHECK_PARA(SOCKET_TO_INDEX(sock) >= 0 && SOCKET_TO_INDEX(sock) < NUM_SOCKETS, EBADF);

  reset_distributed_net_socket(sock);
  return lwip_close_internal(sock);
}

#if LWIP_USE_GET_HOST_BY_NAME_EXTERNAL
typedef union {
  struct sockaddr sa;
#if LWIP_IPV6
  struct sockaddr_in6 sin6;
#endif /* LWIP_IPV6 */
#if LWIP_IPV4
  struct sockaddr_in sin;
#endif /* LWIP_IPV4 */
} aligned_sockaddr;

ssize_t distributed_net_sendto(int sock, const void *buf, size_t buf_len, int flags, const struct sockaddr *addr,
                               socklen_t addr_len)
{
  CHECK_PARA(SOCKET_TO_INDEX(sock) >= 0 && SOCKET_TO_INDEX(sock) < NUM_SOCKETS, EBADF);
  CHECK_PARA(buf != NULL, EINVAL);
  CHECK_PARA(buf_len > 0, EINVAL);

  int type = 0;
  socklen_t type_len = sizeof(type);
  if (lwip_getsockopt(sock, SOL_SOCKET, SO_TYPE, (void *)&type, &type_len) < 0) {
    return -1;
  }

  if (type != SOCK_DGRAM) {
    return lwip_sendto_internal(sock, buf, buf_len, flags, addr, addr_len);
  }

  struct sockaddr_in addr_in = {0};
  (void)memset_s(&addr_in, sizeof(addr_in), 0, sizeof(addr_in));
  if (addr != NULL && addr_len != 0) {
    (void)memcpy_s(&addr_in, sizeof(addr_in), addr, MIN(sizeof(addr_in), addr_len));
  }

  if (IS_LOCAL_UDP_SERVER_ADDR(&addr_in)) {
    set_errno(EPERM);
    return -1;
  }

  if (is_no_proxy_network_segment(&addr_in)) {
    return lwip_sendto_internal(sock, buf, buf_len, flags, addr, addr_len);
  }

  if (!IS_DNS_PORT(addr_in)) {
    set_errno(EPERM);
    return -1;
  }

  ssize_t ret = udp_transmit_sendto(sock, buf, buf_len, &addr_in);
  return ret > 0 ? UDP_PAYLOAD_LEN(ret) : -1;
}

#if LWIP_DISTRIBUTED_NET_ENABLE_SENDMSG
ssize_t distributed_net_sendmsg(int sock, const struct msghdr *hdr, int flags)
{
  CHECK_PARA(SOCKET_TO_INDEX(sock) >= 0 && SOCKET_TO_INDEX(sock) < NUM_SOCKETS, EBADF);
  CHECK_PARA(hdr != NULL, EINVAL);
  CHECK_PARA(hdr->msg_iov != NULL, EINVAL);
  CHECK_PARA(hdr->msg_iovlen > 0, EINVAL);

  int type = 0;
  socklen_t type_len = sizeof(type);
  if (lwip_getsockopt(sock, SOL_SOCKET, SO_TYPE, (void *)&type, &type_len) < 0) {
    return -1;
  }

  if (type != SOCK_DGRAM) {
    return lwip_sendmsg_internal(sock, hdr, flags);
  }

  const struct sockaddr *addr = (const struct sockaddr *)hdr->msg_name;
  socklen_t addr_len = hdr->msg_namelen;

  struct sockaddr_in addr_in = {0};
  (void)memset_s(&addr_in, sizeof(addr_in), 0, sizeof(addr_in));
  if (addr != NULL && addr_len != 0) {
    (void)memcpy_s(&addr_in, sizeof(addr_in), addr, MIN(sizeof(addr_in), addr_len));
  }

  if (IS_LOCAL_UDP_SERVER_ADDR(&addr_in)) {
    set_errno(EPERM);
    return -1;
  }

  if (is_no_proxy_network_segment(&addr_in)) {
    return lwip_sendmsg_internal(sock, hdr, flags);
  }

  if (!IS_DNS_PORT(addr_in)) {
    set_errno(EPERM);
    return -1;
  }

  ssize_t ret = udp_transmit_sendmsg(sock, hdr);
  return ret > 0 ? UDP_PAYLOAD_LEN(ret) : -1;
}
#endif /* LWIP_DISTRIBUTED_NET_ENABLE_SENDMSG */

ssize_t distributed_net_recvfrom(int sock, void *buf, size_t buf_len, int flags, struct sockaddr *from,
                                 socklen_t *from_len)
{
  CHECK_PARA(SOCKET_TO_INDEX(sock) >= 0 && SOCKET_TO_INDEX(sock) < NUM_SOCKETS, EBADF);
  CHECK_PARA(buf != NULL, EINVAL);
  CHECK_PARA(buf_len > 0, EINVAL);

  int type = 0;
  socklen_t type_len = sizeof(type);
  if (lwip_getsockopt(sock, SOL_SOCKET, SO_TYPE, (void *)&type, &type_len) < 0) {
    return -1;
  }

  if (type != SOCK_DGRAM) {
    return lwip_recvfrom_internal(sock, buf, buf_len, flags, from, from_len);
  }

  size_t new_buf_len = buf_len + sizeof(udp_data);
  void *new_buf = mem_malloc(new_buf_len);
  if (new_buf == NULL) {
    set_errno(ENOMEM);
    return -1;
  }

  aligned_sockaddr addr_from = {0};
  socklen_t addr_from_len = sizeof(addr_from);
  ssize_t ret =
      lwip_recvfrom_internal(sock, new_buf, new_buf_len, flags, (struct sockaddr *)&addr_from, &addr_from_len);
  if (ret <= 0) {
    mem_free(new_buf);
    return ret;
  }

  if (!IS_LOCAL_UDP_SERVER_ADDR((struct sockaddr_in *)(&addr_from))) {
    (void)memcpy_s(buf, buf_len, new_buf, ret);
    if (from != NULL && from_len != NULL) {
      if (*from_len > addr_from_len) {
        *from_len = addr_from_len;
      }
      (void)memcpy_s(from, *from_len, &addr_from, *from_len);
      mem_free(new_buf);
      return ret;
    }
  }

  if (ret <= sizeof(udp_data)) {
    mem_free(new_buf);
    set_errno(EINVAL);
    return -1;
  }

  udp_data *data = (udp_data *)new_buf;
  (void)memcpy_s(buf, buf_len, data->payload, ret - sizeof(udp_data));
  if (from != NULL && from_len != NULL) {
    (void)memcpy_s(from, *from_len, &addr_from, MIN(addr_from_len, *from_len));
    if (*from_len >= sizeof(struct sockaddr_in) - SIN_ZERO_LEN) {
      struct sockaddr_in *temp_addr = (struct sockaddr_in *)from;
      INIT_SOCK_ADDR(temp_addr, data->dest_addr, data->dest_port);
      if (*from_len > sizeof(struct sockaddr_in)) {
        *from_len = sizeof(struct sockaddr_in);
      }
    }
  }

  mem_free(new_buf);
  return ret - (ssize_t)sizeof(udp_data);
}
#endif /* LWIP_USE_GET_HOST_BY_NAME_EXTERNAL */

#endif /* LWIP_ENABLE_DISTRIBUTED_NET */
