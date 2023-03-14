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

#include "video_test.h"

using namespace OHOS;
using namespace std;
using namespace testing::ext;
using namespace OHOS::Camera;

void VideoTest::SetUpTestCase(void) {}
void VideoTest::TearDownTestCase(void) {}
void VideoTest::SetUp(void)
{
    Test_ = std::make_shared<OHOS::Camera::Test>();
    Test_->Init();
    Test_->Open();
}
void VideoTest::TearDown(void)
{
    Test_->Close();
}

/**
  * @tc.name: Video
  * @tc.desc: Preview and video streams, Commit 2 streams together, capture in order.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(VideoTest, Camera_Video_0001, TestSize.Level0)
{
    std::cout << "==========[test log]Check video: Preview and video streams, ";
    std::cout << "Commit 2 streams together, capture in order." << std::endl;
    // Configure two stream information
    Test_->intents = {Camera::PREVIEW, Camera::VIDEO};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture video stream
    Test_->StartCapture(Test_->streamId_video, Test_->captureId_video, false, true);
    // Post-processing
    sleep(10);
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_video};
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->streamIds.push_back(Test_->streamId_video);
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: Video
  * @tc.desc: Preview and video streams + 3A, Commit 2 streams together, capture in order.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(VideoTest, Camera_Video_0002, TestSize.Level1)
{
    std::cout << "==========[test log]Check video: Preview and video streams + 3A, ";
    std::cout << "Commit 2 streams together, capture in order." << std::endl;
    // Configure two stream information
    Test_->intents = {Camera::PREVIEW, Camera::VIDEO};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters to increase exposure
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xb0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    Test_->rc = Test_->cameraDevice->UpdateSettings(meta);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]check Capture: UpdateSettings success, for 10s." << std::endl;
    } else {
        std::cout << "==========[test log]check Capture: UpdateSettings fail, rc = " << Test_->rc << std::endl;
    }
    // Capture video stream
    Test_->StartCapture(Test_->streamId_video, Test_->captureId_video, false, true);
    sleep(5);
    // Post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_video};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_video};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: Video
  * @tc.desc: Preview + video, commit together, then close device, and preview + video again.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(VideoTest, Camera_Video_0003, TestSize.Level1)
{
    std::cout << "==========[test log]Check video: Preview + video, ";
    std::cout << "commit together, then close device, and preview + video again." << std::endl;
    // Configure two stream information
    Test_->intents = {Camera::PREVIEW, Camera::VIDEO};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture video stream
    Test_->StartCapture(Test_->streamId_video, Test_->captureId_video, false, true);
    // Post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_video};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_video};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->consumerMap_.clear();
    std::cout << "==========[test log]Check video: The 2nd time." << std::endl;
    // second Configure two stream information
    // Configure two stream information
    Test_->intents = {Camera::PREVIEW, Camera::VIDEO};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture video stream
    Test_->StartCapture(Test_->streamId_video, Test_->captureId_video, false, true);
    // Post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_video};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_video};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: Video
  * @tc.desc: Preview + video, commit together, then close device, and preview + capture.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(VideoTest, Camera_Video_0004, TestSize.Level1)
{
    std::cout << "==========[test log]Check video: Preview + video, ";
    std::cout << "commit together, then close device, and preview + capture." << std::endl;
    // Configure two stream information
    Test_->intents = {Camera::PREVIEW, Camera::VIDEO};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture video stream
    Test_->StartCapture(Test_->streamId_video, Test_->captureId_video, false, true);
    // Post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_video};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_video};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->consumerMap_.clear();

    std::cout << "==========[test log]Check video: The 2nd time." << std::endl;
    // Configure two stream information
    Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture camera stream, continuous capture
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, true);
    // Post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_capture};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_capture};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: Video
  * @tc.desc: Preview + video, commit together, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(VideoTest, Camera_Video_0005, TestSize.Level0)
{
    std::cout << "==========[test log]Check video: Preview + video, commit together, success." << std::endl;
    // Configure two stream information
    Test_->intents = {Camera::PREVIEW, Camera::VIDEO};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture video stream
    Test_->StartCapture(Test_->streamId_video, Test_->captureId_video, false, true);
    // Post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_video};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_video};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: Video
  * @tc.desc: Dynamic Video mode, preview, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(VideoTest, Camera_Video_0020, TestSize.Level1)
{
    std::cout << "==========[test log]Check video: Video mode, preview, success." << std::endl;
    // Start preview stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Start stream
    Test_->intents = {Camera::VIDEO};
    Test_->StartStream(Test_->intents);
    // Start capture
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    Test_->StartCapture(Test_->streamId_video, Test_->captureId_video, false, true);
    // Release stream
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_video};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_video};
    Test_->intents = {Camera::PREVIEW, Camera::VIDEO};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: Video
  * @tc.desc: Video mode, preview, set 3A, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(VideoTest, Camera_Video_0021, TestSize.Level1)
{
    std::cout << "==========[test log]Check video: Video mode, preview, set 3A, success." << std::endl;
    // Configure two stream information
    Test_->intents = {Camera::PREVIEW, Camera::VIDEO};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters to increase exposure
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xc0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    Test_->rc = Test_->cameraDevice->UpdateSettings(meta);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]check Capture: UpdateSettings success, for 10s." << std::endl;
    } else {
        std::cout << "==========[test log]check Capture: UpdateSettings fail, rc = " << Test_->rc << std::endl;
    }
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_video, Test_->captureId_video, false, true);
    sleep(5);
    // Post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_video};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_video};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: Video
  * @tc.desc: Preview and video streams + 3A, Commit 2 streams together, video in order.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(VideoTest, Camera_Video_0022, TestSize.Level2)
{
    std::cout << "==========[test log]Check video: Preview and video streams + 3A, ";
    std::cout << "Commit 2 streams together, capture in order." << std::endl;
    // Configure two stream information
    Test_->intents = {Camera::PREVIEW, Camera::VIDEO};
    Test_->StartStream(Test_->intents);
    Test_->StopConsumer({Camera::VIDEO});
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters to increase exposure
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xb0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    Test_->rc = Test_->cameraDevice->UpdateSettings(meta);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]check Capture: UpdateSettings success, for 10s." << std::endl;
    } else {
        std::cout << "==========[test log]check Capture: UpdateSettings fail, rc = " << Test_->rc << std::endl;
    }
    // Capture video stream
    Test_->StartCapture(Test_->streamId_video, Test_->captureId_video, false, true);
    sleep(180);
    // Post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_video};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_video};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
}