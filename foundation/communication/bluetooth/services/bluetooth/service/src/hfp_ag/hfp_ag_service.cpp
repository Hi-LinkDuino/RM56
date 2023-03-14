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

#include "hfp_ag_service.h"

#include "adapter_config.h"
#include "class_creator.h"
#include "hfp_ag_defines.h"
#include "hfp_ag_system_interface.h"
#include "log_util.h"
#include "profile_service_manager.h"
#include "stub/telephone_service.h"

namespace OHOS {
namespace bluetooth {
HfpAgService::HfpAgService() : utility::Context(PROFILE_NAME_HFP_AG, "1.7.1")
{
    LOG_INFO("[HFP AG]ProfileService:%{public}s Create", Name().c_str());
}

HfpAgService::~HfpAgService()
{
    LOG_INFO("[HFP AG]ProfileService:%{public}s Release", Name().c_str());
}

utility::Context *HfpAgService::GetContext()
{
    return this;
}

HfpAgService *HfpAgService::GetService()
{
    auto servManager = IProfileManager::GetInstance();
    return static_cast<HfpAgService *>(servManager->GetProfileService(PROFILE_NAME_HFP_AG));
}

void HfpAgService::Enable()
{
    LOG_INFO("[HFP AG]%{public}s():==========<enter>==========", __FUNCTION__);
    HfpAgMessage event(HFP_AG_SERVICE_STARTUP_EVT);
    PostEvent(event);
}

void HfpAgService::Disable()
{
    LOG_INFO("[HFP AG]%{public}s():==========<enter>==========", __FUNCTION__);
    HfpAgMessage event(HFP_AG_SERVICE_SHUTDOWN_EVT);
    PostEvent(event);
}

void HfpAgService::StartUp()
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    if (isStarted_ == true) {
        GetContext()->OnEnable(PROFILE_NAME_HFP_AG, true);
        LOG_WARN("[HFP AG]%{public}s():HfpAgService has already been started before.", __FUNCTION__);
        return;
    }

    maxConnectedNum_ = GetMaxConnectionDevicesNum();
    HfpAgSystemEventProcesser::Start();
    int ret = HfpAgProfile::RegisterService();
    GetContext()->OnEnable(PROFILE_NAME_HFP_AG, ret ? false : true);
    if (ret == 0) {
        isStarted_ = true;
        LOG_INFO("[HFP AG]%{public}s():HfpAgService started", __FUNCTION__);
    }
}

void HfpAgService::ShutDown()
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    if (isStarted_ == false) {
        GetContext()->OnDisable(PROFILE_NAME_HFP_AG, true);
        LOG_WARN("[HFP AG]%{public}s():HfpAgService has already been shutdown before.", __FUNCTION__);
        return;
    }

    isShuttingDown_ = true;
    bool isDisconnected = false;
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if ((it->second != nullptr) && (it->second->GetStateInt() > HFP_AG_STATE_DISCONNECTED)) {
            Disconnect(RawAddress(it->first));
            isDisconnected = true;
        }
    }

    if (!isDisconnected) {
        ShutDownDone(true);
    }
}

void HfpAgService::ShutDownDone(bool isAllDisconnected)
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    if (!isAllDisconnected) {
        for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
            if ((it->second != nullptr) && (it->second->GetStateInt() > HFP_AG_STATE_DISCONNECTED)) {
                return;
            }
        }
    }

    stateMachines_.clear();
    HfpAgSystemEventProcesser::Stop();
    HfpAgProfile::SetActiveDevice(NULL_ADDRESS);

    isVrOpened_ = false;
    isInbandRinging_ = false;
    queryClccAddress_ = NULL_ADDRESS;
    maxConnectedNum_ = MAX_DEFAULT_CONNECTIONS_NUM;

    if (dialingOutTimeout_ != nullptr) {
        dialingOutTimeout_->Stop();
        dialingOutTimeout_ = nullptr;
    }

    if (ResponseClccTimeout_ != nullptr) {
        ResponseClccTimeout_->Stop();
        ResponseClccTimeout_ = nullptr;
    }
    if (voiceRecognitionTimeout_ != nullptr) {
        voiceRecognitionTimeout_->Stop();
        voiceRecognitionTimeout_ = nullptr;
    }

    int ret = HfpAgProfile::DeregisterService();
    GetContext()->OnDisable(PROFILE_NAME_HFP_AG, ret ? false : true);
    if (ret == 0) {
        isStarted_ = false;
        LOG_INFO("[HFP AG]%{public}s():HfpAgService shutdown", __FUNCTION__);
    }
    isShuttingDown_ = false;
}

