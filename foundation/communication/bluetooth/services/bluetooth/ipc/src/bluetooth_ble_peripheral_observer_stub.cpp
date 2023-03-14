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

#include "bluetooth_ble_peripheral_observer_stub.h"
#include "bluetooth_log.h"
#include "ipc_types.h"
#include "string_ex.h"

namespace OHOS {
namespace Bluetooth {
BluetoothBlePeripheralObserverStub::BluetoothBlePeripheralObserverStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_ = {
        {static_cast<uint32_t>(IBluetoothBlePeripheralObserver::Code::BLE_ON_READ_REMOTE_RSSI_EVENT),
            &BluetoothBlePeripheralObserverStub::OnReadRemoteRssiEventInner},
        {static_cast<uint32_t>(IBluetoothBlePeripheralObserver::Code::BLE_PAIR_STATUS_CHANGED),
            &BluetoothBlePeripheralObserverStub::OnPairStatusChangedInner},
    };
}

BluetoothBlePeripheralObserverStub::~BluetoothBlePeripheralObserverStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int BluetoothBlePeripheralObserverStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BleCentralManagerCallBackStub::OnRemoteRequest, cmd = %d, flags= %d", code, option.GetFlags());
    std::u16string descriptor = BluetoothBlePeripheralObserverStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOGI("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    HILOGW("BleCentralManagerCallBackStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothBlePeripheralObserverStub::OnReadRemoteRssiEventInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    const int32_t rssi = static_cast<int32_t>(data.ReadInt32());
    const int32_t status = static_cast<int32_t>(data.ReadInt32());

    OnReadRemoteRssiEvent(*device, rssi, status);
    return NO_ERROR;
}

ErrCode BluetoothBlePeripheralObserverStub::OnPairStatusChangedInner(MessageParcel &data, MessageParcel &reply)
{
    const int32_t transport = static_cast<int32_t>(data.ReadInt32());
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    const int32_t status = static_cast<int32_t>(data.ReadInt32());

    OnPairStatusChanged(transport, *device, status);
    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS