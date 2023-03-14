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


#include "bt_hci_stub.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <hdf_sbuf_ipc.h>

namespace ohos {
namespace hardware {
namespace bt {
namespace v1_0 {
int32_t BtHciStub::BtHciStubOnRemoteRequest(int cmdId,
    MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    switch (cmdId) {
        case static_cast<int>(HciCmd::CMD_INIT):
            return BtHciStubInit(data, reply, option);
        case static_cast<int>(HciCmd::CMD_SEND_HCI_PACKET):
            return BtHciStubSendHciPacket(data, reply, option);
        case static_cast<int>(HciCmd::CMD_CLOSE):
            return BtHciStubClose(data, reply, option);
        default: {
            HDF_LOGE("%s: not support cmd %d", __func__, cmdId);
            return HDF_ERR_INVALID_PARAM;
        }
    }
}

int32_t BtHciStub::BtHciStubInit(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    hciCallbacks_ = iface_cast<IBtHciCallbacks>(data.ReadRemoteObject());
    int32_t ec = service_.Init(hciCallbacks_);
    if (ec != HDF_SUCCESS) {
        HDF_LOGE("Init failed, error code is %d", ec);
        return ec;
    }

    return HDF_SUCCESS;
}

int32_t BtHciStub::BtHciStubSendHciPacket(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    BtType type = (BtType)data.ReadUint32();

    std::vector<uint8_t> localData;
    if (!data.ReadUInt8Vector(&localData)) {
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t ec = service_.SendHciPacket(type, localData);
    if (ec != HDF_SUCCESS) {
        HDF_LOGE("SendHciPacket failed, error code is %d", ec);
        return ec;
    }

    return HDF_SUCCESS;
}

int32_t BtHciStub::BtHciStubClose(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    int32_t ec = service_.Close();
    if (ec != HDF_SUCCESS) {
        HDF_LOGE("Close failed, error code is %d", ec);
        return ec;
    }
    hciCallbacks_ = nullptr;

    return HDF_SUCCESS;
}
} // v1_0
} // bt
} // hardware
} // ohos

void *BtHciStubInstance()
{
    using namespace ohos::hardware::bt::v1_0;
    return reinterpret_cast<void *>(new BtHciStub());
}

void BtHciStubRelease(void *obj)
{
    using namespace ohos::hardware::bt::v1_0;
    delete reinterpret_cast<BtHciStub *>(obj);
}

int32_t BtHciServiceOnRemoteRequest(void *stub, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    using namespace ohos::hardware::bt::v1_0;
    BtHciStub *bthciStub = reinterpret_cast<BtHciStub *>(stub);
    OHOS::MessageParcel *dataParcel = nullptr;
    OHOS::MessageParcel *replyParcel = nullptr;

    (void)SbufToParcel(reply, &replyParcel);
    if (SbufToParcel(data, &dataParcel) != HDF_SUCCESS) {
        HDF_LOGE("%s:invalid data sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::MessageOption option;
    return bthciStub->BtHciStubOnRemoteRequest(cmdId, *dataParcel, *replyParcel, option);
}
