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

#include <fstream>
#include <list>
#include <mutex>
#include <string>
#include "bluetooth_pbap_client.h"
#include "bluetooth_host.h"
#include "bluetooth_pbap_pce_proxy.h"
#include "bluetooth_remote_device.h"
#include "bluetooth_pbap_pce_observer_stub.h"
#include "bluetooth_observer_list.h"
#include "bluetooth_log.h"
#include "bluetooth_utils.h"
#include "iservice_registry.h"
#include "raw_address.h"
#include "system_ability_definition.h"
namespace OHOS {
namespace Bluetooth {
std::u16string PbapPullPhoneBookParam::GetName() const
{
    return name_;
}

void PbapPullPhoneBookParam::SetName(const std::u16string &name)
{
    name_ = name;
    specifiedBitset_ |= NAME;
}

uint64_t PbapPullPhoneBookParam::GetPropertySelector() const
{
    return propertySelector_;
}

void PbapPullPhoneBookParam::SetPropertySelector(uint64_t propertySelector)
{
    propertySelector_ = propertySelector;
    specifiedBitset_ |= PROPERTY_SELECTOR;
}

uint8_t PbapPullPhoneBookParam::GetFormat() const
{
    return format_;
}

void PbapPullPhoneBookParam::SetFormat(uint8_t format)
{
    format_ = format;
    specifiedBitset_ |= FORMAT;
}

uint16_t PbapPullPhoneBookParam::GetMaxListCount() const
{
    return maxListCount_;
}

void PbapPullPhoneBookParam::SetMaxListCount(uint16_t maxListCount)
{
    maxListCount_ = maxListCount;
    specifiedBitset_ |= MAX_LIST_COUNT;
}

uint16_t PbapPullPhoneBookParam::GetListStartOffset() const
{
    return listStartOffset_;
}

void PbapPullPhoneBookParam::SetListStartOffset(uint16_t listStartOffset)
{
    listStartOffset_ = listStartOffset;
    specifiedBitset_ |= LIST_START_OFFSET;
}

uint8_t PbapPullPhoneBookParam::GetResetNewMissedCalls() const
{
    return resetNewMissedCalls_;
}

void PbapPullPhoneBookParam::SetResetNewMissedCalls(uint8_t resetNewMissedCalls)
{
    resetNewMissedCalls_ = resetNewMissedCalls;
    specifiedBitset_ |= RESET_NEW_MISSED_CALLS;
}

uint64_t PbapPullPhoneBookParam::GetvCardSelector() const
{
    return vCardSelector_;
}

void PbapPullPhoneBookParam::SetvCardSelector(uint64_t vCardSelector)
{
    vCardSelector_ = vCardSelector;
    specifiedBitset_ |= VCARD_SELECTOR;
}

uint8_t PbapPullPhoneBookParam::GetvCardSelectorOp() const
{
    return vCardSelectorOp_;
}

void PbapPullPhoneBookParam::SetvCardSelectorOp(uint8_t vCardSelectorOp)
{
    vCardSelectorOp_ = vCardSelectorOp;
    specifiedBitset_ |= VCARD_SELECTOROP;
}

uint64_t PbapPullPhoneBookParam::GetSpecifiedBitset() const
{
    return specifiedBitset_;
}

void PbapPullPhoneBookParam::SetSpecifiedBitset(uint64_t specifiedBitset)
{
    specifiedBitset_ = specifiedBitset;
}

bool PbapPullPhoneBookParam::IsSpecified(int bitSpecified) const
{
    if (bitSpecified & specifiedBitset_) {
        return true;
    } else {
        return false;
    }
}

// implement PbapPullvCardListingParam
std::u16string PbapPullvCardListingParam::GetName() const
{
    return name_;
}

void PbapPullvCardListingParam::SetName(const std::u16string &name)
{
    name_ = name;
    specifiedBitset_ |= NAME;
}

uint8_t PbapPullvCardListingParam::GetOrder() const
{
    return order_;
}

void PbapPullvCardListingParam::SetOrder(uint8_t order)
{
    order_ = order;
    specifiedBitset_ |= ORDER;
}

std::string PbapPullvCardListingParam::GetSearchValue() const
{
    return searchValue_;
}

void PbapPullvCardListingParam::SetSearchValue(const std::string &searchValue)
{
    searchValue_ = searchValue;
    specifiedBitset_ |= SEARCH_VALUE;
}

uint8_t PbapPullvCardListingParam::GetSearchProperty() const
{
    return searchProperty_;
}

void PbapPullvCardListingParam::SetSearchProperty(uint8_t searchProperty)
{
    searchProperty_ = searchProperty;
    specifiedBitset_ |= SEARCH_PROPERTY;
}

uint16_t PbapPullvCardListingParam::GetMaxListCount() const
{
    return maxListCount_;
}

void PbapPullvCardListingParam::SetMaxListCount(uint16_t maxListCount)
{
    maxListCount_ = maxListCount;
    specifiedBitset_ |= MAX_LIST_COUNT;
}

uint16_t PbapPullvCardListingParam::GetListStartOffset() const
{
    return listStartOffset_;
}

void PbapPullvCardListingParam::SetListStartOffset(uint16_t listStartOffset)
{
    listStartOffset_ = listStartOffset;
    specifiedBitset_ |= LIST_START_OFFSET;
}

uint8_t PbapPullvCardListingParam::GetResetNewMissedCalls() const
{
    return resetNewMissedCalls_;
}

void PbapPullvCardListingParam::SetResetNewMissedCalls(uint8_t resetNewMissedCalls)
{
    resetNewMissedCalls_ = resetNewMissedCalls;
    specifiedBitset_ |= RESET_NEW_MISSED_CALLS;
}

uint64_t PbapPullvCardListingParam::GetvCardSelector() const
{
    return vCardSelector_;
}

void PbapPullvCardListingParam::SetvCardSelector(uint64_t vCardSelector)
{
    vCardSelector_ = vCardSelector;
    specifiedBitset_ |= VCARD_SELECTOR;
}

uint8_t PbapPullvCardListingParam::GetvCardSelectorOp() const
{
    return vCardSelectorOp_;
}

void PbapPullvCardListingParam::SetvCardSelectorOp(uint8_t vCardSelectorOp)
{
    vCardSelectorOp_ = vCardSelectorOp;
    specifiedBitset_ |= VCARD_SELECTOROP;
}

uint64_t PbapPullvCardListingParam::GetSpecifiedBitset() const
{
    return specifiedBitset_;
}

void PbapPullvCardListingParam::SetSpecifiedBitset(uint64_t specifiedBitset)
{
    specifiedBitset_ = specifiedBitset;
}

bool PbapPullvCardListingParam::IsSpecified(int bitSpecified) const
{
    if (bitSpecified & specifiedBitset_) {
        return true;
    } else {
        return false;
    }
}

// implement PbapPullvCardEntryParam
std::u16string PbapPullvCardEntryParam::GetName() const
{
    return name_;
}

void PbapPullvCardEntryParam::SetName(const std::u16string &name)
{
    name_ = name;
    specifiedBitset_ |= NAME;
}

uint64_t PbapPullvCardEntryParam::GetPropertySelector() const
{
    return propertySelector_;
}

void PbapPullvCardEntryParam::SetPropertySelector(uint64_t propertySelector)
{
    propertySelector_ = propertySelector;
    specifiedBitset_ |= PROPERTY_SELECTOR;
}

uint8_t PbapPullvCardEntryParam::GetFormat() const
{
    return format_;
}

void PbapPullvCardEntryParam::SetFormat(uint8_t format)
{
    format_ = format;
    specifiedBitset_ |= FORMAT;
}

uint64_t PbapPullvCardEntryParam::GetSpecifiedBitset() const
{
    return specifiedBitset_;
}

void PbapPullvCardEntryParam::SetSpecifiedBitset(uint64_t specifiedBitset)
{
    specifiedBitset_ = specifiedBitset;
}

bool PbapPullvCardEntryParam::IsSpecified(int bitSpecified) const
{
    if (bitSpecified & specifiedBitset_) {
        return true;
    } else {
        return false;
    }
}

class BluetoothIPbapPullPhoneBookParamAdapter {
public:
    explicit BluetoothIPbapPullPhoneBookParamAdapter() = default;
    explicit BluetoothIPbapPullPhoneBookParamAdapter(const PbapPullPhoneBookParam &other)
    {
        data_.SetName(other.GetName());
        data_.SetPropertySelector(other.GetPropertySelector());
        data_.SetFormat(other.GetFormat());
        data_.SetMaxListCount(other.GetMaxListCount());
        data_.SetListStartOffset(other.GetListStartOffset());
        data_.SetResetNewMissedCalls(other.GetResetNewMissedCalls());
        data_.SetvCardSelector(other.GetvCardSelector());
        data_.SetvCardSelectorOp(other.GetvCardSelectorOp());
        data_.SetSpecifiedBitset(other.GetSpecifiedBitset());
    }
    virtual ~BluetoothIPbapPullPhoneBookParamAdapter() = default;
    operator BluetoothIPbapPullPhoneBookParam() const
    {
        return data_;
    }

private:
    BluetoothIPbapPullPhoneBookParam data_;
};

class BluetoothIPbapPullvCardListingParamAdapter {
public:
    explicit BluetoothIPbapPullvCardListingParamAdapter() = default;
    explicit BluetoothIPbapPullvCardListingParamAdapter(const PbapPullvCardListingParam &other)
    {
        data_.SetName(other.GetName());
        data_.SetOrder(other.GetOrder());
        data_.SetSearchValue(other.GetSearchValue());
        data_.SetSearchProperty(other.GetSearchProperty());
        data_.SetMaxListCount(other.GetMaxListCount());
        data_.SetListStartOffset(other.GetListStartOffset());
        data_.SetResetNewMissedCalls(other.GetResetNewMissedCalls());
        data_.SetvCardSelector(other.GetvCardSelector());
        data_.SetvCardSelectorOp(other.GetvCardSelectorOp());
        data_.SetSpecifiedBitset(other.GetSpecifiedBitset());
    }
    virtual ~BluetoothIPbapPullvCardListingParamAdapter() = default;
    operator BluetoothIPbapPullvCardListingParam() const
    {
        return data_;
    }

private:
    BluetoothIPbapPullvCardListingParam data_;
};

class BluetoothIPbapPullvCardEntryParamAdapter {
public:
    explicit BluetoothIPbapPullvCardEntryParamAdapter() = default;
    explicit BluetoothIPbapPullvCardEntryParamAdapter(const PbapPullvCardEntryParam &other)
    {
        data_.SetName(other.GetName());
        data_.SetPropertySelector(other.GetPropertySelector());
        data_.SetFormat(other.GetFormat());
        data_.SetSpecifiedBitset(other.GetSpecifiedBitset());
    }
    virtual ~BluetoothIPbapPullvCardEntryParamAdapter() = default;
    operator BluetoothIPbapPullvCardEntryParam() const
    {
        return data_;
    }

private:
    BluetoothIPbapPullvCardEntryParam data_;
};

class PbapPhoneBookDataAdapter {
public:
    explicit PbapPhoneBookDataAdapter(const BluetoothIPbapPhoneBookData &other)
    {
        data_.phoneBookSize_ = other.phoneBookSize_;
        data_.primaryFolderVersion_ = other.primaryFolderVersion_;
        data_.secondaryFolderVersion_ = other.secondaryFolderVersion_;
        data_.databaseIdentifier_ = other.databaseIdentifier_;
        data_.result_ = other.result_;
        const_cast<BluetoothIPbapPhoneBookData &>(other).Load(data_.result_);
    }
    operator PbapPhoneBookData() const
    {
        return data_;
    }

private:
    PbapPhoneBookData data_ {};
};

class BluetoothPbapPceObserverImpl : public BluetoothPbapPceObserverStub {
public:
    BluetoothPbapPceObserverImpl() = default;
    ~BluetoothPbapPceObserverImpl() = default;
    void SetObserver(BluetoothObserverList<PbapClientObserver> *frameworkObserverListPtr)
    {
        frameworkObserverListPtr_ = frameworkObserverListPtr;
        if (frameworkObserverListPtr_ == nullptr) {
            HILOGI("frameworkObserverListPtr_ is nullptr error.");
        }
    }

    void OnServiceConnectionStateChanged(const BluetoothRawAddress &device, int state) override
    {
        HILOGI("enter, device: %{public}s, state: %{public}d", GetEncryptAddr((device).GetAddress()).c_str(), state);
        if (frameworkObserverListPtr_ != nullptr) {
            frameworkObserverListPtr_->ForEach([device, state](std::shared_ptr<PbapClientObserver> observer) {
                BluetoothRemoteDevice dev(device.GetAddress(), 0);
                observer->OnServiceConnectionStateChanged(dev, state);
            });
        }
    }

    void OnServicePasswordRequired(const BluetoothRawAddress &device, 
        const ::std::vector<uint8_t> &description, int8_t charset, bool fullAccess) override
    {
        HILOGI("enter, device: %{public}s, charset: %{public}d, fullAccess: %{public}d",
            GetEncryptAddr((device).GetAddress()).c_str(), charset, fullAccess);
        if (frameworkObserverListPtr_ != nullptr) {
            frameworkObserverListPtr_->ForEach(
                [device, description, charset, fullAccess](std::shared_ptr<PbapClientObserver> observer) {
                    BluetoothRemoteDevice dev(device.GetAddress(), 0);
                    observer->OnServicePasswordRequired(dev, description, charset, fullAccess);
                });
        }
    }

    void  OnActionCompleted(const BluetoothRawAddress &device, 
        int respCode, int actionType, const BluetoothIPbapPhoneBookData &result) override
    {
        HILOGI("enter, device: %{public}s, respCode: %{public}d, actionType: %{public}d",
            GetEncryptAddr((device).GetAddress()).c_str(), respCode, actionType);
        if (frameworkObserverListPtr_ != nullptr) {
            frameworkObserverListPtr_->ForEach([device, 
                respCode, actionType, result](std::shared_ptr<PbapClientObserver> observer) {
                BluetoothRemoteDevice dev(device.GetAddress(), 0);
                observer->OnActionCompleted(dev, respCode, actionType, PbapPhoneBookDataAdapter(result));
            });
        }
    }

private:
    BluetoothObserverList<PbapClientObserver> *frameworkObserverListPtr_ {nullptr};
};

struct PbapClient::impl {
    sptr<IBluetoothPbapPce> proxy_;
    class BluetoothPbapPceDeathRecipient;
    sptr<BluetoothPbapPceDeathRecipient> deathRecipient_ = nullptr;
    impl();
    ~impl()
    {
        if (proxy_ != nullptr) {
            proxy_->DeregisterObserver(serviceObserverImpl_);
            proxy_->AsObject()->RemoveDeathRecipient(deathRecipient_);
        }
    }
    void RegisterObserver(std::shared_ptr<PbapClientObserver> &observer)
    {
        HILOGI("enter");
        frameworkObserverList_.Register(observer);
    }

    void DeregisterObserver(std::shared_ptr<PbapClientObserver> &observer)
    {
        HILOGI("enter");
        frameworkObserverList_.Deregister(observer);
    }

    int GetDeviceState(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (proxy_ != nullptr) {
            return proxy_->GetDeviceState(bluetooth::RawAddress(device.GetDeviceAddr()));
        }
        return 0;
    }

    std::vector<BluetoothRemoteDevice> GetDevicesByStates(const std::vector<int> &states)
    {
        HILOGI("enter");
        std::vector<BluetoothRemoteDevice> remoteDevices;
        std::vector<BluetoothRawAddress> rawDevices;
        std::vector<int32_t> tmpStates;
        for (int state : states) {
            tmpStates.push_back((int32_t)state);
        }
        if (proxy_ != nullptr) {
            proxy_->GetDevicesByStates(tmpStates, rawDevices);
        }
        for (BluetoothRawAddress rawDevice : rawDevices) {
            BluetoothRemoteDevice remoteDevice(rawDevice.GetAddress(), 0);
            remoteDevices.push_back(remoteDevice);
        }
        return remoteDevices;
    }

    std::vector<BluetoothRemoteDevice> GetConnectedDevices()
    {
        HILOGI("enter");
        std::vector<BluetoothRemoteDevice> remoteDevices;
        std::vector<BluetoothRawAddress> rawDevices;
        std::vector<int> states {static_cast<int>(BTConnectState::CONNECTED)};
        if (proxy_ != nullptr) {
            proxy_->GetDevicesByStates(states, rawDevices);
        }
        for (BluetoothRawAddress rawDevice : rawDevices) {
            BluetoothRemoteDevice remoteDevice(rawDevice.GetAddress(), 0);
            remoteDevices.push_back(remoteDevice);
        }
        return remoteDevices;
    }

    bool Connect(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (proxy_ != nullptr) {
            int ret = proxy_->Connect(bluetooth::RawAddress(device.GetDeviceAddr()));
            return ret == RET_NO_ERROR;
        } else {
            return false;
        }
    }

    int PullPhoneBook(const BluetoothRemoteDevice &device, const BluetoothIPbapPullPhoneBookParam &param)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        int ret = -1;
        if (proxy_ != nullptr) {
            ret = proxy_->PullPhoneBook(bluetooth::RawAddress(device.GetDeviceAddr()), param);
        }
        return ret;
    }

    int SetPhoneBook(const BluetoothRemoteDevice &device, const std::u16string &name, int flag)
    {
        HILOGI("enter, device: %{public}s, flag: %{public}d", GET_ENCRYPT_ADDR(device), flag);
        int ret = -1;
        if (proxy_ != nullptr) {
            ret = proxy_->SetPhoneBook(
                bluetooth::RawAddress(device.GetDeviceAddr()), name.c_str(), flag);
        }
        return ret;
    }

    int PullvCardListing(const BluetoothRemoteDevice &device, const BluetoothIPbapPullvCardListingParam &param)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        int ret = -1;
        if (proxy_ != nullptr) {
            proxy_->PullvCardListing(bluetooth::RawAddress(device.GetDeviceAddr()), param);
        }
        return ret;
    }

    int PullvCardEntry(const BluetoothRemoteDevice &device, const BluetoothIPbapPullvCardEntryParam &param)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        int ret = -1;
        if (proxy_ != nullptr) {
            ret = proxy_->PullvCardEntry(bluetooth::RawAddress(device.GetDeviceAddr()), param);
        }
        return ret;
    }

    bool IsDownloading(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        bool ret = false;
        if (proxy_ != nullptr) {
            ret = proxy_->IsDownloading(bluetooth::RawAddress(device.GetDeviceAddr()));
        }
        return ret;
    }

    int AbortDownloading(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        int ret = -1;
        if (proxy_ != nullptr) {
            ret = proxy_->AbortDownloading(bluetooth::RawAddress(device.GetDeviceAddr()));
        }
        return ret;
    }

    bool Disconnect(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (proxy_ != nullptr) {
            int ret = proxy_->Disconnect(bluetooth::RawAddress(device.GetDeviceAddr()));
            return ret == RET_NO_ERROR;
        } else {
            return false;
        }
    }

    bool SetConnectionStrategy(const BluetoothRemoteDevice &device, int strategy)
    {
        HILOGI("enter, device: %{public}s, strategy: %{public}d", GET_ENCRYPT_ADDR(device), strategy);
        if (proxy_ != nullptr) {
            int ret = proxy_->SetConnectionStrategy(bluetooth::RawAddress(device.GetDeviceAddr()), strategy);
            return ret == RET_NO_ERROR;
        } else {
            return false;
        }
    }

    int GetConnectionStrategy(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        int ret = (int)BTStrategyType::CONNECTION_UNKNOWN;
        if (proxy_ != nullptr) {
            ret = proxy_->GetConnectionStrategy(bluetooth::RawAddress(device.GetDeviceAddr()));
        }
        return ret;
    }

    int SetDevicePassword(const BluetoothRemoteDevice &device, const std::string &password, const std::string &userId)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        int ret = -1;
        if (proxy_ != nullptr) {
            ret = proxy_->SetDevicePassword(bluetooth::RawAddress(device.GetDeviceAddr()), 
                                                                        password.c_str(), userId.c_str());
        }
        return ret;
    }

