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

#include "log_collector.h"
#include "log_kmsg.h"
#include "flow_control_init.h"

#include <cstdlib>
#include <cstring>
#include <cinttypes>
#include <iostream>
#include <string>
#include <thread>
#include <string_view>

namespace OHOS {
namespace HiviewDFX {
using namespace std;

void LogCollector::InsertDropInfo(const HilogMsg &msg, int droppedCount)
{
    string dropLog = to_string(droppedCount) + " line(s) dropped!";
    constexpr auto tag = "LOGLIMITD"sv;
    std::vector<char> buffer(sizeof(HilogMsg) + tag.size() + dropLog.size() + 1, '\0');
    HilogMsg *dropMsg = reinterpret_cast<HilogMsg *>(buffer.data());
    if (dropMsg != nullptr) {
        dropMsg->len     = buffer.size();
        dropMsg->version = msg.version;
        dropMsg->type    = msg.type;
        dropMsg->level   = msg.level;
        dropMsg->tag_len = tag.size();
        dropMsg->tv_sec  = msg.tv_sec;
        dropMsg->tv_nsec = msg.tv_nsec;
        dropMsg->pid     = msg.pid;
        dropMsg->tid     = msg.tid;
        dropMsg->domain  = msg.domain;

        auto remainSize = buffer.size() - sizeof(HilogMsg);
        if (memcpy_s(dropMsg->tag, remainSize, tag.data(), tag.size())) {
            std::cerr << "Can't copy tag info of Drop Info message\n";
        }

        remainSize -= tag.size();
        auto logTextPtr = dropMsg->tag + tag.size(); // log text is behind tag data
        if (memcpy_s(logTextPtr, remainSize, dropLog.data(), dropLog.size() + 1)) {
            std::cerr << "Can't copy log text info of Drop Info message\n";
        }

        InsertLogToBuffer(*dropMsg);
    }
}
#ifndef __RECV_MSG_WITH_UCRED_
void LogCollector::onDataRecv(std::vector<char>& data)
#else
void LogCollector::onDataRecv(const ucred& cred, std::vector<char>& data)
#endif
{
    if (data.size() < sizeof(HilogMsg)) {
        std::cerr << "Internal error - received data less than HilogMsg size";
        return;
    }

    HilogMsg *msg = reinterpret_cast<HilogMsg *>(data.data());
#ifdef __RECV_MSG_WITH_UCRED_
    msg->pid = cred.pid;
#endif
    // Domain flow control
    int ret = FlowCtrlDomain(msg);
    if (ret < 0) {
        // dropping message
        return;
    } else if (ret > 0) { /* if >0 !Need  print how many lines was dopped */
        // store info how many was dropped
        InsertDropInfo(*msg, ret);
    }
    InsertLogToBuffer(*msg);
}

size_t LogCollector::InsertLogToBuffer(const HilogMsg& msg)
{
    if (msg.type >= LOG_TYPE_MAX) {
        return ERR_LOG_TYPE_INVALID;
    }
    return m_hilogBuffer.Insert(msg);
}
} // namespace HiviewDFX
} // namespace OHOS
