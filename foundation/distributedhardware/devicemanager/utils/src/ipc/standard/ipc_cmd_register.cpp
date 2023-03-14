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

#include "ipc_cmd_register.h"

#include "dm_constants.h"
#include "dm_log.h"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(IpcCmdRegister);

int32_t IpcCmdRegister::SetRequest(int32_t cmdCode, std::shared_ptr<IpcReq> pBaseReq, MessageParcel &data)
{
    auto setRequestMapIter = setIpcRequestFuncMap_.find(cmdCode);
    if (setRequestMapIter == setIpcRequestFuncMap_.end()) {
        LOGE("cmdCode:%d not register SetRequestFunc", cmdCode);
        return DM_IPC_NOT_REGISTER_FUNC;
    }
    return (setRequestMapIter->second)(pBaseReq, data);
}

int32_t IpcCmdRegister::ReadResponse(int32_t cmdCode, MessageParcel &reply, std::shared_ptr<IpcRsp> pBaseRsp)
{
    auto readResponseMapIter = readResponseFuncMap_.find(cmdCode);
    if (readResponseMapIter == readResponseFuncMap_.end()) {
        LOGE("cmdCode:%d not register ReadResponseFunc", cmdCode);
        return DM_IPC_NOT_REGISTER_FUNC;
    }
    return (readResponseMapIter->second)(reply, pBaseRsp);
}

int32_t IpcCmdRegister::OnIpcCmd(int32_t cmdCode, MessageParcel &data, MessageParcel &reply)
{
    auto onIpcCmdMapIter = onIpcCmdFuncMap_.find(cmdCode);
    if (onIpcCmdMapIter == onIpcCmdFuncMap_.end()) {
        LOGE("cmdCode:%d not register OnIpcCmdFunc", cmdCode);
        return DM_IPC_NOT_REGISTER_FUNC;
    }
    return (onIpcCmdMapIter->second)(data, reply);
}
} // namespace DistributedHardware
} // namespace OHOS
