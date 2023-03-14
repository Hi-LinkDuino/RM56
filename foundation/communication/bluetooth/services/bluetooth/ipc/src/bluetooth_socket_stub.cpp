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

#include "bluetooth_socket_stub.h"
#include "bluetooth_bt_uuid.h"
#include "bluetooth_log.h"
#include "ipc_types.h"

namespace OHOS {
namespace Bluetooth {
BluetoothSocketStub::BluetoothSocketStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(IBluetoothSocket::Code::SOCKET_CONNECT)] = &BluetoothSocketStub::ConnectInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothSocket::Code::SOCKET_LISTEN)] = &BluetoothSocketStub::ListenInner;
}

BluetoothSocketStub::~BluetoothSocketStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int32_t BluetoothSocketStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothSocketStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d", code, option.GetFlags());

    std::u16string descriptor = BluetoothSocketStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOGE("BluetoothSocketStub::OnRemoteRequest, local descriptor is not equal to remote");
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

ErrCode BluetoothSocketStub::ConnectInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothSocketStub: ConnectInner starts");
    std::string addr = data.ReadString();
    std::shared_ptr<BluetoothUuid> uuid(data.ReadParcelable<BluetoothUuid>());
    if (uuid == nullptr) {
        HILOGE("BluetoothSocketStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    int32_t securityFlag = data.ReadInt32();
    int32_t type = data.ReadInt32();

    int fd = Connect(addr, *uuid, securityFlag, type);

    bool ret = reply.WriteFileDescriptor(fd);
    if (!ret) {
        HILOGE("BluetoothSocketStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }

    return NO_ERROR;
}

ErrCode BluetoothSocketStub::ListenInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothSocketStub: ListenInner starts");
    std::string name = data.ReadString();
    std::shared_ptr<BluetoothUuid> uuid(data.ReadParcelable<BluetoothUuid>());
    if (uuid == nullptr) {
        HILOGE("BluetoothSocketStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    int32_t securityFlag = data.ReadInt32();
    int32_t type = data.ReadInt32();

    int fd = Listen(name, *uuid, securityFlag, type);

    bool ret = reply.WriteFileDescriptor(fd);
    if (!ret) {
        HILOGE("BluetoothSocketStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }

    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS