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

#include "hfp_ag_command_processor.h"

#include <algorithm>
#include <regex>
#include <string>

#include "hfp_ag_audio_connection.h"
#include "hfp_ag_defines.h"
#include "hfp_ag_profile_event_sender.h"
#include "packet.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
std::unordered_map<std::string, HfpAgCommandProcessor::HfpAgAtHandler> HfpAgCommandProcessor::g_atCmdMap = {
    std::make_pair<std::string, HfpAgAtHandler>("AT+BRSF", {&HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::BrsfSetter,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+CCWA", {&HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::CcwaSetter,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+CLIP", {&HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::ClipSetter,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+CMER", {&HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::CmerSetter,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+CMEE", {&HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::CmeeSetter,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+BCC", {&HfpAgCommandProcessor::AtEmptyFn,
                                                           &HfpAgCommandProcessor::AtEmptyFn,
                                                           &HfpAgCommandProcessor::AtEmptyFn,
                                                           &HfpAgCommandProcessor::BccExecuter}),
    std::make_pair<std::string, HfpAgAtHandler>("ATA", {&HfpAgCommandProcessor::AtEmptyFn,
                                                        &HfpAgCommandProcessor::AtEmptyFn,
                                                        &HfpAgCommandProcessor::AtEmptyFn,
                                                        &HfpAgCommandProcessor::AtaExecuter}),
    std::make_pair<std::string, HfpAgAtHandler>("ATD", {&HfpAgCommandProcessor::AtEmptyFn,
                                                        &HfpAgCommandProcessor::AtEmptyFn,
                                                        &HfpAgCommandProcessor::AtEmptyFn,
                                                        &HfpAgCommandProcessor::AtdExecuter}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+VGS", {&HfpAgCommandProcessor::AtEmptyFn,
                                                           &HfpAgCommandProcessor::VgsSetter,
                                                           &HfpAgCommandProcessor::AtEmptyFn,
                                                           &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+VGM", {&HfpAgCommandProcessor::AtEmptyFn,
                                                           &HfpAgCommandProcessor::VgmSetter,
                                                           &HfpAgCommandProcessor::AtEmptyFn,
                                                           &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+CHLD", {&HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::ChldSetter,
                                                            &HfpAgCommandProcessor::ChldTester,
                                                            &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+CHUP", {&HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::ChupExecuter}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+CIND", {&HfpAgCommandProcessor::CindGetter,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::CindTester,
                                                            &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+VTS", {&HfpAgCommandProcessor::AtEmptyFn,
                                                           &HfpAgCommandProcessor::VtsSetter,
                                                           &HfpAgCommandProcessor::AtEmptyFn,
                                                           &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+BLDN", {&HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::BldnExecuter}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+BVRA", {&HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::BvraSetter,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+NREC", {&HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::NrecSetter,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+CNUM", {&HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::CnumExecuter}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+CLCC", {&HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::ClccExecuter}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+COPS", {&HfpAgCommandProcessor::CopsGetter,
                                                            &HfpAgCommandProcessor::CopsSetter,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+BIA", {&HfpAgCommandProcessor::AtEmptyFn,
                                                           &HfpAgCommandProcessor::BiaSetter,
                                                           &HfpAgCommandProcessor::AtEmptyFn,
                                                           &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+BCS", {&HfpAgCommandProcessor::AtEmptyFn,
                                                           &HfpAgCommandProcessor::BcsSetter,
                                                           &HfpAgCommandProcessor::AtEmptyFn,
                                                           &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+BIND", {&HfpAgCommandProcessor::BindGetter,
                                                            &HfpAgCommandProcessor::BindSetter,
                                                            &HfpAgCommandProcessor::BindTester,
                                                            &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+BIEV", {&HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::BievSetter,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+BAC", {&HfpAgCommandProcessor::AtEmptyFn,
                                                           &HfpAgCommandProcessor::BacSetter,
                                                           &HfpAgCommandProcessor::AtEmptyFn,
                                                           &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+BTRH", {&HfpAgCommandProcessor::BtrhGetter,
                                                            &HfpAgCommandProcessor::BtrhSetter,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+CKPD", {&HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::CkpdSetter,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn}),
    std::make_pair<std::string, HfpAgAtHandler>("AT+BINP", {&HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::BinpSetter,
                                                            &HfpAgCommandProcessor::AtEmptyFn,
                                                            &HfpAgCommandProcessor::AtEmptyFn}),
};

int HfpAgCommandProcessor::StoiTryCatch(HfpAgDataConnection &dataConn, const std::string &arg)
{
    try {
        return std::stoi(arg);
    } catch (std::exception &e) {
        LOG_ERROR("[HFP AG]%{public}s():Catch exception %{public}s", __FUNCTION__, e.what());
        SendErrorCode(dataConn, HFP_AG_ERROR_AG_FAILURE);
        return 0;
    }
}

HfpAgCommandProcessor &HfpAgCommandProcessor::GetInstance()
{
    static HfpAgCommandProcessor instance;
    return instance;
}

void HfpAgCommandProcessor::SendErrorCode(const HfpAgDataConnection &dataConn, int errorCode)
{
    if (dataConn.cmeeEnabled_) {
        SendAtCommand(dataConn, "+CME ERROR: " + std::to_string(errorCode));
    } else {
        SendAtCommand(dataConn, "ERROR");
    }
}

void HfpAgCommandProcessor::SendAtCommand(const HfpAgDataConnection &dataConn, const std::string &command)
{
    LOG_INFO("[HFP AG]%{public}s():Send Command[%{public}s]", __FUNCTION__, command.c_str());
    std::string fullCommand(HEAD + command + TAIL);
    std::size_t cmdLength = fullCommand.length();
    Packet *packet = PacketMalloc(0, 0, cmdLength);
    Buffer *buf = PacketContinuousPayload(packet);
    void *data = BufferPtr(buf);
    (void)memcpy_s(data, cmdLength, fullCommand.c_str(), cmdLength);
    dataConn.WriteData(*packet);
    PacketFree(packet);
}

void HfpAgCommandProcessor::Handle(
    HfpAgDataConnection &dataConn, const std::string &cmd, const std::string &arg, int cmdType)
{
    LOG_INFO("[HFP AG]%{public}s():cmd[%{public}s], arg[%{public}s], Type[%{public}d]", __FUNCTION__,
        cmd.c_str(), arg.c_str(), cmdType);
    auto it = g_atCmdMap.find(cmd);
    if (it == g_atCmdMap.end()) {
        SendErrorCode(dataConn, HFP_AG_ERROR_AG_FAILURE);
        LOG_ERROR("[HFP AG]%{public}s():%{public}s command handler not found", __FUNCTION__, cmd.c_str());
        return;
    }

    switch (cmdType) {
        case HFP_AG_CMD_SET:
            (this->*(it->second.setter))(dataConn, arg);
            break;
        case HFP_AG_CMD_GET:
            (this->*(it->second.getter))(dataConn, arg);
            break;
        case HFP_AG_CMD_TEST:
            (this->*(it->second.tester))(dataConn, arg);
            break;
        case HFP_AG_CMD_EXEC:
            (this->*(it->second.executer))(dataConn, arg);
            break;
        case HFP_AG_CMD_UNKNOWN:
            LOG_INFO("[HFP AG]%{public}s():HFP_AG_CMD_UNKNOWN", __FUNCTION__);
            break;
        default:
            LOG_INFO("[HFP AG]%{public}s():default", __FUNCTION__);
            break;
    }
    return;
}

void HfpAgCommandProcessor::AtEmptyFn(HfpAgDataConnection &dataConn, const std::string &arg)
{
    LOG_WARN("[HFP AG]%{public}s():Unsupported at command arg[%{public}s]", __FUNCTION__, arg.c_str());
    SendErrorCode(dataConn, HFP_AG_ERROR_AG_FAILURE);
}

void HfpAgCommandProcessor::AtaExecuter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    SendAtCommand(dataConn, OK);
    HfpAgProfileEventSender::GetInstance().AnswerCall(dataConn.remoteAddr_);
}

void HfpAgCommandProcessor::AtdExecuter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    std::string number = arg;
    number.erase(remove_if(number.begin(), number.end(), isspace), number.end());
    int type = HFP_AG_NUMBER_DIAL;

    if (number.length() <= ATD_VALID_LENGTH) {  // number should contains a number and a ';'
        SendErrorCode(dataConn, HFP_AG_ERROR_INVALID_CHARS_IN_DIAL_STRING);
        return;
    }

    if (number.at(0) == '>') {  // ATD> memory dial
        const std::regex baseRegex("[^\\d>;]");
        if (std::regex_search(number, baseRegex) || number.at(number.length() - 1) != ';') {
            SendErrorCode(dataConn, HFP_AG_ERROR_INVALID_CHARS_IN_DIAL_STRING);
            return;
        }

        type = HFP_AG_MEMORY_DIAL;
        number = number.substr(1, number.length() - ATD_VALID_LENGTH);
    } else {  // specific number dial
        const std::regex baseRegex("[^\\d*#+;]");
        if (std::regex_search(number, baseRegex) || number.at(number.length() - 1) != ';') {
            SendErrorCode(dataConn, HFP_AG_ERROR_INVALID_CHARS_IN_DIAL_STRING);
            return;
        }
        number = number.substr(0, number.length() - 1);
    }
    HfpAgProfileEventSender::GetInstance().DialOutCall(dataConn.remoteAddr_, number, type);
}

void HfpAgCommandProcessor::VgsSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    int val = StoiTryCatch(dataConn, arg);
    SendAtCommand(dataConn, OK);
    HfpAgProfileEventSender::GetInstance().HfVolumeChanged(dataConn.remoteAddr_, HFP_AG_VOLUME_TYPE_SPK, val);
}
void HfpAgCommandProcessor::VgmSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    int val = StoiTryCatch(dataConn, arg);
    SendAtCommand(dataConn, OK);
    HfpAgProfileEventSender::GetInstance().HfVolumeChanged(dataConn.remoteAddr_, HFP_AG_VOLUME_TYPE_MIC, val);
}

void HfpAgCommandProcessor::CcwaSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    dataConn.ccwaEnabled_ = StoiTryCatch(dataConn, arg);
    SendAtCommand(dataConn, OK);
}

void HfpAgCommandProcessor::ChldSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    int chld = StoiTryCatch(dataConn, arg);
    LOG_INFO("[HFP AG] ChldSetter chld = %{public}d, arg = %{public}s", chld, arg.c_str());
    if (!(dataConn.g_localFeatures & HFP_AG_FEATURES_ENHANCED_CALL_CONTROL) &&
        (dataConn.remoteFeatures_ & HFP_AG_HF_FEATURES_ENHANCED_CALL_CONTROL)) {
        if (chld != CHLD_RELEASE_ALL_HELD_CALLS && chld != CHLD_RELEASE_ACTIVE_ACCPET_OTHER &&
            chld != CHLD_RELEASE_HOLD_ACCPET_OTHER && chld != CHLD_ADD_CALL_TO_CONVERSATION &&
            chld != CHLD_CONNECT_TWO_CALL && chld != CHLD_RELEASE_INDEX_ONE && chld != CHLD_RELEASE_INDEX_TWO &&
            chld != CHLD_CONSULTATION_INDEX_ONE && chld != CHLD_CONSULTATION_INDEX_TWO) {
            // we does not support enhanced call control(AT+CHLD=1<idx> / AT+CHLD=2<idx>)
            SendErrorCode(dataConn, HFP_AG_ERROR_AG_FAILURE);
            return;
        }
    }

    HfpAgProfileEventSender::GetInstance().HoldCall(dataConn.remoteAddr_, chld);
}

void HfpAgCommandProcessor::ChldTester(HfpAgDataConnection &dataConn, const std::string &arg)
{
    if ((dataConn.sdpInfo_.remoteVersion >= HFP_AG_HFP_VERSION_1_5) &&
        (dataConn.g_localFeatures & HFP_AG_FEATURES_ENHANCED_CALL_CONTROL) &&
        (dataConn.remoteFeatures_ & HFP_AG_HF_FEATURES_ENHANCED_CALL_CONTROL)) {
        SendAtCommand(dataConn, "+CHLD: " + HfpAgDataConnection::CHLD_ECC_SETTINGS);
    } else {
        SendAtCommand(dataConn, "+CHLD: " + HfpAgDataConnection::CHLD_SETTINGS);
    }

    if (!dataConn.slcConnected_ && !((dataConn.g_localFeatures & HFP_AG_FEATURES_HF_INDICATORS) &&
        (dataConn.remoteFeatures_ & HFP_AG_HF_FEATURES_HF_INDICATORS))) {
        dataConn.SetSlcConnectState(true);
        HfpAgProfileEventSender::GetInstance().UpdateConnectState(dataConn.remoteAddr_, HFP_AG_SLC_ESTABLISHED_EVT);
    }
    SendAtCommand(dataConn, OK);
}

void HfpAgCommandProcessor::ChupExecuter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    if (HfpAgAudioConnection::GetActiveDevice().compare(dataConn.remoteAddr_) != 0) {
        LOG_INFO("[HFP AG]%{public}s():AT+CHUP failed because of device is not active, "
            "activeAddr[%{public}s], remoteAddr_[%{public}s]",
            __FUNCTION__,
            HfpAgAudioConnection::GetActiveDevice().c_str(),
            dataConn.remoteAddr_.c_str());
        SendErrorCode(dataConn, HFP_AG_ERROR_OPERATION_NOT_ALLOWED);
    } else {
        SendAtCommand(dataConn, OK);
        HfpAgProfileEventSender::GetInstance().HangupCall(dataConn.remoteAddr_);
    }
}

void HfpAgCommandProcessor::CindGetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    HfpAgProfileEventSender::GetInstance().GetAgIndicator(dataConn.remoteAddr_);
}

void HfpAgCommandProcessor::CindTester(HfpAgDataConnection &dataConn, const std::string &arg)
{
    SendAtCommand(dataConn, "+CIND: " + HfpAgDataConnection::CIND_SETTINGS);
    SendAtCommand(dataConn, OK);
}

void HfpAgCommandProcessor::ClipSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    dataConn.clipEnabled_ = StoiTryCatch(dataConn, arg);
    SendAtCommand(dataConn, OK);
}

void HfpAgCommandProcessor::CmerSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    int data[CMER_ELEMENTS_NUMBER] = {-1, -1, -1, -1};
    int offset = 0;

    int res = sscanf_s(arg.c_str(), "%d,%d,%d,%d%n", &data[CMER_MODE_INDEX], &data[CMER_KEYP_INDEX],
        &data[CMER_DISP_INDEX], &data[CMER_IND_INDEX], &offset);
    HILOGI("[HFP AG]CmerSetter:(%{public}d,%{public}d,%{public}d,%{public}d), res:%{public}d), offset:%{public}d",
        data[CMER_MODE_INDEX], data[CMER_KEYP_INDEX], data[CMER_DISP_INDEX], data[CMER_IND_INDEX], res, offset);
    if (res != CMER_ELEMENTS_NUMBER || offset == 0) {
        goto error;
    } else {
        if (data[CMER_MODE_INDEX] == CMER_MODE_FOR_HFP && data[CMER_IND_INDEX] == 1) {
            dataConn.cmerEnabled_ = true;
        } else if (data[CMER_MODE_INDEX] == CMER_MODE_FOR_HFP && data[CMER_IND_INDEX] == 0) {
            dataConn.cmerEnabled_ = false;
        } else {
            goto error;
        }
    }

    if (!dataConn.slcConnected_ && !((dataConn.g_localFeatures & HFP_AG_FEATURES_THREE_WAY) &&
        (dataConn.remoteFeatures_ & HFP_AG_HF_FEATURES_THREE_WAY))) {
        dataConn.SetSlcConnectState(true);
        HfpAgProfileEventSender::GetInstance().UpdateConnectState(dataConn.remoteAddr_, HFP_AG_SLC_ESTABLISHED_EVT);
    }
    SendAtCommand(dataConn, OK);
    return;

error:
    SendAtCommand(dataConn, ERROR);
}

void HfpAgCommandProcessor::VtsSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    if (arg.length() == 1) {
        uint8_t code = arg[0];
        if ((code >= '0' && code <= '9') || code == '*' || code == '#') {
            HfpAgProfileEventSender::GetInstance().SendDtmf(dataConn.remoteAddr_, code);
        } else {
            goto error;
        }
    } else {
        goto error;
    }
    return;

error:
    SendErrorCode(dataConn, HFP_AG_ERROR_AG_FAILURE);
    LOG_ERROR("[HFP HF]%{public}s():receive invalid dtmf code", __FUNCTION__);
}

void HfpAgCommandProcessor::BldnExecuter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    HfpAgProfileEventSender::GetInstance().DialOutCall(dataConn.remoteAddr_, "", HFP_AG_LAST_NUMBER_REDIAL);
}

void HfpAgCommandProcessor::BvraSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    int bvra = StoiTryCatch(dataConn, arg);
    HfpAgProfileEventSender::GetInstance().VoiceRecognitionStateChanged(dataConn.remoteAddr_, bvra);
}

void HfpAgCommandProcessor::BrsfSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    dataConn.remoteFeatures_ = StoiTryCatch(dataConn, arg);
    uint32_t features = dataConn.g_localFeatures;
    if (dataConn.sdpInfo_.remoteVersion < HFP_AG_HFP_VERSION_1_7) {
        features &= HFP_AG_1_6_FEATURES_MASK;
    }

    std::string cmd = "+BRSF: " + std::to_string(features);
    SendAtCommand(dataConn, cmd);
    SendAtCommand(dataConn, OK);
}

void HfpAgCommandProcessor::NrecSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    if (dataConn.g_localFeatures & HFP_AG_FEATURES_ECNR) {
        int nrec = StoiTryCatch(dataConn, arg);
        if (nrec == 0 || nrec == 1) {
            SendAtCommand(dataConn, OK);
            HfpAgProfileEventSender::GetInstance().EnableNoiseReduction(dataConn.remoteAddr_, nrec);
        } else {
            SendErrorCode(dataConn, HFP_AG_ERROR_AG_FAILURE);
        }
    } else {
        SendErrorCode(dataConn, HFP_AG_ERROR_OPERATION_NOT_SUPPORTED);
    }
}

void HfpAgCommandProcessor::CnumExecuter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    HfpAgProfileEventSender::GetInstance().GetSubscriberNumber(dataConn.remoteAddr_);
}

void HfpAgCommandProcessor::ClccExecuter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    if (!(dataConn.g_localFeatures & HFP_AG_FEATURES_ENHANCED_CALL_STATUS)) {
        SendErrorCode(dataConn, HFP_AG_ERROR_AG_FAILURE);
        return;
    }
    HfpAgProfileEventSender::GetInstance().GetCurrentCalls(dataConn.remoteAddr_);
}

void HfpAgCommandProcessor::CopsGetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    HfpAgProfileEventSender::GetInstance().GetNetworkOperater(dataConn.remoteAddr_);
}

