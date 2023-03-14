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

#ifndef OHOS_DEVICE_MANAGER_IPC_GET_USER_OPERATION_REQ_H
#define OHOS_DEVICE_MANAGER_IPC_GET_USER_OPERATION_REQ_H

#include "dm_device_info.h"
#include "ipc_req.h"

namespace OHOS {
namespace DistributedHardware {
class IpcGetOperationReq : public IpcReq {
    DECLARE_IPC_MODEL(IpcGetOperationReq);

public:
    int32_t GetOperation() const
    {
        return action_;
    }

    void SetOperation(int32_t action)
    {
        action_ = action;
    }

private:
    int32_t action_ { 0 };
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DEVICE_MANAGER_IPC_GET_USER_OPERATION_REQ_H
