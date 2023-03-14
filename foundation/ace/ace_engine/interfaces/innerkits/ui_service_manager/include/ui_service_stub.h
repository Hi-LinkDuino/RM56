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
#ifndef OHOS_AAFWK_UI_SERVICE_STUB_H
#define OHOS_AAFWK_UI_SERVICE_STUB_H

#include <memory>
#include <map>

#include "ui_service_interface.h"
#include "nocopyable.h"

#include <iremote_object.h>
#include <iremote_stub.h>

namespace OHOS {
namespace Ace {
class UIServiceStub : public IRemoteStub<IUIService> {
public:
    UIServiceStub();
    virtual ~UIServiceStub();

    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;

private:

    int OnRequestCallBackInner(MessageParcel& data, MessageParcel& reply);
    int OnPushCallBackInner(MessageParcel& data, MessageParcel& reply);
    int OnReturnRequestInner(MessageParcel& data, MessageParcel& reply);
    using RequestFuncType = int (UIServiceStub::*)(MessageParcel& data, MessageParcel& reply);
    std::map<uint32_t, RequestFuncType> requestFuncMap_;

    DISALLOW_COPY_AND_MOVE(UIServiceStub);
};

/**
 * @class UIServiceCallbackRecipient
 * UIServiceCallbackRecipient notices IRemoteBroker died.
 */
class UIServiceCallbackRecipient : public IRemoteObject::DeathRecipient {
public:
    using RemoteDiedHandler = std::function<void(const wptr<IRemoteObject>&)>;
    explicit UIServiceCallbackRecipient(RemoteDiedHandler handler);
    virtual ~UIServiceCallbackRecipient();
    virtual void OnRemoteDied(const wptr<IRemoteObject>& remote);

private:
    RemoteDiedHandler handler_;
};
}  // namespace Ace
}  // namespace OHOS
#endif  // OHOS_AAFWK_UI_SERVICE_STUB_H
