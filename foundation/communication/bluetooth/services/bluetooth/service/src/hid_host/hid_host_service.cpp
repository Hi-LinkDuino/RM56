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

#include "hid_host_service.h"

namespace OHOS {
namespace bluetooth {
HidHostService::HidHostService() : utility::Context(PROFILE_NAME_HID_HOST, "1.1.1")
{
    LOG_DEBUG("[HIDH Service]%{public}s:%{public}s Create", PROFILE_NAME_HID_HOST.c_str(), Name().c_str());
}

HidHostService::~HidHostService()
{
    LOG_DEBUG("[HIDH Service]%{public}s:%{public}s Destroy", PROFILE_NAME_MAP_MSE.c_str(), Name().c_str());
}

utility::Context *HidHostService::GetContext()
{
    return this;
}

HidHostService *HidHostService::GetService()
{
    auto servManager = IProfileManager::GetInstance();
    return static_cast<HidHostService *>(servManager->GetProfileService(PROFILE_NAME_HID_HOST));
}

void HidHostService::RegisterObserver(IHidHostObserver &hidHostObserver)
{
    LOG_DEBUG("[HIDH Service]%{public}s Enter", __FUNCTION__);

    hidHostObservers_.Register(hidHostObserver);
}

void HidHostService::DeregisterObserver(IHidHostObserver &hidHostObserver)
{
    LOG_DEBUG("[HIDH Service]%{public}s Enter", __FUNCTION__);

    hidHostObservers_.Deregister(hidHostObserver);
}

void HidHostService::NotifyStateChanged(const RawAddress &device, int state)
{
    // Reference "HID_HOST_STATE_CONNECTED"
    LOG_DEBUG("[HIDH Service]%{public}s, state:%{public}d", __FUNCTION__, state);
    int newState = stateMap_.at(state);

    hidHostObservers_.ForEach([device, newState](IHidHostObserver &observer) {
        observer.OnConnectionStateChanged(device, newState);
    });
}

void HidHostService::Enable(void)
{
    LOG_DEBUG("[HIDH Service]%{public}s Enter", __FUNCTION__);

    HidHostMessage event(HID_HOST_SERVICE_STARTUP_EVT);
    PostEvent(event);
}

void HidHostService::Disable(void)
{
    LOG_DEBUG("[HIDH Service]%{public}s Enter", __FUNCTION__);

    HidHostMessage event(HID_HOST_SERVICE_SHUTDOWN_EVT);
    PostEvent(event);
}

void HidHostService::StartUp()
{
    LOG_DEBUG("[HIDH Service]%{public}s():==========<start>==========", __FUNCTION__);
    if (isStarted_) {
        GetContext()->OnEnable(PROFILE_NAME_HID_HOST, true);
        LOG_WARN("[HIDH Service]%{public}s():HidHostService has already been started before.", __FUNCTION__);
        return;
    }

    maxConnectionsNum_ = GetMaxConnectionsDeviceNum();

    int ret = HidHostL2capConnection::Startup();

    GetContext()->OnEnable(PROFILE_NAME_HID_HOST, ret ? false : true);
    if (ret == 0) {
        isStarted_ = true;
        LOG_DEBUG("[HIDH Service]%{public}s():HidHostService started", __FUNCTION__);
    }
}

void HidHostService::ShutDown()
{
    LOG_DEBUG("[HIDH Service]%{public}s():==========<start>==========", __FUNCTION__);
    if (!isStarted_) {
        GetContext()->OnDisable(PROFILE_NAME_HID_HOST, true);
        LOG_WARN("[HIDH Service]%{public}s():HidHostService has already been shutdown before.", __FUNCTION__);
        return;
    }

    isShuttingDown_ = true;
    bool isDisconnected = false;
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if ((it->second != nullptr) && (it->second->GetDeviceStateInt() > HID_HOST_STATE_DISCONNECTED)) {
            Disconnect(RawAddress(it->first));
            isDisconnected = true;
        }
    }

    if (!isDisconnected) {
        ShutDownDone(true);
    }
}

void HidHostService::ShutDownDone(bool isAllDisconnected)
{
    LOG_DEBUG("[HIDH Service]%{public}s():==========<start>==========", __FUNCTION__);
    if (!isAllDisconnected) {
        for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
            if ((it->second != nullptr) && (it->second->GetDeviceStateInt() > HID_HOST_STATE_DISCONNECTED)) {
                return;
            }
        }
    }

    stateMachines_.clear();

    HidHostL2capConnection::Shutdown();
    GetContext()->OnDisable(PROFILE_NAME_HID_HOST, true);
    isStarted_ = false;
    LOG_DEBUG("[HIDH Service]%{public}s():HidHostService shutdown", __FUNCTION__);
    isShuttingDown_ = false;
}

int HidHostService::Connect(const RawAddress &device)
{
    LOG_DEBUG("[HIDH Service]%{public}s Enter", __FUNCTION__);

    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if ((it != stateMachines_.end()) && (it->second != nullptr)) {
        int slcState = it->second->GetDeviceStateInt();
        if ((slcState >= HID_HOST_STATE_CONNECTED) || (slcState == HID_HOST_STATE_CONNECTING)) {
            return HID_HOST_FAILURE;
        }
    }

    if (GetConnectionsDeviceNum() >= maxConnectionsNum_) {
        LOG_INFO("[HIDH Service]%{public}s():Max connection number has reached!", __FUNCTION__);
        return HID_HOST_FAILURE;
    }

    HidHostMessage event(HID_HOST_API_OPEN_EVT);
    event.dev_ = address;
    PostEvent(event);
    return HID_HOST_SUCCESS;
}

int HidHostService::Disconnect(const RawAddress &device)
{
    LOG_DEBUG("[HIDH Service]%{public}s Enter", __FUNCTION__);

    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_DEBUG("[HIDH Service]%{public}s():The state machine is not available!", __FUNCTION__);
        return HID_HOST_FAILURE;
    }

    int slcState = it->second->GetDeviceStateInt();
    if ((slcState != HID_HOST_STATE_CONNECTING) && (slcState < HID_HOST_STATE_CONNECTED)) {
        LOG_DEBUG("[HIDH Service]%{public}s():slcState:%{public}d", __FUNCTION__, slcState);
        return HID_HOST_FAILURE;
    }

    HidHostMessage event(HID_HOST_API_CLOSE_EVT);
    event.dev_ = address;
    PostEvent(event);
    return HID_HOST_SUCCESS;
}

int HidHostService::HidHostVCUnplug(std::string device, uint8_t id, uint16_t size, uint8_t type)
{
    HidHostMessage event(HID_HOST_API_WRITE_DEV_EVT);
    event.dev_ = device;
    event.sendData_.type = HID_HOST_DATA_TYPE_CONTROL;
    event.sendData_.param = type;
    event.sendData_.data = size;
    event.sendData_.reportId = id;
    PostEvent(event);
    return HID_HOST_SUCCESS;
}

int HidHostService::HidHostSendData(std::string device, uint8_t id, uint16_t size, uint8_t type)
{
    HidHostMessage event(HID_HOST_API_WRITE_DEV_EVT);
    event.dev_ = device;
    event.sendData_.type = HID_HOST_DATA_TYPE_DATA;
    event.sendData_.param = type;
    event.sendData_.data = size;
    event.sendData_.reportId = id;
    PostEvent(event);
    return HID_HOST_SUCCESS;
}

int HidHostService::HidHostSetReport(std::string device, uint8_t type, uint16_t size, const uint8_t* report)
{
    HidHostMessage event(HID_HOST_API_WRITE_DEV_EVT);
    event.dev_ = device;
    event.sendData_.type = HID_HOST_DATA_TYPE_SET_REPORT;
    event.sendData_.param = type;
    event.sendData_.data = 0;
    event.sendData_.reportId = 0;
    if ((size > 0) && (report != nullptr)) {
        event.dataLength_ = static_cast<int>(size);
        event.data_ = std::make_unique<uint8_t[]>(size);
        if (memcpy_s(event.data_.get(), size, report, size) != EOK) {
            LOG_ERROR("[HIDH Service]%{public}s():memcpy error", __FUNCTION__);
            return HID_HOST_FAILURE;
        }
    }
    PostEvent(event);
    return HID_HOST_SUCCESS;
}

