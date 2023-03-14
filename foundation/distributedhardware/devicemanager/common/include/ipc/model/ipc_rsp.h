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

#ifndef OHOS_DM_IPC_RSP_H
#define OHOS_DM_IPC_RSP_H

#include <cstdint>

#include "ipc_def.h"

namespace OHOS {
namespace DistributedHardware {
class IpcRsp {
    DECLARE_IPC_MODEL(IpcRsp);

public:
    int32_t GetErrCode() const
    {
        return errCode_;
    }

    void SetErrCode(int32_t errCode)
    {
        errCode_ = errCode;
    }

private:
    int32_t errCode_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_IPC_RSP_H
