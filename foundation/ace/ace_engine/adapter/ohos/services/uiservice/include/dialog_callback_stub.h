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
#ifndef OHOS_ACE_DIALOG_CALLBACK_STUB_H
#define OHOS_ACE_DIALOG_CALLBACK_STUB_H

#include <memory>
#include <map>

#include "dialog_callback_interface.h"
#include "nocopyable.h"

#include <iremote_object.h>
#include <iremote_stub.h>

namespace OHOS {
namespace Ace {
class DialogCallbackStub : public IRemoteStub<IDialogCallback> {
public:
    explicit DialogCallbackStub(DialogCallback callback);
    virtual ~DialogCallbackStub();

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    void OnDialogCallback(int32_t id, const std::string& event, const std::string& params) override;

private:
    int OnDialogCallBackInner(MessageParcel &data, MessageParcel &reply);

    DialogCallback callback_;

    DISALLOW_COPY_AND_MOVE(DialogCallbackStub);
};

/**
 * @class DialogCallbackRecipient
 * DialogCallbackRecipient notices IRemoteBroker died.
 */
class DialogCallbackRecipient : public IRemoteObject::DeathRecipient {
public:
    using RemoteDiedHandler = std::function<void(const wptr<IRemoteObject> &)>;
    explicit DialogCallbackRecipient(RemoteDiedHandler handler);
    virtual ~DialogCallbackRecipient();
    virtual void OnRemoteDied(const wptr<IRemoteObject> &remote);

private:
    RemoteDiedHandler handler_;
};
}  // namespace Ace
}  // namespace OHOS
#endif  // OHOS_ACE_DIALOG_CALLBACK_STUB_H