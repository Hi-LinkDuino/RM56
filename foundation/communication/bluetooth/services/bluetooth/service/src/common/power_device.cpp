/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "power_device.h"
#include <cstring>
#include <memory>
#include <mutex>
#include "btm.h"
#include "log.h"
#include "message.h"
#include "power_state_machine.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
struct PowerDevice::impl {
public:
    impl(const RawAddress rawAddr, utility::Dispatcher &dispatcher):rawAddr_(rawAddr), dispatcher_(dispatcher){};
    ~impl(){};
    const RawAddress rawAddr_;
    utility::Dispatcher &dispatcher_;
    std::map<std::string, RequestStatus> requestPower_ {};
    std::unique_ptr<PowerStateMachine> psm_ = nullptr;
    std::pair<PowerSsrLevel, PowerModeLevel> requestPowerLevel_ =
        std::pair<PowerSsrLevel, PowerModeLevel>(PowerSsrLevel::NO_ACTION, PowerModeLevel::NO_ACTION);
    std::pair<PowerSsrLevel, PowerModeLevel> controlPowerLevel_ =
        std::pair<PowerSsrLevel, PowerModeLevel>(PowerSsrLevel::NO_ACTION, PowerModeLevel::NO_ACTION);
    int controlPowerMode_ {};
    int controlInterval_ {};
    std::unique_ptr<PowerTimer> sniffDelayTimer_ = nullptr;
    std::mutex mutex_ {};

    BT_DISALLOW_COPY_AND_ASSIGN(impl);
};

PowerDevice::PowerDevice(const RawAddress rawAddr, utility::Dispatcher &dispatcher)
    : pimpl(std::make_unique<PowerDevice::impl>(rawAddr, dispatcher))
{
    pimpl->psm_ = std::make_unique<PowerStateMachine>();
    pimpl->psm_->Init(*this);

    pimpl->sniffDelayTimer_ = std::make_unique<PowerTimer>(std::bind(
        [&]()->void {
            std::weak_ptr<PowerDevice> weakDevice = shared_from_this();
            pimpl->dispatcher_.PostTask(std::bind(&bluetooth::PowerDevice::DelayTimeoutCallback, weakDevice));
        }
    ));
}

PowerDevice::~PowerDevice()
{
    pimpl->sniffDelayTimer_->Stop();
}

/// update request power function
void PowerDevice::SetRequestPower(const std::string &profileName, const RequestStatus status) const
{
    pimpl->requestPower_[profileName] = status;
}

void PowerDevice::DeleteRequestPower(const std::string &profileName) const
{
    pimpl->requestPower_.erase(profileName);
}

void PowerDevice::SetPowerMode()
{
    PowerInfo maxPower = CalcMaxPower();
    if (maxPower.powerMode_ == PowerModeLevel::NO_ACTION) {
        return;
    }
    if (maxPower.powerMode_ == PowerModeLevel::LEVEL_ACTIVE) {
        SetActiveMode();
    } else {
        SetSniffMode(maxPower);
    }
}

/// set power mode interface, use by SetPowerMode() / DelayTimeoutCallback()
void PowerDevice::SetActiveMode()
{
    LOG_DEBUG("PM_: SetActiveMode");
    StopDelayTimer();  // Stop Sniff Delay Timer.
    utility::Message msg(PowerStateMachine::MSG_PM_SET_ACTIVE);
    pimpl->psm_->ProcessMessage(msg);
}

void PowerDevice::SetSniffMode(PowerInfo requestPower)
{
    LOG_DEBUG("PM_: SetSniffMode");
    // Because the hardware does not support resetting the sniff parameter in the sniff state,
    // it exits the sniff first, and then delays to enter the sniff.
    PowerModeLevel controlPowerMode = GetControlPowerLevel().second;
    if (requestPower.powerMode_ != controlPowerMode &&
        (controlPowerMode == PowerModeLevel::LEVEL_LOW ||
        controlPowerMode == PowerModeLevel::LEVEL_MID ||
        controlPowerMode == PowerModeLevel::LEVEL_HIG)) {
        LOG_DEBUG("PM_:::SetSniffMode() contorl is in sniff state, to reset the sniff parameter, you need to exit "
                  "sniff state first\n");
        SetActiveMode();
    }

    if (GetDelayTimerRemainMs() <= static_cast<uint64_t>(requestPower.timeout_)) {
        LOG_DEBUG("PM_: Reset Timer: timeoutMs: %{public}d\n", requestPower.timeout_);
        StopDelayTimer();
        StartDelayTimer(static_cast<uint64_t>(requestPower.timeout_));
    }
}

BTPowerMode PowerDevice::GetPowerMode() const
{
    PowerModeLevel powerLevel = GetControlPowerLevel().second;
    BTPowerMode powerMode = BTPowerMode::MODE_ACTIVE;
    if (powerLevel == PowerModeLevel::LEVEL_ACTIVE) {
        powerMode = BTPowerMode::MODE_ACTIVE;
    } else if (powerLevel == PowerModeLevel::LEVEL_HIG) {
        powerMode = BTPowerMode::MODE_SNIFF_LEVEL_HIG;
    } else if (powerLevel == PowerModeLevel::LEVEL_MID) {
        powerMode = BTPowerMode::MODE_SNIFF_LEVEL_MID;
    } else if (powerLevel == PowerModeLevel::LEVEL_LOW) {
        powerMode = BTPowerMode::MODE_SNIFF_LEVEL_LOW;
    }
    return powerMode;
}

void PowerDevice::ModeChangeCallBack(uint8_t status, uint8_t currentMode, uint16_t interval)
{
    if (status == 0) {
        LOG_DEBUG("PM_: %{public}s, status: %{public}d, currentMode: %{public}d, interval: %{public}d\n", __FUNCTION__, status, currentMode, interval);
        /// update state
        if (currentMode == BTM_PM_ACTIVE_MODE) {  // current Mode Active
            UpdateControlSniffSubrating(PowerSsrLevel::NO_ACTION);
            UpdatecontrolPowerLevel(PowerModeLevel::LEVEL_ACTIVE);
            utility::Message msg(PowerStateMachine::MSG_PM_MODE_CHANGE_ACTIVE);
            pimpl->psm_->ProcessMessage(msg);
        } else if (currentMode == BTM_PM_SNIFF_MODE) {  // current Mode Sniff
            if (interval <= SNIFF_LEVEL_LOW_MAX_INTERVAL && interval >= SNIFF_LEVEL_LOW_MIN_INTERVAL) {
                UpdatecontrolPowerLevel(PowerModeLevel::LEVEL_LOW);
            } else if (interval <= SNIFF_LEVEL_MID_MAX_INTERVAL && interval >= SNIFF_LEVEL_MID_MIN_INTERVAL) {
                UpdatecontrolPowerLevel(PowerModeLevel::LEVEL_MID);
            } else if (interval <= SNIFF_LEVEL_HIG_MAX_INTERVAL && interval >= SNIFF_LEVEL_HIG_MIN_INTERVAL) {
                UpdatecontrolPowerLevel(PowerModeLevel::LEVEL_HIG);
            }
            utility::Message msg(PowerStateMachine::MSG_PM_MODE_CHANGE_SNIFF);
            pimpl->psm_->ProcessMessage(msg);
        }
    } else {  /// hci status error
        LOG_DEBUG("PM_: %{public}s, hci status error: %{public}d\n", __FUNCTION__, status);
    }
}

void PowerDevice::SniffSubratingCompleteCallback(uint8_t status) const
{
    LOG_DEBUG("PM_: %{public}s, line: %{public}d\n", __FUNCTION__, __LINE__);
    if (status == 0) {
        UpdateControlSniffSubrating(GetRequestPowerLevel().first);
        utility::Message msg(PowerStateMachine::MSG_PM_SET_SUBRATING_COMPLETE);
        pimpl->psm_->ProcessMessage(msg);
    }
}

/// calc power level and ssr level
PowerInfo PowerDevice::CalcMaxPower() const
{
    PowerInfo maxPower = PowerInfo(PowerModeLevel::NO_ACTION, 0);
    for (auto &its : pimpl->requestPower_) {
        PowerInfo itSpec = PowerSpec::GetPowerSpec(its.first, its.second);
        if (itSpec.powerMode_ > maxPower.powerMode_) {
            maxPower = itSpec;
        }
    }
    return maxPower;
}

PowerSsrLevel PowerDevice::CalcLowestSsrLevel() const
{
    PowerSsrLevel lowestLevel = PowerSsrLevel::NO_ACTION;
    for (auto &its : pimpl->requestPower_) {
        PowerSsrLevel level = PowerSpec::GetPowerSsrLevel(its.first, its.second);
        if ((level != PowerSsrLevel::NO_ACTION) && (level < lowestLevel)) {
            lowestLevel = level;
        }
    }
    return lowestLevel;
}

/// btm power interface
int PowerDevice::BtmSetSniffSubrating(const PowerSsrParam &ssrParam) const
{
    LOG_DEBUG("PM_: %{public}s start, line: %{public}d\n", __FUNCTION__, __LINE__);
    BtAddr btAddr;
    (void)memset_s(&btAddr, sizeof(btAddr), 0, sizeof(btAddr));
    pimpl->rawAddr_.ConvertToUint8(btAddr.addr);

    BtmSniffSubrating btmSsrParam;
    btmSsrParam.maximumLatency = ssrParam.maxLat_;
    btmSsrParam.minimumLocalTimeout = ssrParam.minLocTo_;
    btmSsrParam.minimumRemoteTimeout = ssrParam.minRmtTo_;

    return BTM_SetSniffSubrating(&btAddr, &btmSsrParam);
}

int PowerDevice::BtmExitSniffMode() const
{
    LOG_DEBUG("PM_: %{public}s start, line: %{public}d\n", __FUNCTION__, __LINE__);
    BtAddr btAddr;
    (void)memset_s(&btAddr, sizeof(btAddr), 0, sizeof(btAddr));
    pimpl->rawAddr_.ConvertToUint8(btAddr.addr);
    return BTM_ExitSniffMode(&btAddr);
}

