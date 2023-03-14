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

#ifndef MUTEX_H
#define MUTEX_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Mutex Mutex;

/**
 * @brief Initialize the object of the Mutex.
 *
 * @return Succeed return Mutex pointer, failed return NULL.
 * @since 6
 */
Mutex *MutexCreate();

/**
 * @brief Delete object of the Mutex.
 *
 * @param mutex Mutex pointer.
 * @since 6
 */
void MutexDelete(Mutex *mutex);

/**
 * @brief Lock the Mutex instance.
 *
 * @param mutex Mutex pointer.
 * @since 6
 */
void MutexLock(Mutex *mutex);

/**
 * @brief Unlock the Mutex instance.
 *
 * @param mutex Mutex pointer.
 * @since 6
 */
void MutexUnlock(Mutex *mutex);

/**
 * @brief Trylock the Mutex instance. And return the result immediately.
 *
 * @param mutex Mutex pointer.
 * @return int32_t Succeed return 0, failed return -1.
 * @since 6
 */
int32_t MutexTryLock(Mutex *mutex);

#ifdef __cplusplus
}
#endif

#endif  // MUTEX_H