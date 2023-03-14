/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief The framework interface and callback function of pbap client are defined.
 *
 * @since 6
 */

/**
 * @file bluetooth_pbap_client.h
 *
 * @brief pbap client interface.
 *
 * @since 6
 */

#ifndef BLUETOOTH_PBAP_CLIENT_H
#define BLUETOOTH_PBAP_CLIENT_H

#include <memory>
#include <string>
#include <vector>
#include "bluetooth_remote_device.h"
#include "bluetooth_types.h"
#include "bluetooth_def.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief Pbap PullPhoneBook app parameter
 *
 * @since 6
 */
class BLUETOOTH_API PbapPullPhoneBookParam {
public:
    enum {
        NAME = 1 << 0,
        PROPERTY_SELECTOR = 1 << 1,
        FORMAT = 1 << 2,
        MAX_LIST_COUNT = 1 << 3,
        LIST_START_OFFSET = 1 << 4,
        RESET_NEW_MISSED_CALLS = 1 << 5,
        VCARD_SELECTOR = 1 << 6,
        VCARD_SELECTOROP = 1 << 7,
    };

public:
    std::u16string GetName() const;
    void SetName(const std::u16string &name);
    uint64_t GetPropertySelector() const;
    void SetPropertySelector(uint64_t propertySelector);
    uint8_t GetFormat() const;
    void SetFormat(uint8_t format);
    uint16_t GetMaxListCount() const;
    void SetMaxListCount(uint16_t maxListCount);
    uint16_t GetListStartOffset() const;
    void SetListStartOffset(uint16_t listStartOffset);
    uint8_t GetResetNewMissedCalls() const;
    void SetResetNewMissedCalls(uint8_t resetNewMissedCalls);
    uint64_t GetvCardSelector() const;
    void SetvCardSelector(uint64_t vCardSelector);
    uint8_t GetvCardSelectorOp() const;
    void SetvCardSelectorOp(uint8_t vCardSelectorOp);
    uint64_t GetSpecifiedBitset() const;
    void SetSpecifiedBitset(uint64_t specifiedBitset);
    bool IsSpecified(int bitSpecified) const;

private:
    std::u16string name_ = u"";
    uint64_t propertySelector_ = 0ULL;
    uint8_t format_ = 0;
    uint16_t maxListCount_ = 0;
    uint16_t listStartOffset_ = 0;
    uint8_t resetNewMissedCalls_ = 0;
    uint64_t vCardSelector_ = 0ULL;
    uint8_t vCardSelectorOp_ = 0;
    uint64_t specifiedBitset_ = 0ULL;
};

/**
 * @brief Pbap PullvCardListing app parameter
 *
 * @since 6
 */
class BLUETOOTH_API PbapPullvCardListingParam {
public:
    enum {
        NAME = 1 << 0,
        ORDER = 1 << 1,
        SEARCH_VALUE = 1 << 2,
        SEARCH_PROPERTY = 1 << 3,
        MAX_LIST_COUNT = 1 << 4,
        LIST_START_OFFSET = 1 << 5,
        RESET_NEW_MISSED_CALLS = 1 << 6,
        VCARD_SELECTOR = 1 << 7,
        VCARD_SELECTOROP = 1 << 8,
    };

public:
    std::u16string GetName() const;
    void SetName(const std::u16string &name);
    uint8_t GetOrder() const;
    void SetOrder(uint8_t order);
    std::string GetSearchValue() const;
    void SetSearchValue(const std::string &searchValue);
    uint8_t GetSearchProperty() const;
    void SetSearchProperty(uint8_t searchProperty);
    uint16_t GetMaxListCount() const;
    void SetMaxListCount(uint16_t maxListCount);
    uint16_t GetListStartOffset() const;
    void SetListStartOffset(uint16_t listStartOffset);
    uint8_t GetResetNewMissedCalls() const;
    void SetResetNewMissedCalls(uint8_t resetNewMissedCalls);
    uint64_t GetvCardSelector() const;
    void SetvCardSelector(uint64_t vCardSelector);
    uint8_t GetvCardSelectorOp() const;
    void SetvCardSelectorOp(uint8_t vCardSelectorOp);
    uint64_t GetSpecifiedBitset() const;
    void SetSpecifiedBitset(uint64_t specifiedBitset);
    bool IsSpecified(int bitSpecified) const;

private:
    std::u16string name_ = u"";
    uint8_t order_ = 0;
    std::string searchValue_ = "";
    uint8_t searchProperty_ = 0;
    uint16_t maxListCount_ = 0;
    uint16_t listStartOffset_ = 0;
    uint8_t resetNewMissedCalls_ = 0;
    uint64_t vCardSelector_ = 0ULL;
    uint8_t vCardSelectorOp_ = 0;
    uint64_t specifiedBitset_ = 0ULL;
};

/**
 * @brief Pbap PullvCardEntry app parameter
 *
 * @since 6
 */
class BLUETOOTH_API PbapPullvCardEntryParam {
public:
    enum {
        NAME = 1 << 0,
        PROPERTY_SELECTOR = 1 << 1,
        FORMAT = 1 << 2,
    };

public:
    std::u16string GetName() const;
    void SetName(const std::u16string &name);
    uint64_t GetPropertySelector() const;
    void SetPropertySelector(uint64_t propertySelector);
    uint8_t GetFormat() const;
    void SetFormat(uint8_t format);
    uint64_t GetSpecifiedBitset() const;
    void SetSpecifiedBitset(uint64_t specifiedBitset);
    bool IsSpecified(int bitSpecified) const;

private:
    std::u16string name_ = u"";
    uint64_t propertySelector_ = 0ULL;
    uint8_t format_ = 0;
    uint64_t specifiedBitset_ = 0ULL;
};

/**
 * @brief Pbap PhoneBook response data
 *
 * @since 6
 */
class PbapPhoneBookData {
public:
    uint16_t phoneBookSize_ = 0;                     // VCard size
    std::vector<uint8_t> primaryFolderVersion_ {};    // VCard PrimaryFolderVersion 16 bytes
    std::vector<uint8_t> secondaryFolderVersion_ {};  // VCard SecondaryFolderVersion 16 bytes
    std::vector<uint8_t> databaseIdentifier_ {};      // VCard DatabaseIdentifier 16 bytes
    std::vector<uint8_t> result_ {};                  // VCard bytes with utf-8
};

/**
 * @brief obsever for pbap client
 * when pbap client occur event, call these
 * @since 6
 */
class PbapClientObserver {
public:
    /**
     * @brief Constructor
     * @details Constructor
     * @since 6
     */
    PbapClientObserver() = default;
    
    /**
     * @brief deconstructor
     * @details deconstructor
     * @since 6
     */
    virtual ~PbapClientObserver() = default;

    /**
     * @brief  ConnectionState Changed
     *
     * @param  device     bluetooth address
     * @param  state      changed status
     * @since 6
     */
    virtual void OnServiceConnectionStateChanged(const BluetoothRemoteDevice &device, int state)
    {}

    /**
     * @brief connect password input call back
     *
     * @details when connect password input call  it
     * @param device remote device
     * @param description description bytes
     * @param charset description bytes's chartset
     * @param fullAccess fullAccess
     * @since 6
     */
    virtual void OnServicePasswordRequired(const BluetoothRemoteDevice &device,
        const std::vector<uint8_t> &description, uint8_t charset, bool fullAccess = true)
    {}

    /**
     * @brief action completed call back
     * @details when action completed call  it
     * @param device remote device
     * @param actionType pbap action type
     *     PBAP_ACTION_PULLPHONEBOOKSIZE
     *     PBAP_ACTION_PULLPHONEBOOK
     *     PBAP_ACTION_SETPHONEBOOK
     *     PBAP_ACTION_PULLVCARDLISTINGSIZE
     *     PBAP_ACTION_PULLVCARDLISTING
     *     PBAP_ACTION_PULLVCARDENTRY
     *     PBAP_ACTION_ABORT
     * @param result response result
     * @since 6
     */
    virtual void OnActionCompleted(
        const BluetoothRemoteDevice &device, int respCode, int actionType, const PbapPhoneBookData &result)
    {}
};

/**
 * @brief pbap client
 * pbap client
 * @since 6
 */
class BLUETOOTH_API PbapClient {
public:
    /**
     * @brief get PbapClient
     * @details get PbapClient instance
     * @return PbapClient instance
     * @since 6
     */
    static PbapClient *GetProfile();

    /**
     * @brief register observer
     * @details register observer for the service of phone book client
     * @param observer the pointer that point to a PbapClientObserver
     * @return void
     * @since 6
     */
    void RegisterObserver(PbapClientObserver *observer);

    /**
     * @brief deregister observer
     * @details deregister observer for the service of phone book client
     * @param observer the pointer that point to a PbapClientObserver
     * @return void
     * @since 6
     */
    void DeregisterObserver(PbapClientObserver *observer);

    /**
     * @brief get the remote devices
     * @details get the remote device with the specified states
     * @param states states
     * @return std::vector remote devices
     * @since 6
     */
    std::vector<BluetoothRemoteDevice> GetDevicesByStates(const std::vector<int> &states);

    /**
     * @brief get connected devices
     * @details get connected devices
     * @return std::vector remote devices
     * @since 6
     */
    std::vector<BluetoothRemoteDevice> GetConnectedDevices();

    /**
     * @brief get the state of device
     * @details get the state with the specified remote device
     * @param device  remote device
     * @return int
     *     @c not -1 : state of the specified remote device
     *     @c -1 : device is not exist
     * @since 6
     */
    int GetDeviceState(const BluetoothRemoteDevice &device);

    /**
     * @brief connect to server
     * @details connect to phone book server
     * @param device  remote device
     * @return bool
     *     @c true  success
     *     @c false failure
     * @since 6
     */
    bool Connect(const BluetoothRemoteDevice &device);

    /**
     * @brief disconnect device
     * @details disconnect from remote device
     * @param device  remote device
     * @return bool
     *     @c true  success
     *     @c false failure
     * @since 6
     */
    bool Disconnect(const BluetoothRemoteDevice &device);

    /**
     * @brief set the strategy of device
     * @details set the strategy with the specified remote device
     * @param device  remote device
     * @param strategy  specified strategy
     * @return bool
     *     @c true  success
     *     @c false failure
     * @since 6
     */
    bool SetConnectionStrategy(const BluetoothRemoteDevice &device, int strategy);

    /**
     * @brief get the strategy of device
     * @details get the strategy with the specified remote device
     * @param device  remote device
     * @return int BTStrategyType:
     *           CONNECTION_UNKNOWN
     *           CONNECTION_ALLOWED
     *           CONNECTION_FORBIDDEN
     * @since 6
     */
    int GetConnectionStrategy(const BluetoothRemoteDevice &device);
    /**
     * @brief Set device's password. please call after OnServicePasswordRequired event.
     *
     * @param device device
     * @param password device's password
     * @param userId device's userId
     * @return int
     *     @c 0  success
     *     @c -1 failure
     * @since 6
     */
    int SetDevicePassword(
        const BluetoothRemoteDevice &device, const std::string &password, const std::string &userId = "");

    /**
     * @brief Pull phone book from remote device after connected.
     *
     * @param device device
     * @param param app parameter pbap v1.2.3
     * @return int
     *     @c 0  success
     *     @c -1 failure
     * @since 6
     */
    int PullPhoneBook(const BluetoothRemoteDevice &device, const PbapPullPhoneBookParam &param);

    /**
     * @brief Set phone book from remote device after connected.
     *
     * @param device device
     * @param name phone book path
     * @param flag
     *        @c PBAP_FLAG_GO_TO_ROOT and empty name--to root;
     *        @c PBAP_FLAG_GO_DOWN --go down;
     *        @c PBAP_FLAG_GO_UP --go up
     * @return int
     *     @c 0  success
     *     @c -1 failure
     * @since 6
     */
    int SetPhoneBook(const BluetoothRemoteDevice &device, const std::u16string &name, int flag);

    /**
     * @brief Pull vCard listing from remote device after connected.
     *
     * @param device device
     * @param param app parameter pbap v1.2.3
     * @return int
     *     @c 0  success
     *     @c -1 failure
     * @since 6
     */
    int PullvCardListing(const BluetoothRemoteDevice &device, const PbapPullvCardListingParam &param);

    /**
     * @brief Pull vCard entry from remote device after connected.
     *
     * @param device device
     * @param param app parameter pbap v1.2.3
     * @return int
     *     @c 0  success
     *     @c -1 failure
     * @since 6
     */
    int PullvCardEntry(const BluetoothRemoteDevice &device, const PbapPullvCardEntryParam &param);

    /**
     * @brief Check if local device is downloading phonebook from remote device.
     *
     * @param device device
     * @return bool  true downloading; false not downloading
     * @since 6
     */
    bool IsDownloading(const BluetoothRemoteDevice &device);

    /**
     * @brief Abort downloading phonebook from remote device.
     *
     * @param device device
     * @return int
     *     @c 0  success
     *     @c -1 failure
     * @since 6
     */
    int AbortDownloading(const BluetoothRemoteDevice &device);

private:
    /**
     * @brief constructor
     * @details constructor
     */
    PbapClient();

    /**
     * @brief deconstructor
     * @details deconstructor
     */
    ~PbapClient();

    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(PbapClient);
    BLUETOOTH_DECLARE_IMPL();
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // BLUETOOTH_PBAP_CLIENT_H
