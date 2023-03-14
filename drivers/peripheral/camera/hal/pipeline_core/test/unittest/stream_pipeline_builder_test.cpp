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

#include <thread>
#include <unistd.h>
#include <vector>
#include <gtest/gtest.h>
#include "stream_pipeline_strategy.h"
#include "stream_pipeline_builder.h"

using namespace testing::ext;
namespace OHOS::Camera {
class BuilderTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);

    void SetUp(void);
    void TearDown(void);
protected:
    std::shared_ptr<PipelineSpec> spec_ = nullptr;
};

void BuilderTest::SetUpTestCase(void)
{
    std::cout << "Camera::Builder SetUpTestCase" << std::endl;
}

void BuilderTest::TearDownTestCase(void)
{
    std::cout << "Camera::Builder TearDownTestCase" << std::endl;
}

void BuilderTest::SetUp(void)
{
    std::cout << "Camera::Builder SetUp" << std::endl;
}

void BuilderTest::TearDown(void)
{
    spec_.reset();
    std::cout << "Camera::Builder TearDown.." << std::endl;
}

HWTEST_F(BuilderTest, NormalTest, TestSize.Level0)
{
    std::shared_ptr<HostStreamMgr> streamMgr = HostStreamMgr::Create();
    streamMgr->CreateHostStream({
                .type_ = STILL_CAPTURE
                }, nullptr);
    streamMgr->CreateHostStream({
                .type_ = PREVIEW
                }, nullptr);
    std::unique_ptr<StreamPipelineStrategy> s = StreamPipelineStrategy::Create(streamMgr);
    EXPECT_TRUE(s != nullptr);
    spec_ = s->GeneratePipelineSpec(0);
    EXPECT_TRUE(spec_ != nullptr);

    std::unique_ptr<StreamPipelineBuilder> b = StreamPipelineBuilder::Create(streamMgr);
    EXPECT_TRUE(b != nullptr);
    std::shared_ptr<Pipeline> pipeline = b->Build(spec_);
    EXPECT_TRUE(pipeline != nullptr);
}

HWTEST_F(BuilderTest, AbNormalTest, TestSize.Level0)
{
    std::shared_ptr<HostStreamMgr> streamMgr = HostStreamMgr::Create();
    streamMgr->CreateHostStream({
                .type_ = PREVIEW
                }, nullptr);
    streamMgr->CreateHostStream({
                .type_ = STILL_CAPTURE
                }, nullptr);
    std::unique_ptr<StreamPipelineBuilder> b = StreamPipelineBuilder::Create(streamMgr);
    EXPECT_TRUE(b != nullptr);
    std::shared_ptr<Pipeline> pipeline = b->Build(spec_);
    EXPECT_TRUE(pipeline == nullptr);
}
}
