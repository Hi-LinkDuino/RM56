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

#include "wifi_device_service_impl.h"
#include <algorithm>
#include <unistd.h>
#include <file_ex.h>
#include "wifi_permission_utils.h"
#include "wifi_internal_msg.h"
#include "wifi_auth_center.h"
#include "wifi_config_center.h"
#include "wifi_internal_event_dispatcher.h"
#include "wifi_manager.h"
#include "wifi_service_manager.h"
#include "wifi_protect_manager.h"
#include "wifi_logger.h"
#include "define.h"
#include "wifi_dumper.h"
#include "wifi_common_util.h"
#include "wifi_protect_manager.h"

DEFINE_WIFILOG_LABEL("WifiDeviceServiceImpl");
namespace OHOS {
namespace Wifi {
std::mutex WifiDeviceServiceImpl::g_instanceLock;
sptr<WifiDeviceServiceImpl> WifiDeviceServiceImpl::g_instance;
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(WifiDeviceServiceImpl::GetInstance().GetRefPtr());

sptr<WifiDeviceServiceImpl> WifiDeviceServiceImpl::GetInstance()
{
    if (g_instance == nullptr) {
        std::lock_guard<std::mutex> autoLock(g_instanceLock);
        if (g_instance == nullptr) {
            auto service = new (std::nothrow) WifiDeviceServiceImpl;
            g_instance = service;
        }
    }
    return g_instance;
}

WifiDeviceServiceImpl::WifiDeviceServiceImpl()
    : SystemAbility(WIFI_DEVICE_ABILITY_ID, true), mPublishFlag(false), mState(ServiceRunningState::STATE_NOT_START)
{}

WifiDeviceServiceImpl::~WifiDeviceServiceImpl()
{}

void WifiDeviceServiceImpl::OnStart()
{
    if (mState == ServiceRunningState::STATE_RUNNING) {
        WIFI_LOGD("Service has already started.");
        return;
    }
    if (!Init()) {
        WIFI_LOGE("Failed to init service");
        OnStop();
        return;
    }
    mState = ServiceRunningState::STATE_RUNNING;
    WIFI_LOGI("Start sta service!");
    WifiManager::GetInstance();
}

void WifiDeviceServiceImpl::OnStop()
{
    mState = ServiceRunningState::STATE_NOT_START;
    mPublishFlag = false;
    WIFI_LOGI("Stop sta service!");
}

bool WifiDeviceServiceImpl::Init()
{
    if (!mPublishFlag) {
        bool ret = Publish(WifiDeviceServiceImpl::GetInstance());
        if (!ret) {
            WIFI_LOGE("Failed to publish sta service!");
            return false;
        }
        mPublishFlag = true;
    }
    return true;
}

ErrCode WifiDeviceServiceImpl::EnableWifi()
{
    ErrCode errCode = CheckCanEnableWifi();
    if (errCode != WIFI_OPT_SUCCESS) {
        return errCode;
    }

    WifiOprMidState curState = WifiConfigCenter::GetInstance().GetWifiMidState();
    if (curState != WifiOprMidState::CLOSED) {
        WIFI_LOGI("current wifi state is %{public}d", static_cast<int>(curState));
        if (curState == WifiOprMidState::CLOSING) { /* when current wifi is closing, return */
            return WIFI_OPT_OPEN_FAIL_WHEN_CLOSING;
        } else {
            return WIFI_OPT_OPEN_SUCC_WHEN_OPENED;
        }
    }

    sptr<WifiP2pServiceImpl> p2pService = WifiP2pServiceImpl::GetInstance();
    if (p2pService != nullptr && p2pService->EnableP2p() != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Enable P2p failed!");
        return WIFI_OPT_FAILED;
    }

    if (!WifiConfigCenter::GetInstance().SetWifiMidState(curState, WifiOprMidState::OPENING)) {
        WIFI_LOGI("set wifi mid state opening failed!");
        return WIFI_OPT_OPEN_SUCC_WHEN_OPENED;
    }

    errCode = WIFI_OPT_FAILED;
    do {
        if (WifiServiceManager::GetInstance().CheckAndEnforceService(WIFI_SERVICE_STA) < 0) {
            break;
        }
        IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
        if (pService == nullptr) {
            WIFI_LOGE("Create %{public}s service failed!", WIFI_SERVICE_STA);
            break;
        }

        errCode = pService->RegisterStaServiceCallback(WifiManager::GetInstance().GetStaCallback());
        if (errCode != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("Register sta service callback failed!");
            break;
        }

        errCode = pService->EnableWifi();
        if (errCode != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("service enable sta failed, ret %{public}d!", static_cast<int>(errCode));
            break;
        }
    } while (false);
    if (errCode != WIFI_OPT_SUCCESS) {
        WifiConfigCenter::GetInstance().SetWifiMidState(WifiOprMidState::OPENING, WifiOprMidState::CLOSED);
        WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_STA);
        return errCode;
    }
    WifiSettings::GetInstance().SyncWifiConfig();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceServiceImpl::DisableWifi()
{
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("DisableWifi:VerifySetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("DisableWifi:VerifyWifiConnectionPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    WifiOprMidState curState = WifiConfigCenter::GetInstance().GetWifiMidState();
    if (curState != WifiOprMidState::RUNNING) {
        WIFI_LOGI("current wifi state is %{public}d", static_cast<int>(curState));
        if (curState == WifiOprMidState::OPENING) { /* when current wifi is opening, return */
            return WIFI_OPT_CLOSE_FAIL_WHEN_OPENING;
        } else {
            return WIFI_OPT_CLOSE_SUCC_WHEN_CLOSED;
        }
    }

    sptr<WifiP2pServiceImpl> p2pService = WifiP2pServiceImpl::GetInstance();
    if (p2pService != nullptr && p2pService->DisableP2p() != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Disable P2p failed!");
        return WIFI_OPT_FAILED;
    }

    if (!WifiConfigCenter::GetInstance().SetWifiMidState(curState, WifiOprMidState::CLOSING)) {
        WIFI_LOGI("set wifi mid state opening failed! may be other activity has been operated");
        return WIFI_OPT_CLOSE_SUCC_WHEN_CLOSED;
    }
    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr) {
        WifiConfigCenter::GetInstance().SetWifiMidState(WifiOprMidState::CLOSED);
        WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_STA);
        return WIFI_OPT_SUCCESS;
    }
    ErrCode ret = pService->DisableWifi();
    if (ret != WIFI_OPT_SUCCESS) {
        WifiConfigCenter::GetInstance().SetWifiMidState(WifiOprMidState::CLOSING, WifiOprMidState::RUNNING);
    } else {
        WifiConfigCenter::GetInstance().SetStaLastRunState(false);
    }
    return ret;
}

ErrCode WifiDeviceServiceImpl::InitWifiProtect(const WifiProtectType &protectType, const std::string &protectName)
{
    if (WifiProtectManager::GetInstance().InitWifiProtect(protectType, protectName)) {
        return WIFI_OPT_SUCCESS;
    }
    return WIFI_OPT_FAILED;
}

ErrCode WifiDeviceServiceImpl::GetWifiProtectRef(const WifiProtectMode &protectMode, const std::string &protectName)
{
    if (WifiProtectManager::GetInstance().GetWifiProtect(protectMode, protectName)) {
        return WIFI_OPT_SUCCESS;
    }
    return WIFI_OPT_FAILED;
}

ErrCode WifiDeviceServiceImpl::PutWifiProtectRef(const std::string &protectName)
{
    if (WifiProtectManager::GetInstance().PutWifiProtect(protectName)) {
        return WIFI_OPT_SUCCESS;
    }
    return WIFI_OPT_FAILED;
}

bool WifiDeviceServiceImpl::CheckConfigPwd(const WifiDeviceConfig &config)
{
    if ((config.ssid.length() <= 0) || (config.keyMgmt.length()) <= 0) {
        return false;
    }

    if (config.keyMgmt != KEY_MGMT_NONE && config.preSharedKey.empty()) {
        WIFI_LOGE("CheckConfigPwd: preSharedKey is empty!");
        return false;
    }

    int len = config.preSharedKey.length();
    bool isAllHex = std::all_of(config.preSharedKey.begin(), config.preSharedKey.end(), isxdigit);
    WIFI_LOGI("CheckConfigPwd, keyMgmt: %{public}s, len: %{public}d", config.keyMgmt.c_str(), len);
    if (config.keyMgmt == KEY_MGMT_NONE) {
        for (int i = 0; i != WEPKEYS_SIZE; ++i) {
            if (!config.wepKeys[i].empty()) { // wep
                int wepLen = config.wepKeys[i].size();
                if (wepLen == WEP_KEY_LEN1 || wepLen == WEP_KEY_LEN2 || wepLen == WEP_KEY_LEN3) {
                    return true;
                }
                constexpr int MULTIPLE_HEXT_TO_ASCII = 2;
                if (wepLen == (WEP_KEY_LEN1 * MULTIPLE_HEXT_TO_ASCII) ||
                    wepLen == (WEP_KEY_LEN2 * MULTIPLE_HEXT_TO_ASCII) ||
                    wepLen == (WEP_KEY_LEN3 * MULTIPLE_HEXT_TO_ASCII)) {
                    return isAllHex;
                }
                return false;
            }
        }
        return config.preSharedKey.empty(); // open
    }
    int minLen = config.keyMgmt == KEY_MGMT_SAE ? MIN_SAE_LEN : MIN_PSK_LEN;
    int maxLen = isAllHex ? MAX_HEX_LEN : MAX_PRESHAREDKEY_LEN;
    if (len < minLen || len > maxLen) {
        WIFI_LOGE("CheckConfigPwd: preSharedKey length error: %{public}d", len);
        return false;
    }
    return true;
}

ErrCode WifiDeviceServiceImpl::AddDeviceConfig(const WifiDeviceConfig &config, int &result)
{
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("AddDeviceConfig:VerifySetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifySetWifiConfigPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("AddDeviceConfig:VerifySetWifiConfigPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!CheckConfigPwd(config)) {
        WIFI_LOGE("CheckConfigPwd failed!");
        return WIFI_OPT_INVALID_PARAM;
    }

    if (!IsStaServiceRunning()) {
        return WIFI_OPT_STA_NOT_OPENED;
    }

    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_STA_NOT_OPENED;
    }
    int retNetworkId = pService->AddDeviceConfig(config);
    if (retNetworkId < 0) {
        return WIFI_OPT_FAILED;
    }
    result = retNetworkId;
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceServiceImpl::UpdateDeviceConfig(const WifiDeviceConfig &config, int &result)
{
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("UpdateDeviceConfig:VerifySetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifySetWifiConfigPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("UpdateDeviceConfig:VerifySetWifiConfigPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsStaServiceRunning()) {
        return WIFI_OPT_STA_NOT_OPENED;
    }

    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_STA_NOT_OPENED;
    }

