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

#ifndef INTERFACE_PROFILE_PBAP_PCE_H
#define INTERFACE_PROFILE_PBAP_PCE_H

#include "interface_profile.h"
#include "pbap_pce_parameter.h"
namespace OHOS {
namespace bluetooth {
/**
 * @brief obsever for pbap client
 * when pbap client occur event, call these
 * @since 6
 */
class IPbapPceObserver {
public:
    /**
     * @brief  ConnectionState Changed
     *
     * @param  device     bluetooth address
     * @param  state      changed status
     * @since 6
     */
    virtual void OnServiceConnectionStateChanged(const RawAddress &device, int state) = 0;

    /**
     * @brief connect password input call back
     * @details when connect password input call  it
     * @param device remote device
     * @param description description bytes
     * @param charset description bytes's chartset
     * @param fullAccess fullAccess
     * @since 6
     */
    virtual void OnServicePasswordRequired(
        const RawAddress &device, const std::vector<uint8_t> &description,
        uint8_t charset, bool fullAccess = true) = 0;
    /**
     * @brief Called OnActionCompleted
     *
     * @param device remote device
     * @param respCode Response code
     * @param actionType phonebook action type
     * @param result phonebook action response result
     * @since 6
     */
    virtual void OnActionCompleted(
        const RawAddress &device, int respCode, int actionType, const IPbapPhoneBookData &result) = 0;

    /**
     * @brief constructor
     * @details constructor
     * @return
     * @since 6
     */
    IPbapPceObserver()
    {}

    /**
     * @brief deconstructor
     * @details deconstructor
     * @return
     * @since 6
     */
    virtual ~IPbapPceObserver(){};
};

/**
 * @brief pbap pce service
 * pbap pce service
 */
class IProfilePbapPce : public IProfile {
public:
    /**
     * @brief register observer
     * @details register observer for the service of phone book client
     * @param observer reference to a PbapClientObserver
     * @return void
     * @since 6
     */
    virtual void RegisterObserver(IPbapPceObserver &observer) = 0;

    /**
     * @brief deregister observer
     * @details deregister observer for the service of phone book client
     * @param observer reference to a PbapClientObserver
     * @return void
     * @since 6
     */
    virtual void DeregisterObserver(IPbapPceObserver &observer) = 0;

    /**
     * @brief get the remote devices
     * @details get the remote device with the specified states
     * @param states states
     * @return std::vector remote devices
     * @since 6
     */
    virtual std::vector<RawAddress> GetDevicesByStates(const std::vector<int> &states) = 0;

    /**
     * @brief get the state of device
     * @details get the state with the specified remote device
     * @param device  remote device
     * @return int @c not -1 state of the specified remote device
     *             @c -1 device is not exist
     * @since 6
     */
    virtual int GetDeviceState(const RawAddress &device) = 0;

    /**
     * @brief set the strategy of device
     * @details set the strategy with the specified remote device
     * @param device  remote device
     * @param strategy  specified strategy
     * @return int @c 0  success
     *             @c other failure
     * @since 6
     */
    virtual int SetConnectionStrategy(const RawAddress &device, int strategy) = 0;

    /**
     * @brief get the strategy of device
     * @details get the strategy with the specified remote device
     * @param device  remote device
     * @return int @c  strategy
     * @since 6
     */
    virtual int GetConnectionStrategy(const RawAddress &device) = 0;

    /**
     * @brief Set device's password. please call after OnServicePasswordRequired event.
     *
     * @param device device
     * @param password device's password
     * @return int @c 0 ok
     *             @c -1 fail
     * @since 6
     */
    virtual int SetDevicePassword(
        const RawAddress &device, const std::string &password, const std::string &userId = "") = 0;

    /**
     * @brief Pull phone book. please call after Connected.
     *
     * @param device device
     * @param param app parameter
     * @return int @c 0 ok
     *             @c -1 fail
     * @since 6
     */
    virtual int PullPhoneBook(const RawAddress &device, const IPbapPullPhoneBookParam &param) = 0;

    /**
     * @brief Set phone book. please call after Connected.
     *
     * @param device device
     * @param path path of phone book
     * @param flag
     *        @c PBAP_FLAG_GO_TO_ROOT and empty name--to root;
     *        @c PBAP_FLAG_GO_DOWN --go down;
     *        @c PBAP_FLAG_GO_UP --go up
     * @return int @c 0 ok
     *             @c -1 fail
     * @since 6
     */
    virtual int SetPhoneBook(const RawAddress &device, const std::u16string &name, int flag) = 0;

    /**
     * @brief Pull vCard listing. please call after Connected.
     *
     * @param device device
     * @param param app parameter
     * @return int @c 0 ok
     *             @c -1 fail
     * @since 6
     */
    virtual int PullvCardListing(const RawAddress &device, const IPbapPullvCardListingParam &param) = 0;

    /**
     * @brief Pull vCard entry. please call after Connected.
     *
     * @param device device
     * @param param app parameter
     * @return int @c 0 ok
     *             @c -1 fail
     * @since 6
     */
    virtual int PullvCardEntry(const RawAddress &device, const IPbapPullvCardEntryParam &param) = 0;
    /**
     * @brief is downloading.
     *
     * @param device device
     * @return int @c 0 ok
     *             @c -1 fail
     * @since 6
     */

    virtual bool IsDownloading(const RawAddress &device) = 0;

    /**
     * @brief abort downloading.
     *
     * @param device device
     * @return int @c 0 ok
     *             @c -1 fail
     * @since 6
     */
    virtual int AbortDownloading(const RawAddress &device) = 0;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // INTERFACE_PROFILE_PBAP_PCE_H