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

#ifndef HFP_AG_COMMAND_PROCESSOR_H
#define HFP_AG_COMMAND_PROCESSOR_H

#include <string>
#include <unordered_map>

#include "hfp_ag_data_connection.h"

namespace OHOS {
namespace bluetooth {
typedef enum {
    HFP_AG_CMD_INVALID = 0,
    HFP_AG_CMD_GET,
    HFP_AG_CMD_SET,
    HFP_AG_CMD_TEST,
    HFP_AG_CMD_EXEC,
    HFP_AG_CMD_UNKNOWN
} HfpAgAtType;

/**
 * @brief Class for process AT command from remote HF device or local AG service.
 */
class HfpAgCommandProcessor {
public:
    /**
     * @brief Define AG AT command handler functions.
     */
    typedef void (HfpAgCommandProcessor::*AtCmdFn)(
        HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief Struct for define AG AT command handler.
     */
    struct HfpAgAtHandler {
        HfpAgCommandProcessor::AtCmdFn getter;
        HfpAgCommandProcessor::AtCmdFn setter;
        HfpAgCommandProcessor::AtCmdFn tester;
        HfpAgCommandProcessor::AtCmdFn executer;
    };

    /**
     * @brief Get the HfpAgCommandProcessor instance.
     *
     * @return Returns the HfpAgCommandProcessor instance.
     */
    static HfpAgCommandProcessor &GetInstance();

    /**
     * @brief Get the AT command map.
     *
     * @return Returns the AT command map.
     */
    std::unordered_map<std::string, HfpAgCommandProcessor::HfpAgAtHandler> &GetAtCmdMap()
    {
        return g_atCmdMap;
    }

    /**
     * @brief Send Error command.
     *
     * @param dataConn Data connection.
     * @param errorCode Error code.
     */
    static void SendErrorCode(const HfpAgDataConnection &dataConn, int errorCode);

    /**
     * @brief Send AT command.
     *
     * @param dataConn Data connection.
     * @param command AT command.
     */
    static void SendAtCommand(const HfpAgDataConnection &dataConn, const std::string &command);

    /**
     * @brief Empty function.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void AtEmptyFn(HfpAgDataConnection &dataConn, const std::string &arg);
    /**
     * @brief ATA Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void AtaExecuter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief ATA Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void AtdExecuter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief VGS Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void VgsSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief VGM Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void VgmSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief CCWA Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void CcwaSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief CHLD Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void ChldSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief CHLD Tester.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void ChldTester(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief CHUP Executer.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void ChupExecuter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief CIND Getter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void CindGetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief CIND Tester.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void CindTester(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief CLIP Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void ClipSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief CMER Executer.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void CmerSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief VTS Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void VtsSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief BLDN Executer.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void BldnExecuter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief BVRA Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void BvraSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief BRSF Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void BrsfSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief NREC Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void NrecSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief CNUM Executer.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void CnumExecuter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief CLCC Executer.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void ClccExecuter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief COPS Getter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void CopsGetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief COPS Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void CopsSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief CMEE Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void CmeeSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief BIA Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void BiaSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief BCC Executer.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void BccExecuter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief BCS Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void BcsSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief BIND Getter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void BindGetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief BIND Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void BindSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief BIND tester.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void BindTester(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief BIEV Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void BievSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief BAC Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void BacSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief BTRH Getter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void BtrhGetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief BTRH Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void BtrhSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief CKPD Setter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void CkpdSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief BINP Getter.
     *
     * @param dataConn Data connection.
     * @param arg AT command argument
     */
    void BinpSetter(HfpAgDataConnection &dataConn, const std::string &arg);

    /**
     * @brief Handle specific AT command.
     *
     * @param dataConn Data connection.
     * @param cmd AT command.
     * @param arg AT command argument.
     * @param cmdType AT command type.
     */
    void Handle(
        HfpAgDataConnection &dataConn, const std::string &cmd, const std::string &arg, int cmdType);

private:
    HfpAgCommandProcessor() = default;
    ~HfpAgCommandProcessor() = default;
    static int StoiTryCatch(HfpAgDataConnection &dataConn, const std::string &arg);
    BT_DISALLOW_COPY_AND_ASSIGN(HfpAgCommandProcessor);

    static std::unordered_map<std::string, HfpAgCommandProcessor::HfpAgAtHandler> g_atCmdMap;
    static inline const std::string HEAD = "\r\n";
    static inline const std::string TAIL = "\r\n";
    static inline const std::string OK = "OK";
    static inline const std::string ERROR = "ERROR";
    inline static constexpr int ATD_VALID_LENGTH = 2;
    inline static constexpr int CHLD_RELEASE_ALL_HELD_CALLS = 0;
    inline static constexpr int CHLD_RELEASE_ACTIVE_ACCPET_OTHER = 1;
    inline static constexpr int CHLD_RELEASE_HOLD_ACCPET_OTHER = 2;
    inline static constexpr int CHLD_ADD_CALL_TO_CONVERSATION = 3;
    inline static constexpr int CHLD_CONNECT_TWO_CALL = 4;
    inline static constexpr int CHLD_RELEASE_INDEX_ONE = 11;
    inline static constexpr int CHLD_RELEASE_INDEX_TWO = 12;
    inline static constexpr int CHLD_CONSULTATION_INDEX_ONE = 21;
    inline static constexpr int CHLD_CONSULTATION_INDEX_TWO = 22;
    inline static constexpr int BIEV_ARGS_VALID_LENGTH = 2;
    inline static constexpr int CMER_ELEMENTS_NUMBER = 4;
    inline static constexpr unsigned int CMER_MODE_INDEX = 0;
    inline static constexpr unsigned int CMER_KEYP_INDEX = 1;
    inline static constexpr unsigned int CMER_DISP_INDEX = 2;
    inline static constexpr unsigned int CMER_IND_INDEX = 3;
    inline static constexpr int CMER_MODE_FOR_HFP = 3;

    // Max Number of supported AG indicators
    static inline constexpr int MAX_AG_INDICATORS = 20;

    // Number of supported HF indicators
    // 1 for Enhanced Safety Status, 2 for Battery Level Status
    static inline constexpr int LOCAL_HF_IND_NUM = 2;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_AG_COMMAND_PROCESSOR_H