int PowerDevice::BtmEnterSniffMode(const PowerParam &param) const
{
    LOG_DEBUG("PM_: %{public}s start, line: %{public}d\n", __FUNCTION__, __LINE__);
    BtAddr btAddr;
    BtmSniffParam btmSniffParam;

    (void)memset_s(&btAddr, sizeof(btAddr), 0, sizeof(btAddr));
    pimpl->rawAddr_.ConvertToUint8(btAddr.addr);
    btmSniffParam.attempt = param.attempt_;
    btmSniffParam.maxInterval = param.maxInterval_;
    btmSniffParam.minInterval = param.minInterval_;
    btmSniffParam.timeout = param.timeout_;

    return BTM_EnterSniffMode(&btAddr, &btmSniffParam);
}

const std::pair<PowerSsrLevel, PowerModeLevel> &PowerDevice::GetRequestPowerLevel() const
{
    return pimpl->requestPowerLevel_;
}

void PowerDevice::SetRequestPowerLevel(const PowerSsrLevel ssr, const PowerModeLevel power) const
{
    pimpl->requestPowerLevel_ = std::pair<PowerSsrLevel, PowerModeLevel>(ssr, power);
}

const std::pair<PowerSsrLevel, PowerModeLevel> &PowerDevice::GetControlPowerLevel() const
{
    std::unique_lock<std::mutex> lock(pimpl->mutex_);
    return pimpl->controlPowerLevel_;
}

void PowerDevice::UpdateControlSniffSubrating(const PowerSsrLevel ssr) const
{
    std::unique_lock<std::mutex> lock(pimpl->mutex_);
    pimpl->controlPowerLevel_.first = ssr;
}

void PowerDevice::UpdatecontrolPowerLevel(const PowerModeLevel powerLevel) const
{
    std::unique_lock<std::mutex> lock(pimpl->mutex_);
    pimpl->controlPowerLevel_.second = powerLevel;
}

void PowerDevice::DelayTimeoutCallback(const std::weak_ptr<PowerDevice>& weakDevice)
{
    std::shared_ptr<PowerDevice> sharedDevice = weakDevice.lock();
    if (sharedDevice != nullptr) {
        LOG_DEBUG("PM_: %{public}s start, line: %{public}d\n", __FUNCTION__, __LINE__);
        PowerInfo maxPowerInfo = sharedDevice->CalcMaxPower();
        LOG_DEBUG("PM_: %{public}s, maxPowerInfo: %{public}d\n", __FUNCTION__, static_cast<int>(maxPowerInfo.powerMode_));
        if (maxPowerInfo.powerMode_ == PowerModeLevel::LEVEL_ACTIVE) {
            sharedDevice->SetActiveMode();
        } else if (maxPowerInfo.powerMode_ == PowerModeLevel::NO_ACTION) {
            return;
        } else {
            PowerSsrLevel lowestSsrLevel = sharedDevice->CalcLowestSsrLevel();
            sharedDevice->SetRequestPowerLevel(lowestSsrLevel, maxPowerInfo.powerMode_);
            utility::Message msg(PowerStateMachine::MSG_PM_SET_SNIFF);
            sharedDevice->pimpl->psm_->ProcessMessage(msg);
        }
    } else {
        LOG_DEBUG("PowerDevice::DelayTimeoutCallback sharedDevice == nullptr");
    }
}

/// delay timer function
void PowerDevice::StartDelayTimer(int ms) const
{
    LOG_DEBUG("PM_: %{public}s start, line: %{public}d\n", __FUNCTION__, __LINE__);
    pimpl->sniffDelayTimer_->Start(ms);
}

void PowerDevice::StopDelayTimer() const
{
    LOG_DEBUG("PM_: %{public}s start, line: %{public}d\n", __FUNCTION__, __LINE__);
    pimpl->sniffDelayTimer_->Stop();
}

uint64_t PowerDevice::GetDelayTimerRemainMs() const
{
    return pimpl->sniffDelayTimer_->GetRemainMs();
}

bool PowerDevice::PowerTimer::Start(int ms, bool isPeriodic)
{
    bool ret = utility::Timer::Start(ms, isPeriodic);
    struct timespec ts = {};
    clock_gettime(CLOCK_BOOTTIME, &ts);
    deadLineMs_ = ts.tv_sec * MS_PER_SECOND + ts.tv_nsec / NS_PER_MS + ms;
    return ret;
}

bool PowerDevice::PowerTimer::Stop()
{
    deadLineMs_ = 0;
    return utility::Timer::Stop();
}

uint64_t PowerDevice::PowerTimer::GetRemainMs()
{
    struct timespec ts = {};
    clock_gettime(CLOCK_BOOTTIME, &ts);
    uint64_t presentMs = ts.tv_sec * MS_PER_SECOND + ts.tv_nsec / NS_PER_MS;
    if (deadLineMs_ > presentMs) {
        return deadLineMs_ - presentMs;
    } else {
        deadLineMs_ = 0;
        return 0;
    }
}
}  // namespace bluetooth
}  // namespace OHOS