int HfpAgService::Connect(const RawAddress &device)
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if ((it != stateMachines_.end()) && (it->second != nullptr)) {
        int state = it->second->GetStateInt();
        if ((state >= HFP_AG_STATE_CONNECTED) || (state == HFP_AG_STATE_CONNECTING)) {
            LOG_INFO("[HFP AG]%{public}s():state:%{public}d", __FUNCTION__, state);
            return HFP_AG_FAILURE;
        }
    }

    int size = GetConnectedDeviceNum();
    if (size >= maxConnectedNum_) {
        LOG_INFO("[HFP AG]%{public}s():Max connection has reached!", __FUNCTION__);
        return HFP_AG_FAILURE;
    }

    HfpAgMessage event(HFP_AG_CONNECT_EVT);
    event.dev_ = address;
    PostEvent(event);
    return HFP_AG_SUCCESS;
}

int HfpAgService::Disconnect(const RawAddress &device)
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        return HFP_AG_FAILURE;
    }

    int slcState = it->second->GetStateInt();
    if ((slcState != HFP_AG_STATE_CONNECTING) && (slcState < HFP_AG_STATE_CONNECTED)) {
        return HFP_AG_FAILURE;
    }

    HfpAgMessage event(HFP_AG_DISCONNECT_EVT);
    event.dev_ = address;
    PostEvent(event);
    return HFP_AG_SUCCESS;
}

std::list<RawAddress> HfpAgService::GetConnectDevices()
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::list<RawAddress> devList;
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if ((it->second != nullptr) && (it->second->GetStateInt() >= HFP_AG_STATE_CONNECTED)) {
            devList.push_back(RawAddress(it->first));
        }
    }
    return devList;
}

int HfpAgService::GetConnectState()
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    int result = 0;
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if (it->second == nullptr) {
            result |= PROFILE_STATE_DISCONNECTED;
        } else if (it->second->GetStateInt() >= HFP_AG_STATE_CONNECTED) {
            result |= PROFILE_STATE_CONNECTED;
        } else if (it->second->GetStateInt() == HFP_AG_STATE_CONNECTING) {
            result |= PROFILE_STATE_CONNECTING;
        } else if (it->second->GetStateInt() == HFP_AG_STATE_DISCONNECTING) {
            result |= PROFILE_STATE_DISCONNECTING;
        } else if (it->second->GetStateInt() == HFP_AG_STATE_DISCONNECTED) {
            result |= PROFILE_STATE_DISCONNECTED;
        }
    }
    return result;
}

int HfpAgService::GetDeviceState(const RawAddress &device)
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_INFO("[HFP AG]%{public}s():the statemachine is not available", __FUNCTION__);
        return stateMap_.at(HFP_AG_STATE_DISCONNECTED);
    }

    if (it->second->GetStateInt() >= HFP_AG_STATE_CONNECTED) {
        return stateMap_.at(HFP_AG_STATE_CONNECTED);
    } else {
        return stateMap_.at(it->second->GetStateInt());
    }
}

int HfpAgService::GetMaxConnectNum()
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    return maxConnectedNum_;
}

void HfpAgService::PostEvent(const HfpAgMessage &event)
{
    GetDispatcher()->PostTask(std::bind(&HfpAgService::ProcessEvent, this, event));
}

void HfpAgService::ProcessEvent(const HfpAgMessage &event)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    HILOGI("addr: %{public}s, event_no: %{public}d", GetEncryptAddr(event.dev_).c_str(), event.what_);
    switch (event.what_) {
        case HFP_AG_SERVICE_STARTUP_EVT:
            StartUp();
            break;
        case HFP_AG_SERVICE_SHUTDOWN_EVT:
            ShutDown();
            break;
        case HFP_AG_CONNECT_EVT:
        case HFP_AG_CONNECT_REQUEST_EVT:
            ProcessConnectEvent(event);
            break;
        case HFP_AG_REMOVE_STATE_MACHINE_EVT:
            ProcessRemoveStateMachine(event.dev_);
            break;
        case HFP_AG_VOICE_RECOGNITION_TIME_OUT_EVT:
            ProcessVoiceRecognitionTimeOut(event);
            break;
        case HFP_AG_RESPONSE_CLCC_TIME_OUT_EVT:
            ProcessResponseClccTimeOut(event);
            break;
        case HFP_AG_DIAL_TIME_OUT_EVT:
            ProcessDialOutTimeOut();
            break;
        case HFP_AG_SET_VOLUME_EVT:
        case HFP_AG_NOTIFY_SERVICE_STATE:
        case HFP_AG_NOTIFY_ROAM_STATE:
        case HFP_AG_NOTIFY_SIGNAL_STRENGTH:
        case HFP_AG_NOTIFY_BATTERY_LEVEL:
        case HFP_AG_CALL_STATE_CHANGE:
        case HFP_AG_CALL_STATE_CHANGE_MOCK:
        case HFP_AG_SEND_CCLC_RESPONSE:
        case HFP_AG_DIALING_OUT_RESULT:
        case HFP_AG_SET_INBAND_RING_TONE_EVT:
        case HFP_AG_SEND_INCOMING_EVT:
        case HFP_AG_SEND_CALL_SETUP_EVT:
        case HFP_AG_SEND_BINP_EVT:
        case HFP_AG_GET_BTRH_EVT:
        case HFP_AG_SET_BTRH_EVT:
        case HFP_AG_SEND_RESPONSE_HOLD_STATE:
        case HFP_AG_SEND_BTRH_EVT:
        case HFP_AG_SEND_NO_CARRIER:
        case HFP_AG_START_MOCK:
        case HFP_AG_SEND_CALL_HELD_EVT:
            SendEventToEachStateMachine(event);
            break;
        default:
            ProcessDefaultEvent(event);
            break;
    }
}

