/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "UTTest_ipc_server_listener.h"

#include <unistd.h>
#include "dm_device_info.h"
#include "ipc_remote_broker.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "ipc_client_manager.h"
#include "ipc_set_useroperation_req.h"
#include "ipc_notify_device_state_req.h"
#include "ipc_req.h"
#include "ipc_rsp.h"
#include "dm_constants.h"


namespace OHOS {
namespace DistributedHardware {
void IpcServerListenerTest::SetUp()
{
}

void IpcServerListenerTest::TearDown()
{
}

void IpcServerListenerTest::SetUpTestCase()
{
}

void IpcServerListenerTest::TearDownTestCase()
{
}

namespace {
/**
 * @tc.name: SendRequest_001
 * @tc.desc: 1. set cmdCode not null
 *              set pkgName not null
 *           2. set remoteObject nullptr
 *              set req not null
 *              set rsp not null
 *           3. call IpcServerListener SendRequest
 *           4. check ret is DM_POINT_NULL
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerListenerTest, SendRequest_001, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = 999;
    // set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. set remoteObject nullptr
    sptr<IRemoteObject> remoteObject = nullptr;
    // set req not null
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    // set rsp not null
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    // 3. call IpcServerListener SendRequest
    std::shared_ptr<IpcServerListener> ipcServerListener = std::make_shared<IpcServerListener>();
    int ret = ipcServerListener->SendRequest(cmdCode, req, rsp);
    // 4. check ret is DM_POINT_NULL
    ASSERT_EQ(ret, DM_POINT_NULL);
}

/**
 * @tc.name: SendRequest_002
 * @tc.desc: 1. set cmdCode not null
 *              set pkgName not null
 *           2. set remoteObject nullptr
 *              set req not null
 *              set rsp not null
 *           3. call IpcServerListener SendRequest
 *           4. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerListenerTest, SendRequest_002, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = SERVER_DEVICE_STATE_NOTIFY;
    // set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. set remoteObject not nullptr
    sptr<IpcClientStub> remoteObject = sptr<IpcClientStub>(new IpcClientStub());
    IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, remoteObject);
    // set req not null
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    // set rsp not null
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    // 3. call IpcServerListener SendRequest
    std::shared_ptr<IpcServerListener> ipcServerListener = std::make_shared<IpcServerListener>();
    int ret = ipcServerListener->SendRequest(cmdCode, req, rsp);
    // 4. check ret is not DM_FAILED
    ASSERT_NE(ret, DM_FAILED);
}

/**
 * @tc.name: SendRequest_003
 * @tc.desc: 1. set cmdCode not null
 *              set pkgName not null
 *           2. set remoteObject nullptr
 *              set req not null
 *              set rsp not null
 *           3. call IpcServerListener SendRequest
 *           4. check ret is DM_IPC_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerListenerTest, SendRequest_003, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = 9999;
    // set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. set remoteObject not nullptr
    sptr<IpcClientStub> remoteObject = sptr<IpcClientStub>(new IpcClientStub());
    IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, remoteObject);
    // set req not null
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    // set rsp not null
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    // 3. call IpcServerListener SendRequest
    std::shared_ptr<IpcServerListener> ipcServerListener = std::make_shared<IpcServerListener>();
    int ret = ipcServerListener->SendRequest(cmdCode, req, rsp);
    // 4. check ret is not DM_FAILED
    ASSERT_NE(ret, DM_FAILED);
}

/**
 * @tc.name: SendRequest_004
 * @tc.desc: 1. set cmdCode not null
 *              set pkgName not null
 *           2. set remoteObject nullptr
 *              set req not null
 *              set rsp not null
 *           3. call IpcServerListener SendRequest
 *           4. check ret is DM_POINT_NULL
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerListenerTest, SendRequest_004, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = SERVER_DEVICE_STATE_NOTIFY;
    // set pkgName not null
    std::string pkgName = "";
    // 2. set remoteObject not nullptr
    sptr<IpcClientStub> remoteObject = sptr<IpcClientStub>(new IpcClientStub());
    IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, remoteObject);
    // set req not null
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    // set rsp not null
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    // 3. call IpcServerListener SendRequest
    std::shared_ptr<IpcServerListener> ipcServerListener = std::make_shared<IpcServerListener>();
    int ret = ipcServerListener->SendRequest(cmdCode, req, rsp);
    // 4. check ret is DM_POINT_NULL
    ASSERT_EQ(ret, DM_POINT_NULL);
}

/**
 * @tc.name: SendRequest_005
 * @tc.desc: 1. set cmdCode not null
 *              set pkgName not null
 *           2. set remoteObject nullptr
 *              set req not null
 *              set rsp not null
 *           3. call IpcServerListener SendRequest
 *           4. check ret is DM_POINT_NULL
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerListenerTest, SendRequest_005, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = 9999;
    // set pkgName not null
    std::string pkgName = "";
    // 2. set remoteObject not nullptr
    sptr<IpcClientStub> remoteObject = sptr<IpcClientStub>(new IpcClientStub());
    IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, remoteObject);
    // set req not null
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    // set rsp not null
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    // 3. call IpcServerListener SendRequest
    std::shared_ptr<IpcServerListener> ipcServerListener = std::make_shared<IpcServerListener>();
    int ret = ipcServerListener->SendRequest(cmdCode, req, rsp);
    // 4. check ret is DM_POINT_NULL
    ASSERT_EQ(ret, DM_POINT_NULL);
}

/**
 * @tc.name: SendAll_001
 * @tc.desc: 1. set cmdCode not null
 *              set pkgName not null
 *           2. set remoteObject nullptr
 *              set req not null
 *              set rsp not null
 *           3. call IpcServerListener SendRequest
 *           4. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerListenerTest, SendAll_001, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = SERVER_DEVICE_STATE_NOTIFY;
    // set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. set remoteObject not nullptr
    sptr<IpcClientStub> remoteObject = sptr<IpcClientStub>(new IpcClientStub());
    IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, remoteObject);
    // set req not null
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    // set rsp not null
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    // 3. call IpcServerListener SendRequest
    std::shared_ptr<IpcServerListener> ipcServerListener = std::make_shared<IpcServerListener>();
    int ret = ipcServerListener->SendAll(cmdCode, req, rsp);
    // 4. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: SendAll_002
 * @tc.desc: 1. set cmdCode not null
 *              set pkgName not null
 *           2. set remoteObject nullptr
 *              set req not null
 *              set rsp not null
 *           3. call IpcServerListener SendRequest
 *           4. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerListenerTest, SendAll_002, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = 9999;
    // set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. set remoteObject not nullptr
    sptr<IpcClientStub> remoteObject = sptr<IpcClientStub>(new IpcClientStub());
    IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, remoteObject);
    // set req not null
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    // set rsp not null
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    // 3. call IpcServerListener SendRequest
    std::shared_ptr<IpcServerListener> ipcServerListener = std::make_shared<IpcServerListener>();
    int ret = ipcServerListener->SendAll(cmdCode, req, rsp);
    // 4. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: SendAll_003
 * @tc.desc: 1. set cmdCode not null
 *              set pkgName not null
 *           2. set remoteObject nullptr
 *              set req not null
 *              set rsp not null
 *           3. call IpcServerListener SendRequest
 *           4. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerListenerTest, SendAll_003, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = SERVER_DEVICE_STATE_NOTIFY;
    // set pkgName not null
    std::string pkgName = "";
    // 2. set remoteObject not nullptr
    sptr<IpcClientStub> remoteObject = sptr<IpcClientStub>(new IpcClientStub());
    IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, remoteObject);
    // set req not null
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    // set rsp not null
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    // 3. call IpcServerListener SendRequest
    std::shared_ptr<IpcServerListener> ipcServerListener = std::make_shared<IpcServerListener>();
    int ret = ipcServerListener->SendAll(cmdCode, req, rsp);
    // 4. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: SendAll_004
 * @tc.desc: 1. set cmdCode not null
 *              set pkgName not null
 *           2. set remoteObject nullptr
 *              set req not null
 *              set rsp not null
 *           3. call IpcServerListener SendRequest
 *           4. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerListenerTest, SendAll_004, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = 9999;
    // set pkgName not null
    std::string pkgName = "";
    // 2. set remoteObject not nullptr
    sptr<IpcClientStub> remoteObject = sptr<IpcClientStub>(new IpcClientStub());
    IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, remoteObject);
    // set req not null
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    // set rsp not null
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    // 3. call IpcServerListener SendRequest
    std::shared_ptr<IpcServerListener> ipcServerListener = std::make_shared<IpcServerListener>();
    int ret = ipcServerListener->SendAll(cmdCode, req, rsp);
    // 4. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: SendAll_005
 * @tc.desc: 1. set cmdCode not null
 *              set pkgName not null
 *           2. set remoteObject nullptr
 *              set req not null
 *              set rsp not null
 *           3. call IpcServerListener SendRequest
 *           4. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerListenerTest, SendAll_005, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = SERVER_DEVICE_STATE_NOTIFY;
    // set pkgName not null
    std::string pkgName = "com.ohos.test";
    // 2. set remoteObject not nullptr
    sptr<IpcClientStub> remoteObject = sptr<IpcClientStub>(new IpcClientStub());
    IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, remoteObject);
    // set req not null
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    // set rsp not null
    std::shared_ptr<IpcRsp> rsp = nullptr;
    req->SetPkgName(pkgName);
    // 3. call IpcServerListener SendRequest
    std::shared_ptr<IpcServerListener> ipcServerListener = std::make_shared<IpcServerListener>();
    int ret = ipcServerListener->SendAll(cmdCode, req, rsp);
    // 4. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}
} // namespace
} // namespace DistributedHardware
} // namespace OHOS
