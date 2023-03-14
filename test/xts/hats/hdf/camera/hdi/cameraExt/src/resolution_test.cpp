/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "resolution_test.h"

using namespace OHOS;
using namespace std;
using namespace testing::ext;
using namespace OHOS::Camera;

void ResolutionTest::SetUpTestCase(void) {}
void ResolutionTest::TearDownTestCase(void) {}
void ResolutionTest::SetUp(void)
{
    Test_ = std::make_shared<OHOS::Camera::Test>();
    Test_->Init();
    Test_->Open();
}
void ResolutionTest::TearDown(void)
{
    Test_->Close();
}

/**
  * @tc.name: preview and capture
  * @tc.desc: Preview and still_capture streams, Commit 2 streams together,
  * which Preview's resolution is 640 * 480 and Capture's resolution is 1280 * 960
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(ResolutionTest, Camera_Resolution_0001, TestSize.Level3)
{
    std::cout << "==========[test log]check Capture: Preview and still_capture streams.";
    std::cout << "which Preview's resolution is 640 * 480";
    std::cout << "and Capture's resolution is 1280 * 960" << std::endl;
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(true, Test_->rc == Camera::NO_ERROR);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]GetStreamOperator success." << std::endl;
    } else {
        std::cout << "==========[test log]GetStreamOperator fail, rc = " << Test_->rc << std::endl;
    }
    // Configure preview stream information
    Test_->streamInfo_pre = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo_pre->streamId_ = Test_->streamId_preview;
    Test_->streamInfo_pre->width_ = 640; // 640:width of stream
    Test_->streamInfo_pre->height_ = 480; // 480: height of stream
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo_pre->format_ = IMAGE_PIXEL_FORMAT_NV21;
#else
    Test_->streamInfo_pre->format_ = PIXEL_FMT_YCRCB_420_SP;
#endif
    Test_->streamInfo_pre->datasapce_ = 8; // 8:datasapce of stream
    Test_->streamInfo_pre->intent_ = Camera::PREVIEW;
    Test_->streamInfo_pre->tunneledMode_ = 5; // 5:tunneledMode of stream
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> preview_consumer =
        std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo_pre->bufferQueue_ = preview_consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo_pre->bufferQueue_ = preview_consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo_pre->bufferQueue_->SetQueueSize(8); // 8:size of bufferQueue
    Test_->consumerMap_[Camera::PREVIEW] = preview_consumer;
    Test_->streamInfos.push_back(Test_->streamInfo_pre);
    // Configure capture stream information
    Test_->streamInfo_capture = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo_capture->streamId_ = Test_->streamId_capture;
    Test_->streamInfo_capture->width_ = 1280; // 1280:width of stream
    Test_->streamInfo_capture->height_ = 960; // 960: height of stream
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo_capture->format_ = IMAGE_PIXEL_FORMAT_NV21;
#else
    Test_->streamInfo_capture->format_ = PIXEL_FMT_YCRCB_420_SP;
#endif
    Test_->streamInfo_capture->datasapce_ = 8; // 8:datasapce of stream
    Test_->streamInfo_capture->intent_ = Camera::STILL_CAPTURE;
    Test_->streamInfo_capture->encodeType_ = ENCODE_TYPE_JPEG;
    Test_->streamInfo_capture->tunneledMode_ = 5; // 5:tunneledMode of stream
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer_capture =
        std::make_shared<OHOS::Camera::Test::StreamConsumer>();
    std::cout << "==========[test log]received a capture buffer ... 1" << std::endl;
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo_capture->bufferQueue_ = consumer_capture->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo_capture->bufferQueue_ = consumer_capture->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo_capture->bufferQueue_->SetQueueSize(8); // 8:bufferqueue size
    Test_->consumerMap_[Camera::STILL_CAPTURE] = consumer_capture;
    Test_->streamInfos.push_back(Test_->streamInfo_capture);
    // distribution start
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]CreateStreams success." << std::endl;
    } else {
        std::cout << "==========[test log]CreateStreams fail, rc = " << Test_->rc << std::endl;
    }
    Test_->rc = Test_->streamOperator->CommitStreams(Camera::NORMAL, Test_->ability);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]CommitStreams success." << std::endl;
    } else {
        std::cout << "==========[test log]CommitStreams fail, rc = " << Test_->rc << std::endl;
    }
    sleep(2); // 2:The program waits two seconds
    std::vector<std::shared_ptr<Camera::StreamInfo>>().swap(Test_->streamInfos);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture the capture stream
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, true);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_capture};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_capture};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    preview_consumer->StopConsumer();
    consumer_capture->StopConsumer();
}

/**
  * @tc.name: preview and capture
  * @tc.desc: Preview and Video streams, Commit 2 streams together,
  * which Preview's resolving power is 853 * 480 and Video's resolving power is 1280 * 720
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(ResolutionTest, Camera_Resolution_0002, TestSize.Level3)
{
    std::cout << "==========[test log]check Capture: Preview and Video streams.";
    std::cout << "which Preview's resolution is 853 * 480";
    std::cout << "and Video's resolution is 1280 * 720" << std::endl;
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(true, Test_->rc == Camera::NO_ERROR);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]GetStreamOperator success." << std::endl;
    } else {
        std::cout << "==========[test log]GetStreamOperator fail, rc = " << Test_->rc << std::endl;
    }
    // Configure preview stream information
    Test_->streamInfo_pre = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo_pre->streamId_ = Test_->streamId_preview;
    Test_->streamInfo_pre->width_ = 853; // 853:width of stream
    Test_->streamInfo_pre->height_ = 480; // 480: height of stream
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo_pre->format_ = IMAGE_PIXEL_FORMAT_NV21;
#else
    Test_->streamInfo_pre->format_ = PIXEL_FMT_YCRCB_420_SP;
#endif
    Test_->streamInfo_pre->datasapce_ = 8; // 8:datasapce of stream
    Test_->streamInfo_pre->intent_ = Camera::PREVIEW;
    Test_->streamInfo_pre->tunneledMode_ = 5; // 5:tunneledMode of stream
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> preview_consumer =
        std::make_shared<OHOS::Camera::Test::StreamConsumer>();
    std::cout << "==========[test log]received a preview buffer ... 0" << std::endl;
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo_pre->bufferQueue_ = preview_consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo_pre->bufferQueue_ = preview_consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo_pre->bufferQueue_->SetQueueSize(8); // 8:size of bufferQueue
    Test_->consumerMap_[Camera::PREVIEW] = preview_consumer;
    Test_->streamInfos.push_back(Test_->streamInfo_pre);
    // Configure video stream information
    Test_->streamInfo_video = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo_video->streamId_ = Test_->streamId_video;
    Test_->streamInfo_video->width_ = 1280; // 1280:width of stream
    Test_->streamInfo_video->height_ = 720; // 960: height of stream
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo_video->format_ = IMAGE_PIXEL_FORMAT_NV21;
#else
    Test_->streamInfo_video->format_ = PIXEL_FMT_YCRCB_420_SP;
#endif
    Test_->streamInfo_video->datasapce_ = 8; // 8:datasapce of stream
    Test_->streamInfo_video->intent_ = Camera::VIDEO;
    Test_->streamInfo_video->encodeType_ = ENCODE_TYPE_H265;
    Test_->streamInfo_video->tunneledMode_ = 5; // 5:tunneledMode of stream
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer_video =
        std::make_shared<OHOS::Camera::Test::StreamConsumer>();
    std::cout << "==========[test log]received a video buffer ... 1" << std::endl;
    Test_->SaveVideoFile("video", nullptr, 0, 0);
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo_video->bufferQueue_ = consumer_video->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo_video->bufferQueue_ = consumer_video->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo_video->bufferQueue_->SetQueueSize(8); // 8:bufferqueue size
    Test_->consumerMap_[Camera::VIDEO] = consumer_video;
    Test_->streamInfos.push_back(Test_->streamInfo_video);
    // distribution start
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]CreateStreams success." << std::endl;
    } else {
        std::cout << "==========[test log]CreateStreams fail, rc = " << Test_->rc << std::endl;
    }
    Test_->rc = Test_->streamOperator->CommitStreams(Camera::NORMAL, Test_->ability);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]CommitStreams success." << std::endl;
    } else {
        std::cout << "==========[test log]CommitStreams fail, rc = " << Test_->rc << std::endl;
    }
    sleep(2); // 2:The program waits two seconds
    std::vector<std::shared_ptr<Camera::StreamInfo>>().swap(Test_->streamInfos);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture video stream，Continuous capture
    Test_->StartCapture(Test_->streamId_video, Test_->captureId_video, false, true);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_video};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_video};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    preview_consumer->StopConsumer();
    consumer_video->StopConsumer();
}

/**
  * @tc.name: preview and capture
  * @tc.desc: Preview and Video streams, Commit 2 streams together,
  * which Preview's resolving power is 640 * 360 and Video's resolving power is 1280 * 720
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(ResolutionTest, Camera_Resolution_0003, TestSize.Level3)
{
    std::cout << "==========[test log]check Capture: Preview and Video streams.";
    std::cout << "which Preview's resolution is 640 * 360";
    std::cout << "and Video's resolution is 1280 * 720" << std::endl;
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(true, Test_->rc == Camera::NO_ERROR);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]GetStreamOperator success." << std::endl;
    } else {
        std::cout << "==========[test log]GetStreamOperator fail, rc = " << Test_->rc << std::endl;
    }
    // Configure preview stream information
    Test_->streamInfo_pre = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo_pre->streamId_ = Test_->streamId_preview;
    Test_->streamInfo_pre->width_ = 640; // 640:width of stream
    Test_->streamInfo_pre->height_ = 360; // 360: height of stream
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo_pre->format_ = IMAGE_PIXEL_FORMAT_NV21;
#else
    Test_->streamInfo_pre->format_ = PIXEL_FMT_YCRCB_420_SP;
#endif
    Test_->streamInfo_pre->datasapce_ = 8; // 8:datasapce of stream
    Test_->streamInfo_pre->intent_ = Camera::PREVIEW;
    Test_->streamInfo_pre->tunneledMode_ = 5; // 5:tunneledMode of stream
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> preview_consumer =
        std::make_shared<OHOS::Camera::Test::StreamConsumer>();
    std::cout << "==========[test log]received a preview buffer ... 0" << std::endl;
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo_pre->bufferQueue_ = preview_consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo_pre->bufferQueue_ = preview_consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo_pre->bufferQueue_->SetQueueSize(8); // 8:size of bufferQueue
    Test_->consumerMap_[Camera::PREVIEW] = preview_consumer;
    Test_->streamInfos.push_back(Test_->streamInfo_pre);
    // Configure video stream information
    Test_->streamInfo_video = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo_video->streamId_ = Test_->streamId_video;
    Test_->streamInfo_video->width_ = 1280; // 1280:width of stream
    Test_->streamInfo_video->height_ = 720; // 960: height of stream
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo_video->format_ = IMAGE_PIXEL_FORMAT_NV21;
#else
    Test_->streamInfo_video->format_ = PIXEL_FMT_YCRCB_420_SP;
#endif
    Test_->streamInfo_video->datasapce_ = 8; // 8:datasapce of stream
    Test_->streamInfo_video->intent_ = Camera::VIDEO;
    Test_->streamInfo_video->encodeType_ = ENCODE_TYPE_H265;
    Test_->streamInfo_video->tunneledMode_ = 5; // 5:tunneledMode of stream
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer_video =
        std::make_shared<OHOS::Camera::Test::StreamConsumer>();
    std::cout << "==========[test log]received a video buffer ... 1" << std::endl;
    Test_->SaveVideoFile("video", nullptr, 0, 0);
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo_video->bufferQueue_ = consumer_video->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo_video->bufferQueue_ = consumer_video->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo_video->bufferQueue_->SetQueueSize(8); // 8:bufferqueue size
    Test_->consumerMap_[Camera::VIDEO] = consumer_video;
    Test_->streamInfos.push_back(Test_->streamInfo_video);
    // distribution start
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]CreateStreams success." << std::endl;
    } else {
        std::cout << "==========[test log]CreateStreams fail, rc = " << Test_->rc << std::endl;
    }
    Test_->rc = Test_->streamOperator->CommitStreams(Camera::NORMAL, Test_->ability);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]CommitStreams success." << std::endl;
    } else {
        std::cout << "==========[test log]CommitStreams fail, rc = " << Test_->rc << std::endl;
    }
    sleep(2); // 2:The program waits two seconds
    std::vector<std::shared_ptr<Camera::StreamInfo>>().swap(Test_->streamInfos);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture video stream，Continuous capture
    Test_->StartCapture(Test_->streamId_video, Test_->captureId_video, false, true);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_video};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_video};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    preview_consumer->StopConsumer();
    consumer_video->StopConsumer();
}