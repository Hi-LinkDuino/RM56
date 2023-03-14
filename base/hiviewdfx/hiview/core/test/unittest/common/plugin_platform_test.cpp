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
#include "plugin_platform_test.h"

#include <fstream>
#include <iostream>
#include <set>
#include <unistd.h>

#include "event.h"
#include "file_util.h"
#include "hiview_platform.h"

#include "platform_test_result_listener.h"
using namespace testing::ext;
using namespace OHOS::HiviewDFX;

void PluginPlatformTest::SetUp()
{
    /**
     * @tc.setup: create work directories
     */
    printf("SetUp.\n");
    FileUtil::ForceCreateDirectory("/data/test/faultlog");
}

/**
 * @tc.name: PluginPlatformInit001
 * @tc.desc: send multiple events to event loop
 * @tc.type: FUNC
 * @tc.require: AR000DPTT4
 */
HWTEST_F(PluginPlatformTest, PluginPlatformInit001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. init plugin platform
     */
    printf("PluginPlatformTest.\n");
    OHOS::HiviewDFX::HiviewPlatform platform;
    const int argc = 2;
    char arg0[] = "/system/bin/hiview";
    char arg1[] = "-a";
    char* argv[argc] = {arg0, arg1};
    platform.ProcessArgsRequest(argc, argv);

    if (!platform.InitEnvironment("/data/test/test_data/hiview_platform_config")) {
        printf("Fail to init environment. \n");
    }
    auto listener = std::make_shared<PlatformTestResultListener>("testListener");
    platform.RegisterUnorderedEventListener(listener);
    listener->AddListenerInfo(Event::MessageType::FAULT_EVENT, EventListener::EventIdRange(901000000, 901004006));
    sleep(1);
    std::ofstream fileA("/data/test/faultlog/aaa");
    fileA << "create aaa event" << std::endl;
    fileA.close();

    std::ofstream fileB("/data/test/faultlog/bbb");
    fileB << "create bbb event" << std::endl;
    fileB.close();
    sleep(1);

    EXPECT_EQ(listener->unorderEventCount_, 2ul);

    auto& pluginMap= platform.GetPluginMap();
    auto pluginSize = pluginMap.size();
    EXPECT_EQ(pluginSize, 6ul);

    auto& pipelineMap = platform.GetPipelineMap();
    auto pipelineSize = pipelineMap.size();
    EXPECT_EQ(pipelineSize, 3ul);

    auto workLoopMap = platform.GetWorkLoopMap();
    EXPECT_EQ(workLoopMap.size(), 5ul);

    for (auto& entry : workLoopMap) {
        EXPECT_EQ(true, entry.second->IsRunning());
    }

    auto sharedWorkLoop = platform.GetSharedWorkLoop();
    EXPECT_NE(sharedWorkLoop, nullptr);
    EXPECT_EQ(true, sharedWorkLoop->IsRunning());

    auto baseDir = platform.GetHiViewDirectory(HiviewContext::DirectoryType::CONFIG_DIRECTORY);
    EXPECT_EQ("/data/test/test_data/", baseDir);
    auto cloudDir = platform.GetHiViewDirectory(HiviewContext::DirectoryType::CLOUD_UPDATE_DIRECTORY);
    EXPECT_EQ("/data/system/hiview/", cloudDir);
    auto workDir = platform.GetHiViewDirectory(HiviewContext::DirectoryType::WORK_DIRECTORY);

    auto persistDir = platform.GetHiViewDirectory(HiviewContext::DirectoryType::PERSIST_DIR);
    EXPECT_EQ("/log/hiview/", persistDir);
}

/**
 * @tc.name: PluginPlatformRepackPipelienEventTest001
 * @tc.desc: send multiple events to event loop
 * @tc.type: FUNC
 * @tc.require: AR000DPTT4
 */
