/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "locale_matcher.h"

#include <cstring>
#include <new>
#include "likely_subtags_key_data.cpp"
#include "likely_subtags_value_data.cpp"

#include "utils/common.h"
#include "utils/locale_data.h"
#include "utils/utils.h"

namespace OHOS {
namespace Global {
namespace Resource {
// Special locale constant
uint64_t LocaleMatcher::EN_US_ENCODE = Utils::EncodeLocale("en", nullptr, "US");
uint64_t LocaleMatcher::EN_GB_ENCODE = Utils::EncodeLocale("en", nullptr, "GB");
uint64_t LocaleMatcher::EN_QAAG_ENCODE = Utils::EncodeLocale("en", "Qaag", nullptr);
uint64_t LocaleMatcher::ZH_HANT_MO_ENCODE = Utils::EncodeLocale("zh", "Hant", "MO");
uint64_t LocaleMatcher::ZH_HK_ENCODE = Utils::EncodeLocale("zh", nullptr, "HK");
uint32_t LocaleMatcher::HANT_ENCODE = Utils::EncodeScript("Hant");

bool IsContainRegion(uint64_t encodedLocale)
{
    return (encodedLocale & 0x000000000000FFFFLU) != 0;
}

uint64_t ClearRegion(uint64_t encodedLocale)
{
    return encodedLocale & 0xFFFFFFFFFFFF0000LU;
}

uint64_t AddScript(uint64_t encodedLocale, uint32_t encodedScript)
{
    return  (encodedLocale | ((static_cast<uint64_t>(encodedScript) & 0x00000000FFFFFFFFLU) << 16));
}

/**
 * @brief Find region parent locale, if locale has not contain region, return ROOT_LOCALE.
 * Lookup child-parent locale tables, if tables has not contains, then clear the locale region and return it.
 *
 * @param encodedLocale locale encode
 * @return uint64_t  parent locale encode
 */
uint64_t SearchParentLocale(uint64_t encodedLocale, const ResLocale *request)
{
    uint64_t tempEncodedLocale = encodedLocale;
    if (Utils::EncodeScriptByResLocale(request) == LocaleMatcher::HANT_ENCODE) {
        tempEncodedLocale = AddScript(encodedLocale, LocaleMatcher::HANT_ENCODE);
        if (tempEncodedLocale == LocaleMatcher::ZH_HANT_MO_ENCODE) {
            return LocaleMatcher::ZH_HK_ENCODE;
        }
    }
    if (IsContainRegion(encodedLocale)) {
        size_t len = sizeof(LOCALE_PARENTS_KEY) / sizeof(LOCALE_PARENTS_KEY[0]);
        for (size_t i = 0; i < len; i++) {
            if (LOCALE_PARENTS_KEY[i] == tempEncodedLocale) {
                return LOCALE_PARENTS_VALUE[i];
            }
        }
        return ClearRegion(encodedLocale);
    }
    return LocaleMatcher::ROOT_LOCALE;
}

/**
 * @brief find locale ancestors, including itself and ROOT_LOCALE.
 *
 * @param result
 * @param len
 * @param encodedLocale
 */
void FindTrackPath(const ResLocale *request, size_t len, uint64_t encodedLocale, uint64_t *result)
{
    uint64_t currentEncodedLocale = encodedLocale;
    size_t i = 0;
    do {
        result[i] = currentEncodedLocale;
        currentEncodedLocale = SearchParentLocale(currentEncodedLocale, request);
        ++i;
    } while (currentEncodedLocale != LocaleMatcher::ROOT_LOCALE);
    if (i < len) {
        result[i] = LocaleMatcher::ROOT_LOCALE;
    }
}

/**
 * @brief find encodedLocale pos is in trackpath list.
 *
 * @param paths
 * @param len
 * @param encodedLocale
 * @return int8_t
 */
int8_t SearchTrackPathDistance(const uint64_t *paths, size_t len, uint64_t encodedLocale)
{
    size_t i = 0;
    for (i = 0; i < len; ++i) {
        if (paths[i] == LocaleMatcher::ROOT_LOCALE) {
            return i;
        }
        if (paths[i] == encodedLocale) {
            return i;
        }
    }
    return static_cast<int8_t>(i);
}

/**
 * @brief find locale is in typical locale.
 *
 * @param language
 * @param script
 * @param region
 * @return true
 * @return false
 */
bool IsDefaultLocale(const char *language, const char *script, const char *region)
{
    uint64_t encodedLocale = Utils::EncodeLocale(language, script, region);
    if (ClearRegion(encodedLocale) == LocaleMatcher::EN_QAAG_ENCODE) {
        encodedLocale = Utils::EncodeLocale("en", "Latn", region);
    }
    size_t len = sizeof(TYPICAL_CODES_VALUE) / sizeof(TYPICAL_CODES_VALUE[0]);
    for (size_t i = 0; i < len; i++) {
        if (TYPICAL_CODES_VALUE[i] == encodedLocale) {
            return true;
        }
    }
    return false;
}

/**
 * @brief find the default script of language and region.
 * first search language and region corresponding  script.
 * if not found,search language corresponding script.
 *
 * @param language
 * @param region
 * @return uint32_t
 */
uint32_t FindDefaultScriptEncode(const char *language, const char *region)
{
    uint64_t encodedLocale = Utils::EncodeLocale(language, nullptr, region);
    size_t len = sizeof(LIKELY_TAGS_CODES_KEY) / sizeof(LIKELY_TAGS_CODES_KEY[0]);
    for (size_t i = 0; i < len; i++) {
        if (LIKELY_TAGS_CODES_KEY[i] == encodedLocale) {
            return static_cast<uint32_t>((LIKELY_TAGS_CODES_VALUE[i] & 0x0000ffffffff0000) >> 16);
        }
    }
    if (region != nullptr) {
        encodedLocale = Utils::EncodeLocale(language, nullptr, nullptr);
        for (size_t i = 0; i < len; i++) {
            if (LIKELY_TAGS_CODES_KEY[i] == encodedLocale) {
                return static_cast<uint32_t>((LIKELY_TAGS_CODES_VALUE[i] & 0x0000ffffffff0000) >> 16);
            }
        }
    }
    return LocaleMatcher::NULL_SCRIPT;
}

/**
 * @brief find the default region of language and script.
 * first search language and srcipt corresponding  region.
 * if not found,search language corresponding region.
 *
 * @param language
 * @param script
 * @return uint16_t
 */
uint16_t FindDefaultRegionEncode(const char *language, const char *script)
{
    /* first try language and script */
    uint64_t encodedLocale = Utils::EncodeLocale(language, script, nullptr);
    if (encodedLocale == Utils::EncodeLocale("en", "Qaag", nullptr)) {
        encodedLocale = Utils::EncodeLocale("en", "Latn", nullptr);
    }
    size_t len = sizeof(LIKELY_TAGS_CODES_KEY) / sizeof(LIKELY_TAGS_CODES_KEY[0]);
    for (size_t i = 0; i < len; i++) {
        if (LIKELY_TAGS_CODES_KEY[i] == encodedLocale) {
            return static_cast<uint16_t>((LIKELY_TAGS_CODES_VALUE[i] & 0x000000000000ffff));
        }
    }
    /* if not found and script is not null,try language */
    if (script != nullptr) {
        encodedLocale = Utils::EncodeLocale(language, nullptr, nullptr);
        for (size_t i = 0; i < len; i++) {
            if (LIKELY_TAGS_CODES_KEY[i] == encodedLocale) {
                return static_cast<uint16_t>((LIKELY_TAGS_CODES_VALUE[i] & 0x000000000000ffff));
            }
        }
    }
    return LocaleMatcher::NULL_REGION;
};

/**
 * @brief find the  first locale which in target path,is also in request path.
 * return sum of the locale pos in target path and request path.
 *
 * @param requestPaths
 * @param targetPaths
 * @param len
 * @return size_t
 */
size_t ComputeTrackPathDistance(const uint64_t *requestPaths,
                                const uint64_t *targetPaths, size_t len)
{
    size_t i = 0;
    size_t j = 0;
    for (i = 0; i < len; ++i) {
        if (targetPaths[i] == LocaleMatcher::ROOT_LOCALE) {
            // targetpath not in request path,so distance is 2*len
            return len * 2;
        }
        for (j = 0; j < len; ++j) {
            if (requestPaths[j] == targetPaths[i]) {
                return i + j;
            }
        }
    }
    return len * 2;
}

int8_t CompareRegionWhenQaag(const ResLocale *current,
    const ResLocale *other,
    const ResLocale *request)
{
    if ((request != nullptr) && (Utils::EncodeLocale(request->GetLanguage(), request->GetScript(),
        nullptr) == LocaleMatcher::EN_QAAG_ENCODE)) {
        if ((current != nullptr) && (Utils::EncodeLocale(current->GetLanguage(), nullptr,
            current->GetRegion()) == LocaleMatcher::EN_GB_ENCODE)) {
            return 1;
        }
        if ((other != nullptr) && (Utils::EncodeLocale(other->GetLanguage(), nullptr, other->GetRegion()) ==
            LocaleMatcher::EN_GB_ENCODE)) {
            return -1;
        }
    }
    return 0;
}

/**
 * @brief compare language,support new/old language code
 * NEW_LANGUAGES_CODES is new language code,
 * OLD_LANGUAGES_CODES is old language code.
 * suppor iw/he,tl/fil,ji/yi,jw/jv,in/id.
 * @param current
 * @param other
 * @return true
 * @return false
 */
bool CompareLanguage(const ResLocale *current, const ResLocale *other)
{
    uint16_t currentEncodedLanguage =
        Utils::EncodeLanguageByResLocale(current);
    uint16_t otherEncodedLanguage = Utils::EncodeLanguageByResLocale(
        other);
    // 0-4 NEW/OLD language code means iw/he,tl/fil,ji/yi,jw/jv,in/id
    return ((currentEncodedLanguage == otherEncodedLanguage) ||
        ((currentEncodedLanguage == NEW_LANGUAGES_CODES[0])
            && (otherEncodedLanguage == OLD_LANGUAGES_CODES[0])) ||
        ((otherEncodedLanguage == NEW_LANGUAGES_CODES[0])
            && (currentEncodedLanguage == OLD_LANGUAGES_CODES[0])) ||
        ((currentEncodedLanguage == NEW_LANGUAGES_CODES[1])
            && (otherEncodedLanguage == OLD_LANGUAGES_CODES[1])) ||
        ((otherEncodedLanguage == NEW_LANGUAGES_CODES[1])
            && (currentEncodedLanguage == OLD_LANGUAGES_CODES[1])) ||
        ((currentEncodedLanguage == NEW_LANGUAGES_CODES[2])
            && (otherEncodedLanguage == OLD_LANGUAGES_CODES[2])) ||
        ((otherEncodedLanguage == NEW_LANGUAGES_CODES[2])
            && (currentEncodedLanguage == OLD_LANGUAGES_CODES[2])) ||
        ((currentEncodedLanguage == NEW_LANGUAGES_CODES[3])
            && (otherEncodedLanguage == OLD_LANGUAGES_CODES[3])) ||
        ((otherEncodedLanguage == NEW_LANGUAGES_CODES[3])
            && (currentEncodedLanguage == OLD_LANGUAGES_CODES[3])) ||
        ((currentEncodedLanguage == NEW_LANGUAGES_CODES[4])
            && (otherEncodedLanguage == OLD_LANGUAGES_CODES[4])) ||
        ((otherEncodedLanguage == NEW_LANGUAGES_CODES[4])
            && (currentEncodedLanguage == OLD_LANGUAGES_CODES[4])));
}

bool CompareScript(const ResLocale *current, const ResLocale *other)
{
    uint32_t currentEncodedScript = 0;
    uint32_t otherEncodedScript = 0;
    if ((current != nullptr) && (current->GetScript() == nullptr)) {
        currentEncodedScript = FindDefaultScriptEncode(current->GetLanguage(),
            current->GetRegion());
    } else {
        currentEncodedScript = Utils::EncodeScriptByResLocale(current);
    }
    if ((other != nullptr) && (other->GetScript() == nullptr)) {
        otherEncodedScript = FindDefaultScriptEncode(other->GetLanguage(),
            other->GetRegion());
    } else {
        otherEncodedScript = Utils::EncodeScriptByResLocale(other);
    }
    if (current != nullptr && other != nullptr) {
        // when current locale is en-Qaag is equal en-Latn
        if (Utils::EncodeLocale(current->GetLanguage(), current->GetScript(), nullptr) ==
            Utils::EncodeLocale("en", "Qaag", nullptr)) {
            if (Utils::EncodeLocale(other->GetLanguage(), other->GetScript(), nullptr) ==
                Utils::EncodeLocale("en", "Latn", nullptr)) {
                return true;
            }
        }
    }
    bool compareRegion = false;
    if ((currentEncodedScript == LocaleMatcher::NULL_SCRIPT) || (otherEncodedScript == LocaleMatcher::NULL_SCRIPT)) {
        // if request script is null, region must be same
        compareRegion = true;
    }
    if (compareRegion) {
        uint16_t currentRegionEncode = Utils::EncodeRegionByResLocale(current);
        uint16_t otherRegionEncode = Utils::EncodeRegionByResLocale(other);
        return (otherRegionEncode == LocaleMatcher::NULL_REGION) || (currentRegionEncode == otherRegionEncode);
    }
    return currentEncodedScript == otherEncodedScript;
}

int8_t AlphabeticallyCompare(const ResLocale *current,
    uint64_t currentEncodedLocale,
    const ResLocale *other,
    uint64_t otherEncodedLocale)
{
    if (currentEncodedLocale == otherEncodedLocale) {
        return 0;
    }
    if (current == nullptr || current->GetRegion() == nullptr) {
        return -1;
    }
    if (other == nullptr || other->GetRegion() == nullptr) {
        return 1;
    }
    // be here region is not null
    char currentFirstChar = (current->GetRegion())[0];
    char otherFirstChar = (other->GetRegion())[0];
    if (currentFirstChar >= '0' && currentFirstChar <= '9') {
        if (otherFirstChar < '0' || otherFirstChar > '9') {
            return -1;
        }
    } else {
        if (otherFirstChar >= '0' && otherFirstChar <= '9') {
            return 1;
        }
    }
    if (currentEncodedLocale > otherEncodedLocale) {
        return -1;
    }
    if (otherEncodedLocale > currentEncodedLocale) {
        return 1;
    }
    return 0;
}

int8_t CompareWhenRegionIsNull(uint16_t currentEncodedRegion, uint16_t otherEncodedRegion,
    const ResLocale *current,
    const ResLocale *other,
    const ResLocale *request)
{
    if (current == nullptr || current->GetRegion() == nullptr) {
        return 1;
    }
    if (other == nullptr || other->GetRegion() == nullptr) {
        return -1;
    }
    int8_t qaagResult = CompareRegionWhenQaag(current, other, request);
    if (qaagResult != 0) {
        return qaagResult;
    }
    // get request default region
    uint16_t requestDefaultRegion =
        FindDefaultRegionEncode((request == nullptr) ? nullptr : request->GetLanguage(),
            (request == nullptr) ? nullptr : request->GetScript());
    if (requestDefaultRegion == currentEncodedRegion) {
        return 1;
    }
    if (requestDefaultRegion == otherEncodedRegion) {
        return -1;
    }
    // current and other region is not null.alphabetically
    uint64_t currentEncodedLocale = Utils::EncodeLocale((request == nullptr) ? nullptr : request->GetLanguage(),
        nullptr, (current == nullptr) ? nullptr : current->GetRegion());
    uint64_t otherEncodedLocale = Utils::EncodeLocale(
        (request == nullptr) ? nullptr : request->GetLanguage(), nullptr, other->GetRegion());
    return AlphabeticallyCompare(current, currentEncodedLocale, other, otherEncodedLocale);
}

int8_t CompareDistance(uint64_t currentEncodedLocale, uint64_t otherEncodedLocale,
    const uint64_t *requestEncodedTrackPath, const ResLocale *request)
{
    uint64_t currentEncodedTrackPath[LocaleMatcher::TRACKPATH_ARRAY_SIZE] = {0, 0, 0, 0, 0};
    FindTrackPath(request, LocaleMatcher::TRACKPATH_ARRAY_SIZE, currentEncodedLocale, currentEncodedTrackPath);
    uint64_t otherEncodedTrackPath[LocaleMatcher::TRACKPATH_ARRAY_SIZE] = {0, 0, 0, 0, 0};
    FindTrackPath(request, LocaleMatcher::TRACKPATH_ARRAY_SIZE, otherEncodedLocale, otherEncodedTrackPath);
    const size_t currentDistance = ComputeTrackPathDistance(
        requestEncodedTrackPath, currentEncodedTrackPath, LocaleMatcher::TRACKPATH_ARRAY_SIZE);
    const size_t targetDistance = ComputeTrackPathDistance(
        requestEncodedTrackPath, otherEncodedTrackPath, LocaleMatcher::TRACKPATH_ARRAY_SIZE);
    if (currentDistance < targetDistance) {
        return 1;
    }
    if (currentDistance > targetDistance) {
        return -1;
    }
    return 0;
}

int8_t CompareDefaultRegion(const ResLocale *current,
    const ResLocale *other,
    const ResLocale *request)
{
    int8_t qaagResult = CompareRegionWhenQaag(current, other, request);
    if (qaagResult != 0) {
        return qaagResult;
    } else {
        bool isCurrentDefaultRegion = IsDefaultLocale((request == nullptr) ? nullptr : request->GetLanguage(),
            (request == nullptr) ? nullptr : request->GetScript(),
            (current == nullptr) ? nullptr : current->GetRegion());
        bool isOtherDefaultRegion = IsDefaultLocale((request == nullptr) ? nullptr : request->GetLanguage(),
            (request == nullptr) ? nullptr : request->GetScript(), (other == nullptr) ? nullptr : other->GetRegion());
        if (isCurrentDefaultRegion != isOtherDefaultRegion) {
            if (isCurrentDefaultRegion) {
                return 1;
            } else {
                return -1;
            }
        }
    }
    return 0;
}

/**
 * @brief compare current and target region, which is better for request.
 * @param current current locale
 * @param target target locale
 * @param request request locale
 * @return int8_t if current region is better than target region,return 1. if current region is equal target region,
 *         return 0. If target region is better than current region, return -1.
 */
int8_t CompareRegion(const ResLocale *current,
                     const ResLocale *other,
                     const ResLocale *request)
{
    uint16_t currentEncodedRegion = Utils::EncodeRegionByResLocale(current);
    uint16_t otherEncodedRegion = Utils::EncodeRegionByResLocale(other);
    if (request == nullptr || request->GetRegion() == nullptr) {
        return CompareWhenRegionIsNull(currentEncodedRegion, otherEncodedRegion, current, other, request);
    }
    uint64_t requestEncodedLocale = Utils::EncodeLocale(
        request->GetLanguage(), nullptr, request->GetRegion());
    uint64_t requestEncodedTrackPath[LocaleMatcher::TRACKPATH_ARRAY_SIZE] = {0, 0, 0, 0, 0};
    FindTrackPath(request, LocaleMatcher::TRACKPATH_ARRAY_SIZE, requestEncodedLocale, requestEncodedTrackPath);
    uint64_t currentEncodedLocale = Utils::EncodeLocale(
        request->GetLanguage(), nullptr, (current == nullptr) ? nullptr : current->GetRegion());
    uint64_t otherEncodedLocale = Utils::EncodeLocale(
        request->GetLanguage(), nullptr, (other == nullptr) ? nullptr : other->GetRegion());
    int8_t currentMatchDistance = SearchTrackPathDistance(
        requestEncodedTrackPath,
        LocaleMatcher::TRACKPATH_ARRAY_SIZE,
        currentEncodedLocale);
    int8_t otherMatchDistance = SearchTrackPathDistance(
        requestEncodedTrackPath,
        LocaleMatcher::TRACKPATH_ARRAY_SIZE,
        otherEncodedLocale);
    if (currentMatchDistance < otherMatchDistance) {
        return 1;
    }
    if (currentMatchDistance > otherMatchDistance) {
        return -1;
    }
    int8_t result = CompareDistance(currentEncodedLocale, otherEncodedLocale, requestEncodedTrackPath, request);
    if (result != 0) {
        return result;
    }
    result = CompareDefaultRegion(current, other, request);
    if (result != 0) {
        return result;
    }
    uint16_t requestDefaultRegion =
        FindDefaultRegionEncode(request->GetLanguage(), request->GetScript());
    if (requestDefaultRegion == currentEncodedRegion) {
        return 1;
    }
    if (requestDefaultRegion == otherEncodedRegion) {
        return -1;
    }
    return AlphabeticallyCompare(current, currentEncodedLocale, other, otherEncodedLocale);
}

bool LocaleMatcher::Match(const ResLocale *current, const ResLocale *other)
{
    if (current == nullptr || other == nullptr) {
        return true;
    }
    // language is not null.
    bool isLanguageEqual = CompareLanguage(current, other);
    if (!isLanguageEqual) {
        return false;
    }
    return CompareScript(current, other);
};

bool LocaleMatcher::Normalize(ResLocale *localeInfo)
{
    if (localeInfo == nullptr) {
        return true;
    }
    if (!Utils::IsStrEmpty(localeInfo->GetScript())) {
        return true;
    }
    uint32_t encodedScript = FindDefaultScriptEncode(localeInfo->GetLanguage(),
                                                     localeInfo->GetRegion());
    if (encodedScript == LocaleMatcher::NULL_SCRIPT) {
        return true;
    }
    char *tempScript = new(std::nothrow) char[SCRIPT_ARRAY_LEN];
    if (tempScript == nullptr) {
        return false;
    }
    tempScript[SCRIPT_LEN] = '\0';
    Utils::DecodeScript(encodedScript, tempScript);
    localeInfo->script_ = tempScript;
    return true;
}

int8_t CompareLanguageIgnoreOldNewCode(const ResLocale *current, const ResLocale *other, const ResLocale *request)
{
    uint16_t currentLanguageEncode = Utils::EncodeLanguageByResLocale(current);
    uint16_t otherLanguageEncode = Utils::EncodeLanguageByResLocale(other);
    uint16_t requestLanguageEncode = Utils::EncodeLanguageByResLocale(request);
    if ((currentLanguageEncode == requestLanguageEncode) && (otherLanguageEncode != requestLanguageEncode)) {
        return 1;
    }
    if ((otherLanguageEncode == requestLanguageEncode) && (currentLanguageEncode != requestLanguageEncode)) {
        return -1;
    }
    return 0;
}

bool IsSimilarToUsEnglish(const ResLocale *localeInfo)
{
    uint64_t localeEncode = Utils::EncodeLocale("en", nullptr,
        (localeInfo == nullptr) ? nullptr : localeInfo->GetRegion());
    uint64_t loclaeEncodedTrackPath[LocaleMatcher::TRACKPATH_ARRAY_SIZE] = {0, 0, 0, 0, 0};
    FindTrackPath(nullptr, LocaleMatcher::TRACKPATH_ARRAY_SIZE, localeEncode, loclaeEncodedTrackPath);
    uint8_t len = LocaleMatcher::TRACKPATH_ARRAY_SIZE;
    for (uint8_t i = 0; i < len; ++i) {
        if (loclaeEncodedTrackPath[i] == Utils::EncodeLocale("en", nullptr, nullptr)) {
            return true;
        }
        if (loclaeEncodedTrackPath[i] == Utils::EncodeLocale("en", nullptr, "001")) {
            return false;
        }
    }
    return   false;
}
bool CompareRegionWhenLangIsNotEqual(const ResLocale *current,
    const ResLocale *other,
    const ResLocale *request)
{
    int8_t qaagResult = CompareRegionWhenQaag(current, other, request);
    if (qaagResult != 0) {
        return qaagResult;
    }
    if (request != nullptr && (Utils::EncodeLanguage(request->GetLanguage())) == Utils::EncodeLanguage("en")) {
        // when request is en-us,empty region is better
        if ((Utils::EncodeRegion(request->GetRegion())) == Utils::EncodeRegion("US")) {
            if (current != nullptr) {
                return (current->GetRegion() == nullptr) ||
                            ((Utils::EncodeRegion(current->GetRegion())) == Utils::EncodeRegion("US"));
            } else {
                return !(other->GetRegion() == nullptr ||
                            ((Utils::EncodeRegion(other->GetRegion())) == Utils::EncodeRegion("US")));
            }
        } else if (IsSimilarToUsEnglish(request)) {
            if (current != nullptr) {
                return IsSimilarToUsEnglish(current);
            } else {
                return !IsSimilarToUsEnglish(other);
            }
        }
    }
    return current != nullptr;
}

int8_t LocaleMatcher::IsMoreSuitable(const ResLocale *current,
    const ResLocale *other,
    const ResLocale *request)
{
    if (request == nullptr) {
        return 0;
    }
    if (current == nullptr && other == nullptr) {
        return 0;
    }
    bool isLangEqual = CompareLanguage(current, other);
    if (!isLangEqual) {
        // current or other language is null, not null language is better
        bool result = CompareRegionWhenLangIsNotEqual(current, other, request);
        return result ? 1 : -1;
    }
    uint16_t currentEncodedRegion =
        Utils::EncodeRegionByResLocale(current);
    uint16_t otherEncodedRegion =
        Utils::EncodeRegionByResLocale(other);
    if (currentEncodedRegion == otherEncodedRegion) {
        // same language,same script,same region
        return CompareLanguageIgnoreOldNewCode(current, other, request);
    }
    // equal request region is better
    uint16_t requestEncodedRegion = Utils::EncodeRegionByResLocale(request);
    if (currentEncodedRegion == requestEncodedRegion) {
        return 1;
    }
    if (otherEncodedRegion == requestEncodedRegion) {
        return -1;
    }
    int8_t isRegionEqual = CompareRegion(current, other, request);
    if (isRegionEqual == 0) {
        return CompareLanguageIgnoreOldNewCode(current, other, request);
    }
    return isRegionEqual;
};

/**
 * @brief language tag is 2 or 3 letters
 *
 * @param str
 * @param len
 * @return true
 * @return false
 */
bool LocaleMatcher::IsLanguageTag(const char *str, int32_t len)
{
    if (len < 0) {
        len = strlen(str);
    }
    if (len >= 2 && len <= 3 && Utils::IsAlphaString(str, len)) {
        return true;
    }
    return false;
}

/**
 * @brief script is 4 letters
 *
 * @param str
 * @param len
 * @return true
 * @return false
 */
bool LocaleMatcher::IsScriptTag(const char *str, int32_t len)
{
    if (len < 0) {
        len = strlen(str);
    }
    if (len == 4 && Utils::IsAlphaString(str, len)) {
        return true;
    }
    return false;
}

/**
 * @brief region is 2 letters or 3 digits.
 *
 * @param str
 * @param len
 * @return true
 * @return false
 */
bool LocaleMatcher::IsRegionTag(const char *str, int32_t len)
{
    if (len < 0) {
        len = strlen(str);
    }
    // region is 2 letters if is alpha string
    if (len == 2 && Utils::IsAlphaString(str, len)) {
        return true;
    }
    // region is 3 letters if is numeric string
    if (len == 3 && Utils::IsNumericString(str, len)) {
        return true;
    }
    return false;
}

int8_t LocaleMatcher::IsMoreSpecificThan(const ResLocale *current, const ResLocale *other)
{
    // compare language
    if (current == nullptr && other == nullptr) {
        return 0;
    }
    if (current != nullptr && other == nullptr) {
        return 1;
    }
    if (current == nullptr && other != nullptr) {
        return -1;
    }
    // here language is equal,compare region
    if (current->GetRegion() == other->GetRegion()) {
        return 0;
    }
    if (current->GetRegion() == nullptr) {
        return -1;
    }
    if (other->GetRegion() == nullptr) {
        return 1;
    }
    return 0;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS