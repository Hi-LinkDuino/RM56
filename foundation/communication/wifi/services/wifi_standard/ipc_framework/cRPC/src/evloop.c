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

#include "evloop.h"
#include "log.h"

#undef LOG_TAG
#define LOG_TAG "WifiRpcEventLoop"

const int ELOOP_MAX_FD_SIZE = 1024;

EventLoop *CreateEventLoop(int size)
{
    int flag = 0; /* flag here is used to indicate whether a failure occurs when create event loop struct, 0:success
                     1:failure */
    EventLoop *evLoop = NULL;
    do {
        evLoop = (EventLoop *)calloc(1, sizeof(EventLoop));
        if (evLoop == NULL) {
            LOGE("Failed to calloc EventLoop struct!");
            return NULL;
        }
        evLoop->setSize = size;
        evLoop->epfd = -1;
        if (size <= 0) {
            free(evLoop);
            evLoop = NULL;
            return NULL;
        }
        evLoop->fdMasks = (FdMask *)calloc(size, sizeof(FdMask));
        evLoop->epEvents = (struct epoll_event *)calloc(size, sizeof(struct epoll_event));
        if (evLoop->fdMasks == NULL || evLoop->epEvents == NULL) {
            flag = 1; /* fail */
            LOGE("Failed to calloc events or epoll_event struct!");
            break;
        }
        evLoop->epfd = epoll_create(ELOOP_MAX_FD_SIZE);
        if (evLoop->epfd == -1) {
            flag = 1; /* fail */
            LOGE("Failed to call epoll_create!");
            break;
        }
    } while (0);
    if (flag == 0) {
        return evLoop;
    }
    if (evLoop->fdMasks != NULL) {
        free(evLoop->fdMasks);
    }
    if (evLoop->epEvents != NULL) {
        free(evLoop->epEvents);
    }
    free(evLoop);
    evLoop = NULL;
    return NULL;
}

void DestroyEventLoop(EventLoop *loop)
{
    if (loop == NULL) {
        return;
    }

    if (loop->epfd != -1) {
        close(loop->epfd);
    }
    if (loop->fdMasks != NULL) {
        free(loop->fdMasks);
    }
    if (loop->epEvents != NULL) {
        free(loop->epEvents);
    }
    free(loop);
    loop = NULL;
    return;
}

void StopEventLoop(EventLoop *loop)
{
    if (loop == NULL) {
        return;
    }

    loop->stop = 1;
    return;
}

int AddFdEvent(EventLoop *loop, int fd, unsigned int addMask)
{
    if (loop == NULL) {
        return -1;
    }

    if (fd >= loop->setSize) {
        return -1;
    }
    if (loop->fdMasks[fd].mask & addMask) {
        return 0;
    }
    int op = (loop->fdMasks[fd].mask == NONE_EVENT) ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    addMask |= loop->fdMasks[fd].mask;
    struct epoll_event pollEvent = {0};
    if (addMask & READ_EVENT) {
        pollEvent.events |= EPOLLIN;
    }
    if (addMask & WRIT_EVENT) {
        pollEvent.events |= EPOLLOUT;
    }
    pollEvent.data.fd = fd;
    if (epoll_ctl(loop->epfd, op, fd, &pollEvent) == -1) {
        return -1;
    }
    loop->fdMasks[fd].fd = fd;
    loop->fdMasks[fd].mask |= addMask;
    if (fd > loop->maxFd) {
        loop->maxFd = fd;
    }
    return 0;
}

int DelFdEvent(EventLoop *loop, int fd, unsigned int delMask)
{
    if (loop == NULL) {
        return -1;
    }

    if (fd >= loop->setSize) {
        return 0;
    }
    if (loop->fdMasks[fd].mask == NONE_EVENT) {
        return 0;
    }
    if ((loop->fdMasks[fd].mask & delMask) == 0) {
        return 0;
    }
    unsigned int mask = loop->fdMasks[fd].mask & (~delMask);
    struct epoll_event pollEvent = {0};
    pollEvent.events = 0;
    if (mask & READ_EVENT) {
        pollEvent.events |= EPOLLIN;
    }
    if (mask & WRIT_EVENT) {
        pollEvent.events |= EPOLLOUT;
    }
    pollEvent.data.fd = fd;
    int op = (mask == NONE_EVENT) ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
    if (epoll_ctl(loop->epfd, op, fd, &pollEvent) == -1) {
        return -1;
    }
    loop->fdMasks[fd].mask &= ~delMask;
    if (fd == loop->maxFd && loop->fdMasks[fd].mask == NONE_EVENT) {
        int j = loop->maxFd - 1;
        for (; j >= 0; --j) {
            if (loop->fdMasks[j].mask != NONE_EVENT) {
                break;
            }
        }
        loop->maxFd = j;
    }

    return 0;
}
