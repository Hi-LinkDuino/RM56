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

#ifndef CRPC_EVENT_LOOP_H
#define CRPC_EVENT_LOOP_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FdMask FdMask;
struct FdMask {
    int fd;
    unsigned int mask;
};

typedef struct EventLoop EventLoop;
struct EventLoop {
    int maxFd;
    int setSize;
    FdMask *fdMasks;
    int stop;
    int epfd;
    struct epoll_event *epEvents;
};

/**
 * @Description Create an Event Loop object
 *
 * @param size - Number of event type
 * @return EventLoop* - pointer to the event loop object or NULL if failed
 */
EventLoop *CreateEventLoop(int size);

/**
 * @Description Destroy Events loop
 *
 * @param loop - EventLoop object's pointer
 */
void DestroyEventLoop(EventLoop *loop);

/**
 * @Description Stop Events Loop
 *
 * @param loop - EventLoop object's pointer
 */
void StopEventLoop(EventLoop *loop);

/**
 * @Description Add an event on socket
 *
 * @param loop - EventLoop object's pointer
 * @param fd - Socket fd
 * @param addMask - Event mask
 * @return int - 0 success; -1 add failed
 */
int AddFdEvent(EventLoop *loop, int fd, unsigned int addMask);

/**
 * @Description Remove an event from socket fd
 *
 * @param loop - EventLoop object's pointer
 * @param fd - Socket fd
 * @param delMask - Event mask
 * @return int - 0 success; -1 remove failed
 */
int DelFdEvent(EventLoop *loop, int fd, unsigned int delMask);

#ifdef __cplusplus
}
#endif
#endif