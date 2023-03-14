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

#include "power_manager.h"
#include <cstring>
#include <list>
#include <map>
#include <mutex>
#include <string>
#include "adapter_manager.h"
#include "btm.h"
#include "log.h"
#include "log_util.h"
#include "power_device.h"
#include "timer.h"

namespace OHOS {
namespace bluetooth {
// static function
static std::unique_ptr<PowerManager> g_powerManager = nullptr;
IPowerManager &IPowerManager::GetInstance()
{
    return PowerManager::GetInstance();
}
void IPowerManager::Initialize(utility::Dispatcher &dispatcher)
{
    g_powerManager->Initialize(dispatcher);
}

void IPowerManager::Uninitialize()
{
    g_powerManager->Uninitialize();
}

PowerManager &PowerManager::GetInstance()
{
    return *(g_powerManager.get());
}

void PowerManager::Initialize(utility::Dispatcher &dispatcher)
{
    LOG_DEBUG("PM_: %{public}s start, line: %{public}d\n", __FUNCTION__, __LINE__);
    if (g_powerManager == nullptr) {
        g_powerManager = std::make_unique<PowerManager>(dispatcher);
    }
}

void PowerManager::Uninitialize()
{
    LOG_DEBUG("PM_: %{public}s start, line: %{public}d\n", __FUNCTION__, __LINE__);
    g_powerManager = nullptr;
}

/// PowerManager class
struct PowerManager::impl {
public:
    explicit impl(utility::Dispatcher &dispatcher) : dispatcher_(dispatcher)
    {
        LOG_DEBUG("PM_: impl %{public}s start, line: %{public}d\n", __FUNCTION__, __LINE__);
    };
    ~impl(){};

    std::mutex mutex_ {};
    std::atomic_bool isEnabled_ = false;
    BtmPmCallbacks btmPmCallbacks_ {};
    BtmAclCallbacks btmAclCallbacks_ {};
    utility::Dispatcher &dispatcher_;
    std::map<RawAddress, std::shared_ptr<PowerDevice>> powerDevices_ {};
    std::map<uint16_t, RawAddress> connectionHandles_ {};

    void PowerProcess(const RequestStatus status, const std::string &profileName, const RawAddress rawAddr);
    void UpdatePowerDevicesInfo(const RawAddress rawAddr, const std::string &profileName, const RequestStatus status);

    void ModeChangeCallBackProcess(uint8_t status, const RawAddress rawAddr, uint8_t currentMode, uint16_t interval);
    static void ModeChangeCallBack(
        uint8_t status, const BtAddr *btAddr, uint8_t currentMode, uint16_t interval, void *context);

    void SsrCompleteCallBackProcess(uint8_t status, const RawAddress rawAddr);
    static void SsrCompleteCallBack(uint8_t status, const BtAddr *btAddr, void *context);

    void ConnectionCompleteCallBackProcess(const RawAddress rawAddr, uint16_t connectionHandle);
    static void ConnectionCompleteCallBack(const BtmAclConnectCompleteParam *param, void *context);

    void DisconnectionCompleteCallBackProcess(uint8_t status, uint16_t connectionHandle, uint8_t reason);
    static void DisconnectionCompleteCallBack(uint8_t status, uint16_t connectionHandle, uint8_t reason, void *context);

    BT_DISALLOW_COPY_AND_ASSIGN(impl);
};

PowerManager::PowerManager(utility::Dispatcher &dispatcher) : pimpl(std::make_unique<PowerManager::impl>(dispatcher))
{}
PowerManager::~PowerManager()
{}

void PowerManager::Enable()
{
    LOG_DEBUG("PM_: %{public}s start, line: %{public}d\n", __FUNCTION__, __LINE__);
    pimpl->isEnabled_ = true;
    int retVal = BTM_SetDefaultLinkPolicySettings(BTM_LINK_POLICY_ENABLE_ROLE_SWITCH |
        BTM_LINK_POLICY_ENABLE_SNIFF_MODE);
    LOG_DEBUG("PM_: BTM_SetDefaultLinkPolicySettings retVal: %{public}d\n", retVal);

    pimpl->btmPmCallbacks_.modeChange = PowerManager::impl::ModeChangeCallBack;
    pimpl->btmPmCallbacks_.setSniffSubratingComplete = PowerManager::impl::SsrCompleteCallBack;
    pimpl->btmAclCallbacks_.connectionComplete = PowerManager::impl::ConnectionCompleteCallBack;
    pimpl->btmAclCallbacks_.disconnectionComplete = PowerManager::impl::DisconnectionCompleteCallBack;

    retVal = BTM_RegisterPmCallbacks(&pimpl->btmPmCallbacks_, this);
    LOG_DEBUG("PM_: BTM_RegisterPmCallbacks retVal: %{public}d\n", retVal);

    retVal = BTM_RegisterAclCallbacks(&pimpl->btmAclCallbacks_, this);
    LOG_DEBUG("PM_: BTM_RegisterPmCallbacks retVal: %{public}d\n", retVal);
}

void PowerManager::Disable()
{
    LOG_DEBUG("PM_: %{public}s start, line: %{public}d\n", __FUNCTION__, __LINE__);
    pimpl->isEnabled_ = false;
    BTM_DeregisterPmCallbacks(&pimpl->btmPmCallbacks_);
    BTM_DeregisterAclCallbacks(&pimpl->btmAclCallbacks_);
    pimpl->powerDevices_.clear();
}

void PowerManager::StatusUpdate(
    const RequestStatus status, const std::string &profileName, const RawAddress &addr) const
{
    HILOGI("profileName: %{public}s, status: %{public}u,  addr: %{public}s", profileName.c_str(), status,
        GetEncryptAddr(addr.GetAddress()).c_str());

    if (pimpl->isEnabled_) {
        pimpl->dispatcher_.PostTask(
            std::bind(&PowerManager::impl::PowerProcess, pimpl.get(), status, profileName, addr));
    }
}

BTPowerMode PowerManager::GetPowerMode(const RawAddress &addr) const
{
    std::unique_lock<std::mutex> lock(pimpl->mutex_);
    auto iter = pimpl->powerDevices_.find(addr);
    if (iter != pimpl->powerDevices_.end()) {
        return iter->second->GetPowerMode();
    }
    return BTPowerMode::MODE_INVALID;
}

void PowerManager::impl::PowerProcess(
    const RequestStatus status, const std::string &profileName, const RawAddress rawAddr)
{
    HILOGI("status: %{public}u, profileName: %{public}s", status, profileName.c_str());
    std::unique_lock<std::mutex> lock(mutex_);
    UpdatePowerDevicesInfo(rawAddr, profileName, status);
    auto iter = powerDevices_.find(rawAddr);
    if (iter != powerDevices_.end()) {
        iter->second->SetPowerMode();
    }
}

void PowerManager::impl::UpdatePowerDevicesInfo(
    const RawAddress rawAddr, const std::string &profileName, const RequestStatus status)
{
    auto iter = powerDevices_.find(rawAddr);
    if (iter == powerDevices_.end()) {
        LOG_DEBUG("PM_: UpdatePowerDevicesInfo(), create powerDevices\n");
        powerDevices_[rawAddr] = std::make_shared<PowerDevice>(rawAddr, dispatcher_);
    }

    if (status == RequestStatus::CONNECT_OFF) {
        auto its = powerDevices_.find(rawAddr);
        if (its != powerDevices_.end()) {
            its->second->DeleteRequestPower(profileName);
        }
    } else {
        LOG_DEBUG("PM_:: UpdatePowerDevicesInfo(), execute SetRequesetPower()\n");
        powerDevices_[rawAddr]->SetRequestPower(profileName, status);
    }
}

void PowerManager::impl::ModeChangeCallBackProcess(
    uint8_t status, const RawAddress rawAddr, uint8_t currentMode, uint16_t interval)
{
    LOG_DEBUG("PM_: %{public}s start, status: %u, currentMode: %u, interval: %u, line: %{public}d\n",
        __FUNCTION__,
        status,
        currentMode,
        interval,
        __LINE__);
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = powerDevices_.find(rawAddr);
    if (iter == powerDevices_.end()) {
        if (status != 0) {
            LOG_DEBUG("PM_: ModeChangeCallBackProcess(), no need to create powerDevices for error status\n");
            return;
        }
        LOG_DEBUG("PM_: ModeChangeCallBackProcess(), create powerDevices\n");
        powerDevices_[rawAddr] = std::make_shared<PowerDevice>(rawAddr, dispatcher_);
    }
    powerDevices_[rawAddr]->ModeChangeCallBack(status, currentMode, interval);
}

void PowerManager::impl::ModeChangeCallBack(
    uint8_t status, const BtAddr *btAddr, uint8_t currentMode, uint16_t interval, void *context)
{
    RawAddress rawAddr = bluetooth::RawAddress::ConvertToString(btAddr->addr);
    static_cast<PowerManager*>(context)
        ->pimpl->dispatcher_.PostTask(std::bind(&PowerManager::impl::ModeChangeCallBackProcess,
            static_cast<PowerManager*>(context)->pimpl.get(),
            status,
            rawAddr,
            currentMode,
            interval));
}

void PowerManager::impl::SsrCompleteCallBackProcess(uint8_t status, const RawAddress rawAddr)
{
    LOG_DEBUG("PM_: %{public}s start, status: %u, line: %{public}d\n", __FUNCTION__, status, __LINE__);
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = powerDevices_.find(rawAddr);
    if (iter != powerDevices_.end()) {
        iter->second->SniffSubratingCompleteCallback(status);
    }
}

void PowerManager::impl::SsrCompleteCallBack(uint8_t status, const BtAddr *btAddr, void *context)
{
    RawAddress rawAddr = bluetooth::RawAddress::ConvertToString(btAddr->addr);
    static_cast<PowerManager*>(context)
        ->pimpl->dispatcher_.PostTask(std::bind(
            &PowerManager::impl::SsrCompleteCallBackProcess, static_cast<PowerManager*>(context)->pimpl.get(), status, rawAddr));
}

void PowerManager::impl::ConnectionCompleteCallBackProcess(const RawAddress rawAddr, uint16_t connectionHandle)
{
    HILOGI("addr: %{public}s", GetEncryptAddr(rawAddr.GetAddress()).c_str());
    // construct
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = powerDevices_.find(rawAddr);
    if (iter == powerDevices_.end()) {
        LOG_DEBUG("PM_: ConnectionCompleteCallBackProcess(), create powerDevices\n");
        powerDevices_[rawAddr] = std::make_shared<PowerDevice>(rawAddr, dispatcher_);
    }
    connectionHandles_[connectionHandle] = rawAddr;
}

void PowerManager::impl::ConnectionCompleteCallBack(const BtmAclConnectCompleteParam *param, void *context)
{
    RawAddress rawAddr = bluetooth::RawAddress::ConvertToString(param->addr->addr);
    uint16_t connectionHandle = param->connectionHandle;
    static_cast<PowerManager*>(context)
        ->pimpl->dispatcher_.PostTask(std::bind(&PowerManager::impl::ConnectionCompleteCallBackProcess,
            static_cast<PowerManager*>(context)->pimpl.get(),
            rawAddr,
            connectionHandle));
}

void PowerManager::impl::DisconnectionCompleteCallBackProcess(uint8_t status, uint16_t connectionHandle, uint8_t reason)
{
    LOG_DEBUG("PM_: DisconnectionCompleteCallBackProcess(), status=%u, reason=%u\n", status, reason);
    // destruct
    if (status == 0) {
        std::unique_lock<std::mutex> lock(mutex_);
        auto iter = connectionHandles_.find(connectionHandle);
        if (iter != connectionHandles_.end()) {
            auto its = powerDevices_.find(iter->second);
            if (its != powerDevices_.end()) {
                powerDevices_.erase(its);
            }
            HILOGI("delete powerDevices, addr: %{public}s", GetEncryptAddr(iter->second.GetAddress()).c_str());
            connectionHandles_.erase(iter);
        }
    }
}

void PowerManager::impl::DisconnectionCompleteCallBack(
    uint8_t status, uint16_t connectionHandle, uint8_t reason, void *context)
{
    static_cast<PowerManager*>(context)
        ->pimpl->dispatcher_.PostTask(std::bind(&PowerManager::impl::DisconnectionCompleteCallBackProcess,
            static_cast<PowerManager*>(context)->pimpl.get(),
            status,
            connectionHandle,
            reason));
}
}  // namespace bluetooth
}  // namespace OHOS