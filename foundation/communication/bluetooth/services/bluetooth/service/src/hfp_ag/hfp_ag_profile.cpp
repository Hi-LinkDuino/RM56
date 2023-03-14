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

#include "hfp_ag_profile.h"

#include "btm.h"
#include "hfp_ag_command_processor.h"
#include "hfp_ag_defines.h"
#include "hfp_ag_profile_event_sender.h"
#include "hfp_ag_sdp_server.h"
#include "interface_profile.h"
#include "power_manager.h"

namespace OHOS {
namespace bluetooth {
#define HFP_AG_RETURN_IF_NOT_CONNECTED(state)                                       \
    do {                                                                            \
        if (!state) {                                                               \
            LOG_ERROR("[HFP AG]%{public}s():SLC connection not established", __FUNCTION__); \
            return HFP_AG_FAILURE;                                                  \
        }                                                                           \
    } while (0)

#define HFP_AG_RETURN_IF_NOT_ENABLED(enable)                                 \
    do {                                                                  \
        if (!enable) {                                                    \
            LOG_ERROR("[HFP AG]%{public}s():function not allowed", __FUNCTION__); \
            return HFP_AG_FAILURE;                                        \
        }                                                                 \
    } while (0)

#define HFP_AG_RETURN_IF_NO_FEATURE(feature, mask)                         \
    do {                                                                   \
        if (!(feature & mask)) {                                           \
            LOG_ERROR("[HFP AG]%{public}s():feature not supported", __FUNCTION__); \
            return HFP_AG_FAILURE;                                         \
        }                                                                  \
    } while (0)

#define HFP_AG_RETURN_IF_INDICATOR_EQUAL(ind1, ind2)                                                    \
    do {                                                                                                \
        if (ind1 == ind2) {                                                                             \
            LOG_WARN("[HFP AG]%{public}s():Indicator wouldn't be sent for its value not change", __FUNCTION__); \
            return HFP_AG_SUCCESS;                                                                      \
        }                                                                                               \
    } while (0)

int HfpAgProfile::RegisterService()
{
    HfpAgDataConnection::Init();

    HfpAgDataConnectionServer &connectionServer = HfpAgDataConnectionServer::GetInstance();
    int ret = HfpAgSdpServer::GetInstance().RegisterSdpService(connectionServer.AssignLocalScn());
    HFP_AG_RETURN_IF_FAIL(ret);

    BTM_AddLocalRfcommScnForLogging(BTM_HCI_LOG_FILTER_MODULE_HFP, connectionServer.GetLocalScn());

    ret = connectionServer.RegisterServer();
    HFP_AG_RETURN_IF_FAIL(ret);

    ret = HfpAgAudioConnection::Register();
    HFP_AG_RETURN_IF_FAIL(ret);

    return ret;
}

int HfpAgProfile::DeregisterService()
{
    HfpAgDataConnectionServer &connectionServer = HfpAgDataConnectionServer::GetInstance();
    int ret = HfpAgSdpServer::GetInstance().DeregisterSdpService();
    HFP_AG_RETURN_IF_FAIL(ret);

    BTM_RemoveLocalRfcommScnChannelForLogging(BTM_HCI_LOG_FILTER_MODULE_HFP, connectionServer.GetLocalScn());

    ret = connectionServer.RemoveServer();
    HFP_AG_RETURN_IF_FAIL(ret);

    ret = HfpAgAudioConnection::Deregister();
    HFP_AG_RETURN_IF_FAIL(ret);

    HfpAgDataConnection::CleanUp();

    return ret;
}

HfpAgProfile::HfpAgProfile(const std::string &address) : address_(address)
{
}

void HfpAgProfile::Init()
{
    dataConn_.SetRemoteAddr(address_);
    audioConn_.SetRemoteAddr(address_);
}

int HfpAgProfile::DoServiceDiscovery(int role, int handle)
{
    if (HFP_AG_ACCEPTOR == role) {
        tempRfcommHandle_ = handle;
    }

    dataConn_.SetRole(role);
    return sdpClient_.DoDiscovery(address_, role);
}

int HfpAgProfile::ServiceDiscoveryResult()
{
    int role = dataConn_.GetRole();
    if (!sdpClient_.FindAttributes(address_, role)) {
        LOG_ERROR("[HFP AG]%{public}s():FindAttributes() error", __FUNCTION__);
        return HFP_AG_FAILURE;
    }
    dataConn_.SetSdpInfo(sdpClient_.GetRemoteSdpInfo());

    if (HFP_AG_INITIATOR == role) {
        EstablishDataConnection();
    } else {
        AcceptDataConnection(tempRfcommHandle_);
        tempRfcommHandle_ = -1;
    }
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::EstablishDataConnection()
{
    RawAddress rawAddr(address_);
    BtAddr btAddr;
    rawAddr.ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;
    BTM_AddRemoteRfcommScnForLogging(BTM_HCI_LOG_FILTER_MODULE_HFP, dataConn_.GetRemoteScn(), &btAddr);
    return dataConn_.Connect();
}

int HfpAgProfile::ReleaseDataConnection() const
{
    return dataConn_.Disconnect();
}

int HfpAgProfile::AcceptDataConnection(uint16_t handle)
{
    dataServer_.AcceptConnection(handle);
    dataConn_.SetConnectionHandle(handle);
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::ReadData()
{
    RawAddress rawAddress = RawAddress(address_);
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_HFP_AG, rawAddress);
    HfpAgCommandParser::GetInstance().Read(dataConn_);
    if (HfpAgAudioConnection::IsAudioConnected(address_)) {
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::SCO_ON, PROFILE_NAME_HFP_AG, rawAddress);
    } else {
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_HFP_AG, rawAddress);
    }
    return HFP_AG_SUCCESS;
}

void HfpAgProfile::ProcessSlcEstablished()
{
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_ON, PROFILE_NAME_HFP_AG, RawAddress(address_));
    dataConn_.SetSlcConnectState(true);
    InitInbandRingTone();
}

