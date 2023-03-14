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

#ifndef OHOS_DM_IPC_NOTIFY_DISCOVER_RESULT_REQ_H
#define OHOS_DM_IPC_NOTIFY_DISCOVER_RESULT_REQ_H

#include "ipc_req.h"

namespace OHOS {
namespace DistributedHardware {
class IpcNotifyDiscoverResultReq : public IpcReq {
    DECLARE_IPC_MODEL(IpcNotifyDiscoverResultReq);

public:
    uint16_t GetSubscribeId() const
    {
        return subscribeId_;
    }

    void SetSubscribeId(uint16_t subscribeId)
    {
        subscribeId_ = subscribeId;
    }

    int32_t GetResult() const
    {
        return result_;
    }

    void SetResult(int32_t result)
    {
        result_ = result;
    }

private:
    uint16_t subscribeId_ { 0 };
    int32_t result_ { 0 };
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_IPC_NOTIFY_DISCOVER_RESULT_REQ_H
