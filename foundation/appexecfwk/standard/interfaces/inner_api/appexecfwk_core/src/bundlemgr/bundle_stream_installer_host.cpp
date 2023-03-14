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

#include "bundle_stream_installer_host.h"

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "ipc_types.h"

namespace OHOS {
namespace AppExecFwk {
BundleStreamInstallerHost::BundleStreamInstallerHost()
{
    APP_LOGD("create bundle stream installer host instance");
    init();
}

int BundleStreamInstallerHost::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    APP_LOGD("bundle stream installer host onReceived message, the message code is %{public}u", code);
    std::u16string descriptor = BundleStreamInstallerHost::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        APP_LOGW("[OnRemoteRequest] fail: invalid interface token!");
        return OBJECT_NULL;
    }

    if (funcMap_.find(code) == funcMap_.end()) {
        APP_LOGW("[OnRemoteRequest] fail: unknown code!");
        return IRemoteStub<IBundleStreamInstaller>::OnRemoteRequest(code, data, reply, option);
    }

    return funcMap_[code](data, reply);
}

ErrCode BundleStreamInstallerHost::HandleCreateStream(MessageParcel &data, MessageParcel &reply)
{
    std::string hapName = data.ReadString();
    int64_t offset = data.ReadInt64();
    int32_t fd = CreateStream(hapName, offset);
    if (!reply.WriteFileDescriptor(fd)) {
        APP_LOGE("write fd failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleStreamInstallerHost::HandleInstall(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> object = data.ReadObject<IRemoteObject>();
    if (object == nullptr) {
        reply.WriteBool(false);
        APP_LOGE("read receiver failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    sptr<IStatusReceiver> statusReceiver = iface_cast<IStatusReceiver>(object);
    if (statusReceiver == nullptr) {
        reply.WriteBool(false);
        APP_LOGE("cast remote object to status receiver error");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!Install(statusReceiver)) {
        reply.WriteBool(false);
        APP_LOGE("stream install failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    reply.WriteBool(true);
    return ERR_OK;
}

void BundleStreamInstallerHost::init()
{
    funcMap_.emplace(StreamMessage::CREATE_STREAM, [this](MessageParcel &data, MessageParcel &reply)->ErrCode {
        return this->HandleCreateStream(data, reply);
    });
    funcMap_.emplace(StreamMessage::STREAM_INSTALL, [this](MessageParcel &data, MessageParcel &reply)->ErrCode {
        return this->HandleInstall(data, reply);
    });
}
} // AppExecFwk
} // OHOS