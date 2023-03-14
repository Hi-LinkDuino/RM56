/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "bt_hci_callbacks_stub.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <hdf_sbuf_ipc.h>

namespace ohos {
namespace hardware {
namespace bt {
namespace v1_0 {
int32_t BtHciCallbacksStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HDF_LOGI("%{public}s, code = %{public}d", __func__, code);
    switch (code) {
        case CMD_ON_INITED:
            if (data.ReadInterfaceToken() != BtHciCallbacksStub::GetDescriptor()) {
                return HDF_ERR_INVALID_PARAM;
            }
            return BtHciCallbacksStubOnInited(data, reply, option);
        case CMD_ON_RECEIVED_HCI_PACKET:
            if (data.ReadInterfaceToken() != BtHciCallbacksStub::GetDescriptor()) {
                return HDF_ERR_INVALID_PARAM;
            }
            return BtHciCallbacksStubOnReceivedHciPacket(data, reply, option);
        default: {
            HDF_LOGE("%s: not support cmd %d", __func__, code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}

int32_t BtHciCallbacksStub::BtHciCallbacksStubOnInited(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    BtStatus status = (BtStatus)data.ReadUint32();

    int32_t ec = OnInited(status);
    if (ec != HDF_SUCCESS) {
        HDF_LOGE("OnInited failed, error code is %d", ec);
        return ec;
    }

    return HDF_SUCCESS;
}

int32_t BtHciCallbacksStub::BtHciCallbacksStubOnReceivedHciPacket(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    BtType type = (BtType)data.ReadUint32();

    std::vector<uint8_t> raw;
    if (!data.ReadUInt8Vector(&raw)) {
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t ec = OnReceivedHciPacket(type, raw);
    if (ec != HDF_SUCCESS) {
        HDF_LOGE("OnReceivedHciPacket failed, error code is %d", ec);
        return ec;
    }

    return HDF_SUCCESS;
}
}  // namespace v1_0
}  // namespace bt
}  // namespace hardware
}  // namespace ohos
