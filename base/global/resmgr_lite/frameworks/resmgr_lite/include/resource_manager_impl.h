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
#ifndef OHOS_RESOURCE_MANAGER_RESOURCEMANAGERIMPL_H
#define OHOS_RESOURCE_MANAGER_RESOURCEMANAGERIMPL_H

#include <map>
#include <string>
#include <vector>
#include "hap_manager.h"
#include "resource_manager.h"

namespace OHOS {
namespace Global {
namespace Resource {
class ResourceManagerImpl : public ResourceManager {
public:
    ResourceManagerImpl();

    ~ResourceManagerImpl();

    bool Init();

    /**
     * Add resource path to hap paths
     * @param path the resource path
     * @return true if add resource path success, else false
     */
    virtual bool AddResource(const char *path);

    /**
     * Update the resConfig
     * @param resConfig the resource config
     * @return the resConfig after update
     */
    virtual RState UpdateResConfig(ResConfig &resConfig);

    /**
     * Get the resConfig
     * @param resConfig the resource config
     */
    virtual void GetResConfig(ResConfig &resConfig);

    /**
     * Get string resource by id
     * @param id the resource id
     * @param outValue the string resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetStringById(uint32_t id, std::string &outValue);

    /**
     * Get string by resource name
     * @param name the resource name
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetStringByName(const char *name, std::string &outValue);

    /**
     * Get formatstring by resource id
     * @param id the resource id
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetStringFormatById(std::string &outValue, uint32_t id, ...);

    /**
     * Get formatstring by resource name
     * @param name the resource name
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetStringFormatByName(std::string &outValue, const char *name, ...);

    /**
     * Get the STRINGARRAY resource by resource id
     * @param id the resource id
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetStringArrayById(uint32_t id, std::vector<std::string> &outValue);

    /**
     * Get the STRINGARRAY resource by resource name
     * @param name the resource name
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetStringArrayByName(const char *name, std::vector<std::string> &outValue);

    /**
     * Get the PATTERN resource by resource id
     * @param id the resource id
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetPatternById(uint32_t id, std::map<std::string, std::string> &outValue);

    /**
     * Get the PATTERN resource by resource name
     * @param name the resource name
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetPatternByName(const char *name, std::map<std::string, std::string> &outValue);

    /**
     * Get the plural string by resource id
     * @param id the resource id
     * @param quantity the language quantity
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetPluralStringById(uint32_t id, int quantity, std::string &outValue);

    /**
     * Get the plural string by resource name
     * @param name the resource name
     * @param quantity the language quantity
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetPluralStringByName(const char *name, int quantity, std::string &outValue);

    /**
     * Get the plural format string by resource id
     * @param outValue the resource write to
     * @param id the resource id
     * @param quantity the language quantity
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetPluralStringByIdFormat(std::string &outValue, uint32_t id, int quantity, ...);

    /**
     * Get the plural format string by resource name
     * @param outValue the resource write to
     * @param id the resource id
     * @param quantity the language quantity
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetPluralStringByNameFormat(std::string &outValue, const char *name, int quantity, ...);

    /**
     * Get the THEME resource by resource id
     * @param id the resource id
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetThemeById(uint32_t id, std::map<std::string, std::string> &outValue);

    /**
     * Get the THEME resource by resource name
     * @param name the resource name
     * @param outValue the resource write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetThemeByName(const char *name, std::map<std::string, std::string> &outValue);

    /**
     * Get the BOOLEAN resource by resource id
     * @param id the resource id
     * @param outValue the obtain boolean value write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetBooleanById(uint32_t id, bool &outValue);

    /**
     * Get the BOOLEAN resource by resource name
     * @param name the resource name
     * @param outValue the obtain boolean value write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetBooleanByName(const char *name, bool &outValue);

    /**
     * Get the INTEGER resource by resource id
     * @param id the resource id
     * @param outValue the obtain Integer value write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetIntegerById(uint32_t id, int &outValue);

    /**
     * Get the INTEGER resource by resource name
     * @param name the resource name
     * @param outValue the obtain Integer value write to
     * @return SUCCESS if resource path exist, else NOT_FOUND
     */
    virtual RState GetIntegerByName(const char *name, int &outValue);

    /**
     * Get the FLOAT resource by resource id
     * @param id the resource id
     * @param outValue the obtain float value write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetFloatById(uint32_t id, float &outValue);

    /**
     * Get the FLOAT resource by resource name
     * @param name the resource name
     * @param outValue the obtain float value write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetFloatByName(const char *name, float &outValue);

    /**
     * Get the INTARRAY resource by resource id
     * @param id the resource id
     * @param outValue the obtain resource value convert to vector<int> write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetIntArrayById(uint32_t id, std::vector<int> &outValue);

    /**
     * Get the NTARRAY resource by resource name
     * @param name the resource name
     * @param outValue the obtain resource value convert to vector<int> write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetIntArrayByName(const char *name, std::vector<int> &outValue);

    /**
     * Get the COLOR resource by resource id
     * @param id the resource id
     * @param outValue the obtain resource value convert to uint32_t write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetColorById(uint32_t id, uint32_t &outValue);

    /**
     * Get the COLOR resource by resource name
     * @param name the resource name
     * @param outValue the obtain resource value convert to uint32_t write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetColorByName(const char *name, uint32_t &outValue);

    /**
     * Get the PROF resource by resource id
     * @param id the resource id
     * @param outValue the obtain resource path write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetProfileById(uint32_t id, std::string &outValue);

    /**
     * Get the PROF resource by resource name
     * @param name the resource name
     * @param outValue the obtain resource path write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetProfileByName(const char *name, std::string &outValue);

    /**
     * Get the MEDIA resource by resource id
     * @param id the resource id
     * @param outValue the obtain resource path write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetMediaById(uint32_t id, std::string &outValue);

    /**
     * Get the MEDIA resource by resource name
     * @param name the resource name
     * @param outValue the obtain resource path write to
     * @return SUCCESS if resource exist, else NOT_FOUND
     */
    virtual RState GetMediaByName(const char *name, std::string &outValue);

private:
    RState GetString(const IdItem *idItem, std::string &outValue);

    RState GetStringArray(const IdItem *idItem, std::vector<std::string> &outValue);

    RState GetPattern(const IdItem *idItem, std::map<std::string, std::string> &outValue);

    RState GetTheme(const IdItem *idItem, std::map<std::string, std::string> &outValue);

    RState GetPluralString(const HapResource::ValueUnderQualifierDir *vuqd, int quantity, std::string &outValue);

    RState ResolveReference(const std::string value, std::string &outValue);

    RState GetBoolean(const IdItem *idItem, bool &outValue);

    RState GetFloat(const IdItem *idItem, float &outValue);

    RState GetInteger(const IdItem *idItem, int &outValue);

    RState GetColor(const IdItem *idItem, uint32_t &outValue);

    RState GetIntArray(const IdItem *idItem, std::vector<int> &outValue);

    RState GetRawFile(const HapResource::ValueUnderQualifierDir *vuqd, const ResType resType, std::string &outValue);

    RState ResolveParentReference(const IdItem *idItem, std::map<std::string, std::string> &outValue);

    HapManager *hapManager_;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif