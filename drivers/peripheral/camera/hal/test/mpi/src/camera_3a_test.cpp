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
#include "camera_3a_test.h"

using namespace OHOS;
using namespace std;
using namespace testing::ext;
using namespace OHOS::Camera;

void Camera3ATest::SetUpTestCase(void) {}
void Camera3ATest::TearDownTestCase(void) {}
void Camera3ATest::SetUp(void)
{
    Test_ = std::make_shared<OHOS::Camera::Test>();
    Test_->Init();
    Test_->Open();
}
void Camera3ATest::TearDown(void)
{
    Test_->Close();
}

/**
  * @tc.name: updateSettings AE
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AE_EXPOSURE_COMPENSATION, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(Camera3ATest, Camera_3a_0001, TestSize.Level0)
{
    std::cout << "==========[test log]Preview，then UpdateSettings ";
    std::cout <<"OHOS_CAMERA_AE_EXPOSURE_COMPENSATION, success." << std::endl;
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    std::shared_ptr<CameraStandard::CameraMetadata> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xa0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    Test_->cameraDevice->UpdateSettings(meta);
    std::cout << "==========[test log]UpdateSettings, exposure for 10s." << std::endl;
    sleep(5);
    Test_->GetCameraMetadata();
    // release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_AUTO, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(Camera3ATest, Camera_3a_0002, TestSize.Level1)
{
    std::cout << "==========[test log]Preview，updateSettings";
    std::cout <<" OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_AUTO, success." << std::endl;
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_AUTO;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log]UpdateSettings, awb : OHOS_CAMERA_AWB_MODE_AUTO." << std::endl;
    Test_->cameraDevice->UpdateSettings(meta);
    sleep(2);
    // release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_INCANDESCENT, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(Camera3ATest, Camera_3a_0003, TestSize.Level1)
{
    std::cout << "==========[test log]Preview，updateSettings ";
    std::cout <<"OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_INCANDESCENT, success." << std::endl;
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_INCANDESCENT;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log]UpdateSettings, awb : OHOS_CAMERA_AWB_MODE_INCANDESCENT." << std::endl;
    Test_->cameraDevice->UpdateSettings(meta);
    sleep(2);
    // release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_FLUORESCENT, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(Camera3ATest, Camera_3a_0004, TestSize.Level1)
{
    std::cout << "==========[test log]Preview，updateSettings ";
    std::cout <<"OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_FLUORESCENT, success." << std::endl;
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_FLUORESCENT;
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log]UpdateSettings, awb : OHOS_CAMERA_AWB_MODE_FLUORESCENT." << std::endl;
    Test_->cameraDevice->UpdateSettings(meta);
    // release stream
    sleep(2);
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_WARM_FLUORESCENT, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(Camera3ATest, Camera_3a_0005, TestSize.Level1)
{
    std::cout << "==========[test log]Preview，updateSettings ";
    std::cout <<"OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_WARM_FLUORESCENT, success." << std::endl;
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_WARM_FLUORESCENT;
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log]UpdateSettings, awb : OHOS_CAMERA_AWB_MODE_WARM_FLUORESCENT." << std::endl;
    Test_->cameraDevice->UpdateSettings(meta);
    // release stream
    sleep(2);
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_DAYLIGHT, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(Camera3ATest, Camera_3a_0006, TestSize.Level1)
{
    std::cout << "==========[test log]Preview, updateSettings ";
    std::cout <<"OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_DAYLIGHT, success." << std::endl;
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_DAYLIGHT;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log]UpdateSettings, awb : OHOS_CAMERA_AWB_MODE_DAYLIGHT." << std::endl;
    Test_->cameraDevice->UpdateSettings(meta);
    sleep(2);
    std::cout << "==========[test log]GetCameraMetadata ..." << std::endl;
    Test_->GetCameraMetadata();
    // release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_CLOUDY_DAYLIGHT, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(Camera3ATest, Camera_3a_0007, TestSize.Level1)
{
    std::cout << "==========[test log]Preview，updateSettings ";
    std::cout <<"OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_CLOUDY_DAYLIGHT, success." << std::endl;
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_CLOUDY_DAYLIGHT;
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log]UpdateSettings, awb : OHOS_CAMERA_AWB_MODE_CLOUDY_DAYLIGHT." << std::endl;
    Test_->cameraDevice->UpdateSettings(meta);
    // release stream
    sleep(2);
    std::cout << "==========[test log]GetCameraMetadata ..." << std::endl;
    Test_->GetCameraMetadata();
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_TWILIGHT, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(Camera3ATest, Camera_3a_0008, TestSize.Level1)
{
    std::cout << "==========[test log]Preview，updateSettings ";
    std::cout <<"OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_TWILIGHT, success." << std::endl;
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_TWILIGHT;
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log]UpdateSettings, awb : OHOS_CAMERA_AWB_MODE_TWILIGHT." << std::endl;
    Test_->cameraDevice->UpdateSettings(meta);
    // release stream
    sleep(2);
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_SHADE, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(Camera3ATest, Camera_3a_0009, TestSize.Level1)
{
    std::cout << "==========[test log]Preview，updateSettings ";
    std::cout <<"OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_SHADE, success." << std::endl;
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_SHADE;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log]UpdateSettings, awb : OHOS_CAMERA_AWB_MODE_SHADE." << std::endl;
    Test_->cameraDevice->UpdateSettings(meta);
    sleep(2);
    // release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: updateSettings AWB
  * @tc.desc: Preview，updateSettings OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_OFF, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(Camera3ATest, Camera_3a_0010, TestSize.Level1)
{
    std::cout << "==========[test log]Preview，updateSettings ";
    std::cout <<"OHOS_CAMERA_AWB_MODE-OHOS_CONTROL_AWB_MODE_OFF, success." << std::endl;
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_OFF;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    std::cout << "==========[test log]UpdateSettings, awb : OHOS_CAMERA_AWB_MODE_OFF." << std::endl;
    Test_->cameraDevice->UpdateSettings(meta);
    sleep(2);
    // release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: updateSettings illegal value
  * @tc.desc: UpdateSettings, OHOS_CAMERA_AWB_MODE: illegal value(input AE value).
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(Camera3ATest, Camera_3a_0020, TestSize.Level2) {
    std::cout << "==========[test log]UpdateSettings, ";
    std::cout <<"OHOS_CAMERA_AWB_MODE: illegal value(input AE value)." << std::endl;
    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AE_MODE_ON_ALWAYS_FLASH;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    Test_->rc = Test_->cameraDevice->UpdateSettings(meta);
    std::cout << "rc = " << Test_->rc << std::endl;
    EXPECT_EQ(true, Test_->rc == Camera::NO_ERROR);
}

/**
  * @tc.name: updateSettings reset
  * @tc.desc: Preview set 3A, then close device, and preview, 3A is reset.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(Camera3ATest, Camera_3a_0030, TestSize.Level2)
{
    std::cout << "==========[test log]Preview set 3A, ";
    std::cout <<"then close device, and preview, 3A is reset." << std::endl;
    std::cout << "==========[test log]The 1st time set 3A." << std::endl;
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xa0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    Test_->cameraDevice->UpdateSettings(meta);
    std::cout << "==========[test log]UpdateSettings, exposure for 3s." << std::endl;
    sleep(3);
    // release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
    // Turn off the device
    Test_->cameraDevice->Close();
    std::cout << "cameraDevice->Close" << std::endl;
    std::cout << "==========[test log]Close device, and preview, 3A is reset." << std::endl;
    Test_->consumerMap_.clear();
    // reopen the camera
    Test_ = std::make_shared<OHOS::Camera::Test>();
    Test_->Init();
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
  * @tc.name: updateSettings different 3A params
  * @tc.desc: Preview, updatesetting different 3A params together.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(Camera3ATest, Camera_3a_0031, TestSize.Level1)
{
    std::cout << "==========[test log]Preview, updatesetting different 3A params together." << std::endl;
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xa0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_TWILIGHT;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    Test_->cameraDevice->UpdateSettings(meta);
    std::cout << "==========[test log]UpdateSettings, exposure for 10s." << std::endl;
    sleep(10);
    std::cout << "==========[test log]GetCameraMetadata ..." << std::endl;
    Test_->GetCameraMetadata();
    // release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: updateSettings AE without preview
  * @tc.desc: UpdateSettings-OHOS_CAMERA_AE_EXPOSURE_COMPENSATION, without preview, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(Camera3ATest, Camera_3a_0040, TestSize.Level2) {
    std::cout << "UpdateSettings-OHOS_CAMERA_AE_EXPOSURE_COMPENSATION, success." << std::endl;

    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    int32_t expo = 0xa0;
    meta->addEntry(OHOS_CONTROL_AE_EXPOSURE_COMPENSATION, &expo, 1);
    Test_->cameraDevice->UpdateSettings(meta);
}

/**
  * @tc.name: updateSettings AWB without preview
  * @tc.desc: UpdateSettings-OHOS_CAMERA_AWB_MODE, without preview, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(Camera3ATest, Camera_3a_0041, TestSize.Level2) {
    std::cout << "UpdateSettings-OHOS_CAMERA_AWB_MODE, success." << std::endl;

    // Issue 3A parameters
    std::shared_ptr<Camera::CameraSetting> meta = std::make_shared<Camera::CameraSetting>(100, 2000);
    uint8_t awbMode = OHOS_CAMERA_AWB_MODE_AUTO;
    meta->addEntry(OHOS_CONTROL_AWB_MODE, &awbMode, 1);
    Test_->cameraDevice->UpdateSettings(meta);
}