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

#ifndef REACTOR_H
#define REACTOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Reactor Reactor;
typedef struct ReactorItemCbs ReactorItemCbs;
typedef struct ReactorItem ReactorItem;

typedef enum { REACTOR_STATUS_STOP, REACTOR_STATUS_ERROR, REACTOR_STATUS_DONE } ReactorStatus;

/**
 * @brief Perform instantiation of the Reactor.
 *        Succeed return Reactor instantiation, failed return NULL.
 *
 * @param name
 * @return Reactor pointer.
 * @since 6
 */
Reactor *ReactorCreate();

/**
 * @brief Destroy instantiation of the Reactor.
 *
 * @param reactor Reactor pointer.
 * @since 6
 */
void ReactorDelete(Reactor *reactor);

/**
 * @brief Reactor start to monitor fds.
 *
 * @param reactor Reactor pointer.
 * @return Succeed return 0, failed return -1.
 * @since 6
 */
int32_t ReactorStart(Reactor *reactor);

/**
 * @brief Reactor stop monitor fds.
 *
 * @param reactor Reactor pointer.
 * @since 6
 */
void ReactorStop(const Reactor *reactor);

/**
 * @brief Reactor Set its corresponding thread ID.
 *
 * @param reactor Reactor pointer.
 * @param threadId Corresponding Thread ID.
 * @since 6
 */
void ReactorSetThreadId(Reactor *reactor, unsigned long threadId);

/**
 * @brief Register item into reactor
 *
 * @param reactor Reactor pointer.
 * @param fd Monitor fd.
 * @param context Callback context.
 * @param onReadReady Callback function while fd is readready.
 * @param onWriteReady Callback function while fd is writeready.
 * @return Success return ReactorItem pointer. Failed return NULL.
 * @since 6
 */
ReactorItem *ReactorRegister(Reactor *reactor, int32_t fd, void *context, void (*onReadReady)(void *context),
    void (*onWriteReady)(void *context));

/**
 * @brief UnRegist item from reactor. As well as delete it.
 *
 * @param item ReactorItem pointer.
 * @since 6
 */
void ReactorUnregister(ReactorItem *item);

#ifdef __cplusplus
}
#endif

#endif  // REACTOR_H
