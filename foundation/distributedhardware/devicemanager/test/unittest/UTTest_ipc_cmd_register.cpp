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

#include "UTTest_ipc_cmd_register.h"
#include <unistd.h>
#include "ipc_client_manager.h"
#include "ipc_register_listener_req.h"
#include "ipc_get_trustdevice_rsp.h"
#include "ipc_get_info_by_network_rsp.h"
#include "ipc_get_info_by_network_req.h"
#include "ipc_get_trustdevice_req.h"
#include "ipc_start_discovery_req.h"
#include "ipc_stop_discovery_req.h"
#include "ipc_get_dmfaparam_rsp.h"
#include "ipc_register_dev_state_callback_req.h"
#include "ipc_set_useroperation_req.h"
#include "ipc_verify_authenticate_req.h"
#include "ipc_unauthenticate_device_req.h"
#include "ipc_authenticate_device_req.h"
#include "ipc_get_local_device_info_rsp.h"
#include "device_manager_notify.h"
#include "ipc_req.h"
#include "dm_device_info.h"
#include "dm_constants.h"

namespace OHOS {
namespace DistributedHardware {
void IpcCmdRegisterTest::SetUp()
{
}

void IpcCmdRegisterTest::TearDown()
{
}

void IpcCmdRegisterTest::SetUpTestCase()
{
}

void IpcCmdRegisterTest::TearDownTestCase()
{
}

namespace {
/**
 * @tc.name: SetRequest_001
 * @tc.desc: 1. set cmdCode not null
 *              set MessageParcel data null
 *              set IpcReq null
 *           2. call IpcCmdRegister OnRemoteRequest with parameter
 *           3. check ret is DM_IPC_NOT_REGISTER_FUNC
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcCmdRegisterTest, SetRequest_001, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = 999;
    //  set MessageParcel data null
    MessageParcel data;
    // set IpcReq null
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    // 2. call IpcCmdRegister OnRemoteRequest with parameter
    int ret = 0;
    ret = IpcCmdRegister::GetInstance().SetRequest(cmdCode, req, data);
    // 3. check ret is DM_IPC_NOT_REGISTER_FUNC
    ASSERT_EQ(ret, DM_IPC_NOT_REGISTER_FUNC);
}

/**
 * @tc.name: SetRequest_002
 * @tc.desc: 1. set cmdCode not null
 *              set MessageParcel data null
 *              set IpcRegisterListenerReq null
 *           2. call IpcCmdRegister OnRemoteRequest with parameter
 *           3. check ret is DM_IPC_TRANSACTION_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcCmdRegisterTest, SetRequest_002, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = REGISTER_DEVICE_MANAGER_LISTENER;
    //  set MessageParcel data null
    MessageParcel data;
    // set IpcRegisterListenerReq null
    std::shared_ptr<IpcRegisterListenerReq> req = std::make_shared<IpcRegisterListenerReq>();
    // 2. call IpcCmdRegister OnRemoteRequest with parameter
    int ret = 0;
    ret = IpcCmdRegister::GetInstance().SetRequest(cmdCode, req, data);
    // 3. check ret is DM_IPC_TRANSACTION_FAILED
    ASSERT_EQ(ret, DM_IPC_TRANSACTION_FAILED);
}

/**
 * @tc.name: SetRequest_003
 * @tc.desc: 1. set cmdCode not null
 *              set MessageParcel data null
 *              set IpcRegisterListenerReq with pkgName not null
 *           2. call IpcCmdRegister OnRemoteRequest with parameter
 *           3. check ret is DM_IPC_TRANSACTION_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcCmdRegisterTest, SetRequest_003, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = REGISTER_DEVICE_MANAGER_LISTENER;
    //  set MessageParcel data null
    MessageParcel data;
    // set IpcRegisterListenerReq null
    std::shared_ptr<IpcRegisterListenerReq> req = std::make_shared<IpcRegisterListenerReq>();
    std::string pkgName = "com.ohos.test";
    req->SetPkgName(pkgName);
    // 2. call IpcCmdRegister OnRemoteRequest with parameter
    int ret = 0;
    ret = IpcCmdRegister::GetInstance().SetRequest(cmdCode, req, data);
    // 3. check ret is DM_IPC_TRANSACTION_FAILED
    ASSERT_EQ(ret, DM_IPC_TRANSACTION_FAILED);
}

/**
 * @tc.name: SetRequest_004
 * @tc.desc: 1. set cmdCode not null
 *              set MessageParcel data null
 *              set IpcRegisterListenerReq with listener
 *           2. call IpcCmdRegister OnRemoteRequest with parameter
 *           3. check ret is DM_IPC_TRANSACTION_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcCmdRegisterTest, SetRequest_004, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = REGISTER_DEVICE_MANAGER_LISTENER;
    //  set MessageParcel data null
    MessageParcel data;
    // set IpcRegisterListenerReq null
    std::shared_ptr<IpcRegisterListenerReq> req = std::make_shared<IpcRegisterListenerReq>();
    sptr<IRemoteObject> listener = nullptr;
    std::string pkgName = "";
    req->SetPkgName(pkgName);
    req->SetListener(listener);
    // 2. call IpcCmdRegister OnRemoteRequest with parameter
    int ret = 0;
    ret = IpcCmdRegister::GetInstance().SetRequest(cmdCode, req, data);
    // 3. check ret is DM_IPC_TRANSACTION_FAILED
    ASSERT_EQ(ret, DM_IPC_TRANSACTION_FAILED);
}

/**
 * @tc.name: SetRequest_005
 * @tc.desc: 1. set cmdCode not null
 *              set MessageParcel data null
 *              set IpcRegisterListenerReq with listener
 *           2. call IpcCmdRegister OnRemoteRequest with parameter
 *           3. check ret is DM_IPC_TRANSACTION_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcCmdRegisterTest, SetRequest_005, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = REGISTER_DEVICE_MANAGER_LISTENER;
    //  set MessageParcel data null
    MessageParcel data;
    // set IpcRegisterListenerReq null
    std::shared_ptr<IpcRegisterListenerReq> req = std::make_shared<IpcRegisterListenerReq>();
    sptr<IRemoteObject> listener = nullptr;
    std::string pkgName = "listen not null";
    req->SetPkgName(pkgName);
    req->SetListener(listener);
    // 2. call IpcCmdRegister OnRemoteRequest with parameter
    int ret = 0;
    ret = IpcCmdRegister::GetInstance().SetRequest(cmdCode, req, data);
    // 3. check ret is DM_IPC_TRANSACTION_FAILED
    ASSERT_EQ(ret, DM_IPC_TRANSACTION_FAILED);
}

/**
 * @tc.name: ReadResponse_001
 * @tc.desc: 1. set cmdCode 9999
 *              set MessageParcel reply null
 *           2. call IpcCmdRegister ReadResponse with parameter
 *           3. check ret is DM_IPC_NOT_REGISTER_FUNC
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcCmdRegisterTest, ReadResponse_001, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = 9999;
    //  set MessageParcel data null
    MessageParcel reply;
    // set IpcRegisterListenerReq null
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    // 2. call IpcCmdRegister ReadResponse with parameter
    int ret = 0;
    ret = IpcCmdRegister::GetInstance().ReadResponse(cmdCode, reply, rsp);
    // 3. check ret is DM_IPC_NOT_REGISTER_FUNC
    ASSERT_EQ(ret, DM_IPC_NOT_REGISTER_FUNC);
}

/**
 * @tc.name: ReadResponse_002
 * @tc.desc: 1. set cmdCode REGISTER_DEVICE_MANAGER_LISTENER
 *              set MessageParcel reply null
 *              set IpcRsp null
 *           2. call IpcCmdRegister ReadResponse with parameter
 *           3. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcCmdRegisterTest, ReadResponse_002, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = REGISTER_DEVICE_MANAGER_LISTENER;
    //  set MessageParcel data null
    MessageParcel reply;
    // set IpcRegisterListenerReq null
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    // 2. call IpcCmdRegister OnRemoteRequest with parameter
    int ret = 0;
    ret = IpcCmdRegister::GetInstance().ReadResponse(cmdCode, reply, rsp);;
    // 3. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: ReadResponse_003
 * @tc.desc: 1. set cmdCode UNREGISTER_DEVICE_MANAGER_LISTENER
 *              set MessageParcel reply null
 *              set IpcRsp null
 *           2. call IpcCmdRegister ReadResponse with parameter
 *           3. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcCmdRegisterTest, ReadResponse_003, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = UNREGISTER_DEVICE_MANAGER_LISTENER;
    //  set MessageParcel data null
    MessageParcel reply;
    // set IpcRegisterListenerReq null
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    // 2. call IpcCmdRegister OnRemoteRequest with parameter
    int ret = 0;
    ret = IpcCmdRegister::GetInstance().ReadResponse(cmdCode, reply, rsp);;
    // 3. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: ReadResponse_004
 * @tc.desc: 1. set cmdCode GET_TRUST_DEVICE_LIST
 *              set MessageParcel reply null
 *              set IpcRsp null
 *           2. call IpcCmdRegister ReadResponse with parameter
 *           3. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcCmdRegisterTest, ReadResponse_004, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = GET_TRUST_DEVICE_LIST;
    //  set MessageParcel data null
    MessageParcel reply;
    // set IpcRegisterListenerReq null
    std::shared_ptr<IpcGetTrustDeviceRsp> rsp = std::make_shared<IpcGetTrustDeviceRsp>();
    // 2. call IpcCmdRegister OnRemoteRequest with parameter
    int ret = 0;
    ret = IpcCmdRegister::GetInstance().ReadResponse(cmdCode, reply, rsp);;
    // 3. check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: ReadResponse_005
 * @tc.desc: 1. set cmdCode GET_TRUST_DEVICE_LIST
 *              set MessageParcel reply null
 *              set IpcRsp null
 *           2. call IpcCmdRegister ReadResponse with parameter
 *           3. check ret is DM_IPC_TRANSACTION_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcCmdRegisterTest, ReadResponse_005, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = GET_TRUST_DEVICE_LIST;
    //  set MessageParcel data null
    MessageParcel reply;
    reply.WriteInt32(10);
    int32_t deviceTotalSize = 10 * (int32_t)sizeof(DmDeviceInfo);
    DmDeviceInfo *dmDeviceInfo = nullptr;
    reply.WriteRawData(dmDeviceInfo, deviceTotalSize);
    // set IpcRegisterListenerReq null
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    // 2. call IpcCmdRegister OnRemoteRequest with parameter
    int ret = 0;
    ret = IpcCmdRegister::GetInstance().ReadResponse(cmdCode, reply, rsp);;
    // 3. check ret is DM_IPC_TRANSACTION_FAILED
    ASSERT_EQ(ret, DM_IPC_TRANSACTION_FAILED);
}

/**
 * @tc.name: OnIpcCmd_001
 * @tc.desc: 1. set cmdCode 9999
 *              set MessageParcel reply null
 *           2. call IpcCmdRegister OnIpcCmd with parameter
 *           3. check ret is DEVICEMANAGER_IPC_NOT_REGISTER_FUNC
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcCmdRegisterTest, OnIpcCmd_001, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode not null
    int32_t cmdCode = 9999;
    //  set MessageParcel data null
    MessageParcel reply;
    MessageParcel data;
    // set IpcRegisterListenerReq null
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    // 2. call IpcCmdRegister ReadResponse with parameter
    int ret = 0;
    ret = IpcCmdRegister::GetInstance().OnIpcCmd(cmdCode, data, reply);
    // 3. check ret is DM_IPC_NOT_REGISTER_FUNC
    ASSERT_EQ(ret, DM_IPC_NOT_REGISTER_FUNC);
}

/**
 * @tc.name: OnIpcCmd_002
 * @tc.desc: 1. set cmdCode SERVER_DEVICE_STATE_NOTIFY
 *           2. data.WriteString(pkgname)
 *              data.WriteInt32(DEVICE_STATE_ONLINE)
 *              data.WriteRawData(&dmDeviceInfo, deviceSize)
 *           3. call IpcCmdRegister OnIpcCmd with parameter
 *           4. check ret is DM_OK
 *              check result is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcCmdRegisterTest, OnIpcCmd_002, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode SERVER_DEVICE_STATE_NOTIFY
    int32_t cmdCode = SERVER_DEVICE_STATE_NOTIFY;
    MessageParcel reply;
    MessageParcel data;
    // 2. data.WriteString(pkgname)
    data.WriteString("com.ohos.test");
    // data.WriteInt32(DEVICE_STATE_ONLINE)
    data.WriteInt32(DEVICE_STATE_ONLINE);
    DmDeviceInfo dmDeviceInfo;
    size_t deviceSize = sizeof(DmDeviceInfo);
    // data.WriteRawData(&dmDeviceInfo, deviceSize)
    data.WriteRawData(&dmDeviceInfo, deviceSize);
    // 3. call IpcCmdRegister OnIpcCmd with parameter
    int ret = 0;
    int result = 0;
    ret = IpcCmdRegister::GetInstance().OnIpcCmd(cmdCode, data, reply);
    result = reply.ReadInt32();
    // 4. check result is DM_OK
    // check ret is DEVICEMANAGER_IPC_NOT_REGISTER_FUNC
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: OnIpcCmd_003
 * @tc.desc: 1. set cmdCode SERVER_DEVICE_STATE_NOTIFY
 *           2. data.WriteString(pkgname)
 *              data.WriteInt32(DEVICE_STATE_ONLINE)
 *              data.WriteRawData(nullptr, deviceSize)
 *           3. call IpcCmdRegister OnIpcCmd with parameter
 *           4. check ret is DEVICEMANAGER_OK
 *              check result is DEVICE_STATE_ONLINE
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcCmdRegisterTest, OnIpcCmd_003, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode SERVER_DEVICE_STATE_NOTIFY
    int32_t cmdCode = SERVER_DEVICE_STATE_NOTIFY;
    MessageParcel reply;
    MessageParcel data;
    // 2. data.WriteString(pkgname)
    data.WriteString("com.ohos.test");
    // data.WriteInt32(DEVICE_STATE_ONLINE)
    data.WriteInt32(DEVICE_STATE_ONLINE);
    // data.WriteRawData(nullptr, deviceSize)
    size_t deviceSize = sizeof(DmDeviceInfo);
    data.WriteRawData(nullptr, deviceSize);
    // 3. call IpcCmdRegister OnIpcCmd with parameter
    int ret = 0;
    int result = 0;
    ret = IpcCmdRegister::GetInstance().OnIpcCmd(cmdCode, data, reply);
    result = reply.ReadInt32();
    // 4. check result is DEVICE_STATE_ONLINE
    ASSERT_EQ(result, DEVICE_STATE_ONLINE);
    // check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: OnIpcCmd_004
 * @tc.desc: 1. set cmdCode SERVER_DEVICE_STATE_NOTIFY
 *           2. data.WriteString(pkgname)
 *              data.WriteInt32(DEVICE_STATE_OFFLINE)
 *              data.WriteRawData(nullptr, deviceSize)
 *           3. call IpcCmdRegister OnIpcCmd with parameter
 *           4. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcCmdRegisterTest, OnIpcCmd_004, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode SERVER_DEVICE_STATE_NOTIFY
    int32_t cmdCode = SERVER_DEVICE_STATE_NOTIFY;
    MessageParcel reply;
    MessageParcel data;
    // 2. data.WriteString(pkgname)
    data.WriteString("com.ohos.test");
    // data.WriteInt32(DEVICE_STATE_OFFLINE)
    data.WriteInt32(DEVICE_STATE_OFFLINE);
    size_t deviceSize = sizeof(DmDeviceInfo);
    // data.WriteRawData(nullptr, deviceSize)
    data.WriteRawData(nullptr, deviceSize);
    // 3. call IpcCmdRegister OnIpcCmd with parameter
    int ret = 0;
    int result = 0;
    ret = IpcCmdRegister::GetInstance().OnIpcCmd(cmdCode, data, reply);
    result = reply.ReadInt32();
    // check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: OnIpcCmd_005
 * @tc.desc: 1. set cmdCode SERVER_DEVICE_STATE_NOTIFY
 *           2. data.WriteString(pkgname)
 *              data.WriteInt32(DEVICE_INFO_CHANGED)
 *              data.WriteRawData(nullptr, deviceSize)
 *           3. call IpcCmdRegister OnIpcCmd with parameter
 *           4. check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(IpcCmdRegisterTest, OnIpcCmd_005, testing::ext::TestSize.Level0)
{
    // 1. set cmdCode SERVER_DEVICE_STATE_NOTIFY
    int32_t cmdCode = SERVER_DEVICE_STATE_NOTIFY;
    MessageParcel reply;
    MessageParcel data;
    // 2. data.WriteString(pkgname)
    data.WriteString("com.ohos.test");
    // data.WriteInt32(DEVICE_INFO_CHANGED)
    data.WriteInt32(DEVICE_INFO_CHANGED);
    size_t deviceSize = sizeof(DmDeviceInfo);
    // data.WriteRawData(nullptr, deviceSize)
    data.WriteRawData(nullptr, deviceSize);
    // 3. call IpcCmdRegister OnIpcCmd with parameter
    int ret = 0;
    int result = 0;
    ret = IpcCmdRegister::GetInstance().OnIpcCmd(cmdCode, data, reply);
    result = reply.ReadInt32();
    // 4.check ret is DM_OK
    ASSERT_EQ(ret, DM_OK);
}
} // namespace
ON_IPC_SET_REQUEST(REGISTER_DEVICE_MANAGER_LISTENER, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::shared_ptr<IpcRegisterListenerReq> pReq = std::static_pointer_cast<IpcRegisterListenerReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    sptr<IRemoteObject> listener = pReq->GetListener();
    if (!data.WriteString(pkgName)) {
        return DM_IPC_TRANSACTION_FAILED;
    }
    if (!data.WriteRemoteObject(listener)) {
        return DM_IPC_TRANSACTION_FAILED;
    }
    return DM_OK;
}

ON_IPC_READ_RESPONSE(REGISTER_DEVICE_MANAGER_LISTENER, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(reply.ReadInt32());
    return DM_OK;
}

ON_IPC_SET_REQUEST(UNREGISTER_DEVICE_MANAGER_LISTENER, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::string pkgName = pBaseReq->GetPkgName();
    if (!data.WriteString(pkgName)) {
        return DM_IPC_TRANSACTION_FAILED;
    }
    return DM_OK;
}

ON_IPC_READ_RESPONSE(UNREGISTER_DEVICE_MANAGER_LISTENER, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(reply.ReadInt32());
    return DM_OK;
}

ON_IPC_SET_REQUEST(GET_TRUST_DEVICE_LIST, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::shared_ptr<IpcGetTrustDeviceReq> pReq = std::static_pointer_cast<IpcGetTrustDeviceReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    std::string extra = pReq->GetExtra();
    if (!data.WriteString(pkgName)) {
        return DM_IPC_TRANSACTION_FAILED;
    }
    if (!data.WriteString(extra)) {
        return DM_IPC_TRANSACTION_FAILED;
    }
    return DM_OK;
}

ON_IPC_READ_RESPONSE(GET_TRUST_DEVICE_LIST, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    std::shared_ptr<IpcGetTrustDeviceRsp> pRsp = std::static_pointer_cast<IpcGetTrustDeviceRsp>(pBaseRsp);
    int32_t deviceNum = reply.ReadInt32();
    if (deviceNum > 0) {
        std::vector<DmDeviceInfo> deviceInfoVec;
        DmDeviceInfo *pDmDeviceinfo = nullptr;
        for (int32_t i = 0; i < deviceNum; ++i) {
            pDmDeviceinfo = nullptr;
            pDmDeviceinfo = (DmDeviceInfo *)reply.ReadRawData(sizeof(DmDeviceInfo));
            if (pDmDeviceinfo == nullptr) {
                pRsp->SetErrCode(DM_IPC_TRANSACTION_FAILED);
                return DM_IPC_TRANSACTION_FAILED;
            }
            deviceInfoVec.emplace_back(*pDmDeviceinfo);
        }
        pRsp->SetDeviceVec(deviceInfoVec);
    }
    pRsp->SetErrCode(reply.ReadInt32());
    return DM_OK;
}

ON_IPC_SET_REQUEST(GET_LOCAL_DEVICE_INFO, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::shared_ptr<IpcReq> pReq = std::static_pointer_cast<IpcReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    if (!data.WriteString(pkgName)) {
        return DM_FLATTEN_OBJECT;
    }
    return DM_OK;
}

ON_IPC_READ_RESPONSE(GET_LOCAL_DEVICE_INFO, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    std::shared_ptr<IpcGetLocalDeviceInfoRsp> pRsp = std::static_pointer_cast<IpcGetLocalDeviceInfoRsp>(pBaseRsp);
    DmDeviceInfo *localDeviceInfo = (DmDeviceInfo *)reply.ReadRawData(sizeof(DmDeviceInfo));
    if (localDeviceInfo == nullptr) {
    }
    pRsp->SetLocalDeviceInfo(*localDeviceInfo);
    pRsp->SetErrCode(reply.ReadInt32());
    return DM_OK;
}

ON_IPC_SET_REQUEST(GET_UDID_BY_NETWORK, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::shared_ptr<IpcGetInfoByNetWorkReq> pReq = std::static_pointer_cast<IpcGetInfoByNetWorkReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    std::string netWorkId = pReq->GetNetWorkId();
    if (!data.WriteString(pkgName)) {
        return DM_FLATTEN_OBJECT;
    }
    if (!data.WriteString(netWorkId)) {
        return DM_FLATTEN_OBJECT;
    }
    return DM_OK;
}

ON_IPC_READ_RESPONSE(GET_UDID_BY_NETWORK, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    std::shared_ptr<IpcGetInfoByNetWorkRsp> pRsp = std::static_pointer_cast<IpcGetInfoByNetWorkRsp>(pBaseRsp);
    pRsp->SetErrCode(reply.ReadInt32());
    pRsp->SetUdid(reply.ReadString());
    return DM_OK;
}

ON_IPC_SET_REQUEST(GET_UUID_BY_NETWORK, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::shared_ptr<IpcGetInfoByNetWorkReq> pReq = std::static_pointer_cast<IpcGetInfoByNetWorkReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    std::string netWorkId = pReq->GetNetWorkId();
    if (!data.WriteString(pkgName)) {
        return DM_FLATTEN_OBJECT;
    }
    if (!data.WriteString(netWorkId)) {
        return DM_FLATTEN_OBJECT;
    }
    return DM_OK;
}

ON_IPC_READ_RESPONSE(GET_UUID_BY_NETWORK, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    std::shared_ptr<IpcGetInfoByNetWorkRsp> pRsp = std::static_pointer_cast<IpcGetInfoByNetWorkRsp>(pBaseRsp);
    pRsp->SetErrCode(reply.ReadInt32());
    pRsp->SetUuid(reply.ReadString());
    return DM_OK;
}

ON_IPC_SET_REQUEST(START_DEVICE_DISCOVER, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::shared_ptr<IpcStartDiscoveryReq> pReq = std::static_pointer_cast<IpcStartDiscoveryReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    const DmSubscribeInfo dmSubscribeInfo = pReq->GetSubscribeInfo();
    if (!data.WriteString(pkgName)) {
        return DM_IPC_FLATTEN_OBJECT;
    }
    if (!data.WriteRawData(&dmSubscribeInfo, sizeof(DmSubscribeInfo))) {
        return DM_IPC_FLATTEN_OBJECT;
    }
    return DM_OK;
}

ON_IPC_READ_RESPONSE(START_DEVICE_DISCOVER, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(reply.ReadInt32());
    return DM_OK;
}

ON_IPC_SET_REQUEST(STOP_DEVICE_DISCOVER, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::shared_ptr<IpcStopDiscoveryReq> pReq = std::static_pointer_cast<IpcStopDiscoveryReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    uint16_t subscribeId = pReq->GetSubscribeId();
    if (!data.WriteString(pkgName)) {
        return DM_IPC_FLATTEN_OBJECT;
    }
    if (!data.WriteInt16((int16_t)subscribeId)) {
        return DM_IPC_FLATTEN_OBJECT;
    }
    return DM_OK;
}

ON_IPC_READ_RESPONSE(STOP_DEVICE_DISCOVER, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(reply.ReadInt32());
    return DM_OK;
}

ON_IPC_SET_REQUEST(AUTHENTICATE_DEVICE, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::shared_ptr<IpcAuthenticateDeviceReq> pReq = std::static_pointer_cast<IpcAuthenticateDeviceReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    std::string extra = pReq->GetExtra();
    int32_t authType = pReq->GetAuthType();
    DmDeviceInfo deviceInfo = pReq->GetDeviceInfo();
    std::string deviceId = deviceInfo.deviceId;

    if (!data.WriteString(pkgName)) {
        return DM_IPC_FLATTEN_OBJECT;
    }
    if (!data.WriteString(extra)) {
        return DM_IPC_FLATTEN_OBJECT;
    }
    if (!data.WriteString(deviceId)) {
        return DM_IPC_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(authType)) {
        return DM_IPC_FLATTEN_OBJECT;
    }
    return DM_OK;
}

ON_IPC_READ_RESPONSE(AUTHENTICATE_DEVICE, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(reply.ReadInt32());
    return DM_OK;
}

ON_IPC_SET_REQUEST(UNAUTHENTICATE_DEVICE, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::shared_ptr<IpcUnAuthenticateDeviceReq> pReq = std::static_pointer_cast<IpcUnAuthenticateDeviceReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    DmDeviceInfo deviceInfo = pReq->GetDeviceInfo();
    std::string deviceId = deviceInfo.deviceId;
    if (!data.WriteString(pkgName)) {
        return DM_IPC_FLATTEN_OBJECT;
    }
    if (!data.WriteString(deviceId)) {
        return DM_IPC_FLATTEN_OBJECT;
    }
    return DM_OK;
}

ON_IPC_READ_RESPONSE(UNAUTHENTICATE_DEVICE, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(reply.ReadInt32());
    return DM_OK;
}

ON_IPC_SET_REQUEST(VERIFY_AUTHENTICATION, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::shared_ptr<IpcVerifyAuthenticateReq> pReq = std::static_pointer_cast<IpcVerifyAuthenticateReq>(pBaseReq);
    std::string authPara = pReq->GetAuthPara();
    if (!data.WriteString(authPara)) {
        return DM_IPC_FLATTEN_OBJECT;
    }
    return DM_OK;
}

ON_IPC_READ_RESPONSE(VERIFY_AUTHENTICATION, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(reply.ReadInt32());
    return DM_OK;
}

ON_IPC_SET_REQUEST(SERVER_GET_DMFA_INFO, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::shared_ptr<IpcReq> pReq = std::static_pointer_cast<IpcReq>(pBaseReq);
    std::string packagename = pReq->GetPkgName();
    if (!data.WriteString(packagename)) {
        return DM_IPC_FLATTEN_OBJECT;
    }
    return DM_OK;
}

ON_IPC_READ_RESPONSE(SERVER_GET_DMFA_INFO, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    std::shared_ptr<IpcGetDmFaParamRsp> pRsp = std::static_pointer_cast<IpcGetDmFaParamRsp>(pBaseRsp);
    DmAuthParam authParam;
    authParam.direction = reply.ReadInt32();
    authParam.authType = reply.ReadInt32();
    authParam.authToken = reply.ReadString();
    authParam.packageName = reply.ReadString();
    authParam.appName = reply.ReadString();
    authParam.appDescription = reply.ReadString();
    authParam.business = reply.ReadInt32();
    authParam.pincode = reply.ReadInt32();
    pRsp->SetDmAuthParam(authParam);
    return DM_OK;
}

ON_IPC_SET_REQUEST(SERVER_USER_AUTH_OPERATION, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::shared_ptr<IpcGetOperationReq> pReq = std::static_pointer_cast<IpcGetOperationReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    int32_t action = pReq->GetOperation();

    if (!data.WriteString(pkgName)) {
        return DM_IPC_TRANSACTION_FAILED;
    }
    if (!data.WriteInt32(action)) {
        return DM_WRITE_FAILED;
    }

    return DM_OK;
}

ON_IPC_READ_RESPONSE(SERVER_USER_AUTH_OPERATION, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(reply.ReadInt32());
    return DM_OK;
}

ON_IPC_SET_REQUEST(REGISTER_DEV_STATE_CALLBACK, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::shared_ptr<IpcRegisterDevStateCallbackReq> pReq =
    std::static_pointer_cast<IpcRegisterDevStateCallbackReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    std::string extra = pReq->GetExtra();

    if (!data.WriteString(pkgName)) {
        return DM_IPC_TRANSACTION_FAILED;
    }
    if (!data.WriteString(extra)) {
        return DM_WRITE_FAILED;
    }

    return DM_OK;
}

ON_IPC_READ_RESPONSE(REGISTER_DEV_STATE_CALLBACK, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(reply.ReadInt32());
    return DM_OK;
}

ON_IPC_SET_REQUEST(UNREGISTER_DEV_STATE_CALLBACK, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::shared_ptr<IpcRegisterDevStateCallbackReq> pReq =
    std::static_pointer_cast<IpcRegisterDevStateCallbackReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    std::string extra = pReq->GetExtra();

    if (!data.WriteString(pkgName)) {
        return DM_IPC_TRANSACTION_FAILED;
    }
    if (!data.WriteString(extra)) {
        return DM_WRITE_FAILED;
    }

    return DM_OK;
}

ON_IPC_READ_RESPONSE(UNREGISTER_DEV_STATE_CALLBACK, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(reply.ReadInt32());
    return DM_OK;
}

ON_IPC_CMD(SERVER_DEVICE_STATE_NOTIFY, MessageParcel &data, MessageParcel &reply)
{
    std::string pkgName = data.ReadString();
    DmDeviceState deviceState = static_cast<DmDeviceState>(data.ReadInt32());
    DmDeviceInfo dmDeviceInfo;
    size_t deviceSize = sizeof(DmDeviceInfo);
    void *deviceInfo = (void *)data.ReadRawData(deviceSize);
    if (deviceInfo != nullptr && memcpy_s(&dmDeviceInfo, deviceSize, deviceInfo, deviceSize) != 0) {
        reply.WriteInt32(DM_IPC_COPY_FAILED);
        return DM_OK;
    }
    switch (deviceState) {
        case DEVICE_STATE_ONLINE:
            DeviceManagerNotify::GetInstance().OnDeviceOnline(pkgName, dmDeviceInfo);
            break;
        case DEVICE_STATE_OFFLINE:
            DeviceManagerNotify::GetInstance().OnDeviceOffline(pkgName, dmDeviceInfo);
            break;
        case DEVICE_INFO_CHANGED:
            DeviceManagerNotify::GetInstance().OnDeviceChanged(pkgName, dmDeviceInfo);
            break;
        case DEVICE_INFO_READY:
            DeviceManagerNotify::GetInstance().OnDeviceReady(pkgName, dmDeviceInfo);
            break;
        default:
            break;
    }
    reply.WriteInt32(DM_OK);
    return DM_OK;
}

ON_IPC_CMD(SERVER_DEVICE_FOUND, MessageParcel &data, MessageParcel &reply)
{
    std::string pkgName = data.ReadString();
    int16_t subscribeId = data.ReadInt16();
    DmDeviceInfo dmDeviceInfo;
    size_t deviceSize = sizeof(DmDeviceInfo);
    void *deviceInfo = (void *)data.ReadRawData(deviceSize);
    if (deviceInfo != nullptr && memcpy_s(&dmDeviceInfo, deviceSize, deviceInfo, deviceSize) != 0) {
        reply.WriteInt32(DM_IPC_COPY_FAILED);
        return DM_IPC_COPY_FAILED;
    }
    DeviceManagerNotify::GetInstance().OnDeviceFound(pkgName, subscribeId, dmDeviceInfo);
    reply.WriteInt32(DM_OK);
    return DM_OK;
}

ON_IPC_CMD(SERVER_DISCOVER_FINISH, MessageParcel &data, MessageParcel &reply)
{
    std::string pkgName = data.ReadString();
    int16_t subscribeId = data.ReadInt16();
    int32_t failedReason = data.ReadInt32();

    if (failedReason == DM_OK) {
        DeviceManagerNotify::GetInstance().OnDiscoverySuccess(pkgName, subscribeId);
    } else {
        DeviceManagerNotify::GetInstance().OnDiscoveryFailed(pkgName, subscribeId, failedReason);
    }
    reply.WriteInt32(DM_OK);
    return DM_OK;
}

ON_IPC_CMD(SERVER_AUTH_RESULT, MessageParcel &data, MessageParcel &reply)
{
    std::string pkgName = data.ReadString();
    std::string deviceId = data.ReadString();
    std::string token = data.ReadString();
    int32_t status = data.ReadInt32();
    int32_t reason = data.ReadInt32();
    DeviceManagerNotify::GetInstance().OnAuthResult(pkgName, deviceId, token, (uint32_t)status, reason);
    reply.WriteInt32(DM_OK);
    return DM_OK;
}

ON_IPC_CMD(SERVER_VERIFY_AUTH_RESULT, MessageParcel &data, MessageParcel &reply)
{
    std::string pkgName = data.ReadString();
    std::string deviceId = data.ReadString();
    int32_t resultCode = data.ReadInt32();
    int32_t flag = data.ReadInt32();
    DeviceManagerNotify::GetInstance().OnVerifyAuthResult(pkgName, deviceId, resultCode, flag);
    reply.WriteInt32(DM_OK);
    return DM_OK;
}

ON_IPC_CMD(SERVER_DEVICE_FA_NOTIFY, MessageParcel &data, MessageParcel &reply)
{
    std::string packagename = data.ReadString();
    std::string paramJson = data.ReadString();
    DeviceManagerNotify::GetInstance().OnFaCall(packagename, paramJson);
    if (!reply.WriteInt32(DM_OK)) {
        return DM_WRITE_FAILED;
    }
    return DM_OK;
}
} // namespace DistributedHardware
} // namespace OHOS