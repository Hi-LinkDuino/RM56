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


#include "UTTest_ipc_client_manager.h"
#include "device_manager_notify.h"
#include "dm_device_info.h"
#include "ipc_client_stub.h"
#include "ipc_register_listener_req.h"
#include "ipc_remote_broker.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "dm_constants.h"
#include "system_ability_definition.h"

#include <unistd.h>

namespace OHOS {
namespace DistributedHardware {
void IpcClientManagerTest::SetUp()
{
}

void IpcClientManagerTest::TearDown()
{
}

void IpcClientManagerTest::SetUpTestCase()
{
}

void IpcClientManagerTest::TearDownTestCase()
{
}

namespace {
    /**
 * @tc.name: OnRemoteDied_001
 * @tc.desc: 1. new a dmInterface
 *           2. set IpcClientManager dmInterface_ not null
 *           3. call ClientInit
 *           4. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, OnRemoteDied_001, testing::ext::TestSize.Level0)
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

/**
 * @tc.name: ClientInit_001
 * @tc.desc: 1. new a dmInterface
 *           2. set IpcClientManager dmInterface_ not null
 *           3. call ClientInit
 *           4. check ret is not DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, ClientInit_001, testing::ext::TestSize.Level0)
{
    // 1. new a dmInterface
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto object = samgr->CheckSystemAbility(DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID);
    sptr<IpcRemoteBroker> dmInterface = iface_cast<IpcRemoteBroker>(object);
    // 2. set IpcClientManager dmInterface_ not null
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    instance->dmInterface_ = dmInterface;
    // 3. call ClientInit
    int ret = instance->ClientInit();
    // 4. check ret is not DM_FAILED
    ASSERT_NE(ret, DM_FAILED);
}

/**
 * @tc.name: ClientInit_002
 * @tc.desc: 1. new a dmInterface
 *           2. set IpcClientManager dmInterface_ not null
 *           3. call ClientInit
 *           4. check ret is not DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, ClientInit_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    // 3. call ClientInit
    int ret = instance->ClientInit();
    // 4. check ret is not DM_FAILED
    ASSERT_NE(ret, DM_FAILED);
}

/**
 * @tc.name: Init_001
 * @tc.desc: 1. new a listener
 *           2. set a pkgName not null
 *           3. add listener and pkgName in dmListener_ Map
 *           4. call Init with pkgName
 *           5. check ret is not DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, Init_001, testing::ext::TestSize.Level0)
{
    // 1. new a listener
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    // 2. set a pkgName not null
    std::string pkgName = "com.ohos.test";
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    // 3. add listener and pkgName in dmListener_ Map
    instance->dmListener_[pkgName] = listener;
    // 4. call Init with pkgName
    int32_t ret = instance->Init(pkgName);
    // 5. check ret is not DM_FAILED
    ASSERT_NE(ret, DM_FAILED);
}

/**
 * @tc.name: Init_002
 * @tc.desc: 1. set pkcName not null
 *           2. Mock IpcClientServerProxy SendCmd return DM_FAILED
 *           3. call Init with pkgName
 *           4. check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, Init_002, testing::ext::TestSize.Level0)
{
    // 1. set pkcName not null
    std::string pkgName = "com.ohos.test";
    // 2. Mock IpcClientServerProxy SendCmd return DM_FAILED
    sptr<IRemoteObject> remoteObject = nullptr;
    auto mockInstance = new MockIpcClientManager(remoteObject);
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    instance->dmInterface_ = mockInstance;
    EXPECT_CALL(*mockInstance, SendCmd(testing::_, testing::_, testing::_))
                .Times(1).WillOnce(testing::Return(DM_FAILED));
    // 3. call Init with pkgName
    int32_t ret = instance->Init(pkgName);
    // 4. check ret is DM_FAILED
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: Init_003
 * @tc.desc: 1. set pkcName not null
 *           2. Mock IpcClientServerProxy SendCmd return DM_OK
 *           3. call Init with pkgName
 *           4. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, Init_003, testing::ext::TestSize.Level0)
{
    // 1. set pkcName not null
    std::string pkgName = "com.ohos.test";
    // 2. Mock IpcClientServerProxy SendCmd return DM_OK
    sptr<IRemoteObject> remoteObject = nullptr;
    auto mockInstance = new MockIpcClientManager(remoteObject);
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    instance->dmInterface_ = mockInstance;
    EXPECT_CALL(*mockInstance, SendCmd(testing::_, testing::_, testing::_))
                .Times(1).WillOnce(testing::Return(DM_OK));
    // 3. call Init with pkgName
    int32_t ret = instance->Init(pkgName);
    // 4. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: Init_004
 * @tc.desc: 1. set pkcName not null
 *           2. Mock IpcClientServerProxy SendCmd return DM_OK
 *           3. call Init with pkgName
 *           4. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, Init_004, testing::ext::TestSize.Level0)
{
    // 1. set pkcName not null
    std::string pkgName = "com.ohos.test";
    // 2. Mock IpcClientServerProxy SendCmd return DEVICEMANAGER_SERVICE_NOT_READY
    sptr<IRemoteObject> remoteObject = nullptr;
    auto mockInstance = new MockIpcClientManager(remoteObject);
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    instance->dmInterface_ = mockInstance;
    EXPECT_CALL(*mockInstance, SendCmd(testing::_, testing::_, testing::_))
                .Times(1).WillOnce(testing::Return(DM_SERVICE_NOT_READY));
    // 3. call Init with pkgName
    int32_t ret = instance->Init(pkgName);
    // 4. check ret is DEVICEMANAGER_OK
    ASSERT_EQ(ret, ret);
}

/**
 * @tc.name: Init_005
 * @tc.desc:  1. set pkcName not null
 *            2. Mock IpcClientServerProxy SendCmd return DM_IPC_FAILED
 *            3. call Init with pkgName
 *            4. check ret is DM_IPC_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, Init_005, testing::ext::TestSize.Level0)
{
    // 1. set pkcName not null
    std::string pkgName = "com.ohos.test";
    // 2. Mock IpcClientServerProxy SendCmd return DM_OK
    sptr<IRemoteObject> remoteObject = nullptr;
    auto mockInstance = new MockIpcClientManager(remoteObject);
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    instance->dmInterface_ = mockInstance;
    EXPECT_CALL(*mockInstance, SendCmd(testing::_, testing::_, testing::_))
                .Times(1).WillOnce(testing::Return(DM_IPC_FAILED));
    // 3. call Init with pkgName
    int32_t ret = instance->Init(pkgName);
    // 4. check ret is DM_IPC_FAILED
    ASSERT_EQ(ret, DM_IPC_FAILED);
}

/**
 * @tc.name: UnInit_001
 * @tc.desc:  1. set pkgName null
 *               set IpcClientManager dmInterface_ null
 *            2. call UnInit with pkgName
 *            3. check ret is DEVICEMANAGER_SERVICE_NOT_READY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, UnInit1, testing::ext::TestSize.Level0)
{
    // 1. set pkgName null
    std::string pkgName = "";
    // set IpcClientManager dmInterface_ null
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    instance->dmInterface_ = nullptr;
    // 2. call UnInit with pkgName
    int32_t ret = instance->UnInit(pkgName);
    // 3. check ret is DM_SERVICE_NOT_READY
    ASSERT_EQ(ret, DM_SERVICE_NOT_READY);
}

/**
 * @tc.name: UnInit_002
 * @tc.desc:  1. set pkgName not null
 *            2. Mock IpcClientServerProxy SendCmd return DM_FAILED
 *            3. set IpcClientManager dmInterface_ not null
 *            4. set IpcClientManager dmListener_ not null
 *            5. call UnInit with pkgName
 *            6. check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, UnInit_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. Mock IpcClientServerProxy SendCmd return DM_FAILED
    sptr<IRemoteObject> remoteObject = nullptr;
    auto mockInstance = new MockIpcClientManager(remoteObject);
    EXPECT_CALL(*mockInstance, SendCmd(testing::_, testing::_, testing::_))
                .Times(1).WillOnce(testing::Return(DM_FAILED));
    // 3. set IpcClientManager dmInterface_ not null
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    instance->dmInterface_ = mockInstance;
    // 4. set IpcClientManager dmListener_ not null
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    instance->dmListener_[pkgName] = listener;
    // 5. call UnInit with pkgName
    int32_t ret = instance->UnInit(pkgName);
    // 6. check ret is DM_FAILED
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: UnInit_003
 * @tc.desc:  1. set pkgName not null
 *            2. Mock IpcClientServerProxy SendCmd return DM_OK
 *            3. set IpcClientManager dmInterface_ not null
 *            4. set IpcClientManager dmListener_ not null
 *            5. call UnInit with pkgName
 *            6. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, UnInit_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. Mock IpcClientServerProxy SendCmd return DM_OK
    sptr<IRemoteObject> remoteObject = nullptr;
    auto mockInstance = new MockIpcClientManager(remoteObject);
    EXPECT_CALL(*mockInstance, SendCmd(testing::_, testing::_, testing::_))
                .Times(1).WillOnce(testing::Return(DM_OK));
    // 3. set IpcClientManager dmInterface_ not null
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    instance->dmInterface_ = mockInstance;
    // 4. set IpcClientManager dmListener_ not null
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    instance->dmListener_[pkgName] = listener;
    // 5. call UnInit with pkgName
    int32_t ret = instance->UnInit(pkgName);
    // 6. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: UnInit_004
 * @tc.desc:  1. set pkgName not null
 *            2. Mock IpcClientServerProxy SendCmd return DM_SERVICE_NOT_READY
 *            3. set IpcClientManager dmInterface_ not null
 *            4. set IpcClientManager dmListener_ not null
 *            5. call UnInit with pkgName
 *            6. check ret is DM_SERVICE_NOT_READY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, UnInit_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. Mock IpcClientServerProxy SendCmd return DM_SERVICE_NOT_READY
    sptr<IRemoteObject> remoteObject = nullptr;
    auto mockInstance = new MockIpcClientManager(remoteObject);
    EXPECT_CALL(*mockInstance, SendCmd(testing::_, testing::_, testing::_))
                .Times(1).WillOnce(testing::Return(DM_SERVICE_NOT_READY));
    // 3. set IpcClientManager dmInterface_ not null
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    instance->dmInterface_ = mockInstance;
    // 4. set IpcClientManager dmListener_ not null
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    instance->dmListener_[pkgName] = listener;
    // 5. call UnInit with pkgName
    int32_t ret = instance->UnInit(pkgName);
    // 6. check ret is DM_SERVICE_NOT_READY
    ASSERT_EQ(ret, DM_SERVICE_NOT_READY);
}

/**
 * @tc.name: UnInit_005
 * @tc.desc:  1. set pkgName not null
 *            2. Mock IpcClientServerProxy SendCmd return DM_IPC_FAILED
 *            3. set IpcClientManager dmInterface_ not null
 *            4. set IpcClientManager dmListener_ not null
 *            5. call UnInit with pkgName
 *            6. check ret is DM_IPC_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, UnInit_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. Mock IpcClientServerProxy SendCmd return DM_IPC_FAILED
    sptr<IRemoteObject> remoteObject = nullptr;
    auto mockInstance = new MockIpcClientManager(remoteObject);
    EXPECT_CALL(*mockInstance, SendCmd(testing::_, testing::_, testing::_))
                .Times(1).WillOnce(testing::Return(DM_IPC_FAILED));
    // 3. set IpcClientManager dmInterface_ not null
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    instance->dmInterface_ = mockInstance;
    // 4. set IpcClientManager dmListener_ not null
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    instance->dmListener_[pkgName] = listener;
    // 5. call UnInit with pkgName
    int32_t ret = instance->UnInit(pkgName);
    // 6. check ret is DM_IPC_FAILED
    ASSERT_EQ(ret, DM_IPC_FAILED);
}

/**
 * @tc.name: SendRequest_001
 * @tc.desc:  1. set pkgName null
 *            2. set IpcClientManager dmInterface_null
 *            3. call SendRequest with parameter
 *            4. check ret is DM_SERVICE_NOT_READY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, SendRequest_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName null
    std::string pkgName = "";
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    // 2. set IpcClientManager dmInterface_null
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    instance->dmInterface_ = nullptr;
    // 3. call SendRequest with parameter
    int ret = instance->SendRequest(0, req, rsp);
    // 4. check ret is DM_SERVICE_NOT_READY
    ASSERT_EQ(ret, DM_SERVICE_NOT_READY);
}

/**
 * @tc.name: SendRequest_002
 * @tc.desc:  1. set pkgName not null
 *            2. Mock IpcClientServerProxy SendCmd return DM_FAILED
 *            3. set IpcClientManager dmInterface_ not null
 *            4. set IpcClientManager dmListener_ not null
 *            5. call SendRequest with parameter
 *            6. check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, SendRequest_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    // 2. Mock IpcClientServerProxy SendCmd return DM_FAILED
    sptr<IRemoteObject> remoteObject = nullptr;
    auto mockInstance = new MockIpcClientManager(remoteObject);
    EXPECT_CALL(*mockInstance, SendCmd(testing::_, testing::_, testing::_))
                .Times(1).WillOnce(testing::Return(DM_FAILED));
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    // 3. set IpcClientManager dmInterface_ not null
    instance->dmInterface_ = mockInstance;
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    // 4. set IpcClientManager dmListener_ not null
    instance->dmListener_[pkgName] = listener;
    // 5. call SendRequest with parameter
    int ret = instance->SendRequest(0, req, rsp);
    // 6. check ret is DM_FAILED
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: SendRequest_003
 * @tc.desc:  1. set pkgName not null
 *            2. Mock IpcClientServerProxy SendCmd return DM_OK
 *            3. set IpcClientManager dmInterface_ not null
 *            4. set IpcClientManager dmListener_ not null
 *            5. call SendRequest with parameter
 *            6. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, SendRequest_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    // 2. Mock IpcClientServerProxy SendCmd return DM_OK
    sptr<IRemoteObject> remoteObject = nullptr;
    auto mockInstance = new MockIpcClientManager(remoteObject);
    EXPECT_CALL(*mockInstance, SendCmd(testing::_, testing::_, testing::_))
                .Times(1).WillOnce(testing::Return(DM_OK));
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    // 3. set IpcClientManager dmInterface_ not null
    instance->dmInterface_ = mockInstance;
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    // 4. set IpcClientManager dmListener_ not null
    instance->dmListener_[pkgName] = listener;
    // 5. call SendRequest with parameter
    int ret = instance->SendRequest(0, req, rsp);
    // 6. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: SendRequest_004
 * @tc.desc:  1. set pkgName not null
 *            2. Mock IpcClientServerProxy SendCmd return DM_SERVICE_NOT_READY
 *            3. set IpcClientManager dmInterface_ not null
 *            4. set IpcClientManager dmListener_ not null
 *            5. call SendRequest with parameter
 *            6. check ret is DM_SERVICE_NOT_READY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, SendRequest_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    // 2. Mock IpcClientServerProxy SendCmd return DM_SERVICE_NOT_READY
    sptr<IRemoteObject> remoteObject = nullptr;
    auto mockInstance = new MockIpcClientManager(remoteObject);
    EXPECT_CALL(*mockInstance, SendCmd(testing::_, testing::_, testing::_))
                .Times(1).WillOnce(testing::Return(DM_SERVICE_NOT_READY));
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    // 3. set IpcClientManager dmInterface_ not null
    instance->dmInterface_ = mockInstance;
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    // 4. set IpcClientManager dmListener_ not null
    instance->dmListener_[pkgName] = listener;
    // 5. call SendRequest with parameter
    int ret = instance->SendRequest(0, req, rsp);
    // 6. check ret is DM_SERVICE_NOT_READY
    ASSERT_EQ(ret, DM_SERVICE_NOT_READY);
}

/**
 * @tc.name: SendRequest_005
 * @tc.desc:  1. set pkgName not null
 *            2. Mock IpcClientServerProxy SendCmd return DM_IPC_FAILED
 *            3. set IpcClientManager dmInterface_ not null
 *            4. set IpcClientManager dmListener_ not null
 *            5. call SendRequest with parameter
 *            6. check ret is DM_IPC_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, SendRequest_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    // 2. Mock IpcClientServerProxy SendCmd return DM_IPC_FAILED
    sptr<IRemoteObject> remoteObject = nullptr;
    auto mockInstance = new MockIpcClientManager(remoteObject);
    EXPECT_CALL(*mockInstance, SendCmd(testing::_, testing::_, testing::_))
                .Times(1).WillOnce(testing::Return(DM_IPC_FAILED));
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    // 3. set IpcClientManager dmInterface_ not null
    instance->dmInterface_ = mockInstance;
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    // 4. set IpcClientManager dmListener_ not null
    instance->dmListener_[pkgName] = listener;
    // 5. call SendRequest with parameter
    int ret = instance->SendRequest(0, req, rsp);
    // 6. check ret is DM_IPC_FAILED
    ASSERT_EQ(ret, DM_IPC_FAILED);
}

/**
 * @tc.name: IsInit_001
 * @tc.desc: 1. set pkgName null
 *           2. set IpcClientManager dmInterface_null
 *           3. call IsInit with parameter
 *           4. check ret is false
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, IsInit_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName null
    std::string pkgName = "";
    // 2. set IpcClientManager dmInterface_null
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    instance->dmInterface_ = nullptr;
    // 3. call SendRequest with parameter
    bool ret = instance->IsInit(pkgName);
    // 4. check ret is false
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: IsInit_002
 * @tc.desc: 1. set pkgName not null
 *           2. set IpcClientManager dmInterface_ not null
 *           3. call IsInit with parameter
 *           4. check ret is false
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientManagerTest, IsInit_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName null
    std::string pkgName = "";
    // 2. set IpcClientManager dmInterface_ not null
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto object = samgr->CheckSystemAbility(DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID);
    sptr<IpcRemoteBroker> dmInterface = iface_cast<IpcRemoteBroker>(object);
    std::shared_ptr<IpcClientManager> instance = std::make_shared<IpcClientManager>();
    instance->dmInterface_ = dmInterface;
    // 3. call IsInit with parameter
    bool ret = instance->IsInit(pkgName);
    // 4. check ret is false
    ASSERT_EQ(ret, false);
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
} // namespace DistributedHardware
} // namespace OHOS
