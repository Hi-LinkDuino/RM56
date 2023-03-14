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

#include "dialog_callback_proxy.h"
#include "hilog_wrapper.h"
#include "message_parcel.h"

namespace OHOS {
namespace Ace {
DialogCallbackProxy::DialogCallbackProxy(const sptr<IRemoteObject> &remote)
    : IRemoteProxy<IDialogCallback>(remote)
{}
DialogCallbackProxy::~DialogCallbackProxy()
{}

void DialogCallbackProxy::OnDialogCallback(int32_t id, const std::string& event, const std::string& params)
{
    auto remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("%{public}s remote is nullptr", __func__);
        return;
    }

    OHOS::MessageParcel dataParcel;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;

    if (!dataParcel.WriteInterfaceToken(DialogCallbackProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s dataParcel.WriteInterfaceToken(GetDescriptor()) return false", __func__);
        return;
    }
    if (!dataParcel.WriteInt32(id)) {
        HILOG_ERROR("fail to WriteInt32 id");
        return;
    }
    if (!dataParcel.WriteString(event)) {
        HILOG_ERROR("fail to WriteString event");
        return;
    }
    if (!dataParcel.WriteString(params)) {
        HILOG_ERROR("fail to WriteString params");
        return;
    }
    int result = remote->SendRequest(IDialogCallback::UI_SERVICE_DIALOG_CALLBACK, dataParcel, reply, option);
    if (result == ERR_NONE) {
        HILOG_INFO("%{public}s SendRequest ok, retval is %{public}d", __func__, reply.ReadInt32());
        return;
    } else {
        HILOG_ERROR("%{public}s SendRequest error, result=%{public}d", __func__, result);
        return;
    }
}
}  // namespace Ace
}  // namespace OHOS
