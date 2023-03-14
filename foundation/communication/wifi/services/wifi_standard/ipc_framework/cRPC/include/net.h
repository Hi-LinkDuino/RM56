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

#ifndef CRPC_NET_H
#define CRPC_NET_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @Description Open/close fd flag O_NONBLOCK
 *
 * @param fd - Socket fd
 * @param type - 1 open O_NONBLOCK; other close O_NONBLOCK
 * @return int - 0 Success; -1 Failed
 */
int SetNonBlock(int fd, int type);

/**
 * @Description Read fd's message
 *
 * @param fd - Socket fd
 * @param buf - Buffer for storing the read content
 * @param count - Specifies the number of records to be read
 * @return int - return read message numbers or read error return error code
 */
int MyRead(int fd, char *buf, int count);

/**
 * @Description Write to fd
 *
 * @param fd - Socket fd
 * @param buf - Contents to write to the file
 * @param count - Size of the contents to write to the file
 * @return int - return write message numbers or write error return error code
 */
int MyWrite(int fd, const char *buf, int count);

/**
 * @Description Create a Unix Server
 *
 * @param path - Location of the domain socket communication file
 * @param backlog - Maximum number of connections that can be queued by a socket
 * @return int - -1 failed; else return server listen fd
 */
int CreateUnixServer(const char *path, int backlog);

/**
 * @Description Connect to Unix Server
 *
 * @param path - Location of the domain socket communication file
 * @return int - -1 failed; else return client socket fd
 */
int ConnectUnixServer(const char *path);

/**
 * @Description Waiting for socket events to occur
 *
 * @param fd - socket fd
 * @param mask - event mask
 * @param milliseconds - max Waiting time
 * @return int - -1 Failed; 0 Time out; 1 events occurred
 */
int WaitFdEvent(int fd, unsigned int mask, int milliseconds);

#ifdef __cplusplus
}
#endif
#endif