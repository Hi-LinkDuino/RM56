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

#include "freeze_detector_plugin_module_test.h"

#include <fcntl.h>
#include <fstream>
#include <mutex>

#include "file_util.h"
#include "freeze_detector_resolver.h"
#include "freeze_detector_utils.h"
#include "log_manager.h"
#include "plugin_platform.h"

namespace OHOS {
namespace HiviewDFX {
using namespace testing::ext;

void FreezeDetectorPluginModuleTest::SetUp() {}

void FreezeDetectorPluginModuleTest::TearDown()
{
}

void FreezeDetectorPluginModuleTest::SetUpTestCase()
{
    OHOS::HiviewDFX::PluginPlatform &platform = PluginPlatform::GetInstance();
    if (!platform.InitEnvironment("/data/test/test_data/hiview_platform_config")) {
        printf("Fail to init environment.\n");
    }
}

void FreezeDetectorPluginModuleTest::TearDownTestCase()
{
    std::shared_ptr<Plugin> plugin = PluginPlatform::GetInstance().GetPluginMap().find("FreezeDetectorPlugin")->second;
    if (plugin != nullptr) {
        plugin->OnUnload();
    }
}

std::shared_ptr<SysEvent> FreezeDetectorPluginModuleTest::CreateFreezeEvent(const std::string& extraInfo,
    bool fromKernel, long happenTime, const std::string& domain, const std::string& stringid) const
{
    auto event = std::make_shared<SysEvent>("SysEvent", nullptr);
    if (fromKernel) {
        event->normalExtraInfo_ = extraInfo;
    } else {
        event->jsonExtraInfo_ = extraInfo;
    }
    event->domain_ = domain;
    event->eventName_ = stringid;
    event->happenTime_ = happenTime;
    return event;
}

std::shared_ptr<FreezeDetectorPlugin> FreezeDetectorPluginModuleTest::GetFreezeDetectorPlugin()
{
    auto plugin = std::static_pointer_cast<FreezeDetectorPlugin>(PluginPlatform::GetInstance()
        .GetPluginMap().find("FreezeDetectorPlugin")->second);
    return plugin;
}

/**
 * @tc.name: OnEventTest001
 * @tc.desc: check resolver
 * @tc.type: FUNC
 * @tc.require: AR000CPN26
 */
HWTEST_F(FreezeDetectorPluginModuleTest, OnEventTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create UI_BLOCK_6S watchpoint
     * @tc.steps: step2. create FWK_BLOCK watchpoint
     * @tc.steps: step3. check resolver, FWK_BLOCK trigger UI_BLOCK_6S resolver
     */
    auto sysEventPlugin = PluginPlatform::GetInstance().GetPluginMap().find("SysEventSource")->second;
    ASSERT_NE(sysEventPlugin, nullptr);

    long startTime = time(nullptr);
    std::string json = "{\"MSG\":\"UI_BLOCK_6S\",\"PID\":1669,\"UID\":2208,"
        "\"PROCESS_NAME\":\"test.app\",\"PACKAGE_NAME\":\"test.app\"}";
    auto event = CreateFreezeEvent(json, false, startTime, "APPEXECFWK", "UI_BLOCK_6S");
    PipelineEvent::FillPipelineInfo(sysEventPlugin, "SysEventPipeline", event, true);
    ASSERT_EQ("SysEventPipeline", event->GetPipelineInfo());
    event->OnContinue();
    json = "{\"MSG\":\"FWK_BLOCK\",\"PID\":1669,\"UID\":2208,"
        "\"PROCESS_NAME\":\"test.app\",\"PACKAGE_NAME\":\"test.app\"}";
    auto eventTrigger = CreateFreezeEvent(json, false, startTime + 6, "FRAMEWORK", "FWK_BLOCK");
    PipelineEvent::FillPipelineInfo(sysEventPlugin, "SysEventPipeline", eventTrigger, true);
    ASSERT_EQ("SysEventPipeline", eventTrigger->GetPipelineInfo());
    eventTrigger->OnContinue();
}

/**
 * @tc.name: FreezeResolverNotiyActionTest001
 * @tc.desc: check resolver and action
 * @tc.type: FUNC
 * @tc.require: AR000CPN2D SR000CPN29
 */
HWTEST_F(FreezeDetectorPluginModuleTest, FreezeResolverNotiyActionTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create UI_BLOCK_3S watchpoint
     * @tc.steps: step2. create APP_NOT_RESPONSE watchpoint
     * @tc.steps: step5. check resolver, APP_NOT_RESPONSE trigger UI_BLOCK_3S resolver
     */
    auto sysEventPlugin = PluginPlatform::GetInstance().GetPluginMap().find("SysEventSource")->second;
    ASSERT_NE(sysEventPlugin, nullptr);

    long startTime = time(nullptr);
    std::string json = "{\"MSG\":\"UI_BLOCK_3S\",\"PID\":1669,\"UID\":2208,"
        "\"PROCESS_NAME\":\"test.app\",\"PACKAGE_NAME\":\"test.app\"}";
    auto event = CreateFreezeEvent(json, false, startTime, "APPEXECFWK", "UI_BLOCK_3S");
    PipelineEvent::FillPipelineInfo(sysEventPlugin, "SysEventPipeline", event, true);
    ASSERT_EQ("SysEventPipeline", event->GetPipelineInfo());
    event->OnContinue();

    json = "{\"MSG\":\"APP_NOT_RESPONSE\",\"PID\":1669,\"UID\":2208,"
        "\"PROCESS_NAME\":\"test.app\",\"PACKAGE_NAME\":\"test.app\"}";
    auto eventTrigger = CreateFreezeEvent(json, false, startTime + 6, "FRAMEWORK", "APP_NOT_RESPONSE");
    PipelineEvent::FillPipelineInfo(sysEventPlugin, "SysEventPipeline", eventTrigger, true);
    ASSERT_EQ("SysEventPipeline", eventTrigger->GetPipelineInfo());
    eventTrigger->OnContinue();
}


/**
 * @tc.name: FreezeResolverCheckIsFaultTest001
 * @tc.desc: check resolver and action
 * @tc.type: FUNC
 * @tc.require: AR000CPN24
 */
HWTEST_F(FreezeDetectorPluginModuleTest, FreezeResolverCheckIsFaultTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create TRANSPARENT_WINDOWS watchpoint
     * @tc.steps: step5. check resolver, TRANSPARENT_WINDOWS resolver
     */
    auto sysEventPlugin = PluginPlatform::GetInstance().GetPluginMap().find("SysEventSource")->second;
    ASSERT_NE(sysEventPlugin, nullptr);

    long startTime = time(nullptr);
    std::string json = "{\"MSG\":\"TRANSPARENT_WINDOWS, isFault = true\",\"PID\":1669,\"UID\":2208,"
        "\"PROCESS_NAME\":\"test.app\",\"PACKAGE_NAME\":\"test.app\"}";
    auto event = CreateFreezeEvent(json, false, startTime, "FRAMEWORK", "TRANSPARENT_WINDOWS");
    PipelineEvent::FillPipelineInfo(sysEventPlugin, "SysEventPipeline", event, true);
    ASSERT_EQ("SysEventPipeline", event->GetPipelineInfo());
    event->OnContinue();
}

/**
 * @tc.name: FreezeResolverBackKeyTest001
 * @tc.desc: check resolver and action
 * @tc.type: FUNC
 * @tc.require: AR000CPN2E
 */
HWTEST_F(FreezeDetectorPluginModuleTest, FreezeResolverBackKeyTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create UI_BLOCK_6S watchpoint
     * @tc.steps: step5. check resolver, BACK_KEY resolver
     */
    auto sysEventPlugin = PluginPlatform::GetInstance().GetPluginMap().find("SysEventSource")->second;
    ASSERT_NE(sysEventPlugin, nullptr);

    long startTime = time(nullptr);
    std::string json = "{\"MSG\":\"UI_BLOCK_6S\",\"PID\":1669,\"UID\":2208,"
        "\"PROCESS_NAME\":\"test.app\",\"PACKAGE_NAME\":\"test.app\"}";
    auto event = CreateFreezeEvent(json, false, startTime, "APPEXECFWK", "UI_BLOCK_6S");
    PipelineEvent::FillPipelineInfo(sysEventPlugin, "SysEventPipeline", event, true);
    ASSERT_EQ("SysEventPipeline", event->GetPipelineInfo());
    event->OnContinue();

    auto triggerEvent = CreateFreezeEvent("", false, startTime + 1, "KEY_PRESS", "BACK_KEY");
    PipelineEvent::FillPipelineInfo(sysEventPlugin, "SysEventPipeline", triggerEvent, true);
    ASSERT_EQ("SysEventPipeline", triggerEvent->GetPipelineInfo());
    riggerEvent->OnContinue();
}
}
}
