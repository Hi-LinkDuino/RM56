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

#include "map_mse_service.h"
#include "log_util.h"

namespace OHOS {
namespace bluetooth {
const uint16_t MapMseService::GOEP_L2CAP_PSM_VALUE[LPSM_SIZE] = {
    0x1005, 0x1007, 0x1009, 0x100B, 0x100D, 0x100F, 0x1011, 0x1013, 0x1015, 0x1017, 0x1019, 0x101B
};
MapMseService::MapMseService() : utility::Context(PROFILE_NAME_MAP_MSE, "1.4.2")
{
    MSE_LOG_INFO("%{public}s:%{public}s Create", PROFILE_NAME_MAP_MSE.c_str(), Name().c_str());
}

MapMseService::~MapMseService()
{
    MSE_LOG_INFO("%{public}s:%{public}s Destroy", PROFILE_NAME_MAP_MSE.c_str(), Name().c_str());

    masInstanceMap_.clear();
}

utility::Context *MapMseService::GetContext()
{
    return this;
}

MapMseService::MnsCallback::MnsCallback(MapMseService &mseService) : mseService_(mseService)
{}

void MapMseService::MnsCallback::OnDisconnected()
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    mseService_.DisableComplete();
}

MapMseService::Observer::Observer(MapMseService &mseService) : mseService_(mseService)
{}

void MapMseService::Observer::OnIncomingConnect(const RawAddress &remoteAddr, const int masInstanceId)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    MSE_LOG_INFO("OnIncomingConnect instance id : %{public}d ", masInstanceId);

    auto addr = remoteAddr.GetAddress();
    if (mseService_.incomingConnectMap_.find(addr) != mseService_.incomingConnectMap_.end()) {
        mseService_.incomingConnectMap_[addr] = mseService_.incomingConnectMap_[addr] + (masInstanceId + 1);
        return;
    }
    mseService_.incomingConnectMap_[addr] = masInstanceId + 1;
    mseService_.mapMseObservers_.ForEach(
        [remoteAddr](IMapMseObserver &observer) { observer.OnPermission(remoteAddr); });
}

void MapMseService::Observer::OnTransportConnected(const RawAddress &remoteAddr, const int masInstanceId)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (mseService_.mnsClient_) {
        mseService_.mnsClient_->SdpSearch(remoteAddr);
    }
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_ON, PROFILE_NAME_MAP_MSE, remoteAddr);
}

void MapMseService::Observer::OnTransportDisconnected(const RawAddress &remoteAddr, const int masInstanceId)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    bool hasEnd = false;
    {
        std::lock_guard<std::recursive_mutex> lock(mseService_.mseMapMutex_);
        std::string addr = remoteAddr.GetAddress();
        if (mseService_.incomingConnectMap_.find(addr) != mseService_.incomingConnectMap_.end()) {
            mseService_.incomingConnectMap_[addr] = mseService_.incomingConnectMap_[addr] - (masInstanceId + 1);
            if (mseService_.incomingConnectMap_[addr] == 0) {
                mseService_.incomingConnectMap_.erase(addr);
                hasEnd = true;
            }
        } else {
            hasEnd = true;
        }
    }
    if (hasEnd) {
        if (mseService_.masInstanceMap_.size() > 1) {
            OnRejectConnection(remoteAddr, masInstanceId);
        } else {
            OnConnectionStateChanged(remoteAddr, masInstanceId, BTConnectState::DISCONNECTED);
        }
    }
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_OFF, PROFILE_NAME_MAP_MSE, remoteAddr);
    mseService_.DisableComplete();
}

void MapMseService::Observer::OnBusy(const RawAddress &remoteAddr, bool isBusy)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    auto addr = remoteAddr.GetAddress();
    for (auto &instance : mseService_.masInstanceMap_) {
        if (instance.second->IsBusy(addr)) {
            IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_MAP_MSE, remoteAddr);
            return;
        }
    }
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_MAP_MSE, remoteAddr);
}

void MapMseService::Observer::OnConnectionStateChanged(
    const RawAddress &remoteAddr, const int masInstanceId, BTConnectState state)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    mseService_.NotifyConnectionState(remoteAddr, masInstanceId, state);
}

