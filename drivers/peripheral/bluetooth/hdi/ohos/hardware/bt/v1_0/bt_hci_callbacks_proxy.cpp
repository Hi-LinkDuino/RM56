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

#include "bt_hci_callbacks_proxy.h"
#include <hdf_base.h>
#include <message_option.h>
#include <message_parcel.h>

namespace ohos {
namespace hardware {
namespace bt {
namespace v1_0 {
int32_t BtHciCallbacksProxy::OnInited(BtStatus status)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(BtHciCallbacksProxy::GetDescriptor()) ||
        !data.WriteUint32((uint32_t)status)) {
        HDF_LOGE("%{public}s: write status failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t ec = Remote()->SendRequest(CMD_ON_INITED, data, reply, option);
    if (ec != HDF_SUCCESS) {
        HDF_LOGE("OnInited failed, error code is %{public}d", ec);
        return ec;
    }

    return HDF_SUCCESS;
}

int32_t BtHciCallbacksProxy::OnReceivedHciPacket(BtType type, const std::vector<uint8_t>& data)
{
    MessageParcel localData;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!localData.WriteInterfaceToken(BtHciCallbacksProxy::GetDescriptor())) {
        HDF_LOGE("%s: write interface token failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!localData.WriteUint32((uint32_t)type)) {
        HDF_LOGE("%s: write type failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!localData.WriteUInt8Vector(data)) {
        HDF_LOGE("%s: write data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t ec = Remote()->SendRequest(CMD_ON_RECEIVED_HCI_PACKET, localData, reply, option);
    if (ec != HDF_SUCCESS) {
        HDF_LOGE("OnReceivedHciPacket failed, error code is %d", ec);
        return ec;
    }

    return HDF_SUCCESS;
}
} // v1_0
} // bt
} // hardware
} // ohos
