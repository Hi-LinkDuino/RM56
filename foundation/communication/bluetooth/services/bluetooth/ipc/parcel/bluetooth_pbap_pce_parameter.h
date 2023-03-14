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

#ifndef BLUETOOTH_PARCEL_PBAP_PCE_PARAMETER_H
#define BLUETOOTH_PARCEL_PBAP_PCE_PARAMETER_H

#include "parcel.h"
#include "pbap_pce_parameter.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief Pbap PullPhoneBook Param
 *
 * @since 1.0
 * @version 1.0
 */
class BluetoothIPbapPullPhoneBookParam : public Parcelable, public bluetooth::IPbapPullPhoneBookParam {
public:
    BluetoothIPbapPullPhoneBookParam() = default;
    virtual ~BluetoothIPbapPullPhoneBookParam() = default;
    BluetoothIPbapPullPhoneBookParam(const bluetooth::IPbapPullPhoneBookParam& other);
    BluetoothIPbapPullPhoneBookParam(const BluetoothIPbapPullPhoneBookParam& other);
    /*
     * @brief Write BluetoothIPbapPullPhoneBookParam values to parcel.
     *
     * @param parcel Parcel which store BluetoothIPbapPullPhoneBookParam values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool WriteToParcel(Parcel &parcel);

    bool Marshalling(Parcel &parcel) const override;

    static BluetoothIPbapPullPhoneBookParam *Unmarshalling(Parcel &parcel);
    /**
     * @brief Read BluetoothIPbapPullPhoneBookParam values from parcel.
     *
     * @param parcel Parcel which bring BluetoothIPbapPullPhoneBookParam values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool ReadFromParcel(Parcel &parcel);
};

/**
 * @brief Pbap PullvCardListing Param
 *
 * @since 1.0
 * @version 1.0
 */
class BluetoothIPbapPullvCardListingParam : public Parcelable, public bluetooth::IPbapPullvCardListingParam {
public:
    BluetoothIPbapPullvCardListingParam() = default;
    virtual ~BluetoothIPbapPullvCardListingParam() = default;
    BluetoothIPbapPullvCardListingParam(const bluetooth::IPbapPullvCardListingParam& other);
    BluetoothIPbapPullvCardListingParam(const BluetoothIPbapPullvCardListingParam& other);

    bool Marshalling(Parcel &parcel) const override;

    static BluetoothIPbapPullvCardListingParam *Unmarshalling(Parcel &parcel);
    /*
     * @brief Write BluetoothIPbapPullvCardListingParam values to parcel.
     *
     * @param parcel Parcel which store BluetoothIPbapPullvCardListingParam values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool WriteToParcel(Parcel &parcel);

    /**
     * @brief Read BluetoothIPbapPullvCardListingParam values from parcel.
     *
     * @param parcel Parcel which bring BluetoothIPbapPullvCardListingParam values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool ReadFromParcel(Parcel &parcel);
};

/**
 * @brief Pbap PullvCardEntry Param
 *
 * @since 1.0
 * @version 1.0
 */
class BluetoothIPbapPullvCardEntryParam : public Parcelable, public bluetooth::IPbapPullvCardEntryParam {
public:
    BluetoothIPbapPullvCardEntryParam() = default;
    virtual ~BluetoothIPbapPullvCardEntryParam() = default;
    BluetoothIPbapPullvCardEntryParam(const bluetooth::IPbapPullvCardEntryParam& other);
    BluetoothIPbapPullvCardEntryParam(const BluetoothIPbapPullvCardEntryParam& other);
    bool Marshalling(Parcel &parcel) const override;

    static BluetoothIPbapPullvCardEntryParam *Unmarshalling(Parcel &parcel);
    /*
     * @brief Write BluetoothIPbapPullvCardEntryParam values to parcel.
     *
     * @param parcel Parcel which store BluetoothIPbapPullvCardEntryParam values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool WriteToParcel(Parcel &parcel);

    /**
     * @brief Read BluetoothIPbapPullvCardEntryParam values from parcel.
     *
     * @param parcel Parcel which bring BluetoothIPbapPullvCardEntryParam values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool ReadFromParcel(Parcel &parcel);
};


/**
 * @brief Pbap PhoneBook Data
 *
 * @since 1.0
 * @version 1.0
 */
class BluetoothIPbapPhoneBookData : public Parcelable, public bluetooth::IPbapPhoneBookData {
public:
    explicit BluetoothIPbapPhoneBookData() = default;
    virtual ~BluetoothIPbapPhoneBookData() = default;
    BluetoothIPbapPhoneBookData(const bluetooth::IPbapPhoneBookData& other);
    BluetoothIPbapPhoneBookData(const BluetoothIPbapPhoneBookData& other);
    bool Marshalling(Parcel &parcel) const override;

    static BluetoothIPbapPhoneBookData *Unmarshalling(Parcel &parcel);
    /*
     * @brief Write BluetoothIPbapPhoneBookData values to parcel.
     *
     * @param parcel Parcel which store BluetoothIPbapPhoneBookData values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool WriteToParcel(Parcel &parcel);

    /**
     * @brief Read BluetoothIPbapPhoneBookData values from parcel.
     *
     * @param parcel Parcel which bring BluetoothIPbapPhoneBookData values.
     * @return Operation result.
     * @since 1.0
     * @version 1.0
     */
    bool ReadFromParcel(Parcel &parcel);
};

}  // namespace Bluetooth
}  // namespace OHOS

#endif // BLUETOOTH_PARCEL_PBAP_PCE_PARAMETER_H