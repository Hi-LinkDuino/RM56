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

#include "power_interface_impl.h"

#include <atomic>
#include <hdf_base.h>
#include <file_ex.h>
#include <sys/eventfd.h>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <cstdlib>
#include <thread>
#include "errors.h"
#include "hdf_sbuf.h"
#include "pubdef.h"
#include "utils/hdf_log.h"
#include "hdf_device_desc.h"
#include "hdf_remote_service.h"
#include "unique_fd.h"

namespace OHOS {
namespace HDI {
namespace Power {
namespace V1_0 {
static constexpr const char * const SUSPEND_STATE = "mem";
static constexpr const char * const SUSPEND_STATE_PATH = "/sys/power/state";
static constexpr const char * const LOCK_PATH = "/sys/power/wake_lock";
static constexpr const char * const UNLOCK_PATH = "/sys/power/wake_unlock";
static constexpr const char * const WAKEUP_COUNT_PATH = "/sys/power/wakeup_count";
static std::chrono::milliseconds waitTime_(100); // {100ms};
static std::mutex mutex_;
static std::mutex suspendMutex_;
static std::condition_variable suspendCv_;
static std::unique_ptr<std::thread> daemon_;
static std::atomic_bool suspending_;
static std::atomic_bool suspendRetry_;
static sptr<IPowerHdiCallback> callback_;
static UniqueFd wakeupCountFd;
static void AutoSuspendLoop();
static int32_t DoSuspend();
static std::string ReadWakeCount();
static bool WriteWakeCount(const std::string& count);
static void NotifyCallback(int code);

int32_t PowerInterfaceImpl::RegisterCallback(const sptr<IPowerHdiCallback>& ipowerHdiCallback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    callback_ = ipowerHdiCallback;
    return HDF_SUCCESS;
}

int32_t PowerInterfaceImpl::StartSuspend()
{
    std::lock_guard<std::mutex> lock(mutex_);
    suspendRetry_ = true;
    if (suspending_) {
        suspendCv_.notify_one();
        return HDF_SUCCESS;
    }
    suspending_ = true;
    daemon_ = std::make_unique<std::thread>(&AutoSuspendLoop);
    daemon_->detach();
    return HDF_SUCCESS;
}

void AutoSuspendLoop()
{
    auto suspendLock = std::unique_lock(suspendMutex_);
    while (true) {
        std::this_thread::sleep_for(waitTime_);

        const std::string wakeupCount = ReadWakeCount();
        if (wakeupCount.empty()) {
            continue;
        }
        if (!suspendRetry_) {
            suspendCv_.wait(suspendLock);
        }
        if (!WriteWakeCount(wakeupCount)) {
            continue;
        }

        NotifyCallback(CMD_ON_SUSPEND);
        DoSuspend();
        NotifyCallback(CMD_ON_WAKEUP);
    }
    suspending_ = false;
    suspendRetry_ = false;
}

int32_t DoSuspend()
{
    std::lock_guard<std::mutex> lock(mutex_);
    UniqueFd suspendStateFd(TEMP_FAILURE_RETRY(open(SUSPEND_STATE_PATH, O_RDWR | O_CLOEXEC)));
    if (suspendStateFd < 0) {
        return HDF_FAILURE;
    }
    bool ret = false;
    do {
        ret = SaveStringToFd(suspendStateFd, SUSPEND_STATE);
    } while (!ret && (errno == EINTR || errno == EBUSY));

    if (!ret) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

void NotifyCallback(int code)
{
    if (callback_ == nullptr) {
        return;
    }
    switch (code) {
        case CMD_ON_SUSPEND:
            callback_->OnSuspend();
            break;
        case CMD_ON_WAKEUP:
            callback_->OnWakeup();
            break;
        default:
            break;
    }
}

int32_t PowerInterfaceImpl::StopSuspend()
{
    suspendRetry_ = false;

    return HDF_SUCCESS;
}

int32_t PowerInterfaceImpl::ForceSuspend()
{
    suspendRetry_ = false;

    NotifyCallback(CMD_ON_SUSPEND);
    DoSuspend();
    NotifyCallback(CMD_ON_WAKEUP);
    return HDF_SUCCESS;
}

int32_t PowerInterfaceImpl::SuspendBlock(const std::string& name)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (name.empty()) {
        return HDF_ERR_INVALID_PARAM;
    }
    UniqueFd fd(TEMP_FAILURE_RETRY(open(LOCK_PATH, O_RDWR | O_CLOEXEC)));
    bool ret = SaveStringToFd(fd, name);
    if (!ret) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t PowerInterfaceImpl::SuspendUnblock(const std::string& name)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (name.empty()) {
        return HDF_ERR_INVALID_PARAM;
    }
    UniqueFd fd(TEMP_FAILURE_RETRY(open(UNLOCK_PATH, O_RDWR | O_CLOEXEC)));
    bool ret = SaveStringToFd(fd, name);
    if (!ret) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

std::string ReadWakeCount()
{
    if (wakeupCountFd < 0) {
        wakeupCountFd = UniqueFd(TEMP_FAILURE_RETRY(open(WAKEUP_COUNT_PATH, O_RDWR | O_CLOEXEC)));
    }
    std::string wakeupCount;
    LoadStringFromFd(wakeupCountFd, wakeupCount);

    return wakeupCount;
}

bool WriteWakeCount(const std::string& count)
{
    if (wakeupCountFd < 0) {
        wakeupCountFd = UniqueFd(TEMP_FAILURE_RETRY(open(WAKEUP_COUNT_PATH, O_RDWR | O_CLOEXEC)));
    }
    bool ret = SaveStringToFd(wakeupCountFd, count.c_str());
    return ret;
}

static void LoadSystemInfo(const std::string& path, std::string& info)
{
    UniqueFd fd(TEMP_FAILURE_RETRY(open(path.c_str(), O_RDWR | O_CLOEXEC)));
    std::string str;
    if (fd >= 0) {
        bool ret = LoadStringFromFd(fd, str);
        if (!ret) {
            str = "# Failed to read";
        }
    } else {
        str = "# Failed to open";
    }
    info.append(path);
    info.append(": " + str + "\n");
}

int32_t PowerInterfaceImpl::PowerDump(std::string& info)
{
    std::string dumpInfo("");
    LoadSystemInfo(SUSPEND_STATE_PATH, dumpInfo);
    LoadSystemInfo(WAKEUP_COUNT_PATH, dumpInfo);
    LoadSystemInfo(LOCK_PATH, dumpInfo);
    LoadSystemInfo(UNLOCK_PATH, dumpInfo);
    info = dumpInfo;

    return HDF_SUCCESS;
}
} // V1_0
} // Power
} // HDI
} // OHOS
