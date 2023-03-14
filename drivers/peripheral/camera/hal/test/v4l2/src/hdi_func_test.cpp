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
#include "hdi_func_test.h"

void UtestHdiFuncTest::SetUpTestCase(void)
{}
void UtestHdiFuncTest::TearDownTestCase(void)
{}
void UtestHdiFuncTest::SetUp(void)
{
    if (display_ == nullptr)
    display_ = std::make_shared<TestDisplay>();
    display_->FBInit();
    display_->Init();
}
void UtestHdiFuncTest::TearDown(void)
{
    display_->Close();
}

/**
  * @tc.name: Capture
  * @tc.desc: Capture, input normal.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestHdiFuncTest, camera_hdi_0190)
{
    std::cout << "==========[test log] Capture, input normal." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // Create data stream
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
  * @tc.name: Capture
  * @tc.desc: Preview, Capture->captureInfo->streamID = -1 ,return error.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestHdiFuncTest, camera_hdi_0191)
{
    std::cout << "==========[test log] Preview, Capture->captureInfo->streamID = -1 ,return error." << std::endl;
    display_->OpenCamera();
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // Get preview
    int captureId = 2001;
    std::shared_ptr<OHOS::Camera::CaptureInfo> captureInfo = std::make_shared<OHOS::Camera::CaptureInfo>();
    captureInfo->streamIds_ = {-1};
    captureInfo->enableShutterCallback_ = true;
    display_->rc = display_->streamOperator->Capture(captureId, captureInfo, true);
    EXPECT_EQ(Camera::CamRetCode::INVALID_ARGUMENT, display_->rc);
    if (display_->rc == Camera::CamRetCode::INVALID_ARGUMENT)
    {
        std::cout << "==========[test log] Capture fail." << std::endl;
    } else {
        std::cout << "==========[test log] Capture success." << std::endl;
    }
    sleep(3);
}

/**
  * @tc.name: Capture
  * @tc.desc: Preview, Capture->captureInfo->streamID = 2147483647 ,return success.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestHdiFuncTest, camera_hdi_0192)
{
    std::cout << "==========[test log] Preview,";
    std::cout << "Capture->captureInfo->streamID = 2147483647 ,return success." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // Get preview
    int captureId = 2001;
    std::shared_ptr<OHOS::Camera::CaptureInfo> captureInfo = std::make_shared<OHOS::Camera::CaptureInfo>();
    captureInfo->streamIds_ = {2147483647};
    captureInfo->enableShutterCallback_ = true;
    bool isStreaming = true;
    display_->rc = display_->streamOperator->Capture(captureId, captureInfo, isStreaming);
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] Capture success." << std::endl;
    } else {
        std::cout << "==========[test log] Capture fail, rc = " << display_->rc << std::endl;
    }
    sleep(5);
    display_->rc = display_->streamOperator->CancelCapture(captureId);
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] CancelCapture success." << std::endl;
    } else {
        std::cout << "==========[test log] CancelCapture fail, rc = " << display_->rc << std::endl;
    }
    display_->cameraDevice->Close();
    std::cout << "cameraDevice->Close" << std::endl;
}

/**
  * @tc.name: Capture
  * @tc.desc: Preview, Capture->captureInfo->enableShutterCallback = false ,return success.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestHdiFuncTest, camera_hdi_0193)
{
    std::cout << "==========[test log] Preview,";
    std::cout << "Capture->captureInfo->enableShutterCallback = false ,return success." << std::endl;
    // Create and get streamOperator information
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
  * @tc.name: Capture
  * @tc.desc: Preview, Capture->isStreaming = false ,expected success.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestHdiFuncTest, camera_hdi_0194)
{
    std::cout << "==========[test log] Preview, Capture->isStreaming = false ,expected success." << std::endl;
    std::cout << "==========[test log] Preview, Capture->isStreaming = false ,expected success." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // Get preview
    int captureId = 2001;
    std::shared_ptr<OHOS::Camera::CaptureInfo> captureInfo = std::make_shared<OHOS::Camera::CaptureInfo>();
    captureInfo->streamIds_ = {display_->streamId_preview};
    captureInfo->enableShutterCallback_ = true;
    display_->rc = display_->streamOperator->Capture(captureId, captureInfo, false);
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    sleep(5);
    display_->streamOperator->CancelCapture(captureId);
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] CancelCapture success." << std::endl;
    } else {
        std::cout << "==========[test log] CancelCapture fail, rc = " << display_->rc << std::endl;
    }
    // release stream
    display_->rc = display_->streamOperator->ReleaseStreams(captureInfo->streamIds_);
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] ReleaseStreams success." << std::endl;
    } else {
        std::cout << "==========[test log] ReleaseStreams fail, rc = " << display_->rc << std::endl;
    }
}

/**
  * @tc.name: Capture
  * @tc.desc: Preview, Capture->captureId = -1 ,return error.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestHdiFuncTest, camera_hdi_0195)
{
    std::cout << "==========[test log] Preview, Capture->captureId = -1 ,return error." << std::endl;
    display_->OpenCamera();
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // Get preview
    int captureId = -1;
    std::shared_ptr<OHOS::Camera::CaptureInfo> captureInfo = std::make_shared<OHOS::Camera::CaptureInfo>();
    captureInfo->streamIds_ = {display_->streamId_preview};
    captureInfo->enableShutterCallback_ = false;
    bool isStreaming = true;
    display_->rc = display_->streamOperator->Capture(captureId, captureInfo, isStreaming);
    EXPECT_EQ(Camera::CamRetCode::INVALID_ARGUMENT, display_->rc);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] Capture success." << std::endl;
    } else {
        std::cout << "==========[test log] Capture fail, rc = " << display_->rc << std::endl;
    }
    sleep(3);
}

/**
  * @tc.name: Capture
  * @tc.desc: Preview, Capture->captureInfo->enableShutterCallback = true ,return success.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestHdiFuncTest, camera_hdi_0196)
{
    std::cout << "==========[test log] Preview,";
    std::cout << "Capture->captureInfo->enableShutterCallback = true ,return success." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // Get preview
    display_->StartCapture(display_->streamId_preview, display_->captureId_preview, true, true);
    // release stream
    display_->captureIds = {display_->captureId_preview};
    display_->streamIds = {display_->streamId_preview};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: CancelCapture
  * @tc.desc: CancelCapture, success.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestHdiFuncTest, camera_hdi_0200)
{
    std::cout << "==========[test log] CancelCapture, success." << std::endl;
    // Create and get streamOperator information
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
  * @tc.name: CancelCapture
  * @tc.desc: CancelCapture captureID = -1.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestHdiFuncTest, camera_hdi_0201)
{
    std::cout << "==========[test log] CancelCapture captureID = -1, expected fail." << std::endl;
    // Create and get streamOperator information
    display_->AchieveStreamOperator();
    // start stream
    display_->intents = {Camera::PREVIEW};
    display_->StartStream(display_->intents);
    // Get preview
    int captureId = 100;
    std::shared_ptr<OHOS::Camera::CaptureInfo> captureInfo = std::make_shared<OHOS::Camera::CaptureInfo>();
    captureInfo->streamIds_ = {display_->streamId_preview};
    captureInfo->enableShutterCallback_ = false;
    bool isStreaming = true;
    display_->rc = display_->streamOperator->Capture(captureId, captureInfo, isStreaming);
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    sleep(5);
    display_->rc = display_->streamOperator->CancelCapture(-1);
    EXPECT_EQ(Camera::CamRetCode::INVALID_ARGUMENT, display_->rc);
    display_->rc = display_->streamOperator->CancelCapture(captureId);
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] CancelCapture success." << std::endl;
    } else {
        std::cout << "==========[test log] CancelCapture fail, rc = " << display_->rc << std::endl;
    }
    // release stream
    display_->rc = display_->streamOperator->ReleaseStreams(captureInfo->streamIds_);
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] ReleaseStreams success." << std::endl;
    } else {
        std::cout << "==========[test log] ReleaseStreams fail, rc = " << display_->rc << std::endl;
    }
}

/**
  * @tc.name: AttachBufferQueue
  * @tc.desc: AttachBufferQueue, normal input.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  // */
