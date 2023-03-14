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

#include "utest_stream_operator_impl.h"
#include "stream_operator_callback.h"
#include "istream_operator_callback.h"

#define SURFACE_ID (12345 + 666 + 2333)
const int CAMERA_BUFFER_QUEUE_IPC = 654320;

void StreamOperatorImplTest::SetUpTestCase(void)
{
    std::cout << "Camera::StreamOperatorImpl SetUpTestCase" << std::endl;
}

void StreamOperatorImplTest::TearDownTestCase(void)
{
    std::cout << "Camera::StreamOperatorImpl TearDownTestCase" << std::endl;
}

void StreamOperatorImplTest::SetUp(void)
{
    bool ret = InitCameraHost();
    if (!ret) {
        std::cout << "StreamOperatorImplTest init camerahost failed" << std::endl;
        return;
    }

    ret = GetCameraIds();
    if (!ret) {
        std::cout << "StreamOperatorImplTest init GetCameraIds failed" << std::endl;
        return;
    }

    ret = GetCameraDevice();
    if (!ret) {
        std::cout << "StreamOperatorImplTest init GetCameraDevice failed" << std::endl;
        return;
    }

    ret = GetStreamOperator();
    if (!ret) {
        std::cout << "StreamOperatorImplTest init GetStreamOperator failed" << std::endl;
        return;
    }
}

void StreamOperatorImplTest::TearDown(void)
{
    std::cout << "Camera::StreamOperatorImpl TearDown.." << std::endl;
}

HWTEST_F(StreamOperatorImplTest, UTestIsStreamsSupported, TestSize.Level0)
{
    EXPECT_EQ(false, cameraDevice_ == nullptr);
    bool rc = GetStreamOperator();
    EXPECT_EQ(true, rc);

    OperationMode mode = NORMAL;
    std::shared_ptr<CameraStandard::CameraMetadata> modeSetting = std::make_shared<CameraStandard::CameraMetadata>(2, 128);
    int64_t expoTime = 0;
    modeSetting->addEntry(OHOS_SENSOR_EXPOSURE_TIME, &expoTime, 1);
    int64_t colorGains[4] = {0};
    modeSetting->addEntry(OHOS_SENSOR_COLOR_CORRECTION_GAINS, &colorGains, 4);
    std::shared_ptr<StreamInfo> streamInfo = std::make_shared<StreamInfo>();
    streamInfo->width_ = 1280;
    streamInfo->height_ = 720;
    streamInfo->format_ = CAMERA_FORMAT_RGBA_8888;
    streamInfo->datasapce_ = 10;
    streamInfo->intent_ = PREVIEW;
    streamInfo->tunneledMode_ = 5;
    std::vector<std::shared_ptr<StreamInfo>> infos;
    infos.push_back(streamInfo);
    StreamSupportType pType;
    OHOS::Camera::CamRetCode ret = streamOperator_->IsStreamsSupported(
        mode, modeSetting, infos, pType);
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);
}

HWTEST_F(StreamOperatorImplTest, UTestCapture, TestSize.Level0)
{
    OperationMode operationMode = NORMAL;
    StreamSupportType supportType;
    std::vector<std::shared_ptr<StreamInfo>> streamInfos;
    std::shared_ptr<StreamInfo> streamInfo = std::make_shared<StreamInfo>();
    streamInfo->streamId_ = 1005;
    streamInfo->width_ = 640;
    streamInfo->height_ = 480;
    streamInfo->format_ = PIXEL_FMT_YCRCB_420_SP;
    streamInfo->datasapce_ = 8;
    streamInfo->intent_ = PREVIEW;
    std::shared_ptr<StreamConsumer> previewConsumer = std::make_shared<StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    streamInfo->bufferQueue_ = previewConsumer->CreateProducer(
        [](OHOS::SurfaceBuffer* buffer) {
        std::cout << "received a preview buffer ..." << std::endl; });
#else
    streamInfo->bufferQueue_ = previewConsumer->CreateProducer(
        [](void* addr, uint32_t size) {
        std::cout << "received a preview buffer ..." << std::endl; });
#endif
    streamInfo->bufferQueue_->SetQueueSize(8);
    streamInfo->tunneledMode_ = 5;
    streamInfos.push_back(streamInfo);

    OHOS::Camera::CamRetCode ret = streamOperator_->CreateStreams(streamInfos);
    std::cout << "streamOperator->CreateStreams = " << ret << std::endl;
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);

    std::vector<std::string> cameraIds;
    ret = cameraHost_->GetCameraIds(cameraIds);
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);

    std::shared_ptr<CameraAbility> ability = nullptr;
    std::string cameraId = cameraIds.front();
    ret = cameraHost_->GetCameraAbility(cameraId, ability);
    ret = streamOperator_->CommitStreams(NORMAL, ability);
    EXPECT_EQ(true, ret == Camera::NO_ERROR);

    int captureId = 2001;
    std::shared_ptr<OHOS::Camera::CaptureInfo> captureInfo = std::make_shared<OHOS::Camera::CaptureInfo>();
    captureInfo->streamIds_ = {streamInfo->streamId_};
    captureInfo->captureSetting_ = ability;
    captureInfo->enableShutterCallback_ = false;
    ret = streamOperator_->Capture(captureId, captureInfo, true);
    std::cout << "streamOperator->Capture = " << ret << std::endl;
    EXPECT_EQ(true, ret == Camera::NO_ERROR);
    sleep(5);

    ret = streamOperator_->CancelCapture(captureId);
    EXPECT_EQ(true, ret == Camera::NO_ERROR);

    std::vector<int> streamIds = {1005};
    ret = streamOperator_->ReleaseStreams(streamIds);
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);
}

HWTEST_F(StreamOperatorImplTest, UTestCreateStreams, TestSize.Level0)
{
    OperationMode operationMode = NORMAL;
    StreamSupportType supportType;
    std::vector<std::shared_ptr<StreamInfo>> streamInfos;
    std::shared_ptr<StreamInfo> streamInfo = std::make_shared<StreamInfo>();
    streamInfo->streamId_ = 1001;
    streamInfo->width_ = 720;
    streamInfo->height_ = 480;
    streamInfo->intent_ = PREVIEW;
    streamInfo->format_ = PIXEL_FMT_YCRCB_420_SP;
    streamInfo->datasapce_ = 8;
    StreamConsumer previewConsumer;
    streamInfo->tunneledMode_ = 5;
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    streamInfo->bufferQueue_ = previewConsumer.CreateProducer(
        [](OHOS::SurfaceBuffer* buffer) {
        std::cout << "received a preview buffer ..." << std::endl; });
#else
    streamInfo->bufferQueue_ = previewConsumer.CreateProducer(
        [](void* addr, uint32_t size) {
        std::cout << "received a preview buffer ..." << std::endl; });
#endif
    streamInfos.push_back(streamInfo);

    std::shared_ptr<StreamInfo> streamInfoSnapshot = std::make_shared<StreamInfo>();
    streamInfoSnapshot->streamId_ = 1002;
    streamInfoSnapshot->width_ = 720;
    streamInfoSnapshot->height_ = 480;
    streamInfoSnapshot->format_ = PIXEL_FMT_YCRCB_420_SP;
    streamInfoSnapshot->datasapce_ = 8;
    streamInfoSnapshot->intent_ = STILL_CAPTURE;
    StreamConsumer snapshotConsumer;
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    streamInfoSnapshot->bufferQueue_ = snapshotConsumer.CreateProducer(
            [](OHOS::SurfaceBuffer* buffer) {
               std::cout << "received a snapshot buffer ..." << std::endl; });
#else
    streamInfoSnapshot->bufferQueue_ = snapshotConsumer.CreateProducer(
            [](void* addr, uint32_t size) {
               std::cout << "received a snapshot buffer ..." << std::endl; });
#endif
    streamInfoSnapshot->tunneledMode_ = 5;
    streamInfos.push_back(streamInfoSnapshot);

    OHOS::Camera::CamRetCode ret = streamOperator_->CreateStreams(streamInfos);
    std::cout << "streamOperator->CreateStreams = " << ret << std::endl;
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);

    std::vector<int> streamIds = {1001, 1002};
    ret = streamOperator_->ReleaseStreams(streamIds);
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);
}
#ifdef CAMERA_BUILT_ON_OHOS_LITE
HWTEST_F(StreamOperatorImplTest, UTestAttachBufferQueue, TestSize.Level0)
{
    OperationMode operationMode = NORMAL;
    StreamSupportType supportType;
    std::vector<std::shared_ptr<StreamInfo>> streamInfos;
    std::shared_ptr<StreamInfo> streamInfo = std::make_shared<StreamInfo>();
    streamInfo->streamId_ = 1011;
    streamInfo->width_ = 720;
    streamInfo->height_ = 480;
    streamInfo->format_ = PIXEL_FMT_YCRCB_420_SP;
    streamInfo->datasapce_ = 8;
    streamInfo->intent_ = PREVIEW;
    StreamConsumer previewConsumer;
    streamInfo->bufferQueue_ = previewConsumer.CreateProducer(
        [](OHOS::SurfaceBuffer* buffer) {
        std::cout << "received a preview buffer ..." << std::endl; });
    streamInfo->tunneledMode_ = 5;
    streamInfos.push_back(streamInfo);

    std::shared_ptr<StreamInfo> streamInfoSnapshot = std::make_shared<StreamInfo>();
    streamInfoSnapshot->streamId_ = 1012;
    streamInfoSnapshot->width_ = 1920;
    streamInfoSnapshot->datasapce_ = 8;
    streamInfoSnapshot->height_ = 960;
    streamInfoSnapshot->intent_ = STILL_CAPTURE;
    streamInfoSnapshot->format_ = PIXEL_FMT_YCRCB_420_SP;
    StreamConsumer snapshotConsumer;
    streamInfoSnapshot->bufferQueue_ = snapshotConsumer.CreateProducer(
            [](OHOS::SurfaceBuffer* buffer) {
               std::cout << "received a snapshot buffer ..." << std::endl; });
    streamInfoSnapshot->tunneledMode_ = 5;
    streamInfos.push_back(streamInfoSnapshot);

    OHOS::Camera::CamRetCode ret = streamOperator_->CreateStreams(streamInfos);
    std::cout << "streamOperator->CreateStreams = " << ret << std::endl;
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);

    StreamConsumer preview_consumer;
    OHOS::sptr<OHOS::IBufferProducer> producer =
        preview_consumer.CreateProducer([](OHOS::SurfaceBuffer* buffer) {
        std::cout << "receive a frame..." << std::endl;
    });
    ret = streamOperator_->AttachBufferQueue(streamInfo->streamId_, producer);
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);

    ret = streamOperator_->DetachBufferQueue(streamInfo->streamId_);
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);

    std::vector<int> streamIds = {1011, 1012};
    ret = streamOperator_->ReleaseStreams(streamIds);
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);
}
#else
HWTEST_F(StreamOperatorImplTest, UTestAttachBufferQueue, TestSize.Level0)
{
    OperationMode operationMode = NORMAL;
    StreamSupportType supportType;
    std::vector<std::shared_ptr<StreamInfo>> streamInfos;
    std::shared_ptr<StreamInfo> streamInfo = std::make_shared<StreamInfo>();
    streamInfo->streamId_ = 1011;
    streamInfo->width_ = 720;
    streamInfo->height_ = 480;
    streamInfo->format_ = PIXEL_FMT_YCRCB_420_SP;
    streamInfo->datasapce_ = 8;
    streamInfo->intent_ = PREVIEW;
    StreamConsumer previewConsumer;
    streamInfo->bufferQueue_ = previewConsumer.CreateProducer(
        [](void* addr, uint32_t size) {
        std::cout << "received a preview buffer ..." << std::endl; });
    streamInfo->tunneledMode_ = 5;
    streamInfos.push_back(streamInfo);

    std::shared_ptr<StreamInfo> streamInfoSnapshot = std::make_shared<StreamInfo>();
    streamInfoSnapshot->streamId_ = 1012;
    streamInfoSnapshot->width_ = 1920;
    streamInfoSnapshot->datasapce_ = 8;
    streamInfoSnapshot->height_ = 960;
    streamInfoSnapshot->intent_ = STILL_CAPTURE;
    streamInfoSnapshot->format_ = PIXEL_FMT_YCRCB_420_SP;
    StreamConsumer snapshotConsumer;
    streamInfoSnapshot->bufferQueue_ = snapshotConsumer.CreateProducer(
            [](void* addr, uint32_t size) {
               std::cout << "received a snapshot buffer ..." << std::endl; });
    streamInfoSnapshot->tunneledMode_ = 5;
    streamInfos.push_back(streamInfoSnapshot);

    OHOS::Camera::CamRetCode ret = streamOperator_->CreateStreams(streamInfos);
    std::cout << "streamOperator->CreateStreams = " << ret << std::endl;
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);

    StreamConsumer preview_consumer;
    OHOS::sptr<OHOS::IBufferProducer> producer =
        preview_consumer.CreateProducer([this](void* addr, uint32_t size) {
        std::cout << "receive a frame..." << std::endl;
    });
    ret = streamOperator_->AttachBufferQueue(streamInfo->streamId_, producer);
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);

    ret = streamOperator_->DetachBufferQueue(streamInfo->streamId_);
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);

    std::vector<int> streamIds = {1011, 1012};
    ret = streamOperator_->ReleaseStreams(streamIds);
    EXPECT_EQ(true, ret == OHOS::Camera::NO_ERROR);
}
#endif