int HidHostService::HidHostGetReport(std::string device, uint8_t id, uint16_t size, uint8_t type)
{
    HidHostMessage event(HID_HOST_API_WRITE_DEV_EVT);
    event.dev_ = device;
    event.sendData_.type = HID_HOST_DATA_TYPE_GET_REPORT;
    event.sendData_.param = type;
    event.sendData_.data = size;
    event.sendData_.reportId = id;
    PostEvent(event);
    return HID_HOST_SUCCESS;
}

bool HidHostService::IsConnected(const std::string &address) const
{
    LOG_INFO("[HIDH Service] IsConnected start");

    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_ERROR("[HIDH Service] IsConnected:Invalid Device address:%{public}s", address.c_str());
        return false;
    }
    if (it->second->GetDeviceStateInt() < HID_HOST_STATE_CONNECTED) {
        LOG_INFO("[HIDH Service] IsConnected:false");
        return false;
    }
    return true;
}

std::vector<RawAddress> HidHostService::GetDevicesByStates(std::vector<int> states)
{
    LOG_INFO("[HIDH Service] GetDevicesByStates Enter");
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

int HidHostService::GetDeviceState(const RawAddress &device)
{
    LOG_INFO("[HIDH Service] GetDeviceState start");
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_INFO("[HID HOST]GetDeviceState:The state machine is not available!");
        return stateMap_.at(HID_HOST_STATE_DISCONNECTED);
    }

    int state = it->second->GetDeviceStateInt();
    LOG_INFO("[HIDH Service] GetDeviceState, state:%{public}d", state);
    if (state >= HID_HOST_STATE_CONNECTED) {
        return stateMap_.at(HID_HOST_STATE_CONNECTED);
    } else {
        return stateMap_.at(state);
    }
}

std::list<RawAddress> HidHostService::GetConnectDevices(void)
{
    LOG_INFO("[HIDH Service] GetConnectDevices Enter");
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::list<RawAddress> devList;
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if ((it->second != nullptr) && (it->second->GetDeviceStateInt() >= HID_HOST_STATE_CONNECTED)) {
            devList.push_back(RawAddress(it->first));
        }
    }
    return devList;
}

int HidHostService::GetConnectState(void)
{
    LOG_INFO("[HIDH Service] GetConnectState Enter");
    int result = 0;
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if (it->second == nullptr) {
            result |= PROFILE_STATE_DISCONNECTED;
        } else if (it->second->GetDeviceStateInt() >= HID_HOST_STATE_CONNECTED) {
            result |= PROFILE_STATE_CONNECTED;
        } else if (it->second->GetDeviceStateInt() == HID_HOST_STATE_CONNECTING) {
            result |= PROFILE_STATE_CONNECTING;
        } else if (it->second->GetDeviceStateInt() == HID_HOST_STATE_DISCONNECTING) {
            result |= PROFILE_STATE_DISCONNECTING;
        } else if (it->second->GetDeviceStateInt() == HID_HOST_STATE_DISCONNECTED) {
            result |= PROFILE_STATE_DISCONNECTED;
        }
    }
    LOG_INFO("[HIDH Service] GetConnectState result:%{public}d", result);
    return result;
}

int HidHostService::GetMaxConnectNum(void)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    LOG_INFO("[HIDH Service] GetMaxConnectNum:%{public}d", maxConnectionsNum_);
    return maxConnectionsNum_;
}

int HidHostService::GetConnectionsDeviceNum() const
{
    int size = 0;
    for (auto iter = stateMachines_.begin(); iter != stateMachines_.end(); ++iter) {
        if (iter->second != nullptr) {
            auto connectionState = iter->second->GetDeviceStateInt();
            if ((connectionState == HID_HOST_STATE_CONNECTING) || (connectionState >= HID_HOST_STATE_CONNECTED)) {
                size++;
            }
        }
    }
    return size;
}

int HidHostService::GetMaxConnectionsDeviceNum() const
{
    int number = HID_HOST_MAX_DEFAULT_CONNECTIONS_NUMR;
    if (!AdapterConfig::GetInstance()->GetValue(SECTION_HID_HOST_SERVICE, PROPERTY_MAX_CONNECTED_DEVICES, number)) {
        LOG_INFO("[HIDH Service] GetMaxConnectionsDeviceNum failed");
    }
    return number;
}

void HidHostService::RemoveStateMachine(const std::string &device)
{
    HidHostMessage event(HID_HOST_REMOVE_STATE_MACHINE_EVT);
    event.dev_ = device;
    PostEvent(event);
}

void HidHostService::PostEvent(const HidHostMessage &event)
{
    GetDispatcher()->PostTask(std::bind(&HidHostService::ProcessEvent, this, event));
}

void HidHostService::ProcessEvent(const HidHostMessage &event)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = event.dev_;
    LOG_INFO("[HIDH Service] ProcessEvent:address[%{public}s], event_no[%{public}d]",
        address.c_str(), event.what_);
    switch (event.what_) {
        case HID_HOST_SERVICE_STARTUP_EVT:
            StartUp();
            break;
        case HID_HOST_SERVICE_SHUTDOWN_EVT:
            ShutDown();
            break;
        case HID_HOST_API_OPEN_EVT:
        case HID_HOST_L2CAP_CONNECT_REQ_EVT:
            ProcessConnectEvent(event);
            break;
        case HID_HOST_REMOVE_STATE_MACHINE_EVT:
            ProcessRemoveStateMachine(event.dev_);
            break;
        default:
            ProcessDefaultEvent(event);
            break;
    }
}

void HidHostService::ProcessConnectEvent(const HidHostMessage &event)
{
    if (GetConnectionsDeviceNum() < maxConnectionsNum_) {
        auto it = stateMachines_.find(event.dev_);
        if (it != stateMachines_.end() && it->second != nullptr && it->second->IsRemoving()) {
            // peer device may send connect request before we remove statemachine for last connection.
            // so post this connect request, process it after we remove statemachine completely.
            PostEvent(event);
        } else if (it == stateMachines_.end() || it->second == nullptr) {
            stateMachines_[event.dev_] = std::make_unique<HidHostStateMachine>(event.dev_);
            stateMachines_[event.dev_]->Init();
            stateMachines_[event.dev_]->ProcessMessage(event);
            if (event.what_ == HID_HOST_L2CAP_CONNECT_REQ_EVT) {
                stateMachines_[event.dev_]->ProcessL2capConnectionEvent(event);
            }
        } else {
            it->second->ProcessMessage(event);
            if (event.what_ == HID_HOST_L2CAP_CONNECT_REQ_EVT) {
                it->second->ProcessL2capConnectionEvent(event);
            }
        }
    }
}

std::string HidHostService::HidHostFindDeviceByLcid(uint16_t lcid, bool *isControlLcid)
{
    std::string ret;

    std::lock_guard<std::recursive_mutex> lk(mutex_);
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if ((it->second != nullptr) && ((it->second->GetDeviceControlLcid() == lcid) ||
                (it->second->GetDeviceInterruptLcid() == lcid))) {
            if (it->second->GetDeviceControlLcid() == lcid) {
                *isControlLcid = true;
            }
            return it->first;
        }
    }
    return ret;
}

void HidHostService::ProcessRemoveStateMachine(const std::string &address)
{
    stateMachines_.insert_or_assign(address, nullptr);
    if (isShuttingDown_) {
        ShutDownDone(false);
    }
}

void HidHostService::ProcessDefaultEvent(const HidHostMessage &event) const
{
    auto it = stateMachines_.find(event.dev_);
    if ((it != stateMachines_.end()) && (it->second != nullptr)) {
        if ((event.what_ > HID_HOST_L2CAP_START_EVT) && (event.what_ < HID_HOST_L2CAP_END_EVT)) {
            it->second->ProcessL2capConnectionEvent(event);
        } else if ((event.what_ > HID_HOST_HOGP_START_EVT) && (event.what_ < HID_HOST_HOGP_END_EVT)) {
            it->second->ProcessHogpEvent(event);
        } else {
            it->second->ProcessMessage(event);
        }
    } else {
        LOG_ERROR("[HIDH Service] ProcessDefaultEvent:invalid address[%{public}s]", event.dev_.c_str());
    }
}
REGISTER_CLASS_CREATOR(HidHostService);
}  // namespace bluetooth
}  // namespace OHOS
