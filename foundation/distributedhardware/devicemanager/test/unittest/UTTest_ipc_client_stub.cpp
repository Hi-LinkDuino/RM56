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

#include "UTTest_ipc_client_stub.h"
#include "dm_device_info.h"
#include "ipc_remote_broker.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "ipc_client_manager.h"
#include "ipc_set_useroperation_req.h"
#include "ipc_cmd_register.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "ipc_rsp.h"
#include "ipc_def.h"
#include "dm_constants.h"

#include <unistd.h>

namespace OHOS {
namespace DistributedHardware {
void IpcClientStubTest::SetUp()
{
}

void IpcClientStubTest::TearDown()
{
}

void IpcClientStubTest::SetUpTestCase()
{
}

void IpcClientStubTest::TearDownTestCase()
{
}

namespace {
/**
 * @tc.name: OnRemoteRequest_001
 * @tc.desc: 1. set MessageOption not null
 *              set MessageParcel not null
 *              set MessageParcel not null
 *           2. set set code is 999
 *           3. call IpcClientStub OnRemoteRequest with parameter
 *           4. check result is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientStubTest, OnRemoteRequest_001, testing::ext::TestSize.Level0)
{
    // 1. set MessageOption not null
    MessageOption option;
    // set MessageParcel not null
    MessageParcel data;
    // set MessageParcel not null
    MessageParcel reply;
    // 2. set set code is 999
    int code = 999;
    sptr<IpcClientStub> instance = new IpcClientStub();
    // 3. call IpcClientStub OnRemoteRequest with parameter
    int32_t result = instance->OnRemoteRequest(code, data, reply, option);
    if (result != DM_OK) {
        result = DM_FAILED;
    }
    // 4. check result is DM_FAILED
    ASSERT_EQ(result, DM_FAILED);
}

/**
 * @tc.name: OnRemoteRequest_002
 * @tc.desc: 1. set MessageOption not null
 *              set MessageParcel not null
 *              set MessageParcel not null
 *           2. set set code is SERVER_DEVICE_FA_NOTIFY
 *           3. call IpcClientStub OnRemoteRequest with parameter
 *           4. check result is ERR_INVALID_STATE
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientStubTest, OnRemoteRequest_002, testing::ext::TestSize.Level0)
{
    // 1. set MessageOption not null
    MessageOption option;
    // set MessageParcel not null
    MessageParcel data;
    // set MessageParcel not null
    MessageParcel reply;
    // 2. set set code is SERVER_DEVICE_FA_NOTIFY
    int code = SERVER_DEVICE_FA_NOTIFY;
    sptr<IpcClientStub> instance = new IpcClientStub();
    // 3. call IpcClientStub OnRemoteRequest with parameter
    int ret = instance->OnRemoteRequest(code, data, reply, option);
    // 4. check result is ERR_INVALID_STATE
    ASSERT_EQ(ret, ERR_INVALID_STATE);
}

/**
 * @tc.name: SendCmd_001
 * @tc.desc: 1. set set code is SERVER_DEVICE_FA_NOTIFY
 *              set req is nullptr
 *              set rsp is nullptr
 *           2. call IpcClientStub SendCmd with parameter
 *           3. check result is DM_IPC_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcClientStubTest, SendCmd_001, testing::ext::TestSize.Level0)
{
    // 1. set set code is SERVER_DEVICE_FA_NOTIFY
    int cmdCode = SERVER_DEVICE_FA_NOTIFY;
    // set req is nullptr
    std::shared_ptr<IpcReq> req = nullptr;
    // set rsp is nullptr
    std::shared_ptr<IpcRsp> rsp = nullptr;
    sptr<IpcClientStub> instance = new IpcClientStub();
    // 2. call IpcClientStub SendCmd with parameter
    int ret = instance->SendCmd(cmdCode, req, rsp);
    // 3. check result is DM_IPC_FAILED
    ASSERT_EQ(ret, DM_IPC_FAILED);
}
} // namespace
} // namespace DistributedHardware
} // namespace OHOS
