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
#ifndef HFP_HF_COMMAND_PARSER_H
#define HFP_HF_COMMAND_PARSER_H

#include <cstdint>
#include <string>
#include <unordered_map>

#include "base_def.h"
#include "hfp_hf_command_processor.h"
#include "hfp_hf_data_connection.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for parse AT command from remote AG device.
 */
class HfpHfCommandParser {
public:
    /**
     * @brief Get the HfpHfCommandParser instance.
     *
     * @return Returns HfpHfCommandParser instance.
     */
    static HfpHfCommandParser &GetInstance();

    /**
     * @brief Read data from data link.
     *
     * @param dataConn Data connection.
     * @param commandProcessor Command processor pointer.
     */
    void Read(HfpHfDataConnection &dataConn, HfpHfCommandProcessor &commandProcessor);

    /**
     * @brief Parse data bufffer.
     *
     * @param dataConn Data connection.
     * @param commandProcessor Command processor pointer.
     * @param data Data buffer pointer.
     * @param len Data buffer length.
     */
    void Parse(HfpHfDataConnection &dataConn, HfpHfCommandProcessor &commandProcessor,
        std::vector<uint8_t> &data, size_t len);

private:
    inline static constexpr int AT_HEAD_SIZE = 2;
    void ParseBody(HfpHfDataConnection &dataConn,
        HfpHfCommandProcessor &commandProcessor, std::vector<uint8_t> &data, size_t &pos, const size_t len);
    HfpHfCommandParser() = default;
    ~HfpHfCommandParser() = default;
    BT_DISALLOW_COPY_AND_ASSIGN(HfpHfCommandParser);

    std::unordered_map<std::string, HfpHfCommandProcessor::HfpHfAtHandler> &g_atCmdMap {
        HfpHfCommandProcessor::GetAtCmdMap()
    };
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_HF_COMMAND_PARSER_H