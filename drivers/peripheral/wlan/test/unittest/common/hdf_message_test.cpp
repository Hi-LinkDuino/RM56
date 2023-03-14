/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <string>
#include <unistd.h>
#include "hdf_uhdf_test.h"
#include "hdf_wifi_test.h"

using namespace testing::ext;

namespace MessageTest {
class WiFiMessageTest : public testing::Test {
public:
    WiFiMessageTest() {}
    virtual ~WiFiMessageTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void WiFiMessageTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void WiFiMessageTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void WiFiMessageTest::SetUp() {}

void WiFiMessageTest::TearDown() {}

/**
 * @tc.name: WIFI_MESSAGE_QUEUE001
 * @tc.desc: Message queue test case 001.
 * @tc.type: FUNC
 * @tc.require: AR000F86AL
 */
HWTEST_F(WiFiMessageTest, WIFI_MESSAGE_QUEUE001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_WIFI_TYPE, WIFI_MESSAGE_QUEUE_001, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: WIFI_MESSAGE_QUEUE002
 * @tc.desc: Message queue test case 002
 * @tc.type: FUNC
 * @tc.require: AR000F86AL
 */
HWTEST_F(WiFiMessageTest, WIFI_MESSAGE_QUEUE002, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_WIFI_TYPE, WIFI_MESSAGE_QUEUE_002, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: WIFI_MESSAGE_QUEUE003
 * @tc.desc: Message queue test case 003
 * @tc.type: FUNC
 * @tc.require: AR000F86AL
 */
HWTEST_F(WiFiMessageTest, WIFI_MESSAGE_QUEUE003, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_WIFI_TYPE, WIFI_MESSAGE_QUEUE_003, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: WIFI_MESSAGE_SINGLE_NODE001
 * @tc.desc: Single node message route test 001
 * @tc.type: FUNC
 * @tc.require: AR000F86AL
 */
HWTEST_F(WiFiMessageTest, WIFI_MESSAGE_SINGLE_NODE001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_WIFI_TYPE, WIFI_MESSAGE_SINGLE_NODE_001, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: WIFI_MESSAGE_SINGLE_NODE002
 * @tc.desc: Single node message route test 002
 * @tc.type: FUNC
 * @tc.require: AR000F86AL
 */
HWTEST_F(WiFiMessageTest, WIFI_MESSAGE_SINGLE_NODE002, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_WIFI_TYPE, WIFI_MESSAGE_SINGLE_NODE_002, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: WIFI_MESSAGE_SINGLE_NODE003
 * @tc.desc: Single node message route test 003
 * @tc.type: FUNC
 * @tc.require: AR000F86AL
 */
HWTEST_F(WiFiMessageTest, WIFI_MESSAGE_SINGLE_NODE003, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_WIFI_TYPE, WIFI_MESSAGE_SINGLE_NODE_003, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: WIFI_MESSAGE_SINGLE_NODE004
 * @tc.desc: Single node message route test 004
 * @tc.type: FUNC
 * @tc.require: AR000F86AL
 */
HWTEST_F(WiFiMessageTest, WIFI_MESSAGE_SINGLE_NODE004, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_WIFI_TYPE, WIFI_MESSAGE_SINGLE_NODE_004, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: WIFI_MESSAGE_SINGLE_NODE005
 * @tc.desc: Single node message route test 005
 * @tc.type: FUNC
 * @tc.require: AR000F86AL
 */
HWTEST_F(WiFiMessageTest, WIFI_MESSAGE_SINGLE_NODE005, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_WIFI_TYPE, WIFI_MESSAGE_SINGLE_NODE_005, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
};
