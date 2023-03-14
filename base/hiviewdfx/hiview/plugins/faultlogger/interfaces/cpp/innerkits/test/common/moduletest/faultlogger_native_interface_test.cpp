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
#include <ctime>
#include <list>

#include <gtest/gtest.h>

#include "faultlog_query_result.h"
#include "faultlogger_client.h"
#include "faultlogger_client_test.h"
#include "file_util.h"
using namespace testing::ext;
using namespace OHOS::HiviewDFX;
namespace OHOS {
namespace HiviewDFX {
class FaultloggerNativeInterfaceTest : public testing::Test {
public:
    void SetUp()
    {
        sleep(1);
    };
    void TearDown(){};
};

/**
 * @tc.name: AddFaultLogTest001
 * @tc.desc: check the action of the AddFaultLog interface
 * @tc.type: FUNC
 * @tc.require: SR000F7ULN AR000F83AK
 */
HWTEST_F(FaultloggerNativeInterfaceTest, AddFaultLogTest001, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. add faultlog with simplified parameters
     * @tc.steps: step2. check the return value of the interface
     * @tc.steps: step3. check the existance of target log file
     * @tc.expected: the calling is success and the file has been created
     */
    auto now = time(nullptr);
    auto info = CreateFaultLogInfo(now, getuid(), FaultLogType::CPP_CRASH, "faultloggertest");
    AddFaultLog(info);
    ASSERT_TRUE(CheckLogFileExist(now, getuid(), "cppcrash", "faultloggertest"));
}

/**
 * @tc.name: AddFaultLogTest002
 * @tc.desc: check the action of the AddFaultLog interface
 * @tc.type: FUNC
 * @tc.require: SR000F7ULN AR000F83AK
 */
HWTEST_F(FaultloggerNativeInterfaceTest, AddFaultLogTest002, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. add faultlog with complete parameters
     * @tc.steps: step2. check the return value of the interface
     * @tc.steps: step3. check the existance of target log file
     * @tc.expected: the calling is success and the file has been created
     */
    auto now = time(nullptr);
    auto module = "faultlogtest1";
    AddFaultLog(now, FaultLogType::CPP_CRASH, module, "faultloggertestsummary2");
    ASSERT_TRUE(CheckLogFileExist(now, getuid(), "cppcrash", module));
}

/**
 * @tc.name: AddFaultLogTest003
 * @tc.desc: add more than MAX_KEEP_LOGS into faultlogger, check the exist files
 * @tc.type: FUNC
 * @tc.require: SR000F7ULN AR000F83AL
 */
HWTEST_F(FaultloggerNativeInterfaceTest, AddFaultLogTest003, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. add faultlog with complete parameters
     * @tc.steps: step2. check the return value of the interface
     * @tc.steps: step3. check the existance of target log file
     * @tc.expected: the calling is success and the file has been created
     */
    auto now = time(nullptr);
    auto module = "faultlogtest5";
    const int loopTimes = 20;
    const int maxKeepFiles = 10;
    for (int i = 0; i < loopTimes; i++) {
        AddFaultLog(now + i, FaultLogType::CPP_CRASH, module, "faultloggertestsummary2");
        usleep(100000); // 0.1 s
    }

    sleep(1);
    for (int i = loopTimes - 1; i > maxKeepFiles; i--) {
        ASSERT_TRUE(CheckLogFileExist(now + i, getuid(), "cppcrash", module));
    }

    for (int i = 0; i < maxKeepFiles; i++) {
        ASSERT_FALSE(CheckLogFileExist(now + i, getuid(), "cppcrash", module));
    }
}

/**
 * @tc.name: AddFaultLogTest004
 * @tc.desc: add java crash log and check whether the log file is exist
 * @tc.type: FUNC
 * @tc.require: SR000F7ULN AR000F83BB
 */
HWTEST_F(FaultloggerNativeInterfaceTest, AddFaultLogTest004, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. add faultlog with complete parameters
     * @tc.steps: step2. check the existance of target log file
     * @tc.expected: the calling is success and the file has been created
     */
    auto now = time(nullptr);
    auto module = "faultlogtest4";
    AddFaultLog(now, FaultLogType::JAVA_CRASH, module, "faultloggertestsummary2");
    ASSERT_TRUE(CheckLogFileExist(now, getuid(), "javacrash", module));
}

/**
 * @tc.name: QuerySelfFaultLogTest001
 * @tc.desc: check the existance of the previous added faultlog
 * @tc.type: FUNC
 * @tc.require: SR000F7ULN AR000F83AK
 */
HWTEST_F(FaultloggerNativeInterfaceTest, QuerySelfFaultLogTest001, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. add multiple fault log by add fault log interface
     */
    auto now = time(nullptr);
    auto module = "faultlogtest6";
    const int loopTimes = 10;
    for (int i = 0; i < loopTimes; i++) {
        AddFaultLog(now + i, FaultLogType::CPP_CRASH, module, "faultloggertestsummary6");
        usleep(100000); // 0.1 s
    }
    sleep(5);

    /**
     * @tc.steps: step2. query the log by QuerySelfFaultLog interfaces
     * @tc.steps: step3. check counts and contents of the log
     * @tc.expected: the count is correct and the contents is complete
     */
    const int maxQueryCount = 100;
    int32_t currentCount = 0;
    auto result = QuerySelfFaultLog(FaultLogType::NO_SPECIFIC, maxQueryCount);
    if (result != nullptr) {
        while (result->HasNext()) {
            if (currentCount >= maxQueryCount) {
                break;
            }
            auto info = result->Next();
            if (info == nullptr) {
                FAIL();
            }
            currentCount++;
            printf("Current LogNo:%d module:%s time:%d \n", currentCount, info->GetModuleName().c_str(),
                   static_cast<int32_t>(info->GetTimeStamp()));
            if (info->GetModuleName() == "faultloggertest") {
                ASSERT_EQ("TestReason", info->GetFaultReason());
            }

            int32_t fd = info->GetRawFileDescriptor();
            if (fd < 0) {
                printf("Fail to get fd:%d\n", fd);
            } else {
                close(fd);
            }
        }
    }
    sleep(5);
    ASSERT_GE(currentCount, 0);
    printf("QuerySelfFaultLog count:%d", currentCount);
}

/**
 * @tc.name: QuerySelfFaultLogTest002
 * @tc.desc: One application should always has one query
 * @tc.type: FUNC
 * @tc.require: SR000F7ULN AR000F83AK
 */
HWTEST_F(FaultloggerNativeInterfaceTest, QuerySelfFaultLogTest002, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. call QuerySelfFaultLog interface twice
     * @tc.steps: step2. check the query result
     * @tc.expected: the first query success and the second fail
     */
    const int maxQueryCount = 10;
    auto result = QuerySelfFaultLog(FaultLogType::NO_SPECIFIC, maxQueryCount);
    ASSERT_NE(nullptr, result);
    auto result2 = QuerySelfFaultLog(FaultLogType::NO_SPECIFIC, maxQueryCount);
    ASSERT_EQ(nullptr, result2);
}

/**
 * @tc.name: QuerySelfFaultLogTest003
 * @tc.desc: query java crash log
 * @tc.type: FUNC
 * @tc.require: SR000F7ULN AR000F83AH
 */
HWTEST_F(FaultloggerNativeInterfaceTest, QuerySelfFaultLogTest003, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. call QuerySelfFaultLog interface twice
     * @tc.steps: step2. check the query result
     * @tc.expected: the first query success and the second fail
     */
    const int maxQueryCount = 10;
    auto result = QuerySelfFaultLog(FaultLogType::JAVA_CRASH, maxQueryCount);
    ASSERT_NE(nullptr, result);
    auto result2 = QuerySelfFaultLog(FaultLogType::JAVA_CRASH, maxQueryCount);
    ASSERT_EQ(nullptr, result2);
}

/**
 * @tc.name: QuerySelfFaultLogTest004
 * @tc.desc: query cpp crash log
 * @tc.type: FUNC
 * @tc.require: SR000F7ULN AR000F84UR
 */
HWTEST_F(FaultloggerNativeInterfaceTest, QuerySelfFaultLogTest004, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. call QuerySelfFaultLog interface twice
     * @tc.steps: step2. check the query result
     * @tc.expected: the first query success and the second fail
     */
    const int maxQueryCount = 10;
    auto result = QuerySelfFaultLog(FaultLogType::CPP_CRASH, maxQueryCount);
    ASSERT_NE(nullptr, result);
    auto result2 = QuerySelfFaultLog(FaultLogType::CPP_CRASH, maxQueryCount);
    ASSERT_EQ(nullptr, result2);
}


/**
 * @tc.name: QuerySelfFaultLogTest005
 * @tc.desc: query interface pass invalid parameter
 * @tc.type: FUNC
 * @tc.require: SR000F7UQ8 AR000F83AJ
 */
HWTEST_F(FaultloggerNativeInterfaceTest, QuerySelfFaultLogTest005, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. call QuerySelfFaultLog interface twice
     * @tc.steps: step2. check the query result
     * @tc.expected: the first query success and the second fail
     */
    auto result = QuerySelfFaultLog(static_cast<OHOS::HiviewDFX::FaultLogType>(-1), -1); // -1 : invalid value
    ASSERT_EQ(nullptr, result);
    auto result2 = QuerySelfFaultLog(FaultLogType::CPP_CRASH, 100000); // 100000 : max count
    ASSERT_NE(nullptr, result2);
}
} // namespace HiviewDFX
} // namespace OHOS
