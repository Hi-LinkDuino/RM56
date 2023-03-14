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

#include "device_manager_service_listener.h"

#include "dm_anonymous.h"
#include "dm_constants.h"
#include "dm_log.h"
#include "ipc_notify_auth_result_req.h"
#include "ipc_notify_device_found_req.h"
#include "ipc_notify_device_state_req.h"
#include "ipc_notify_discover_result_req.h"
#include "ipc_notify_verify_auth_result_req.h"

namespace OHOS {
namespace DistributedHardware {
void DeviceManagerServiceListener::OnDeviceStateChange(const std::string &pkgName, const DmDeviceState &state,
                                                       const DmDeviceInfo &info)
{
    LOGI("OnDeviceStateChange, state=%d", state);
    std::shared_ptr<IpcNotifyDeviceStateReq> pReq = std::make_shared<IpcNotifyDeviceStateReq>();
    std::shared_ptr<IpcRsp> pRsp = std::make_shared<IpcRsp>();

    pReq->SetPkgName(pkgName);
    pReq->SetDeviceState(state);
    pReq->SetDeviceInfo(info);
    if (pkgName == DM_PKG_NAME) {
        ipcServerListener_.SendAll(SERVER_DEVICE_STATE_NOTIFY, pReq, pRsp);
    } else {
        ipcServerListener_.SendRequest(SERVER_DEVICE_STATE_NOTIFY, pReq, pRsp);
    }
}

void DeviceManagerServiceListener::OnDeviceFound(const std::string &pkgName, uint16_t subscribeId,
                                                 const DmDeviceInfo &info)
{
    LOGI("call OnDeviceFound for %s, originId %hu, deviceId %s", pkgName.c_str(), subscribeId,
         GetAnonyString(std::string(info.deviceId)).c_str());
    std::shared_ptr<IpcNotifyDeviceFoundReq> pReq = std::make_shared<IpcNotifyDeviceFoundReq>();
    std::shared_ptr<IpcRsp> pRsp = std::make_shared<IpcRsp>();

    pReq->SetPkgName(pkgName);
    pReq->SetSubscribeId(subscribeId);
    pReq->SetDeviceInfo(info);
    ipcServerListener_.SendRequest(SERVER_DEVICE_FOUND, pReq, pRsp);
}

void DeviceManagerServiceListener::OnDiscoveryFailed(const std::string &pkgName, uint16_t subscribeId,
                                                     int32_t failedReason)
{
    LOGI("OnDiscoveryFailed");
    std::shared_ptr<IpcNotifyDiscoverResultReq> pReq = std::make_shared<IpcNotifyDiscoverResultReq>();
    std::shared_ptr<IpcRsp> pRsp = std::make_shared<IpcRsp>();

    pReq->SetPkgName(pkgName);
    pReq->SetSubscribeId(subscribeId);
    pReq->SetResult(failedReason);
    ipcServerListener_.SendRequest(SERVER_DISCOVER_FINISH, pReq, pRsp);
}

void DeviceManagerServiceListener::OnDiscoverySuccess(const std::string &pkgName, int32_t subscribeId)
{
    LOGI("OnDiscoverySuccess");
    std::shared_ptr<IpcNotifyDiscoverResultReq> pReq = std::make_shared<IpcNotifyDiscoverResultReq>();
    std::shared_ptr<IpcRsp> pRsp = std::make_shared<IpcRsp>();

    pReq->SetPkgName(pkgName);
    pReq->SetSubscribeId((uint16_t)subscribeId);
    pReq->SetResult(DM_OK);
    ipcServerListener_.SendRequest(SERVER_DISCOVER_FINISH, pReq, pRsp);
}

void DeviceManagerServiceListener::OnAuthResult(const std::string &pkgName, const std::string &deviceId,
                                                const std::string &token, int32_t status, int32_t reason)
{
    LOGI("%s, package: %s, deviceId: %s", __FUNCTION__, pkgName.c_str(), GetAnonyString(deviceId).c_str());
    std::shared_ptr<IpcNotifyAuthResultReq> pReq = std::make_shared<IpcNotifyAuthResultReq>();
    std::shared_ptr<IpcRsp> pRsp = std::make_shared<IpcRsp>();

    pReq->SetPkgName(pkgName);
    pReq->SetDeviceId(deviceId);
    pReq->SetToken(token);
    pReq->SetStatus(status);
    pReq->SetReason(reason);
    ipcServerListener_.SendRequest(SERVER_AUTH_RESULT, pReq, pRsp);
}

void DeviceManagerServiceListener::OnVerifyAuthResult(const std::string &pkgName, const std::string &deviceId,
                                                      int32_t resultCode, const std::string &flag)
{
    std::shared_ptr<IpcNotifyVerifyAuthResultReq> pReq = std::make_shared<IpcNotifyVerifyAuthResultReq>();
    std::shared_ptr<IpcRsp> pRsp = std::make_shared<IpcRsp>();

    pReq->SetDeviceId(deviceId);
    pReq->SetResult(resultCode);
    ipcServerListener_.SendAll(SERVER_VERIFY_AUTH_RESULT, pReq, pRsp);
}

void DeviceManagerServiceListener::OnFaCall(std::string &pkgName, std::string &paramJson)
{
    LOGI("OnFaCall in");
    std::shared_ptr<IpcNotifyDMFAResultReq> pReq = std::make_shared<IpcNotifyDMFAResultReq>();
    std::shared_ptr<IpcRsp> pRsp = std::make_shared<IpcRsp>();

    pReq->SetPkgName(pkgName);
    pReq->SetJsonParam(paramJson);
    ipcServerListener_.SendRequest(SERVER_DEVICE_FA_NOTIFY, pReq, pRsp);
}
} // namespace DistributedHardware
} // namespace OHOS
