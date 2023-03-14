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

#include "hfp_ag_system_event_processer.h"

#include "hfp_ag_defines.h"
#include "hfp_ag_message.h"
#include "hfp_ag_service.h"

namespace OHOS {
namespace bluetooth {
HfpAgSystemEventProcesser::HfpAgSystemEventProcesser(HfpAgProfile &profile, const std::string &address)
    :profile_(profile), address_(address)
{
}

void HfpAgSystemEventProcesser::Start()
{
    HfpAgSystemInterface::GetInstance().Start();
    return;
}

void HfpAgSystemEventProcesser::Stop()
{
    HfpAgSystemInterface::GetInstance().Stop();
    return;
}

void HfpAgSystemEventProcesser::ExecuteEventProcess(const HfpAgMessage &event)
{
    LOG_INFO("[HFP AG]%{public}s():[%{public}s]", __FUNCTION__, GetEventType(event.type_).c_str());
    switch (event.type_) {
        case HFP_AG_MSG_TYPE_ANSWER_CALL:
            ProcessAnswerCallEvent();
            break;
        case HFP_AG_MSG_TYPE_HANGUP_CALL:
            ProcessHangupCallEvent();
            break;
        case HFP_AG_MSG_TYPE_VOLUME_CHANGED:
            ProcessHfVolumeChangedEvent(event.arg1_, event.arg3_);
            break;
        case HFP_AG_MSG_TYPE_DIAL_CALL:
            ProcessDialOutCallEvent(event.str_, event.arg1_);
            break;
        case HFP_AG_MSG_TYPE_SEND_DTMF:
            ProcessSendDtmfEvent(event.arg1_);
            break;
        case HFP_AG_MSG_TYPE_NOISE_REDUCTION:
            ProcessNoiseReductionEvent(event.arg1_);
            break;
        case HFP_AG_MSG_TYPE_AT_WBS:
            ProcessWideBandSpeechEvent(event.arg1_);
            break;
        case HFP_AG_MSG_TYPE_AT_CHLD:
            ProcessHoldCallEvent(event.arg1_);
            break;
        case HFP_AG_MSG_TYPE_SUBSCRIBER_NUMBER_REQUEST:
            GetSubscriberNumber();
            break;
        case HFP_AG_MSG_TYPE_AT_CIND:
            GetAgIndicator();
            break;
        case HFP_AG_MSG_TYPE_AT_COPS:
            GetNetworkOperator();
            break;
        case HFP_AG_MSG_TYPE_AT_CLCC:
            GetCurrentCalls();
            break;
        case HFP_AG_MSG_TYPE_AT_UNKNOWN:
            ProcessAtUnknownEvent(event.str_);
            break;
        case HFP_AG_MSG_TYPE_KEY_PRESSED:
            ProcessKeyPressedEvent();
            break;
        case HFP_AG_MSG_TYPE_AT_BIND:
            ProcessATBindEvent(event.str_);
            break;
        case HFP_AG_MSG_TYPE_AT_BIEV:
            SendHfIndicator(event.arg1_, event.arg3_);
            break;
        case HFP_AG_MSG_TYPE_AT_BIA:
            ProcessAtBiaEvent(event);
            break;
        case HFP_AG_MSG_TYPE_QUERY_AG_INDICATOR:
            QueryAgIndicator();
            break;
        case HFP_AG_MSG_TYPE_VR_CHANGED:
            VoiceRecognitionStateChanged(event.arg1_);
            break;
        case HFP_AG_NOTIFY_INDICATOR_EVT:
            UpdataAgIndicator();
            break;
        case HFP_AG_GET_VOICE_NUMBER:
            GetVoiceNumber();
            break;
        case HFP_AG_GET_BTRH_EVT:
            GetResponseHoldState(event.dev_);
            break;
        case HFP_AG_SET_BTRH_EVT:
            SetResponseHold(event.dev_, event.arg1_);
            break;
        case HFP_AG_CALL_STATE_CHANGE_MOCK:
            HandlePhoneStateMock(event.state_.number, event.state_.callState, event.state_.type);
            break;
        default:
            GetCurrentCalls();
            break;
    }
}

void HfpAgSystemEventProcesser::ProcessAnswerCallEvent() const
{
    LOG_INFO("[HFP AG]%{public}s():Answer call! address:%{public}s", __FUNCTION__, address_.c_str());
    systemInterface_.AnswerCall(address_);
}

void HfpAgSystemEventProcesser::ProcessHangupCallEvent() const
{
    LOG_INFO("[HFP AG]%{public}s():Hang up call! address:%{public}s", __FUNCTION__, address_.c_str());
    systemInterface_.HangupCall(address_);
}

void HfpAgSystemEventProcesser::ProcessHfVolumeChangedEvent(int type, int volume) const
{
    // the devcie can't change SCO volume if it is not the active device.
    if (HfpAgProfile::GetActiveDevice() != address_) {
        LOG_INFO("[HFP AG]%{public}s():This device is not active!", __FUNCTION__);
        return;
    }
    int flag = 1;
    if (type == HFP_AG_VOLUME_TYPE_SPK) {
        systemInterface_.SetStreamVolume(type, volume, flag);
    } else if (type == HFP_AG_VOLUME_TYPE_MIC) {
        systemInterface_.SetStreamVolume(type, volume, flag);
    } else {
        LOG_ERROR("[HFP AG]%{public}s():The error volume type:%{public}d", __FUNCTION__, type);
    }
}

void HfpAgSystemEventProcesser::ProcessDialOutCallEvent(const std::string &number, int dialType)
{
    std::string dialNumber;
    if (dialType == HFP_AG_LAST_NUMBER_REDIAL) {
        dialNumber = systemInterface_.GetLastDialNumber();
    } else if (dialType == HFP_AG_MEMORY_DIAL) {
        // Before phone service supply real memory dial support,
        // use last dial number for memory dial function.
        dialNumber = systemInterface_.GetLastDialNumber();
    } else if (dialType == HFP_AG_NUMBER_DIAL) {
        dialNumber = number;
    } else {
        LOG_ERROR("[HFP AG]%{public}s():: dialType[%{public}d] is invalid", __FUNCTION__, dialType);
        return;
    }

    LOG_INFO("[HFP AG]%{public}s():: number[%{public}s], dialNumber[%{public}s], dialType[%{public}d]",
        __FUNCTION__,
        number.c_str(),
        dialNumber.c_str(),
        dialType);

    if (dialNumber.length() == 0) {
        profile_.SendResultCode(HFP_AG_RESULT_ERROR);
        return;
    }

    HfpAgService *service = HfpAgService::GetService();

    if (service != nullptr) {
        if (service->DialOutCallByHf(address_)) {
            systemInterface_.DialOutCall(address_, dialNumber);
        } else {
            profile_.SendResultCode(HFP_AG_RESULT_ERROR);
        }
    } else {
        profile_.SendResultCode(HFP_AG_RESULT_ERROR);
    }
}

void HfpAgSystemEventProcesser::ProcessSendDtmfEvent(int dtmf) const
{
    profile_.SendResultCode(HFP_AG_RESULT_OK);
    systemInterface_.SendDtmf(dtmf, address_);
}

void HfpAgSystemEventProcesser::ProcessNoiseReductionEvent(int flag) const
{
    std::string strValue("bt_headset_nrec+");
    bool enable = (flag == 0) ? false : true;
    if (enable) {
        strValue.append("on");
    } else {
        strValue.append("off");
    }
    systemInterface_.SetAudioParameters(strValue);
}

void HfpAgSystemEventProcesser::ProcessWideBandSpeechEvent(int codec) const
{
    std::string strValue("bt_wbs+");
    switch (codec) {
        case HFP_AG_WBS_YES:
            strValue.append("on");
            break;
        case HFP_AG_WBS_NONE:
        case HFP_AG_WBS_NO:
            strValue.append("off");
            break;
        default:
            strValue.clear();
            break;
    }
    systemInterface_.SetAudioParameters(strValue);
}

void HfpAgSystemEventProcesser::ProcessHoldCallEvent(int chld) const
{
    bool result = systemInterface_.HoldCall(chld);
    HfpAgService *service = HfpAgService::GetService();
    int mock = service->GetMockState();
    if (mock != HfpAgMockState::HFP_AG_MOCK) {
        if (result) {
            profile_.SendResultCode(HFP_AG_RESULT_OK);
        } else {
            profile_.SendResultCode(HFP_AG_RESULT_ERROR);
        }
    }
}

void HfpAgSystemEventProcesser::GetSubscriberNumber() const
{
    std::string number = systemInterface_.GetSubscriberNumber();
    if (!number.empty()) {
        int type;
        if (number[0] == '+') {
            type = HFP_AG_CALL_NUMBER_INTERNATIONAL;
        } else {
            type = HFP_AG_CALL_NUMBER_UNKNOW;
        }
        profile_.SendSubscriberNumberInformation(type, number);
        profile_.SendResultCode(HFP_AG_RESULT_OK);
    } else {
        profile_.SendResultCode(HFP_AG_RESULT_ERROR);
    }
}

void HfpAgSystemEventProcesser::GetAgIndicator()
{
    int activeNum = systemInterface_.GetActiveCallNumber();
    int heldNum = systemInterface_.GetHeldCallNumber();
    int callState = systemInterface_.GetCallState();

    LOG_INFO("[HFP AG]%{public}s():activeNum[%{public}d], heldNum[%{public}d], callState[%{public}d]!", __FUNCTION__, activeNum, heldNum, callState);

    int call = HFP_AG_CALL_INACTIVE;
    if (activeNum + heldNum > 0) {
        call = HFP_AG_CALL_ACTIVE;
    }

    int callsetup = HFP_AG_CALLSETUP_NONE;
    if (callState == HFP_AG_CALL_STATE_INCOMING) {
        callsetup = HFP_AG_CALLSETUP_INCOMING;
    } else if (callState == HFP_AG_CALL_STATE_DIALING) {
        callsetup = HFP_AG_CALLSETUP_OUTGOING;
    } else if (callState == HFP_AG_CALL_STATE_ALERTING) {
        callsetup = HFP_AG_CALLSETUP_ALERTING;
    } else {
        LOG_ERROR("[HFP AG]%{public}s():The error state of callsetup!", __FUNCTION__);
    }

    int callheld = HFP_AG_CALLHELD_INACTIVE;
    if (heldNum == 0) {
        callheld = 0;
    } else if (heldNum > 0 && activeNum > 0) {
        callheld = HFP_AG_CALLHELD_ACTIVE;
    } else if (heldNum > 0 && activeNum == 0) {
        callheld = HFP_AG_CALLHELD_NOACTIVE;
    } else {
        LOG_ERROR("[HFP AG]%{public}s():The error state of callheld!", __FUNCTION__);
    }

    int status = systemInterface_.GetServiceState();
    int signal = systemInterface_.GetSignalStrength();
    int roam = systemInterface_.GetRoamState();
    int battery = systemInterface_.GetBatteryLevel();

    HfpAgProfile::CindIndicators indicators = {status, call, callsetup, callheld, signal, roam, battery};
    profile_.ReportIndicators(indicators);
}

void HfpAgSystemEventProcesser::GetNetworkOperator() const
{
    std::string networkOperator = systemInterface_.GetNetworkOperator();
    profile_.ReportCurrentNetworkOperator(networkOperator);
}

void HfpAgSystemEventProcesser::GetCurrentCalls() const
{
    HfpAgService *service = HfpAgService::GetService();
    if (service != nullptr) {
        service->SetResponseClccTimer(address_);
    }
    systemInterface_.QueryCurrentCallsList(address_);
}

void HfpAgSystemEventProcesser::ProcessAtUnknownEvent(const std::string &atString) const
{}

void HfpAgSystemEventProcesser::ProcessKeyPressedEvent() const
{}

void HfpAgSystemEventProcesser::ProcessATBindEvent(const std::string &atString) const
{}

void HfpAgSystemEventProcesser::SendHfIndicator(int indId, int indValue) const
{
    systemInterface_.SendHfIndicator(address_, indId, indValue);
}

void HfpAgSystemEventProcesser::ProcessAtBiaEvent(const HfpAgMessage &event)
{
    LOG_INFO("[HFP AG]%{public}s(): BIA command is battery[%{public}d], roam[%{public}d], "
        "service[%{public}d], signal[%{public}d]",
        __FUNCTION__,
        event.data_.battery,
        event.data_.roam,
        event.data_.service,
        event.data_.signal);
    profile_.ProcessBia(event.data_.service, event.data_.roam, event.data_.signal, event.data_.battery);
}

void HfpAgSystemEventProcesser::QueryAgIndicator()
{
    systemInterface_.QueryAgIndicator();
}

void HfpAgSystemEventProcesser::UpdataAgIndicator()
{
    systemInterface_.UpdateCallList();
    systemInterface_.UpdateAgIndicator();
}

void HfpAgSystemEventProcesser::GetVoiceNumber()
{
    systemInterface_.GetVoiceNumber();
}

void HfpAgSystemEventProcesser::GetResponseHoldState(std::string address)
{
    systemInterface_.GetResponseHoldState(address);
}

void HfpAgSystemEventProcesser::SetResponseHold(std::string address, int btrh)
{
    systemInterface_.SetResponseHoldState(address, btrh);
}

void HfpAgSystemEventProcesser::HandlePhoneStateMock(std::string number, int state, int type)
{
    systemInterface_.HandlePhoneStateMock(number, state, type);
}


void HfpAgSystemEventProcesser::VoiceRecognitionStateChanged(int status)
{
    HfpAgService *service = HfpAgService::GetService();
    if (service != nullptr) {
        if (status == HFP_AG_HF_VR_OPENED) {
            if (service->OpenVoiceRecognitionByHf(address_)) {
                systemInterface_.StartVoiceRecognition(address_);
                profile_.SendResultCode(HFP_AG_RESULT_OK);
            } else {
                profile_.SendResultCode(HFP_AG_RESULT_ERROR);
            }
        } else if (status == HFP_AG_HF_VR_ClOSED) {
            if (service->CloseVoiceRecognitionByHf(address_)) {
                profile_.SendResultCode(HFP_AG_RESULT_OK);
            } else {
                profile_.SendResultCode(HFP_AG_RESULT_ERROR);
            }
        } else {
            profile_.SendResultCode(HFP_AG_RESULT_ERROR);
        }
    } else {
        profile_.SendResultCode(HFP_AG_RESULT_ERROR);
    }
}

std::string HfpAgSystemEventProcesser::GetEventType(int type)
{
    switch (type) {
        case HFP_AG_MSG_TYPE_TYPE_NONE:
            return "HFP_AG_MSG_TYPE_TYPE_NONE";
        case HFP_AG_MSG_TYPE_ANSWER_CALL:
            return "HFP_AG_MSG_TYPE_ANSWER_CALL";
        case HFP_AG_MSG_TYPE_HANGUP_CALL:
            return "HFP_AG_MSG_TYPE_HANGUP_CALL";
        case HFP_AG_MSG_TYPE_VOLUME_CHANGED:
            return "HFP_AG_MSG_TYPE_VOLUME_CHANGED";
        case HFP_AG_MSG_TYPE_DIAL_CALL:
            return "HFP_AG_MSG_TYPE_DIAL_CALL";
        case HFP_AG_MSG_TYPE_SEND_DTMF:
            return "HFP_AG_MSG_TYPE_SEND_DTMF";
        case HFP_AG_MSG_TYPE_NOISE_REDUCTION:
            return "HFP_AG_MSG_TYPE_NOISE_REDUCTION";
        case HFP_AG_MSG_TYPE_AT_WBS:
            return "HFP_AG_MSG_TYPE_AT_WBS";
        case HFP_AG_MSG_TYPE_AT_CHLD:
            return "HFP_AG_MSG_TYPE_AT_CHLD";
        case HFP_AG_MSG_TYPE_SUBSCRIBER_NUMBER_REQUEST:
            return "HFP_AG_MSG_TYPE_SUBSCRIBER_NUMBER_REQUEST";
        case HFP_AG_MSG_TYPE_AT_CIND:
            return "HFP_AG_MSG_TYPE_AT_CIND";
        case HFP_AG_MSG_TYPE_AT_COPS:
            return "HFP_AG_MSG_TYPE_AT_COPS";
        case HFP_AG_MSG_TYPE_AT_CLCC:
            return "HFP_AG_MSG_TYPE_AT_CLCC";
        case HFP_AG_MSG_TYPE_AT_UNKNOWN:
            return "HFP_AG_MSG_TYPE_AT_UNKNOWN";
        case HFP_AG_MSG_TYPE_KEY_PRESSED:
            return "HFP_AG_MSG_TYPE_KEY_PRESSED";
        case HFP_AG_MSG_TYPE_AT_BIND:
            return "HFP_AG_MSG_TYPE_AT_BIND";
        case HFP_AG_MSG_TYPE_AT_BIEV:
            return "HFP_AG_MSG_TYPE_AT_BIEV";
        case HFP_AG_MSG_TYPE_AT_BIA:
            return "HFP_AG_MSG_TYPE_AT_BIA";
        case HFP_AG_MSG_TYPE_QUERY_AG_INDICATOR:
            return "HFP_AG_MSG_TYPE_QUERY_AG_INDICATOR";
        default:
            return "Unknown";
    }
}
}  // namespace bluetooth
}  // namespace OHOS