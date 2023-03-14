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

#include <gtest/gtest.h>
#include "dhcp_define.h"
#include "common_util.h"

using namespace testing::ext;
HWTEST(CommonUtilTest, LogTimeTest, TestSize.Level1)
{
    LogTime();
    uint64_t begin = Tmspsec();
    EXPECT_TRUE(begin > 0);
    sleep(1);
    uint64_t curr = Tmspsec();
    EXPECT_TRUE(curr > begin);
    begin = Tmspusec();
    EXPECT_TRUE(begin > 0);
    sleep(1);
    curr = Tmspusec();
    EXPECT_TRUE(begin > 0);
}

HWTEST(CommonUtilTest, LeftTirmTest, TestSize.Level1)
{
    char src1[] = " aabbccdd";
    LeftTrim(src1);
    EXPECT_STREQ("aabbccdd", src1);
}

HWTEST(CommonUtilTest, RightTrimTest, TestSize.Level1)
{
    char src1[] = "aabbccdd ";
    RightTrim(src1);
    EXPECT_STREQ("aabbccdd", src1);
}

HWTEST(CommonUtilTest, TrimStringTest, TestSize.Level1)
{
    char src1[] = "aabbccdd ";
    char src2[] = " aabbccdd";
    char src3[] = " aabbccdd  ";
    TrimString(src1);
    TrimString(src2);
    TrimString(src3);
    EXPECT_STREQ("aabbccdd", src1);
    EXPECT_STREQ("aabbccdd", src2);
    EXPECT_STREQ("aabbccdd", src3);
}

HWTEST(CommonUtilTest, GetFilePathTest, TestSize.Level1)
{
    const char *testPath = "/etc/dhcp/dhcp_server.conf";
    EXPECT_TRUE(GetFilePath(NULL) == NULL);
    EXPECT_TRUE(GetFilePath("") == NULL);
    EXPECT_STREQ("/etc/dhcp", GetFilePath(testPath));
}

HWTEST(CommonUtilTest, GetLeaseFileTest, TestSize.Level1)
{
    const char *leaseFile = "/etc/dhcp/dhcp_server.lease";
    const char *ifname = "test_if0";
    EXPECT_TRUE(GetLeaseFile(leaseFile, NULL) == NULL);
    EXPECT_TRUE(GetLeaseFile(NULL, NULL) == NULL);
    EXPECT_STREQ("/etc/dhcp/dhcp_server.lease.test_if0", GetLeaseFile(leaseFile, ifname));
}

HWTEST(CommonUtilTest, CreatePathTest, TestSize.Level1)
{
    const char *testPath = "./test/data/testpath";
    ASSERT_EQ(RET_FAILED, CreatePath(NULL));
    ASSERT_EQ(RET_FAILED, CreatePath(""));
    ASSERT_EQ(RET_SUCCESS, CreatePath(testPath));
    EXPECT_EQ(0, remove(testPath));
}