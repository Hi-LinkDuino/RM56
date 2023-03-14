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
#include "hdf_io_service_if.h"
#include "hdf_wifi_test.h"

using namespace testing::ext;

namespace ModuleTest {
class WiFiModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void WiFiModuleTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void WiFiModuleTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void WiFiModuleTest::SetUp() {}

void WiFiModuleTest::TearDown() {}

/**
  * @tc.name: WiFiModuleCreate001
  * @tc.desc: wifi create module function test
  * @tc.type: FUNC
  * @tc.require: SR000F869D
  */
HWTEST_F(WiFiModuleTest, WiFiModuleCreate001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_MODULE_CREATE_MODULE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: WiFiModuleAddFeature001
  * @tc.desc: wifi module add feature function test
  * @tc.type: FUNC
  * @tc.require: AR000F869K
  */
HWTEST_F(WiFiModuleTest, WiFiModuleAddFeature001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_MODULE_ADD_FEATURE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: WiFiModuleDeleteFeature001
  * @tc.desc: wifi module delete feature function test
  * @tc.type: FUNC
  * @tc.require: AR000F869K
  */
HWTEST_F(WiFiModuleTest, WiFiModuleDeleteFeature001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_MODULE_DELETE_FEATURE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: WiFiModuleDelete001
  * @tc.desc: wifi delete module function test
  * @tc.type: FUNC
  * @tc.require: AR000F869K
  */
HWTEST_F(WiFiModuleTest, WiFiModuleDelete001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_MODULE_DELETE_MODULE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
};
