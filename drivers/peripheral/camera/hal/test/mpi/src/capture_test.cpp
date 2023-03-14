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

using namespace OHOS;
using namespace std;
using namespace testing::ext;
using namespace OHOS::Camera;

void CaptureTest::SetUpTestCase(void) {}
void CaptureTest::TearDownTestCase(void) {}
void CaptureTest::SetUp(void)
{
    Test_ = std::make_shared<OHOS::Camera::Test>();
    Test_->Init();
    Test_->Open();
}
void CaptureTest::TearDown(void)
{
    Test_->Close();
}

/**
  * @tc.name: preview and capture
  * @tc.desc: Preview and still_capture streams, Commit 2 streams together, capture in order, isStreaming is true.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(CaptureTest, Camera_Capture_0001, TestSize.Level0)
{
    std::cout << "==========[test log]check Capture: Preview and still_capture streams.";
    std::cout << " Commit 2 streams together, capture in order, isStreaming is true." << std::endl;
    // Configure two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture the photo stream, continuous capture
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, true);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_capture};
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->streamIds.push_back(Test_->streamId_capture);
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
}

/**
  * @tc.name: preview and capture
  * @tc.desc: Preview + capture, then cloase camera, and preview + capture again.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(CaptureTest, Camera_Capture_0002, TestSize.Level2)
{
    std::cout << "==========[test log]Preview + capture, cloase camera, and preview + capture." << std::endl;
    // Configure two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture the photo stream, continuous capture
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, true);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_capture};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_capture};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->consumerMap_.clear();
    // the 2nd time configures two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture the photo stream, continuous capture
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, true);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_capture};
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->streamIds.push_back(Test_->streamId_capture);
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
}

/**
  * @tc.name: preview and capture
  * @tc.desc: Preview and still_capture streams + 3A, Commit 2 streams together, capture in order, isStreaming is true.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(CaptureTest, Camera_Capture_0003, TestSize.Level1)
{
    std::cout << "==========[test log]check Capture: Preview and still_capture streams + 3A,";
    std::cout  << "Commit 2 streams together, capture in order, isStreaming is true." << std::endl;
    // Configure two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters to increase exposure
    int32_t expo = 0xa0;
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    Test_->rc = Test_->cameraDevice->UpdateSettings(meta);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]check Capture: UpdateSettings success, for 10s." << std::endl;
    } else {
        std::cout << "==========[test log]check Capture: UpdateSettings fail, rc = " << Test_->rc << std::endl;
    }
    sleep(5);
    // Capture the photo stream, continuous capture
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, true);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_capture};
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->streamIds.push_back(Test_->streamId_capture);
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
}

/**
  * @tc.name: preview and capture
  * @tc.desc: Preview + capture, then switch to preview + video.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(CaptureTest, Camera_Capture_0004, TestSize.Level1)
{
    std::cout << "==========[test log]check Capture: Preview + capture, then switch to preview + video." << std::endl;
    std::cout << "==========[test log]check Capture: First, create preview + capture." << std::endl;
    // Configure two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture the photo stream, continuous capture
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, true);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_capture};
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->streamIds.push_back(Test_->streamId_capture);
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->consumerMap_.clear();
    std::cout << "==========[test log]check Capture: Next, switch to preview + video." << Test_->rc << std::endl;
    // Configure two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::VIDEO};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture video stream
    Test_->StartCapture(Test_->streamId_video, Test_->captureId_video, false, true);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_video};
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->streamIds.push_back(Test_->streamId_video);
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
}

/**
  * @tc.name: video cannot capture
  * @tc.desc: Preview + video, then capture a photo, expected not support.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(CaptureTest, Camera_Capture_0005, TestSize.Level2)
{
    std::cout << "==========[test log]check Capture: Preview + video, then capture a photo." << std::endl;
    std::cout << "==========[test log]check Capture: First, create Preview + video." << std::endl;
    // Configure two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::VIDEO};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture video stream
    Test_->StartCapture(Test_->streamId_video, Test_->captureId_video, false, true);

    // Start the capture stream
    std::shared_ptr<Camera::StreamInfo> streamInfo_capture = std::make_shared<Camera::StreamInfo>();
    streamInfo_capture->streamId_ = Test_->streamId_capture;
    streamInfo_capture->width_ = 640;
    streamInfo_capture->height_ = 480;
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    streamInfo_capture->format_ = IMAGE_PIXEL_FORMAT_NV21;
#else
    streamInfo_capture->format_ = PIXEL_FMT_YCRCB_420_SP;
#endif
    streamInfo_capture->datasapce_ = 8;
    streamInfo_capture->intent_ = Camera::STILL_CAPTURE;
    streamInfo_capture->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> capture_consumer =
        std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    streamInfo_capture->bufferQueue_ = capture_consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("capture", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    streamInfo_capture->bufferQueue_ = capture_consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("capture", addr, size);
    });
#endif
    streamInfo_capture->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Camera::STILL_CAPTURE] = capture_consumer;
    // Query whether the IsStreamsSupported interface supports
    Camera::StreamSupportType pType;
    std::shared_ptr<CameraStandard::CameraMetadata> modeSetting =
        std::make_shared<CameraStandard::CameraMetadata>(2, 128);
    int64_t expoTime = 0;
    modeSetting->addEntry(OHOS_SENSOR_EXPOSURE_TIME, &expoTime, 1);
    int64_t colorGains[4] = {0};
    modeSetting->addEntry(OHOS_SENSOR_COLOR_CORRECTION_GAINS, &colorGains, 4);
    std::vector<std::shared_ptr<Camera::StreamInfo>> stre;
    stre.push_back(streamInfo_capture);
    Test_->rc = Test_->streamOperator->IsStreamsSupported(Camera::NORMAL, modeSetting, stre, pType);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::cout << "ptype = " << pType << std::endl;
    EXPECT_EQ(true, pType == Camera::RE_CONFIGURED_REQUIRED);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_video};
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->streamIds.push_back(Test_->streamId_video);
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
}

/**
  * @tc.name: preview and capture
  * @tc.desc: Commit 2 streams together, Preview and still_capture streams, isStreaming is false.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(CaptureTest, Camera_Capture_0006, TestSize.Level0)
{
    std::cout << "==========[test log]check Capture: Commit 2 streams together,";
    std::cout << "Preview and still_capture streams, isStreaming is false." << std::endl;
    // Configure two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture the photo stream, single capture
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, false);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->streamIds.push_back(Test_->streamId_capture);
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
}

/**
  * @tc.name: preview and capture
  * @tc.desc: Commit 2 streams together, Preview and still_capture streams, isStreaming is false,
  * Do not stop the stream, multiple single capture
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(CaptureTest, Camera_Capture_0007, TestSize.Level0)
{
    std::cout << "==========[test log]check Capture: Commit 2 streams together,";
    std::cout << "Preview and still_capture streams, isStreaming is false, multiple single capture" << std::endl;
    // Configure two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture the photo stream, multiple single capture
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, false);
    sleep(1);
    Test_->StartCapture(Test_->streamId_capture, (Test_->captureId_capture) + 1, false, false);
    sleep(1);
    Test_->StartCapture(Test_->streamId_capture, (Test_->captureId_capture) + 2, false, false);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->streamIds.push_back(Test_->streamId_capture);
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
}

/**
  * @tc.name: preview and capture
  * @tc.desc: Commit 2 streams in order, Preview and still_capture streams.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(CaptureTest, Camera_Capture_0010, TestSize.Level1)
{
    std::cout << "==========[test log]Commit 2 streams in order, Preview and still_capture." << std::endl;
    // start preview stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // start stream
    Test_->intents = {Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // Start capture
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, true);
    // release stream
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_capture};
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->streamIds.push_back(Test_->streamId_capture);
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
}

/**
  * @tc.name: preview and capture
  * @tc.desc: Preview and still_capture streams, Commit 2 streams together, capture together, isStreaming is true.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(CaptureTest, Camera_Capture_0030, TestSize.Level2)
{
    std::cout << "==========[test log]check Capture: Preview and still_capture streams,";
    std::cout << " Commit 2 streams together, capture together, isStreaming is true." << std::endl;
    // Configure two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture the photo stream, continuous capture
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, true);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_capture};
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->streamIds.push_back(Test_->streamId_capture);
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
}

/**
  * @tc.name: preview and capture
  * @tc.desc: Preview and still_capture streams + 3A, Commit 2 streams together, capture together, isStreaming is true.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(CaptureTest, Camera_Capture_0040, TestSize.Level2)
{
    std::cout << "==========[test log]check Capture: Preview and still_capture streams + 3A,";
    std::cout << " Commit 2 streams together, capture together, isStreaming is true." << std::endl;
    // Configure two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters to increase exposure
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xa0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    Test_->rc = Test_->cameraDevice->UpdateSettings(meta);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]check Capture: UpdateSettings success, for 10s." << std::endl;
    } else {
        std::cout << "==========[test log]check Capture: UpdateSettings fail, rc = " << Test_->rc << std::endl;
    }
    sleep(5);
    // Capture the photo stream, continuous capture
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, true);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_capture};
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->streamIds.push_back(Test_->streamId_capture);
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
}