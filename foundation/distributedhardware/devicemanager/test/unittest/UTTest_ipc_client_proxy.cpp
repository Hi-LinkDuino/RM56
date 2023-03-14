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

#include "UTTest_ipc_client_proxy.h"
#include "dm_device_info.h"
#include "ipc_remote_broker.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "ipc_client_manager.h"
#include "dm_constants.h"

#include <unistd.h>

namespace OHOS {
namespace DistributedHardware {
void IpcClientProxyTest::SetUp()
{
}

void IpcClientProxyTest::TearDown()
{
}

void IpcClientProxyTest::SetUpTestCase()
{
}

void IpcClientProxyTest::TearDownTestCase()
{
}

namespace {
/**
 * @tc.name: Init_001
 * @tc.desc: 1. set pkgName not null
 *           2. set IpcClientProxy ipcClientManager nullptr
 *           3. call IpcClientProxy Init
 *           4. check ret is DM_POINT_NULL
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientProxyTest, Init_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. set IpcClientProxy ipcClientManager nullptr
    std::shared_ptr<IpcClient> ipcClientManager = nullptr;
    std::shared_ptr<IpcClientProxy> ipcClientProxy = std::make_shared<IpcClientProxy>(ipcClientManager);
    // 3. call IpcClientProxy
    int32_t ret = ipcClientProxy->Init(pkgName);
    // 4. check ret is DM_POINT_NULL
    ASSERT_EQ(ret, DM_POINT_NULL);
}

/**
 * @tc.name: Init_002
 * @tc.desc: 1. set pkgName not null
 *           2. Mock IpcClient Init return DM_FAILED
 *           3. call IpcClientProxy Init
 *           4. check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientProxyTest, Init_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. Mock IpcClient Init return DM_FAILED
    std::shared_ptr<MockIpcClient> mockInstance = std::make_shared<MockIpcClient>();
    std::shared_ptr<IpcClient> ipcClientManager = mockInstance;
    EXPECT_CALL(*mockInstance, Init(testing::_)).Times(1).WillOnce(testing::Return(DM_FAILED));
    // 3. call IpcClientProxy Init
    std::shared_ptr<IpcClientProxy> ipcClientProxy = std::make_shared<IpcClientProxy>(ipcClientManager);
    int32_t ret = ipcClientProxy->Init(pkgName);
    // 4. check ret is DM_FAILED
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: Init_003
 * @tc.desc: 1. set pkgName not null
 *           2. Mock IpcClient Init return DM_OK
 *           3. call IpcClientProxy Init
 *           4. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientProxyTest, Init_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. Mock IpcClient Init return DM_OK
    std::shared_ptr<MockIpcClient> mockInstance = std::make_shared<MockIpcClient>();
    std::shared_ptr<IpcClient> ipcClientManager = mockInstance;
    EXPECT_CALL(*mockInstance, Init(testing::_)).Times(1).WillOnce(testing::Return(DM_OK));
    // 3. call IpcClientProxy Init
    std::shared_ptr<IpcClientProxy> ipcClientProxy = std::make_shared<IpcClientProxy>(ipcClientManager);
    int32_t ret = ipcClientProxy->Init(pkgName);
    // 4. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: Init_004
 * @tc.desc: 1. set pkgName not null
 *           2. Mock IpcClient Init return DM_SERVICE_NOT_READY
 *           3. call IpcClientProxy Init
 *           4. check ret is DM_SERVICE_NOT_READY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientProxyTest, Init_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. Mock IpcClient Init return DM_SERVICE_NOT_READY
    std::shared_ptr<MockIpcClient> mockInstance = std::make_shared<MockIpcClient>();
    std::shared_ptr<IpcClient> ipcClientManager = mockInstance;
    EXPECT_CALL(*mockInstance, Init(testing::_)).Times(1).WillOnce(testing::Return(DM_SERVICE_NOT_READY));
    // 3. call IpcClientProxy Init
    std::shared_ptr<IpcClientProxy> ipcClientProxy = std::make_shared<IpcClientProxy>(ipcClientManager);
    int32_t ret = ipcClientProxy->Init(pkgName);
    // 4. check ret is DM_SERVICE_NOT_READY
    ASSERT_EQ(ret, DM_SERVICE_NOT_READY);
}

/**
 * @tc.name: Init_005
 * @tc.desc: 1. set pkgName not null
 *           2. Mock IpcClient Init return DM_IPC_FAILED
 *           3. call IpcClientProxy Init
 *           4. check ret is DM_IPC_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientProxyTest, Init_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. Mock IpcClient Init return DM_IPC_FAILED
    std::shared_ptr<MockIpcClient> mockInstance = std::make_shared<MockIpcClient>();
    std::shared_ptr<IpcClient> ipcClientManager = mockInstance;
    EXPECT_CALL(*mockInstance, Init(testing::_)).Times(1).WillOnce(testing::Return(DM_IPC_FAILED));
    // 3. call IpcClientProxy Init
    std::shared_ptr<IpcClientProxy> ipcClientProxy = std::make_shared<IpcClientProxy>(ipcClientManager);
    int32_t ret = ipcClientProxy->Init(pkgName);
    // 4. check ret is DM_IPC_FAILED
    ASSERT_EQ(ret, DM_IPC_FAILED);
}

/**
 * @tc.name: UnInit_001
 * @tc.desc: 1. set pkgName not null
 *           2. set IpcClientProxy ipcClientManager nullptr
 *           3. call IpcClientProxy UnInit
 *           4. check ret is DM_POINT_NULL
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientProxyTest, UnInit_001, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. set IpcClientProxy ipcClientManager nullptr
    std::shared_ptr<IpcClient> ipcClientManager = nullptr;
    std::shared_ptr<IpcClientProxy> ipcClientProxy = std::make_shared<IpcClientProxy>(ipcClientManager);
    // 3. call IpcClientProxy
    int32_t ret = ipcClientProxy->UnInit(pkgName);
    // 4. check ret is DM_POINT_NULL
    ASSERT_EQ(ret, DM_POINT_NULL);
}

/**
 * @tc.name: UnInit_002
 * @tc.desc: 1. set pkgName not null
 *           2. Mock IpcClient Init return DM_FAILED
 *           3. call IpcClientProxy UnInit
 *           4. check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientProxyTest, UnInit_002, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. Mock IpcClient Init return DM_FAILED
    std::shared_ptr<MockIpcClient> mockInstance = std::make_shared<MockIpcClient>();
    std::shared_ptr<IpcClient> ipcClientManager = mockInstance;
    EXPECT_CALL(*mockInstance, UnInit(testing::_)).Times(1).WillOnce(testing::Return(DM_FAILED));
    // 3. call IpcClientProxy Init
    std::shared_ptr<IpcClientProxy> ipcClientProxy = std::make_shared<IpcClientProxy>(ipcClientManager);
    int32_t ret = ipcClientProxy->UnInit(pkgName);
    // 4. check ret is DM_FAILED
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: UnInit_003
 * @tc.desc: 1. set pkgName not null
 *           2. Mock IpcClient UnInit return DM_OK
 *           3. call IpcClientProxy UnInit
 *           4. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientProxyTest, UnInit_003, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. Mock IpcClient Init return DM_OK
    std::shared_ptr<MockIpcClient> mockInstance = std::make_shared<MockIpcClient>();
    std::shared_ptr<IpcClient> ipcClientManager = mockInstance;
    EXPECT_CALL(*mockInstance, UnInit(testing::_)).Times(1).WillOnce(testing::Return(DM_OK));
    // 3. call IpcClientProxy Init
    std::shared_ptr<IpcClientProxy> ipcClientProxy = std::make_shared<IpcClientProxy>(ipcClientManager);
    int32_t ret = ipcClientProxy->UnInit(pkgName);
    // 4. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: UnInit_004
 * @tc.desc: 1. set pkgName not null
 *           2. Mock IpcClient UnInit return DM_SERVICE_NOT_READY
 *           3. call IpcClientProxy UnInit
 *           4. check ret is DM_SERVICE_NOT_READY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientProxyTest, UnInit_004, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. Mock IpcClient Init return DM_SERVICE_NOT_READY
    std::shared_ptr<MockIpcClient> mockInstance = std::make_shared<MockIpcClient>();
    std::shared_ptr<IpcClient> ipcClientManager = mockInstance;
    EXPECT_CALL(*mockInstance, UnInit(testing::_)).Times(1).WillOnce(testing::Return(DM_SERVICE_NOT_READY));
    // 3. call IpcClientProxy Init
    std::shared_ptr<IpcClientProxy> ipcClientProxy = std::make_shared<IpcClientProxy>(ipcClientManager);
    int32_t ret = ipcClientProxy->UnInit(pkgName);
    // 4. check ret is DM_SERVICE_NOT_READY
    ASSERT_EQ(ret, DM_SERVICE_NOT_READY);
}

/**
 * @tc.name: UnInit_005
 * @tc.desc: 1. set pkgName not null
 *           2. Mock IpcClient UnInit return DM_IPC_FAILED
 *           3. call IpcClientProxy UnInit
 *           4. check ret is DM_IPC_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientProxyTest, UnInit_005, testing::ext::TestSize.Level0)
{
    // 1. set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. Mock IpcClient Init return DM_IPC_FAILED
    std::shared_ptr<MockIpcClient> mockInstance = std::make_shared<MockIpcClient>();
    std::shared_ptr<IpcClient> ipcClientManager = mockInstance;
    EXPECT_CALL(*mockInstance, UnInit(testing::_)).Times(1).WillOnce(testing::Return(DM_IPC_FAILED));
    // 3. call IpcClientProxy Init
    std::shared_ptr<IpcClientProxy> ipcClientProxy = std::make_shared<IpcClientProxy>(ipcClientManager);
    int32_t ret = ipcClientProxy->UnInit(pkgName);
    // 4. check ret is DM_IPC_FAILED
    ASSERT_EQ(ret, DM_IPC_FAILED);
}

/**
 * @tc.name: SendRequest_001
 * @tc.desc: 1. set req nullptr
 *              set rsp not nullptr
 *              set IpcClientProxy ipcClientManager not null
 *           2. call IpcClientProxy SendRequest
 *           3. check ret is DEVICEMANAGER_NULLPTR
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientProxyTest, SendRequest_001, testing::ext::TestSize.Level0)
{
    // 1. set req nullptr
    std::shared_ptr<IpcReq> req = nullptr;
    // set rsp not nullptr
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    // set pcClientProxy ipcClientManager not null
    std::shared_ptr<IpcClient> ipcClientManager = std::make_shared<IpcClientManager>();
    // 2. call IpcClientProxy SendRequest
    std::shared_ptr<IpcClientProxy> ipcClientProxy = std::make_shared<IpcClientProxy>(ipcClientManager);
    int32_t ret = ipcClientProxy->SendRequest(0, req, rsp);
    // 3. check ret is DEVICEMANAGER_NULLPTR
    ASSERT_EQ(ret, DM_POINT_NULL);
}

/**
 * @tc.name: SendRequest_002
 * @tc.desc: 1. set req not nullptr
 *              set rsp  nullptr
 *              set IpcClientProxy ipcClientManager not null
 *           2. call IpcClientProxy SendRequest
 *           3. check ret is DM_POINT_NULL
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientProxyTest, SendRequest_002, testing::ext::TestSize.Level0)
{
    // 1. set req not nullptr
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    // set rsp nullptr
    std::shared_ptr<IpcRsp> rsp = nullptr;
    // set pcClientProxy ipcClientManager not null
    std::shared_ptr<IpcClient> ipcClientManager = std::make_shared<IpcClientManager>();
    // 2. call IpcClientProxy SendRequest
    std::shared_ptr<IpcClientProxy> ipcClientProxy = std::make_shared<IpcClientProxy>(ipcClientManager);
    int32_t ret = ipcClientProxy->SendRequest(0, req, rsp);
    // 3. check ret is DM_POINT_NULL
    ASSERT_EQ(ret, DM_POINT_NULL);
}

/**
 * @tc.name: SendRequest_003
 * @tc.desc: 1. set req not nullptr
 *              set rsp not nullptr
 *              set IpcClientProxy ipcClientManager null
 *           2. call IpcClientProxy SendRequest
 *           3. check ret is DM_POINT_NULL
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientProxyTest, SendRequest_003, testing::ext::TestSize.Level0)
{
    // 1. set req not nullptr
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    // set rsp not nullptr
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    // set pcClientProxy ipcClientManager  null
    std::shared_ptr<IpcClient> ipcClientManager = nullptr;
    // 2. call IpcClientProxy SendRequest
    std::shared_ptr<IpcClientProxy> ipcClientProxy = std::make_shared<IpcClientProxy>(ipcClientManager);
    int32_t ret = ipcClientProxy->SendRequest(0, req, rsp);
    // 3. check ret is DM_POINT_NULL
    ASSERT_EQ(ret, DM_POINT_NULL);
}

/**
 * @tc.name: SendRequest_004
 * @tc.desc: 1. set req not nullptr
 *              set rsp  not nullptr
 *           2. Mock IpcClient SendRequest return DM_FAILED
 *           3. call IpcClientProxy SendRequest
 *           4. check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientProxyTest, SendRequest_004, testing::ext::TestSize.Level0)
{
    // 1. set req not nullptr
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    // set rsp  not nullptr
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    // 2. Mock IpcClient SendRequest return DM_FAILED
    std::shared_ptr<MockIpcClient> mockInstance = std::make_shared<MockIpcClient>();
    std::shared_ptr<IpcClient> ipcClientManager = mockInstance;
    EXPECT_CALL(*mockInstance, SendRequest(testing::_, testing::_, testing::_))
                .Times(1).WillOnce(testing::Return(DM_FAILED));
    // 3. call IpcClientProxy SendRequest
    std::shared_ptr<IpcClientProxy> ipcClientProxy = std::make_shared<IpcClientProxy>(ipcClientManager);
    int32_t ret = ipcClientProxy->SendRequest(0, req, rsp);
    // 4. check ret is DM_FAILED
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: SendRequest_004
 * @tc.desc: 1. set req not nullptr
 *              set rsp  not nullptr
 *           2. Mock IpcClient SendRequest return DM_OK
 *           3. call IpcClientProxy SendRequest
 *           4. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientProxyTest, SendRequest5, testing::ext::TestSize.Level0)
{
    // 1. set req not nullptr
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    // set rsp  not nullptr
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    // 2. Mock IpcClient SendRequest return DM_FAILED
    std::shared_ptr<MockIpcClient> mockInstance = std::make_shared<MockIpcClient>();
    std::shared_ptr<IpcClient> ipcClientManager = mockInstance;
    EXPECT_CALL(*mockInstance, SendRequest(testing::_, testing::_, testing::_))
                .Times(1).WillOnce(testing::Return(DM_OK));
    // 3. call IpcClientProxy SendRequest
    std::shared_ptr<IpcClientProxy> ipcClientProxy = std::make_shared<IpcClientProxy>(ipcClientManager);
    int32_t ret = ipcClientProxy->SendRequest(0, req, rsp);
    // 4. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}
} // namespace
} // namespace DistributedHardware
} // namespace OHOS
