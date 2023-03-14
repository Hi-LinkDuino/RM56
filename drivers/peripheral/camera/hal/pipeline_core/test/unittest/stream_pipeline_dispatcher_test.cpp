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
#include "stream_pipeline_dispatcher.h"

using namespace testing::ext;
namespace OHOS::Camera {
class DispatcherTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);

    void SetUp(void);
    void TearDown(void);
};

void DispatcherTest::SetUpTestCase(void)
{
    std::cout << "Camera::DispatcherTest SetUpTestCase" << std::endl;
}

void DispatcherTest::TearDownTestCase(void)
{
    std::cout << "Camera::DispatcherTest TearDownTestCase" << std::endl;
}

void DispatcherTest::SetUp(void)
{
    std::cout << "Camera::DispatcherTest SetUp" << std::endl;
}

void DispatcherTest::TearDown(void)
{
    std::cout << "Camera::DispatcherTest TearDown.." << std::endl;
}

HWTEST_F(DispatcherTest, NormalTest, TestSize.Level0)
{
    std::shared_ptr<HostStreamMgr> streamMgr = HostStreamMgr::Create();
    streamMgr->CreateHostStream({
                .type_ = PREVIEW
                }, nullptr);
    streamMgr->CreateHostStream({
                .type_ = STILL_CAPTURE
                }, nullptr);
    std::unique_ptr<StreamPipelineStrategy> s = StreamPipelineStrategy::Create(streamMgr);
    EXPECT_TRUE(s != nullptr);
    std::shared_ptr<PipelineSpec> spec_ = s->GeneratePipelineSpec(0);
    EXPECT_TRUE(spec_ != nullptr);

    std::unique_ptr<StreamPipelineBuilder> b = StreamPipelineBuilder::Create(streamMgr);
    EXPECT_TRUE(b != nullptr);
    std::shared_ptr<Pipeline> pipeline = b->Build(spec_);
    EXPECT_TRUE(pipeline != nullptr);
    std::unique_ptr<StreamPipelineDispatcher> d = StreamPipelineDispatcher::Create();
    EXPECT_TRUE(d != nullptr);
    RetCode re = d->Update(pipeline);
    EXPECT_TRUE(re == RC_OK);
}

HWTEST_F(DispatcherTest, ConfigTest, TestSize.Level0)
{
    std::shared_ptr<HostStreamMgr> streamMgrConf = HostStreamMgr::Create();
    streamMgrConf->CreateHostStream({
                .type_ = PREVIEW
                }, nullptr);
    streamMgrConf->CreateHostStream({
                .type_ = STILL_CAPTURE
                }, nullptr);
    std::unique_ptr<StreamPipelineStrategy> s = StreamPipelineStrategy::Create(streamMgrConf);
    EXPECT_TRUE(s != nullptr);
    std::shared_ptr<PipelineSpec> spec_ = s->GeneratePipelineSpec(0);
    EXPECT_TRUE(spec_ != nullptr);

    std::unique_ptr<StreamPipelineBuilder> b = StreamPipelineBuilder::Create(streamMgrConf);
    EXPECT_TRUE(b != nullptr);
    std::shared_ptr<Pipeline> pipeline = b->Build(spec_);
    EXPECT_TRUE(pipeline != nullptr);
    std::unique_ptr<StreamPipelineDispatcher> d = StreamPipelineDispatcher::Create();
    EXPECT_TRUE(d != nullptr);
    RetCode re = d->Update(pipeline);
    EXPECT_TRUE(re == RC_OK);
}

HWTEST_F(DispatcherTest, AbConfigTest, TestSize.Level0)
{
    std::shared_ptr<HostStreamMgr> streamMgrAbConf = HostStreamMgr::Create();
    streamMgrAbConf->CreateHostStream({
                .type_ = PREVIEW
                }, nullptr);
    streamMgrAbConf->CreateHostStream({
                .type_ = STILL_CAPTURE
                }, nullptr);
    std::unique_ptr<StreamPipelineStrategy> s = StreamPipelineStrategy::Create(streamMgrAbConf);
    EXPECT_TRUE(s != nullptr);
    std::shared_ptr<PipelineSpec> spec_ = s->GeneratePipelineSpec(0);
    EXPECT_TRUE(spec_ != nullptr);

    std::unique_ptr<StreamPipelineBuilder> b = StreamPipelineBuilder::Create(streamMgrAbConf);
    EXPECT_TRUE(b != nullptr);
    std::shared_ptr<Pipeline> pipeline = b->Build(spec_);
    EXPECT_TRUE(pipeline != nullptr);
    std::unique_ptr<StreamPipelineDispatcher> d = StreamPipelineDispatcher::Create();
    EXPECT_TRUE(d != nullptr);
    RetCode re = d->Update(pipeline);
    EXPECT_TRUE(re == RC_OK);
}
}
