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

#include "pbap_pse_vcard_manager.h"
#include <algorithm>
#include <cstring>
#include <exception>
#include <functional>
#include <set>
#include <sstream>
#include "../obex/obex_utils.h"
#include "data_access.h"
#include "log.h"
#include "pbap_pse_def.h"
#include "stub/vcard_util.h"

using namespace stub;

namespace OHOS {
namespace bluetooth {
class PbapPseVcardDataAccess {
public:
    struct SetPullvCardEntryParam {
        std::u16string folderId = u"";
        std::u16string entryId = u"";
        uint32_t supportedFeatures = 0;
    };
    static void SetPullvCardListingSize(const std::unique_ptr<DataAccess> &dataAccess, const std::u16string &folderId,
        PbapPseVcardManager::PhoneBookResult &result, const PbapPseAppParams &pbapAppParams)
    {
        PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
        std::string select = "select count(distinct vcard_uid) from vcard_view t where folder_id = ? ";
        select += GetVcardSelectorWhere(pbapAppParams);
        if (pbapAppParams.GetSearchValueUtf8().size() > 0) {
            select += GetVcardSearchPropertyWhere(pbapAppParams);
        }
        auto stmt = dataAccess->CreateStatement(select);
        if (!stmt) {
            result.rspCode_ = ObexRspCode::INTERNAL_SERVER_ERROR;
            return;
        }
        int pIndex = 1;
        stmt->SetParam16String(pIndex++, folderId);
        if (pbapAppParams.GetSearchValueUtf8().size() > 0) {
            const uint8_t *data = pbapAppParams.GetSearchValueUtf8().data();
            std::string searchValue(data, data + pbapAppParams.GetSearchValueUtf8().size());
            stmt->SetParamString(pIndex++, searchValue);
        }
        auto dataResult = stmt->Query();
        int count = 0;
        if (dataResult->Next()) {
            count = dataResult->GetInt(0);
        }
        result.phoneBookSize_ = count;
    }

    static void SetPullvCardListing(const std::unique_ptr<DataAccess> &dataAccess, const std::u16string &folderId,
        PbapPseVcardManager::PhoneBookResult &result, const PbapPseAppParams &pbapAppParams)
    {
        PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
        auto vcardListing = SelectPullvCardListing(dataAccess, folderId, pbapAppParams);
        if (vcardListing.size() == 0) {
            PBAP_PSE_LOG_DEBUG("SetPullvCardListing size is 0");
            return;
        }
        std::stringstream ss;
        ss << "<?xml version=\"1.0\"?>";
        ss << "<!DOCTYPE vcard-listing SYSTEM \"vcard-listing.dtd\">";
        ss << "<vCard-listing version=\"1.0\">";
        for (auto &item : vcardListing) {
            ss << "<card handle=\"";
            ss << std::hex << item.first;
            ss << ".vcf\" name=\"";
            ss << NameEncodeForXml(item.second);
            ss << "\"/>";
        }
        ss << "</vCard-listing>";
        std::string xml = ss.str();
        result.result_.insert(result.result_.end(), xml.begin(), xml.end());
    }

    static std::string NameEncodeForXml(const std::string &name)
    {
        std::stringstream ss;
        for (char c : name) {
            switch (c) {
                case '<':
                    ss << "&lt;";
                    break;
                case '>':
                    ss << "&gt;";
                    break;
                case '\"':
                    ss << "&quot;";
                    break;
                case '\'':
                    ss << "&#039;";
                    break;
                case '&':
                    ss << "&amp;";
                    break;

                default:
                    ss << c;
                    break;
            }
        }
        return ss.str();
    }

    static void SetNewMissedCalls(const std::unique_ptr<DataAccess> &dataAccess, const std::u16string &folderId,
        PbapPseVcardManager::PhoneBookResult &result)
    {
        PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
        std::string select =
            "select count(vcard_uid) from vcard where vcard_folder_id = ? and vcard_new_missed_call = 1";
        auto stmt = dataAccess->CreateStatement(select);
        if (!stmt) {
            result.rspCode_ = ObexRspCode::INTERNAL_SERVER_ERROR;
            return;
        }
        stmt->SetParam16String(1, folderId);
        auto dataResult = stmt->Query();
        int count = 0;
        if (dataResult->Next()) {
            count = dataResult->GetInt(0);
        }
        result.newMissedCallsSize_ = count;
    }

    static void SetPhoneBookSize(const std::unique_ptr<DataAccess> &dataAccess, const std::u16string &folderId,
        PbapPseVcardManager::PhoneBookResult &result, const PbapPseAppParams &pbapAppParams)
    {
        PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
        std::string select = "select count(distinct vcard_uid) from vcard_view t where folder_id = ? ";
        select += GetVcardSelectorWhere(pbapAppParams);

        auto stmt = dataAccess->CreateStatement(select);
        if (!stmt) {
            result.rspCode_ = ObexRspCode::INTERNAL_SERVER_ERROR;
            return;
        }
        stmt->SetParam16String(1, folderId);
        auto dataResult = stmt->Query();
        int count = 0;
        if (dataResult->Next()) {
            count = dataResult->GetInt(0);
        }
        result.phoneBookSize_ = count;
    }

    static void SetPhoneBook(const std::unique_ptr<DataAccess> &dataAccess, const std::u16string &folderId,
        PbapPseVcardManager::PhoneBookResult &result, const PbapPseAppParams &pbapAppParams,
        const uint32_t &supportedFeatures)
    {
        PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
        auto vcardUids = SelectPhoneBookVcardUids(dataAccess, folderId, pbapAppParams);

        // 5.1.4.2 Format :vCard2.1, vCard3.0
        bool isOutVcard21 = true;  // The format vCard 2.1 shall be the default format if this header is not specified.
        if (pbapAppParams.GetFormat()) {
            // 0x00 = 2.1 0x01 = 3.0
            isOutVcard21 = (*pbapAppParams.GetFormat() == 0x00);
        }
        // 5.1.4.11 ResetNewMissedCalls :not support in this impl

        // 5.1.4.1 PropertySelector :PropertyMask (64-bit value)
        std::vector<std::string> includeProperties;
        // Mandatory properties for vCard 2.1 are VERSION ,N and TEL.
        // Mandatory properties for vCard 3.0 are VERSION, N, FN and TEL.
        std::vector<std::string> mandatoryProperties;
        mandatoryProperties.push_back("VERSION");
        mandatoryProperties.push_back("N");
        if (!isOutVcard21) {
            mandatoryProperties.push_back("FN");
        }
        mandatoryProperties.push_back("TEL");
        if (pbapAppParams.GetPropertySelector()) {
            includeProperties = PbapPseVcardManager::GetIncludeProperties(*pbapAppParams.GetPropertySelector());
            if (includeProperties.size() > 0) {
                includeProperties.insert(
                    includeProperties.end(), mandatoryProperties.begin(), mandatoryProperties.end());
            }
        }
        const VCardVersion outVer = isOutVcard21 ? VCardVersion::VER_2_1 : VCardVersion::VER_3_0;

        std::function<void(VCard &)> fun = [&mandatoryProperties, &result, &outVer](VCard &vcard) {
            // 5.1.4.1 all Mandatory here means that the PSE shall always return the properties VERSION, N and TEL
            // vCard 2.1 or VERSION, N, FN and TEL for a vCard 3.0.
            AddMissProperties(vcard, mandatoryProperties);

            if (vcard.Properties().size() > 0) {
                std::vector<uint8_t> vcardBytes = VCardUtil::Build(vcard, outVer);
                result.result_.insert(result.result_.end(), vcardBytes.begin(), vcardBytes.end());
            }
        };
        SelectPhoneBookVcardList(
            dataAccess, vcardUids, GetPropertySelectorWhere(includeProperties), supportedFeatures, fun);
    }

    static void SetPullvCardEntry(
            const std::unique_ptr<DataAccess> &dataAccess,
            const SetPullvCardEntryParam& param,
            const PbapPseAppParams &pbapAppParams, PbapPseVcardManager::PhoneBookResult &result)
    {
        PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
        auto vcardUids = SelectVcardEntryUid(dataAccess, param.folderId, param.entryId, pbapAppParams);
        if (vcardUids.size() == 0) {
            PBAP_PSE_LOG_DEBUG("SelectPhoneBookVcardUids size = 0");
            result.rspCode_ = ObexRspCode::NOT_FOUND;
            return;
        }
        // 5.1.4.2 Format :vCard2.1, vCard3.0
        bool isOutVcard21 = true;  // The format vCard 2.1 shall be the default format if this header is not specified.
        if (pbapAppParams.GetFormat()) {
            // 0x00 = 2.1 0x01 = 3.0
            isOutVcard21 = (*pbapAppParams.GetFormat() == 0x00);
        }

        // 5.1.4.1 PropertySelector :PropertyMask (64-bit value)
        std::vector<std::string> includeProperties;
        // Mandatory properties for vCard 2.1 are VERSION ,N and TEL.
        // Mandatory properties for vCard 3.0 are VERSION, N, FN and TEL.
        std::vector<std::string> mandatoryProperties;
        mandatoryProperties.push_back("VERSION");
        mandatoryProperties.push_back("N");
        if (!isOutVcard21) {
            mandatoryProperties.push_back("FN");
        }
        mandatoryProperties.push_back("TEL");
        if (pbapAppParams.GetPropertySelector()) {
            includeProperties = PbapPseVcardManager::GetIncludeProperties(*pbapAppParams.GetPropertySelector());
            if (includeProperties.size() > 0) {
                includeProperties.insert(
                    includeProperties.end(), mandatoryProperties.begin(), mandatoryProperties.end());
            }
        }
        const VCardVersion outVer = isOutVcard21 ? VCardVersion::VER_2_1 : VCardVersion::VER_3_0;
        std::function<void(VCard &)> fun = [&mandatoryProperties, &result, &outVer](VCard &vcard) {
            // 5.1.4.1 all Mandatory here means that the PSE shall always return the properties VERSION, N and TEL
            // vCard 2.1 or VERSION, N, FN and TEL for a vCard 3.0.
            AddMissProperties(vcard, mandatoryProperties);

            if (vcard.Properties().size() > 0) {
                std::vector<uint8_t> vcardBytes = VCardUtil::Build(vcard, outVer);
                result.result_.insert(result.result_.end(), vcardBytes.begin(), vcardBytes.end());
            }
        };
        SelectPhoneBookVcardList(
            dataAccess, vcardUids, GetPropertySelectorWhere(includeProperties), param.supportedFeatures, fun);
    }

