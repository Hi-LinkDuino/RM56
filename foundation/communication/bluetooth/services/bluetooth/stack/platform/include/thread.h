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

#ifndef THREAD_H
#define THREAD_H

#include "reactor.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_NAME_SIZE 16
#define MAX_STATIC_PRIORITY 20
#define MIN_STATIC_PRIORITY -20

typedef struct Thread Thread;
typedef void (*TaskFunc)(void *context);

/**
 * @brief Perform instantiation of the Thread.
 *
 * @param name Thread name, length less or equal 16 bytes.
 * @return Thread pointer.
 * @since 6
 */
Thread *ThreadCreate(const char *name);

/**
 * @brief Destroy instantiation of the Thread.
 *
 * @param thread Thread pointer.
 * @since 6
 */
void ThreadDelete(Thread *thread);

/**
 * @brief Post Task to Thread.
 *
 * @param thread Thread pointer.
 * @param func TaskFunc.
 * @param parameter TaskFunc's context.
 * @since 6
 */
void ThreadPostTask(Thread *thread, TaskFunc func, void *context);

/**
 * @brief Return whether current running thread is equal to thread in parameter.
 *
 * @param thread Thread pointer.
 * @return Same return 0, not same return -1.
 * @since 6
 */
int32_t ThreadIsSelf(const Thread *thread);

/**
 * @brief Get Thread Reactor.
 *
 * @param thread Thread pointer.
 * @return Reactor of thread
 * @since 6
 */
Reactor *ThreadGetReactor(const Thread *thread);

#ifdef __cplusplus
}
#endif

#endif  // THREAD_H