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

#include "hfp_hf_service.h"

#include "adapter_config.h"
#include "class_creator.h"
#include "hfp_hf_defines.h"
#include "profile_service_manager.h"
#include "stub/telephone_client_service.h"

namespace OHOS {
namespace bluetooth {
class TelephoneHfObserver : public stub::TelephoneClientServiceObserver {
public:
    explicit TelephoneHfObserver(HfpHfService& service) : service_(service)
    {
    }
    ~TelephoneHfObserver() = default;

    void OnBatteryLevelChanged(int batteryLevel) override
    {
        LOG_DEBUG("[HFP HF]%{public}s() batteryLevel[%{public}d]", __PRETTY_FUNCTION__, batteryLevel);
        service_.BatteryLevelChanged(batteryLevel);
    }

    void OnVolumeChanged(int type, int volume) override
    {
        LOG_DEBUG("[HFP HF]%{public}s() type[%{public}d], volume[%{public}d]", __PRETTY_FUNCTION__, type, volume);
        service_.SetHfVolume(volume, type);
    }

    void OnEnhancedDriverSafety(int state) override
    {
        LOG_DEBUG("[HFP HF]%{public}s() state[%{public}d]", __PRETTY_FUNCTION__, state);
        service_.EnhancedDriverSafety(state);
    }

private:
    HfpHfService &service_;
    BT_DISALLOW_COPY_AND_ASSIGN(TelephoneHfObserver);
};

struct HfpHfService::impl {
    explicit impl(HfpHfService& service) : observer_(service)
    {
    }
    TelephoneHfObserver observer_;
};

HfpHfService *HfpHfService::GetService()
{
    auto servManager = IProfileManager::GetInstance();
    return static_cast<HfpHfService *>(servManager->GetProfileService(PROFILE_NAME_HFP_HF));
}

HfpHfService::HfpHfService() : utility::Context(PROFILE_NAME_HFP_HF, "1.7.1"), pimpl(nullptr)
{
    LOG_INFO("[HFP HF]ProfileService:%{public}s Create", Name().c_str());
    pimpl = std::make_unique<impl>(*this);
}

HfpHfService::~HfpHfService()
{
    LOG_INFO("[HFP HF]ProfileService:%{public}s Release", Name().c_str());
}

utility::Context *HfpHfService::GetContext()
{
    return this;
}

void HfpHfService::Enable()
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<enter>==========", __FUNCTION__);
    stub::TelePhoneClientService::GetInstance().RegisterObserver(&pimpl->observer_);
    HfpHfMessage event(HFP_HF_SERVICE_STARTUP_EVT);
    PostEvent(event);
}

void HfpHfService::Disable()
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<enter>==========", __FUNCTION__);
    stub::TelePhoneClientService::GetInstance().DeregisterObserver(&pimpl->observer_);
    HfpHfMessage event(HFP_HF_SERVICE_SHUTDOWN_EVT);
    PostEvent(event);
}

void HfpHfService::StartUp()
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    if (isStarted_ == true) {
        GetContext()->OnEnable(PROFILE_NAME_HFP_HF, true);
        LOG_WARN("[HFP HF]%{public}s():HfpHfService has already been started before.", __FUNCTION__);
        return;
    }

    maxConnectionsNum_ = GetMaxConnectionsDeviceNum();
    int ret = HfpHfProfile::RegisterService();
    GetContext()->OnEnable(PROFILE_NAME_HFP_HF, ret ? false : true);
    if (ret == 0) {
        isStarted_ = true;
        LOG_DEBUG("[HFP HF]%{public}s():HfpHfService started", __FUNCTION__);
    }
}

void HfpHfService::ShutDown()
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    if (isStarted_ == false) {
        GetContext()->OnDisable(PROFILE_NAME_HFP_HF, true);
        LOG_WARN("[HFP HF]%{public}s():HfpHfService has already been shutdown before.", __FUNCTION__);
        return;
    }

    isShuttingDown_ = true;
    bool isDisconnected = false;
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if ((it->second != nullptr) && (it->second->GetDeviceStateInt() > HFP_HF_STATE_DISCONNECTED)) {
            Disconnect(RawAddress(it->first));
            isDisconnected = true;
        }
    }

    if (!isDisconnected) {
        ShutDownDone(true);
    }
}

void HfpHfService::ShutDownDone(bool isAllDisconnected)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    if (!isAllDisconnected) {
        for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
            if ((it->second != nullptr) && (it->second->GetDeviceStateInt() > HFP_HF_STATE_DISCONNECTED)) {
                return;
            }
        }
    }

    int ret = HfpHfProfile::DeregisterService();
    stateMachines_.clear();
    GetContext()->OnDisable(PROFILE_NAME_HFP_HF, ret ? false : true);
    if (ret == 0) {
        isStarted_ = false;
        LOG_DEBUG("[HFP HF]%{public}s():HfpHfService shutdown", __FUNCTION__);
    }
    isShuttingDown_ = false;
}

int HfpHfService::Connect(const RawAddress &device)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if ((it != stateMachines_.end()) && (it->second != nullptr)) {
        int slcState = it->second->GetDeviceStateInt();
        if ((slcState >= HFP_HF_STATE_CONNECTED) || (slcState == HFP_HF_STATE_CONNECTING)) {
            return HFP_HF_FAILURE;
        }
    }

    if (GetConnectionsDeviceNum() >= maxConnectionsNum_) {
        LOG_INFO("[HFP HF]%{public}s():Max connection number has reached!", __FUNCTION__);
        return HFP_HF_FAILURE;
    }

    HfpHfMessage event(HFP_HF_CONNECT_EVT);
    event.dev_ = address;
    PostEvent(event);
    return HFP_HF_SUCCESS;
}

int HfpHfService::Disconnect(const RawAddress &device)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_DEBUG("[HFP HF]%{public}s():The state machine is not available!", __FUNCTION__);
        return HFP_HF_FAILURE;
    }

    int slcState = it->second->GetDeviceStateInt();
    if ((slcState != HFP_HF_STATE_CONNECTING) && (slcState < HFP_HF_STATE_CONNECTED)) {
        LOG_DEBUG("[HFP HF]%{public}s():slcState:%{public}d", __FUNCTION__, slcState);
        return HFP_HF_FAILURE;
    }

    HfpHfMessage event(HFP_HF_DISCONNECT_EVT);
    event.dev_ = address;
    PostEvent(event);
    return HFP_HF_SUCCESS;
}

std::list<RawAddress> HfpHfService::GetConnectDevices()
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::list<RawAddress> devList;
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if ((it->second != nullptr) && (it->second->GetDeviceStateInt() >= HFP_HF_STATE_CONNECTED)) {
            devList.push_back(RawAddress(it->first));
        }
    }
    return devList;
}

int HfpHfService::GetConnectState()
{
    int result = 0;
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if (it->second == nullptr) {
            result |= PROFILE_STATE_DISCONNECTED;
        } else if (it->second->GetDeviceStateInt() >= HFP_HF_STATE_CONNECTED) {
            result |= PROFILE_STATE_CONNECTED;
        } else if (it->second->GetDeviceStateInt() == HFP_HF_STATE_CONNECTING) {
            result |= PROFILE_STATE_CONNECTING;
        } else if (it->second->GetDeviceStateInt() == HFP_HF_STATE_DISCONNECTING) {
            result |= PROFILE_STATE_DISCONNECTING;
        } else if (it->second->GetDeviceStateInt() == HFP_HF_STATE_DISCONNECTED) {
            result |= PROFILE_STATE_DISCONNECTED;
        }
    }
    return result;
}

int HfpHfService::GetMaxConnectNum()
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    return maxConnectionsNum_;
}

bool HfpHfService::IsScoConnected() const
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if (it->second != nullptr) {
            auto audioState = it->second->GetDeviceStateInt();
            if (audioState > HFP_HF_AUDIO_STATE_DISCONNECTED) {
                return true;
            }
        }
    }
    return false;
}

bool HfpHfService::ConnectSco(const RawAddress &device)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_DEBUG("[HFP HF]%{public}s():The state machine is not available!", __FUNCTION__);
        return false;
    }

    if (it->second->GetDeviceStateInt() != HFP_HF_STATE_CONNECTED) {
        LOG_DEBUG("[HFP HF]%{public}s():state:%{public}d", __FUNCTION__, it->second->GetDeviceStateInt());
        return false;
    }

    HfpHfMessage event(HFP_HF_CONNECT_AUDIO_EVT);
    event.dev_ = address;
    PostEvent(event);
    return true;
}