    int retNetworkId = pService->UpdateDeviceConfig(config);
    if (retNetworkId <= INVALID_NETWORK_ID) {
        return WIFI_OPT_FAILED;
    }
    result = retNetworkId;
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceServiceImpl::RemoveDevice(int networkId)
{
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("RemoveDevice:VerifySetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("RemoveDevice:VerifyWifiConnectionPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsStaServiceRunning()) {
        return WIFI_OPT_STA_NOT_OPENED;
    }

    if (networkId < 0) {
        return WIFI_OPT_INVALID_PARAM;
    }

    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_STA_NOT_OPENED;
    }
    return pService->RemoveDevice(networkId);
}

ErrCode WifiDeviceServiceImpl::RemoveAllDevice()
{
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("RemoveAllDevice:VerifySetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("RemoveAllDevice:VerifyWifiConnectionPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsStaServiceRunning()) {
        return WIFI_OPT_STA_NOT_OPENED;
    }

    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_STA_NOT_OPENED;
    }
    return pService->RemoveAllDevice();
}

ErrCode WifiDeviceServiceImpl::GetDeviceConfigs(std::vector<WifiDeviceConfig> &result)
{
    if (WifiPermissionUtils::VerifyGetWifiInfoInternalPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetDeviceConfigs:VerifyGetWifiInfoPermission() PERMISSION_DENIED!");

        if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
            WIFI_LOGE("GetDeviceConfigs:VerifyGetWifiInfoPermission() PERMISSION_DENIED!");
            return WIFI_OPT_PERMISSION_DENIED;
        }

        if (WifiPermissionUtils::VerifyGetScanInfosPermission() == PERMISSION_DENIED) {
            WIFI_LOGE("GetDeviceConfigs:VerifyGetWifiInfoPermission() PERMISSION_DENIED!");
            return WIFI_OPT_PERMISSION_DENIED;
        }

        if (WifiPermissionUtils::VerifyGetWifiConfigPermission() == PERMISSION_DENIED) {
            WIFI_LOGE("GetDeviceConfigs:VerifyGetWifiInfoPermission() PERMISSION_DENIED!");
            return WIFI_OPT_PERMISSION_DENIED;
        }
    }

    WifiConfigCenter::GetInstance().GetDeviceConfig(result);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceServiceImpl::EnableDeviceConfig(int networkId, bool attemptEnable)
{
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("EnableDeviceConfig:VerifySetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsStaServiceRunning()) {
        return WIFI_OPT_STA_NOT_OPENED;
    }

    if (networkId < 0) {
        return WIFI_OPT_INVALID_PARAM;
    }

    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_STA_NOT_OPENED;
    }
    return pService->EnableDeviceConfig(networkId, attemptEnable);
}

