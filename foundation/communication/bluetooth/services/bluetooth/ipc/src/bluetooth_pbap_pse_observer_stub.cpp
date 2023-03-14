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

#include "bluetooth_pbap_pse_observer_stub.h"
#include "bluetooth_log.h"
#include "ipc_types.h"
#include "string_ex.h"
namespace OHOS {
namespace Bluetooth {
BluetoothPbapPseObserverStub::BluetoothPbapPseObserverStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPseObserverStub::Code::PBAP_PSE_ON_SERVICE_CONNECTION_STATE_CHANGE)] =
        &BluetoothPbapPseObserverStub::OnServiceConnectionStateChangedInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPseObserverStub::Code::PBAP_PSE_ON_SERVICE_PASSWORD_REQUIRED)] =
        &BluetoothPbapPseObserverStub::OnServicePermissionInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPseObserverStub::Code::PBAP_PSE_ON_SERVICE_PASSWORD_REQUIRED)] =
        &BluetoothPbapPseObserverStub::OnServicePasswordRequiredInner;
}

BluetoothPbapPseObserverStub::~BluetoothPbapPseObserverStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}
int BluetoothPbapPseObserverStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothPbapPseObserverStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d", code, option.GetFlags());
    std::u16string descriptor = BluetoothPbapPseObserverStub::GetDescriptor();
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
    HILOGW("BluetoothPbapPseObserverStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
};

ErrCode BluetoothPbapPseObserverStub::OnServiceConnectionStateChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int state = data.ReadInt32();

    OnServiceConnectionStateChanged(*device, state);
    
    return NO_ERROR;
}

ErrCode BluetoothPbapPseObserverStub::OnServicePasswordRequiredInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    std::vector<uint8_t>* des = nullptr;
    if (!data.ReadUInt8Vector(des)) {
        HILOGW("BluetoothPbapPseObserverStub::OnServicePasswordRequiredInner: get description failed.");
        return INVALID_DATA;
    }
    const ::std::vector<uint8_t> description = *des;
    int8_t charset = data.ReadInt8();
    bool fullAccess = data.ReadBool();
    
    OnServicePasswordRequired(*device, description, charset, fullAccess);
    
    return NO_ERROR;
}

ErrCode BluetoothPbapPseObserverStub::OnServicePermissionInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }

    OnServicePermission(*device);
    
    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS