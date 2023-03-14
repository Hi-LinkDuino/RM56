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

#if LWIP_ENABLE_DISTRIBUTED_NET && LWIP_USE_GET_HOST_BY_NAME_EXTERNAL

#include "lwip/distributed_net/udp_transmit.h"

static s32_t get_msg_data_len(const struct msghdr *hdr)
{
  if (hdr->msg_iovlen > MAX_IOV_NUM) {
    return -1;
  }

  s32_t data_len = 0;
  for (int i = 0; i < hdr->msg_iovlen; ++i) {
    if (hdr->msg_iov[i].iov_len > MAX_UDP_PAYLOAD_LEN) {
      set_errno(EMSGSIZE);
      return -1;
    }
    data_len += (s32_t)hdr->msg_iov[i].iov_len;
    if (data_len > MAX_UDP_PAYLOAD_LEN) {
      set_errno(EMSGSIZE);
      return -1;
    }
  }

  return data_len;
}

ssize_t udp_transmit_sendto(int sock, const void *buf, size_t buf_len, const struct sockaddr_in *dest_addr)
{
  if (buf_len > MAX_UDP_PAYLOAD_LEN) {
    set_errno(EMSGSIZE);
    return -1;
  }

  udp_data data = {0};
  (void)memset_s(&data, sizeof(data), 0, sizeof(data));
  (void)strcpy_s(data.dest_addr, sizeof(data.dest_addr), inet_ntoa(dest_addr->sin_addr));
  data.dest_port = ntohs(dest_addr->sin_port);

  struct iovec iov[2] = {0};
  (void)memset_s(iov, sizeof(iov), 0, sizeof(iov));
  iov[0].iov_base = (void *)&data;
  iov[0].iov_len = sizeof(data);
  iov[1].iov_base = (void *)buf;
  iov[1].iov_len = buf_len;

  struct sockaddr_in transmit_addr = {0};
  (void)memset_s(&transmit_addr, sizeof(transmit_addr), 0, sizeof(transmit_addr));
  INIT_SOCK_ADDR(&transmit_addr, LOCAL_SERVER_IP, get_local_udp_server_port());

  struct msghdr send_hdr = {0};
  (void)memset_s(&send_hdr, sizeof(send_hdr), 0, sizeof(send_hdr));
  SET_MSG_ADDR(&send_hdr, &transmit_addr, sizeof(transmit_addr));
  send_hdr.msg_iov = iov;
  send_hdr.msg_iovlen = 2;

#if LWIP_DISTRIBUTED_NET_ENABLE_SENDMSG
  return lwip_sendmsg_internal(sock, &send_hdr, 0);
#else
  return lwip_sendmsg(sock, &send_hdr, 0);
#endif
}

ssize_t udp_transmit_sendmsg(int sock, const struct msghdr *hdr)
{
  if (get_msg_data_len(hdr) < 0) {
    return -1;
  }

  struct sockaddr_in temp_addr_in = {0};
  (void)memset_s(&temp_addr_in, sizeof(temp_addr_in), 0, sizeof(temp_addr_in));
  (void)memcpy_s(&temp_addr_in, sizeof(temp_addr_in), hdr->msg_name, MIN(sizeof(temp_addr_in), hdr->msg_namelen));

  udp_data data = {0};
  (void)memset_s(&data, sizeof(data), 0, sizeof(data));
  (void)strcpy_s(data.dest_addr, sizeof(data.dest_addr), inet_ntoa(temp_addr_in.sin_addr));
  data.dest_port = ntohs(temp_addr_in.sin_port);

  u32_t size = sizeof(struct iovec) * (hdr->msg_iovlen + 1);
  struct iovec *iov = mem_malloc(size);
  if (iov == NULL) {
    set_errno(ENOMEM);
    return -1;
  }
  (void)memset_s(iov, size, 0, size);
  iov[0].iov_base = (void *)&data;
  iov[0].iov_len = sizeof(data);
  for (int i = 0; i < hdr->msg_iovlen; ++i) {
    iov[i + 1].iov_base = hdr->msg_iov[i].iov_base;
    iov[i + 1].iov_len = hdr->msg_iov[i].iov_len;
  }

  struct sockaddr_in transmit_addr = {0};
  (void)memset_s(&transmit_addr, sizeof(transmit_addr), 0, sizeof(transmit_addr));
  INIT_SOCK_ADDR(&transmit_addr, LOCAL_SERVER_IP, get_local_udp_server_port());

  struct msghdr send_hdr = {0};
  (void)memset_s(&send_hdr, sizeof(send_hdr), 0, sizeof(send_hdr));
  SET_MSG_ADDR(&send_hdr, &transmit_addr, sizeof(transmit_addr));
  send_hdr.msg_iov = iov;
  send_hdr.msg_iovlen = hdr->msg_iovlen + 1;

#if LWIP_DISTRIBUTED_NET_ENABLE_SENDMSG
  ssize_t ret = lwip_sendmsg_internal(sock, &send_hdr, 0);
#else
  ssize_t ret = lwip_sendmsg(sock, &send_hdr, 0);
#endif
  mem_free(iov);
  return ret;
}

#endif /* LWIP_ENABLE_DISTRIBUTED_NET && LWIP_USE_GET_HOST_BY_NAME_EXTERNAL */
