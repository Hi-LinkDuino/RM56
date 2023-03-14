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

#include "wifi_hotspot_service_impl.h"
#include <file_ex.h>
#include "wifi_permission_utils.h"
#include "wifi_global_func.h"
#include "wifi_auth_center.h"
#include "wifi_config_center.h"
#include "wifi_manager.h"
#include "wifi_service_manager.h"
#include "wifi_internal_event_dispatcher.h"
#include "wifi_logger.h"
#include "define.h"
#include "wifi_logger.h"
#include "wifi_dumper.h"
#include "wifi_common_util.h"

DEFINE_WIFILOG_HOTSPOT_LABEL("WifiHotspotServiceImpl");

namespace OHOS {
namespace Wifi {
std::mutex WifiHotspotServiceImpl::g_instanceLock;
sptr<WifiHotspotServiceImpl> WifiHotspotServiceImpl::g_instance;
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(WifiHotspotServiceImpl::GetInstance().GetRefPtr());

sptr<WifiHotspotServiceImpl> WifiHotspotServiceImpl::GetInstance()
{
    if (g_instance == nullptr) {
        std::lock_guard<std::mutex> autoLock(g_instanceLock);
        if (g_instance == nullptr) {
            auto service = new (std::nothrow) WifiHotspotServiceImpl;
            g_instance = service;
        }
    }
    return g_instance;
}

WifiHotspotServiceImpl::WifiHotspotServiceImpl()
    : SystemAbility(WIFI_HOTSPOT_ABILITY_ID, true), mPublishFlag(false), mState(ServiceRunningState::STATE_NOT_START)
{}

WifiHotspotServiceImpl::~WifiHotspotServiceImpl()
{}

void WifiHotspotServiceImpl::OnStart()
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
    WIFI_LOGI("Start ap service!");
    WifiManager::GetInstance().AddSupportedFeatures(WifiFeatures::WIFI_FEATURE_MOBILE_HOTSPOT);
}

void WifiHotspotServiceImpl::OnStop()
{
    mState = ServiceRunningState::STATE_NOT_START;
    mPublishFlag = false;
    WIFI_LOGI("Stop ap service!");
}

bool WifiHotspotServiceImpl::Init()
{
    if (!mPublishFlag) {
        bool ret = Publish(WifiHotspotServiceImpl::GetInstance());
        if (!ret) {
            WIFI_LOGE("Failed to publish hotspot service!");
            return false;
        }
        mPublishFlag = true;
    }
    return true;
}

ErrCode WifiHotspotServiceImpl::IsHotspotActive(bool &bActive)
{
    WIFI_LOGI("IsHotspotActive");
    if (WifiPermissionUtils::VerifyManageWifiHotspotPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("IsHotspotActive:VerifyManageWifiHotspotPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    bActive = IsApServiceRunning();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiHotspotServiceImpl::GetHotspotState(int &state)
{
    WIFI_LOGI("GetHotspotState");
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetHotspotState:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifySetWifiConfigPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetHotspotState:VerifySetWifiConfigPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    state = WifiConfigCenter::GetInstance().GetHotspotState();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiHotspotServiceImpl::GetHotspotConfig(HotspotConfig &result)
{
    WIFI_LOGI("GetHotspotConfig");
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetHotspotConfig:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifyGetWifiConfigPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetHotspotConfig:VerifyGetWifiConfigPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    WifiConfigCenter::GetInstance().GetHotspotConfig(result);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiHotspotServiceImpl::SetHotspotConfig(const HotspotConfig &config)
{
    WIFI_LOGI("SetHotspotConfig band %{public}d", static_cast<int>(config.GetBand()));
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("SetHotspotConfig:VerifySetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifySetWifiConfigPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("SetHotspotConfig:VerifySetWifiConfigPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (!mGetChannels) {
        if  (!WifiConfigCenter::GetInstance().GetSupportedBandChannel()) {
            WIFI_LOGE("Failed to get supported band and channel!");
        } else {
            mGetChannels = true;
        }
    }
    std::vector<BandType> bandsFromCenter;
    WifiConfigCenter::GetInstance().GetValidBands(bandsFromCenter);
    ChannelsTable channInfoFromCenter;
    WifiConfigCenter::GetInstance().GetValidChannels(channInfoFromCenter);
    HotspotConfig configFromCenter;
    WifiConfigCenter::GetInstance().GetHotspotConfig(configFromCenter);
    ErrCode validRetval = IsValidHotspotConfig(config, configFromCenter, bandsFromCenter, channInfoFromCenter);
    if (validRetval != ErrCode::WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Hotspot config is invalid!");
        return validRetval;
    }

    WifiLinkedInfo linkInfo;
    WifiConfigCenter::GetInstance().GetLinkedInfo(linkInfo);
    if (!linkInfo.ssid.empty() && linkInfo.ssid == config.GetSsid()) {
        WIFI_LOGD("set ssid equal current linked ap ssid, no premission!");
        return WIFI_OPT_INVALID_PARAM;
    }

    if (!IsApServiceRunning()) {
        WifiConfigCenter::GetInstance().SetHotspotConfig(config);
    } else {
        IApService *pService = WifiServiceManager::GetInstance().GetApServiceInst();
        if (pService == nullptr) {
            return WIFI_OPT_AP_NOT_OPENED;
        }
        return pService->SetHotspotConfig(config);
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiHotspotServiceImpl::GetStationList(std::vector<StationInfo> &result)
{
    WIFI_LOGI("GetStationList");
    if (WifiPermissionUtils::VerifyGetWifiInfoInternalPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetStationList:VerifyGetWifiInfoInternalPermission PERMISSION_DENIED!");

        if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
            WIFI_LOGE("GetStationList:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
            return WIFI_OPT_PERMISSION_DENIED;
        }

        if (WifiPermissionUtils::VerifyGetWifiLocalMacPermission() == PERMISSION_DENIED) {
            WIFI_LOGE("GetStationList:VerifyGetWifiLocalMacPermission PERMISSION_DENIED!");
            return WIFI_OPT_PERMISSION_DENIED;
        }

        if (WifiPermissionUtils::VerifyGetScanInfosPermission() == PERMISSION_DENIED) {
            WIFI_LOGE("GetStationList:VerifyGetScanInfosPermission PERMISSION_DENIED!");
            return WIFI_OPT_PERMISSION_DENIED;
        }

        if (WifiPermissionUtils::VerifyManageWifiHotspotPermission() == PERMISSION_DENIED) {
            WIFI_LOGE("GetStationList:VerifyManageWifiHotspotPermission PERMISSION_DENIED!");
            return WIFI_OPT_PERMISSION_DENIED;
        }
    }

    if (!IsApServiceRunning()) {
        return WIFI_OPT_AP_NOT_OPENED;
    }

    IApService *pService = WifiServiceManager::GetInstance().GetApServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_AP_NOT_OPENED;
    }
    return pService->GetStationList(result);
}

ErrCode WifiHotspotServiceImpl::DisassociateSta(const StationInfo &info)
{
    WIFI_LOGI("DisassociateSta device name [%{private}s]", info.deviceName.c_str());
    if (WifiPermissionUtils::VerifySetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("DisassociateSta:VerifySetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }
    if (CheckMacIsValid(info.bssid)) {
        return WIFI_OPT_INVALID_PARAM;
    }
    if (!IsApServiceRunning()) {
        return WIFI_OPT_AP_NOT_OPENED;
    }

    IApService *pService = WifiServiceManager::GetInstance().GetApServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_AP_NOT_OPENED;
    }
    return pService->DisconnetStation(info);
}

ErrCode WifiHotspotServiceImpl::CheckCanEnableHotspot(void)
{
    if (WifiPermissionUtils::VerifyManageWifiHotspotPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("EnableHotspot:VerifyManageWifiHotspotPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiConfigCenter::GetInstance().GetAirplaneModeState() == 1) {
        WIFI_LOGI("current airplane mode and can not use ap, open failed!");
        return WIFI_OPT_FORBID_AIRPLANE;
    }
    if (WifiConfigCenter::GetInstance().GetPowerSavingModeState() == 1) {
        WIFI_LOGI("current power saving mode and can not use ap, open failed!");
        return WIFI_OPT_FORBID_POWSAVING;
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiHotspotServiceImpl::EnableHotspot(void)
{
    WIFI_LOGI("EnableHotspot");
    ErrCode errCode = CheckCanEnableHotspot();
    if (errCode != WIFI_OPT_SUCCESS) {
        return errCode;
    }

    WifiOprMidState curState = WifiConfigCenter::GetInstance().GetApMidState();
    if (curState != WifiOprMidState::CLOSED) {
        WIFI_LOGI("current ap state is %{public}d", static_cast<int>(curState));
        if (curState == WifiOprMidState::CLOSING) { /* when ap is closing, return */
            return WIFI_OPT_OPEN_FAIL_WHEN_CLOSING;
        } else {
            return WIFI_OPT_OPEN_SUCC_WHEN_OPENED;
        }
    }
    if (!WifiConfigCenter::GetInstance().SetApMidState(curState, WifiOprMidState::OPENING)) {
        WIFI_LOGI("set ap mid state opening failed! may be other activity has been operated");
        return WIFI_OPT_OPEN_SUCC_WHEN_OPENED;
    }
    errCode = WIFI_OPT_FAILED;
    do {
        if (WifiServiceManager::GetInstance().CheckAndEnforceService(WIFI_SERVICE_AP) < 0) {
            WIFI_LOGE("Load %{public}s service failed!", WIFI_SERVICE_AP);
            break;
        }
        IApService *pService = WifiServiceManager::GetInstance().GetApServiceInst();
        if (pService == nullptr) {
            WIFI_LOGE("Create %{public}s service failed!", WIFI_SERVICE_AP);
            break;
        }
        errCode = pService->RegisterApServiceCallbacks(WifiManager::GetInstance().GetApCallback());
        if (errCode != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("Register ap service callback failed!");
            break;
        }
        errCode = pService->EnableHotspot();
        if (errCode != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("service enable ap failed, ret %{public}d!", static_cast<int>(errCode));
            break;
        }
    } while (false);
    if (errCode != WIFI_OPT_SUCCESS) {
        WifiConfigCenter::GetInstance().SetApMidState(WifiOprMidState::OPENING, WifiOprMidState::CLOSED);
        WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_AP);
    }
    return errCode;
}

ErrCode WifiHotspotServiceImpl::DisableHotspot(void)
{
    WIFI_LOGI("DisableHotspot");
    if (WifiPermissionUtils::VerifyManageWifiHotspotPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("DisableHotspot:VerifyManageWifiHotspotPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    WifiOprMidState curState = WifiConfigCenter::GetInstance().GetApMidState();
    if (curState != WifiOprMidState::RUNNING) {
        WIFI_LOGI("current ap state is %{public}d", static_cast<int>(curState));
        if (curState == WifiOprMidState::OPENING) { /* when ap is opening, return */
            return WIFI_OPT_CLOSE_FAIL_WHEN_OPENING;
        } else {
            return WIFI_OPT_CLOSE_SUCC_WHEN_CLOSED;
        }
    }
    if (!WifiConfigCenter::GetInstance().SetApMidState(curState, WifiOprMidState::CLOSING)) {
        WIFI_LOGI("set ap mid state closing failed! may be other activity has been operated");
        return WIFI_OPT_CLOSE_SUCC_WHEN_CLOSED;
    }
    IApService *pService = WifiServiceManager::GetInstance().GetApServiceInst();
    if (pService == nullptr) {
        WifiConfigCenter::GetInstance().SetApMidState(WifiOprMidState::CLOSED);
        WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_AP);
        return WIFI_OPT_SUCCESS;
    }
    ErrCode ret = pService->DisableHotspot();
    if (ret != WIFI_OPT_SUCCESS) {
        WifiConfigCenter::GetInstance().SetApMidState(WifiOprMidState::CLOSING, WifiOprMidState::RUNNING);
    }
    return ret;
}

ErrCode WifiHotspotServiceImpl::AddBlockList(const StationInfo &info)
{
    WIFI_LOGI("AddBlockList, device name [%{private}s]", info.deviceName.c_str());
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("AddBlockList:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifySetWifiConfigPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("AddBlockList:VerifySetWifiConfigPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }
    if (CheckMacIsValid(info.bssid)) {
        return WIFI_OPT_INVALID_PARAM;
    }
    if (!IsApServiceRunning()) {
        WIFI_LOGE("ApService is not running!");
        return WIFI_OPT_AP_NOT_OPENED;
    }

    if (WifiConfigCenter::GetInstance().AddBlockList(info) < 0) {
        WIFI_LOGE("Add block list failed!");
        return WIFI_OPT_FAILED;
    }
    IApService *pService = WifiServiceManager::GetInstance().GetApServiceInst();
    if (pService == nullptr) {
        return WIFI_OPT_AP_NOT_OPENED;
    }
    return pService->AddBlockList(info);
}

ErrCode WifiHotspotServiceImpl::DelBlockList(const StationInfo &info)
{
    WIFI_LOGI("DelBlockList, device name [%{private}s]", info.deviceName.c_str());
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("DelBlockList:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiPermissionUtils::VerifySetWifiConfigPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("DelBlockList:VerifySetWifiConfigPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (CheckMacIsValid(info.bssid)) {
        return WIFI_OPT_INVALID_PARAM;
    }

    if (IsApServiceRunning()) {
        IApService *pService = WifiServiceManager::GetInstance().GetApServiceInst();
        if (pService == nullptr) {
            return WIFI_OPT_AP_NOT_OPENED;
        }
        if (pService->DelBlockList(info) != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("request del hotspot blocklist failed!");
            return WIFI_OPT_FAILED;
        }
    }

    if (WifiConfigCenter::GetInstance().DelBlockList(info) < 0) {
        WIFI_LOGE("Delete block list failed!");
        return WIFI_OPT_FAILED;
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiHotspotServiceImpl::GetValidBands(std::vector<BandType> &bands)
{
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetValidBands:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiConfigCenter::GetInstance().GetValidBands(bands) < 0) {
        return WIFI_OPT_FAILED;
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiHotspotServiceImpl::GetValidChannels(BandType band, std::vector<int32_t> &validchannels)
{
    WIFI_LOGI("GetValidChannels band %{public}d", static_cast<int>(band));
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetValidChannels:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (band == BandType::BAND_NONE) {
        return WIFI_OPT_INVALID_PARAM;
    }
    ChannelsTable channInfoFromCenter;
    WifiConfigCenter::GetInstance().GetValidChannels(channInfoFromCenter);
    auto iter = channInfoFromCenter.find(band);
    if (iter != channInfoFromCenter.end()) {
        validchannels = iter->second;
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiHotspotServiceImpl::GetBlockLists(std::vector<StationInfo> &infos)
{
    WIFI_LOGI("GetBlockLists");
    if (WifiPermissionUtils::VerifyGetWifiInfoPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("GetBlockLists:VerifyGetWifiInfoPermission PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }

    if (WifiConfigCenter::GetInstance().GetBlockLists(infos) < 0) {
        WIFI_LOGE("Get block list failed!");
        return WIFI_OPT_FAILED;
    }
    return WIFI_OPT_SUCCESS;
}

bool WifiHotspotServiceImpl::IsApServiceRunning()
{
    WifiOprMidState curState = WifiConfigCenter::GetInstance().GetApMidState();
    if (curState != WifiOprMidState::RUNNING) {
        WIFI_LOGI("current ap state is %{public}d", static_cast<int>(curState));
        return false;
    }
    return true;
}

ErrCode WifiHotspotServiceImpl::RegisterCallBack(const sptr<IWifiHotspotCallback> &callback)
{
    WIFI_LOGI("RegisterCallBack");
    if (WifiPermissionUtils::VerifyWifiConnectionPermission() == PERMISSION_DENIED) {
        WIFI_LOGE("RegisterCallBack:VerifyWifiConnectionPermission() PERMISSION_DENIED!");
        return WIFI_OPT_PERMISSION_DENIED;
    }
    WifiInternalEventDispatcher::GetInstance().SetSingleHotspotCallback(callback);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiHotspotServiceImpl::GetSupportedFeatures(long &features)
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

void WifiHotspotServiceImpl::ConfigInfoDump(std::string& result)
{
    HotspotConfig config;
    WifiConfigCenter::GetInstance().GetHotspotConfig(config);
    std::stringstream ss;
    ss << "Hotspot config: " << "\n";
    ss << "  Config.ssid: " << config.GetSsid() << "\n";

    std::map<KeyMgmt, std::string> mapKeyMgmtToStr = {
        {KeyMgmt::NONE, "Open"}, {KeyMgmt::WPA_PSK, "WPA_PSK"}, {KeyMgmt::WPA_EAP, "WPA_EAP"},
        {KeyMgmt::IEEE8021X, "IEEE8021X"}, {KeyMgmt::WPA2_PSK, "WPA2_PSK"}, {KeyMgmt::OSEN, "OSEN"},
        {KeyMgmt::FT_PSK, "FT_PSK"}, {KeyMgmt::FT_EAP, "FT_EAP"}
    };

    auto funcStrKeyMgmt = [&mapKeyMgmtToStr](KeyMgmt secType) {
        std::map<KeyMgmt, std::string>::iterator iter = mapKeyMgmtToStr.find(secType);
        return (iter != mapKeyMgmtToStr.end()) ? iter->second : "Unknow";
    };
    ss << "  Config.security_type: " << funcStrKeyMgmt(config.GetSecurityType()) << "\n";

    auto funcStrBand = [](BandType band) {
        std::string retStr;
        switch (band) {
            case BandType::BAND_2GHZ:
                retStr = "2.4GHz";
                break;
            case BandType::BAND_5GHZ:
                retStr = "5GHz";
                break;
            case BandType::BAND_ANY:
                retStr = "dual-mode frequency band";
                break;
            default:
                retStr = "unknown band";
        }
        return retStr;
    };
    ss << "  Config.band: " << funcStrBand(config.GetBand()) << "\n";
    ss << "  Config.channel: " << config.GetChannel() << "\n";
    ss << "  Config.max_conn: " << config.GetMaxConn() << "\n";
    result += "\n";
    result += ss.str();
    result += "\n";
}

void WifiHotspotServiceImpl::StationsInfoDump(std::string& result)
{
    IApService *pService = WifiServiceManager::GetInstance().GetApServiceInst();
    if (pService != nullptr) {
        std::stringstream ss;
        std::vector<StationInfo> vecStations;
        pService->GetStationList(vecStations);
        ss << "Station list size: " << vecStations.size() << "\n";
        int idx = 0;
        for (auto& each : vecStations) {
            ++idx;
            ss << "  Station[" << idx << "].deviceName: " << each.deviceName << "\n";
            ss << "  Station[" << idx << "].bssid: " << MacAnonymize(each.bssid) << "\n";
            ss << "  Station[" << idx << "].ipAddr: " << IpAnonymize(each.ipAddr) << "\n";
            ss << "\n";
        }
        result += ss.str();
        result += "\n";
    }

    std::vector<StationInfo> vecBlockStations;
    WifiConfigCenter::GetInstance().GetBlockLists(vecBlockStations);
    if (!vecBlockStations.empty()) {
        std::stringstream ss;
        ss << "Block station list size: " << vecBlockStations.size() << "\n";
        int idx = 0;
        for (auto& each : vecBlockStations) {
            ++idx;
            ss << "  BlockStation[" << idx << "].deviceName: " << each.deviceName << "\n";
            ss << "  BlockStation[" << idx << "].bssid: " << MacAnonymize(each.bssid) << "\n";
            ss << "  BlockStation[" << idx << "].ipAddr: " << IpAnonymize(each.ipAddr) << "\n";
            ss << "\n";
        }
        result += ss.str();
        result += "\n";
    }
}

void WifiHotspotServiceImpl::SaBasicDump(std::string& result)
{
    WifiHotspotServiceImpl impl;
    bool isActive = impl.IsApServiceRunning();
    result.append("WiFi hotspot active state: ");
    std::string strActive = isActive ? "activated" : "inactive";
    result += strActive + "\n";

    if (isActive) {
        ConfigInfoDump(result);
        StationsInfoDump(result);
    }
}

int32_t WifiHotspotServiceImpl::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    std::vector<std::string> vecArgs;
    std::transform(args.begin(), args.end(), std::back_inserter(vecArgs), [](const std::u16string &arg) {
        return Str16ToStr8(arg);
    });

    WifiDumper dumper;
    std::string result;
    dumper.HotspotDump(SaBasicDump, vecArgs, result);
    if (!SaveStringToFd(fd, result)) {
        WIFI_LOGE("WiFi hotspot save string to fd failed.");
        return ERR_OK;
    }
    return ERR_OK;
}
}  // namespace Wifi
}  // namespace OHOS
