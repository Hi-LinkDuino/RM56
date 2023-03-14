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

#include "hfp_hf_profile.h"

#include "btm.h"
#include "hfp_hf_command_processor.h"
#include "hfp_hf_defines.h"
#include "hfp_hf_profile_event_sender.h"
#include "hfp_hf_sdp_server.h"
#include "interface_profile.h"
#include "power_manager.h"

namespace OHOS {
namespace bluetooth {
#define HFP_HF_RETURN_IF_NOT_CONNECTED(state)                                       \
    do {                                                                            \
        if (!state) {                                                               \
            LOG_ERROR("[HFP HF]%{public}s():SLC connection not established", __FUNCTION__); \
            return false;                                                           \
        }                                                                           \
    } while (0)

#define HFP_HF_RETURN_IF_NOT_ENABLED(enable)                              \
    do {                                                                  \
        if (!enable) {                                                    \
            LOG_ERROR("[HFP HF]%{public}s():function not allowed", __FUNCTION__); \
            return false;                                                 \
        }                                                                 \
    } while (0)

#define HFP_HF_RETURN_IF_NO_FEATURE(feature, mask)                         \
    do {                                                                   \
        if (!(feature & mask)) {                                           \
            LOG_ERROR("[HFP HF]%{public}s():feature not supported", __FUNCTION__); \
            return false;                                                  \
        }                                                                  \
    } while (0)

#define HFP_HF_RETURN_IF_INDICATOR_EQUAL(ind1, ind2)                                                    \
    do {                                                                                                \
        if (ind1 == ind2) {                                                                             \
            LOG_WARN("[HFP HF]%{public}s():Indicator wouldn't be sent for its value not change", __FUNCTION__); \
            return true;                                                                                \
        }                                                                                               \
    } while (0)

int HfpHfProfile::RegisterService()
{
    HfpHfDataConnection::Init();
    HfpHfDataConnectionServer &connectionServer = HfpHfDataConnectionServer::GetInstance();
    int ret = HfpHfSdpServer::GetInstance().RegisterSdpService(connectionServer.AssignLocalScn());
    HFP_HF_RETURN_IF_FAIL(ret);

    BTM_AddLocalRfcommScnForLogging(BTM_HCI_LOG_FILTER_MODULE_HFP, connectionServer.GetLocalScn());

    ret = connectionServer.RegisterServer();
    HFP_HF_RETURN_IF_FAIL(ret);

    ret = HfpHfAudioConnection::Register();
    HFP_HF_RETURN_IF_FAIL(ret);

    return ret;
}

int HfpHfProfile::DeregisterService()
{
    HfpHfDataConnectionServer &connectionServer = HfpHfDataConnectionServer::GetInstance();
    int ret = HfpHfSdpServer::GetInstance().DeregisterSdpService();
    HFP_HF_RETURN_IF_FAIL(ret);

    BTM_RemoveLocalRfcommScnChannelForLogging(BTM_HCI_LOG_FILTER_MODULE_HFP, connectionServer.GetLocalScn());

    ret = connectionServer.RemoveServer();
    HFP_HF_RETURN_IF_FAIL(ret);

    ret = HfpHfAudioConnection::Deregister();
    HFP_HF_RETURN_IF_FAIL(ret);

    HfpHfDataConnection::CleanUp();
    return ret;
}

HfpHfProfile::HfpHfProfile(std::string address) : address_(address)
{
}

void HfpHfProfile::Init()
{
    dataConn_.SetRemoteAddr(address_);
    audioConn_.SetRemoteAddr(address_);
    commandProcessor_.Init(address_);
}

int HfpHfProfile::DoServiceDiscovery(int role)
{
    dataConn_.SetRole(role);
    return sdpClient_.DoDiscovery(address_, role);
}

int HfpHfProfile::ServiceDiscoveryResult()
{
    int role = dataConn_.GetRole();
    if (!sdpClient_.FindAttributes(address_, role)) {
        LOG_ERROR("[HFP HF]%{public}s():FindAttributes() error", __FUNCTION__);
        return HFP_HF_FAILURE;
    }
    dataConn_.SetSdpInfo(sdpClient_.GetRemoteSdpInfo());
    if (HFP_HF_INITIATOR == role) {
        EstablishDataConnection();
    }
    return HFP_HF_SUCCESS;
}

int HfpHfProfile::EstablishDataConnection()
{
    RawAddress rawAddr(address_);
    BtAddr btAddr;
    rawAddr.ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;
    BTM_AddRemoteRfcommScnForLogging(BTM_HCI_LOG_FILTER_MODULE_HFP, dataConn_.GetRemoteScn(), &btAddr);
    return dataConn_.Connect();
}

int HfpHfProfile::ReleaseDataConnection() const
{
    return dataConn_.Disconnect();
}

int HfpHfProfile::AcceptDataConnection(uint16_t handle)
{
    DoServiceDiscovery(HFP_HF_ACCEPTOR);
    dataServer_.AcceptConnection(handle);
    dataConn_.SetConnectionHandle(handle);
    return HFP_HF_SUCCESS;
}

int HfpHfProfile::EstablishServiceLevelConnection()
{
    commandProcessor_.ConnectSlc(dataConn_);
    return HFP_HF_SUCCESS;
}

int HfpHfProfile::ReadData()
{
    HfpHfCommandParser::GetInstance().Read(dataConn_, commandProcessor_);
    return HFP_HF_SUCCESS;
}

void HfpHfProfile::ProcessSlcEstablished()
{
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_ON, PROFILE_NAME_HFP_HF, RawAddress(address_));
    if (!QuerySubscriberInfo()) {
        LOG_ERROR("[HFP HF]%{public}s():can't query the subscriber information!", __FUNCTION__);
    }
    dataConn_.SetSlcConnectState(true);
}

int HfpHfProfile::SetupCodecConnection()
{
    if (dataConn_.IsCodecNegotiationSupport()) {
        isCodecSetupInitiator_ = true;
        SendBcc();
    } else {
        dataConn_.inUseCodec_ = HFP_HF_CODEC_CVSD;
        EstablishAudioConnection();
    }

    return HFP_HF_SUCCESS;
}

int HfpHfProfile::EstablishAudioConnection()
{
    audioConn_.SetSupportFeatures(dataConn_.IsEscoSupport(), dataConn_.IsEscoS4Support(), dataConn_.inUseCodec_);
    audioConn_.ConnectAudio();
    return HFP_HF_SUCCESS;
}

int HfpHfProfile::ReleaseAudioConnection() const
{
    audioConn_.DisconnectAudio();
    return HFP_HF_SUCCESS;
}

int HfpHfProfile::AcceptAudioConnection()
{
    audioConn_.SetSupportFeatures(dataConn_.IsEscoSupport(), dataConn_.IsEscoS4Support(), dataConn_.inUseCodec_);
    audioConn_.AcceptAudioConnection();
    return HFP_HF_SUCCESS;
}

int HfpHfProfile::RejectAudioConnection() const
{
    audioConn_.RejectAudioConnection();
    return HFP_HF_SUCCESS;
}

int HfpHfProfile::ProcessAudioConnReqInAudioConnecting()
{
    if (isCodecSetupInitiator_ == false) {
        return HFP_HF_FAILURE;
    }

    if (HfpHfAudioConnection::IsAudioConnected() == false) {
        AcceptAudioConnection();
    }
    return HFP_HF_SUCCESS;
}

int HfpHfProfile::RestorCodecSetupRole()
{
    isCodecSetupInitiator_ = false;
    return HFP_HF_SUCCESS;
}

bool HfpHfProfile::QueryCurrentCalls()
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    std::string cmd("AT+CLCC");
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_CLCC_EXECUTER);
    return true;
}

