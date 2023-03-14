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

#include "clean_cache_callback_host.h"

#include "ipc_types.h"

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
CleanCacheCallbackHost::CleanCacheCallbackHost()
{
    APP_LOGI("create clean cache callback host instance");
}

CleanCacheCallbackHost::~CleanCacheCallbackHost()
{
    APP_LOGI("destroyclean cache callback host instance");
}

int CleanCacheCallbackHost::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_LOGD("clean cache callback host onReceived message, the message code is %{public}u", code);
    std::u16string descripter = CleanCacheCallbackHost::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        APP_LOGE("fail to write reply message in clean cache host due to the reply is nullptr");
        return OBJECT_NULL;
    }

    switch (code) {
        case static_cast<uint32_t>(ICleanCacheCallback::Message::ON_CLEAN_CACHE_CALLBACK): {
            bool succeed = data.ReadBool();
            OnCleanCacheFinished(succeed);
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return NO_ERROR;
}
}  // namespace AppExecFwk
}  // namespace OHOS
