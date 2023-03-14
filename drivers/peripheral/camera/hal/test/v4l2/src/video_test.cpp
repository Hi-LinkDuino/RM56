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

void UtestVideoTest::SetUpTestCase(void)
{}
void UtestVideoTest::TearDownTestCase(void)
{}
void UtestVideoTest::SetUp(void)
{
    if (display_ == nullptr)
    display_ = std::make_shared<TestDisplay>();
    display_->FBInit();
    display_->Init();
}
void UtestVideoTest::TearDown(void)
{
    display_->Close();
}

/**
  * @tc.name: Video
  * @tc.desc: Preview + video, commit together, success.
  * @tc.level: Level0
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestVideoTest, camera_video_0001)
{
    std::cout << "==========[test log] 1 Preview + video, commit together, success." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::VIDEO};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_video, display_->captureId_preview, false, true);
    // release stream
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview, display_->streamId_video};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: Video
  * @tc.desc: Preview + video, commit together, set 3A, success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestVideoTest, camera_video_0002)
{
    std::cout << "==========[test log] Preview + video, commit together, set 3A, success." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();

    // start stream
    display_->intents = {Camera::PREVIEW, Camera::VIDEO};
    display_->StartStream(display_->intents);
    // Issue 3A parameters to increase exposure
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xa0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    display_->rc = display_->cameraDevice->UpdateSettings(meta);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] UpdateSettings success, for 5s." << std::endl;
    } else {
        std::cout << "==========[test log] UpdateSettings fail, rc = " << display_->rc << std::endl;
    }
    sleep(3);

    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_video, false, true);
    display_->StartCapture(display_->streamId_video, display_->captureId_video, false, true);
    // release stream
    display_->captureIds = {display_->captureId_video};
    display_->streamIds = {display_->streamId_preview, display_->streamId_video};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: Video
  * @tc.desc: Preview + video, commit together, then close device, and preview + video again.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestVideoTest, camera_video_0003)
{
    std::cout << "==========[test log] Preview + video, commit together, then close device,";
    std::cout << "and preview + video again." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::VIDEO};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_video, display_->captureId_preview, false, true);
    // release stream
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview, display_->streamId_video};
    display_->StopStream(display_->captureIds, display_->streamIds);

    // Turn off the device
    display_->Close();
    // Turn on the device
    display_->Init();
    std::cout << "==========[test log] The 2nd time." << std::endl;

    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::VIDEO};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_video, display_->captureId_preview, false, true);
    // release stream
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview, display_->streamId_video};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: Video
  * @tc.desc: Preview + video, commit together, then close device, and preview + capture.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestVideoTest, camera_video_0004)
{
    std::cout << "==========[test log] Preview + video, commit together, then close device,";
    std::cout << "and preview + capture." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::VIDEO};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_video, display_->captureId_preview, false, true);
    // release stream
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview, display_->streamId_video};
    display_->StopStream(display_->captureIds, display_->streamIds);

    // Turn off the device
    display_->Close();
    std::cout << "==========[test log] cameraDevice->Close" << std::endl;
    // Turn on the device
    display_->Init();

    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_capture, display_->captureId_preview, false, true);
    // release stream
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview, display_->streamId_capture};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: Video
  * @tc.desc: Preview + video, commit together, success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestVideoTest, camera_video_0005)
{
    std::cout << "==========[test log] 1 Preview + video, commit together, success." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::VIDEO};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_video, display_->captureId_preview, false, true);
    // release stream
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview, display_->streamId_video};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: Video
  * @tc.desc: Video start&stop, for 5 times, success.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestVideoTest, camera_video_0010)
{
    std::cout << "==========[test log] Video start&stop, for 5 times, success." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    for (int i = 0; i < 5; i++) {
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::VIDEO};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_video, display_->captureId_preview, false, true);
    // release stream
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview, display_->streamId_video};
    display_->StopStream(display_->captureIds, display_->streamIds);
}
}

/**
  * @tc.name: Video
  * @tc.desc: Video start&stop, for 5 times, success.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestVideoTest, camera_video_0011)
{
    std::cout << "==========[test log] Video start&stop, for 5 times, success." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    for (int i = 0; i < 5; i++) {
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::VIDEO};
    display_->StartStream(display_->intents);

    // Start capture preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    // Start capture recording
    display_->StartCapture(display_->streamId_video, display_->captureId_video, false, true);

    // post-processing
    display_->captureIds = {display_->captureId_preview, display_->captureId_video};
    display_->streamIds = {display_->streamId_preview, display_->streamId_video};
    display_->StopStream(display_->captureIds, display_->streamIds);
}
}

/**
  * @tc.name: Video
  * @tc.desc: Dynamic Video mode, preview, success.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestVideoTest, camera_video_0020)
{
    std::cout << "==========[test log] Video mode, preview, success." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // Create data stream
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // capture
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    // Create video stream
    display_->intents = {Camera::VIDEO};
    display_->StartStream(display_->intents);
    // Start capture preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    // Start capture recording
    display_->StartCapture(display_->streamId_video, display_->captureId_video, false, true);

    // post-processing
    display_->rc = display_->streamOperator->CancelCapture(display_->captureId_video);
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] CancelCapture success, captureId = ";
        std::cout << display_->captureId_video << std::endl;
    } else {
        std::cout << "==========[test log] CancelCapture fail, rc = " << display_->rc << std::endl;
    }
    display_->rc = display_->streamOperator->CancelCapture(display_->captureId_preview);
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] CancelCapture success, captureId = ";
        std::cout << display_->captureId_preview << std::endl;
    } else {
        std::cout << "==========[test log] CancelCapture fail, rc = " << display_->rc << std::endl;
    }
    display_->rc = display_->streamOperator->ReleaseStreams({display_->streamId_video});
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] ReleaseStreams success." << std::endl;
    } else {
        std::cout << "==========[test log] ReleaseStreams fail, rc = " << display_->rc << std::endl;
    }
    display_->rc = display_->streamOperator->ReleaseStreams({display_->streamId_preview});
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] ReleaseStreams success." << std::endl;
    } else {
        std::cout << "==========[test log] ReleaseStreams fail, rc = " << display_->rc << std::endl;
    }
}

/**
  * @tc.name: Video
  * @tc.desc: Video mode, preview, set 3A, success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestVideoTest, camera_video_0021)
{
    std::cout << "==========[test log] Video mode, preview, set 3A, success." << std::endl;
    EXPECT_EQ(true, display_->cameraDevice != nullptr);
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::VIDEO};
    display_->StartStream(display_->intents);
    // capture
    display_->StartCapture(display_->streamId_preview, display_->captureId_video, false, true);
    display_->StartCapture(display_->streamId_video, display_->captureId_video, false, true);
    // Issue 3A parameters to increase exposure
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xa0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    display_->rc = display_->cameraDevice->UpdateSettings(meta);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] UpdateSettings success, for 5s." << std::endl;
    } else {
        std::cout << "==========[test log] UpdateSettings fail, rc = " << display_->rc << std::endl;
    }
    sleep(3);

    // post-processing
    display_->captureIds = {display_->captureId_video};
    display_->streamIds = {display_->streamId_preview, display_->streamId_video};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: Video
  * @tc.desc: Video mode without preview, system not support, expected return fail.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestVideoTest, camera_video_0030)
{
    std::cout << "==========[test log] Video mode without preview, system not support,";
    std::cout << "expected return fail." << std::endl;

    EXPECT_EQ(true, display_->cameraDevice != nullptr);
    display_->AchieveStreamOperator();
    // Create video stream
    std::vector<std::shared_ptr<StreamInfo>> streamInfos;
    std::shared_ptr<IBufferProducer> producer = IBufferProducer::CreateBufferQueue();
    producer->SetQueueSize(8); // 8:set bufferQueue size
    if (producer->GetQueueSize() != 8) { // 8:get bufferQueue size
        std::cout << "~~~~~~~" << std::endl;
    }
    auto callbackVideo = [this](std::shared_ptr<SurfaceBuffer> b) {
        display_->BufferCallback(b, display_->video_mode);
        return;
    };
    producer->SetCallback(callbackVideo);
    display_->streamInfo = std::make_shared<OHOS::Camera::StreamInfo>();
    display_->streamInfo->streamId_ = display_->streamId_video;
    display_->streamInfo->width_ = 640; // 640:picture width
    display_->streamInfo->height_ = 480; // 480:picture height
    display_->streamInfo->format_ = CAMERA_FORMAT_YUYV_422_PKG;
    display_->streamInfo->datasapce_ = 10;
    display_->streamInfo->intent_ = Camera::VIDEO;
    display_->streamInfo->tunneledMode_ = 5; // 5:tunnel mode
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