bool HfpHfProfile::QueryOperatorName(bool isQuery)
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    std::string cmd("AT+COPS");
    int commandId;
    if (isQuery) {
        cmd.append("?");
        commandId = HfpHfCommandProcessor::AT_COPS_GETTER;
    } else {
        cmd.append("=3,0");
        commandId = HfpHfCommandProcessor::AT_COPS_SETTER;
    }
    commandProcessor_.SendAtCommand(dataConn_, cmd, commandId);
    return true;
}

bool HfpHfProfile::QuerySubscriberInfo()
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    std::string cmd("AT+CNUM");
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_CNUM_EXECUTER);
    return true;
}

bool HfpHfProfile::SendDtmf(uint8_t code)
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    std::string cmd("AT+VTS=");
    cmd += code;
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_VTS_SETTER);
    return true;
}

bool HfpHfProfile::SetHfVolume(int volume, int type)
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    // Make sure volume is in the range 0-15
    int value = (volume > 15) ? 15 : volume;
    value = (value < 0) ? 0 : value;

    std::string cmd("AT+");
    int commandId = HfpHfCommandProcessor::AT_COMMAND_NONE;
    if (type == HFP_HF_VOLUME_TYPE_SPK) {
        cmd.append("VGS");
        commandId = HfpHfCommandProcessor::AT_VGS_SETTER;
    } else if (type == HFP_HF_VOLUME_TYPE_MIC) {
        cmd.append("VGM");
        commandId = HfpHfCommandProcessor::AT_VGM_SETTER;
    } else {
        LOG_ERROR("[HFP HF]%{public}s():invalid type for volume!", __FUNCTION__);
        return false;
    }
    cmd.append("=" + std::to_string(value));
    commandProcessor_.SendAtCommand(dataConn_, cmd, commandId);
    return true;
}

