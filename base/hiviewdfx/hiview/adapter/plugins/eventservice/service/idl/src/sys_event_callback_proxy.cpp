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

#include "sys_event_callback_proxy.h"

#include "errors.h"
#include "hilog/log.h"

namespace OHOS {
namespace HiviewDFX {
static constexpr HiLogLabel LABEL = { LOG_CORE, 0xD002D08, "HiView-SysEventCallbackProxy" };
void SysEventCallbackProxy::Handle(const std::u16string& domain, const std::u16string& eventName, uint32_t eventType,
    const std::u16string& eventDetail)
{
    auto remote = Remote();
    if (remote == nullptr) {
        HiLog::Error(LABEL, "SysEventService Remote is NULL.");
        return;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(SysEventCallbackProxy::GetDescriptor())) {
        HiLog::Error(LABEL, "write descriptor failed.");
        return;
    }

    bool ret = data.WriteString16(domain);
    if (!ret) {
        HiLog::Error(LABEL, "parcel write domain failed.");
        return;
    }
    ret = data.WriteString16(eventName);
    if (!ret) {
        HiLog::Error(LABEL, "parcel write eventName failed.");
        return;
    }
    ret = data.WriteUint32(eventType);
    if (!ret) {
        HiLog::Error(LABEL, "parcel write event type failed.");
        return;
    }
    ret = data.WriteString16(eventDetail);
    if (!ret) {
        HiLog::Error(LABEL, "parcel write event detail failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int32_t res = remote->SendRequest(HANDLE, data, reply, option);
    if (res != ERR_OK) {
        HiLog::Error(LABEL, "send request failed, error is %{public}d.", res);
    }
}
} // namespace HiviewDFX
} // namespace OHOS