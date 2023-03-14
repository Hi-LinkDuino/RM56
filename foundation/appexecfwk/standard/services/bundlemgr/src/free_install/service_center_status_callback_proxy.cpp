/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "service_center_status_callback_proxy.h"

#include "ipc_types.h"
#include "message_parcel.h"

namespace OHOS {
namespace AppExecFwk {
using OHOS::AppExecFwk::IServiceCenterStatusCallback;

ServiceCenterStatusCallbackProxy::ServiceCenterStatusCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IServiceCenterStatusCallback>(impl)
{
}

void ServiceCenterStatusCallbackProxy::OnRemoteInstallFinished(std::string installResult)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(IServiceCenterStatusCallback::GetDescriptor())) {
        APP_LOGE("Write interface token failed.");
        return;
    }

    if (!data.WriteString(installResult)) {
        APP_LOGE("Write resultCode error.");
        return;
    }

    int32_t error = Remote()->SendRequest(0, data, reply, option);
    if (error != NO_ERROR) {
        APP_LOGE("OnFinished fail, error: %{public}d", error);
    }
}
}  // namespace AppExecFwk
}  // namespace OHOS
