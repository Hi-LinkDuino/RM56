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
#include "wifi_p2p_impl.h"
#include "iservice_registry.h"
#include "if_config.h"
#include "mac_address.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_P2P_LABEL("WifiP2pImpl");
namespace OHOS {
namespace Wifi {

#define RETURN_IF_FAIL(cond)                          \
    do {                                              \
        if (!(cond)) {                                \
            WIFI_LOGI("'%{public}s' failed.", #cond); \
            return WIFI_OPT_FAILED;                   \
        }                                             \
    } while (0)

WifiP2pImpl::WifiP2pImpl(int systemAbilityId) : systemAbilityId_(systemAbilityId), client_(nullptr)
{}

WifiP2pImpl::~WifiP2pImpl()
{}

bool WifiP2pImpl::Init(void)
{
    sptr<ISystemAbilityManager> sa_mgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sa_mgr == nullptr) {
        WIFI_LOGE("failed to get SystemAbilityManager");
        return false;
    }

    sptr<IRemoteObject> object = sa_mgr->GetSystemAbility(systemAbilityId_);
    if (object == nullptr) {
        WIFI_LOGE("failed to get P2P_SERVICE");
        return false;
    }

    client_ = iface_cast<IWifiP2p>(object);
    if (client_ == nullptr) {
        client_ = new (std::nothrow) WifiP2pProxy(object);
    }

    if (client_ == nullptr) {
        WIFI_LOGE("wifi p2p init failed. %{public}d", systemAbilityId_);
        return false;
    }

    return true;
}

ErrCode WifiP2pImpl::EnableP2p(void)
{
    RETURN_IF_FAIL(client_);
    return client_->EnableP2p();
}

ErrCode WifiP2pImpl::DisableP2p(void)
{
    RETURN_IF_FAIL(client_);
    return client_->DisableP2p();
}

ErrCode WifiP2pImpl::DiscoverDevices(void)
{
    RETURN_IF_FAIL(client_);
    return client_->DiscoverDevices();
}

ErrCode WifiP2pImpl::StopDiscoverDevices(void)
{
    RETURN_IF_FAIL(client_);
    return client_->StopDiscoverDevices();
}

ErrCode WifiP2pImpl::DiscoverServices(void)
{
    RETURN_IF_FAIL(client_);
    return client_->DiscoverServices();
}

ErrCode WifiP2pImpl::StopDiscoverServices(void)
{
    RETURN_IF_FAIL(client_);
    return client_->StopDiscoverServices();
}

ErrCode WifiP2pImpl::RequestService(const WifiP2pDevice &device, const WifiP2pServiceRequest &request)
{
    RETURN_IF_FAIL(client_);
    return client_->RequestService(device, request);
}

ErrCode WifiP2pImpl::PutLocalP2pService(const WifiP2pServiceInfo &srvInfo)
{
    RETURN_IF_FAIL(client_);
    return client_->PutLocalP2pService(srvInfo);
}

ErrCode WifiP2pImpl::DeleteLocalP2pService(const WifiP2pServiceInfo &srvInfo)
{
    RETURN_IF_FAIL(client_);
    return client_->DeleteLocalP2pService(srvInfo);
}

ErrCode WifiP2pImpl::StartP2pListen(int period, int interval)
{
    RETURN_IF_FAIL(client_);
    return client_->StartP2pListen(period, interval);
}

ErrCode WifiP2pImpl::StopP2pListen(void)
{
    RETURN_IF_FAIL(client_);
    return client_->StopP2pListen();
}

ErrCode WifiP2pImpl::CreateGroup(const WifiP2pConfig &config)
{
    RETURN_IF_FAIL(client_);
    return client_->CreateGroup(config);
}

ErrCode WifiP2pImpl::RemoveGroup(void)
{
    RETURN_IF_FAIL(client_);
    return client_->RemoveGroup();
}

ErrCode WifiP2pImpl::DeleteGroup(const WifiP2pGroupInfo &group)
{
    RETURN_IF_FAIL(client_);
    return client_->DeleteGroup(group);
}

ErrCode WifiP2pImpl::P2pConnect(const WifiP2pConfig &config)
{
    RETURN_IF_FAIL(client_);
    return client_->P2pConnect(config);
}

ErrCode WifiP2pImpl::P2pCancelConnect(void)
{
    RETURN_IF_FAIL(client_);
    return client_->P2pCancelConnect();
}

ErrCode WifiP2pImpl::QueryP2pLinkedInfo(WifiP2pLinkedInfo &linkedInfo)
{
    RETURN_IF_FAIL(client_);
    return client_->QueryP2pLinkedInfo(linkedInfo);
}

ErrCode WifiP2pImpl::GetCurrentGroup(WifiP2pGroupInfo &group)
{
    RETURN_IF_FAIL(client_);
    return client_->GetCurrentGroup(group);
}

ErrCode WifiP2pImpl::GetP2pEnableStatus(int &status)
{
    RETURN_IF_FAIL(client_);
    return client_->GetP2pEnableStatus(status);
}

ErrCode WifiP2pImpl::GetP2pDiscoverStatus(int &status)
{
    RETURN_IF_FAIL(client_);
    return client_->GetP2pDiscoverStatus(status);
}

ErrCode WifiP2pImpl::GetP2pConnectedStatus(int &status)
{
    RETURN_IF_FAIL(client_);
    return client_->GetP2pConnectedStatus(status);
}

ErrCode WifiP2pImpl::QueryP2pDevices(std::vector<WifiP2pDevice> &devives)
{
    RETURN_IF_FAIL(client_);
    return client_->QueryP2pDevices(devives);
}

ErrCode WifiP2pImpl::QueryP2pGroups(std::vector<WifiP2pGroupInfo> &groups)
{
    RETURN_IF_FAIL(client_);
    return client_->QueryP2pGroups(groups);
}

ErrCode WifiP2pImpl::QueryP2pServices(std::vector<WifiP2pServiceInfo> &services)
{
    RETURN_IF_FAIL(client_);
    return client_->QueryP2pServices(services);
}

ErrCode WifiP2pImpl::RegisterCallBack(const sptr<IWifiP2pCallback> &callback)
{
    RETURN_IF_FAIL(client_);
    return client_->RegisterCallBack(callback);
}

ErrCode WifiP2pImpl::GetSupportedFeatures(long &features)
{
    RETURN_IF_FAIL(client_);
    return client_->GetSupportedFeatures(features);
}

bool WifiP2pImpl::IsFeatureSupported(long feature)
{
    RETURN_IF_FAIL(client_);
    long tmpFeatures = 0;
    if (client_->GetSupportedFeatures(tmpFeatures) != WIFI_OPT_SUCCESS) {
        return false;
    }
    return ((tmpFeatures & feature) == feature);
}

ErrCode WifiP2pImpl::SetP2pDeviceName(const std::string &deviceName)
{
    RETURN_IF_FAIL(client_);
    return client_->SetP2pDeviceName(deviceName);
}

ErrCode WifiP2pImpl::SetP2pWfdInfo(const WifiP2pWfdInfo &wfdInfo)
{
    RETURN_IF_FAIL(client_);
    return client_->SetP2pWfdInfo(wfdInfo);
}

ErrCode WifiP2pImpl::Hid2dRequestGcIp(const std::string& gcMac, std::string& ipAddr)
{
    RETURN_IF_FAIL(client_);
    return client_->Hid2dRequestGcIp(gcMac, ipAddr);
}

ErrCode WifiP2pImpl::Hid2dSharedlinkIncrease()
{
    RETURN_IF_FAIL(client_);
    return client_->Hid2dSharedlinkIncrease();
}

ErrCode WifiP2pImpl::Hid2dSharedlinkDecrease()
{
    RETURN_IF_FAIL(client_);
    return client_->Hid2dSharedlinkDecrease();
}

ErrCode WifiP2pImpl::Hid2dCreateGroup(const int frequency, FreqType type)
{
    RETURN_IF_FAIL(client_);
    return client_->Hid2dCreateGroup(frequency, type);
}

ErrCode WifiP2pImpl::Hid2dRemoveGcGroup(const std::string& gcIfName)
{
    RETURN_IF_FAIL(client_);
    return client_->Hid2dRemoveGcGroup(gcIfName);
}

ErrCode WifiP2pImpl::Hid2dConnect(const Hid2dConnectConfig& config)
{
    RETURN_IF_FAIL(client_);
    return client_->Hid2dConnect(config);
}

ErrCode WifiP2pImpl::Hid2dConfigIPAddr(const std::string& ifName, const IpAddrInfo& ipInfo)
{
    RETURN_IF_FAIL(client_);
    return client_->Hid2dConfigIPAddr(ifName, ipInfo);
}

ErrCode WifiP2pImpl::Hid2dReleaseIPAddr(const std::string& ifName)
{
    RETURN_IF_FAIL(client_);
    return client_->Hid2dReleaseIPAddr(ifName);
}

ErrCode WifiP2pImpl::Hid2dGetRecommendChannel(const RecommendChannelRequest& request,
    RecommendChannelResponse& response)
{
    RETURN_IF_FAIL(client_);
    return client_->Hid2dGetRecommendChannel(request, response);
}

ErrCode WifiP2pImpl::Hid2dGetChannelListFor5G(std::vector<int>& vecChannelList)
{
    RETURN_IF_FAIL(client_);
    return client_->Hid2dGetChannelListFor5G(vecChannelList);
}

ErrCode WifiP2pImpl::Hid2dGetSelfWifiCfgInfo(SelfCfgType cfgType, char cfgData[CFG_DATA_MAX_BYTES], int* getDatValidLen)
{
    RETURN_IF_FAIL(client_);
    return client_->Hid2dGetSelfWifiCfgInfo(cfgType, cfgData, getDatValidLen);
}

ErrCode WifiP2pImpl::Hid2dSetPeerWifiCfgInfo(PeerCfgType cfgType, char cfgData[CFG_DATA_MAX_BYTES], int setDataValidLen)
{
    RETURN_IF_FAIL(client_);
    return client_->Hid2dSetPeerWifiCfgInfo(cfgType, cfgData, setDataValidLen);
}
}  // namespace Wifi
}  // namespace OHOS