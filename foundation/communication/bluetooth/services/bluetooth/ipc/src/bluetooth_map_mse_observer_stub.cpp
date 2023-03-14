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

#include "bluetooth_map_mse_observer_stub.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
BluetoothMapMseObserverStub::BluetoothMapMseObserverStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(
        IBluetoothMapMseObserver::Code::MSE_ON_CONNECTION_STATE_CHANGED)] =
        &BluetoothMapMseObserverStub::OnConnectionStateChangedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMseObserver::Code::MSE_ON_PERMISSION)] =
        &BluetoothMapMseObserverStub::OnPermissionInner;
}

BluetoothMapMseObserverStub::~BluetoothMapMseObserverStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int BluetoothMapMseObserverStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothMapMseObserverStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d",
        code,
        option.GetFlags());
    std::u16string descriptor = BluetoothMapMseObserverStub::GetDescriptor();
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
    HILOGW("BluetoothMapMseObserverStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothMapMseObserverStub::OnConnectionStateChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMseObserverStub::OnConnectionStateChangedInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> addr(data.ReadParcelable<BluetoothRawAddress>());
    if (!addr) {
        return TRANSACTION_ERR;
    }
    int32_t status = data.ReadInt32();
    OnConnectionStateChanged(*addr, status);
    return NO_ERROR;
}

ErrCode BluetoothMapMseObserverStub::OnPermissionInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMseObserverStub::OnPermissionInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> addr(data.ReadParcelable<BluetoothRawAddress>());
    if (!addr) {
        return TRANSACTION_ERR;
    }
    OnPermission(*addr);
    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS
