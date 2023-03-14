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

#include "hfp_hf_statemachine.h"

#include "hfp_hf_data_connection_server.h"
#include "hfp_hf_service.h"
#include "power_manager.h"
#include "audio_system_manager.h"

namespace OHOS {
namespace bluetooth {
HfpHfStateMachine::HfpHfStateMachine(const std::string &address)
    : address_(address), profile_(address)
{}

void HfpHfStateMachine::Init()
{
    profile_.Init();
    calls_ = std::make_unique<HfpHfCallManager>(address_);
    connTimer_ = std::make_unique<utility::Timer>(std::bind(&bluetooth::HfpHfStateMachine::ConnectionTimeout, this));

    std::unique_ptr<utility::StateMachine::State> disconnectedState =
        std::make_unique<HfpHfDisconnected>(DISCONNECTED, *this, profile_);
    std::unique_ptr<utility::StateMachine::State> connectingState =
        std::make_unique<HfpHfConnecting>(CONNECTING, *this, profile_);
    std::unique_ptr<utility::StateMachine::State> disconnectingState =
        std::make_unique<HfpHfDisconnecting>(DISCONNECTING, *this, profile_);
    std::unique_ptr<utility::StateMachine::State> connectedState =
        std::make_unique<HfpHfConnected>(CONNECTED, *this, profile_);
    std::unique_ptr<utility::StateMachine::State> audioConnecting =
        std::make_unique<HfpHfAudioConnecting>(AUDIO_CONNECTING, *this, profile_, *connectedState);
    std::unique_ptr<utility::StateMachine::State> audioDisconnecting =
        std::make_unique<HfpHfAudioDisconnecting>(AUDIO_DISCONNECTING, *this, profile_, *connectedState);
    std::unique_ptr<utility::StateMachine::State> audioConnected =
        std::make_unique<HfpHfAudioConnected>(AUDIO_CONNECTED, *this, profile_, *connectedState);

    Move(disconnectedState);
    Move(connectingState);
    Move(disconnectingState);
    Move(audioConnecting);
    Move(audioDisconnecting);
    Move(audioConnected);
    Move(connectedState);   // Add parenet state at last

    InitState(DISCONNECTED);
}

void HfpHfDisconnected::Entry()
{
    stateMachine_.ProcessDeferredMessage();

    if (isReentry_) {
        stateMachine_.SetRemoving(true);
        profile_.RemoveStateMachine();
        stateMachine_.NotifyStateTransitions();
    }
}

void HfpHfDisconnected::Exit()
{
    isReentry_ = true;
}

bool HfpHfDisconnected::Dispatch(const utility::Message &msg)
{
    HfpHfMessage &event = (HfpHfMessage &)msg;
    LOG_DEBUG("[HFP HF]%{public}s():[Disconnected][%{public}s]", __FUNCTION__, HfpHfStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case HFP_HF_CONNECT_EVT:
            profile_.DoServiceDiscovery(HFP_HF_INITIATOR);
            Transition(HfpHfStateMachine::CONNECTING);
            break;
        case HFP_HF_CONNECT_REQUEST_EVT:
            profile_.AcceptDataConnection(event.arg1_);
            Transition(HfpHfStateMachine::CONNECTING);
            break;
        case HFP_HF_AUDIO_CONNECT_REQUEST_EVT:
            profile_.RejectAudioConnection();
            break;
        default:
            break;
    }
    return true;
}

void HfpHfConnecting::Entry()
{
    stateMachine_.ProcessDeferredMessage();
    stateMachine_.NotifyStateTransitions();
    stateMachine_.StartConnectionTimer();
}
void HfpHfConnecting::Exit()
{
    stateMachine_.StopConnectionTimer();
}

bool HfpHfConnecting::Dispatch(const utility::Message &msg)
{
    HfpHfMessage &event = (HfpHfMessage &)msg;
    LOG_DEBUG("[HFP HF]%{public}s():[Connecting][%{public}s]", __FUNCTION__, HfpHfStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case HFP_HF_SDP_DISCOVERY_RESULT_SUCCESS:
            if (profile_.ServiceDiscoveryResult() != HFP_HF_SUCCESS) {
                Transition(HfpHfStateMachine::DISCONNECTED);
            }
            break;
        case HFP_HF_SDP_DISCOVERY_RESULT_FAIL:
            Transition(HfpHfStateMachine::DISCONNECTED);
            break;
        case HFP_HF_CONNECT_EVT:
        case HFP_HF_DISCONNECT_EVT:
            stateMachine_.AddDeferredMessage(event);
            break;
        case HFP_HF_DATA_AVAILABLE_EVT:
            profile_.ReadData();
            break;
        case HFP_HF_CONNECTED_EVT:
            profile_.EstablishServiceLevelConnection();
            break;
        case HFP_HF_DISCONNECTED_EVT:
            profile_.RemoveRemoteScnLoging();
            Transition(HfpHfStateMachine::DISCONNECTED);
            break;
        case HFP_HF_AUDIO_CONNECT_REQUEST_EVT:
            profile_.ProcessAudioConnectRequest();
            break;
        case HFP_HF_AUDIO_CONNECTED_EVT:
        case HFP_HF_AUDIO_DISCONNECTED_EVT:
            stateMachine_.AddDeferredMessage(event);
            break;
        case HFP_HF_CONNECT_FAILED_EVT:
            profile_.RemoveRemoteScnLoging();
            Transition(HfpHfStateMachine::DISCONNECTED);
            break;
        case HFP_HF_SLC_ESTABLISHED_EVT:
            profile_.ProcessSlcEstablished();
            Transition(HfpHfStateMachine::CONNECTED);
            break;
        case HFP_HF_BATTERY_LEVEL_CHANGED_EVT:
            profile_.ReportBatteryLevel(event.arg1_);
            break;
        case HFP_HF_ENHANCED_DRIVER_SAFETY_CHANGED_EVT:
            profile_.ReportDriverSafety(event.arg1_);
            break;
        case HFP_HF_INTERACTIVE_EVT:
            stateMachine_.AddDeferredMessage(event);
            break;
        case HFP_HF_CONNECTION_TIMEOUT_EVT:
            Transition(HfpHfStateMachine::DISCONNECTED);
            break;
        default:
            break;
    }
    return true;
}

void HfpHfDisconnecting::Entry()
{
    stateMachine_.ProcessDeferredMessage();
    stateMachine_.NotifyStateTransitions();
    stateMachine_.StartConnectionTimer();
}

void HfpHfDisconnecting::Exit()
{
    stateMachine_.StopConnectionTimer();
}

bool HfpHfDisconnecting::Dispatch(const utility::Message &msg)
{
    HfpHfMessage &event = (HfpHfMessage &)msg;
    LOG_DEBUG("[HFP HF]%{public}s():[Disconnecting][%{public}s]", __FUNCTION__, HfpHfStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case HFP_HF_CONNECT_EVT:
            stateMachine_.AddDeferredMessage(event);
            break;
        case HFP_HF_DISCONNECTED_EVT:
            IPowerManager::GetInstance().StatusUpdate(
                RequestStatus::CONNECT_OFF, PROFILE_NAME_HFP_HF, RawAddress(event.dev_));
            profile_.RemoveRemoteScnLoging();
            Transition(HfpHfStateMachine::DISCONNECTED);
            break;
        case HFP_HF_DISCONNECT_FAILED_EVT:
        case HFP_HF_CONNECTION_TIMEOUT_EVT:
            Transition(HfpHfStateMachine::CONNECTED);
            break;
        case HFP_HF_AUDIO_CONNECT_REQUEST_EVT:
            profile_.RejectAudioConnection();
            break;
        default:
            break;
    }
    return true;
}

void HfpHfConnected::Entry()
{
    stateMachine_.ProcessDeferredMessage();
    stateMachine_.NotifyStateTransitions();
}

bool HfpHfConnected::Dispatch(const utility::Message &msg)
{
    HfpHfMessage &event = (HfpHfMessage &)msg;
    LOG_DEBUG("[HFP HF]%{public}s():[Connected][%{public}s]", __FUNCTION__, HfpHfStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case HFP_HF_CONNECT_AUDIO_EVT:
        case HFP_HF_RETRY_CONNECT_AUDIO_EVT:
            profile_.SetupCodecConnection();
            Transition(HfpHfStateMachine::AUDIO_CONNECTING);
            break;
        case HFP_HF_AUDIO_CONNECT_REQUEST_EVT:
            profile_.ProcessAudioConnectRequest();
            Transition(HfpHfStateMachine::AUDIO_CONNECTING);
            break;
        case HFP_HF_AUDIO_CONNECTING_EVT:
            Transition(HfpHfStateMachine::AUDIO_CONNECTING);
            break;
        case HFP_HF_AUDIO_CONNECTED_EVT:
            IPowerManager::GetInstance().StatusUpdate(
                RequestStatus::SCO_ON, PROFILE_NAME_HFP_HF, RawAddress(event.dev_));
            Transition(HfpHfStateMachine::AUDIO_CONNECTED);
            break;
        case HFP_HF_DISCONNECT_EVT:
            profile_.ReleaseDataConnection();
            Transition(HfpHfStateMachine::DISCONNECTING);
            break;
        case HFP_HF_DISCONNECTED_EVT:
            IPowerManager::GetInstance().StatusUpdate(
                RequestStatus::CONNECT_OFF, PROFILE_NAME_HFP_HF, RawAddress(event.dev_));
            profile_.RemoveRemoteScnLoging();
            Transition(HfpHfStateMachine::DISCONNECTED);
            break;
        case HFP_HF_DATA_AVAILABLE_EVT:
            profile_.ReadData();
            break;
        case HFP_HF_INTERACTIVE_EVT:
            stateMachine_.ProcessInteractiveEvent(event);
            break;
        case HFP_HF_SEND_DTMF_EVT:
            profile_.SendDtmf(event.arg1_);
            break;
        case HFP_HF_SET_VOLUME_EVT:
            profile_.SetHfVolume(event.arg1_, event.arg3_);
            break;
        case HFP_HF_ACCEPT_CALL_EVT:
            stateMachine_.ProcessAcceptCall(event.arg1_);
            break;
        case HFP_HF_HOLD_CALL_EVT:
            stateMachine_.ProcessHoldCall();
            break;
        case HFP_HF_REJECT_CALL_EVT:
            stateMachine_.ProcessRejectCall();
            break;
        case HFP_HF_SEND_KEY_PRESSED:
            stateMachine_.PrecessSendKeyPressed();
            break;
        case HFP_HF_HANDLE_INCOMING_CALL_EVT:
            stateMachine_.ProcessHandleIncomingCall(event.arg1_);
            break;
        case HFP_HF_HANDLE_MULTI_CALL_EVT:
            stateMachine_.ProcessHandleMultiCall(event.arg1_, event.arg3_);
            break;
        case HFP_HF_DIAL_LAST_NUMBER:
            stateMachine_.ProcessDialLastNumber();
            break;
        case HFP_HF_DIAL_MEMORY:
            stateMachine_.ProcessDialMemory(event.arg1_);
            break;
        case HFP_HF_SEND_VOICE_TAG:
            stateMachine_.ProcessSendVoiceTag(event.arg1_);
            break;
        case HFP_HF_FINISH_CALL_EVT:
            stateMachine_.ProcessFinishCall(event);
            break;
        case HFP_HF_DIAL_CALL_EVT:
            stateMachine_.ProcessDialCall(event);
            break;
        case HFP_HF_OPEN_VOICE_RECOGNITION_EVT:
            stateMachine_.ProcessOpenVoiceRecognition();
            break;
        case HFP_HF_CLOSE_VOICE_RECOGNITION_EVT:
            stateMachine_.ProcessCloseVoiceRecognition();
            break;
        case HFP_HF_BATTERY_LEVEL_CHANGED_EVT:
            profile_.ReportBatteryLevel(event.arg1_);
            break;
        case HFP_HF_ENHANCED_DRIVER_SAFETY_CHANGED_EVT:
            profile_.ReportDriverSafety(event.arg1_);
            break;
        case HFP_HF_SEND_AT_COMMAND_EVT:
            profile_.SendAtCommand(event.arg1_, event.str_);
            break;
        default:
            break;
    }
    return true;
}

void HfpHfAudioConnecting::Entry()
{
    stateMachine_.ProcessDeferredMessage();
    stateMachine_.NotifyStateTransitions();
    stateMachine_.StartConnectionTimer();
}

void HfpHfAudioConnecting::Exit()
{
    stateMachine_.StopConnectionTimer();
}

bool HfpHfAudioConnecting::Dispatch(const utility::Message &msg)
{
    HfpHfMessage &event = (HfpHfMessage &)msg;
    LOG_DEBUG("[HFP HF]%{public}s():[AudioConnecting][%{public}s]", __FUNCTION__,
        HfpHfStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case HFP_HF_CONNECT_AUDIO_EVT:
        case HFP_HF_AUDIO_CONNECTING_EVT:
            break;
        case HFP_HF_AUDIO_CONNECT_REQUEST_EVT:
            profile_.ProcessAudioConnReqInAudioConnecting();
            break;
        case HFP_HF_DISCONNECT_AUDIO_EVT:
        case HFP_HF_DISCONNECT_EVT:
            stateMachine_.AddDeferredMessage(event);
            break;
        case HFP_HF_AUDIO_CONNECTED_EVT:
            IPowerManager::GetInstance().StatusUpdate(
                RequestStatus::SCO_ON, PROFILE_NAME_HFP_HF, RawAddress(event.dev_));
            profile_.RestorCodecSetupRole();
            Transition(HfpHfStateMachine::AUDIO_CONNECTED);
            break;
        case HFP_HF_AUDIO_CONNECT_FAILED_EVT:
        case HFP_HF_CONNECTION_TIMEOUT_EVT:
        case HFP_HF_AUDIO_DISCONNECTED_EVT:
            profile_.RestorCodecSetupRole();
            stateMachine_.NotifyChildStateToParentState(HFP_HF_AUDIO_STATE_CONNECTING, HFP_HF_AUDIO_STATE_DISCONNECTED);
            Transition(HfpHfStateMachine::CONNECTED);
            break;
        case HFP_HF_DISCONNECTED_EVT:
            stateMachine_.AddDeferredMessage(event);
            stateMachine_.NotifyChildStateToParentState(HFP_HF_AUDIO_STATE_CONNECTING, HFP_HF_AUDIO_STATE_DISCONNECTED);
            Transition(HfpHfStateMachine::CONNECTED);
            break;
        default:
            return false;
    }
    return true;
}

void HfpHfAudioDisconnecting::Entry()
{
    stateMachine_.NotifyStateTransitions();
    stateMachine_.StartConnectionTimer();
}

void HfpHfAudioDisconnecting::Exit()
{
    stateMachine_.StopConnectionTimer();
}

bool HfpHfAudioDisconnecting::Dispatch(const utility::Message &msg)
{
    HfpHfMessage &event = (HfpHfMessage &)msg;
    LOG_DEBUG("[HFP HF]%{public}s():[AudioConnected][%{public}s]", __FUNCTION__,
        HfpHfStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case HFP_HF_DISCONNECT_EVT:
        case HFP_HF_CONNECT_AUDIO_EVT:
        case HFP_HF_AUDIO_CONNECT_REQUEST_EVT:
        case HFP_HF_DISCONNECTED_EVT:
            stateMachine_.AddDeferredMessage(event);
            break;
        case HFP_HF_AUDIO_CONNECTING_EVT:
        case HFP_HF_DISCONNECT_AUDIO_EVT:
        case HFP_HF_AUDIO_DISCONNECTING_EVT:
        case HFP_HF_AUDIO_CONNECTED_EVT:
        case HFP_HF_AUDIO_CONNECT_FAILED_EVT:
            break;
        case HFP_HF_AUDIO_DISCONNECTED_EVT:
            stateMachine_.ProcessAudioDisconnected();
            Transition(HfpHfStateMachine::CONNECTED);
            break;
        case HFP_HF_AUDIO_DISCONNECT_FAILED_EVT:
        case HFP_HF_CONNECTION_TIMEOUT_EVT:
            stateMachine_.NotifyChildStateToParentState(HFP_HF_AUDIO_STATE_DISCONNECTING, HFP_HF_AUDIO_STATE_CONNECTED);
            Transition(HfpHfStateMachine::AUDIO_CONNECTED);
            break;
        default:
            return false;
    }
    return true;
}

void HfpHfAudioConnected::Entry()
{
    stateMachine_.ProcessDeferredMessage();
    stateMachine_.NotifyStateTransitions();
}

bool HfpHfAudioConnected::Dispatch(const utility::Message &msg)
{
    HfpHfMessage &event = (HfpHfMessage &)msg;
    LOG_DEBUG("[HFP HF]%{public}s():[AudioConnected][%{public}s]", __FUNCTION__,
        HfpHfStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case HFP_HF_DISCONNECT_EVT:
            ProcessDisconnect(event);
            break;
        case HFP_HF_DISCONNECTED_EVT:
            stateMachine_.AddDeferredMessage(event);
            break;
        case HFP_HF_CONNECT_AUDIO_EVT:
        case HFP_HF_AUDIO_CONNECT_REQUEST_EVT:
        case HFP_HF_AUDIO_CONNECTING_EVT:
        case HFP_HF_AUDIO_CONNECTED_EVT:
            break;
        case HFP_HF_AUDIO_DISCONNECTING_EVT:
            Transition(HfpHfStateMachine::AUDIO_DISCONNECTING);
            break;
        case HFP_HF_DISCONNECT_AUDIO_EVT:
            profile_.ReleaseAudioConnection();
            break;
        case HFP_HF_AUDIO_DISCONNECTED_EVT:
            stateMachine_.ProcessAudioDisconnected();
            Transition(HfpHfStateMachine::CONNECTED);
            break;
        default:
            return false;
    }
    return true;
}

void HfpHfAudioConnected::ProcessDisconnect(const HfpHfMessage &event)
{
    profile_.ReleaseAudioConnection();
    stateMachine_.routeHfAudio(false);
    stateMachine_.AddDeferredMessage(event);
}

void HfpHfStateMachine::ProcessFinishCall(const HfpHfMessage &event)
{
    if ((calls_->GetCallByState(HFP_HF_CALL_STATE_ACTIVE) != nullptr) ||
        (calls_->GetCallByState(HFP_HF_CALL_STATE_DIALING) != nullptr) ||
        (calls_->GetCallByState(HFP_HF_CALL_STATE_ALERTING) != nullptr)) {
        LOG_DEBUG("[HFP HF]%{public}s():Finish active or dialing or alerting call", __FUNCTION__);
        if (!profile_.SendChup()) {
            LOG_ERROR("[HFP HF]%{public}s():Send CHUP failed", __FUNCTION__);
        }
    } else if (calls_->GetCallByState(HFP_HF_CALL_STATE_HELD) != nullptr) {
        LOG_DEBUG("[HFP HF]%{public}s():Finish hold call", __FUNCTION__);
        if (!profile_.SendChld(HFP_HF_CHLD_ACTION_0)) {
            LOG_ERROR("[HFP HF]%{public}s():Send CHLD 0 failed", __FUNCTION__);
        }
    } else {
        LOG_DEBUG("[HFP HF]%{public}s():no call to finish", __FUNCTION__);
    }
    return;
}

void HfpHfStateMachine::ProcessDialCall(const HfpHfMessage &event)
{
    profile_.DialOutCall(event.calls_.GetNumber());
}

void HfpHfStateMachine::ProcessOpenVoiceRecognition()
{
    if (GetOpenVoiceRecognitionState() == HFP_HF_VR_STATE_CLOSED) {
        if (!profile_.OpenVoiceRecognition()) {
            LOG_ERROR("[HFP HF]%{public}s():can't open the voice recognition!", __FUNCTION__);
        }
    }
}

void HfpHfStateMachine::ProcessCloseVoiceRecognition()
{
    if (GetOpenVoiceRecognitionState() == HFP_HF_VR_STATE_OPENED) {
        if (!profile_.CloseVoiceRecognition()) {
            LOG_ERROR("[HFP HF]%{public}s():can't close the voice recognition!", __FUNCTION__);
        }
    }
}

void HfpHfStateMachine::ProcessRejectCall()
{
    if (calls_->GetCallByState(HFP_HF_CALL_STATE_INCOMING) != nullptr) {
        LOG_DEBUG("[HFP HF]%{public}s():Reject incoming call", __FUNCTION__);
        if (!profile_.SendChup()) {
            LOG_ERROR("[HFP HF]%{public}s():Send CHUP failed", __FUNCTION__);
        }
    } else if ((calls_->GetCallByState(HFP_HF_CALL_STATE_HELD) != nullptr) ||
               (calls_->GetCallByState(HFP_HF_CALL_STATE_WAITING) != nullptr)) {
        LOG_DEBUG("[HFP HF]%{public}s():Reject held or waiting call", __FUNCTION__);
        if (!profile_.SendChld(HFP_HF_CHLD_ACTION_0)) {
            LOG_ERROR("[HFP HF]%{public}s():Send CHLD 0 failed", __FUNCTION__);
        }
    } else {
        LOG_DEBUG("[HFP HF]%{public}s():No call to reject", __FUNCTION__);
    }
    return;
}

void HfpHfStateMachine::PrecessSendKeyPressed()
{
    LOG_INFO("[HFP HF]%{public}s():enter", __FUNCTION__);
    if (!profile_.SendKeyPressed()) {
        LOG_ERROR("[HFP HF]%{public}s():PrecessSendKeyPressed failed", __FUNCTION__);
    }
}

void HfpHfStateMachine::ProcessHandleIncomingCall(int flag)
{
    HILOGI("[HFP HF]:handle incoming call flag = %{public}d", flag);
    if (calls_->GetCallByState(HFP_HF_CALL_STATE_INCOMING) != nullptr) {
        if (flag != static_cast<int>(HfpHfHandleIncomingCalAction::HFP_HF_HOLD_INCOMING_ACTION)) {
            HILOGE("[HFP HF]:No incoming call hold");
        }
        if (profile_.SendBtrh(flag)) {
            HILOGE("[HFP HF]:Send BTRH = %{public}d failed", flag);
        }
    } else if (calls_->GetCallByState(HFP_HF_CALL_STATE_RESPONSE_HELD) != nullptr) {
        if (!profile_.SendBtrh(flag)) {
            HILOGE("[HFP HF]:Send BTRH = %{public}d failed", flag);
        }
    } else {
        HILOGI("[HFP HF]:No call handle");
    }
    return;
}

void HfpHfStateMachine::ProcessHandleMultiCall(int flag, int index)
{
    HILOGI("[HFP HF]:enter flag = %{public}d, index = %{public}d", flag, index);
    if (!profile_.SendChld(flag, index)) {
            HILOGE("[HFP HF]:Send CHLD failed");
    }
}

void HfpHfStateMachine::ProcessDialLastNumber()
{
    HILOGI("[HFP HF]:enter");
    if (!profile_.CallLastDialedNumber()) {
            HILOGE("[HFP HF]:ProcessDialLastNumber failed");
    }
}

void HfpHfStateMachine::ProcessDialMemory(int index)
{
    HILOGI("[HFP HF]:enter index = %{public}d", index);
    if (!profile_.DialMemory(index)) {
            HILOGE("[HFP HF]:ProcessDialMemory failed");
    }
}

void HfpHfStateMachine::ProcessSendVoiceTag(int index)
{
    LOG_INFO("[HFP HF]%{public}s():enter index = %{public}d", __FUNCTION__, index);
    if (!profile_.SendVoiceTag(index)) {
            LOG_ERROR("[HFP HF]%{public}s():ProcessSendVoiceTag failed", __FUNCTION__);
    }
}

void HfpHfStateMachine::ProcessHoldCall()
{
    if (calls_->GetCallByState(HFP_HF_CALL_STATE_ACTIVE) != nullptr) {
        LOG_DEBUG("[HFP HF]%{public}s():Hold active call", __FUNCTION__);
        if (!profile_.SendChld(HFP_HF_CHLD_ACTION_2)) {
            LOG_ERROR("[HFP HF]%{public}s():Send CHLD 2 failed", __FUNCTION__);
        }
    } else {
        LOG_DEBUG("[HFP HF]%{public}s():No call to hold", __FUNCTION__);
    }
    return;
}

void HfpHfStateMachine::ProcessAcceptCall(int flag)
{
    if (calls_->GetCallByState(HFP_HF_CALL_STATE_INCOMING) != nullptr) {
        if (flag != HFP_HF_ACCEPT_CALL_ACTION_NONE) {
            return;
        }

        LOG_DEBUG("[HFP HF]%{public}s():Accept incoming call", __FUNCTION__);
        if (!profile_.SendAta()) {
            LOG_ERROR("[HFP HF]%{public}s():Send ATA failed", __FUNCTION__);
        }
    } else if (calls_->GetCallByState(HFP_HF_CALL_STATE_WAITING) != nullptr) {
        if ((calls_->GetCallByState(HFP_HF_CALL_STATE_ACTIVE) == nullptr) && (flag != HFP_HF_ACCEPT_CALL_ACTION_NONE)) {
            return;
        }

        int action;
        if ((flag == HFP_HF_ACCEPT_CALL_ACTION_NONE) || (flag == HFP_HF_ACCEPT_CALL_ACTION_HOLD)) {
            action = HFP_HF_CHLD_ACTION_2;
        } else if (flag == HFP_HF_ACCEPT_CALL_ACTION_FINISH) {
            action = HFP_HF_CHLD_ACTION_1;
        } else {
            LOG_ERROR("[HFP HF]%{public}s():Invalid flag[%{public}d]", __FUNCTION__, flag);
            return;
        }

        LOG_DEBUG("[HFP HF]%{public}s():Accept waiting call", __FUNCTION__);
        if (!profile_.SendChld(action)) {
            LOG_ERROR("[HFP HF]%{public}s():Send CHLD[%{public}d] failed", __FUNCTION__, action);
        }
    } else if (calls_->GetCallByState(HFP_HF_CALL_STATE_HELD) != nullptr) {
        int action = HFP_HF_CHLD_ACTION_2;
        if (flag == HFP_HF_ACCEPT_CALL_ACTION_FINISH) {
            action = HFP_HF_CHLD_ACTION_1;
        } else if ((flag == HFP_HF_ACCEPT_CALL_ACTION_NONE) || (flag == HFP_HF_ACCEPT_CALL_ACTION_HOLD)) {
            action = HFP_HF_CHLD_ACTION_2;
        } else if (calls_->GetCallByState(HFP_HF_CALL_STATE_ACTIVE) != nullptr) {
            action = HFP_HF_CHLD_ACTION_3;
        }

        LOG_DEBUG("[HFP HF]%{public}s():Accept held call", __FUNCTION__);
        if (!profile_.SendChld(action)) {
            LOG_ERROR("[HFP HF]%{public}s():Send CHLD[%{public}d] failed", __FUNCTION__, action);
        }
    } else {
        LOG_DEBUG("[HFP HF]%{public}s():No call to accept", __FUNCTION__);
    }

    if (flag == HFP_HF_ACCEPT_CALL_ACTION_HOLD) {
        routeHfAudio(true);
    }
    return;
}

void HfpHfStateMachine::ProcessInteractiveEvent(const HfpHfMessage &event)
{
    LOG_DEBUG("[HFP HF]%{public}s():[%{public}s]", __FUNCTION__, GetInteractiveEventType(event.type_).c_str());
    switch (event.type_) {
        case HFP_HF_TYPE_NETWORK_STATE:
            ProcessNetworkStateEvent(event);
            break;
        case HFP_HF_TYPE_NETWORK_ROAM:
            ProcessNetworkRoamEvent(event);
            break;
        case HFP_HF_TYPE_NETWORK_SIGNAL:
            ProcessNetworkSignalEvent(event);
            break;
        case HFP_HF_TYPE_BATTERY_LEVEL:
            ProcessBatteryLevelEvent(event);
            break;
        case HFP_HF_TYPE_CURRENT_OPERATOR:
            ProcessOperatorEvent(event);
            break;
        case HFP_HF_TYPE_CALL_STATE:
        case HFP_HF_TYPE_CALL_SETUP_STATE:
        case HFP_HF_TYPE_CALL_HELD_STATE:
        case HFP_HF_TYPE_HOLD_RESULT:
        case HFP_HF_TYPE_CALLING_LINE_IDENTIFICATION:
        case HFP_HF_TYPE_CALL_WAITING:
            ProcessCallIndicatorEvent(event);
            break;
        case HFP_HF_TYPE_CURRENT_CALLS:
            ProcessCurrentCallEvent(event);
            break;
        case HFP_HF_TYPE_SET_VOLUME:
            ProcessSetVolumeEvent(event);
            break;
        case HFP_HF_TYPE_SUBSCRIBER_NUMBER:
            ProcessSubscriberNumberEvent(event);
            break;
        case HFP_HF_TYPE_INBAND_RING:
            ProcessInbandRingEvent(event);
            break;
        case HFP_HF_TYPE_QUERY_CURRENT_CALLS_DONE:
            calls_->UpdateCallDone();
            break;
        case HFP_HF_TYPE_VOICE_RECOGNITION_CHANGED:
            ProcessVrChangedEvent(event);
            break;
        case HFP_HF_TYPE_OPEN_VR_RESULT:
            ProcessOpenVrResultEvent(event);
            break;
        case HFP_HF_TYPE_CLOSE_VR_RESULT:
            ProcessCloseVrResultEvent(event);
            break;
        default:
            break;
    }
}

void HfpHfStateMachine::NotifyStateTransitions()
{
    HfpHfService *service = HfpHfService::GetService();
    int toState = GetDeviceStateInt();
    if (service != nullptr) {
        RawAddress device(address_);
        if ((preState_ != toState) && (preState_ <= HFP_HF_STATE_CONNECTED) && (toState <= HFP_HF_STATE_CONNECTED)) {
            service->NotifyStateChanged(device, toState);
        }

        if ((preState_ != toState) && (preState_ >= HFP_HF_AUDIO_STATE_DISCONNECTED) &&
            (toState >= HFP_HF_AUDIO_STATE_DISCONNECTED)) {
            service->NotifyScoStateChanged(device, toState);
        }
    }

    if (toState == HFP_HF_AUDIO_STATE_CONNECTED) {
        SyncScoEvents(toState);
    }

    preState_ = toState;
}

void HfpHfStateMachine::NotifyChildStateToParentState(int fromState, int toState)
{
    HfpHfService *service = HfpHfService::GetService();
    if (service != nullptr) {
        if ((fromState != toState) && (fromState >= HFP_HF_AUDIO_STATE_DISCONNECTED) &&
            (toState >= HFP_HF_AUDIO_STATE_DISCONNECTED)) {
            RawAddress device(address_);
            service->NotifyScoStateChanged(device, toState);
        }
    }

    if (toState == HFP_HF_AUDIO_STATE_DISCONNECTED) {
        SyncScoEvents(toState);
    }

    preState_ = toState;
    ProcessDeferredMessage();
}

void HfpHfStateMachine::ProcessAudioDisconnected()
{
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::SCO_OFF, PROFILE_NAME_HFP_HF, RawAddress(address_));
    NotifyChildStateToParentState(HFP_HF_AUDIO_STATE_CONNECTED, HFP_HF_AUDIO_STATE_DISCONNECTED);
}

void HfpHfStateMachine::SyncScoEvents(int state)
{
    if (state == HFP_HF_AUDIO_STATE_CONNECTED) {
        int spkVolume = OHOS::AudioStandard::AudioSystemManager::GetInstance()->GetVolume(
            OHOS::AudioStandard::STREAM_VOICE_CALL);
        int micVolume = OHOS::AudioStandard::AudioSystemManager::GetInstance()->GetVolume(
            OHOS::AudioStandard::STREAM_VOICE_CALL);
        profile_.SetHfVolume(spkVolume, HFP_HF_VOLUME_TYPE_SPK);
        profile_.SetHfVolume(micVolume, HFP_HF_VOLUME_TYPE_MIC);

        routeHfAudio(true);
        OHOS::AudioStandard::AudioSystemManager::GetInstance()->SetAudioParameter(
            "hfp_volume", std::to_string(spkVolume));
    }

    if (state == HFP_HF_AUDIO_STATE_DISCONNECTED) {
        routeHfAudio(false);
        if (voiceRecognitionStatus_ == HFP_HF_VR_STATE_OPENED) {
            NotifyVoiceRecognitionStatusChanged(HFP_HF_VR_STATE_CLOSED);
        }
    }
}

int HfpHfStateMachine::GetOpenVoiceRecognitionState() const
{
    return voiceRecognitionStatus_;
}

bool HfpHfStateMachine::GetIsInbandRing() const
{
    return isInBandRing_;
}

int HfpHfStateMachine::GetVolume() const
{
    return volume_;
}

int HfpHfStateMachine::GetDeviceStateInt() const
{
    return static_cast<const HfpHfState*>(GetState())->GetStateInt();
}

std::vector<HandsFreeUnitCalls> HfpHfStateMachine::GetCurrentCallList()
{
    return calls_->GetCurrentCalls();
}

void HfpHfStateMachine::AddDeferredMessage(const HfpHfMessage &msg)
{
    deferMsgs_.push_back(msg);
}

void HfpHfStateMachine::ProcessDeferredMessage()
{
    auto size = deferMsgs_.size();
    while (size-- > 0 && !deferMsgs_.empty()) {
        HfpHfMessage event = deferMsgs_.front();
        deferMsgs_.pop_front();
        HfpHfService::GetService()->PostEvent(event);
    }
}

void HfpHfStateMachine::StartConnectionTimer() const
{
    connTimer_->Start(CONNECTION_TIMEOUT_MS);
    LOG_INFO("[HFP HF]%{public}s():Start connection timer!", __FUNCTION__);
}

void HfpHfStateMachine::StopConnectionTimer() const
{
    connTimer_->Stop();
    LOG_INFO("[HFP HF]%{public}s():Stop connection timer!", __FUNCTION__);
}

void HfpHfStateMachine::ConnectionTimeout() const
{
    HfpHfMessage event(HFP_HF_CONNECTION_TIMEOUT_EVT);
    event.dev_ = address_;
    HfpHfService::GetService()->PostEvent(event);
}

bool HfpHfStateMachine::IsRemoving() const
{
    return isRemoving_;
}

void HfpHfStateMachine::SetRemoving(bool isRemoving)
{
    isRemoving_ = isRemoving;
}

void HfpHfStateMachine::ProcessNetworkStateEvent(const HfpHfMessage &event)
{
    HfpHfService *service = HfpHfService::GetService();
    RawAddress rawAddress(address_);
    if (networkState_ != event.arg1_) {
        networkState_ = event.arg1_;
        service->NotifyRegistrationStatusChanged(rawAddress, networkState_);
    }

    if (networkState_ == HFP_HF_NETWORK_STATE_AVAILABLE) {
        if (!profile_.QueryOperatorName(true)) {
            LOG_ERROR("[HFP HF]%{public}s():can't query the operator name!", __FUNCTION__);
        }
    }
}
void HfpHfStateMachine::ProcessNetworkRoamEvent(const HfpHfMessage &event)
{
    HfpHfService *service = HfpHfService::GetService();
    RawAddress rawAddress(address_);
    if (networkTRoam_ != event.arg1_) {
        networkTRoam_ = event.arg1_;
        service->NotifyRoamingStatusChanged(rawAddress, networkTRoam_);
    }
}
void HfpHfStateMachine::ProcessNetworkSignalEvent(const HfpHfMessage &event)
{
    HfpHfService *service = HfpHfService::GetService();
    RawAddress rawAddress(address_);
    if (networkSignal_ != event.arg1_) {
        networkSignal_ = event.arg1_;
        service->NotifySignalStrengthChanged(rawAddress, networkSignal_);
    }
}
void HfpHfStateMachine::ProcessBatteryLevelEvent(const HfpHfMessage &event)
{
    HfpHfService *service = HfpHfService::GetService();
    RawAddress rawAddress(address_);
    if (batteryLevel_ != event.arg1_) {
        batteryLevel_ = event.arg1_;
        service->NotifyBatteryLevelChanged(rawAddress, batteryLevel_);
    }
}

void HfpHfStateMachine::ProcessOperatorEvent(const HfpHfMessage &event)
{
    HfpHfService *service = HfpHfService::GetService();
    RawAddress rawAddress(address_);
    if (operatorName_ != event.str_) {
        operatorName_ = event.str_;
        service->NotifyOperatorSelectionChanged(rawAddress, operatorName_);
    }
}

void HfpHfStateMachine::ProcessCallIndicatorEvent(const HfpHfMessage &event)
{
    if (!profile_.QueryCurrentCalls()) {
        LOG_ERROR("[HFP HF]%{public}s():can't query the current calls!", __FUNCTION__);
    }
}

void HfpHfStateMachine::ProcessCurrentCallEvent(const HfpHfMessage &event) const
{
    HandsFreeUnitCalls call(address_,
        event.call_.index,
        event.call_.status,
        event.call_.number,
        (event.call_.mprty == HFP_HF_MPTY_TYPE_MULTI),
        (event.call_.dir == HFP_HF_DIRECTION_TYPE_OUTGOING),
        isInBandRing_);
    calls_->UpdateCall(call);
}

void HfpHfStateMachine::ProcessSetVolumeEvent(const HfpHfMessage &event) const
{
    if (event.arg1_ == HFP_HF_VOLUME_TYPE_SPK) {
        OHOS::AudioStandard::AudioSystemManager::GetInstance()->SetVolume(
            OHOS::AudioStandard::STREAM_VOICE_CALL, event.arg3_);
    } else if (event.arg1_ == HFP_HF_VOLUME_TYPE_MIC) {
        OHOS::AudioStandard::AudioSystemManager::GetInstance()->SetMicrophoneMute(event.arg3_ == 0);
    } else {
        LOG_ERROR("[HFP HF]%{public}s():the error volume type!", __FUNCTION__);
    }
}

void HfpHfStateMachine::ProcessSubscriberNumberEvent(const HfpHfMessage &event)
{
    HfpHfService *service = HfpHfService::GetService();
    RawAddress rawAddress(address_);
    if (subscriberNumber_ != event.str_) {
        subscriberNumber_ = event.str_;
        if (service != nullptr) {
            service->NotifySubscriberNumberChanged(rawAddress, subscriberNumber_);
        } else {
            LOG_DEBUG("[HFP HF]%{public}s():service is null.", __FUNCTION__);
        }
    }
}
void HfpHfStateMachine::ProcessInbandRingEvent(const HfpHfMessage &event)
{
    HfpHfService *service = HfpHfService::GetService();
    RawAddress rawAddress(address_);
    bool ring = (event.arg1_ == 0) ? false : true;
    bool isFirstTime = (event.arg3_ == 0) ? false : true;
    if (isInBandRing_ != ring || isFirstTime == true) {
        isInBandRing_ = ring;
        service->NotifyInBandRingTone(rawAddress, event.arg1_);
    }
}

void HfpHfStateMachine::NotifyVoiceRecognitionStatusChanged(int status)
{
    if (status == voiceRecognitionStatus_) {
        return;
    }
    voiceRecognitionStatus_ = status;
    HfpHfService *service = HfpHfService::GetService();
    if (service != nullptr) {
        RawAddress device(address_);
        service->NotifyVoiceRecognitionStatusChanged(device, voiceRecognitionStatus_);
    }
}

void HfpHfStateMachine::routeHfAudio(bool state)
{
    if (state) {
        if (!isAudioRouted_) {
            OHOS::AudioStandard::AudioSystemManager::GetInstance()->SetAudioParameter("hfp_enable", "true");
        }
    } else {
        OHOS::AudioStandard::AudioSystemManager::GetInstance()->SetAudioParameter("hfp_enable", "false");
    }
    isAudioRouted_ = state;
}

void HfpHfStateMachine::ProcessVrChangedEvent(const HfpHfMessage &event)
{
    NotifyVoiceRecognitionStatusChanged(event.arg1_);
}

void HfpHfStateMachine::ProcessOpenVrResultEvent(const HfpHfMessage &event)
{
    int status = voiceRecognitionStatus_;
    if (event.arg1_ == HFP_HF_AT_RESULT_OK) {
        status = HFP_HF_VR_STATE_OPENED;
    }

    NotifyVoiceRecognitionStatusChanged(status);
}

void HfpHfStateMachine::ProcessCloseVrResultEvent(const HfpHfMessage &event)
{
    int status = voiceRecognitionStatus_;
    if (event.arg1_ == HFP_HF_AT_RESULT_OK) {
        status = HFP_HF_VR_STATE_CLOSED;
    }

    NotifyVoiceRecognitionStatusChanged(status);
}

std::string HfpHfStateMachine::GetEventName(int what)
{
    switch (what) {
        case HFP_HF_INVALID_EVT:
            return "HFP_HF_INVALID_EVT";
        case HFP_HF_SERVICE_STARTUP_EVT:
            return "HFP_HF_SERVICE_STARTUP_EVT";
        case HFP_HF_SERVICE_SHUTDOWN_EVT:
            return "HFP_HF_SERVICE_SHUTDOWN_EVT";
        case HFP_HF_CONNECT_EVT:
            return "HFP_HF_CONNECT_EVT";
        case HFP_HF_DISCONNECT_EVT:
            return "HFP_HF_DISCONNECT_EVT";
        case HFP_HF_CONNECT_AUDIO_EVT:
            return "HFP_HF_CONNECT_AUDIO_EVT";
        case HFP_HF_DISCONNECT_AUDIO_EVT:
            return "HFP_HF_DISCONNECT_AUDIO_EVT";
        case HFP_HF_RETRY_CONNECT_AUDIO_EVT:
            return "HFP_HF_RETRY_CONNECT_AUDIO_EVT";
        case HFP_HF_REMOVE_STATE_MACHINE_EVT:
            return "HFP_HF_REMOVE_STATE_MACHINE_EVT";
        case HFP_HF_INTERACTIVE_EVT:
            return "HFP_HF_INTERACTIVE_EVT";
        case HFP_HF_CONNECTION_TIMEOUT_EVT:
            return "HFP_HF_CONNECTION_TIMEOUT_EVT";
        case HFP_HF_DISCONNECT_TIMEOUT_EVT:
            return "HFP_HF_DISCONNECT_TIMEOUT_EVT";
        case HFP_HF_CONNECT_AUDIO_TIMEOUT_EVT:
            return "HFP_HF_CONNECT_AUDIO_TIMEOUT_EVT";
        case HFP_HF_DISCONNECT_AUDIO_TIMEOUT_EVT:
            return "HFP_HF_DISCONNECT_AUDIO_TIMEOUT_EVT";
        case HFP_HF_SEND_DTMF_EVT:
            return "HFP_HF_SEND_DTMF_EVT";
        case HFP_HF_ACCEPT_CALL_EVT:
            return "HFP_HF_ACCEPT_CALL_EVT";
        case HFP_HF_HOLD_CALL_EVT:
            return "HFP_HF_HOLD_CALL_EVT";
        case HFP_HF_REJECT_CALL_EVT:
            return "HFP_HF_REJECT_CALL_EVT";
        case HFP_HF_HANDLE_INCOMING_CALL_EVT:
            return "HFP_HF_HANDLE_INCOMING_CALL_EVT";
        case HFP_HF_HANDLE_MULTI_CALL_EVT:
            return "HFP_HF_HANDLE_MULTI_CALL_EVT";
        case HFP_HF_DIAL_LAST_NUMBER:
            return "HFP_HF_DIAL_LAST_NUMBER";
        case HFP_HF_DIAL_MEMORY:
            return "HFP_HF_DIAL_MEMORY";
        case HFP_HF_FINISH_CALL_EVT:
            return "HFP_HF_FINISH_CALL_EVT";
        case HFP_HF_DIAL_CALL_EVT:
            return "HFP_HF_DIAL_CALL_EVT";
        case HFP_HF_OPEN_VOICE_RECOGNITION_EVT:
            return "HFP_HF_OPEN_VOICE_RECOGNITION_EVT";
        case HFP_HF_CLOSE_VOICE_RECOGNITION_EVT:
            return "HFP_HF_CLOSE_VOICE_RECOGNITION_EVT";
        case HFP_HF_SET_VOLUME_EVT:
            return "HFP_HF_SET_VOLUME_EVT";
        case HFP_HF_BATTERY_LEVEL_CHANGED_EVT:
            return "HFP_HF_BATTERY_LEVEL_CHANGED_EVT";
        case HFP_HF_ENHANCED_DRIVER_SAFETY_CHANGED_EVT:
            return "HFP_HF_ENHANCED_DRIVER_SAFETY_CHANGED_EVT";
        case HFP_HF_SDP_DISCOVERY_RESULT_SUCCESS:
            return "HFP_HF_SDP_DISCOVERY_RESULT_SUCCESS";
        case HFP_HF_SDP_DISCOVERY_RESULT_FAIL:
            return "HFP_HF_SDP_DISCOVERY_RESULT_FAIL";
        case HFP_HF_CONNECT_REQUEST_EVT:
            return "HFP_HF_CONNECT_REQUEST_EVT";
        case HFP_HF_CONNECTED_EVT:
            return "HFP_HF_CONNECTED_EVT";
        case HFP_HF_DISCONNECTED_EVT:
            return "HFP_HF_DISCONNECTED_EVT";
        case HFP_HF_CONNECT_FAILED_EVT:
            return "HFP_HF_CONNECT_FAILED_EVT";
        case HFP_HF_DISCONNECT_FAILED_EVT:
            return "HFP_HF_DISCONNECT_FAILED_EVT";
        case HFP_HF_DATA_AVAILABLE_EVT:
            return "HFP_HF_DATA_AVAILABLE_EVT";
        case HFP_HF_SLC_ESTABLISHED_EVT:
            return "HFP_HF_SLC_ESTABLISHED_EVT";
        case HFP_HF_AUDIO_CONNECT_REQUEST_EVT:
            return "HFP_HF_AUDIO_CONNECT_REQUEST_EVT";
        case HFP_HF_AUDIO_CONNECTING_EVT:
            return "HFP_HF_AUDIO_CONNECTING_EVT";
        case HFP_HF_AUDIO_DISCONNECTING_EVT:
            return "HFP_HF_AUDIO_DISCONNECTING_EVT";
        case HFP_HF_AUDIO_CONNECTED_EVT:
            return "HFP_HF_AUDIO_CONNECTED_EVT";
        case HFP_HF_AUDIO_CONNECT_FAILED_EVT:
            return "HFP_HF_AUDIO_CONNECT_FAILED_EVT";
        case HFP_HF_AUDIO_DISCONNECTED_EVT:
            return "HFP_HF_AUDIO_DISCONNECTED_EVT";
        case HFP_HF_AUDIO_DISCONNECT_FAILED_EVT:
            return "HFP_HF_AUDIO_DISCONNECT_FAILED_EVT";
        case HFP_HF_SEND_AT_COMMAND_EVT:
            return "HFP_HF_SEND_AT_COMMAND_EVT";
        default:
            return "Unknown";
    }
}

std::string HfpHfStateMachine::GetInteractiveEventType(int type) const
{
    switch (type) {
        case HFP_HF_TYPE_NONE:
            return "HFP_HF_TYPE_NONE";
        case HFP_HF_TYPE_NETWORK_STATE:
            return "HFP_HF_TYPE_NETWORK_STATE";
        case HFP_HF_TYPE_NETWORK_ROAM:
            return "HFP_HF_TYPE_NETWORK_ROAM";
        case HFP_HF_TYPE_NETWORK_SIGNAL:
            return "HFP_HF_TYPE_NETWORK_SIGNAL";
        case HFP_HF_TYPE_BATTERY_LEVEL:
            return "HFP_HF_TYPE_BATTERY_LEVEL";
        case HFP_HF_TYPE_CURRENT_OPERATOR:
            return "HFP_HF_TYPE_CURRENT_OPERATOR";
        case HFP_HF_TYPE_CALL_STATE:
            return "HFP_HF_TYPE_CALL_STATE";
        case HFP_HF_TYPE_CALL_SETUP_STATE:
            return "HFP_HF_TYPE_CALL_SETUP_STATE";
        case HFP_HF_TYPE_CALL_HELD_STATE:
            return "HFP_HF_TYPE_CALL_HELD_STATE";
        case HFP_HF_TYPE_HOLD_RESULT:
            return "HFP_HF_TYPE_HOLD_RESULT";
        case HFP_HF_TYPE_CALLING_LINE_IDENTIFICATION:
            return "HFP_HF_TYPE_CALLING_LINE_IDENTIFICATION";
        case HFP_HF_TYPE_CALL_WAITING:
            return "HFP_HF_TYPE_CALL_WAITING";
        case HFP_HF_TYPE_CURRENT_CALLS:
            return "HFP_HF_TYPE_CURRENT_CALLS";
        case HFP_HF_TYPE_SET_VOLUME:
            return "HFP_HF_TYPE_SET_VOLUME";
        case HFP_HF_TYPE_AT_CMD_RESULT:
            return "HFP_HF_TYPE_AT_CMD_RESULT";
        case HFP_HF_TYPE_SUBSCRIBER_NUMBER:
            return "HFP_HF_TYPE_SUBSCRIBER_NUMBER";
        case HFP_HF_TYPE_INBAND_RING:
            return "HFP_HF_TYPE_INBAND_RING";
        case HFP_HF_TYPE_LAST_NUMBER:
            return "HFP_HF_TYPE_LAST_NUMBER";
        case HFP_HF_TYPE_RING_ALERT:
            return "HFP_HF_TYPE_RING_ALERT";
        case HFP_HF_TYPE_UNKNOWN:
            return "HFP_HF_TYPE_UNKNOWN";
        case HFP_HF_TYPE_QUERY_CURRENT_CALLS_DONE:
            return "HFP_HF_TYPE_QUERY_CURRENT_CALLS_DONE";
        case HFP_HF_SETUP_CODEC_CVSD:
            return "HFP_HF_SETUP_CODEC_CVSD";
        case HFP_HF_SLC_ESTABLISHED_EVT:
            return "HFP_HF_SLC_ESTABLISHED_EVT";
        case HFP_HF_AUDIO_CONNECT_REQUEST_EVT:
            return "HFP_HF_AUDIO_CONNECT_REQUEST_EVT";
        default:
            return "Unknown";
    }
}
}  // namespace bluetooth
}  // namespace OHOS
