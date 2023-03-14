/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CRPC_COMMON_H
#define CRPC_COMMON_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>

#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SOCK_CLOSE (-200)
#define SOCK_ERR (-100)

#define NONE_EVENT (0X00)
#define READ_EVENT (0x01)
#define WRIT_EVENT (0X02)
#define EXCP_EVENT (0X04)

#define INSERT_HASH_DUPLICATE (-2)
#define SERIAL_READ_TOO_LONG (-2)
#define SERIAL_READ_INVALID_DIGIT (-3)
#define MAX_ONE_LINE_SIZE (1024)

#ifdef __cplusplus
}
#endif
#endif