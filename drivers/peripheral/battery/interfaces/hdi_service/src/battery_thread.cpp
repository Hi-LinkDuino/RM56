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

#include "battery_thread.h"
#include <cerrno>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <linux/netlink.h>
#include "hdf_base.h"
#include "battery_log.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
namespace {
constexpr int32_t UEVENT_BUFF_SIZE = (64 * 1024);
constexpr int32_t UEVENT_RESERVED_SIZE = 2;
constexpr int32_t UEVENT_MSG_LEN = (2 * 1024);
constexpr int32_t TIMER_INTERVAL = 10;
constexpr int32_t TIMER_FAST_SEC = 2;
constexpr int32_t TIMER_SLOW_SEC = 10;
constexpr int32_t SEC_TO_MSEC = 1000;
const std::string POWER_SUPPLY = "SUBSYSTEM=power_supply";
}
static sptr<IBatteryCallback> g_callback;

void BatteryThread::InitCallback(const sptr<IBatteryCallback>& callback)
{
    g_callback = callback;
    BATTERY_HILOGD(FEATURE_BATT_INFO, "g_callback is %{public}p", callback.GetRefPtr());
}

int32_t BatteryThread::OpenUeventSocket()
{
    int32_t bufferSize = UEVENT_BUFF_SIZE;
    struct sockaddr_nl address = {
        .nl_family = AF_NETLINK,
        .nl_pid = getpid(),
        .nl_groups = 0xffffffff
    };

    int32_t fd = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, NETLINK_KOBJECT_UEVENT);
    if (fd == INVALID_FD) {
        BATTERY_HILOGE(COMP_HDI, "open uevent socket failed, fd is invalid");
        return INVALID_FD;
    }

    int32_t ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUFFORCE, &bufferSize, sizeof(bufferSize));
    if (ret < 0) {
        BATTERY_HILOGE(COMP_HDI, "set socket opt failed, ret: %{public}d", ret);
        close(fd);
        return INVALID_FD;
    }

    ret = bind(fd, reinterpret_cast<const struct sockaddr*>(&address), sizeof(struct sockaddr_nl));
    if (ret < 0) {
        BATTERY_HILOGE(COMP_HDI, "bind socket address failed, ret: %{public}d", ret);
        close(fd);
        return INVALID_FD;
    }
    return fd;
}

int32_t BatteryThread::RegisterCallback(int32_t fd, EventType et)
{
    struct epoll_event ev = {0};

    ev.events = EPOLLIN;
    if (et == EVENT_TIMER_FD) {
        ev.events |= EPOLLWAKEUP;
    }

    ev.data.ptr = reinterpret_cast<void*>(this);
    ev.data.fd = fd;
    if (epoll_ctl(epFd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
        BATTERY_HILOGE(COMP_HDI, "epoll_ctl failed, error num =%{public}d", errno);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

void BatteryThread::SetTimerInterval(int32_t interval)
{
    struct itimerspec itval = {};

    if (timerFd_ == INVALID_FD) {
        BATTERY_HILOGW(COMP_HDI, "invalid timer fd");
        return;
    }

    timerInterval_ = interval;
    if (interval < 0) {
        interval = 0;
    }

    itval.it_interval.tv_sec = interval;
    itval.it_interval.tv_nsec = 0;
    itval.it_value.tv_sec = interval;
    itval.it_value.tv_nsec = 0;

    if (timerfd_settime(timerFd_, 0, &itval, nullptr) == -1) {
        BATTERY_HILOGE(COMP_HDI, "timerfd_settime failed");
    }
}

void BatteryThread::UpdateEpollInterval(const int32_t chargeState)
{
    int32_t interval;
    if ((chargeState != PowerSupplyProvider::CHARGE_STATE_NONE) &&
        (chargeState != PowerSupplyProvider::CHARGE_STATE_RESERVED)) {
        interval = TIMER_FAST_SEC;
        epollInterval_ = interval * SEC_TO_MSEC;
    } else {
        interval = TIMER_SLOW_SEC;
        epollInterval_ = -1;
    }

    if ((interval != timerInterval_) && (timerInterval_ > 0)) {
        SetTimerInterval(interval);
    }
}

int32_t BatteryThread::InitUevent()
{
    ueventFd_ = OpenUeventSocket();
    if (ueventFd_ == INVALID_FD) {
        BATTERY_HILOGE(COMP_HDI, "open uevent socket failed, fd is invalid");
        return HDF_ERR_BAD_FD;
    }

    fcntl(ueventFd_, F_SETFL, O_NONBLOCK);
    callbacks_.insert(std::make_pair(ueventFd_, &BatteryThread::UeventCallback));

    if (RegisterCallback(ueventFd_, EVENT_UEVENT_FD)) {
        BATTERY_HILOGE(COMP_HDI, "register Uevent event failed");
        return HDF_ERR_BAD_FD;
    }
    return HDF_SUCCESS;
}

int32_t BatteryThread::Init([[maybe_unused]] void* service)
{
    provider_ = std::make_unique<PowerSupplyProvider>();
    if (provider_ != nullptr) {
        provider_->InitBatteryPath();
        provider_->InitPowerSupplySysfs();
    }

    epFd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epFd_ == INVALID_FD) {
        BATTERY_HILOGE(COMP_HDI, "epoll create failed, epFd_ is invalid");
        return HDF_ERR_BAD_FD;
    }

    InitTimer();
    InitUevent();

    return HDF_SUCCESS;
}

int32_t BatteryThread::UpdateWaitInterval()
{
    return HDF_FAILURE;
}

int32_t BatteryThread::InitTimer()
{
    timerFd_ = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timerFd_ == INVALID_FD) {
        BATTERY_HILOGE(COMP_HDI, "epoll create failed, timerFd_ is invalid");
        return HDF_ERR_BAD_FD;
    }

    SetTimerInterval(TIMER_INTERVAL);
    fcntl(timerFd_, F_SETFL, O_NONBLOCK);
    callbacks_.insert(std::make_pair(timerFd_, &BatteryThread::TimerCallback));

    if (RegisterCallback(timerFd_, EVENT_TIMER_FD)) {
        BATTERY_HILOGE(COMP_HDI, "register Timer event failed");
    }

    return HDF_SUCCESS;
}

void BatteryThread::TimerCallback(void* service)
{
    uint64_t timers;

    if (read(timerFd_, &timers, sizeof(timers)) == -1) {
        BATTERY_HILOGE(COMP_HDI, "read timerFd_ failed");
        return;
    }

    UpdateBatteryInfo(service);
}

void BatteryThread::UeventCallback(void* service)
{
    char msg[UEVENT_MSG_LEN + UEVENT_RESERVED_SIZE] = { 0 };

    ssize_t len = recv(ueventFd_, msg, UEVENT_MSG_LEN, 0);
    if (len < 0 || len >= UEVENT_MSG_LEN) {
        BATTERY_HILOGI(COMP_HDI, "recv return msg is invalid, len: %{public}zd", len);
        return;
    }

    // msg separator
    msg[len] = '\0';
    msg[len + 1] = '\0';
    if (!IsPowerSupplyEvent(msg)) {
        return;
    }
    UpdateBatteryInfo(service, msg);
}

void BatteryThread::UpdateBatteryInfo(void* service, char* msg)
{
    BatteryInfo event = {};
    std::unique_ptr<BatterydInfo> batteryInfo = std::make_unique<BatterydInfo>();
    if (batteryInfo == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "make_unique BatterydInfo error");
        return;
    }

    provider_->ParseUeventToBatterydInfo(msg, batteryInfo.get());
    event.capacity = batteryInfo->capacity_;
    event.voltage= batteryInfo->voltage_;
    event.temperature = batteryInfo->temperature_;
    event.healthState = batteryInfo->healthState_;
    event.pluggedType = batteryInfo->pluggedType_;
    event.pluggedMaxCurrent = batteryInfo->pluggedMaxCurrent_;
    event.pluggedMaxVoltage = batteryInfo->pluggedMaxVoltage_;
    event.chargeState = batteryInfo->chargeState_;
    event.chargeCounter = batteryInfo->chargeCounter_;
    event.present = batteryInfo->present_;
    event.technology = batteryInfo->technology_;

    BATTERY_HILOGD(FEATURE_BATT_INFO, "BatteryInfo capacity=%{public}d, voltage=%{public}d, temperature=%{public}d, " \
        "healthState=%{public}d, pluggedType=%{public}d, pluggedMaxCurrent=%{public}d, " \
        "pluggedMaxVoltage=%{public}d, chargeState=%{public}d, chargeCounter=%{public}d, present=%{public}d, " \
        "technology=%{public}s", batteryInfo->capacity_, batteryInfo->voltage_,
        batteryInfo->temperature_, batteryInfo->healthState_, batteryInfo->pluggedType_,
        batteryInfo->pluggedMaxCurrent_, batteryInfo->pluggedMaxVoltage_, batteryInfo->chargeState_,
        batteryInfo->chargeCounter_, batteryInfo->present_, batteryInfo->technology_.c_str());

    if (g_callback != nullptr) {
        g_callback->Update(event);
    } else {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "g_callback is nullptr");
    }
}

void BatteryThread::UpdateBatteryInfo(void* service)
{
    BatteryInfo event = {};
    std::unique_ptr<BatterydInfo> batteryInfo = std::make_unique<BatterydInfo>();
    if (batteryInfo == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "make_unique BatterydInfo error");
        return;
    }

    provider_->UpdateInfoByReadSysFile(batteryInfo.get());
    event.capacity = batteryInfo->capacity_;
    event.voltage= batteryInfo->voltage_;
    event.temperature = batteryInfo->temperature_;
    event.healthState = batteryInfo->healthState_;
    event.pluggedType = batteryInfo->pluggedType_;
    event.pluggedMaxCurrent = batteryInfo->pluggedMaxCurrent_;
    event.pluggedMaxVoltage = batteryInfo->pluggedMaxVoltage_;
    event.chargeState = batteryInfo->chargeState_;
    event.chargeCounter = batteryInfo->chargeCounter_;
    event.present = batteryInfo->present_;
    event.technology = batteryInfo->technology_;

    BATTERY_HILOGI(FEATURE_BATT_INFO, "BatteryInfo capacity=%{public}d, voltage=%{public}d, temperature=%{public}d, " \
        "healthState=%{public}d, pluggedType=%{public}d, pluggedMaxCurrent=%{public}d, " \
        "pluggedMaxVoltage=%{public}d, chargeState=%{public}d, chargeCounter=%{public}d, present=%{public}d, " \
        "technology=%{public}s", batteryInfo->capacity_, batteryInfo->voltage_,
        batteryInfo->temperature_, batteryInfo->healthState_, batteryInfo->pluggedType_,
        batteryInfo->pluggedMaxCurrent_, batteryInfo->pluggedMaxVoltage_, batteryInfo->chargeState_,
        batteryInfo->chargeCounter_, batteryInfo->present_, batteryInfo->technology_.c_str());

    if (g_callback != nullptr) {
        g_callback->Update(event);
    } else {
        BATTERY_HILOGI(FEATURE_BATT_INFO, "g_callback is nullptr");
    }
}

bool BatteryThread::IsPowerSupplyEvent(const char* msg)
{
    while (*msg) {
        if (!strcmp(msg, POWER_SUPPLY.c_str())) {
            return true;
        }
        while (*msg++) {} // move to next
    }

    return false;
}

int32_t BatteryThread::LoopingThreadEntry(void* arg)
{
    int32_t nevents = 0;
    size_t size = callbacks_.size();
    struct epoll_event events[size];

    BATTERY_HILOGI(COMP_HDI, "start battery thread looping");
    while (true) {
        if (!nevents) {
            CycleMatters();
        }

        HandleStates();

        int32_t timeout = epollInterval_;
        int32_t waitTimeout = UpdateWaitInterval();
        if ((timeout < 0) || (waitTimeout > 0 && waitTimeout < timeout)) {
            timeout = waitTimeout;
        }
        BATTERY_HILOGD(COMP_HDI, "timeout=%{public}d, nevents=%{public}d", timeout, nevents);

        nevents = epoll_wait(epFd_, events, static_cast<int32_t>(size), timeout);
        if (nevents <= 0) {
            continue;
        }

        for (int32_t n = 0; n < nevents; ++n) {
            if (events[n].data.ptr) {
                auto* func = const_cast<BatteryThread*>(this);
                (callbacks_.find(events[n].data.fd)->second)(func, arg);
            }
        }
    }
}

void BatteryThread::StartThread(void* service)
{
    Init(service);
    Run(service);
}

void BatteryThread::Run(void* service)
{
    std::make_unique<std::thread>(&BatteryThread::LoopingThreadEntry, this, service)->detach();
}
} // namespace V1_0
} // namespace Battery
} // namespace HDI
} // namespace OHOS
