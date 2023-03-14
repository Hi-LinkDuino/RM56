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

#include "status_receiver_host.h"

#include "ipc_types.h"
#include "string_ex.h"

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
StatusReceiverHost::StatusReceiverHost()
{
    APP_LOGI("create status receiver host instance");
}

StatusReceiverHost::~StatusReceiverHost()
{
    APP_LOGI("destroy status receiver host instance");
}

int StatusReceiverHost::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_LOGI("status receiver host onReceived message, the message code is %{public}u", code);
    std::u16string descripter = StatusReceiverHost::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        APP_LOGE("fail to write reply message in status receiver host due to the reply is nullptr");
        return OBJECT_NULL;
    }

    switch (code) {
        case static_cast<uint32_t>(IStatusReceiver::Message::ON_FINISHED): {
            int32_t resultCode = data.ReadInt32();
            std::string resultMsg = Str16ToStr8(data.ReadString16());
            OnFinished(resultCode, resultMsg);
            break;
        }
        case static_cast<uint32_t>(IStatusReceiver::Message::ON_STATUS_NOTIFY): {
            int32_t progress = data.ReadInt32();
            OnStatusNotify(progress);
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return NO_ERROR;
}
}  // namespace AppExecFwk
}  // namespace OHOS