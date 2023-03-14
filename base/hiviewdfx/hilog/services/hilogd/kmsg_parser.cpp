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

#include "kmsg_parser.h"
#include "hilog/log.h"

#include <cstdlib>
#include <cinttypes>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <regex>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string_view>

namespace OHOS {
namespace HiviewDFX {
using namespace std::chrono;
using namespace std::literals;

constexpr int  DEC = 10;

// Avoid name collision between sys/syslog.h and our log_c.h
#undef LOG_FATAL
#undef LOG_ERR
#undef LOG_WARN
#undef LOG_INFO
#undef LOG_DEBUG

using Priority = enum {
    PV0 = 0,
    PV1,
    PV2,
    PV3,
    PV4,
    PV5,
    PV6
};

static void ParseHeader(std::string& str, uint16_t* level, uint64_t* timestamp)
{
    static const std::string pattern = "(\\d+),(\\d+),(\\d+),(\\S);";
    static const std::regex express(pattern);
    std::match_results<std::string::iterator> res;
    if (std::regex_search(str.begin(), str.end(), res, express)) {
        *level = strtoul(res[1].str().c_str(), nullptr, DEC);
        *timestamp = strtoumax(res[3].str().c_str(), nullptr, DEC);
        str.erase(res.position(), res.length());
    }
}

// Parse pid if exists
static uint32_t ParsePid(std::string& str)
{
    static const std::string pattern = "\\[pid=(\\d+)\\]";
    static const std::regex express(pattern);
    std::match_results<std::string::iterator> res;
    if (std::regex_search(str.begin(), str.end(), res, express)) {
        uint32_t ret = strtoumax(res[1].str().c_str(), nullptr, DEC);
        str.erase(res.position(), res.length());
        return ret;
    }
    return 0;
}

static std::string ParseTag(std::string& str)
{
    static const std::string pattern = "\\[.*?\\]";
    static const std::regex express(pattern);
    std::match_results<std::string::iterator> res;
    if (std::regex_search(str.begin(), str.end(), res, express)) {
        std::string ret = res[0].str();
        str.erase(res.position(), res.length());
        return ret;
    }
    return {};
}

// Log levels are different in syslog.h and hilog log_c.h
static uint16_t KmsgLevelMap(uint16_t prio)
{
    uint16_t level;
    switch (prio) {
        case Priority::PV0:
        case Priority::PV1:
        case Priority::PV2:
            level = LOG_FATAL;
            break;
        case Priority::PV3:
            level = LOG_ERROR;
            break;
        case Priority::PV4:
        case Priority::PV5:
            level = LOG_WARN;
            break;
        case Priority::PV6:
            level = LOG_INFO;
            break;
        default:
            level = LOG_DEBUG;
            break;
    }
    return level;
}

static constexpr timespec TimepointToTimespec(time_point<system_clock, nanoseconds> tp)
{
    auto secs = time_point_cast<seconds>(tp);
    auto nsecs = time_point_cast<nanoseconds>(tp) - time_point_cast<nanoseconds>(secs);
    return timespec{secs.time_since_epoch().count(), nsecs.count()};
}

// Kmsg has microseconds from system boot. Now get the time of system boot.
KmsgParser::BootTp KmsgParser::BootTime()
{
    struct timespec t_uptime;
    clock_gettime(CLOCK_BOOTTIME, &t_uptime);
    auto uptime = seconds{t_uptime.tv_sec} + nanoseconds{t_uptime.tv_nsec};
    auto current = system_clock::now();
    auto boottime = current - uptime;
    return boottime;
}
std::optional<HilogMsgWrapper> KmsgParser::ParseKmsg(const std::vector<char>& kmsgBuffer)
{
    std::string kmsgStr(kmsgBuffer.data());
    std::vector<char> mtag(MAX_TAG_LEN, '\0');
    uint16_t mLevel = 0;
    uint64_t timestamp = 0;
    ParseHeader(kmsgStr, &mLevel, &timestamp);
    // Parses pid if exists. Pid in kmsg content is like: [pid=xxx,...]
    uint32_t mpid = ParsePid(kmsgStr);
    // If there are some other content wrapped in square brackets "[]", parse it as tag
    // Otherwise, use default tag  "kmsg"
    size_t tagLen = 0;
    std::string tagStr = ParseTag(kmsgStr);
    if (!tagStr.empty()) {
        tagLen = tagStr.size();
        if (strncpy_s(mtag.data(), MAX_TAG_LEN - 1, tagStr.c_str(), tagStr.size()) != 0) {
            return {};
        }
    } else {
        constexpr auto defaultTag = "kmsg"sv;
        tagLen = defaultTag.size();
        if (strncpy_s(mtag.data(), MAX_TAG_LEN - 1, defaultTag.data(), defaultTag.size()) != 0) {
            return {};
        }
    }
    // Now build HilogMsg and insert it into buffer
    auto len = kmsgStr.size() + 1;
    auto msgLen = sizeof(HilogMsg) + tagLen + len + 1;
    HilogMsgWrapper msgWrap((std::vector<char>(msgLen, '\0')));
    HilogMsg& msg = msgWrap.GetHilogMsg();
    msg.len = msgLen;
    msg.tag_len = tagLen + 1;
    msg.type = LOG_KMSG;
    msg.domain = 0xdfffffff;
    msg.level = KmsgLevelMap(mLevel);
    time_point<system_clock, nanoseconds> logtime = BootTime() + microseconds{timestamp};
    struct timespec logts = TimepointToTimespec(logtime);
    msg.tv_sec = static_cast<uint32_t>(logts.tv_sec);
    msg.tv_nsec = static_cast<uint32_t>(logts.tv_nsec);
    msg.pid = mpid;
    msg.tid = mpid;
    if (strncpy_s(msg.tag, tagLen + 1, mtag.data(), tagLen) != 0) {
        return {};
    }
    if (strncpy_s(CONTENT_PTR((&msg)), MAX_LOG_LEN, kmsgStr.c_str(), len) != 0) {
        return {};
    }
    return msgWrap;
}
} // namespace HiviewDFX
} // namespace OHOS
