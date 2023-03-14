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
#include "flashlight_test.h"

using namespace OHOS;
using namespace std;
using namespace testing::ext;
using namespace OHOS::Camera;

void FlashlightTest::SetUpTestCase(void) {}
void FlashlightTest::TearDownTestCase(void) {}
void FlashlightTest::SetUp(void)
{
    Test_ = std::make_shared<OHOS::Camera::Test>();
    Test_->Init();
    Test_->GetCameraAbility();
}
void FlashlightTest::TearDown(void)
{
    Test_->Close();
}

/**
  * @tc.name: flashlight
  * @tc.desc: Turn on the flashlight, not the camera, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(FlashlightTest, Camera_Flashlight_0001, TestSize.Level0)
{
    std::cout << "==========[test log]Turn on the flashlight, not the camera, success." << std::endl;
    Test_->status = true;
    Test_->rc = Test_->service->SetFlashlight(Test_->cameraIds.front(), Test_->status);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    sleep(5);
}

/**
  * @tc.name: flashlight
  * @tc.desc: Turn off the flashlight, not the camera, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(FlashlightTest, Camera_Flashlight_0002, TestSize.Level0)
{
    std::cout << "==========[test log]Flashlight turn off, not the camera, success." << std::endl;
    std::cout << "==========[test log]1. turn on flashlight for 5s." << std::endl;
    Test_->status = true;
    Test_->rc = Test_->service->SetFlashlight(Test_->cameraIds.front(), Test_->status);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    sleep(3);
    std::cout << "==========[test log]2. turn off flashlight." << std::endl;
    Test_->status = false;
    Test_->rc = Test_->service->SetFlashlight(Test_->cameraIds.front(), Test_->status);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
}

/**
  * @tc.name: flashlight
  * @tc.desc: Turn on/off the flashlight for 10 times, not the camera, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(FlashlightTest, Camera_Flashlight_0003, TestSize.Level3)
{
    std::cout << "==========[test log]Turn on/off the flashlight for 10 times,";
    std::cout <<" not the camera, success." << std::endl;
    for (int i = 0; i < 10; i++) {
        if (i%2 == 0) {
            Test_->status = true;
        } else {
            Test_->status = false;
        }
        Test_->rc = Test_->service->SetFlashlight(Test_->cameraIds.front(), Test_->status);
        if (Test_->rc != Camera::NO_ERROR) {
            std::cout << "==========[test log]Flashlight turn on fail, at the ";
            std::cout << (i+1) <<"times, RetCode is " << Test_->rc << std::endl;
        }
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
}

/**
  * @tc.name: flashlight
  * @tc.desc: Open the camera, preview, then turn on the flashlight, expected return errorcode.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(FlashlightTest, Camera_Flashlight_0010, TestSize.Level1)
{
    std::cout << "==========[test log]Open the camera, preview, then turn on the flashlight,";
    std::cout <<" expected return errorcode." << std::endl;
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    Test_->status = true;
    Test_->rc = Test_->service->SetFlashlight(Test_->cameraIds.front(), Test_->status);
    EXPECT_EQ(Test_->rc, Camera::METHOD_NOT_SUPPORTED);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]turn on the flashlight success." << std::endl;
    } else {
        std::cout << "==========[test log]turn on the flashlight fail, Test_->rc = ";
        std::cout << Test_->rc << std::endl;
    }
    std::cout << "==========[test log]turn off/on flashlight fail, preview go on, for 5s." << std::endl;
    sleep(5);
    // release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: flashlight
  * @tc.desc: Open the camera, preview, then turn off the flashlight, expected return errorcode.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(FlashlightTest, Camera_Flashlight_0011, TestSize.Level1)
{
    std::cout << "==========[test log]Open the camera, preview, then turn on the flashlight,";
    std::cout <<" expected return errorcode." << std::endl;
    Test_->Open();
    // Start stream
    Test_->intents = {Camera::PREVIEW};
    Test_->StartStream(Test_->intents);
    // Get preview
    Test_->StartCapture(Test_->streamId_preview, Test_->captureId_preview, false, true);
    Test_->status = false;
    Test_->rc = Test_->service->SetFlashlight(Test_->cameraIds.front(), Test_->status);
    EXPECT_EQ(Test_->rc, Camera::METHOD_NOT_SUPPORTED);
    if (Test_->rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]turn off the flashlight success." << std::endl;
    } else {
        std::cout << "==========[test log]turn off the flashlight fail,rc = " << Test_->rc << std::endl;
    }
    std::cout << "==========[test log]turn off the flashlight fail, preview go on, for 5s." << std::endl;
    sleep(5);
    // release stream
    Test_->captureIds = {Test_->captureId_preview};
    Test_->streamIds = {Test_->streamId_preview};
    Test_->StopStream(Test_->captureIds, Test_->streamIds);
    Test_->StopConsumer(Test_->intents);
}

/**
  * @tc.name: flashlight
  * @tc.desc: Turn on the flashlight ,then open the camera, the light will turn off.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(FlashlightTest, Camera_Flashlight_0020, TestSize.Level1)
{
    std::cout << "==========[test log]Turn on the flashlight for 5s,then open the camera,";
    std::cout <<" the light will turn off." << std::endl;
    Test_->status = true;
    Test_->rc = Test_->service->SetFlashlight(Test_->cameraIds.front(), Test_->status);
    sleep(5);
    // Open the camera, preview
    Test_->Open();
    // query the status of the flashlight, currently only supports callback reporting
    std::cout << "==========[test log]check the flashlight status is off." << std::endl;
    sleep(5);
}
