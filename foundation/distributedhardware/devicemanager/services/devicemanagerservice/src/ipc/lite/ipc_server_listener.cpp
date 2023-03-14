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

#include "ipc_server_listener.h"

#include "dm_constants.h"
#include "dm_log.h"
#include "ipc_cmd_register.h"
#include "ipc_def.h"
#include "ipc_server_listenermgr.h"

namespace OHOS {
namespace DistributedHardware {
void IpcServerListener::CommonSvcToIdentity(CommonSvcId *svcId, SvcIdentity *identity)
{
    identity->handle = svcId->handle;
    identity->token = svcId->token;
    identity->cookie = svcId->cookie;
#ifdef __LINUX__
    identity->ipcContext = svcId->ipcCtx;
#endif
}

int32_t IpcServerListener::GetIdentityByPkgName(std::string &name, SvcIdentity *svc)
{
    CommonSvcId svcId;
    if (IpcServerListenermgr::GetInstance().GetListenerByPkgName(name, &svcId) != DM_OK) {
        LOGE("get identity failed.");
        return DM_FAILED;
    }
    CommonSvcToIdentity(&svcId, svc);
    return DM_OK;
}

int32_t IpcServerListener::SendRequest(int32_t cmdCode, std::shared_ptr<IpcReq> req, std::shared_ptr<IpcRsp> rsp)
{
    std::string pkgName = req->GetPkgName();
    SvcIdentity svc;
    if (GetIdentityByPkgName(pkgName, &svc) != DM_OK) {
        LOGE("OnDeviceFound callback get listener failed.");
        return DM_FAILED;
    }

    IpcIo io;
    uint8_t data[MAX_DM_IPC_LEN] = {0};
    if (IpcCmdRegister::GetInstance().SetRequest(cmdCode, req, io, data, MAX_DM_IPC_LEN) != DM_OK) {
        LOGE("SetRequest failed cmdCode:%d", cmdCode);
        return DM_FAILED;
    }

    if (::SendRequest(nullptr, svc, cmdCode, &io, nullptr, LITEIPC_FLAG_ONEWAY, nullptr) != DM_OK) {
        LOGI("SendRequest failed cmdCode:%d", cmdCode);
    }
    return DM_OK;
}

int32_t IpcServerListener::SendAll(int32_t cmdCode, std::shared_ptr<IpcReq> req, std::shared_ptr<IpcRsp> rsp)
{
    const std::map<std::string, CommonSvcId> &listenerMap = IpcServerListenermgr::GetInstance().GetAllListeners();
    for (auto &kv : listenerMap) {
        SvcIdentity svc;
        IpcIo io;
        uint8_t data[MAX_DM_IPC_LEN] = {0};
        std::string pkgName = kv.first;

        req->SetPkgName(pkgName);
        if (IpcCmdRegister::GetInstance().SetRequest(cmdCode, req, io, data, MAX_DM_IPC_LEN) != DM_OK) {
            LOGE("SetRequest failed cmdCode:%d", cmdCode);
            continue;
        }
        CommonSvcId svcId = kv.second;
        CommonSvcToIdentity(&svcId, &svc);
        if (::SendRequest(nullptr, svc, cmdCode, &io, nullptr, LITEIPC_FLAG_ONEWAY, nullptr) != DM_OK) {
            LOGI("SendRequest failed cmdCode:%d", cmdCode);
        }
    }
    return DM_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
