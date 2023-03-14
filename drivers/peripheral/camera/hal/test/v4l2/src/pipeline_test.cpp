/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file expected in compliance with the License.
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

void UtestPipelineTest::SetUpTestCase(void)
{}
void UtestPipelineTest::TearDownTestCase(void)
{}
void UtestPipelineTest::SetUp(void)
{
    if (display_ == nullptr)
    display_ = std::make_shared<TestDisplay>();
    display_->FBInit();
    display_->Init();
}
void UtestPipelineTest::TearDown(void)
{
    display_->Close();
}

/**
  * @tc.name: Check ppl
  * @tc.desc: preview success.
  * @tc.level: Level0
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestPipelineTest, camera_ppl_0001)
{
    std::cout << "==========[test log] Check ppl: preview success." << std::endl;
    // Get the stream manager
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    // release stream
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: Check ppl
  * @tc.desc: preview + capture success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestPipelineTest, camera_ppl_0002)
{
    std::cout << "==========[test log] Check ppl: preview + capture success." << std::endl;
    // Get the stream manager
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_capture, display_->captureId_capture, false, true);
    // release stream
    display_->captureIds = {display_->captureId_preview, display_->captureId_capture};
    display_->streamIds = {display_->streamId_preview, display_->streamId_capture};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: Check ppl
  * @tc.desc: preview + video success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestPipelineTest, camera_ppl_0003)
{
    std::cout << "==========[test log] Check ppl: preview + video success." << std::endl;
    // Get the stream manager
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::VIDEO};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_video, display_->captureId_video, false, true);
    // release stream
    display_->captureIds = {display_->captureId_preview, display_->captureId_video};
    display_->streamIds = {display_->streamId_preview, display_->streamId_video};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: Check ppl
  * @tc.desc: video mode without preview, system not support, expected return fail.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestPipelineTest, camera_ppl_0004)
{
    std::cout << "==========[test log] Video mode without preview, system not support, ";
    std::cout << "expected return fail." << std::endl;

    EXPECT_EQ(true, display_->cameraDevice != nullptr);
    display_->AchieveStreamOperator();
    // Create video stream
    std::vector<std::shared_ptr<StreamInfo>> streamInfos;
    std::shared_ptr<IBufferProducer> producer = IBufferProducer::CreateBufferQueue();
    producer->SetQueueSize(8); // 8:set bufferqueue size
    if (producer->GetQueueSize() != 8) { // 8:get bufferqueue size
        std::cout << "~~~~~~~" << std::endl;
    }
    auto callback = [this](std::shared_ptr<SurfaceBuffer> b) {
        display_->BufferCallback(b, display_->video_mode);
        return;
    };
    producer->SetCallback(callback);
    display_->streamInfo = std::make_shared<OHOS::Camera::StreamInfo>();
    display_->streamInfo->streamId_ = display_->streamId_video;
    display_->streamInfo->width_ = 640; // 640:picture width // 640:picture width
    display_->streamInfo->height_ = 480; // 480:picture height // 480:picture height
    display_->streamInfo->format_ = CAMERA_FORMAT_YUYV_422_PKG;
    display_->streamInfo->datasapce_ = 10; // 10:picture datasapce
    display_->streamInfo->intent_ = Camera::VIDEO;
    display_->streamInfo->tunneledMode_ = 5; // 5:tunnel mode // 5:tunnel mode
    display_->streamInfo->bufferQueue_ = producer;
    std::vector<std::shared_ptr<OHOS::Camera::StreamInfo>>().swap(streamInfos);
    streamInfos.push_back(display_->streamInfo);
    display_->rc = display_->streamOperator->CreateStreams(streamInfos);
    EXPECT_EQ(false, display_->rc == Camera::METHOD_NOT_SUPPORTED);
    if (display_->rc == Camera::METHOD_NOT_SUPPORTED) {
        std::cout << "==========[test log] CreateStreams METHOD_NOT_SUPPORTED, streamId = ";
        std::cout << display_->streamId_video <<", intent = Camera::VIDEO" << std::endl;
    } else {
        std::cout << "==========[test log] CreateStreams fail, rc = " << display_->rc << std::endl;
    }

    display_->rc = display_->streamOperator->CommitStreams(Camera::NORMAL, nullptr);
    EXPECT_EQ(false, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] CommitStreams success." << std::endl;
    } else {
        std::cout << "==========[test log] CommitStreams fail, rc = ." << display_->rc << std::endl;
    }
}