bool HfpHfProfile::OpenVoiceRecognition()
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
    HFP_HF_RETURN_IF_NO_FEATURE(dataConn_.remoteFeatures_, HFP_HF_AG_FEATURES_VOICE_RECOGNITION);

    std::string cmd("AT+BVRA=1");
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_BVRA_1_SETTER);
    return true;
}

bool HfpHfProfile::CloseVoiceRecognition()
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
    HFP_HF_RETURN_IF_NO_FEATURE(dataConn_.remoteFeatures_, HFP_HF_AG_FEATURES_VOICE_RECOGNITION);

    std::string cmd("AT+BVRA=0");
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_BVRA_0_SETTER);
    return true;
}

bool HfpHfProfile::DialOutCall(const std::string &number)
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    if (number.length() == 0) {
        LOG_ERROR("[HFP HF]%{public}s():Invalid call number", __FUNCTION__);
        return false;
    }

    std::string cmd("ATD");
    cmd.append(number + ";");
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_ATD_EXECUTER);
    return true;
}

bool HfpHfProfile::DialMemory(int memory)
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    std::string cmd("ATD>");
    cmd.append(std::to_string(memory) + ";");
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_ATD_EXECUTER);
    return true;
}

bool HfpHfProfile::SendVoiceTag(int index)
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    std::string cmd("AT+BINP");
    cmd.append("=" + std::to_string(index));
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_BINP_SETTER);
    return true;
}

bool HfpHfProfile::SendKeyPressed()
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    std::string cmd("AT+CKPD=200");
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_CKPD_SETTER);
    return true;
}


bool HfpHfProfile::SendAta()
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    std::string cmd("ATA");
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_ATA_EXECUTER);
    return true;
}

bool HfpHfProfile::SendChld(int action, int idx)
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    std::string cmd("AT+CHLD");
    cmd.append("=" + std::to_string(action));
    if (idx > 0) {
        cmd.append(std::to_string(idx));
    }
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_CHLD_SETTER);
    return true;
}

bool HfpHfProfile::SendChup()
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    std::string cmd("AT+CHUP");
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_CHUP_EXECUTER);
    return true;
}

bool HfpHfProfile::SendBtrh(int action)
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    std::string cmd("AT+BTRH");
    cmd.append("=" + std::to_string(action));
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_CHLD_SETTER);
    return true;
}

bool HfpHfProfile::SendBcc()
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    std::string cmd("AT+BCC");
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_BCC_EXECUTER);
    return true;
}

bool HfpHfProfile::CallLastDialedNumber()
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);

    std::string cmd("AT+BLDN");
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_BLDN_EXECUTER);
    return true;
}