    static void SetDbId(const std::unique_ptr<DataAccess> &dataAccess, PbapPseVcardManager::PhoneBookResult &result)
    {
        PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
        std::string select = "select db_id from vcard_db_id limit 1";

        auto stmt = dataAccess->CreateStatement(select);
        if (!stmt) {
            result.rspCode_ = ObexRspCode::INTERNAL_SERVER_ERROR;
            return;
        }

        auto dataResult = stmt->Query();
        if (dataResult->Next()) {
            std::string dbId = dataResult->GetString(0);
            if (dbId.size() > 0) {
                std::vector<uint8_t> tmpPv(dbId.c_str(), dbId.c_str() + dbId.size());
                result.databaseIdentifier_ = tmpPv;
                FitListToSize(result.databaseIdentifier_, 0x10);
            }
        }
    }

    static void SetFolderVersion(const std::unique_ptr<DataAccess> &dataAccess, const std::u16string &folderId,
        PbapPseVcardManager::PhoneBookResult &result)
    {
        PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
        std::string select =
            "select folder_id, primary_folder_version, secondary_folder_version from vcard_folder where folder_id = ?";

        auto stmt = dataAccess->CreateStatement(select);
        if (!stmt) {
            result.rspCode_ = ObexRspCode::INTERNAL_SERVER_ERROR;
            return;
        }
        stmt->SetParam16String(1, folderId);
        auto dataResult = stmt->Query();
        if (dataResult->Next()) {
            std::string pv = dataResult->GetString(0x01);
            if (pv.size() > 0) {
                std::vector<uint8_t> tmpPv(pv.c_str(), pv.c_str() + pv.size());
                result.primaryFolderVersion_ = tmpPv;
                FitListToSize(result.primaryFolderVersion_, 0x10);
            }
            std::string sv = dataResult->GetString(0x02);
            if (sv.size() > 0) {
                std::vector<uint8_t> tmpSv(sv.c_str(), sv.c_str() + sv.size());
                result.secondaryFolderVersion_ = tmpSv;
                FitListToSize(result.secondaryFolderVersion_, 0x10);
            }
        }
    }

    static void SetCommonResult(const std::unique_ptr<DataAccess> &dataAccess, const std::u16string &folderId,
        PbapPseVcardManager::PhoneBookResult &result, const uint32_t &supportedFeatures)
    {
        if (folderId == u"mch") {
            // 5.1.4.6 NewMissedCalls
            // This application parameter shall be used in the response when the phone book object is mch. It shall
            // indicate the number of undismissed missed calls on the PSE at the point of the request. This value is the
            // same for mch and cch objects.
            result.newMissedCalls_ = true;
            SetNewMissedCalls(dataAccess, folderId, result);
            if (result.rspCode_ != ObexRspCode::SUCCESS) {
                return;
            }
        }
        if (PbapPseVcardManager::IsSupportedDbVer(supportedFeatures)) {
            SetFolderVersion(dataAccess, folderId, result);
            if (result.rspCode_ != ObexRspCode::SUCCESS) {
                return;
            }
        }
        if (PbapPseVcardManager::IsSupportedDbVer(supportedFeatures)) {
            SetDbId(dataAccess, result);
            if (result.rspCode_ != ObexRspCode::SUCCESS) {
                return;
            }
        }
    }

    static void FitListToSize(std::vector<uint8_t> &list, size_t size)
    {
        if (list.size() == size) {
            return;
        }
        if (list.size() > size) {
            while (list.size() > size) {
                list.pop_back();
            }
        } else {
            size_t fitSize = size - list.size();
            list.insert(list.begin(), fitSize, 0x00);
        }
    }

private:
    static std::string GetSelectPullvCardListingSql(const PbapPseAppParams &pbapAppParams)
    {
        std::string select = "select distinct t.vcard_handle_id, t.property_value as name from vcard_view t ";
        std::string where = " where t.folder_id = ? and t.property_id ='N' ";
        where += GetVcardSelectorWhere(pbapAppParams);
        if (pbapAppParams.GetSearchValueUtf8().size() > 0) {
            where += GetVcardSearchPropertyWhere(pbapAppParams);
        }
        // If this application parameter is not specified, the default order is “Indexed”.
        std::string orderBy = " order by t.vcard_handle_id asc ";
        // 5.3.4.1 Order : Alphabetical, Indexed, Phonetical
        if (pbapAppParams.GetOrder()) {
            switch (*pbapAppParams.GetOrder()) {
                case 0x01:  // alphanumeric
                    orderBy = " order by t.property_value asc, t.vcard_handle_id asc";
                    break;
                case 0x02:  // phonetic
                    orderBy = " order by (select property_value from vcard_view where vcard_uid = t.vcard_uid and "
                              "property_id = "
                              "'SOUND' limit 1) is null asc ";
                    orderBy +=
                        ", (select property_value from vcard_view where vcard_uid = t.vcard_uid and property_id = "
                        "'SOUND' limit 1) asc ";
                    orderBy += ", t.vcard_handle_id asc ";
                    break;
                default:
                    // default indexed
                    break;
            }
        }
        select += where;
        select += orderBy;
        select += " limit ? offset ?";
        return select;
    }

    static std::vector<std::pair<int, std::string>> SelectPullvCardListing(
        const std::unique_ptr<DataAccess> &dataAccess, const std::u16string &folderId,
        const PbapPseAppParams &pbapAppParams)
    {
        PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
        std::string select = GetSelectPullvCardListingSql(pbapAppParams);
        // 5.1.4.3 MaxListCount  default max is 65535
        uint16_t maxListCount = 0xFFFF;
        if (pbapAppParams.GetMaxListCount()) {
            maxListCount = *pbapAppParams.GetMaxListCount();
        }
        // 5.1.4.4 ListStartOffset
        uint16_t listStartOffset = 0;  // The offset shall be 0 if this header is not specified
        if (pbapAppParams.GetListStartOffset()) {
            listStartOffset = *pbapAppParams.GetListStartOffset();
        }
        std::vector<std::pair<int, std::string>> vcardUids;
        auto stmtVcardIds = dataAccess->CreateStatement(select);
        if (!stmtVcardIds) {
            return vcardUids;
        }
        int paramIndex = 1;
        stmtVcardIds->SetParam16String(paramIndex++, folderId);
        if (pbapAppParams.GetSearchValueUtf8().size() > 0) {
            const uint8_t *data = pbapAppParams.GetSearchValueUtf8().data();
            std::string searchValue(data, data + pbapAppParams.GetSearchValueUtf8().size());
            stmtVcardIds->SetParamString(paramIndex++, searchValue);
        }
        stmtVcardIds->SetParamInt(paramIndex++, maxListCount);
        stmtVcardIds->SetParamInt(paramIndex++, listStartOffset);
        auto dataResult = stmtVcardIds->Query();
        while (dataResult->Next()) {
            int index = 0;
            int vcardUid = dataResult->GetInt(index++);
            std::string name = dataResult->GetString(index++);
            vcardUids.push_back(std::make_pair(vcardUid, name));
        }
        return vcardUids;
    }

    static std::vector<std::pair<int64_t, int>> SelectPhoneBookVcardUids(
        const std::unique_ptr<DataAccess> &dataAccess, const std::u16string &folderId,
        const PbapPseAppParams &pbapAppParams)
    {
        PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
        std::string select = "select distinct vcard_uid, vcard_version from vcard_view t where folder_id = ? ";
        select += GetVcardSelectorWhere(pbapAppParams);
        select += " order by vcard_handle_id asc";
        select += " limit ? offset ?";

        // 5.1.4.3 MaxListCount  default max is 65535
        uint16_t maxListCount = 0xFFFF;
        if (pbapAppParams.GetMaxListCount()) {
            maxListCount = *pbapAppParams.GetMaxListCount();
        }
        // 5.1.4.4 ListStartOffset
        uint16_t listStartOffset = 0;  // The offset shall be 0 if this header is not specified
        if (pbapAppParams.GetListStartOffset()) {
            listStartOffset = *pbapAppParams.GetListStartOffset();
        }
        std::vector<std::pair<int64_t, int>> vcardUids;
        auto stmtVcardIds = dataAccess->CreateStatement(select);
        if (!stmtVcardIds) {
            return vcardUids;
        }
        int pIndex = 1;
        stmtVcardIds->SetParam16String(pIndex++, folderId);
        stmtVcardIds->SetParamInt(pIndex++, maxListCount);
        stmtVcardIds->SetParamInt(pIndex++, listStartOffset);
        auto dataResult = stmtVcardIds->Query();
        while (dataResult->Next()) {
            int index = 0;
            int64_t vcardUid = dataResult->GetInt64(index++);
            int vcardVer = dataResult->GetInt(index++);
            vcardUids.push_back(std::make_pair(vcardUid, vcardVer));
        }
        return vcardUids;
    }

    static std::string GetSelectPhoneBookVcardListSql(const std::string &whereSql)
    {
        std::string sql = "select"
                          "    vcard_handle_id"
                          "  , vcard_version"
                          "  , property_group"
                          "  , property_id"
                          "  , property_details"
                          "  , property_value "
                          "from"
                          "  vcard_view t "
                          "where"
                          "  vcard_uid = ? " +
                          whereSql +
                          "order by"
                          "  profile_property_order is null asc"
                          "  , profile_property_order asc";
        return sql;
    }

    static bool IsNeedInclude(const std::string &propertyId, const uint32_t &supportedFeatures)
    {
        if (propertyId == "X-BT-UCI" && !PbapPseVcardManager::IsSupportedXBtUCI(supportedFeatures)) {
            return false;
        }
        if (propertyId == "X-BT-UID" && !PbapPseVcardManager::IsSupportedXBtUID(supportedFeatures)) {
            return false;
        }
        return true;
    }

    static bool SelectPhoneBookVcardList(const std::unique_ptr<DataAccess> &dataAccess,
        const std::vector<std::pair<int64_t, int>> &vcardUids, const std::string whereSql,
        const uint32_t &supportedFeatures, const std::function<void(VCard &)> &handleFun)
    {
        PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
        std::string select = GetSelectPhoneBookVcardListSql(whereSql);
        auto stmt = dataAccess->CreateStatement(select);
        if (!stmt) {
            PBAP_PSE_LOG_ERROR("Error in CreateStatement");
            return false;
        }
        for (size_t i = 0; i < vcardUids.size(); i++) {
            auto pair = vcardUids.at(i);
            int64_t vcardUid = pair.first;
            int vcardVer = pair.second;
            stmt->ClearParams();
            stmt->SetParamInt64(1, vcardUid);
            bool isCurVcard21 = (vcardVer == 0);
            auto dataResult = stmt->Query();
            VCard vcard;
            const VCardVersion curVer = isCurVcard21 ? VCardVersion::VER_2_1 : VCardVersion::VER_3_0;
            while (dataResult->Next()) {
                int index = 0;
                int vcardHandleId = dataResult->GetInt(index++);
                int vcardVersion = dataResult->GetInt(index++);
                std::string propertyGroup = dataResult->GetString(index++);
                std::string propertyId = dataResult->GetString(index++);
                if (!IsNeedInclude(propertyId, supportedFeatures)) {
                    continue;
                }
                std::string propertyDetails = dataResult->GetString(index++);
                std::string propertyValue = dataResult->GetString(index++);
                PBAP_PSE_LOG_DEBUG("[%{public}zu]: %jd\t%{public}d\t%{public}d\t%{public}s\t%{public}s\t%{public}s",
                    i,
                    vcardUid,
                    vcardHandleId,
                    vcardVersion,
                    propertyId.c_str(),
                    propertyDetails.c_str(),
                    propertyValue.c_str());
                vcard.AddProperty(VCardProperty(curVer, propertyGroup, propertyId, propertyValue, propertyDetails));
            }
            handleFun(vcard);
        }
        return true;
    }

    static std::vector<std::pair<int64_t, int>> SelectVcardEntryUid(const std::unique_ptr<DataAccess> &dataAccess,
        const std::u16string &folderId, std::u16string entryId, const PbapPseAppParams &pbapAppParams)
    {
        PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
        std::string select = "select distinct t.vcard_uid, t.vcard_version from vcard_view t where 1 = 1 ";
        std::string where = "";
        bool isXBtUid = entryId.find(PBAP_PSE_X_BT_UID_PREFIX) == 0;
        if (isXBtUid) {
            where += "and exists (";
            where += "select 1 from vcard_view s where s.vcard_uid = t.vcard_uid and "
                     "s.property_id = 'X-BT-UID' and s.property_value = ? ";
            where += ")";
        } else {
            where += "and t.folder_id = ? and t.vcard_handle_id = ? ";
        }
        where += GetVcardSelectorWhere(pbapAppParams);
        select += where;
        select += " limit 1";

        std::vector<std::pair<int64_t, int>> vcardUids;
        auto stmtVcardIds = dataAccess->CreateStatement(select);
        if (!stmtVcardIds) {
            return vcardUids;
        }
        int pIndex = 1;
        if (isXBtUid) {
            entryId = entryId.substr(PBAP_PSE_X_BT_UID_PREFIX.size());
            stmtVcardIds->SetParam16String(pIndex++, entryId);
        } else {
            entryId = entryId.substr(0, entryId.size() - PBAP_PSE_VCARD_SUFFIX.size());
            stmtVcardIds->SetParam16String(pIndex++, folderId);
            int handleId = PbapPseVcardManager::VcardHandleId2Int(entryId);
            stmtVcardIds->SetParamInt(pIndex++, handleId);
        }

        auto dataResult = stmtVcardIds->Query();
        while (dataResult->Next()) {
            int index = 0;
            int64_t vcardUid = dataResult->GetInt64(index++);
            int vcardVer = dataResult->GetInt(index++);
            vcardUids.push_back(std::make_pair(vcardUid, vcardVer));
        }
        return vcardUids;
    }

    static std::string GetPropertySelectorWhere(const std::vector<std::string> &includeProperties)
    {
        std::string inSection = "";
        if (includeProperties.size() > 0) {
            inSection += " and property_id in (";
            for (size_t i = 0; i < includeProperties.size(); i++) {
                if (i > 0) {
                    inSection += ",";
                }
                inSection += "'" + includeProperties.at(i) + "'";
            }
            inSection += ") ";
        }

        return inSection;
    }

    static std::string GetVcardSearchPropertyWhere(const PbapPseAppParams &pbapAppParams)
    {
        std::string andSection = "";
        // 5.3.4.3 SearchValue :<text string>
        if (pbapAppParams.GetSearchValueUtf8().size() > 0) {
            andSection += "and exists (";
            andSection += "select 1 from vcard_view s where s.vcard_uid = t.vcard_uid and "
                          "s.property_id = '";

            // 5.3.4.2 SearchProperty :Name, Number, Sound
            std::string searchPropertyId = "N";
            if (pbapAppParams.GetSearchProperty()) {
                switch (*pbapAppParams.GetSearchProperty()) {
                    case 0x01:  // Number
                        searchPropertyId = "TEL";
                        break;
                    case 0x02:  // Sound
                        searchPropertyId = "SOUND";
                        break;
                    default:
                        searchPropertyId = "N";  // default Name
                        break;
                }
            }
            andSection += searchPropertyId + "' and s.property_value like '%' || ? || '%'";
            andSection += ")";
        }
        return andSection;
    }

    static std::string GetVcardSelectorWhere(const PbapPseAppParams &pbapAppParams)
    {
        std::string andSection = "";
        // 5.1.4.7 vCardSelector
        if (!pbapAppParams.GetVcardSelector()) {
            return andSection;
        }
        uint64_t vcardSelector = *pbapAppParams.GetVcardSelector();
        // 5.1.4.8 vCardSelectorOperator
        // All vCards shall be returned if contains all zeros.
        if (vcardSelector == 0) {
            return andSection;
        }
        auto selectorProperties = PbapPseVcardManager::GetIncludeProperties(vcardSelector);
        if (selectorProperties.size() == 0) {
            return andSection;
        }
        std::string relations = "or";
        if (pbapAppParams.GetVcardSelectorOperator()) {
            if (*pbapAppParams.GetVcardSelectorOperator() == 0x01) {
                relations = "and";
            }
        }
        andSection += " and (";
        for (size_t i = 0; i < selectorProperties.size(); i++) {
            if (i > 0) {
                andSection += " " + relations + " ";
            }
            andSection += " exists (";
            andSection += "select 1 from vcard_view s where s.vcard_uid = t.vcard_uid and "
                          "s.property_id = '" +
                          selectorProperties.at(i) + "' and s.property_value != ''";
            andSection += ")";
        }
        andSection += ") ";
        return andSection;
    }

    static void AddMissProperties(VCard &vcard, const std::vector<std::string> &properties)
    {
        for (auto propertyId : properties) {
            if (!vcard.HasProperty(propertyId)) {
                vcard.AddProperty(VCardProperty(vcard.GetVersion(), "", propertyId, "", ""));
            }
        }
    }
};

const std::map<char16_t, uint8_t> PbapPseVcardManager::VCARD_HANDLE_CHAR_MAP = {
    {'0', 0},
    {'1', 1},
    {'2', 2},
    {'3', 3},
    {'4', 4},
    {'5', 5},
    {'6', 6},
    {'7', 7},
    {'8', 8},
    {'9', 9},
    {'A', 10},
    {'B', 11},
    {'C', 12},
    {'D', 13},
    {'E', 14},
    {'F', 15},
    {'a', 10},
    {'b', 11},
    {'c', 12},
    {'d', 13},
    {'e', 14},
    {'f', 15}
};

const std::map<std::u16string, std::u16string> PbapPseVcardManager::VIRTUAL_VCF_FOLDER_MAP = {
    {u"/telecom/pb.vcf", u"pb"},
    {u"/telecom/ich.vcf", u"ich"},
    {u"/telecom/och.vcf", u"och"},
    {u"/telecom/mch.vcf", u"mch"},
    {u"/telecom/cch.vcf", u"cch"},
    {u"/telecom/spd.vcf", u"spd"},
    {u"/telecom/fav.vcf", u"fav"},
    {u"/SIM1/telecom/pb.vcf", u"pb1"},
    {u"/SIM1/telecom/ich.vcf", u"ich1"},
    {u"/SIM1/telecom/och.vcf", u"och1"},
    {u"/SIM1/telecom/mch.vcf", u"mch1"},
    {u"/SIM1/telecom/cch.vcf", u"cch1"},
};

const std::map<uint64_t, std::string> PbapPseVcardManager::PROPERTY_MASK_MAP = {
    {uint64_t(1) << 0, "VERSION"},
    {uint64_t(1) << 1, "FN"},
    {uint64_t(1) << 2, "N"},
    {uint64_t(1) << 3, "PHOTO"},
    {uint64_t(1) << 4, "BDAY"},
    {uint64_t(1) << 5, "ADR"},
    {uint64_t(1) << 6, "LABEL"},
    {uint64_t(1) << 7, "TEL"},
    {uint64_t(1) << 8, "EMAIL"},
    {uint64_t(1) << 9, "MAILER"},
    {uint64_t(1) << 10, "TZ"},
    {uint64_t(1) << 11, "GEO"},
    {uint64_t(1) << 12, "TITLE"},
    {uint64_t(1) << 13, "ROLE"},
    {uint64_t(1) << 14, "LOGO"},
    {uint64_t(1) << 15, "AGENT"},
    {uint64_t(1) << 16, "ORG"},
    {uint64_t(1) << 17, "NOTE"},
    {uint64_t(1) << 18, "REV"},
    {uint64_t(1) << 19, "SOUND"},
    {uint64_t(1) << 20, "URL"},
    {uint64_t(1) << 21, "UID"},
    {uint64_t(1) << 22, "KEY"},
    {uint64_t(1) << 23, "NICKNAME"},
    {uint64_t(1) << 24, "CATEGORIES"},
    {uint64_t(1) << 25, "PROID"},
    {uint64_t(1) << 26, "CLASS"},
    {uint64_t(1) << 27, "SORT-STRING"},
    {uint64_t(1) << 28, "X-IRMC-CALL-DATETIME"},
    {uint64_t(1) << 29, "X-BT-SPEEDDIALKEY"},
    {uint64_t(1) << 30, "X-BT-UCI"},
    {uint64_t(1) << 31, "X-BT-UID"}
};

std::string PbapPseVcardManager::g_pbapDbFile = PBAP_PSE_DEFAULT_DB_FILE;

void PbapPseVcardManager::SetDbFile(const std::string dbFile)
{
    g_pbapDbFile = dbFile;
}

void PbapPseVcardManager::PullPhoneBook(std::u16string nameWithFolder, const PbapPseAppParams &pbapAppParams,
    const uint32_t &supportedFeatures, PhoneBookResult &result)
{
    PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
    if (nameWithFolder.at(0) != '/') {
        nameWithFolder = u"/" + nameWithFolder;
    }
    auto target = VIRTUAL_VCF_FOLDER_MAP.find(nameWithFolder);
    // not find folder
    if (target == VIRTUAL_VCF_FOLDER_MAP.end()) {
        PBAP_PSE_LOG_ERROR("can't find vcard %{public}s", ObexUtils::UnicodeToUtf8(nameWithFolder).c_str());
        result.rspCode_ = ObexRspCode::NOT_FOUND;
        return;
    }

    auto dataAccess = DataAccess::GetConnection(g_pbapDbFile);
    if (!dataAccess) {
        result.rspCode_ = ObexRspCode::NOT_FOUND;
        return;
    }

    const std::u16string &folderId = target->second;
    if (pbapAppParams.GetMaxListCount()) {
        uint16_t maxListCount = *pbapAppParams.GetMaxListCount();
        // PBAP_v1.2.3-5.1.4.3 MaxListCount
        result.phoneBookSizeOnly_ = maxListCount == 0;
    }
    if (result.phoneBookSizeOnly_) {
        PBAP_PSE_LOG_DEBUG("GetPhoneBookSizeOnly");
        PbapPseVcardDataAccess::SetPhoneBookSize(dataAccess, folderId, result, pbapAppParams);
        if (result.rspCode_ != ObexRspCode::SUCCESS) {
            return;
        }
    }
    PbapPseVcardDataAccess::SetCommonResult(dataAccess, folderId, result, supportedFeatures);
    if (result.rspCode_ != ObexRspCode::SUCCESS) {
        return;
    }
    if (!result.phoneBookSizeOnly_) {
        PBAP_PSE_LOG_DEBUG("GetPhoneBook");
        PbapPseVcardDataAccess::SetPhoneBook(dataAccess, folderId, result, pbapAppParams, supportedFeatures);
    }
}

void PbapPseVcardManager::PullvCardListing(std::u16string currentPath, std::u16string folderName,
    const PbapPseAppParams &pbapAppParams, const uint32_t &supportedFeatures, PhoneBookResult &result)
{
    PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
    std::u16string nameWithFolder = currentPath;
    if (folderName.size() > 0) {
        nameWithFolder += u"/" + folderName;
    }
    nameWithFolder += PBAP_PSE_VCARD_SUFFIX;
    auto target = VIRTUAL_VCF_FOLDER_MAP.find(nameWithFolder);
    // not find folder
    if (target == VIRTUAL_VCF_FOLDER_MAP.end()) {
        PBAP_PSE_LOG_ERROR("can't find folder [%{public}s] in [%{public}s]",
            ObexUtils::UnicodeToUtf8(folderName).c_str(),
            ObexUtils::UnicodeToUtf8(currentPath).c_str());
        result.rspCode_ = ObexRspCode::NOT_FOUND;
        return;
    }
    auto dataAccess = DataAccess::GetConnection(g_pbapDbFile);
    if (!dataAccess) {
        result.rspCode_ = ObexRspCode::NOT_FOUND;
        return;
    }
    const std::u16string &folderId = target->second;

    if (pbapAppParams.GetMaxListCount()) {
        uint16_t maxListCount = *pbapAppParams.GetMaxListCount();
        // PBAP_v1.2.3-5.1.4.3 MaxListCount
        result.phoneBookSizeOnly_ = maxListCount == 0;
    }
    if (result.phoneBookSizeOnly_) {
        PbapPseVcardDataAccess::SetPullvCardListingSize(dataAccess, folderId, result, pbapAppParams);
        if (result.rspCode_ != ObexRspCode::SUCCESS) {
            return;
        }
    }

    PbapPseVcardDataAccess::SetCommonResult(dataAccess, folderId, result, supportedFeatures);

    if (result.rspCode_ != ObexRspCode::SUCCESS) {
        return;
    }

    if (!result.phoneBookSizeOnly_) {
        PbapPseVcardDataAccess::SetPullvCardListing(dataAccess, folderId, result, pbapAppParams);
    }
}

void PbapPseVcardManager::PullvCardEntry(std::u16string currentPath, std::u16string entryId,
    const PbapPseAppParams &pbapAppParams, const uint32_t &supportedFeatures, PhoneBookResult &result)
{
    std::u16string folderId = u"";
    if (entryId.find(PBAP_PSE_X_BT_UID_PREFIX) == std::string::npos) {
        // not find folder
        std::u16string nameWithFolder = currentPath + PBAP_PSE_VCARD_SUFFIX;
        auto target = VIRTUAL_VCF_FOLDER_MAP.find(nameWithFolder);
        if (target == VIRTUAL_VCF_FOLDER_MAP.end()) {
            PBAP_PSE_LOG_ERROR("can't find folder [%{public}s] ", ObexUtils::UnicodeToUtf8(currentPath).c_str());
            result.rspCode_ = ObexRspCode::NOT_FOUND;
            return;
        }
        folderId = target->second;
    }
    auto dataAccess = DataAccess::GetConnection(g_pbapDbFile);
    if (!dataAccess) {
        result.rspCode_ = ObexRspCode::NOT_FOUND;
        return;
    }
    if (IsSupportedDbVer(supportedFeatures)) {
        PbapPseVcardDataAccess::SetDbId(dataAccess, result);
        if (result.rspCode_ != ObexRspCode::SUCCESS) {
            return;
        }
    }

    PbapPseVcardDataAccess::SetPullvCardEntryParam param = {
        folderId, entryId, supportedFeatures
    };
    PbapPseVcardDataAccess::SetPullvCardEntry(dataAccess, param, pbapAppParams, result);
}

bool PbapPseVcardManager::CheckPhoneBookPath(std::u16string path)
{
    if (path.size() == 0) {
        return true;
    }
    if (path.at(0) != '/') {
        path = u"/" + path;
    }
    if (path.back() != '/') {
        path += u"/";
    }
    for (auto &item : VIRTUAL_VCF_FOLDER_MAP) {
        std::u16string fullPath = item.first;
        fullPath = fullPath.substr(0, fullPath.size() - PBAP_PSE_VCARD_SUFFIX.size()) + u"/";
        if (fullPath.find(path) == 0) {
            return true;
        }
    }
    return false;
}

bool PbapPseVcardManager::CheckVcardHandleId(const std::u16string &handleId)
{
    if (handleId.size() == 0) {
        return false;
    }
    for (char16_t c : handleId) {
        if (VCARD_HANDLE_CHAR_MAP.find(c) == VCARD_HANDLE_CHAR_MAP.end()) {
            return false;
        }
    }
    return true;
}

int PbapPseVcardManager::VcardHandleId2Int(const std::u16string &handleId)
{
    size_t size = handleId.size();
    if (size == 0) {
        return -1;
    }
    int dec = 0;
    for (size_t index = 0; index < size; index++) {
        char16_t c = handleId.at(index);
        if (VCARD_HANDLE_CHAR_MAP.find(c) == VCARD_HANDLE_CHAR_MAP.end()) {
            return -1;
        }
        uint8_t num = VCARD_HANDLE_CHAR_MAP.find(c)->second;
        if (num == 0) {
            continue;
        }
        int m = 1;
        for (size_t count = 0; count < size - index - 1; count++) {
            m *= 0x10;
        }
        dec += (num * m);
    }
    return dec;
}

std::vector<std::string> PbapPseVcardManager::GetIncludeProperties(uint64_t propertySelector)
{
    std::vector<std::string> list;
    for (auto item : PROPERTY_MASK_MAP) {
        if ((propertySelector & item.first) != 0) {
            list.push_back(item.second);
        }
    }
    return list;
}

bool PbapPseVcardManager::IsSupportedFolderVer(const uint32_t &supportedFeatures)
{
    return (PBAP_FEATURES_FOLDER_VERSION_COUNTERS & supportedFeatures) != 0;
}

bool PbapPseVcardManager::IsSupportedDbVer(const uint32_t &supportedFeatures)
{
    return (PBAP_FEATURES_DATABASE_IDENTIFIER & supportedFeatures) != 0;
}

bool PbapPseVcardManager::IsSupportedXBtUCI(const uint32_t &supportedFeatures)
{
    return (PBAP_FEATURES_X_BT_UCI_VCARD_PROPERTY & supportedFeatures) != 0;
}

bool PbapPseVcardManager::IsSupportedXBtUID(const uint32_t &supportedFeatures)
{
    return (PBAP_FEATURES_X_BT_UID_VCARD_PROPERTY & supportedFeatures) != 0;
}
}  // namespace bluetooth
}  // namespace OHOS
