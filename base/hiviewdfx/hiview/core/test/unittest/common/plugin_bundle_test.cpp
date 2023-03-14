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
#include "plugin_bundle_test.h"

#include <fstream>
#include <iostream>

#include <unistd.h>

#include "event.h"
#include "file_util.h"
#include "hiview_platform.h"
#include "plugin.h"
#include "plugin_proxy.h"

#include "platform_test_result_listener.h"
using namespace testing::ext;
using namespace OHOS::HiviewDFX;

void PluginBundleTest::SetUp()
{
    /**
     * @tc.setup: create work directories
     */
    printf("SetUp.\n");
    FileUtil::ForceCreateDirectory("/data/test/faultlog");
}

/**
 * @tc.name: PluginBundleLoadTest001
 * @tc.desc: load plugin bundle from bundle config
 * @tc.type: FUNC
 */
HWTEST_F(PluginBundleTest, PluginBundleLoadTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. init plugin platform and set proxy idle time
     */
    printf("PluginBundleTest.\n");
    OHOS::HiviewDFX::HiviewPlatform platform;
    platform.SetMaxProxyIdleTime(10); // 10 seconds
    platform.SetCheckProxyIdlePeriod(5); // 5 seconds
    if (!platform.InitEnvironment("/data/test/test_data/hiview_platform_config")) {
        printf("Fail to init environment. \n");
    }

    sleep(1);

    /**
     * @tc.steps: step2. check whether the bundle plugins have been loaded.
     */
    ASSERT_EQ(true, platform.IsReady());
    auto& pluginList = platform.GetPluginMap();
    auto size = pluginList.size();
    ASSERT_EQ(size, 6ul);

    auto& bundleMap = platform.GetPluginBundleInfoMap();
    if (bundleMap.find("bundletest") == bundleMap.end()) {
        printf("Failed to find bundletest bundle.\n");
        FAIL();
    }

    /**
     * @tc.steps: step3. check whether the proxy plugin has been created
     */
    auto pluginPair = pluginList.find("EventProcessorExample5");
    if (pluginPair == pluginList.end()) {
        printf("Failed to find EventProcessorExample5.\n");
        FAIL();
    }

    if (pluginPair->second->GetType() != Plugin::PluginType::PROXY) {
        printf("Failed to proxy EventProcessorExample5.\n");
        FAIL();
    }

    /**
     * @tc.steps: step4. check whether the proxy plugin has been instaniated
     */
    if (std::static_pointer_cast<PluginProxy>(pluginPair->second)->HoldInstance()) {
        printf("Proxy should not be instaniated.\n");
        FAIL();
    }

    std::ofstream fileC("/data/test/faultlog/ccc");
    fileC << "create ccc event" << std::endl;
    fileC.close();
    sleep(1);

    if (!std::static_pointer_cast<PluginProxy>(pluginPair->second)->HoldInstance()) {
        printf("Proxy should be instaniated.\n");
        FAIL();
    }

    sleep(15); // destroy instance after 15 seconds
    if (std::static_pointer_cast<PluginProxy>(pluginPair->second)->HoldInstance()) {
        printf("Proxy should not be instaniated.\n");
        FAIL();
    }
}