private:
    std::mutex mutex_;
    BluetoothObserverList<PbapClientObserver> frameworkObserverList_ {};
    sptr<BluetoothPbapPceObserverImpl> serviceObserverImpl_;
};

class PbapClient::impl::BluetoothPbapPceDeathRecipient final : public IRemoteObject::DeathRecipient {
public:
    BluetoothPbapPceDeathRecipient(PbapClient::impl &pbapPce) : pbapPce_(pbapPce) {};
    ~BluetoothPbapPceDeathRecipient() final = default;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothPbapPceDeathRecipient);

    void OnRemoteDied(const wptr<IRemoteObject> &remote) final
    {
        HILOGI("starts");
        pbapPce_.proxy_->AsObject()->RemoveDeathRecipient(pbapPce_.deathRecipient_);
        pbapPce_.proxy_ = nullptr;
    }

private:
    PbapClient::impl &pbapPce_;
};
PbapClient::impl::impl()
{
    serviceObserverImpl_ = new BluetoothPbapPceObserverImpl();
    serviceObserverImpl_->SetObserver(&frameworkObserverList_);  // bind frameworkObserverList_
    HILOGI("starts");
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> hostRemote = samgr->GetSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID);

    if (!hostRemote) {
        HILOGI("failed: no hostRemote");
        return;
    }
    sptr<IBluetoothHost> hostProxy = iface_cast<IBluetoothHost>(hostRemote);
    sptr<IRemoteObject> remote = hostProxy->GetProfile(PROFILE_PBAP_PCE);

    if (!remote) {
        HILOGE("failed: no remote");
        return;
    }
    HILOGI("remote obtained");

    proxy_ = iface_cast<IBluetoothPbapPce>(remote);
    if (proxy_ == nullptr) {
        return;
    }
    deathRecipient_ = new BluetoothPbapPceDeathRecipient(*this);
    proxy_->AsObject()->AddDeathRecipient(deathRecipient_);
    proxy_->RegisterObserver(serviceObserverImpl_);
}
PbapClient *PbapClient::GetProfile()
{
    static PbapClient instance;
    return &instance;
}

PbapClient::PbapClient()
{
    pimpl = std::make_unique<impl>();
}

PbapClient::~PbapClient()
{}

void PbapClient::RegisterObserver(PbapClientObserver *observer)
{
    HILOGI("enter");
    std::shared_ptr<PbapClientObserver> pointer(observer, [](PbapClientObserver *) {});
    return pimpl->RegisterObserver(pointer);
}

void PbapClient::DeregisterObserver(PbapClientObserver *observer)
{
    HILOGI("enter");
    std::shared_ptr<PbapClientObserver> pointer(observer, [](PbapClientObserver *) {});
    return pimpl->DeregisterObserver(pointer);
}

int PbapClient::GetDeviceState(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    if (!IS_BT_ENABLED()) {
        return (int)BTConnectState::DISCONNECTED;
    }
    if (!device.IsValidBluetoothRemoteDevice()) {
        return (int)BTConnectState::DISCONNECTED;
    }
    return pimpl->GetDeviceState(device);
}

std::vector<BluetoothRemoteDevice> PbapClient::GetDevicesByStates(const std::vector<int> &states)
{
    HILOGI("enter");
    if (!IS_BT_ENABLED()) {
        return std::vector<BluetoothRemoteDevice>();
    }
    return pimpl->GetDevicesByStates(states);
}

std::vector<BluetoothRemoteDevice> PbapClient::GetConnectedDevices()
{
    HILOGI("enter");
    if (!IS_BT_ENABLED()) {
        return std::vector<BluetoothRemoteDevice>();
    }
    return pimpl->GetConnectedDevices();
}

