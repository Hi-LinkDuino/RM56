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
#include "holistic_platform_test.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <unistd.h>

#include "event.h"
#include "file_util.h"

#include "bundle_event_source_example.h"
#include "event_source_example.h"
using namespace testing::ext;
using namespace OHOS::HiviewDFX;

void HolisticPlatformTest::SetUp()
{
    /**
     * @tc.setup: create work directories
     */
    printf("SetUp.\n");
}
OHOS::HiviewDFX::HiviewPlatform HolisticPlatformTest::platform_;
std::set<std::string>* HolisticPlatformTest::count1_;
std::set<std::string>* HolisticPlatformTest::count2_;
void HolisticPlatformTest::SetUpTestCase()
{
    printf("SetUpTestCase.\n");
    FileUtil::ForceCreateDirectory("/data/test/faultlog/");
    FileUtil::ForceCreateDirectory("/data/test/faultlog2/");

    const time_t IDLE_TIME = 10; // 10 seconds
    const time_t CHECK_TIME = IDLE_TIME / 2; // 5 seconds
    platform_.SetMaxProxyIdleTime(IDLE_TIME);
    platform_.SetCheckProxyIdlePeriod(CHECK_TIME);
    if (!platform_.InitEnvironment("/data/test/test_data/hiview_platform_config")) {
        printf("Fail to init environment. \n");
        FAIL();
    }
    auto& pluginMap = platform_.GetPluginMap();
    for (auto& i : pluginMap) {
        printf("name:%s, \n", i.first.c_str());
    }

    auto itaa = pluginMap.find("EventSourceExample");
    if (itaa == pluginMap.end()) {
        FAIL();
    }
    auto eventSourceExample = std::static_pointer_cast<EventSourceExample>(itaa->second);

    auto itbb = pluginMap.find("BundleEventSourceExample");
    if (itbb == pluginMap.end()) {
        FAIL();
    }
    auto bundleEventSourceExample = std::static_pointer_cast<BundleEventSourceExample>(itbb->second);
    count1_ = &(eventSourceExample->count);
    count2_ = &(bundleEventSourceExample->count);
}

void HolisticPlatformTest::TearDownTestCase()
{
    printf("TearDownTestCase.\n");
}

void HolisticPlatformTest::TearDown()
{
    printf("TearDown.\n");
}

/**
 * @tc.name: HolisticPlatformLoadingPlugins001
 * @tc.desc: 平台插件与插件包一起加载，动态卸载查询插件数目
 * @tc.type: FUNC
 * @tc.require: AR000GICT3
 */
HWTEST_F(HolisticPlatformTest, HolisticPlatformLoadingPlugins001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. init plugin platform
     */
    printf("HolisticPlatformLoadingPlugins001.\n");
    auto& pluginMap = platform_.GetPluginMap();
    for (auto& i : pluginMap) {
        printf("name:%s, \n", i.first.c_str());
    }

    auto pluginSize = pluginMap.size();
    ASSERT_EQ(pluginSize, 11ul);

    for (auto& i : *count1_) {
        printf("1eventSourceExample name:%s, \n", i.c_str());
    }
    ASSERT_EQ(count1_->size(), 4ul);
    for (auto& i : *count2_) {
        printf("1bundleEventSourceExample name:%s, \n", i.c_str());
    }
    ASSERT_EQ(count2_->size(), 3ul);
    auto ita1 = count1_->find("EventProcessorExample3");
    if (ita1 != count1_->end()) {
        FAIL();
    }
    auto itb2 = count1_->find("EventProcessorExample4");
    if (itb2 == count1_->end()) {
        FAIL();
    }
    auto itc3 = count2_->find("BundlePluginExample2");
    if (itc3 == count2_->end()) {
        FAIL();
    }
    // 等时间动态插件卸载
    sleep(12);

    for (auto& i : *count1_) {
        printf("2eventSourceExample name:%s, \n", i.c_str());
    }
    ASSERT_EQ(count1_->size(), 3ul);
    for (auto& i : *count2_) {
        printf("2bundleEventSourceExample name:%s, \n", i.c_str());
    }
    ASSERT_EQ(count2_->size(), 1ul);
    ita1 = count1_->find("EventProcessorExample3");
    if (ita1 != count1_->end()) {
        FAIL();
    }
    itb2 = count1_->find("EventProcessorExample4");
    if (itb2 != count1_->end()) {
        FAIL();
    }
    itc3 = count2_->find("BundlePluginExample2");
    if (itc3 != count2_->end()) {
        FAIL();
    }
}

