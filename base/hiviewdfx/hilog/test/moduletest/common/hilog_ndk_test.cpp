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

#include <unistd.h>

#include <gtest/gtest.h>

#include "hilog/log.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD002D00

#undef LOG_TAG
#define LOG_TAG "HILOGTEST_C"

using namespace testing::ext;

namespace OHOS {
namespace HiviewDFX {
namespace HiLogTest {
const HiLogLabel LABEL = { LOG_CORE, 0xD002D00, "HILOGTEST_CPP" };
static constexpr unsigned int SOME_LOGS = 10;
static constexpr unsigned int MORE_LOGS = 100;

enum LogInterfaceType {
    DEBUG_METHOD = 0,
    INFO_METHOD = 1,
    WARN_METHOD = 2,
    ERROR_METHOD = 3,
    FATAL_METHOD = 4,
    METHODS_NUMBER = 5,
};

using LogMethodFunc = std::function<void(const std::string &msg)>;

static const std::array<LogMethodFunc, METHODS_NUMBER> LOG_C_METHODS = {
    [] (const std::string &msg) {
        HILOG_DEBUG(LOG_CORE, "%{public}s", msg.c_str());
    },
    [] (const std::string &msg) {
        HILOG_INFO(LOG_CORE, "%{public}s", msg.c_str());
    },
    [] (const std::string &msg) {
        HILOG_WARN(LOG_CORE, "%{public}s", msg.c_str());
    },
    [] (const std::string &msg) {
        HILOG_ERROR(LOG_CORE, "%{public}s", msg.c_str());
    },
    [] (const std::string &msg) {
        HILOG_FATAL(LOG_CORE, "%{public}s", msg.c_str());
    },
};

static const std::array<LogMethodFunc, METHODS_NUMBER> LOG_CPP_METHODS = {
    [] (const std::string &msg) {
        HiLog::Debug(LABEL, "%{public}s", msg.c_str());
    },
    [] (const std::string &msg) {
        HiLog::Info(LABEL, "%{public}s", msg.c_str());
    },
    [] (const std::string &msg) {
        HiLog::Warn(LABEL, "%{public}s", msg.c_str());
    },
    [] (const std::string &msg) {
        HiLog::Error(LABEL, "%{public}s", msg.c_str());
    },
    [] (const std::string &msg) {
        HiLog::Fatal(LABEL, "%{public}s", msg.c_str());
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

class HiLogNDKTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown() {}
};

void HiLogNDKTest::SetUp()
{
    (void)PopenToString("hilog -r");
}

static std::string RandomStringGenerator()
{
    std::string str;
    int logLen = 16;
    char index;
    for (int i = 0; i < logLen; ++i) {
        index = rand() % ('z' - 'a') + 'a';
        str.append(1, index);
    }
    return str;
}

static void HiLogWriteTest(LogInterfaceType methodType, unsigned int count,
    const std::array<LogMethodFunc, METHODS_NUMBER> &logMethods)
{
    std::string logMsg(RandomStringGenerator());
    for (unsigned int i = 0; i < count; ++i) {
        logMethods.at(methodType)(logMsg + std::to_string(i));
    }
    usleep(1000); /* 1000: sleep 1 ms */
    std::string logMsgs = PopenToString("/system/bin/hilog -x");
    unsigned int realCount = 0;
    std::stringstream ss(logMsgs);
    std::string str;
    while (!ss.eof()) {
        getline(ss, str);
        if (str.find(logMsg) != std::string::npos) {
            ++realCount;
        }
    }
    unsigned int allowedLeastLogCount = count - count * 1 / 10; /* 1 / 10: loss rate less than 10% */
    if (methodType == DEBUG_METHOD) {
        allowedLeastLogCount = 0; /* 0: debug log is allowed to be closed */
    }
    EXPECT_GE(realCount, allowedLeastLogCount);
}

/**
 * @tc.name: Dfx_HiLogNDKTest_PrintDebugLog_001
 * @tc.desc: Call HILOG_DEBUG to print logs.
 * @tc.type: FUNC
 */
HWTEST_F(HiLogNDKTest, PrintDebugLog_001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Call HILOG_DEBUG to print logs and call hilog to read it
     * @tc.expected: step1. Logs can be printed only if hilog.debug is enabled.
     */
    HiLogWriteTest(DEBUG_METHOD, SOME_LOGS, LOG_C_METHODS);
}

/**
 * @tc.name: Dfx_HiLogNDKTest_PrintInfoLog_001
 * @tc.desc: Call HILOG_INFO to print logs.
 * @tc.type: FUNC
 */
HWTEST_F(HiLogNDKTest, PrintInfoLog_001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Call HILOG_INFO to print logs and call hilog to read it
     * @tc.expected: step1. Logs printed without loss.
     */
    HiLogWriteTest(INFO_METHOD, SOME_LOGS, LOG_C_METHODS);
}

/**
 * @tc.name: Dfx_HiLogNDKTest_PrintWarnLog_001
 * @tc.desc: Call HILOG_WARN to print logs.
 * @tc.type: FUNC
 */
HWTEST_F(HiLogNDKTest, PrintWarnLog_001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Call HILOG_WARN to print logs and call hilog to read it
     * @tc.expected: step1. Logs printed without loss.
     */
    HiLogWriteTest(WARN_METHOD, SOME_LOGS, LOG_C_METHODS);
}

/**
 * @tc.name: Dfx_HiLogNDKTest_PrintErrorLog_001
 * @tc.desc: Call HILOG_ERROR to print logs.
 * @tc.type: FUNC
 */
HWTEST_F(HiLogNDKTest, PrintErrorLog_001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Call HILOG_ERROR to print logs and call hilog to read it
     * @tc.expected: step1. Logs printed without loss.
     */
    HiLogWriteTest(ERROR_METHOD, SOME_LOGS, LOG_C_METHODS);
}

/**
 * @tc.name: Dfx_HiLogNDKTest_PrintFatalLog_001
 * @tc.desc: Call HILOG_FATAL to print logs.
 * @tc.type: FUNC
 */
HWTEST_F(HiLogNDKTest, PrintFatalLog_001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Call HILOG_FATAL to print logs and call hilog to read it
     * @tc.expected: step1. Logs printed without loss.
     */
    HiLogWriteTest(FATAL_METHOD, SOME_LOGS, LOG_C_METHODS);
}

/**
 * @tc.name: Dfx_HiLogNDKTest_LogLossCheck_001
 * @tc.desc: HiLog log loss rate must less than 10%.
 * @tc.type: FUNC
 */
HWTEST_F(HiLogNDKTest, LogLossCheck_001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Call HILOG_INFO to print logs and call hilog to read it
     * @tc.expected: step1. Calculate log loss rate and it should less than 10%
     */
    HiLogWriteTest(INFO_METHOD, MORE_LOGS, LOG_C_METHODS);
}

/**
 * @tc.name: Dfx_HiLogNDKTest_PrintDebugLog_002
 * @tc.desc: Call HiLog::Debug to print logs.
 * @tc.type: FUNC
 */
HWTEST_F(HiLogNDKTest, PrintDebugLog_002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Call HiLog::Debug to print logs and call hilog to read it
     * @tc.expected: step1. Logs can be printed only if hilog.debug is enabled.
     */
    HiLogWriteTest(DEBUG_METHOD, SOME_LOGS, LOG_CPP_METHODS);
}

/**
 * @tc.name: Dfx_HiLogNDKTest_PrintInfoLog_002
 * @tc.desc: Call HiLog::Info to print logs.
 * @tc.type: FUNC
 */
HWTEST_F(HiLogNDKTest, PrintInfoLog_002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Call HiLog::Info to print logs and call hilog to read it
     * @tc.expected: step1. Logs printed without loss.
     */
    HiLogWriteTest(INFO_METHOD, SOME_LOGS, LOG_CPP_METHODS);
}

/**
 * @tc.name: Dfx_HiLogNDKTest_PrintWarnLog_002
 * @tc.desc: Call HiLog::Warn print logs.
 * @tc.type: FUNC
 */
HWTEST_F(HiLogNDKTest, PrintWarnLog_002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Call HiLog::Warn to print logs and call hilog to read it
     * @tc.expected: step1. Logs printed without loss.
     */
    HiLogWriteTest(WARN_METHOD, SOME_LOGS, LOG_CPP_METHODS);
}

/**
 * @tc.name: Dfx_HiLogNDKTest_PrintErrorLog_002
 * @tc.desc: Call HiLog::Error to print logs.
 * @tc.type: FUNC
 */
HWTEST_F(HiLogNDKTest, PrintErrorLog_002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Call HiLog::Error to print logs and call hilog to read it
     * @tc.expected: step1. Logs printed without loss.
     */
    HiLogWriteTest(ERROR_METHOD, SOME_LOGS, LOG_CPP_METHODS);
}

/**
 * @tc.name: Dfx_HiLogNDKTest_PrintFatalLog_002
 * @tc.desc: Call HiLog::Fatal to print logs.
 * @tc.type: FUNC
 */
HWTEST_F(HiLogNDKTest, PrintFatalLog_002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Call HiLog::Fatal to print logs and call hilog to read it
     * @tc.expected: step1. Logs printed without loss.
     */
    HiLogWriteTest(FATAL_METHOD, SOME_LOGS, LOG_CPP_METHODS);
}

/**
 * @tc.name: Dfx_HiLogNDKTest_LogLossCheck_002
 * @tc.desc: HiLog log loss rate must less than 10%
 * @tc.type: FUNC
 */
HWTEST_F(HiLogNDKTest, LogLossCheck_002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Call HiLog::Info to print logs and call hilog to read it
     * @tc.expected: step1. Calculate log loss rate and it should less than 10%
     */
    HiLogWriteTest(INFO_METHOD, MORE_LOGS, LOG_CPP_METHODS);
}

/**
 * @tc.name: Dfx_HiLogNDKTest_IsLoggable_001
 * @tc.desc: Check whether is loggable for each log level
 * @tc.type: FUNC
 */
HWTEST_F(HiLogNDKTest, IsLoggable_001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Call HiLogIsLoggable to check whether is loggable for each log level.
     * @tc.expected: step1. LOG_DEBUG and lower level should return false, and others return true.
     */
    EXPECT_TRUE(HiLogIsLoggable(0xD002D00, LOG_TAG, LOG_DEBUG));
    EXPECT_TRUE(HiLogIsLoggable(0xD002D00, LOG_TAG, LOG_INFO));
    EXPECT_TRUE(HiLogIsLoggable(0xD002D00, LOG_TAG, LOG_WARN));
    EXPECT_TRUE(HiLogIsLoggable(0xD002D00, LOG_TAG, LOG_ERROR));
    EXPECT_TRUE(HiLogIsLoggable(0xD002D00, LOG_TAG, LOG_FATAL));
    EXPECT_TRUE(HiLogIsLoggable(0xD002D00, "abc", LOG_WARN));
}

} // namespace HiLogTest
} // namespace HiviewDFX
} // namespace OHOS