void MapMseService::Observer::OnIncomingTimeout(const RawAddress &remoteAddr, const int masInstanceId)
{
    std::lock_guard<std::recursive_mutex> lock(mseService_.mseMapMutex_);
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    auto addr = remoteAddr.GetAddress();
    if (mseService_.incomingConnectMap_.find(addr) != mseService_.incomingConnectMap_.end()) {
        MSE_LOG_INFO("IncomingTimeout instance : %{public}d ", mseService_.incomingConnectMap_[addr]);
        mseService_.incomingConnectMap_[addr] = mseService_.incomingConnectMap_[addr] - (masInstanceId + 1);
        if (mseService_.incomingConnectMap_[addr] == 0) {
            mseService_.incomingConnectMap_.erase(addr);
            mseService_.mapMseObservers_.ForEach([remoteAddr](IMapMseObserver &observer) {
                observer.OnConnectionStateChanged(remoteAddr, static_cast<int>(BTConnectState::DISCONNECTED));
            });
        }
    }
}

void MapMseService::Observer::OnRejectConnection(const RawAddress &remoteAddr, const int masInstanceId)
{
    std::lock_guard<std::recursive_mutex> lock(mseService_.mseMapMutex_);
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    auto addr = remoteAddr.GetAddress();
    if (mseService_.incomingConnectMap_.find(addr) != mseService_.incomingConnectMap_.end()) {
        MSE_LOG_INFO("Reject Connection instance : %{public}d ", mseService_.incomingConnectMap_[addr]);
        return;
    }
    mseService_.RejectConnection(addr, masInstanceId);
}

void MapMseService::RejectConnection(std::string &addr, const int masInstanceId)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    rejectTimer_[addr] =
        std::make_unique<utility::Timer>(std::bind(&MapMseService::ProcessReject, this, addr, masInstanceId));
    rejectTimer_[addr]->Start(MAP_MSE_DELAY_GRANT_TIME * masInstanceMap_.size(), false);
}

void MapMseService::ProcessReject(std::string &addr, const int masInstanceId)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    RawAddress remoteAddr(addr);
    mapMseObservers_.ForEach([remoteAddr](IMapMseObserver &observer) {
        observer.OnConnectionStateChanged(remoteAddr, static_cast<int>(BTConnectState::DISCONNECTED));
    });
}

void MapMseService::RegisterObserver(MapMseInstance &masInstance, const int masId)
{
    std::lock_guard<std::recursive_mutex> lock(mseMapMutex_);
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (observerMap_.find(masId) != observerMap_.end()) {
        masInstance.RegistMapObserver(*static_cast<MapObserver *>(observerMap_[masId].get()));
    }
}

void MapMseService::DeregisterObserver(MapMseInstance &masInstance, const int masId)
{
    std::lock_guard<std::recursive_mutex> lock(mseMapMutex_);
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (observerMap_.find(masId) != observerMap_.end()) {
        masInstance.DeregisterObserver(*(static_cast<MapObserver *>(observerMap_[masId].get())));
    }
}

void MapMseService::NotifyConnectionState(const RawAddress &remoteAddr, const int masInstanceId, BTConnectState state)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    BTConnectState currentState = BTConnectState(GetConnectionState(remoteAddr));
    MSE_LOG_INFO("CurrentState : %{public}d ", (int)currentState);
    MSE_LOG_INFO("Change to state : %{public}d ", (int)state);
    bool notify = false;
    switch (currentState) {
        case BTConnectState::DISCONNECTED:
            if (BTConnectState::CONNECTING == state || BTConnectState::DISCONNECTED == state) {
                notify = true;
            }
            break;
        case BTConnectState::CONNECTED:
            if (BTConnectState::DISCONNECTING == state) {
                if (masInstanceMap_.find(masInstanceId) != masInstanceMap_.end()) {
                    masInstanceMap_[masInstanceId]->SetConnectState(
                        remoteAddr.GetAddress(), BTConnectState::DISCONNECTING);
                }
                notify = true;
            }
            if (BTConnectState::CONNECTED == state || BTConnectState::DISCONNECTING == state) {
                notify = true;
            }
            break;
        case BTConnectState::CONNECTING:
            break;
        case BTConnectState::DISCONNECTING:
            break;
        default:
            break;
    }
    if (notify) {
        mapMseObservers_.ForEach([remoteAddr, state](IMapMseObserver &observer) {
            observer.OnConnectionStateChanged(remoteAddr, static_cast<int>(state));
        });
    }
}

