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

#include "hfp_ag_profile_event_sender.h"

#include "hfp_ag_defines.h"
#include "hfp_ag_service.h"

namespace OHOS {
namespace bluetooth {
HfpAgProfileEventSender &HfpAgProfileEventSender::GetInstance()
{
    static HfpAgProfileEventSender hfpAgUplinkService;
    return hfpAgUplinkService;
}

void HfpAgProfileEventSender::SendMessageToService(const HfpAgMessage &msg)
{
    HfpAgService *service = HfpAgService::GetService();
    if (service != nullptr) {
        service->PostEvent(msg);
    }
}

void HfpAgProfileEventSender::AnswerCall(const std::string &device) const
{
    HfpAgMessage msg(HFP_AG_CONTROL_OTHER_MODULES_EVT);
    msg.dev_ = device;
    msg.type_ = HFP_AG_MSG_TYPE_ANSWER_CALL;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::HangupCall(const std::string &device) const
{
    HfpAgMessage msg(HFP_AG_CONTROL_OTHER_MODULES_EVT);
    msg.dev_ = device;
    msg.type_ = HFP_AG_MSG_TYPE_HANGUP_CALL;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::HfVolumeChanged(const std::string &device, int type, int volume) const
{
    HfpAgMessage msg(HFP_AG_CONTROL_OTHER_MODULES_EVT, type);
    msg.arg3_ = volume;
    msg.dev_ = device;
    msg.type_ = HFP_AG_MSG_TYPE_VOLUME_CHANGED;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::DialOutCall(const std::string &device, const std::string &number, int dialType) const
{
    HfpAgMessage msg(HFP_AG_CONTROL_OTHER_MODULES_EVT);
    msg.dev_ = device;
    msg.str_ = number;
    msg.type_ = HFP_AG_MSG_TYPE_DIAL_CALL;
    msg.arg1_ = dialType;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::SendDtmf(const std::string &device, int dtmf) const
{
    HfpAgMessage msg(HFP_AG_CONTROL_OTHER_MODULES_EVT, dtmf);
    msg.dev_ = device;
    msg.type_ = HFP_AG_MSG_TYPE_SEND_DTMF;
    msg.arg1_ = dtmf;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::EnableNoiseReduction(const std::string &device, bool enable) const
{
    HfpAgMessage msg(HFP_AG_CONTROL_OTHER_MODULES_EVT);
    msg.dev_ = device;
    msg.type_ = HFP_AG_MSG_TYPE_NOISE_REDUCTION;
    msg.arg1_ = enable ? 1 : 0;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::VoiceRecognitionStateChanged(const std::string &device, int status) const
{
    HfpAgMessage msg(HFP_AG_CONTROL_OTHER_MODULES_EVT);
    msg.dev_ = device;
    msg.type_ = HFP_AG_MSG_TYPE_VR_CHANGED;
    msg.arg1_ = status;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::HoldCall(const std::string &device, int chld) const
{
    HfpAgMessage msg(HFP_AG_CONTROL_OTHER_MODULES_EVT, chld);
    msg.dev_ = device;
    msg.type_ = HFP_AG_MSG_TYPE_AT_CHLD;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::GetSubscriberNumber(const std::string &device) const
{
    HfpAgMessage msg(HFP_AG_CONTROL_OTHER_MODULES_EVT);
    msg.dev_ = device;
    msg.type_ = HFP_AG_MSG_TYPE_SUBSCRIBER_NUMBER_REQUEST;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::GetAgIndicator(const std::string &device) const
{
    HfpAgMessage msg(HFP_AG_CONTROL_OTHER_MODULES_EVT);
    msg.dev_ = device;
    msg.type_ = HFP_AG_MSG_TYPE_AT_CIND;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::GetNetworkOperater(const std::string &device) const
{
    HfpAgMessage msg(HFP_AG_CONTROL_OTHER_MODULES_EVT);
    msg.dev_ = device;
    msg.type_ = HFP_AG_MSG_TYPE_AT_COPS;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::GetCurrentCalls(const std::string &device) const
{
    HfpAgMessage msg(HFP_AG_CONTROL_OTHER_MODULES_EVT);
    msg.dev_ = device;
    msg.type_ = HFP_AG_MSG_TYPE_AT_CLCC;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::ProcessATBind(const std::string &device, const std::string &atString) const
{
    HfpAgMessage msg(HFP_AG_CONTROL_OTHER_MODULES_EVT);
    msg.dev_ = device;
    msg.str_ = atString;
    msg.type_ = HFP_AG_MSG_TYPE_AT_BIND;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::SendHfIndicator(const std::string &device, int indId, int indValue) const
{
    HfpAgMessage msg(HFP_AG_CONTROL_OTHER_MODULES_EVT, indId);
    msg.arg3_ = indValue;
    msg.dev_ = device;
    msg.type_ = HFP_AG_MSG_TYPE_AT_BIEV;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::ProcessAtBia(const std::string &device, const HfpAgTransferData &data) const
{
    HfpAgMessage msg(HFP_AG_CONTROL_OTHER_MODULES_EVT);
    msg.dev_ = device;
    msg.type_ = HFP_AG_MSG_TYPE_AT_BIA;
    msg.data_ = data;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::ConnectRequest(const std::string &device, int handle, int what) const
{
    HfpAgMessage msg(what);
    msg.dev_ = device;
    msg.arg1_ = handle;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::UpdateConnectState(const std::string &device, int what) const
{
    HfpAgMessage msg(what);
    msg.dev_ = device;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::ScoConnectRequest(const std::string &device, int what, uint8_t linkType) const
{
    HfpAgMessage msg(what);
    msg.arg1_ = linkType;
    msg.dev_ = device;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::UpdateScoConnectState(const std::string &device, int what) const
{
    HfpAgMessage msg(what);
    msg.dev_ = device;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::GetResponseHoldState(const std::string &device, int what) const
{
    HfpAgMessage msg(what);
    msg.dev_ = device;
    msg.type_ = what;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::SetResponseHoldState(const std::string &device, int what, int btrh) const
{
    HfpAgMessage msg(what);
    msg.dev_ = device;
    msg.type_ = what;
    msg.arg1_ = btrh;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::ProcessSdpDiscoveryResult(const std::string &device, int what) const
{
    HfpAgMessage msg(what);
    msg.dev_ = device;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::ProcessAtBcc(const std::string &device) const
{
    HfpAgMessage msg(HFP_AG_CONNECT_AUDIO_EVT);
    msg.dev_ = device;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::ProcessAtBcs(const std::string &device) const
{
    HfpAgMessage msg(HFP_AG_CODEC_NEGOTIATED_EVT);
    msg.dev_ = device;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::RemoveStateMachine(const std::string &device) const
{
    HfpAgMessage msg(HFP_AG_REMOVE_STATE_MACHINE_EVT);
    msg.dev_ = device;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::SendRingAndClip(const std::string &device) const
{
    HfpAgMessage msg(HFP_AG_RING_TIMEOUT_EVT);
    msg.dev_ = device;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::ProcessCKpdEvent(const std::string &device) const
{
    HfpAgMessage msg(HFP_AG_PROCESS_CKPD_EVT);
    msg.dev_ = device;
    SendMessageToService(msg);
}

void HfpAgProfileEventSender::GetVoiceTagNumber(const std::string &device) const
{
    HfpAgMessage msg(HFP_AG_GET_VOICE_NUMBER);
    msg.dev_ = device;
    SendMessageToService(msg);
}

utility::Dispatcher *HfpAgProfileEventSender::GetDispatchter() const
{
    HfpAgService *service = HfpAgService::GetService();
    if (service != nullptr) {
        return service->GetDispatcher();
    } else {
        LOG_ERROR("[HFP AG]%{public}s():hfpAgService_ is nullptr!", __FUNCTION__);
        return nullptr;
    }
}
}  // namespace bluetooth
}  // namespace OHOS
