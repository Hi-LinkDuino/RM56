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

#include "bluetooth_pbap_pse_proxy.h"
#include "bluetooth_log.h"
#include "i_bluetooth_pbap_pse.h"
#include "bluetooth_errorcode.h"

namespace OHOS {
namespace Bluetooth {
int BluetoothPbapPseProxy::GetDeviceState(const BluetoothRawAddress& device)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPseProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPseProxy::GetDeviceState WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPseProxy::GetDeviceState error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPse::Code::PBAP_PSE_GET_DEVICE_STATE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPseProxy::GetDeviceState done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

void BluetoothPbapPseProxy::GetDevicesByStates(const std::vector<int32_t> tmpStates, std::vector<BluetoothRawAddress> &rawDevices)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    HILOGI("BluetoothPbapPseProxy::GetDevicesByStates start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPseProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPseProxy::GetServices WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32Vector(tmpStates)) {
        HILOGE("BluetoothPbapPseProxy::GetServices transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(IBluetoothPbapPse::Code::PBAP_PSE_GET_DEVICES_BY_STATES, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPseProxy::GetServices done fail, error: %d", error);
    }
    int DevNum = reply.ReadInt32();
    for (int i = DevNum; i > 0; i--) {
        std::shared_ptr<BluetoothRawAddress> dev(reply.ReadParcelable<BluetoothRawAddress>());
        if (!dev) {
            return;
        }
        rawDevices.push_back(*dev);
    }
}

int BluetoothPbapPseProxy::Disconnect(const BluetoothRawAddress& device)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPseProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPseProxy::Disconnect WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPseProxy::Disconnect error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPse::Code::PBAP_PSE_DISCONNECT, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPseProxy::Disconnect done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothPbapPseProxy::SetConnectionStrategy(const BluetoothRawAddress& device, int32_t strategy)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPseProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPseProxy::SetConnectionStrategy WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPseProxy::SetConnectionStrategy error");
        return ERROR;
    }

    if (!data.WriteInt32(strategy)) {
        HILOGE("BluetoothPbapPseProxy::SetConnectionStrategy error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPse::Code::PBAP_PSE_SET_CONNECTION_STRATEGY, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPseProxy::SetConnectionStrategy done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothPbapPseProxy::GetConnectionStrategy(const BluetoothRawAddress& device)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPseProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPseProxy::GetConnectionStrategy WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPseProxy::GetConnectionStrategy error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPse::Code::PBAP_PSE_GET_CONNECTION_STRATEGY, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPseProxy::GetConnectionStrategy done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

void BluetoothPbapPseProxy::GrantPermission(const BluetoothRawAddress& device, bool allow, bool save)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPseProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPseProxy::GrantPermission WriteInterfaceToken error");
        return;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPseProxy::GrantPermission error");
        return;
    }

    if (!data.WriteBool(allow)) {
            HILOGE("BluetoothPbapPseProxy::GrantPermission error");
            return;
        }

    if (!data.WriteBool(save)) {
            HILOGE("BluetoothPbapPseProxy::GrantPermission error");
            return;
        }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPse::Code::PBAP_PSE_GRANT_PERMISSION, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPseProxy::GrantPermission done fail, error: %d", error);
        return;
    }
}

int BluetoothPbapPseProxy::SetDevicePassword(const BluetoothRawAddress &device, const std::string &password, const std::string &userId)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPseProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPseProxy::SetDevicePassword WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPseProxy::SetDevicePassword error");
        return ERROR;
    }

    if (!data.WriteString(password)) {
        HILOGE("BluetoothPbapPseProxy::SetDevicePassword error");
        return ERROR;
    }

    if (!data.WriteString(userId)) {
        HILOGE("BluetoothPbapPseProxy::SetDevicePassword error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPse::Code::PBAP_PSE_SET_DEVICE_PASSWORD, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPseProxy::SetDevicePassword done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

void BluetoothPbapPseProxy::RegisterObserver(const sptr<IBluetoothPbapPseObserver> &observer)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPseProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPseProxy::RegisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothPbapPseProxy::RegisterObserver error");
        return;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(IBluetoothPbapPse::Code::PBAP_PSE_REGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPseProxy::RegisterObserver done fail, error: %d", error);
        return;
    }
    return;
}

void BluetoothPbapPseProxy::DeregisterObserver(const sptr<IBluetoothPbapPseObserver> &observer)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPseProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPseProxy::RegisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothPbapPseProxy::RegisterObserver error");
        return;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(IBluetoothPbapPse::Code::PBAP_PSE_DEREGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPseProxy::RegisterObserver done fail, error: %d", error);
        return;
    }
    return;
}
}  // namespace Bluetooth
}  // namespace OHOS