bool PbapClient::Connect(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    if (!IS_BT_ENABLED()) {
        return false;
    }
    if (!device.IsValidBluetoothRemoteDevice()) {
        return false;
    }
    return pimpl->Connect(device);
}

bool PbapClient::Disconnect(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    if (!IS_BT_ENABLED()) {
        return false;
    }
    if (!device.IsValidBluetoothRemoteDevice()) {
        return false;
    }
    return pimpl->Disconnect(device);
}

bool PbapClient::SetConnectionStrategy(const BluetoothRemoteDevice &device, int strategy)
{
    HILOGI("enter, device: %{public}s, strategy: %{public}d", GET_ENCRYPT_ADDR(device), strategy);
    if (!IS_BT_ENABLED()) {
        return false;
    }
    if (!device.IsValidBluetoothRemoteDevice()) {
        return false;
    }
    return pimpl->SetConnectionStrategy(device, strategy);
}

int PbapClient::GetConnectionStrategy(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    if (!IS_BT_ENABLED()) {
        return (int)BTStrategyType::CONNECTION_FORBIDDEN;
    }
    if (!device.IsValidBluetoothRemoteDevice()) {
        return (int)BTStrategyType::CONNECTION_FORBIDDEN;
    }
    return pimpl->GetConnectionStrategy(device);
}

int PbapClient::SetDevicePassword(
    const BluetoothRemoteDevice &device, const std::string &password, const std::string &userId)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    if (!IS_BT_ENABLED()) {
        return RET_BAD_STATUS;
    }
    if (!device.IsValidBluetoothRemoteDevice()) {
        return RET_BAD_PARAM;
    }
    return pimpl->SetDevicePassword(device, password, userId);
}

int PbapClient::PullPhoneBook(const BluetoothRemoteDevice &device, const PbapPullPhoneBookParam &param)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    if (!IS_BT_ENABLED()) {
        return RET_BAD_STATUS;
    }
    if (!device.IsValidBluetoothRemoteDevice()) {
        return RET_BAD_PARAM;
    }
    return pimpl->PullPhoneBook(device, BluetoothIPbapPullPhoneBookParamAdapter(param));
}

int PbapClient::SetPhoneBook(const BluetoothRemoteDevice &device, const std::u16string &name, int flag)
{
    HILOGI("enter, device: %{public}s, flag: %{public}d", GET_ENCRYPT_ADDR(device), flag);
    if (!IS_BT_ENABLED()) {
        return RET_BAD_STATUS;
    }
    if (!device.IsValidBluetoothRemoteDevice()) {
        return RET_BAD_PARAM;
    }
    return pimpl->SetPhoneBook(device, name, flag);
}

int PbapClient::PullvCardListing(const BluetoothRemoteDevice &device, const PbapPullvCardListingParam &param)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    if (!IS_BT_ENABLED()) {
        return RET_BAD_STATUS;
    }
    if (!device.IsValidBluetoothRemoteDevice()) {
        return RET_BAD_PARAM;
    }
    return pimpl->PullvCardListing(device, BluetoothIPbapPullvCardListingParamAdapter(param));
}

int PbapClient::PullvCardEntry(const BluetoothRemoteDevice &device, const PbapPullvCardEntryParam &param)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    if (!IS_BT_ENABLED()) {
        return RET_BAD_STATUS;
    }
    if (!device.IsValidBluetoothRemoteDevice()) {
        return RET_BAD_PARAM;
    }
    return pimpl->PullvCardEntry(device, BluetoothIPbapPullvCardEntryParamAdapter(param));
}

bool PbapClient::IsDownloading(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    if (!IS_BT_ENABLED()) {
        return false;
    }
    if (!device.IsValidBluetoothRemoteDevice()) {
        return false;
    }
    return pimpl->IsDownloading(device);
}

int PbapClient::AbortDownloading(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    if (!IS_BT_ENABLED()) {
        return RET_BAD_STATUS;
    }
    if (!device.IsValidBluetoothRemoteDevice()) {
        return RET_BAD_PARAM;
    }
    return pimpl->AbortDownloading(device);
}
}  // namespace Bluetooth
}  // namespace OHOS