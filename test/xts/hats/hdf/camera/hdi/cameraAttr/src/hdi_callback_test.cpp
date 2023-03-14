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

#include "hdi_callback_test.h"

using namespace OHOS;
using namespace std;
using namespace testing::ext;
using namespace OHOS::Camera;

void HdiCallbackTest::SetUpTestCase(void) {}
void HdiCallbackTest::TearDownTestCase(void) {}
void HdiCallbackTest::SetUp(void)
{
    Test_ = std::make_shared<OHOS::Camera::Test>();
    Test_->Init();
}
void HdiCallbackTest::TearDown(void)
{
    Test_->Close();
}

/**
  * @tc.name: OnCameraStatus
  * @tc.desc: CameraHostCallback, OnCameraStatus
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiCallbackTest, Camera_Hdi_1001, Function | MediumTest | Level1)
{
    Test_->Open();
    Test_->onCameraStatusFlag = false;
    std::cout << "==========[test log]Preview stream, 640*480, expected success." << std::endl;
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
    if (Test_->onCameraStatusFlag) {
        std::cout << "==========[test log]OnCameraStatus" << std::endl;
    }
    EXPECT_EQ(Test_->onCameraStatusFlag, false);
}

/**
  * @tc.name: OnFlashlightStatus
  * @tc.desc: CameraHostCallback, OnFlashlightStatus
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiCallbackTest, Camera_Hdi_1002, Function | MediumTest | Level1)
{
    Test_->GetCameraAbility();
    Test_->onFlashlightStatusFlag = false;
    std::cout << "==========[test log]Turn on the flashlight, not the camera, success." << std::endl;
    Test_->status = true;
    Test_->rc = Test_->service->SetFlashlight(Test_->cameraIds.front(), Test_->status);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    Test_->status = false;
    Test_->rc = Test_->service->SetFlashlight(Test_->cameraIds.front(), Test_->status);
    sleep(5);
    if (Test_->onFlashlightStatusFlag) {
        std::cout << "==========[test log]OnFlashlightStatus" << std::endl;
    }
    EXPECT_EQ(Test_->onFlashlightStatusFlag, true);
}

/**
  * @tc.name: OnError
  * @tc.desc: CameraDeviceCallback, OnError.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiCallbackTest, Camera_Hdi_1010, Function | MediumTest | Level1)
{
    Test_->Open();
    Test_->onErrorFlag = false;
    std::cout << "==========[test log]Preview stream, 640*480, expected success." << std::endl;
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
    if (Test_->onErrorFlag) {
        std::cout << "==========[test log]OnError" << std::endl;
    }
    EXPECT_EQ(Test_->onErrorFlag, false);
}

/**
  * @tc.name: OnResult
  * @tc.desc: CameraDeviceCallback, OnResult.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiCallbackTest, Camera_Hdi_1011, Function | MediumTest | Level1)
{
    Test_->onResultFlag = false;
    Test_->Open();
    EXPECT_EQ(true, Test_->cameraDevice != nullptr);
    std::cout << "==========[test log]Check hdi_device: SetResultMode is PER_FRAME." << std::endl;
    std::vector<Camera::MetaType> results;
    results.push_back(OHOS_SENSOR_EXPOSURE_TIME);
    results.push_back(OHOS_SENSOR_COLOR_CORRECTION_GAINS);
    Test_->rc = Test_->cameraDevice->EnableResult(results);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    Test_->rc = Test_->cameraDevice->SetResultMode(Camera::PER_FRAME);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
    if (Test_->onResultFlag) {
        std::cout << "==========[test log]OnResult" << std::endl;
    }
    EXPECT_EQ(Test_->onResultFlag, true);
}

/**
  * @tc.name: OnCaptureStarted
  * @tc.desc: IStreamOpereatorCallback, OnCaptureStarted
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiCallbackTest, Camera_Hdi_1020, Function | MediumTest | Level1)
{
    Test_->Open();
    Test_->captureStartFlag = false;
    std::cout << "==========[test log]Preview stream, 640*480, expected success." << std::endl;
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
    if (Test_->captureStartFlag) {
        std::cout << "==========[test log]OnCaptureStarted" << std::endl;
    }
    EXPECT_EQ(Test_->captureStartFlag, true);
}

/**
  * @tc.name: OnCaptureEnded
  * @tc.desc: IStreamOpereatorCallback, OnCaptureEnded
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiCallbackTest, Camera_Hdi_1021, Function | MediumTest | Level1)
{
    Test_->Open();
    Test_->captureEndFlag = false;
    std::cout << "==========[test log]Preview stream, 640*480, expected success." << std::endl;
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    // release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
    if (Test_->captureEndFlag) {
        std::cout << "==========[test log]OnCaptureEnded" << std::endl;
    }
    EXPECT_EQ(Test_->captureEndFlag, true);
}

/**
  * @tc.name: OnCaptureError
  * @tc.desc: IStreamOpereatorCallback, OnCaptureError
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiCallbackTest, Camera_Hdi_1022, Function | MediumTest | Level1)
{
    std::cout << "==========[test log]IStreamOpereatorCallback, OnCaptureError" << std::endl;
}

/**
  * @tc.name: OnFrameShutter
  * @tc.desc: IStreamOpereatorCallback, OnFrameShutter
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiCallbackTest, Camera_Hdi_1023, Function | MediumTest | Level1)
{
    Test_->Open();
    Test_->frameShutterFlag = false;
    std::cout << "==========[test log]Preview stream, 640*480, expected success." << std::endl;
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, true, true);
    // release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds.push_back(Test_->streamId_preview);
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
    if (Test_->frameShutterFlag) {
        std::cout << "==========[test log]OnFrameShutter" << std::endl;
    }
    EXPECT_EQ(Test_->frameShutterFlag, true);
}