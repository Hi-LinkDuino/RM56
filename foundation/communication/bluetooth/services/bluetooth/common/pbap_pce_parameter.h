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

#ifndef PBAP_PCE_PARAMETER_H
#define PBAP_PCE_PARAMETER_H

#include <stdint.h>
#include <string>
#include <vector>

#include "cstdint"
#include "iosfwd"

namespace OHOS {
namespace bluetooth {
class IPbapPullPhoneBookParam {
public:
    IPbapPullPhoneBookParam();
    virtual ~IPbapPullPhoneBookParam();
    IPbapPullPhoneBookParam(const IPbapPullPhoneBookParam &other);
    std::string ToDebugString() const;
    bool Validate() const;
    IPbapPullPhoneBookParam &operator=(const IPbapPullPhoneBookParam &other) = delete;
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

class IPbapPullvCardListingParam {
public:
    IPbapPullvCardListingParam();
    virtual ~IPbapPullvCardListingParam();
    IPbapPullvCardListingParam(const IPbapPullvCardListingParam &other);
    IPbapPullvCardListingParam &operator=(const IPbapPullvCardListingParam &other) = delete;
    std::string ToDebugString() const;
    bool Validate() const;
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

class IPbapPullvCardEntryParam {
public:
    IPbapPullvCardEntryParam();
    virtual ~IPbapPullvCardEntryParam();
    IPbapPullvCardEntryParam(const IPbapPullvCardEntryParam &other);
    IPbapPullvCardEntryParam &operator=(const IPbapPullvCardEntryParam &other) = delete;
    std::string ToDebugString() const;
    bool Validate() const;
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
 * @brief
 *
 * @since 1.0
 * @version 1.0
 */
class IPbapPhoneBookData {
public:
    explicit IPbapPhoneBookData();
    virtual ~IPbapPhoneBookData();
    explicit IPbapPhoneBookData(const IPbapPhoneBookData &other);
    IPbapPhoneBookData &operator=(const IPbapPhoneBookData &other);
    void Load(std::vector<uint8_t> &retResult);
    uint16_t phoneBookSize_ = 0;                      // VCard size
    std::vector<uint8_t> primaryFolderVersion_ {};    // VCard PrimaryFolderVersion 16 bytes
    std::vector<uint8_t> secondaryFolderVersion_ {};  // VCard SecondaryFolderVersion 16 bytes
    std::vector<uint8_t> databaseIdentifier_ {};      // VCard DatabaseIdentifier 16 bytes
    std::string deviceAddr_ = "";                     // device address
    std::string vcardPath_ = "";                      // vcard path
    std::string vcardFileName_ = "";                  // file name
    uint16_t resultLoaded_ = 1;                       // result loaded:1 , otherwise 0
    std::vector<uint8_t> result_ {};                  // VCard bytes with utf-8
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // PBAP_PCE_PARAMETER_H
