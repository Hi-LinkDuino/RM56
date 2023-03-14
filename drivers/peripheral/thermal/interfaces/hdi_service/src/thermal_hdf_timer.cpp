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

#include "thermal_hdf_timer.h"
#include <cerrno>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <hdf_log.h>
#include <hdf_base.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <linux/netlink.h>

#define HDF_LOG_TAG ThermalHdfTimer

namespace OHOS {
namespace HDI {
namespace Thermal {
namespace V1_0 {
namespace {
const int32_t ERR_INVALID_FD = -1;
const int32_t MS_PER_SECOND = 1000;
const std::string THERMAL_SIMULATION_TAG = "sim_tz";
}
ThermalHdfTimer::ThermalHdfTimer(const std::shared_ptr<ThermalSimulationNode> &node,
    const std::shared_ptr<ThermalZoneManager> &thermalZoneMgr)
{
    node_ = node;
    thermalZoneMgr_ = thermalZoneMgr;
    reportTime_ = 0;
}

void ThermalHdfTimer::SetThermalEventCb(const sptr<IThermalCallback> &thermalCb)
{
    thermalCb_ = thermalCb;
}

void ThermalHdfTimer::SetSimluationFlag()
{
    auto baseConfigList = ThermalHdfConfig::GetInsance().GetBaseConfig()->GetBaseItem();
    if (baseConfigList.empty()) {
        HDF_LOGE("%{public}s: baseConfigList is empty", __func__);
        return;
    }
    auto baseIter = std::find(baseConfigList.begin(), baseConfigList.end(), THERMAL_SIMULATION_TAG);
    if (baseIter != baseConfigList.end()) {
        isSim_ = atoi(baseIter->value.c_str());
        HDF_LOGI("%{public}s: isSim value:%{public}d", __func__, isSim_);
    } else {
        HDF_LOGI("%{public}s: not found", __func__);
    }
}

void ThermalHdfTimer::SetSimFlag(int32_t flag)
{
    isSim_ = flag;
}

int32_t ThermalHdfTimer::GetSimluationFlag()
{
    return isSim_;
}

int32_t ThermalHdfTimer::CreateProviderFd()
{
    timerFd_ = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timerFd_ == ERR_INVALID_FD) {
        HDF_LOGE("%{public}s epoll create failed, epFd_ is invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("%{public}s: interval %{public}d", __func__, thermalZoneMgr_->maxCd_);
    SetTimerInterval(thermalZoneMgr_->maxCd_, timerFd_);
    fcntl(timerFd_, F_SETFL, O_NONBLOCK);
    callbackHandler_.insert(std::make_pair(timerFd_, &ThermalHdfTimer::TimerProviderCallback));
    if (RegisterCallback(timerFd_, EVENT_TIMER_FD, epFd_)) {
        HDF_LOGI("%{public}s register Timer event failed", __func__);
    }

    HDF_LOGI("%{public}s return", __func__);
    return HDF_SUCCESS;
}

int32_t ThermalHdfTimer::RegisterCallback(const int32_t fd, const EventType et, int32_t epfd)
{
    struct epoll_event ev;

    ev.events = EPOLLIN;
    if (et == EVENT_TIMER_FD) {
        ev.events |= EPOLLWAKEUP;
    }
    HDF_LOGI("%{public}d, %{public}d", epfd, fd);
    ev.data.ptr = reinterpret_cast<void*>(this);
    ev.data.fd = fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == HDF_FAILURE) {
        HDF_LOGE("%{public}s: epoll_ctl failed, error num =%{public}d",
            __func__, errno);
        return HDF_FAILURE;
    }
    HDF_LOGI("%{public}s return", __func__);
    return HDF_SUCCESS;
}

void ThermalHdfTimer::TimerProviderCallback(void *service)
{
    unsigned long long timers;

    if (read(timerFd_, &timers, sizeof(timers)) == -1) {
        HDF_LOGE("%{public}s read timerFd_ failed", __func__);
        return;
    }

    reportTime_ = reportTime_ + 1;
    ReportThermalData();
    ResetCount();
    return;
}

void ThermalHdfTimer::SetTimerInterval(int32_t interval, int32_t timerfd)
{
    struct itimerspec itval;

    if (timerfd == ERR_INVALID_FD) {
        return;
    }

    timerInterval_ = interval;

    if (interval < 0) {
        interval = 0;
    }

    itval.it_interval.tv_sec = interval / MS_PER_SECOND;
    itval.it_interval.tv_nsec = 0;
    itval.it_value.tv_sec = interval / MS_PER_SECOND;
    itval.it_value.tv_nsec = 0;
    if (timerfd_settime(timerfd, 0, &itval, nullptr) == -1) {
        HDF_LOGE("%{public}s: timer failed\n", __func__);
    }
    HDF_LOGI("return");
}

int32_t ThermalHdfTimer::InitProviderTimer()
{
    int32_t ret;
    epFd_ = epoll_create1(EPOLL_CLOEXEC);

    ret = CreateProviderFd();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed to create polling fd", __func__);
        return ret;
    }
    return HDF_SUCCESS;
}

int32_t ThermalHdfTimer::LoopingThreadEntry(void *arg, int32_t epfd)
{
    int32_t nevents = 0;
    size_t eventct = callbackHandler_.size();
    struct epoll_event events[eventct];
    HDF_LOGI("%{public}s: %{public}d, %{public}zu", __func__, epfd, eventct);
    while (true) {
        nevents = epoll_wait(epfd, events, eventct, -1);
        if (nevents == -1) {
            continue;
        }
        for (int32_t n = 0; n < nevents; ++n) {
            if (events[n].data.ptr) {
                ThermalHdfTimer *func = const_cast<ThermalHdfTimer *>(this);
                (callbackHandler_.find(events[n].data.fd)->second)(func, arg);
            }
        }
    }
}

void ThermalHdfTimer::Run(void *service, int32_t epfd)
{
    std::make_unique<std::thread>(&ThermalHdfTimer::LoopingThreadEntry, this, service, epfd)->detach();
}

void ThermalHdfTimer::StartThread(void *service)
{
    int32_t ret = InitProviderTimer();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s init Timer failed, ret: %{public}d", __func__, ret);
        return;
    }
    Run(service, epFd_);
}

int32_t ThermalHdfTimer::Init()
{
    StartThread(this);
    return HDF_SUCCESS;
}

void ThermalHdfTimer::ReportThermalData()
{
    if (thermalCb_ == nullptr) {
        HDF_LOGE("%{public}s: check thermalCb_ failed", __func__);
        return;
    }

    thermalZoneMgr_->ReportThermalZoneData(reportTime_, multipleList_);
    tzInfoEvent_ = thermalZoneMgr_->tzInfoAcaualEvent_;
    // callback thermal event
    thermalCb_->OnThermalDataEvent(tzInfoEvent_);
}

void ThermalHdfTimer::ResetCount()
{
    HDF_LOGI("%{public}s: multipleList_:%{public}zu", __func__, multipleList_.size());
    if (multipleList_.empty()) return;

    int32_t maxValue = *(std::max_element(multipleList_.begin(), multipleList_.end()));
    if (reportTime_ == maxValue) {
        HDF_LOGI("%{public}s: reportTime:%{public}d", __func__, reportTime_);
        reportTime_ = 0;
    }
    tzInfoEvent_.info.clear();
}

void ThermalHdfTimer::DumpSensorConfigInfo()
{
    auto sensorTypeMap = ThermalHdfConfig::GetInsance().GetSensorTypeMap();
    for (auto sensorIter : sensorTypeMap) {
        HDF_LOGI("%{public}s: groupName %{public}s", __func__, sensorIter.first.c_str());
        HDF_LOGI("%{public}s: interval %{public}d", __func__, sensorIter.second->GetInterval());
        HDF_LOGI("%{public}s: multiple %{public}d", __func__, sensorIter.second->multiple_);
        for (auto tzIter : sensorIter.second->GetXMLThermalZoneInfo()) {
            HDF_LOGI("%{public}s: type %{public}s", __func__, tzIter.type.c_str());
            HDF_LOGI("%{public}s: replace %{public}s", __func__, tzIter.replace.c_str());
        }
        for (auto tnIter : sensorIter.second->GetXMLThermalNodeInfo()) {
            HDF_LOGI("%{public}s: type %{public}s", __func__, tnIter.type.c_str());
            HDF_LOGI("%{public}s: path %{public}s", __func__, tnIter.path.c_str());
        }
        for (auto dataIter : sensorIter.second->thermalDataList_) {
            HDF_LOGI("%{public}s: data type %{public}s", __func__, dataIter.type.c_str());
            HDF_LOGI("%{public}s: data temp path %{public}s", __func__, dataIter.tempPath.c_str());
        }
    }
}
} // V1_0
} // Thermal
} // HDI
} // OHOS