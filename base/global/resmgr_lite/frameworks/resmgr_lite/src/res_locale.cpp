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

#include "res_locale.h"

#include <cctype>
#include <cstdint>
#include <cstring>
#include <new>

#include "auto_mutex.h"
#include "hilog_wrapper.h"
#include "locale_matcher.h"
#include "res_config.h"
#include "rstate.h"
#include "utils/common.h"
#include "utils/utils.h"

namespace OHOS {
namespace Global {
namespace Resource {
LocaleInfo *ResLocale::defaultLocale_ = nullptr;
Lock ResLocale::lock_;

ResLocale::ResLocale() : language_(nullptr), region_(nullptr), script_(nullptr)
{
}

RState ResLocale::SetLanguage(const char *language, size_t len)
{
    if (len == 0) {
        delete this->language_;
        this->language_ = nullptr;
        return SUCCESS;
    }
    char *temp = new(std::nothrow) char[len + 1];
    if (temp == nullptr) {
        return NOT_ENOUGH_MEM;
    }
    delete this->language_;
    this->language_ = temp;
    size_t i = 0;
    while (i < len) {
        *(temp + i) = tolower(*(language + i));
        ++i;
    }
    *(temp + len) = '\0';
    return SUCCESS;
}

RState ResLocale::SetRegion(const char *region, size_t len)
{
    if (len == 0) {
        delete this->region_;
        this->region_ = nullptr;
        return SUCCESS;
    }
    char *temp = new(std::nothrow) char[len + 1];
    if (temp == nullptr) {
        return NOT_ENOUGH_MEM;
    }
    delete this->region_;
    this->region_ = temp;
    size_t i = 0;
    while (i < len) {
        *(temp + i) = toupper(*(region + i));
        ++i;
    }
    *(temp + len) = '\0';
    return SUCCESS;
}

RState ResLocale::SetScript(const char *script, size_t len)
{
    if (len == 0) {
        delete this->script_;
        this->script_ = nullptr;
        return SUCCESS;
    }
    char *temp = new(std::nothrow) char[len + 1];
    if (temp == nullptr) {
        return NOT_ENOUGH_MEM;
    }
    delete this->script_;
    this->script_ = temp;
    size_t i = 0;
    while (i < len) {
        if (i == 0) {
            *(temp + i) = toupper(*(script + i));
        } else {
            *(temp + i) = tolower(*(script + i));
        }
        ++i;
    }
    *(temp + len) = '\0';
    return SUCCESS;
}

RState ResLocale::Init(const char *language, size_t languageLen, const char *script, size_t scriptLen,
    const char *region, size_t regionLen)
{
    RState r = this->SetLanguage(language, languageLen);
    if (r != SUCCESS) {
        return r;
    }
    r = this->SetScript(script, scriptLen);
    if (r != SUCCESS) {
        return r;
    }
    r = this->SetRegion(region, regionLen);
    if (r != SUCCESS) {
        return r;
    }
    return SUCCESS;
}

RState ResLocale::CopyFromLocaleInfo(const LocaleInfo *other)
{
    if (other == nullptr) {
        return ERROR;
    }
    return this->Init(other->GetLanguage(), Utils::StrLen(other->GetLanguage()), other->GetScript(),
        Utils::StrLen(other->GetScript()), other->GetRegion(), Utils::StrLen(other->GetRegion()));
}

RState ResLocale::Copy(const ResLocale *other)
{
    if (other == nullptr) {
        return ERROR;
    }
    return this->Init(other->GetLanguage(), Utils::StrLen(other->GetLanguage()), other->GetScript(),
        Utils::StrLen(other->GetScript()), other->GetRegion(), Utils::StrLen(other->GetRegion()));
}

const char *ResLocale::GetLanguage() const
{
    return this->language_;
}

const char *ResLocale::GetRegion() const
{
    return this->region_;
}

const char *ResLocale::GetScript() const
{
    return this->script_;
}

RState ProcessSubtag(const char *curPos, int32_t subTagLen, uint16_t &nextType, ParseResult &r)
{
    if ((ResLocale::LANG_TYPE & nextType) && (LocaleMatcher::IsLanguageTag(curPos, subTagLen))) {
        r.tempLanguage = curPos;
        r.languageTagLen = subTagLen;
        nextType = ResLocale::SCRIPT_TYPE | ResLocale::REGION_TYPE;
        return SUCCESS;
    }
    if ((ResLocale::SCRIPT_TYPE & nextType) && LocaleMatcher::IsScriptTag(curPos, subTagLen)) {
        r.tempScript = curPos;
        r.scriptTagLen = subTagLen;
        nextType = ResLocale::REGION_TYPE;
        return SUCCESS;
    }
    if ((ResLocale::REGION_TYPE & nextType) && LocaleMatcher::IsRegionTag(curPos, subTagLen)) {
        r.tempRegion = curPos;
        r.regionTagLen = subTagLen;
        nextType = ResLocale::END_TYPE;
        return SUCCESS;
    }
    return ERROR;
}

void CheckArg(char sep, RState &rState)
{
    rState = SUCCESS;
    if (sep != DASH_SEP && sep != UNDERLINE_SEP) {
        rState = NOT_SUPPORT_SEP;
    }
}

ResLocale *ResLocale::CreateResLocale(ParseResult &r, RState &rState)
{
    ResLocale *resLocale = new(std::nothrow) ResLocale;
    if (resLocale == nullptr) {
        rState = NOT_ENOUGH_MEM;
        return nullptr;
    }
    rState = resLocale->Init(r.tempLanguage, r.languageTagLen, r.tempScript, r.scriptTagLen,
        r.tempRegion, r.regionTagLen);
    if (rState == SUCCESS) {
        return resLocale;
    }
    delete resLocale;
    return nullptr;
}

ResLocale *ResLocale::DoParse(const char *str, char sep, RState &rState)
{
    uint16_t nextType = LANG_TYPE;
    const char *nextPos = str;
    const char *curPos = nextPos;
    ParseResult r;
    while (nextPos) {
        if (nextType == END_TYPE) {
            break;
        }
        const char *pSep = nextPos;
        curPos = nextPos;
        while (*pSep) {
            if (*pSep == sep) {
                break;
            }
            pSep++;
        }
        nextPos = ((*pSep == 0) ? nullptr : (pSep + 1));
        int16_t subTagLen = pSep - curPos;
        if (nextType & LANG_TYPE) {
            rState = ProcessSubtag(curPos, subTagLen, nextType, r);
            if (rState == SUCCESS) {
                continue;
            }
            rState = INVALID_BCP47_LANGUAGE_SUBTAG;
            return nullptr;
        }
        if (nextType & SCRIPT_TYPE) {
            rState = ProcessSubtag(curPos, subTagLen, nextType, r);
            if (rState == SUCCESS) {
                continue;
            }
            rState = INVALID_BCP47_SCRIPT_SUBTAG;
            return nullptr;
        }
        if (nextType & REGION_TYPE) {
            rState = ProcessSubtag(curPos, subTagLen, nextType, r);
            if (rState == SUCCESS) {
                continue;
            }
            rState = INVALID_BCP47_REGION_SUBTAG;
            return nullptr;
        }
    }
    return CreateResLocale(r, rState);
}

ResLocale *ResLocale::BuildFromString(const char *str, char sep, RState &rState)
{
    CheckArg(sep, rState);
    if (rState != SUCCESS) {
        return nullptr;
    }
    size_t strLen = Utils::StrLen(str);
    if (strLen == 0) {
        return nullptr;
    }
    return DoParse(str, sep, rState);
} // end of ParseBCP47Tag

ResLocale *ResLocale::BuildFromParts(const char *language,
    const char *script,
    const char *region,
    RState &rState)
{
    size_t len = Utils::StrLen(language);
    if (len == 0) {
        rState = INVALID_BCP47_LANGUAGE_SUBTAG;
        return nullptr;
    }

    const char *tempLanguage = nullptr;
    const char *tempScript = nullptr;
    const char *tempRegion = nullptr;
    size_t languageTagLen = 0;
    size_t scriptTagLen = 0;
    size_t regionTagLen = 0;
    if (LocaleMatcher::IsLanguageTag(language, len)) {
        tempLanguage = language;
        languageTagLen = len;
    } else {
        rState = INVALID_BCP47_LANGUAGE_SUBTAG;
        return nullptr;
    }

    len = Utils::StrLen(script);
    if (len > 0) {
        if (LocaleMatcher::IsScriptTag(script, len)) {
            tempScript = script;
            scriptTagLen = len;
        } else {
            rState = INVALID_BCP47_SCRIPT_SUBTAG;
            return nullptr;
        }
    }
    len = Utils::StrLen(region);
    if (len > 0) {
        if (LocaleMatcher::IsRegionTag(region, len)) {
            tempRegion = region;
            regionTagLen = len;
        } else {
            rState = INVALID_BCP47_REGION_SUBTAG;
            return nullptr;
        }
    }
    ResLocale *resLocale = new(std::nothrow) ResLocale;
    if (resLocale == nullptr) {
        rState = NOT_ENOUGH_MEM;
        return nullptr;
    }
    rState = resLocale->Init(tempLanguage, languageTagLen, tempScript, scriptTagLen, tempRegion, regionTagLen);
    if (rState == SUCCESS) {
        return resLocale;
    }
    delete resLocale;
    return nullptr;
};

const LocaleInfo *ResLocale::GetDefault()
{
    AutoMutex mutex(ResLocale::lock_);
    return ResLocale::defaultLocale_;
}

bool ResLocale::UpdateDefault(const LocaleInfo &localeInfo, bool needNotify)
{
    AutoMutex mutex(ResLocale::lock_);
    LocaleInfo *temp = new(std::nothrow) LocaleInfo(localeInfo.GetLanguage(),
        localeInfo.GetScript(), localeInfo.GetRegion());
    if (temp == nullptr) {
        return false;
    }
    delete ResLocale::defaultLocale_;
    ResLocale::defaultLocale_ = temp;
    return true;
};

ResLocale::~ResLocale()
{
    if (this->language_ != nullptr) {
        delete[] this->language_;
        this->language_ = nullptr;
    }

    if (this->script_ != nullptr) {
        delete[] this->script_;
        this->script_ = nullptr;
    }

    if (this->region_ != nullptr) {
        delete[] this->region_;
        this->region_ = nullptr;
    }
}

LocaleInfo *BuildFromString(const char *str, char sep, RState &rState)
{
    ResLocale *resLocale = ResLocale::BuildFromString(str, sep, rState);
    if (rState == SUCCESS) {
        LocaleInfo *localeInfo = new(std::nothrow) LocaleInfo(resLocale->GetLanguage(),
            resLocale->GetScript(), resLocale->GetRegion());
        if (localeInfo == nullptr) {
            delete resLocale;
            rState = ERROR;
            return nullptr;
        }
        return localeInfo;
    }
    return nullptr;
};

LocaleInfo *BuildFromParts(const char *language, const char *script, const char *region, RState &rState)
{
    size_t len = Utils::StrLen(language);
    if (len == 0) {
        rState = INVALID_BCP47_LANGUAGE_SUBTAG;
        return nullptr;
    }
    if (!(LocaleMatcher::IsLanguageTag(language, len))) {
        rState = INVALID_BCP47_LANGUAGE_SUBTAG;
        return nullptr;
    }

    len = Utils::StrLen(script);
    if (len > 0) {
        if (LocaleMatcher::IsScriptTag(script, len) == 0) {
            rState = INVALID_BCP47_SCRIPT_SUBTAG;
            return nullptr;
        }
    }
    len = Utils::StrLen(region);
    if (len > 0) {
        if (LocaleMatcher::IsRegionTag(region, len) == 0) {
            rState = INVALID_BCP47_REGION_SUBTAG;
            return nullptr;
        }
    }
    LocaleInfo *localeInfo = new(std::nothrow) LocaleInfo(language, script, region);
    if (localeInfo == nullptr) {
        rState = ERROR;
        return nullptr;
    }
    return localeInfo;
}

const LocaleInfo *GetSysDefault()
{
    return ResLocale::GetDefault();
}

void UpdateSysDefault(const LocaleInfo &localeInfo, bool needNotify)
{
    ResLocale::UpdateDefault(localeInfo, needNotify);
}

void FindAndSort(std::string localeStr, std::vector<std::string> &candidateLocale, std::vector<std::string> &outValue)
{
    if (candidateLocale.size() == 0) {
        return;
    }
    std::vector<ResLocale *> tempCandidate;
    RState state = SUCCESS;
    ResLocale *currentLocale = ResLocale::BuildFromString(localeStr.c_str(), DASH_SEP, state);
    LocaleMatcher::Normalize(currentLocale);
    std::vector<std::string>::const_iterator iter;
    for (iter = candidateLocale.cbegin(); iter != candidateLocale.cend(); ++iter) {
        ResLocale *resLocale = ResLocale::BuildFromString(iter->c_str(), DASH_SEP, state);
        if (state == SUCCESS) {
            LocaleMatcher::Normalize(resLocale);
            bool isMatch = LocaleMatcher::Match(currentLocale, resLocale);
            if (isMatch) {
                tempCandidate.push_back(resLocale);
                outValue.push_back(*iter);
            } else {
                delete resLocale;
            }
        } else {
            delete resLocale;
        }
    }
    // sort
    std::size_t len = tempCandidate.size();
    if (len == 0) {
        delete currentLocale;
        return;
    }
    for (std::size_t i = 0; i < len - 1; i++) {
        for (std::size_t j = 0; j < len - 1 - i; j++) {
            if (LocaleMatcher::IsMoreSuitable(tempCandidate.at(j), tempCandidate.at(j + 1), currentLocale) <= 0) {
                ResLocale *temp = tempCandidate.at(j + 1);
                tempCandidate.at(j + 1) = tempCandidate.at(j);
                tempCandidate.at(j) = temp;
                std::string tempStr = outValue.at(j + 1);
                outValue.at(j + 1) = outValue.at(j);
                outValue.at(j) = tempStr;
            }
        }
    }

    for (auto iter = tempCandidate.cbegin(); iter != tempCandidate.cend(); iter++) {
        delete *iter;
    }
    delete currentLocale;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS