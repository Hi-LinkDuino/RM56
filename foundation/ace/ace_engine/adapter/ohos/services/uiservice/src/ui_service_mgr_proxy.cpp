/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "ui_service_mgr_proxy.h"

#include "errors.h"
#include "string_ex.h"
#include "ui_service_mgr_errors.h"
#include "ui_service_proxy.h"
#include "ui_service_stub.h"

namespace OHOS {
namespace Ace {
bool UIServiceMgrProxy::WriteInterfaceToken(MessageParcel& data)
{
    if (!data.WriteInterfaceToken(UIServiceMgrProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

int UIServiceMgrProxy::RegisterCallBack(const AAFwk::Want& want, const sptr<IUIService>& uiService)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        return UI_SERVICE_PROXY_INNER_ERR;
    }
    if (!data.WriteParcelable(&want)) {
        HILOG_ERROR("register callback fail, want error");
        return ERR_INVALID_VALUE;
    }
    if (uiService == nullptr) {
        HILOG_ERROR("register callback fail, uiService is nullptr");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteRemoteObject(uiService->AsObject())) {
        HILOG_ERROR("register callback fail, uiService error");
        return ERR_INVALID_VALUE;
    }
    int error = Remote()->SendRequest(IUIServiceMgr::REGISTER_CALLBACK, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("register callback fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int UIServiceMgrProxy::UnregisterCallBack(const AAFwk::Want& want)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return UI_SERVICE_PROXY_INNER_ERR;
    }
    data.WriteParcelable(&want);
    int error = Remote()->SendRequest(IUIServiceMgr::UNREGISTER_CALLBACK, data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("unregister callback fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int UIServiceMgrProxy::Push(const AAFwk::Want& want, const std::string& name, const std::string& jsonPath,
    const std::string& data, const std::string& extraData)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(dataParcel)) {
        return UI_SERVICE_PROXY_INNER_ERR;
    }
    dataParcel.WriteParcelable(&want);
    if (!dataParcel.WriteString(name)) {
        HILOG_ERROR("fail to WriteString name");
        return INVALID_DATA;
    }
    if (!dataParcel.WriteString(jsonPath)) {
        HILOG_ERROR("fail to WriteString jsonPath");
        return INVALID_DATA;
    }
    if (!dataParcel.WriteString(data)) {
        HILOG_ERROR("fail to WriteString data");
        return INVALID_DATA;
    }
    if (!dataParcel.WriteString(extraData)) {
        HILOG_ERROR("fail to WriteString extraData");
        return INVALID_DATA;
    }
    int error = Remote()->SendRequest(IUIServiceMgr::PUSH, dataParcel, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Push fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int UIServiceMgrProxy::Request(const AAFwk::Want& want, const std::string& name, const std::string& data)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(dataParcel)) {
        return UI_SERVICE_PROXY_INNER_ERR;
    }
    dataParcel.WriteParcelable(&want);

    if (!dataParcel.WriteString(name)) {
        HILOG_ERROR("fail to WriteString name");
        return INVALID_DATA;
    }

    if (!dataParcel.WriteString(data)) {
        HILOG_ERROR("fail to WriteString data");
        return INVALID_DATA;
    }

    int error = Remote()->SendRequest(IUIServiceMgr::REQUEST, dataParcel, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Request fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int UIServiceMgrProxy::ReturnRequest(const AAFwk::Want& want, const std::string& source, const std::string& data,
    const std::string& extraData)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(dataParcel)) {
        return UI_SERVICE_PROXY_INNER_ERR;
    }
    dataParcel.WriteParcelable(&want);

    if (!dataParcel.WriteString(source)) {
        HILOG_ERROR("fail to WriteString source");
        return INVALID_DATA;
    }

    if (!dataParcel.WriteString(data)) {
        HILOG_ERROR("fail to WriteString data");
        return INVALID_DATA;
    }
    if (!dataParcel.WriteString(extraData)) {
        HILOG_ERROR("fail to WriteString extraData");
        return INVALID_DATA;
    }
    int error = Remote()->SendRequest(IUIServiceMgr::RETURN_REQUEST, dataParcel, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Request fail, error: %d", error);
        return error;
    }
    return reply.ReadInt32();
}

int UIServiceMgrProxy::ShowDialog(const std::string& name,
                                  const std::string& params,
                                  OHOS::Rosen::WindowType windowType,
                                  int x,
                                  int y,
                                  int width,
                                  int height,
                                  const sptr<OHOS::Ace::IDialogCallback>& dialogCallback,
                                  int* id)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(dataParcel)) {
        return UI_SERVICE_PROXY_INNER_ERR;
    }

    if (!dataParcel.WriteString(name)) {
        HILOG_ERROR("fail to WriteString name");
        return INVALID_DATA;
    }

    if (!dataParcel.WriteString(params)) {
        HILOG_ERROR("fail to WriteString params");
        return INVALID_DATA;
    }

    if (!dataParcel.WriteUint32(static_cast<uint32_t>(windowType))) {
        HILOG_ERROR("fail to WriteUInt32 windowType");
        return INVALID_DATA;
    }

    if (!dataParcel.WriteInt32(x)) {
        HILOG_ERROR("fail to WriteInt32 x");
        return INVALID_DATA;
    }

    if (!dataParcel.WriteInt32(y)) {
        HILOG_ERROR("fail to WriteInt32 y");
        return INVALID_DATA;
    }
    if (!dataParcel.WriteInt32(width)) {
        HILOG_ERROR("fail to WriteInt32 width");
        return INVALID_DATA;
    }
    if (!dataParcel.WriteInt32(height)) {
        HILOG_ERROR("fail to WriteInt32 height");
        return INVALID_DATA;
    }

    if (dialogCallback == nullptr) {
        HILOG_ERROR("dialogCallback is nullptr");
        return ERR_INVALID_VALUE;
    }
    if (!dataParcel.WriteRemoteObject(dialogCallback->AsObject())) {
        HILOG_ERROR("dialogCallback error");
        return ERR_INVALID_VALUE;
    }

    int error = Remote()->SendRequest(IUIServiceMgr::SHOW_DIALOG, dataParcel, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Request fail, error: %{public}d", error);
        return error;
    }

    if (id != nullptr) {
        *id = reply.ReadInt32();
    }
    return reply.ReadInt32();
}

int UIServiceMgrProxy::CancelDialog(int id)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(dataParcel)) {
        return UI_SERVICE_PROXY_INNER_ERR;
    }

    if (!dataParcel.WriteInt32(id)) {
        HILOG_ERROR("fail to WriteString id");
        return INVALID_DATA;
    }

    int error = Remote()->SendRequest(IUIServiceMgr::CANCEL_DIALOG, dataParcel, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("Request fail, error: %{public}d", error);
        return error;
    }
    return reply.ReadInt32();
}
}  // namespace Ace
}  // namespace OHOS
