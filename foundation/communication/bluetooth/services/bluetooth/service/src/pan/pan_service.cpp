/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "pan_service.h"
#include "sdp.h"

namespace OHOS {
namespace bluetooth {
PanService::PanService() : utility::Context(PROFILE_NAME_PAN, "1.0")
{
    LOG_DEBUG("[PAN Service]%{public}s:%{public}s Create", PROFILE_NAME_PAN.c_str(), Name().c_str());
}

PanService::~PanService()
{
    LOG_DEBUG("[PAN Service]%{public}s:%{public}s Destroy", PROFILE_NAME_PAN.c_str(), Name().c_str());
}

utility::Context *PanService::GetContext()
{
    return this;
}

PanService *PanService::GetService()
{
    auto servManager = IProfileManager::GetInstance();
    return static_cast<PanService *>(servManager->GetProfileService(PROFILE_NAME_PAN));
}

void PanService::RegisterObserver(IPanObserver &panObserver)
{
    LOG_DEBUG("[PAN Service]%{public}s Enter", __PRETTY_FUNCTION__);

    panObservers_.Register(panObserver);
}

void PanService::DeregisterObserver(IPanObserver &panObserver)
{
    LOG_DEBUG("[PAN Service]%{public}s Enter", __PRETTY_FUNCTION__);

    panObservers_.Deregister(panObserver);
}

void PanService::NotifyStateChanged(const RawAddress &device, int state)
{
    LOG_DEBUG("[PAN Service]%{public}s Enter", __PRETTY_FUNCTION__);
    int newState = stateMap_.at(state);

    panObservers_.ForEach([device, newState](IPanObserver &observer) {
        observer.OnConnectionStateChanged(device, newState);
    });
}

void PanService::Enable(void)
{
    LOG_DEBUG("[PAN Service]%{public}s Enter", __PRETTY_FUNCTION__);

    PanMessage event(PAN_SERVICE_STARTUP_EVT);
    PostEvent(event);
}

void PanService::Disable(void)
{
    LOG_DEBUG("[PAN Service]%{public}s Enter", __PRETTY_FUNCTION__);

    PanMessage event(PAN_SERVICE_SHUTDOWN_EVT);
    PostEvent(event);
}

void PanService::StartUp()
{
    LOG_DEBUG("[PAN Service]%{public}s():==========<start>==========", __FUNCTION__);
    if (isStarted_) {
        GetContext()->OnEnable(PROFILE_NAME_PAN, true);
        LOG_WARN("[PAN Service]%{public}s():PanService has already been started before.", __FUNCTION__);
        return;
    }

    maxConnectionsNum_ = GetMaxConnectionsDeviceNum();

    int resultSdp = BT_NO_ERROR;
    int resultBnep = BT_NO_ERROR;
    panSdp_ = std::make_unique<PanSdp>();
    resultSdp = panSdp_->Register();
    resultBnep = PanBnep::Startup();
    if (resultSdp == BT_NO_ERROR && resultBnep == BT_NO_ERROR) {
        GetContext()->OnEnable(PROFILE_NAME_PAN, true);
        isStarted_ = true;
        LOG_DEBUG("[PAN Service]%{public}s():PanService started", __FUNCTION__);
    } else {
        GetContext()->OnEnable(PROFILE_NAME_PAN, false);
    }
}

void PanService::ShutDown()
{
    LOG_DEBUG("[PAN Service]%{public}s():==========<start>==========", __FUNCTION__);
    if (!isStarted_) {
        GetContext()->OnDisable(PROFILE_NAME_PAN, true);
        LOG_WARN("[PAN Service]%{public}s():PanService has already been shutdown before.", __FUNCTION__);
        return;
    }

    isShuttingDown_ = true;
    bool isDisconnected = false;
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if ((it->second != nullptr) && (it->second->GetDeviceStateInt() > PAN_STATE_DISCONNECTED)) {
            Disconnect(RawAddress(it->first));
            isDisconnected = true;
        }
    }

    if (!isDisconnected) {
        ShutDownDone(true);
    }
}

void PanService::ShutDownDone(bool isAllDisconnected)
{
    LOG_DEBUG("[PAN Service]%{public}s():==========<start>==========", __FUNCTION__);
    if (!isAllDisconnected) {
        for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
            if ((it->second != nullptr) && (it->second->GetDeviceStateInt() > PAN_STATE_DISCONNECTED)) {
                return;
            }
        }
    }

    stateMachines_.clear();
    panSdp_->Deregister();
    GetContext()->OnDisable(PROFILE_NAME_PAN, true);
    isStarted_ = false;
    LOG_DEBUG("[PAN Service]%{public}s():PanService shutdown", __FUNCTION__);
    isShuttingDown_ = false;
}

int PanService::Connect(const RawAddress &device)
{
    LOG_DEBUG("[PAN Service]%{public}s Enter", __PRETTY_FUNCTION__);
    // DO NOTHING AT THIS TIME
    return PAN_SUCCESS;
}

int PanService::Disconnect(const RawAddress &device)
{
    LOG_DEBUG("[PAN Service]%{public}s Enter", __PRETTY_FUNCTION__);

    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_DEBUG("[PAN Service]%{public}s():The state machine is not available!", __FUNCTION__);
        return PAN_FAILURE;
    }

    int slcState = it->second->GetDeviceStateInt();
    if ((slcState != PAN_STATE_CONNECTING) && (slcState < PAN_STATE_CONNECTED)) {
        LOG_DEBUG("[PAN Service]%{public}s():slcState:%{public}d", __FUNCTION__, slcState);
        return PAN_FAILURE;
    }

    PanMessage event(PAN_API_CLOSE_EVT);
    event.dev_ = address;
    PostEvent(event);
    return PAN_SUCCESS;
}

bool PanService::SetTethering(bool enable)
{
    LOG_DEBUG("[PAN Service]%{public}s Enter", __PRETTY_FUNCTION__);

    std::lock_guard<std::recursive_mutex> lk(mutex_);
    if (!enable && (isTetheringOn_ != enable)) {
        for (auto it = stateMachines_.begin(); it != stateMachines_.end(); it++) {
            if ((it->second != nullptr) && (it->second->GetDeviceStateInt() == PAN_STATE_CONNECTED)) {
                PanMessage event(PAN_API_CLOSE_EVT);
                event.dev_ = it->first;
                PostEvent(event);
            }
        }
    }
    isTetheringOn_ = enable;
    return true;
}

bool PanService::IsTetheringOn()
{
    return isTetheringOn_;
}

void PanService::OpenNetwork()
{
    if (panNetwork_ == nullptr) {
        panNetwork_ = std::make_unique<PanNetwork>();
    }
    panNetwork_->Open();
}

void PanService::CloseNetwork(std::string device)
{
    if (panNetwork_ == nullptr) {
        LOG_ERROR("[PAN Service]%{public}s():panNetwork_ is null", __FUNCTION__);
        return;
    }
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); it++) {
        if ((it->second == nullptr) && (it->second->GetDeviceStateInt() == PAN_STATE_CONNECTED) &&
            (it->first != device)) {
            LOG_DEBUG("[PAN Service]%{public}s had other connected device", __PRETTY_FUNCTION__);
            return;
        }
    }
    panNetwork_->Close();
    panNetwork_ = nullptr;
}

int PanService::ReceiveRemoteBusy(bool isBusy)
{
    if (panNetwork_ == nullptr) {
        LOG_ERROR("[PAN Service]%{public}s():panNetwork_ is null", __FUNCTION__);
        return PAN_FAILURE;
    }
    panNetwork_->ReceiveRemoteBusy(isBusy);
    return PAN_SUCCESS;
}

void PanService::WriteNetworkData(std::string address, EthernetHeader head, uint8_t *data, int len)
{
    if (head.destAddr[0] & 0x01) {
        LOG_DEBUG("[PAN Service]%{public}s is broadcast,also forward to other device", __PRETTY_FUNCTION__);
        for (auto it = stateMachines_.begin(); it != stateMachines_.end(); it++) {
            if ((it->second != nullptr) && (it->second->GetDeviceStateInt() == PAN_STATE_CONNECTED)
                && (it->first != address)) {
                PanSendData(it->first, head, data, len);
            }
        }
    } else {
        uint8_t bluetoothDestAddr[BT_ADDRESS_LENGTH];
        ReverseAddress(head.destAddr, bluetoothDestAddr);
        std::string destAddr = RawAddress::ConvertToString(bluetoothDestAddr).GetAddress();
        auto it = stateMachines_.find(destAddr);
        if ((it != stateMachines_.end()) && (it->second == nullptr) &&
            (it->second->GetDeviceStateInt() == PAN_STATE_CONNECTED)) {
            LOG_DEBUG("[PAN Service]%{public}s():destination address is other connected device!", __FUNCTION__);
            PanSendData(it->first, head, data, len);
            return;
        }
    }
    if (panNetwork_ == nullptr) {
        LOG_ERROR("[PAN Service]%{public}s():panNetwork_ is null", __FUNCTION__);
        return;
    }
    panNetwork_->WriteData(head, data, len);
}

