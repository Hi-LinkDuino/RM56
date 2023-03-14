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

#include "bluetooth_host_observer_stub.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
BluetoothHostObserverStub::BluetoothHostObserverStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(IBluetoothHostObserver::Code::BT_HOST_OBSERVER_STATE_CHANGE)] =
        &BluetoothHostObserverStub::OnStateChangedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothHostObserver::Code::BT_HOST_OBSERVER_DISCOVERY_STATE_CHANGE)] =
        &BluetoothHostObserverStub::OnDiscoveryStateChangedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothHostObserver::Code::BT_HOST_OBSERVER_DISCOVERY_RESULT)] =
        &BluetoothHostObserverStub::OnDiscoveryResultInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothHostObserver::Code::BT_HOST_OBSERVER_PAIR_REQUESTED)] =
        &BluetoothHostObserverStub::OnPairRequestedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothHostObserver::Code::BT_HOST_OBSERVER_PAIR_CONFIRMED)] =
        &BluetoothHostObserverStub::OnPairConfirmedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothHostObserver::Code::BT_HOST_OBSERVER_SCAN_MODE_CHANGED)] =
        &BluetoothHostObserverStub::OnScanModeChangedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothHostObserver::Code::BT_HOST_OBSERVER_DEVICE_NAME_CHANGED)] =
        &BluetoothHostObserverStub::OnDeviceNameChangedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothHostObserver::Code::BT_HOST_OBSERVER_DEVICE_ADDR_CHANGED)] =
        &BluetoothHostObserverStub::OnDeviceAddrChangedInner;
}

BluetoothHostObserverStub::~BluetoothHostObserverStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int32_t BluetoothHostObserverStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothHostObserverStub: transaction of code: %{public}d is received", code);
    std::u16string descriptor = BluetoothHostObserverStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOGE("BluetoothHostObserverStub::OnRemoteRequest, local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }

    HILOGW("BluetoothHostObserverStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothHostObserverStub::OnStateChangedInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport = data.ReadInt32();
    int32_t status = data.ReadInt32();

    HILOGI("BluetoothHostObserverStub::OnStateChangedInner starts");
    OnStateChanged(transport, status);

    return NO_ERROR;
}

// ON_DIS_STA_CHANGE_CODE
ErrCode BluetoothHostObserverStub::OnDiscoveryStateChangedInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t status = data.ReadInt32();

    HILOGI("BluetoothHostObserverStub::OnDiscoveryStateChangedInner starts");
    OnDiscoveryStateChanged(status);

    return NO_ERROR;
}

ErrCode BluetoothHostObserverStub::OnDiscoveryResultInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }

    HILOGI("BluetoothHostObserverStub::OnDiscoveryResultInner starts");
    OnDiscoveryResult(*device);

    return NO_ERROR;
}

ErrCode BluetoothHostObserverStub::OnPairRequestedInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport = data.ReadInt32();
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }

    HILOGI("BluetoothHostObserverStub::OnPairRequestedInner starts");
    OnPairRequested(transport, *device);

    return NO_ERROR;
}

ErrCode BluetoothHostObserverStub::OnPairConfirmedInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport = data.ReadInt32();
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int32_t reqType = data.ReadInt32();
    int32_t number = data.ReadInt32();

    HILOGI("BluetoothHostObserverStub::OnPairConfirmedInner starts");
    OnPairConfirmed(transport, *device, reqType, number);

    return NO_ERROR;
}

ErrCode BluetoothHostObserverStub::OnScanModeChangedInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t mode = data.ReadInt32();

    HILOGI("BluetoothHostObserverStub::OnScanModeChangedInner starts");
    OnScanModeChanged(mode);

    return NO_ERROR;
}

ErrCode BluetoothHostObserverStub::OnDeviceNameChangedInner(MessageParcel &data, MessageParcel &reply)
{
    std::string deviceName = data.ReadString();

    HILOGI("BluetoothHostObserverStub::OnDeviceNameChangedInner starts");
    OnDeviceNameChanged(deviceName);

    return NO_ERROR;
}

ErrCode BluetoothHostObserverStub::OnDeviceAddrChangedInner(MessageParcel &data, MessageParcel &reply)
{
    std::string address = data.ReadString();

    HILOGI("BluetoothHostObserverStub::OnDeviceAddrChangedInner starts");
    OnDeviceAddrChanged(address);

    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS
