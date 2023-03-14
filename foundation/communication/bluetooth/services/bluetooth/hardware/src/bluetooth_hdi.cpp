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

#include "bluetooth_hdi.h"
#include <v1_0/ihci_interface.h>
#include "bluetooth_hci_callbacks.h"

using OHOS::HDI::Bluetooth::Hci::V1_0::IHciInterface;

static OHOS::sptr<IHciInterface> g_iBtHci = nullptr;
static OHOS::sptr<BluetoothHciCallbacks> g_bluetoothHciCallbacks = nullptr;

int HdiInit(BtHciCallbacks *callbacks)
{
    if (callbacks == nullptr) {
        return INITIALIZATION_ERROR;
    }

    OHOS::sptr<IHciInterface> iBtHci = IHciInterface::Get();
    if (iBtHci == nullptr) {
        return INITIALIZATION_ERROR;
    }

    g_bluetoothHciCallbacks = new (std::nothrow) BluetoothHciCallbacks(callbacks);
    if (g_bluetoothHciCallbacks == nullptr) {
        return INITIALIZATION_ERROR;
    }

    int32_t result = iBtHci->Init(g_bluetoothHciCallbacks);
    if (result != BtStatus::SUCCESS) {
        return INITIALIZATION_ERROR;
    }

    g_iBtHci = iBtHci;

    return SUCCESS;
}

int HdiSendHciPacket(BtPacketType type, const BtPacket *packet)
{
    if (packet == nullptr) {
        return TRANSPORT_ERROR;
    }
    if (g_iBtHci == nullptr) {
        return INITIALIZATION_ERROR;
    }
    BtType btType = BtType::ACL_DATA;
    switch (type) {
        case PACKET_TYPE_CMD:
            btType = BtType::HCI_CMD;
            break;
        case PACKET_TYPE_ACL:
            btType = BtType::ACL_DATA;
            break;
        case PACKET_TYPE_SCO:
            btType = BtType::SCO_DATA;
            break;
        default:
            break;
    }
    std::vector<uint8_t> data;
    data.assign(packet->data, packet->data + packet->size);
    int32_t result = g_iBtHci->SendHciPacket(btType, data);
    if (result != BtStatus::SUCCESS) {
        return TRANSPORT_ERROR;
    }
    return SUCCESS;
}

void HdiClose()
{
    if (g_bluetoothHciCallbacks != nullptr) {
        g_bluetoothHciCallbacks = nullptr;
    }

    if (g_iBtHci != nullptr) {
        g_iBtHci->Close();
        g_iBtHci = nullptr;
    }
}
