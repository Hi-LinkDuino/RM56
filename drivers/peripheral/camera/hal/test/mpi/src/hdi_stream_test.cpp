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

#include "hdi_stream_test.h"

using namespace OHOS;
using namespace std;
using namespace testing::ext;
using namespace OHOS::Camera;

void HdiStreamTest::SetUpTestCase(void) {}
void HdiStreamTest::TearDownTestCase(void) {}
void HdiStreamTest::SetUp(void)
{
    Test_ = std::make_shared<OHOS::Camera::Test>();
    Test_->Init();
}
void HdiStreamTest::TearDown(void)
{
    Test_->Close();
}

/**
  * @tc.name: IsStreamsSupported
  * @tc.desc: IsStreamsSupported, normal cameraId.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0120, TestSize.Level0)
{
    std::cout << "==========[test log]Check hdi: IsStreamsSupported, normal cameraId." << std::endl;
    // Turn on the camera
    Test_->Open();
    EXPECT_EQ(false, Test_->cameraDevice == nullptr);
    // Get streamOperator
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Configure mode and modeSetting
    Camera::OperationMode mode = Camera::NORMAL;
    std::shared_ptr<CameraStandard::CameraMetadata> modeSetting =
        std::make_shared<CameraStandard::CameraMetadata>(2, 128);
    int64_t expoTime = 0;
    modeSetting->addEntry(OHOS_SENSOR_EXPOSURE_TIME, &expoTime, 1);
    int64_t colorGains[4] = {0};
    modeSetting->addEntry(OHOS_SENSOR_COLOR_CORRECTION_GAINS, &colorGains, 4);
    // Configure stream information
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001; // 1001:streamId
    Test_->StreamInfoFormat();
    Test_->streamInfo->height_ = 480; // 480:height of stream
    Test_->streamInfo->width_ = 640; // 640:width of stream
    Test_->streamInfo->datasapce_ = 8;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    Camera::StreamSupportType pType;
    std::vector<std::shared_ptr<Camera::StreamInfo>> stre;
    stre.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->IsStreamsSupported(NORMAL, modeSetting, stre, pType);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    EXPECT_NE(pType, Camera::NOT_SUPPORTED);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]Check hdi: IsStreamsSupported success, pType = " << pType << std::endl;
    } else {
        std::cout << "==========[test log]Check hdi: IsStreamsSupported fail, rc = " << Test_->rc << std::endl;
    }
    consumer->StopConsumer();
}

/**
  * @tc.name: IsStreamsSupported
  * @tc.desc: IsStreamsSupported, input modeSetting nullptr.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0121, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: IsStreamsSupported, input modeSetting nullptr." << std::endl;
    // Turn on the camera
    Test_->Open();
    EXPECT_EQ(false, Test_->cameraDevice == nullptr);
    // Get streamOperator
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Configure stream information
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001; // 1001:streamId
    Test_->streamInfo->width_ = 640; // 640:width of stream
    Test_->streamInfo->height_ = 480; // 480:height of stream
    Test_->streamInfo->tunneledMode_ = 5;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;

    Camera::StreamSupportType pType;
    std::vector<std::shared_ptr<Camera::StreamInfo>> stre;
    stre.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->IsStreamsSupported(NORMAL, nullptr, stre, pType);
    EXPECT_EQ(true, Test_->rc == Camera::INVALID_ARGUMENT);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]Check hdi: IsStreamsSupported success." << std::endl;
    } else {
        std::cout << "==========[test log]Check hdi: IsStreamsSupported fail, rc = " << Test_->rc << std::endl;
    }
    consumer->StopConsumer();
}

/**
  * @tc.name: IsStreamsSupported
  * @tc.desc: IsStreamsSupported, input streamInfo nullptr.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0122, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: IsStreamsSupported, input mode abnormal." << std::endl;
    // Turn on the camera
    Test_->Open();
    EXPECT_EQ(false, Test_->cameraDevice == nullptr);
    // Get streamOperator
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    Camera::StreamSupportType pType;
    std::vector<std::shared_ptr<OHOS::Camera::StreamInfo>> infos = {};
    Test_->rc = Test_->streamOperator->IsStreamsSupported(NORMAL, Test_->ability, infos, pType);
    EXPECT_EQ(Test_->rc, Camera::INVALID_ARGUMENT);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]Check hdi: IsStreamsSupported success." << std::endl;
    } else {
        std::cout << "==========[test log]Check hdi: IsStreamsSupported fail, rc = " << Test_->rc << std::endl;
    }
}

/**
  * @tc.name: IsStreamsSupported
  * @tc.desc: IsStreamsSupported, normal cameraId, excepted not supported.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0123, TestSize.Level1)
{
    std::cout << "==========[test log] CAMERA_FORMAT_YVYU_422_PKG format is not supported." << std::endl;
    // Turn on the camera
    Test_->Open();
    EXPECT_EQ(false, Test_->cameraDevice == nullptr);
    // Get streamOperator
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]Check hdi: GetStreamOperator success." << std::endl;
    } else {
        std::cout << "==========[test log]Check hdi: GetStreamOperator fail, rc = " << Test_->rc << std::endl;
    }
    // Configure mode and modeSetting
    Camera::OperationMode mode = Camera::NORMAL;
    std::shared_ptr<CameraStandard::CameraMetadata> modeSetting =
        std::make_shared<CameraStandard::CameraMetadata>(2, 128);
    // Configure stream information
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->width_ = 640; // 640:width of stream
    Test_->streamInfo->height_ = 480; // 480:height of stream
    Test_->streamInfo->streamId_ = 1001; // 1001:streamId
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Camera::StreamSupportType pType;
    std::vector<std::shared_ptr<Camera::StreamInfo>> stre;
    stre.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->IsStreamsSupported(NORMAL, modeSetting, stre, pType);
    EXPECT_EQ(Test_->rc, Camera::INVALID_ARGUMENT);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]Check hdi: IsStreamsSupported success, pType = " << pType << std::endl;
    } else {
        std::cout << "==========[test log]Check hdi: IsStreamsSupported fail, rc = " << Test_->rc << std::endl;
    }
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0130, TestSize.Level0)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, success." << std::endl;
    Test_->Open();
    // Create and get streamOperator information
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 640;
    Test_->streamInfo->height_ = 480;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // release stream
    std::vector<int> streamIds;
    streamIds.push_back(Test_->streamInfo->streamId_);
    Test_->rc = Test_->streamOperator->ReleaseStreams(streamIds);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->streamId = -1, return error.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0131, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, StreamInfo->streamId = -1, error." << std::endl;
    Test_->Open();
    // Create and get streamOperator information
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = -1;
    Test_->streamInfo->width_ = 640;
    Test_->StreamInfoFormat();
    Test_->streamInfo->height_ = 480;
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(INVALID_ARGUMENT, Test_->rc);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->streamId = 2147483647, return error.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0132, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, StreamInfo->streamId = 2147483647, error" << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 2147483647;
    Test_->streamInfo->width_ = 640;
    Test_->streamInfo->height_ = 480;
    Test_->streamInfo->datasapce_ = 8;
    Test_->StreamInfoFormat();
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // release stream
    Test_->rc = Test_->streamOperator->ReleaseStreams({Test_->streamInfo->streamId_});
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->width = -1, return error.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0133, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, StreamInfo->width = -1, return error." << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = -1;
    Test_->streamInfo->height_ = 640;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(INVALID_ARGUMENT, Test_->rc);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->width = 2147483647, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0134, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, StreamInfo->width = 2147483647, success." << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 2147483647;
    Test_->streamInfo->height_ = 640;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // release stream
    Test_->rc = Test_->streamOperator->ReleaseStreams({Test_->streamInfo->streamId_});
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->height = -1, return error.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0135, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, StreamInfo->height = -1, return error." << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 1920;
    Test_->streamInfo->height_ = -1;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(INVALID_ARGUMENT, Test_->rc);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->height = 2147483647, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0136, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, StreamInfo->height = 2147483647, success" << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 1920;
    Test_->streamInfo->height_ = 2147483647;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // release stream
    Test_->rc = Test_->streamOperator->ReleaseStreams({Test_->streamInfo->streamId_});
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->format = -1, return error.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0137, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, StreamInfo->format = -1, return error." << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 1920;
    Test_->streamInfo->height_ = 1080;
    Test_->streamInfo->format_ = -1;
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    // Configure mode and modeSetting
    Camera::OperationMode mode = Camera::NORMAL;
    std::shared_ptr<CameraStandard::CameraMetadata> modeSetting =
        std::make_shared<CameraStandard::CameraMetadata>(2, 128);
    Camera::StreamSupportType pType;
    std::vector<std::shared_ptr<Camera::StreamInfo>> stre;
    stre.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->IsStreamsSupported(NORMAL, modeSetting, stre, pType);
    EXPECT_EQ(Test_->rc, Camera::INVALID_ARGUMENT);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]Check hdi: IsStreamsSupported success." << std::endl;
    } else {
        std::cout << "==========[test log]Check hdi: IsStreamsSupported fail, rc = " << Test_->rc << std::endl;
    }

    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(INVALID_ARGUMENT, Test_->rc);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->format = 2147483647, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0138, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, StreamInfo->format = 2147483647, success" << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 1920;
    Test_->streamInfo->height_ = 1080;
    Test_->streamInfo->format_ = 2147483647;
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // release stream
    Test_->rc = Test_->streamOperator->ReleaseStreams({Test_->streamInfo->streamId_});
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->datasapce = -1, return error.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0139, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, StreamInfo->datasapce = -1, error." << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Configure mode and modeSetting
    Camera::OperationMode mode = Camera::NORMAL;
    std::shared_ptr<CameraStandard::CameraMetadata> modeSetting = nullptr;
    // Configure stream information
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->datasapce_ = -1;
    Test_->streamInfo->height_ = 480;
    Test_->streamInfo->width_ = 640;
    Test_->StreamInfoFormat();
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Camera::StreamSupportType pType;
    std::vector<std::shared_ptr<Camera::StreamInfo>> stre;
    stre.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->IsStreamsSupported(NORMAL, modeSetting, stre, pType);
    EXPECT_EQ(Test_->rc, INVALID_ARGUMENT);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]Check hdi: IsStreamsSupported success." << std::endl;
    } else {
        std::cout << "==========[test log]Check hdi: IsStreamsSupported fail, rc = " << Test_->rc << std::endl;
    }
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->datasapce = 2147483647, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0140, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, datasapce = 2147483647, success" << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 1920;
    Test_->streamInfo->height_ = 1080;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 2147483647;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // release stream
    Test_->rc = Test_->streamOperator->ReleaseStreams({Test_->streamInfo->streamId_});
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->StreamIntent = Camera::PREVIEW, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0141, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, StreamIntent = Camera::PREVIEW, success." << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 1920;
    Test_->streamInfo->height_ = 1080;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // release stream
    Test_->rc = Test_->streamOperator->ReleaseStreams({Test_->streamInfo->streamId_});
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->StreamIntent = Camera::VIDEO, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0142, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, StreamIntent = Camera::VIDEO, success." << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 1920;
    Test_->streamInfo->height_ = 1080;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::VIDEO;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // release stream
    Test_->rc = Test_->streamOperator->ReleaseStreams({Test_->streamInfo->streamId_});
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->StreamIntent = Camera::STILL_CAPTURE, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0143, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, Camera::STILL_CAPTURE, success." << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 1920;
    Test_->streamInfo->height_ = 1080;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::STILL_CAPTURE;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // release stream
    Test_->rc = Test_->streamOperator->ReleaseStreams({Test_->streamInfo->streamId_});
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->StreamIntent = Camera::POST_VIEW;, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0144, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams,  Camera::POST_VIEW;, success." << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 1920;
    Test_->streamInfo->height_ = 1080;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::POST_VIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // release stream
    Test_->rc = Test_->streamOperator->ReleaseStreams({Test_->streamInfo->streamId_});
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->StreamIntent = Camera::ANALYZE;, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0145, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, Camera::ANALYZE;, success." << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 1920;
    Test_->streamInfo->height_ = 1080;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::ANALYZE;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // release stream
    Test_->rc = Test_->streamOperator->ReleaseStreams({Test_->streamInfo->streamId_});
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->StreamIntent = Camera::CUSTOM;, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0146, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, Camera::CUSTOM;, success." << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 1920;
    Test_->streamInfo->height_ = 1080;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::CUSTOM;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_NE(Test_->rc, Camera::NO_ERROR);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->tunneledMode = false, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0147, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, tunneledMode = false, success." << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 1920;
    Test_->streamInfo->height_ = 1080;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::CUSTOM;
    Test_->streamInfo->tunneledMode_ = 0;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::INSUFFICIENT_RESOURCES);
    consumer->StopConsumer();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->minFrameDuration = -1, return error.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0148, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, minFrameDuration = -1, return error." << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
}

/**
  * @tc.name: CreateStreams
  * @tc.desc: CreateStreams, StreamInfo->minFrameDuration = 2147483647, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0149, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CreateStreams, minFrameDuration = 2147483647, success." << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
}

/**
  * @tc.name: ReleaseStreams
  * @tc.desc: ReleaseStreams,streamID normal.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0160, TestSize.Level0)
{
    std::cout << "==========[test log]Check hdi: ReleaseStreams,streamID normal." << std::endl;
    Test_->Open();
    // Create and get streamOperator information
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 640;
    Test_->streamInfo->height_ = 480;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->tunneledMode_ = 5;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // release stream
    Test_->rc = Test_->streamOperator->ReleaseStreams({Test_->streamInfo->streamId_});
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    consumer->StopConsumer();
}

/**
  * @tc.name: ReleaseStreams
  * @tc.desc: ReleaseStreams-> streamID = -1, expected success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0161, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: ReleaseStreams-> streamID = -1, expected success." << std::endl;
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // release capture and stream
    Test_->streamOperator->CancelCapture(Test_->captureId_preview);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    Test_->rc = Test_->streamOperator->ReleaseStreams({-1});
    EXPECT_EQ(true, Test_->rc != Camera::NO_ERROR);
    std::cout << "streamOperator->ReleaseStreams's rc " << Test_->rc << std::endl;
    Test_->rc = Test_->streamOperator->ReleaseStreams({Test_->streamId_preview});
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::cout << "streamOperator->ReleaseStreams's rc " << Test_->rc << std::endl;
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: CommitStreams
  * @tc.desc: CommitStreams, input normal.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0170, TestSize.Level0)
{
    std::cout << "==========[test log]Check hdi: CommitStreams, input normal." << std::endl;
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // release stream
    Test_->captureIds = {};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: CommitStreams
  * @tc.desc: CommitStreams, modeSetting is nullptr.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0171, TestSize.Level1)
{
    std::cout << "==========[test log]Check hdi: CommitStreams, modeSetting is nullptr." << std::endl;
    Test_->Open();
    // Create and get streamOperator information
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = Test_->streamId_preview;
    Test_->streamInfo->width_ = 640;
    Test_->streamInfo->height_ = 480;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
    Test_->streamInfo->intent_ = Camera::PREVIEW;
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->streamInfo->tunneledMode_ = 5;
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::shared_ptr<CameraStandard::CameraMetadata> modeSetting = nullptr;
    // Distribution stream
    Test_->rc = Test_->streamOperator->CommitStreams(Camera::NORMAL, nullptr);
    EXPECT_EQ(Test_->rc, INVALID_ARGUMENT);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]Check hdi: CommitStreams success." << std::endl;
    } else {
        std::cout << "==========[test log]Check hdi: CommitStreams fail, rc = " << Test_->rc << std::endl;
    }
    // release stream
    Test_->captureIds = {};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    consumer->StopConsumer();
}

/**
  * @tc.name: GetStreamAttributes
  * @tc.desc: GetStreamAttributes, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0180, TestSize.Level0)
{
    std::cout << "==========[test log]Check hdi: GetStreamAttributes, success." << std::endl;
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);

    std::vector<std::shared_ptr<Camera::StreamAttribute>> attributes;
    Test_->rc = Test_->streamOperator->GetStreamAttributes(attributes);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]Check hdi: GetStreamAttributes success." << std::endl;
    } else {
        std::cout << "==========[test log]Check hdi: GetStreamAttributes fail, rc = " << Test_->rc << std::endl;
    }
    // release stream
    Test_->rc = Test_->streamOperator->ReleaseStreams({Test_->streamId_preview});
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::cout << "streamOperator->ReleaseStreams's rc " << Test_->rc << std::endl;
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: Capture
  * @tc.desc: Capture, input normal.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0190, TestSize.Level0)
{
    std::cout << "==========[test log]Check hdi: Capture, input normal." << std::endl;
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: Capture
  * @tc.desc: Preview, Capture->captureInfo->streamID = -1 ,return error.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
#if 1
HWTEST_F(HdiStreamTest, Camera_Hdi_0191, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: Preview, captureInfo->streamID = -1 ,return error." << std::endl;
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    int captureId = 2001;
    Test_->captureInfo = std::make_shared<Camera::CaptureInfo>();
    Test_->captureInfo->streamIds_ = {-1};
    Test_->captureInfo->captureSetting_ = Test_->ability;
    Test_->captureInfo->enableShutterCallback_ = true;
    Test_->rc = Test_->streamOperator->Capture(captureId, Test_->captureInfo, true);
    EXPECT_EQ(INVALID_ARGUMENT, Test_->rc);
    sleep(5);
    Test_->streamOperator->CancelCapture(captureId);
    // release stream
    Test_->captureIds = {};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}
#endif

/**
  * @tc.name: Capture
  * @tc.desc: Preview, Capture->captureInfo->streamID = 2147483647 ,return success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0192, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: captureInfo->streamID = 2147483647 ,return success." << std::endl;
    Test_->Open();
    // Create and get streamOperator information
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 2147483647;
    Test_->streamInfo->width_ = 640;
    Test_->streamInfo->height_ = 480;
    Test_->StreamInfoFormat();
    Test_->streamInfo->tunneledMode_ = 5;
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    Test_->streamInfo->bufferQueue_ = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->streamInfo->bufferQueue_->SetQueueSize(8);
    Test_->consumerMap_[Test_->streamInfo->intent_] = consumer;
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Distribution stream
    Test_->rc = Test_->streamOperator->CommitStreams(Camera::NORMAL, Test_->ability);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Get preview
    int captureId = 2001;
    Test_->captureInfo = std::make_shared<Camera::CaptureInfo>();
    Test_->captureInfo->streamIds_ = {2147483647};
    Test_->captureInfo->captureSetting_ = Test_->ability;
    Test_->captureInfo->enableShutterCallback_ = true;
    bool isStreaming = true;
    Test_->rc = Test_->streamOperator->Capture(captureId, Test_->captureInfo, isStreaming);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    sleep(3);
    Test_->streamOperator->CancelCapture(captureId);

    // release stream
    Test_->rc = Test_->streamOperator->ReleaseStreams(Test_->captureInfo->streamIds_);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Turn off the device
    Test_->cameraDevice->Close();
    std::cout << "cameraDevice->Close" << std::endl;
    consumer->StopConsumer();
}

/**
  * @tc.name: Capture
  * @tc.desc: Preview, Capture->captureInfo->enableShutterCallback = false ,return success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0193, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: captureInfo->enableShutterCallback = false , success." << std::endl;
    Test_->Open();
    // Configure two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture the photo stream, single capture
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, false);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_capture};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: Capture
  * @tc.desc: Preview, Capture->isStreaming = false ,expected success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0194, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: isStreaming = false ,expected success." << std::endl;
    Test_->Open();
    // Configure two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture the photo stream, single capture
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, false);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_capture};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: Capture
  * @tc.desc: Preview, Capture->captureId = -1 ,return error.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0195, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: Preview, Capture->captureId = -1 ,return error." << std::endl;
    Test_->Open();
    // Configure preview stream information
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Get preview
    Test_->captureInfo = std::make_shared<Camera::CaptureInfo>();
    Test_->captureInfo->streamIds_ = {Test_->streamId_preview};
    Test_->captureInfo->captureSetting_ = Test_->ability;
    Test_->captureInfo->enableShutterCallback_ = false;
    bool isStreaming = true;
    int captureId = -1;
    Test_->rc = Test_->streamOperator->Capture(captureId, Test_->captureInfo, isStreaming);
    EXPECT_EQ(INVALID_ARGUMENT, Test_->rc);
    sleep(5);
    // release stream
    Test_->rc = Test_->streamOperator->ReleaseStreams({Test_->streamId_preview});
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: Capture
  * @tc.desc: Preview, Capture->captureInfo->enableShutterCallback = true ,return success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0196, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: captureInfo->enableShutterCallback = true , success." << std::endl;
    Test_->Open();
    // Configure preview stream information
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, true, true);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: CancelCapture
  * @tc.desc: CancelCapture, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0200, TestSize.Level0)
{
    std::cout << "==========[test log]Check hdi: CancelCapture, success." << std::endl;
    Test_->Open();
    // Configure preview stream information
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, true, true);
    // post-processing
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: CancelCapture
  * @tc.desc: CancelCapture captureID = -1.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0201, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: CancelCapture captureID = -1, expected fail." << std::endl;
    Test_->Open();
    // Configure preview stream information
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, true, true);
    // Cancel capture
    Test_->rc = Test_->streamOperator->CancelCapture(-1);
    EXPECT_EQ(INVALID_ARGUMENT, Test_->rc);
    std::cout << "==========[test log]Check hdi: CancelCapture captureID = -1, return INVALID_ARGUMENT." << std::endl;
    // post-processing
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: AttachBufferQueue
  * @tc.desc: AttachBufferQueue, normal input.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0210, TestSize.Level0)
{
    std::cout << "==========[test log]Check hdi: AttachBufferQueue, normal input." << std::endl;
    // Turn on the camera
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->height_ = 480;
    Test_->streamInfo->width_ = 640;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::vector<std::shared_ptr<Camera::StreamInfo>>().swap(Test_->streamInfos);
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "==========[test log]Check hdi: streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Distribution stream
    Test_->rc = Test_->streamOperator->CommitStreams(Camera::NORMAL, Test_->ability);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::cout << "==========[test log]Check hdi: streamOperator->CommitStreams's rc " << Test_->rc << std::endl;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<OHOS::Surface> producer = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    OHOS::sptr<OHOS::IBufferProducer> producer = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->rc = Test_->streamOperator->AttachBufferQueue(Test_->streamInfo->streamId_, producer);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]Check hdi: AttachBufferQueue success. " << std::endl;
    } else {
        std::cout << "==========[test log]Check hdi: AttachBufferQueue fail, rc = " << Test_->rc << std::endl;
    }
    // release stream
    Test_->captureIds = {};
    Test_->streamIds = {1001};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    consumer->StopConsumer();
}

/**
  * @tc.name: AttachBufferQueue
  * @tc.desc: AttachBufferQueue, streamID is not exist.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0211, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: AttachBufferQueue, streamID is not exist.." << std::endl;
    // Turn on the camera
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 640;
    Test_->StreamInfoFormat();
    Test_->streamInfo->height_ = 480;
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::vector<std::shared_ptr<Camera::StreamInfo>>().swap(Test_->streamInfos);
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "==========[test log]Check hdi: streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Distribution stream
    Test_->rc = Test_->streamOperator->CommitStreams(Camera::NORMAL, Test_->ability);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::cout << "==========[test log]Check hdi: streamOperator->CommitStreams's rc " << Test_->rc << std::endl;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<OHOS::Surface> producer = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    OHOS::sptr<OHOS::IBufferProducer> producer = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->rc = Test_->streamOperator->AttachBufferQueue(0, producer);
    EXPECT_EQ(Test_->rc, Camera::INVALID_ARGUMENT);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]Check hdi: AttachBufferQueue success. " << std::endl;
    } else {
        std::cout << "==========[test log]Check hdi: AttachBufferQueue fail, rc = " << Test_->rc << std::endl;
    }
    // release stream
    Test_->captureIds = {};
    Test_->streamIds = {1001};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    consumer->StopConsumer();
}

/**
  * @tc.name: AttachBufferQueue
  * @tc.desc: AttachBufferQueue, producer is nullptr.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0212, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: AttachBufferQueue, producer is nullptr." << std::endl;
    // Turn on the camera
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 640;
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->height_ = 480;
    Test_->StreamInfoFormat();
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->tunneledMode_ = 5;
    std::vector<std::shared_ptr<Camera::StreamInfo>>().swap(Test_->streamInfos);
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "==========[test log]Check hdi: streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Distribution stream
    Test_->rc = Test_->streamOperator->CommitStreams(Camera::NORMAL, Test_->ability);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::cout << "==========[test log]Check hdi: streamOperator->CommitStreams's rc " << Test_->rc << std::endl;
    Test_->rc = Test_->streamOperator->AttachBufferQueue(Test_->streamInfo->streamId_, nullptr);
    EXPECT_EQ(Test_->rc, Camera::INVALID_ARGUMENT);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]Check hdi: AttachBufferQueue success. " << std::endl;
    } else {
        std::cout << "==========[test log]Check hdi: AttachBufferQueue fail, rc = " << Test_->rc << std::endl;
    }
    // release stream
    Test_->captureIds = {};
    Test_->streamIds = {1001};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
}

/**
  * @tc.name: DetachBufferQueue
  * @tc.desc: DetachBufferQueue, normal input.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0220, TestSize.Level0)
{
    std::cout << "==========[test log]Check hdi: DetachBufferQueue, normal input." << std::endl;
    // Turn on the camera
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 640;
    Test_->streamInfo->height_ = 480;
    Test_->StreamInfoFormat();
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->tunneledMode_ = 5;
    std::vector<std::shared_ptr<Camera::StreamInfo>>().swap(Test_->streamInfos);
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "==========[test log]Check hdi: streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Distribution stream
    Test_->rc = Test_->streamOperator->CommitStreams(Camera::NORMAL, Test_->ability);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::cout << "==========[test log]Check hdi: streamOperator->CommitStreams's rc " << Test_->rc << std::endl;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<OHOS::Surface> producer = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    OHOS::sptr<OHOS::IBufferProducer> producer = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->rc = Test_->streamOperator->AttachBufferQueue(Test_->streamInfo->streamId_, producer);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]Check hdi: AttachBufferQueue success. " << std::endl;
    } else {
        std::cout << "==========[test log]Check hdi: AttachBufferQueue fail, rc = " << Test_->rc << std::endl;
    }
    sleep(3);
    Test_->rc = Test_->streamOperator->DetachBufferQueue(Test_->streamInfo->streamId_);
    std::cout << "==========[test log]Check hdi: streamOperator->DetachBufferQueue's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // release stream
    Test_->captureIds = {};
    Test_->streamIds = {1001};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    consumer->StopConsumer();
}

/**
  * @tc.name: DetachBufferQueue
  * @tc.desc: DetachBufferQueue, streamID is not exist.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0221, TestSize.Level2)
{
    std::cout << "==========[test log]Check hdi: DetachBufferQueue, streamID is not exist." << std::endl;
    // Turn on the camera
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Create data stream
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001;
    Test_->streamInfo->width_ = 640;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->height_ = 480;
    Test_->StreamInfoFormat();
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->tunneledMode_ = 5;
    std::vector<std::shared_ptr<Camera::StreamInfo>>().swap(Test_->streamInfos);
    Test_->streamInfos.push_back(Test_->streamInfo);
    Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
    std::cout << "==========[test log]Check hdi: streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Distribution stream
    Test_->rc = Test_->streamOperator->CommitStreams(Camera::NORMAL, Test_->ability);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::cout << "==========[test log]Check hdi: streamOperator->CommitStreams's rc " << Test_->rc << std::endl;
    std::shared_ptr<OHOS::Camera::Test::StreamConsumer> consumer =
      std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<OHOS::Surface> producer = consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
        Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
    });
#else
    OHOS::sptr<OHOS::IBufferProducer> producer = consumer->CreateProducer([this](void* addr, uint32_t size) {
        Test_->SaveYUV("preview", addr, size);
    });
#endif
    Test_->rc = Test_->streamOperator->AttachBufferQueue(Test_->streamInfo->streamId_, producer);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]Check hdi: AttachBufferQueue success. " << std::endl;
    } else {
        std::cout << "==========[test log]Check hdi: AttachBufferQueue fail, rc = " << Test_->rc << std::endl;
    }
    sleep(3);
    Test_->rc = Test_->streamOperator->DetachBufferQueue(100);
    std::cout << "==========[test log]Check hdi: streamOperator->DetachBufferQueue's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::INVALID_ARGUMENT);
    // release stream
    Test_->captureIds = {};
    Test_->streamIds = {1001};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    consumer->StopConsumer();
}

/**
  * @tc.name: ChangeToOfflineStream
  * @tc.desc: ChangeToOfflineStream, normal input.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0230, TestSize.Level0)
{
    std::cout << "==========[test log]ChangeToOfflineStream, normal input." << std::endl;
    Test_->Open();
    // 1. Configure two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // 2. Capture the preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // 3. Capture the camera stream, continuous shooting
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, true);
    sleep(5);
    // 4. Convert to offline stream
    Test_->CreateOfflineStreamOperatorCallback();
    std::vector<int> offlineIds;
    offlineIds.push_back(Test_->streamId_capture);
    Test_->rc = Test_->streamOperator->ChangeToOfflineStream(
        offlineIds, Test_->offlineStreamOperatorCallback, Test_->offlineStreamOperator);
    ASSERT_EQ(Test_->rc, Camera::NO_ERROR);
    std::cout << "==========[test log] ChangeToOfflineStream rc = " << Test_->rc << std::endl;
    EXPECT_EQ(true, Test_->offlineStreamOperator != nullptr);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] offline StreamOperator != nullptr" << std::endl;
    } else {
        std::cout << "==========[test log] offline StreamOperator == nullptr" << std::endl;
    }
    // 5. Post-processing of the original stream
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_capture};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_capture};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    // 6. Post-processing of offline streams
    Test_->cameraDevice->Close();
    std::cout << "==========[test log] Pretend to wait 5s for callback..." << std::endl;
    sleep(5);
    Test_->StopOfflineStream(Test_->captureId_capture);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: ChangeToOfflineStream
  * @tc.desc: ChangeToOfflineStream, streamId is not exist.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0231, TestSize.Level2)
{
    std::cout << "==========[test log]ChangeToOfflineStream, streamId is not exist." << std::endl;
    Test_->Open();
    // Configure two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture the photo stream, single capture
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, true);
    sleep(10);
    // Convert to offline stream
    Test_->CreateOfflineStreamOperatorCallback();
    Test_->rc = Test_->streamOperator->ChangeToOfflineStream(
        {99}, Test_->offlineStreamOperatorCallback, Test_->offlineStreamOperator);
    ASSERT_EQ(Test_->rc, Camera::INVALID_ARGUMENT);
    std::cout << "==========[test log]Check offline: ChangeToOfflineStream rc = " << Test_->rc << std::endl;
    EXPECT_EQ(true, Test_->offlineStreamOperator == nullptr);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]Check offline stream: offline StreamOperator != nullptr" << std::endl;
    } else {
        std::cout << "==========[test log]Check offline stream: offline StreamOperator == nullptr" << std::endl;
    }
    // Post-processing of the original stream
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_capture};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_capture};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: ChangeToOfflineStream
  * @tc.desc: ChangeToOfflineStream, offlineStreamOperatorCallback is nullptr.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0232, TestSize.Level2)
{
    std::cout << "==========[test log]ChangeToOfflineStream, offlineStreamOperatorCallback is nullptr." << std::endl;
    Test_->Open();
    // Configure two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // Capture preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Capture the photo stream, single capture
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, true);
    sleep(10);
    // Convert to offline stream
    Test_->offlineStreamOperatorCallback = nullptr;
    Test_->rc = Test_->streamOperator->ChangeToOfflineStream(
        {99}, Test_->offlineStreamOperatorCallback, Test_->offlineStreamOperator);
    ASSERT_EQ(Test_->rc, Camera::INVALID_ARGUMENT);
    std::cout << "==========[test log]Check offline: ChangeToOfflineStream rc = " << Test_->rc << std::endl;
    EXPECT_EQ(true, Test_->offlineStreamOperator == nullptr);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]Check offline stream: offline StreamOperator != nullptr" << std::endl;
    } else {
        std::cout << "==========[test log]Check offline stream: offline StreamOperator == nullptr" << std::endl;
    }
    // Post-processing of the original stream
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_capture};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_capture};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: offlineStreamOperator->CancelCapture
  * @tc.desc: offlineStreamOperator->CancelCapture, -1.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiStreamTest, Camera_Hdi_0241, TestSize.Level2)
{
    std::cout << "==========[test log]ChangeToOfflineStream, normal input." << std::endl;
    Test_->Open();
    // 1. Configure two streams of information
    Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
    Test_->StartStream(Test_->intents);
    // 2. Capture the preview stream
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // 3. Capture the camera stream, continuous shooting
    Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, true);
    sleep(5);
    // 4. Convert to offline stream
    Test_->CreateOfflineStreamOperatorCallback();
    std::vector<int> offlineIds;
    offlineIds.push_back(Test_->streamId_capture);
    Test_->rc = Test_->streamOperator->ChangeToOfflineStream(
        offlineIds, Test_->offlineStreamOperatorCallback, Test_->offlineStreamOperator);
    ASSERT_EQ(Test_->rc, Camera::NO_ERROR);
    std::cout << "==========[test log] ChangeToOfflineStream rc = " << Test_->rc << std::endl;
    EXPECT_EQ(true, Test_->offlineStreamOperator != nullptr);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] offline StreamOperator != nullptr" << std::endl;
    } else {
        std::cout << "==========[test log] offline StreamOperator == nullptr" << std::endl;
    }
    // 5. Post-processing of the original stream
    Test_->captureIds = {Test_->captureId_preview, Test_->captureId_capture};
    Test_->streamIds = {Test_->streamId_preview, Test_->streamId_capture};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    // 6. Post-processing of offline streams
    Test_->cameraDevice->Close();
    std::cout << "==========[test log] Pretend to wait 5s for callback..." << std::endl;
    sleep(5);

    Test_->rc = Test_->offlineStreamOperator->CancelCapture(-1);
    EXPECT_EQ(Test_->rc, Camera::INVALID_ARGUMENT);
    std::cout << "==========[test log]check offline: CancelCapture fail, rc = " << Test_->rc;
    std::cout << "captureId = -1" << std::endl;

    Test_->StopOfflineStream(Test_->captureId_capture);
    Test_->StopConsumer(Test_->intents);
}
