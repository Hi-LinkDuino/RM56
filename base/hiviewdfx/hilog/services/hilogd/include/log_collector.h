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

#ifndef LOG_COLLECTOR_H
#define LOG_COLLECTOR_H
#include <list>

#include "log_buffer.h"
#include "hilog_input_socket_server.h"

namespace OHOS {
namespace HiviewDFX {
class LogCollector {
public:
    explicit LogCollector(HilogBuffer& buffer) : m_hilogBuffer(buffer) {}
    void InsertDropInfo(const HilogMsg &msg, int droppedCount);
    size_t InsertLogToBuffer(const HilogMsg& msg);
#ifndef __RECV_MSG_WITH_UCRED_
    void onDataRecv(std::vector<char>& data);
#else
    void onDataRecv(const ucred& cred, std::vector<char>& data);
#endif
    ~LogCollector() = default;
private:
    HilogBuffer& m_hilogBuffer;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif
