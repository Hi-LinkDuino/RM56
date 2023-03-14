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
#include "capture_test.h"

void UtestCaptureTest::SetUpTestCase(void)
{}
void UtestCaptureTest::TearDownTestCase(void)
{}
void UtestCaptureTest::SetUp(void)
{
    if (display_ == nullptr)
    display_ = std::make_shared<TestDisplay>();
    display_->FBInit();
    display_->Init();
}
void UtestCaptureTest::TearDown(void)
{
    display_->Close();
}

/**
  * @tc.name: preview and capture
  * @tc.desc: Commit 2 streams together, Preview and still_capture streams, isStreaming is true.
  * @tc.level: Level0
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCaptureTest, camera_capture_0001)
{
    // Get the stream manager
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
  * @tc.name: preview and capture
  * @tc.desc: Preview + capture, then close camera, and preview + capture again.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCaptureTest, camera_capture_0002)
{
    std::cout << "==========[test log] Preview + capture, then cloase camera,";
    std::cout << "and preview + capture again." << std::endl;
    // Get the stream manager
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

    // the 2nd time
    // Configure two streams of information
    // Get the stream manager
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
  * @tc.name: preview and capture
  * @tc.desc: Preview + capture with 3A, success.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCaptureTest, camera_capture_0003)
{
    std::cout << "==========[test log] Capture with 3A, success." << std::endl;
    // Get the stream manager
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_capture, display_->captureId_preview, false, true);
    // Issue 3A parameters to increase exposure
    std::shared_ptr<OHOS::Camera::CameraSetting> meta = std::make_shared<OHOS::Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xa0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    display_->rc = display_->cameraDevice->UpdateSettings(meta);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] UpdateSettings success, for 10s." << std::endl;
    } else {
        std::cout << "==========[test log] UpdateSettings fail, rc = " << display_->rc << std::endl;
    }
    // release stream
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview, display_->streamId_capture};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: preview and capture
  * @tc.desc: Preview + capture, then switch to preview + video.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCaptureTest, camera_capture_0004)
{
    std::cout << "==========[test log] Preview + capture, then switch to preview + video." << std::endl;
    std::cout << "==========[test log] First, create preview + capture." << std::endl;
    // Get the stream manager
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

    std::cout << "==========[test log] Next, switch to preview + video." << display_->rc << std::endl;
    // Get the stream manager
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
  * @tc.name: preview and capture
  * @tc.desc: Commit 2 streams together, Preview and still_capture streams, isStreaming is false.
  * @tc.level: Level0
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCaptureTest, camera_capture_0006)
{
    // Get the stream manager
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, false);
    display_->StartCapture(display_->streamId_capture, display_->captureId_preview, false, false);
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview, display_->streamId_capture};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: preview and capture
  * @tc.desc: Commit 2 streams in order, Preview and still_capture streams.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCaptureTest, camera_capture_0010)
{
    std::cout << "==========[test log] Preview and still_capture streams." << std::endl;
    // Configure two streams of information
    EXPECT_EQ(true, display_->cameraDevice != nullptr);
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // Start capture
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);

    // Configure capture stream information
    display_->intents = {Camera::STILL_CAPTURE};
    display_->StartStream(display_->intents);
    // Start capture
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, false, true);
    display_->StartCapture(display_->streamId_capture, display_->captureId_capture, false, true);

    // post-processing
    display_->rc = display_->streamOperator->CancelCapture(display_->captureId_capture);
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] CancelCapture success." << std::endl;
    } else {
        std::cout << "==========[test log] CancelCapture fail, rc = ." << display_->rc << std::endl;
    }
    display_->rc = display_->streamOperator->CancelCapture(display_->captureId_preview);
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] CancelCapture success." << std::endl;
    } else {
        std::cout << "==========[test log] CancelCapture fail, rc = ." << display_->rc << std::endl;
    }
    display_->rc = display_->streamOperator->ReleaseStreams({display_->streamId_capture});
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] ReleaseStreams success." << std::endl;
    } else {
        std::cout << "==========[test log] ReleaseStreams fail, rc = ." << display_->rc << std::endl;
    }
    display_->rc = display_->streamOperator->ReleaseStreams({display_->streamId_preview});
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] ReleaseStreams success." << std::endl;
    } else {
        std::cout << "==========[test log] ReleaseStreams fail, rc = ." << display_->rc << std::endl;
    }
}

/**
  * @tc.name: Only Still_capture stream
  * @tc.desc: Only Still_capture stream, capture->isStreaming = false.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCaptureTest, camera_capture_0020)
{
    std::cout << "==========[test log] No preview, only still_capture." << std::endl;
    // start stream
    display_->AchieveStreamOperator();
    std::shared_ptr<IBufferProducer> producer = IBufferProducer::CreateBufferQueue();
    producer->SetQueueSize(8); // 8:buffer queue size
    if (producer->GetQueueSize() != 8) { // 8:buffer queue size
        std::cout << "~~~~~~~" << std::endl;
    }
    auto callback = [this](std::shared_ptr<SurfaceBuffer> b) {
        display_->BufferCallback(b, display_->capture_mode);
        return;
    };
    producer->SetCallback(callback);
    std::vector<std::shared_ptr<StreamInfo>> streamInfos;
    display_->streamInfo = std::make_shared<OHOS::Camera::StreamInfo>();
    display_->streamInfo->streamId_ = 1001;
    display_->streamInfo->width_ = 640; // 640:picture width
    display_->streamInfo->height_ = 480; // 640:picture height
    display_->streamInfo->format_ = CAMERA_FORMAT_YUYV_422_PKG;
    display_->streamInfo->datasapce_ = 8; // 8:picture datasapce
    display_->streamInfo->intent_ = STILL_CAPTURE;
    display_->streamInfo->tunneledMode_ = 5; // 5:tunnel mode
    display_->streamInfo->bufferQueue_ = producer;
    streamInfos.push_back(display_->streamInfo);
    display_->rc = display_->streamOperator->CreateStreams(streamInfos);
    CAMERA_LOGE("CreateStreams! rc:0x%x\n", display_->rc);

    display_->rc = display_->streamOperator->CommitStreams(NORMAL, nullptr);
    CAMERA_LOGE("CommitStreams! rc:0x%x\n", display_->rc);
    int captureId = 2001;
    std::shared_ptr<CaptureInfo> captureInfo = std::make_shared<CaptureInfo>();
    captureInfo->streamIds_ = {1001};
    captureInfo->enableShutterCallback_ = false;

    display_->rc = display_->streamOperator->Capture(captureId, captureInfo, false);
    CAMERA_LOGE("Capture! rc:0x%x\n", display_->rc);
    sleep(5);
    display_->rc = display_->streamOperator->CancelCapture(captureId);
    CAMERA_LOGE("CancelCapture! rc:0x%x\n", display_->rc);
    display_->rc = display_->streamOperator->ReleaseStreams(captureInfo->streamIds_);
    CAMERA_LOGE("ReleaseStreams! rc:0x%x\n", display_->rc);
}

/**
  * @tc.name: Only Still_capture stream
  * @tc.desc: Only Still_capture stream, capture->isStreaming = true.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestCaptureTest, camera_capture_0021)
{
    std::cout << "==========[test log] Still_capture stream, capture->isStreaming = true." << std::endl;
    // start stream
    display_->AchieveStreamOperator();
    std::shared_ptr<IBufferProducer> producer = IBufferProducer::CreateBufferQueue();
    producer->SetQueueSize(8); // 8:set bufferqueue size
    if (producer->GetQueueSize() != 8) { // 8:get bufferqueue size
        std::cout << "~~~~~~~" << std::endl;
    }
    auto callback = [this](std::shared_ptr<SurfaceBuffer> b) {
        display_->BufferCallback(b, display_->capture_mode);
        return;
    };
    producer->SetCallback(callback);
    std::vector<std::shared_ptr<StreamInfo>> streamInfos;
    display_->streamInfo = std::make_shared<OHOS::Camera::StreamInfo>();
    display_->streamInfo->streamId_ = 1001;
    display_->streamInfo->width_ = 640; // 640:picture width
    display_->streamInfo->height_ = 480; // 640:picture height
    display_->streamInfo->format_ = CAMERA_FORMAT_YUYV_422_PKG;
    display_->streamInfo->datasapce_ = 8; // 8:picture datasapce
    display_->streamInfo->intent_ = STILL_CAPTURE;
    display_->streamInfo->tunneledMode_ = 5; // 5:tunnel mode
    display_->streamInfo->bufferQueue_ = producer;
    streamInfos.push_back(display_->streamInfo);
    display_->rc = display_->streamOperator->CreateStreams(streamInfos);
    CAMERA_LOGE("CreateStreams! rc:0x%x\n", display_->rc);

    display_->rc = display_->streamOperator->CommitStreams(NORMAL, nullptr);
    CAMERA_LOGE("CommitStreams! rc:0x%x\n", display_->rc);
    int captureId = 2001;
    std::shared_ptr<CaptureInfo> captureInfo = std::make_shared<CaptureInfo>();
    captureInfo->streamIds_ = {1001};
    captureInfo->enableShutterCallback_ = false;

    display_->rc = display_->streamOperator->Capture(captureId, captureInfo, true);
    CAMERA_LOGE("Capture! rc:0x%x\n", display_->rc);
    sleep(5);
    display_->rc = display_->streamOperator->CancelCapture(captureId);
    CAMERA_LOGE("CancelCapture! rc:0x%x\n", display_->rc);
    display_->rc = display_->streamOperator->ReleaseStreams(captureInfo->streamIds_);
    CAMERA_LOGE("ReleaseStreams! rc:0x%x\n", display_->rc);
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] ReleaseStreams success." << std::endl;
    } else {
        std::cout << "==========[test log] ReleaseStreams fail, rc = ." << display_->rc << std::endl;
    }
}