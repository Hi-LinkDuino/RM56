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

#ifndef OHOS_HIVIEWDFX_SYS_EVENT_SERVICE_STUB_H
#define OHOS_HIVIEWDFX_SYS_EVENT_SERVICE_STUB_H

#include "iremote_stub.h"
#include "isys_event_service.h"

namespace OHOS {
namespace HiviewDFX {
class SysEventServiceStub : public IRemoteStub<ISysEventService> {
public:
    SysEventServiceStub() = default;
    virtual ~SysEventServiceStub() = default;
    DISALLOW_COPY_AND_MOVE(SysEventServiceStub);

    virtual int32_t OnRemoteRequest(uint32_t code,
        MessageParcel& data, MessageParcel& reply, MessageOption& option) override;

private:
    int32_t HandleAddSysEventListener(MessageParcel& data,
        MessageParcel& reply, MessageOption& option);
    int32_t HandleRemoveSysEventListener(MessageParcel& data,
        MessageParcel& reply, MessageOption& option);
    int32_t HandleQueryEvent(MessageParcel& data,
        MessageParcel& reply, MessageOption& option);
    int32_t HandleSetDebugMode(MessageParcel& data,
        MessageParcel& reply, MessageOption& option);
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // OHOS_HIVIEWDFX_SYS_EVENT_SERVICE_STUB_H