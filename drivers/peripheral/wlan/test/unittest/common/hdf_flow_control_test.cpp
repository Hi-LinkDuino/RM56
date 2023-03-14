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

#include "hdf_wifi_test.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <gtest/gtest.h>
#include "hdf_uhdf_test.h"
#include "hdf_io_service_if.h"

using namespace testing::ext;

namespace FlowControlTest {
class WiFiFlowControlTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void WiFiFlowControlTest::SetUpTestCase()
{
    HdfTestOpenService();
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_FLOW_CONTROL_INIT, -1};
    HdfTestSendMsgToService(&msg);
}

void WiFiFlowControlTest::TearDownTestCase()
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_FLOW_CONTROL_DEINIT, -1};
    HdfTestSendMsgToService(&msg);
    HdfTestCloseService();
}

void WiFiFlowControlTest::SetUp() {}

void WiFiFlowControlTest::TearDown() {}

/**
 * @tc.name: WiFiFlowControlGetQueueID001
 * @tc.desc: Obtains the priority ID of a flow control queue
 * @tc.type: FUNC
 * @tc.require: AR000F869F
 */
HWTEST_F(WiFiFlowControlTest, WiFiFlowControlGetQueueID001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_FLOW_CONTROL_GET_QUEUE_ID, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: WiFiFlowControlSendData001
 * @tc.desc: Sends data of a flow control
 * @tc.type: FUNC
 * @tc.require: AR000F869F
 */
HWTEST_F(WiFiFlowControlTest, WiFiFlowControlSendData001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_FLOW_CONTROL_SEND_DATA, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
};
