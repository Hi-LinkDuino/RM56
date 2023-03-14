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

#ifndef HFP_AG_COMMAND_PARSER_H
#define HFP_AG_COMMAND_PARSER_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "base_def.h"
#include "hfp_ag_command_processor.h"
#include "hfp_ag_data_connection.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for parse AT command from remote HF device.
 */
class HfpAgCommandParser {
public:
    /**
     * @brief Get the HfpAgCommandParser instance.
     *
     * @return Returns HfpAgCommandParser instance.
     */
    static HfpAgCommandParser &GetInstance();

    /**
     * @brief Struct for store command position.
     */
    struct CommandPosition {
        uint16_t pos;
        uint16_t endPos;
        uint16_t setPos;
        uint16_t getPos;
        uint16_t startPos;
        bool headValid;
        bool tailValid;
    };

    /**
     * @brief Read data from data link.
     *
     * @param dataConn Data connection.
     */
    void Read(HfpAgDataConnection &dataConn) const;

    /**
     * @brief Parse data bufffer.
     *
     * @param dataConn Data connection.
     * @param data Data buffer pointer.
     * @param len Data buffer length.
     */
    void Parse(HfpAgDataConnection &dataConn, std::vector<uint8_t> &data, size_t len) const;

    /**
     * @brief Extract At command from data buffer.
     *
     * @param data Data buffer pointer.
     * @param cmd AT command.
     * @param arg AT command argument.
     * @param cmdLen AT command length include command & argument.
     * @param len Data buffer length.
     * @return Returns error code of the result.
     */
    int Extract(std::vector<uint8_t> &data, std::string &cmd, std::string &arg, size_t &cmdLen, size_t len) const;

    /**
     * @brief Extract command argument for AT Setter command.
     *
     * @param cmd AT command.
     * @param arg AT command argument.
     */
    void ExtractArg(std::string &cmd, std::string &arg) const;

    /**
     * @brief Get the Command Position object.
     *
     * @param data Data buffer pointer.
     * @param len Data buffer length.
     * @param pos Data buffer position.
     */
    void GetCommandPosition(const std::vector<uint8_t> &data, size_t len,
                            HfpAgCommandParser::CommandPosition &pos) const;

private:
    HfpAgCommandParser() = default;
    ~HfpAgCommandParser() = default;
    BT_DISALLOW_COPY_AND_ASSIGN(HfpAgCommandParser);

    inline static constexpr int HFP_AG_AT_HEAD_SIZE = 2;
    inline static constexpr int ATA_LENGTH = 3;
    inline static constexpr int ATD_LENGTH = 3;
    inline static constexpr int AT_EXEC_OPERATOR_LENGTH = 0;    // operator ""
    inline static constexpr int AT_GET_OPERATOR_LENGTH = 1;     // operator "?"
    inline static constexpr int AT_SET_OPERATOR_LENGTH = 1;     // operator "="
    inline static constexpr int AT_TEST_OPERATOR_LENGTH = 2;    // operator "=?"

    std::unordered_map<std::string, HfpAgCommandProcessor::HfpAgAtHandler> &g_atCmdMap {
        HfpAgCommandProcessor::GetInstance().GetAtCmdMap()
    };
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_AG_COMMAND_PARSER_H