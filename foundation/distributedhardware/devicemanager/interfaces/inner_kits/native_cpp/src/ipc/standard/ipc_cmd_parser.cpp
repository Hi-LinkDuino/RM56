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

#include "device_manager_notify.h"
#include "dm_constants.h"
#include "dm_device_info.h"
#include "dm_log.h"
#include "ipc_authenticate_device_req.h"
#include "ipc_cmd_register.h"
#include "ipc_def.h"
#include "ipc_get_dmfaparam_rsp.h"
#include "ipc_get_info_by_network_req.h"
#include "ipc_get_info_by_network_rsp.h"
#include "ipc_get_local_device_info_rsp.h"
#include "ipc_get_trustdevice_req.h"
#include "ipc_get_trustdevice_rsp.h"
#include "ipc_register_listener_req.h"
#include "ipc_req.h"
#include "ipc_rsp.h"
#include "ipc_set_useroperation_req.h"
#include "ipc_start_discovery_req.h"
#include "ipc_stop_discovery_req.h"
#include "ipc_unauthenticate_device_req.h"
#include "ipc_verify_authenticate_req.h"
#include "ipc_register_dev_state_callback_req.h"
#include "securec.h"

namespace OHOS {
namespace DistributedHardware {
ON_IPC_SET_REQUEST(REGISTER_DEVICE_MANAGER_LISTENER, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::shared_ptr<IpcRegisterListenerReq> pReq = std::static_pointer_cast<IpcRegisterListenerReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    sptr<IRemoteObject> listener = pReq->GetListener();
    if (!data.WriteString(pkgName)) {
        LOGE("write pkgName failed");
        return DM_IPC_TRANSACTION_FAILED;
    }
    if (!data.WriteRemoteObject(listener)) {
        LOGE("write listener failed");
        return DM_IPC_TRANSACTION_FAILED;
    }
    return DM_OK;
}

ON_IPC_READ_RESPONSE(REGISTER_DEVICE_MANAGER_LISTENER, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    if (pBaseRsp == nullptr) {
        LOGE("pBaseRsp is null");
        return DM_FAILED;
    }
    pBaseRsp->SetErrCode(reply.ReadInt32());
    return DM_OK;
}

ON_IPC_SET_REQUEST(UNREGISTER_DEVICE_MANAGER_LISTENER, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    std::string pkgName = pBaseReq->GetPkgName();
    if (!data.WriteString(pkgName)) {
        LOGE("write papam failed");
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
        LOGE("write pkg failed");
        return DM_IPC_TRANSACTION_FAILED;
    }
    if (!data.WriteString(extra)) {
        LOGE("write extra failed");
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
                LOGE("GetTrustedDeviceList read node info failed!");
                if (pRsp == nullptr) {
                    LOGE("pRsp is null");
                    return DM_FAILED;
                }
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
    if (localDeviceInfo != nullptr) {
        pRsp->SetLocalDeviceInfo(*localDeviceInfo);
    }
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
        LOGE("write pkgName failed");
        return DM_IPC_FLATTEN_OBJECT;
    }
    if (!data.WriteRawData(&dmSubscribeInfo, sizeof(DmSubscribeInfo))) {
        LOGE("write subscribe info failed");
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
        LOGE("write pkgName failed");
        return DM_IPC_FLATTEN_OBJECT;
    }
    if (!data.WriteInt16((int16_t)subscribeId)) {
        LOGE("write subscribeId failed");
        return DM_IPC_FLATTEN_OBJECT;
    }
    return DM_OK;
}

ON_IPC_READ_RESPONSE(STOP_DEVICE_DISCOVER, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    if (pBaseRsp == nullptr) {
        LOGE("pBaseRsp is null");
        return DM_FAILED;
    }
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
        LOGE("write pkgName failed");
        return DM_IPC_FLATTEN_OBJECT;
    }
    if (!data.WriteString(extra)) {
        LOGE("write extra failed");
        return DM_IPC_FLATTEN_OBJECT;
    }
    if (!data.WriteString(deviceId)) {
        LOGE("write extra failed");
        return DM_IPC_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(authType)) {
        LOGE("write pkgName failed");
        return DM_IPC_FLATTEN_OBJECT;
    }
    return DM_OK;
}

ON_IPC_READ_RESPONSE(AUTHENTICATE_DEVICE, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    if (pBaseRsp == nullptr) {
        LOGE("pBaseRsp is null");
        return DM_FAILED;
    }
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
        LOGE("write pkgName failed");
        return DM_IPC_FLATTEN_OBJECT;
    }
    if (!data.WriteString(deviceId)) {
        LOGE("write extra failed");
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
        LOGE("write pkgName failed");
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
        LOGE("write pkgName failed");
        return DM_IPC_TRANSACTION_FAILED;
    }
    if (!data.WriteInt32(action)) {
        LOGE("write action failed");
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
        LOGE("write pkgName failed");
        return DM_IPC_TRANSACTION_FAILED;
    }
    if (!data.WriteString(extra)) {
        LOGE("write extra failed");
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
        LOGE("write pkgName failed");
        return DM_IPC_TRANSACTION_FAILED;
    }
    if (!data.WriteString(extra)) {
        LOGE("write extra failed");
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
            LOGE("unknown device state:%d", deviceState);
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
        LOGE("write return failed");
        return DM_WRITE_FAILED;
    }
    return DM_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
