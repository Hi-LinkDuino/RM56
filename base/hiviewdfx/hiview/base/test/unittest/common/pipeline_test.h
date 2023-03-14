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
#ifndef HIVIEW_PLATFORM_PLUGIN_PIPELINE_TEST_H
#define HIVIEW_PLATFORM_PLUGIN_PIPELINE_TEST_H
#include <list>
#include <memory>

#include <gtest/gtest.h>

#include "defines.h"
#include "pipeline.h"
#include "plugin.h"

class PipelineTest : public testing::Test {
public:
    void SetUp() {};
    void TearDown() {};
    void DoTest(const std::string& name, bool stopBeforeEnd, bool withThread);
    void BindWorkLoop(std::shared_ptr<OHOS::HiviewDFX::Plugin> plugin);
};

class PipelineEventProducerTest : public OHOS::HiviewDFX::PipelineEventProducer {
public:
    PipelineEventProducerTest() {};
    virtual ~PipelineEventProducerTest() {};
    void Recycle(OHOS::HiviewDFX::PipelineEvent* event __UNUSED) override
    {
        printf("PipelineEventProducerTest Recycle.\n");
    }

    void PauseDispatch(std::weak_ptr<OHOS::HiviewDFX::Plugin> plugin __UNUSED) override
    {
        printf("PipelineEventProducerTest PauseDispatch.\n");
    }
};
#endif // HIVIEW_PLATFORM_PLUGIN_PIPELINE_TEST_H