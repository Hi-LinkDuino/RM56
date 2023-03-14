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

#ifndef OHOS_DM_IPC_SERVER_LISTENER_H
#define OHOS_DM_IPC_SERVER_LISTENER_H

#include <memory>

#include "ipc_req.h"
#include "ipc_rsp.h"
#include "ipc_server_listenermgr.h"

namespace OHOS {
namespace DistributedHardware {
class IpcServerListener {
public:
    IpcServerListener() = default;
    virtual ~IpcServerListener() = default;

public:
    int32_t SendRequest(int32_t cmdCode, std::shared_ptr<IpcReq> req, std::shared_ptr<IpcRsp> rsp);
    int32_t SendAll(int32_t cmdCode, std::shared_ptr<IpcReq> req, std::shared_ptr<IpcRsp> rsp);

private:
    void CommonSvcToIdentity(CommonSvcId *svcId, SvcIdentity *identity);
    int32_t GetIdentityByPkgName(std::string &name, SvcIdentity *svc);
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_IPC_SERVER_LISTENER_H