HWTEST_F(PluginPlatformTest, PluginPlatformRepackPipelienEventTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. plugin platform
     */
    printf("PluginPlatformTest2.\n");
    OHOS::HiviewDFX::HiviewPlatform platform;
    if (!platform.InitEnvironment("/data/test/test_data/hiview_platform_config")) {
        printf("Fail to init environment. \n");
    }

    auto listener = std::make_shared<PlatformTestResultListener>("testListener");
    platform.RegisterUnorderedEventListener(listener);
    listener->AddListenerInfo(Event::MessageType::FAULT_EVENT, EventListener::EventIdRange(901000000, 901000002));
    sleep(1);
    std::ofstream fileA("/data/test/faultlog/aaa");
    fileA << "create aaa event" << std::endl;
    fileA.close();

    std::ofstream fileB("/data/test/faultlog/bbb");
    fileB << "create bbb event" << std::endl;
    fileB.close();

    std::ofstream fileC("/data/test/faultlog/ccc");
    fileC << "create ccc event" << std::endl;
    fileC.close();
    sleep(1);
    ASSERT_EQ(listener->unorderEventCount_, 3ul);
}

/**
 * @tc.name: PluginPlatformPluginUnloadTest001
 * @tc.desc: request unload plugin with name
 * @tc.type: FUNC
 * @tc.require: AR000DPTT4
 */
HWTEST_F(PluginPlatformTest, PluginPlatformPluginUnloadTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. plugin platform
     */
    printf("PluginPlatformTest2.\n");
    OHOS::HiviewDFX::HiviewPlatform& platform = HiviewPlatform::GetInstance();
    if (!platform.InitEnvironment("/data/test/test_data/hiview_platform_config")) {
        printf("Fail to init environment. \n");
    }
    auto listener = std::make_shared<PlatformTestResultListener>("testListener");
    platform.RegisterUnorderedEventListener(listener);
    listener->AddListenerInfo(Event::MessageType::FAULT_EVENT, EventListener::EventIdRange(901000000, 901000002));
    sleep(1);
    ASSERT_EQ(true, platform.IsReady());
    auto& pluginList = platform.GetPluginMap();
    auto size = pluginList.size();
    ASSERT_EQ(size, 6ul);

    // the example4 plugin will be unloaded after receiving the ccc message
    std::ofstream fileC("/data/test/faultlog/ccc");
    fileC << "create ccc event" << std::endl;
    fileC.close();
    sleep(2);

    auto& pluginList2 = platform.GetPluginMap();
    auto size2 = pluginList2.size();
    ASSERT_EQ(size2, 5ul);

    std::ofstream fileC2("/data/test/faultlog/ccc");
    fileC2 << "create ccc event" << std::endl;
    fileC2.close();
    sleep(2);
    ASSERT_EQ(listener->unorderEventCount_, 1ul);
}

/**
 * @tc.name: PluginPlatformPluginPendLoadTest001
 * @tc.desc: test pend loading plugin feature
 * @tc.type: FUNC
 * @tc.require: AR000DPTT4
 */
HWTEST_F(PluginPlatformTest, PluginPlatformPluginPendLoadTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. init plugin platform
     */
    printf("PluginPlatformTest2.\n");
    OHOS::HiviewDFX::HiviewPlatform platform;
    if (!platform.InitEnvironment("/data/test/test_data/hiview_platform_config1")) {
        printf("Fail to init environment. \n");
    }

    sleep(1);
    /**
     * @tc.steps: step2. check current loaded plugin number
     */
    ASSERT_EQ(true, platform.IsReady());
    auto& pluginList = platform.GetPluginMap();
    auto size = pluginList.size();
    ASSERT_EQ(size, 4ul);

    sleep(5);
    /**
     * @tc.steps: step3. check final loaded plugin number
     */
    auto& pluginList2 = platform.GetPluginMap();
    auto size2 = pluginList2.size();
    ASSERT_EQ(size2, 5ul);
}

/**
 * @tc.name: PluginPlatformPluginSyncEventCallTest001
 * @tc.desc: test pend loading plugin feature
 * @tc.type: FUNC
 * @tc.require: AR000DPTT4
 */
