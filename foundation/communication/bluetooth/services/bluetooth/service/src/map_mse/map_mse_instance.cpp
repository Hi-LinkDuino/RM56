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

#include "map_mse_instance.h"
#include "adapter_config.h"
#include "btm.h"
#include "map_mse_server.h"
#include "map_mse_service.h"

namespace OHOS {
namespace bluetooth {
struct MapMseInstance::impl {
    class ConnectObserver : public MseObserver {
    public:
        /**
         * @brief Construct a new Connect Observer object
         *
         * @param masInstance Reference to the Mas instance
         */
        explicit ConnectObserver(MapMseInstance &masInstance);

        /**
         * @brief A destructor used to delete the Connect Observer object.
         *
         */
        virtual ~ConnectObserver() = default;

    private:
        /**
         * @brief The function is called when new MCE connection available
         *
         * @param session Reference to the ObexServerSession Value
         */
        void OnConnected(ObexServerSession &session) override;

        /**
         * @brief The function is called when MCE connect is incoming
         *
         * @param incomingConnect Reference to the ObexIncomingConnect Value
         */
        void OnIncomingConnect(ObexIncomingConnect &incomingConnect) override;

        /**
         * @brief The function is called when MCE transport disconnect is incoming
         *
         * @param remoteAddr Remote Bluetooth address
         */
        void OnTransportDisconnected(const RawAddress &remoteAddr) override;

        /**
         * @brief The function is called when MCE Disconnect is incoming
         *
         * @param remoteAddr Remote Bluetooth address
         */
        void OnDisconnect(const RawAddress &remoteAddr) override;

        /**
         * @brief The function is called when Obex busy or not busy
         *
         * @param remoteAddr Remote Bluetooth address
         * @param isBusy true:busy false:not busy
         */
        void OnBusy(const RawAddress &remoteAddr, bool isBusy) override;

        /**
         * @brief Permission handle for incoming connection
         *
         * @param incomingConnect Reference to the ObexIncomingConnect Value
         */
        void PermissionHandle(ObexIncomingConnect &incomingConnect);

        MapMseInstance &masInstance_;

        BT_DISALLOW_COPY_AND_ASSIGN(ConnectObserver);
    };
    std::unique_ptr<MapMseServer> mseServer_ = nullptr;
    std::unique_ptr<MapMseResource> content_ = nullptr;
    std::unique_ptr<ConnectObserver> connectObserver_ = nullptr;
    std::unordered_map<std::string, ObexIncomingConnect *> obexIncomingMap_ {};
    std::unordered_map<std::string, ObexServerSession *> obexSessionMap_ {};
};

MapMseInstance::MapMseInstance(utility::Dispatcher &dispatcher, uint8_t masId, uint16_t l2capPsm,
    const MapAccountItem &accountItem, bool smsSupport)
    : dispatcher_(dispatcher),
      masInstanceId_(masId),
      goepL2capPsm_(l2capPsm),
      accountItem_(accountItem),
      smsSupport_(smsSupport),
      pimpl(std::make_unique<impl>())
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    pimpl->connectObserver_ = std::make_unique<MapMseInstance::impl::ConnectObserver>(*this);
    shutDownEnable_ = false;
}

MapMseInstance::MapMseInstance(utility::Dispatcher &dispatcher, uint8_t masId, uint16_t l2capPsm, bool smsSupport)
    : dispatcher_(dispatcher),
      masInstanceId_(masId),
      goepL2capPsm_(l2capPsm),
      smsSupport_(smsSupport),
      pimpl(std::make_unique<impl>())
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    accountItem_ = MapAccountItem();
    pimpl->connectObserver_ = std::make_unique<MapMseInstance::impl::ConnectObserver>(*this);
    shutDownEnable_ = false;
}

MapMseInstance::~MapMseInstance()
{
    stateMap_.clear();
    nativeMap_.clear();
    pimpl->obexIncomingMap_.clear();
    pimpl->obexSessionMap_.clear();
}

int MapMseInstance::RegistMapObserver(MapObserver &observer)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    mapObserver_ = &observer;
    return Result::SUCCEED;
}

void MapMseInstance::DeregisterObserver(MapObserver &observer)
{
    mapObserver_ = nullptr;
}

MapMseInstance::NativeData::NativeData(MapMseInstance &masInstance) : masInstance_(masInstance)
{
    MSE_LOG_INFO("%{public}s Create", __PRETTY_FUNCTION__);
}

bool MapMseInstance::NativeData::StartTimer(std::function<void()> callback, int ms)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    timer_ = std::make_unique<utility::Timer>(callback);
    return timer_->Start(ms, false);
}