bool HfpHfService::DisconnectSco(const RawAddress &device)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_DEBUG("[HFP HF]%{public}s():The state machine is not available!", __FUNCTION__);
        return false;
    }

    if (it->second->GetDeviceStateInt() <= HFP_HF_STATE_CONNECTED) {
        LOG_DEBUG("[HFP HF]%{public}s():audio state:%{public}d", __FUNCTION__, it->second->GetDeviceStateInt());
        return false;
    }

    HfpHfMessage event(HFP_HF_DISCONNECT_AUDIO_EVT);
    event.dev_ = address;
    PostEvent(event);
    return true;
}

std::vector<RawAddress> HfpHfService::GetDevicesByStates(std::vector<int> states)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
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

int HfpHfService::GetDeviceState(const RawAddress &device)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_DEBUG("[HFP HF]%{public}s():The state machine is not available!", __FUNCTION__);
        return stateMap_.at(HFP_HF_STATE_DISCONNECTED);
    }

    if (it->second->GetDeviceStateInt() >= HFP_HF_STATE_CONNECTED) {
        return stateMap_.at(HFP_HF_STATE_CONNECTED);
    } else {
        return stateMap_.at(it->second->GetDeviceStateInt());
    }
}

int HfpHfService::GetScoState(const RawAddress &device)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        return HFP_HF_AUDIO_STATE_DISCONNECTED;
    }

    if (it->second->GetDeviceStateInt() <= HFP_HF_AUDIO_STATE_DISCONNECTED) {
        return HFP_HF_AUDIO_STATE_DISCONNECTED;
    } else {
        return it->second->GetDeviceStateInt();
    }
}

bool HfpHfService::SendDTMFTone(const RawAddress &device, uint8_t code)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    if (IsConnected(address) == false) {
        return false;
    }

    if ((code < '0' || code > '9') && code != '*' && code != '#') {
        LOG_ERROR("[HFP HF]%{public}s():invalid dtmf code", __FUNCTION__);
        return false;
    }

    HfpHfMessage event(HFP_HF_SEND_DTMF_EVT, code);
    event.dev_ = address;
    PostEvent(event);

    return true;
}

std::vector<HandsFreeUnitCalls> HfpHfService::GetCurrentCallList(const RawAddress &device)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::vector<HandsFreeUnitCalls> calls;
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_DEBUG("[HFP HF]%{public}s():The state machine is not available!", __FUNCTION__);
        return calls;
    }

    if (it->second->GetDeviceStateInt() < HFP_HF_STATE_CONNECTED) {
        LOG_DEBUG("[HFP HF]%{public}s():It's not connected!", __FUNCTION__);
        return calls;
    }

    return it->second->GetCurrentCallList();
}

bool HfpHfService::AcceptIncomingCall(const RawAddress &device, int flag)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    if (IsConnected(address) == false) {
        return false;
    }

    HfpHfMessage event(HFP_HF_ACCEPT_CALL_EVT, flag);
    event.dev_ = address;
    PostEvent(event);
    return true;
}

bool HfpHfService::HoldActiveCall(const RawAddress &device)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    if (IsConnected(address) == false) {
        return false;
    }

    HfpHfMessage event(HFP_HF_HOLD_CALL_EVT);
    event.dev_ = address;
    PostEvent(event);
    return true;
}

bool HfpHfService::RejectIncomingCall(const RawAddress &device)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    if (IsConnected(address) == false) {
        return false;
    }

    HfpHfMessage event(HFP_HF_REJECT_CALL_EVT);
    event.dev_ = address;
    PostEvent(event);
    return true;
}

bool HfpHfService::HandleIncomingCall(const RawAddress &device, int flag)
{
    HILOGI("[HFP HF]:==========<start>==========");
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    if (IsConnected(address) == false) {
        return false;
    }

    HfpHfMessage event(HFP_HF_HANDLE_INCOMING_CALL_EVT, flag);
    event.dev_ = address;
    PostEvent(event);
    return true;
}

bool HfpHfService::HandleMultiCall(const RawAddress &device, int flag, int index)
{
    HILOGI("[HFP HF]:==========<start>==========");
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    if (IsConnected(address) == false) {
        return false;
    }

    HfpHfMessage event(HFP_HF_HANDLE_MULTI_CALL_EVT, flag);
    event.arg3_ = index;
    event.dev_ = address;
    PostEvent(event);
    return true;
}

bool HfpHfService::DialLastNumber(const RawAddress &device)
{
    HILOGI("[HFP HF]:==========<start>==========");
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    if (IsConnected(address) == false) {
        return false;
    }

    HfpHfMessage event(HFP_HF_DIAL_LAST_NUMBER);
    event.dev_ = address;
    PostEvent(event);
    return true;
}

bool HfpHfService::DialMemory(const RawAddress &device, int index)
{
    HILOGI("[HFP HF]:==========<start>========== index = %{public}d", index);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    if (IsConnected(address) == false) {
        return false;
    }

    HfpHfMessage event(HFP_HF_DIAL_MEMORY, index);
    event.dev_ = address;
    PostEvent(event);
    return true;
}

bool HfpHfService::SendVoiceTag(const RawAddress &device, int index)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>========== index = %{public}d", __FUNCTION__, index);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    if (IsConnected(address) == false) {
        return false;
    }

    HfpHfMessage event(HFP_HF_SEND_VOICE_TAG, index);
    event.dev_ = address;
    PostEvent(event);
    return true;
}

bool HfpHfService::SendKeyPressed(const RawAddress &device)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    if (IsConnected(address) == false) {
        return false;
    }

    HfpHfMessage event(HFP_HF_SEND_KEY_PRESSED);
    event.dev_ = address;
    PostEvent(event);
    return true;
}

bool HfpHfService::FinishActiveCall(const RawAddress &device, const HandsFreeUnitCalls &call)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    if (IsConnected(address) == false) {
        return false;
    }

    HfpHfMessage event(HFP_HF_FINISH_CALL_EVT);
    event.dev_ = address;
    event.calls_ = call;
    PostEvent(event);
    return true;
}

std::optional<HandsFreeUnitCalls> HfpHfService::StartDial(const RawAddress &device, const std::string &number)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_DEBUG("[HFP HF]%{public}s():The state machine is not available!", __FUNCTION__);
        return std::nullopt;
    }

    if (it->second->GetDeviceStateInt() < HFP_HF_STATE_CONNECTED) {
        LOG_DEBUG("[HFP HF]%{public}s():It's not connected!", __FUNCTION__);
        return std::nullopt;
    }

    HandsFreeUnitCalls calls(
        address, HFP_HF_OUTGOING_CALL_ID, HFP_CALL_STATE_DIALING, number, false, true, it->second->GetIsInbandRing());
    HfpHfMessage event(HFP_HF_DIAL_CALL_EVT);
    event.dev_ = address;
    event.calls_ = calls;
    PostEvent(event);
    return calls;
}

void HfpHfService::RegisterObserver(HfpHfServiceObserver &observer)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::list<HfpHfServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        if (*iter == &observer) {
            LOG_INFO("Already registered!");
            return;
        }
    }
    observers_.push_back(&observer);
}

void HfpHfService::DeregisterObserver(HfpHfServiceObserver &observer)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::list<HfpHfServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        if (*iter == &observer) {
            break;
        }
    }

    if (iter != observers_.end()) {
        observers_.erase(iter);
    }
    return;
}

void HfpHfService::SetHfVolume(int volume, int type)
{
    HfpHfMessage event(HFP_HF_SET_VOLUME_EVT, volume);
    event.arg3_ = type;
    PostEvent(event);
    return;
}

bool HfpHfService::OpenVoiceRecognition(const RawAddress &device)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    if (IsConnected(address) == false) {
        return false;
    }

    HfpHfMessage event(HFP_HF_OPEN_VOICE_RECOGNITION_EVT);
    event.dev_ = address;
    PostEvent(event);
    return true;
}

bool HfpHfService::CloseVoiceRecognition(const RawAddress &device)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    if (IsConnected(address) == false) {
        return false;
    }

    HfpHfMessage event(HFP_HF_CLOSE_VOICE_RECOGNITION_EVT);
    event.dev_ = address;
    PostEvent(event);
    return true;
}

void HfpHfService::BatteryLevelChanged(int batteryLevel)
{
    HfpHfMessage event(HFP_HF_BATTERY_LEVEL_CHANGED_EVT, batteryLevel);
    PostEvent(event);
}

void HfpHfService::EnhancedDriverSafety(int state)
{
    HfpHfMessage event(HFP_HF_ENHANCED_DRIVER_SAFETY_CHANGED_EVT, state);
    PostEvent(event);
}

void HfpHfService::SendEventToEachStateMachine(const HfpHfMessage &event) const
{
    HfpHfMessage curEvent = event;
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if (it->second != nullptr) {
            curEvent.dev_ = it->first;
            it->second->ProcessMessage(curEvent);
        }
    }
}

void HfpHfService::ProcessConnectEvent(const HfpHfMessage &event)
{
    if (GetConnectionsDeviceNum() < maxConnectionsNum_) {
        auto it = stateMachines_.find(event.dev_);
        if (it != stateMachines_.end() && it->second != nullptr && it->second->IsRemoving()) {
            // peer device may send connect request before we remove statemachine for last connection.
            // so post this connect request, process it after we remove statemachine completely.
            PostEvent(event);
        } else if (it == stateMachines_.end() || it->second == nullptr) {
            stateMachines_[event.dev_] = std::make_unique<HfpHfStateMachine>(event.dev_);
            stateMachines_[event.dev_]->Init();
            stateMachines_[event.dev_]->ProcessMessage(event);
        } else {
            it->second->ProcessMessage(event);
        }
    }
}

void HfpHfService::ProcessRemoveStateMachine(const std::string &address)
{
    stateMachines_.insert_or_assign(address, nullptr);
    if (isShuttingDown_) {
        ShutDownDone(false);
    }
}

void HfpHfService::ProcessDefaultEvent(const HfpHfMessage &event) const
{
    auto it = stateMachines_.find(event.dev_);
    if ((it != stateMachines_.end()) && (it->second != nullptr)) {
        it->second->ProcessMessage(event);
    } else {
        LOG_DEBUG("[HFP HF]%{public}s():invalid address[%{public}s]", __FUNCTION__, event.dev_.c_str());
    }
}

void HfpHfService::PostEvent(const HfpHfMessage &event)
{
    GetDispatcher()->PostTask(std::bind(&HfpHfService::ProcessEvent, this, event));
}

void HfpHfService::ProcessEvent(const HfpHfMessage &event)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = event.dev_;
    LOG_DEBUG("[HFP HF]%{public}s():address[%{public}s] event_no[%{public}d]", __FUNCTION__, address.c_str(), event.what_);
    switch (event.what_) {
        case HFP_HF_SERVICE_STARTUP_EVT:
            StartUp();
            break;
        case HFP_HF_SERVICE_SHUTDOWN_EVT:
            ShutDown();
            break;
        case HFP_HF_CONNECT_EVT:
        case HFP_HF_CONNECT_REQUEST_EVT:
            ProcessConnectEvent(event);
            break;
        case HFP_HF_REMOVE_STATE_MACHINE_EVT:
            ProcessRemoveStateMachine(event.dev_);
            break;
        case HFP_HF_SET_VOLUME_EVT:
        case HFP_HF_BATTERY_LEVEL_CHANGED_EVT:
        case HFP_HF_ENHANCED_DRIVER_SAFETY_CHANGED_EVT:
            SendEventToEachStateMachine(event);
            break;
        default:
            ProcessDefaultEvent(event);
            break;
    }
}

void HfpHfService::NotifyStateChanged(const RawAddress &device, int state)
{
    LOG_INFO("[HFP HF]%{public}s():", __FUNCTION__);
    std::list<HfpHfServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        (*iter)->OnConnectionStateChanged(device, stateMap_.at(state));
    }
}

void HfpHfService::NotifyScoStateChanged(const RawAddress &device, int state)
{
    LOG_INFO("[HFP HF]%{public}s():", __FUNCTION__);
    std::list<HfpHfServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        (*iter)->OnScoStateChanged(device, state);
    }
}

void HfpHfService::NotifyCallChanged(const RawAddress &device, const HandsFreeUnitCalls &call)
{
    LOG_INFO("[HFP HF]%{public}s():", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::list<HfpHfServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        (*iter)->OnCallChanged(device, call);
    }
}

void HfpHfService::NotifyBatteryLevelChanged(const RawAddress &device, int batteryLevel)
{
    LOG_INFO("[HFP HF]%{public}s():", __FUNCTION__);
    std::list<HfpHfServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        (*iter)->OnBatteryLevelChanged(device, batteryLevel);
    }
}

void HfpHfService::NotifySignalStrengthChanged(const RawAddress &device, int signal)
{
    LOG_INFO("[HFP HF]%{public}s():", __FUNCTION__);
    std::list<HfpHfServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        (*iter)->OnSignalStrengthChanged(device, signal);
    }
}

void HfpHfService::NotifyRegistrationStatusChanged(const RawAddress &device, int status)
{
    LOG_INFO("[HFP HF]%{public}s():", __FUNCTION__);
    std::list<HfpHfServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        (*iter)->OnRegistrationStatusChanged(device, status);
    }
}

void HfpHfService::NotifyRoamingStatusChanged(const RawAddress &device, int status)
{
    LOG_INFO("[HFP HF]%{public}s():", __FUNCTION__);
    std::list<HfpHfServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        (*iter)->OnRoamingStatusChanged(device, status);
    }
}

void HfpHfService::NotifyOperatorSelectionChanged(const RawAddress &device, const std::string &name)
{
    LOG_INFO("[HFP HF]%{public}s():", __FUNCTION__);
    std::list<HfpHfServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        (*iter)->OnOperatorSelectionChanged(device, name);
    }
}

void HfpHfService::NotifySubscriberNumberChanged(const RawAddress &device, const std::string &number)
{
    LOG_INFO("[HFP HF]%{public}s():", __FUNCTION__);
    std::list<HfpHfServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        (*iter)->OnSubscriberNumberChanged(device, number);
    }
}

void HfpHfService::NotifyVoiceRecognitionStatusChanged(const RawAddress &device, int status)
{
    LOG_INFO("[HFP HF]%{public}s():", __FUNCTION__);
    std::list<HfpHfServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        (*iter)->OnVoiceRecognitionStatusChanged(device, status);
    }
}

void HfpHfService::NotifyInBandRingTone(const RawAddress &device, int status)
{
    LOG_INFO("[HFP HF]%{public}s():", __FUNCTION__);
    std::list<HfpHfServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        (*iter)->OnInBandRingToneChanged(device, status);
    }
}

int HfpHfService::GetConnectionsDeviceNum() const
{
    int size = 0;
    for (auto iter = stateMachines_.begin(); iter != stateMachines_.end(); ++iter) {
        if (iter->second != nullptr) {
            auto connectionState = iter->second->GetDeviceStateInt();
            if ((connectionState == HFP_HF_STATE_CONNECTING) || (connectionState >= HFP_HF_STATE_CONNECTED)) {
                size++;
            }
        }
    }
    return size;
}

int HfpHfService::GetMaxConnectionsDeviceNum() const
{
    int number = MAX_CONNECTIONS_NUM;
    if (!AdapterConfig::GetInstance()->GetValue(SECTION_HFP_HF_SERVICE, PROPERTY_MAX_CONNECTED_DEVICES, number)) {
        LOG_DEBUG("[HFP HF]%{public}s():It's failed to get the max connection number", __FUNCTION__);
    }
    return number;
}

void HfpHfService::SendAtCommand(const RawAddress &device, int cmdId, const std::string &arg)
{
    LOG_DEBUG("[HFP HF]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    if (IsConnected(address) == false) {
        return;
    }

    HfpHfMessage event(HFP_HF_SEND_AT_COMMAND_EVT);
    event.dev_ = address;
    event.arg1_ = cmdId;
    event.str_ = arg;
    PostEvent(event);
    return;
}

bool HfpHfService::IsConnected(const std::string &address) const
{
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_ERROR("[HFP AG]%{public}s():Invalid Device address:%{public}s", __FUNCTION__, address.c_str());
        return false;
    }
    if (it->second->GetDeviceStateInt() < HFP_HF_STATE_CONNECTED) {
        LOG_DEBUG("[HFP HF]%{public}s():It's not connected!", __FUNCTION__);
        return false;
    }
    return true;
}

REGISTER_CLASS_CREATOR(HfpHfService);
}  // namespace bluetooth
}  // namespace OHOS