int PanService::PanSendData(EthernetHeader head, uint8_t *data, int len)
{
    int isBroadcast = head.destAddr[0] & 1;
    uint8_t bluetoothDestAddr[BT_ADDRESS_LENGTH];
    uint8_t bluetoothSrcAddr[BT_ADDRESS_LENGTH];
    ReverseAddress(head.destAddr, bluetoothDestAddr);
    ReverseAddress(head.srcAddr, bluetoothSrcAddr);
    std::string destAddr = RawAddress::ConvertToString(bluetoothDestAddr).GetAddress();
    std::string srcAddr = RawAddress::ConvertToString(bluetoothSrcAddr).GetAddress();

    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); it++) {
        if ((it->second->GetDeviceStateInt() == PAN_STATE_CONNECTED) &&
            (isBroadcast || (destAddr == it->first) || (srcAddr == it->first))) {
            PanSendData(it->first, head, data, len);
        }
    }
    return PAN_SUCCESS;
}

void PanService::ReverseAddress(uint8_t *oldAddr, uint8_t *newAddr)
{
    for (int i = 0; i < BT_ADDRESS_LENGTH; i++) {
        newAddr[i] = oldAddr[BT_ADDRESS_LENGTH - i - 1];
    }
}

void PanService::PanSendData(std::string address, EthernetHeader head, uint8_t *data, int len)
{
    PanMessage event(PAN_API_WRITE_DATA_EVT);
    event.dev_ = address;
    event.ethernetHeader_ = head;
    if ((len > 0) && (data != nullptr)) {
        event.dataLength_ = len;
        std::unique_ptr<uint8_t[]> buff = std::make_unique<uint8_t[]>(event.dataLength_);
        if (memcpy_s(buff.get(), len, data, len) != EOK) {
            LOG_ERROR("[PAN Service]%{public}s(): memcpy error", __FUNCTION__);
            return;
        }
        event.data_ = std::make_shared<std::unique_ptr<uint8_t[]>>(std::move(buff));
    }
    PostEvent(event);
}

bool PanService::IsConnected(const std::string &address) const
{
    LOG_DEBUG("[PAN Service]%{public}s Enter", __PRETTY_FUNCTION__);

    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_ERROR("[PAN Service]%{public}s():Invalid Device address:%{public}s", __FUNCTION__, address.c_str());
        return false;
    }
    if (it->second->GetDeviceStateInt() < PAN_STATE_CONNECTED) {
        LOG_DEBUG("[PAN Service]%{public}s():It's not connected!", __FUNCTION__);
        return false;
    }
    return true;
}

std::vector<RawAddress> PanService::GetDevicesByStates(std::vector<int> states)
{
    LOG_DEBUG("[PAN Service]%{public}s Enter", __PRETTY_FUNCTION__);

    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::vector<RawAddress> devices;
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        RawAddress device(it->first);
        for (size_t i = 0; i < states.size(); i++) {
            if (GetDeviceState(device) == states[i]) {
                devices.push_back(device);
                break;
            }
        }
    }
    return devices;
}

int PanService::GetDeviceState(const RawAddress &device)
{
    LOG_DEBUG("[PAN Service]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_DEBUG("[PAN Service]%{public}s():The state machine is not available!", __FUNCTION__);
        return stateMap_.at(PAN_STATE_DISCONNECTED);
    }

    if (it->second->GetDeviceStateInt() >= PAN_STATE_CONNECTED) {
        return stateMap_.at(PAN_STATE_CONNECTED);
    } else {
        return stateMap_.at(it->second->GetDeviceStateInt());
    }
}

std::list<RawAddress> PanService::GetConnectDevices(void)
{
    LOG_DEBUG("[PAN Service]%{public}s Enter", __PRETTY_FUNCTION__);

    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::list<RawAddress> devList;
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if ((it->second != nullptr) && (it->second->GetDeviceStateInt() >= PAN_STATE_CONNECTED)) {
            devList.push_back(RawAddress(it->first));
        }
    }
    return devList;
}

int PanService::GetConnectState(void)
{
    LOG_DEBUG("[PAN Service]%{public}s Enter", __PRETTY_FUNCTION__);

    int result = 0;
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if (it->second == nullptr) {
            result |= PROFILE_STATE_DISCONNECTED;
        } else if (it->second->GetDeviceStateInt() >= PAN_STATE_CONNECTED) {
            result |= PROFILE_STATE_CONNECTED;
        } else if (it->second->GetDeviceStateInt() == PAN_STATE_CONNECTING) {
            result |= PROFILE_STATE_CONNECTING;
        } else if (it->second->GetDeviceStateInt() == PAN_STATE_DISCONNECTING) {
            result |= PROFILE_STATE_DISCONNECTING;
        } else if (it->second->GetDeviceStateInt() == PAN_STATE_DISCONNECTED) {
            result |= PROFILE_STATE_DISCONNECTED;
        }
    }
    return result;
}

int PanService::GetMaxConnectNum(void)
{
    LOG_DEBUG("[PAN Service]%{public}s Enter", __PRETTY_FUNCTION__);

    std::lock_guard<std::recursive_mutex> lk(mutex_);
    return maxConnectionsNum_;
}

int PanService::GetConnectionsDeviceNum() const
{
    int size = 0;
    for (auto iter = stateMachines_.begin(); iter != stateMachines_.end(); ++iter) {
        if (iter->second != nullptr) {
            auto connectionState = iter->second->GetDeviceStateInt();
            if ((connectionState == PAN_STATE_CONNECTING) || (connectionState >= PAN_STATE_CONNECTED)) {
                size++;
            }
        }
    }
    return size;
}

int PanService::GetMaxConnectionsDeviceNum() const
{
    int number = PAN_MAX_DEFAULT_CONNECTIONS_NUMR;
    if (!AdapterConfig::GetInstance()->GetValue(SECTION_PAN_SERVICE, PROPERTY_MAX_CONNECTED_DEVICES, number)) {
        LOG_DEBUG("[PAN Service]%{public}s():It's failed to get the max connection number", __FUNCTION__);
    }
    return number;
}

void PanService::RemoveStateMachine(const std::string &device)
{
    PanMessage event(PAN_REMOVE_STATE_MACHINE_EVT);
    event.dev_ = device;
    PostEvent(event);
}

void PanService::PostEvent(const PanMessage &event)
{
    GetDispatcher()->PostTask(std::bind(&PanService::ProcessEvent, this, event));
}

void PanService::ProcessEvent(const PanMessage &event)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = event.dev_;
    LOG_DEBUG("[PAN Service]%{public}s():address[%{public}s] event_no[%{public}d]",
        __FUNCTION__, address.c_str(), event.what_);
    switch (event.what_) {
        case PAN_SERVICE_STARTUP_EVT:
            StartUp();
            break;
        case PAN_SERVICE_SHUTDOWN_EVT:
            ShutDown();
            break;
        case PAN_INT_OPEN_EVT:
        case BNEP_L2CAP_CONNECT_REQ_EVT:
            ProcessConnectEvent(event);
            break;
        case PAN_REMOVE_STATE_MACHINE_EVT:
            ProcessRemoveStateMachine(event.dev_);
            break;
        default:
            ProcessDefaultEvent(event);
            break;
    }
}

void PanService::ProcessConnectEvent(const PanMessage &event)
{
    if (GetConnectionsDeviceNum() < maxConnectionsNum_) {
        auto it = stateMachines_.find(event.dev_);
        if (it != stateMachines_.end() && it->second != nullptr && it->second->IsRemoving()) {
            // peer device may send connect request before we remove statemachine for last connection.
            // so post this connect request, process it after we remove statemachine completely.
            PostEvent(event);
        } else if (it == stateMachines_.end() || it->second == nullptr) {
            stateMachines_[event.dev_] = std::make_unique<PanStateMachine>(event.dev_);
            stateMachines_[event.dev_]->Init();
            stateMachines_[event.dev_]->ProcessMessage(event);
            if (event.what_ == BNEP_L2CAP_CONNECT_REQ_EVT) {
                stateMachines_[event.dev_]->ProcessL2capConnectionEvent(event);
            }
        } else {
            it->second->ProcessMessage(event);
            if (event.what_ == BNEP_L2CAP_CONNECT_REQ_EVT) {
                it->second->ProcessL2capConnectionEvent(event);
            }
        }
    }
}

std::string PanService::PanFindDeviceByLcid(uint16_t lcid)
{
    std::string ret;

    std::lock_guard<std::recursive_mutex> lk(mutex_);
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if ((it->second != nullptr) && ((it->second->GetDeviceLcid() == lcid))) {
            return it->first;
        }
    }
    return ret;
}

std::string PanService::GetLocalAddress()
{
    IAdapterClassic *adapterClassic = (IAdapterClassic *)(IAdapterManager::GetInstance()->
        GetAdapter(ADAPTER_BREDR));
    if (adapterClassic != nullptr) {
        return adapterClassic->GetLocalAddress();
    }
    return std::string();
}

void PanService::ProcessRemoveStateMachine(const std::string &address)
{
    stateMachines_.insert_or_assign(address, nullptr);
    if (isShuttingDown_) {
        ShutDownDone(false);
    }
}

void PanService::ProcessDefaultEvent(const PanMessage &event) const
{
    auto it = stateMachines_.find(event.dev_);
    if ((it != stateMachines_.end()) && (it->second != nullptr)) {
        if ((event.what_ > BNEP_L2CAP_START_EVT) && (event.what_ < BNEP_L2CAP_END_EVT)) {
            it->second->ProcessL2capConnectionEvent(event);
        } else {
            it->second->ProcessMessage(event);
        }
    } else {
        LOG_ERROR("[PAN Service]%{public}s():invalid address[%{public}s]", __FUNCTION__, event.dev_.c_str());
    }
}
REGISTER_CLASS_CREATOR(PanService);
}  // namespace bluetooth
}  // namespace OHOS
