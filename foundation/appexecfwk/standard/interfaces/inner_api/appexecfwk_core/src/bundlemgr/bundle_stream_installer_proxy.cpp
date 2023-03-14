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

#include "bundle_stream_installer_proxy.h"

#include <unistd.h>

#include "app_log_wrapper.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
BundleStreamInstallerProxy::BundleStreamInstallerProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IBundleStreamInstaller>(object)
{
    APP_LOGD("create bundle stream installer proxy instance");
}

BundleStreamInstallerProxy:: ~BundleStreamInstallerProxy()
{
    APP_LOGD("destory bundle stream installer proxy instance");
}

int BundleStreamInstallerProxy::CreateStream(const std::string &hapName, long offset)
{
    APP_LOGD("bundle stream installer proxy create stream begin");
    int fd = -1;
    if (hapName.empty()) {
        APP_LOGE("BundleStreamInstallerProxy create stream faile due to empty hapName");
        return fd;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(BundleStreamInstallerProxy::GetDescriptor())) {
        APP_LOGE("fail to CreateStream due to write interface token fail");
        return fd;
    }
    if (!data.WriteString(hapName)) {
        APP_LOGE("fail to CreateStream due to write hapName fail");
        return fd;
    }
    if (!data.WriteInt64(offset)) {
        APP_LOGE("fail to GetBundleInfo due to write offset fail");
        return fd;
    }

    MessageParcel reply;
    if (!SendStreamInstallRequest(IBundleStreamInstaller::StreamMessage::CREATE_STREAM, data, reply)) {
        APP_LOGE("fail to SendStreamInstallRequest");
        return fd;
    }

    int sharedFd = reply.ReadFileDescriptor();
    if (sharedFd < 0) {
        APP_LOGE("fail to CreateStream");
        return fd;
    }

    fd = dup(sharedFd);
    close(sharedFd);

    APP_LOGD("bundle stream installer proxy create stream end");
    return fd;
}

bool BundleStreamInstallerProxy::Install(const sptr<IStatusReceiver>& receiver)
{
    APP_LOGD("bundle stream installer proxy install begin");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BundleStreamInstallerProxy::GetDescriptor())) {
        APP_LOGE("fail to Install due to write interface token fail");
        return false;
    }
    if (!receiver) {
        APP_LOGE("fail to install, for receiver is nullptr");
        return false;
    }
    if (!data.WriteObject<IRemoteObject>(receiver->AsObject())) {
        APP_LOGE("write parcel failed");
        return false;
    }

    MessageParcel reply;
    bool res = SendStreamInstallRequest(IBundleStreamInstaller::StreamMessage::STREAM_INSTALL, data, reply);
    if (!res) {
        APP_LOGE("fail to SendStreamInstallRequest");
        return res;
    }
    APP_LOGD("bundle stream installer proxy install end");
    return reply.ReadBool();
}

uint32_t BundleStreamInstallerProxy::GetInstallerId() const
{
    return installerId_;
}

void BundleStreamInstallerProxy::SetInstallerId(uint32_t installerId)
{
    installerId_ = installerId;
}

bool BundleStreamInstallerProxy::SendStreamInstallRequest(const uint32_t& code, MessageParcel& data,
    MessageParcel& reply)
{
    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        APP_LOGE("fail to send request, for remote is nullptr");
        return false;
    }

    MessageOption option(MessageOption::TF_SYNC);
    int32_t ret = remote->SendRequest(code, data, reply, option);
    if (ret != NO_ERROR) {
        APP_LOGE("fail to sendRequest, for transact is failed and error code is: %{public}d", ret);
        return false;
    }
    return true;
}
} // AppExecFwk
} // OHOS