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

#ifndef OHOS_DM_IPC_CLIENT_SERVER_PROXY_H
#define OHOS_DM_IPC_CLIENT_SERVER_PROXY_H

#include <cstdint>
#include <memory>
#include <mutex>

#include "ipc_req.h"
#include "ipc_rsp.h"
#include "iproxy_client.h"
#include "liteipc_adapter.h"
#include "samgr_lite.h"

namespace OHOS {
namespace DistributedHardware {
class IpcClientServerProxy {
public:
    int32_t Init();
    int32_t SendCmd(int32_t cmdCode, std::shared_ptr<IpcReq> req, std::shared_ptr<IpcRsp> rsp);

private:
    IClientProxy *GetServerProxy(void);
    int32_t RegisterServerDeathCb(void);

private:
    std::mutex lock_;
    IClientProxy *serviceProxy_ { nullptr };
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_IPC_CLIENT_SERVER_PROXY_H
