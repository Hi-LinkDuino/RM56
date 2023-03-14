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

#include "bluetooth_pbap_pce_parameter.h"

namespace OHOS {
namespace Bluetooth {
// BluetoothIPbapPullPhoneBookParam
BluetoothIPbapPullPhoneBookParam::BluetoothIPbapPullPhoneBookParam(const bluetooth::IPbapPullPhoneBookParam &other)
{
    SetName(other.GetName());
    SetPropertySelector(other.GetPropertySelector());
    SetFormat(other.GetFormat());
    SetMaxListCount(other.GetMaxListCount());
    SetListStartOffset(other.GetListStartOffset());
    SetResetNewMissedCalls(other.GetResetNewMissedCalls());
    SetvCardSelector(other.GetvCardSelector());
    SetvCardSelectorOp(other.GetvCardSelectorOp());
    SetSpecifiedBitset(other.GetSpecifiedBitset());
}

BluetoothIPbapPullPhoneBookParam::BluetoothIPbapPullPhoneBookParam(const BluetoothIPbapPullPhoneBookParam &other)
{
    SetName(other.GetName());
    SetPropertySelector(other.GetPropertySelector());
    SetFormat(other.GetFormat());
    SetMaxListCount(other.GetMaxListCount());
    SetListStartOffset(other.GetListStartOffset());
    SetResetNewMissedCalls(other.GetResetNewMissedCalls());
    SetvCardSelector(other.GetvCardSelector());
    SetvCardSelectorOp(other.GetvCardSelectorOp());
    SetSpecifiedBitset(other.GetSpecifiedBitset());
}

bool BluetoothIPbapPullPhoneBookParam::Marshalling(Parcel &parcel) const
{
    bool status = false;
    status = parcel.WriteString16(GetName());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt64(GetPropertySelector());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt32(GetFormat());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt32(GetMaxListCount());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt32(GetListStartOffset());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt32(GetResetNewMissedCalls());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt64(GetvCardSelector());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt32(GetvCardSelectorOp());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt64(GetSpecifiedBitset());

    return status;
}

bool BluetoothIPbapPullPhoneBookParam::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

bool BluetoothIPbapPullPhoneBookParam::ReadFromParcel(Parcel &parcel)
{
    bool status = false;
    std::u16string valString16;
    int64_t val64 = 0;
    int val = 0;
    status = parcel.ReadString16(valString16);
    if (!status) {
        return status;
    }
    SetName(valString16);
    status = parcel.ReadInt64(val64);
    if (!status) {
        return status;
    }
    SetPropertySelector(val64);
    status = parcel.ReadInt32(val);
    if (!status) {
        return status;
    }
    SetFormat(val);
    status = parcel.ReadInt32(val);
    if (!status) {
        return status;
    }
    SetMaxListCount(val);
    status = parcel.ReadInt32(val);
    if (!status) {
        return status;
    }
    SetListStartOffset(val);
    status = parcel.ReadInt32(val);
    if (!status) {
        return status;
    }
    SetResetNewMissedCalls(val);
    status = parcel.ReadInt64(val64);
    if (!status) {
        return status;
    }
    SetvCardSelector(val64);
    status = parcel.ReadInt32(val);
    if (!status) {
        return status;
    }
    SetvCardSelectorOp(val);
    status = parcel.ReadInt64(val64);
    if (!status) {
        return status;
    }
    SetSpecifiedBitset(val64);

    return status;
}

BluetoothIPbapPullPhoneBookParam *BluetoothIPbapPullPhoneBookParam::Unmarshalling(Parcel &parcel)
{
    BluetoothIPbapPullPhoneBookParam *pullPhoneBookParam = new BluetoothIPbapPullPhoneBookParam();
    if (pullPhoneBookParam != nullptr && !pullPhoneBookParam->ReadFromParcel(parcel)) {
        delete pullPhoneBookParam;
        pullPhoneBookParam = nullptr;
    }
    return pullPhoneBookParam;
}

// BluetoothIPbapPullvCardListingParam
BluetoothIPbapPullvCardListingParam::BluetoothIPbapPullvCardListingParam(
    const bluetooth::IPbapPullvCardListingParam &other)
{
    SetName(other.GetName());
    SetOrder(other.GetOrder());
    SetSearchValue(other.GetSearchValue());
    SetSearchProperty(other.GetSearchProperty());
    SetMaxListCount(other.GetMaxListCount());
    SetListStartOffset(other.GetListStartOffset());
    SetResetNewMissedCalls(other.GetResetNewMissedCalls());
    SetvCardSelector(other.GetvCardSelector());
    SetvCardSelectorOp(other.GetvCardSelectorOp());
    SetSpecifiedBitset(other.GetSpecifiedBitset());
}

BluetoothIPbapPullvCardListingParam::BluetoothIPbapPullvCardListingParam(
    const BluetoothIPbapPullvCardListingParam &other)
{
    SetName(other.GetName());
    SetOrder(other.GetOrder());
    SetSearchValue(other.GetSearchValue());
    SetSearchProperty(other.GetSearchProperty());
    SetMaxListCount(other.GetMaxListCount());
    SetListStartOffset(other.GetListStartOffset());
    SetResetNewMissedCalls(other.GetResetNewMissedCalls());
    SetvCardSelector(other.GetvCardSelector());
    SetvCardSelectorOp(other.GetvCardSelectorOp());
    SetSpecifiedBitset(other.GetSpecifiedBitset());
}

bool BluetoothIPbapPullvCardListingParam::Marshalling(Parcel &parcel) const
{
    bool status = false;
    status = parcel.WriteString16(GetName());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt32(GetOrder());
    if (!status) {
        return status;
    }
    status = parcel.WriteString(GetSearchValue());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt32(GetSearchProperty());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt32(GetMaxListCount());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt32(GetListStartOffset());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt32(GetResetNewMissedCalls());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt64(GetvCardSelector());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt32(GetvCardSelectorOp());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt64(GetSpecifiedBitset());

    return status;
}

bool BluetoothIPbapPullvCardListingParam::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

bool BluetoothIPbapPullvCardListingParam::ReadFromParcel(Parcel &parcel)
{
    std::u16string valString16;
    std::string valString;
    int64_t val64 = 0;
    int val = 0;
    if (!parcel.ReadString16(valString16)) {
        return false;
    }
    SetName(valString16);
    if (!parcel.ReadInt32(val)) {
        return false;
    }
    SetOrder(val);
    if (!parcel.ReadString(valString)) {
        return false;
    }
    SetSearchValue(valString);
    if (!parcel.ReadInt32(val)) {
        return false;
    }
    SetSearchProperty(val);
    if (!parcel.ReadInt32(val)) {
        return false;
    }
    SetMaxListCount(val);
    if (!parcel.ReadInt32(val)) {
        return false;
    }
    SetListStartOffset(val);
    if (!parcel.ReadInt32(val)) {
        return false;
    }
    SetResetNewMissedCalls(val);
    if (!parcel.ReadInt64(val64)) {
        return false;
    }
    SetvCardSelector(val64);
    if (!parcel.ReadInt32(val)) {
        return false;
    }
    SetvCardSelectorOp(val);
    if (!parcel.ReadInt64(val64)) {
        return false;
    }
    SetSpecifiedBitset(val64);

    return true;
}

BluetoothIPbapPullvCardListingParam *BluetoothIPbapPullvCardListingParam::Unmarshalling(Parcel &parcel)
{
    BluetoothIPbapPullvCardListingParam *pullvCardListingParam = new BluetoothIPbapPullvCardListingParam();
    if (pullvCardListingParam != nullptr && !pullvCardListingParam->ReadFromParcel(parcel)) {
        delete pullvCardListingParam;
        pullvCardListingParam = nullptr;
    }
    return pullvCardListingParam;
}

// BluetoothIPbapPullvCardEntryParam
BluetoothIPbapPullvCardEntryParam::BluetoothIPbapPullvCardEntryParam(
    const bluetooth::IPbapPullvCardEntryParam &other)
{
    SetName(other.GetName());
    SetPropertySelector(other.GetPropertySelector());
    SetFormat(other.GetFormat());
    SetSpecifiedBitset(other.GetSpecifiedBitset());
}

BluetoothIPbapPullvCardEntryParam::BluetoothIPbapPullvCardEntryParam(const BluetoothIPbapPullvCardEntryParam &other)
{
    SetName(other.GetName());
    SetPropertySelector(other.GetPropertySelector());
    SetFormat(other.GetFormat());
    SetSpecifiedBitset(other.GetSpecifiedBitset());
}

bool BluetoothIPbapPullvCardEntryParam::Marshalling(Parcel &parcel) const
{
    bool status = false;
    status = parcel.WriteString16(GetName());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt64(GetPropertySelector());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt32(GetFormat());
    if (!status) {
        return status;
    }
    status = parcel.WriteInt64(GetSpecifiedBitset());

    return status;
}

bool BluetoothIPbapPullvCardEntryParam::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

bool BluetoothIPbapPullvCardEntryParam::ReadFromParcel(Parcel &parcel)
{
    bool status = false;
    std::u16string valString16;
    int64_t val64 = 0;
    int val = 0;
    status = parcel.ReadString16(valString16);
    if (!status) {
        return status;
    }
    SetName(valString16);
    status = parcel.ReadInt64(val64);
    if (!status) {
        return status;
    }
    SetPropertySelector(val64);
    status = parcel.ReadInt32(val);
    if (!status) {
        return status;
    }
    SetFormat(val);
    status = parcel.ReadInt64(val64);
    if (!status) {
        return status;
    }
    SetSpecifiedBitset(val64);
    return status;
}

BluetoothIPbapPullvCardEntryParam *BluetoothIPbapPullvCardEntryParam::Unmarshalling(Parcel &parcel)
{
    BluetoothIPbapPullvCardEntryParam *pullvCardEntryParam = new BluetoothIPbapPullvCardEntryParam();
    if (pullvCardEntryParam != nullptr && !pullvCardEntryParam->ReadFromParcel(parcel)) {
        delete pullvCardEntryParam;
        pullvCardEntryParam = nullptr;
    }
    return pullvCardEntryParam;
}

// BluetoothIPbapPhoneBookData functions
BluetoothIPbapPhoneBookData::BluetoothIPbapPhoneBookData(const bluetooth::IPbapPhoneBookData &other)
{
    phoneBookSize_ = other.phoneBookSize_;
    primaryFolderVersion_ = other.primaryFolderVersion_;
    secondaryFolderVersion_ = other.secondaryFolderVersion_;
    databaseIdentifier_ = other.databaseIdentifier_;
    deviceAddr_ = other.deviceAddr_;
    vcardPath_ = other.vcardPath_;
    vcardFileName_ = other.vcardFileName_;
    resultLoaded_ = other.resultLoaded_;
    result_ = other.result_;
}

BluetoothIPbapPhoneBookData::BluetoothIPbapPhoneBookData(const BluetoothIPbapPhoneBookData &other)
{
    phoneBookSize_ = other.phoneBookSize_;
    primaryFolderVersion_ = other.primaryFolderVersion_;
    secondaryFolderVersion_ = other.secondaryFolderVersion_;
    databaseIdentifier_ = other.databaseIdentifier_;
    deviceAddr_ = other.deviceAddr_;
    vcardPath_ = other.vcardPath_;
    vcardFileName_ = other.vcardFileName_;
    resultLoaded_ = other.resultLoaded_;
    result_ = other.result_;
}

bool BluetoothIPbapPhoneBookData::Marshalling(Parcel &parcel) const
{
    bool status = false;
    status = parcel.WriteUint16(phoneBookSize_);
    if (!status) {
        return status;
    }

    status = parcel.WriteUInt8Vector(primaryFolderVersion_);
    if (!status) {
        return status;
    }

    status = parcel.WriteUInt8Vector(secondaryFolderVersion_);
    if (!status) {
        return status;
    }

    status = parcel.WriteUInt8Vector(databaseIdentifier_);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(deviceAddr_);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(vcardPath_);
    if (!status) {
        return status;
    }

    status = parcel.WriteString(vcardFileName_);
    if (!status) {
        return status;
    }

    status = parcel.WriteUint16(resultLoaded_);
    if (!status) {
        return status;
    }

    status = parcel.WriteUInt8Vector(result_);
    return status;
}

bool BluetoothIPbapPhoneBookData::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

bool BluetoothIPbapPhoneBookData::ReadFromParcel(Parcel &parcel)
{
    bool status = false;
    status = parcel.ReadUint16(phoneBookSize_);
    if (!status) {
        return status;
    }

    status = parcel.ReadUInt8Vector(&primaryFolderVersion_);
    if (!status) {
        return status;
    }

    status = parcel.ReadUInt8Vector(&secondaryFolderVersion_);
    if (!status) {
        return status;
    }

    status = parcel.ReadUInt8Vector(&databaseIdentifier_);
    if (!status) {
        return status;
    }

    status = parcel.ReadString(deviceAddr_);
    if (!status) {
        return status;
    }
    status = parcel.ReadString(vcardPath_);
    if (!status) {
        return status;
    }
    status = parcel.ReadString(vcardFileName_);
    if (!status) {
        return status;
    }
    status = parcel.ReadUint16(resultLoaded_);
    if (!status) {
        return status;
    }

    status = parcel.ReadUInt8Vector(&result_);
    return status;
}

BluetoothIPbapPhoneBookData *BluetoothIPbapPhoneBookData::Unmarshalling(Parcel &parcel)
{
    BluetoothIPbapPhoneBookData *phoneBookData = new BluetoothIPbapPhoneBookData();
    if (phoneBookData != nullptr && !phoneBookData->ReadFromParcel(parcel)) {
        delete phoneBookData;
        phoneBookData = nullptr;
    }
    return phoneBookData;
}
}  // namespace Bluetooth
}  // namespace OHOS
