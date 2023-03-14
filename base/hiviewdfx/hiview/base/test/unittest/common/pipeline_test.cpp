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
#include "pipeline_test.h"

#include <fstream>
#include <iostream>

#include "event_loop.h"
#include "event_source.h"
#include "event_source_example.h"
#include "pipeline.h"
#include "plugin.h"
#include "plugin_factory.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
void PipelineTest::BindWorkLoop(std::shared_ptr<OHOS::HiviewDFX::Plugin> plugin)
{
    if (plugin != nullptr && plugin->GetWorkLoop() == nullptr) {
        auto eventLoop = std::make_shared<EventLoop>(plugin->GetName());
        if (eventLoop != nullptr) {
            eventLoop->StartLoop();
            plugin->BindWorkLoop(eventLoop);
        }
    }
}

void PipelineTest::DoTest(const std::string& name, bool stopBeforeEnd, bool withThread)
{
    /**
     * @tc.steps: step1. create plugins
     */
    printf("PipelineTest. DoTest:%s.\n", name.c_str());
    auto plugin1 = PluginFactory::GetPlugin("EventProcessorExample1");
    plugin1->SetName("EventProcessorExample1");
    auto plugin2 = PluginFactory::GetPlugin("EventProcessorExample2");
    plugin2->SetName("EventProcessorExample2");
    auto plugin3 = PluginFactory::GetPlugin("EventProcessorExample3");
    plugin3->SetName("EventProcessorExample3");
    auto plugin4 = PluginFactory::GetPlugin("EventProcessorExample4");
    plugin4->SetName("EventProcessorExample4");
    if (withThread) {
        BindWorkLoop(plugin1);
        BindWorkLoop(plugin2);
        BindWorkLoop(plugin3);
        BindWorkLoop(plugin4);
    }

    std::list<std::weak_ptr<Plugin>> pluginList;
    pluginList.push_back(plugin1);
    pluginList.push_back(plugin2);
    pluginList.push_back(plugin3);
    pluginList.push_back(plugin4);

    /**
     * @tc.steps: step1. create pipeline
     */
    auto pipeline = std::make_shared<Pipeline>("PipelineTest", pluginList);

    /**
     * @tc.steps: step2. create pipeline event
     */
    auto producer = std::make_shared<PipelineEventProducerTest>();
    auto event = std::make_shared<PipelineEvent>(name, producer.get());
    event->messageType_ = Event::MessageType::FAULT_EVENT;
    event->eventId_ = EventSourceExample::PIPELINE_EVENT_ID_AAA;
    if (stopBeforeEnd) {
        event->SetValue("Finish", "EventProcessorExample2");
    }

    /**
     * @tc.steps: step3. process pipeline event
     */
    if (pipeline->CanProcessEvent(event)) {
        pipeline->ProcessEvent(event);
    }

    const int sleepTime = 5;
    sleep(sleepTime);

    /**
     * @tc.steps: step4. check whether event has been processed
     */
    ASSERT_EQ(event->GetValue("EventProcessorExample1"), "Done");
    ASSERT_EQ(event->GetValue("EventProcessorExample2"), "Done");
    if (stopBeforeEnd) {
        ASSERT_EQ(event->GetValue("EventProcessorExample3"), "");
        ASSERT_EQ(event->GetValue("EventProcessorExample4"), "");
    }

    plugin1->OnUnload();
    plugin2->OnUnload();
    plugin3->OnUnload();
    plugin4->OnUnload();
}

/**
 * @tc.name: PluginPipelineCreateTest001
 * @tc.desc: create pipeline with multiple plugins
 * @tc.type: FUNC
 * @tc.require: AR000DPTT3
 */
HWTEST_F(PipelineTest, PluginPipelineCreateTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create pipeline
     * @tc.steps: step2. create pipeline event
     * @tc.steps: step3. process pipeline event
     * @tc.steps: step4. check whether event has been processed
     */
    DoTest("event0", false, true);
}

/**
 * @tc.name: PluginPipelineCreateTest002
 * @tc.desc: create pipeline with multiple plugins
 * @tc.type: FUNC
 * @tc.require: AR000DPTT3
 */
HWTEST_F(PipelineTest, PluginPipelineCreateTest002, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create pipeline
     * @tc.steps: step2. create pipeline event
     * @tc.steps: step3. process pipeline event
     * @tc.steps: step4. check whether event has been processed
     */
    DoTest("event1", true, true);
}

/**
 * @tc.name: PluginPipelineCreateTest003
 * @tc.desc: create pipeline with multiple plugins
 * @tc.type: FUNC
 * @tc.require: AR000DPTT3
 */
HWTEST_F(PipelineTest, PluginPipelineCreateTest003, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create pipeline
     * @tc.steps: step2. create pipeline event
     * @tc.steps: step3. process pipeline event
     * @tc.steps: step4. check whether event has been processed
     */
    DoTest("event0", false, false);
}

/**
 * @tc.name: PluginPipelineCreateTest004
 * @tc.desc: create pipeline with multiple plugins
 * @tc.type: FUNC
 * @tc.require: AR000DPTT3
 */
HWTEST_F(PipelineTest, PluginPipelineCreateTest004, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create pipeline
     * @tc.steps: step2. create pipeline event
     * @tc.steps: step3. process pipeline event
     * @tc.steps: step4. check whether event has been processed
     */
    DoTest("event1", true, false);
}
