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

#include "bluetooth_pbap_pce_stub.h"
#include "bluetooth_log.h"
#include "ipc_types.h"
#include "string_ex.h"
namespace OHOS {
namespace Bluetooth {
BluetoothPbapPceStub::BluetoothPbapPceStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPceStub::Code::PBAP_PCE_GET_DEVICE_STATE)] =
        &BluetoothPbapPceStub::GetDeviceStateInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPceStub::Code::PBAP_PCE_CONNECT)] =
        &BluetoothPbapPceStub::ConnectInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPceStub::Code::PBAP_PCE_PULL_PHONEBOOK)] =
        &BluetoothPbapPceStub::PullPhoneBookInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPceStub::Code::PBAP_PCE_SET_PHONEBOOK)] =
        &BluetoothPbapPceStub::SetPhoneBookInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPceStub::Code::PBAP_PCE_PULL_VCARD_LISTING)] =
        &BluetoothPbapPceStub::PullvCardListingInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPceStub::Code::PBAP_PCE_PULL_VCARD_ENTRY)] =
        &BluetoothPbapPceStub::PullvCardEntryInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPceStub::Code::PBAP_PCE_PULL_ISDOWNLOAGING)] =
        &BluetoothPbapPceStub::IsDownloadingInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPceStub::Code::PBAP_PCE_ABORT_DOWNLOADING)] =
        &BluetoothPbapPceStub::AbortDownloadingInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPceStub::Code::PBAP_PCE_SET_DEVICE_PASSWORD)] =
        &BluetoothPbapPceStub::SetDevicePasswordInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPceStub::Code::PBAP_PCE_DISCONNECT)] =
        &BluetoothPbapPceStub::DisconnectInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPceStub::Code::PBAP_PCE_SET_CONNECT_STRATEGY)] =
        &BluetoothPbapPceStub::SetConnectionStrategyInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPceStub::Code::PBAP_PCE_GET_CONNECT_STRATEGY)] =
        &BluetoothPbapPceStub::GetConnectionStrategyInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPceStub::Code::PBAP_PCE_GET_DEVICES_BY_STATE)] =
        &BluetoothPbapPceStub::GetDevicesByStatesInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPceStub::Code::PBAP_PCE_REGISTER_OBSERVER)] =
        &BluetoothPbapPceStub::RegisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPceStub::Code::PBAP_PCE_DEREGISTER_OBSERVER)] =
        &BluetoothPbapPceStub::DeregisterObserverInner;
}

BluetoothPbapPceStub::~BluetoothPbapPceStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}
int BluetoothPbapPceStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothPbapPceStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d", code, option.GetFlags());
    std::u16string descriptor = BluetoothPbapPceStub::GetDescriptor();
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
    HILOGW("BluetoothPbapPceStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
};

ErrCode BluetoothPbapPceStub::GetDeviceStateInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = GetDeviceState(*device);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothPbapPceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPceStub::ConnectInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = Connect(*device);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothPbapPceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPceStub::PullPhoneBookInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothIPbapPullPhoneBookParam> param(data.ReadParcelable<BluetoothIPbapPullPhoneBookParam>());
    if (!param) {
        return TRANSACTION_ERR;
    }
    int result = PullPhoneBook(*device, *param);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothPbapPceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPceStub::SetPhoneBookInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    const std::u16string name = data.ReadString16();
    int32_t flag = data.ReadInt32();
    int result = SetPhoneBook(*device, name, flag);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothPbapPceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPceStub::PullvCardListingInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothIPbapPullvCardListingParam> param(
        data.ReadParcelable<BluetoothIPbapPullvCardListingParam>());
    int result = PullvCardListing(*device, *param);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothPbapPceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPceStub::PullvCardEntryInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothIPbapPullvCardEntryParam> param(data.ReadParcelable<BluetoothIPbapPullvCardEntryParam>());
    if (!param) {
        return TRANSACTION_ERR;
    }
    int result = PullvCardEntry(*device, *param);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothPbapPceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPceStub::IsDownloadingInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    bool result = IsDownloading(*device);
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothPbapPceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPceStub::AbortDownloadingInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = AbortDownloading(*device);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothPbapPceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPceStub::SetDevicePasswordInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    const std::string password = data.ReadString();
    const std::string userId = data.ReadString();
    int result = SetDevicePassword(*device, password, userId);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothPbapPceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPceStub::DisconnectInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = Disconnect(*device);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothPbapPceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPceStub::SetConnectionStrategyInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int32_t strategy = data.ReadInt32();
    int result = SetConnectionStrategy(*device, strategy);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothPbapPceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPceStub::GetConnectionStrategyInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = GetConnectionStrategy(*device);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothPbapPceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPceStub::GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothPbapPceStub::GetDevicesByStatesInner starts");
    std::vector<int32_t> state;
    if (!data.ReadInt32Vector(&state)) {
        HILOGW("BluetoothPbapPceStub::GetDevicesByStatesInner: get tmpState failed.");
        return INVALID_DATA;
    }
    std::vector<BluetoothRawAddress> rawDevices;
    GetDevicesByStates(state, rawDevices);
    reply.WriteInt32(rawDevices.size());
    int num = rawDevices.size();
    for (int i = 0; i < num; i++) {
        bool ret = reply.WriteParcelable(&rawDevices[i]);
        if (!ret) {
            HILOGE("WriteParcelable<GetDevicesByStatesInner> failed");
            return ERR_INVALID_VALUE;
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPceStub::RegisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothPbapPceObserver> observer = OHOS::iface_cast<IBluetoothPbapPceObserver>(remote);
    RegisterObserver(observer);

    return NO_ERROR;
}

ErrCode BluetoothPbapPceStub::DeregisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothPbapPceObserver> observer = OHOS::iface_cast<IBluetoothPbapPceObserver>(remote);
    DeregisterObserver(observer);
    
    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS