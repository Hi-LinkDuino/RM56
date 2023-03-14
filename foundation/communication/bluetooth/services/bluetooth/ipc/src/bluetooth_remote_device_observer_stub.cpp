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

#include "bluetooth_remote_device_observer_stub.h"
#include "bluetooth_bt_uuid.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
constexpr uint16_t UUID_SIZE_MAX = 1024;
const std::map<uint32_t, std::function<ErrCode(BluetoothRemoteDeviceObserverstub *, MessageParcel &, MessageParcel &)>>
    BluetoothRemoteDeviceObserverstub::memberFuncMap_ = {
        {IBluetoothRemoteDeviceObserver::Code::BT_REMOTE_DEVICE_OBSERVER_PSIR_STATUS,
            std::bind(&BluetoothRemoteDeviceObserverstub::OnPairStatusChangedInner, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {IBluetoothRemoteDeviceObserver::Code::BT_REMOTE_DEVICE_OBSERVER_REMOTE_UUID,
            std::bind(&BluetoothRemoteDeviceObserverstub::OnRemoteNameUuidChangedInner, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {IBluetoothRemoteDeviceObserver::Code::BT_REMOTE_DEVICE_OBSERVER_REMOTE_NAME,
            std::bind(&BluetoothRemoteDeviceObserverstub::OnRemoteNameChangedInner, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {IBluetoothRemoteDeviceObserver::Code::BT_REMOTE_DEVICE_OBSERVER_ALIAS_CHANGED,
            std::bind(&BluetoothRemoteDeviceObserverstub::OnRemoteAliasChangedInner, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {IBluetoothRemoteDeviceObserver::Code::BT_REMOTE_DEVICE_OBSERVER_REMOTE_COD,
            std::bind(&BluetoothRemoteDeviceObserverstub::OnRemoteCodChangedInner, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {IBluetoothRemoteDeviceObserver::Code::BT_REMOTE_DEVICE_OBSERVER_REMOTE_BATTERY_LEVEL,
            std::bind(&BluetoothRemoteDeviceObserverstub::OnRemoteBatteryLevelChangedInner, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
};

BluetoothRemoteDeviceObserverstub::BluetoothRemoteDeviceObserverstub()
{
    HILOGD("%{public}s start.", __func__);
}

BluetoothRemoteDeviceObserverstub::~BluetoothRemoteDeviceObserverstub()
{
    HILOGD("%{public}s start.", __func__);
}

int BluetoothRemoteDeviceObserverstub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothRemoteDeviceObserverstub::OnRemoteRequest, cmd = %d, flags= %d", code, option.GetFlags());
    std::u16string descriptor = BluetoothRemoteDeviceObserverstub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOGE("BluetoothHostStub::OnRemoteRequest, local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return memberFunc(this, data, reply);
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothRemoteDeviceObserverstub::OnPairStatusChangedInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport = data.ReadInt32();
    std::shared_ptr<BluetoothRawAddress> result(data.ReadParcelable<BluetoothRawAddress>());
    if (!result) {
        return TRANSACTION_ERR;
    }
    int32_t status = data.ReadInt32();
    OnPairStatusChanged(transport, *result, status);
    return NO_ERROR;
}

ErrCode BluetoothRemoteDeviceObserverstub::OnRemoteNameUuidChangedInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothRawAddress> result(data.ReadParcelable<BluetoothRawAddress>());
    if (!result) {
        return TRANSACTION_ERR;
    }
    uint16_t uuidSize = data.ReadUint16();
    if (uuidSize > UUID_SIZE_MAX) {
        HILOGE("%{public}s, uuidSize = %{public}hu exceeds the maximum 1024.", __func__, uuidSize);
        return INVALID_DATA;
    }
    std::vector<bluetooth::Uuid> uuids;
    for (uint16_t i = 0; i < uuidSize; ++i) {
        std::shared_ptr<BluetoothUuid> uuid(data.ReadParcelable<BluetoothUuid>());
        if (!uuid) {
            return TRANSACTION_ERR;
        }
        uuids.push_back(*uuid);
    }
    OnRemoteUuidChanged(*result, uuids);
    return NO_ERROR;
}

ErrCode BluetoothRemoteDeviceObserverstub::OnRemoteNameChangedInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothRawAddress> result(data.ReadParcelable<BluetoothRawAddress>());
    if (!result) {
        return TRANSACTION_ERR;
    }
    std::string deviceName = data.ReadString();
    OnRemoteNameChanged(*result, deviceName);
    return NO_ERROR;
}

ErrCode BluetoothRemoteDeviceObserverstub::OnRemoteAliasChangedInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothRawAddress> result(data.ReadParcelable<BluetoothRawAddress>());
    if (!result) {
        return TRANSACTION_ERR;
    }
    std::string alias = data.ReadString();
    OnRemoteAliasChanged(*result, alias);
    return NO_ERROR;
}

ErrCode BluetoothRemoteDeviceObserverstub::OnRemoteCodChangedInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothRawAddress> result(data.ReadParcelable<BluetoothRawAddress>());
    if (!result) {
        return TRANSACTION_ERR;
    }
    int32_t cod = data.ReadInt32();
    OnRemoteCodChanged(*result, cod);
    return NO_ERROR;
}

ErrCode BluetoothRemoteDeviceObserverstub::OnRemoteBatteryLevelChangedInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothRawAddress> result(data.ReadParcelable<BluetoothRawAddress>());
    if (!result) {
        return TRANSACTION_ERR;
    }
    int32_t batteryLevel = data.ReadInt32();
    OnRemoteBatteryLevelChanged(*result, batteryLevel);
    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS