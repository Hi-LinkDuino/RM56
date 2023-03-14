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
#include "device_manager_test.h"

using namespace OHOS;
using namespace std;
using namespace testing::ext;
using namespace OHOS::Camera;

void DeviceManagerTest::SetUpTestCase(void) {}
void DeviceManagerTest::TearDownTestCase(void) {}
void DeviceManagerTest::SetUp(void)
{
    Test_ = std::make_shared<OHOS::Camera::Test>();
    Test_->Init();
    Test_->Open();
}
void DeviceManagerTest::TearDown(void)
{
    Test_->Close();
}

/**
  * @tc.name: Check device manager.
  * @tc.desc: The GBM/ION buffer command is requested, the parameters are normal,
  * and the buffer application through the Display module is successful.
  * @tc.size: MediumTest
  * @tc.type: Function
  */
HWTEST_F(DeviceManagerTest, Camera_Device_0001, TestSize.Level2)
{
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
}