void HfpAgCommandProcessor::CopsSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    if (arg == "3,0") {  // set name format to long alphanumeric
        SendAtCommand(dataConn, OK);
    } else {
        SendErrorCode(dataConn, HFP_AG_ERROR_AG_FAILURE);
    }
}

void HfpAgCommandProcessor::CmeeSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    if (dataConn.g_localFeatures & HFP_AG_FEATURES_EXTEND_ERROR_CODE) {
        int cmee = StoiTryCatch(dataConn, arg);
        dataConn.cmeeEnabled_ = static_cast<bool>(cmee);
        SendAtCommand(dataConn, OK);
    } else {
        SendErrorCode(dataConn, HFP_AG_ERROR_OPERATION_NOT_SUPPORTED);
    }
}

void HfpAgCommandProcessor::BiaSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    size_t i;
    int indId;
    size_t len = arg.length();
    int biaMaskOut = dataConn.biaMaskOut_;
    for (i = 0, indId = 1; i < len && indId <= MAX_AG_INDICATORS; i++) {
        if (arg.c_str()[i] == ',') {
            indId++;
            continue;
        }

        if (arg.c_str()[i] == '0') {
            biaMaskOut |= 1 << indId;
        } else if (arg.c_str()[i] == '1') {
            biaMaskOut &= ~(1 << indId);
        } else {
            break;
        }

        if (i + 1 < len) {
            if (arg.c_str()[i + 1] != ',') {
                break;
            }
        } else {
            continue;
        }
    }

    LOG_INFO("[HFP AG]%{public}s(): i[%zu], indId[%{public}d], len[%zu]", __FUNCTION__, i, indId, len);

    if (i == len) {
        SendAtCommand(dataConn, OK);

        dataConn.biaMaskOut_ = biaMaskOut;
        bool service = !(biaMaskOut & (1 << HFP_AG_INDICATOR_SERVICE));
        bool roam = !(biaMaskOut & (1 << HFP_AG_INDICATOR_ROAMING_STATE));
        bool signal = !(biaMaskOut & (1 << HFP_AG_INDICATOR_SIGNAL_STRENGTH));
        bool battery = !(biaMaskOut & (1 << HFP_AG_INDICATOR_BATTERY_LEVEL));

        HfpAgTransferData data = {service, roam, signal, battery};
        HfpAgProfileEventSender::GetInstance().ProcessAtBia(dataConn.remoteAddr_, data);
    } else {
        SendErrorCode(dataConn, HFP_AG_ERROR_INVALID_INDEX);
    }
}

