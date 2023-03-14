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

#include "platform/include/reactor.h"
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include "platform/include/mutex.h"
#include "platform/include/list.h"
#include "platform/include/semaphore.h"
#include "platform/include/platform_def.h"

typedef struct Reactor {
    int epollFd;
    int stopFd;
    bool isRunning;
    bool itemRemoved;
    pthread_t threadId;
    List *movedItems;
    Mutex *apiMutex;
} ReactorInternal;

typedef struct ReactorItem {
    int fd;
    Mutex *lock;
    Reactor *reactor;
    void *context;
    void (*onReadReady)(void *context);
    void (*onWriteReady)(void *context);
} ReactorItemInternal;

const int MAXEPOLLEVENTS = 64;

static inline bool DataCmp(void *data1, void *data2)
{
    if (data1 != data2) {
        return false;
    }
    return true;
}

void ReactorSetThreadId(Reactor *reactor, unsigned long threadId)
{
    reactor->threadId = (pthread_t)threadId;
}

Reactor *ReactorCreate()
{
    Reactor *reactor = (Reactor *)calloc(1, sizeof(Reactor));
    reactor->epollFd = -1;
    reactor->stopFd = -1;

    int epollFd = epoll_create1(EPOLL_CLOEXEC);
    if (epollFd == -1) {
        LOG_ERROR("ReatorCreate: epoll create failed, error no: %{public}d.", errno);
        goto ERROR;
    }

    int stopFd = eventfd(0, 0);
    if (stopFd == -1) {
        LOG_ERROR("ReatorCreate: eventfd failed, error no: %{public}d.", errno);
        goto ERROR;
    }

    struct epoll_event event = {0};
    event.data.ptr = NULL;
    event.events = EPOLLIN;

    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, stopFd, &event) == -1) {
        LOG_ERROR("ReatorCreate: epoll_ctl ADD-Option failed, error no: %{public}d.", errno);
        goto ERROR;
    }

    reactor->movedItems = ListCreate(NULL);
    if (reactor->movedItems == NULL) {
        goto ERROR;
    }
    reactor->apiMutex = MutexCreate();
    if (reactor->apiMutex == NULL) {
        goto ERROR;
    }
    reactor->epollFd = epollFd;
    reactor->stopFd = stopFd;

    return reactor;

ERROR:
    ReactorDelete(reactor);
    return NULL;
}

void ReactorDelete(Reactor *reactor)
{
    if (reactor == NULL) {
        return;
    }

    MutexDelete(reactor->apiMutex);
    ListDelete(reactor->movedItems);
    close(reactor->stopFd);
    close(reactor->epollFd);
    free(reactor);
}

int32_t ReactorStart(Reactor *reactor)
{
    ASSERT(reactor);

    reactor->isRunning = true;

    struct epoll_event events[MAXEPOLLEVENTS];
    for (;;) {
        MutexLock(reactor->apiMutex);
        ListClear(reactor->movedItems);
        MutexUnlock(reactor->apiMutex);

        int nfds;
        CHECK_EXCEPT_INTR(nfds = epoll_wait(reactor->epollFd, events, MAXEPOLLEVENTS, -1));
        if (nfds == -1) {
            reactor->isRunning = false;
            LOG_ERROR("ReactorStart: epoll_wait failed, error no: %{public}d.", errno);
            return -1;
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.ptr == NULL) {
                eventfd_t val;
                eventfd_read(reactor->stopFd, &val);
                reactor->isRunning = false;
                return 0;
            }

            ReactorItem *item = (ReactorItem *)events[i].data.ptr;

            MutexLock(reactor->apiMutex);
            if (ListForEachData(reactor->movedItems, DataCmp, item) != NULL) {
                MutexUnlock(reactor->apiMutex);
                continue;
            }

            MutexLock(item->lock);
            MutexUnlock(reactor->apiMutex);
            reactor->itemRemoved = false;
            if ((events[i].events & (EPOLLIN | EPOLLRDHUP)) && (item->onReadReady != NULL)) {
                item->onReadReady(item->context);
            }
            if ((events[i].events & EPOLLOUT) && (item->onWriteReady != NULL) && (!reactor->itemRemoved)) {
                item->onWriteReady(item->context);
            }

            MutexUnlock(item->lock);

            if (reactor->itemRemoved) {
                free(item->lock);
                free(item);
            }
        }
    }
}

void ReactorStop(const Reactor *reactor)
{
    ASSERT(reactor);
    eventfd_write(reactor->stopFd, 1);
}

ReactorItem *ReactorRegister(
    Reactor *reactor, int fd, void *context, void (*onReadReady)(void *context), void (*onWriteReady)(void *context))
{
    ASSERT(reactor);

    ReactorItem *item = (ReactorItem *)calloc(1, (sizeof(ReactorItem)));

    item->lock = MutexCreate();
    if (item->lock == NULL) {
        goto ERROR;
    }

    item->fd = fd;
    item->context = context;
    item->reactor = reactor;
    item->onReadReady = onReadReady;
    item->onWriteReady = onWriteReady;

    struct epoll_event event = {0};
    event.data.ptr = item;
    if (onReadReady != NULL) {
        event.events |= (EPOLLIN | EPOLLRDHUP);
    }
    if (onWriteReady != NULL) {
        event.events |= EPOLLOUT;
    }

    if (epoll_ctl(reactor->epollFd, EPOLL_CTL_ADD, item->fd, &event) == -1) {
        goto ERROR;
    }

    return item;

ERROR:
    if (item != NULL) {
        MutexDelete(item->lock);
        free(item);
    }

    return NULL;
}

void ReactorUnregister(ReactorItem *item)
{
    ASSERT(item);

    struct epoll_event event = {0};
    if (epoll_ctl(item->reactor->epollFd, EPOLL_CTL_DEL, item->fd, &event) != 0) {
        LOG_ERROR("ReactorUnregister: epoll_ctl delete-option failed, error no: %{public}d.", errno);
    }

    if (pthread_equal(item->reactor->threadId, pthread_self())) {
        if (item->reactor->isRunning) {
            item->reactor->itemRemoved = true;
            return;
        }
    }

    MutexLock(item->reactor->apiMutex);
    ListAddLast(item->reactor->movedItems, item);
    MutexUnlock(item->reactor->apiMutex);

    MutexLock(item->lock);
    MutexUnlock(item->lock);

    free(item->lock);
    free(item);
}