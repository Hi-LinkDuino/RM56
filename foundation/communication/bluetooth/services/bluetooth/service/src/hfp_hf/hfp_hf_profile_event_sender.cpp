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

#include "hfp_hf_profile_event_sender.h"

#include "hfp_hf_defines.h"

namespace OHOS {
namespace bluetooth {
HfpHfProfileEventSender &HfpHfProfileEventSender::GetInstance()
{
    static HfpHfProfileEventSender profileEventSender;
    return profileEventSender;
}

void HfpHfProfileEventSender::SendMessageToService(const HfpHfMessage &event)
{
    HfpHfService *service = HfpHfService::GetService();
    if (service != nullptr) {
        service->PostEvent(event);
    } else {
        LOG_ERROR("[HFP HF]%{public}s():hfpHfService_ is nullptr!", __FUNCTION__);
    }
}

void HfpHfProfileEventSender::NetworkStateChanged(const std::string &address, int state) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT, state);
    event.dev_ = address;
    event.type_ = HFP_HF_TYPE_NETWORK_STATE;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::NetworkRoamingChanged(const std::string &address, int state) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT, state);
    event.dev_ = address;
    event.type_ = HFP_HF_TYPE_NETWORK_ROAM;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::NetworkSignalChanged(const std::string &address, int signal) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT, signal);
    event.dev_ = address;
    event.type_ = HFP_HF_TYPE_NETWORK_SIGNAL;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::BatteryLevelChanged(const std::string &address, int level) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT, level);
    event.dev_ = address;
    event.type_ = HFP_HF_TYPE_BATTERY_LEVEL;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::CurrentOperatorReply(const std::string &address, const std::string &name) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT);
    event.dev_ = address;
    event.str_ = name;
    event.type_ = HFP_HF_TYPE_CURRENT_OPERATOR;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::CallStateNotify(const std::string &address, int call) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT, call);
    event.dev_ = address;
    event.type_ = HFP_HF_TYPE_CALL_STATE;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::CallSetupStateNotify(const std::string &address, int callsetup) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT, callsetup);
    event.dev_ = address;
    event.type_ = HFP_HF_TYPE_CALL_SETUP_STATE;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::CallHeldStateNotify(const std::string &address, int callheld) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT, callheld);
    event.dev_ = address;
    event.type_ = HFP_HF_TYPE_CALL_HELD_STATE;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::HoldResultReply(const std::string &address, int respAndHold) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT, respAndHold);
    event.dev_ = address;
    event.type_ = HFP_HF_TYPE_HOLD_RESULT;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::Callinglineidentification(const std::string &address, const std::string &number) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT);
    event.dev_ = address;
    event.str_ = number;
    event.type_ = HFP_HF_TYPE_CALLING_LINE_IDENTIFICATION;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::CallWaitingNotify(const std::string &address, const std::string &number) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT);
    event.dev_ = address;
    event.str_ = number;
    event.type_ = HFP_HF_TYPE_CALL_WAITING;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::CurrentCallsReply(const std::string &address, const HfpHfCurrentCallData &call) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT);
    event.dev_ = address;
    event.type_ = HFP_HF_TYPE_CURRENT_CALLS;
    event.call_ = call;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::SetVolume(const std::string &address, int type, int volume) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT, type);
    event.arg3_ = volume;
    event.dev_ = address;
    event.type_ = HFP_HF_TYPE_SET_VOLUME;
    SendMessageToService(event);
}
void HfpHfProfileEventSender::AtCmdResultReply(const std::string &address, int type, int cme) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT, type);
    event.arg3_ = cme;
    event.dev_ = address;
    event.type_ = HFP_HF_TYPE_AT_CMD_RESULT;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::SubscriberNumberReply(const std::string &address, const std::string &number, int type) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT, type);
    event.dev_ = address;
    event.str_ = number;
    event.type_ = HFP_HF_TYPE_SUBSCRIBER_NUMBER;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::SetInBandRing(const std::string &address, int inBand, bool isFirstTime) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT, inBand);
    event.dev_ = address;
    event.type_ = HFP_HF_TYPE_INBAND_RING;
    event.arg3_ = isFirstTime;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::RingIndicationNotify(const std::string &address) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT);
    event.dev_ = address;
    event.type_ = HFP_HF_TYPE_RING_ALERT;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::UnknownEventNotify(const std::string &address, const std::string &eventString) const
{
    HfpHfMessage event(HFP_HF_INTERACTIVE_EVT);
    event.dev_ = address;
    event.type_ = HFP_HF_TYPE_UNKNOWN;
    event.str_ = eventString;
    SendMessageToService(event);
}

