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

#include "global.h"

#include <securec.h>

#include "auto_mutex.h"
#include "hap_manager.h"
#include "hilog_wrapper.h"
#include "utils/errors.h"
#include "utils/string_utils.h"

using namespace OHOS::Global::Resource;

static ResConfigImpl *g_resConfig = nullptr;

static Lock g_lock;

static void FreeValue(char **value)
{
    if (*value != nullptr) {
        free(*value);
        *value = nullptr;
    }
}

static void InitResConfig()
{
    AutoMutex mutex(g_lock);
    if (g_resConfig == nullptr) {
        g_resConfig = new (std::nothrow) ResConfigImpl;
        if (g_resConfig == nullptr) {
            HILOG_ERROR("new ResConfigImpl failed when GLOBAL_ConfigLanguage");
        }
    }
}

void GLOBAL_ConfigLanguage(const char *appLanguage)
{
    if (appLanguage == nullptr) {
        return;
    }
    InitResConfig();

    std::string lan(appLanguage);
    std::string language, script, region;
    auto index1 = lan.find("-");
    auto index2 = lan.find("_");
    auto indexStart = std::string::npos;
    auto indexEnd = std::string::npos;
    if (index1 != std::string::npos) {
        indexStart = index1;
        indexEnd = lan.find("-", index1 + 1);
    } else if (index2 != std::string::npos) {
        indexStart = index2;
        indexEnd = lan.find("_", index2 + 1);
    }

    if (indexStart != std::string::npos) {
        if (indexEnd != std::string::npos) {
            language.assign(appLanguage, indexStart);
            script.assign(appLanguage + indexStart + 1, indexEnd - indexStart - 1);
            region.assign(appLanguage + indexEnd + 1);
            g_resConfig->SetLocaleInfo(language.c_str(), script.c_str(), region.c_str());
        } else {
            language.assign(appLanguage, indexStart);
            region.assign(appLanguage + indexStart + 1);
            g_resConfig->SetLocaleInfo(language.c_str(), nullptr, region.c_str());
        }
    } else {
        g_resConfig->SetLocaleInfo(appLanguage, nullptr, nullptr);
    }
}

int32_t GLOBAL_GetLanguage(char *language, uint8_t len)
{
    if (g_resConfig == nullptr || g_resConfig->GetLocaleInfo() == nullptr
        || g_resConfig->GetLocaleInfo()->GetLanguage() == nullptr) {
        return SYS_ERROR;
    }
    if (language == nullptr || len == 0) {
        return SYS_ERROR;
    }
    if (strncpy_s(language, len, g_resConfig->GetLocaleInfo()->GetLanguage(),
        strlen(g_resConfig->GetLocaleInfo()->GetLanguage())) != EOK) {
        return SYS_ERROR;
    }
    return OK;
}

int32_t GLOBAL_GetRegion(char *region, uint8_t len)
{
    if (g_resConfig == nullptr || g_resConfig->GetLocaleInfo() == nullptr
        || g_resConfig->GetLocaleInfo()->GetRegion() == nullptr) {
        return SYS_ERROR;
    }
    if (region == nullptr || len == 0) {
        return SYS_ERROR;
    }
    if (strncpy_s(region, len, g_resConfig->GetLocaleInfo()->GetRegion(),
        strlen(g_resConfig->GetLocaleInfo()->GetRegion())) != EOK) {
        return SYS_ERROR;
    }
    return OK;
}

int32_t GetValue(const IdItem *idItem, char **value)
{
    if (idItem == nullptr) {
        return SYS_ERROR;
    }
    if (idItem->isArray_) {
        std::string ret("[");
        for (size_t i = 0; i < idItem->values_.size(); ++i) {
            ret.append(FormatString("'%s',", idItem->values_[i].c_str()));
        }
        ret.append("]");
        *value = static_cast<char *>(malloc(ret.size() + 1));
        if (*value == nullptr || strcpy_s(*value, ret.size() + 1, ret.c_str()) != EOK) {
            FreeValue(value);
            return SYS_ERROR;
        }
        (*value)[ret.size()] = '\0';
    } else {
        *value = static_cast<char *>(malloc(idItem->valueLen_ + 1));
        if (*value == nullptr || strcpy_s(*value, idItem->valueLen_ + 1, idItem->value_.c_str()) != EOK) {
            FreeValue(value);
            return SYS_ERROR;
        }
        (*value)[idItem->valueLen_] = '\0';
    }
    return OK;
}

int32_t GLOBAL_GetValueById(uint32_t id, const char *path, char **value)
{
    bool resConfigSet = true;
    if (g_resConfig == nullptr || g_resConfig->GetLocaleInfo() == nullptr
        || g_resConfig->GetLocaleInfo()->GetLanguage() == nullptr) {
        HILOG_WARN("GLOBAL_GetValueById language is null, use default");
        resConfigSet = false;
    }
    ResConfigImpl *resConfig = new(std::nothrow) ResConfigImpl;
    if (resConfig == nullptr) {
        HILOG_ERROR("new ResConfigImpl failed when GLOBAL_GetValueByName");
        return SYS_ERROR;
    }
    HapManager hapManager(resConfig);
    if (resConfigSet) {
        hapManager.UpdateResConfig(*g_resConfig);
    }

    bool ret = hapManager.AddResource(path);
    if (!ret) {
        HILOG_ERROR("GLOBAL_GetValueById AddResource error %s", path);
        return SYS_ERROR;
    }

    auto idItem = hapManager.FindResourceById(id);
    if (idItem == nullptr) {
        return OBJ_NOT_FOUND;
    }

    return GetValue(idItem, value);
}

int32_t GLOBAL_GetValueByName(const char *name, const char *path, char **value)
{
    bool resConfigSet = true;
    if (g_resConfig == nullptr || g_resConfig->GetLocaleInfo() == nullptr
        || g_resConfig->GetLocaleInfo()->GetLanguage() == nullptr) {
        HILOG_WARN("GLOBAL_GetValueByName language is null, use default");
        resConfigSet = false;
    }

    ResConfigImpl *resConfig = new(std::nothrow) ResConfigImpl;
    if (resConfig == nullptr) {
        HILOG_ERROR("new ResConfigImpl failed when GLOBAL_GetValueByName");
        return SYS_ERROR;
    }
    HapManager hapManager(resConfig);
    if (resConfigSet) {
        hapManager.UpdateResConfig(*g_resConfig);
    }

    bool ret = hapManager.AddResource(path);
    if (!ret) {
        HILOG_ERROR("GLOBAL_GetValueByName AddResource error %s", path);
        return SYS_ERROR;
    }

    const IdItem *idItem = nullptr;
    for (int i = 0; i < ResType::MAX_RES_TYPE; ++i) {
        idItem = hapManager.FindResourceByName(name, (ResType)i);
        if (idItem != nullptr) {
            break;
        }
    }
    if (idItem == nullptr) {
        return OBJ_NOT_FOUND;
    }

    return GetValue(idItem, value);
}
