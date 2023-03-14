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

#include "ui_service_proxy.h"
#include "hilog_wrapper.h"
#include "message_parcel.h"
#include "want.h"

namespace OHOS {
namespace Ace {
UIServiceProxy::UIServiceProxy(const sptr<IRemoteObject>& remote)
    : IRemoteProxy<IUIService>(remote)
{}
UIServiceProxy::~UIServiceProxy()
{}

void UIServiceProxy::OnPushCallBack(const AAFwk::Want& want, const std::string& name, const std::string& jsonPath,
    const std::string& data, const std::string& extraData)
{
    auto remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("%{public}s remote is nullptr", __func__);
        return;
    }

    OHOS::MessageParcel dataParcel;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;

    if (!dataParcel.WriteInterfaceToken(UIServiceProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s dataParcel.WriteInterfaceToken(GetDescriptor()) return false", __func__);
        return;
    }
    if (!dataParcel.WriteParcelable(&want)) {
        HILOG_ERROR("want write failed.");
        return;
    }
    if (!dataParcel.WriteString(name)) {
        HILOG_ERROR("fail to WriteString name");
        return;
    }
    if (!dataParcel.WriteString(jsonPath)) {
        HILOG_ERROR("fail to WriteString jsonPath");
        return;
    }
    if (!dataParcel.WriteString(data)) {
        HILOG_ERROR("fail to WriteString data");
        return;
    }
    if (!dataParcel.WriteString(extraData)) {
        HILOG_ERROR("fail to WriteString extraData");
        return;
    }
    int result = remote->SendRequest(IUIService::UI_SERVICE_PUSH_CALL_BACK, dataParcel, reply, option);
    if (result == ERR_NONE) {
        HILOG_INFO("%{public}s SendRequest ok, retval is %d", __func__, reply.ReadInt32());
        return;
    } else {
        HILOG_ERROR("%{public}s SendRequest error, result=%{public}d", __func__, result);
        return;
    }
}

void UIServiceProxy::OnRequestCallBack(const AAFwk::Want& want, const std::string& name, const std::string& data)
{
    auto remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("%{public}s remote is nullptr", __func__);
        return;
    }

    OHOS::MessageParcel dataParcel;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;

    if (!dataParcel.WriteInterfaceToken(UIServiceProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s dataParcel.WriteInterfaceToken(GetDescriptor()) return false", __func__);
        return;
    }
    if (!dataParcel.WriteParcelable(&want)) {
        HILOG_ERROR("want write failed.");
        return;
    }
    if (!dataParcel.WriteString(name)) {
        HILOG_ERROR("fail to WriteString name");
        return;
    }
    if (!dataParcel.WriteString(data)) {
        HILOG_ERROR("fail to WriteString data");
        return;
    }
    int result = remote->SendRequest(IUIService::UI_SERVICE_REQUEST_CALL_BACK, dataParcel, reply, option);
    if (result == ERR_NONE) {
        HILOG_INFO("%{public}s SendRequest ok, retval is %d", __func__, reply.ReadInt32());
        return;
    } else {
        HILOG_ERROR("%{public}s SendRequest error, result=%{public}d", __func__, result);
        return;
    }
}

void UIServiceProxy::OnReturnRequest(
    const AAFwk::Want& want, const std::string& source, const std::string& data, const std::string& extraData)
{
    auto remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("%{public}s remote is nullptr", __func__);
        return;
    }

    OHOS::MessageParcel dataParcel;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;

    if (!dataParcel.WriteInterfaceToken(UIServiceProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s dataParcel.WriteInterfaceToken(GetDescriptor()) return false", __func__);
        return;
    }
    if (!dataParcel.WriteParcelable(&want)) {
        HILOG_ERROR("want write failed.");
        return;
    }
    if (!dataParcel.WriteString(source)) {
        HILOG_ERROR("fail to WriteString source");
        return;
    }
    if (!dataParcel.WriteString(data)) {
        HILOG_ERROR("fail to WriteString data");
        return;
    }
    if (!dataParcel.WriteString(extraData)) {
        HILOG_ERROR("fail to WriteString extraData");
        return;
    }
    int result = remote->SendRequest(IUIService::UI_SERVICE_RETURN_REQUEST, dataParcel, reply, option);
    if (result == ERR_NONE) {
        HILOG_INFO("%{public}s SendRequest ok, retval is %d", __func__, reply.ReadInt32());
        return;
    } else {
        HILOG_ERROR("%{public}s SendRequest error, result=%{public}d", __func__, result);
        return;
    }
}
}  // namespace Ace
}  // namespace OHOS
