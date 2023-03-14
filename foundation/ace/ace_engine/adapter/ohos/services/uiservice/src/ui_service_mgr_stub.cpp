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

#include "ui_service_mgr_stub.h"

#include "errors.h"
#include "string_ex.h"
#include "ui_service_mgr_errors.h"
#include "ui_service_proxy.h"
#include "ui_service_stub.h"

namespace OHOS {
namespace Ace {
UIServiceMgrStub::UIServiceMgrStub()
{
    requestFuncMap_[REGISTER_CALLBACK] = &UIServiceMgrStub::RegisterCallBackInner;
    requestFuncMap_[UNREGISTER_CALLBACK] = &UIServiceMgrStub::UnregisterCallBackInner;
    requestFuncMap_[PUSH] = &UIServiceMgrStub::PushInner;
    requestFuncMap_[REQUEST] = &UIServiceMgrStub::RequestInner;
    requestFuncMap_[RETURN_REQUEST] = &UIServiceMgrStub::ReturnRequestInner;
    requestFuncMap_[SHOW_DIALOG] = &UIServiceMgrStub::ShowDialogInner;
    requestFuncMap_[CANCEL_DIALOG] = &UIServiceMgrStub::CancelDialogInner;
}

UIServiceMgrStub::~UIServiceMgrStub()
{
    requestFuncMap_.clear();
}

int UIServiceMgrStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    HILOG_DEBUG("UIServiceMgrStub::OnRemoteRequest, cmd = %d, flags= %d", code, option.GetFlags());
    std::u16string descriptor = UIServiceMgrStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }
    auto itFunc = requestFuncMap_.find(code);
    if (itFunc != requestFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }
    HILOG_WARN("UIServiceMgrStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int UIServiceMgrStub::RegisterCallBackInner(MessageParcel& data, MessageParcel& reply)
{
    std::shared_ptr<AAFwk::Want> want(data.ReadParcelable<AAFwk::Want>());
    if (want == nullptr) {
        HILOG_ERROR("RegisterCallBackInner want is nullptr");
        return ERR_INVALID_VALUE;
    }

    auto object = data.ReadRemoteObject();
    if (object == nullptr) {
        HILOG_ERROR("RegisterCallBackInner read remote object failed");
        return ERR_INVALID_VALUE;
    }

    auto uiService = iface_cast<IUIService>(object);
    int32_t result = RegisterCallBack(*want, uiService);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int UIServiceMgrStub::UnregisterCallBackInner(MessageParcel& data, MessageParcel& reply)
{
    std::shared_ptr<AAFwk::Want> want(data.ReadParcelable<AAFwk::Want>());
    if (want == nullptr) {
        HILOG_ERROR("UnregisterCallBackInner want is nullptr");
        return ERR_INVALID_VALUE;
    }
    int32_t result = UnregisterCallBack(*want);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int UIServiceMgrStub::PushInner(MessageParcel& data, MessageParcel& reply)
{
    std::shared_ptr<AAFwk::Want> want(data.ReadParcelable<AAFwk::Want>());
    if (want == nullptr) {
        HILOG_ERROR("PushInner want is nullptr");
        return ERR_INVALID_VALUE;
    }
    const std::string& name = data.ReadString();
    const std::string& jsonPath = data.ReadString();
    const std::string& dataStr = data.ReadString();
    const std::string& extraData = data.ReadString();

    int32_t result = Push(*want, name, jsonPath, dataStr, extraData);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int UIServiceMgrStub::RequestInner(MessageParcel& data, MessageParcel& reply)
{
    std::shared_ptr<AAFwk::Want> want(data.ReadParcelable<AAFwk::Want>());
    if (want == nullptr) {
        HILOG_ERROR("RequestInner want is nullptr");
        return ERR_INVALID_VALUE;
    }
    const std::string& name = data.ReadString();
    const std::string& dataStr = data.ReadString();
    int32_t result = Request(*want, name, dataStr);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int UIServiceMgrStub::ReturnRequestInner(MessageParcel& data, MessageParcel& reply)
{
    std::shared_ptr<AAFwk::Want> want(data.ReadParcelable<AAFwk::Want>());
    if (want == nullptr) {
        HILOG_ERROR("ReturnRequestInner want is nullptr");
        return ERR_INVALID_VALUE;
    }
    const std::string& source = data.ReadString();
    const std::string& dataStr = data.ReadString();
    const std::string& extraData = data.ReadString();
    int32_t result = ReturnRequest(*want, source, dataStr, extraData);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int UIServiceMgrStub::ShowDialogInner(MessageParcel &data, MessageParcel &reply)
{
    const std::string& name = data.ReadString();
    const std::string& params = data.ReadString();
    auto windowType = static_cast<OHOS::Rosen::WindowType>(data.ReadUint32());
    int x = data.ReadInt32();
    int y = data.ReadInt32();
    int width = data.ReadInt32();
    int height = data.ReadInt32();
    int id = 0;
    auto object = data.ReadRemoteObject();
    if (object == nullptr) {
        HILOG_ERROR("ShowDialogInner read remote object failed");
        return ERR_INVALID_VALUE;
    }
    auto dialogCallback = iface_cast<OHOS::Ace::IDialogCallback>(object);
    int32_t result = ShowDialog(name, params, windowType, x, y, width, height, dialogCallback, &id);
    reply.WriteInt32(id);
    reply.WriteInt32(result);
    return NO_ERROR;
}

int UIServiceMgrStub::CancelDialogInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t id = data.ReadInt32();
    int32_t result = CancelDialog(id);
    reply.WriteInt32(result);
    return NO_ERROR;
}
}  // namespace Ace
}  // namespace OHOS
