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

#include "hdi_device_test.h"

using namespace OHOS;
using namespace std;
using namespace testing::ext;
using namespace OHOS::Camera;

void HdiDeviceTest::SetUpTestCase(void) {}
void HdiDeviceTest::TearDownTestCase(void) {}
void HdiDeviceTest::SetUp(void)
{
    Test_ = std::make_shared<OHOS::Camera::Test>();
    Test_->Init();
}
void HdiDeviceTest::TearDown(void)
{
    Test_->Close();
}

/**
  * @tc.name: GetStreamOprator
  * @tc.desc: GetStreamOprator, normal callback input.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0050, Function | MediumTest | Level1)
{
    std::cout << "==========[test log]Check hdi_device: GetStreamOprator, normal callback input." << std::endl;
    Test_->Open();
    Test_->CreateStreamOperatorCallback();
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
}

/**
  * @tc.name: GetStreamOprator
  * @tc.desc: GetStreamOprator, callback is nullptr.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0051, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_device: GetStreamOprator, normal callback input." << std::endl;
    Test_->Open();
    Test_->streamOperatorCallback = nullptr;
    Test_->rc = Test_->cameraDevice->GetStreamOperator(Test_->streamOperatorCallback, Test_->streamOperator);
    EXPECT_EQ(true, Test_->rc == Camera::INVALID_ARGUMENT);
}

/**
  * @tc.name: UpdateSettings
  * @tc.desc: UpdateSettings, OHOS_CONTROL_AE_EXPOSURE_COMPENSATION.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0060, Function | MediumTest | Level1)
{
    std::cout << "==========[test log]Check hdi_device: UpdateSettings,";
    std::cout <<" OHOS_CONTROL_AE_EXPOSURE_COMPENSATION." << std::endl;
    Test_->Open();
    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xa0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    Test_->rc = Test_->cameraDevice->UpdateSettings(meta);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::cout << "cameraDevice->UpdateSettings's rc " << Test_->rc << std::endl;
}

/**
  * @tc.name: UpdateSettings
  * @tc.desc: UpdateSettings, OHOS_CONTROL_AWB_MODE:OHOS_CONTROL_AWB_MODE_AUTO.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0061, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_device: UpdateSettings, ";
    std::cout <<"OHOS_CONTROL_AWB_MODE:OHOS_CONTROL_AWB_MODE_AUTO." << std::endl;
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_AUTO;
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log]UpdateSettings, awb : OHOS_CONTROL_AWB_MODE_AUTO." << std::endl;
    Test_->cameraDevice->UpdateSettings(meta);
    sleep(1);
    // Release stream
    Test_->streamIds = {Test_->streamId_preview};
    Test_->captureIds = {Test_->captureId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: UpdateSettings
  * @tc.desc: UpdateSettings, OHOS_CONTROL_AWB_MODE:OHOS_CONTROL_AWB_MODE_CLOUDY_DAYLIGHT.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0062, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_device: UpdateSettings, ";
    std::cout <<"OHOS_CONTROL_AWB_MODE:OHOS_CONTROL_AWB_MODE_CLOUDY_DAYLIGHT." << std::endl;
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_CLOUDY_DAYLIGHT;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log]UpdateSettings, awb : OHOS_CONTROL_AWB_MODE_CLOUDY_DAYLIGHT." << std::endl;
    Test_->cameraDevice->UpdateSettings(meta);
    sleep(1);
    // Release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: UpdateSettings
  * @tc.desc: UpdateSettings, OHOS_CONTROL_AWB_MODE:OHOS_CONTROL_AWB_MODE_TWILIGHT.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0063, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_device: UpdateSettings,";
    std::cout << "OHOS_CONTROL_AWB_MODE:OHOS_CONTROL_AWB_MODE_TWILIGHT." << std::endl;
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_TWILIGHT;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log]UpdateSettings, awb : OHOS_CONTROL_AWB_MODE_TWILIGHT." << std::endl;
    Test_->cameraDevice->UpdateSettings(meta);
    sleep(1);
    // Release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: UpdateSettings
  * @tc.desc: UpdateSettings, OHOS_CONTROL_AWB_MODE:OHOS_CONTROL_AWB_MODE_FLUORESCENT.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0065, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_device: UpdateSettings,";
    std::cout << "OHOS_CONTROL_AWB_MODE:OHOS_CONTROL_AWB_MODE_FLUORESCENT." << std::endl;
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_FLUORESCENT;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log]UpdateSettings, awb : OHOS_CONTROL_AWB_MODE_FLUORESCENT." << std::endl;
    Test_->cameraDevice->UpdateSettings(meta);
    sleep(1);
    // Release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: UpdateSettings
  * @tc.desc: UpdateSettings, OHOS_CONTROL_AWB_MODE:OHOS_CONTROL_AWB_MODE_WARM_FLUORESCENT.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0066, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_device: UpdateSettings,";
    std::cout << "OHOS_CONTROL_AWB_MODE:OHOS_CONTROL_AWB_MODE_WARM_FLUORESCENT." << std::endl;
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_WARM_FLUORESCENT;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log]UpdateSettings, awb : OHOS_CONTROL_AWB_MODE_WARM_FLUORESCENT." << std::endl;
    Test_->cameraDevice->UpdateSettings(meta);
    sleep(1);
    // Release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: UpdateSettings
  * @tc.desc: UpdateSettings, OHOS_CONTROL_AWB_MODE: illegal value(input AE value).
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0067, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_device: UpdateSettings,";
    std::cout <<" OHOS_CONTROL_AWB_MODE: illegal value(input AE value)." << std::endl;
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AE_MODE_ON_ALWAYS_FLASH;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    Test_->rc = Test_->cameraDevice->UpdateSettings(meta);
    std::cout << "cameraDevice->UpdateSettings's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    sleep(1);
    // Release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: UpdateSettings
  * @tc.desc: UpdateSettings, OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, illegal value.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0068, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_device: UpdateSettings,";
    std::cout <<" OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, illegal value." << std::endl;
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xFFFF;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    Test_->rc = Test_->cameraDevice->UpdateSettings(meta);
    std::cout << "cameraDevice->UpdateSettings's rc " << Test_->rc << std::endl;
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    sleep(1);
    // Release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: SetResultMode
  * @tc.desc: SetResultMode is PER_FRAME.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0070, Function | MediumTest | Level1)
{
    std::cout << "==========[test log]Check hdi_device: SetResultMode is PER_FRAME." << std::endl;
    Test_->Open();
    EXPECT_EQ(true, Test_->cameraDevice != nullptr);
    std::vector<Camera::MetaType> enableTypes;
    Test_->rc = Test_->cameraDevice->GetEnabledResults(enableTypes);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    for (auto &type : enableTypes) {
        std::cout << "==========[test log]Check hdi_device: type = " << type << std::endl;
    }
    Test_->rc = Test_->cameraDevice->SetResultMode(Camera::PER_FRAME);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
}

/**
  * @tc.name: SetResultMode
  * @tc.desc: SetResultMode is ON_CHANGED.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0071, Function | MediumTest | Level2)
{
    Test_->Open();
    EXPECT_EQ(true, Test_->cameraDevice != nullptr);
    std::cout << "==========[test log]Check hdi_device: SetResultMode is ON_CHANGED." << std::endl;
    std::vector<Camera::MetaType> enableTypes;
    Test_->rc = Test_->cameraDevice->GetEnabledResults(enableTypes);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    for (auto &type : enableTypes) {
        std::cout << "==========[test log] hdi_device: type = " << type << std::endl;
    }
    Test_->rc = Test_->cameraDevice->SetResultMode(Camera::ON_CHANGED);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
}

/**
  * @tc.name: GetEnabledResults
  * @tc.desc: GetEnabledResults expected success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0080, Function | MediumTest | Level1)
{
    std::cout << "==========[test log]Check hdi_device: GetEnabledResults expected success." << std::endl;
    Test_->Open();
    std::vector<Camera::MetaType> results;
    Test_->rc = Test_->cameraDevice->GetEnabledResults(results);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::cout << "GetEnabledResults is :" << std::endl;
    for (int i=0; i<results.size(); ++i) {
        std::cout << results.at(i) << std::endl;
    }
}

/**
  * @tc.name: EnableResult
  * @tc.desc: EnableResult one tag, without preview, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0090, Function | MediumTest | Level1)
{
    std::cout << "==========[test log]Check hdi_device: EnableResult one tag, without preview, success." << std::endl;
    Test_->Open();
    std::cout << "==========[test log]Check hdi_device: 1. Get the tags..." << std::endl;
    std::vector<Camera::MetaType> enable_tag;
    enable_tag.push_back(OHOS_SENSOR_EXPOSURE_TIME);
    std::cout << "==========[test log]Check hdi_device: 2. Enable the tag: " << enable_tag[0] << std::endl;
    Test_->rc = Test_->cameraDevice->EnableResult(enable_tag);
    // add this tag
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::vector<Camera::MetaType> results_original;
    Test_->rc = Test_->cameraDevice->GetEnabledResults(results_original);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
}

/**
  * @tc.name: EnableResult
  * @tc.desc: EnableResult multiple tags, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0091, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_device: EnableResult multiple tags, success." << std::endl;
    Test_->Open();
    std::vector<Camera::MetaType> results;
    results.push_back(OHOS_SENSOR_EXPOSURE_TIME);
    results.push_back(OHOS_SENSOR_COLOR_CORRECTION_GAINS);
    Test_->rc = Test_->cameraDevice->EnableResult(results);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::vector<Camera::MetaType> results_original;
    Test_->rc = Test_->cameraDevice->GetEnabledResults(results_original);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);

    // 2、Disable all tags
    std::cout << "then, disable the tag..." << std::endl;
    Test_->rc = Test_->cameraDevice->DisableResult(results_original);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);

    // 3、Get the parameter tag currently supported by the device again
    Test_->rc = Test_->cameraDevice->GetEnabledResults(results);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);

    // Add multiple tags
    std::cout << "then, enable the tag..." << std::endl;
    Test_->rc = Test_->cameraDevice->EnableResult(results_original);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);

    // Get the parameter tag currently supported by the device again
    Test_->rc = Test_->cameraDevice->GetEnabledResults(results);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
}

/**
  * @tc.name: EnableResult
  * @tc.desc: EnableResult error tag, expected success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0092, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_device: EnableResult error tag, expected fail." << std::endl;
    Test_->Open();
    // Get the parameter tag currently supported by the device
    std::vector<Camera::MetaType> results_original;
    Test_->rc = Test_->cameraDevice->GetEnabledResults(results_original);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);

    // add a tag
    std::vector<Camera::MetaType> enable_tag;
    enable_tag.push_back(0);
    std::cout << "then, enable the tag..." << std::endl;
    Test_->rc = Test_->cameraDevice->EnableResult(enable_tag);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
}

/**
  * @tc.name: DisableResult
  * @tc.desc: DisableResult one tag, without preview, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0100, Function | MediumTest | Level1)
{
    std::cout << "==========[test log]Check hdi_device: DisEnabledReuslts, expected success." << std::endl;
    Test_->Open();
    std::vector<Camera::MetaType> results;
    results.push_back(OHOS_SENSOR_EXPOSURE_TIME);
    results.push_back(OHOS_SENSOR_COLOR_CORRECTION_GAINS);
    Test_->rc = Test_->cameraDevice->EnableResult(results);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::vector<Camera::MetaType> results_original;
    Test_->rc = Test_->cameraDevice->GetEnabledResults(results_original);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::cout << "==========[test log]GetEnabledResults, size = " << results_original.size() << std::endl;

    // Disable a tag
    std::vector<Camera::MetaType> disable_tag;
    disable_tag.push_back(results_original[1]);
    Test_->rc = Test_->cameraDevice->DisableResult(disable_tag);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::cout << "==========[test log]Check hdi_device: DisableResult the tag:" << results_original[0] << std::endl;

    // Get the parameter tag currently supported by the device again
    Test_->rc = Test_->cameraDevice->GetEnabledResults(results);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
}

/**
  * @tc.name: DisableResult
  * @tc.desc: DisableResult all tag, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0101, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_device: DisableResult all tag, success." << std::endl;
    Test_->Open();
    std::vector<Camera::MetaType> results;
    results.push_back(OHOS_SENSOR_EXPOSURE_TIME);
    results.push_back(OHOS_SENSOR_COLOR_CORRECTION_GAINS);
    Test_->rc = Test_->cameraDevice->EnableResult(results);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::vector<Camera::MetaType> results_original;
    Test_->rc = Test_->cameraDevice->GetEnabledResults(results_original);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);

    // Disable all tags
    std::cout << "then, disable the tag..." << std::endl;
    Test_->rc = Test_->cameraDevice->DisableResult(results_original);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);

    // Get the parameter tag currently supported by the device again
    Test_->rc = Test_->cameraDevice->GetEnabledResults(results);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
}

/**
  * @tc.name: DisableResult
  * @tc.desc: DisableResult error tag, expected success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0102, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_device: DisableResult error tag, expected success." << std::endl;
    Test_->Open();
    std::vector<Camera::MetaType> results;
    results.push_back(OHOS_SENSOR_EXPOSURE_TIME);
    results.push_back(OHOS_SENSOR_COLOR_CORRECTION_GAINS);
    Test_->rc = Test_->cameraDevice->EnableResult(results);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    std::vector<Camera::MetaType> results_original;
    Test_->rc = Test_->cameraDevice->GetEnabledResults(results_original);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);

    // Disable a tag
    std::vector<Camera::MetaType> disable_tag;
    disable_tag.push_back(0);
    std::cout << "then, disenable the tag..." << std::endl;
    Test_->rc = Test_->cameraDevice->DisableResult(disable_tag);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);

    // Get the parameter tag currently supported by the device again
    Test_->rc = Test_->cameraDevice->GetEnabledResults(results);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
}

/**
  * @tc.name: Close
  * @tc.desc: Close the device.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiDeviceTest, Camera_Hdi_0110, Function | MediumTest | Level1)
{
    std::cout << "==========[test log]Check hdi_device: Close the device." << std::endl;
    Test_->Open();
    EXPECT_EQ(false, Test_->cameraDevice == nullptr);
    if (Test_->cameraDevice != nullptr) {
        Test_->cameraDevice->Close();
        std::cout << "==========[test log]Check hdi_device: Test_->cameraDevice->Close()." << std::endl;
        Test_->cameraDevice = nullptr;
    }
}