void HfpHfProfileEventSender::ConnectRequest(const std::string &device, int handle, int what) const
{
    HfpHfMessage msg(what);
    msg.dev_ = device;
    msg.arg1_ = handle;
    SendMessageToService(msg);
}

void HfpHfProfileEventSender::UpdateConnectState(const std::string &device, int what) const
{
    HfpHfMessage msg(what);
    msg.dev_ = device;
    SendMessageToService(msg);
}

void HfpHfProfileEventSender::ScoConnectRequest(const std::string &device, int what, uint8_t linkType) const
{
    HfpHfMessage msg(what);
    msg.arg1_ = linkType;
    msg.dev_ = device;
    SendMessageToService(msg);
}

void HfpHfProfileEventSender::UpdateScoConnectState(const std::string &device, int what) const
{
    HfpHfMessage msg(what);
    msg.dev_ = device;
    SendMessageToService(msg);
}

void HfpHfProfileEventSender::ProcessSdpDiscoveryResult(const std::string &device, int what) const
{
    HfpHfMessage msg(what);
    msg.dev_ = device;
    SendMessageToService(msg);
}

void HfpHfProfileEventSender::RemoveSateMachine(const std::string &address) const
{
    HfpHfMessage msg(HFP_HF_REMOVE_STATE_MACHINE_EVT);
    msg.dev_ = address;
    SendMessageToService(msg);
}

void HfpHfProfileEventSender::CurrentCallsReplyDone(const std::string &address) const
{
    HfpHfMessage msg(HFP_HF_INTERACTIVE_EVT);
    msg.dev_ = address;
    msg.type_ = HFP_HF_TYPE_QUERY_CURRENT_CALLS_DONE;
    SendMessageToService(msg);
}

void HfpHfProfileEventSender::OpenVoiceRecognitionResult(const std::string &address, int result) const
{
    HfpHfMessage msg(HFP_HF_INTERACTIVE_EVT, result);
    msg.dev_ = address;
    msg.type_ = HFP_HF_TYPE_OPEN_VR_RESULT;
    SendMessageToService(msg);
}

void HfpHfProfileEventSender::CloseVoiceRecognitionResult(const std::string &address, int result) const
{
    HfpHfMessage msg(HFP_HF_INTERACTIVE_EVT, result);
    msg.dev_ = address;
    msg.type_ = HFP_HF_TYPE_CLOSE_VR_RESULT;
    SendMessageToService(msg);
}

void HfpHfProfileEventSender::VoiceRecognitionChanged(const std::string &address, int status) const
{
    HfpHfMessage msg(HFP_HF_INTERACTIVE_EVT, status);
    msg.dev_ = address;
    msg.type_ = HFP_HF_TYPE_VOICE_RECOGNITION_CHANGED;
    SendMessageToService(msg);
}

utility::Dispatcher *HfpHfProfileEventSender::GetDispatchter() const
{
    HfpHfService *service = HfpHfService::GetService();
    if (service != nullptr) {
        return service->GetDispatcher();
    } else {
        LOG_ERROR("[HFP AG]%{public}s():hfpAgService_ is nullptr!", __FUNCTION__);
        return nullptr;
    }
}
}  // namespace bluetooth
}  // namespace OHOS