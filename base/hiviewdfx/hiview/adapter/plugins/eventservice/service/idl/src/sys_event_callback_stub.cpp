/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "sys_event_callback_stub.h"

#include "errors.h"
#include "hilog/log.h"

namespace OHOS {
namespace HiviewDFX {
static constexpr HiLogLabel LABEL = { LOG_CORE, 0xD002D08, "HiView-SysEventCallbackStub" };
int32_t SysEventCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    std::u16string descripter = SysEventCallbackStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        HiLog::Error(LABEL, "read descriptor failed.");
        return ERR_INVALID_VALUE;
    }
    switch (code) {
        case HANDLE: {
            std::u16string doamin;
            bool ret = data.ReadString16(doamin);
            if (!ret) {
                HiLog::Error(LABEL, "parcel read doamin failed.");
                return ERR_FLATTEN_OBJECT;
            }
            std::u16string eventName;
            ret = data.ReadString16(eventName);
            if (!ret) {
                HiLog::Error(LABEL, "parcel read name failed.");
                return ERR_FLATTEN_OBJECT;
            }
            uint32_t eventType = 0;
            ret = data.ReadUint32(eventType);
            if (!ret) {
                HiLog::Error(LABEL, "parcel read type failed.");
                return ERR_FLATTEN_OBJECT;
            }
            std::u16string eventDetail;
            ret = data.ReadString16(eventDetail);
            if (!ret) {
                HiLog::Error(LABEL, "parcel read detail failed.");
                return ERR_FLATTEN_OBJECT;
            }
            Handle(doamin, eventName, eventType, eventDetail);
            return ERR_OK;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
} // namespace HiviewDFX
} // namespace OHOS