/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef HDI_SHARED_MEM_QUEUE_SYNCER_H
#define HDI_SHARED_MEM_QUEUE_SYNCER_H

#include <atomic>
#include <parcel.h>
#include <stdint.h>

namespace OHOS {
namespace HDI {
namespace Base {
class SharedMemQueueSyncer {
public:
    explicit SharedMemQueueSyncer(std::atomic<uint32_t> *syncerPtr);
    ~SharedMemQueueSyncer() = default;
    enum SyncWord : uint32_t {
        SYNC_WORD_WRITE = 0x01,
        SYNC_WORD_READ = 0x02,
    };

    int Wait(uint32_t bitset, int64_t timeoutNanoSec);
    int Wake(uint32_t bitset);

private:
    int FutexWait(uint32_t bitset, int64_t timeoutNanoSec);
    void TimeoutToRealtime(int64_t timeout, struct timespec &realtime);
    std::atomic<uint32_t> *syncAddr_;
};
} // namespace Base
} // namespace HDI
} // namespace OHOS

#endif /* HDI_SHARED_MEM_QUEUE_SYNCER_H */
