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

#ifndef PBAP_PSE_VCARD_MANAGER_H
#define PBAP_PSE_VCARD_MANAGER_H

#include <cstdint>
#include <cstring>
#include <map>
#include <vector>
#include "../obex/obex_types.h"
#include "pbap_pse_app_params.h"

namespace OHOS {
namespace bluetooth {
class PbapPseVcardManager {
public:
    struct PhoneBookResult {
        ObexRspCode rspCode_ {ObexRspCode::SUCCESS};
        bool phoneBookSizeOnly_ = false;
        uint16_t phoneBookSize_ = 0;  // VCard size
        bool newMissedCalls_ = false;
        uint8_t newMissedCallsSize_ = 0;                 // New missed calls size
        std::vector<uint8_t> primaryFolderVersion_ {};    // VCard PrimaryFolderVersion 16 bytes
        std::vector<uint8_t> secondaryFolderVersion_ {};  // VCard SecondaryFolderVersion 16 bytes
        std::vector<uint8_t> databaseIdentifier_ {};      // VCard DatabaseIdentifier 16 bytes
        std::vector<uint8_t> result_ {};                  // VCard bytes with utf-8
    };
    static void SetDbFile(const std::string dbFile);
    virtual ~PbapPseVcardManager() = default;

    static void PullPhoneBook(std::u16string nameWithFolder, const PbapPseAppParams &pbapAppParams,
        const uint32_t &supportedFeatures, PhoneBookResult &result);
    static void PullvCardListing(std::u16string currentPath, std::u16string folderName, const PbapPseAppParams &pbapAppParams,
        const uint32_t &supportedFeatures, PhoneBookResult &result);
    static void PullvCardEntry(std::u16string currentPath, std::u16string entryId,
        const PbapPseAppParams &pbapAppParams, const uint32_t &supportedFeatures, PhoneBookResult &result);

    static bool CheckPhoneBookPath(std::u16string path);
    static bool CheckVcardHandleId(const std::u16string &handleId);
    static int VcardHandleId2Int(const std::u16string &handleId);
    static std::vector<std::string> GetIncludeProperties(uint64_t propertySelector);
    static bool IsSupportedFolderVer(const uint32_t &supportedFeatures);
    static bool IsSupportedDbVer(const uint32_t &supportedFeatures);
    static bool IsSupportedXBtUCI(const uint32_t &supportedFeatures);
    static bool IsSupportedXBtUID(const uint32_t &supportedFeatures);

private:
    PbapPseVcardManager() = default;
    static std::string g_pbapDbFile;
    static const std::map<std::u16string, std::u16string> VIRTUAL_VCF_FOLDER_MAP;
    static const std::map<uint64_t, std::string> PROPERTY_MASK_MAP;
    static const std::map<char16_t, uint8_t> VCARD_HANDLE_CHAR_MAP;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PBAP_PSE_VCARD_MANAGER_H
