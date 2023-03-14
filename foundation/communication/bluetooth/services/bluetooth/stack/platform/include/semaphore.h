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

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Semaphore Semaphore;

/**
 * @brief Perform instantiation of the Semaphore.
 *
 * @param val set Semaphore initial val.
 * @return Semaphore* Succeed return Semaphore pointer, failed return NULL.
 * @since 6
 */
Semaphore *SemaphoreCreate(uint32_t val);

/**
 * @brief Delete an instantiation of the Semaphore.
 *
 * @param sem Semaphore pointer.
 * @since 6
 */
void SemaphoreDelete(Semaphore *sem);

/**
 * @brief Block reduce Semaphore val by 1.
 *
 * @param sem Semaphore pointer.
 * @since 6
 */
void SemaphoreWait(const Semaphore *sem);

/**
 * @brief NonBlock reduce Semaphore val by 1.
 *
 * @param sem Semaphore pointer.
 * @return Succeed return 0, failed return -1.
 * @since 6
 */
int32_t SemaphoreTryWait(Semaphore *sem);

/**
 * @brief Block increase Semaphore val by 1.
 *
 * @param sem Semaphore pointer.
 * @since 6
 */
void SemaphorePost(const Semaphore *sem);

/**
 * @brief Try Increase Semaphore val by 1.
 *
 * @param sem Semaphore pointer.
 * @return Succeed return 0, failed return -1.
 * @since 6
 */
int32_t SemaphoreTryPost(Semaphore *sem);

/**
 * @brief Get Semaphore instantiation fd.
 *
 * @param sem Semaphore pointer.
 * @return Succeed return fd, failed return -1.
 * @since 6
 */
int32_t SemaphoreGetfd(const Semaphore *sem);

#ifdef __cplusplus
}
#endif

#endif  // SEMAPHORE_H