TEST_F(UtestHdiFuncTest, camera_hdi_0210)
{
    std::cout << "==========[test log] AttachBufferQueue, normal input." << std::endl;
    display_->AchieveStreamOperator();
    // Create data stream
    std::shared_ptr<IBufferProducer> producer = IBufferProducer::CreateBufferQueue();
    producer->SetQueueSize(8); // 8:set bufferQueue size
    if (producer->GetQueueSize() != 8) { // 8:get bufferQueue size
        std::cout << "~~~~~~~" << std::endl;
    }
    auto callback = [this](std::shared_ptr<SurfaceBuffer> b) {
        display_->BufferCallback(b, display_->preview_mode);
        return;
    };
    producer->SetCallback(callback);

    display_->streamInfo = std::make_shared<OHOS::Camera::StreamInfo>();
    display_->streamInfo->streamId_ = 1001;
    display_->streamInfo->width_ = 640; // 640:picture width
    display_->streamInfo->height_ = 480; // 480:picture height
    display_->streamInfo->format_ = CAMERA_FORMAT_YUYV_422_PKG;
    display_->streamInfo->datasapce_ = 8; // 8:picture datasapce
    display_->streamInfo->intent_ = Camera::PREVIEW;
    display_->streamInfo->tunneledMode_ = 5; // 5:tunnel mode
    display_->streamInfos.push_back(display_->streamInfo);
    display_->rc = display_->streamOperator->CreateStreams(display_->streamInfos);
    std::cout << "==========[test log] streamOperator->CreateStreams's RetCode = " << display_->rc << std::endl;
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] CreateStreams success." << std::endl;
    } else {
        std::cout << "==========[test log] CreateStreams fail, rc = " << display_->rc << std::endl;
    }
    display_->rc = display_->streamOperator->AttachBufferQueue(display_->streamInfo->streamId_, producer);
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] AttachBufferQueue success. " << std::endl;
    } else {
        std::cout << "==========[test log] AttachBufferQueue fail, rc = " << display_->rc << std::endl;
    }
    sleep(3);
    // release stream
    display_->streamIds = {1001};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: AttachBufferQueue
  * @tc.desc: AttachBufferQueue, streamID is not exist.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestHdiFuncTest, camera_hdi_0211)
{
    std::cout << "==========[test log] AttachBufferQueue, streamID is not exist.." << std::endl;
    display_->AchieveStreamOperator();
    // Create data stream
    display_->streamInfo = std::make_shared<OHOS::Camera::StreamInfo>();
    std::shared_ptr<IBufferProducer> producer = IBufferProducer::CreateBufferQueue();
    producer->SetQueueSize(8); // 8:set bufferQueue size
    if (producer->GetQueueSize() != 8) { // 8:get bufferQueue size
        std::cout << "~~~~~~~" << std::endl;
    }
    auto callback = [this](std::shared_ptr<SurfaceBuffer> b) {
        display_->BufferCallback(b, display_->preview_mode);
        return;
    };
    producer->SetCallback(callback);
    display_->streamInfo->streamId_ = 1001;
    display_->streamInfo->width_ = 640; // 640:picture width
    display_->streamInfo->height_ = 480; // 480:picture height
    display_->streamInfo->format_ = CAMERA_FORMAT_YUYV_422_PKG;
    display_->streamInfo->datasapce_ = 8; // 8:picture datasapce
    display_->streamInfo->intent_ = Camera::PREVIEW;
    display_->streamInfo->tunneledMode_ = 5; // 5:tunnel mode
    display_->streamInfos.push_back(display_->streamInfo);
    display_->rc = display_->streamOperator->CreateStreams(display_->streamInfos);
    std::cout << "==========[test log] streamOperator->CreateStreams's RetCode = " << display_->rc << std::endl;
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] CreateStreams success." << std::endl;
    } else {
        std::cout << "==========[test log] CreateStreams fail, rc = " << display_->rc << std::endl;
    }
    display_->rc = display_->streamOperator->AttachBufferQueue(0, producer);
    EXPECT_EQ(true, display_->rc != Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] AttachBufferQueue success. " << std::endl;
    } else {
        std::cout << "==========[test log] AttachBufferQueue fail, rc = " << display_->rc << std::endl;
    }
    sleep(3);
    // Release the stream
    display_->streamIds = {1001};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: AttachBufferQueue
  * @tc.desc: AttachBufferQueue, producer is nullptr.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestHdiFuncTest, camera_hdi_0212)
{
    std::cout << "==========[test log] AttachBufferQueue, producer is nullptr." << std::endl;
    display_->AchieveStreamOperator();
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] GetStreamOperator success. " << std::endl;
    } else {
        std::cout << "==========[test log] GetStreamOperator fail, rc = " << display_->rc << std::endl;
    }
    std::shared_ptr<IBufferProducer> producer = nullptr;
    // Create data stream
    display_->streamInfo = std::make_shared<OHOS::Camera::StreamInfo>();
    display_->streamInfo->streamId_ = 1001;
    display_->streamInfo->width_ = 640; // 640:picture width
    display_->streamInfo->height_ = 480; // 480:picture height
    display_->streamInfo->format_ = CAMERA_FORMAT_YUYV_422_PKG;
    display_->streamInfo->datasapce_ = 8; // 8:picture datasapce
    display_->streamInfo->intent_ = Camera::PREVIEW;
    display_->streamInfo->tunneledMode_ = 5; // 5:tunnel mode
    display_->streamInfos.push_back(display_->streamInfo);
    display_->rc = display_->streamOperator->CreateStreams(display_->streamInfos);
    std::cout << "==========[test log] streamOperator->CreateStreams's RetCode = ";
    std::cout << display_->rc << std::endl;
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] CreateStreams success. " << std::endl;
    } else {
        std::cout << "==========[test log] CreateStreams fail, rc = " << display_->rc << std::endl;
    }
    display_->rc = display_->streamOperator->AttachBufferQueue(display_->streamInfo->streamId_, nullptr);
    EXPECT_EQ(true, display_->rc != Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] AttachBufferQueue success. " << std::endl;
    } else {
        std::cout << "==========[test log] AttachBufferQueue fail, rc = " << display_->rc << std::endl;
    }
    sleep(3);
    // release stream
    display_->streamIds = {1001};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: DetachBufferQueue
  * @tc.desc: DetachBufferQueue, normal input.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestHdiFuncTest, camera_hdi_0220)
{
    std::cout << "==========[test log] DetachBufferQueue, normal input." << std::endl;
    display_->AchieveStreamOperator();
    // Create data stream
    std::shared_ptr<IBufferProducer> producer = IBufferProducer::CreateBufferQueue();
    producer->SetQueueSize(8); // 8:set bufferQueue size
    if (producer->GetQueueSize() != 8) { // 8:get bufferQueue size
        std::cout << "~~~~~~~" << std::endl;
    }
    auto callback = [this](std::shared_ptr<SurfaceBuffer> b) {
        display_->BufferCallback(b, display_->preview_mode);
        return;
    };
    producer->SetCallback(callback);
    display_->streamInfo = std::make_shared<OHOS::Camera::StreamInfo>();
    display_->streamInfo->streamId_ = 1001;
    display_->streamInfo->width_ = 640; // 640:picture width
    display_->streamInfo->height_ = 480; // 480:picture height
    display_->streamInfo->format_ = CAMERA_FORMAT_YUYV_422_PKG;
    display_->streamInfo->datasapce_ = 8; // 8:picture datasapce
    display_->streamInfo->intent_ = Camera::PREVIEW;
    display_->streamInfo->tunneledMode_ = 5; // 5:tunnel mode
    display_->streamInfos.push_back(display_->streamInfo);
    display_->rc = display_->streamOperator->CreateStreams(display_->streamInfos);
    std::cout << "==========[test log] streamOperator->CreateStreams's RetCode = ";
    std::cout << display_->rc << std::endl;
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    std::cout << "==========[test log] CreateStreams success. " << std::endl;
    display_->rc = display_->streamOperator->AttachBufferQueue(display_->streamInfo->streamId_, producer);
    std::cout << "==========[test log] streamOperator->AttachBufferQueue's RetCode = ";
    std::cout << display_->rc << std::endl;
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    sleep(3);
    std::cout << "==========[test log] AttachBufferQueue success. " << std::endl;
    display_->rc = display_->streamOperator->DetachBufferQueue(display_->streamInfo->streamId_);
    std::cout << "==========[test log] streamOperator->DetachBufferQueue's RetCode = ";
    std::cout << display_->rc << std::endl;
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] DetachBufferQueue success. " << std::endl;
    } else {
        std::cout << "==========[test log] DetachBufferQueue fail, rc = " << display_->rc << std::endl;
    }
    // release stream
    display_->streamIds = {1001};
    display_->StopStream(display_->captureIds, display_->streamIds);
}

/**
  * @tc.name: DetachBufferQueue
  * @tc.desc: DetachBufferQueue, streamID is not exist.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestHdiFuncTest, camera_hdi_0221)
{
    std::cout << "==========[test log] DetachBufferQueue, streamID is not exist." << std::endl;
    display_->AchieveStreamOperator();
    // Create data stream
    std::shared_ptr<IBufferProducer> producer = IBufferProducer::CreateBufferQueue();
    producer->SetQueueSize(8); // 8:set bufferQueue size
    if (producer->GetQueueSize() != 8) { // 8:get bufferQueue size
        std::cout << "~~~~~~~" << std::endl;
    }
    auto callback = [this](std::shared_ptr<SurfaceBuffer> b) {
        display_->BufferCallback(b, display_->preview_mode);
        return;
    };
    producer->SetCallback(callback);
    display_->streamInfo = std::make_shared<OHOS::Camera::StreamInfo>();
    display_->streamInfo->streamId_ = 1001;
    display_->streamInfo->width_ = 640; // 640:picture width
    display_->streamInfo->height_ = 480; // 480:picture height
    display_->streamInfo->format_ = CAMERA_FORMAT_YUYV_422_PKG;
    display_->streamInfo->datasapce_ = 8; // 8:picture datasapce
    display_->streamInfo->intent_ = Camera::PREVIEW;
    display_->streamInfo->tunneledMode_ = 5; // 5:tunnel mode
    display_->streamInfos.push_back(display_->streamInfo);
    display_->rc = display_->streamOperator->CreateStreams(display_->streamInfos);
    std::cout << "==========[test log] streamOperator->CreateStreams's RetCode = ";
    std::cout << display_->rc << std::endl;
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    std::cout << "==========[test log] CreateStreams success. " << std::endl;
    display_->rc = display_->streamOperator->AttachBufferQueue(display_->streamInfo->streamId_, producer);
    std::cout << "==========[test log] streamOperator->AttachBufferQueue's RetCode = " << display_->rc << std::endl;
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
    std::cout << "==========[test log] AttachBufferQueue success. " << std::endl;
    sleep(3);

    display_->rc = display_->streamOperator->DetachBufferQueue(0);
    std::cout << "==========[test log] streamOperator->DetachBufferQueue's RetCode = " << display_->rc << std::endl;
    EXPECT_EQ(true, display_->rc != Camera::NO_ERROR);
    if (display_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] DetachBufferQueue success. " << std::endl;
    } else {
        std::cout << "==========[test log] DetachBufferQueue fail, rc = " << display_->rc << std::endl;
    }
    // release stream
    display_->streamIds = {1001};
    display_->StopStream(display_->captureIds, display_->streamIds);
}