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

#include "hfp_hf_command_processor.h"

#include <regex>

#include "adapter_config.h"
#include "hfp_hf_profile_event_sender.h"
#include "log.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
std::unordered_map<std::string, HfpHfCommandProcessor::HfpHfAtHandler> HfpHfCommandProcessor::g_atCmdMap = {
    std::make_pair<std::string, HfpHfAtHandler>("OK", {&HfpHfCommandProcessor::ProcessOK}),
    std::make_pair<std::string, HfpHfAtHandler>("ERROR", {&HfpHfCommandProcessor::ProcessErrorCmd}),
    std::make_pair<std::string, HfpHfAtHandler>("+CME ERROR:", {&HfpHfCommandProcessor::ProcessCmeError}),
    std::make_pair<std::string, HfpHfAtHandler>("RING", {&HfpHfCommandProcessor::ProcessRing}),
    std::make_pair<std::string, HfpHfAtHandler>("+CLIP:", {&HfpHfCommandProcessor::ProcessClip}),
    std::make_pair<std::string, HfpHfAtHandler>("+BRSF:", {&HfpHfCommandProcessor::ProcessBrsf}),
    std::make_pair<std::string, HfpHfAtHandler>("+CIND:", {&HfpHfCommandProcessor::ProcessCind}),
    std::make_pair<std::string, HfpHfAtHandler>("+CHLD:", {&HfpHfCommandProcessor::ProcessChld}),
    std::make_pair<std::string, HfpHfAtHandler>("+BIND:", {&HfpHfCommandProcessor::ProcessBind}),
    std::make_pair<std::string, HfpHfAtHandler>("+CIEV:", {&HfpHfCommandProcessor::ProcessCiev}),
    std::make_pair<std::string, HfpHfAtHandler>("+CCWA:", {&HfpHfCommandProcessor::ProcessCcwa}),
    std::make_pair<std::string, HfpHfAtHandler>("+BCS:", {&HfpHfCommandProcessor::ProcessBcs}),
    std::make_pair<std::string, HfpHfAtHandler>("+CLCC:", {&HfpHfCommandProcessor::ProcessClcc}),
    std::make_pair<std::string, HfpHfAtHandler>("+BSIR:", {&HfpHfCommandProcessor::ProcessBsir}),
    std::make_pair<std::string, HfpHfAtHandler>("+BVRA:", {&HfpHfCommandProcessor::ProcessBvra}),
    std::make_pair<std::string, HfpHfAtHandler>("+CNUM:", {&HfpHfCommandProcessor::ProcessCnum}),
    std::make_pair<std::string, HfpHfAtHandler>("+VGM:", {&HfpHfCommandProcessor::ProcessVgm}),
    std::make_pair<std::string, HfpHfAtHandler>("+VGS:", {&HfpHfCommandProcessor::ProcessVgs}),
    std::make_pair<std::string, HfpHfAtHandler>("+COPS:", {&HfpHfCommandProcessor::ProcessCops}),
    std::make_pair<std::string, HfpHfAtHandler>("+BTRH:", {&HfpHfCommandProcessor::ProcessBtrh}),
    std::make_pair<std::string, HfpHfAtHandler>("BUSY", {&HfpHfCommandProcessor::ProcessBusy}),
    std::make_pair<std::string, HfpHfAtHandler>("DELAYED", {&HfpHfCommandProcessor::ProcessDelayed}),
    std::make_pair<std::string, HfpHfAtHandler>("NO CARRIER", {&HfpHfCommandProcessor::ProcessNoCarrier}),
    std::make_pair<std::string, HfpHfAtHandler>("NO ANSWER", {&HfpHfCommandProcessor::ProcessNoAnswer}),
    std::make_pair<std::string, HfpHfAtHandler>("BLOCKLISTED", {&HfpHfCommandProcessor::ProcessBlocklisted}),
};

std::unordered_map<std::string, HfpHfCommandProcessor::HfpHfAtHandler> &HfpHfCommandProcessor::GetAtCmdMap()
{
    return g_atCmdMap;
}

int HfpHfCommandProcessor::StoiTryCatch(const std::string &arg)
{
    try {
        return std::stoi(arg);
    } catch (std::exception &e) {
        LOG_ERROR("[HFP HF]%{public}s():Catch exception %{public}s", __FUNCTION__, e.what());
        return 0;
    }
}

void HfpHfCommandProcessor::Init(const std::string& address)
{
    this->address_ = address;
    AdapterConfig::GetInstance()->GetValue(HSP_HS_STATE_SECTION_NAME, HSP_HS_STATE_PROPERY_NAME, hspState_);
    respTimer_ = std::make_unique<utility::Timer>(std::bind(&HfpHfCommandProcessor::RespondTimeout, this));
}

void HfpHfCommandProcessor::ProcessCommand(
    HfpHfDataConnection &dataConn, const std::string &cmd, const std::string &arg)
{
    LOG_DEBUG("[HFP HF]%{public}s():command[%{public}s], arg[%{public}s]", __FUNCTION__, cmd.c_str(), arg.c_str());

    auto it = g_atCmdMap.find(cmd);
    if (it == g_atCmdMap.end()) {
        LOG_ERROR("[HFP HF]%{public}s():%{public}s command handler not found", __FUNCTION__, cmd.c_str());
        return;
    }

    auto newArg = arg;
    newArg.erase(remove_if(newArg.begin(), newArg.end(), isspace), newArg.end());

    (this->*(it->second.fn))(dataConn, newArg);
}

void HfpHfCommandProcessor::ProcessOK(HfpHfDataConnection &dataConn, const std::string &arg)
{
    respTimer_->Stop();

    if (!dataConn.slcConnected_) {
        ConnectSlc(dataConn);
        return;
    }

    HfpHfProfileEventSender &sender = HfpHfProfileEventSender::GetInstance();
    switch (currentCommand_) {
        case AT_COMMAND_NONE:
        case AT_BIA_SETTER:
        case AT_BCC_EXECUTER:
        case AT_CLIP_SETTER:
        case AT_BCS_SETTER:
            break;
        case AT_CLCC_EXECUTER:
            sender.CurrentCallsReplyDone(dataConn.remoteAddr_);
            break;
        case AT_BVRA_1_SETTER:
            sender.OpenVoiceRecognitionResult(dataConn.remoteAddr_, HFP_HF_AT_RESULT_OK);
            break;
        case AT_BVRA_0_SETTER:
            sender.CloseVoiceRecognitionResult(dataConn.remoteAddr_, HFP_HF_AT_RESULT_OK);
            break;
        default:
            sender.AtCmdResultReply(dataConn.remoteAddr_, HFP_HF_AT_RESULT_OK, 0);
            break;
    }

    currentCommand_ = AT_COMMAND_NONE;
    SendQueuedAtCommand(dataConn);
}

void HfpHfCommandProcessor::ProcessCmeError(HfpHfDataConnection &dataConn, const std::string &arg)
{
    const std::regex baseRegex("[^\\d]");
    if (!std::regex_search(arg, baseRegex)) {
        int errorCode = StoiTryCatch(arg);
        if (errorCode != INVALID_CME_ERROR_CODE) {
            ProcessErrorCode(dataConn, errorCode);
        } else {
            LOG_ERROR("[HFP HF]%{public}s():Invalid error code[%{public}s]", __FUNCTION__, arg.c_str());
        }
    } else {
        LOG_ERROR("[HFP HF]%{public}s():Invalid error code[%{public}s]", __FUNCTION__, arg.c_str());
    }
}

void HfpHfCommandProcessor::ProcessErrorCmd(HfpHfDataConnection &dataConn, const std::string &arg)
{
    ProcessErrorCode(dataConn, INVALID_CME_ERROR_CODE);
}

void HfpHfCommandProcessor::ProcessErrorCode(HfpHfDataConnection &dataConn, int errorCode)
{
    respTimer_->Stop();

    if (!dataConn.slcConnected_) {
        ConnectSlc(dataConn);
        return;
    }

    HfpHfProfileEventSender &sender = HfpHfProfileEventSender::GetInstance();
    switch (currentCommand_) {
        case AT_BIA_SETTER:
        case AT_BCC_EXECUTER:
        case AT_BCS_SETTER:
        case AT_CLIP_SETTER:
            break;
        default:
            if (errorCode == INVALID_CME_ERROR_CODE) {
                sender.AtCmdResultReply(dataConn.remoteAddr_, HFP_HF_AT_RESULT_ERROR, 0);
            } else {
                sender.AtCmdResultReply(dataConn.remoteAddr_, HFP_HF_AT_RESULT_CME, errorCode);
            }
            break;
    }

    currentCommand_ = AT_COMMAND_NONE;
    SendQueuedAtCommand(dataConn);
}

void HfpHfCommandProcessor::ProcessCops(HfpHfDataConnection &dataConn, const std::string &arg)
{
    uint8_t mode = 0;
    char operatorName[OPERATOR_NAME_MAX_LEN] = {0};    // Operator name max length is 16

    int res = sscanf_s(arg.c_str(), "%hhi,0,\"%16[^\"]\"", &mode, operatorName, sizeof(operatorName));
    if (res < COPS_ARGS_NUMBER) {
        LOG_WARN("[HFP HF]%{public}s():Invalid command format arg[%{public}s]", __FUNCTION__, arg.c_str());
        return;
    }

    std::string name = std::string(operatorName);
    HfpHfProfileEventSender::GetInstance().CurrentOperatorReply(dataConn.remoteAddr_, name);
}

void HfpHfCommandProcessor::ProcessBtrh(HfpHfDataConnection &dataConn, const std::string &arg)
{
    int response = 0;

    int res = sscanf_s(arg.c_str(), "%d", &response);
    if (res < 1) {
        HILOGE("[HFP HF]ProcessBtrh failed, res:%{public}d, arg:%{public}s", res, arg.c_str());
        return;
    }

    HfpHfProfileEventSender::GetInstance().HoldResultReply(dataConn.remoteAddr_, response);
}

void HfpHfCommandProcessor::ProcessBusy(HfpHfDataConnection &dataConn, const std::string &arg)
{
    HfpHfProfileEventSender::GetInstance().AtCmdResultReply(
        dataConn.remoteAddr_, HFP_HF_AT_RESULT_BUSY, INVALID_CME_ERROR_CODE);
}

void HfpHfCommandProcessor::ProcessDelayed(HfpHfDataConnection &dataConn, const std::string &arg)
{
    HfpHfProfileEventSender::GetInstance().AtCmdResultReply(
        dataConn.remoteAddr_, HFP_HF_AT_RESULT_DELAYED, INVALID_CME_ERROR_CODE);
}

void HfpHfCommandProcessor::ProcessNoCarrier(HfpHfDataConnection &dataConn, const std::string &arg)
{
    HfpHfProfileEventSender::GetInstance().AtCmdResultReply(
        dataConn.remoteAddr_, HFP_HF_AT_RESULT_NO_CARRIER, INVALID_CME_ERROR_CODE);
}

void HfpHfCommandProcessor::ProcessNoAnswer(HfpHfDataConnection &dataConn, const std::string &arg)
{
    HfpHfProfileEventSender::GetInstance().AtCmdResultReply(
        dataConn.remoteAddr_, HFP_HF_AT_RESULT_NO_ANSWER, INVALID_CME_ERROR_CODE);
}

void HfpHfCommandProcessor::ProcessBlocklisted(HfpHfDataConnection &dataConn, const std::string &arg)
{
    HfpHfProfileEventSender::GetInstance().AtCmdResultReply(
        dataConn.remoteAddr_, HFP_HF_AT_RESULT_BLOCKLISTED, INVALID_CME_ERROR_CODE);
}

void HfpHfCommandProcessor::ProcessRing(HfpHfDataConnection &dataConn, const std::string &arg)
{
    HfpHfProfileEventSender::GetInstance().RingIndicationNotify(dataConn.remoteAddr_);
}

void HfpHfCommandProcessor::ProcessClip(HfpHfDataConnection &dataConn, const std::string &arg)
{
    HfpHfProfileEventSender::GetInstance().Callinglineidentification(dataConn.remoteAddr_, arg);
}

void HfpHfCommandProcessor::ProcessBrsf(HfpHfDataConnection &dataConn, const std::string &arg)
{
    dataConn.remoteFeatures_ = StoiTryCatch(arg);
}

void HfpHfCommandProcessor::ProcessCindTester(HfpHfDataConnection &dataConn, std::string arg)
{
    int offset = 0;
    uint32_t min = 0;
    uint32_t max = 0;
    uint32_t index = 0;
    char name[INDICATOR_NAME_MAX_LEN] = {0};
    const char *buf = arg.c_str();

    for (; ;) {
        auto res = sscanf_s(buf, "(\"%16[^\"]\",(%u%*[-,]%u))%n", name, sizeof(name), &min, &max, &offset);
        LOG_DEBUG(
            "[HFP HF]%{public}s():name[%{public}s], min[%u], max[%u], offset[%{public}d], res[%{public}d]", __FUNCTION__, name, min, max, offset, res);
        if (res != CHLD_TEST_ARGS_NUMBER) {
            LOG_WARN("[HFP HF]%{public}s():invalid command name[%{public}s], min[%u], max[%u], offset[%{public}d], res[%{public}d]",
                __FUNCTION__,
                name,
                min,
                max,
                offset,
                res);
            return;
        }

        HfpHfDataConnection::AgIndicator indicator {name, ++index, min, max, 0, true};
        dataConn.remoteAgIndicators_.push_back(indicator);

        buf += offset;
        if (*buf != ',') {
            break;
        }
        buf++;
    }
}

void HfpHfCommandProcessor::ProcessClcc(HfpHfDataConnection &dataConn, const std::string &arg)
{
    uint16_t idx = 0;
    uint16_t dir = 0;
    uint16_t status = 0;
    uint16_t mode = 0;
    uint16_t mprty = 0;
    char number[PHONE_NUMBER_MAX_LEN] = {0};
    uint16_t type = 0;
    int offset = 0;

    const char *buf = arg.c_str();
    int res = sscanf_s(buf, "%hu,%hu,%hu,%hu,%hu%n", &idx, &dir, &status, &mode, &mprty, &offset);
    if (res < CLCC_ARGS_NUMBER || offset == 0) {
        LOG_WARN("[HFP HF]%{public}s():Invalid command format arg[%{public}s]", __FUNCTION__, arg.c_str());
        return;
    }

    buf += offset;
    offset = 0;

    /* check number and type */
    if (*buf == ',') {
        int res2 = sscanf_s(buf, ",\"%32[^\"]\",%hu%n", number, sizeof(number), &type, &offset);
        if (res2 < 0) {
            return;
        }

        if (res2 == 0) {
            res2 = sscanf_s(buf, ",\"\",%hu%n", &type, &offset);
            if (res2 < 0) {
                return;
            }

            /* number match failed */
            res2++;
            number[0] = '\0';
        }

        if (res2 >= CHLD_SUB_ARGS_NUMBER) {
            res += res2;
            if (offset == 0) {
                LOG_WARN("[HFP HF]%{public}s():Invalid command format arg[%{public}s]", __FUNCTION__, arg.c_str());
                return;
            }
        }
    }

    HfpHfCurrentCallData callData {idx, dir, status, mode, mprty, "", 0};
    if (res > CLCC_ARGS_NUMBER + 1) {
        callData.number = number;
        callData.type = type;
    }
    HfpHfProfileEventSender::GetInstance().CurrentCallsReply(dataConn.remoteAddr_, callData);
}

void HfpHfCommandProcessor::ProcessBsir(HfpHfDataConnection &dataConn, const std::string &arg)
{
    int inBand = (arg.length() == 1) ? StoiTryCatch(arg) : 0;
    HfpHfProfileEventSender::GetInstance().SetInBandRing(dataConn.remoteAddr_, inBand, false);
}

void HfpHfCommandProcessor::ProcessBvra(HfpHfDataConnection &dataConn, const std::string &arg)
{
    int bvra = (arg.length() == 1) ? StoiTryCatch(arg) : 0;
    HfpHfProfileEventSender::GetInstance().VoiceRecognitionChanged(dataConn.remoteAddr_, bvra);
}

void HfpHfCommandProcessor::ProcessCnum(HfpHfDataConnection &dataConn, const std::string &arg)
{
    char number[PHONE_NUMBER_MAX_LEN] = {0};
    uint16_t type;
    // Indicates which service this phone number relates to. Shall be either 4 (voice) or 5 (fax).
    uint16_t service = 0;

    int res = sscanf_s(arg.c_str(), ",\"%32[^\"]\",%hu,,%hu", number, sizeof(number), &type, &service);
    if (res < CNUM_ARGS_NUMBER) {
        LOG_WARN("[HFP HF]%{public}s():Invalid command format arg[%{public}s]", __FUNCTION__, arg.c_str());
        return;
    }

    HfpHfProfileEventSender::GetInstance().SubscriberNumberReply(dataConn.remoteAddr_, number, type);
}

void HfpHfCommandProcessor::ProcessCindGetter(HfpHfDataConnection &dataConn, std::string arg)
{
    size_t index = 0;
    uint32_t value = 0;
    int offset = 0;
    const char *buf = arg.c_str();

    for (; ;) {
        auto res = sscanf_s(buf, "%u%n", &value, &offset);
        if (res != 1) {
            LOG_WARN("[HFP HF]%{public}s():invalid command buf[%{public}s]", __FUNCTION__, buf);
            return;
        }

        auto size = dataConn.remoteAgIndicators_.size();
        if (index < size) {
            if ((value >= dataConn.remoteAgIndicators_[index].minVal) &&
                (value <= dataConn.remoteAgIndicators_[index].maxVal)) {
                dataConn.remoteAgIndicators_[index].value = value;
            } else {
                dataConn.remoteAgIndicators_[index].value = dataConn.remoteAgIndicators_[index].minVal;
            }
        }

        buf += offset;
        if (*buf != ',') {
            break;
        }

        index++;
        buf++;
    }

    for (auto it = dataConn.remoteAgIndicators_.begin(); it != dataConn.remoteAgIndicators_.end(); ++it) {
        ProcessIndicator(dataConn, *it, it->value);
    }
}

void HfpHfCommandProcessor::ProcessCind(HfpHfDataConnection &dataConn, const std::string &arg)
{
    if (arg.at(0) == '(') {
        // List items
        ProcessCindTester(dataConn, arg);
    } else {
        // Get values
        ProcessCindGetter(dataConn, arg);
    }
}

