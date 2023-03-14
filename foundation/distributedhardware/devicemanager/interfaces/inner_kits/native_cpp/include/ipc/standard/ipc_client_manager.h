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

#ifndef OHOS_DM_IPC_CLIENT_MANAGER_H
#define OHOS_DM_IPC_CLIENT_MANAGER_H

#include <cstdint>
#include <map>
#include <mutex>
#include <string>

#include "ipc_client.h"
#include "ipc_client_stub.h"
#include "ipc_def.h"
#include "ipc_remote_broker.h"
#include "iremote_object.h"

namespace OHOS {
namespace DistributedHardware {
class DmDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    void OnRemoteDied(const wptr<IRemoteObject> &remote) override;
    DmDeathRecipient() = default;
    ~DmDeathRecipient() = default;
};

class IpcClientManager : public IpcClient {
    friend class DmDeathRecipient;
    DECLARE_IPC_INTERFACE(IpcClientManager);

public:
    virtual int32_t Init(const std::string &pkgName) override;
    virtual int32_t UnInit(const std::string &pkgName) override;
    virtual int32_t SendRequest(int32_t cmdCode, std::shared_ptr<IpcReq> req, std::shared_ptr<IpcRsp> rsp) override;

private:
    bool IsInit(const std::string &pkgName);
    int32_t ClientInit();

private:
    std::mutex lock_;
    std::map<std::string, sptr<IpcClientStub>> dmListener_;
    sptr<IpcRemoteBroker> dmInterface_ { nullptr };
    sptr<DmDeathRecipient> dmRecipient_ { nullptr };
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_IPC_CLIENT_MANAGER_H
