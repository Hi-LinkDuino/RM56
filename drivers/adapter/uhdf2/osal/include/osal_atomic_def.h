/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OSAL_ATOMIC_DEF_H
#define OSAL_ATOMIC_DEF_H

#include "hdf_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OsalAtomicReadWrapper(v) (*(volatile int *)&(v)->counter)
#define OsalAtomicSetWrapper(v, value) ((v)->counter = (value))
#define OsalAtomicIncWrapper(v) __sync_fetch_and_add(&(v)->counter, 1)
#define OsalAtomicDecWrapper(v) __sync_sub_and_fetch(&(v)->counter, 1)
static inline int32_t BitDoNotSupport(int nr, unsigned long *addr)
{
    (void)nr;
    (void)addr;
    return HDF_ERR_NOT_SUPPORT;
}
#define OsalTestBitWrapper(nr, addr) BitDoNotSupport(nr, addr)
#define OsalTestSetBitWrapper(nr, addr) BitDoNotSupport(nr, addr)
#define OsalTestClearBitWrapper(nr, addr) BitDoNotSupport(nr, addr)
#define OsalClearBitWrapper(nr, addr) BitDoNotSupport(nr, addr)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_ATOMIC_DEF_H */

