/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "wifi_protect_manager.h"
#include "wifi_log.h"
#include "wifi_chip_hal_interface.h"
#include "wifi_supplicant_hal_interface.h"

#undef LOG_TAG
#define LOG_TAG "OHWIFI_MANAGER_LOCK_MANAGER"

namespace OHOS {
namespace Wifi {
WifiProtectManager::WifiProtectManager()
{
    mWifiConnected = false;
    mScreenOn = false;
    mForceHiPerfMode = false;
    mForceLowLatencyMode = false;
    mCurrentOpMode = WifiProtectMode::WIFI_PROTECT_NO_HELD;
    mFullHighPerfProtectsAcquired = 0;
    mFullHighPerfProtectsReleased = 0;
    mFullLowLatencyProtectsAcquired = 0;
    mFullLowLatencyProtectsReleased = 0;
    mWifiProtects.clear();
}

WifiProtectManager::~WifiProtectManager()
{}

WifiProtectManager &WifiProtectManager::GetInstance()
{
    static WifiProtectManager instance;
    return instance;
}

bool WifiProtectManager::IsValidProtectMode(WifiProtectMode &protectMode)
{
    if (protectMode != WifiProtectMode::WIFI_PROTECT_FULL && protectMode != WifiProtectMode::WIFI_PROTECT_SCAN_ONLY &&
        protectMode != WifiProtectMode::WIFI_PROTECT_FULL_HIGH_PERF &&
        protectMode != WifiProtectMode::WIFI_PROTECT_FULL_LOW_LATENCY) {
        return false;
    }

    return true;
}

WifiProtectMode WifiProtectManager::GetNearlyProtectMode()
{
    /* If Wifi Client is not connected, then all protects are not effective */
    if (!mWifiConnected) {
        return WifiProtectMode::WIFI_PROTECT_NO_HELD;
    }

    /* Check if mode is forced to hi-perf */
    if (mForceHiPerfMode) {
        return WifiProtectMode::WIFI_PROTECT_FULL_HIGH_PERF;
    }

    /* Check if mode is forced to low-latency */
    if (mForceLowLatencyMode) {
        return WifiProtectMode::WIFI_PROTECT_FULL_LOW_LATENCY;
    }

    if (mFullHighPerfProtectsAcquired > mFullHighPerfProtectsReleased) {
        return WifiProtectMode::WIFI_PROTECT_FULL_HIGH_PERF;
    }

    return WifiProtectMode::WIFI_PROTECT_NO_HELD;
}

bool WifiProtectManager::InitWifiProtect(const WifiProtectType &protectType, const std::string &protectName)
{
    WifiProtect* pProtect = new WifiProtect(protectType, WifiProtectMode::WIFI_PROTECT_FULL, protectName);
    mWifiProtects.push_back(pProtect);
    return true;
}

bool WifiProtectManager::GetWifiProtect(const WifiProtectMode &protectMode, const std::string name)
{
    bool isAlreadyExist = false;

    std::vector<WifiProtect *>::iterator itor = mWifiProtects.begin();
    while (itor != mWifiProtects.end()) {
        if ((*itor)->GetName() == name) {
            LOGD("old name = %{public}s, and new Name = %{public}s",
                (*itor)->GetName().c_str(),
                (*itor)->GetName().c_str());
            isAlreadyExist = true;
            break;
        }
        itor++;
    }

    if (isAlreadyExist) {
        LOGD("attempted to add a protect when already holding one");
        return false;
    }

    WifiProtect *pProtect = new WifiProtect(name);
    if (pProtect == nullptr) {
        LOGE("Wifi protect pointer is null.");
        return false;
    }
    pProtect->SetProtectMode(protectMode);
    return AddProtect(pProtect);
}

bool WifiProtectManager::ChangeToPerfMode(bool isEnabled)
{
    mForceHiPerfMode = isEnabled;
    mForceLowLatencyMode = false;
    if (!ChangeWifiPowerMode()) {
        LOGE("Failed to force hi-perf mode, returning to normal mode");
        mForceHiPerfMode = false;
        return false;
    }

    return true;
}
void WifiProtectManager::HandleScreenStateChanged(bool screenOn)
{
    mScreenOn = screenOn;
}

void WifiProtectManager::UpdateWifiClientConnected(bool isConnected)
{
    mWifiConnected = isConnected;
}

bool WifiProtectManager::AddProtect(WifiProtect *pProtect)
{
    mWifiProtects.push_back(pProtect);
    switch (pProtect->GetProtectMode()) {
        case WifiProtectMode::WIFI_PROTECT_FULL_HIGH_PERF:
            if (mWifiConnected) {
                ++mFullHighPerfProtectsAcquired;
            }
            break;
        case WifiProtectMode::WIFI_PROTECT_FULL_LOW_LATENCY:
            ++mFullLowLatencyProtectsAcquired;
            break;
        default:
            break;
    }

    ChangeWifiPowerMode();
    LOGD("GetWifiProtect finished!");
    return true;
}

bool WifiProtectManager::PutWifiProtect(const std::string &name)
{
    WifiProtect *pWifiProtect = RemoveProtect(name);
    if (pWifiProtect == nullptr) {
        /* attempting to release a protect that does not exist. */
        return false;
    }

    switch (pWifiProtect->GetProtectMode()) {
        case WifiProtectMode::WIFI_PROTECT_FULL_HIGH_PERF:
            if (mWifiConnected) {
                ++mFullHighPerfProtectsReleased;
            }
            break;
        case WifiProtectMode::WIFI_PROTECT_FULL_LOW_LATENCY:
            ++mFullLowLatencyProtectsReleased;
            break;
        default:
            break;
    }

    /* Recalculate the operating mode */
    ChangeWifiPowerMode();
    LOGD("PutWifiProtect finished!");

    delete pWifiProtect;
    pWifiProtect = nullptr;
    return true;
}

WifiProtect *WifiProtectManager::RemoveProtect(const std::string &name)
{
    WifiProtect *pProtect = nullptr;
    std::vector<WifiProtect *>::iterator itor = mWifiProtects.begin();
    while (itor != mWifiProtects.end()) {
        if ((*itor)->GetName() == name) {
            pProtect = *itor;
            itor = mWifiProtects.erase(itor);
            break;
        }
        itor++;
    }
    return pProtect;
}

bool WifiProtectManager::ChangeWifiPowerMode()
{
    WifiProtectMode newProtectMode = GetNearlyProtectMode();
    if (newProtectMode == mCurrentOpMode) {
        /* No action is needed */
        return true;
    }

    /* Otherwise, we need to change current mode, first reset it to normal */
    switch (mCurrentOpMode) {
        case WifiProtectMode::WIFI_PROTECT_FULL_HIGH_PERF:
            if (!WifiSupplicantHalInterface::GetInstance().SetPowerSave(true)) {
                LOGE("Failed to reset the OpMode from hi-perf to Normal");
                return false;
            }
            break;
        case WifiProtectMode::WIFI_PROTECT_FULL_LOW_LATENCY:

            break;
        case WifiProtectMode::WIFI_PROTECT_NO_HELD:
        default:
            /* No action */
            break;
    }

    /* Set the current mode, before we attempt to set the new mode */
    mCurrentOpMode = WifiProtectMode::WIFI_PROTECT_NO_HELD;

    /* Now switch to the new opMode */
    switch (newProtectMode) {
        case WifiProtectMode::WIFI_PROTECT_FULL_HIGH_PERF:
            if (!WifiSupplicantHalInterface::GetInstance().SetPowerSave(false)) {
                LOGE("Failed to set the OpMode to hi-perf");
                return false;
            }
            break;
        case WifiProtectMode::WIFI_PROTECT_FULL_LOW_LATENCY:
            if (!SetLowLatencyMode(true)) {
                LOGE("Failed to set the OpMode to low-latency");
                return false;
            }
            break;
        case WifiProtectMode::WIFI_PROTECT_NO_HELD:
            /* No action */
            break;
        default:
            /* Invalid mode, don't change currentOpMode , and exit with error */
            LOGE("Invalid new opMode: %{public}d", (int)newProtectMode);
            return false;
    }

    /* Now set the mode to the new value */
    mCurrentOpMode = newProtectMode;
    return true;
}

bool WifiProtectManager::SetLowLatencyMode(bool enabled)
{
    /* Only set power save mode */
    if (!WifiSupplicantHalInterface::GetInstance().SetPowerSave(!enabled)) {
        LOGE("Failed to set power save mode");
        return false;
    }

    return true;
}
}  // namespace Wifi
}  // namespace OHOS