void MapMseService::Enable(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    PostMessage(INIT_INSTANCE);
    PostMessage(START_INSTANCE);
}

void MapMseService::ServiceStartUpComplete(bool whether)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    shutDownEnable_ = false;
    GetContext()->OnEnable(PROFILE_NAME_MAP_MSE, whether);
}

std::vector<MapAccountItem> MapMseService::GetAccountItems(void) const
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::vector<MapAccountItem> items;
    items.clear();
    for (auto &iter : stub::MapService::GetInstance()->GetEmailImAccounts()) {
        MapAccountItem item;
        item.id_ = iter->GetAccountId();
        item.name_ = iter->GetName();
        item.msgType_ = (stub::AccountType::EMAIL == iter->GetType()) ? MessageType::EMAIL : MessageType::IM;
        item.pAuthority_ = iter->GetProviderAuthority();
        item.uci_ = iter->GetUci();
        item.uciPrefix_ = iter->GetUciPrefix();
        items.push_back(std::move(item));
    }
    return items;
}

void MapMseService::InitMasInstances(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    masInstanceMap_.clear();
    incomingConnectMap_.clear();
    rejectTimer_.clear();
    int masId = MAS_ID_SMS_MMS;
    uint16_t psm = GOEP_L2CAP_PSM_VALUE[masId];

    bool smsSupport = stub::MapService::GetInstance()->GetSystemTelephonyService().isSmsCapable;
    if (smsSupport) {
        masInstanceMap_[masId] = std::make_unique<MapMseInstance>(*GetDispatcher(), masId, psm, smsSupport);
        observerMap_[masId] = std::make_unique<Observer>(*this);
        masId++;
    }
    auto items = GetAccountItems();
    for (auto &iter : items) {
        if (masId == 0xC) {
            break;
        }
        psm = GOEP_L2CAP_PSM_VALUE[masId];
        masInstanceMap_[masId] = std::make_unique<MapMseInstance>(*GetDispatcher(), masId, psm, iter, smsSupport);
        observerMap_[masId] = std::make_unique<Observer>(*this);
        masId++;
    }
}

void MapMseService::InstancesStarting(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    mnsObserver_ = std::make_unique<MnsCallback>(*this);
    mnsClient_ = std::make_unique<MapMseMnscli>(*GetDispatcher(), (MnsObserver &)*mnsObserver_);
    bool notifyFlag = true;
    for (auto iter = masInstanceMap_.begin(); iter != masInstanceMap_.end(); iter++) {
        iter->second->CreateMasSdpRecord();
        iter->second->RegistGap();
        if (Result::SUCCEED != iter->second->Enable(*mnsClient_)) {
            notifyFlag = false;
        }
        RegisterObserver(*iter->second, iter->first);
    }
    ServiceStartUpComplete(notifyFlag);
}

void MapMseService::Disable(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    PostMessage(SHUT_DOWN);
}

void MapMseService::ServiceShutDown(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    shutDownEnable_ = true;
    bool hasSession = false;
    bool hasMnsConnect = false;
    for (auto &instance : masInstanceMap_) {
        for (auto &incoming : incomingConnectMap_) {
            instance.second->DestroyIncomingConnect(incoming.first);
        }
        if (instance.second->IsConnected()) {
            hasSession = true;
            instance.second->DisConnect("");
        }
        if (instance.second->AcceptedNoCallBack(shutDownEnable_)) {
            hasSession = true;
        }
    }
    if (mnsClient_) {
        hasMnsConnect = mnsClient_->IsDisconnected();
    }
    if (!hasSession && hasMnsConnect) {
        shutDownEnable_ = false;
        PostMessage(SHUT_DOWN_COMPLETE);
    }
}

void MapMseService::DisableComplete(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    bool notEnd = true;
    for (auto &instance : masInstanceMap_) {
        if (instance.second->IsConnected()) {
            notEnd = false;
            break;
        }
        if (instance.second->AcceptedNoCallBack(shutDownEnable_)) {
            notEnd = false;
            break;
        }
    }
    bool mnsNotEnd = true;
    if (mnsClient_) {
        if (!mnsClient_->IsDisconnected()) {
            mnsNotEnd = false;
        }
    }
    if (shutDownEnable_ && notEnd && mnsNotEnd) {
        shutDownEnable_ = false;
        PostMessage(SHUT_DOWN_COMPLETE);
    }
}

