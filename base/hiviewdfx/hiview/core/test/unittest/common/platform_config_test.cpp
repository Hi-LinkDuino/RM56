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
#include "platform_config_test.h"
#include "plugin_config.h"
using namespace testing::ext;
using namespace OHOS::HiviewDFX;

void PlatformConfigTest::SetUp()
{
    /**
     * @tc.setup: create an event loop and multiple event handlers
     */
    printf("SetUp.\n");
}

void PlatformConfigTest::TearDown()
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
HWTEST_F(PlatformConfigTest, PlatformConfigParse001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create event handler and events
     */
    PluginConfig config("/data/test/test_data/plugin_config1");
    if (!config.StartParse()) {
        printf("fail to parse plugin config. exit! \n");
        FAIL();
    }

    /**
     * @tc.expected: step1. the event has been processed
     */
    auto pluginInfoList = config.GetPluginInfoList();
    EXPECT_EQ(pluginInfoList.size(), 37ul);
    for (auto& pluginInfo : pluginInfoList) {
        printf("read plugin with name:%s \n", pluginInfo.name.c_str());
    }

    auto pipelineInfoList = config.GetPipelineInfoList();
    EXPECT_EQ(pipelineInfoList.size(), 10ul);
    for (auto& pipelineInfo : pipelineInfoList) {
        printf("read pipeline with name:%s \n", pipelineInfo.name.c_str());
    }
}

/**
 * @tc.name: PlatformConfigParse002
 * @tc.desc: parse a incorrect config file and check result
 * @tc.type: FUNC
 * @tc.require: AR000DPTT5
 */
HWTEST_F(PlatformConfigTest, PlatformConfigParse002, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create event handler and events
     */
    PluginConfig config("/data/test/test_data/plugin_config_incomplete");
    if (!config.StartParse()) {
        printf("fail to parse plugin config. exit! \n");
        FAIL();
    }

    /**
     * @tc.expected: step1. the event has been processed
     */
    auto pluginInfoList = config.GetPluginInfoList();
    EXPECT_EQ(pluginInfoList.size(), 5ul);
    for (auto& pluginInfo : pluginInfoList) {
        printf("read plugin with name:%s \n", pluginInfo.name.c_str());
    }

    auto pipelineInfoList = config.GetPipelineInfoList();
    EXPECT_EQ(pipelineInfoList.size(), 0ul);
    for (auto& pipelineInfo : pipelineInfoList) {
        printf("read pipeline with name:%s \n", pipelineInfo.name.c_str());
    }
}