ErrCode WifiDeviceServiceImpl::DisableDeviceConfig(int networkId)
{
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("DisableDeviceConfig:VerifySetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("DisableDeviceConfig:VerifySetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsStaServiceRunning()) {
        return WIFI_OPT_STA_NOT_OPENED;
    }

    if (networkId < 0) {
        return WIFI_OPT_INVALID_PARAM;
    }

    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_STA_NOT_OPENED;
    }
    return pService->DisableDeviceConfig(networkId);
}

ErrCode WifiDeviceServiceImpl::ConnectToNetwork(int networkId)
{
    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("ConnectToNetwork:VerifyWifiConnectionPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsStaServiceRunning()) {
        return WIFI_OPT_STA_NOT_OPENED;
    }

    if (networkId < 0) {
        return WIFI_OPT_INVALID_PARAM;
    }

    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_STA_NOT_OPENED;
    }
    return pService->ConnectToNetwork(networkId);
}

ErrCode WifiDeviceServiceImpl::ConnectToDevice(const WifiDeviceConfig &config)
{
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("ConnectToDevice:VerifySetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("ConnectToDevice:VerifyWifiConnectionPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifySetWifiConfigPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("ConnectToDevice:VerifySetWifiConfigPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!CheckConfigPwd(config)) {
        WIFI_LOGE("CheckConfigPwd failed!");
        return WIFI_OPT_INVALID_PARAM;
    }
    if (!IsStaServiceRunning()) {
        return WIFI_OPT_STA_NOT_OPENED;
    }
    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_STA_NOT_OPENED;
    }
    return pService->ConnectToDevice(config);
}

