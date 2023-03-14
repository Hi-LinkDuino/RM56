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

#ifndef HFP_HF_COMMAND_PROCESSOR_H
#define HFP_HF_COMMAND_PROCESSOR_H

#include <cstdint>
#include <queue>
#include <string>
#include <tuple>
#include <unordered_map>

#include "hfp_hf_data_connection.h"
#include "timer.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for process AT command.
 */
class HfpHfCommandProcessor {
public:
    enum {
        AT_COMMAND_NONE,
        AT_BRSF_SETTER,
        AT_BAC_SETTER,
        AT_CIND_GETTER,
        AT_CIND_TESTER,
        AT_CMER_EXECUTER,
        AT_CHLD_GETTER,
        AT_CHLD_TESTER,
        AT_BIND_SETTER,
        AT_BIND_GETTER,
        AT_BIND_TESTER,
        AT_CLCC_EXECUTER,
        AT_COPS_GETTER,
        AT_COPS_SETTER,
        AT_CNUM_EXECUTER,
        AT_VTS_SETTER,
        AT_VGS_SETTER,
        AT_VGM_SETTER,
        AT_BVRA_1_SETTER,
        AT_BVRA_0_SETTER,
        AT_ATD_EXECUTER,
        AT_ATA_EXECUTER,
        AT_CHLD_SETTER,
        AT_BTRH_GETTER,
        AT_BTRH_SETTER,
        AT_CHUP_EXECUTER,
        AT_BCC_EXECUTER,
        AT_BINP_SETTER,
        AT_BLDN_EXECUTER,
        AT_NREC_SETTER,
        AT_CMEE_SETTER,
        AT_BIA_SETTER,
        AT_CLIP_SETTER,
        AT_CCWA_SETTER,
        AT_CCME_SETTER,
        AT_BCS_SETTER,
        AT_BIEV_SETTER,
        AT_CKPD_SETTER
    };

    /**
     * @brief Define HF AT command handler functions.
     */
    typedef void (HfpHfCommandProcessor::*AtCmdFn)(
        HfpHfDataConnection &dataConn, const std::string &arg);

    /**
     * @brief Struct for define HF AT command handler.
     */
    struct HfpHfAtHandler {
        HfpHfCommandProcessor::AtCmdFn fn;
    };

    /**
     * @brief Get the AT command map.
     *
     * @return Returns the AT command map.
     */
    static std::unordered_map<std::string, HfpHfCommandProcessor::HfpHfAtHandler> &GetAtCmdMap();

    /**
     * @brief Construct a new HfpHfCommandProcessor object.
     */
    HfpHfCommandProcessor() = default;

    /**
     * @brief Destroy the HfpHfCommandProcessor object.
     */
    ~HfpHfCommandProcessor() = default;

    /**
     * @brief Init command processor.
     *
     * @param address Remote device address.
     */
    void Init(const std::string &address);

    /**
     * @brief Connect service level Connection in profile level.
     *
     * @param dataConn Data connection.
     */
    void ConnectSlc(HfpHfDataConnection &dataConn);

    /**
     * @brief Send At command.
     *
     * @param dataConn Data connection.
     * @param command command string.
     * @param commandId command id.
     */
    void SendAtCommand(HfpHfDataConnection &dataConn, const std::string &command, int commandId);

    /**
     * @brief Handle specific AT command.
     *
     * @param dataConn Data connection.
     * @param cmd AT command.
     * @param arg AT command argument.
     */
    void ProcessCommand(HfpHfDataConnection &dataConn, const std::string &cmd, const std::string &arg);

    /**
     * @brief Clear up after disconnection.
     */
    void CleanUp();

private:
    // Remote device address
    std::string address_ {""};

    // Respond timer object
    std::unique_ptr<utility::Timer> respTimer_ {nullptr};

    int hspState_ = 1;

    static inline constexpr int HF_INDICATOR_MAX = 20;
    static inline constexpr int INDICATOR_NAME_MAX_LEN = 16;
    static inline constexpr int OPERATOR_NAME_MAX_LEN = 16;
    static inline constexpr int PHONE_NUMBER_MAX_LEN = 32;

    // Respond Timeout
    static inline constexpr int RESPOND_TIMEOUT_MS = 30000;
    int currentCommand_ {AT_COMMAND_NONE};
    std::queue<std::tuple<std::string, int>> commandQueue_ {};
    inline static const std::string COMMAND_NONE = "";
    inline static const std::string COMMAND_TYPE_EXECUTER = "";
    inline static const std::string COMMAND_TYPE_GETTER = "?";
    inline static const std::string COMMAND_TYPE_SETTER = "=";
    inline static const std::string COMMAND_TYPE_TESTER = "=?";

