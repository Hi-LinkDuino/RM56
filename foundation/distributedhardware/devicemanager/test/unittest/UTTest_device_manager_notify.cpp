/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "UTTest_device_manager_notify.h"
#include "device_manager_notify.h"
#include "dm_device_info.h"
#include "ipc_remote_broker.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "ipc_client_manager.h"
#include "ipc_set_useroperation_req.h"
#include "ipc_rsp.h"
#include "ipc_def.h"

#include <unistd.h>

namespace OHOS {
namespace DistributedHardware {
void DeviceManagerNotifyTest::SetUp()
{
}

void DeviceManagerNotifyTest::TearDown()
{
}

void DeviceManagerNotifyTest::SetUpTestCase()
{
}

void DeviceManagerNotifyTest::TearDownTestCase()
{
}

namespace {
/**
 * @tc.name: RegisterDeathRecipientCallback_001
 * @tc.desc: 1. set pkgName not null
 *              set dmInitCallback not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 * deviceTypeId
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDeathRecipientCallback_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DmInitCallback> dmInitCallback = std::make_shared<DmInitCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DmInitCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: RegisterDeathRecipientCallback_002
 * @tc.desc: 1. set pkgName not null
 *              set dmInitCallback null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap null
 * deviceTypeId
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDeathRecipientCallback_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback null
    std::shared_ptr<DmInitCallback> dmInitCallback = nullptr;
    // 2. set checkMap not null
    std::shared_ptr<DmInitCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    // 5. check checkMap null
    ASSERT_EQ(checkMap, nullptr);
}

/**
 * @tc.name: RegisterDeathRecipientCallback_003
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set dmInitCallback not null
 *           2. set checkMap null
 *           3. set testpkcName com.ohos.test1
 *           4. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
 *           5. Get checkMap from DeviceManagerNotify with testpkcName
 *           6. check checkMap null
 * deviceTypeId
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDeathRecipientCallback_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName com.ohos.test
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    std::shared_ptr<DmInitCallback> dmInitCallback = nullptr;
    // 2. set checkMap null
    std::shared_ptr<DmInitCallback> checkMap = nullptr;
    // 3. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 4. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    // 5. Get checkMap from DeviceManagerNotify with testpkcName
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[testPkgName];
    // 6. check checkMap not null
    ASSERT_EQ(checkMap, nullptr);
}

/**
 * @tc.name: RegisterDeathRecipientCallback_004
 * @tc.desc: 1. set pkgName not null
 *              set dmInitCallback not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. call checkMap OnRemoteDied
 *           7. check count is 1
 * deviceTypeId
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDeathRecipientCallback_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DmInitCallback> dmInitCallback = std::make_shared<DmInitCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DmInitCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call checkMap OnRemoteDied
    checkMap->OnRemoteDied();
    // 7. check count is 1
    ASSERT_EQ(count, 1);
}

/**
 * @tc.name: RegisterDeathRecipientCallback_005
 * @tc.desc: 1. set pkgName not null
 *              set dmInitCallback not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. call checkMap OnRemoteDied
 *           7. check count is 1
 * deviceTypeId
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDeathRecipientCallback_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DmInitCallback> dmInitCallback = nullptr;
    // 2. set checkMap null
    std::shared_ptr<DmInitCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    // 5. check checkMap not null
    if (checkMap == nullptr) {
        ASSERT_NE(count, 1);
        return;
    }
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterDeathRecipientCallback_001
 * @tc.desc: 1. set pkgName not null
 *              set dmInitCallback not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. call DeviceManagerNotifyTest UnRegisterDeathRecipientCallback with parameter
 *           7. Get checkMap from DeviceManagerNotify
 *           8. check checkMap null
 * deviceTypeId
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDeathRecipientCallback_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DmInitCallback> dmInitCallback = std::make_shared<DmInitCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DmInitCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotifyTest UnRegisterDeathRecipientCallback with parameter
    DeviceManagerNotify::GetInstance().UnRegisterDeathRecipientCallback(pkgName);
    // 7. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    // 8 check checkMap null
    ASSERT_EQ(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterDeathRecipientCallback_002
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set dmInitCallback not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. set testpkcName com.ohos.test1
 *           7. call DeviceManagerNotifyTest UnRegisterDeathRecipientCallback with testpkcName
 *           8. Get checkMap from DeviceManagerNotify
 *           9. check checkMap not null
 * deviceTypeId
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDeathRecipientCallback_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DmInitCallback> dmInitCallback = std::make_shared<DmInitCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DmInitCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 7. call DeviceManagerNotifyTest UnRegisterDeathRecipientCallback with testpkcName
    DeviceManagerNotify::GetInstance().UnRegisterDeathRecipientCallback(testPkgName);
    // 8. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    // 8 check checkMap not null
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterDeathRecipientCallback_003
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set dmInitCallback not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. set testpkcName com.ohos.test1
 *           7. call DeviceManagerNotifyTest UnRegisterDeathRecipientCallback with testpkcName
 *           8. Get checkMap from DeviceManagerNotify
 *           9. call checkMap OnRemoteDied
 *           10. check count is 1
 * deviceTypeId
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDeathRecipientCallback_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DmInitCallback> dmInitCallback = std::make_shared<DmInitCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DmInitCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 7. call DeviceManagerNotifyTest UnRegisterDeathRecipientCallback with testpkcName
    DeviceManagerNotify::GetInstance().UnRegisterDeathRecipientCallback(testPkgName);
    // 8. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    // 9. call checkMap OnRemoteDied
    checkMap->OnRemoteDied();
    // 10. check count is 1
    ASSERT_EQ(count, 1);
}

/**
 * @tc.name: UnRegisterDeathRecipientCallback_004
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set dmInitCallback not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. set testpkcName com.ohos.test1
 *           7. call DeviceManagerNotifyTest UnRegisterDeathRecipientCallback with testpkcName
 *           8. Get checkMap from DeviceManagerNotify
 *           9. check checkMap not null
 * deviceTypeId
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDeathRecipientCallback_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DmInitCallback> dmInitCallback = std::make_shared<DmInitCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DmInitCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 7. call DeviceManagerNotifyTest UnRegisterDeathRecipientCallback with testpkcName
    DeviceManagerNotify::GetInstance().UnRegisterDeathRecipientCallback(testPkgName);
    // 8. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    // 9. check checkMap not null
    if (checkMap == nullptr) {
        ASSERT_NE(count, 1);
        return;
    }
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterDeathRecipientCallback_005
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set dmInitCallback not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. set testpkcName com.ohos.test1
 *           7. call DeviceManagerNotifyTest UnRegisterDeathRecipientCallback with testpkcName
 *           8. Get checkMap from DeviceManagerNotify
 *           9. check checkMap not null
 * deviceTypeId
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDeathRecipientCallback_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DmInitCallback> dmInitCallback = std::make_shared<DmInitCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DmInitCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName nullptr
    std::string testPkgName = "";
    // 7. call DeviceManagerNotifyTest UnRegisterDeathRecipientCallback with testpkcName
    DeviceManagerNotify::GetInstance().UnRegisterDeathRecipientCallback(testPkgName);
    // 8. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    // 8 check checkMap not null
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: RegisterDeviceStateCallback_001
 * @tc.desc: 1. set pkgName not null
 *              set Callback not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 * deviceTypeId
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDeviceStateCallback_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: RegisterDeviceStateCallback_002
 * @tc.desc: 1. set pkgName not null
 *                     set Callback null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDeviceStateCallback_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback null
    std::shared_ptr<DeviceStateCallback> callback = nullptr;
    // 2. set checkMap not null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap null
    ASSERT_EQ(checkMap, nullptr);
}

/**
 * @tc.name: RegisterDeviceStateCallback_003
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set Callback not null
 *           2. set checkMap null
 *           3. set testpkcName com.ohos.test1
 *           4. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *           5. Get checkMap from DeviceManagerNotify with testpkcName
 *           6. check checkMap null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDeviceStateCallback_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName com.ohos.test
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    std::shared_ptr<DeviceStateCallback> callback = nullptr;
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 4. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 5. Get checkMap from DeviceManagerNotify with testpkcName
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[testPkgName];
    // 6. check checkMap not null
    ASSERT_EQ(checkMap, nullptr);
}

/**
 * @tc.name: RegisterDeviceStateCallback_004
 * @tc.desc: 1. set pkgName not null
 *              set Callback not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. call checkMap OnDeviceOnline
 *           7. check count is 1
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDeviceStateCallback_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call checkMap OnDeviceOnline
    DmDeviceInfo deviceInfo;
    checkMap->OnDeviceOnline(deviceInfo);
    // 7. check count is 1
    ASSERT_EQ(count, 1);
}

/**
 * @tc.name: RegisterDeviceStateCallback_005
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set Callback not null
 *           2. set checkMap null
 *           3. set testpkcName com.ohos.test1
 *           4. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *           5. Get checkMap from DeviceManagerNotify with testpkcName
 *           6. check checkMap null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDeviceStateCallback_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName com.ohos.test
    std::string pkgName = "com.ohos.test";
    int count = 0;
    // set dmInitCallback not null
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 4. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 5. Get checkMap from DeviceManagerNotify with testpkcName
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[testPkgName];
    // 6. check checkMap not null
    if (checkMap == nullptr) {
        ASSERT_NE(count, 1);
        return;
    }
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterDeviceStateCallback_001
 * @tc.desc: 1. set pkgName not null
 *              set Callback not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. call DeviceManagerNotifyTest UnRegisterDeviceStateCallback with parameter
 *           7. Get checkMap from DeviceManagerNotify
 *           8. check checkMap null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDeviceStateCallback_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotifyTest UnRegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().UnRegisterDeviceStateCallback(pkgName);
    // 7. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 8 check checkMap null
    ASSERT_EQ(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterDeviceStateCallback_002
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set Callback not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. set testpkcName com.ohos.test1
 *           7. call DeviceManagerNotifyTest UnRegisterDeviceStateCallback with testpkcName
 *           8. Get checkMap from DeviceManagerNotify
 *           9. check checkMap not null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDeviceStateCallback_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 7. call DeviceManagerNotifyTest UnRegisterDeviceStateCallback with testpkcName
    DeviceManagerNotify::GetInstance().UnRegisterDeviceStateCallback(testPkgName);
    // 8. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 8 check checkMap not null
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterDeviceStateCallback_003
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set Callback not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. set testpkcName com.ohos.test1
 *           7. call DeviceManagerNotifyTest UnRegisterDeviceStateCallback with testpkcName
 *           8. Get checkMap from DeviceManagerNotify
 *           9. check checkMap not null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDeviceStateCallback_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 7. call DeviceManagerNotifyTest UnRegisterDeviceStateCallback with testpkcName
    DeviceManagerNotify::GetInstance().UnRegisterDeviceStateCallback(testPkgName);
    // 8. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 8 check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 9. call checkMap OnDeviceOnline
    DmDeviceInfo deviceInfo;
    checkMap->OnDeviceOnline(deviceInfo);
    // 10. check count is 1
    ASSERT_EQ(count, 1);
}

/**
 * @tc.name: UnRegisterDeviceStateCallback_004
 * @tc.desc: 1. set pkgName not null
 *              set Callback not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. call DeviceManagerNotifyTest UnRegisterDeviceStateCallback with parameter
 *           7. Get checkMap from DeviceManagerNotify
 *           8. check checkMap null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDeviceStateCallback_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotifyTest UnRegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().UnRegisterDeviceStateCallback(pkgName);
    // 7. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 8 check checkMap null
    if (checkMap == nullptr) {
        ASSERT_NE(count, 1);
        return;
    }
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterDeviceStateCallback_005
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set Callback not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. set testpkcName com.ohos.test1
 *           7. call DeviceManagerNotifyTest UnRegisterDeviceStateCallback with testpkcName
 *           8. Get checkMap from DeviceManagerNotify
 *           9. check checkMap not null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDeviceStateCallback_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "";
    // 7. call DeviceManagerNotifyTest UnRegisterDeviceStateCallback with testpkcName
    DeviceManagerNotify::GetInstance().UnRegisterDeviceStateCallback(testPkgName);
    // 8. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 8 check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 9. call checkMap OnDeviceOnline
    DmDeviceInfo deviceInfo;
    checkMap->OnDeviceOnline(deviceInfo);
    // 10. check count is 1
    ASSERT_EQ(count, 1);
}

/**
 * @tc.name: RegisterDiscoverCallback_001
 * @tc.desc: 1. set pkgName not null
 *              set Callback not null
 *              set subscribeId not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDiscoveryCallback_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: RegisterDiscoverCallback_002
 * @tc.desc: 1. set pkgName not null
 *              set Callback null
 *              set subscribeId not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDiscoveryCallback_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback null
    std::shared_ptr<DiscoveryCallback> callback = nullptr;
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap not null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap null
    ASSERT_EQ(checkMap, nullptr);
}

/**
 * @tc.name: RegisterDiscoverCallback_003
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set Callback not null
 *              set subscribeId not null
 *           2. set checkMap null
 *           3. set testpkcName com.ohos.test1
 *           4. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *           5. Get checkMap from DeviceManagerNotify with testpkcName
 *           6. check checkMap null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDiscoveryCallback_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName com.ohos.test
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    std::shared_ptr<DiscoveryCallback> callback = nullptr;
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 4. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 5. Get checkMap from DeviceManagerNotify with testpkcName
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 6. check checkMap not null
    ASSERT_EQ(checkMap, nullptr);
}

/**
 * @tc.name: RegisterDiscoverCallback_004
 * @tc.desc: 1. set pkgName not null
 *              set Callback not null
 *              set subscribeId not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. call checkMap OnDiscoverySuccess
 *           7. check count is 1
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDiscoveryCallback_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call checkMap OnDiscoverySuccess
    checkMap->OnDiscoverySuccess(subscribeId);
    // 7. check count is 1
    ASSERT_EQ(count, 1);
}

/**
 * @tc.name: RegisterDiscoverCallback_005
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set Callback not null
 *              set subscribeId not null
 *           2. set checkMap null
 *           3. set testpkcName com.ohos.test1
 *           4. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *           5. Get checkMap from DeviceManagerNotify with testpkcName
 *           6. check checkMap null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDiscoveryCallback_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName com.ohos.test
    std::string pkgName = "com.ohos.test";
    int count = 0;
    // set dmInitCallback not null
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 4. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 5. Get checkMap from DeviceManagerNotify with testpkcName
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 6. check checkMap not null
    if (checkMap == nullptr) {
        ASSERT_NE(count, 1);
        return;
    }
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterDiscoverCallback_001
 * @tc.desc: 1. set pkgName not null
 *              set callback not null
 *              set subscribeId not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. call DeviceManagerNotifyTest UnRegisterDiscoverCallback with parameter
 *           7. Get checkMap from DeviceManagerNotify
 *           8. check checkMap null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDiscoveryCallback_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotifyTest UnRegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().UnRegisterDiscoveryCallback(pkgName, subscribeId);
    // 7. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 8 check checkMap null
    ASSERT_EQ(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterDiscoverCallback_002
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set dmInitCallback not null
 *              set subscribeId not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. set testpkcName com.ohos.test1
 *           7. call DeviceManagerNotifyTest UnRegisterDiscoverCallback with testpkcName
 *           8. Get checkMap from DeviceManagerNotify
 *           9. check checkMap not null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDiscoveryCallback_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 7. call DeviceManagerNotifyTest UnRegisterDiscoverCallback with testpkcName
    DeviceManagerNotify::GetInstance().UnRegisterDiscoveryCallback(testPkgName, subscribeId);
    // 8. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 8 check checkMap not null
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterDiscoverCallback_003
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set dmInitCallback not null
 *              set subscribeId not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. set testpkcName com.ohos.test1
 *           7. call DeviceManagerNotifyTest UnRegisterDiscoverCallback with testpkcName
 *           8. Get checkMap from DeviceManagerNotify
 *           9. check checkMap not null
 *          10. call checkMap OnDiscoverySuccess
 *          11. check count is 1
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDiscoveryCallback_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 7. call DeviceManagerNotifyTest UnRegisterDiscoverCallback with testpkcName
    DeviceManagerNotify::GetInstance().UnRegisterDiscoveryCallback(testPkgName, subscribeId);
    // 8. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 9 check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 10. call checkMap OnDiscoverySuccess
    checkMap->OnDiscoverySuccess(subscribeId);
    // 11. check count is 1
    ASSERT_EQ(count, 1);
}

/**
 * @tc.name: UnRegisterDiscoverCallback_004
 * @tc.desc: 1. set pkgName not null
 *              set callback not null
 *              set subscribeId not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. call DeviceManagerNotifyTest UnRegisterDiscoverCallback with parameter
 *           7. Get checkMap from DeviceManagerNotify
 *           8. check checkMap null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDiscoveryCallback_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotifyTest UnRegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().UnRegisterDiscoveryCallback(pkgName, subscribeId);
    // 7. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 8 check checkMap null
    if (checkMap == nullptr) {
        ASSERT_NE(count, 1);
        return;
    }
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterDiscoverCallback_005
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set dmInitCallback not null
 *              set subscribeId not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. set testpkcName com.ohos.test1
 *           7. call DeviceManagerNotifyTest UnRegisterDiscoverCallback with testpkcName
 *           8. Get checkMap from DeviceManagerNotify
 *           9. check checkMap not null
 *          10. call checkMap OnDiscoverySuccess
 *          11. check count is 1
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDiscoveryCallback_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "";
    // 7. call DeviceManagerNotifyTest UnRegisterDiscoverCallback with testpkcName
    DeviceManagerNotify::GetInstance().UnRegisterDiscoveryCallback(testPkgName, subscribeId);
    // 8. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 9 check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 10. call checkMap OnDiscoverySuccess
    checkMap->OnDiscoverySuccess(subscribeId);
    // 11. check count is 1
    ASSERT_EQ(count, 1);
}

/**
 * @tc.name: RegisterAuthenticateCallback_001
 * @tc.desc: 1. set pkgName not null
 *              set Callback not null
 *              set deviceId not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterAuthenticateCallback_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<AuthenticateCallback> callback = std::make_shared<AuthenticateCallbackTest>(count);
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: RegisterAuthenticateCallback_002
 * @tc.desc: 1. set pkgName not null
 *              set Callback null
 *              set deviceId not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterAuthenticateCallback_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback null
    std::shared_ptr<AuthenticateCallback> callback = nullptr;
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap not null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 5. check checkMap null
    ASSERT_EQ(checkMap, nullptr);
}

/**
 * @tc.name: RegisterAuthenticateCallback_003
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set Callback not null
 *              set deviceId not null
 *           2. set checkMap null
 *           3. set testpkcName com.ohos.test1
 *           4. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *           5. Get checkMap from DeviceManagerNotify with testpkcName
 *           6. check checkMap null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterAuthenticateCallback_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName com.ohos.test
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    std::shared_ptr<AuthenticateCallback> callback = nullptr;
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 4. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 5. Get checkMap from DeviceManagerNotify with testpkcName
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 6. check checkMap not null
    ASSERT_EQ(checkMap, nullptr);
}

/**
 * @tc.name: RegisterAuthenticateCallback_004
 * @tc.desc: 1. set pkgName not null
 *              set Callback not null
 *              set deviceId not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterAuthenticateCallback_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<AuthenticateCallback> callback = std::make_shared<AuthenticateCallbackTest>(count);
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call checkMap OnAuthResult
    std::string token ="1";
    int32_t status = 1;
    int32_t reason = 1;
    checkMap->OnAuthResult(deviceId, token, status, reason);
    // 7. check count is 1
    ASSERT_EQ(count, 1);
}

/**
 * @tc.name: RegisterAuthenticateCallback_005
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set Callback not null
 *              set deviceId not null
 *           2. set checkMap null
 *           3. set testpkcName com.ohos.test1
 *           4. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *           5. Get checkMap from DeviceManagerNotify with testpkcName
 *           6. check checkMap null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterAuthenticateCallback_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName com.ohos.test
    std::string pkgName = "com.ohos.test";
    int count = 0;
    // set dmInitCallback not null
    std::shared_ptr<AuthenticateCallback> callback = std::make_shared<AuthenticateCallbackTest>(count);
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 4. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 5. Get checkMap from DeviceManagerNotify with testpkcName
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 6. check checkMap not null
    if (checkMap == nullptr) {
        ASSERT_NE(count, 1);
        return;
    }
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterAuthenticateCallback_001
 * @tc.desc: 1. set pkgName not null
 *              set callback not null
 *              set deviceId not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. call DeviceManagerNotifyTest UnRegisterAuthenticateCallback with parameter
 *           7. Get checkMap from DeviceManagerNotify
 *           8. check checkMap null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterAuthenticateCallback_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<AuthenticateCallback> callback = std::make_shared<AuthenticateCallbackTest>(count);
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotifyTest UnRegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().UnRegisterAuthenticateCallback(pkgName, deviceId);
    // 7. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 8 check checkMap null
    ASSERT_EQ(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterAuthenticateCallback_002
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set dmInitCallback not null
 *              set subscribeId not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. set testpkcName com.ohos.test1
 *           7. call DeviceManagerNotifyTest UnRegisterAuthenticateCallback with testpkcName
 *           8. Get checkMap from DeviceManagerNotify
 *           9. check checkMap not null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterAuthenticateCallback_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<AuthenticateCallback> callback = std::make_shared<AuthenticateCallbackTest>(count);
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 7. call DeviceManagerNotifyTest UnRegisterAuthenticateCallback with testpkcName
    DeviceManagerNotify::GetInstance().UnRegisterAuthenticateCallback(testPkgName, deviceId);
    // 8. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 8 check checkMap not null
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterAuthenticateCallback_003
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set dmInitCallback not null
 *              set subscribeId not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. set testpkcName com.ohos.test1
 *           7. call DeviceManagerNotifyTest UnRegisterAuthenticateCallback with testpkcName
 *           8. Get checkMap from DeviceManagerNotify
 *           9. check checkMap not null
 *          10. call checkMap OnAuthResult
 *          11. check count is 1
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterAuthenticateCallback_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<AuthenticateCallback> callback = std::make_shared<AuthenticateCallbackTest>(count);
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 7. call DeviceManagerNotifyTest UnRegisterAuthenticateCallback with testpkcName
    DeviceManagerNotify::GetInstance().UnRegisterAuthenticateCallback(testPkgName, deviceId);
    // 8. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 9 check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 10. call checkMap OnAuthResult
    std::string token ="1";
    int32_t status = 1;
    int32_t reason = 1;
    checkMap->OnAuthResult(deviceId, token, status, reason);
    // 11. check count is 1
    ASSERT_EQ(count, 1);
}

/**
 * @tc.name: UnRegisterAuthenticateCallback_004
 * @tc.desc: 1. set pkgName not null
 *              set callback not null
 *              set deviceId not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. call DeviceManagerNotifyTest UnRegisterAuthenticateCallback with parameter
 *           7. Get checkMap from DeviceManagerNotify
 *           8. check checkMap null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterAuthenticateCallback_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<AuthenticateCallback> callback = std::make_shared<AuthenticateCallbackTest>(count);
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotifyTest UnRegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().UnRegisterAuthenticateCallback(pkgName, deviceId);
    // 7. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 8. check checkMap not null
    if (checkMap == nullptr) {
        ASSERT_NE(count, 1);
        return;
    }
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterAuthenticateCallback_005
 * @tc.desc: 1. set pkgName com.ohos.test
 *              set dmInitCallback not null
 *              set subscribeId not null
 *           2. set checkMap null
 *           3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *           4. Get checkMap from DeviceManagerNotify
 *           5. check checkMap not null
 *           6. set testpkcName com.ohos.test1
 *           7. call DeviceManagerNotifyTest UnRegisterAuthenticateCallback with testpkcName
 *           8. Get checkMap from DeviceManagerNotify
 *           9. check checkMap not null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterAuthenticateCallback_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<AuthenticateCallback> callback = std::make_shared<AuthenticateCallbackTest>(count);
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "";
    // 7. call DeviceManagerNotifyTest UnRegisterAuthenticateCallback with testpkcName
    DeviceManagerNotify::GetInstance().UnRegisterAuthenticateCallback(testPkgName, deviceId);
    // 8. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 8 check checkMap not null
    ASSERT_NE(checkMap, nullptr);
}

/**
 * @tc.name: UnRegisterPackageCallback_001
 * @tc.desc: 1. RegisterDeathRecipientCallback with pkgName and dmInitCallback
 *           2. RegisterDeviceStateCallback with pkgName and deviceStateCallback
 *           3. RegisterDiscoverCallback with pkgName and deviceStateCallback and subscribeId
 *           4. RegisterAuthenticateCallback with pkgName and deviceId and authenticateCallback
 *           5. call DeviceManagerNotify UnRegisterPackageCallback with PkgName
 *           6. check if checkMap checkMap1 checkMap2 checkMap3 is null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterPackageCallback_001, testing::ext::TestSize.Level0)
{
    // 1. RegisterDeathRecipientCallback with pkgName and dmInitCallback
    std::string pkgName = "com.ohos.test";
    int count = 0;
    std::shared_ptr<DmInitCallback> dmInitCallback = std::make_shared<DmInitCallbackTest>(count);
    std::shared_ptr<DmInitCallback> checkMap = nullptr;
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    ASSERT_NE(checkMap, nullptr);
    // 2. RegisterDeviceStateCallback with pkgName and deviceStateCallback
    int count1 = 0;
    std::shared_ptr<DeviceStateCallback> deviceStateCallback = std::make_shared<DeviceStateCallbackTest>(count1);
    std::shared_ptr<DeviceStateCallback> checkMap1 = nullptr;
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, deviceStateCallback);
    checkMap1 = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    ASSERT_NE(checkMap1, nullptr);
    // 3. RegisterDiscoverCallback with pkgName and deviceStateCallback and subscribeId
    int count2 = 0;
    std::shared_ptr<DiscoveryCallback> deviceDiscoveryCallback = std::make_shared<DiscoveryCallbackTest>(count2);
    uint16_t subscribeId = 0;
    std::shared_ptr<DiscoveryCallback> checkMap2 = nullptr;
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, deviceDiscoveryCallback);
    checkMap2 = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    ASSERT_NE(checkMap2, nullptr);
    // 4. RegisterAuthenticateCallback with pkgName and deviceId and authenticateCallback
    int count3 = 0;
    std::shared_ptr<AuthenticateCallback> authenticateCallback = std::make_shared<AuthenticateCallbackTest>(count3);
    std::string deviceId = "1";
    std::shared_ptr<AuthenticateCallback> checkMap3 = nullptr;
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, authenticateCallback);
    checkMap3 = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    ASSERT_NE(checkMap3, nullptr);
    // 5. call DeviceManagerNotify UnRegisterPackageCallback with PkgName
    DeviceManagerNotify::GetInstance().UnRegisterPackageCallback(pkgName);
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    checkMap1 = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    checkMap2 = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    checkMap3 = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 6. check if checkMap checkMap1 checkMap2 checkMap3 is null
    ASSERT_EQ(checkMap, nullptr);
    ASSERT_EQ(checkMap1, nullptr);
    ASSERT_EQ(checkMap2, nullptr);
    ASSERT_EQ(checkMap3, nullptr);
}

/**
 * @tc.name: UnRegisterPackageCallback_002
 * @tc.desc: 1. RegisterDeathRecipientCallback with pkgName and dmInitCallback
 *           2. RegisterDeviceStateCallback with pkgName and deviceStateCallback
 *           3. RegisterDiscoverCallback with pkgName and deviceStateCallback and subscribeId
 *           4. RegisterAuthenticateCallback with pkgName and deviceId and authenticateCallback
 *           5. call DeviceManagerNotify UnRegisterPackageCallback with testPkgName
 *           6. check if checkMap checkMap1 checkMap2 checkMap3 is not null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterPackageCallback_002, testing::ext::TestSize.Level0)
{
    // 1. RegisterDeathRecipientCallback with pkgName and dmInitCallback
    std::string pkgName = "com.ohos.test";
    int count = 0;
    std::shared_ptr<DmInitCallback> dmInitCallback = std::make_shared<DmInitCallbackTest>(count);
    std::shared_ptr<DmInitCallback> checkMap = nullptr;
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    ASSERT_NE(checkMap, nullptr);
    // 2. RegisterDeviceStateCallback with pkgName and deviceStateCallback
    int count1 = 0;
    std::shared_ptr<DeviceStateCallback> deviceStateCallback = std::make_shared<DeviceStateCallbackTest>(count1);
    std::shared_ptr<DeviceStateCallback> checkMap1 = nullptr;
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, deviceStateCallback);
    checkMap1 = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    ASSERT_NE(checkMap1, nullptr);
    // 3. RegisterDiscoverCallback with pkgName and deviceStateCallback and subscribeId
    int count2 = 0;
    std::shared_ptr<DiscoveryCallback> deviceDiscoveryCallback = std::make_shared<DiscoveryCallbackTest>(count2);
    uint16_t subscribeId = 0;
    std::shared_ptr<DiscoveryCallback> checkMap2 = nullptr;
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, deviceDiscoveryCallback);
    checkMap2 = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    ASSERT_NE(checkMap2, nullptr);
    // 4. RegisterAuthenticateCallback with pkgName and deviceId and authenticateCallback
    int count3 = 0;
    std::shared_ptr<AuthenticateCallback> authenticateCallback = std::make_shared<AuthenticateCallbackTest>(count3);
    std::string deviceId = "1";
    std::shared_ptr<AuthenticateCallback> checkMap3 = nullptr;
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, authenticateCallback);
    checkMap3 = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    ASSERT_NE(checkMap3, nullptr);
    // 5. call DeviceManagerNotify UnRegisterPackageCallback with testPkgName
    std::string testPkgName = "com.ohos.test1";
    DeviceManagerNotify::GetInstance().UnRegisterPackageCallback(testPkgName);
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    checkMap1 = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    checkMap2 = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    checkMap3 = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 6. check if checkMap checkMap1 checkMap2 checkMap3 is null
    ASSERT_NE(checkMap, nullptr);
    ASSERT_NE(checkMap1, nullptr);
    ASSERT_NE(checkMap2, nullptr);
    ASSERT_NE(checkMap3, nullptr);
}

/**
 * @tc.name: UnRegisterPackageCallback_003
 * @tc.desc: 1. RegisterDeathRecipientCallback with pkgName and dmInitCallback
 *           2. RegisterDeviceStateCallback with pkgName and deviceStateCallback
 *           3. RegisterDiscoverCallback with pkgName and deviceStateCallback and subscribeId
 *           4. RegisterAuthenticateCallback with pkgName and deviceId and authenticateCallback
 *           5. call DeviceManagerNotify UnRegisterPackageCallback with testPkgName
 *           6. check if checkMap checkMap1 checkMap2 checkMap3 is not null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterPackageCallback_003, testing::ext::TestSize.Level0)
{
    // 1. RegisterDeathRecipientCallback with pkgName and dmInitCallback
    std::string pkgName = "com.ohos.test";
    int count[4] = {0, 0, 0, 0};
    std::shared_ptr<DmInitCallback> dmInitCallback = std::make_shared<DmInitCallbackTest>(count[0]);
    std::shared_ptr<DmInitCallback> checkMap = nullptr;
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    ASSERT_NE(checkMap, nullptr);
    // 2. RegisterDeviceStateCallback with pkgName and deviceStateCallback
    std::shared_ptr<DeviceStateCallback> deviceStateCallback = std::make_shared<DeviceStateCallbackTest>(count[1]);
    std::shared_ptr<DeviceStateCallback> checkMap1 = nullptr;
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, deviceStateCallback);
    checkMap1 = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    ASSERT_NE(checkMap1, nullptr);
    // 3. RegisterDiscoverCallback with pkgName and deviceStateCallback and subscribeId
    std::shared_ptr<DiscoveryCallback> deviceDiscoveryCallback = std::make_shared<DiscoveryCallbackTest>(count[2]);
    uint16_t subscribeId = 0;
    std::shared_ptr<DiscoveryCallback> checkMap2 = nullptr;
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, deviceDiscoveryCallback);
    checkMap2 = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    ASSERT_NE(checkMap2, nullptr);
    // 4. RegisterAuthenticateCallback with pkgName and deviceId and authenticateCallback
    std::shared_ptr<AuthenticateCallback> authenticateCallback = std::make_shared<AuthenticateCallbackTest>(count[3]);
    std::string deviceId = "1";
    std::shared_ptr<AuthenticateCallback> checkMap3 = nullptr;
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, authenticateCallback);
    checkMap3 = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    ASSERT_NE(checkMap3, nullptr);
    // 5. call DeviceManagerNotify UnRegisterPackageCallback with testPkgName
    std::string testPkgName = "com.ohos.test1";
    DeviceManagerNotify::GetInstance().UnRegisterPackageCallback(testPkgName);
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    checkMap1 = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    checkMap2 = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    checkMap3 = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 6. check if checkMap checkMap1 checkMap2 checkMap3 is null
    ASSERT_NE(checkMap, nullptr);
    ASSERT_NE(checkMap1, nullptr);
    ASSERT_NE(checkMap2, nullptr);
    ASSERT_NE(checkMap3, nullptr);
    // 7. call checkMap OnRemoteDied()
    checkMap->OnRemoteDied();
    // call checkMap1 OnDeviceOnline
    DmDeviceInfo deviceInfo;
    checkMap1->OnDeviceOnline(deviceInfo);
    // call checkMap2 OnDeviceFound
    checkMap2->OnDeviceFound(subscribeId, deviceInfo);
    // call checkMap3 OnAuthResult
    std::string token ="1";;
    int32_t status = 1;
    int32_t reason = 1;
    checkMap3->OnAuthResult(deviceId, token, status, reason);
    // 8. check count count1 count2 count3 is 1
    ASSERT_EQ(count[0], 1);
    ASSERT_EQ(count[1], 1);
    ASSERT_EQ(count[2], 1);
    ASSERT_EQ(count[3], 1);
}

/**
 * @tc.name: UnRegisterPackageCallback_004
 * @tc.desc: 1. RegisterDeathRecipientCallback with pkgName and dmInitCallback
 *           2. RegisterDeviceStateCallback with pkgName and deviceStateCallback
 *           3. RegisterDiscoverCallback with pkgName and deviceStateCallback and subscribeId
 *           4. RegisterAuthenticateCallback with pkgName and deviceId and authenticateCallback
 *           5. call DeviceManagerNotify UnRegisterPackageCallback with PkgName
 *           6. check if checkMap checkMap1 checkMap2 checkMap3 is null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterPackageCallback_004, testing::ext::TestSize.Level0)
{
    // 1. RegisterDeathRecipientCallback with pkgName and dmInitCallback
    std::string pkgName = "com.ohos.test";
    int count = 0;
    std::shared_ptr<DmInitCallback> dmInitCallback = std::make_shared<DmInitCallbackTest>(count);
    std::shared_ptr<DmInitCallback> checkMap = nullptr;
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    ASSERT_NE(checkMap, nullptr);
    // 2. RegisterDeviceStateCallback with pkgName and deviceStateCallback
    int count1 = 0;
    std::shared_ptr<DeviceStateCallback> deviceStateCallback = std::make_shared<DeviceStateCallbackTest>(count1);
    std::shared_ptr<DeviceStateCallback> checkMap1 = nullptr;
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, deviceStateCallback);
    checkMap1 = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    ASSERT_NE(checkMap1, nullptr);
    // 3. RegisterDiscoverCallback with pkgName and deviceStateCallback and subscribeId
    int count2 = 0;
    std::shared_ptr<DiscoveryCallback> deviceDiscoveryCallback = std::make_shared<DiscoveryCallbackTest>(count2);
    uint16_t subscribeId = 0;
    std::shared_ptr<DiscoveryCallback> checkMap2 = nullptr;
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, deviceDiscoveryCallback);
    checkMap2 = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    ASSERT_NE(checkMap2, nullptr);
    // 4. RegisterAuthenticateCallback with pkgName and deviceId and authenticateCallback
    int count3 = 0;
    std::shared_ptr<AuthenticateCallback> authenticateCallback = std::make_shared<AuthenticateCallbackTest>(count3);
    std::string deviceId = "1";
    std::shared_ptr<AuthenticateCallback> checkMap3 = nullptr;
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, authenticateCallback);
    checkMap3 = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    ASSERT_NE(checkMap3, nullptr);
    // 5. call DeviceManagerNotify UnRegisterPackageCallback with PkgName
    DeviceManagerNotify::GetInstance().UnRegisterPackageCallback(pkgName);
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    checkMap1 = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    checkMap2 = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    checkMap3 = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 6. check if checkMap checkMap1 checkMap2 checkMap3 is null
    if (checkMap == nullptr && checkMap1 == nullptr && checkMap2 == nullptr && checkMap3 == nullptr) {
        ASSERT_NE(count, 1);
        ASSERT_NE(count1, 1);
        ASSERT_NE(count2, 1);
        ASSERT_NE(count3, 1);
        return;
    }
    ASSERT_EQ(checkMap, nullptr);
    ASSERT_EQ(checkMap1, nullptr);
    ASSERT_EQ(checkMap2, nullptr);
    ASSERT_EQ(checkMap3, nullptr);
}

/**
 * @tc.name: UnRegisterPackageCallback_005
 * @tc.desc: 1. RegisterDeathRecipientCallback with pkgName and dmInitCallback
 *                  2. RegisterDeviceStateCallback with pkgName and deviceStateCallback
 *                  3. RegisterDiscoverCallback with pkgName and deviceStateCallback and subscribeId
 *                  4. RegisterAuthenticateCallback with pkgName and deviceId and authenticateCallback
 *                  5. call DeviceManagerNotify UnRegisterPackageCallback with testPkgName
 *                  6. check if checkMap checkMap1 checkMap2 checkMap3 is not null
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterPackageCallback_005, testing::ext::TestSize.Level0)
{
    // 1. RegisterDeathRecipientCallback with pkgName and dmInitCallback
    std::string pkgName = "com.ohos.test";
    int count = 0;
    std::shared_ptr<DmInitCallback> dmInitCallback = std::make_shared<DmInitCallbackTest>(count);
    std::shared_ptr<DmInitCallback> checkMap = nullptr;
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    ASSERT_NE(checkMap, nullptr);
    // 2. RegisterDeviceStateCallback with pkgName and deviceStateCallback
    int count1 = 0;
    std::shared_ptr<DeviceStateCallback> deviceStateCallback = std::make_shared<DeviceStateCallbackTest>(count1);
    std::shared_ptr<DeviceStateCallback> checkMap1 = nullptr;
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, deviceStateCallback);
    checkMap1 = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    ASSERT_NE(checkMap1, nullptr);
    // 3. RegisterDiscoverCallback with pkgName and deviceStateCallback and subscribeId
    int count2 = 0;
    std::shared_ptr<DiscoveryCallback> deviceDiscoveryCallback = std::make_shared<DiscoveryCallbackTest>(count2);
    uint16_t subscribeId = 0;
    std::shared_ptr<DiscoveryCallback> checkMap2 = nullptr;
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, deviceDiscoveryCallback);
    checkMap2 = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    ASSERT_NE(checkMap2, nullptr);
    // 4. RegisterAuthenticateCallback with pkgName and deviceId and authenticateCallback
    int count3 = 0;
    std::shared_ptr<AuthenticateCallback> authenticateCallback = std::make_shared<AuthenticateCallbackTest>(count3);
    std::string deviceId = "1";
    std::shared_ptr<AuthenticateCallback> checkMap3 = nullptr;
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, authenticateCallback);
    checkMap3 = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    ASSERT_NE(checkMap3, nullptr);
    // 5. call DeviceManagerNotify UnRegisterPackageCallback with testPkgName
    std::string testPkgName = "";
    DeviceManagerNotify::GetInstance().UnRegisterPackageCallback(testPkgName);
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    checkMap1 = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    checkMap2 = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    checkMap3 = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 6. check if checkMap checkMap1 checkMap2 checkMap3 is null
    ASSERT_NE(checkMap, nullptr);
    ASSERT_NE(checkMap1, nullptr);
    ASSERT_NE(checkMap2, nullptr);
    ASSERT_NE(checkMap3, nullptr);
}

/*
 * Feature: DeviceManagerNotifyTest RegisterDeviceManagerFaCallback
 * Function: DeviceManagerNotifyTest
 * SubFunction: RegisterDeviceManagerFaCallback
 * FunctionPoints: DeviceManagerNotifyTest RegisterDeviceManagerFaCallback
 * EnvConditions: RegisterDeviceManagerFaCallback success.
 * CaseDescription: 1. set pkgName not null
 *                     set DmFaCallback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDeviceManagerFaCallback_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set DmFaCallback not null
    int count = 0;
    std::shared_ptr<DeviceManagerFaCallback> DmFaCallback = std::make_shared<DeviceManagerFaCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceManagerFaCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceManagerFaCallback(pkgName, DmFaCallback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
}

/*
 * Feature: DeviceManagerNotifyTest RegisterDeviceManagerFaCallback
 * Function: DeviceManagerNotifyTest
 * SubFunction: RegisterDeviceManagerFaCallback
 * FunctionPoints: DeviceManagerNotifyTest RegisterDeviceManagerFaCallback
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set DmFaCallback null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap null
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDeviceManagerFaCallback_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback null
    std::shared_ptr<DeviceManagerFaCallback> DmFaCallback = nullptr;
    // 2. set checkMap not null
    std::shared_ptr<DeviceManagerFaCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceManagerFaCallback(pkgName, DmFaCallback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 5. check checkMap null
    ASSERT_EQ(checkMap, nullptr);
}

/*
 * Feature: DeviceManagerNotifyTest RegisterDeviceManagerFaCallback
 * Function: DeviceManagerNotifyTest
 * SubFunction: RegisterDeviceManagerFaCallback
 * FunctionPoints: DeviceManagerNotifyTest RegisterDeviceManagerFaCallback
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName com.ohos.test
 *                     set DmFaCallback not null
 *                  2. set checkMap null
 *                  3. set testpkcName com.ohos.test1
 *                  4. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
 *                  5. Get checkMap from DeviceManagerNotify with testpkcName
 *                  6. check checkMap null
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDeviceManagerFaCallback_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName com.ohos.test
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    std::shared_ptr<DeviceManagerFaCallback> DmFaCallback = nullptr;
    // 2. set checkMap null
    std::shared_ptr<DeviceManagerFaCallback> checkMap = nullptr;
    // 3. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 4. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceManagerFaCallback(pkgName, DmFaCallback);
    // 5. Get checkMap from DeviceManagerNotify with testpkcName
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[testPkgName];
    // 6. check checkMap not null
    ASSERT_EQ(checkMap, nullptr);
}

/*
 * Feature: DeviceManagerNotifyTest RegisterDeviceManagerFaCallback
 * Function: DeviceManagerNotifyTest
 * SubFunction: RegisterDeviceManagerFaCallback
 * FunctionPoints: DeviceManagerNotifyTest RegisterDeviceManagerFaCallback
 * EnvConditions: RegisterDeviceManagerFaCallback success.
 * CaseDescription: 1. set pkgName not null
 *                     set DmFaCallback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDeviceManagerFaCallback_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set DmFaCallback not null
    int count = 0;
    std::shared_ptr<DeviceManagerFaCallback> DmFaCallback = std::make_shared<DeviceManagerFaCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceManagerFaCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceManagerFaCallback(pkgName, DmFaCallback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call checkMap OnCall
    std::string paramJson = "test";
    checkMap->OnCall(paramJson);
    ASSERT_EQ(count, 1);
}

/*
 * Feature: DeviceManagerNotifyTest RegisterDeviceManagerFaCallback
 * Function: DeviceManagerNotifyTest
 * SubFunction: RegisterDeviceManagerFaCallback
 * FunctionPoints: DeviceManagerNotifyTest RegisterDeviceManagerFaCallback
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName com.ohos.test
 *                     set DmFaCallback not null
 *                  2. set checkMap null
 *                  3. set testpkcName com.ohos.test1
 *                  4. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
 *                  5. Get checkMap from DeviceManagerNotify with testpkcName
 *                  6. check checkMap null
 */
HWTEST_F(DeviceManagerNotifyTest, RegisterDeviceManagerFaCallback_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName com.ohos.test
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DeviceManagerFaCallback> DmFaCallback = std::make_shared<DeviceManagerFaCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceManagerFaCallback> checkMap = nullptr;
    // 3. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 4. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceManagerFaCallback(pkgName, DmFaCallback);
    // 5. Get checkMap from DeviceManagerNotify with testpkcName
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[testPkgName];
    // 6. check checkMap not null
    if (checkMap == nullptr) {
        ASSERT_NE(count, 1);
        return;
    }
    ASSERT_EQ(checkMap, nullptr);
}

/*
 * Feature: DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback
 * Function: DeviceManagerNotifyTest
 * SubFunction: UnRegisterDeviceManagerFaCallback
 * FunctionPoints: DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback
 * EnvConditions: UnRegisterDeviceManagerFaCallback success.
 * CaseDescription: 1. set pkgName not null
 *                     set DmFaCallback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback with parameter
 *                  7. Get checkMap from DeviceManagerNotify
 *                  8. check checkMap null
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDeviceManagerFaCallback_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DeviceManagerFaCallback> DmFaCallback = std::make_shared<DeviceManagerFaCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceManagerFaCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceManagerFaCallback(pkgName, DmFaCallback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().UnRegisterDeviceManagerFaCallback(pkgName);
    // 7. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 8 check checkMap null
    ASSERT_EQ(checkMap, nullptr);
}

/*
 * Feature: DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback
 * Function: DeviceManagerNotifyTest
 * SubFunction: UnRegisterDeviceManagerFaCallback
 * FunctionPoints: DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName com.ohos.test
 *                     set dmInitCallback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. set testpkcName com.ohos.test1
 *                  7. call DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback with testpkcName
 *                  8. Get checkMap from DeviceManagerNotify
 *                  9. check checkMap not null
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDeviceManagerFaCallback_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set DmFaCallback not null
    int count = 0;
    std::shared_ptr<DeviceManagerFaCallback> DmFaCallback = std::make_shared<DeviceManagerFaCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceManagerFaCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceManagerFaCallback(pkgName, DmFaCallback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 7. call DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback with testpkcName
    DeviceManagerNotify::GetInstance().UnRegisterDeviceManagerFaCallback(testPkgName);
    // 8. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 8 check checkMap not null
    ASSERT_NE(checkMap, nullptr);
}

/*
 * Feature: DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback_003
 * Function: DeviceManagerNotifyTest
 * SubFunction: UnRegisterDeviceManagerFaCallback
 * FunctionPoints: DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName com.ohos.test
 *                     set dmInitCallback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. set testpkcName com.ohos.test1
 *                  7. call DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback with testpkcName
 *                  8. Get checkMap from DeviceManagerNotify
 *                  9. check checkMap not null
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDeviceManagerFaCallback_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set DmFaCallback not null
    int count = 0;
    std::shared_ptr<DeviceManagerFaCallback> DmFaCallback = std::make_shared<DeviceManagerFaCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceManagerFaCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceManagerFaCallback(pkgName, DmFaCallback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 7. call DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback with testpkcName
    DeviceManagerNotify::GetInstance().UnRegisterDeviceManagerFaCallback(testPkgName);
    // 8. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 9. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 10. call checkMap OnCall
    std::string paramJson = "test";
    checkMap->OnCall(paramJson);
    ASSERT_EQ(count, 1);
}

/*
 * Feature: DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback_004
 * Function: DeviceManagerNotifyTest
 * SubFunction: UnRegisterDeviceManagerFaCallback
 * FunctionPoints: DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback
 * EnvConditions: UnRegisterDeviceManagerFaCallback success.
 * CaseDescription: 1. set pkgName not null
 *                     set DmFaCallback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback with parameter
 *                  7. Get checkMap from DeviceManagerNotify
 *                  8. check checkMap null
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDeviceManagerFaCallback_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DeviceManagerFaCallback> DmFaCallback = std::make_shared<DeviceManagerFaCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceManagerFaCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceManagerFaCallback(pkgName, DmFaCallback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().UnRegisterDeviceManagerFaCallback(pkgName);
    // 7. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 8 check checkMap null
    if (checkMap == nullptr) {
        ASSERT_NE(count, 1);
        return;
    }
    ASSERT_EQ(checkMap, nullptr);
}

/*
 * Feature: DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback_005
 * Function: DeviceManagerNotifyTest
 * SubFunction: UnRegisterDeviceManagerFaCallback
 * FunctionPoints: DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName com.ohos.test
 *                     set dmInitCallback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. set testpkcName com.ohos.test1
 *                  7. call DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback with testpkcName
 *                  8. Get checkMap from DeviceManagerNotify
 *                  9. check checkMap not null
 */
HWTEST_F(DeviceManagerNotifyTest, UnRegisterDeviceManagerFaCallback_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set DmFaCallback not null
    int count = 0;
    std::shared_ptr<DeviceManagerFaCallback> DmFaCallback = std::make_shared<DeviceManagerFaCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceManagerFaCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceManagerFaCallback(pkgName, DmFaCallback);
    // 4. Get checkMap from DeviceManagerNotify RegisterDeathRecipientCallback
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "";
    // 7. call DeviceManagerNotifyTest UnRegisterDeviceManagerFaCallback with testpkcName
    DeviceManagerNotify::GetInstance().UnRegisterDeviceManagerFaCallback(testPkgName);
    // 8. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 9. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 10. call checkMap OnCall
    std::string paramJson = "test";
    checkMap->OnCall(paramJson);
    ASSERT_EQ(count, 1);
}

/*
 * Feature: DeviceManagerNotifyTest OnRemoteDied
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnRemoteDied
 * FunctionPoints: DeviceManagerNotifyTest OnRemoteDied
 * EnvConditions: OnRemoteDied success.
 * CaseDescription: 1. set pkgName not null
 *                     set dmInitCallback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnRemoteDied
 *                  7. check if dmInitCallback OnRemoteDied called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnRemoteDied1, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set dmInitCallback not null
    int count = 0;
    std::shared_ptr<DmInitCallback> dmInitCallback = std::make_shared<DmInitCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DmInitCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeathRecipientCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmInitCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnRemoteDied
    DeviceManagerNotify::GetInstance().OnRemoteDied();
    // 7. check if dmInitCallback OnRemoteDied called
    ASSERT_EQ(count, 1);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceOnline
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceOnline
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceOnline
 * EnvConditions: OnDeviceOnline success.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDeviceOnline
 *                  7. check if callback OnDeviceOnline called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceOnline_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnDeviceOnline
    DmDeviceInfo deviceInfo;
    DeviceManagerNotify::GetInstance().OnDeviceOnline(pkgName, deviceInfo);
    // 7. check if callback OnDeviceOnline called
    ASSERT_EQ(count, 1);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceOnline
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceOnline
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceOnline
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. set testpkcName com.ohos.test1
 *                  7. call DeviceManagerNotify OnDeviceOnline with testpkcName
 *                  8. check if callback OnDeviceOnline called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceOnline_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 7. call DeviceManagerNotify OnDeviceOnline with testpkcName
    DmDeviceInfo deviceInfo;
    DeviceManagerNotify::GetInstance().OnDeviceOnline(testPkgName, deviceInfo);
    // 8. check if callback OnDeviceOnline called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceOnline
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceOnline
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceOnline
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. set testpkcName com.ohos.test1
 *                  7. call DeviceManagerNotify OnDeviceOnline with testpkcName
 *                  8. check if callback OnDeviceOnline called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceOnline_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "";
    // 7. call DeviceManagerNotify OnDeviceOnline with testpkcName
    DmDeviceInfo deviceInfo;
    DeviceManagerNotify::GetInstance().OnDeviceOnline(testPkgName, deviceInfo);
    // 8. check if callback OnDeviceOnline called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceOnline
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceOnline
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceOnline
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. set testpkcName com.ohos.test1
 *                  7. call DeviceManagerNotify OnDeviceOnline with testpkcName
 *                  8. check if callback OnDeviceOnline called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceOnline_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "";
    // 7. call DeviceManagerNotify OnDeviceOnline with testpkcName
    DmDeviceInfo deviceInfo;
    DeviceManagerNotify::GetInstance().OnDeviceOnline(testPkgName, deviceInfo);
    // 8. check if callback OnDeviceOnline called
    ASSERT_EQ(count, 1);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceOnline
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceOnline
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceOnline
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify UnRegisterDeviceStateCallback withtestPkgName
 *                  7. call DeviceManagerNotify OnDeviceOnline with testpkcName
 *                  8. check if callback OnDeviceOnline called
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceOnline_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    std::string testPkgName = "";
    // 6. call DeviceManagerNotify UnRegisterDeviceStateCallback withtestPkgName
    DeviceManagerNotify::GetInstance().UnRegisterDeviceStateCallback(testPkgName);
    // 7. call DeviceManagerNotify OnDeviceOnline with testpkcName
    DmDeviceInfo deviceInfo;
    DeviceManagerNotify::GetInstance().OnDeviceOnline(testPkgName, deviceInfo);
    // 8. check if callback OnDeviceOnline called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceOffline
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceOffline
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceOffline
 * EnvConditions: OnDeviceOffline success.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDeviceOffline
 *                  7. check if callback OnDeviceOffline called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceOffline_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnDeviceOffline
    DmDeviceInfo deviceInfo;
    DeviceManagerNotify::GetInstance().OnDeviceOffline(pkgName, deviceInfo);
    // 7. check if callback OnDeviceOffline called
    ASSERT_EQ(count, 1);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceOffline
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceOffline
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceOffline
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. set testpkcName com.ohos.test1
 *                  7. call DeviceManagerNotify OnDeviceOffline with testpkcName
 *                  8. check if callback OnDeviceOffline called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceOffline_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 7. call DeviceManagerNotify OnDeviceOffline with testpkcName
    DmDeviceInfo deviceInfo;
    DeviceManagerNotify::GetInstance().OnDeviceOffline(testPkgName, deviceInfo);
    // 8. check if callback OnDeviceOffline called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceOffline
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceOffline
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceOffline
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. set testpkcName com.ohos.test1
 *                  7. call DeviceManagerNotify OnDeviceOffline with testpkcName
 *                  8. check if callback OnDeviceOffline called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceOffline_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName null
    std::string testPkgName = "";
    // 7. call DeviceManagerNotify OnDeviceOffline with testpkcName
    DmDeviceInfo deviceInfo;
    DeviceManagerNotify::GetInstance().OnDeviceOffline(testPkgName, deviceInfo);
    // 8. check if callback OnDeviceOffline called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceOffline
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceOffline
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceOffline
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. set testpkcName com.ohos.test1
 *                  7. call DeviceManagerNotify OnDeviceOffline with testpkcName
 *                  8. check if callback OnDeviceOffline called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceOffline_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName null
    std::string testPkgName = "";
    // 7. call DeviceManagerNotify OnDeviceOffline with testpkcName
    DmDeviceInfo deviceInfo;
    DeviceManagerNotify::GetInstance().OnDeviceOffline(testPkgName, deviceInfo);
    // 8. check if callback OnDeviceOffline called
    ASSERT_EQ(count, 1);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceOffline
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceOffline
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceOffline
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify UnRegisterDeviceStateCallback withtestPkgName
 *                  7. call DeviceManagerNotify OnDeviceOffline with testpkcName
 *                  8. check if callback OnDeviceOffline called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceOffline_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify UnRegisterDeviceStateCallback withtestPkgName
    std::string testPkgName = "";
    DeviceManagerNotify::GetInstance().UnRegisterDeviceStateCallback(testPkgName);
    // 7. call DeviceManagerNotify OnDeviceOffline with testpkcName
    DmDeviceInfo deviceInfo;
    DeviceManagerNotify::GetInstance().OnDeviceOffline(testPkgName, deviceInfo);
    // 8. check if callback OnDeviceOffline called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceChanged
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceChanged
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceChanged
 * EnvConditions: OnDeviceChanged success.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDeviceChanged
 *                  7. check if callback OnDeviceChanged called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceChanged_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnDeviceChanged
    DmDeviceInfo deviceInfo;
    DeviceManagerNotify::GetInstance().OnDeviceChanged(pkgName, deviceInfo);
    // 7. check if callback OnDeviceChanged called
    ASSERT_EQ(count, 1);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceChanged
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceChanged
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceChanged
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. set testpkcName com.ohos.test1
 *                  7. call DeviceManagerNotify OnDeviceChanged with testpkcName
 *                  8. check if callback OnDeviceChanged called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceChanged_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "com.ohos.test1";
    // 7. call DeviceManagerNotify OnDeviceChanged with testpkcName
    DmDeviceInfo deviceInfo;
    DeviceManagerNotify::GetInstance().OnDeviceChanged(testPkgName, deviceInfo);
    // 8. check if callback OnDeviceChanged called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceChanged
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceChanged
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceChanged
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. set testpkcName com.ohos.test1
 *                  7. call DeviceManagerNotify OnDeviceChanged with testpkcName
 *                  8. check if callback OnDeviceChanged called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceChanged_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "";
    // 7. call DeviceManagerNotify OnDeviceChanged with testpkcName
    DmDeviceInfo deviceInfo;
    DeviceManagerNotify::GetInstance().OnDeviceChanged(testPkgName, deviceInfo);
    // 8. check if callback OnDeviceChanged called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceChanged
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceChanged
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceChanged
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. set testpkcName com.ohos.test1
 *                  7. call DeviceManagerNotify OnDeviceChanged with testpkcName
 *                  8. check if callback OnDeviceChanged called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceChanged_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. set testpkcName com.ohos.test1
    std::string testPkgName = "";
    // 7. call DeviceManagerNotify OnDeviceChanged with testpkcName
    DmDeviceInfo deviceInfo;
    DeviceManagerNotify::GetInstance().OnDeviceChanged(testPkgName, deviceInfo);
    // 8. check if callback OnDeviceChanged called
    ASSERT_EQ(count, 1);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceChanged
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceChanged
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceChanged
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. set testpkcName com.ohos.test1
 *                  7. call DeviceManagerNotify OnDeviceChanged with testpkcName
 *                  8. check if callback OnDeviceChanged called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceChanged_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set Callback not null
    int count = 0;
    std::shared_ptr<DeviceStateCallback> callback = std::make_shared<DeviceStateCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceStateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceStateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceStateCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify UnRegisterDeviceStateCallback withtestPkgName
    std::string testPkgName = "";
    DeviceManagerNotify::GetInstance().UnRegisterDeviceStateCallback(testPkgName);
    // 7. call DeviceManagerNotify OnDeviceChanged with testpkcName
    DmDeviceInfo deviceInfo;
    DeviceManagerNotify::GetInstance().OnDeviceChanged(testPkgName, deviceInfo);
    // 8. check if callback OnDeviceChanged called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceFound
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceFound
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceFound
 * EnvConditions: OnDeviceFound success.
 * CaseDescription: 1. set pkgName not null
 *                     set callback not null
 *                     set subscribeId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDeviceFound
 *                  7. check if callback OnDeviceFound called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceFound1, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    DmDeviceInfo deviceInfo;
    // 6. call DeviceManagerNotify OnDeviceFound
    DeviceManagerNotify::GetInstance().OnDeviceFound(pkgName, subscribeId, deviceInfo);
    // 7. check if callback OnDeviceFound called
    ASSERT_EQ(count, 1);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceFound
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceFound
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceFound
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set callback not null
 *                     set subscribeId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDeviceFound With testPkgName
 *                  7. check if callback OnDeviceFound called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceFound2, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    DmDeviceInfo deviceInfo;
    std::string testPkgName = "com.ohos.test1";
    // 6. call DeviceManagerNotify OnDeviceFound
    DeviceManagerNotify::GetInstance().OnDeviceFound(testPkgName, subscribeId, deviceInfo);
    // 7. check if callback OnDeviceFound called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceFound
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceFound
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceFound
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set callback not null
 *                     set subscribeId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDeviceFound With testsubscribeId
 *                  7. check if callback OnDeviceFound called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceFound3, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    DmDeviceInfo deviceInfo;
    uint16_t testsubscribeId = 1;
    // 6. call DeviceManagerNotify OnDeviceFound
    DeviceManagerNotify::GetInstance().OnDeviceFound(pkgName, testsubscribeId, deviceInfo);
    // 7. check if callback OnDeviceFound called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceFound
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceFound
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceFound
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set callback not null
 *                     set subscribeId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDeviceFound With testPkgName
 *                  7. check if callback OnDeviceFound called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceFound4, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    DmDeviceInfo deviceInfo;
    std::string testPkgName = "";
    // 6. call DeviceManagerNotify OnDeviceFound
    DeviceManagerNotify::GetInstance().OnDeviceFound(testPkgName, subscribeId, deviceInfo);
    // 7. check if callback OnDeviceFound called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDeviceFound
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDeviceFound
 * FunctionPoints: DeviceManagerNotifyTest OnDeviceFound
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set callback not null
 *                     set subscribeId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDeviceFound With testPkgName
 *                  7. check if callback OnDeviceFound called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDeviceFound5, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "";
    // set callback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    DmDeviceInfo deviceInfo;
    std::string testPkgName = "";
    DeviceManagerNotify::GetInstance().UnRegisterDiscoveryCallback(testPkgName, subscribeId);
    // 6. call DeviceManagerNotify OnDeviceFound
    DeviceManagerNotify::GetInstance().OnDeviceFound(testPkgName, subscribeId, deviceInfo);
    // 7. check if callback OnDeviceFound called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDiscoverFailed
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDiscoverFailed
 * FunctionPoints: DeviceManagerNotifyTest OnDiscoverFailed
 * EnvConditions: OnDiscoverFailed success.
 * CaseDescription: 1. set pkgName not null
 *                     set callback not null
 *                     set subscribeId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDiscoverFailed
 *                  7. check if callback OnDiscoverFailed called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDiscoverFailed1, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    int32_t failedReason = 0;
    // 6. call DeviceManagerNotify OnDiscoverFailed
    DeviceManagerNotify::GetInstance().OnDiscoveryFailed(pkgName, subscribeId, failedReason);
    // 7. check if callback OnDiscoverFailed called
    ASSERT_EQ(count, 1);
}

/*
 * Feature: DeviceManagerNotifyTest OnDiscoverFailed
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDiscoverFailed
 * FunctionPoints: DeviceManagerNotifyTest OnDiscoverFailed
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set callback not null
 *                     set subscribeId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDiscoverFailed With testPkgName
 *                  7. check if callback OnDiscoverFailed called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDiscoverFailed2, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    std::string testPkgName = "com.ohos.test1";
    int32_t failedReason = 0;
    // 6. call DeviceManagerNotify OnDiscoverFailed
    DeviceManagerNotify::GetInstance().OnDiscoveryFailed(testPkgName, subscribeId, failedReason);
    // 7. check if callback OnDiscoverFailed called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDiscoverFailed
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDiscoverFailed
 * FunctionPoints: DeviceManagerNotifyTest OnDiscoverFailed
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set callback not null
 *                     set subscribeId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDiscoverFailed With testsubscribeId
 *                  7. check if callback OnDiscoverFailed called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDiscoverFailed3, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    int32_t failedReason = 0;
    uint16_t testsubscribeId = 1;
    // 6. call DeviceManagerNotify OnDiscoverFailed
    DeviceManagerNotify::GetInstance().OnDiscoveryFailed(pkgName, testsubscribeId, failedReason);
    // 7. check if callback OnDiscoverFailed called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDiscoverFailed
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDiscoverFailed
 * FunctionPoints: DeviceManagerNotifyTest OnDiscoverFailed
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set callback not null
 *                     set subscribeId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDiscoverFailed With testPkgName
 *                  7. check if callback OnDiscoverFailed called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDiscoverFailed4, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    std::string testPkgName = "";
    int32_t failedReason = 0;
    // 6. call DeviceManagerNotify OnDiscoverFailed
    DeviceManagerNotify::GetInstance().OnDiscoveryFailed(testPkgName, subscribeId, failedReason);
    // 7. check if callback OnDiscoverFailed called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDiscoverFailed
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDiscoverFailed
 * FunctionPoints: DeviceManagerNotifyTest OnDiscoverFailed
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set callback not null
 *                     set subscribeId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDiscoverFailed With testPkgName
 *                  7. check if callback OnDiscoverFailed called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDiscoverFailed5, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "";
    // set callback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    std::string testPkgName = "";
    DeviceManagerNotify::GetInstance().UnRegisterDiscoveryCallback(testPkgName, subscribeId);
    int32_t failedReason = 0;
    // 6. call DeviceManagerNotify OnDiscoverFailed
    DeviceManagerNotify::GetInstance().OnDiscoveryFailed(testPkgName, subscribeId, failedReason);
    // 7. check if callback OnDiscoverFailed called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDiscoverySuccess
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDiscoverySuccess
 * FunctionPoints: DeviceManagerNotifyTest OnDiscoverySuccess
 * EnvConditions: OnDiscoverySuccess success.
 * CaseDescription: 1. set pkgName not null
 *                     set callback not null
 *                     set subscribeId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDiscoverySuccess
 *                  7. check if callback OnDiscoverySuccess called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDiscoverySuccess1, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnDiscoverySuccess
    DeviceManagerNotify::GetInstance().OnDiscoverySuccess(pkgName, subscribeId);
    // 7. check if callback OnDiscoverySuccess called
    ASSERT_EQ(count, 1);
}

/*
 * Feature: DeviceManagerNotifyTest OnDiscoverySuccess
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDiscoverySuccess
 * FunctionPoints: DeviceManagerNotifyTest OnDiscoverySuccess
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set callback not null
 *                     set subscribeId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDiscoverySuccess With testPkgName
 *                  7. check if callback OnDiscoverySuccess called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDiscoverySuccess2, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    std::string testPkgName = "com.ohos.test1";
    // 6. call DeviceManagerNotify OnDiscoverySuccess
    DeviceManagerNotify::GetInstance().OnDiscoverySuccess(testPkgName, subscribeId);
    // 7. check if callback OnDiscoverySuccess called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDiscoverySuccess
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDiscoverySuccess
 * FunctionPoints: DeviceManagerNotifyTest OnDiscoverySuccess
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set callback not null
 *                     set subscribeId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDiscoverySuccess With testsubscribeId
 *                  7. check if callback OnDiscoverySuccess called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDiscoverySuccess3, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    uint16_t testsubscribeId = 1;
    // 6. call DeviceManagerNotify OnDiscoverySuccess
    DeviceManagerNotify::GetInstance().OnDiscoverySuccess(pkgName, testsubscribeId);
    // 7. check if callback OnDiscoverySuccess called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDiscoverySuccess
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDiscoverySuccess
 * FunctionPoints: DeviceManagerNotifyTest OnDiscoverySuccess
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set callback not null
 *                     set subscribeId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDiscoverySuccess With testPkgName
 *                  7. check if callback OnDiscoverySuccess called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDiscoverySuccess4, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    std::string testPkgName = "";
    // 6. call DeviceManagerNotify OnDiscoverySuccess
    DeviceManagerNotify::GetInstance().OnDiscoverySuccess(testPkgName, subscribeId);
    // 7. check if callback OnDiscoverySuccess called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnDiscoverySuccess
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnDiscoverySuccess
 * FunctionPoints: DeviceManagerNotifyTest OnDiscoverySuccess
 * EnvConditions: N/A.
 * CaseDescription: 1. set pkgName not null
 *                     set callback not null
 *                     set subscribeId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnDiscoverySuccess With testPkgName
 *                  7. check if callback OnDiscoverySuccess called.
 */
HWTEST_F(DeviceManagerNotifyTest, OnDiscoverySuccess5, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "";
    // set callback not null
    int count = 0;
    std::shared_ptr<DiscoveryCallback> callback = std::make_shared<DiscoveryCallbackTest>(count);
    // set subscribeId not null
    uint16_t subscribeId = 0;
    // 2. set checkMap null
    std::shared_ptr<DiscoveryCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDiscoverCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().deviceDiscoveryCallbacks_[pkgName][subscribeId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    std::string testPkgName = "";
    DeviceManagerNotify::GetInstance().UnRegisterDiscoveryCallback(testPkgName, subscribeId);
    // 6. call DeviceManagerNotify OnDiscoverySuccess
    DeviceManagerNotify::GetInstance().OnDiscoverySuccess(testPkgName, subscribeId);
    // 7. check if callback OnDiscoverySuccess called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnAuthResult
 * Function: OnAuthResult
 * SubFunction: RegisterAuthenticateCallback
 * FunctionPoints: DeviceManagerNotifyTest OnAuthResult
 * EnvConditions: OnAuthResult success.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                     set deviceId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnAuthResult
 *                  7. check if callback OnDiscoverySuccess called
 */
HWTEST_F(DeviceManagerNotifyTest, OnAuthResult1, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<AuthenticateCallback> callback = std::make_shared<AuthenticateCallbackTest>(count);
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnAuthResult
    std::string token = "111";
    uint32_t status = 0;
    uint32_t reason = 0;
    DeviceManagerNotify::GetInstance().OnAuthResult(pkgName, deviceId, token, status, reason);
    // 7. check if callback OnDiscoverySuccess called
    ASSERT_EQ(count, 1);
}

/*
 * Feature: DeviceManagerNotifyTest OnAuthResult
 * Function: OnAuthResult
 * SubFunction: RegisterAuthenticateCallback
 * FunctionPoints: DeviceManagerNotifyTest OnAuthResult
 * EnvConditions: OnAuthResult success.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                     set deviceId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnAuthResult wiht testPkgName
 *                  7. check if callback OnDiscoverySuccess called
 */
HWTEST_F(DeviceManagerNotifyTest, OnAuthResult2, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<AuthenticateCallback> callback = std::make_shared<AuthenticateCallbackTest>(count);
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnAuthResult
    std::string testPkgName = "com.ohos.test1";
    std::string token = "111";
    uint32_t status = 0;
    uint32_t reason = 0;
    DeviceManagerNotify::GetInstance().OnAuthResult(testPkgName, deviceId, token, status, reason);
    // 7. check if callback OnDiscoverySuccess called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnAuthResult
 * Function: OnAuthResult
 * SubFunction: RegisterAuthenticateCallback
 * FunctionPoints: DeviceManagerNotifyTest OnAuthResult
 * EnvConditions: OnAuthResult success.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                     set deviceId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnAuthResult wiht testDeviceId
 *                  7. check if callback OnDiscoverySuccess called
 */
HWTEST_F(DeviceManagerNotifyTest, OnAuthResult3, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<AuthenticateCallback> callback = std::make_shared<AuthenticateCallbackTest>(count);
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnAuthResult
    std::string testDeviceId = "2";
    std::string token = "111";
    uint32_t status = 0;
    uint32_t reason = 0;
    DeviceManagerNotify::GetInstance().OnAuthResult(pkgName, testDeviceId, token, status, reason);
    // 7. check if callback OnDiscoverySuccess called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnAuthResult
 * Function: OnAuthResult
 * SubFunction: RegisterAuthenticateCallback
 * FunctionPoints: DeviceManagerNotifyTest OnAuthResult
 * EnvConditions: OnAuthResult success.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                     set deviceId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnAuthResult
 *                  7. check if callback OnDiscoverySuccess called
 */
HWTEST_F(DeviceManagerNotifyTest, OnAuthResult4, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<AuthenticateCallback> callback = std::make_shared<AuthenticateCallbackTest>(count);
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnAuthResult
    std::string token = "111";
    uint32_t status = 0;
    uint32_t reason = 0;
    DeviceManagerNotify::GetInstance().OnAuthResult(pkgName, deviceId, token, status, reason);
    // 7. check if callback OnDiscoverySuccess called
    ASSERT_EQ(count, 1);
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    ASSERT_EQ(checkMap, nullptr);
}

/*
 * Feature: DeviceManagerNotifyTest OnAuthResult
 * Function: OnAuthResult
 * SubFunction: RegisterAuthenticateCallback
 * FunctionPoints: DeviceManagerNotifyTest OnAuthResult
 * EnvConditions: OnAuthResult success.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                     set deviceId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnAuthResult
 *                  7. check if callback OnDiscoverySuccess called
 */
HWTEST_F(DeviceManagerNotifyTest, OnAuthResult5, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<AuthenticateCallback> callback = std::make_shared<AuthenticateCallbackTest>(count);
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnAuthResult
    std::string testPkaName = "com.ohos.test1";
    std::string token = "111";
    uint32_t status = 0;
    uint32_t reason = 0;
    DeviceManagerNotify::GetInstance().OnAuthResult(testPkaName, deviceId, token, status, reason);
    // 7. check if callback OnDiscoverySuccess called
    ASSERT_EQ(count, 0);
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    ASSERT_NE(checkMap, nullptr);
}

/*
 * Feature: DeviceManagerNotifyTest OnAuthResult
 * Function: OnAuthResult
 * SubFunction: RegisterAuthenticateCallback
 * FunctionPoints: DeviceManagerNotifyTest OnAuthResult
 * EnvConditions: OnAuthResult success.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                     set deviceId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnAuthResult
 *                  7. check if callback OnDiscoverySuccess called
 */
HWTEST_F(DeviceManagerNotifyTest, OnAuthResult6, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<AuthenticateCallback> callback = std::make_shared<AuthenticateCallbackTest>(count);
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnAuthResult
    std::string testDeviceId = "2";
    std::string token = "111";
    uint32_t status = 0;
    uint32_t reason = 0;
    DeviceManagerNotify::GetInstance().OnAuthResult(pkgName, testDeviceId, token, status, reason);
    // 7. check if callback OnDiscoverySuccess called
    ASSERT_EQ(count, 0);
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    ASSERT_NE(checkMap, nullptr);
}

/*
 * Feature: DeviceManagerNotifyTest OnAuthResult
 * Function: OnAuthResult
 * SubFunction: RegisterAuthenticateCallback
 * FunctionPoints: DeviceManagerNotifyTest OnAuthResult
 * EnvConditions: OnAuthResult success.
 * CaseDescription: 1. set pkgName not null
 *                     set Callback not null
 *                     set deviceId not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnAuthResult
 *                  7. check if callback OnDiscoverySuccess called
 */
HWTEST_F(DeviceManagerNotifyTest, OnAuthResult7, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set callback not null
    int count = 0;
    std::shared_ptr<AuthenticateCallback> callback = std::make_shared<AuthenticateCallbackTest>(count);
    // set subscribeId not null
    std::string deviceId = "1";
    // 2. set checkMap null
    std::shared_ptr<AuthenticateCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterAuthenticateCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, deviceId, callback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnAuthResult
    std::string token = "111";
    uint32_t status = 0;
    uint32_t reason = 0;
    DeviceManagerNotify::GetInstance().OnAuthResult(pkgName, deviceId, token, status, reason);
    // 7. check if callback OnDiscoverySuccess called
    ASSERT_EQ(count, 1);
    checkMap = DeviceManagerNotify::GetInstance().authenticateCallback_[pkgName][deviceId];
    ASSERT_EQ(checkMap, nullptr);
}

/*
 * Feature: DeviceManagerNotifyTest OnFaCall
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnFaCall
 * FunctionPoints: DeviceManagerNotifyTest OnFaCall
 * EnvConditions: OnFaCall success.
 * CaseDescription: 1. set pkgName not null
 *                     set DmFaCallback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnFaCall
 *                  7. check if callback OnCheckAuthResult called
 */
HWTEST_F(DeviceManagerNotifyTest, OnFaCall1, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set DmFaCallback not null
    int count = 0;
    std::shared_ptr<DeviceManagerFaCallback> DmFaCallback = std::make_shared<DeviceManagerFaCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceManagerFaCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceManagerFaCallback(pkgName, DmFaCallback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnFaCall
    std::string paramJson = "trstParamJson";
    DeviceManagerNotify::GetInstance().OnFaCall(pkgName, paramJson);
    // 7. check if callback OnCheckAuthResult called
    ASSERT_EQ(count, 1);
}

/*
 * Feature: DeviceManagerNotifyTest OnFaCall
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnFaCall
 * FunctionPoints: DeviceManagerNotifyTest OnFaCall
 * EnvConditions: OnFaCall success.
 * CaseDescription: 1. set pkgName not null
 *                     set DmFaCallback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnFaCall with testPkgName
 *                  7. check if callback OnCheckAuthResult called
 */
HWTEST_F(DeviceManagerNotifyTest, OnFaCall2, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set DmFaCallback not null
    int count = 0;
    std::shared_ptr<DeviceManagerFaCallback> DmFaCallback = std::make_shared<DeviceManagerFaCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceManagerFaCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceManagerFaCallback(pkgName, DmFaCallback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnFaCall
    std::string testPkgName = "com.ohos.test1";
    std::string paramJson = "trstParamJson";
    DeviceManagerNotify::GetInstance().OnFaCall(testPkgName, paramJson);
    // 7. check if callback OnCheckAuthResult called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnFaCall
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnFaCall
 * FunctionPoints: DeviceManagerNotifyTest OnFaCall
 * EnvConditions: OnFaCall success.
 * CaseDescription: 1. set pkgName not null
 *                     set DmFaCallback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnFaCall with testPkgName
 *                  7. check if callback OnCheckAuthResult called
 */
HWTEST_F(DeviceManagerNotifyTest, OnFaCall3, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // set DmFaCallback not null
    int count = 0;
    std::shared_ptr<DeviceManagerFaCallback> DmFaCallback = std::make_shared<DeviceManagerFaCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceManagerFaCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceManagerFaCallback(pkgName, DmFaCallback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnFaCall
    std::string testPkgName = "";
    std::string paramJson = "trstParamJson";
    DeviceManagerNotify::GetInstance().OnFaCall(testPkgName, paramJson);
    // 7. check if callback OnCheckAuthResult called
    ASSERT_EQ(count, 0);
}

/*
 * Feature: DeviceManagerNotifyTest OnFaCall
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnFaCall
 * FunctionPoints: DeviceManagerNotifyTest OnFaCall
 * EnvConditions: OnFaCall success.
 * CaseDescription: 1. set pkgName not null
 *                     set DmFaCallback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnFaCall with testPkgName
 *                  7. check if callback OnCheckAuthResult called
 */
HWTEST_F(DeviceManagerNotifyTest, OnFaCall4, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "";
    // set DmFaCallback not null
    int count = 0;
    std::shared_ptr<DeviceManagerFaCallback> DmFaCallback = std::make_shared<DeviceManagerFaCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceManagerFaCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceManagerFaCallback(pkgName, DmFaCallback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnFaCall
    std::string testPkgName = "";
    std::string paramJson = "trstParamJson";
    DeviceManagerNotify::GetInstance().OnFaCall(testPkgName, paramJson);
    // 7. check if callback OnCheckAuthResult called
    ASSERT_EQ(count, 1);
}

/*
 * Feature: DeviceManagerNotifyTest OnFaCall
 * Function: DeviceManagerNotifyTest
 * SubFunction: OnFaCall
 * FunctionPoints: DeviceManagerNotifyTest OnFaCall
 * EnvConditions: OnFaCall success.
 * CaseDescription: 1. set pkgName not null
 *                     set DmFaCallback not null
 *                  2. set checkMap null
 *                  3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
 *                  4. Get checkMap from DeviceManagerNotify
 *                  5. check checkMap not null
 *                  6. call DeviceManagerNotify OnFaCall with testPkgName
 *                  7. check if callback OnCheckAuthResult called
 */
HWTEST_F(DeviceManagerNotifyTest, OnFaCall5, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "";
    // set DmFaCallback not null
    int count = 0;
    std::shared_ptr<DeviceManagerFaCallback> DmFaCallback = std::make_shared<DeviceManagerFaCallbackTest>(count);
    // 2. set checkMap null
    std::shared_ptr<DeviceManagerFaCallback> checkMap = nullptr;
    // 3. call DeviceManagerNotifyTest RegisterDeviceManagerFaCallback with parameter
    DeviceManagerNotify::GetInstance().RegisterDeviceManagerFaCallback(pkgName, DmFaCallback);
    // 4. Get checkMap from DeviceManagerNotify
    checkMap = DeviceManagerNotify::GetInstance().dmFaCallback_[pkgName];
    // 5. check checkMap not null
    ASSERT_NE(checkMap, nullptr);
    // 6. call DeviceManagerNotify OnFaCall
    std::string testPkgName = "";
    DeviceManagerNotify::GetInstance().UnRegisterDeviceManagerFaCallback(testPkgName);
    std::string paramJson = "trstParamJson";
    DeviceManagerNotify::GetInstance().OnFaCall(testPkgName, paramJson);
    // 7. check if callback OnCheckAuthResult called
    ASSERT_EQ(count, 0);
}
} // namespace

DmInitCallbackTest::DmInitCallbackTest(int &count) : DmInitCallback()
{
    count_ = &count;
}
void DmInitCallbackTest::OnRemoteDied()
{
    *count_ = *count_ + 1;
}

DeviceStateCallbackTest::DeviceStateCallbackTest(int &count) : DeviceStateCallback()
{
    count_ = &count;
}

void DeviceStateCallbackTest::OnDeviceOnline(const DmDeviceInfo &deviceInfo)
{
    *count_ = *count_ + 1;
}

void DeviceStateCallbackTest::OnDeviceReady(const DmDeviceInfo &deviceInfo)
{
    *count_ = *count_ + 1;
}

void DeviceStateCallbackTest::OnDeviceOffline(const DmDeviceInfo &deviceInfo)
{
    *count_ = *count_ + 1;
}

void DeviceStateCallbackTest::OnDeviceChanged(const DmDeviceInfo &deviceInfo)
{
    *count_ = *count_ + 1;
}

DiscoveryCallbackTest::DiscoveryCallbackTest(int &count) : DiscoveryCallback()
{
    count_ = &count;
}

void DiscoveryCallbackTest::OnDeviceFound(uint16_t subscribeId, const DmDeviceInfo &deviceInfo)
{
    *count_ = *count_ + 1;
}

void DiscoveryCallbackTest::OnDiscoveryFailed(uint16_t subscribeId, int32_t failedReason)
{
    *count_ = *count_ + 1;
}

void DiscoveryCallbackTest::OnDiscoverySuccess(uint16_t subscribeId)
{
    *count_ = *count_ + 1;
}

AuthenticateCallbackTest::AuthenticateCallbackTest(int &count) : AuthenticateCallback()
{
    count_ = &count;
}

void AuthenticateCallbackTest::OnAuthResult(const std::string &deviceId, const std::string &token, int32_t status,
    int32_t reason)
{
    *count_ = *count_ + 1;
}

DeviceManagerFaCallbackTest::DeviceManagerFaCallbackTest(int &count) : DeviceManagerFaCallback()
{
    count_ = &count;
}

void DeviceManagerFaCallbackTest::OnCall(const std::string &paramJson)
{
    *count_ = *count_ + 1;
}
} // namespace DistributedHardware
} // namespace OHOS