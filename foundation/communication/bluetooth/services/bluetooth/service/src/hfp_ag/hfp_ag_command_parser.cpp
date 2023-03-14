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

#include <cstring>
#include <string>
#include "packet.h"
#include "hfp_ag_defines.h"
#include "hfp_ag_command_parser.h"


namespace OHOS {
namespace bluetooth {
HfpAgCommandParser &HfpAgCommandParser::GetInstance()
{
    static HfpAgCommandParser instance;
    return instance;
}

void HfpAgCommandParser::Read(HfpAgDataConnection &dataConn) const
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
        Parse(dataConn, data, len);
        PacketFree(pkt);
    }
}

void HfpAgCommandParser::Parse(HfpAgDataConnection &dataConn, std::vector<uint8_t> &data, size_t len) const
{
    std::string cmd;
    std::string arg;
    size_t cmdLen = 0;
    int cmdType = HFP_AG_CMD_UNKNOWN;

    if (data.empty() == true) {
        HILOGI("data is nullptr");
        return;
    }

    for (size_t pos = 0; pos < len; pos++) {
        // skip null characters
        if (data[pos] == '\0') {
            continue;
        }
        std::vector<uint8_t> partData(data.begin() + pos, data.end());
        cmdType = Extract(partData, cmd, arg, cmdLen, len - pos);
        pos += cmdLen;
        HfpAgCommandProcessor::GetInstance().Handle(dataConn, cmd, arg, cmdType);
    }
    return;
}

int HfpAgCommandParser::Extract(std::vector<uint8_t> &data, std::string &cmd, std::string &arg,
                                size_t &cmdLen, size_t len) const
{
    HfpAgCommandParser::CommandPosition position {0, 0, 0, 0, 0, false, false};
    GetCommandPosition(data, len, position);
    HILOGI("headValid: %{public}d, tailValid: %{public}d,"
        " startPos: %{public}hu, setPos: %{public}hu, getPos: %{public}hu, endPos: %{public}hu",
        position.headValid,
        position.tailValid,
        position.startPos,
        position.setPos,
        position.getPos,
        position.endPos);

    if (!position.headValid || !position.tailValid || position.endPos < HFP_AG_AT_HEAD_SIZE ||
        (position.setPos >= position.endPos) || (position.getPos >= position.endPos)) {
        cmdLen = position.pos;
        HILOGI("HFP_AG_CMD_INVALID");
        return HFP_AG_CMD_INVALID;
    }

    int type;
    cmdLen = position.endPos - position.startPos + 1;
    // cmd include cmd itself and cmd arguments
    cmd = std::string((char *)&data[position.startPos], cmdLen);
    if (cmd.compare(0, ATA_LENGTH, "ATA", 0, ATA_LENGTH) == 0) {
        type = HFP_AG_CMD_EXEC;
        cmd = "ATA";
    } else if (cmd.compare(0, ATD_LENGTH, "ATD", 0, ATD_LENGTH) == 0) {
        type = HFP_AG_CMD_EXEC;
        arg = std::string(cmd, ATD_LENGTH, cmd.length() - (ATD_LENGTH - 1));
        cmd = "ATD";
    } else if (!position.setPos && !position.getPos) {
        type = HFP_AG_CMD_EXEC;
        cmd = cmd.substr(0, cmd.length() - (AT_EXEC_OPERATOR_LENGTH + 1));
    } else if (!position.setPos && position.getPos) {
        type = HFP_AG_CMD_GET;
        cmd = cmd.substr(0, cmd.length() - (AT_GET_OPERATOR_LENGTH + 1));
    } else if (position.setPos && !position.getPos) {
        type = HFP_AG_CMD_SET;
        ExtractArg(cmd, arg);
    } else if (position.setPos == position.getPos - 1) {
        type = HFP_AG_CMD_TEST;
        cmd = cmd.substr(0, cmd.length() - (AT_TEST_OPERATOR_LENGTH + 1));
    } else {
        type = HFP_AG_CMD_UNKNOWN;
    }
    HILOGI(
        "cmd: %{public}s, arg: %{public}s, type: %{public}d, cmdLen: %{public}zu",
        cmd.c_str(), arg.c_str(), type, cmdLen);
    return type;
}

void HfpAgCommandParser::GetCommandPosition(const std::vector<uint8_t> &data,
                                            size_t len,
                                            HfpAgCommandParser::CommandPosition &position) const
{
    for (position.pos = 0; position.pos < len; position.pos++) {
        if (data[position.pos] == 'A' || data[position.pos] == 'a') {
            if ((position.pos + 1 < (int)len) &&
                ((data[position.pos + 1] == 'T') || (data[position.pos + 1] == 't'))) {
                position.startPos = position.pos;
                position.headValid = true;
            }
        }

        if (data[position.pos] == '=') {
            position.setPos = position.pos;
        }
        if (data[position.pos] == '?') {
            position.getPos = position.pos;
        }

        if (((data[position.pos] == '\r') || (data[position.pos] == '\n')) &&
            (position.pos > position.startPos) &&
            (position.headValid)) {
            position.tailValid = true;
            position.endPos = position.pos;
            break;
        }
    }
}

void HfpAgCommandParser::ExtractArg(std::string &cmd, std::string &arg) const
{
    for (auto it : g_atCmdMap) {
        if (cmd.find(it.first) != std::string::npos) {
            // -2 because '\r' and '=' occupied two chars
            arg = std::string(cmd, it.first.length() + 1,
                              cmd.length() - it.first.length() - (AT_SET_OPERATOR_LENGTH + 1));
            cmd = it.first;
            break;
        }
    }
    HILOGI("cmd: %{public}s arg: %{public}s", cmd.c_str(), arg.c_str());
}
}  // namespace bluetooth
}  // namespace OHOS