bool WifiDeviceServiceImpl::IsConnected()
{
    WifiLinkedInfo linkedInfo;

    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("IsConnected:VerifyGetWifiInfoPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    WifiConfigCenter::GetInstance().GetLinkedInfo(linkedInfo);
    return linkedInfo.connState == ConnState::CONNECTED;
}

ErrCode WifiDeviceServiceImpl::ReConnect()
{
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("ReConnect:VerifySetWifiInfoPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("ReConnect:VerifyWifiConnectionPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsStaServiceRunning()) {
        return WIFI_OPT_STA_NOT_OPENED;
    }

    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_STA_NOT_OPENED;
    }
    return pService->ReConnect();
}

ErrCode WifiDeviceServiceImpl::ReAssociate(void)
{
    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("ReAssociate:VerifyWifiConnectionPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsStaServiceRunning()) {
        return WIFI_OPT_STA_NOT_OPENED;
    }

    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_STA_NOT_OPENED;
    }
    return pService->ReAssociate();
}

ErrCode WifiDeviceServiceImpl::Disconnect(void)
{
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("Disconnect:VerifySetWifiInfoPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("Disconnect:VerifyWifiConnectionPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsStaServiceRunning()) {
        return WIFI_OPT_STA_NOT_OPENED;
    }

    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_STA_NOT_OPENED;
    }
    return pService->Disconnect();
}

ErrCode WifiDeviceServiceImpl::StartWps(const WpsConfig &config)
{
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("StartWps:VerifySetWifiInfoPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsStaServiceRunning()) {
        return WIFI_OPT_STA_NOT_OPENED;
    }

    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_STA_NOT_OPENED;
    }
    return pService->StartWps(config);
}

ErrCode WifiDeviceServiceImpl::CancelWps(void)
{
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("CancelWps:VerifySetWifiInfoPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsStaServiceRunning()) {
        return WIFI_OPT_STA_NOT_OPENED;
    }

    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_STA_NOT_OPENED;
    }
    return pService->CancelWps();
}