bool HfpHfProfile::CloseRemoteNrec()
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
    HFP_HF_RETURN_IF_NO_FEATURE(dataConn_.remoteFeatures_, HFP_HF_AG_FEATURES_ECNR);

    std::string cmd("AT+NREC=0");
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_NREC_SETTER);
    return true;
}

bool HfpHfProfile::ReportBatteryLevel(uint32_t battery)
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
    HFP_HF_RETURN_IF_NOT_ENABLED(dataConn_.localHfIndicators_[0].isEnabled);
    HFP_HF_RETURN_IF_INDICATOR_EQUAL(battery, dataConn_.localHfIndicators_[0].value);

    dataConn_.localHfIndicators_[0].value = battery;
    return ReportHfIndicator(HFP_HF_INDICATOR_BATTERY_LEVEL, battery);
}

bool HfpHfProfile::ReportDriverSafety(uint32_t safety)
{
    HFP_HF_RETURN_IF_NOT_CONNECTED(dataConn_.slcConnected_);
    HFP_HF_RETURN_IF_NOT_ENABLED(dataConn_.localHfIndicators_[1].isEnabled);
    HFP_HF_RETURN_IF_INDICATOR_EQUAL(safety, dataConn_.localHfIndicators_[1].value);

    dataConn_.localHfIndicators_[1].value = safety;
    return ReportHfIndicator(HFP_HF_INDICATOR_DRIVER_SAFETY, safety);
}

bool HfpHfProfile::ReportHfIndicator(int indicator, int value)
{
    auto it = std::find_if(dataConn_.remoteHfIndicators_.begin(),
        dataConn_.remoteHfIndicators_.end(),
        [&](const HfpHfDataConnection::HfIndicator &hfIndicator) {
            return (hfIndicator.anum == indicator &&
                (hfIndicator.isEnabled == false || hfIndicator.isSupported == false));
        });

    if (it != dataConn_.remoteHfIndicators_.end()) {
        LOG_INFO("[HFP HF]%{public}s():Remote device not support/enable indicator[%{public}d]", __FUNCTION__, indicator);
        return true;
    }

    std::string cmd("AT+BIEV=");
    cmd.append(std::to_string(indicator) + "," + std::to_string(value));
    commandProcessor_.SendAtCommand(dataConn_, cmd, HfpHfCommandProcessor::AT_BIEV_SETTER);
    return true;
}

void HfpHfProfile::RemoveStateMachine()
{
    commandProcessor_.CleanUp();
    HfpHfProfileEventSender::GetInstance().RemoveSateMachine(address_);
}

void HfpHfProfile::SendAtCommand(int cmdId, const std::string &arg)
{
    switch (cmdId) {
        case HfpHfCommandProcessor::AT_NREC_SETTER:
            CloseRemoteNrec();
            break;
        case HfpHfCommandProcessor::AT_ATA_EXECUTER:
            SendAta();
            break;
        case HfpHfCommandProcessor::AT_CLCC_EXECUTER:
            QueryCurrentCalls();
            break;
        case HfpHfCommandProcessor::AT_BLDN_EXECUTER:
            CallLastDialedNumber();
            break;
        case HfpHfCommandProcessor::AT_ATD_EXECUTER:
            DialMemory(std::stoi(arg));
            break;
        default:
            break;
    }
}

void HfpHfProfile::ProcessAudioConnectRequest()
{
    if (HfpHfAudioConnection::IsAudioConnected()) {
        RejectAudioConnection();
    } else {
        AcceptAudioConnection();
    }
}

void HfpHfProfile::RemoveRemoteScnLoging() const
{
    if (dataConn_.GetRole() == HFP_HF_INITIATOR) {
        RawAddress rawAddr(address_);
        BtAddr btAddr;
        rawAddr.ConvertToUint8(btAddr.addr);
        btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;
        BTM_RemoveRemoteRfcommScnChannelForLogging(BTM_HCI_LOG_FILTER_MODULE_HFP, dataConn_.GetRemoteScn(), &btAddr);
    }
}
}  // namespace bluetooth
}  // namespace OHOS