    // 0 Releases all held calls or sets User Determined User Busy (UDUB) for a waiting call.
    static inline constexpr int CHLD_RELEASE_ALL = 0x00000001;
    // 1 Releases all active calls (if any exist) and accepts the other (held or waiting) call.
    static inline constexpr int CHLD_RELEASE_ALL_AND_ACCPET =
        0x00000002;  // 1x Releases specified active call only (<idx>).
    static inline constexpr int CHLD_RELEASE_SPECIFIC = 0x00000004;
    // 2 Places all active calls (if any exist) on hold and accepts the other (held or waiting) call.
    static inline constexpr int CHLD_HOLD_ALL_AND_ACCPET = 0x00000008;
    // 2x Request private consultation mode with specified call (<idx>). (Place all calls on hold EXCEPT the call
    // indicated by <idx>.)
    static inline constexpr int CHLD_HOLD_ALL_AND_PRIVATE = 0x00000010;
    // 3 Adds a held call to the conversation.
    static inline constexpr int CHLD_MERGE = 0x00000020;
    // 4  Connects the two calls and disconnects the subscriber from both calls (Explicit Call Transfer).
    static inline constexpr int CHLD_EXPLICT_CALL_TRANSFER = 0x00000040;

    static inline constexpr int INVALID_CME_ERROR_CODE = 0xFFFFFFFF;

    inline static constexpr int COPS_ARGS_NUMBER = 2;
    inline static constexpr int CIEV_ARGS_NUMBER = 2;
    inline static constexpr int CCWA_ARGS_NUMBER = 2;
    inline static constexpr int CNUM_ARGS_NUMBER = 3;
    inline static constexpr int CLCC_ARGS_NUMBER = 5;
    inline static constexpr int CHLD_ARGS_MIN_LENGTH = 3;
    inline static constexpr int CHLD_TEST_ARGS_NUMBER = 3;
    inline static constexpr int CHLD_MAIN_ARGS_NUMBER = 5;
    inline static constexpr int CHLD_SUB_ARGS_NUMBER = 2;
    inline static constexpr int BIND_SET_ARGS_NUMBER = 2;

    static std::unordered_map<std::string, HfpHfCommandProcessor::HfpHfAtHandler> g_atCmdMap;
    static int StoiTryCatch(const std::string &arg);
    void RespondTimeout();
    void SendQueuedAtCommand(HfpHfDataConnection &dataConn);
    void ProcessOK(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessErrorCode(HfpHfDataConnection &dataConn, int errorCode);
    void ProcessErrorCmd(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessBusy(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessDelayed(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessNoCarrier(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessNoAnswer(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessBlocklisted(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessCmeError(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessCops(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessBtrh(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessRing(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessClip(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessBrsf(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessCind(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessCindGetter(HfpHfDataConnection &dataConn, std::string arg);
    void ProcessCindTester(HfpHfDataConnection &dataConn, std::string arg);
    void ProcessChld(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessBind(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessListHfIndicators(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessChangeIndicatorState(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessCiev(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessClcc(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessBsir(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessCcwa(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessBcs(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessBvra(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessCnum(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessVgm(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessVgs(HfpHfDataConnection &dataConn, const std::string &arg);
    void ProcessIndicator(
        HfpHfDataConnection &dataConn, HfpHfDataConnection::AgIndicator &it, uint32_t value);
    void ProcessSlcBrsf(HfpHfDataConnection &dataConn);
    void ProcessSlcCmer(HfpHfDataConnection &dataConn);
    void ProcessSlcChld(HfpHfDataConnection &dataConn);
    void ProcessSlcConnected(HfpHfDataConnection &dataConn);
    void SendAtBrsfSetter(HfpHfDataConnection &dataConn);
    void SendAtBacSetter(HfpHfDataConnection &dataConn);
    void SendAtCindTester(HfpHfDataConnection &dataConn);
    void SendAtCindGetter(HfpHfDataConnection &dataConn);
    void SendAtCmerSetter(HfpHfDataConnection &dataConn);
    void SendAtChldTester(HfpHfDataConnection &dataConn);
    void SendAtBindGetter(HfpHfDataConnection &dataConn);
    void SendAtBindSetter(HfpHfDataConnection &dataConn);
    void SendAtBindTester(HfpHfDataConnection &dataConn);
    void SendAtBiaSetter(HfpHfDataConnection &dataConn);
    BT_DISALLOW_COPY_AND_ASSIGN(HfpHfCommandProcessor);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_HF_COMMAND_PROCESSOR_H