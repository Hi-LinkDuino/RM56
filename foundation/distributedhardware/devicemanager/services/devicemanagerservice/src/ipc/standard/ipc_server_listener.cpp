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
#include "ipc_server_stub.h"

namespace OHOS {
namespace DistributedHardware {
int32_t IpcServerListener::SendRequest(int32_t cmdCode, std::shared_ptr<IpcReq> req, std::shared_ptr<IpcRsp> rsp)
{
    std::string pkgName = req->GetPkgName();
    sptr<IpcRemoteBroker> listener = IpcServerStub::GetInstance().GetDmListener(pkgName);
    if (listener == nullptr) {
        LOGI("cannot get listener for package:%s.", pkgName.c_str());
        return DM_POINT_NULL;
    }
    return listener->SendCmd(cmdCode, req, rsp);
}

int32_t IpcServerListener::SendAll(int32_t cmdCode, std::shared_ptr<IpcReq> req, std::shared_ptr<IpcRsp> rsp)
{
    std::map<std::string, sptr<IRemoteObject>> listeners = IpcServerStub::GetInstance().GetDmListener();
    for (auto iter : listeners) {
        auto pkgName = iter.first;
        auto remote = iter.second;
        req->SetPkgName(pkgName);
        sptr<IpcRemoteBroker> listener = iface_cast<IpcRemoteBroker>(remote);
        if (listener == nullptr) {
            LOGI("cannot get listener for package:%s.", pkgName.c_str());
            return DM_FAILED;
        }
        listener->SendCmd(cmdCode, req, rsp);
    }
    return DM_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
