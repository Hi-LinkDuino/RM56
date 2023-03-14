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
#include "ipipeline_core.h"

using namespace testing::ext;
namespace OHOS::Camera {
enum testStreamId {
    PREVIEW_ID,
    VIDEO_ID,
    CAPTURE_ID,
};
class PipelineCoreTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);

    void SetUp(void);
    void TearDown(void);
};

void PipelineCoreTest::SetUpTestCase(void)
{
    std::cout << "Camera::PipelineCore SetUpTestCase" << std::endl;
}

void PipelineCoreTest::TearDownTestCase(void)
{
    std::cout << "Camera::PipelineCore TearDownTestCase" << std::endl;
}

void PipelineCoreTest::SetUp(void)
{
    std::cout << "Camera::PipelineCore SetUp" << std::endl;
}

void PipelineCoreTest::TearDown(void)
{
    std::cout << "Camera::PipelineCore TearDown.." << std::endl;
}

HWTEST_F(PipelineCoreTest, PipelineCore_NormalPreviewTest, TestSize.Level0)
{
    RetCode re = RC_OK;
    std::shared_ptr<IPipelineCore> core = IPipelineCore::Create();
    EXPECT_TRUE(core != nullptr);
    re = core->Init();
    EXPECT_TRUE(re == RC_OK);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = PREVIEW,
                .streamId_ = PREVIEW_ID
                }, nullptr);
    EXPECT_TRUE(re == RC_OK);
    std::shared_ptr<IStreamPipelineCore> s = core->GetStreamPipelineCore();
    EXPECT_TRUE(s != nullptr);
    re = s->Init();
    EXPECT_TRUE(re == RC_OK);
    re = s->CreatePipeline(0);
    EXPECT_TRUE(re == RC_OK);
}

HWTEST_F(PipelineCoreTest, PipelineCore_NormalSnapshotTest, TestSize.Level0)
{
    RetCode re = RC_OK;
    std::shared_ptr<IPipelineCore> core = IPipelineCore::Create();
    EXPECT_TRUE(core != nullptr);
    re = core->Init();
    EXPECT_TRUE(re == RC_OK);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = STILL_CAPTURE,
                .streamId_ = CAPTURE_ID
                }, nullptr);
    EXPECT_TRUE(re == RC_OK);
    std::shared_ptr<IStreamPipelineCore> s = core->GetStreamPipelineCore();
    EXPECT_TRUE(s != nullptr);
    re = s->Init();
    EXPECT_TRUE(re == RC_OK);
    re = s->CreatePipeline(0);
    EXPECT_TRUE(re == RC_OK);
}

HWTEST_F(PipelineCoreTest, PipelineCore_NormalPreviewSnapshotTest, TestSize.Level0)
{
    RetCode re = RC_OK;
    std::shared_ptr<IPipelineCore> core = IPipelineCore::Create();
    EXPECT_TRUE(core != nullptr);
    re = core->Init();
    EXPECT_TRUE(re == RC_OK);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = PREVIEW,
                .streamId_ = PREVIEW_ID
                }, nullptr);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = STILL_CAPTURE,
                .streamId_ = CAPTURE_ID
                }, nullptr);
    EXPECT_TRUE(re == RC_OK);
    std::shared_ptr<IStreamPipelineCore> s = core->GetStreamPipelineCore();
    EXPECT_TRUE(s != nullptr);
    re = s->Init();
    EXPECT_TRUE(re == RC_OK);
    re = s->CreatePipeline(0);
    EXPECT_TRUE(re == RC_OK);
}

HWTEST_F(PipelineCoreTest, PipelineCore_NormalPreviewToSnapshotTest, TestSize.Level0)
{
    RetCode re = RC_OK;
    std::shared_ptr<IPipelineCore> core = IPipelineCore::Create();
    EXPECT_TRUE(core != nullptr);
    re = core->Init();
    EXPECT_TRUE(re == RC_OK);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = PREVIEW,
                .streamId_ = PREVIEW_ID
                }, nullptr);
    EXPECT_TRUE(re == RC_OK);
    std::shared_ptr<IStreamPipelineCore> s = core->GetStreamPipelineCore();
    EXPECT_TRUE(s != nullptr);
    re = s->Init();
    EXPECT_TRUE(re == RC_OK);
    re = s->CreatePipeline(0);
    EXPECT_TRUE(re == RC_OK);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = STILL_CAPTURE,
                .streamId_ = CAPTURE_ID
                }, nullptr);
    EXPECT_TRUE(re == RC_OK);
    re = s->CreatePipeline(0);
    EXPECT_TRUE(re == RC_OK);
}

HWTEST_F(PipelineCoreTest, PipelineCore_NormalSnapshotPreviewTest, TestSize.Level0)
{
    RetCode re = RC_OK;
    std::shared_ptr<IPipelineCore> core = IPipelineCore::Create();
    EXPECT_TRUE(core != nullptr);
    re = core->Init();
    EXPECT_TRUE(re == RC_OK);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = STILL_CAPTURE,
                .streamId_ = CAPTURE_ID
                }, nullptr);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = PREVIEW,
                .streamId_ = PREVIEW_ID
                }, nullptr);
    EXPECT_TRUE(re == RC_OK);
    std::shared_ptr<IStreamPipelineCore> s = core->GetStreamPipelineCore();
    EXPECT_TRUE(s != nullptr);
    re = s->Init();
    EXPECT_TRUE(re == RC_OK);
    re = s->CreatePipeline(0);
    EXPECT_TRUE(re == RC_OK);
}

