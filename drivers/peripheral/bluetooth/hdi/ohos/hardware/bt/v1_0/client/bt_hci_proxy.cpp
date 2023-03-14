/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "bt_hci_proxy.h"
#include <hdf_base.h>
#include <message_option.h>
#include <message_parcel.h>

namespace ohos {
namespace hardware {
namespace bt {
namespace v1_0 {
sptr<IBtHci> IBtHci::Get()
{
    do {
        using namespace OHOS::HDI::ServiceManager::V1_0;
        auto servMgr = IServiceManager::Get();
        if (servMgr == nullptr) {
            HDF_LOGE("%s:get IServiceManager failed!", __func__);
            break;
        }

        sptr<IRemoteObject> remote = servMgr->GetService("BtHciService");
        if (remote != nullptr) {
            return iface_cast<IBtHci>(remote);
        }
    } while (false);
    HDF_LOGE("%s: get BtHciService failed!", __func__);
    return nullptr;
}

int32_t BtHciProxy::Init(const sptr<IBtHciCallbacks> &callbacks)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteRemoteObject(callbacks->AsObject())) {
        HDF_LOGE("%s: write callbacks failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t ec = Remote()->SendRequest((uint32_t)HciCmd::CMD_INIT, data, reply, option);
    if (ec != HDF_SUCCESS) {
        HDF_LOGE("Init failed, error code is %d", ec);
        return ec;
    }

    return HDF_SUCCESS;
}

int32_t BtHciProxy::SendHciPacket(BtType type, const std::vector<uint8_t> &raw)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteUint32((uint32_t)type)) {
        HDF_LOGE("%s: write type failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!data.WriteUInt8Vector(raw)) {
        HDF_LOGE("%s: write raw failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t ec = Remote()->SendRequest((uint32_t)HciCmd::CMD_SEND_HCI_PACKET, data, reply, option);
    if (ec != HDF_SUCCESS) {
        HDF_LOGE("SendHciPacket failed, error code is %d", ec);
        return ec;
    }

    return HDF_SUCCESS;
}

int32_t BtHciProxy::Close()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    int32_t ec = Remote()->SendRequest((uint32_t)HciCmd::CMD_CLOSE, data, reply, option);
    if (ec != HDF_SUCCESS) {
        HDF_LOGE("Close failed, error code is %d", ec);
        return ec;
    }

    return HDF_SUCCESS;
}
}  // namespace v1_0
}  // namespace bt
}  // namespace hardware
}  // namespace ohos
