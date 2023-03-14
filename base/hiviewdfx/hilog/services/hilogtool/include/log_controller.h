/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef LOG_CONTROLLER_H
#define LOG_CONTROLLER_H

#include <string>
#include <vector>

#include <hilog/log.h>
#include "hilogtool_msg.h"
#include "seq_packet_socket_client.h"
#include "hilogtool.h"

namespace OHOS {
namespace HiviewDFX {
constexpr int RECV_BUF_LEN = MAX_LOG_LEN * 2;

void SetMsgHead(MessageHeader* msgHeader, const uint8_t msgCmd, const uint16_t msgLen);
int MultiQuerySplit(const std::string& src, const char& delim, std::vector<std::string>& vec);
void NextRequestOp(SeqPacketSocketClient& controller, uint16_t sendId);
void LogQueryRequestOp(SeqPacketSocketClient& controller, const HilogArgs* context);
void LogQueryResponseOp(SeqPacketSocketClient& controller, char* recvBuffer, uint32_t bufLen,
    const HilogArgs* context, uint32_t format);
int32_t BufferSizeOp(SeqPacketSocketClient& controller, uint8_t msgCmd,
    const std::string& logTypeStr, const std::string& buffSizeStr);
int32_t StatisticInfoOp(SeqPacketSocketClient& controller, uint8_t msgCmd,
    const std::string& logTypeStr, const std::string& domainStr);
int32_t LogClearOp(SeqPacketSocketClient& controller, uint8_t msgCmd, const std::string& logTypeStr);
int32_t LogPersistOp(SeqPacketSocketClient& controller, uint8_t msgCmd, LogPersistParam* logPersistParam);
int32_t SetPropertiesOp(SeqPacketSocketClient& controller, uint8_t operationType, SetPropertyParam* propertyParm);
} // namespace HiviewDFX
} // namespace OHOS
#endif