void HfpHfCommandProcessor::ProcessChld(HfpHfDataConnection &dataConn, const std::string &arg)
{
    if (arg.length() < CHLD_ARGS_MIN_LENGTH) {
        LOG_WARN("[HFP HF]%{public}s():Invalid Chld command [%{public}s] from Ag!", __FUNCTION__, arg.c_str());
    }

    std::string tmpStr = arg.substr(1, arg.length() - (CHLD_ARGS_MIN_LENGTH - 1));
    std::regex regex("\\,");
    std::vector<std::string> out(
        std::sregex_token_iterator(tmpStr.begin(), tmpStr.end(), regex, -1), std::sregex_token_iterator());
    for (auto s = out.begin(); s != out.end(); s++) {
        if (*s == "0") {
            dataConn.remoteChldFeatures_ |= CHLD_RELEASE_ALL;
        } else if (*s == "1") {
            dataConn.remoteChldFeatures_ |= CHLD_RELEASE_ALL_AND_ACCPET;
        } else if (*s == "1x") {
            dataConn.remoteChldFeatures_ |= CHLD_RELEASE_SPECIFIC;
        } else if (*s == "2") {
            dataConn.remoteChldFeatures_ |= CHLD_HOLD_ALL_AND_ACCPET;
        } else if (*s == "2x") {
            dataConn.remoteChldFeatures_ |= CHLD_HOLD_ALL_AND_PRIVATE;
        } else if (*s == "3") {
            dataConn.remoteChldFeatures_ |= CHLD_MERGE;
        } else if (*s == "4") {
            dataConn.remoteChldFeatures_ |= CHLD_EXPLICT_CALL_TRANSFER;
        } else {
            LOG_DEBUG("[HFP HF]%{public}s():Invalid Chld command [%{public}s] from Ag!", __FUNCTION__, (*s).c_str());
        }
    }
}

void HfpHfCommandProcessor::ProcessListHfIndicators(
    HfpHfDataConnection &dataConn, const std::string &arg)
{
    char anums[HF_INDICATOR_MAX] = {0};  // assume peer device support 20 hf indicators at most.

    int res = sscanf_s(arg.c_str(), "(%20[^)]", anums, sizeof(anums));
    if (res < 1) {
        LOG_WARN("[HFP HF]%{public}s():invalid command arg[%{public}s]", __FUNCTION__, arg.c_str());
    }

    std::string tmpStr = anums;
    std::regex regex("\\,");
    std::vector<std::string> out(
        std::sregex_token_iterator(tmpStr.begin(), tmpStr.end(), regex, -1), std::sregex_token_iterator());
    for (auto &s : out) {
        bool isExist = false;
        uint16_t anum = StoiTryCatch(s);

        auto it = std::find_if(dataConn.remoteHfIndicators_.begin(),
            dataConn.remoteHfIndicators_.end(),
            [&anum](const HfpHfDataConnection::HfIndicator &indicator) {
                return indicator.anum == anum;
            });

        if (it != dataConn.remoteHfIndicators_.end()) {
            isExist = true;
        }

        if (!isExist) {
            HfpHfDataConnection::HfIndicator hfIndicator = {anum, 0, 0, 0, true, true};
            dataConn.remoteHfIndicators_.push_back(hfIndicator);
        }
    }
}

void HfpHfCommandProcessor::ProcessChangeIndicatorState(
    HfpHfDataConnection &dataConn, const std::string &arg)
{
    uint16_t anum = 0;
    uint16_t value = 0;

    int res = sscanf_s(arg.c_str(), "%hu,%hu", &anum, &value);
    if (res != BIND_SET_ARGS_NUMBER) {
        LOG_WARN("[HFP HF]%{public}s():invalid command arg[%{public}s]", __FUNCTION__, arg.c_str());
    }

    for (auto &indicator : dataConn.remoteHfIndicators_) {
        if (indicator.anum == anum) {
            if (indicator.isEnabled == false && value == 1) {
                std::string cmd = "AT+BIEV=" + std::to_string(anum) + "," +
                                  std::to_string(dataConn.localHfIndicators_[anum - 1].value);
                SendAtCommand(dataConn, cmd, HfpHfCommandProcessor::AT_BIEV_SETTER);
            }
            indicator.isEnabled = value;
        }
    }
}

void HfpHfCommandProcessor::ProcessBind(HfpHfDataConnection &dataConn, const std::string &arg)
{
    if (arg.length() >= 1 && arg[0] == '(') {
        // List hf indicators
        ProcessListHfIndicators(dataConn, arg);
    } else {
        // Enable/Disable hf indicator
        ProcessChangeIndicatorState(dataConn, arg);
    }
}

void HfpHfCommandProcessor::ProcessIndicator(
    HfpHfDataConnection &dataConn, HfpHfDataConnection::AgIndicator &it, uint32_t value)
{
    HfpHfProfileEventSender &sender = HfpHfProfileEventSender::GetInstance();

    if ((value >= it.minVal) && (value <= it.maxVal)) {
        auto valueOrgi = it.value;
        it.value = value;
        if (it.name == "service") {
            sender.NetworkStateChanged(dataConn.remoteAddr_, value);
        } else if (it.name == "call") {
            sender.CallStateNotify(dataConn.remoteAddr_, value);
        } else if (it.name == "callsetup") {
            sender.CallSetupStateNotify(dataConn.remoteAddr_, value);
        } else if (it.name == "callheld") {
            sender.CallHeldStateNotify(dataConn.remoteAddr_, value);
        } else if (it.name == "signal") {
            sender.NetworkSignalChanged(dataConn.remoteAddr_, value);
        } else if (it.name == "roam") {
            sender.NetworkRoamingChanged(dataConn.remoteAddr_, value);
        } else if (it.name == "battchg") {
            sender.BatteryLevelChanged(dataConn.remoteAddr_, value);
        } else {
            it.value = valueOrgi;
            LOG_WARN("[HFP HF]%{public}s():Invalid indicator name [%{public}s]", __FUNCTION__, it.name.c_str());
        }
    } else {
        LOG_WARN("[HFP HF]%{public}s():Indicator value is not valid indicator[%{public}s], value[%u]",
            __FUNCTION__,
            it.name.c_str(),
            value);
    }
}

void HfpHfCommandProcessor::ProcessCiev(HfpHfDataConnection &dataConn, const std::string &arg)
{
    uint32_t index = 0;
    uint32_t value = 0;

    int res = sscanf_s(arg.c_str(), "%u,%u", &index, &value);
    if (res < CIEV_ARGS_NUMBER) {
        LOG_WARN("[HFP HF]%{public}s():invalid command arg[%{public}s]", __FUNCTION__, arg.c_str());
    }

    auto it = std::find_if(dataConn.remoteAgIndicators_.begin(),
        dataConn.remoteAgIndicators_.end(),
        [&index](const HfpHfDataConnection::AgIndicator &indicator) {
            return indicator.index == index;
        });

    if (it != dataConn.remoteAgIndicators_.end()) {
        ProcessIndicator(dataConn, *it, value);
    }
}

void HfpHfCommandProcessor::ProcessCcwa(HfpHfDataConnection &dataConn, const std::string &arg)
{
    char number[PHONE_NUMBER_MAX_LEN] = {0};
    uint32_t type = 0;

    int res = sscanf_s(arg.c_str(), "\"%32[^\"]\",%u", number, sizeof(number), &type);
    if (res < CCWA_ARGS_NUMBER) {
        LOG_WARN("[HFP HF]%{public}s():invalid command format arg[%{public}s]", __FUNCTION__, arg.c_str());
        return;
    }

    HfpHfProfileEventSender::GetInstance().CallWaitingNotify(dataConn.remoteAddr_, std::string(number));
}

void HfpHfCommandProcessor::ProcessVgm(HfpHfDataConnection &dataConn, const std::string &arg)
{
    int val = StoiTryCatch(arg);
    HfpHfProfileEventSender::GetInstance().SetVolume(dataConn.remoteAddr_, HFP_HF_VOLUME_TYPE_MIC, val);
}

void HfpHfCommandProcessor::ProcessVgs(HfpHfDataConnection &dataConn, const std::string &arg)
{
    int val = StoiTryCatch(arg);
    HfpHfProfileEventSender::GetInstance().SetVolume(dataConn.remoteAddr_, HFP_HF_VOLUME_TYPE_SPK, val);
}

void HfpHfCommandProcessor::ProcessSlcBrsf(HfpHfDataConnection &dataConn)
{
    if ((dataConn.g_localFeatures & HFP_HF_FEATURES_CODEC_NEGOTIATION) &&
        (dataConn.remoteFeatures_ & HFP_HF_AG_FEATURES_CODEC_NEGOTIATION)) {
        SendAtBacSetter(dataConn);
    } else {
        SendAtCindTester(dataConn);
    }
}
void HfpHfCommandProcessor::ProcessSlcCmer(HfpHfDataConnection &dataConn)
{
    if ((dataConn.g_localFeatures & HFP_HF_FEATURES_THREE_WAY) &&
        (dataConn.remoteFeatures_ & HFP_HF_AG_FEATURES_THREE_WAY)) {
        SendAtChldTester(dataConn);
    } else {
        ProcessSlcConnected(dataConn);
    }
}
void HfpHfCommandProcessor::ProcessSlcChld(HfpHfDataConnection &dataConn)
{
    if ((dataConn.g_localFeatures & HFP_HF_FEATURES_HF_INDICATORS) &&
        (dataConn.remoteFeatures_ & HFP_HF_AG_FEATURES_HF_INDICATORS)) {
        SendAtBindSetter(dataConn);
    } else {
        ProcessSlcConnected(dataConn);
    }
}

void HfpHfCommandProcessor::ProcessSlcConnected(HfpHfDataConnection &dataConn)
{
    HfpHfProfileEventSender &sender = HfpHfProfileEventSender::GetInstance();
    dataConn.SetSlcConnectState(true);
    sender.UpdateConnectState(dataConn.remoteAddr_, HFP_HF_SLC_ESTABLISHED_EVT);
    currentCommand_ = AT_COMMAND_NONE;

    if (dataConn.remoteFeatures_ & HFP_HF_AG_FEATURES_IN_BAND_RING) {
        sender.SetInBandRing(dataConn.remoteAddr_, 1, true);
    }

    SendAtCommand(dataConn, "AT+COPS=3,0", HfpHfCommandProcessor::AT_COPS_SETTER);
    SendAtBiaSetter(dataConn);
    SendAtCommand(dataConn, "AT+CLIP=1", HfpHfCommandProcessor::AT_CLIP_SETTER);
    SendAtCommand(dataConn, "AT+CCWA=1", HfpHfCommandProcessor::AT_CCWA_SETTER);
    SendAtCommand(dataConn, "AT+CMEE=1", HfpHfCommandProcessor::AT_CCME_SETTER);

    LOG_DEBUG("[HFP HF]%{public}s():Service level connection established!!!", __FUNCTION__);
}