HWTEST_F(PipelineCoreTest, PipelineCore_NormalPreviewPreviewTest, TestSize.Level0)
{
    RetCode re = RC_OK;
    std::shared_ptr<IPipelineCore> core = IPipelineCore::Create();
    EXPECT_TRUE(core != nullptr);
    re = core->Init();
    EXPECT_TRUE(re == RC_OK);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = PREVIEW,
                .streamId_ = PREVIEW_ID
                }, nullptr);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = PREVIEW,
                .streamId_ = 2000
                }, nullptr);
    EXPECT_TRUE(re == RC_OK);
    std::shared_ptr<IStreamPipelineCore> s = core->GetStreamPipelineCore();
    EXPECT_TRUE(s != nullptr);
    re = s->Init();
    EXPECT_TRUE(re == RC_OK);
    re = s->CreatePipeline(0);
    EXPECT_TRUE(re == RC_OK);
}

HWTEST_F(PipelineCoreTest, PipelineCore_NormalTest, TestSize.Level0)
{
    RetCode re = RC_OK;
    std::shared_ptr<IPipelineCore> core = IPipelineCore::Create();
    EXPECT_TRUE(core != nullptr);
    re = core->Init();
    EXPECT_TRUE(re == RC_OK);
    std::shared_ptr<IStreamPipelineCore> s = core->GetStreamPipelineCore();
    EXPECT_TRUE(s != nullptr);
    re = s->Init();
    EXPECT_TRUE(re == RC_OK);
    re = s->CreatePipeline(0);
    EXPECT_TRUE(re != RC_OK);
}

HWTEST_F(PipelineCoreTest, PipelineCore_AbNormalPreviewTest, TestSize.Level0)
{
    RetCode re = RC_OK;
    std::shared_ptr<IPipelineCore> core = IPipelineCore::Create();
    EXPECT_TRUE(core != nullptr);
    re = core->Init();
    EXPECT_TRUE(re == RC_OK);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = PREVIEW
                }, nullptr);
    EXPECT_TRUE(re == RC_OK);
    std::shared_ptr<IStreamPipelineCore> s = core->GetStreamPipelineCore();
    EXPECT_TRUE(s != nullptr);
    re = s->Init();
    EXPECT_TRUE(re == RC_OK);
    re = s->CreatePipeline(1);
    EXPECT_TRUE(re != RC_OK);
}

HWTEST_F(PipelineCoreTest, PipelineCore_NormalPreviewSnapshotSingleConfigTest, TestSize.Level0)
{
    RetCode re = RC_OK;
    std::shared_ptr<IPipelineCore> core = IPipelineCore::Create();
    EXPECT_TRUE(core != nullptr);
    re = core->Init();
    EXPECT_TRUE(re == RC_OK);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = PREVIEW,
                .streamId_ = PREVIEW_ID
                }, nullptr);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = STILL_CAPTURE,
                .streamId_ = CAPTURE_ID
                }, nullptr);
    EXPECT_TRUE(re == RC_OK);
    std::shared_ptr<IStreamPipelineCore> s = core->GetStreamPipelineCore();
    EXPECT_TRUE(s != nullptr);
    re = s->Init();
    EXPECT_TRUE(re == RC_OK);
    re = s->CreatePipeline(0);
    EXPECT_TRUE(re == RC_OK);
    CaptureMeta meta;
    re = s->Config({0}, meta);
    EXPECT_TRUE(re != RC_OK);
    re = s->Config({3}, meta);
    EXPECT_TRUE(re != RC_OK);
}
HWTEST_F(PipelineCoreTest, PipelineCore_NormalPreviewSnapshotConfigTest, TestSize.Level0)
{
    RetCode re = RC_OK;
    std::shared_ptr<IPipelineCore> core = IPipelineCore::Create();
    EXPECT_TRUE(core != nullptr);
    re = core->Init();
    EXPECT_TRUE(re == RC_OK);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = PREVIEW,
                .streamId_ = PREVIEW_ID
                }, nullptr);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = STILL_CAPTURE,
                .streamId_ = CAPTURE_ID
                }, nullptr);
    EXPECT_TRUE(re == RC_OK);
    std::shared_ptr<IStreamPipelineCore> s = core->GetStreamPipelineCore();
    CaptureMeta meta;
    EXPECT_TRUE(s != nullptr);
    re = s->Init();
    EXPECT_TRUE(re == RC_OK);
    re = s->CreatePipeline(0);
    EXPECT_TRUE(re == RC_OK);
    re = s->Config({0}, meta);
    EXPECT_TRUE(re != RC_OK);
    re = s->Config({3}, meta);
    EXPECT_TRUE(re != RC_OK);
}
HWTEST_F(PipelineCoreTest, PipelineCore_NormalPreviewSnapshotDestroyTest, TestSize.Level0)
{
    RetCode re = RC_OK;
    std::shared_ptr<IPipelineCore> core = IPipelineCore::Create();
    EXPECT_TRUE(core != nullptr);
    re = core->Init();
    EXPECT_TRUE(re == RC_OK);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = PREVIEW,
                .streamId_ = PREVIEW_ID
                }, nullptr);
    re = core->GetHostStreamMgr()->CreateHostStream({
                .type_ = STILL_CAPTURE,
                .streamId_ = CAPTURE_ID
                }, nullptr);
    EXPECT_TRUE(re == RC_OK);
    std::shared_ptr<IStreamPipelineCore> s = core->GetStreamPipelineCore();
    EXPECT_TRUE(s != nullptr);
    re = s->Init();
    EXPECT_TRUE(re == RC_OK);
    re = s->CreatePipeline(0);
    EXPECT_TRUE(re == RC_OK);
    re = s->DestroyPipeline({0, 2});
    EXPECT_TRUE(re == RC_OK);
}
}
