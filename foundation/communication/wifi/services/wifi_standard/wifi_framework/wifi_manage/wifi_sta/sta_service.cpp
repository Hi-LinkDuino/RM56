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

#include "sta_service.h"
#include "sta_define.h"
#include "sta_service_callback.h"
#include "wifi_common_util.h"
#include "wifi_hisysevent.h"
#include "wifi_logger.h"
#include "wifi_settings.h"
#include "wifi_sta_hal_interface.h"
#include "wifi_supplicant_hal_interface.h"

DEFINE_WIFILOG_LABEL("StaService");
namespace OHOS {
namespace Wifi {
StaService::StaService()
    : pStaStateMachine(nullptr), pStaMonitor(nullptr), pStaAutoConnectService(nullptr)
{}

StaService::~StaService()
{
    WIFI_LOGI("StaService::~StaService");
    if (pStaMonitor != nullptr) {
        pStaMonitor->UnInitStaMonitor();
        delete pStaMonitor;
        pStaMonitor = nullptr;
    }

    if (pStaAutoConnectService != nullptr) {
        delete pStaAutoConnectService;
        pStaAutoConnectService = nullptr;
    }

    if (pStaStateMachine != nullptr) {
        delete pStaStateMachine;
        pStaStateMachine = nullptr;
    }
}

ErrCode StaService::InitStaService(const StaServiceCallback &callbacks)
{
    WIFI_LOGI("Enter StaService::InitStaService.\n");
    pStaStateMachine = new (std::nothrow) StaStateMachine();
    if (pStaStateMachine == nullptr) {
        WIFI_LOGE("Alloc pStaStateMachine failed.\n");
        return WIFI_OPT_FAILED;
    }

    if (pStaStateMachine->InitStaStateMachine() != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("InitStaStateMachine failed.\n");
        return WIFI_OPT_FAILED;
    }

    RegisterStaServiceCallback(callbacks);

    pStaMonitor = new (std::nothrow) StaMonitor();
    if (pStaMonitor == nullptr) {
        WIFI_LOGE("Alloc pStaMonitor failed.\n");
        return WIFI_OPT_FAILED;
    }

    if (pStaMonitor->InitStaMonitor() != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("InitStaMonitor failed.\n");
        return WIFI_OPT_FAILED;
    }

    pStaMonitor->SetStateMachine(pStaStateMachine);

    pStaAutoConnectService = new (std::nothrow) StaAutoConnectService(pStaStateMachine);
    if (pStaAutoConnectService == nullptr) {
        WIFI_LOGE("Alloc pStaAutoConnectService failed.\n");
        return WIFI_OPT_FAILED;
    }
    if (pStaAutoConnectService->InitAutoConnectService() != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("InitAutoConnectService failed.\n");
        return WIFI_OPT_FAILED;
    }
    WIFI_LOGI("Init staservice successfully.\n");
    return WIFI_OPT_SUCCESS;
}

ErrCode StaService::EnableWifi() const
{
    WIFI_LOGI("Enter StaService::EnableWifi.\n");
    pStaStateMachine->SendMessage(WIFI_SVR_CMD_STA_ENABLE_WIFI, STA_CONNECT_MODE);
    WriteWifiStateHiSysEvent(HISYS_SERVICE_TYPE_STA, WifiOperType::ENABLE);
    return WIFI_OPT_SUCCESS;
}

ErrCode StaService::DisableWifi() const
{
    WIFI_LOGI("Enter StaService::DisableWifi.\n");
    pStaStateMachine->SendMessage(WIFI_SVR_CMD_STA_DISABLE_WIFI);
    WriteWifiStateHiSysEvent(HISYS_SERVICE_TYPE_STA, WifiOperType::DISABLE);
    return WIFI_OPT_SUCCESS;
}

int StaService::AddDeviceConfig(const WifiDeviceConfig &config) const
{
    LOGI("Enter StaService::AddDeviceConfig.\n");
    int netWorkId = INVALID_NETWORK_ID;
    std::string bssid;
    WifiDeviceConfig tempDeviceConfig;
    if (WifiSettings::GetInstance().GetDeviceConfig(config.ssid, config.keyMgmt, tempDeviceConfig) == 0) {
        LOGI("A network with the same name already exists in the configuration center!\n");
        netWorkId = tempDeviceConfig.networkId;
        bssid = config.bssid.empty() ? tempDeviceConfig.bssid : config.bssid;
        pStaAutoConnectService->EnableOrDisableBssid(bssid, true, 0);
    } else {
        if (WifiStaHalInterface::GetInstance().GetNextNetworkId(netWorkId) != WIFI_IDL_OPT_OK) {
            LOGE("StaService::AddDeviceConfig GetNextNetworkId failed!");
            return INVALID_NETWORK_ID;
        }
        LOGD("StaService::AddDeviceConfig alloc new id[%{public}d] succeed!", netWorkId);
    }
    tempDeviceConfig = config;
    tempDeviceConfig.networkId = netWorkId;
    if (!bssid.empty()) {
        tempDeviceConfig.bssid = bssid;
    }

    /* Setting the network to wpa */
    if(pStaStateMachine->ConvertDeviceCfg(tempDeviceConfig) != WIFI_OPT_SUCCESS) {
        LOGE("StaService::AddDeviceConfig ConvertDeviceCfg failed!");
        return INVALID_NETWORK_ID;
    }

    /* Add the new network to WifiSettings. */
    WifiSettings::GetInstance().AddDeviceConfig(tempDeviceConfig);
    WifiSettings::GetInstance().SyncDeviceConfig();
    return netWorkId;
}

int StaService::UpdateDeviceConfig(const WifiDeviceConfig &config) const
{
    return AddDeviceConfig(config);
}

ErrCode StaService::RemoveDevice(int networkId) const
{
    LOGD("Enter StaService::RemoveDevice.\n");
    /* Remove network configuration. */
    if (WifiStaHalInterface::GetInstance().RemoveDevice(networkId) != WIFI_IDL_OPT_OK) {
        LOGE("RemoveDeviceConfig() failed!");
        return WIFI_OPT_FAILED;
    }
    if (WifiStaHalInterface::GetInstance().SaveDeviceConfig() != WIFI_IDL_OPT_OK) {
        LOGW("RemoveDevice-SaveDeviceConfig() failed!");
    } else {
        LOGD("RemoveDevice-SaveDeviceConfig() succeed!");
    }
    WifiDeviceConfig config;
    if (WifiSettings::GetInstance().GetDeviceConfig(networkId, config) == 0) {
        pStaAutoConnectService->EnableOrDisableBssid(config.bssid, true, 0);
    }
    /* Remove network configuration directly without notification to InterfaceService. */
    WifiSettings::GetInstance().RemoveDevice(networkId);
    WifiSettings::GetInstance().SyncDeviceConfig();
    return WIFI_OPT_SUCCESS;
}

ErrCode StaService::RemoveAllDevice() const
{
    LOGD("Enter StaService::RemoveAllDevice.\n");
    if (WifiStaHalInterface::GetInstance().ClearDeviceConfig() == WIFI_IDL_OPT_OK) {
        LOGD("Remove all device config successfully!");
        if (WifiStaHalInterface::GetInstance().SaveDeviceConfig() != WIFI_IDL_OPT_OK) {
            LOGE("WifiStaHalInterface:RemoveAllDevice:SaveDeviceConfig failed!");
        }
    } else {
        LOGE("WifiStaHalInterface:RemoveAllDevice failed!");
        return WIFI_OPT_FAILED;
    }

    WifiSettings::GetInstance().ClearDeviceConfig();
    if (WifiSettings::GetInstance().SyncDeviceConfig() != 0) {
        LOGE("RemoveAllDevice-SyncDeviceConfig() failed!");
        return WIFI_OPT_FAILED;
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode StaService::ConnectToDevice(const WifiDeviceConfig &config) const
{
    LOGI("Enter StaService::ConnectToDevice.\n");
    int netWorkId = AddDeviceConfig(config);
    if(netWorkId == INVALID_NETWORK_ID) {
        LOGD("StaService::ConnectTo  AddDeviceConfig failed!");
        return WIFI_OPT_FAILED;
    }
    LOGD("StaService::ConnectTo  AddDeviceConfig succeed!");
    pStaStateMachine->SendMessage(WIFI_SVR_CMD_STA_CONNECT_NETWORK, netWorkId, NETWORK_SELECTED_BY_THE_USER);
    WriteWifiConnectionHiSysEvent(WifiConnectionType::CONNECT, GetBundleName());
    return WIFI_OPT_SUCCESS;
}

ErrCode StaService::ConnectToNetwork(int networkId) const
{
    LOGI("Enter StaService::ConnectToNetwork, networkId is %{public}d.\n", networkId);
    WifiDeviceConfig config;
    if (WifiSettings::GetInstance().GetDeviceConfig(networkId, config) != 0) {
        LOGE("WifiDeviceConfig is null!");
        return WIFI_OPT_FAILED;
    }

    pStaAutoConnectService->EnableOrDisableBssid(config.bssid, true, 0);
    pStaStateMachine->SendMessage(WIFI_SVR_CMD_STA_CONNECT_SAVED_NETWORK, networkId, NETWORK_SELECTED_BY_THE_USER);
    WriteWifiConnectionHiSysEvent(WifiConnectionType::CONNECT, GetBundleName());
    return WIFI_OPT_SUCCESS;
}

ErrCode StaService::ReAssociate() const
{
    WIFI_LOGI("Enter StaService::ReAssociate.\n");
    pStaStateMachine->SendMessage(WIFI_SVR_CMD_STA_REASSOCIATE_NETWORK);
    WriteWifiConnectionHiSysEvent(WifiConnectionType::CONNECT, GetBundleName());
    return WIFI_OPT_SUCCESS;
}

ErrCode StaService::EnableDeviceConfig(int networkId, bool attemptEnable) const
{
    WIFI_LOGD("Enter StaService::EnableDeviceConfig! networkid is %{public}d", networkId);

    /* Update wifi status. */
    if (WifiSettings::GetInstance().SetDeviceState(networkId, (int)WifiDeviceConfigStatus::ENABLED, attemptEnable) <
        0) {
        WIFI_LOGE("Enable device config failed!");
        return WIFI_OPT_FAILED;
    }
    WifiSettings::GetInstance().SyncDeviceConfig();
    return WIFI_OPT_SUCCESS;
}

ErrCode StaService::DisableDeviceConfig(int networkId) const
{
    WIFI_LOGD("Enter StaService::DisableDeviceConfig.networkid is %{public}d", networkId);

    if (WifiSettings::GetInstance().SetDeviceState(networkId, (int)WifiDeviceConfigStatus::DISABLED) < 0) {
        WIFI_LOGE("Disable device config failed!");
        return WIFI_OPT_FAILED;
    }
    WifiSettings::GetInstance().SyncDeviceConfig();
    return WIFI_OPT_SUCCESS;
}

ErrCode StaService::Disconnect() const
{
    WIFI_LOGI("Enter StaService::Disconnect.\n");
    WifiLinkedInfo linkedInfo;
    WifiSettings::GetInstance().GetLinkedInfo(linkedInfo);
    if (pStaAutoConnectService->EnableOrDisableBssid(linkedInfo.bssid, false, AP_CANNOT_HANDLE_NEW_STA)) {
        WIFI_LOGI("The blocklist is updated.\n");
    }
    pStaStateMachine->SendMessage(WIFI_SVR_CMD_STA_DISCONNECT);
    WriteWifiConnectionHiSysEvent(WifiConnectionType::DISCONNECT, GetBundleName());
    return WIFI_OPT_SUCCESS;
}

ErrCode StaService::StartWps(const WpsConfig &config) const
{
    WIFI_LOGI("Enter StaService::StartWps.\n");
    InternalMessage *msg = pStaStateMachine->CreateMessage();
    msg->SetMessageName(WIFI_SVR_CMD_STA_STARTWPS);
    msg->SetParam1(static_cast<int>(config.setup));
    msg->AddStringMessageBody(config.pin);
    msg->AddStringMessageBody(config.bssid);
    pStaStateMachine->SendMessage(msg);
    return WIFI_OPT_SUCCESS;
}

ErrCode StaService::CancelWps() const
{
    WIFI_LOGI("Enter StaService::CanceltWps.\n");
    pStaStateMachine->SendMessage(WIFI_SVR_CMD_STA_CANCELWPS);
    return WIFI_OPT_SUCCESS;
}

ErrCode StaService::SetCountryCode(const std::string &countryCode) const
{
    LOGI("Enter StaService::SetCountryCode, countryCode=[%s]!", countryCode.c_str());
    if (WifiSupplicantHalInterface::GetInstance().WpaSetCountryCode(countryCode) != WIFI_IDL_OPT_OK) {
        LOGE("WpaSetCountryCode() failed!");
        return WIFI_OPT_FAILED;
    }
    LOGI("WpaSetCountryCode() succeed!");
    WifiSettings::GetInstance().SetCountryCode(countryCode);
    return WIFI_OPT_SUCCESS;
}

ErrCode StaService::AutoConnectService(const std::vector<InterScanInfo> &scanInfos)
{
    WIFI_LOGI("Enter StaService::AutoConnectService.\n");
    pStaAutoConnectService->OnScanInfosReadyHandler(scanInfos);
    return WIFI_OPT_SUCCESS;
}

void StaService::RegisterStaServiceCallback(const StaServiceCallback &callbacks) const
{
    LOGI("Enter StaService::RegisterStaServiceCallback.");
    if (pStaStateMachine == nullptr) {
        LOGE("pStaStateMachine is null.\n");
        return;
    }
    pStaStateMachine->RegisterStaServiceCallback(callbacks);
}

ErrCode StaService::ReConnect() const
{
    WIFI_LOGI("Enter StaService::ReConnect.\n");
    pStaStateMachine->SendMessage(WIFI_SVR_CMD_STA_RECONNECT_NETWORK);
    WriteWifiConnectionHiSysEvent(WifiConnectionType::CONNECT, GetBundleName());
    return WIFI_OPT_SUCCESS;
}
}  // namespace Wifi
}  // namespace OHOS
