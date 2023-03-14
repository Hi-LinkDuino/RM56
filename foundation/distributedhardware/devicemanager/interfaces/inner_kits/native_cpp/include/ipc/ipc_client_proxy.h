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

#ifndef OHOS_DM_IPC_CLIENT_PROXY_H
#define OHOS_DM_IPC_CLIENT_PROXY_H

#include <cstdint>
#include <memory>

#include "ipc_client.h"
#include "ipc_req.h"
#include "ipc_rsp.h"

namespace OHOS {
namespace DistributedHardware {
class IpcClientProxy : public IpcClient {
    DECLARE_IPC_INTERFACE(IpcClientProxy);

public:
    explicit IpcClientProxy(std::shared_ptr<IpcClient> ipcClientManager) : ipcClientManager_(ipcClientManager) {};

public:
    virtual int32_t Init(const std::string &pkgName);
    virtual int32_t UnInit(const std::string &pkgName);
    virtual int32_t SendRequest(int32_t cmdCode, std::shared_ptr<IpcReq> req, std::shared_ptr<IpcRsp> rsp);

private:
    std::shared_ptr<IpcClient> ipcClientManager_ { nullptr };
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_IPC_CLIENT_PROXY_H