ErrCode WifiDeviceServiceImpl::IsWifiActive(bool &bActive)
{
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("IsWifiActive:VerifyGetWifiInfoPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    bActive = IsStaServiceRunning();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceServiceImpl::GetWifiState(int &state)
{
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetWifiState:VerifyGetWifiInfoPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    state = WifiConfigCenter::GetInstance().GetWifiState();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceServiceImpl::GetLinkedInfo(WifiLinkedInfo &info)
{
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetLinkedInfo:VerifyGetWifiInfoPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    WifiConfigCenter::GetInstance().GetLinkedInfo(info);
    if (WifiPermissionUtils::VerifyGetWifiLocalMacPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetLinkedInfo:VerifyGetWifiLocalMacPermission() PERMISSION_DENIED!");
        /* Clear mac addr */
        info.macAddress = "";
    }

    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceServiceImpl::GetIpInfo(IpInfo &info)
{
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetIpInfo:VerifyGetWifiInfoPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    WifiConfigCenter::GetInstance().GetIpInfo(info);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceServiceImpl::SetCountryCode(const std::string &countryCode)
{
    if (countryCode.length() != WIFI_COUNTRY_CODE_LEN) {
        return WIFI_OPT_INVALID_PARAM;
    }
    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("SetCountryCode:VerifyWifiConnectionPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!IsStaServiceRunning()) {
        return WIFI_OPT_STA_NOT_OPENED;
    }

    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_STA_NOT_OPENED;
    }
    return pService->SetCountryCode(countryCode);
}

ErrCode WifiDeviceServiceImpl::GetCountryCode(std::string &countryCode)
{
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetCountryCode:VerifyGetWifiInfoPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    WifiConfigCenter::GetInstance().GetCountryCode(countryCode);
    WIFI_LOGI("GetCountryCode: country code is %{public}s", countryCode.c_str());
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceServiceImpl::RegisterCallBack(const sptr<IWifiDeviceCallBack> &callback)
{
    WIFI_LOGI("RegisterCallBack");
    if (callback == nullptr) {
        WIFI_LOGE("Get call back client failed!");
        return WIFI_OPT_FAILED;
    }

    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("RegisterCallBackClient:VerifyWifiConnectionPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    WifiInternalEventDispatcher::GetInstance().SetSingleStaCallback(callback);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceServiceImpl::GetSignalLevel(const int &rssi, const int &band, int &level)
{
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetSignalLevel:VerifyGetWifiInfoPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    level = WifiConfigCenter::GetInstance().GetSignalLevel(rssi, band);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceServiceImpl::GetSupportedFeatures(long &features)
{
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetSupportedFeatures:VerifyGetWifiInfoPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }
    int ret = WifiManager::GetInstance().GetSupportedFeatures(features);
    if (ret < 0) {
        WIFI_LOGE("Failed to get supported features!");
        return WIFI_OPT_FAILED;
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceServiceImpl::GetDeviceMacAddress(std::string &result)
{
    WIFI_LOGI("GetDeviceMacAddress");
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetDeviceMacAddress:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifyGetWifiLocalMacPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetDeviceMacAddress:VerifyGetWifiLocalMacPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    WifiConfigCenter::GetInstance().GetMacAddress(result);
    return WIFI_OPT_SUCCESS;
}

bool WifiDeviceServiceImpl::SetLowLatencyMode(bool enabled)
{
    WIFI_LOGI("SetLowLatencyMode");
    return WifiProtectManager::GetInstance().SetLowLatencyMode(enabled);
}

ErrCode WifiDeviceServiceImpl::CheckCanEnableWifi(void)
{
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("EnableWifi:VerifySetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("EnableWifi:VerifyWifiConnectionPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    /**
     * when airplane mode opened, if the config "can_use_sta_when_airplanemode"
     * opened, then can open sta; other, return forbid.
     */
    if (!WifiConfigCenter::GetInstance().GetCanOpenStaWhenAirplaneMode() &&
        WifiConfigCenter::GetInstance().GetAirplaneModeState() == 1 &&
        !WifiConfigCenter::GetInstance().GetCanUseStaWhenAirplaneMode()) {
        WIFI_LOGI("current airplane mode and can not use sta, open failed!");
        return WIFI_OPT_FORBID_AIRPLANE;
    }
    /* when power saving mode opened, can't open sta, return forbid. */
    if (WifiConfigCenter::GetInstance().GetPowerSavingModeState() == 1) {
        WIFI_LOGI("current power saving mode and can not use sta, open failed!");
        return WIFI_OPT_FORBID_POWSAVING;
    }
    /**
     * Check the interval between the last STA shutdown and the current STA
     * startup.
     */
    double interval = WifiConfigCenter::GetInstance().GetWifiStaInterval();
    if (interval <= REOPEN_STA_INTERVAL) {
        int waitMils = REOPEN_STA_INTERVAL - int(interval) + 1;
        WIFI_LOGI("open wifi too frequent, interval since last close is %{public}lf, and wait %{public}d ms",
            interval,
            waitMils);
        usleep(waitMils * MSEC);
    }
    return WIFI_OPT_SUCCESS;
}

bool WifiDeviceServiceImpl::IsStaServiceRunning()
{
    WifiOprMidState curState = WifiConfigCenter::GetInstance().GetWifiMidState();
    if (curState != WifiOprMidState::RUNNING) {
        WIFI_LOGD("current wifi state is %{public}d", static_cast<int>(curState));
        return false;
    }
    return true;
}

bool WifiDeviceServiceImpl::IsScanServiceRunning()
{
    WifiOprMidState curState = WifiConfigCenter::GetInstance().GetScanMidState();
    if (curState != WifiOprMidState::RUNNING) {
        WIFI_LOGD("scan service does not started!");
        return false;
    }
    return true;
}

void WifiDeviceServiceImpl::SaBasicDump(std::string& result)
{
    WifiDeviceServiceImpl impl;
    bool isActive = impl.IsStaServiceRunning();
    result.append("WiFi active state: ");
    std::string strActive = isActive ? "activated" : "inactive";
    result += strActive + "\n\n";

    WifiLinkedInfo linkedInfo;
    WifiConfigCenter::GetInstance().GetLinkedInfo(linkedInfo);
    bool isConnected = linkedInfo.connState == ConnState::CONNECTED;
    result.append("WiFi connection status: ");
    std::string strIsConnected = isConnected ? "connected" : "not connected";
    result += strIsConnected + "\n";
    if (isConnected) {
        std::stringstream ss;
        ss << "  Connection.ssid: " << linkedInfo.ssid << "\n";
        ss << "  Connection.bssid: " << MacAnonymize(linkedInfo.bssid) << "\n";
        ss << "  Connection.rssi: " << linkedInfo.rssi << "\n";

        enum {BAND_2GHZ = 1, BAND_5GHZ = 2, BAND_ANY = 3};
        auto funcStrBand = [](int band) {
            std::string retStr;
            switch (band) {
                case BAND_2GHZ:
                    retStr = "2.4GHz";
                    break;
                case BAND_5GHZ:
                    retStr = "5GHz";
                    break;
                case BAND_ANY:
                    retStr = "dual-mode frequency band";
                    break;
                default:
                    retStr = "unknown band";
            }
            return retStr;
        };
        ss << "  Connection.band: " << funcStrBand(linkedInfo.band) << "\n";
        ss << "  Connection.frequency: " << linkedInfo.frequency << "\n";
        ss << "  Connection.linkSpeed: " << linkedInfo.linkSpeed << "\n";
        ss << "  Connection.macAddress: " << MacAnonymize(linkedInfo.macAddress) << "\n";
        ss << "  Connection.isHiddenSSID: " << (linkedInfo.ifHiddenSSID ? "true" : "false") << "\n";

        int level = WifiConfigCenter::GetInstance().GetSignalLevel(linkedInfo.rssi, linkedInfo.band);
        ss << "  Connection.signalLevel: " << level << "\n";
        result += ss.str();
    }
    result += "\n";

    std::string cc;
    WifiConfigCenter::GetInstance().GetCountryCode(cc);
    result.append("Country Code: ").append(cc);
    result += "\n";
}

int32_t WifiDeviceServiceImpl::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    std::vector<std::string> vecArgs;
    std::transform(args.begin(), args.end(), std::back_inserter(vecArgs), [](const std::u16string &arg) {
        return Str16ToStr8(arg);
    });

    WifiDumper dumper;
    std::string result;
    dumper.DeviceDump(SaBasicDump, vecArgs, result);
    if (!SaveStringToFd(fd, result)) {
        WIFI_LOGE("WiFi device save string to fd failed.");
        return ERR_OK;
    }
    return ERR_OK;
}
}  // namespace Wifi
}  // namespace OHOS
