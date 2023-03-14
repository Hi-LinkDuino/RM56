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

#include "hdi_host_test.h"

using namespace OHOS;
using namespace std;
using namespace testing::ext;
using namespace OHOS::Camera;

void HdiHostTest::SetUpTestCase(void) {}
void HdiHostTest::TearDownTestCase(void) {}
void HdiHostTest::SetUp(void)
{
    Test_ = std::make_shared<OHOS::Camera::Test>();
    Test_->Init();
}
void HdiHostTest::TearDown(void)
{
    Test_->Close();
}

/**
  * @tc.name: SetCallback
  * @tc.desc: CamRetCode SetCallback([in] ICameraHostCallback callback);
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiHostTest, Camera_Hdi_0001, Function | MediumTest | Level1)
{
    std::cout << "==========[test log]Check hdi_host: init setcallback success." << std::endl;
}

/**
  * @tc.name: GetCameraIds
  * @tc.desc: CamRetCode GetCameraIds([out] String[] ids);
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiHostTest, Camera_Hdi_0010, Function | MediumTest | Level1)
{
    std::cout << "==========[test log]Check hdi_host: GetCameraIds([out] String[] ids)." << std::endl;
    if (Test_->cameraDevice == nullptr) {
        Test_->rc = Test_->service->GetCameraIds(Test_->cameraIds);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        EXPECT_LT(0, Test_->cameraIds.size());
        std::cout << "==========[test log]Check hdi_host:cameraIds.size()= ."<< Test_->cameraIds.size() << std::endl;
    }
}

/**
  * @tc.name: GetCameraAbility
  * @tc.desc: GetCameraAbility, normal cameraId.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiHostTest, Camera_Hdi_0020, Function | MediumTest | Level1)
{
    std::cout << "==========[test log]Check hdi_host: GetCameraAbility, normal cameraId." << std::endl;
    if (Test_->cameraDevice == nullptr) {
        Test_->rc = Test_->service->GetCameraIds(Test_->cameraIds);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        for (int i=0; i<Test_->cameraIds.size(); i++) {
            Test_->rc = Test_->service->GetCameraAbility(Test_->cameraIds[i], Test_->ability);
            std::cout << "==========[test log]Check hdi_host: cameraid = " << Test_->cameraIds[i] << std::endl;
            EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        }
    }
}

/**
  * @tc.name: GetCameraAbility
  * @tc.desc: GetCameraAbility, abnormal cameraId = 'abc'.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiHostTest, Camera_Hdi_0021, Function | MediumTest | Level2)
{
    std::string cameraId = "abc";
    std::cout << "==========[test log]Check hdi_host: GetCameraAbility, abnormal cameraId = 'abc'." << std::endl;
    if (Test_->cameraDevice == nullptr) {
        Test_->rc = Test_->service->GetCameraAbility(cameraId, Test_->ability);
        std::cout << "==========[test log]Check hdi_host: Test_->rc ="<< Test_->rc << std::endl;
        EXPECT_EQ(true, Test_->rc == Camera::CamRetCode::INVALID_ARGUMENT);
    }
}

/**
  * @tc.name: GetCameraAbility
  * @tc.desc: GetCameraAbility, abnormal cameraId = ''
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiHostTest, Camera_Hdi_0022, Function | MediumTest | Level2)
{
    std::string cameraId = "";
    std::cout << "==========[test log]Check hdi_host: GetCameraAbility, abnormal cameraId = ''." << std::endl;
    if (Test_->cameraDevice == nullptr) {
        Test_->rc = Test_->service->GetCameraAbility(cameraId, Test_->ability);
        std::cout << "==========[test log]Check hdi_host: Test_->rc ="<< Test_->rc << std::endl;
        EXPECT_EQ(true, Test_->rc == Camera::CamRetCode::INVALID_ARGUMENT);
    }
}

/**
  * @tc.name: OpenCamera
  * @tc.desc: OpenCamera, normal cameraId.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiHostTest, Camera_Hdi_0030, Function | MediumTest | Level1)
{
    std::cout << "==========[test log]Check hdi_host: OpenCamera, normal cameraId."<< std::endl;
    if (Test_->cameraDevice == nullptr) {
        Test_->service->GetCameraIds(Test_->cameraIds);
        Test_->CreateDeviceCallback();
        Test_->rc = Test_->service->OpenCamera(Test_->cameraIds.front(), Test_->deviceCallback, Test_->cameraDevice);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        if (Test_->rc != Camera::NO_ERROR || Test_->cameraDevice == nullptr) {
            std::cout << "==========[test log]Check hdi_host: OpenCamera failed." << std::endl;
            return;
        }
        std::cout << "==========[test log]Check hdi_host: OpenCamera success." << std::endl;
    }
}

/**
  * @tc.name: OpenCamera
  * @tc.desc: OpenCamera, cameraID is not found.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiHostTest, Camera_Hdi_0031, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_host: OpenCamera, cameraID is not found."<< std::endl;
    std::string cameraId = "qwerty";
    if (Test_->cameraDevice == nullptr) {
        Test_->CreateDeviceCallback();
        Test_->rc = Test_->service->OpenCamera(cameraId, Test_->deviceCallback, Test_->cameraDevice);
        EXPECT_EQ(true, Test_->rc == Camera::INVALID_ARGUMENT);
    }
}

/**
  * @tc.name: OpenCamera
  * @tc.desc: OpenCamera, cameraID is illegal.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiHostTest, Camera_Hdi_0032, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_host: OpenCamera, cameraID is illegal."<< std::endl;
    std::string cameraId = "1";
    if (Test_->cameraDevice == nullptr) {
        Test_->CreateDeviceCallback();
        Test_->rc = Test_->service->OpenCamera(cameraId, Test_->deviceCallback, Test_->cameraDevice);
        EXPECT_EQ(true, Test_->rc == Camera::INVALID_ARGUMENT);
    }
}

/**
  * @tc.name: OpenCamera
  * @tc.desc: OpenCamera, cameraID is Empty.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiHostTest, Camera_Hdi_0033, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_host: OpenCamera, cameraID is Empty."<< std::endl;
    std::string cameraId = "";
    if (Test_->cameraDevice == nullptr) {
        Test_->CreateDeviceCallback();
        Test_->rc = Test_->service->OpenCamera(cameraId, Test_->deviceCallback, Test_->cameraDevice);
        EXPECT_EQ(true, Test_->rc == Camera::INVALID_ARGUMENT);
    }
}

/**
  * @tc.name: OpenCamera
  * @tc.desc: OpenCamera, Callback is Null.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiHostTest, Camera_Hdi_0034, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_host: OpenCamera, Callback is Null."<< std::endl;
    if (Test_->cameraDevice == nullptr) {
        Test_->service->GetCameraIds(Test_->cameraIds);
        Test_->deviceCallback = nullptr;
        Test_->rc = Test_->service->OpenCamera(Test_->cameraIds.front(), Test_->deviceCallback, Test_->cameraDevice);
        EXPECT_EQ(true, Test_->rc == Camera::INVALID_ARGUMENT);
    }
}

/**
  * @tc.name: OpenCamera
  * @tc.desc: cameraID is not found, callback is null.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiHostTest, Camera_Hdi_0035, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_host: OpenCamera, cameraID is not found, callback is null."<< std::endl;
    std::string cameraId = "abc";
    if (Test_->cameraDevice == nullptr) {
        Test_->CreateDeviceCallback();
        Test_->rc = Test_->service->OpenCamera(cameraId, Test_->deviceCallback, Test_->cameraDevice);
        EXPECT_EQ(true, Test_->rc == Camera::INVALID_ARGUMENT);
    }
}

/**
  * @tc.name: SetFlashlight
  * @tc.desc: SetFlashlight, normal cameraId.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiHostTest, Camera_Hdi_0040, Function | MediumTest | Level1)
{
    std::cout << "==========[test log]Check hdi_host: SetFlashlight, normal cameraId." << std::endl;
    if (Test_->cameraDevice == nullptr) {
        Test_->service->GetCameraIds(Test_->cameraIds);
        Test_->status = true;
        Test_->rc = Test_->service->SetFlashlight(Test_->cameraIds.front(), Test_->status);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
}

/**
  * @tc.name: SetFlashlight
  * @tc.desc: SetFlashlight, cameraId is not found.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiHostTest, Camera_Hdi_0041, Function | MediumTest | Level1)
{
    std::cout << "==========[test log]Check hdi_host: SetFlashlight, cameraId is not found." << std::endl;
    if (Test_->cameraDevice == nullptr) {
        std::string cameraId = "abc";
        Test_->status = true;
        Test_->rc = Test_->service->SetFlashlight(cameraId, Test_->status);
        EXPECT_EQ(true, Test_->rc == Camera::INVALID_ARGUMENT);
    }
}

/**
  * @tc.name: SetFlashlight
  * @tc.desc: SetFlashlight, cameraId is empty.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiHostTest, Camera_Hdi_0042, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_host: SetFlashlight, cameraId is empty." << std::endl;
    if (Test_->cameraDevice == nullptr) {
        std::string cameraId = "";
        Test_->status = true;
        Test_->rc = Test_->service->SetFlashlight(cameraId, Test_->status);
        EXPECT_EQ(true, Test_->rc == Camera::INVALID_ARGUMENT);
    }
}

/**
  * @tc.name: SetFlashlight
  * @tc.desc: SetFlashlight, status is true.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiHostTest, Camera_Hdi_0043, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_host: SetFlashlight, normal cameraId." << std::endl;
    if (Test_->cameraDevice == nullptr) {
        Test_->service->GetCameraIds(Test_->cameraIds);
        Test_->status = true;
        Test_->rc = Test_->service->SetFlashlight(Test_->cameraIds.front(), Test_->status);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
}

/**
  * @tc.name: SetFlashlight
  * @tc.desc: SetFlashlight, status is false.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(HdiHostTest, Camera_Hdi_0044, Function | MediumTest | Level2)
{
    std::cout << "==========[test log]Check hdi_host: SetFlashlight, normal cameraId." << std::endl;
    if (Test_->cameraDevice == nullptr) {
        Test_->service->GetCameraIds(Test_->cameraIds);
        Test_->status = false;
        Test_->rc = Test_->service->SetFlashlight(Test_->cameraIds.front(), Test_->status);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    }
}