std::vector<RawAddress> HfpAgService::GetDevicesByStates(std::vector<int> states)
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
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

bool HfpAgService::ConnectSco()
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string activeDevice = HfpAgProfile::GetActiveDevice();
    if (activeDevice == NULL_ADDRESS) {
        LOG_DEBUG("[HFP AG]%{public}s():The active device is null!", __FUNCTION__);
        return false;
    }

    if (IsConnected(activeDevice) == false) {
        return false;
    }

    if (IsAudioConnected()) {
        LOG_INFO("[HFP AG]%{public}s():Sco is not idle!", __FUNCTION__);
        return false;
    }

    HfpAgMessage event(HFP_AG_CONNECT_AUDIO_EVT);
    event.dev_ = activeDevice;
    PostEvent(event);
    return true;
}

bool HfpAgService::DisconnectSco()
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    bool ret = false;
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if ((it->second != nullptr) && (it->second->GetStateInt() > HFP_AG_AUDIO_STATE_DISCONNECTED) &&
            (DisconnectSingleSco(it->first))) {
            ret = true;
        }
    }

    if (!ret) {
        LOG_DEBUG("[HFP AG]%{public}s(): No audio connection exist!!!", __FUNCTION__);
    }
    return ret;
}

bool HfpAgService::DisconnectSingleSco(const std::string &address)
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_ERROR("[HFP AG]%{public}s():Invalid Device address:%{public}s", __FUNCTION__, address.c_str());
        return false;
    }

    if (it->second->GetStateInt() == HFP_AG_AUDIO_STATE_DISCONNECTED) {
        LOG_ERROR("[HFP AG]%{public}s():Sco is already disconnected on specific device! address:%{public}s, state[%{public}d]",
            __FUNCTION__,
            address.c_str(),
            it->second->GetStateInt());
        return false;
    }

    HfpAgMessage event(HFP_AG_DISCONNECT_AUDIO_EVT);
    event.dev_ = address;
    PostEvent(event);
    return true;
}

int HfpAgService::GetScoState(const RawAddress &device)
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_INFO("[HFP AG]%{public}s():the statemachine is not available.", __FUNCTION__);
        return HFP_AG_AUDIO_STATE_DISCONNECTED;
    }

    if (it->second->GetStateInt() <= HFP_AG_AUDIO_STATE_DISCONNECTED) {
        return HFP_AG_AUDIO_STATE_DISCONNECTED;
    } else {
        return it->second->GetStateInt();
    }
}

bool HfpAgService::IsAudioConnected() const
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if (it->second != nullptr) {
            auto audioState = it->second->GetStateInt();
            if (audioState > HFP_AG_AUDIO_STATE_DISCONNECTED) {
                return true;
            }
        }
    }
    return false;
}

bool HfpAgService::IsAudioIdle() const
{
    if (isVrOpened_) {
        return false;
    }

    const HfpAgSystemInterface &systemInterface = HfpAgSystemInterface::GetInstance();
    if (systemInterface.IsInCall()) {
        return false;
    }

    if (systemInterface.IsRinging()) {
        return false;
    }

    return true;
}

void HfpAgService::SendEventToEachStateMachine(const HfpAgMessage &event) const
{
    HfpAgMessage curEvent = event;
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if (it->second != nullptr) {
            curEvent.dev_ = it->first;
            it->second->ProcessMessage(curEvent);
        }
    }
}

void HfpAgService::UpdateAgIndicators() const
{
    HfpAgMessage evt(HFP_AG_NOTIFY_INDICATOR_EVT);
    evt.type_ = HFP_AG_NOTIFY_INDICATOR_EVT;
    for (auto it = stateMachines_.begin(); it != stateMachines_.end(); ++it) {
        if (it->second != nullptr) {
            evt.dev_ = it->first;
            it->second->ProcessMessage(evt);
        }
    }
}

void HfpAgService::UpdateMockCallList(int callState, const std::string &number, int type)
{
    HfpAgMessage curEvent(HFP_AG_CALL_STATE_CHANGE_MOCK);
    curEvent.state_.callState = callState;
    curEvent.state_.number = number;
    curEvent.state_.type = type;
    PostEvent(curEvent);
    MockCall call;
    call.callstate = callState;
    call.number = number;
    call.type = CALL_TYPE_DEFAULT;
    int sameindex = -1;
    LOG_INFO("HFP AG MOCK moko changed number = %{public}s, state = %{public}d", number.c_str(), callState);
    for (int i = 0; i < callList_.size(); i++) {
        if (strcmp(callList_[i].number.c_str(), number.c_str()) == 0) {
            sameindex = i;
        }
    }
    if (sameindex != -1) {
        callList_.erase(callList_.begin() + sameindex);
    }
    callList_.push_back(call);
}

void HfpAgService::PhoneStateChanged(
    int numActive, int numHeld, int callState, const std::string &number, int type, const std::string &name)
{
    LOG_INFO("[HFP AG]%{public}s(): ==========<start>==========", __FUNCTION__);
    if (mockState_ == HFP_AG_MOCK) {
        UpdateMockCallList(callState, number, type);
        return;
    }
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    if (dialingOutTimeout_ != nullptr) {
        if ((callState == HFP_AG_CALL_STATE_ACTIVE) || (callState == HFP_AG_CALL_STATE_IDLE)) {
            dialingOutTimeout_->Stop();
            dialingOutTimeout_ = nullptr;
        } else if (callState == HFP_AG_CALL_STATE_DIALING) {
            dialingOutTimeout_->Stop();
            dialingOutTimeout_ = nullptr;
            HfpAgMessage event(HFP_AG_DIALING_OUT_RESULT, HFP_AG_RESULT_OK);
            PostEvent(event);
        }
    }

    if ((numActive > 0) || (numHeld > 0) || (callState != HFP_AG_CALL_STATE_IDLE)) {
        if (isVrOpened_) {
            LOG_INFO("[HFP AG]%{public}s():close the voice recognition", __FUNCTION__);
            CloseVoiceRecognition(RawAddress(HfpAgProfile::GetActiveDevice()));
        }
    }

    HfpAgSystemInterface::GetInstance().SetActiveCallNumber(numActive);
    HfpAgSystemInterface::GetInstance().SetHeldCallNumber(numHeld);
    HfpAgSystemInterface::GetInstance().SetCallState(callState);

    HfpAgMessage curEvent(HFP_AG_CALL_STATE_CHANGE);
    curEvent.state_.activeNum = numActive;
    curEvent.state_.heldNum = numHeld;
    curEvent.state_.callState = callState;
    curEvent.state_.number = number;
    curEvent.state_.type = type;
    curEvent.state_.name = name;
    PostEvent(curEvent);
    UpdateAgIndicators();
}

void HfpAgService::ClccResponse(
    int index, int direction, int status, int mode, bool mpty, const std::string &number, int type)
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    if (ResponseClccTimeout_ != nullptr) {
        ResponseClccTimeout_->Stop();
        queryClccAddress_ = NULL_ADDRESS;
        ResponseClccTimeout_ = nullptr;
    }

    HfpAgMessage curEvent(HFP_AG_SEND_CCLC_RESPONSE);
    curEvent.call_.index = index;
    curEvent.call_.dir = direction;
    curEvent.call_.state = status;
    curEvent.call_.mode = mode;
    curEvent.call_.mpty = mpty;
    curEvent.call_.type = type;
    curEvent.call_.number = number;
    PostEvent(curEvent);
}

void HfpAgService::SendOpenVoiceEvent(const std::string &address, bool isRequestByHf)
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    if (isRequestByHf) {
        HfpAgMessage evt(HFP_AG_VOICE_RECOGNITION_RESULT_EVT, 1);
        evt.dev_ = address;
        PostEvent(evt);
    } else {
        HfpAgMessage evt(HFP_AG_OPEN_VOICE_RECOGNITION_EVT);
        evt.dev_ = address;
        PostEvent(evt);
    }
    isVrOpened_ = true;
}

bool HfpAgService::OpenVoiceRecognition(const RawAddress &device)
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    if (IsVoiceRecognitionAvailable(address) == false) {
        return false;
    }

    if (voiceRecognitionTimeout_ != nullptr) {
        // VR is opening by handsfree unit, send result to handsfree unit
        voiceRecognitionTimeout_->Stop();
        voiceRecognitionTimeout_ = nullptr;
        SendOpenVoiceEvent(address, true);
    } else {
        // VR is opening by audio gateway, notify handsfree unit
        SendOpenVoiceEvent(address, false);
    }

    return true;
}

bool HfpAgService::CloseVoiceRecognition(const RawAddress &device)
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    if (!isVrOpened_) {
        LOG_INFO("[HFP AG]%{public}s():The VR is not opened!", __FUNCTION__);
        return false;
    }

    std::string address = device.GetAddress();
    if (IsConnected(address) == false) {
        return false;
    }

    if (IsActiveDevice(address) == false) {
        return false;
    }

    SendCloseVoiceEvent(address);
    return true;
}

void HfpAgService::SendCloseVoiceEvent(const std::string &address)
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    HfpAgMessage evt1(HFP_AG_CLOSE_VOICE_RECOGNITION_EVT);
    evt1.dev_ = address;
    PostEvent(evt1);

    HfpAgMessage evt2(HFP_AG_DISCONNECT_AUDIO_EVT);
    evt2.dev_ = address;
    PostEvent(evt2);

    isVrOpened_ = false;
}

void HfpAgService::SendMockCmd(const HfpAgMessage &event)
{
    SendEventToEachStateMachine(event);
}

bool HfpAgService::SetActiveDevice(const RawAddress &device)
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    if (address == NULL_ADDRESS) {
        ClearActiveDevice();
        return true;
    }

    if (IsConnected(address) == false) {
        return false;
    }

    if (IsActiveDevice(address) == false) {
        ModifyActiveDevice(address);
    }

    return true;
}

bool HfpAgService::IntoMock(int state)
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    mockState_ = state;
    HfpAgMessage evt(HFP_AG_START_MOCK);
    evt.arg1_ = state;
    PostEvent(evt);
    if (state == HFP_AG_MOCK_DEFAULT) {
        callList_.clear();
    }
    return true;
}

bool HfpAgService::SendNoCarrier(const RawAddress &device)
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::string address = device.GetAddress();
    HfpAgMessage evt(HFP_AG_SEND_NO_CARRIER);
    evt.dev_ = address;
    PostEvent(evt);
    return true;
}

int HfpAgService::GetMockState()
{
    return mockState_;
}

std::vector<MockCall> HfpAgService::GetCallList()
{
    return callList_;
}

void HfpAgService::ClearActiveDevice()
{
    std::string preActiveDevice = HfpAgProfile::GetActiveDevice();
    if (isVrOpened_) {
        HfpAgMessage evt(HFP_AG_CLOSE_VOICE_RECOGNITION_EVT);
        evt.dev_ = preActiveDevice;
        PostEvent(evt);
    }
    DisconnectSingleSco(preActiveDevice);
    HfpAgProfile::SetActiveDevice(NULL_ADDRESS);
    NotifyCurrentActiveDevice(NULL_ADDRESS);
}

bool HfpAgService::IsInbandRinging()
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    return isInbandRinging_;
}

bool HfpAgService::IsIncall()
{
    const HfpAgSystemInterface &systemInterface = HfpAgSystemInterface::GetInstance();
    return (systemInterface.IsInCall() || (systemInterface.IsRinging() && IsInbandRinging()));
}

std::string HfpAgService::GetActiveDevice()
{
    LOG_INFO("[HFP AG]%{public}s():==========<start>==========", __FUNCTION__);
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    return HfpAgProfile::GetActiveDevice();
}

void HfpAgService::NotifyAgIndicatorStateChanged(int what, int state)
{
    HfpAgMessage curEvent(what, state);
    PostEvent(curEvent);
}

void HfpAgService::NotifyAgIncomingStateChanged(int what, std::string number, int type)
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    HfpAgMessage curEvent(what, type);
    curEvent.str_ = number;
    PostEvent(curEvent);
}

void HfpAgService::NotifyAgResponseHoldStateChanged(int what, int state)
{
    LOG_INFO("[HFP AG]%{public}s():enter",  __FUNCTION__);
    HfpAgMessage curEvent(what, state);
    PostEvent(curEvent);
}

void HfpAgService::SendBinpNumber(std::string number)
{
    HfpAgMessage curEvent(HFP_AG_SEND_BINP_EVT);
    curEvent.str_ = number;
    PostEvent(curEvent);
}

void HfpAgService::RegisterObserver(HfpAgServiceObserver &observer)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::list<HfpAgServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        if (*iter == &observer) {
            LOG_INFO("Already registered!");
            return;
        }
    }
    observers_.push_back(&observer);
}

void HfpAgService::DeregisterObserver(HfpAgServiceObserver &observer)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    std::list<HfpAgServiceObserver *>::iterator iter;
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

void HfpAgService::NotifySlcStateChanged(const RawAddress &device, int toState)
{
    HILOGI("[HFP AG] device:%{public}s, toState:%{public}d", GET_ENCRYPT_ADDR(device), toState);
    std::list<HfpAgServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        (*iter)->OnConnectionStateChanged(device, stateMap_.at(toState));
    }
}

void HfpAgService::NotifyAudioStateChanged(const RawAddress &device, int toState)
{
    HILOGI("[HFP AG] device:%{public}s, toState:%{public}d", GET_ENCRYPT_ADDR(device), toState);
    std::list<HfpAgServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        (*iter)->OnScoStateChanged(device, toState);
    }
}

void HfpAgService::NotifyCurrentActiveDevice(const RawAddress &device)
{
    LOG_INFO("[HFP AG]%{public}s():", __FUNCTION__);
    std::list<HfpAgServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        (*iter)->OnActiveDeviceChanged(device);
    }
}

void HfpAgService::NotifyHfEnhancedDriverSafety(const RawAddress &device, int indValue)
{
    LOG_INFO("[HFP AG]%{public}s():", __FUNCTION__);
    std::list<HfpAgServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        (*iter)->OnHfEnhancedDriverSafetyChanged(device, indValue);
    }
}

void HfpAgService::NotifyHfBatteryLevel(const RawAddress &device, int indValue)
{
    LOG_INFO("[HFP AG]%{public}s():", __FUNCTION__);
    std::list<HfpAgServiceObserver *>::iterator iter;
    for (iter = observers_.begin(); iter != observers_.end(); ++iter) {
        (*iter)->OnHfBatteryLevelChanged(device, indValue);
    }
}

bool HfpAgService::DialOutCallByHf(const std::string &address)
{
    if (mockState_ == HFP_AG_MOCK) {
        return true;
    }
    if (dialingOutTimeout_ != nullptr) {
        LOG_ERROR("[HFP AG]%{public}s():already dialing out!", __FUNCTION__);
        return false;
    }

    if (!SetActiveDevice(RawAddress(address))) {
        LOG_ERROR("[HFP AG]%{public}s():It's failed to set active device to %{public}s", __FUNCTION__, address.c_str());
        return false;
    }

    dialingOutTimeout_ =
        std::make_unique<utility::Timer>(std::bind(&bluetooth::HfpAgService::DialOutCallTimeOut, this));
    dialingOutTimeout_->Start(DIALING_OUT_TIMEOUT_TIME);
    LOG_INFO("[HFP AG]%{public}s():start dial timer!", __FUNCTION__);
    return true;
}

void HfpAgService::DialOutCallTimeOut()
{
    HfpAgMessage event(HFP_AG_DIAL_TIME_OUT_EVT);
    PostEvent(event);
}

bool HfpAgService::OpenVoiceRecognitionByHf(const std::string &address)
{
    // VR opening
    if (voiceRecognitionTimeout_ != nullptr) {
        LOG_ERROR("[HFP AG]%{public}s():voice recognition is opening", __FUNCTION__);
        return false;
    }

    // VR opened or in call
    if (!IsAudioIdle()) {
        LOG_INFO("[HFP AG]%{public}s():sco is not idle!", __FUNCTION__);
        return false;
    }

    if (!SetActiveDevice(RawAddress(address))) {
        LOG_ERROR("[HFP AG]%{public}s():It's failed to set active device!", __FUNCTION__);
        return false;
    }

    voiceRecognitionTimeout_ =
        std::make_unique<utility::Timer>(std::bind(&bluetooth::HfpAgService::VoiceRecognitionTimeOut, this));
    voiceRecognitionTimeout_->Start(VOICE_RECOGNITION_TIMEOUT_TIME);
    LOG_INFO("[HFP AG]%{public}s():start open voice recognition timer!", __FUNCTION__);
    return true;
}

bool HfpAgService::CloseVoiceRecognitionByHf(const std::string &address)
{
    if (IsActiveDevice(address) == false) {
        return false;
    }

    // VR not opening or opened
    if (!isVrOpened_ && (voiceRecognitionTimeout_ == nullptr)) {
        LOG_ERROR("[HFP AG]%{public}s():voice recognition is not opening or opened!", __FUNCTION__);
        return false;
    }

    // VR opening
    if (voiceRecognitionTimeout_ != nullptr) {
        voiceRecognitionTimeout_->Stop();
        voiceRecognitionTimeout_ = nullptr;
    }

    // VR opened
    if (isVrOpened_) {
        DisconnectSingleSco(address);
        isVrOpened_ = false;
    }

    const HfpAgSystemInterface &systemInterface = HfpAgSystemInterface::GetInstance();
    systemInterface.StopVoiceRecogition(address);
    return true;
}

void HfpAgService::VoiceRecognitionTimeOut()
{
    HfpAgMessage event(HFP_AG_VOICE_RECOGNITION_TIME_OUT_EVT);
    PostEvent(event);
}

void HfpAgService::SetResponseClccTimer(const std::string &address)
{
    if (ResponseClccTimeout_ != nullptr) {
        ResponseClccTimeout_->Stop();
        queryClccAddress_ = NULL_ADDRESS;
        ResponseClccTimeout_ = nullptr;
    }

    ResponseClccTimeout_ =
        std::make_unique<utility::Timer>(std::bind(&bluetooth::HfpAgService::ResponseClccTimeOut, this));
    ResponseClccTimeout_->Start(RESPONSE_CLCC_TIMEOUT_TIME);
    queryClccAddress_ = address;
    LOG_INFO("[HFP AG]%{public}s():start clcc timer!", __FUNCTION__);
}

void HfpAgService::ResponseClccTimeOut()
{
    HfpAgMessage event(HFP_AG_RESPONSE_CLCC_TIME_OUT_EVT);
    PostEvent(event);
}

void HfpAgService::SlcStateChanged(const std::string &address, int toState)
{
    if (toState == HFP_AG_STATE_DISCONNECTED) {
        if (HfpAgProfile::GetActiveDevice() == address) {
            SetActiveDevice(RawAddress(NULL_ADDRESS));
        }
    }
}

void HfpAgService::ScoStateChanged(const std::string &address, int fromState, int toState)
{
    if (toState == HFP_AG_AUDIO_STATE_DISCONNECTED) {
        if (voiceRecognitionTimeout_) {
            if (!CloseVoiceRecognitionByHf(address)) {
                LOG_ERROR("[HFP AG]%{public}s():Failed to close the voice recognition!", __FUNCTION__);
            }
        }
    }
}

int HfpAgService::GetConnectedDeviceNum() const
{
    int size = 0;
    for (auto iter = stateMachines_.begin(); iter != stateMachines_.end(); ++iter) {
        if (iter->second != nullptr) {
            auto connectionState = iter->second->GetStateInt();
            if ((connectionState == HFP_AG_STATE_CONNECTING) || (connectionState >= HFP_AG_STATE_CONNECTED)) {
                size++;
            }
        }
    }
    return size;
}

int HfpAgService::GetMaxConnectionDevicesNum() const
{
    int number = MAX_DEFAULT_CONNECTIONS_NUM;
    if (!AdapterConfig::GetInstance()->GetValue(SECTION_HFP_AG_SERVICE, PROPERTY_MAX_CONNECTED_DEVICES, number)) {
        LOG_INFO("[HFP HF]%{public}s():It's failed to get the max connection number", __FUNCTION__);
    }
    return number;
}

void HfpAgService::NotifyRegistrationStatusChanged(int status)
{
    HfpAgSystemInterface::GetInstance().OnSubscriptionStateChanged(status);
}

void HfpAgService::NotifySignalStrengthChanged(int signalStrength)
{
    HfpAgSystemInterface::GetInstance().OnSignalStrengthChanged(signalStrength);
}

void HfpAgService::NotifyRoamingStatusChanged(int status)
{
    HfpAgSystemInterface::GetInstance().OnRoamStateChanged(status);
}

void HfpAgService::NotifyBatteryLevelChanged(int batteryLevel)
{
    HfpAgSystemInterface::GetInstance().OnBatteryLevel(batteryLevel);
}

void HfpAgService::NotifyVolumeChanged(int type, int volume)
{
    HfpAgMessage event(HFP_AG_SET_VOLUME_EVT, type);
    event.arg3_ = volume;
    PostEvent(event);
}

void HfpAgService::ProcessConnectEvent(const HfpAgMessage &event)
{
    if (GetConnectedDeviceNum() < maxConnectedNum_) {
        auto it = stateMachines_.find(event.dev_);
        if (it != stateMachines_.end() && it->second != nullptr && it->second->IsRemoving()) {
            // peer device may send connect request before we remove statemachine for last connection.
            // so post this connect request, process it after we remove statemachine completely.
            PostEvent(event);
        } else if (it == stateMachines_.end() || it->second == nullptr) {
            stateMachines_[event.dev_] = std::make_unique<HfpAgStateMachine>(event.dev_);
            stateMachines_[event.dev_]->Init();
            stateMachines_[event.dev_]->ProcessMessage(event);
        } else {
            it->second->ProcessMessage(event);
        }
    }
}

void HfpAgService::ProcessRemoveStateMachine(const std::string &address)
{
    stateMachines_.insert_or_assign(address, nullptr);
    if (isShuttingDown_) {
        ShutDownDone(false);
    }
}

void HfpAgService::ProcessDefaultEvent(const HfpAgMessage &event) const
{
    auto it = stateMachines_.find(event.dev_);
    if ((it != stateMachines_.end()) && (it->second != nullptr)) {
        it->second->ProcessMessage(event);
    } else {
        HILOGI("addr: %{public}s", GetEncryptAddr(event.dev_).c_str());
    }
}

void HfpAgService::ProcessVoiceRecognitionTimeOut(const HfpAgMessage &event)
{
    if (voiceRecognitionTimeout_ != nullptr) {
        voiceRecognitionTimeout_ = nullptr;
    }

    SendEventToEachStateMachine(event);
}

void HfpAgService::ProcessResponseClccTimeOut(const HfpAgMessage &event)
{
    if (ResponseClccTimeout_ != nullptr) {
        if (queryClccAddress_ != event.dev_) {
            return;
        } else {
            queryClccAddress_ = NULL_ADDRESS;
            ResponseClccTimeout_ = nullptr;
        }
    }

    SendEventToEachStateMachine(event);
}

void HfpAgService::ProcessDialOutTimeOut()
{
    if (dialingOutTimeout_ != nullptr) {
        dialingOutTimeout_ = nullptr;
    }

    HfpAgMessage event(HFP_AG_DIALING_OUT_RESULT, HFP_AG_RESULT_ERROR);
    PostEvent(event);
}

bool HfpAgService::IsVoiceRecognitionAvailable(const std::string &address) const
{
    if (IsConnected(address) == false) {
        return false;
    }

    if (IsActiveDevice(address) == false) {
        return false;
    }

    if (IsAudioIdle() == false) {
        LOG_INFO("[HFP AG]%{public}s():sco is not idle!", __FUNCTION__);
        return false;
    }
    return true;
}

void HfpAgService::SetInbandRing(bool action)
{
    std::lock_guard<std::recursive_mutex> lk(mutex_);
    isInbandRinging_ = action;
    HfpAgMessage curEvent(HFP_AG_SET_INBAND_RING_TONE_EVT, action);
    PostEvent(curEvent);
}

void HfpAgService::ResponesOK(const std::string &address)
{
    HfpAgMessage event(HFP_AG_RESPONE_OK_EVT);
    event.dev_ = address;
    PostEvent(event);
}

bool HfpAgService::IsConnected(const std::string &address) const
{
    auto it = stateMachines_.find(address);
    if (it == stateMachines_.end() || it->second == nullptr) {
        LOG_ERROR("[HFP AG]%{public}s():Invalid Device address:%{public}s", __FUNCTION__, address.c_str());
        return false;
    }
    if (it->second->GetStateInt() < HFP_AG_STATE_CONNECTED) {
        LOG_ERROR("[HFP AG]%{public}s():Device not connected! address:%{public}s, state[%{public}d]",
            __FUNCTION__,
            address.c_str(),
            it->second->GetStateInt());
        return false;
    }
    return true;
}

bool HfpAgService::IsActiveDevice(const std::string &address) const
{
    std::string activeDevice = HfpAgProfile::GetActiveDevice();
    if (activeDevice == address) {
        return true;
    } else {
        LOG_INFO("[HFP AG]%{public}s():Current Device is not active device, address:%{public}s, activeDevice:%{public}s",
            __FUNCTION__, address.c_str(), activeDevice.c_str());
        return false;
    }
}

void HfpAgService::ModifyActiveDevice(const std::string &newAddress)
{
    std::string preActiveDevice = HfpAgProfile::GetActiveDevice();
    LOG_INFO("[HFP AG]%{public}s():preActiveDevice address:%{public}s, activeDevice newAddress:%{public}s",
        __FUNCTION__, preActiveDevice.c_str(), newAddress.c_str());

    HfpAgProfile::SetActiveDevice(newAddress);
    if (IsIncall()) {
        DisconnectSingleSco(preActiveDevice);
        HfpAgMessage event(HFP_AG_CONNECT_AUDIO_EVT);
        event.dev_ = newAddress;
        PostEvent(event);
    } else if (isVrOpened_) {
        SendCloseVoiceEvent(preActiveDevice);
    }

    NotifyCurrentActiveDevice(newAddress);
}

REGISTER_CLASS_CREATOR(HfpAgService);
}  // namespace bluetooth
}  // namespace OHOS
