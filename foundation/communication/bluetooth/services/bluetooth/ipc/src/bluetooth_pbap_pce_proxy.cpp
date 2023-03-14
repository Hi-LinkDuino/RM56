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

#include "bluetooth_pbap_pce_proxy.h"
#include "bluetooth_log.h"
#include "i_bluetooth_pbap_pce.h"
#include "bluetooth_errorcode.h"

namespace OHOS {
namespace Bluetooth {
int BluetoothPbapPceProxy::GetDeviceState(const BluetoothRawAddress &device)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPceProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPceProxy::GetDeviceState WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPceProxy::GetDeviceState error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPce::Code::PBAP_PCE_GET_DEVICE_STATE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPceProxy::GetDeviceState done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothPbapPceProxy::Connect(const BluetoothRawAddress &device)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPceProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPceProxy::Connect WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPceProxy::Connect error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPce::Code::PBAP_PCE_CONNECT, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPceProxy::Connect done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothPbapPceProxy::PullPhoneBook(const BluetoothRawAddress &device, const BluetoothIPbapPullPhoneBookParam &param)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPceProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPceProxy::PullPhoneBook WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPceProxy::PullPhoneBook error");
        return ERROR;
    }

    if (!data.WriteParcelable(&param)) {
        HILOGE("BluetoothPbapPceProxy::PullPhoneBook error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPce::Code::PBAP_PCE_PULL_PHONEBOOK, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPceProxy::PullPhoneBook done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothPbapPceProxy::SetPhoneBook(const BluetoothRawAddress &device, const std::u16string &name, int32_t flag)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPceProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPceProxy::SetPhoneBook WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPceProxy::SetPhoneBook error");
        return ERROR;
    }

    if (!data.WriteString16(name)) {
        HILOGE("BluetoothPbapPceProxy::SetPhoneBook error");
        return ERROR;
    }

    if (!data.WriteInt32(flag)) {
        HILOGE("BluetoothPbapPceProxy::SetPhoneBook error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPce::Code::PBAP_PCE_SET_PHONEBOOK, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPceProxy::SetPhoneBook done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothPbapPceProxy::PullvCardListing(const BluetoothRawAddress &device, const BluetoothIPbapPullvCardListingParam &param)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPceProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPceProxy::PullvCardListing WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPceProxy::PullvCardListing error");
        return ERROR;
    }

    if (!data.WriteParcelable(&param)) {
        HILOGE("BluetoothPbapPceProxy::PullvCardListing error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPce::Code::PBAP_PCE_PULL_VCARD_LISTING, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPceProxy::PullvCardListing done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothPbapPceProxy::PullvCardEntry(const BluetoothRawAddress &device, const BluetoothIPbapPullvCardEntryParam &param)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPceProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPceProxy::PullvCardEntry WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPceProxy::PullvCardEntry error");
        return ERROR;
    }

    if (!data.WriteParcelable(&param)) {
        HILOGE("BluetoothPbapPceProxy::PullvCardEntry error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPce::Code::PBAP_PCE_PULL_VCARD_ENTRY, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPceProxy::PullvCardEntry done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

bool BluetoothPbapPceProxy::IsDownloading(const BluetoothRawAddress &device)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPceProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPceProxy::IsDownloading WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPceProxy::IsDownloading error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPce::Code::PBAP_PCE_PULL_ISDOWNLOAGING, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPceProxy::IsDownloading done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadBool();
}

int BluetoothPbapPceProxy::AbortDownloading(const BluetoothRawAddress &device)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPceProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPceProxy::AbortDownloading WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPceProxy::AbortDownloading error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPce::Code::PBAP_PCE_ABORT_DOWNLOADING, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPceProxy::AbortDownloading done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothPbapPceProxy::SetDevicePassword(const BluetoothRawAddress &device, const std::string &password, const std::string &userId)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPceProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPceProxy::SetDevicePassword WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPceProxy::SetDevicePassword error");
        return ERROR;
    }

    if (!data.WriteString(password)) {
        HILOGE("BluetoothPbapPceProxy::SetDevicePassword error");
        return ERROR;
    }

    if (!data.WriteString(userId)) {
        HILOGE("BluetoothPbapPceProxy::SetDevicePassword error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPce::Code::PBAP_PCE_SET_DEVICE_PASSWORD, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPceProxy::SetDevicePassword done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothPbapPceProxy::Disconnect(const BluetoothRawAddress &device)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPceProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPceProxy::Disconnect WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPceProxy::Disconnect error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPce::Code::PBAP_PCE_DISCONNECT, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPceProxy::Disconnect done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothPbapPceProxy::SetConnectionStrategy(const BluetoothRawAddress &device, int32_t strategy)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPceProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPceProxy::SetConnectionStrategy WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPceProxy::SetConnectionStrategy error");
        return ERROR;
    }

    if (!data.WriteInt32(strategy)) {
        HILOGE("BluetoothPbapPceProxy::SetConnectionStrategy error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPce::Code::PBAP_PCE_SET_CONNECT_STRATEGY, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPceProxy::SetConnectionStrategy done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothPbapPceProxy::GetConnectionStrategy(const BluetoothRawAddress &device)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPceProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPceProxy::GetConnectionStrategy WriteInterfaceToken error");
        return ERROR;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPceProxy::GetConnectionStrategy error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPce::Code::PBAP_PCE_GET_CONNECT_STRATEGY, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPceProxy::GetConnectionStrategy done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

void BluetoothPbapPceProxy::GetDevicesByStates(const std::vector<int32_t> tmpStates, std::vector<BluetoothRawAddress> &rawDevices)
{
    HILOGI("BluetoothPbapPceProxy::GetServices start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPceProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPceProxy::GetServices WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32Vector(tmpStates)) {
        HILOGE("BluetoothPbapPceProxy::GetServices transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(IBluetoothPbapPce::Code::PBAP_PCE_GET_DEVICES_BY_STATE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPceProxy::GetServices done fail, error: %d", error);
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

void BluetoothPbapPceProxy::RegisterObserver(const sptr<IBluetoothPbapPceObserver> &observer)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPceProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPceProxy::RegisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothPbapPceProxy::RegisterObserver error");
        return;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(IBluetoothPbapPce::Code::PBAP_PCE_REGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPceProxy::RegisterObserver done fail, error: %d", error);
        return;
    }
    return;
}

void BluetoothPbapPceProxy::DeregisterObserver(const sptr<IBluetoothPbapPceObserver> &observer)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPceProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPceProxy::RegisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothPbapPceProxy::RegisterObserver error");
        return;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(IBluetoothPbapPce::Code::PBAP_PCE_DEREGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPceProxy::RegisterObserver done fail, error: %d", error);
        return;
    }
    return;
}
}  // namespace Bluetooth
}  // namespace OHOS