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

#include "hfp_ag_statemachine.h"

#include "adapter_config.h"
#include "hfp_ag_data_connection_server.h"
#include "hfp_ag_defines.h"
#include "hfp_ag_message.h"
#include "hfp_ag_service.h"
#include "hfp_ag_system_interface.h"
#include "power_manager.h"
#include "log_util.h"

namespace OHOS {
namespace bluetooth {
HfpAgStateMachine::HfpAgStateMachine(const std::string &address)
    : address_(address), profile_(address), eventProcessor_(profile_, address)
{
}

void HfpAgStateMachine::Init()
{
    profile_.Init();
    connTimer_ = std::make_unique<utility::Timer>(std::bind(&bluetooth::HfpAgStateMachine::ConnectionTimeout, this));

    std::unique_ptr<utility::StateMachine::State> disconnectedState =
        std::make_unique<HfpAgDisconnected>(DISCONNECTED, *this, profile_, eventProcessor_);
    std::unique_ptr<utility::StateMachine::State> connectingState =
        std::make_unique<HfpAgConnecting>(CONNECTING, *this, profile_, eventProcessor_);
    std::unique_ptr<utility::StateMachine::State> disconnectingState =
        std::make_unique<HfpAgDisconnecting>(DISCONNECTING, *this, profile_, eventProcessor_);
    std::unique_ptr<utility::StateMachine::State> connectedState =
        std::make_unique<HfpAgConnected>(CONNECTED, *this, profile_, eventProcessor_);
    std::unique_ptr<utility::StateMachine::State> audioConnecting =
        std::make_unique<HfpAgAudioConnecting>(AUDIO_CONNECTING, *this, profile_, eventProcessor_, *connectedState);
    std::unique_ptr<utility::StateMachine::State> audioConnected =
        std::make_unique<HfpAgAudioConnected>(AUDIO_CONNECTED, *this, profile_, eventProcessor_, *connectedState);
    std::unique_ptr<utility::StateMachine::State> audioDisconnecting =
        std::make_unique<HfpAgAudioDisconnecting>(AUDIO_DISCONNECTING,
                                                  *this, profile_, eventProcessor_, *connectedState);

    Move(disconnectedState);
    Move(connectingState);
    Move(disconnectingState);
    Move(audioConnecting);
    Move(audioConnected);
    Move(audioDisconnecting);
    Move(connectedState);   // Add parenet state at last
    InitState(DISCONNECTED);
}

inline std::string HfpAgStateMachine::GetDeviceAddr() const
{
    return address_;
}

int HfpAgStateMachine::GetStateInt() const
{
    return static_cast<const HfpAgState*>(GetState())->GetStateInt();
}

void HfpAgStateMachine::AddDeferredMessage(const HfpAgMessage &msg)
{
    deferMsgs_.push_back(msg);
}

void HfpAgStateMachine::ProcessDeferredMessage()
{
    auto size = deferMsgs_.size();
    while (size-- > 0 && !deferMsgs_.empty()) {
        HfpAgMessage event = deferMsgs_.front();
        deferMsgs_.pop_front();
        HfpAgService::GetService()->PostEvent(event);
    }
}

void HfpAgStateMachine::NotifyStateTransitions()
{
    int toState = GetStateInt();
    if (toState == HFP_AG_STATE_CONNECTED && preState_ == HFP_AG_STATE_CONNECTING) {
        HfpAgSystemInterface::GetInstance().QueryPhoneState();
    }

    HfpAgService *service = HfpAgService::GetService();
    if (service != nullptr) {
        RawAddress device(address_);
        if ((preState_ != toState) && (preState_ <= HFP_AG_STATE_CONNECTED) && (toState <= HFP_AG_STATE_CONNECTED)) {
            service->NotifySlcStateChanged(device, toState);
        }

        if ((preState_ != toState) && (preState_ >= HFP_AG_AUDIO_STATE_DISCONNECTED) &&
            (toState >= HFP_AG_AUDIO_STATE_DISCONNECTED)) {
            service->NotifyAudioStateChanged(device, toState);
        }

        if (((toState == HFP_AG_STATE_CONNECTED) && (preState_ < toState)) ||
            ((toState == HFP_AG_STATE_DISCONNECTED) && (preState_ > toState))) {
            service->SlcStateChanged(address_, toState);
        }

        service->ScoStateChanged(address_, preState_, toState);
    }

    preState_ = toState;
}

void HfpAgStateMachine::NotifyChildStateToParentState(int fromState, int toState)
{
    LOG_INFO("[HFP AG]%{public}s(): fromState[%{public}d], toState[%{public}d]", __FUNCTION__, fromState, toState);
    HfpAgService *service = HfpAgService::GetService();
    if (service != nullptr) {
        if ((fromState != toState) && (fromState >= HFP_AG_AUDIO_STATE_DISCONNECTED) &&
            (toState >= HFP_AG_AUDIO_STATE_DISCONNECTED)) {
            RawAddress device(address_);
            service->NotifyAudioStateChanged(device, toState);
        }

        if (((toState == HFP_AG_STATE_CONNECTED) && (fromState < toState)) ||
            ((toState == HFP_AG_STATE_DISCONNECTED) && (fromState > toState))) {
            service->SlcStateChanged(address_, toState);
        }

        service->ScoStateChanged(address_, fromState, toState);
    }
    preState_ = toState;
    ProcessDeferredMessage();
}

void HfpAgStateMachine::ProcessAudioDisconnected()
{
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::SCO_OFF, PROFILE_NAME_HFP_AG, RawAddress(address_));
    NotifyChildStateToParentState(HFP_AG_AUDIO_STATE_CONNECTED, HFP_AG_AUDIO_STATE_DISCONNECTED);
}

void HfpAgStateMachine::StartConnectionTimer() const
{
    connTimer_->Start(CONNECTION_TIMEOUT_MS);
    LOG_INFO("[HFP AG]%{public}s():Start connection timer!", __FUNCTION__);
}

void HfpAgStateMachine::StopConnectionTimer() const
{
    connTimer_->Stop();
    LOG_INFO("[HFP AG]%{public}s():Stop connection timer!", __FUNCTION__);
}

bool HfpAgStateMachine::IsRemoving() const
{
    return isRemoving_;
}

void HfpAgStateMachine::SetRemoving(bool isRemoving)
{
    isRemoving_ = isRemoving;
}

void HfpAgStateMachine::ConnectionTimeout() const
{
    HfpAgMessage event(HFP_AG_CONNECTION_TIMEOUT_EVT);
    event.dev_ = address_;
    HfpAgService::GetService()->PostEvent(event);
}

void HfpAgStateMachine::ProcessKeyPressed(const RawAddress &device, const int &callState) const
{
    LOG_DEBUG("[HFP AG]%{public}s():enter",  __FUNCTION__);
    HfpAgSystemInterface& mSystemInterface = HfpAgSystemInterface::GetInstance();
    if (mSystemInterface.IsRinging()) {
        mSystemInterface.AnswerCall(device.GetAddress());
    } else if (mSystemInterface.IsInCall()) {
        if (GetStateInt() == HfpAgAudioState::HFP_AG_AUDIO_STATE_DISCONNECTED) {
            if (!HfpAgService::GetService()->SetActiveDevice(device)) {
                HILOGI("[HFP AG][failed to set active device to][%{public}s]", GET_ENCRYPT_ADDR(device));
            }
        } else {
            mSystemInterface.HangupCall(device.GetAddress());
        }
    } else if (GetStateInt() != HfpAgAudioState::HFP_AG_AUDIO_STATE_DISCONNECTED) {
        profile_.ReleaseAudioConnection();
    } else {
        if (callState == HFP_AG_CALL_STATE_DIALING) {
            LOG_INFO("[HFP AG]%{public}s():already dialling!", __FUNCTION__);
            return;
        }
        std::string dialNumber = mSystemInterface.GetLastDialNumber();
        if (dialNumber.empty()) {
            LOG_INFO("[HFP AG]%{public}s():last dial number null!", __FUNCTION__);
            return;
        }
        mSystemInterface.DialOutCall(device.GetAddress(), dialNumber);
    }
}

void HfpAgDisconnected::Entry()
{
    stateMachine_.ProcessDeferredMessage();

    if (isReentry_) {
        stateMachine_.SetRemoving(true);
        profile_.RemoveStateMachine();
        stateMachine_.NotifyStateTransitions();
    }
}

void HfpAgDisconnected::Exit()
{
    isReentry_ = true;
}

bool HfpAgDisconnected::Dispatch(const utility::Message &msg)
{
    HfpAgMessage &event = (HfpAgMessage &)msg;
    LOG_INFO("[HFP AG]%{public}s():[Disconnected][%{public}s]", __FUNCTION__,
        HfpAgStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case HFP_AG_CONNECT_EVT:
            profile_.DoServiceDiscovery(HFP_AG_INITIATOR);
            Transition(HfpAgStateMachine::CONNECTING);
            break;
        case HFP_AG_CONNECT_REQUEST_EVT:
            profile_.DoServiceDiscovery(HFP_AG_ACCEPTOR, event.arg1_);
            Transition(HfpAgStateMachine::CONNECTING);
            break;
        case HFP_AG_AUDIO_CONNECT_REQUEST_EVT:
            profile_.RejectAudioConnection();
            break;
        default:
            break;
    }
    return true;
}

void HfpAgConnecting::Entry()
{
    stateMachine_.NotifyStateTransitions();
    stateMachine_.StartConnectionTimer();
    AdapterConfig::GetInstance()->GetValue(HSP_AG_STATE_SECTION_NAME, HSP_AG_STATE_PROPERY_NAME, hspState_);
}

void HfpAgConnecting::Exit()
{
    stateMachine_.StopConnectionTimer();
}

bool HfpAgConnecting::Dispatch(const utility::Message &msg)
{
    HfpAgMessage &event = (HfpAgMessage &)msg;
    LOG_INFO("[HFP AG]%{public}s():[Connecting][%{public}s]", __FUNCTION__,
        HfpAgStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case HFP_AG_AUDIO_CONNECT_REQUEST_EVT:
            profile_.ProcessAudioConnectRequest();
            break;
        case HFP_AG_AUDIO_CONNECTED_EVT:
        case HFP_AG_AUDIO_DISCONNECTED_EVT:
        case HFP_AG_CONNECT_EVT:
        case HFP_AG_DISCONNECT_EVT:
            stateMachine_.AddDeferredMessage(event);
            break;
        case HFP_AG_SDP_DISCOVERY_RESULT_SUCCESS:
            if (profile_.ServiceDiscoveryResult() != HFP_AG_SUCCESS) {
                Transition(HfpAgStateMachine::DISCONNECTED);
            }
            break;
        case HFP_AG_SDP_DISCOVERY_RESULT_FAIL:
            Transition(HfpAgStateMachine::DISCONNECTED);
            break;
        case HFP_AG_DISCONNECTED_EVT:
            profile_.RemoveRemoteScnLoging();
            Transition(HfpAgStateMachine::DISCONNECTED);
            break;
        case HFP_AG_CONNECT_FAILED_EVT:
            profile_.RemoveRemoteScnLoging();
            Transition(HfpAgStateMachine::DISCONNECTED);
            break;
        case HFP_AG_DATA_AVAILABLE_EVT:
            profile_.ReadData();
            break;
        case HFP_AG_SLC_ESTABLISHED_EVT:
            profile_.ProcessSlcEstablished();
            Transition(HfpAgStateMachine::CONNECTED);
            break;
        case HFP_AG_CONNECTED_EVT:
            if (hspState_ == HSP_AG_STATE_HSP) {
                Transition(HfpAgStateMachine::CONNECTED);
            }
            break;
        case HFP_AG_CONTROL_OTHER_MODULES_EVT:
            eventProcessor_.ExecuteEventProcess(event);
            break;
        case HFP_AG_CONNECTION_TIMEOUT_EVT:
            Transition(HfpAgStateMachine::DISCONNECTED);
            break;
        default:
            break;
    }
    return true;
}

void HfpAgDisconnecting::Entry()
{
    stateMachine_.ProcessDeferredMessage();
    stateMachine_.NotifyStateTransitions();
    stateMachine_.StartConnectionTimer();
}

void HfpAgDisconnecting::Exit()
{
    stateMachine_.StopConnectionTimer();
}

bool HfpAgDisconnecting::Dispatch(const utility::Message &msg)
{
    HfpAgMessage &event = (HfpAgMessage &)msg;
    LOG_INFO("[HFP AG]%{public}s():[Disconnecting][%{public}s]", __FUNCTION__,
        HfpAgStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case HFP_AG_CONNECT_EVT:
            stateMachine_.AddDeferredMessage(event);
            break;
        case HFP_AG_DISCONNECTED_EVT:
            IPowerManager::GetInstance().StatusUpdate(
                RequestStatus::CONNECT_OFF, PROFILE_NAME_HFP_AG, RawAddress(event.dev_));
            profile_.RemoveRemoteScnLoging();
            Transition(HfpAgStateMachine::DISCONNECTED);
            break;
        case HFP_AG_AUDIO_CONNECT_REQUEST_EVT:
            profile_.RejectAudioConnection();
            break;
        case HFP_AG_CONNECTION_TIMEOUT_EVT:
            Transition(HfpAgStateMachine::CONNECTED);
            break;
        default:
            break;
    }
    return true;
}

void HfpAgConnected::Entry()
{
    stateMachine_.ProcessDeferredMessage();

    HfpAgMessage event(HFP_AG_CONTROL_OTHER_MODULES_EVT);
    event.type_ = HFP_AG_MSG_TYPE_QUERY_AG_INDICATOR;
    eventProcessor_.ExecuteEventProcess(event);

    stateMachine_.NotifyStateTransitions();
}

bool HfpAgConnected::Dispatch(const utility::Message &msg)
{
    HfpAgMessage &event = (HfpAgMessage &)msg;
    LOG_INFO("[HFP AG]%{public}s():[Connected][%{public}s]", __FUNCTION__,
        HfpAgStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case HFP_AG_CONNECT_AUDIO_EVT:
            profile_.SetupCodecConnection();
            Transition(HfpAgStateMachine::AUDIO_CONNECTING);
            break;
        case HFP_AG_AUDIO_CONNECT_REQUEST_EVT:
            if (profile_.ProcessAudioConnectRequest()) {
                Transition(HfpAgStateMachine::AUDIO_CONNECTING);
            }
            break;
        case HFP_AG_AUDIO_CONNECTING_EVT:
            Transition(HfpAgStateMachine::AUDIO_CONNECTING);
            break;
        case HFP_AG_AUDIO_CONNECTED_EVT:
            IPowerManager::GetInstance().StatusUpdate(
                RequestStatus::SCO_ON, PROFILE_NAME_HFP_AG, RawAddress(event.dev_));
            Transition(HfpAgStateMachine::AUDIO_CONNECTED);
            break;
        case HFP_AG_DISCONNECT_EVT:
            profile_.ReleaseDataConnection();
            Transition(HfpAgStateMachine::DISCONNECTING);
            break;
        case HFP_AG_DISCONNECTED_EVT:
            IPowerManager::GetInstance().StatusUpdate(
                RequestStatus::CONNECT_OFF, PROFILE_NAME_HFP_AG, RawAddress(event.dev_));
            profile_.RemoveRemoteScnLoging();
            Transition(HfpAgStateMachine::DISCONNECTED);
            break;
        case HFP_AG_DATA_AVAILABLE_EVT:
            profile_.ReadData();
            break;
        case HFP_AG_CONTROL_OTHER_MODULES_EVT:
            if (event.type_ == HFP_AG_MSG_TYPE_DIAL_CALL) {
                profile_.SendResultCode(HFP_AG_RESULT_OK);
            }
            eventProcessor_.ExecuteEventProcess(event);
            break;
        case HFP_AG_OPEN_VOICE_RECOGNITION_EVT:
            profile_.ActivateVoiceRecognition();
            break;
        case HFP_AG_CLOSE_VOICE_RECOGNITION_EVT:
            profile_.DeactivateVoiceRecognition();
            break;
        case HFP_AG_PROCESS_CKPD_EVT:
            stateMachine_.ProcessKeyPressed(RawAddress(event.dev_), callState_);
            break;
        case HFP_AG_VOICE_RECOGNITION_RESULT_EVT:
            ProcessVoiceRecognitionResult(event.arg1_);
            break;
        case HFP_AG_VOICE_RECOGNITION_TIME_OUT_EVT:
            profile_.SendResultCode(HFP_AG_RESULT_ERROR);
            break;
        case HFP_AG_CALL_STATE_CHANGE:
            callState_ = event.state_.callState;
            ProcessPhoneStateChange(event);
            break;
        case HFP_AG_CALL_STATE_CHANGE_MOCK:
            event.type_ = HFP_AG_CALL_STATE_CHANGE_MOCK;
            eventProcessor_.ExecuteEventProcess(event);
            break;
        case HFP_AG_SEND_CCLC_RESPONSE:
        case HFP_AG_RESPONSE_CLCC_TIME_OUT_EVT:
            ProcessResponseClcc(event);
            break;
        case HFP_AG_NOTIFY_SERVICE_STATE:
            profile_.ReportRegistrationStatus(event.arg1_);
            break;
        case HFP_AG_NOTIFY_ROAM_STATE:
            profile_.ReportRoamingState(event.arg1_);
            break;
        case HFP_AG_NOTIFY_SIGNAL_STRENGTH:
            profile_.ReportSignalStrength(event.arg1_);
            break;
        case HFP_AG_NOTIFY_BATTERY_LEVEL:
            profile_.ReportBatteryLevel(event.arg1_);
            break;
        case HFP_AG_DIALING_OUT_RESULT:
            profile_.SendResultCode(event.arg1_);
            break;
        case HFP_AG_SET_INBAND_RING_TONE_EVT:
            profile_.SetInbandRingTone(event.arg1_);
            break;
        case HFP_AG_RING_TIMEOUT_EVT:
            profile_.SendRingAndClip();
            break;
        case HFP_AG_NOTIFY_INDICATOR_EVT:
            eventProcessor_.ExecuteEventProcess(event);
            break;
        case HFP_AG_RESPONE_OK_EVT:
            ProcessResponesOK();
            break;
        case HFP_AG_SEND_INCOMING_EVT:
            profile_.NotifyIncomingCallWaiting(event.arg1_, event.str_);
            LOG_INFO(" incoming call");
            break;
        case HFP_AG_SEND_CALL_SETUP_EVT:
            LOG_INFO(" incoming call set up");
            profile_.ReportCallsetupStatus(event.arg1_);
            break;
        case HFP_AG_SEND_CALL_HELD_EVT:
            profile_.ReportCallheldStatus(event.arg1_);
            break;
        case HFP_AG_SEND_CALL_STATE_EVT:
            profile_.ReportCallStatus(event.arg1_);
            break;
        case HFP_AG_GET_VOICE_NUMBER:
            event.type_ = HFP_AG_GET_VOICE_NUMBER;
            eventProcessor_.ExecuteEventProcess(event);
            break;
        case HFP_AG_SEND_BINP_EVT:
            profile_.SendBinp(event.str_);
            break;
        case HFP_AG_GET_BTRH_EVT:
        case HFP_AG_SET_BTRH_EVT:
            eventProcessor_.ExecuteEventProcess(event);
            break;
        case HFP_AG_SEND_RESPONSE_HOLD_STATE:
            profile_.ReportResponseHoldStatus(event.arg1_, -1);
            break;
        case HFP_AG_SEND_BTRH_EVT:
            profile_.ReportResponseHoldStatus(event.arg1_, event.arg3_);
            break;
        case HFP_AG_SEND_NO_CARRIER:
            profile_.SendResultCode(HFP_AG_RESULT_NO_CARRIER);
            break;
        case HFP_AG_START_MOCK:
            profile_.startMock(event.arg1_);
            break;
        case HFP_AG_MOCK_RING:
            profile_.SendResultCode(HFP_AG_RESULT_RING);
            break;
        case HFP_AG_MOCK_CLIP:
            profile_.NotifyCallingLineIdentification(CALL_TYPE_DEFAULT, event.str_);
            break;
        default:
            break;
    }
    return true;
}

void HfpAgConnected::ProcessVoiceRecognitionResult(int result)
{
    if (result == 1) {
        profile_.SendResultCode(HFP_AG_RESULT_OK);

        auto address = stateMachine_.GetDeviceAddr();
        if (HfpAgProfile::IsAudioConnected(address) == false) {
            HfpAgMessage msg(HFP_AG_CONNECT_AUDIO_EVT);
            msg.dev_ = address;
            HfpAgService::GetService()->PostEvent(msg);
        }
    } else {
        profile_.SendResultCode(HFP_AG_RESULT_ERROR);
    }
}

void HfpAgConnected::ProcessPhoneStateChange(const HfpAgMessage &event) const
{
    profile_.PhoneStateChange(event.state_);
}

void HfpAgConnected::ProcessResponseClcc(const HfpAgMessage &event) const
{
    profile_.ReportCurrentCallList(event.call_);
}

void HfpAgConnected::ProcessResponesOK() const
{
    profile_.ResponesOK();
}

void HfpAgAudioConnecting::Entry()
{
    stateMachine_.ProcessDeferredMessage();
    stateMachine_.NotifyStateTransitions();
    stateMachine_.StartConnectionTimer();
}

void HfpAgAudioConnecting::Exit()
{
    stateMachine_.StopConnectionTimer();
}

bool HfpAgAudioConnecting::Dispatch(const utility::Message &msg)
{
    HfpAgMessage &event = (HfpAgMessage &)msg;
    LOG_INFO("[HFP AG]%{public}s():[AudioConnecting][%{public}s]", __FUNCTION__,
        HfpAgStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case HFP_AG_CONNECT_AUDIO_EVT:
        case HFP_AG_AUDIO_CONNECT_REQUEST_EVT:
        case HFP_AG_AUDIO_CONNECTING_EVT:
            break;
        case HFP_AG_DISCONNECT_EVT:
        case HFP_AG_DISCONNECT_AUDIO_EVT:
            stateMachine_.AddDeferredMessage(event);
            break;
        case HFP_AG_RETRY_CONNECT_AUDIO_EVT:
        case HFP_AG_CODEC_NEGOTIATED_EVT:
            profile_.EstablishAudioConnection();
            break;
        case HFP_AG_SETUP_CODEC_CVSD:
            profile_.SetupCodecCvsd();
            break;
        case HFP_AG_CODEC_NEGOTIATION_FAILED:
            profile_.SetupCodecConnection();
            break;
        case HFP_AG_AUDIO_CONNECTED_EVT:
            IPowerManager::GetInstance().StatusUpdate(
                RequestStatus::SCO_ON, PROFILE_NAME_HFP_AG, RawAddress(event.dev_));
            Transition(HfpAgStateMachine::AUDIO_CONNECTED);
            break;
        case HFP_AG_AUDIO_CONNECT_FAILED_EVT:
        case HFP_AG_CONNECTION_TIMEOUT_EVT:
        case HFP_AG_AUDIO_DISCONNECTED_EVT:
            stateMachine_.NotifyChildStateToParentState(HFP_AG_AUDIO_STATE_CONNECTING, HFP_AG_AUDIO_STATE_DISCONNECTED);
            Transition(HfpAgStateMachine::CONNECTED);
            break;
        case HFP_AG_DISCONNECTED_EVT:
            stateMachine_.AddDeferredMessage(event);
            stateMachine_.NotifyChildStateToParentState(HFP_AG_AUDIO_STATE_CONNECTING, HFP_AG_AUDIO_STATE_DISCONNECTED);
            Transition(HfpAgStateMachine::CONNECTED);
            break;
        default:
            return false;
    }
    return true;
}

void HfpAgAudioDisconnecting::Entry()
{
    stateMachine_.NotifyStateTransitions();
    stateMachine_.StartConnectionTimer();
}

void HfpAgAudioDisconnecting::Exit()
{
    stateMachine_.StopConnectionTimer();
}

bool HfpAgAudioDisconnecting::Dispatch(const utility::Message &msg)
{
    HfpAgMessage &event = (HfpAgMessage &)msg;
    LOG_INFO("[HFP AG]%{public}s():[AudioDisconnecting][%{public}s]", __FUNCTION__,
        HfpAgStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case HFP_AG_DISCONNECT_EVT:
        case HFP_AG_CONNECT_AUDIO_EVT:
        case HFP_AG_AUDIO_CONNECT_REQUEST_EVT:
        case HFP_AG_DISCONNECTED_EVT:
            stateMachine_.AddDeferredMessage(event);
            break;
        case HFP_AG_AUDIO_CONNECTING_EVT:
        case HFP_AG_DISCONNECT_AUDIO_EVT:
        case HFP_AG_AUDIO_DISCONNECTING_EVT:
        case HFP_AG_AUDIO_CONNECTED_EVT:
        case HFP_AG_AUDIO_CONNECT_FAILED_EVT:
            break;
        case HFP_AG_AUDIO_DISCONNECTED_EVT:
            stateMachine_.ProcessAudioDisconnected();
            Transition(HfpAgStateMachine::CONNECTED);
            break;
        case HFP_AG_AUDIO_DISCONNECT_FAILED_EVT:
        case HFP_AG_CONNECTION_TIMEOUT_EVT:
            stateMachine_.NotifyChildStateToParentState(HFP_AG_AUDIO_STATE_DISCONNECTING, HFP_AG_AUDIO_STATE_CONNECTED);
            Transition(HfpAgStateMachine::AUDIO_CONNECTED);
            break;
        default:
            return false;
    }
    return true;
}

void HfpAgAudioConnected::Entry()
{
    stateMachine_.ProcessDeferredMessage();
    stateMachine_.NotifyStateTransitions();
    profile_.PostAudioConnectionEstablishment();
}

bool HfpAgAudioConnected::Dispatch(const utility::Message &msg)
{
    HfpAgMessage &event = (HfpAgMessage &)msg;
    LOG_INFO("[HFP AG]%{public}s():[AudioConnected][%{public}s]", __FUNCTION__,
        HfpAgStateMachine::GetEventName(event.what_).c_str());
    switch (event.what_) {
        case HFP_AG_DISCONNECT_EVT:
            ProcessDisconnect(event);
            break;
        case HFP_AG_DISCONNECTED_EVT:
            stateMachine_.AddDeferredMessage(event);
            break;
        case HFP_AG_CONNECT_AUDIO_EVT:
        case HFP_AG_AUDIO_CONNECT_REQUEST_EVT:
        case HFP_AG_AUDIO_CONNECTING_EVT:
        case HFP_AG_AUDIO_CONNECTED_EVT:
            break;
        case HFP_AG_DISCONNECT_AUDIO_EVT:
            profile_.ReleaseAudioConnection();
            break;
        case HFP_AG_AUDIO_DISCONNECTED_EVT:
            stateMachine_.ProcessAudioDisconnected();
            Transition(HfpAgStateMachine::CONNECTED);
            break;
        case HFP_AG_AUDIO_DISCONNECTING_EVT:
            Transition(HfpAgStateMachine::AUDIO_DISCONNECTING);
            break;
        case HFP_AG_SET_VOLUME_EVT:
            ProcessSetVolume(event);
            break;
        default:
            return false;
    }
    return true;
}

void HfpAgAudioConnected::ProcessDisconnect(const HfpAgMessage &event)
{
    profile_.ReleaseAudioConnection();
    stateMachine_.AddDeferredMessage(event);
}

void HfpAgAudioConnected::ProcessSetVolume(const HfpAgMessage &event)
{
    if (event.arg1_ == HFP_AG_VOLUME_TYPE_SPK) {
        profile_.SetSpeakerVolume(event.arg3_);
    } else if (event.arg1_ == HFP_AG_VOLUME_TYPE_MIC) {
        profile_.SetMicrophoneGain(event.arg3_);
    } else {
        LOG_ERROR("the error type:%{public}d", event.arg1_);
    }
}

std::string HfpAgStateMachine::GetEventName(int what)
{
    switch (what) {
        case HFP_AG_INVALID_EVT:
            return "HFP_AG_INVALID_EVT";
        case HFP_AG_SERVICE_STARTUP_EVT:
            return "HFP_AG_SERVICE_STARTUP_EVT";
        case HFP_AG_SERVICE_SHUTDOWN_EVT:
            return "HFP_AG_SERVICE_SHUTDOWN_EVT";
        case HFP_AG_CONNECT_EVT:
            return "HFP_AG_CONNECT_EVT";
        case HFP_AG_DISCONNECT_EVT:
            return "HFP_AG_DISCONNECT_EVT";
        case HFP_AG_PROCESS_CKPD_EVT:
            return "HFP_AG_PROCESS_CKPD_EVT";
        case HFP_AG_CONNECT_AUDIO_EVT:
            return "HFP_AG_CONNECT_AUDIO_EVT";
        case HFP_AG_DISCONNECT_AUDIO_EVT:
            return "HFP_AG_DISCONNECT_AUDIO_EVT";
        case HFP_AG_RETRY_CONNECT_AUDIO_EVT:
            return "HFP_AG_RETRY_CONNECT_AUDIO_EVT";
        case HFP_AG_CONNECTION_TIMEOUT_EVT:
            return "HFP_AG_CONNECTION_TIMEOUT_EVT";
        case HFP_AG_DISCONNECT_TIMEOUT_EVT:
            return "HFP_AG_DISCONNECT_TIMEOUT_EVT";
        case HFP_AG_CONNECT_AUDIO_TIMEOUT_EVT:
            return "HFP_AG_CONNECT_AUDIO_TIMEOUT_EVT";
        case HFP_AG_DISCONNECT_AUDIO_TIMEOUT_EVT:
            return "HFP_AG_DISCONNECT_AUDIO_TIMEOUT_EVT";
        case HFP_AG_OPEN_VOICE_RECOGNITION_EVT:
            return "HFP_AG_OPEN_VOICE_RECOGNITION_EVT";
        case HFP_AG_CLOSE_VOICE_RECOGNITION_EVT:
            return "HFP_AG_CLOSE_VOICE_RECOGNITION_EVT";
        case HFP_AG_VOICE_RECOGNITION_RESULT_EVT:
            return "HFP_AG_VOICE_RECOGNITION_RESULT_EVT";
        case HFP_AG_SET_MICROPHONE_GAIN_EVT:
            return "HFP_AG_SET_MICROPHONE_GAIN_EVT";
        case HFP_AG_SET_VOLUME_EVT:
            return "HFP_AG_SET_VOLUME_EVT";
        case HFP_AG_SET_INBAND_RING_TONE_EVT:
            return "HFP_AG_SET_INBAND_RING_TONE_EVT";
        case HFP_AG_SEND_SUBSCRIBER_NUMBER_EVT:
            return "HFP_AG_SEND_SUBSCRIBER_NUMBER_EVT";
        case HFP_AG_SEND_NETWORK_OPERATOR_EVT:
            return "HFP_AG_SEND_NETWORK_OPERATOR_EVT";
        case HFP_AG_CONTROL_OTHER_MODULES_EVT:
            return "HFP_AG_CONTROL_OTHER_MODULES_EVT";
        case HFP_AG_DIALING_OUT_RESULT:
            return "HFP_AG_DIALING_OUT_RESULT";
        case HFP_AG_CALL_STATE_CHANGE:
            return "HFP_AG_CALL_STATE_CHANGE";
        case HFP_AG_SEND_CCLC_RESPONSE:
            return "HFP_AG_SEND_CCLC_RESPONSE";
        case HFP_AG_NOTIFY_SERVICE_STATE:
            return "HFP_AG_NOTIFY_SERVICE_STATE";
        case HFP_AG_NOTIFY_ROAM_STATE:
            return "HFP_AG_NOTIFY_ROAM_STATE";
        case HFP_AG_NOTIFY_SIGNAL_STRENGTH:
            return "HFP_AG_NOTIFY_SIGNAL_STRENGTH";
        case HFP_AG_NOTIFY_BATTERY_LEVEL:
            return "HFP_AG_NOTIFY_BATTERY_LEVEL";
        case HFP_AG_SDP_DISCOVERY_RESULT_SUCCESS:
            return "HFP_AG_SDP_DISCOVERY_RESULT_SUCCESS";
        case HFP_AG_SDP_DISCOVERY_RESULT_FAIL:
            return "HFP_AG_SDP_DISCOVERY_RESULT_FAIL";
        case HFP_AG_CONNECT_REQUEST_EVT:
            return "HFP_AG_CONNECT_REQUEST_EVT";
        case HFP_AG_CONNECTED_EVT:
            return "HFP_AG_CONNECTED_EVT";
        case HFP_AG_DISCONNECTED_EVT:
            return "HFP_AG_DISCONNECTED_EVT";
        case HFP_AG_CONNECT_FAILED_EVT:
            return "HFP_AG_CONNECT_FAILED_EVT";
        case HFP_AG_DISCONNECT_FAILED_EVT:
            return "HFP_AG_DISCONNECT_FAILED_EVT";
        case HFP_AG_DATA_AVAILABLE_EVT:
            return "HFP_AG_DATA_AVAILABLE_EVT";
        case HFP_AG_CODEC_NEGOTIATION_FAILED:
            return "HFP_AG_CODEC_NEGOTIATION_FAILED";
        case HFP_AG_SETUP_CODEC_CVSD:
            return "HFP_AG_SETUP_CODEC_CVSD";
        case HFP_AG_SLC_ESTABLISHED_EVT:
            return "HFP_AG_SLC_ESTABLISHED_EVT";
        case HFP_AG_CODEC_NEGOTIATED_EVT:
            return "HFP_AG_CODEC_NEGOTIATED_EVT";
        case HFP_AG_AUDIO_CONNECT_REQUEST_EVT:
            return "HFP_AG_AUDIO_CONNECT_REQUEST_EVT";
        case HFP_AG_AUDIO_CONNECTING_EVT:
            return "HFP_AG_AUDIO_CONNECTING_EVT";
        case HFP_AG_AUDIO_DISCONNECTING_EVT:
            return "HFP_AG_AUDIO_DISCONNECTING_EVT";
        case HFP_AG_AUDIO_CONNECTED_EVT:
            return "HFP_AG_AUDIO_CONNECTED_EVT";
        case HFP_AG_AUDIO_DISCONNECTED_EVT:
            return "HFP_AG_AUDIO_DISCONNECTED_EVT";
        case HFP_AG_AUDIO_CONNECT_FAILED_EVT:
            return "HFP_AG_AUDIO_CONNECT_FAILED_EVT";
        case HFP_AG_AUDIO_DISCONNECT_FAILED_EVT:
            return "HFP_AG_AUDIO_DISCONNECT_FAILED_EVT";
        case HFP_AG_RESPONSE_CLCC_TIME_OUT_EVT:
            return "HFP_AG_RESPONSE_CLCC_TIME_OUT_EVT";
        case HFP_AG_DIAL_TIME_OUT_EVT:
            return "HFP_AG_DIAL_TIME_OUT_EVT";
        case HFP_AG_RING_TIMEOUT_EVT:
            return "HFP_AG_RING_TIMEOUT_EVT";
        default:
            return "Unknown";
    }
}
}  // namespace bluetooth
}  // namespace OHOS
