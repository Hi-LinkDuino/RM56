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
#include "wifi_hotspot_impl.h"
#include "iservice_registry.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_HOTSPOT_LABEL("WifiHotspotImpl");

namespace OHOS {
namespace Wifi {
#define RETURN_IF_FAIL(cond)                          \
    do {                                              \
        if (!(cond)) {                                \
            WIFI_LOGI("'%{public}s' failed.", #cond); \
            return WIFI_OPT_FAILED;                   \
        }                                             \
    } while (0)

WifiHotspotImpl::WifiHotspotImpl(int systemAbilityId) : systemAbilityId_(systemAbilityId)
{}

WifiHotspotImpl::~WifiHotspotImpl()
{}

bool WifiHotspotImpl::Init()
{
    sptr<ISystemAbilityManager> sa_mgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sa_mgr == nullptr) {
        WIFI_LOGE("failed to get SystemAbilityManager");
        return false;
    }

    sptr<IRemoteObject> object = sa_mgr->GetSystemAbility(systemAbilityId_);
    if (object == nullptr) {
        WIFI_LOGE("failed to get HOTSPOT_SERVICE");
        return false;
    }

    client_ = iface_cast<IWifiHotspot>(object);
    if (client_ == nullptr) {
        client_ = new (std::nothrow) WifiHotspotProxy(object);
    }

    if (client_ == nullptr) {
        WIFI_LOGE("wifi device init failed. %{public}d", systemAbilityId_);
        return false;
    }

    return true;
}

bool WifiHotspotImpl::IsHotspotActive(void)
{
    if (!(client_)) {
        WIFI_LOGI("get client failed.");
        return false;
    }
    bool bActive = false;
    client_->IsHotspotActive(bActive);
    return bActive;
}

ErrCode WifiHotspotImpl::GetHotspotState(int &state)
{
    RETURN_IF_FAIL(client_);
    return client_->GetHotspotState(state);
}

ErrCode WifiHotspotImpl::GetHotspotConfig(HotspotConfig &config)
{
    RETURN_IF_FAIL(client_);
    return client_->GetHotspotConfig(config);
}

ErrCode WifiHotspotImpl::SetHotspotConfig(const HotspotConfig &config)
{
    RETURN_IF_FAIL(client_);
    return client_->SetHotspotConfig(config);
}

ErrCode WifiHotspotImpl::GetStationList(std::vector<StationInfo> &result)
{
    RETURN_IF_FAIL(client_);
    return client_->GetStationList(result);
}

ErrCode WifiHotspotImpl::DisassociateSta(const StationInfo &info)
{
    RETURN_IF_FAIL(client_);
    return client_->DisassociateSta(info);
}

ErrCode WifiHotspotImpl::EnableHotspot(void)
{
    RETURN_IF_FAIL(client_);
    return client_->EnableHotspot();
}

ErrCode WifiHotspotImpl::DisableHotspot(void)
{
    RETURN_IF_FAIL(client_);
    return client_->DisableHotspot();
}

ErrCode WifiHotspotImpl::GetBlockLists(std::vector<StationInfo> &infos)
{
    RETURN_IF_FAIL(client_);
    return client_->GetBlockLists(infos);
}

ErrCode WifiHotspotImpl::AddBlockList(const StationInfo &info)
{
    RETURN_IF_FAIL(client_);
    return client_->AddBlockList(info);
}

ErrCode WifiHotspotImpl::DelBlockList(const StationInfo &info)
{
    RETURN_IF_FAIL(client_);
    return client_->DelBlockList(info);
}

ErrCode WifiHotspotImpl::GetValidBands(std::vector<BandType> &bands)
{
    RETURN_IF_FAIL(client_);
    return client_->GetValidBands(bands);
}

ErrCode WifiHotspotImpl::GetValidChannels(BandType band, std::vector<int32_t> &validchannels)
{
    RETURN_IF_FAIL(client_);
    return client_->GetValidChannels(band, validchannels);
}

ErrCode WifiHotspotImpl::RegisterCallBack(const sptr<IWifiHotspotCallback> &callback)
{
    RETURN_IF_FAIL(client_);
    return client_->RegisterCallBack(callback);
}

ErrCode WifiHotspotImpl::GetSupportedFeatures(long &features)
{
    RETURN_IF_FAIL(client_);
    return client_->GetSupportedFeatures(features);
}

bool WifiHotspotImpl::IsFeatureSupported(long feature)
{
    RETURN_IF_FAIL(client_);
    long tmpFeatures = 0;
    if (client_->GetSupportedFeatures(tmpFeatures) != WIFI_OPT_SUCCESS) {
        return false;
    }
    return ((tmpFeatures & feature) == feature);
}
}  // namespace Wifi
}  // namespace OHOS