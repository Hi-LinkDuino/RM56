/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "open_camera_test.h"

void UtestOpenCameraTest::SetUpTestCase(void)
{}
void UtestOpenCameraTest::TearDownTestCase(void)
{}
void UtestOpenCameraTest::SetUp(void)
{
    std::cout << "==========[test log] UtestOpenCameraTest::SetUp.display_0： "<<display_<< std::endl;
    if (display_ == nullptr) {
        std::cout << "==========[test log] UtestOpenCameraTest::SetUp.display_1： "<<display_<< std::endl;
        display_ = std::make_shared<TestDisplay>();
        std::cout << "==========[test log] UtestOpenCameraTest::SetUp.display_2： "<<display_<< std::endl;
        display_->FBInit();
        std::cout << "==========[test log] UtestOpenCameraTest::SetUp.display_3： "<<display_<< std::endl;
        display_->Init();
        std::cout << "==========[test log] UtestOpenCameraTest::SetUp.display_4： "<<display_<< std::endl;
    }
}
void UtestOpenCameraTest::TearDown(void)
{
    display_->Close();
}

/**
  * @tc.name: OpenCamera
  * @tc.desc: OpenCamera, success.
  * @tc.level: Level0
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestOpenCameraTest, camera_open_0001)
{
    std::cout << "==========[test log] OpenCamera, success."<< std::endl;
    std::vector<std::string> cameraIds;
    display_->cameraHost->GetCameraIds(cameraIds);
    for (auto &cameraId : cameraIds) {
        std::cout << "cameraId = " << cameraId << std::endl;
    }
    std::string cameraId = cameraIds.front();
    const std::shared_ptr<OHOS::Camera::ICameraDeviceCallback> callback =
        std::make_shared<OHOS::Camera::ICameraDeviceCallback>();
    std::shared_ptr<OHOS::Camera::ICameraDevice> cameraDevice;
    display_->rc = display_->cameraHost->OpenCamera(cameraId, callback, cameraDevice);
    EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
}

/**
  * @tc.name: OpenCamera cameraID input error
  * @tc.desc: OpenCamera, cameraID is not found.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestOpenCameraTest, camera_open_0010)
{
    std::cout << "==========[test log] OpenCamera, cameraID is not found."<< std::endl;
    std::shared_ptr<OHOS::Camera::CameraHost> cameraHost = Camera::CameraHost::CreateCameraHost();
    std::string cameraId = "qwerty";
    std::shared_ptr<OHOS::Camera::ICameraDeviceCallback> callback =
        std::make_shared<OHOS::Camera::ICameraDeviceCallback>();
    std::cout << "opencamera begin" << std::endl;
    display_->rc = cameraHost->OpenCamera(cameraId, callback, display_->cameraDevice);
    std::cout << "opencamera end" << std::endl;
    EXPECT_EQ(Camera::CamRetCode::INVALID_ARGUMENT, display_->rc);
}

/**
  * @tc.name: OpenCamera cameraID input error
  * @tc.desc: OpenCamera, cameraID is illegal.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestOpenCameraTest, camera_open_0011)
{
    std::cout << "==========[test log] OpenCamera, cameraID is illegal."<< std::endl;
    std::string cameraId = "1";
    std::shared_ptr<OHOS::Camera::ICameraDeviceCallback> callback =
        std::make_shared<OHOS::Camera::ICameraDeviceCallback>();
    display_->rc = display_->cameraHost->OpenCamera(cameraId, callback, display_->cameraDevice);
    EXPECT_EQ(Camera::CamRetCode::INVALID_ARGUMENT, display_->rc);
}

/**
  * @tc.name: OpenCamera cameraID input error
  * @tc.desc: OpenCamera, cameraID is Empty.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestOpenCameraTest, camera_open_0012)
{
    std::cout << "==========[test log] OpenCamera, cameraID is Empty."<< std::endl;
    std::string cameraId;
    std::shared_ptr<OHOS::Camera::ICameraDeviceCallback> callback =
        std::make_shared<OHOS::Camera::ICameraDeviceCallback>();
    display_->rc = display_->cameraHost->OpenCamera(cameraId, callback, display_->cameraDevice);
    EXPECT_EQ(Camera::CamRetCode::INVALID_ARGUMENT, display_->rc);
}

/**
  * @tc.name: OpenCamera Callback input error
  * @tc.desc: OpenCamera, Callback is Null.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestOpenCameraTest, camera_open_0020)
{
    std::cout << "==========[test log] OpenCamera, Callback is Null."<< std::endl;
    std::string cameraId = "CAMERA_FIRST";
    std::shared_ptr<OHOS::Camera::ICameraDeviceCallback> callback = nullptr;
    display_->rc = display_->cameraHost->OpenCamera(cameraId, callback, display_->cameraDevice);
    EXPECT_EQ(Camera::CamRetCode::INVALID_ARGUMENT, display_->rc);
}

/**
  * @tc.name: OpenCamera cameraID & Callback input error
  * @tc.desc: OpenCamera, cameraID is not found, callback is null.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestOpenCameraTest, camera_open_0030)
{
    std::cout << "==========[test log] OpenCamera, cameraID is not found, callback is null."<< std::endl;
    std::shared_ptr<OHOS::Camera::CameraHost> cameraHost = Camera::CameraHost::CreateCameraHost();
    std::string cameraId = "qwerty";
    std::shared_ptr<OHOS::Camera::ICameraDeviceCallback> callback = nullptr;
    std::cout << "opencamera begin" << std::endl;
    display_->rc = display_->cameraHost->OpenCamera(cameraId, callback, display_->cameraDevice);
    std::cout << "opencamera end" << std::endl;
    EXPECT_EQ(Camera::CamRetCode::INVALID_ARGUMENT, display_-> rc);
}

/**
  * @tc.name: OpenCamera cameraID & Callback input error
  * @tc.desc: OpenCamera, cameraID is illegal, callback is null.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestOpenCameraTest, camera_open_0031)
{
    std::cout << "==========[test log] OpenCamera, cameraID is illegal, callback is null."<< std::endl;
    std::string cameraId = "1";
    std::shared_ptr<OHOS::Camera::ICameraDeviceCallback> callback = nullptr;
    display_->rc = display_->cameraHost->OpenCamera(cameraId, callback, display_->cameraDevice);
    EXPECT_EQ(Camera::CamRetCode::INVALID_ARGUMENT, display_-> rc);
}

/**
  * @tc.name: OpenCamera cameraID & Callback input error
  * @tc.desc: OpenCamera, cameraID is Empty, callback is null.
  * @tc.level: Level1
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestOpenCameraTest, camera_open_0032)
{
    std::cout << "==========[test log] OpenCamera, cameraID is Empty, callback is null."<< std::endl;
    std::string cameraId;
    std::shared_ptr<OHOS::Camera::ICameraDeviceCallback> callback = nullptr;
    display_->rc = display_->cameraHost->OpenCamera(cameraId, callback, display_->cameraDevice);
    EXPECT_EQ(Camera::CamRetCode::INVALID_ARGUMENT, display_->rc);
}

/**
  * @tc.name: Open all Cameras
  * @tc.desc: Open every Cameras what the getCameraId get.
  * @tc.level: Level2
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestOpenCameraTest, camera_open_0050)
{
    std::cout << "==========[test log] Open all Cameras."<< std::endl;
    std::vector<std::string> cameraIds;
    display_->cameraHost->GetCameraIds(cameraIds);
    for (auto &cameraId : cameraIds) {
        std::cout << "cameraId = " << cameraId << std::endl;
        const std::shared_ptr<OHOS::Camera::ICameraDeviceCallback> callback =
            std::make_shared<OHOS::Camera::ICameraDeviceCallback>();
        display_->rc = display_->cameraHost->OpenCamera(cameraId, callback, display_->cameraDevice);
        EXPECT_EQ(true, display_->rc == Camera::NO_ERROR);
        if (display_->rc == Camera::NO_ERROR) {
            std::cout << "==========[test log] OpenCamera success, cameraId = " << cameraId << std::endl;
            } else {
                std::cout << "==========[test log] OpenCamera fail, rc = ";
                std::cout << display_->rc << ", cameraId = " << cameraId << std::endl;
            }
        }
}

int main(int argc, char *argv[])
{
    // start running all tests
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}