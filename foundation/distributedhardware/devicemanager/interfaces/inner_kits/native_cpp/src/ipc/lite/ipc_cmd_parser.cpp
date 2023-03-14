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
#include "dm_subscribe_info.h"
#include "ipc_authenticate_device_req.h"
#include "ipc_cmd_register.h"
#include "ipc_def.h"
#include "ipc_get_local_device_info_rsp.h"
#include "ipc_get_trustdevice_req.h"
#include "ipc_get_trustdevice_rsp.h"
#include "ipc_register_listener_req.h"
#include "ipc_start_discovery_req.h"
#include "ipc_stop_discovery_req.h"
#include "ipc_verify_authenticate_req.h"
#include "ipc_set_useroperation_req.h"
#include "securec.h"

namespace OHOS {
namespace DistributedHardware {
ON_IPC_SET_REQUEST(REGISTER_DEVICE_MANAGER_LISTENER, std::shared_ptr<IpcReq> pBaseReq, IpcIo &request, uint8_t *buffer,
                   size_t buffLen)
{
    std::shared_ptr<IpcRegisterListenerReq> pReq = std::static_pointer_cast<IpcRegisterListenerReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    SvcIdentity svcIdentity = pReq->GetSvcIdentity();

    IpcIoInit(&request, buffer, buffLen, 1);
    IpcIoPushString(&request, pkgName.c_str());
    IpcIoPushSvc(&request, &svcIdentity);
    return DM_OK;
}

ON_IPC_READ_RESPONSE(REGISTER_DEVICE_MANAGER_LISTENER, IpcIo &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    if (pBaseRsp == nullptr) {
        LOGE("pBaseRsp is null");
        return DM_FAILED;
    }
    pBaseRsp->SetErrCode(IpcIoPopInt32(&reply));
    return DM_OK;
}

ON_IPC_SET_REQUEST(UNREGISTER_DEVICE_MANAGER_LISTENER, std::shared_ptr<IpcReq> pBaseReq, IpcIo &request,
                   uint8_t *buffer, size_t buffLen)
{
    std::string pkgName = pBaseReq->GetPkgName();

    IpcIoInit(&request, buffer, buffLen, 0);
    IpcIoPushString(&request, pkgName.c_str());
    return DM_OK;
}

ON_IPC_READ_RESPONSE(UNREGISTER_DEVICE_MANAGER_LISTENER, IpcIo &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(IpcIoPopInt32(&reply));
    return DM_OK;
}

ON_IPC_SET_REQUEST(GET_TRUST_DEVICE_LIST, std::shared_ptr<IpcReq> pBaseReq, IpcIo &request, uint8_t *buffer,
                   size_t buffLen)
{
    std::shared_ptr<IpcGetTrustDeviceReq> pReq = std::static_pointer_cast<IpcGetTrustDeviceReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    std::string extra = pReq->GetExtra();

    IpcIoInit(&request, buffer, buffLen, 0);
    IpcIoPushString(&request, pkgName.c_str());
    IpcIoPushString(&request, extra.c_str());
    return DM_OK;
}

ON_IPC_READ_RESPONSE(GET_TRUST_DEVICE_LIST, IpcIo &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    std::shared_ptr<IpcGetTrustDeviceRsp> pRsp = std::static_pointer_cast<IpcGetTrustDeviceRsp>(pBaseRsp);
    int32_t deviceNum = IpcIoPopInt32(&reply);
    uint32_t deviceTotalSize = deviceNum * (int32_t)sizeof(DmDeviceInfo);

    if (deviceTotalSize > 0) {
        std::vector<DmDeviceInfo> deviceInfoVec;
        DmDeviceInfo *pDmDeviceinfo = (DmDeviceInfo *)IpcIoPopFlatObj(&reply, &deviceTotalSize);
        if (pDmDeviceinfo == nullptr) {
            LOGE("GetTrustedDeviceList read node info failed!");
            pRsp->SetErrCode(DM_IPC_TRANSACTION_FAILED);
            return DM_IPC_TRANSACTION_FAILED;
        }
        for (int32_t i = 0; i < deviceNum; ++i) {
            deviceInfoVec.emplace_back(*pDmDeviceinfo);
            pDmDeviceinfo = ++pDmDeviceinfo;
        }
        pRsp->SetDeviceVec(deviceInfoVec);
    }
    pRsp->SetErrCode(IpcIoPopInt32(&reply));
    return DM_OK;
}

ON_IPC_SET_REQUEST(GET_LOCAL_DEVICE_INFO, std::shared_ptr<IpcReq> pBaseReq, IpcIo &request, uint8_t *buffer,
                   size_t buffLen)
{
    IpcIoInit(&request, buffer, buffLen, 0);
    return DM_OK;
}

ON_IPC_READ_RESPONSE(GET_LOCAL_DEVICE_INFO, IpcIo &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    std::shared_ptr<IpcGetLocalDeviceInfoRsp> pRsp = std::static_pointer_cast<IpcGetLocalDeviceInfoRsp>(pBaseRsp);

    uint32_t size = 0;
    DmDeviceInfo *dmDeviceInfo = (DmDeviceInfo *)IpcIoPopFlatObj(&reply, &size);
    if (dmDeviceInfo != nullptr) {
        pRsp->SetLocalDeviceInfo(*dmDeviceInfo);
    }
    pRsp->SetErrCode(IpcIoPopInt32(&reply));
    return DM_OK;
}

ON_IPC_SET_REQUEST(START_DEVICE_DISCOVER, std::shared_ptr<IpcReq> pBaseReq, IpcIo &request, uint8_t *buffer,
                   size_t buffLen)
{
    std::shared_ptr<IpcStartDiscoveryReq> pReq = std::static_pointer_cast<IpcStartDiscoveryReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    std::string extra = pReq->GetExtra();
    const DmSubscribeInfo dmSubscribeInfo = pReq->GetSubscribeInfo();

    IpcIoInit(&request, buffer, buffLen, 0);
    IpcIoPushString(&request, pkgName.c_str());
    IpcIoPushString(&request, extra.c_str());
    IpcIoPushFlatObj(&request, &dmSubscribeInfo, sizeof(DmSubscribeInfo));
    return DM_OK;
}

ON_IPC_READ_RESPONSE(START_DEVICE_DISCOVER, IpcIo &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(IpcIoPopInt32(&reply));
    return DM_OK;
}

ON_IPC_SET_REQUEST(STOP_DEVICE_DISCOVER, std::shared_ptr<IpcReq> pBaseReq, IpcIo &request, uint8_t *buffer,
                   size_t buffLen)
{
    std::shared_ptr<IpcStopDiscoveryReq> pReq = std::static_pointer_cast<IpcStopDiscoveryReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    uint16_t subscribeId = pReq->GetSubscribeId();

    IpcIoInit(&request, buffer, buffLen, 0);
    IpcIoPushString(&request, pkgName.c_str());
    IpcIoPushUint16(&request, subscribeId);
    return DM_OK;
}

ON_IPC_READ_RESPONSE(STOP_DEVICE_DISCOVER, IpcIo &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(IpcIoPopInt32(&reply));
    return DM_OK;
}

ON_IPC_SET_REQUEST(AUTHENTICATE_DEVICE, std::shared_ptr<IpcReq> pBaseReq, IpcIo &request, uint8_t *buffer,
                   size_t buffLen)
{
    std::shared_ptr<IpcAuthenticateDeviceReq> pReq = std::static_pointer_cast<IpcAuthenticateDeviceReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    std::string extra = pReq->GetExtra();
    DmDeviceInfo deviceInfo = pReq->GetDeviceInfo();

    IpcIoInit(&request, buffer, buffLen, 0);
    IpcIoPushString(&request, pkgName.c_str());
    IpcIoPushString(&request, extra.c_str());
    IpcIoPushFlatObj(&request, &deviceInfo, sizeof(DmDeviceInfo));
    // L1 暂时没有考虑appimage校验（8k限制）
    return DM_OK;
}

ON_IPC_READ_RESPONSE(AUTHENTICATE_DEVICE, IpcIo &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(IpcIoPopInt32(&reply));
    return DM_OK;
}

ON_IPC_SET_REQUEST(VERIFY_AUTHENTICATION, std::shared_ptr<IpcReq> pBaseReq, IpcIo &request, uint8_t *buffer,
                   size_t buffLen)
{
    std::shared_ptr<IpcVerifyAuthenticateReq> pReq = std::static_pointer_cast<IpcVerifyAuthenticateReq>(pBaseReq);
    std::string authPara = pReq->GetAuthPara();

    IpcIoInit(&request, buffer, buffLen, 0);
    IpcIoPushString(&request, authPara.c_str());
    return DM_OK;
}

ON_IPC_READ_RESPONSE(VERIFY_AUTHENTICATION, IpcIo &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(IpcIoPopInt32(&reply));
    return DM_OK;
}

ON_IPC_SET_REQUEST(SERVER_USER_AUTH_OPERATION, std::shared_ptr<IpcReq> pBaseReq, IpcIo &request, uint8_t *buffer,
                   size_t buffLen)
{
    std::shared_ptr<IpcGetOperationReq> pReq = std::static_pointer_cast<IpcGetOperationReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    int32_t action = pReq->GetOperation();

    IpcIoInit(&request, buffer, buffLen, 0);
    IpcIoPushString(&request, pkgName.c_str());
    IpcIoPushInt32(&request, action);
    return DM_OK;
}

ON_IPC_READ_RESPONSE(SERVER_USER_AUTH_OPERATION, IpcIo &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(IpcIoPopInt32(&reply));
    return DM_OK;
}

ON_IPC_CMD(SERVER_DEVICE_STATE_NOTIFY, IpcIo &reply)
{
    size_t len = 0;
    std::string pkgName = (const char *)IpcIoPopString(&reply, &len);
    DmDeviceState deviceState = static_cast<DmDeviceState>(IpcIoPopInt32(&reply));
    uint32_t size;
    const DmDeviceInfo *deviceInfo = (const DmDeviceInfo *)IpcIoPopFlatObj(&reply, &size);
    if (pkgName == "" || len == 0 || deviceInfo == nullptr) {
        LOGE("OnDeviceOnline, get para failed");
        return;
    }
    switch (deviceState) {
        case DEVICE_STATE_ONLINE:
            DeviceManagerNotify::GetInstance().OnDeviceOnline(pkgName, *deviceInfo);
            break;
        case DEVICE_STATE_OFFLINE:
            DeviceManagerNotify::GetInstance().OnDeviceOffline(pkgName, *deviceInfo);
            break;
        case DEVICE_INFO_CHANGED:
            DeviceManagerNotify::GetInstance().OnDeviceChanged(pkgName, *deviceInfo);
            break;
        default:
            LOGE("unknown device state:%d", deviceState);
            break;
    }
}

ON_IPC_CMD(SERVER_DEVICE_FOUND, IpcIo &reply)
{
    size_t len = 0;
    std::string pkgName = (const char *)IpcIoPopString(&reply, &len);
    uint16_t subscribeId = IpcIoPopUint16(&reply);
    uint32_t size;
    const DmDeviceInfo *deviceInfo = (const DmDeviceInfo *)IpcIoPopFlatObj(&reply, &size);
    if (pkgName == "" || len == 0 || deviceInfo == nullptr) {
        LOGE("OnDeviceChanged, get para failed");
        return;
    }
    DeviceManagerNotify::GetInstance().OnDeviceFound(pkgName, subscribeId, *deviceInfo);
}

ON_IPC_CMD(SERVER_DISCOVER_FINISH, IpcIo &reply)
{
    size_t len = 0;
    std::string pkgName = (const char *)IpcIoPopString(&reply, &len);
    uint16_t subscribeId = IpcIoPopUint16(&reply);
    int32_t failedReason = IpcIoPopInt32(&reply);

    if (pkgName == "" || len == 0) {
        LOGE("OnDiscoverySuccess, get para failed");
        return;
    }
    if (failedReason == DM_OK) {
        DeviceManagerNotify::GetInstance().OnDiscoverySuccess(pkgName, subscribeId);
    } else {
        DeviceManagerNotify::GetInstance().OnDiscoveryFailed(pkgName, subscribeId, failedReason);
    }
}

ON_IPC_CMD(SERVER_AUTH_RESULT, IpcIo &reply)
{
    size_t len = 0;
    std::string pkgName = (const char *)IpcIoPopString(&reply, &len);
    size_t devIdLen = 0;
    std::string deviceId = (const char *)IpcIoPopString(&reply, &devIdLen);
    int32_t status = IpcIoPopInt32(&reply);
    int32_t reason = IpcIoPopInt32(&reply);

    if (pkgName == "" || len == 0 || deviceId == "" || devIdLen == 0) {
        LOGE("OnAuthResult, get para failed");
        return;
    }
    DeviceManagerNotify::GetInstance().OnAuthResult(pkgName, deviceId, nullptr, status, reason);
}

ON_IPC_CMD(SERVER_VERIFY_AUTH_RESULT, IpcIo &reply)
{
    size_t len = 0;
    std::string pkgName = (const char *)IpcIoPopString(&reply, &len);
    size_t devIdLen = 0;
    std::string deviceId = (const char *)IpcIoPopString(&reply, &devIdLen);
    int32_t resultCode = IpcIoPopInt32(&reply);
    int32_t flag = IpcIoPopInt32(&reply);

    if (pkgName == "" || len == 0 || deviceId == "" || devIdLen == 0) {
        LOGE("OnAuthResult, get para failed");
        return;
    }
    DeviceManagerNotify::GetInstance().OnVerifyAuthResult(pkgName, deviceId, resultCode, flag);
}

ON_IPC_CMD(SERVER_DEVICE_FA_NOTIFY, IpcIo &reply)
{
    size_t len = 0;
    std::string packagename = (const char *)IpcIoPopString(&reply, &len);
    size_t jsonLen = 0;
    std::string paramJson = (const char *)IpcIoPopString(&reply, &jsonLen);
    DeviceManagerNotify::GetInstance().OnFaCall(packagename, paramJson);
}
} // namespace DistributedHardware
} // namespace OHOS