HWTEST_F(PluginPlatformTest, PluginPlatformPluginSyncEventCallTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. init plugin platform
     */
    printf("PluginPlatformTest2.\n");
    OHOS::HiviewDFX::HiviewPlatform platform;
    if (!platform.InitEnvironment("/data/test/test_data/hiview_platform_config")) {
        printf("Fail to init environment. \n");
    }
    auto listener = std::make_shared<PlatformTestResultListener>("testListener");
    platform.RegisterUnorderedEventListener(listener);
    std::set<EventListener::EventIdRange> set;
    set.emplace(EventListener::EventIdRange(901000000));
    set.emplace(EventListener::EventIdRange(901000001));
    set.emplace(EventListener::EventIdRange(901000002));
    listener->AddListenerInfo(Event::MessageType::FAULT_EVENT, set);
    sleep(1);
    ASSERT_EQ(true, platform.IsReady());
    auto& pluginList = platform.GetPluginMap();
    auto size = pluginList.size();
    ASSERT_EQ(size, 6ul);

    std::shared_ptr<Plugin> plugin = nullptr;
    auto pos = pluginList.find("EventProcessorExample1");
    if (pos == pluginList.end()) {
        FAIL();
    } else {
        plugin = pos->second;
    }
    ASSERT_NE(plugin, nullptr);

    auto event = plugin->GetEvent(Event::MessageType::FAULT_EVENT);
    event->eventId_ = 901000002;
    bool ret = platform.PostSyncEventToTarget(plugin, "EventProcessorExample4", event);
    ASSERT_EQ(ret, true);
    auto str = event->GetValue("EventProcessorExample4");
    ASSERT_EQ(str, "Done");
    sleep(1);
    ASSERT_EQ(listener->unorderEventCount_, 1ul);
    platform.PostAsyncEventToTarget(plugin, "EventProcessorExample4", event);
    sleep(1);
    ASSERT_EQ(listener->unorderEventCount_, 2ul);

    auto pipeEvent = std::make_shared<PipelineEvent>("test", nullptr);
    PipelineEvent::FillPipelineInfo(plugin, "NormalPipeline", pipeEvent, true);
    ASSERT_EQ("NormalPipeline", pipeEvent->GetPipelineInfo());
}

/**
 * @tc.name: PluginPlatformServiceStartTest001
 * @tc.desc: start fault detect service
 * @tc.type: FUNC
 * @tc.require: AR000DPTT4
 */
HWTEST_F(PluginPlatformTest, PluginPlatformServiceStartTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. init plugin platform
     */
    printf("PluginPlatformTest2.\n");
    OHOS::HiviewDFX::HiviewPlatform platform;
    printf("PluginPlatformServiceStartTest001. called\n");
    if (!platform.InitEnvironment()) {
        printf("Fail to init environment. \n");
    }
    sleep(1);
    printf("PluginPlatformServiceStartTest001. end\n");
    ASSERT_EQ(true, platform.IsReady());
}

#ifndef TEST_LOCAL_SRC
/**
 * @tc.name: PluginPlatformDynamicPluginUnloadTest001
 * @tc.desc: test pend loading plugin feature
 * @tc.type: FUNC
 * @tc.require: AR000DPTT4
 */
HWTEST_F(PluginPlatformTest, PluginPlatformDynamicPluginUnloadTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. init plugin platform
     */
    printf("PluginPlatformTest2.\n");
    OHOS::HiviewDFX::HiviewPlatform platform;
    if (!platform.InitEnvironment("/data/test/test_data/hiview_platform_config")) {
        printf("Fail to init environment. \n");
    }

    if (access("/system/lib64/libdynamicloadpluginexample.z.so", F_OK) != 0) {
        printf("dynamic plugin has not been installed.\n");
        return;
    }

    sleep(1);
    ASSERT_EQ(true, platform.IsReady());
    auto& pluginList = platform.GetPluginMap();
    auto size = pluginList.size();
    ASSERT_EQ(size, 6ul);

    std::shared_ptr<Plugin> plugin = nullptr;
    auto pos = pluginList.find("EventProcessorExample1");
    if (pos == pluginList.end()) {
        FAIL();
    } else {
        plugin = pos->second;
    }
    ASSERT_NE(plugin, nullptr);

    auto event = plugin->GetEvent(Event::MessageType::FAULT_EVENT);
    event->eventId_ = 901000002;
    bool ret = platform.PostSyncEventToTarget(plugin, "DynamicLoadPluginExample", event);
    ASSERT_EQ(ret, true);
    auto str = event->GetValue("DynamicLoadPluginExample");
    printf("event %p  str:%s \n", event.get(), str.c_str());
    ASSERT_EQ(str, "Done");
    auto unloadEvent = plugin->GetEvent(Event::MessageType::PLUGIN_MAINTENANCE);
    unloadEvent->SetValue("DynamicLoadPluginExample", "Unload");
    platform.PostUnorderedEvent(plugin, unloadEvent);
    sleep(3);
    size = pluginList.size();
    ASSERT_EQ(size, 5ul);
}
#endif
