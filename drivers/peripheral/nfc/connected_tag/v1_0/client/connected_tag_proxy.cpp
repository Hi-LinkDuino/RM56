/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include <hdf_base.h>
#include <message_option.h>
#include <message_parcel.h>
#include "cmd_def.h"
#include "connected_tag_proxy.h"

namespace OHOS {
namespace HDI {
namespace NFC {
namespace V1_0 {
sptr<IConnectedTagHdi> IConnectedTagHdi::Get()
{
    do {
        using namespace OHOS::HDI::ServiceManager::V1_0;
        auto servMgr = IServiceManager::Get();
        if (servMgr == nullptr) {
            HDF_LOGE("%s:get IServiceManager failed!", __func__);
            break;
        }

        sptr<IRemoteObject> remote = servMgr->GetService("ConnectedTagHdiService");
        if (remote != nullptr) {
            return iface_cast<IConnectedTagHdi>(remote);
        }
    } while (false);
    HDF_LOGE("%s: get IConnectedTagHdi failed!", __func__);
    return nullptr;
}

int32_t ConnectedTagProxy::Init()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        HDF_LOGE("%s: write interface token failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ec = Remote()->SendRequest(CMD_INIT, data, reply, option);
    if (ec != HDF_SUCCESS) {
        HDF_LOGE("CMD_INIT failed, error code is = %{public}d", ec);
        return ec;
    }

    return HDF_SUCCESS;
}

int32_t ConnectedTagProxy::Uninit()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        HDF_LOGE("%s: write interface token failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ec = Remote()->SendRequest(CMD_UNINIT, data, reply, option);
    if (ec != HDF_SUCCESS) {
        HDF_LOGE("CMD_UNINIT failed, error code is = %{public}d", ec);
        return ec;
    }

    return HDF_SUCCESS;
}
std::string ConnectedTagProxy::ReadNdefTag()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    std::string ndefData = "";
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        HDF_LOGE("%s: write interface token failed!", __func__);
        return "";
    }
    int32_t ec = Remote()->SendRequest(CMD_READ_NDEF, data, reply, option);
    int exception = reply.ReadInt32();
    if (ec != HDF_SUCCESS || exception != HDF_SUCCESS) {
        HDF_LOGE("CMD_READ_NDEF failed, ec = %{public}d, exception = %{public}d", ec, exception);
        return "";
    }

    ndefData = reply.ReadString();
    return ndefData;
}

int32_t ConnectedTagProxy::WriteNdefTag(std::string ndefData)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        HDF_LOGE("%s: write interface token failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    data.WriteInt32(HDF_SUCCESS);
    data.WriteString(ndefData);
    HDF_LOGE("CMD_WRITE_NDEF ndefData = %{public}s, len = %{public}d", ndefData.c_str(), ndefData.length());

    int32_t ec = Remote()->SendRequest(CMD_WRITE_NDEF, data, reply, option);
    if (ec != HDF_SUCCESS) {
        HDF_LOGE("CMD_WRITE_NDEF failed, error code is = %{public}d", ec);
        return ec;
    }
    int exception = reply.ReadInt32();
    if (exception != HDF_SUCCESS) {
        return exception;
    }
    return reply.ReadInt32();
}
}  // namespace V1_0
}  // namespace NFC
}  // namespace HDI
}  // namespace OHOS