void MapMseService::ServiceShutDownComplete(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    for (auto iter = masInstanceMap_.begin(); iter != masInstanceMap_.end(); iter++) {
        DeregisterObserver(*iter->second, iter->first);
        iter->second->DestroyMasSdpRecord();
        iter->second->UnRegistGap();
        iter->second->Disable();
    }
    if (mnsClient_) {
        mnsClient_->DeregisterL2capLPsm();
    }
    GetContext()->OnDisable(PROFILE_NAME_MAP_MSE, true);
    shutDownEnable_ = false;
}

int MapMseService::Disconnect(const RawAddress &device)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (BTConnectState::DISCONNECTING == BTConnectState(GetConnectionState(device))) {
        return RET_BAD_STATUS;
    }
    for (auto &instance : masInstanceMap_) {
        if (instance.second->IsConnected(device.GetAddress()) == false) {
            return RET_BAD_STATUS;
        }
    }
    utility::Message msg(DISCONNECT);
    std::unique_ptr<RawAddress> rawAddr = std::make_unique<RawAddress>(device.GetAddress());
    msg.arg2_ = (void *)rawAddr.release();
    GetDispatcher()->PostTask(std::bind(&MapMseService::ProcessMessage, this, msg));
    return RET_NO_ERROR;
}

void MapMseService::DisConnectInternal(const utility::Message &msg) const
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    RawAddress *rawAddress = (RawAddress *)msg.arg2_;
    if (!rawAddress) {
        return;
    }
    for (auto &instance : masInstanceMap_) {
        instance.second->DisConnect(rawAddress->GetAddress());
    }
    delete rawAddress;
}

void MapMseService::PostMessage(int what)
{
    utility::Message msg(what);
    GetDispatcher()->PostTask(std::bind(&MapMseService::ProcessMessage, this, msg));
}

void MapMseService::ProcessMessage(const utility::Message &msg)
{
    switch (msg.what_) {
        case INIT_INSTANCE:
            InitMasInstances();
            break;
        case START_INSTANCE:
            InstancesStarting();
            break;
        case DISCONNECT:
            DisConnectInternal(msg);
            break;
        case SHUT_DOWN:
            ServiceShutDown();
            break;
        case SHUT_DOWN_COMPLETE:
            ServiceShutDownComplete();
            break;
        default:
            break;
    }
}

int MapMseService::GetState(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    int result = GetConnectState();
    if (result & PROFILE_STATE_CONNECTED) {
        return (int)BTConnectState::CONNECTED;
    } else if (result & PROFILE_STATE_CONNECTING) {
        return (int)BTConnectState::CONNECTING;
    } else if (result & PROFILE_STATE_DISCONNECTING) {
        return (int)BTConnectState::DISCONNECTING;
    } else {
        return (int)BTConnectState::DISCONNECTED;
    }
}

bool MapMseService::IsConnected(const RawAddress &device)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    bool result = false;
    for (auto &instance : masInstanceMap_) {
        if (instance.second->IsConnected(device.GetAddress())) {
            result = true;
            break;
        }
    }
    return result;
}

std::vector<RawAddress> MapMseService::GetDevicesByStates(std::vector<int> states)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::vector<RawAddress> devices;
    for (auto &instance : masInstanceMap_) {
        instance.second->GetDevicesByStates(devices, states);
    }
    return devices;
}

int MapMseService::GetConnectionState(const RawAddress &device)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    int result = 0;
    for (auto &instance : masInstanceMap_) {
        instance.second->GetDeviceState(device.GetAddress(), result);
    }
    MSE_LOG_INFO("Get Connection State : %{public}d", result);
    if (result & PROFILE_STATE_DISCONNECTING) {
        return (int)BTConnectState::DISCONNECTING;
    } else if (result & PROFILE_STATE_CONNECTING) {
        return (int)BTConnectState::CONNECTING;
    } else if (result & PROFILE_STATE_CONNECTED) {
        return (int)BTConnectState::CONNECTED;
    } else {
        return (int)BTConnectState::DISCONNECTED;
    }
}

bool MapMseService::SetConnectionStrategy(const RawAddress &device, int strategy)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    IProfileConfig *config = ProfileConfig::GetInstance();
    int value = 0;
    std::string addr = device.GetAddress();
    switch (BTStrategyType(strategy)) {
        case BTStrategyType::CONNECTION_UNKNOWN:
            return false;
        case BTStrategyType::CONNECTION_ALLOWED:
            value = 1;
            break;
        case BTStrategyType::CONNECTION_FORBIDDEN:
            break;
        default:
            return false;
    }
    return config->SetValue(device.GetAddress(), SECTION_CONNECTION_POLICIES, PROPERTY_MAP_CONNECTION_POLICY, value);
}

int MapMseService::GetConnectionStrategy(const RawAddress &device)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    IProfileConfig *config = ProfileConfig::GetInstance();
    int value = 0;
    if (!config->GetValue(device.GetAddress(), SECTION_CONNECTION_POLICIES, PROPERTY_MAP_CONNECTION_POLICY, value)) {
        HILOGI("addr: %{public}s, %{public}s not found",
            GET_ENCRYPT_ADDR(device), PROPERTY_MAP_CONNECTION_POLICY.c_str());
        return (int)BTStrategyType::CONNECTION_UNKNOWN;
    }
    if (value) {
        return (int)BTStrategyType::CONNECTION_ALLOWED;
    } else {
        return (int)BTStrategyType::CONNECTION_FORBIDDEN;
    }
}

void MapMseService::GrantPermission(const RawAddress &device, bool allow, bool save)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    GetDispatcher()->PostTask(std::bind(&MapMseService::GrantPermissionTimer, this, device, allow, save));
}

void MapMseService::GrantPermissionTimer(const RawAddress &device, bool allow, bool save)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (BTConnectState::CONNECTING != BTConnectState(GetConnectionState(device))) {
        MSE_LOG_INFO("GrantPermissionTimer is returned.");
        return;
    }
    std::string addr = device.GetAddress();
    grantTimer_[addr] =
        std::make_unique<utility::Timer>(std::bind(&MapMseService::ProcessGrantPermission, this, device, allow, save));
    grantTimer_[addr]->Start(MAP_MSE_DELAY_GRANT_TIME * masInstanceMap_.size(), false);
}

void MapMseService::ProcessGrantPermission(const RawAddress &device, bool allow, bool save)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    GetDispatcher()->PostTask(std::bind(&MapMseService::GrantPermissionNative, this, device, allow, save));
}

void MapMseService::GrantPermissionNative(const RawAddress &device, bool allow, bool save)
{
    {
        std::lock_guard<std::recursive_mutex> lock(mseMapMutex_);
        MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
        std::string addr = device.GetAddress();
        incomingConnectMap_.erase(addr);
    }
    for (auto &instance : masInstanceMap_) {
        instance.second->GrantPermission(device, allow, save);
    }
}

int MapMseService::Connect(const RawAddress &device)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    return Result::SUCCEED;
}

std::list<RawAddress> MapMseService::GetConnectDevices(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::list<RawAddress> devList;
    for (auto &instance : masInstanceMap_) {
        instance.second->GetConnectDevices(devList);
    }
    return devList;
}

int MapMseService::GetConnectState(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    uint8_t result = PROFILE_STATE_DISCONNECTED;
    for (auto &instance : masInstanceMap_) {
        result |= instance.second->GetConnectState();
    }
    return result;
}

int MapMseService::GetMaxConnectNum(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    int maxNum = MAP_MSE_MAX_DEFAULT_CONNECTIONS_NUMR;
    if (AdapterConfig::GetInstance()->GetValue(SECTION_MAP_MSE_SERVICE, PROPERTY_MAX_CONNECTED_DEVICES, maxNum)) {
        return maxNum;
    }
    return maxNum;
}

void MapMseService::RegisterObserver(IMapMseObserver &mapMseObserver)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    mapMseObservers_.Register(mapMseObserver);
}

void MapMseService::DeregisterObserver(IMapMseObserver &mapMseObserver)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    mapMseObservers_.Deregister(mapMseObserver);
}
REGISTER_CLASS_CREATOR(MapMseService);
}  // namespace bluetooth
}  // namespace OHOS