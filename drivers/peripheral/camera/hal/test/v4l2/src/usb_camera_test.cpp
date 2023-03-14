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
#include "usb_camera_test.h"

void UtestUSBCameraTest::SetUpTestCase(void)
{}
void UtestUSBCameraTest::TearDownTestCase(void)
{}
void UtestUSBCameraTest::SetUp(void)
{
    if (display_ == nullptr)
    display_ = std::make_shared<TestDisplay>();
    display_->FBInit();
    display_->UsbInit();
}
void UtestUSBCameraTest::TearDown(void)
{
    display_->Close();
}

/**
  * @tc.name: USB Camera
  * @tc.desc: USB Camera, getCameraID success.
  * @tc.level: Level0
  * @tc.size: MediumTest
  * @tc.type: Function
  */
TEST_F(UtestUSBCameraTest, camera_usb_0001)
{
    std::cout << "==========[test log] USB Camera, getCameraID success."<< std::endl;
    std::vector<std::string> cameraIds;
    std::cout << "==========[test log] 1. get current system cameraID."<< std::endl;
    display_->cameraHost->GetCameraIds(cameraIds);
    std::cout << "==========[test log] Fisrt cameraId.size = " << cameraIds.size() << std::endl;
    std::cout << "==========[test log] OnCameraStatus interface has been mobilized" << std::endl;
    for (auto &cameraId : cameraIds) {
        std::cout << "==========[test log] cameraId = " << cameraId << std::endl;
    }
    std::cout << "==========[test log] 2. please add or delete the usb camera, wait for 10s..."<< std::endl;
    sleep(10);
    std::cout << "==========[test log] r u ready? wait for 10s..."<< std::endl;
    sleep(10);
    std::cout << "==========[test log] 3. check zhe cameraID again... wait for 10s..."<< std::endl;
    sleep(10);
    std::cout << "==========[test log] Second cameraId.size = " << cameraIds.size() << std::endl;
    if (cameraIds.size() == 1) {
        cameraIds.clear();
    }
    display_->cameraHost->GetCameraIds(cameraIds);
    for (auto &cameraId : cameraIds) {
        std::cout << "cameraId = " << cameraId << std::endl;
    }
}