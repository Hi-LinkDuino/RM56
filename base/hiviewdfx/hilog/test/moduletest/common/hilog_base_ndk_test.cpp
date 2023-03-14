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

#include <array>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>
#include <hilog_common.h>

#include "hilog_base/log_base.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD002D00

#undef LOG_TAG
#define LOG_TAG "HILOGBASETEST"

using namespace testing::ext;

namespace OHOS {
namespace HiviewDFX {
namespace HiLogBaseTest {
static constexpr unsigned int SOME_LOGS = 10;
static constexpr unsigned int MORE_LOGS = 100;
static constexpr unsigned int SHORT_LOG = 16;
static constexpr unsigned int LONG_LOG = 1000;
static constexpr unsigned int VERY_LONG_LOG = 2048;

enum LogInterfaceType {
    DEBUG_METHOD = 0,
    INFO_METHOD = 1,
    WARN_METHOD = 2,
    ERROR_METHOD = 3,
    FATAL_METHOD = 4,
    METHODS_NUMBER = 5,
};

using LogMethodFunc = std::function<void(const std::string &msg)>;

static const std::array<std::string, METHODS_NUMBER> METHOD_NAMES = {
    "Debug", "Info", "Warn", "Error", "Fatal"
};

static const std::array<LogMethodFunc, METHODS_NUMBER> LOG_C_METHODS = {
    [] (const std::string &msg) {
        HILOG_BASE_DEBUG(LOG_CORE, "%{public}s", msg.c_str());
    },
    [] (const std::string &msg) {
        HILOG_BASE_INFO(LOG_CORE, "%{public}s", msg.c_str());
    },
    [] (const std::string &msg) {
        HILOG_BASE_WARN(LOG_CORE, "%{public}s", msg.c_str());
    },
    [] (const std::string &msg) {
        HILOG_BASE_ERROR(LOG_CORE, "%{public}s", msg.c_str());
    },
    [] (const std::string &msg) {
        HILOG_BASE_FATAL(LOG_CORE, "%{public}s", msg.c_str());
    },
};

static std::string PopenToString(const std::string &command)
{
    std::string str;
    constexpr int bufferSize = 1024;
    FILE *fp = popen(command.c_str(), "re");
    if (fp != nullptr) {
        char buf[bufferSize] = {0};
        size_t n = fread(buf, 1, sizeof(buf), fp);
        while (n > 0) {
            str.append(buf, n);
            n = fread(buf, 1, sizeof(buf), fp);
        }
        pclose(fp);
    }
    return str;
}

class HiLogBaseNDKTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown() {}
};

void HiLogBaseNDKTest::SetUp()
{
    (void)PopenToString("hilog -r");
}

static std::string RandomStringGenerator(uint32_t logLen = 16)
{
    std::string str;
    for (uint32_t i = 0; i < logLen; ++i) {
        char newChar = rand() % ('z' - 'a') + 'a';
        str.append(1, newChar);
    }
    return str;
}

static void HiLogWriteTest(LogMethodFunc loggingMethod, uint32_t logCount, uint32_t logLen, bool isDebug)
{
    std::string logMsg(RandomStringGenerator(logLen));
    for (uint32_t i = 0; i < logCount; ++i) {
        loggingMethod(logMsg + std::to_string(i));
    }
    if (logMsg.length() > MAX_LOG_LEN-1) {
        logMsg = logMsg.substr(0, MAX_LOG_LEN-1);
    }
    usleep(1000); /* 1000: sleep 1 ms */
    std::string logMsgs = PopenToString("/system/bin/hilog -x");
    uint32_t realCount = 0;
    std::stringstream ss(logMsgs);
    std::string str;
    while (!ss.eof()) {
        getline(ss, str);
        if (str.find(logMsg) != std::string::npos) {
            ++realCount;
        }
    }
    uint32_t allowedLeastLogCount = logCount - logCount * 1 / 10; /* 1 / 10: loss rate less than 10% */
    if (isDebug) {
        allowedLeastLogCount = 0; /* 0: debug log is allowed to be closed */
    }
    EXPECT_GE(realCount, allowedLeastLogCount);
}

HWTEST_F(HiLogBaseNDKTest, SomeLogs, TestSize.Level1)
{
    for(uint32_t i = 0; i < METHODS_NUMBER; ++i) {
        std::cout << "Starting " << METHOD_NAMES[i] << " test\n";
        HiLogWriteTest(LOG_C_METHODS[i], SOME_LOGS, SHORT_LOG, i == DEBUG_METHOD);
    }
}

HWTEST_F(HiLogBaseNDKTest, MoreLogs, TestSize.Level1)
{
    for(uint32_t i = 0; i < METHODS_NUMBER; ++i) {
        std::cout << "Starting " << METHOD_NAMES[i] << " test\n";
        HiLogWriteTest(LOG_C_METHODS[i], MORE_LOGS, SHORT_LOG, i == DEBUG_METHOD);
    }
}

HWTEST_F(HiLogBaseNDKTest, LongLogs, TestSize.Level1)
{
    for(uint32_t i = 0; i < METHODS_NUMBER; ++i) {
        std::cout << "Starting " << METHOD_NAMES[i] << " test\n";
        HiLogWriteTest(LOG_C_METHODS[i], 5, LONG_LOG, i == DEBUG_METHOD);
    }
}

HWTEST_F(HiLogBaseNDKTest, VeryLongLogs, TestSize.Level1)
{
    for(uint32_t i = 0; i < METHODS_NUMBER; ++i) {
        std::cout << "Starting " << METHOD_NAMES[i] << " test\n";
        HiLogWriteTest(LOG_C_METHODS[i], 5, VERY_LONG_LOG, i == DEBUG_METHOD);
    }
}

HWTEST_F(HiLogBaseNDKTest, MemAllocTouch1, TestSize.Level1)
{
    #undef TEXT_TO_CHECK 
    #define TEXT_TO_CHECK "Float potential mem alloc"
    HILOG_BASE_ERROR(LOG_CORE, TEXT_TO_CHECK " %{public}515.2f", 123.3);
    usleep(1000); /* 1000: sleep 1 ms */
    std::string logMsgs = PopenToString("/system/bin/hilog -x");
    uint32_t realCount = 0;
    std::stringstream ss(logMsgs);
    std::string str;
    while (!ss.eof()) {
        getline(ss, str);
        if (str.find(TEXT_TO_CHECK) != std::string::npos) {
            ++realCount;
        }
    }
    
    EXPECT_EQ(realCount, 1);
}

HWTEST_F(HiLogBaseNDKTest, MemAllocTouch2, TestSize.Level1)
{
    #undef TEXT_TO_CHECK 
    #define TEXT_TO_CHECK "Float potential mem alloc"
    HILOG_BASE_ERROR(LOG_CORE, TEXT_TO_CHECK " %{public}000000005.00000002f", 123.3);
    usleep(1000); /* 1000: sleep 1 ms */
    std::string logMsgs = PopenToString("/system/bin/hilog -x");
    uint32_t realCount = 0;
    std::stringstream ss(logMsgs);
    std::string str;
    while (!ss.eof()) {
        getline(ss, str);
        if (str.find(TEXT_TO_CHECK) != std::string::npos) {
            ++realCount;
        }
    }
    
    EXPECT_EQ(realCount, 1);
}

HWTEST_F(HiLogBaseNDKTest, IsLoggable, TestSize.Level1)
{
    EXPECT_TRUE(HiLogBaseIsLoggable(0xD002D00, LOG_TAG, LOG_DEBUG));
    EXPECT_TRUE(HiLogBaseIsLoggable(0xD002D00, LOG_TAG, LOG_INFO));
    EXPECT_TRUE(HiLogBaseIsLoggable(0xD002D00, LOG_TAG, LOG_WARN));
    EXPECT_TRUE(HiLogBaseIsLoggable(0xD002D00, LOG_TAG, LOG_ERROR));
    EXPECT_TRUE(HiLogBaseIsLoggable(0xD002D00, LOG_TAG, LOG_FATAL));
    EXPECT_TRUE(HiLogBaseIsLoggable(0xD002D00, "abc", LOG_WARN));
}

} // namespace HiLogTest
} // namespace HiviewDFX
} // namespace OHOS
