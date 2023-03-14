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

#include "pbap_pce_parameter.h"
#include <algorithm>
#include <codecvt>
#include <fstream>
#include <locale>
#include <sstream>
#include "bt_def.h"
#include "ios"
#include "istream"
#include "iterator"
#include "new"
#include "ostream"
#include <cstdio>
#include "streambuf"
#include "string"
#include "vector"

namespace OHOS {
namespace bluetooth {
IPbapPullPhoneBookParam::IPbapPullPhoneBookParam()
    : name_(u""),
      propertySelector_(0),
      format_(0),
      maxListCount_(0),
      listStartOffset_(0),
      resetNewMissedCalls_(0),
      vCardSelector_(0),
      vCardSelectorOp_(0),
      specifiedBitset_(0ULL)
{}

IPbapPullPhoneBookParam::~IPbapPullPhoneBookParam()
{}

IPbapPullPhoneBookParam::IPbapPullPhoneBookParam(const IPbapPullPhoneBookParam &other)
    : name_(other.name_),
      propertySelector_(other.propertySelector_),
      format_(other.format_),
      maxListCount_(other.maxListCount_),
      listStartOffset_(other.listStartOffset_),
      resetNewMissedCalls_(other.resetNewMissedCalls_),
      vCardSelector_(other.vCardSelector_),
      vCardSelectorOp_(other.vCardSelectorOp_),
      specifiedBitset_(other.specifiedBitset_)
{}

std::string IPbapPullPhoneBookParam::ToDebugString() const
{
    std::stringstream ss;
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    ss << "IPbapPullPhoneBookParam:" << std::endl;
    ss << "name:" << converter.to_bytes(GetName())
       << std::endl;
    if (IsSpecified(IPbapPullPhoneBookParam::PROPERTY_SELECTOR)) {
        ss << "propertySelector:" << GetPropertySelector() << std::endl;
    }
    if (IsSpecified(IPbapPullPhoneBookParam::FORMAT)) {
        ss << "format:" << static_cast<int>(GetFormat()) << std::endl;
    }
    if (IsSpecified(IPbapPullPhoneBookParam::MAX_LIST_COUNT)) {
        ss << "maxListCount:" << GetMaxListCount() << std::endl;
    }
    if (IsSpecified(IPbapPullPhoneBookParam::LIST_START_OFFSET)) {
        ss << "listStartOffset:" << GetListStartOffset() << std::endl;
    }
    if (IsSpecified(IPbapPullPhoneBookParam::RESET_NEW_MISSED_CALLS)) {
        ss << "resetNewMissedCalls:" << static_cast<int>(GetResetNewMissedCalls()) << std::endl;
    }
    if (IsSpecified(IPbapPullPhoneBookParam::VCARD_SELECTOR)) {
        ss << "vCardSelector:" << GetvCardSelector() << std::endl;
    }
    if (IsSpecified(IPbapPullPhoneBookParam::VCARD_SELECTOROP)) {
        ss << "vCardSelectorOp:" << static_cast<int>(GetvCardSelectorOp()) << std::endl;
    }
    return ss.str();
}

bool IPbapPullPhoneBookParam::Validate() const
{
    if ((format_ != PBAP_FORMAT_VCARD2_1) && (format_ != PBAP_FORMAT_VCARD3_0)) {
        return false;
    }

    if ((vCardSelectorOp_ != PBAP_SELECTOR_OPERATOR_OR) && (vCardSelectorOp_ != PBAP_SELECTOR_OPERATOR_AND)) {
        return false;
    }

    return true;
}

std::u16string IPbapPullPhoneBookParam::GetName() const
{
    return name_;
}

void IPbapPullPhoneBookParam::SetName(const std::u16string &name)
{
    name_ = name;
    specifiedBitset_ |= NAME;
}

uint64_t IPbapPullPhoneBookParam::GetPropertySelector() const
{
    return propertySelector_;
}

void IPbapPullPhoneBookParam::SetPropertySelector(uint64_t propertySelector)
{
    propertySelector_ = propertySelector;
    specifiedBitset_ |= PROPERTY_SELECTOR;
}

uint8_t IPbapPullPhoneBookParam::GetFormat() const
{
    return format_;
}

void IPbapPullPhoneBookParam::SetFormat(uint8_t format)
{
    format_ = format;
    specifiedBitset_ |= FORMAT;
}

uint16_t IPbapPullPhoneBookParam::GetMaxListCount() const
{
    return maxListCount_;
}

void IPbapPullPhoneBookParam::SetMaxListCount(uint16_t maxListCount)
{
    maxListCount_ = maxListCount;
    specifiedBitset_ |= MAX_LIST_COUNT;
}

uint16_t IPbapPullPhoneBookParam::GetListStartOffset() const
{
    return listStartOffset_;
}

void IPbapPullPhoneBookParam::SetListStartOffset(uint16_t listStartOffset)
{
    listStartOffset_ = listStartOffset;
    specifiedBitset_ |= LIST_START_OFFSET;
}

uint8_t IPbapPullPhoneBookParam::GetResetNewMissedCalls() const
{
    return resetNewMissedCalls_;
}

void IPbapPullPhoneBookParam::SetResetNewMissedCalls(uint8_t resetNewMissedCalls)
{
    resetNewMissedCalls_ = resetNewMissedCalls;
    specifiedBitset_ |= RESET_NEW_MISSED_CALLS;
}

uint64_t IPbapPullPhoneBookParam::GetvCardSelector() const
{
    return vCardSelector_;
}

void IPbapPullPhoneBookParam::SetvCardSelector(uint64_t vCardSelector)
{
    vCardSelector_ = vCardSelector;
    specifiedBitset_ |= VCARD_SELECTOR;
}

uint8_t IPbapPullPhoneBookParam::GetvCardSelectorOp() const
{
    return vCardSelectorOp_;
}

void IPbapPullPhoneBookParam::SetvCardSelectorOp(uint8_t vCardSelectorOp)
{
    vCardSelectorOp_ = vCardSelectorOp;
    specifiedBitset_ |= VCARD_SELECTOROP;
}

uint64_t IPbapPullPhoneBookParam::GetSpecifiedBitset() const
{
    return specifiedBitset_;
}

void IPbapPullPhoneBookParam::SetSpecifiedBitset(uint64_t specifiedBitset)
{
    specifiedBitset_ = specifiedBitset;
}

bool IPbapPullPhoneBookParam::IsSpecified(int bitSpecified) const
{
    if (bitSpecified & specifiedBitset_) {
        return true;
    } else {
        return false;
    }
}

// IPbapPullvCardListingParam
IPbapPullvCardListingParam::IPbapPullvCardListingParam()
    : name_(u""),
      order_(0),
      searchValue_(""),
      searchProperty_(0),
      maxListCount_(0),
      listStartOffset_(0),
      resetNewMissedCalls_(0),
      vCardSelector_(0),
      vCardSelectorOp_(0),
      specifiedBitset_(0ULL)
{}

IPbapPullvCardListingParam::~IPbapPullvCardListingParam()
{}

IPbapPullvCardListingParam::IPbapPullvCardListingParam(const IPbapPullvCardListingParam &other)
    : name_(other.name_),
      order_(other.order_),
      searchValue_(other.searchValue_),
      searchProperty_(other.searchProperty_),
      maxListCount_(other.maxListCount_),
      listStartOffset_(other.listStartOffset_),
      resetNewMissedCalls_(other.resetNewMissedCalls_),
      vCardSelector_(other.vCardSelector_),
      vCardSelectorOp_(other.vCardSelectorOp_),
      specifiedBitset_(other.specifiedBitset_)
{}

std::string IPbapPullvCardListingParam::ToDebugString() const
{
    std::stringstream ss;
    ss << "IPbapPullvCardListingParam:" << std::endl;
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    ss << "name:" << converter.to_bytes(GetName())
       << std::endl;
    if (IsSpecified(IPbapPullvCardListingParam::ORDER)) {
        ss << "order:" << static_cast<int>(GetOrder()) << std::endl;
    }
    if (IsSpecified(IPbapPullvCardListingParam::SEARCH_VALUE)) {
        ss << "searchValue:" << GetSearchValue() << std::endl;
    }
    if (IsSpecified(IPbapPullvCardListingParam::SEARCH_PROPERTY)) {
        ss << "searchProperty:" << static_cast<int>(GetSearchProperty()) << std::endl;
    }
    if (IsSpecified(IPbapPullvCardListingParam::MAX_LIST_COUNT)) {
        ss << "maxListCount:" << GetMaxListCount() << std::endl;
    }
    if (IsSpecified(IPbapPullvCardListingParam::LIST_START_OFFSET)) {
        ss << "listStartOffset:" << GetListStartOffset() << std::endl;
    }
    if (IsSpecified(IPbapPullvCardListingParam::RESET_NEW_MISSED_CALLS)) {
        ss << "resetNewMissedCalls:" << static_cast<int>(GetResetNewMissedCalls()) << std::endl;
    }
    if (IsSpecified(IPbapPullvCardListingParam::VCARD_SELECTOR)) {
        ss << "vCardSelector:" << GetvCardSelector() << std::endl;
    }
    if (IsSpecified(IPbapPullvCardListingParam::VCARD_SELECTOROP)) {
        ss << "vCardSelectorOp:" << static_cast<int>(GetvCardSelectorOp()) << std::endl;
    }
    return ss.str();
}

bool IPbapPullvCardListingParam::Validate() const
{
    std::vector<int> orders = {
        PBAP_ORDER_INDEXED,
        PBAP_ORDER_ALPHANUMERIC,
        PBAP_ORDER_PHONETIC
    };
    if (std::find(orders.begin(), orders.end(), order_) == orders.end()) {
        return false;
    }

    std::vector<int> searchProperties = {
        PBAP_SEARCH_PROPERTY_NAME,
        PBAP_SEARCH_PROPERTY_NUMBER,
        PBAP_SEARCH_PROPERTY_SOUND
    };
    if (std::find(searchProperties.begin(), searchProperties.end(), searchProperty_) == searchProperties.end()) {
        return false;
    }

    if ((vCardSelectorOp_ != PBAP_SELECTOR_OPERATOR_OR) && (vCardSelectorOp_ != PBAP_SELECTOR_OPERATOR_AND)) {
        return false;
    }
    return true;
}

std::u16string IPbapPullvCardListingParam::GetName() const
{
    return name_;
}

void IPbapPullvCardListingParam::SetName(const std::u16string &name)
{
    name_ = name;
    specifiedBitset_ |= NAME;
}

uint8_t IPbapPullvCardListingParam::GetOrder() const
{
    return order_;
}

void IPbapPullvCardListingParam::SetOrder(uint8_t order)
{
    order_ = order;
    specifiedBitset_ |= ORDER;
}

std::string IPbapPullvCardListingParam::GetSearchValue() const
{
    return searchValue_;
}

void IPbapPullvCardListingParam::SetSearchValue(const std::string &searchValue)
{
    searchValue_ = searchValue;
    specifiedBitset_ |= SEARCH_VALUE;
}

uint8_t IPbapPullvCardListingParam::GetSearchProperty() const
{
    return searchProperty_;
}

void IPbapPullvCardListingParam::SetSearchProperty(uint8_t searchProperty)
{
    searchProperty_ = searchProperty;
    specifiedBitset_ |= SEARCH_PROPERTY;
}

uint16_t IPbapPullvCardListingParam::GetMaxListCount() const
{
    return maxListCount_;
}

void IPbapPullvCardListingParam::SetMaxListCount(uint16_t maxListCount)
{
    maxListCount_ = maxListCount;
    specifiedBitset_ |= MAX_LIST_COUNT;
}

uint16_t IPbapPullvCardListingParam::GetListStartOffset() const
{
    return listStartOffset_;
}

void IPbapPullvCardListingParam::SetListStartOffset(uint16_t listStartOffset)
{
    listStartOffset_ = listStartOffset;
    specifiedBitset_ |= LIST_START_OFFSET;
}

uint8_t IPbapPullvCardListingParam::GetResetNewMissedCalls() const
{
    return resetNewMissedCalls_;
}

void IPbapPullvCardListingParam::SetResetNewMissedCalls(uint8_t resetNewMissedCalls)
{
    resetNewMissedCalls_ = resetNewMissedCalls;
    specifiedBitset_ |= RESET_NEW_MISSED_CALLS;
}

uint64_t IPbapPullvCardListingParam::GetvCardSelector() const
{
    return vCardSelector_;
}

void IPbapPullvCardListingParam::SetvCardSelector(uint64_t vCardSelector)
{
    vCardSelector_ = vCardSelector;
    specifiedBitset_ |= VCARD_SELECTOR;
}

uint8_t IPbapPullvCardListingParam::GetvCardSelectorOp() const
{
    return vCardSelectorOp_;
}

void IPbapPullvCardListingParam::SetvCardSelectorOp(uint8_t vCardSelectorOp)
{
    vCardSelectorOp_ = vCardSelectorOp;
    specifiedBitset_ |= VCARD_SELECTOROP;
}

uint64_t IPbapPullvCardListingParam::GetSpecifiedBitset() const
{
    return specifiedBitset_;
}

void IPbapPullvCardListingParam::SetSpecifiedBitset(uint64_t specifiedBitset)
{
    specifiedBitset_ = specifiedBitset;
}

bool IPbapPullvCardListingParam::IsSpecified(int bitSpecified) const
{
    if (bitSpecified & specifiedBitset_) {
        return true;
    } else {
        return false;
    }
}

// IPbapPullvCardEntryParam
IPbapPullvCardEntryParam::IPbapPullvCardEntryParam()
    : name_(u""), propertySelector_(0), format_(0), specifiedBitset_(0ULL)
{}

IPbapPullvCardEntryParam::~IPbapPullvCardEntryParam()
{}

IPbapPullvCardEntryParam::IPbapPullvCardEntryParam(const IPbapPullvCardEntryParam &other)
    : name_(other.name_),
      propertySelector_(other.propertySelector_),
      format_(other.format_),
      specifiedBitset_(other.specifiedBitset_)
{}

std::string IPbapPullvCardEntryParam::ToDebugString() const
{
    std::stringstream ss;
    ss << "IPbapPullvCardEntryParam:" << std::endl;
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    ss << "name:" << converter.to_bytes(GetName())
       << std::endl;
    if (IsSpecified(IPbapPullvCardEntryParam::PROPERTY_SELECTOR)) {
        ss << "propertySelector:" << GetPropertySelector() << std::endl;
    }
    if (IsSpecified(IPbapPullvCardEntryParam::FORMAT)) {
        ss << "format:" << static_cast<int>(GetFormat()) << std::endl;
    }
    return ss.str();
}

bool IPbapPullvCardEntryParam::Validate() const
{
    if ((format_ != PBAP_FORMAT_VCARD2_1) && (format_ != PBAP_FORMAT_VCARD3_0)) {
        return false;
    }

    return true;
}

std::u16string IPbapPullvCardEntryParam::GetName() const
{
    return name_;
}

void IPbapPullvCardEntryParam::SetName(const std::u16string &name)
{
    name_ = name;
    specifiedBitset_ |= NAME;
}

uint64_t IPbapPullvCardEntryParam::GetPropertySelector() const
{
    return propertySelector_;
}

void IPbapPullvCardEntryParam::SetPropertySelector(uint64_t propertySelector)
{
    propertySelector_ = propertySelector;
    specifiedBitset_ |= PROPERTY_SELECTOR;
}

uint8_t IPbapPullvCardEntryParam::GetFormat() const
{
    return format_;
}

void IPbapPullvCardEntryParam::SetFormat(uint8_t format)
{
    format_ = format;
    specifiedBitset_ |= FORMAT;
}

uint64_t IPbapPullvCardEntryParam::GetSpecifiedBitset() const
{
    return specifiedBitset_;
}

void IPbapPullvCardEntryParam::SetSpecifiedBitset(uint64_t specifiedBitset)
{
    specifiedBitset_ = specifiedBitset;
}

bool IPbapPullvCardEntryParam::IsSpecified(int bitSpecified) const
{
    if (bitSpecified & specifiedBitset_) {
        return true;
    } else {
        return false;
    }
}

// IPbapPhoneBookData
IPbapPhoneBookData::IPbapPhoneBookData()
    : phoneBookSize_(0),
      primaryFolderVersion_(),
      secondaryFolderVersion_(),
      databaseIdentifier_(),
      deviceAddr_(""),
      vcardPath_(""),
      vcardFileName_(""),
      resultLoaded_(1),
      result_()
{}

IPbapPhoneBookData::~IPbapPhoneBookData()
{}

IPbapPhoneBookData::IPbapPhoneBookData(const IPbapPhoneBookData &other)
    : phoneBookSize_(other.phoneBookSize_),
      primaryFolderVersion_(other.primaryFolderVersion_),
      secondaryFolderVersion_(other.secondaryFolderVersion_),
      databaseIdentifier_(other.databaseIdentifier_),
      deviceAddr_(other.deviceAddr_),
      vcardPath_(other.vcardPath_),
      vcardFileName_(other.vcardFileName_),
      resultLoaded_(other.resultLoaded_),
      result_(other.result_)
{}

void IPbapPhoneBookData::Load(std::vector<uint8_t> &retResult)
{
    std::string fileName = deviceAddr_ + vcardPath_ + vcardFileName_;
    std::ifstream ifs(fileName, std::ios::in | std::ios::binary);
    if (!ifs.is_open() || resultLoaded_ == 1) {
        return;
    } else {
        retResult.clear();
        retResult.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        resultLoaded_ = 1;
        ifs.close();
        remove(fileName.c_str());
    }
    return;
}
}  // namespace bluetooth
}  // namespace OHOS