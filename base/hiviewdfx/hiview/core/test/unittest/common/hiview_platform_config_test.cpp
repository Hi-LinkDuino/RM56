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
#include "hiview_platform_config_test.h"

#include "hiview_platform_config.h"
using namespace testing::ext;
using namespace OHOS::HiviewDFX;

void HiviewPlatformConfigTest::SetUp()
{
    /**
     * @tc.setup: create an event loop and multiple event handlers
     */
    printf("SetUp.\n");
}

void HiviewPlatformConfigTest::TearDown()
{
    /**
     * @tc.teardown: destroy the event loop we have created
     */
    printf("TearDown.\n");
}

/**
 * @tc.name: PlatformConfigParse001
 * @tc.desc: parse a correct config file and check result
 * @tc.type: FUNC
 * @tc.require: AR000DPTT5
 */
HWTEST_F(HiviewPlatformConfigTest, PlatformConfigParse001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create event handler and events
     */
    HiviewPlatformConfig platformConfig = HiviewPlatformConfig("/data/test/test_data/hiview_platform_config");
    HiviewPlatformConfig::PlatformConfigInfo platformConfigInfo;
    bool state = platformConfig.ParsesConfig(platformConfigInfo);
    ASSERT_EQ(true, state);
    ASSERT_EQ(platformConfigInfo.defaultPluginConfigName, "plugin_config");
    ASSERT_EQ(platformConfigInfo.pluginConfigFileDir, "/data/test/test_data/");
    ASSERT_EQ(platformConfigInfo.dynamicLibSearchDir, "/system/lib/");
    ASSERT_EQ(platformConfigInfo.dynamicLib64SearchDir, "/system/lib64/");
    ASSERT_EQ(platformConfigInfo.cloudUpdateConfigDir, "/data/system/hiview/");
    ASSERT_EQ(platformConfigInfo.workDir, "/data/log/LogService/");
    ASSERT_EQ(platformConfigInfo.commercialWorkDir, "/log/LogService/");
    ASSERT_EQ(platformConfigInfo.persistDir, "/log/hiview/");
}