void MapMseInstance::NativeData::StopTimer()
{
    if (timer_) {
        MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
        timer_->Stop();
        timer_ = nullptr;
    }
}

MapMseInstance::impl::ConnectObserver::ConnectObserver(MapMseInstance &masInstance) : masInstance_(masInstance)
{
    MSE_LOG_INFO("%{public}s Create", __PRETTY_FUNCTION__);
}

void MapMseInstance::impl::ConnectObserver::OnConnected(ObexServerSession &session)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string remoteAddr = session.GetRemoteAddr().GetAddress();
    {
        std::lock_guard<std::recursive_mutex> lock(masInstance_.instanceMapMutex_);
        if (masInstance_.stateMap_.find(remoteAddr) != masInstance_.stateMap_.end()) {
            masInstance_.stateMap_[remoteAddr] = BTConnectState::CONNECTED;
        }
        if (masInstance_.pimpl->obexSessionMap_.find(remoteAddr) == masInstance_.pimpl->obexSessionMap_.end()) {
            masInstance_.pimpl->obexSessionMap_[remoteAddr] = &session;
        }
        if (auto iter = masInstance_.nativeMap_.find(remoteAddr); iter != masInstance_.nativeMap_.end()) {
            masInstance_.nativeMap_[remoteAddr]->IsAccepted = false;
        }
    }
    if (masInstance_.mapObserver_) {
        masInstance_.mapObserver_->OnConnectionStateChanged(
            remoteAddr, masInstance_.masInstanceId_, BTConnectState::CONNECTED);
    }
}

void MapMseInstance::impl::ConnectObserver::OnIncomingConnect(ObexIncomingConnect &incomingConnect)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    RawAddress rawAddr = incomingConnect.GetRemoteAddress();
    auto addr = rawAddr.GetAddress();
    if (masInstance_.shutDownEnable_) {
        MSE_LOG_ERROR("Connection denied during service shutdown.");
        incomingConnect.RejectConnection();
        return;
    }
    {
        std::lock_guard<std::recursive_mutex> lock(masInstance_.instanceMapMutex_);
        if (masInstance_.nativeMap_.find(addr) == masInstance_.nativeMap_.end()) {
            masInstance_.nativeMap_[addr] = std::move(std::make_unique<NativeData>(masInstance_));
        }
        if (masInstance_.stateMap_.find(addr) == masInstance_.stateMap_.end()) {
            masInstance_.stateMap_[addr] = BTConnectState::DISCONNECTED;
        }
        if (masInstance_.stateMap_[addr] == BTConnectState::DISCONNECTING) {
            MSE_LOG_ERROR("Connection denied while disconnected.");
            incomingConnect.RejectConnection();
            return;
        }
    }
    if (masInstance_.mapObserver_) {
        masInstance_.mapObserver_->OnConnectionStateChanged(
            rawAddr, masInstance_.masInstanceId_, BTConnectState::CONNECTING);
    }
    PermissionHandle(incomingConnect);
}

void MapMseInstance::impl::ConnectObserver::PermissionHandle(ObexIncomingConnect &incomingConnect)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::lock_guard<std::recursive_mutex> lock(masInstance_.instanceMapMutex_);
    RawAddress rawAddr = incomingConnect.GetRemoteAddress();
    auto addr = rawAddr.GetAddress();
    masInstance_.stateMap_[addr] = BTConnectState::CONNECTING;
    BTPermissionType permission = AdapterManager::GetInstance()->GetMessagePermission(rawAddr.GetAddress());
    if (BTPermissionType::ACCESS_UNKNOWN == permission) {
        masInstance_.pimpl->obexIncomingMap_[addr] = &incomingConnect;
        masInstance_.nativeMap_[addr]->IncomingStartTimer(rawAddr);
        if (masInstance_.mapObserver_) {
            masInstance_.mapObserver_->OnIncomingConnect(rawAddr, masInstance_.masInstanceId_);
        }
    } else if (BTPermissionType::ACCESS_ALLOWED == permission) {
        masInstance_.nativeMap_[addr]->StopTimer();
        incomingConnect.AcceptConnection();
        masInstance_.nativeMap_[addr]->IsAccepted = true;
        if (masInstance_.mapObserver_) {
            masInstance_.mapObserver_->OnTransportConnected(rawAddr, masInstance_.masInstanceId_);
        }
    } else if (BTPermissionType::ACCESS_FORBIDDEN == permission) {
        masInstance_.nativeMap_[addr]->StopTimer();
        incomingConnect.RejectConnection();
        masInstance_.nativeMap_[addr]->IsAccepted = false;
        masInstance_.stateMap_[addr] = BTConnectState::DISCONNECTED;
        if (masInstance_.mapObserver_) {
            masInstance_.mapObserver_->OnRejectConnection(rawAddr, masInstance_.masInstanceId_);
        }
    }
}

void MapMseInstance::NativeData::IncomingStartTimer(const RawAddress &rawAddr)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    StartTimer(std::bind(&MapMseInstance::NativeData::IncomingTimeout, this, rawAddr), MAP_MSE_INCOMING_TIME_OUT);
}

void MapMseInstance::NativeData::IncomingTimeout(const RawAddress &rawAddr)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    masInstance_.dispatcher_.PostTask(std::bind(&MapMseInstance::NativeData::ProcessIncomingTimeout, this, rawAddr));
}

void MapMseInstance::NativeData::ProcessIncomingTimeout(const RawAddress &rawAddr)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string addr = rawAddr.GetAddress();
    if (masInstance_.pimpl->obexIncomingMap_.find(addr) != masInstance_.pimpl->obexIncomingMap_.end()) {
        masInstance_.pimpl->obexIncomingMap_[addr]->RejectConnection();
        IsAccepted = false;
        if (masInstance_.mapObserver_) {
            masInstance_.SetConnectState(addr, BTConnectState::DISCONNECTED);
            masInstance_.mapObserver_->OnIncomingTimeout(rawAddr, masInstance_.masInstanceId_);
        }
    }
    if (auto iter = masInstance_.pimpl->obexIncomingMap_.find(addr);
        iter != masInstance_.pimpl->obexIncomingMap_.end()) {
        masInstance_.pimpl->obexIncomingMap_.erase(iter);
    }
}

void MapMseInstance::impl::ConnectObserver::OnTransportDisconnected(const RawAddress &remoteAddr)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    auto addr = remoteAddr.GetAddress();
    {
        std::lock_guard<std::recursive_mutex> lock(masInstance_.instanceMapMutex_);
        if (auto iter = masInstance_.pimpl->obexSessionMap_.find(addr);
            iter != masInstance_.pimpl->obexSessionMap_.end()) {
            masInstance_.pimpl->obexSessionMap_.erase(iter);
        }
        if (auto iter = masInstance_.pimpl->obexIncomingMap_.find(addr);
            iter != masInstance_.pimpl->obexIncomingMap_.end()) {
            masInstance_.pimpl->obexIncomingMap_.erase(iter);
        }
        if (auto iter = masInstance_.nativeMap_.find(addr); iter != masInstance_.nativeMap_.end()) {
            masInstance_.nativeMap_[addr]->StopTimer();
            masInstance_.nativeMap_[addr]->IsAccepted = false;
        }
        if (masInstance_.stateMap_.find(addr) != masInstance_.stateMap_.end()) {
            masInstance_.stateMap_[addr] = BTConnectState::DISCONNECTED;
        }
    }
    if (masInstance_.mapObserver_) {
        masInstance_.mapObserver_->OnTransportDisconnected(remoteAddr, masInstance_.masInstanceId_);
    }
}

void MapMseInstance::impl::ConnectObserver::OnDisconnect(const RawAddress &remoteAddr)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    auto addr = remoteAddr.GetAddress();
    {
        std::lock_guard<std::recursive_mutex> lock(masInstance_.instanceMapMutex_);
        if (auto iter = masInstance_.pimpl->obexSessionMap_.find(addr);
            iter != masInstance_.pimpl->obexSessionMap_.end()) {
            masInstance_.pimpl->obexSessionMap_.erase(iter);
        }
        if (auto iter = masInstance_.pimpl->obexIncomingMap_.find(addr);
            iter != masInstance_.pimpl->obexIncomingMap_.end()) {
            masInstance_.pimpl->obexIncomingMap_.erase(iter);
        }
        if (auto iter = masInstance_.nativeMap_.find(addr); iter != masInstance_.nativeMap_.end()) {
            masInstance_.nativeMap_[addr]->StopTimer();
            masInstance_.nativeMap_[addr]->IsAccepted = false;
        }
        if (masInstance_.stateMap_.find(addr) != masInstance_.stateMap_.end()) {
            masInstance_.stateMap_[addr] = BTConnectState::DISCONNECTED;
        }
    }
    if (masInstance_.pimpl->content_) {
        masInstance_.pimpl->content_->SetNotificationRegistration(remoteAddr, (int)NotificationStatus::OFF);
    }
}

