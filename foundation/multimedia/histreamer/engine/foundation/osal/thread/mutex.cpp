/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "Mutex"

#include "mutex.h"
#include "foundation/log.h"

namespace OHOS {
namespace Media {
namespace OSAL {
Mutex::Mutex() : created_(true)
{
    int rtv = pthread_mutex_init(&nativeHandle_, nullptr);
    if (rtv != 0) {
        created_ = false;
        MEDIA_LOG_E("failed to init pthread mutex");
    }
}

Mutex::~Mutex()
{
    if (created_) {
        pthread_mutex_destroy(&nativeHandle_);
    }
}

void Mutex::Lock()
{
    if (!created_) {
        MEDIA_LOG_E("Lock uninitialized pthread mutex!");
        return;
    }
    pthread_mutex_lock(&nativeHandle_);
}

bool Mutex::TryLock()
{
    if (!created_) {
        MEDIA_LOG_E("TryLock uninitialized pthread mutex.");
        return false;
    }
    int ret = pthread_mutex_trylock(&nativeHandle_);
    if (ret != 0) {
        MEDIA_LOG_E("TryLock failed with ret = " PUBLIC_LOG_D32, ret);
    }
    return ret == 0;
}

void Mutex::Unlock()
{
    if (!created_) {
        MEDIA_LOG_E("Unlock uninitialized pthread mutex!");
        return;
    }
    pthread_mutex_unlock(&nativeHandle_);
}
} // namespace OSAL
} // namespace Media
} // namespace OHOS