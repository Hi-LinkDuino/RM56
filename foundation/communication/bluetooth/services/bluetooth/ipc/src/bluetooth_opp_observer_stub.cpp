/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "bluetooth_opp_observer_stub.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
int BluetoothOppObserverStub::OnRemoteRequest(
    uint32_t code,
    MessageParcel& data,
    MessageParcel& reply,
    MessageOption& option)
{
    HILOGI("BluetoothOppObserverStub OnRemoteRequest start");
    switch (code) {
        case COMMAND_ON_RECEIVE_INCOMING_FILE_CHANGED: {
            std::u16string descriptor = BluetoothOppObserverStub::GetDescriptor();
            std::u16string remoteDescriptor = data.ReadInterfaceToken();
            if (descriptor != remoteDescriptor) {
                HILOGE("local descriptor is not equal to remote");
                return IPC_INVOKER_TRANSLATE_ERR;
            }
            const BluetoothIOppTransferInformation *oppInformation =
                data.ReadParcelable<BluetoothIOppTransferInformation>();
            if (oppInformation == nullptr) {
                return ERR_NULL_OBJECT;
            }
            OnReceiveIncomingFileChanged(*oppInformation);
            HILOGE("COMMAND_ON_RECEIVE_INCOMING_FILE_CHANGED end");
            return NO_ERROR;
        }
        case COMMAND_ON_TRANSFER_STATE_CHANGED: {
            std::u16string descriptor = BluetoothOppObserverStub::GetDescriptor();
            std::u16string remoteDescriptor = data.ReadInterfaceToken();
            if (descriptor != remoteDescriptor) {
                HILOGE("local descriptor is not equal to remote");
                return IPC_INVOKER_TRANSLATE_ERR;
            }
            const BluetoothIOppTransferInformation *oppInformation =
                data.ReadParcelable<BluetoothIOppTransferInformation>();
            if (oppInformation == nullptr) {
                return ERR_NULL_OBJECT;
            }
            OnTransferStateChanged(*oppInformation);
            HILOGE("COMMAND_ON_TRANSFER_STATE_CHANGED end");
            return NO_ERROR;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return ERR_TRANSACTION_FAILED;
}
}  // namespace Bluetooth
}  // namespace OHOS