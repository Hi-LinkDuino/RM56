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

#ifndef OHOS_DM_IPC_NOTIFY_DEVICE_STATE_REQ_H
#define OHOS_DM_IPC_NOTIFY_DEVICE_STATE_REQ_H

#include "dm_device_info.h"
#include "ipc_req.h"

namespace OHOS {
namespace DistributedHardware {
class IpcNotifyDeviceStateReq : public IpcReq {
    DECLARE_IPC_MODEL(IpcNotifyDeviceStateReq);

public:
    int32_t GetDeviceState() const
    {
        return deviceState_;
    }

    void SetDeviceState(int32_t deviceState)
    {
        deviceState_ = deviceState;
    }

    const DmDeviceInfo &GetDeviceInfo() const
    {
        return dmDeviceInfo_;
    }

    void SetDeviceInfo(const DmDeviceInfo &dmDeviceInfo)
    {
        dmDeviceInfo_ = dmDeviceInfo;
    }

private:
    int32_t deviceState_ { 0 };
    DmDeviceInfo dmDeviceInfo_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_IPC_NOTIFY_DEVICE_STATE_REQ_H