void HfpHfCommandProcessor::ConnectSlc(HfpHfDataConnection &dataConn)
{
    if (dataConn.slcConnected_) {
        LOG_WARN("[HFP HF]%{public}s():Service level Connection already connected!!!", __FUNCTION__);
        return;
    }

    LOG_DEBUG("[HFP HF]%{public}s():currentCommand_[%{public}d]", __FUNCTION__, currentCommand_);
    switch (currentCommand_) {
        case AT_COMMAND_NONE:
            SendAtBrsfSetter(dataConn);
            break;
        case AT_BRSF_SETTER:
            ProcessSlcBrsf(dataConn);
            break;
        case AT_BAC_SETTER:
            SendAtCindTester(dataConn);
            break;
        case AT_CIND_TESTER:
            SendAtCindGetter(dataConn);
            break;
        case AT_CIND_GETTER:
            SendAtCmerSetter(dataConn);
            break;
        case AT_CMER_EXECUTER:
            ProcessSlcCmer(dataConn);
            break;
        case AT_CHLD_TESTER:
            ProcessSlcChld(dataConn);
            break;
        case AT_BIND_SETTER:
            SendAtBindTester(dataConn);
            break;
        case AT_BIND_TESTER:
            SendAtBindGetter(dataConn);
            break;
        case AT_BIND_GETTER:
            ProcessSlcConnected(dataConn);
            break;
        default:
            LOG_ERROR("[HFP HF]%{public}s():invalid commandId[%{public}d] in slc connection command", __FUNCTION__, currentCommand_);
            HfpHfProfileEventSender::GetInstance().UpdateConnectState(dataConn.remoteAddr_, HFP_HF_DISCONNECT_EVT);
            break;
    }
}

void HfpHfCommandProcessor::ProcessBcs(HfpHfDataConnection &dataConn, const std::string &arg)
{
    dataConn.remoteSelectedCodec_ = StoiTryCatch(arg);
    if (dataConn.remoteSelectedCodec_ & dataConn.g_localSupportCodecs) {
        dataConn.inUseCodec_ = dataConn.remoteSelectedCodec_;
        SendAtCommand(dataConn, "AT+BCS=" + arg, AT_BCS_SETTER);
    } else {
        /* Hfp profile 4.11.3
         The AG shall always respond with OK if the ID received in AT+BCS is the same as the one sent,
         and with ERROR otherwise. If no AT+BCS is received, but instead an AT+BAC is received after sending +BCS,
         the procedure shall end but may be restarted by the AG after re-selecting codec ID based on the information
         in the just received AT+BAC. */
        SendAtBacSetter(dataConn);
        LOG_ERROR("[HFP HF]%{public}s():Setup code connection failed, send BAC again!", __FUNCTION__);
    }
}

void HfpHfCommandProcessor::SendAtBrsfSetter(HfpHfDataConnection &dataConn)
{
    LOG_DEBUG("[HFP HF]%{public}s()", __FUNCTION__);
    std::string cmd = "AT+BRSF=" + std::to_string(dataConn.g_localFeatures);
    SendAtCommand(dataConn, cmd, AT_BRSF_SETTER);
}

void HfpHfCommandProcessor::SendAtBacSetter(HfpHfDataConnection &dataConn)
{
    LOG_DEBUG("[HFP HF]%{public}s()", __FUNCTION__);
    std::string cmd = "AT+BAC=";
    if (dataConn.g_localFeatures & HFP_HF_FEATURES_CODEC_NEGOTIATION) {
        cmd = cmd + "1,2";
    } else {
        cmd = cmd + "1";
    }
    SendAtCommand(dataConn, cmd, AT_BAC_SETTER);
}

void HfpHfCommandProcessor::SendAtCindTester(HfpHfDataConnection &dataConn)
{
    LOG_DEBUG("[HFP HF]%{public}s()", __FUNCTION__);
    std::string cmd = "AT+CIND=?";
    SendAtCommand(dataConn, cmd, AT_CIND_TESTER);
}

void HfpHfCommandProcessor::SendAtCindGetter(HfpHfDataConnection &dataConn)
{
    LOG_DEBUG("[HFP HF]%{public}s()", __FUNCTION__);
    std::string cmd = "AT+CIND?";
    SendAtCommand(dataConn, cmd, AT_CIND_GETTER);
}

void HfpHfCommandProcessor::SendAtCmerSetter(HfpHfDataConnection &dataConn)
{
    LOG_DEBUG("[HFP HF]%{public}s()", __FUNCTION__);
    std::string cmd = "AT+CMER=3,0,0,1";
    SendAtCommand(dataConn, cmd, AT_CMER_EXECUTER);
}

