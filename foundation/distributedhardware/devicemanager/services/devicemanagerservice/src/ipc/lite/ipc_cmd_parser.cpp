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

#include "device_manager_service.h"
#include "dm_constants.h"
#include "dm_log.h"
#include "ipc_cmd_register.h"
#include "ipc_def.h"
#include "ipc_notify_auth_result_req.h"
#include "ipc_notify_device_found_req.h"
#include "ipc_notify_device_state_req.h"
#include "ipc_notify_discover_result_req.h"
#include "ipc_notify_verify_auth_result_req.h"
#include "ipc_server_stub.h"

namespace OHOS {
namespace DistributedHardware {
ON_IPC_SET_REQUEST(SERVER_DEVICE_STATE_NOTIFY, std::shared_ptr<IpcReq> pBaseReq, IpcIo &request, uint8_t *buffer,
                   size_t buffLen)
{
    std::shared_ptr<IpcNotifyDeviceStateReq> pReq = std::static_pointer_cast<IpcNotifyDeviceStateReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    int32_t deviceState = pReq->GetDeviceState();
    DmDeviceInfo deviceInfo = pReq->GetDeviceInfo();

    IpcIoInit(&request, buffer, buffLen, 0);
    IpcIoPushString(&request, pkgName.c_str());
    IpcIoPushInt32(&request, deviceState);
    IpcIoPushFlatObj(&request, &deviceInfo, sizeof(DmDeviceInfo));
    return DM_OK;
}

ON_IPC_READ_RESPONSE(SERVER_DEVICE_STATE_NOTIFY, IpcIo &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    if (pBaseRsp == nullptr) {
        LOGE("pBaseRsp is null");
        return DM_FAILED;
    }
    pBaseRsp->SetErrCode(IpcIoPopInt32(&reply));
    return DM_OK;
}

ON_IPC_SET_REQUEST(SERVER_DEVICE_FOUND, std::shared_ptr<IpcReq> pBaseReq, IpcIo &request, uint8_t *buffer,
                   size_t buffLen)
{
    std::shared_ptr<IpcNotifyDeviceFoundReq> pReq = std::static_pointer_cast<IpcNotifyDeviceFoundReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    uint16_t subscribeId = pReq->GetSubscribeId();
    DmDeviceInfo deviceInfo = pReq->GetDeviceInfo();

    IpcIoInit(&request, buffer, buffLen, 0);
    IpcIoPushString(&request, pkgName.c_str());
    IpcIoPushUint16(&request, subscribeId);
    IpcIoPushFlatObj(&request, &deviceInfo, sizeof(DmDeviceInfo));
    return DM_OK;
}

ON_IPC_READ_RESPONSE(SERVER_DEVICE_FOUND, IpcIo &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    if (pBaseRsp == nullptr) {
        LOGE("pBaseRsp is null");
        return DM_FAILED;
    }
    pBaseRsp->SetErrCode(IpcIoPopInt32(&reply));
    return DM_OK;
}

ON_IPC_SET_REQUEST(SERVER_DISCOVER_FINISH, std::shared_ptr<IpcReq> pBaseReq, IpcIo &request, uint8_t *buffer,
                   size_t buffLen)
{
    std::shared_ptr<IpcNotifyDiscoverResultReq> pReq = std::static_pointer_cast<IpcNotifyDiscoverResultReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    uint16_t subscribeId = pReq->GetSubscribeId();
    int32_t result = pReq->GetResult();

    IpcIoInit(&request, buffer, buffLen, 0);
    IpcIoPushString(&request, pkgName.c_str());
    IpcIoPushUint16(&request, subscribeId);
    IpcIoPushInt32(&request, result);
    return DM_OK;
}

ON_IPC_READ_RESPONSE(SERVER_DISCOVER_FINISH, IpcIo &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(IpcIoPopInt32(&reply));
    return DM_OK;
}

ON_IPC_SET_REQUEST(SERVER_AUTH_RESULT, std::shared_ptr<IpcReq> pBaseReq, IpcIo &request, uint8_t *buffer,
                   size_t buffLen)
{
    std::shared_ptr<IpcNotifyAuthResultReq> pReq = std::static_pointer_cast<IpcNotifyAuthResultReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    std::string deviceId = pReq->GetDeviceId();
    std::string token = pReq->GetPinToken();
    int32_t status = pReq->GetStatus();
    int32_t reason = pReq->GetReason();

    IpcIoInit(&request, buffer, buffLen, 0);
    IpcIoPushString(&request, pkgName.c_str());
    IpcIoPushString(&request, deviceId.c_str());
    IpcIoPushString(&request, token.c_str());
    IpcIoPushInt32(&request, status);
    IpcIoPushInt32(&request, reason);
    return DM_OK;
}

ON_IPC_READ_RESPONSE(SERVER_AUTH_RESULT, IpcIo &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(IpcIoPopInt32(&reply));
    return DM_OK;
}

ON_IPC_SET_REQUEST(SERVER_VERIFY_AUTH_RESULT, std::shared_ptr<IpcReq> pBaseReq, IpcIo &request, uint8_t *buffer,
                   size_t buffLen)
{
    std::shared_ptr<IpcNotifyVerifyAuthResultReq> pReq =
        std::static_pointer_cast<IpcNotifyVerifyAuthResultReq>(pBaseReq);
    std::string pkgName = pReq->GetPkgName();
    std::string deviceId = pReq->GetDeviceId();
    int32_t result = pReq->GetResult();
    int32_t flag = pReq->GetFlag();

    IpcIoInit(&request, buffer, buffLen, 0);
    IpcIoPushString(&request, pkgName.c_str());
    IpcIoPushString(&request, deviceId.c_str());
    IpcIoPushInt32(&request, result);
    IpcIoPushInt32(&request, flag);
    return DM_OK;
}

ON_IPC_READ_RESPONSE(SERVER_VERIFY_AUTH_RESULT, IpcIo &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    pBaseRsp->SetErrCode(IpcIoPopInt32(&reply));
    return DM_OK;
}

ON_IPC_SERVER_CMD(REGISTER_DEVICE_MANAGER_LISTENER, IpcIo &req, IpcIo &reply)
{
    int32_t errCode = RegisterDeviceManagerListener(&req, &reply);
    IpcIoPushInt32(&reply, errCode);
}

ON_IPC_SERVER_CMD(UNREGISTER_DEVICE_MANAGER_LISTENER, IpcIo &req, IpcIo &reply)
{
    int32_t errCode = UnRegisterDeviceManagerListener(&req, &reply);
    IpcIoPushInt32(&reply, errCode);
}

ON_IPC_SERVER_CMD(GET_TRUST_DEVICE_LIST, IpcIo &req, IpcIo &reply)
{
    LOGI("enter GetTrustedDeviceList.");
    std::string pkgName = (const char *)IpcIoPopString(&req, nullptr);
    std::string extra = (const char *)IpcIoPopString(&req, nullptr);

    std::vector<DmDeviceInfo> deviceList;
    int32_t ret = DeviceManagerService::GetInstance().GetTrustedDeviceList(pkgName, extra, deviceList);
    IpcIoPushInt32(&reply, deviceList.size());
    if (deviceList.size() > 0) {
        IpcIoPushFlatObj(&reply, deviceList.data(), sizeof(DmDeviceInfo) * deviceList.size());
    }
    IpcIoPushInt32(&reply, ret);
}

ON_IPC_SERVER_CMD(GET_LOCAL_DEVICE_INFO, IpcIo &req, IpcIo &reply)
{
    LOGI("enter GetLocalDeviceInfo.");
    DmDeviceInfo dmDeviceInfo;
    int32_t ret = DeviceManagerService::GetInstance().GetLocalDeviceInfo(dmDeviceInfo);
    IpcIoPushFlatObj(&reply, &dmDeviceInfo, sizeof(DmDeviceInfo));
    IpcIoPushInt32(&reply, ret);
}

ON_IPC_SERVER_CMD(START_DEVICE_DISCOVER, IpcIo &req, IpcIo &reply)
{
    LOGI("StartDeviceDiscovery service listener.");
    std::string pkgName = (const char *)IpcIoPopString(&req, nullptr);
    std::string extra = (const char *)IpcIoPopString(&req, nullptr);

    uint32_t size = 0;
    DmSubscribeInfo *pDmSubscribeInfo = (DmSubscribeInfo *)IpcIoPopFlatObj(&req, &size);
    int32_t ret = DeviceManagerService::GetInstance().StartDeviceDiscovery(pkgName, *pDmSubscribeInfo, extra);
    IpcIoPushInt32(&reply, ret);
}

ON_IPC_SERVER_CMD(STOP_DEVICE_DISCOVER, IpcIo &req, IpcIo &reply)
{
    LOGI("StopDeviceDiscovery service listener.");
    std::string pkgName = (const char *)IpcIoPopString(&req, nullptr);
    uint16_t subscribeId = IpcIoPopUint16(&req);
    int32_t ret = DeviceManagerService::GetInstance().StopDeviceDiscovery(pkgName, subscribeId);
    IpcIoPushInt32(&reply, ret);
}

ON_IPC_SERVER_CMD(AUTHENTICATE_DEVICE, IpcIo &req, IpcIo &reply)
{
    LOGI("AuthenticateDevice service listener.");
    std::string pkgName = (const char *)IpcIoPopString(&req, nullptr);
    std::string extra = (const char *)IpcIoPopString(&req, nullptr);
    std::string deviceId = (const char *)IpcIoPopString(&req, nullptr);
    int32_t authType = IpcIoPopInt32(&req);
    int32_t ret = DeviceManagerService::GetInstance().AuthenticateDevice(pkgName, authType, deviceId, extra);
    IpcIoPushInt32(&reply, ret);
}

ON_IPC_SERVER_CMD(UNAUTHENTICATE_DEVICE, IpcIo &req, IpcIo &reply)
{
    LOGI("UnAuthenticateDevice service listener.");
    std::string pkgName = (const char *)IpcIoPopString(&req, nullptr);
    std::string deviceId = (const char *)IpcIoPopString(&req, nullptr);

    int32_t ret = DeviceManagerService::GetInstance().UnAuthenticateDevice(pkgName, deviceId);
    IpcIoPushInt32(&reply, ret);
}

ON_IPC_SERVER_CMD(VERIFY_AUTHENTICATION, IpcIo &req, IpcIo &reply)
{
    LOGI("VerifyAuthentication service listener.");
    std::string authParam = (const char *)IpcIoPopString(&req, nullptr);
    int32_t ret = DeviceManagerService::GetInstance().VerifyAuthentication(authParam);
    IpcIoPushInt32(&reply, ret);
}

ON_IPC_SERVER_CMD(SERVER_USER_AUTH_OPERATION, IpcIo &req, IpcIo &reply)
{
    size_t len = 0;
    std::string packName = (const char *)IpcIoPopString(&req, &len);
    int32_t action = IpcIoPopInt32(&reply);
    DeviceManagerService::GetInstance().SetUserOperation(packName, action);
    IpcIoPushInt32(&reply, action);
}

ON_IPC_SET_REQUEST(SERVER_DEVICE_FA_NOTIFY, std::shared_ptr<IpcReq> pBaseReq, IpcIo &request, uint8_t *buffer,
                   size_t buffLen)
{
    std::shared_ptr<IpcNotifyDMFAResultReq> pReq = std::static_pointer_cast<IpcNotifyDMFAResultReq>(pBaseReq);
    std::string packagname = pReq->GetPkgName();
    std::string paramJson = pReq->GetJsonParam();
    IpcIoPushString(&request, packagname.c_str());
    IpcIoPushString(&request, paramJson.c_str());
    return DM_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