void HolisticPlatformTest::LoadingPlugins002Check()
{
    ASSERT_EQ((count1_->size() + count2_->size()), 4ul);
    auto ita = count1_->find("EventProcessorExample3");
    if (ita != count1_->end()) {
        FAIL();
    }
    auto itb = count1_->find("EventProcessorExample4");
    if (itb != count1_->end()) {
        FAIL();
    }

    auto itc = count2_->find("BundlePluginExample2");
    if (itc != count2_->end()) {
        FAIL();
    }
}

/**
 * @tc.name: HolisticPlatformLoadingPlugins002
 * @tc.desc: 平台插件与插件包一起加载，运行加载查询插件数目
 * @tc.type: FUNC
 * @tc.require: AR000GICT3
 */
HWTEST_F(HolisticPlatformTest, HolisticPlatformLoadingPlugins002, TestSize.Level3)
{
    printf("HolisticPlatformLoadingPlugins002.\n");
    auto& pluginMap = platform_.GetPluginMap();
    for (auto& i : pluginMap) {
        printf("name:%s, \n", i.first.c_str());
    }
    ASSERT_EQ(pluginMap.size(), 11ul);

    const std::map<std::string, std::shared_ptr<Pipeline>>& pipelines = platform_.GetPipelineMap();
    auto it = pipelines.find("NormalPipeline");
    if (it == pipelines.end()) {
        printf("Fail to find pipeline with name :NormalPipeline");
        FAIL();
    }
    ASSERT_EQ(it->second->GetProcessSequence().size(), 5ul);
    for (auto& ita : it->second->GetProcessSequence()) {
        auto ptr = ita.lock();
        if (ptr != nullptr) {
            printf("pipelines : %s\n", ptr->GetName().c_str());
        } else {
            printf("ptr != nullptr\n");
        }
    }

    LoadingPlugins002Check();

    // 发生事件，启动流水线
    std::ofstream fileA("/data/test/faultlog/testbb");
    fileA << "create testbb event" << std::endl;
    fileA.close();
    sleep(3);
    ASSERT_EQ(count1_->size(), 5ul);
    auto ita = count1_->find("EventProcessorExample3");
    if (ita == count1_->end()) {
        FAIL();
    }
    auto itb = count1_->find("EventProcessorExample4");
    if (itb == count1_->end()) {
        FAIL();
    }

    ASSERT_EQ(count2_->size(), 2ul);
    auto itc = count2_->find("BundlePluginExample2");
    if (itc == count2_->end()) {
        FAIL();
    }
}

/**
 * @tc.name: HolisticPlatformLoadingPlugins003
 * @tc.desc: 平台插件与插件包一起加载，运行加载查询插件数目
 * @tc.type: FUNC
 * @tc.require: AR000GICT3
 */
HWTEST_F(HolisticPlatformTest, HolisticPlatformLoadingPlugins003, TestSize.Level3)
{
    printf("HolisticPlatformLoadingPlugins003.\n");

    auto& pluginMap = platform_.GetPluginMap();
    for (auto& i : pluginMap) {
        printf("name:%s, \n", i.first.c_str());
    }

    auto pluginSize = pluginMap.size();
    ASSERT_EQ(pluginSize, 11ul);
    sleep(12); // 等待动态插件被卸载
    auto count = count1_->size() + count2_->size();
    ASSERT_EQ(count, 4ul);
    auto itb = count1_->find("EventProcessorExample4");
    if (itb != count1_->end()) {
        FAIL();
    }

    auto itc = count2_->find("BundlePluginExample3");
    if (itc != count2_->end()) {
        FAIL();
    }

    // 发生监听事件
    std::shared_ptr<Event> event = std::make_shared<Event>("HolisticPlatformLoadingPlugins003", "testaa");
    event->messageType_ = OHOS::HiviewDFX::Event::MessageType::SYS_EVENT;
    event->SetValue("HolisticPlatformLoadingPlugins003", "SYS_EVENT testaa");
    platform_.PostUnorderedEvent(nullptr, event);
    sleep(3);
    ASSERT_EQ(count1_->size(), 4ul);
    itb = count1_->find("EventProcessorExample4");
    if (itb == count1_->end()) {
        FAIL();
    }

    ASSERT_EQ(count2_->size(), 2ul);
    itc = count2_->find("BundlePluginExample3");
    if (itc == count2_->end()) {
        FAIL();
    }
}

void HolisticPlatformTest::Run001Check01()
{
    std::string be3t = platform_.GetHiviewProperty("BE3_Listening", "");
    printf("be3t %s\n", be3t.c_str());
    
    std::string be4t = platform_.GetHiviewProperty("EPE4_Listening", "");
    printf("be4t %s\n", be4t.c_str());
    
    std::string dpet = platform_.GetHiviewProperty("DPE_Listening", "");
    printf("dpet %s\n", dpet.c_str());
    
    std::string be2t = platform_.GetHiviewProperty("EPE2_Listening", "");
    printf("be2t %s\n", be2t.c_str());
    if (be3t.find("testbb") == be3t.npos) {
        FAIL();
    }
    if (be4t.find("testbb") == be4t.npos) {
        FAIL();
    }
    if (dpet.find("testbb") == dpet.npos) {
        FAIL();
    }
    if (be2t.find("testbb") != be2t.npos) {
        FAIL();
    }
}

void HolisticPlatformTest::Run001Check02()
{
    std::string be3tt = platform_.GetHiviewProperty("BE3_Listening", "");
    printf("be3tt %s\n", be3tt.c_str());
    if (be3tt.find("testRun001") == be3tt.npos) {
        FAIL();
    }
    std::string be2tt = platform_.GetHiviewProperty("EPE2_Listening", "");
    printf("be2tt %s\n", be2tt.c_str());
    if (be2tt.find("testRun001") == be2tt.npos) {
        FAIL();
    }
    std::string dpett = platform_.GetHiviewProperty("DPE_Listening", "");
    printf("dpett %s\n", dpett.c_str());
    if (dpett.find("testRun001") == dpett.npos) {
        FAIL();
    }
    std::string be4tt = platform_.GetHiviewProperty("EPE4_Listening", "");
    printf("be4tt %s\n", be4tt.c_str());
    if (be4tt.find("testRun001") != be4tt.npos) {
        FAIL();
    }
}
/**
 * @tc.name: HolisticPlatformRun001
 * @tc.desc: 插件平台订阅运行检查
 * @tc.type: FUNC
 * @tc.require: AR000GICT3
 */
HWTEST_F(HolisticPlatformTest, HolisticPlatformRun001, TestSize.Level3)
{
    printf("HolisticPlatformRun001.\n");

    sleep(12); // 等待动态插件被卸载
    auto count = count1_->size() + count2_->size();
    ASSERT_EQ(count, 4ul);
    std::string be3 = platform_.GetHiviewProperty("BE3_Listening", "");
    if ((be3.find("testbb") != be3.npos) || (be3.find("testRun001") != be3.npos)) {
        FAIL();
    }

    std::string epe4 = platform_.GetHiviewProperty("EPE4_Listening", "");
    if ((epe4.find("testbb") != epe4.npos) || (epe4.find("testRun001") != epe4.npos)) {
        FAIL();
    }

    std::string epe2 = platform_.GetHiviewProperty("EPE2_Listening", "");
    if ((epe2.find("testbb") != epe2.npos) || (epe2.find("testRun001") != epe2.npos)) {
        FAIL();
    }

    std::string dpe = platform_.GetHiviewProperty("DPE_Listening", "");
    if ((dpe.find("testbb") != dpe.npos) || (dpe.find("testRun001") != dpe.npos)) {
        FAIL();
    }

    // 发生监听事件1
    std::shared_ptr<Event> event1 = std::make_shared<Event>("HolisticPlatformRun001", "testbb");
    event1->messageType_ = OHOS::HiviewDFX::Event::MessageType::SYS_EVENT;
    event1->SetValue("HolisticPlatformLoadingPlugins003", "SYS_EVENT testaa");
    platform_.PostUnorderedEvent(nullptr, event1);
    sleep(3);
    Run001Check01();
    sleep(1);

    // 发生监听事件2
    std::shared_ptr<Event> event2 = std::make_shared<Event>("HolisticPlatformRun001", "testRun001");
    event2->messageType_ = OHOS::HiviewDFX::Event::MessageType::RAW_EVENT;
    const int EVENT_ID_1 = 901000111;
    event2->eventId_ = EVENT_ID_1;
    event2->SetValue("HolisticPlatformLoadingPlugins003", "SYS_EVENT testRun001");
    platform_.PostUnorderedEvent(nullptr, event2);
    sleep(3);
    Run001Check02();
    sleep(1);
}

void HolisticPlatformTest::Run002Check01()
{
    int count = count1_->size() + count2_->size();
    ASSERT_EQ(count, 4ul);
    std::string be1 = platform_.GetHiviewProperty("BE1_OnEvent", "");
    if (be1.find("testbbbb") != be1.npos) {
        FAIL();
    }
    std::string be2 = platform_.GetHiviewProperty("BE2_OnEvent", "");
    if ((be2.find("testbbbb") != be2.npos) || (be2.find("testcc") != be2.npos)) {
        FAIL();
    }

    std::string epe1 = platform_.GetHiviewProperty("EPE1_OnEvent", "");
    if (epe1.find("testcc") != epe1.npos) {
        FAIL();
    }

    std::string epe2 = platform_.GetHiviewProperty("EPE2_OnEvent", "");
    if (epe2.find("testcc") != epe2.npos) {
        FAIL();
    }

    std::string epe3 = platform_.GetHiviewProperty("EPE3_OnEvent", "");
    if (epe3.find("testcc") != epe3.npos) {
        FAIL();
    }

    std::string epe4 = platform_.GetHiviewProperty("EPE4_OnEvent", "");
    if (epe4.find("testcc") != epe4.npos) {
        FAIL();
    }
}

void HolisticPlatformTest::Run002Check02()
{
    std::string epe1t = platform_.GetHiviewProperty("EPE1_OnEvent", "");
    printf("epe1t %s\n", epe1t.c_str());
    if (epe1t.find("testcc") == epe1t.npos) {
        FAIL();
    }

    std::string epe2t = platform_.GetHiviewProperty("EPE2_OnEvent", "");
    printf("epe2t %s\n", epe2t.c_str());
    if (epe2t.find("testcc") == epe2t.npos) {
        FAIL();
    }

    std::string epe3t = platform_.GetHiviewProperty("EPE3_OnEvent", "");
    printf("epe3t %s\n", epe3t.c_str());
    if (epe3t.find("testcc") == epe3t.npos) {
        FAIL();
    }

    std::string epe4t = platform_.GetHiviewProperty("EPE4_OnEvent", "");
    printf("epe4t %s\n", epe4t.c_str());
    if (epe4t.find("testcc") == epe4t.npos) {
        FAIL();
    }
    
    std::string be2t = platform_.GetHiviewProperty("BE2_OnEvent", "");
    printf("be2t %s\n", be2t.c_str());
    if (be2t.find("testcc") == be2t.npos) {
        FAIL();
    }
}
/**
 * @tc.name: HolisticPlatformRun002
 * @tc.desc: 插件平台流水线运行检查
 * @tc.type: FUNC
 * @tc.require: AR000GICT3
 */
HWTEST_F(HolisticPlatformTest, HolisticPlatformRun002, TestSize.Level3)
{
    printf("HolisticPlatformRun002.\n");

    sleep(12); // 等待动态插件被卸载
    Run002Check01();

    // 发生事件1，启动流水线
    std::ofstream fileC("/data/test/faultlog/testcc");
    fileC << "create testcc event" << std::endl;
    fileC.close();
    sleep(2);
    Run002Check02();
    sleep(12); // 等待动态插件被卸载

    // 发生事件2，启动流水线
    std::ofstream fileB("/data/test/faultlog2/testbb");
    fileB << "create testbbbb event" << std::endl;
    fileB.close();
    sleep(2);
    std::string be1tt = platform_.GetHiviewProperty("BE1_OnEvent", "");
    printf("be1tt %s\n", be1tt.c_str());
    if (be1tt.find("testbbbb") == be1tt.npos) {
        FAIL();
    }
    std::string be2tt = platform_.GetHiviewProperty("BE2_OnEvent", "");
    printf("be2tt %s\n", be2tt.c_str());
    if (be2tt.find("testbbbb") == be2tt.npos) {
        FAIL();
    }
    sleep(1);
}