void HfpAgProfile::InitInbandRingTone()
{
    int action = HFP_AG_INBAND_RING_DISABLE;
    uint32_t localFeatures = HfpAgDataConnection::GetLocalFeatures();
    if ((localFeatures & HFP_AG_FEATURES_IN_BAND_RING) == HFP_AG_FEATURES_IN_BAND_RING) {
        action = HFP_AG_INBAND_RING_ENABLE;
    }
    SetInbandRingTone(action);

    if (!dataConn_.ringTimer_) {
        dataConn_.ringTimer_ = std::make_unique<utility::Timer>(std::bind(&HfpAgProfile::RingTimeout, this));
    }
}

void HfpAgProfile::SendRingAndClip()
{
    if (dataConn_.callsetupInd_ != HFP_AG_CALLSETUP_INCOMING) {
        return;
    }

    // Send command RING
    SendRing();

    // Send incoming call line identification
    NotifyCallingLineIdentification(dataConn_.clipType_, dataConn_.clipNumber_);

    // Start Ring timer again
    if (dataConn_.ringTimer_ != nullptr) {
        dataConn_.ringTimer_->Start(HfpAgDataConnection::RING_TIMEOUT_MS, false);
    }
}

void HfpAgProfile::RingTimeout()
{
    HfpAgProfileEventSender::GetInstance().SendRingAndClip(address_);
}

int HfpAgProfile::SetupCodecConnection()
{
    if (dataConn_.IsCodecNegotiationSupport()) {
        if (dataConn_.remoteSupportCodecsUpdated_) {
            if ((dataConn_.localDemandCodec_ & HFP_AG_CODEC_MSBC) &&
                (dataConn_.g_localSupportCodecs & HFP_AG_CODEC_MSBC) &&
                (dataConn_.remoteSupportCodecs_ & HFP_AG_CODEC_MSBC)) {
                dataConn_.codecNegotiating_ = true;
                commandProcessor_.SendAtCommand(dataConn_, "+BCS: 2");
                dataConn_.localSelectedCodec_ = HFP_AG_CODEC_MSBC;
            } else {
                dataConn_.codecNegotiating_ = true;
                commandProcessor_.SendAtCommand(dataConn_, "+BCS: 1");
                dataConn_.localSelectedCodec_ = HFP_AG_CODEC_CVSD;
            }
            dataConn_.remoteSupportCodecsUpdated_ = false;
        } else {
            EstablishAudioConnection();
        }
    } else {
        dataConn_.inUseCodec_ = HFP_AG_CODEC_CVSD;
        EstablishAudioConnection();
    }

    return HFP_AG_SUCCESS;
}

