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

#include <gtest/gtest.h>
#include "datetime_ex.h"
#include <unistd.h>
using namespace testing::ext;
using namespace OHOS;
#include <iostream>
using namespace std;

class UtilsDateTimeTest : public testing::Test {
public :
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void UtilsDateTimeTest::SetUpTestCase(void)
{
}

void UtilsDateTimeTest::TearDownTestCase(void)
{
}

void UtilsDateTimeTest::SetUp(void)
{
}

void UtilsDateTimeTest::TearDown(void)
{
}

/*
 * @tc.name: testTimecover001
 * @tc.desc: convert all letters of str to uppercase
 */
HWTEST_F(UtilsDateTimeTest, testTimecover001, TestSize.Level0)
{
    int64_t second = 20;
    EXPECT_EQ(SecToNanosec(second), 20000000000);
    int64_t millsec = 10;
    EXPECT_EQ(MillisecToNanosec(millsec), 10000000);
    int64_t microsec = 5;
    EXPECT_EQ(MicrosecToNanosec(microsec), 5000);

    int64_t nanoces = 1000000000;
    EXPECT_EQ(NanosecToSec(nanoces), 1);
    EXPECT_EQ(NanosecToMillisec(nanoces), 1000);
    EXPECT_EQ(NanosecToMicrosec(nanoces), 1000000);
}

/*
 * @tc.name: testTime001
 * @tc.desc: datetime unit
 */
HWTEST_F(UtilsDateTimeTest, testTime001, TestSize.Level0)
{
    int64_t second = GetSecondsSince1970ToNow();

    struct tm curTime = {0};
    bool ret = GetSystemCurrentTime(&curTime);
    EXPECT_EQ(ret, true);

    int64_t second2 = GetSecondsSince1970ToPointTime(curTime);
    EXPECT_EQ(second, second2);
}

/*
 * @tc.name: testTime002
 * @tc.desc: datetime unit
 */
HWTEST_F(UtilsDateTimeTest, testTime002, TestSize.Level0)
{
    int64_t days = GetDaysSince1970ToNow();
    int64_t seconds = GetSecondsSince1970ToNow();
    int64_t resultdays = seconds / (3600 * 24);
    EXPECT_EQ(days, resultdays);
}

/*
 * @tc.name: testTime003
 * @tc.desc: datetime unit
 */
HWTEST_F(UtilsDateTimeTest, testTime003, TestSize.Level0)
{
    struct tm curTime = { 0 };
    bool ret = GetSystemCurrentTime(&curTime);
    EXPECT_EQ(ret, true);

    sleep(3);

    struct tm curTime2 = { 0 };
    ret = GetSystemCurrentTime(&curTime2);
    EXPECT_EQ(ret, true);
    int64_t betweensec = GetSecondsBetween(curTime, curTime2);
    EXPECT_TRUE(betweensec >= 3);
}

/*
 * @tc.name: testTime004
 * @tc.desc: datetime unit
 */
HWTEST_F(UtilsDateTimeTest, testTime004, TestSize.Level0)
{
    int timezone = 0;
    bool ret = GetLocalTimeZone(timezone);
    EXPECT_EQ(ret, true);
}

/*
 * @tc.name: testGetTickCount001
 * @tc.desc: datetime unit
 */
HWTEST_F(UtilsDateTimeTest, testGetTickCount001, TestSize.Level0)
{
    int64_t begin = GetTickCount();
    usleep(100000);
    int64_t end = GetTickCount();

    EXPECT_TRUE(end - begin >= 100);
    EXPECT_TRUE(end - begin < 120);
}

/*
 * @tc.name: testGetMicroTickCount001
 * @tc.desc: datetime unit
 */
HWTEST_F(UtilsDateTimeTest, testGetMicroTickCount001, TestSize.Level0)
{
    int64_t begin = GetMicroTickCount();
    usleep(100000);
    int64_t end = GetMicroTickCount();

    EXPECT_TRUE(end - begin >= 100000);
    EXPECT_TRUE(end - begin < 120000);
}