void MapMseInstance::impl::ConnectObserver::OnBusy(const RawAddress &remoteAddr, bool isBusy)
{
    std::lock_guard<std::recursive_mutex> lock(masInstance_.instanceMapMutex_);
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    std::string address = remoteAddr.GetAddress();
    if (masInstance_.nativeMap_[address]->IsBusy == isBusy) {
        return;
    }
    masInstance_.nativeMap_[address]->IsBusy = isBusy;
    if (masInstance_.mapObserver_) {
        masInstance_.mapObserver_->OnBusy(remoteAddr, isBusy);
    }
}

void MapMseInstance::GrantPermission(const RawAddress &remoteAddr, bool allow, bool save)
{
    bool notify = false;
    {
        std::lock_guard<std::recursive_mutex> lock(instanceMapMutex_);
        MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

        std::string address = remoteAddr.GetAddress();
        if (pimpl->obexIncomingMap_.find(address) == pimpl->obexIncomingMap_.end()) {
            return;
        }
        if (pimpl->obexIncomingMap_[address]->GetRemoteAddress().GetAddress() == remoteAddr.GetAddress()) {
            if (allow) {
                nativeMap_[address]->StopTimer();
                pimpl->obexIncomingMap_[address]->AcceptConnection();
                nativeMap_[address]->IsAccepted = true;
                if (mapObserver_) {
                    mapObserver_->OnTransportConnected(remoteAddr, masInstanceId_);
                }
            } else {
                nativeMap_[address]->StopTimer();
                pimpl->obexIncomingMap_[address]->RejectConnection();
                nativeMap_[address]->IsAccepted = false;
                stateMap_[address] = BTConnectState::DISCONNECTED;
                notify = true;
            }
            pimpl->obexIncomingMap_.erase(address);
        }
    }
    if (notify) {
        if (mapObserver_) {
            mapObserver_->OnConnectionStateChanged(remoteAddr, masInstanceId_, BTConnectState::DISCONNECTED);
        }
    }
    BTPermissionType permission = AdapterManager::GetInstance()->GetMessagePermission(remoteAddr.GetAddress());
    if (BTPermissionType::ACCESS_UNKNOWN == permission && save) {
        if (allow) {
            AdapterManager::GetInstance()->SetMessagePermission(
                remoteAddr.GetAddress(), BTPermissionType::ACCESS_ALLOWED);
        } else {
            AdapterManager::GetInstance()->SetMessagePermission(
                remoteAddr.GetAddress(), BTPermissionType::ACCESS_FORBIDDEN);
        }
    }
}

void MapMseInstance::DestroyIncomingConnect(std::string address)
{
    std::lock_guard<std::recursive_mutex> lock(instanceMapMutex_);
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (pimpl->obexIncomingMap_.find(address) != pimpl->obexIncomingMap_.end()) {
        if (pimpl->obexIncomingMap_[address]->GetRemoteAddress().GetAddress() == address) {
            nativeMap_[address]->StopTimer();
            pimpl->obexIncomingMap_[address]->RejectConnection();
            nativeMap_[address]->IsAccepted = false;
            pimpl->obexIncomingMap_.erase(address);
        }
    }
}

uint64_t MapMseInstance::GetDatabaseIdentifier(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    return databaseIdentifier_;
}

void MapMseInstance::UpdateDatabaseIdentifier(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    databaseIdentifier_++;
}

void MapMseInstance::UpdateFolderVersionCounter(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    folderVersionCounter_++;
}

uint64_t MapMseInstance::GetFolderVersionCounter(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    return folderVersionCounter_;
}

uint64_t MapMseInstance::GetConversationVersionCounter(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    return conversationVersionCounter_;
}

void MapMseInstance::UpdateConversationVersionCounter(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    conversationVersionCounter_++;
}

uint8_t MapMseInstance::GetMasId(void)
{
    MSE_LOG_INFO("%{public}s Enter Mas id : %u", __PRETTY_FUNCTION__, masInstanceId_);

    return masInstanceId_;
}

bool MapMseInstance::IsConnected(const std::string &addr)
{
    std::lock_guard<std::recursive_mutex> lock(instanceMapMutex_);
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    bool result = false;
    if (addr == "") {
        if (pimpl->obexSessionMap_.size() > 0)
            result = true;
    } else {
        if (pimpl->obexSessionMap_.find(addr) != pimpl->obexSessionMap_.end())
            result = true;
    }
    MSE_LOG_INFO("Is Connected result : %{public}d", result);
    return result;
}

int MapMseInstance::GetConnectState(void)
{
    std::lock_guard<std::recursive_mutex> lock(instanceMapMutex_);
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    uint8_t result = PROFILE_STATE_DISCONNECTED;
    for (auto &state : stateMap_) {
        switch (state.second) {
            case BTConnectState::CONNECTING:
                result |= PROFILE_STATE_CONNECTING;
                break;
            case BTConnectState::CONNECTED:
                result |= PROFILE_STATE_CONNECTED;
                break;
            case BTConnectState::DISCONNECTING:
                result |= PROFILE_STATE_DISCONNECTING;
                break;
            case BTConnectState::DISCONNECTED:
                result |= PROFILE_STATE_DISCONNECTED;
                break;
            default:
                break;
        }
    }
    MSE_LOG_INFO("Get Connect State : %{public}d", result);
    return result;
}

void MapMseInstance::GetConnectDevices(std::list<RawAddress> &devices)
{
    std::lock_guard<std::recursive_mutex> lock(instanceMapMutex_);
    for (auto &address : pimpl->obexSessionMap_) {
        bool has = true;
        for (auto &device : devices) {
            if (device.GetAddress() == address.first) {
                has = false;
                break;
            }
        }
        if (auto state = stateMap_.find(address.first); state != stateMap_.end()) {
            if (state->second != BTConnectState::CONNECTED) {
                has = false;
            }
        }
        if (has) {
            devices.push_back(RawAddress(address.first));
        }
    }
}

void MapMseInstance::GetDeviceState(const std::string &addr, int &result)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::lock_guard<std::recursive_mutex> lock(instanceMapMutex_);
    if (auto state = stateMap_.find(addr); state != stateMap_.end()) {
        switch (state->second) {
            case BTConnectState::CONNECTING:
                result = PROFILE_STATE_CONNECTING;
                break;
            case BTConnectState::CONNECTED:
                result |= PROFILE_STATE_CONNECTED;
                break;
            case BTConnectState::DISCONNECTING:
                result = PROFILE_STATE_DISCONNECTING;
                break;
            case BTConnectState::DISCONNECTED:
                result |= PROFILE_STATE_DISCONNECTED;
                break;
            default:
                break;
        }
    }
}

void MapMseInstance::GetDevicesByStates(std::vector<RawAddress> &devices, const std::vector<int> &states)
{
    std::lock_guard<std::recursive_mutex> lock(instanceMapMutex_);
    for (auto &stateMap : stateMap_) {
        for (auto &state : states) {
            if (stateMap.second == BTConnectState(state)) {
                GetDevicesNative(devices, stateMap.first);
            }
        }
    }
}

void MapMseInstance::GetDevicesNative(std::vector<RawAddress> &devices, const std::string &address)
{
    bool has = true;
    for (auto &device : devices) {
        if (device.GetAddress() == address) {
            has = false;
            break;
        }
    }
    if (has) {
        devices.push_back(RawAddress(address));
    }
}
void MapMseInstance::DisConnect(const std::string &addr)
{
    {
        std::lock_guard<std::recursive_mutex> lock(instanceMapMutex_);
        if (addr == "") {
            for (auto iter = pimpl->obexSessionMap_.begin(); iter != pimpl->obexSessionMap_.end(); iter++) {
                if (BTConnectState::DISCONNECTING != stateMap_[addr]) {
                    iter->second->Disconnect();
                }
            }
        }
        if (pimpl->obexSessionMap_.find(addr) == pimpl->obexSessionMap_.end()) {
            return;
        }
    }
    if (mapObserver_) {
        mapObserver_->OnConnectionStateChanged(RawAddress(addr), masInstanceId_, BTConnectState::DISCONNECTING);
    }
    {
        std::lock_guard<std::recursive_mutex> lock(instanceMapMutex_);
        if (RET_NO_ERROR == pimpl->obexSessionMap_[addr]->Disconnect()) {
            return;
        }
        MSE_LOG_INFO("Call Obex Session Disconnect error.");
        pimpl->obexSessionMap_.erase(addr);
    }
    SetConnectState(addr, BTConnectState::DISCONNECTED);
    if (mapObserver_) {
        mapObserver_->OnConnectionStateChanged(RawAddress(addr), masInstanceId_, BTConnectState::DISCONNECTED);
    }
}

bool MapMseInstance::AcceptedNoCallBack(bool disable)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    shutDownEnable_ = disable;
    bool ret = false;
    {
        std::lock_guard<std::recursive_mutex> lock(instanceMapMutex_);
        for (auto &iter : nativeMap_) {
            if (iter.second->IsAccepted) {
                ret = true;
                break;
            }
        }
    }
    return ret;
}

bool MapMseInstance::IsBusy(const std::string &addr)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (nativeMap_.find(addr) == nativeMap_.end()) {
        return false;
    }
    return nativeMap_[addr]->IsBusy;
}

int MapMseInstance::Enable(MapMseMnscli &mnsClient)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    shutDownEnable_ = false;
    pimpl->content_ = std::make_unique<MapMseResource>(mnsClient, dispatcher_, *this);
    pimpl->content_->SetRemoteFeatureMask(supportedFeatures_);
    pimpl->mseServer_ = std::make_unique<MapMseServer>(*pimpl->content_,
        dispatcher_,
        *pimpl->connectObserver_,
        *this,
        rfcommNo_,
        goepL2capPsm_,
        serviceName_,
        accountItem_,
        smsSupport_);
    pimpl->mseServer_->SetRemoteFeatureMask(supportedFeatures_);
    return pimpl->mseServer_->Enable();
}

void MapMseInstance::Disable(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    shutDownEnable_ = true;
    if (pimpl->mseServer_) {
        pimpl->mseServer_->Disable();
    }
}

int MapMseInstance::AddServiceClassIdList(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    BtUuid btUuid = {BT_UUID_16, {MAS_SERVICE_CLASS_UUID}};
    if (SDP_AddServiceClassIdList(sdpHandle_, &btUuid, MAS_SERVICE_CLASS_ID_NUMBER)) {
        MSE_LOG_ERROR("Call SDP_AddServiceClassIdList Error");
        return Result::FAIL;
    }
    return Result::SUCCEED;
}

int MapMseInstance::AddProtocolDescriptorList(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    rfcommNo_ = RFCOMM_AssignServerNum();
    SdpProtocolDescriptor descriptor[PROTOCOL_DESCRIPTOR_NUMBER];
    descriptor[0].parameterNumber = 0;
    descriptor[0].protocolUuid.type = BT_UUID_16;
    descriptor[0].protocolUuid.uuid16 = UUID_PROTOCOL_L2CAP;
    descriptor[1].protocolUuid.type = BT_UUID_16;
    descriptor[1].protocolUuid.uuid16 = UUID_PROTOCOL_RFCOMM;
    descriptor[1].parameterNumber = PROTOCOL_DESCRIPTOR_PARAMETER_NUMBER;
    descriptor[1].parameter[0].type = SDP_TYPE_UINT_8;
    descriptor[1].parameter[0].value = rfcommNo_;
    descriptor[0x2].protocolUuid.type = BT_UUID_16;
    descriptor[0x2].protocolUuid.uuid16 = UUID_PROTOCOL_OBEX;
    descriptor[0x2].parameterNumber = 0;

    if (SDP_AddProtocolDescriptorList(sdpHandle_, descriptor, PROTOCOL_DESCRIPTOR_NUMBER)) {
        MSE_LOG_ERROR("Call SDP_AddProtocolDescriptorList Error");
        return Result::FAIL;
    }
    return Result::SUCCEED;
}

int MapMseInstance::AddServiceName(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    serviceName_ = "MAP MAS-";
    if (smsSupport_) {
        serviceName_.append(SMS_MMS_NAME);
        supportedMsgTypes_ =
            MAP_SUPPORTED_MESSAGE_TYPE_SMS_GSM | MAP_SUPPORTED_MESSAGE_TYPE_SMS_CDMA | MAP_SUPPORTED_MESSAGE_TYPE_MMS;
        if (accountItem_.msgType_ == MessageType::EMAIL) {
            serviceName_.append("_");
            serviceName_.append(EMAIL_NAME);
            supportedMsgTypes_ |= MAP_SUPPORTED_MESSAGE_TYPE_EMAIL;
        } else if (accountItem_.msgType_ == MessageType::IM) {
            serviceName_.append("_");
            serviceName_.append(IM_NAME);
            supportedMsgTypes_ |= MAP_SUPPORTED_MESSAGE_TYPE_IM;
        }
    }

    if (SDP_AddServiceName(sdpHandle_, SDP_ATTRIBUTE_PRIMARY_LANGUAGE_BASE, serviceName_.data(), serviceName_.size())) {
        MSE_LOG_ERROR("Call SDP_AddServiceName Error");
        return Result::FAIL;
    }
    return Result::SUCCEED;
}

void MapMseInstance::GetProfileVersion(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    std::string version = "map14";
    profileVersion_ = MAP_PROFILE_VERSION14;
    supportedFeatures_ = MAP_SUPPORTED_FEATURES_V14;
    if (AdapterConfig::GetInstance()->GetValue(SECTION_MAP_MSE_SERVICE, PROPERTY_MAP_VERSION, version)) {
        if (version == "map12") {
            profileVersion_ = MAP_PROFILE_VERSION12;
            supportedFeatures_ = MAP_SUPPORTED_FEATURES_V12;
        } else if (version == "map13") {
            profileVersion_ = MAP_PROFILE_VERSION13;
            supportedFeatures_ = MAP_SUPPORTED_FEATURES_V13;
        }
    }
}

int MapMseInstance::AddBluetoothProfileDescriptorList(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (SdpProfileDescriptor profileDescriptor = {
        .profileUuid = {BT_UUID_16, {MAP_PROFILE_DESCRIPTOR_UUID}},
        .versionNumber = profileVersion_
        };
        SDP_AddBluetoothProfileDescriptorList(sdpHandle_, &profileDescriptor, MAP_PROFILE_DESCRIPTOR_NUMBER)) {
        MSE_LOG_ERROR("Call SDP_AddBluetoothProfileDescriptorList Error");
        return Result::FAIL;
    }
    return Result::SUCCEED;
}

int MapMseInstance::AddAttribute(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
#ifdef MAP_MSE_L2CAP_ENABLE
    if (SDP_AddAttribute(
        sdpHandle_, GOEP_L2CAP_PSM_ATTRIBUTE_ID, SdpDataType::SDP_TYPE_UINT_16, (uint8_t *)&goepL2capPsm_, 0x2)) {
        MSE_LOG_ERROR("Call SDP_AddAttribute Error");
        return Result::FAIL;
    }
#endif
    if (SDP_AddAttribute(sdpHandle_, MAS_INSTANCE_ATTRIBUTE_ID, SdpDataType::SDP_TYPE_UINT_8, &masInstanceId_, 0x1)) {
        MSE_LOG_ERROR("Call SDP_AddAttribute Error");
        return Result::FAIL;
    }
    if (SDP_AddAttribute(sdpHandle_,
        MAP_SUPPORTED_MESSAGE_TYPE_ATTRIBUTE_ID,
        SdpDataType::SDP_TYPE_UINT_8,
        &supportedMsgTypes_,
        0x1)) {
        MSE_LOG_ERROR("Call SDP_AddAttribute Error");
        return Result::FAIL;
    }

    if (SDP_AddAttribute(sdpHandle_,
        MAP_SUPPORTED_FEATURES_ATTRIBUTE_ID,
        SdpDataType::SDP_TYPE_UINT_32,
        (uint8_t *)&supportedFeatures_,
        0x4)) {
        MSE_LOG_ERROR("Call SDP_AddAttribute Error");
        return Result::FAIL;
    }
    return Result::SUCCEED;
}

int MapMseInstance::AddBrowseGroupList(void)
{
    if (BtUuid btUuid = {BT_UUID_16, {SDP_PUBLIC_BROWSE_GROUP_ROOT_UUID}};
        SDP_AddBrowseGroupList(sdpHandle_, &btUuid, MAS_BROWSE_GROUP_UUID_NUMBER)) {
        MSE_LOG_ERROR("Call SDP_AddBrowseGroupList Error");
        return Result::FAIL;
    }
    return Result::SUCCEED;
}

int MapMseInstance::RegisterServiceRecord(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (SDP_RegisterServiceRecord(sdpHandle_)) {
        MSE_LOG_ERROR("Call SDP_RegisterServiceRecord Error");
        return Result::FAIL;
    }
    BTM_AddLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, goepL2capPsm_);
    BTM_AddLocalRfcommScnForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, rfcommNo_);

    return Result::SUCCEED;
}

int MapMseInstance::CreateMasSdpRecord(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    int retVal = Result::SUCCEED;
    sdpHandle_ = SDP_CreateServiceRecord();
    GetProfileVersion();
    retVal |= AddServiceClassIdList();
    retVal |= AddProtocolDescriptorList();
    retVal |= AddServiceName();
    retVal |= AddBluetoothProfileDescriptorList();
    retVal |= AddAttribute();
    retVal |= AddBrowseGroupList();
    retVal |= RegisterServiceRecord();
    folderVersionCounter_ = 1;
    databaseIdentifier_ = 1;
    conversationVersionCounter_ = 1;
    return retVal;
}

void MapMseInstance::DestroyMasSdpRecord(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    if (SDP_DeregisterServiceRecord(sdpHandle_)) {
        BTM_RemoveLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, goepL2capPsm_);
        BTM_RemoveLocalRfcommScnChannelForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, rfcommNo_);
        MSE_LOG_ERROR("Call SDP_DeregisterServiceRecord Error");
        return;
    }
    BTM_RemoveLocalL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, goepL2capPsm_);
    BTM_RemoveLocalRfcommScnChannelForLogging(BTM_HCI_LOG_FILTER_MODULE_MAP, rfcommNo_);
    if (RFCOMM_FreeServerNum(rfcommNo_)) {
        MSE_LOG_ERROR("Call RFCOMM_FreeServerNum Error");
        return;
    }
    if (SDP_DestroyServiceRecord(sdpHandle_)) {
        MSE_LOG_ERROR("Call SDP_DestroyServiceRecord Error");
        return;
    }
}

int MapMseInstance::RegistGap(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

#ifdef MAP_MSE_L2CAP_ENABLE
    // register for l2cap
    GapSecChannel secChannel = {.l2capPsm = goepL2capPsm_};
    GapServiceSecurityInfo serviceInfo = {
        .direction = INCOMING,
        .serviceId = GAP_Service(MAP_SERVER_ID_START + masInstanceId_),
        .protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_L2CAP,
        .channelId = secChannel
    };
    if (GAPIF_RegisterServiceSecurity(nullptr,
        &serviceInfo,
        GAP_SEC_IN_AUTHENTICATION | GAP_SEC_IN_ENCRYPTION | GAP_SEC_OUT_AUTHENTICATION | GAP_SEC_OUT_ENCRYPTION)) {
        MSE_LOG_INFO("Call GAP_RegisterServiceSecurity Error");
        return Result::FAIL;
    }
#else
    GapSecChannel secChannel = {.rfcommChannel = rfcommNo_};
    GapServiceSecurityInfo serviceInfo = {
        .direction = INCOMING,
        .serviceId = GAP_Service(MAP_SERVER_ID_START + masInstanceId_),
        .protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_RFCOMM,
        .channelId = secChannel
    };
#endif
    // register for rfcomm
    MSE_LOG_INFO("%{public}s rfcomm no %u", __PRETTY_FUNCTION__, rfcommNo_);
    secChannel.rfcommChannel = rfcommNo_;
    serviceInfo.channelId = secChannel;
    serviceInfo.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_RFCOMM;
    if (GAPIF_RegisterServiceSecurity(nullptr,
        &serviceInfo,
        GAP_SEC_IN_AUTHENTICATION | GAP_SEC_IN_ENCRYPTION | GAP_SEC_OUT_AUTHENTICATION | GAP_SEC_OUT_ENCRYPTION)) {
        MSE_LOG_INFO("Call GAP_RegisterServiceSecurity Error");
        return Result::FAIL;
    }
    return Result::SUCCEED;
}

void MapMseInstance::UnRegistGap(void)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
#ifdef MAP_MSE_L2CAP_ENABLE
    // deregister for l2cap
    GapSecChannel secChannel = {.l2capPsm = goepL2capPsm_};
    GapServiceSecurityInfo serviceInfo = {
        .direction = INCOMING,
        .serviceId = GAP_Service(MAP_SERVER_ID_START + masInstanceId_),
        .protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_L2CAP,
        .channelId = secChannel
    };
    if (GAPIF_DeregisterServiceSecurity(nullptr, &serviceInfo)) {
        MSE_LOG_INFO("Call GAP_UnregisterServiceSecurity Error");
    }
#else
    GapSecChannel secChannel = {.rfcommChannel = rfcommNo_};
    GapServiceSecurityInfo serviceInfo = {
        .direction = INCOMING,
        .serviceId = GAP_Service(MAP_SERVER_ID_START + masInstanceId_),
        .protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_RFCOMM,
        .channelId = secChannel
    };
#endif
    // deregister for rfcomm
    secChannel.rfcommChannel = rfcommNo_;
    serviceInfo.channelId = secChannel;
    serviceInfo.protocolId = GAP_SecMultiplexingProtocol::SEC_PROTOCOL_RFCOMM;
    if (GAPIF_DeregisterServiceSecurity(nullptr, &serviceInfo)) {
        MSE_LOG_INFO("Call GAP_UnregisterServiceSecurity Error");
    }
}

void MapMseInstance::SetConnectState(std::string address, BTConnectState state)
{
    std::lock_guard<std::recursive_mutex> lock(instanceMapMutex_);
    if (stateMap_.find(address) != stateMap_.end()) {
        stateMap_[address] = state;
    }
}

std::string MapAccountItem::GetUciFullName(void)
{
    std::string fullUci = "";
    if (uci_ == "" || uciPrefix_ == "")
        return "";
    fullUci.append(uciPrefix_);
    fullUci.append(":");
    fullUci.append(uci_);
    return fullUci;
}
}  // namespace bluetooth
}  // namespace OHOS