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

#include "ipc_client_stub.h"

#include "dm_constants.h"
#include "dm_log.h"
#include "ipc_cmd_register.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"

namespace OHOS {
namespace DistributedHardware {
int32_t IpcClientStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    LOGI("code = %u, flags= %d.", code, option.GetFlags());
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        LOGI("ReadInterfaceToken fail!");
        return ERR_INVALID_STATE;
    }

    if (IpcCmdRegister::GetInstance().OnIpcCmd((int32_t)code, data, reply) == DM_OK) {
        LOGE("on ipc cmd success");
        return DM_OK;
    }
    LOGW("unsupported code: %u", code);
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t IpcClientStub::SendCmd(int32_t cmdCode, std::shared_ptr<IpcReq> req, std::shared_ptr<IpcRsp> rsp)
{
    LOGI("SendCmd cmdCode: %d", cmdCode);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (IpcCmdRegister::GetInstance().SetRequest(cmdCode, req, data) != DM_OK) {
        LOGE("set request cmd failed");
        return DM_IPC_FAILED;
    }
    
    LOGI("cmdCode = %d, flags= %d.", cmdCode, option.GetFlags());
    if (IpcCmdRegister::GetInstance().OnIpcCmd(cmdCode, data, reply) == DM_OK) {
        LOGE("on ipc cmd success");
        return DM_OK;
    }
    return IpcCmdRegister::GetInstance().ReadResponse(cmdCode, reply, rsp);
}
} // namespace DistributedHardware
} // namespace OHOS
