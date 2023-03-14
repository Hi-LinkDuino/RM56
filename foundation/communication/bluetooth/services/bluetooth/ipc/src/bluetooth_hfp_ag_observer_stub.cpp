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


#include "bluetooth_hfp_ag_observer_stub.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
BluetoothHfpAgObserverStub::BluetoothHfpAgObserverStub() {
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgObserverStub::Code::BT_HFP_AG_OBSERVER_CONNECTION_STATE_CHANGED)] =
        &BluetoothHfpAgObserverStub::OnConnectionStateChangedInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgObserverStub::Code::BT_HFP_AG_OBSERVER_SCO_STATE_CHANGED)] =
        &BluetoothHfpAgObserverStub::OnScoStateChangedInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgObserverStub::Code::BT_HFP_AG_OBSERVER_ACTIVE_DEVICE_CHANGED)] =
        &BluetoothHfpAgObserverStub::OnActiveDeviceChangedInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgObserverStub::Code::BT_HFP_AG_OBSERVER_HF_ENHANCED_DRIVER_SAFETY_CHANGED)] =
        &BluetoothHfpAgObserverStub::OnHfEnhancedDriverSafetyChangedInner;

    HILOGD("%{public}s ends.", __func__);
}

BluetoothHfpAgObserverStub::~BluetoothHfpAgObserverStub() {
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int BluetoothHfpAgObserverStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, 
    MessageOption &option) {
    HILOGD("BluetoothHfpAgObserverStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d", code, option.GetFlags());
    std::u16string descriptor = BluetoothHfpAgObserverStub::GetDescriptor();
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
    HILOGW("BluetoothHfpAgObserverStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);   
}

ErrCode BluetoothHfpAgObserverStub::OnConnectionStateChangedInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int state = data.ReadInt32();
    OnConnectionStateChanged(*device, state);
    return NO_ERROR;
}

ErrCode BluetoothHfpAgObserverStub::OnScoStateChangedInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int state = data.ReadInt32();
    OnScoStateChanged(*device, state);
    return NO_ERROR;
}

ErrCode BluetoothHfpAgObserverStub::OnActiveDeviceChangedInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    OnActiveDeviceChanged(*device);
    return NO_ERROR;
}

ErrCode BluetoothHfpAgObserverStub::OnHfEnhancedDriverSafetyChangedInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int indValue = data.ReadInt32();
    OnScoStateChanged(*device, indValue);
    return NO_ERROR;
}


}  // namespace Bluetooth
}  // namespace OHOS
