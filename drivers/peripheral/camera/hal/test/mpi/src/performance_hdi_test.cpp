/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
#include <fstream>
#include "performance_hdi_test.h"
namespace {
    const int Times = 1000; // 1000:Cycle 1000 times
    const int TimeTransformation_us = 1000000; // 1000000:1000000 microseconds
    std::ofstream writeIntoFile;
}

using namespace OHOS;
using namespace std;
using namespace testing::ext;
using namespace OHOS::Camera;
float PerformanceHdiTest::calTime(struct timeval start, struct timeval end)
{
    float time_use = 0;
    time_use = (end.tv_sec - start.tv_sec) * TimeTransformation_us + (end.tv_usec - start.tv_usec);
    return time_use;
    // return time us
}
void PerformanceHdiTest::SetUpTestCase(void) {}
void PerformanceHdiTest::TearDownTestCase(void) {}
void PerformanceHdiTest::SetUp(void)
{
    Test_ = std::make_shared<OHOS::Camera::Test>();
    Test_->Init();
}
void PerformanceHdiTest::TearDown(void)
{
    Test_->Close();
}

/**
  * @tc.name: HDI_GetCameraIds's time consuming.
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0010, TestSize.Level3)
{
    std::cout << "==========[test log] Performance: HDI_GetCameraIds's time consuming."<< std::endl;
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    for (int i= 0; i < Times; i++) {
        std::cout << "Running " << i << " time" << std::endl;
        gettimeofday(&start, NULL);
        Test_->rc = Test_->service->GetCameraIds(Test_->cameraIds);
        gettimeofday(&end, NULL);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
    float avrg_time = totle_time_use/ Times;
    std::cout << "==========[test log] Performance: HDI_GetCameraIds's average time consuming: ";
    std::cout << avrg_time << "us." << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_GetCameraIds's average time consuming: ";
    writeIntoFile << avrg_time << "us." << std::endl;
}

/**
  * @tc.name: GetCameraAbility
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0020, TestSize.Level0)
{
    std::cout << "==========[test log] Performance: GetCameraAbility's average time consuming." << std::endl;
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    if (Test_->cameraDevice == nullptr) {
        Test_->rc = Test_->service->GetCameraIds(Test_->cameraIds);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        for (int i= 0; i < Times; i++) {
            std::cout << "Running " << i << " time" << std::endl;
            gettimeofday(&start, NULL);
            Test_->rc = Test_->service->GetCameraAbility(Test_->cameraIds.front(), Test_->ability);
            gettimeofday(&end, NULL);
            time_use = calTime(start, end);
            totle_time_use = totle_time_use + time_use;
            EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        }
        float avrg_time = totle_time_use/ Times;
        std::cout << "==========[test log] Performance: GetCameraAbility's average time consuming: ";
        std::cout << avrg_time << "us." << std::endl;
        writeIntoFile << "==========[test log] Performance: GetCameraAbility's average time consuming: ";
        writeIntoFile << avrg_time << "us." << std::endl;
    }
}

/**
  * @tc.name: HDI_OpenCamera's time consuming.
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0030, TestSize.Level3)
{
    std::cout << "==========[test log] Performance: HDI_OpenCamera's time consuming."<< std::endl;
    Test_->service->GetCameraIds(Test_->cameraIds);
    std::string cameraId = Test_->cameraIds.front();
    Test_->CreateDeviceCallback();
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    for (int i= 0; i < Times; i++) {
        std::cout << "Running " << i << " time" << std::endl;
        gettimeofday(&start, NULL);
        Test_->rc = Test_->service->OpenCamera(cameraId, Test_->deviceCallback, Test_->cameraDevice);
        gettimeofday(&end, NULL);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
    float avrg_time = totle_time_use/Times;
    std::cout << "==========[test log] Performance: HDI_OpenCamera's average time consuming: ";
    std::cout << avrg_time << "us." << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_OpenCamera's average time consuming: ";
    writeIntoFile << avrg_time << "us." << std::endl;
    writeIntoFile.close();
}

/**
  * @tc.name: HDI_SetFlashlight's time consuming.
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0040, TestSize.Level3)
{
    std::cout << "==========[test log] Performance: HDI_SetFlashlight's time consuming." << std::endl;
    // Open camera
    Test_->Open();
    // Cycle to turn on and off the flashlight
    bool status;
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    for (int i = 0; i < Times/2; i++) {
        std::cout << "Running " << i << " time" << std::endl;
        // Turn on the flashlight
        status = true;
        gettimeofday(&start, NULL);
        Test_->rc = Test_->service->SetFlashlight(Test_->cameraIds.front(), status);
        gettimeofday(&end, NULL);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
        // Turn off the flashlight
        status = false;
        gettimeofday(&start, NULL);
        Test_->rc = Test_->service->SetFlashlight(Test_->cameraIds.front(), status);
        gettimeofday(&end, NULL);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: HDI_SetFlashlight's average time consuming: ";
    std::cout << avrg_time << "us. " << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_SetFlashlight's average time consuming: ";
    writeIntoFile << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
}

/**
  * @tc.name: GetStreamOperator's time consuming.
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0050, TestSize.Level3)
{
    std::cout << "==========[test log] Performance: GetStreamOperator success, 1000 times." << std::endl;
    // Get the configured cameraId
    Test_->service->GetCameraIds(Test_->cameraIds);
    std::cout << "cameraIds.front() = " << Test_->cameraIds.front() << std::endl;
    // Open camera
    Test_->Open();
    // Call the GetStreamOperator function of the device to get the streamOperator
    Test_->CreateStreamOperatorCallback();
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    for (int i = 0; i < Times; i++) {
        std::cout << "Running " << i << " time" << std::endl;
        gettimeofday(&start, NULL);
        Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
        gettimeofday(&end, NULL);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: HDI_cameraDevice->GetStreamOperator's average time: ";
    std::cout << avrg_time << "us. " << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_cameraDevice->GetStreamOperator's average time: ";
    writeIntoFile << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
}

/**
  * @tc.name: HDI_UpdateSettings's time consuming.
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0060, TestSize.Level3)
{
    std::cout << "==========[test log] Check HDI_UpdateSettings's time consuming." << std::endl;
    Test_->Open();
    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    std::vector<uint8_t> awbMode = {
        OHOS_CAMERA_AWB_MODE_OFF,
        OHOS_CAMERA_AWB_MODE_TWILIGHT,
        OHOS_CAMERA_AWB_MODE_AUTO,
        OHOS_CAMERA_AWB_MODE_WARM_FLUORESCENT,
        OHOS_CAMERA_AWB_MODE_DAYLIGHT,
        OHOS_CAMERA_AWB_MODE_CLOUDY_DAYLIGHT,
        OHOS_CAMERA_AWB_MODE_INCANDESCENT,
        OHOS_CAMERA_AWB_MODE_FLUORESCENT,
        OHOS_CAMERA_AWB_MODE_SHADE
    };
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    for (int round = 0; round < Times; round ++) {
        int i = rand() % 9;
        std::cout << "round = "<< round << ", i = " << i << std::endl;
        meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode.at(i), 1);
        gettimeofday(&start, NULL);
        Test_->rc = Test_->cameraDevice->UpdateSettings(meta);
        gettimeofday(&end, NULL);
        std::cout << "rc = "<< Test_->rc << std::endl;
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: HDI_UpdateSettings's  turn on average time : ";
    std::cout << avrg_time << "us. " << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_UpdateSettings's  turn on average time : ";
    writeIntoFile << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
}

/**
  * @tc.name: SetResultMode's time consuming.
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0070, TestSize.Level3)
{
    std::cout << "==========[test log]Check Performance: HDI_cameraDevice->SetResultMode's average time" << std::endl;
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    Test_->Open();
    EXPECT_EQ(true, Test_->cameraDevice != nullptr);
    std::vector<Camera::MetaType> enableTypes;
    Test_->rc = Test_->cameraDevice->GetEnabledResults(enableTypes);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    for (int i = 0; i < Times; i++) {
        std::cout << "Running " << i << " time" << std::endl;
        gettimeofday(&start, NULL);
        Test_->rc = Test_->cameraDevice->SetResultMode(Camera::PER_FRAME);
        gettimeofday(&end, NULL);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: HDI_cameraDevice->SetResultMode's average time: ";
    std::cout << avrg_time << "us. " << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_cameraDevice->SetResultMode's average time: ";
    writeIntoFile << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
}

/**
  * @tc.name: GetEnabledResults
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0080, TestSize.Level3)
{
    std::cout << "==========[test log]Performance: HDI_cameraDevice->GetEnabledResults's average time." << std::endl;
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    Test_->Open();
    std::vector<Camera::MetaType> results;
    for (int i = 0; i < Times; i++) {
        std::cout << "Running " << i << " time" << std::endl;
        gettimeofday(&start, NULL);
        Test_->rc = Test_->cameraDevice->GetEnabledResults(results);
        gettimeofday(&end, NULL);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: HDI_cameraDevice->GetEnabledResults's average time: ";
    std::cout << avrg_time << "us. " << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_cameraDevice->GetEnabledResults's average time: ";
    writeIntoFile << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
}

/**
  * @tc.name: EnableResult
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0090, TestSize.Level3)
{
    std::cout << "==========[test log]Performance: HDI_cameraDevice->EnableResult's average time." << std::endl;
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    Test_->Open();
    // Get the parameter tag currently supported by the device
    std::cout << "==========[test log]Check hdi_device: 1. Get the tags..." << std::endl;
    std::vector<Camera::MetaType> results_original;
    results_original.push_back(OHOS_SENSOR_EXPOSURE_TIME);
    results_original.push_back(OHOS_SENSOR_COLOR_CORRECTION_GAINS);
    Test_->rc = Test_->cameraDevice->GetEnabledResults(results_original);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Add this tag
    std::vector<Camera::MetaType> enable_tag;
    enable_tag.push_back(results_original[1]);
    for (int i = 0; i < Times; i++) {
        std::cout << "Running " << i << " time" << std::endl;
        gettimeofday(&start, NULL);
        Test_->rc = Test_->cameraDevice->EnableResult(enable_tag);
        gettimeofday(&end, NULL);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: HDI_cameraDevice->EnableResult's average time: ";
    std::cout << avrg_time << "us. " << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_cameraDevice->EnableResult's average time: ";
    writeIntoFile << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
}

/**
  * @tc.name: DisableResult
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0100, TestSize.Level3)
{
    std::cout << "==========[test log]Performance: HDI_cameraDevice->DisableResult's average time." << std::endl;
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    Test_->Open();
    // Get the parameter tag currently supported by the device
    std::cout << "==========[test log]Check hdi_device: 1. Get the tags..." << std::endl;
    std::vector<Camera::MetaType> results_original;
    results_original.push_back(OHOS_SENSOR_EXPOSURE_TIME);
    results_original.push_back(OHOS_SENSOR_COLOR_CORRECTION_GAINS);
    Test_->rc = Test_->cameraDevice->GetEnabledResults(results_original);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Disable this tag
    std::vector<Camera::MetaType> disable_tag;
    disable_tag.push_back(results_original[1]);
    for (int i = 0; i < Times; i++) {
        std::cout << "Running " << i << " time" << std::endl;
        gettimeofday(&start, NULL);
        Test_->rc = Test_->cameraDevice->DisableResult(disable_tag);
        gettimeofday(&end, NULL);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: HDI_cameraDevice->DisableResult's average time: ";
    std::cout << avrg_time << "us. " << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_cameraDevice->DisableResult's average time: ";
    writeIntoFile << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
}

/**
  * @tc.name: IsStreamsSupported
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0120, TestSize.Level0)
{
    std::cout << "==========[test log]Performance: HDI_IsStreamsSupported's average time." << std::endl;
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    // Open camera
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
    int64_t colorGains[4] = {0};
    modeSetting->addEntry(OHOS_SENSOR_COLOR_CORRECTION_GAINS, &colorGains, 4);
    int64_t expoTime = 0;
    modeSetting->addEntry(OHOS_SENSOR_EXPOSURE_TIME, &expoTime, 1);
    // Configure stream information
    Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
    Test_->streamInfo->streamId_ = 1001; // 1001:streamId
    Test_->streamInfo->datasapce_ = 8;
    Test_->streamInfo->intent_ = Camera::PREVIEW;
    Test_->streamInfo->width_ = 640; // 640:Stream width
    Test_->streamInfo->height_ = 480; // 480:Stream height
    Test_->StreamInfoFormat();
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
    for (int i = 0; i < Times; i++) {
        std::cout << "Running " << i << " time" << std::endl;
        gettimeofday(&start, NULL);
        std::vector<std::shared_ptr<OHOS::Camera::StreamInfo>> stre;
        stre.push_back(Test_->streamInfo);
        Test_->rc = Test_->streamOperator->IsStreamsSupported(NORMAL, modeSetting, stre, pType);
        gettimeofday(&end, NULL);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: HDI_IsStreamsSupported's average time: ";
    std::cout << avrg_time << "us. " << std::endl;
    writeIntoFile << "HDI_IsStreamsSupported's average time: " << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
}

/**
  * @tc.name: HDI_CreateStreams's time consuming.
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0130, TestSize.Level3)
{
    std::cout << "==========[test log] Performance: HDI_CreateStreams's time consuming." << std::endl;
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    Test_->Open();
    for (int i = 0; i < Times; i ++) {
        std::cout << "Running " << i << " time" << std::endl;
        // Create and get streamOperator information
        Test_->CreateStreamOperatorCallback();
        Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
        EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
        // Create data flow
        Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
        Test_->streamInfo->streamId_ = 1001;
        Test_->streamInfo->width_ = 1920;
        Test_->streamInfo->height_ = 1080;
        Test_->StreamInfoFormat();
        Test_->streamInfo->datasapce_ = 10;
        Test_->streamInfo->intent_ = Camera::PREVIEW;
        Test_->streamInfo->tunneledMode_ = 5;
        std::shared_ptr<OHOS::Camera::Test::StreamConsumer> preview_consumer =
            std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
        Test_->streamInfo->bufferQueue_ = preview_consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
            Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
        });
#else
        Test_->streamInfo->bufferQueue_ = preview_consumer->CreateProducer([this](void* addr, uint32_t size) {
            Test_->SaveYUV("preview", addr, size);
        });
#endif
        Test_->streamInfo->bufferQueue_->SetQueueSize(8);
        Test_->consumerMap_[Camera::PREVIEW] = preview_consumer;
        std::vector<std::shared_ptr<Camera::StreamInfo>>().swap(Test_->streamInfos);
        Test_->streamInfos.push_back(Test_->streamInfo);
        gettimeofday(&start, NULL);
        Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
        gettimeofday(&end, NULL);
        std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
        // Release stream
        Test_->rc = Test_->streamOperator->ReleaseStreams({1001});
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: HDI_CreateStreams's average time consuming: ";
    std::cout << avrg_time << "us. " << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_CreateStreams's average time consuming: ";
    writeIntoFile << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
}

/**
  * @tc.name: HDI_ReleaseStreams's time consuming.
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0160, TestSize.Level3)
{
    std::cout << "==========[test log] Performance: HDI_ReleaseStreams's time consuming."<< std::endl;
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    Test_->Open();
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    for (int i = 0; i < Times; i ++) {
        std::cout  << "Times =" << i << std::endl;
        // Start stream
        Test_->intents = {Camera::PREVIEW};
        Test_->StartStream(Test_->intents);
        // Release stream
        gettimeofday(&start, NULL);
        Test_->rc = Test_->streamOperator->ReleaseStreams({Test_->streamId_preview});
        gettimeofday(&end, NULL);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: HDI_ReleaseStreams's average time consuming: ";
    std::cout  << avrg_time << "us. " << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_ReleaseStreams's average time consuming: ";
    writeIntoFile  << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
}

/**
  * @tc.name: CommitStreams's time consuming.
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0170, TestSize.Level3)
{
    std::cout << "==========[test log] Performance: CommitStreams's time consuming." << std::endl;

    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    Test_->Open();
    for (int i = 0; i < Times; i ++) {
        std::cout << "Running " << i << " time" << std::endl;
        // Create and get streamOperator information
        Test_->CreateStreamOperatorCallback();
        Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
        EXPECT_EQ(false, Test_->rc != Camera::NO_ERROR || Test_->streamOperator == nullptr);
        // Create data flow
        Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
        Test_->streamInfo->streamId_ = 1001;
        Test_->streamInfo->width_ = 1920;
        Test_->streamInfo->height_ = 1080;
        Test_->StreamInfoFormat();
        Test_->streamInfo->datasapce_ = 10;
        Test_->streamInfo->intent_ = Camera::PREVIEW;
        Test_->streamInfo->tunneledMode_ = 5;
        std::shared_ptr<OHOS::Camera::Test::StreamConsumer> preview_consumer =
        std::make_shared<OHOS::Camera::Test::StreamConsumer>();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
        Test_->streamInfo->bufferQueue_ = preview_consumer->CreateProducer([this](OHOS::SurfaceBuffer* buffer) {
            Test_->SaveYUV("preview", buffer->GetVirAddr(), buffer->GetSize());
        });
#else
        Test_->streamInfo->bufferQueue_ = preview_consumer->CreateProducer([this](void* addr, uint32_t size) {
            Test_->SaveYUV("preview", addr, size);
        });
#endif
        Test_->streamInfo->bufferQueue_->SetQueueSize(8);
        Test_->consumerMap_[Camera::PREVIEW] = preview_consumer;
        std::vector<std::shared_ptr<Camera::StreamInfo>>().swap(Test_->streamInfos);
        Test_->streamInfos.push_back(Test_->streamInfo);
        Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
        std::cout << "streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        // Flow distribution
        gettimeofday(&start, NULL);
        Test_->rc = Test_->streamOperator->CommitStreams(Camera::NORMAL, Test_->ability);
        gettimeofday(&end, NULL);
        std::cout << "streamOperator->CommitStreams's rc " << Test_->rc << std::endl;
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
        // Release stream
        Test_->rc = Test_->streamOperator->ReleaseStreams({1001});
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: CommitStreams's average time consuming: ";
    std::cout << avrg_time << "us. " << std::endl;
    writeIntoFile << "==========[test log] Performance: CommitStreams's average time consuming: ";
    writeIntoFile << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
}

/**
  * @tc.name: GetStreamAttributes
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0180, TestSize.Level3)
{
    std::cout << "==========[test log]Check Performance: HDI_GetStreamAttributes's average time." << std::endl;
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    std::vector<std::shared_ptr<Camera::StreamAttribute>> attributes;
    for (int i = 0; i < Times; i++) {
        std::cout << "Running " << i << " time" << std::endl;
        gettimeofday(&start, NULL);
        Test_->rc = Test_->streamOperator->GetStreamAttributes(attributes);
        gettimeofday(&end, NULL);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: HDI_GetStreamAttributes's average time: ";
    std::cout << avrg_time << "us. " << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_GetStreamAttributes's average time: ";
    writeIntoFile << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
    // Release stream
    Test_->rc = Test_->streamOperator->ReleaseStreams({Test_->streamId_preview});
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::cout << "streamOperator->ReleaseStreams's rc " << Test_->rc << std::endl;
}

/**
  * @tc.name: HDI_Capture's time consuming.
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0190, TestSize.Level3)
{
    std::cout << "==========[test log] Performance: HDI_Capture's average time consuming." << std::endl;
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    // Open camera
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    int captureId = 2001;
    Test_->captureInfo = std::make_shared<Camera::CaptureInfo>();
    Test_->captureInfo->streamIds_ = {Test_->streamId_preview};
    Test_->captureInfo->captureSetting_ = Test_->ability;
    Test_->captureInfo->enableShutterCallback_ = true;
    for (int i = 0; i < Times; i++) {
        std::cout  << "Times =" << i << std::endl;
        gettimeofday(&start, NULL);
        Test_->rc = Test_->streamOperator->Capture(captureId, Test_->captureInfo, true);
        if (Test_->rc == Camera::NO_ERROR) {
            std::cout << "==========[test log]check Capture: Capture success, " << captureId << std::endl;
        } else {
            std::cout << "==========[test log]check Capture: Capture fail, rc = " << Test_->rc << std::endl;
        }
        gettimeofday(&end, NULL);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
        sleep(1);
        Test_->rc = Test_->streamOperator->CancelCapture(captureId++);
        if (Test_->rc == Camera::NO_ERROR) {
            std::cout << "==========[test log]check Capture: CancelCapture success," << captureId << std::endl;
        } else {
            std::cout << "==========[test log]check Capture: CancelCapture fail, rc = " << Test_->rc << std::endl;
            std::cout << "captureId = " << captureId << std::endl;
        }
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: HDI_Capture's average time consuming: ";
    std::cout << avrg_time << "us. " << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_Capture's average time consuming: ";
    writeIntoFile << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
    // post-processing
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->rc = Test_->streamOperator->ReleaseStreams(Test_->streamIds);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
}

/**
  * @tc.name: HDI_CancelCapture's time consuming.
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0200, TestSize.Level3)
{
    std::cout << "==========[test log] Performance: HDI_CancelCapture's average time consuming." << std::endl;
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    // Open camera
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    int captureId = 2001;
    Test_->captureInfo = std::make_shared<Camera::CaptureInfo>();
    Test_->captureInfo->streamIds_ = {Test_->streamId_preview};
    Test_->captureInfo->captureSetting_ = Test_->ability;
    Test_->captureInfo->enableShutterCallback_ = true;
    for (int i = 0; i < Times; i++) {
        std::cout  << "Times =" << i << std::endl;
        Test_->rc = Test_->streamOperator->Capture(captureId, Test_->captureInfo, true);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        sleep(1);
        gettimeofday(&start, NULL);
        Test_->rc = Test_->streamOperator->CancelCapture(captureId);
        gettimeofday(&end, NULL);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
        captureId++;
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: HDI_CancelCapture's average time consuming: ";
    std::cout << avrg_time << "us. " << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_CancelCapture's average time consuming: ";
    writeIntoFile << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
    // post-processing
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->rc = Test_->streamOperator->ReleaseStreams(Test_->streamIds);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
}

/**
  * @tc.name: AttachBufferQueue
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0210, TestSize.Level3)
{
    std::cout << "==========[test log] Performance: HDI_AttachBufferQueue's average time consuming." << std::endl;
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    // Open camera
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    for (int i = 0; i < Times; i++) {
        std::cout << "Running " << i << " time" << std::endl;
        // Create data flow
        Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
        Test_->streamInfo->streamId_ = Test_->streamId_preview;
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
        // Flow distribution
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
        gettimeofday(&start, NULL);
        Test_->rc = Test_->streamOperator->AttachBufferQueue(Test_->streamInfo->streamId_, producer);
        std::cout << "==========[test log]Check hdi: streamOperator->AttachBufferQueue's rc " << Test_->rc << std::endl;
        gettimeofday(&end, NULL);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
        Test_->rc = Test_->streamOperator->DetachBufferQueue(Test_->streamInfo->streamId_);
        std::cout << "==========[test log]Check hdi: streamOperator->DetachBufferQueue's rc " << Test_->rc << std::endl;
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        // Release stream
        Test_->streamIds.push_back(Test_->streamId_preview);
        Test_->rc = Test_->streamOperator->ReleaseStreams(Test_->streamIds);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: HDI_AttachBufferQueue's average time consuming: ";
    std::cout << avrg_time << "us. " << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_AttachBufferQueue's average time consuming: ";
    writeIntoFile << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
}

/**
  * @tc.name: DetachBufferQueue
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0220, TestSize.Level3)
{
    std::cout << "==========[test log] Performance: HDI_DetachBufferQueue's average time consuming." << std::endl;
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    Test_->Open();
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    for (int i = 0; i < Times; i++) {
        std::cout << "Running " << i << " time" << std::endl;
        Test_->CreateStreamOperatorCallback();
        Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        // Create data flow
        Test_->streamInfo = std::make_shared<Camera::StreamInfo>();
        Test_->streamInfo->intent_ = Camera::PREVIEW;
        Test_->streamInfo->streamId_ = Test_->streamId_preview;
        Test_->streamInfo->width_ = 640;
        Test_->streamInfo->height_ = 480;
        Test_->StreamInfoFormat();
        Test_->streamInfo->datasapce_ = 8;
        Test_->streamInfo->tunneledMode_ = 5;
        std::vector<std::shared_ptr<Camera::StreamInfo>>().swap(Test_->streamInfos);
        Test_->streamInfos.push_back(Test_->streamInfo);
        Test_->rc = Test_->streamOperator->CreateStreams(Test_->streamInfos);
        std::cout << "==========[test log]Check hdi: streamOperator->CreateStreams's rc " << Test_->rc << std::endl;
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        // Flow distribution
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
        std::cout << "==========[test log]Check hdi: streamOperator->AttachBufferQueue's rc ";
        std::cout << Test_->rc << std::endl;
        gettimeofday(&start, NULL);
        Test_->rc = Test_->streamOperator->DetachBufferQueue(Test_->streamInfo->streamId_);
        std::cout << "==========[test log]Check hdi: streamOperator->DetachBufferQueue's rc ";
        std::cout << Test_->rc << std::endl;
        gettimeofday(&end, NULL);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
        // Release stream
        Test_->streamIds.push_back(Test_->streamId_preview);
        Test_->rc = Test_->streamOperator->ReleaseStreams(Test_->streamIds);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: HDI_DetachBufferQueue's average time consuming: ";
    std::cout << avrg_time << "us. " << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_DetachBufferQueue's average time consuming: ";
    writeIntoFile << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
}

/**
  * @tc.name: ChangeToOfflineStream
  * @tc.desc: the average time for 1000 times.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(PerformanceHdiTest, Camera_Performance_Hdi_0230, TestSize.Level0)
{
    std::cout << "==========[test log] Performance: HDI_ChangeToOfflineStream's average time consuming." << std::endl;
    struct timeval start;
    struct timeval end;
    float time_use;
    float totle_time_use = 0;
    Test_->Open();
    writeIntoFile.open("TimeConsuming.txt", ios::app);
    for (int i = 0; i < Times; i++) {
        std::cout << "Running " << i << " time" << std::endl;
        // 1、Configure two stream information
        Test_->intents = {Camera::PREVIEW, Camera::STILL_CAPTURE};
        Test_->StartStream(Test_->intents);
        // 2、Capture preview stream
        Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
        // 3、Capture camera stream, continuous shooting
        Test_->StartCapture(Test_->streamId_capture, Test_->captureId_capture, false, true);
        sleep(1);
        // 4、Convert to offline stream
        Test_->CreateOfflineStreamOperatorCallback();
        gettimeofday(&start, NULL);
        std::vector<int> offlineIds;
        offlineIds.push_back(Test_->streamId_capture);
        Test_->rc = Test_->streamOperator->ChangeToOfflineStream(
            offlineIds, Test_->offlineStreamOperatorCallback, Test_->offlineStreamOperator);
        gettimeofday(&end, NULL);
        ASSERT_EQ(Test_->rc, Camera::NO_ERROR);
        time_use = calTime(start, end);
        totle_time_use = totle_time_use + time_use;
        std::cout << "==========[test log] ChangeToOfflineStream rc = " << Test_->rc << std::endl;
        EXPECT_EQ(true, Test_->offlineStreamOperator != nullptr);
        if (Test_->rc == Camera::NO_ERROR) {
            std::cout << "==========[test log] offline StreamOperator != nullptr" << std::endl;
        } else {
            std::cout << "==========[test log] offline StreamOperator == nullptr" << std::endl;
        }
        // 5、Post-processing of the original stream
        Test_->captureIds = {Test_->captureId_preview, Test_->captureId_capture};
        Test_->streamIds.push_back(Test_->streamId_preview);
        Test_->streamIds.push_back(Test_->streamId_capture);
        Test_->StopStream(Test_->captureIds, Test_->streamIds);
        // 6、Post-processing of offline streams
        Test_->cameraDevice->Close();
        std::cout << "==========[test log] Pretend to wait 5s for callback..." << std::endl;
        sleep(1);
        Test_->StopOfflineStream(Test_->captureId_capture);
    }
    float avrg_time = totle_time_use / Times;
    std::cout << "==========[test log] Performance: HDI_ChangeToOfflineStream's average time consuming: ";
    std::cout << avrg_time << "us. " << std::endl;
    writeIntoFile << "==========[test log] Performance: HDI_ChangeToOfflineStream's average time consuming: ";
    writeIntoFile << avrg_time << "us. " << std::endl;
    writeIntoFile.close();
}