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

#ifndef OHOS_DEVICE_MANAGER_IPC_VERIFY_AUTHENTICATE_REQ_H
#define OHOS_DEVICE_MANAGER_IPC_VERIFY_AUTHENTICATE_REQ_H

#include "ipc_req.h"

namespace OHOS {
namespace DistributedHardware {
class IpcVerifyAuthenticateReq : public IpcReq {
    DECLARE_IPC_MODEL(IpcVerifyAuthenticateReq);

public:
    const std::string &GetAuthPara() const
    {
        return authPara_;
    }

    void SetAuthPara(const std::string &authPara)
    {
        authPara_ = authPara;
    }

private:
    std::string authPara_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DEVICE_MANAGER_IPC_VERIFY_AUTHENTICATE_REQ_H
