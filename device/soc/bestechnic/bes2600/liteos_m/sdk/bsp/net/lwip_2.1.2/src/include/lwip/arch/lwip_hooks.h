/**
  ******************************************************************************
  * Copyright 2015-2019 BES.
  * All rights reserved. All unpublished rights reserved.
  *
  * @file    lwip_hooks.h
  * @author  BES Firm Team(Shanghai, China)
  * @version V1.0.0
  * @date    7/20/2022
  * @brief   lwIP Hooks Configuration.
  *          Hooks are undefined by default, define them to a function if you need them. Declare
  *          your hook function prototypes in there, you may also \#include all headers providing
  *          data types that are need in this file.
  ******************************************************************************
  */

#ifndef __LWIP_HOOKS_H__
#define __LWIP_HOOKS_H__


#ifdef LWIP_HOOK_SOCKET
#include "lwip/sockets.h"
#include "lwip/priv/sockets_priv.h"

int lwip_hook_sockets_setsockopt(int s, struct lwip_sock *sock, int level, int optname,
                                 const void *optval, socklen_t optlen, int *err);

#define LWIP_HOOK_SOCKETS_SETSOCKOPT(s, sock, level, optname, optval, optlen, err) \
        lwip_hook_sockets_setsockopt(s, sock, level, optname, optval, optlen, err)
#endif /* LWIP_HOOK_SOCKET */


#endif /* __LWIP_HOOKS_H__ */