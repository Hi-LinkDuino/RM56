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

#ifndef OHOS_DM_IPC_SERVER_STUB_H
#define OHOS_DM_IPC_SERVER_STUB_H

#include <map>
#include <memory>
#include <mutex>
#include <tuple>
#include <vector>

#include "hichain_connector.h"
#include "ipc_remote_broker.h"
#include "iremote_stub.h"
#include "nlohmann/json.hpp"
#include "single_instance.h"
#include "system_ability.h"
#include "thread_pool.h"

namespace OHOS {
namespace DistributedHardware {
enum class ServiceRunningState { STATE_NOT_START, STATE_RUNNING };

class AppDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    void OnRemoteDied(const wptr<IRemoteObject> &remote) override;
    AppDeathRecipient() = default;
    ~AppDeathRecipient() = default;
};

class IpcServerStub : public SystemAbility, public IRemoteStub<IpcRemoteBroker> {
    DECLARE_SYSTEM_ABILITY(IpcServerStub);
    DECLARE_SINGLE_INSTANCE_BASE(IpcServerStub);

public:
    void OnStart() override;
    void OnStop() override;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    int32_t SendCmd(int32_t cmdCode, std::shared_ptr<IpcReq> req, std::shared_ptr<IpcRsp> rsp) override;
    int32_t RegisterDeviceManagerListener(std::string &pkgName, sptr<IRemoteObject> listener);
    int32_t UnRegisterDeviceManagerListener(std::string &pkgName);
    ServiceRunningState QueryServiceState() const;
    const std::map<std::string, sptr<IRemoteObject>> &GetDmListener();
    const sptr<IpcRemoteBroker> GetDmListener(std::string pkgName) const;

private:
    IpcServerStub();
    ~IpcServerStub() = default;
    bool Init();

private:
    bool registerToService_;
    ServiceRunningState state_;
    std::mutex listenerLock_;
    std::map<std::string, sptr<AppDeathRecipient>> appRecipient_;
    std::map<std::string, sptr<IRemoteObject>> dmListener_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_IPC_SERVER_STUB_H