int HfpAgProfile::SetupCodecCvsd()
{
    if (dataConn_.IsCodecNegotiationSupport()) {
        dataConn_.codecNegotiating_ = true;
        commandProcessor_.SendAtCommand(dataConn_, "+BCS: 1");
        dataConn_.localSelectedCodec_ = HFP_AG_CODEC_CVSD;
    } else {
        dataConn_.inUseCodec_ = HFP_AG_CODEC_CVSD;
        EstablishAudioConnection();
    }

    return HFP_AG_SUCCESS;
}

int HfpAgProfile::EstablishAudioConnection()
{
    audioConn_.SetSupportFeatures(dataConn_.IsEscoSupport(), dataConn_.IsEscoS4Support(), dataConn_.inUseCodec_);
    audioConn_.ConnectAudio();
    return HFP_AG_SUCCESS;
}

void HfpAgProfile::PostAudioConnectionEstablishment()
{
    if (scoPostProcess_ == true) {
        SendRingAndClip();
        scoPostProcess_ = false;
    }
}

int HfpAgProfile::ReleaseAudioConnection() const
{
    audioConn_.DisconnectAudio();
    return HFP_AG_SUCCESS;
}

bool HfpAgProfile::ProcessAudioConnectRequest()
{
    if (HfpAgAudioConnection::GetActiveDevice() == address_) {
        AcceptAudioConnection();
        return true;
    } else {
        RejectAudioConnection();
        return false;
    }
}

int HfpAgProfile::AcceptAudioConnection()
{
    audioConn_.SetSupportFeatures(dataConn_.IsEscoSupport(), dataConn_.IsEscoS4Support(), dataConn_.inUseCodec_);
    audioConn_.AcceptAudioConnection();
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::RejectAudioConnection() const
{
    audioConn_.RejectAudioConnection();
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::ActivateVoiceRecognition()
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
    HFP_AG_RETURN_IF_NO_FEATURE(dataConn_.remoteFeatures_, HFP_AG_HF_FEATURES_VOICE_RECOGNITION);

    std::string cmd("+BVRA: 1");
    commandProcessor_.SendAtCommand(dataConn_, cmd);

    if (HfpAgAudioConnection::IsAudioConnected(address_) == false) {
        HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(address_, HFP_AG_CONNECT_AUDIO_EVT);
    }
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::DeactivateVoiceRecognition() const
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
    HFP_AG_RETURN_IF_NO_FEATURE(dataConn_.remoteFeatures_, HFP_AG_HF_FEATURES_VOICE_RECOGNITION);

    std::string cmd("+BVRA: 0");
    commandProcessor_.SendAtCommand(dataConn_, cmd);
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::SetMicrophoneGain(int val) const
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    // Make sure volume is in the range 0-15
    int value = (val > 15) ? 15 : val;
    value = (value < 0) ? 0 : value;

    std::string cmd("+VGM: ");
    cmd.append(std::to_string(value));
    commandProcessor_.SendAtCommand(dataConn_, cmd);
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::SetSpeakerVolume(int val) const
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    // Make sure volume is in the range 0-15
    int value = (val > 15) ? 15 : val;
    value = (value < 0) ? 0 : value;

    std::string cmd("+VGS: ");
    cmd.append(std::to_string(value));
    commandProcessor_.SendAtCommand(dataConn_, cmd);
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::SetInbandRingTone(int action)
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    dataConn_.inBandRingTone_ = action;

    std::string cmd("+BSIR: ");
    cmd.append(std::to_string(action));
    commandProcessor_.SendAtCommand(dataConn_, cmd);
    return HFP_AG_SUCCESS;
}

void HfpAgProfile::SetActiveDevice(const std::string &address)
{
    HfpAgAudioConnection::SetActiveDevice(address);
}

std::string HfpAgProfile::GetActiveDevice()
{
    return HfpAgAudioConnection::GetActiveDevice();
}

// Should send OK command after all subscriber number having been sent.
int HfpAgProfile::SendSubscriberNumberInformation(uint16_t type, const std::string &number, int service)
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    dataConn_.subscriberNumber_ = number;

    std::string cmd("+CNUM: ");
    cmd.append(",\"" + number + "\"," + std::to_string(type) + ",," + std::to_string(service));
    commandProcessor_.SendAtCommand(dataConn_, cmd);
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::SendResultCode(int result) const
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    std::string cmd("");
    switch (result) {
        case HFP_AG_RESULT_OK:
            cmd.append("OK");
            break;
        case HFP_AG_RESULT_RING:
            cmd.append("RING");
            break;
        case HFP_AG_RESULT_NO_CARRIER:
            cmd.append("NO CARRIER");
            break;
        case HFP_AG_RESULT_ERROR:
            cmd.append("ERROR");
            break;
        case HFP_AG_RESULT_BUSY:
            cmd.append("BUSY");
            break;
        case HFP_AG_RESULT_NO_ANSWER:
            cmd.append("NO ANSWER");
            break;
        case HFP_AG_RESULT_DELAYED:
            cmd.append("DELAYED");
            break;
        case HFP_AG_RESULT_BLOCKLISTED:
            cmd.append("BLOCKLISTED");
            break;
        default:
            LOG_ERROR("[HFP AG]%{public}s():Unknown HfpAgResultType[%{public}d]", __FUNCTION__, result);
            return HFP_AG_FAILURE;
    }
    commandProcessor_.SendAtCommand(dataConn_, cmd);
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::ReportCallStatus(uint32_t call)
{
    if (mockState_ != HFP_AG_MOCK) {
        HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
        HFP_AG_RETURN_IF_INDICATOR_EQUAL(call, dataConn_.callInd_);
    }

    dataConn_.callInd_ = call;
    return ReportAgIndicator(HFP_AG_INDICATOR_CALL, call);
}

int HfpAgProfile::ReportIndicators(const CindIndicators &indicators)
{
    dataConn_.serviceInd_ = indicators.status;
    dataConn_.callInd_ = indicators.call;
    dataConn_.callsetupInd_ = indicators.callsetup;
    dataConn_.callheldInd_ = indicators.callheld;
    dataConn_.signalInd_ = indicators.signal;
    dataConn_.roamInd_ = indicators.roam;
    dataConn_.batteryInd_ = indicators.battchg;

    std::string cmd = "+CIND: " + std::to_string(dataConn_.serviceInd_) + "," + std::to_string(dataConn_.callInd_) +
                      "," + std::to_string(dataConn_.callsetupInd_) + "," + std::to_string(dataConn_.callheldInd_) +
                      "," + std::to_string(dataConn_.signalInd_) + "," + std::to_string(dataConn_.roamInd_) + "," +
                      std::to_string(dataConn_.batteryInd_);

    commandProcessor_.SendAtCommand(dataConn_, cmd);
    commandProcessor_.SendAtCommand(dataConn_, "OK");
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::ReportCallsetupStatus(uint32_t callsetup)
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
    HFP_AG_RETURN_IF_INDICATOR_EQUAL(callsetup, dataConn_.callsetupInd_);

    dataConn_.callsetupInd_ = callsetup;
    return ReportAgIndicator(HFP_AG_INDICATOR_CALLSETUP, callsetup);
}

int HfpAgProfile::ReportCallheldStatus(uint32_t callheld)
{
    if (mockState_ != HFP_AG_MOCK) {
        HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
    }

    dataConn_.callheldInd_ = callheld;
    return ReportAgIndicator(HFP_AG_INDICATOR_CALLHELD, callheld);
}

int HfpAgProfile::ReportResponseHoldStatus(uint32_t state, int test)
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
    std::string cmd("+BTRH: ");
    cmd.append(std::to_string(state));
    commandProcessor_.SendAtCommand(dataConn_, cmd);
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::ReportRegistrationStatus(uint32_t status)
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
    HFP_AG_RETURN_IF_NOT_ENABLED(dataConn_.serviceIndicatorEnabled_);
    HFP_AG_RETURN_IF_INDICATOR_EQUAL(status, dataConn_.serviceInd_);

    dataConn_.serviceInd_ = status;
    return ReportAgIndicator(HFP_AG_INDICATOR_SERVICE, status);
}

int HfpAgProfile::ReportSignalStrength(uint32_t signal)
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
    HFP_AG_RETURN_IF_NOT_ENABLED(dataConn_.signalStrengthIndicatorEnabled_);
    HFP_AG_RETURN_IF_INDICATOR_EQUAL(signal, dataConn_.signalInd_);

    dataConn_.signalInd_ = signal;
    return ReportAgIndicator(HFP_AG_INDICATOR_SIGNAL_STRENGTH, signal);
}

int HfpAgProfile::ReportRoamingState(uint32_t state)
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
    HFP_AG_RETURN_IF_NOT_ENABLED(dataConn_.roamIndicatorEnabled_);
    HFP_AG_RETURN_IF_INDICATOR_EQUAL(state, dataConn_.roamInd_);

    dataConn_.roamInd_ = state;
    return ReportAgIndicator(HFP_AG_INDICATOR_ROAMING_STATE, state);
}

int HfpAgProfile::ReportBatteryLevel(uint32_t battery)
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
    HFP_AG_RETURN_IF_NOT_ENABLED(dataConn_.batteryIndicatorEnabled_);
    HFP_AG_RETURN_IF_INDICATOR_EQUAL(battery, dataConn_.batteryInd_);

    dataConn_.batteryInd_ = battery;
    return ReportAgIndicator(HFP_AG_INDICATOR_BATTERY_LEVEL, battery);
}

int HfpAgProfile::ReportCurrentNetworkOperator(const std::string &operatorName)
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    dataConn_.operatorName_ = operatorName;

    std::string cmd("+COPS: ");
    cmd.append(std::to_string(HFP_AG_COPS_MODE_SET_FORMAT) + "," + std::to_string(HFP_AG_COPS_FORMAT_LONG) + ",\"" +
               operatorName + "\"");
    commandProcessor_.SendAtCommand(dataConn_, cmd);
    SendResultCode(HFP_AG_RESULT_OK);
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::PhoneStateChange(const HfpAgPhoneState &phoneState)
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    LOG_INFO("[HFP AG]%{public}s():preCallState_[%{public}s], preNumActiveCalls_[%{public}d], preNumHeldCalls_[%{public}d], currentCallState[%{public}s], "
             "activeNum[%{public}d], heldNum[%{public}d]",
        __FUNCTION__,
        GetCallState(preCallState_).c_str(),
        preNumActiveCalls_,
        preNumHeldCalls_,
        GetCallState(phoneState.callState).c_str(),
        phoneState.activeNum,
        phoneState.heldNum);

    switch (phoneState.callState) {
        case HFP_AG_CALL_STATE_DIALING:
            ProcessCurrentCallStateDialing(phoneState.activeNum, phoneState.heldNum);
            break;
        case HFP_AG_CALL_STATE_ALERTING:
            ProcessCurrentCallStateAlerting();
            break;
        case HFP_AG_CALL_STATE_INCOMING:
            ProcessCurrentCallStateIncominging(
                phoneState.activeNum, phoneState.heldNum, phoneState.type, phoneState.number);
            break;
        case HFP_AG_CALL_STATE_IDLE:
            ProcessCurrentCallStateIdle(phoneState.activeNum, phoneState.heldNum);
            break;
        default:
            LOG_WARN("[HFP AG]%{public}s():Ignore current call state[%{public}d]", __FUNCTION__, phoneState.callState);
            return HFP_AG_SUCCESS;
    }

    if (preNumActiveCalls_ == 1 && preNumHeldCalls_ == 1 &&  phoneState.activeNum == 1 && phoneState.heldNum == 0) {
        ReportCallheldStatus(HFP_AG_CALLHELD_INACTIVE);
    }

    SetCallStates(phoneState.heldNum, phoneState.activeNum, phoneState.callState);
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::ReportCurrentCallList(const HfpAgCallList &clcc) const
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    // This means that the current call list all has been transmitted
    if (clcc.index == 0) {
        SendResultCode(HFP_AG_RESULT_OK);
    } else {
        std::string cmd("+CLCC: ");
        cmd.append(std::to_string(clcc.index) + "," + std::to_string(clcc.dir) + "," + std::to_string(clcc.state) +
                   "," + std::to_string(clcc.mode) + "," + std::to_string(clcc.mpty));
        if (clcc.number.length() > 0) {
            if ((clcc.type == HFP_AG_CALL_NUMBER_INTERNATIONAL) && (clcc.number.at(0) != '+')) {
                cmd.append(",\"+" + clcc.number + "\"," + std::to_string(clcc.type));
            } else {
                cmd.append(",\"" + clcc.number + "\"," + std::to_string(clcc.type));
            }
        }

        commandProcessor_.SendAtCommand(dataConn_, cmd);
    }
    return HFP_AG_SUCCESS;
}

void HfpAgProfile::ResponesOK() const
{
    SendResultCode(HFP_AG_RESULT_OK);
}