void HfpAgCommandProcessor::BccExecuter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    if (HfpAgAudioConnection::GetActiveDevice().compare(dataConn.remoteAddr_) != 0) {
        SendErrorCode(dataConn, HFP_AG_ERROR_OPERATION_NOT_ALLOWED);
        LOG_WARN("[HFP AG]%{public}s():AT+BCC be rejected because activeAddr[%{public}s], remoteAddr[%{public}s]",
            __FUNCTION__,
            HfpAgAudioConnection::GetActiveDevice().c_str(),
            dataConn.remoteAddr_.c_str());
    } else {
        SendAtCommand(dataConn, OK);
        HfpAgProfileEventSender::GetInstance().ProcessAtBcc(dataConn.remoteAddr_);
    }
}

void HfpAgCommandProcessor::BcsSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    dataConn.remoteSelectedCodec_ = StoiTryCatch(dataConn, arg);
    dataConn.codecNegotiating_ = false;
    if (dataConn.remoteSelectedCodec_ == dataConn.localSelectedCodec_) {
        dataConn.inUseCodec_ = dataConn.remoteSelectedCodec_;
        SendAtCommand(dataConn, OK);
        HfpAgProfileEventSender::GetInstance().ProcessAtBcs(dataConn.remoteAddr_);
    } else {
        LOG_ERROR("[HFP AG]%{public}s():Setup code connection failed", __FUNCTION__);
        SendErrorCode(dataConn, HFP_AG_ERROR_AG_FAILURE);
    }
}

void HfpAgCommandProcessor::BindGetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    std::string cmd;
    for (auto &indicator : dataConn.localHfIndicators_) {
        if (indicator.isSupported) {
            cmd = "+BIND: " + std::to_string(indicator.anum) + "," + std::to_string(indicator.isEnabled);
            SendAtCommand(dataConn, cmd);
        }
    }

    if (!dataConn.slcConnected_) {
        dataConn.SetSlcConnectState(true);
        HfpAgProfileEventSender::GetInstance().UpdateConnectState(dataConn.remoteAddr_, HFP_AG_SLC_ESTABLISHED_EVT);
    }
    SendAtCommand(dataConn, OK);
}

void HfpAgCommandProcessor::BindSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    bool isFirst = true;
    std::string event;
    std::regex regex("\\,");
    std::vector<std::string> out(
        std::sregex_token_iterator(arg.begin(), arg.end(), regex, -1), std::sregex_token_iterator());
    for (auto &s : out) {
        HfpAgDataConnection::HfIndicator indicator;
        indicator.anum = StoiTryCatch(dataConn, s);
        dataConn.remoteHfIndicators_.push_back(indicator);
        if (isFirst) {
            event = event + std::to_string(indicator.anum);
        } else {
            event = "," + event + std::to_string(indicator.anum);
        }
        isFirst = false;
    }

    SendAtCommand(dataConn, OK);
    HfpAgProfileEventSender::GetInstance().ProcessATBind(dataConn.remoteAddr_, event);
}

void HfpAgCommandProcessor::BindTester(HfpAgDataConnection &dataConn, const std::string &arg)
{
    bool isFirst = true;
    std::string cmd;

    for (auto &indicator : dataConn.localHfIndicators_) {
        if (indicator.isSupported) {
            if (isFirst) {
                cmd = cmd + std::to_string(indicator.anum);
            } else {
                cmd = cmd + "," + std::to_string(indicator.anum);
            }
            isFirst = false;
        }
    }

    cmd = "(" + cmd + ")";
    cmd = "+BIND: " + cmd;
    SendAtCommand(dataConn, cmd);
    SendAtCommand(dataConn, OK);
}

