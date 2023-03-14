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

#include "hfp_hf_command_parser.h"

#include <cstring>

#include "hfp_hf_defines.h"
#include "packet.h"

namespace OHOS {
namespace bluetooth {
HfpHfCommandParser &HfpHfCommandParser::GetInstance()
{
    static HfpHfCommandParser instance;
    return instance;
}

void HfpHfCommandParser::Read(
    HfpHfDataConnection &dataConn, HfpHfCommandProcessor &commandProcessor)
{
    Packet *pkt = nullptr;
    Buffer *buf = nullptr;
    uint8_t *pData = nullptr;

    dataConn.ReadData(&pkt);
    if (pkt != nullptr) {
        auto len = PacketPayloadSize(pkt);
        buf = PacketContinuousPayload(pkt);
        pData = (uint8_t *)BufferPtr(buf);

        std::vector<uint8_t> data(pData, pData + len);
        Parse(dataConn, commandProcessor, data, len);
        PacketFree(pkt);
    }
}

void HfpHfCommandParser::ParseBody(HfpHfDataConnection &dataConn,
    HfpHfCommandProcessor &commandProcessor, std::vector<uint8_t> &data, size_t &pos, const size_t len)
{
    for (auto &it : g_atCmdMap) {
        if (!strncmp(it.first.c_str(), (char *)(&data[pos]), it.first.length())) {
            // command body matched
            size_t bodyPos = pos;
            while (!(data[pos] == '\r' && data[pos + 1] == '\n')) {
                pos++;
            }

            if (pos + 1 < len) {
                // command tail "\r\n" matched
                size_t tailPos = pos;
                pos++;
                std::string arg {(char *)&data[bodyPos + it.first.length()], tailPos - it.first.length() - bodyPos};
                LOG_DEBUG("[HFP HF]%{public}s():cmd[%{public}s], arg[%{public}s]", __FUNCTION__, it.first.c_str(), arg.c_str());
                commandProcessor.ProcessCommand(dataConn, it.first, arg);
                break;
            } else {
                LOG_DEBUG("[HFP HF]%{public}s():Command format invalid!", __FUNCTION__);
                break;
            }
        }
    }
}

void HfpHfCommandParser::Parse(HfpHfDataConnection &dataConn,
    HfpHfCommandProcessor &commandProcessor, std::vector<uint8_t> &data, size_t len)
{
    if (data.empty() == true) {
        LOG_DEBUG("[HFP HF]%{public}s():data is nullptr", __FUNCTION__);
        return;
    }

    for (size_t pos = 0; pos < len; pos++) {
        // skip null characters
        if (data[pos] == '\0') {
            continue;
        }

        if (pos + 1 < len && (data[pos] == '\r' && data[pos + 1] == '\n')) {
            // command head "\r\n" matched, skip it
            pos += AT_HEAD_SIZE;
            ParseBody(dataConn, commandProcessor, data, pos, len);
        }
    }
    return;
}
}  // namespace bluetooth
}  // namespace OHOS