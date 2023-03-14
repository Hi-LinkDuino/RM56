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
#include "open_camera_test.h"

using namespace OHOS;
using namespace std;
using namespace testing::ext;
using namespace OHOS::Camera;

void OpenCameraTest::SetUpTestCase(void) {}
void OpenCameraTest::TearDownTestCase(void) {}
void OpenCameraTest::SetUp(void)
{
    Test_ = std::make_shared<OHOS::Camera::Test>();
    Test_->Init();
}
void OpenCameraTest::TearDown(void)
{
    ;
}

/**
  * @tc.name: OpenCamera
  * @tc.desc: OpenCamera, success.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(OpenCameraTest, Camera_Open_0001, TestSize.Level0)
{
    std::cout << "==========[test log]OpenCamera, success."<< std::endl;
    Test_->service->GetCameraIds(Test_->cameraIds);
    std::cout << "==========[test log]cameraIds size = "<< Test_->cameraIds.size() << std::endl;
    for (auto &cameraId : Test_->cameraIds) {
        std::cout << "==========[test log]cameraId = " << cameraId << std::endl;
    }
    std::string cameraId = Test_->cameraIds.front();
    Test_->CreateDeviceCallback();
    Test_->rc = Test_->service->OpenCamera(cameraId, Test_->deviceCallback, Test_->cameraDevice);
    EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
    if (Test_->rc== Camera::NO_ERROR) {
        std::cout << "==========[test log]OpenCamera success." << std::endl;
    } else {
        std::cout << "==========[test log]OpenCamera fail, rc = " << Test_->rc << std::endl;
    }
    if (Test_->cameraDevice != nullptr) {
        Test_->cameraDevice->Close();
        std::cout << "==========[test log]Test_->cameraDevice->Close()." << std::endl;
    }
}

/**
  * @tc.name: Open all Cameras
  * @tc.desc: Open every Cameras what the getCameraId get.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(OpenCameraTest, Camera_Open_0010, TestSize.Level2)
{
    std::cout << "==========[test log]Open all Cameras."<< std::endl;
    Test_->service->GetCameraIds(Test_->cameraIds);
    for (auto &cameraId : Test_->cameraIds) {
        std::cout << "cameraId = " << cameraId << std::endl;
        Test_->CreateDeviceCallback();
        Test_->rc = Test_->service->OpenCamera(cameraId, Test_->deviceCallback, Test_->cameraDevice);
        EXPECT_EQ(Test_->rc, Camera::NO_ERROR);
        if (Test_->rc== Camera::NO_ERROR) {
        std::cout << "==========[test log]OpenCamera success, cameraId = " << cameraId << std::endl;
        } else {
            std::cout << "==========[test log]OpenCamera fail, Test_->rc = ";
            std::cout << Test_->rc << ", cameraId = " << cameraId << std::endl;
        }

        if (Test_->cameraDevice != nullptr) {
            Test_->cameraDevice->Close();
        }
    }
}

int main(int argc, char *argv[])
{
    // start test
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
