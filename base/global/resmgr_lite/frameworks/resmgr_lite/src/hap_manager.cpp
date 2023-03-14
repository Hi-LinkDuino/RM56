/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "hap_manager.h"

#include <algorithm>
#include <types.h>

#include "auto_mutex.h"
#include "hilog_wrapper.h"
#include "locale_matcher.h"

namespace OHOS {
namespace Global {
namespace Resource {
constexpr uint32_t PLURAL_CACHE_MAX_COUNT = 3;

HapManager::HapManager(ResConfigImpl *resConfig)
    : resConfig_(resConfig)
{
}

std::string ConvertToPluralStr(int idxRet)
{
    switch (idxRet) {
        case OHOS::I18N::PluralRuleType::ZERO:
            return "zero";
        case OHOS::I18N::PluralRuleType::ONE:
            return "one";
        case OHOS::I18N::PluralRuleType::TWO:
            return "two";
        case OHOS::I18N::PluralRuleType::FEW:
            return "few";
        case OHOS::I18N::PluralRuleType::MANY:
            return "many";
        case OHOS::I18N::PluralRuleType::OTHER:
        default:
            return "other";
    }
}

std::string HapManager::GetPluralRulesAndSelect(int quantity)
{
    AutoMutex mutex(this->lock_);
    std::string defaultRet("other");
    if (this->resConfig_ == nullptr || this->resConfig_->GetResLocale() == nullptr ||
        this->resConfig_->GetResLocale()->GetLanguage() == nullptr) {
        HILOG_ERROR("GetPluralRules language is null!");
        return defaultRet;
    }
    std::string language = this->resConfig_->GetResLocale()->GetLanguage();

    OHOS::I18N::PluralFormat *pluralFormat = nullptr;
    for (uint32_t i = 0; i < plurRulesCache_.size(); i++) {
        auto pair = plurRulesCache_[i];
        if (language == pair.first) {
            // cache hit
            pluralFormat = pair.second;
            break;
        }
    }

    OHOS::I18N::I18nStatus status = OHOS::I18N::I18nStatus::ISUCCESS;
    if (pluralFormat == nullptr) {
        // no cache hit
        OHOS::I18N::LocaleInfo locale(language.c_str(), "", "");
        pluralFormat = new(std::nothrow) OHOS::I18N::PluralFormat(locale, status);
        if (pluralFormat == nullptr) {
            HILOG_ERROR("new PluralFormat failed");
            return defaultRet;
        }
        if (status != OHOS::I18N::I18nStatus::ISUCCESS) {
            HILOG_ERROR("PluralFormat init failed");
            delete pluralFormat;
            return defaultRet;
        }
        // after pluralFormat created, we add it to cache, if > 3 delete oldest one
        if (plurRulesCache_.size() >= PLURAL_CACHE_MAX_COUNT) {
            HILOG_DEBUG("cache rotate delete plurRulesMap_ %s", plurRulesCache_[0].first.c_str());
            delete (plurRulesCache_[0].second);
            plurRulesCache_.erase(plurRulesCache_.begin());
        }
        auto plPair = std::make_pair(language, pluralFormat);
        plurRulesCache_.push_back(plPair);
    }
    int idxRet = pluralFormat->GetPluralRuleIndex(quantity, status);
    return ConvertToPluralStr(idxRet);
}

const IdItem *HapManager::FindResourceById(uint32_t id)
{
    auto qualifierValue = FindQualifierValueById(id);
    if (qualifierValue == nullptr) {
        return nullptr;
    }
    return qualifierValue->GetIdItem();
}

const IdItem *HapManager::FindResourceByName(const char *name, const ResType resType)
{
    auto qualifierValue = FindQualifierValueByName(name, resType);
    if (qualifierValue == nullptr) {
        return nullptr;
    }
    return qualifierValue->GetIdItem();
}

const HapResource::ValueUnderQualifierDir *HapManager::FindQualifierValueByName(
    const char *name, const ResType resType)
{
    const HapResource::IdValues *idValues = this->GetResourceListByName(name, resType);
    if (idValues == nullptr) {
        return nullptr;
    }
    const std::vector<HapResource::ValueUnderQualifierDir *> paths = idValues->GetLimitPathsConst();

    size_t len = paths.size();
    size_t i = 0;
    size_t bestIndex = -1;
    const ResConfigImpl *bestResConfig = nullptr;
    const ResConfigImpl *currentResConfig = this->resConfig_;
    for (i = 0; i < len; i++) {
        HapResource::ValueUnderQualifierDir *path = paths[i];
        const ResConfigImpl *resConfig = path->GetResConfig();
        if (!this->resConfig_->Match(resConfig)) {
            continue;
        }
        if (bestResConfig == nullptr) {
            bestIndex = i;
            bestResConfig = resConfig;
            continue;
        }
        if (!bestResConfig->IsMoreSuitable(resConfig, currentResConfig)) {
            bestResConfig = resConfig;
            bestIndex = i;
        }
    }
    return paths[bestIndex];
}

const HapResource::ValueUnderQualifierDir *HapManager::FindQualifierValueById(uint32_t id)
{
    const HapResource::IdValues *idValues = this->GetResourceList(id);
    if (idValues == nullptr) {
        return nullptr;
    }
    const std::vector<HapResource::ValueUnderQualifierDir *> paths = idValues->GetLimitPathsConst();

    size_t len = paths.size();
    size_t i = 0;
    size_t bestIndex = -1;
    const ResConfigImpl *bestResConfig = nullptr;
    const ResConfigImpl *currentResConfig = this->resConfig_;
    for (i = 0; i < len; i++) {
        HapResource::ValueUnderQualifierDir *path = paths[i];
        const ResConfigImpl *resConfig = path->GetResConfig();
        if (!this->resConfig_->Match(resConfig)) {
            continue;
        }
        if (bestResConfig == nullptr) {
            bestIndex = i;
            bestResConfig = resConfig;
            continue;
        }
        if (!bestResConfig->IsMoreSuitable(resConfig, currentResConfig)) {
            bestResConfig = resConfig;
            bestIndex = i;
        }
    }
    return paths[bestIndex];
}

RState HapManager::UpdateResConfig(ResConfig &resConfig)
{
    AutoMutex mutex(this->lock_);
    this->resConfig_->Copy(resConfig);
    RState rState = this->ReloadAll();
    if (rState != SUCCESS) {
        HILOG_ERROR("ReloadAll() failed when UpdateResConfig!");
    }
    return rState;
}


void HapManager::GetResConfig(ResConfig &resConfig)
{
    AutoMutex mutex(this->lock_);
    resConfig.Copy(*(this->resConfig_));
}

bool HapManager::AddResource(const char *path)
{
    AutoMutex mutex(this->lock_);
    return this->AddResourcePath(path);
}

HapManager::~HapManager()
{
    for (size_t i = 0; i < hapResources_.size(); ++i) {
        auto ptr = hapResources_[i];
        delete (ptr);
    }
    delete resConfig_;

    auto iter = plurRulesCache_.begin();
    for (; iter != plurRulesCache_.end(); iter++) {
        HILOG_DEBUG("delete plurRulesMap_ %s", iter->first.c_str());
        auto ptr = iter->second;
        delete (ptr);
    }
}

const HapResource::IdValues *HapManager::GetResourceList(uint32_t ident) const
{
    // one id only exit in one hap
    for (size_t i = 0; i < hapResources_.size(); ++i) {
        HapResource *pResource = hapResources_[i];
        const HapResource::IdValues *out = pResource->GetIdValues(ident);
        if (out != nullptr) {
            return out;
        }
    }
    return nullptr;
}

const HapResource::IdValues *HapManager::GetResourceListByName(const char *name, const ResType resType) const
{
    // first match will return
    for (size_t i = 0; i < hapResources_.size(); ++i) {
        HapResource *pResource = hapResources_[i];
        const HapResource::IdValues *out = pResource->GetIdValuesByName(std::string(name), resType);
        if (out != nullptr) {
            return out;
        }
    }
    return nullptr;
}

bool HapManager::AddResourcePath(const char *path)
{
    std::string sPath(path);
    std::vector<std::string>::iterator it = std::find(loadedHapPaths_.begin(),
        loadedHapPaths_.end(), sPath);
    if (it != loadedHapPaths_.end()) {
        HILOG_ERROR(" %s has already been loaded!", path);
        return false;
    }
    const HapResource *pResource = HapResource::LoadFromIndex(path, resConfig_);
    if (pResource == nullptr) {
        return false;
    }
    this->hapResources_.push_back(const_cast<HapResource *>(pResource));
    this->loadedHapPaths_.push_back(sPath);
    return true;
}

RState HapManager::ReloadAll()
{
    if (hapResources_.size() == 0) {
        return SUCCESS;
    }
    std::vector<HapResource *> newResources;
    for (size_t i = 0; i < hapResources_.size(); ++i) {
        const HapResource *pResource = HapResource::LoadFromIndex(hapResources_[i]->GetIndexPath().c_str(), resConfig_);
        if (pResource == nullptr) {
            for (size_t j = 0; j < newResources.size(); ++j) {
                delete (newResources[j]);
            }
            return HAP_INIT_FAILED;
        }
        newResources.push_back(const_cast<HapResource *>(pResource));
    }
    for (size_t i = 0; i < hapResources_.size(); ++i) {
        delete (hapResources_[i]);
    }
    hapResources_ = newResources;
    return SUCCESS;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS