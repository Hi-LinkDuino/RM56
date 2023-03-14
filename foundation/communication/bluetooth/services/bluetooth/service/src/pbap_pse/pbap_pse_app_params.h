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

#ifndef PBAP_PSE_APP_PARAMS_H
#define PBAP_PSE_APP_PARAMS_H

#include <cstdint>
#include <map>
#include <memory>
#include <vector>
#include "../obex/obex_headers.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Pbap Pse App Params
 * wrap Pbap Pse App Params
 */
class PbapPseAppParams {
public:
    PbapPseAppParams() = default;
    virtual ~PbapPseAppParams() = default;
    bool Init(const ObexTlvParamters &obexTlvParamters);
    void AddToObexHeader(ObexHeader &hdr) const;
    static bool CheckParams(const ObexTlvParamters &obexTlvParamters);

    static const uint8_t ORDER = 0x01;
    static const uint8_t SEARCH_VALUE = 0x02;
    static const uint8_t SEARCH_PROPERTY = 0x03;
    static const uint8_t MAX_LIST_COUNT = 0x04;
    static const uint8_t LIST_START_OFFSET = 0x05;
    static const uint8_t PROPERTY_SELECTOR = 0x06;
    static const uint8_t FORMAT = 0x07;
    static const uint8_t PHONEBOOK_SIZE = 0x08;
    static const uint8_t NEW_MISSED_CALLS = 0x09;
    static const uint8_t PRIMARY_FOLDER_VERSION = 0x0A;
    static const uint8_t SECONDARY_FOLDER_VERSION = 0x0B;
    static const uint8_t VCARD_SELECTOR = 0x0C;
    static const uint8_t DATABASE_IDENTIFIER = 0x0D;
    static const uint8_t VCARD_SELECTOR_OPERATOR = 0x0E;
    static const uint8_t RESET_NEW_MISSED_CALLS = 0x0F;
    static const uint8_t PBAP_SUPPORTED_FEATURES = 0x10;

    const uint8_t *GetOrder() const;
    const std::vector<uint8_t> &GetSearchValueUtf8() const;
    const uint8_t *GetSearchProperty() const;
    const uint16_t *GetMaxListCount() const;
    const uint16_t *GetListStartOffset() const;
    const uint64_t *GetPropertySelector() const;
    const uint8_t *GetFormat() const;
    const uint16_t *GetPhonebookSize() const;
    const uint8_t *GetNewMissedCalls() const;
    const std::vector<uint8_t> &GetPrimaryFolderVersion() const;
    const std::vector<uint8_t> &GetSecondaryFolderVersion() const;
    const uint64_t *GetVcardSelector() const;
    const std::vector<uint8_t> &GetDatabaseIdentifier() const;
    const uint8_t *GetVcardSelectorOperator() const;
    const uint8_t *GetResetNewMissedCalls() const;
    const uint32_t *GetPbapSupportedFeatures() const;

    void SetOrder(uint8_t val);
    void SetSearchValueUtf8(const std::vector<uint8_t> &val);
    void SetSearchProperty(uint8_t val);
    void SetMaxListCount(uint16_t val);
    void SetListStartOffset(uint16_t val);
    void SetPropertySelector(uint64_t val);
    void SetFormat(uint8_t val);
    void SetPhonebookSize(uint16_t val);
    void SetNewMissedCalls(uint8_t val);
    void SetPrimaryFolderVersion(const std::vector<uint8_t> &val);
    void SetSecondaryFolderVersion(const std::vector<uint8_t> &val);
    void SetVcardSelector(uint64_t val);
    void SetDatabaseIdentifier(const std::vector<uint8_t> &val);
    void SetVcardSelectorOperator(uint8_t val);
    void SetResetNewMissedCalls(uint8_t val);
    void SetPbapSupportedFeatures(uint32_t val);

private:
    static const std::map<uint8_t, int> LENS_MAP;

    void SetOrderTlv(const ObexTlvParamters &obexTlvParamters);
    void SetSearchValueUtf8Tlv(const ObexTlvParamters &obexTlvParamters);
    void SetSearchPropertyTlv(const ObexTlvParamters &obexTlvParamters);
    void SetMaxListCountTlv(const ObexTlvParamters &obexTlvParamters);
    void SetListStartOffsetTlv(const ObexTlvParamters &obexTlvParamters);
    void SetPropertySelectorTlv(const ObexTlvParamters &obexTlvParamters);
    void SetFormatTlv(const ObexTlvParamters &obexTlvParamters);
    void SetPhonebookSizeTlv(const ObexTlvParamters &obexTlvParamters);
    void SetNewMissedCallsTlv(const ObexTlvParamters &obexTlvParamters);
    void SetPrimaryFolderVersionTlv(const ObexTlvParamters &obexTlvParamters);
    void SetSecondaryFolderVersionTlv(const ObexTlvParamters &obexTlvParamters);
    void SetVcardSelectorTlv(const ObexTlvParamters &obexTlvParamters);
    void SetDatabaseIdentifierTlv(const ObexTlvParamters &obexTlvParamters);
    void SetVcardSelectorOperatorTlv(const ObexTlvParamters &obexTlvParamters);
    void SetResetNewMissedCallsTlv(const ObexTlvParamters &obexTlvParamters);
    void SetPbapSupportedFeaturesTlv(const ObexTlvParamters &obexTlvParamters);

    void AddTlvOrder(ObexTlvParamters &obexTlvParamters) const;
    void AddTlvSearchValueUtf8(ObexTlvParamters &obexTlvParamters) const;
    void AddTlvSearchProperty(ObexTlvParamters &obexTlvParamters) const;
    void AddTlvMaxListCount(ObexTlvParamters &obexTlvParamters) const;
    void AddTlvListStartOffAddTlv(ObexTlvParamters &obexTlvParamters) const;
    void AddTlvPropertySelector(ObexTlvParamters &obexTlvParamters) const;
    void AddTlvFormat(ObexTlvParamters &obexTlvParamters) const;
    void AddTlvPhonebookSize(ObexTlvParamters &obexTlvParamters) const;
    void AddTlvNewMissedCalls(ObexTlvParamters &obexTlvParamters) const;
    void AddTlvPrimaryFolderVersion(ObexTlvParamters &obexTlvParamters) const;
    void AddTlvSecondaryFolderVersion(ObexTlvParamters &obexTlvParamters) const;
    void AddTlvVcardSelector(ObexTlvParamters &obexTlvParamters) const;
    void AddTlvDatabaseIdentifier(ObexTlvParamters &obexTlvParamters) const;
    void AddTlvVcardSelectorOperator(ObexTlvParamters &obexTlvParamters) const;
    void AddTlvReAddTlvNewMissedCalls(ObexTlvParamters &obexTlvParamters) const;
    void AddTlvPbapSupportedFeatures(ObexTlvParamters &obexTlvParamters) const;
    std::unique_ptr<uint8_t> order_ = nullptr;
    std::vector<uint8_t> searchValueUtf8_ {};
    std::unique_ptr<uint8_t> searchProperty_ = nullptr;
    std::unique_ptr<uint16_t> maxListCount_ = nullptr;
    std::unique_ptr<uint16_t> listStartOffset_ = nullptr;
    std::unique_ptr<uint64_t> propertySelector_ = nullptr;
    std::unique_ptr<uint8_t> format_ = nullptr;
    std::unique_ptr<uint16_t> phonebookSize_ = nullptr;
    std::unique_ptr<uint8_t> newMissedCalls_ = nullptr;
    std::vector<uint8_t> primaryFolderVer_ {};    // 16 bytes
    std::vector<uint8_t> secondaryFolderVer_ {};  // 16 bytes
    std::unique_ptr<uint64_t> vcardSelector_ = nullptr;
    std::vector<uint8_t> databaseIdentifier_ {};  // 16 bytes
    std::unique_ptr<uint8_t> vcardSelectorOp_ = nullptr;
    std::unique_ptr<uint8_t> resetNewMissedCalls_ = nullptr;
    std::unique_ptr<uint32_t> pbapSupportedFeatures_ = nullptr;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PBAP_PSE_APP_PARAMS_H