void HfpHfCommandProcessor::SendAtChldTester(HfpHfDataConnection &dataConn)
{
    LOG_DEBUG("[HFP HF]%{public}s()", __FUNCTION__);
    std::string cmd = "AT+CHLD=?";
    SendAtCommand(dataConn, cmd, AT_CHLD_TESTER);
}

void HfpHfCommandProcessor::SendAtBindSetter(HfpHfDataConnection &dataConn)
{
    LOG_DEBUG("[HFP HF]%{public}s()", __FUNCTION__);
    std::string cmd = "AT+BIND=" + HfpHfDataConnection::BIND_SETTINGS;
    SendAtCommand(dataConn, cmd, AT_BIND_SETTER);
}

void HfpHfCommandProcessor::SendAtBindGetter(HfpHfDataConnection &dataConn)
{
    LOG_DEBUG("[HFP HF]%{public}s()", __FUNCTION__);
    std::string cmd = "AT+BIND?";
    SendAtCommand(dataConn, cmd, AT_BIND_GETTER);
}

void HfpHfCommandProcessor::SendAtBiaSetter(HfpHfDataConnection &dataConn)
{
    LOG_DEBUG("[HFP HF]%{public}s()", __FUNCTION__);

    std::string cmd("AT+BIA=");
    auto size = dataConn.remoteAgIndicators_.size();
    for (size_t i = 0; i < size; i++) {
        auto it = std::find_if(dataConn.remoteAgIndicators_.begin(),
            dataConn.remoteAgIndicators_.end(),
            [&i](const HfpHfDataConnection::AgIndicator &indicator) {
                return i == indicator.index;
            });

        int value = 0;
        if (it != dataConn.remoteAgIndicators_.end()) {
            value = it->isEnabled ? 1 : 0;
        }

        if (i < size - 1) {
            cmd.append(std::to_string(value) + ",");
        } else {
            cmd.append(std::to_string(value));
        }
    }
    SendAtCommand(dataConn, cmd, HfpHfCommandProcessor::AT_BIA_SETTER);
}

void HfpHfCommandProcessor::SendAtBindTester(HfpHfDataConnection &dataConn)
{
    LOG_DEBUG("[HFP HF]%{public}s()", __FUNCTION__);
    std::string cmd = "AT+BIND=?";
    SendAtCommand(dataConn, cmd, AT_BIND_TESTER);
}

void HfpHfCommandProcessor::RespondTimeout()
{
    LOG_DEBUG("[HFP HF]%{public}s()", __FUNCTION__);
    if (hspState_ == HSP_HS_STATE_HSP) {
        LOG_INFO("[HFP HF] hsp only remove time out");
        return;
    }
    HfpHfProfileEventSender& instance = HfpHfProfileEventSender::GetInstance();
    instance.GetDispatchter()->PostTask(
        std::bind(&HfpHfProfileEventSender::UpdateConnectState, &instance, address_, HFP_HF_DISCONNECT_EVT));
}

void HfpHfCommandProcessor::SendAtCommand(
    HfpHfDataConnection &dataConn, const std::string &command, int commandId)
{
    std::string fullCommand(command + "\r");
    if (currentCommand_ == AT_COMMAND_NONE || !dataConn.slcConnected_ || (hspState_ == HSP_HS_STATE_HSP)) {
        LOG_DEBUG("[HFP HF]%{public}s(): commandId[%{public}d], command[%{public}s]", __FUNCTION__, commandId, command.c_str());
        currentCommand_ = commandId;
        std::size_t cmdLength = fullCommand.length();
        Packet *packet = PacketMalloc(0, 0, cmdLength);
        Buffer *buf = PacketContinuousPayload(packet);
        void *data = BufferPtr(buf);
        if (memcpy_s(data, cmdLength, fullCommand.c_str(), cmdLength) != EOK) {
            LOG_DEBUG("[HFP HF]%{public}s(): memcpy_s fail", __FUNCTION__);
            return;
        }
        dataConn.WriteData(*packet);
        PacketFree(packet);
        respTimer_->Start(RESPOND_TIMEOUT_MS, false);
    } else {
        commandQueue_.push(std::make_tuple(command, commandId));
    }
}

void HfpHfCommandProcessor::SendQueuedAtCommand(HfpHfDataConnection &dataConn)
{
    if (commandQueue_.size() == 0) {
        return;
    }

    auto command = commandQueue_.front();
    commandQueue_.pop();
    LOG_DEBUG(
        "[HFP HF]%{public}s() commandId[%{public}d], command[%{public}s]", __FUNCTION__, std::get<1>(command), std::get<0>(command).c_str());
    SendAtCommand(dataConn, std::get<0>(command), std::get<1>(command));
}

void HfpHfCommandProcessor::CleanUp()
{
    respTimer_->Stop();
    commandQueue_ = {};
    currentCommand_ = AT_COMMAND_NONE;
}
}  // namespace bluetooth
}  // namespace OHOS