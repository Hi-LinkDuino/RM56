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

#include "ipc_client_manager.h"

#include "dm_constants.h"
#include "dm_log.h"
#include "ipc_client_stub.h"
#include "ipc_register_listener_req.h"

namespace OHOS {
namespace DistributedHardware {
int32_t IpcClientManager::Init(const std::string &pkgName)
{
    if (IsInit(pkgName)) {
        LOGI("already init");
        return DM_OK;
    }
    if (serverProxy_.Init() != DM_OK) {
        LOGE("server proxy init failed.");
        return DM_INIT_FAILED;
    }
    if (IpcClientStub::GetInstance().Init() != DM_OK) {
        LOGE("ipcclientstub init failed.");
        return DM_INIT_FAILED;
    }

    std::shared_ptr<IpcRegisterListenerReq> req = std::make_shared<IpcRegisterListenerReq>();
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    req->SetSvcIdentity(IpcClientStub::GetInstance().GetSvcIdentity());
    int32_t ret = serverProxy_.SendCmd(REGISTER_DEVICE_MANAGER_LISTENER, req, rsp);
    if (ret != DM_OK) {
        LOGE("InitDeviceManager: RegisterDeviceManagerListener Failed with ret %d", ret);
        return ret;
    }
    ret = rsp->GetErrCode();
    if (ret != DM_OK) {
        LOGE("DeviceManager::InitDeviceManager completed, pkgName: %s, ret=%d", pkgName.c_str(), ret);
        return ret;
    }
    packageInitSet_.emplace(pkgName);
    return DM_OK;
}

int32_t IpcClientManager::UnInit(const std::string &pkgName)
{
    LOGI("UnInitDeviceManager in, pkgName %s", pkgName.c_str());
    if (!IsInit(pkgName)) {
        return DM_FAILED;
    }
    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    if (req == nullptr) {
        LOGE("req is null");
        return DM_FAILED;
    }
    req->SetPkgName(pkgName);
    int32_t ret = serverProxy_.SendCmd(UNREGISTER_DEVICE_MANAGER_LISTENER, req, rsp);
    if (ret != DM_OK) {
        LOGE("UnRegisterDeviceManagerListener Failed with ret %d", ret);
        return ret;
    }
    packageInitSet_.erase(pkgName);
    LOGI("UnInitDeviceManager SUCCESS");
    return DM_OK;
}

int32_t IpcClientManager::SendRequest(int32_t cmdCode, std::shared_ptr<IpcReq> req, std::shared_ptr<IpcRsp> rsp)
{
    std::string pkgName = req->GetPkgName();
    if (!IsInit(pkgName)) {
        return DM_SERVICE_NOT_READY;
    }
    return serverProxy_.SendCmd(cmdCode, req, rsp);
}

bool IpcClientManager::IsInit(const std::string &pkgName)
{
    return (packageInitSet_.count(pkgName) > 0);
}
} // namespace DistributedHardware
} // namespace OHOS