int HfpAgProfile::ReportCallStatusByCallNums(int numActive, int numHeld)
{
    int state = HFP_AG_CALL_INACTIVE;
    if ((numActive + numHeld) > 0) {
        state = HFP_AG_CALL_ACTIVE;
    }
    if (preNumActiveCalls_ == 1 && preNumHeldCalls_ == 0 && numActive == 1 && numHeld == 0) {
        return HFP_AG_SUCCESS;
    }

    ReportCallStatus(state);
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::ReportCallheldStatusByCallNums(int numActive, int numHeld)
{
    int state = HFP_AG_CALLHELD_INACTIVE;
    if (numHeld > 0) {
        if (numActive > 0) {
            state = HFP_AG_CALLHELD_ACTIVE;
        } else {
            state = HFP_AG_CALLHELD_NOACTIVE;
        }
    }
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::ReportAgIndicator(int indicator, int value) const
{
    if (dataConn_.cmerEnabled_) {
        std::string cmd("+CIEV: ");
        cmd.append(std::to_string(indicator) + "," + std::to_string(value));
        commandProcessor_.SendAtCommand(dataConn_, cmd);
    }
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::NotifyCallingLineIdentification(uint16_t type, const std::string &number) const
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
    HFP_AG_RETURN_IF_NOT_ENABLED(dataConn_.clipEnabled_);

    std::string cmd("+CLIP: ");
    if ((type == HFP_AG_CALL_NUMBER_INTERNATIONAL) && (number.at(0) != '+')) {
        cmd.append("\"+" + number + "\"," + std::to_string(type));
    } else {
        cmd.append("\"" + number + "\"," + std::to_string(type));
    }
    commandProcessor_.SendAtCommand(dataConn_, cmd);
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::NotifyIncomingCallWaiting(uint16_t type, const std::string &number) const
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
    HFP_AG_RETURN_IF_NOT_ENABLED(dataConn_.ccwaEnabled_);

    std::string cmd("+CCWA: ");
    if ((type == HFP_AG_CALL_NUMBER_INTERNATIONAL) && (number.at(0) != '+')) {
        cmd.append("\"+" + number + "\"," + std::to_string(type));
    } else {
        cmd.append("\"" + number + "\"," + std::to_string(type));
    }
    commandProcessor_.SendAtCommand(dataConn_, cmd);
    return HFP_AG_SUCCESS;
}

void HfpAgProfile::SendBinp(std::string number) const
{
    std::string cmd("+BINP: ");
    cmd.append(number);
    if (!number.empty()) {
        commandProcessor_.SendAtCommand(dataConn_, cmd);
        SendResultCode(HFP_AG_RESULT_OK);
    } else {
        SendResultCode(HFP_AG_RESULT_ERROR);
    }
}

int HfpAgProfile::SendRing() const
{
    HFP_AG_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    std::string cmd("RING");
    commandProcessor_.SendAtCommand(dataConn_, cmd);
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::ProcessCurrentCallStateDialing(int numActive, int numHeld)
{
    // Report callsetup indicator: outgoing
    ReportCallsetupStatus(HFP_AG_CALLSETUP_OUTGOING);

    // Report callheld indicator: no active and have held
    if ((numActive == 0) && (numHeld > 0)) {
        ReportCallheldStatus(HFP_AG_CALLHELD_NOACTIVE);
    }

    // Connect SCO if not have been established
    auto isAudioConnected = HfpAgAudioConnection::IsAudioConnected(address_);
    if ((preNumActiveCalls_ == 0) && (preNumHeldCalls_ == 0) && isAudioConnected == false) {
        HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(address_, HFP_AG_CONNECT_AUDIO_EVT);
    }
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::ProcessCurrentCallStateAlerting()
{
    // Report callsetup indicator: alerting
    ReportCallsetupStatus(HFP_AG_CALLSETUP_ALERTING);
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::ProcessCurrentCallStateIncominging(
    int numActive, int numHeld, uint16_t type, const std::string &number)
{
    if (preCallState_ == HFP_AG_CALL_STATE_INCOMING) {
        // If active or held calls changed
        if ((numActive != preNumActiveCalls_) || (numHeld != preNumHeldCalls_)) {
            // If calls swapped, do nothing
            if ((numActive == preNumHeldCalls_) && (numHeld == preNumActiveCalls_)) {
                return HFP_AG_SUCCESS;
            }

            // Report callheld indicator due to numActive and numHeld
            ReportCallheldStatusByCallNums(numActive, numHeld);

            // Report call indicator due to numActive and numHeld
            ReportCallStatusByCallNums(numActive, numHeld);
        }
        return HFP_AG_SUCCESS;
    }

    // If have active or held calls
    if ((numActive > 0) || (numHeld > 0)) {
        // Send incoming call waiting
        NotifyIncomingCallWaiting(type, number);
        ReportCallsetupStatus(HFP_AG_CALLSETUP_INCOMING);
        return HFP_AG_SUCCESS;
    }

    // Report callsetup indicator incoming
    ReportCallsetupStatus(HFP_AG_CALLSETUP_INCOMING);

    dataConn_.clipNumber_ = number;
    dataConn_.clipType_ = type;
    SendRingAndClip();
    LOG_INFO("inBandRingTone_ %{public}d", dataConn_.inBandRingTone_);
    auto isAudioConnected = HfpAgAudioConnection::IsAudioConnected(address_);
    if ((preNumActiveCalls_ == 0) && (preNumHeldCalls_ == 0) &&
        (dataConn_.inBandRingTone_ == HFP_AG_INBAND_RING_ENABLE) &&
        (isAudioConnected == false)) {
        HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(address_, HFP_AG_CONNECT_AUDIO_EVT);
        scoPostProcess_ = true;
    }

    return HFP_AG_SUCCESS;
}

int HfpAgProfile::ProcessCurrentCallStateIdle(int numActive, int numHeld)
{
    HILOGI("[HFP AG]:Pre-CallState[%{public}s], numActive:%{public}d, numHeld:%{public}d",
        GetCallState(preCallState_).c_str(), numActive, numHeld);
    if (dataConn_.ringTimer_ != nullptr) {
        dataConn_.ringTimer_->Stop();
    }
    dataConn_.clipNumber_ = "";
    dataConn_.clipType_ = 0;

    switch (preCallState_) {
        case HFP_AG_CALL_STATE_DIALING:
        case HFP_AG_CALL_STATE_ALERTING:
            ProcessPreviousCallStateDialingAlerting(numActive, numHeld);
            break;
        case HFP_AG_CALL_STATE_INCOMING:
            ProcessPreviousCallStateIncoming(numActive, numHeld);
            break;
        case HFP_AG_CALL_STATE_IDLE:
            ProcessPreviousCallStateIdle(numActive, numHeld);
            break;
        default:
            LOG_WARN("[HFP AG]%{public}s():Ignore Pre-CallState[%{public}d]", __FUNCTION__, preCallState_);
            break;
    }

    auto isAudioConnected = HfpAgAudioConnection::IsAudioConnected(address_);
    if (numActive == 0 && numHeld == 0 && isAudioConnected == true) {
        HILOGI("[HFP AG]:No call exist , disconnect sco");
        HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(address_, HFP_AG_DISCONNECT_AUDIO_EVT);
    }

    return HFP_AG_SUCCESS;
}

int HfpAgProfile::ProcessPreviousCallStateDialingAlerting(int numActive, int numHeld)
{
    // If have more active calls, report call indicator: active
    if (numActive > preNumActiveCalls_) {
        ReportCallStatus(HFP_AG_CALL_ACTIVE);
    }

    // Report callheld indicator due to numActive and numHeld
    ReportCallheldStatusByCallNums(numActive, numHeld);

    // Report callsetup indicator: none
    ReportCallsetupStatus(HFP_AG_CALLSETUP_NONE);
    return HFP_AG_SUCCESS;
}

int HfpAgProfile::ProcessPreviousCallStateIncoming(int numActive, int numHeld)
{
    // If have more active calls, report call indicator: active
    if (numActive > preNumActiveCalls_) {
        ReportCallStatus(HFP_AG_CALL_ACTIVE);

        // Connect SCO if not have been established
        auto isAudioConnected = HfpAgAudioConnection::IsAudioConnected(address_);
        if ((preNumActiveCalls_ == 0) && (preNumHeldCalls_ == 0) && isAudioConnected == false) {
            HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(address_, HFP_AG_CONNECT_AUDIO_EVT);
        }
    }

    // Report callsetup indicator: none
    ReportCallsetupStatus(HFP_AG_CALLSETUP_NONE);

    // Report callheld indicator: have active and have held
    if ((numHeld >= preNumHeldCalls_) && numHeld > 0) {
        ReportCallheldStatus(HFP_AG_CALLHELD_ACTIVE);
    }

    return HFP_AG_SUCCESS;
}

int HfpAgProfile::ProcessPreviousCallStateIdle(int numActive, int numHeld)
{
    // Make callheld indicator INACTIVE if active and held calls swapped
    if ((numHeld > 0) && (numActive > 0) && (numActive == preNumHeldCalls_) && (numHeld == preNumActiveCalls_)) {
        dataConn_.callheldInd_ = HFP_AG_CALLHELD_INACTIVE;
        // Connect SCO when there are new calls
    } else if (((numActive > 0) || (numHeld > 0)) && (preNumActiveCalls_ == 0) && (preNumHeldCalls_ == 0)) {
        auto isAudioConnected = HfpAgAudioConnection::IsAudioConnected(address_);
        if ((isAudioConnected == false) && (address_ == HfpAgAudioConnection::GetActiveDevice())) {
            HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(address_, HFP_AG_CONNECT_AUDIO_EVT);
        }
    }

    // Report callheld indicator due to numActive and numHeld
    ReportCallheldStatusByCallNums(numActive, numHeld);

    // Report call indicator due to numActive and numHeld
    ReportCallStatusByCallNums(numActive, numHeld);

    // Report callsetup indicator: none
    ReportCallsetupStatus(HFP_AG_CALLSETUP_NONE);

    // Send NO CARRIER if service is not available
    if ((numActive == 0) && (numHeld == 0) && (dataConn_.serviceInd_ == HFP_AG_SERVICE_NOT_AVAILABLE) &&
        ((preNumActiveCalls_ > 0) || (preNumHeldCalls_ > 0))) {
        SendResultCode(HFP_AG_RESULT_NO_CARRIER);
    }
    return HFP_AG_SUCCESS;
}

void HfpAgProfile::startMock(int state)
{
    mockState_ = state;
}

void HfpAgProfile::SetCallStates(int numHeld, int numActive, int callState)
{
    preNumHeldCalls_ = numHeld;
    preNumActiveCalls_ = numActive;
    preCallState_ = callState;
}

void HfpAgProfile::ProcessBia(bool service, bool roam, bool signal, bool battery)
{
    dataConn_.serviceIndicatorEnabled_ = service;
    dataConn_.roamIndicatorEnabled_ = roam;
    dataConn_.signalStrengthIndicatorEnabled_ = signal;
    dataConn_.batteryIndicatorEnabled_ = battery;
}

void HfpAgProfile::RemoveStateMachine()
{
    HfpAgProfileEventSender::GetInstance().RemoveStateMachine(address_);
}

bool HfpAgProfile::IsAudioConnected(const std::string &address)
{
    return HfpAgAudioConnection::IsAudioConnected(address);
}

void HfpAgProfile::RemoveRemoteScnLoging() const
{
    if (dataConn_.GetRole() == HFP_AG_INITIATOR) {
        RawAddress rawAddr(address_);
        BtAddr btAddr;
        rawAddr.ConvertToUint8(btAddr.addr);
        btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;
        BTM_RemoveRemoteRfcommScnChannelForLogging(BTM_HCI_LOG_FILTER_MODULE_HFP, dataConn_.GetRemoteScn(), &btAddr);
    }
}

std::string HfpAgProfile::GetCallState(int state)
{
    switch (state) {
        case HFP_AG_CALL_STATE_ACTIVE:
            return "HFP_AG_CALL_STATE_ACTIVE";
        case HFP_AG_CALL_STATE_HELD:
            return "HFP_AG_CALL_STATE_HELD";
        case HFP_AG_CALL_STATE_DIALING:
            return "HFP_AG_CALL_STATE_DIALING";
        case HFP_AG_CALL_STATE_ALERTING:
            return "HFP_AG_CALL_STATE_ALERTING";
        case HFP_AG_CALL_STATE_INCOMING:
            return "HFP_AG_CALL_STATE_INCOMING";
        case HFP_AG_CALL_STATE_WAITING:
            return "HFP_AG_CALL_STATE_WAITING";
        case HFP_AG_CALL_STATE_IDLE:
            return "HFP_AG_CALL_STATE_IDLE";
        case HFP_AG_CALL_STATE_DISCONNECTED:
            return "HFP_AG_CALL_STATE_DISCONNECTED";
        case HFP_AG_CALL_STATE_DISCONNECTING:
            return "HFP_AG_CALL_STATE_DISCONNECTING";
        default:
            return "Unknown";
    }
}
}  // namespace bluetooth
}  // namespace OHOS
