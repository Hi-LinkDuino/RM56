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

using namespace testing::ext;
namespace OHOS::Camera {
class StrategyTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);

    void SetUp(void);
    void TearDown(void);
};

void StrategyTest::SetUpTestCase(void)
{
    std::cout << "Camera::StrategyTest SetUpTestCase" << std::endl;
}

void StrategyTest::TearDownTestCase(void)
{
    std::cout << "Camera::StrategyTest TearDownTestCase" << std::endl;
}

void StrategyTest::SetUp(void)
{
    std::cout << "Camera::StrategyTest SetUp" << std::endl;
}

void StrategyTest::TearDown(void)
{
    std::cout << "Camera::StrategyTest TearDown.." << std::endl;
}

HWTEST_F(StrategyTest, NormalPreviewTest, TestSize.Level0)
{
    std::shared_ptr<HostStreamMgr> streamMgr = HostStreamMgr::Create();
    streamMgr->CreateHostStream({
                    .type_ = PREVIEW
                }, nullptr);
    std::unique_ptr<StreamPipelineStrategy> s = StreamPipelineStrategy::Create(streamMgr);
    EXPECT_TRUE(s != nullptr);
    std::shared_ptr<PipelineSpec> spec = s->GeneratePipelineSpec(0);
    EXPECT_TRUE(spec != nullptr);
}

HWTEST_F(StrategyTest, NormalSnapshotTest, TestSize.Level0)
{
    std::shared_ptr<HostStreamMgr> streamMgr = HostStreamMgr::Create();
    streamMgr->CreateHostStream({
                .type_ = STILL_CAPTURE
                }, nullptr);
    std::unique_ptr<StreamPipelineStrategy> s = StreamPipelineStrategy::Create(streamMgr);
    EXPECT_TRUE(s != nullptr);
    std::shared_ptr<PipelineSpec> spec = s->GeneratePipelineSpec(0);
    EXPECT_TRUE(spec != nullptr);
}

HWTEST_F(StrategyTest, NormalPreviewSnapshotTest, TestSize.Level0)
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
    std::shared_ptr<PipelineSpec> spec = s->GeneratePipelineSpec(0);
    EXPECT_TRUE(spec != nullptr);
}

HWTEST_F(StrategyTest, AbNormalTest, TestSize.Level0)
{
    std::shared_ptr<HostStreamMgr> streamMgr = HostStreamMgr::Create();
    std::unique_ptr<StreamPipelineStrategy> s = StreamPipelineStrategy::Create(streamMgr);
    std::shared_ptr<PipelineSpec> spec = s->GeneratePipelineSpec(1);
    EXPECT_TRUE(spec == nullptr);
}
}
