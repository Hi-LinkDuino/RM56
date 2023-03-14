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
#include "dialog_callback_stub.h"
#include "hilog_wrapper.h"
#include "ui_service_stub.h"

namespace OHOS {
namespace Ace {
DialogCallbackStub::DialogCallbackStub(DialogCallback callback)
{
    callback_ = callback;
}

DialogCallbackStub::~DialogCallbackStub()
{
}

void DialogCallbackStub::OnDialogCallback(int32_t id, const std::string& event, const std::string& params)
{
    callback_(id, event, params);
}

int DialogCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG("DialogCallbackStub::%{public}s, cmd = %{public}d, flags= %{public}d, code=%{public}d",
        __func__, code, option.GetFlags(), code);
    std::u16string descriptor = DialogCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    if (code == IDialogCallback::UI_SERVICE_DIALOG_CALLBACK) {
        return OnDialogCallBackInner(data, reply);
    }
    HILOG_WARN("%{public}s, default case, need check.", __func__);
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int DialogCallbackStub::OnDialogCallBackInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t id = data.ReadInt32();
    const std::string& event = data.ReadString();
    const std::string& params = data.ReadString();
    OnDialogCallback(id, event, params);
    return ERR_NONE;
}

void DialogCallbackRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    HILOG_ERROR("recv DialogCallbackRecipient death notice");

    if (handler_) {
        handler_(remote);
    }
}

DialogCallbackRecipient::DialogCallbackRecipient(RemoteDiedHandler handler) : handler_(handler)
{}

DialogCallbackRecipient::~DialogCallbackRecipient()
{}
}  // namespace Ace
}  // namespace OHOS
