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
#include <cerrno>
#include <climits>
#include <cstring>
#include <ctime>
#include <datetime_ex.h>
#include <hdf_base.h>
#include <hdf_log.h>
#include <hdi_smq_syncer.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/futex.h>

#define HDF_LOG_TAG smq_syncer

namespace OHOS {
namespace HDI {
namespace Base {
SharedMemQueueSyncer::SharedMemQueueSyncer(std::atomic<uint32_t> *syncerPtr) : syncAddr_(syncerPtr)
{
}

int SharedMemQueueSyncer::Wait(uint32_t bitset, int64_t timeoutNanoSec)
{
    int ret;
    while (true) {
        ret = FutexWait(bitset, timeoutNanoSec);
        if (ret == -EINTR || ret == -EAGAIN) {
            HDF_LOGE("wait smq futex %{public}d,try again", ret);
            continue;
        }
        break;
    }

    return ret;
}

int SharedMemQueueSyncer::FutexWait(uint32_t bitset, int64_t timeoutNanoSec)
{
    // clean wait bit
    uint32_t syncWordOld = std::atomic_fetch_and(syncAddr_, ~bitset);
    // if sync bit already set, not nedd futex wait
    if (syncWordOld & bitset) {
        return HDF_SUCCESS;
    }
    // futex will check sync word equal this expected val or not. If equal, sleep to wait, else return EAGAIN.
    uint32_t valParm = syncWordOld & (~bitset);
    int status;
    if (timeoutNanoSec > 0) {
        struct timespec waitTime;
        TimeoutToRealtime(timeoutNanoSec, waitTime);
        status = syscall(__NR_futex, syncAddr_, FUTEX_WAIT_BITSET, valParm, &waitTime, NULL, bitset);
    } else {
        status = syscall(__NR_futex, syncAddr_, FUTEX_WAIT_BITSET, valParm, NULL, NULL, bitset);
    }
    if (status == 0) {
        syncWordOld = std::atomic_fetch_and(syncAddr_, ~bitset);
        if ((syncWordOld & bitset) == 0) {
            return -EINTR;
        }

        return status;
    }
    status = -errno;
    if (status != ETIMEDOUT) {
        HDF_LOGE("failed to wait smq futex, %{publich}d", status);
    }
    return status;
}

int SharedMemQueueSyncer::Wake(uint32_t bitset)
{
    uint32_t syncWordOld = std::atomic_fetch_or(syncAddr_, bitset);
    // if sync bit already set, not nedd futex wake
    if (syncWordOld & bitset) {
        return HDF_SUCCESS;
    }

    int ret = syscall(__NR_futex, syncAddr_, FUTEX_WAKE_BITSET, INT_MAX, 0, 0, bitset);
    if (ret < 0) {
        HDF_LOGE("failed to wakeup smq futex, %{public}d", errno);
        return -errno;
    }

    return HDF_SUCCESS;
}

void SharedMemQueueSyncer::TimeoutToRealtime(int64_t timeout, struct timespec &realtime)
{
    constexpr int64_t nano = SEC_TO_NANOSEC;

    clock_gettime(CLOCK_MONOTONIC, &realtime);
    int64_t timeoutNanoSec = timeout % nano;
    int64_t timeoutSec = timeout / nano;

    if (timeoutNanoSec + realtime.tv_nsec >= nano) {
        realtime.tv_nsec = (timeoutNanoSec + realtime.tv_nsec) - nano;
        realtime.tv_sec += timeoutSec + 1;
    } else {
        realtime.tv_nsec += timeoutNanoSec;
        realtime.tv_sec += timeoutSec;
    }
}
} // namespace Base
} // namespace HDI
} // namespace OHOS