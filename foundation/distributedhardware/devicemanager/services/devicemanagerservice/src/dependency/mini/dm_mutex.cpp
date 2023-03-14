/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string>

#include "dm_log.h"
#include "dm_mutex.h"

namespace OHOS {
namespace DistributedHardware {
DmMutex::DmMutex()
{
    uint32_t ret = pthread_mutex_init(&lock_, NULL);
    if (ret != 0) {
        LOGE("init mutex lock failed: %d.", ret);
    }
    pthread_mutex_lock(&lock_);
}

DmMutex::~DmMutex()
{
    pthread_mutex_unlock(&lock_);
    uint32_t ret = pthread_mutex_destroy(&lock_);
    if (ret != 0) {
        LOGI("destroy mutex lock failed: %d.", ret);
    }
}
} // namespace DistributedHardware
} // namespace OHOS