void HfpAgCommandProcessor::BievSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    int index;
    int anumOut;
    uint16_t anum;
    uint16_t value;
    std::regex regex("\\,");
    std::vector<std::string> out(
        std::sregex_token_iterator(arg.begin(), arg.end(), regex, -1), std::sregex_token_iterator());
    if (out.size() != BIEV_ARGS_VALID_LENGTH) {
        goto INDEX_ERROR;
    }

    anumOut = StoiTryCatch(dataConn, out[0]);
    if (anumOut <= 0 || anumOut > LOCAL_HF_IND_NUM) {
        goto INDEX_ERROR;
    } else {
        anum = anumOut;
    }

    for (index = 0; index < static_cast<int>(dataConn.localHfIndicators_.size()); index++) {
        if (anum == dataConn.localHfIndicators_[index].anum) {
            break;
        }
    }

    if (index == static_cast<int>(dataConn.localHfIndicators_.size()) ||
        !dataConn.localHfIndicators_[index].isEnabled || !dataConn.localHfIndicators_[index].isSupported) {
        goto INDEX_ERROR;
    }

    value = StoiTryCatch(dataConn, out[1]);
    if (value < dataConn.localHfIndicators_[index].minVal || value > dataConn.localHfIndicators_[index].maxVal) {
        SendErrorCode(dataConn, HFP_AG_ERROR_AG_FAILURE);
        return;
    } else {
        SendAtCommand(dataConn, OK);
        HfpAgProfileEventSender::GetInstance().SendHfIndicator(dataConn.remoteAddr_, ++index, value);
        return;
    }

INDEX_ERROR:
    SendErrorCode(dataConn, HFP_AG_ERROR_INVALID_INDEX);
    return;
}

void HfpAgCommandProcessor::BacSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    std::regex regex("\\,");
    std::vector<std::string> out(
        std::sregex_token_iterator(arg.begin(), arg.end(), regex, -1), std::sregex_token_iterator());

    int currentRemoteCodec = HFP_AG_CODEC_NONE;
    for (auto &s : out) {
        if (s.find("1") != std::string::npos) {
            currentRemoteCodec |= HFP_AG_CODEC_CVSD;
        }
        if (s.find("2") != std::string::npos) {
            currentRemoteCodec |= HFP_AG_CODEC_MSBC;
        }
    }

    if (currentRemoteCodec != dataConn.remoteSupportCodecs_) {
        LOG_INFO(
            "[HFP AG]%{public}s():Remote support codecs updated, currentRemoteCodec[%{public}d], remoteSupportCodecs_[%{public}d]",
            __FUNCTION__,
            currentRemoteCodec,
            dataConn.remoteSupportCodecs_);
        dataConn.remoteSupportCodecsUpdated_ = true;
    }
    dataConn.remoteSupportCodecs_ = currentRemoteCodec;

    SendAtCommand(dataConn, OK);

    // If no AT+BCS is received, but instead an AT+BAC is received after sending +BCS,
    // the procedure shall end but may be restarted by the AG after re-selecting codec ID
    // based on the information in the just received AT+BAC.
    if (dataConn.codecNegotiating_) {
        LOG_INFO("[HFP AG]%{public}s():Codec negotiation failed", __FUNCTION__);
        HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(
            dataConn.remoteAddr_, HFP_AG_CODEC_NEGOTIATION_FAILED);
    }
}

void HfpAgCommandProcessor::BtrhGetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    LOG_INFO("[HFP AG]%{public}s():BtrhGetter arg = %{public}s", __FUNCTION__, arg.c_str());
    HfpAgProfileEventSender::GetInstance().GetResponseHoldState(dataConn.remoteAddr_, HFP_AG_GET_BTRH_EVT);
}

void HfpAgCommandProcessor::BtrhSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    LOG_INFO("[HFP AG]%{public}s():BtrhSetter arg = %{public}s", __FUNCTION__, arg.c_str());
    int btrh = StoiTryCatch(dataConn, arg);
    HfpAgProfileEventSender::GetInstance().SetResponseHoldState(dataConn.remoteAddr_, HFP_AG_GET_BTRH_EVT, btrh);
}

void HfpAgCommandProcessor::BinpSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    LOG_INFO("[HFP AG]%{public}s():BinpGetter arg = %{public}s", __FUNCTION__, arg.c_str());
    HfpAgProfileEventSender::GetInstance().GetVoiceTagNumber(dataConn.remoteAddr_);
}

void HfpAgCommandProcessor::CkpdSetter(HfpAgDataConnection &dataConn, const std::string &arg)
{
    SendAtCommand(dataConn, OK);
    HfpAgProfileEventSender::GetInstance().ProcessCKpdEvent(dataConn.remoteAddr_);
}
}  // namespace bluetooth
}  // namespace OHOS