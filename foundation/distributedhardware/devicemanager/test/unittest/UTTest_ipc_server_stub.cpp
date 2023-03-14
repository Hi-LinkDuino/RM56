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

#include "UTTest_ipc_server_stub.h"
#include "dm_device_info.h"
#include "ipc_server_stub.h"
#include "device_manager_impl.h"
#include "dm_constants.h"
#include "if_system_ability_manager.h"
#include "ipc_cmd_register.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"

#include <algorithm>
#include <thread>
#include <unistd.h>

namespace OHOS {
namespace DistributedHardware {
void IpcServerStubTest::SetUp()
{
}

void IpcServerStubTest::TearDown()
{
}

void IpcServerStubTest::SetUpTestCase()
{
}

void IpcServerStubTest::TearDownTestCase()
{
}

namespace {
/**
 * @tc.name: OnStart_001
 * @tc.desc: 1. Call IpcServerStub OnStart
 *           2. check IpcServerStub.state is ServiceRunningState::STATE_RUNNING
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, OnStart_001, testing::ext::TestSize.Level0)
{
    // 1. Call IpcServerStub OnStart
    IpcServerStub::GetInstance().OnStart();
    // 2. check IpcServerStub.state is ServiceRunningState::STATE_RUNNING
    ASSERT_EQ(ServiceRunningState::STATE_RUNNING, IpcServerStub::GetInstance().state_);
}

/**
 * @tc.name: OnStart_002
 * @tc.desc: 1. set IpcServerStub state is ServiceRunningState::STATE_RUNNING
 *           2. Call IpcServerStub OnStart
 *           3. check IpcServerStub.state is ServiceRunningState::STATE_RUNNING
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, OnStart_002, testing::ext::TestSize.Level0)
{
    // 1. set IpcServerStub state is ServiceRunningState::STATE_RUNNING
    IpcServerStub::GetInstance().state_ = ServiceRunningState::STATE_RUNNING;
    // 2. Call IpcServerStub OnStart
    IpcServerStub::GetInstance().OnStart();
    // 3. check IpcServerStub.state is ServiceRunningState::STATE_RUNNING
    ASSERT_EQ(ServiceRunningState::STATE_RUNNING, IpcServerStub::GetInstance().state_);
}

/**
 * @tc.name: Init_001
 * @tc.desc: 1. Call IpcServerStub OnStart
 *           2. check IpcServerStub.state is ServiceRunningState::STATE_RUNNING
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, Init_001, testing::ext::TestSize.Level0)
{
    IpcServerStub::GetInstance().registerToService_=true;
    bool result = IpcServerStub::GetInstance().Init();
    // 2. check IpcServerStub.state is ServiceRunningState::STATE_RUNNING
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: Init_002
 * @tc.desc: 1. Call IpcServerStub OnStart
 *           2. check IpcServerStub.state is ServiceRunningState::STATE_RUNNING
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, Init_002, testing::ext::TestSize.Level0)
{
    IpcServerStub::GetInstance().registerToService_=false;
    bool result = IpcServerStub::GetInstance().Init();
    // 2. check IpcServerStub.state is ServiceRunningState::STATE_RUNNING
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: OnStop_001
 * @tc.desc: 1. Call IpcServerStub OnStop
 *           2. check IpcServerStub.state is ServiceRunningState::STATE_RUNNING
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, OnStop_001, testing::ext::TestSize.Level0)
{
    // 1. Call IpcServerStub OnStop
    IpcServerStub::GetInstance().OnStop();
    // 2. check IpcServerStub.state is ServiceRunningState::STATE_RUNNING
    ASSERT_EQ(ServiceRunningState::STATE_NOT_START, IpcServerStub::GetInstance().state_);
    ASSERT_EQ(IpcServerStub::GetInstance().registerToService_, false);
}

/**
 * @tc.name: OnRemoteRequest_001
 * @tc.desc: 1. Set Code = 999
 *           2. Call IpcServerStub OnRemoteRequest with param
 *           3. check ret not DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, OnRemoteRequest_001, testing::ext::TestSize.Level0)
{
    // 1. Set Code = 999
    uint32_t code = 999;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int ret = 0;
    // 2. Call IpcServerStub OnRemoteRequest with param
    ret = IpcServerStub::GetInstance().OnRemoteRequest(code, data, reply, option);
    // 3. check ret not DM_OK
    ASSERT_NE(ret, DM_OK);
}

/**
 * @tc.name: OnRemoteRequest_002
 * @tc.desc: 1. Set Code = 999
 *           2. Call IpcServerStub OnRemoteRequest with param
 *           3. check ret not DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, OnRemoteRequest_002, testing::ext::TestSize.Level0)
{
    // 1. Set Code is SERVER_DEVICE_STATE_NOTIFY
    uint32_t code = SERVER_DEVICE_STATE_NOTIFY;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int ret = 0;
    // 2. Call IpcServerStub OnRemoteRequest with param
    ret = IpcServerStub::GetInstance().OnRemoteRequest(code, data, reply, option);
    // 3. check ret not ERR_INVALID_STATE
    ASSERT_EQ(ret, ERR_INVALID_STATE);
}

/**
 * @tc.name: SendCmd_001
 * @tc.desc: 1. Call IpcServerStub SendCmd
 *           2. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, SendCmd_001, testing::ext::TestSize.Level0)
{
    int32_t cmdCode = 1;
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    // 1. Call IpcServerStub SendCmd
    int32_t ret = IpcServerStub::GetInstance().SendCmd(cmdCode, req, rsp);
    // 2. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: QueryServiceState_001
 * @tc.desc: 1. Call IpcServerStub QueryServiceState
 *           2. check IpcServerStub.state is ServiceRunningState::STATE_RUNNING
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, QueryServiceState_001, testing::ext::TestSize.Level0)
{
    IpcServerStub::GetInstance().state_ = ServiceRunningState::STATE_NOT_START;
    // 1. Call IpcServerStub QueryServiceState
    ServiceRunningState state = IpcServerStub::GetInstance().QueryServiceState();
    // 2. check IpcServerStub.state is ServiceRunningState::STATE_RUNNING
    ASSERT_EQ(state, ServiceRunningState::STATE_NOT_START);
}

/**
 * @tc.name: RegisterDeviceManagerListener_001
 * @tc.desc: 1. Call IpcServerStub RegisterDeviceManagerListener
 *           2. check IpcServerStub.state is ServiceRunningState::STATE_RUNNING
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, RegisterDeviceManagerListener_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "";
    int ret = 0;
    sptr<IRemoteObject> listener = nullptr;
    ret = IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, listener);
    ASSERT_EQ(ret, DM_POINT_NULL);
}

/**
 * @tc.name: RegisterDeviceManagerListener_002
 * @tc.desc: 1. Call IpcServerStub RegisterDeviceManagerListener
 *           2. check IpcServerStub.state is ServiceRunningState::STATE_RUNNING
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, RegisterDeviceManagerListener_002, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.test";
    int ret = 0;
    sptr<IRemoteObject> listener = sptr<IpcClientStub>(new IpcClientStub());
    ret = IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, listener);
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: RegisterDeviceManagerListener_003
 * @tc.desc: 1. Call IpcServerStub RegisterDeviceManagerListener
 *           2. check IpcServerStub.state is ServiceRunningState::STATE_RUNNING
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, RegisterDeviceManagerListener_003, testing::ext::TestSize.Level0)
{
    std::string pkgName = "";
    int ret = 0;
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    ret = IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, listener);
    ASSERT_EQ(ret, DM_POINT_NULL);
}

/**
 * @tc.name: RegisterDeviceManagerListener_004
 * @tc.desc: 1. Set PkgName is com.ohos.test
 *           2. Call IpcServerStub RegisterDeviceManagerListener with param
 *           3. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, RegisterDeviceManagerListener_004, testing::ext::TestSize.Level0)
{
    // 1. Set PkgName is com.ohos.test
    std::string pkgName = "com.ohos.test";
    int ret = 0;
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    // 2. Call IpcServerStub RegisterDeviceManagerListener with param
    ret = IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, listener);
    // 3. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: RegisterDeviceManagerListener_005
 * @tc.desc: 1. Set PkgName is com.ohos.test
 *                  2. Call IpcServerStub RegisterDeviceManagerListener with param
 *                  3. check ret is DM_OK
 *                  4. Call IpcServerStub RegisterDeviceManagerListener with same pkgName another listener
 *                  5. check result is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, RegisterDeviceManagerListener_005, testing::ext::TestSize.Level0)
{
    // 1. Set PkgName is com.ohos.test
    std::string pkgName = "com.ohos.test";
    int ret = 0;
    int result = 0;
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    // 2. Call IpcServerStub RegisterDeviceManagerListener with param
    ret = IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, listener);
    // 3. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
    sptr<IpcClientStub> listener2 = sptr<IpcClientStub>(new IpcClientStub());
    // 4. Call IpcServerStub RegisterDeviceManagerListener with same pkgName another listener
    result = IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, listener2);
    // 5. check result is DM_OK
    ASSERT_EQ(result, DM_OK);
}

/**
 * @tc.name: UnRegisterDeviceManagerListener_001
 * @tc.desc:  1. Call IpcServerStub UnRegisterDeviceManagerListener
 *            2. check ret is DM_POINT_NULL
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, UnRegisterDeviceManagerListener_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "";
    int ret = 0;
    ret = IpcServerStub::GetInstance().UnRegisterDeviceManagerListener(pkgName);
    ASSERT_EQ(ret, DM_POINT_NULL);
}

/**
 * @tc.name: UnRegisterDeviceManagerListener_002
 * @tc.desc:  1. Set PkgName is com.ohos.test
 *            2. Call IpcServerStub RegisterDeviceManagerListener with param
 *            3. check ret is DM_OK
 *            4. Call IpcServerStub UnRegisterDeviceManagerListener
 *            5. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, UnRegisterDeviceManagerListener_002, testing::ext::TestSize.Level0)
{
    // 1. Set PkgName is com.ohos.test
    std::string pkgName = "com.ohos.test";
    int ret = 0;
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    // 2. Call IpcServerStub RegisterDeviceManagerListener with param
    ret = IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, listener);
    // 3. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
    int result = 0;
    // 4. Call IpcServerStub UnRegisterDeviceManagerListener
    result = IpcServerStub::GetInstance().UnRegisterDeviceManagerListener(pkgName);
    // 5. check ret is DM_OK
    ASSERT_EQ(result, DM_OK);
}

/**
 * @tc.name: UnRegisterDeviceManagerListener_003
 * @tc.desc:  1. Set pkgName is com.ohos.test
 *            2. Call IpcServerStub UnRegisterDeviceManagerListener
 *            3. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, UnRegisterDeviceManagerListener_003, testing::ext::TestSize.Level0)
{
    // 1. Set pkgName is com.ohos.test
    std::string pkgName = "com.ohos.test";
    int ret = 0;
    // 2. Call IpcServerStub UnRegisterDeviceManagerListener
    ret = IpcServerStub::GetInstance().UnRegisterDeviceManagerListener(pkgName);
    // 3. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: UnRegisterDeviceManagerListener_004
 * @tc.desc:  1. Set PkgName is com.ohos.test
 *            2. Call IpcServerStub RegisterDeviceManagerListener with param
 *            3. check ret is DM_OK
 *            4. Call IpcServerStub UnRegisterDeviceManagerListener
 *            5. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, UnRegisterDeviceManagerListener_004, testing::ext::TestSize.Level0)
{
    // 1. Set PkgName is com.ohos.test
    std::string pkgName = "com.ohos.test1";
    int ret = 0;
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    // 2. Call IpcServerStub RegisterDeviceManagerListener with param
    ret = IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, listener);
    // 3. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
    int result = 0;
    // 4. Call IpcServerStub UnRegisterDeviceManagerListener
    result = IpcServerStub::GetInstance().UnRegisterDeviceManagerListener(pkgName);
    // 5. check ret is DM_OK
    ASSERT_EQ(result, DM_OK);
    sptr<IRemoteObject> dmListener = IpcServerStub::GetInstance().dmListener_[pkgName];
    ASSERT_EQ(dmListener, nullptr);
}

/**
 * @tc.name: UnRegisterDeviceManagerListener_005
 * @tc.desc:  1. Set PkgName is com.ohos.test
 *            2. Call IpcServerStub RegisterDeviceManagerListener with param
 *            3. check ret is DM_OK
 *            4. Call IpcServerStub UnRegisterDeviceManagerListener
 *            5. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, UnRegisterDeviceManagerListener_005, testing::ext::TestSize.Level0)
{
    // 1. Set PkgName is com.ohos.test
    std::string pkgName = "com.ohos.test2";
    int ret = 0;
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    // 2. Call IpcServerStub RegisterDeviceManagerListener with param
    ret = IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, listener);
    // 3. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
    int result = 0;
    // 4. Call IpcServerStub UnRegisterDeviceManagerListener
    std::string testPkgName = "com.test";
    result = IpcServerStub::GetInstance().UnRegisterDeviceManagerListener(testPkgName);
    // 5. check ret is DM_OK
    ASSERT_EQ(result, DM_OK);
    sptr<IRemoteObject> dmListener = IpcServerStub::GetInstance().dmListener_[pkgName];
    ASSERT_NE(dmListener, nullptr);
}

/**
 * @tc.name: GetDmListener_001
 * @tc.desc: 1. Set pkgName is com.ohos.test
 *           2. Call IpcServerStub GetDmListener
 *           3. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, GetDmListener_001, testing::ext::TestSize.Level0)
{
    // 1. Set pkgName is com.ohos.test
    std::string pkgName = "com.ohos.test";
    sptr<IpcRemoteBroker> ret = nullptr;
    // 2. Call IpcServerStub UnRegisterDeviceManagerListener
    ret = IpcServerStub::GetInstance().GetDmListener(pkgName);
    // 3. check ret is DM_OK
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: GetDmListener_002
 * @tc.desc: 1. Set pkgName is com.ohos.test
 *           2. Call IpcServerStub GetDmListener
 *           3. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, GetDmListener_002, testing::ext::TestSize.Level0)
{
    // 1. Set pkgName is com.ohos.test
    std::string pkgName = "com.ohos.test";
    int result = 0;
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    // 2. Call IpcServerStub RegisterDeviceManagerListener with param
    result = IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, listener);
    // 3. check ret is DM_OK
    ASSERT_EQ(result, DM_OK);
    sptr<IpcRemoteBroker> ret = nullptr;
    // 2. Call IpcServerStub UnRegisterDeviceManagerListener
    ret = IpcServerStub::GetInstance().GetDmListener(pkgName);
    // 3. check ret is DM_OK
    ASSERT_NE(ret, nullptr);
}

/**
 * @tc.name: GetDmListener_003
 * @tc.desc: 1. Set pkgName is com.ohos.test
 *           2. Call IpcServerStub GetDmListener
 *           3. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, GetDmListener_003, testing::ext::TestSize.Level0)
{
    // 1. Set pkgName is com.ohos.test
    std::string pkgName = "com.ohos.test";
    int result = 0;
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    // 2. Call IpcServerStub RegisterDeviceManagerListener with param
    result = IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, listener);
    // 3. check ret is DM_OK
    ASSERT_EQ(result, DM_OK);
    sptr<IpcRemoteBroker> ret = nullptr;
    // 2. Call IpcServerStub UnRegisterDeviceManagerListener
    std::string testPkgName = "test";
    ret = IpcServerStub::GetInstance().GetDmListener(testPkgName);
    // 3. check ret is DM_OK
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: GetDmListener_004
 * @tc.desc: 1. Set pkgName is com.ohos.test
 *           2. Call IpcServerStub GetDmListener
 *           3. check ret is DM_POINT_NULL
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, GetDmListener_004, testing::ext::TestSize.Level0)
{
    // 1. Set pkgName is null
    std::string pkgName = "";
    int result = 0;
    sptr<IpcClientStub> listener = sptr<IpcClientStub>(new IpcClientStub());
    // 2. Call IpcServerStub RegisterDeviceManagerListener with param
    result = IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, listener);
    // 3. check ret is DM_POINT_NULL
    ASSERT_EQ(result, DM_POINT_NULL);
    sptr<IpcRemoteBroker> ret = nullptr;
    // 2. Call IpcServerStub UnRegisterDeviceManagerListener
    ret = IpcServerStub::GetInstance().GetDmListener(pkgName);
    // 3. check ret is nullptr
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: GetDmListener_005
 * @tc.desc: 1. Set pkgName is com.ohos.test
 *           2. Call IpcServerStub GetDmListener
 *           3. check ret is DM_POINT_NULL
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcServerStubTest, GetDmListener_005, testing::ext::TestSize.Level0)
{
    // 1. Set pkgName is null
    std::string pkgName = "com.test.ohos";
    int result = 0;
    sptr<IpcClientStub> listener = nullptr;
    // 2. Call IpcServerStub RegisterDeviceManagerListener with param
    result = IpcServerStub::GetInstance().RegisterDeviceManagerListener(pkgName, listener);
    // 3. check ret is DM_POINT_NULL
    ASSERT_EQ(result, DM_POINT_NULL);
    sptr<IpcRemoteBroker> ret = nullptr;
    // 2. Call IpcServerStub UnRegisterDeviceManagerListener
    ret = IpcServerStub::GetInstance().GetDmListener(pkgName);
    // 3. check ret is nullptr
    ASSERT_EQ(ret, nullptr);
}
} // namespace
} // namespace DistributedHardware
} // namespace OHOS
