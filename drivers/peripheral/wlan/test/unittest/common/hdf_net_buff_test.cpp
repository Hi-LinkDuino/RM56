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
#include "hdf_io_service_if.h"
#include "hdf_uhdf_test.h"
#include "hdf_wifi_test.h"

using namespace testing::ext;

namespace NetBuffTest {
class WiFiNetBuffTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void WiFiNetBuffTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void WiFiNetBuffTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void WiFiNetBuffTest::SetUp() {}

void WiFiNetBuffTest::TearDown() {}

/**
  * @tc.name: WiFiNetBuff001
  * @tc.desc: netbuf function test
  * @tc.type: FUNC
  * @tc.require: AR000F869I
  */
HWTEST_F(WiFiNetBuffTest, WiFiNetBuff001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_BUF_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: WiFiNetBuffQueue001
  * @tc.desc: netbuf queue function test
  * @tc.type: FUNC
  * @tc.require: AR000F869I
  */
HWTEST_F(WiFiNetBuffTest, WiFiNetBuffQueue001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_BUF_QUEUE_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
};
