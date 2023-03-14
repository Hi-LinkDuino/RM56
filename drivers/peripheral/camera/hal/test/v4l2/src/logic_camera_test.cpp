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

#include "logic_camera_test.h"

void UtestLogicCameraTest::SetUpTestCase(void)
{}
void UtestLogicCameraTest::TearDownTestCase(void)
{}
void UtestLogicCameraTest::SetUp(void)
{
    if (display_ == nullptr)
    display_ = std::make_shared<TestDisplay>();
    display_->FBInit();
    display_->Init();
}
void UtestLogicCameraTest::TearDown(void)
{
    display_->Close();
}

/**
  * @tc.name: test logic csamera
  * @tc.desc: single stream
  * @tc.level: Level0
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestLogicCameraTest, camera_logic_0001)
{
    std::cout << "==========[test log] test single stream"<< std::endl;
    // Get the stream manager
    display_->AchieveStreamOperator();
    // Configure preview stream information
    std::shared_ptr<IBufferProducer> producer = IBufferProducer::CreateBufferQueue();
    producer->SetQueueSize(8);
    if (producer->GetQueueSize() != 8) {
        std::cout << "~~~~~~~" << std::endl;
    }
    auto callback = [this](std::shared_ptr<SurfaceBuffer> b) {
        display_->BufferCallback(b, display_->preview_mode);
        return;
    };
    producer->SetCallback(callback);
    std::shared_ptr<OHOS::Camera::StreamInfo> streamInfoPre = std::make_shared<OHOS::Camera::StreamInfo>();
    streamInfoPre->streamId_ = display_->streamId_preview;
    streamInfoPre->width_ = 640;
    streamInfoPre->height_ = 480;
    streamInfoPre->format_ = CAMERA_FORMAT_YUYV_422_PKG;
    streamInfoPre->datasapce_ = 8;
    streamInfoPre->intent_ = Camera::PREVIEW;
    streamInfoPre->tunneledMode_ = 5;
    streamInfoPre->bufferQueue_ = producer;
    display_->streamInfos.push_back(streamInfoPre);
    display_->rc = display_->streamOperator->CreateStreams(display_->streamInfos);
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] CreateStreams success, streamId = ";
        std::cout << display_->streamId_capture <<", intent = Camera::STILL_CAPTURE" << std::endl;
    } else {
        std::cout << "==========[test log] CreateStreams fail, rc = " << display_->rc <<" , streamId = ";
        std::cout << display_->streamId_capture <<", intent = Camera::STILL_CAPTURE" << std::endl;
    }
    // Submit stream information
    display_->rc = display_->streamOperator->CommitStreams(DUAL, nullptr);
    EXPECT_EQ(false, display_->rc != Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] CommitStreams DUAL success." << std::endl;
    } else {
        std::cout << "==========[test log] CommitStreams DUAL fail, rc = " << display_->rc << std::endl;
    }
    // capture
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    // post-processing
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview};
    display_->StopStream(display_->captureIds, display_->streamIds);
}