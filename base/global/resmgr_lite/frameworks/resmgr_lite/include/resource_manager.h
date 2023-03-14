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
#ifndef OHOS_RESOURCE_MANAGER_RESOURCEMANAGER_H
#define OHOS_RESOURCE_MANAGER_RESOURCEMANAGER_H

#include <map>
#include <string>
#include <vector>
#include "res_config.h"

namespace OHOS {
namespace Global {
namespace Resource {
class ResourceManager {
public:

    virtual ~ResourceManager() = 0;

    virtual bool AddResource(const char *path) = 0;

    virtual RState UpdateResConfig(ResConfig &resConfig) = 0;

    virtual void GetResConfig(ResConfig &resConfig) = 0;

    virtual RState GetStringById(uint32_t id, std::string &outValue) = 0;

    virtual RState GetStringByName(const char *name, std::string &outValue) = 0;

    virtual RState GetStringFormatById(std::string &outValue, uint32_t id, ...) = 0;

    virtual RState GetStringFormatByName(std::string &outValue, const char *name, ...) = 0;

    virtual RState GetStringArrayById(uint32_t id, std::vector<std::string> &outValue) = 0;

    virtual RState GetStringArrayByName(const char *name, std::vector<std::string> &outValue) = 0;

    virtual RState GetPatternById(uint32_t id, std::map<std::string, std::string> &outValue) = 0;

    virtual RState GetPatternByName(const char *name, std::map<std::string, std::string> &outValue) = 0;

    virtual RState GetPluralStringById(uint32_t id, int quantity, std::string &outValue) = 0;

    virtual RState GetPluralStringByName(const char *name, int quantity, std::string &outValue) = 0;

    virtual RState GetPluralStringByIdFormat(std::string &outValue, uint32_t id, int quantity, ...) = 0;

    virtual RState GetPluralStringByNameFormat(std::string &outValue, const char *name, int quantity, ...) = 0;

    virtual RState GetThemeById(uint32_t id, std::map<std::string, std::string> &outValue) = 0;

    virtual RState GetThemeByName(const char *name, std::map<std::string, std::string> &outValue) = 0;

    virtual RState GetBooleanById(uint32_t id, bool &outValue) = 0;

    virtual RState GetBooleanByName(const char *name, bool &outValue) = 0;

    virtual RState GetIntegerById(uint32_t id, int &outValue) = 0;

    virtual RState GetIntegerByName(const char *name, int &outValue) = 0;

    virtual RState GetFloatById(uint32_t id, float &outValue) = 0;

    virtual RState GetFloatByName(const char *name, float &outValue) = 0;

    virtual RState GetIntArrayById(uint32_t id, std::vector<int> &outValue) = 0;

    virtual RState GetIntArrayByName(const char *name, std::vector<int> &outValue) = 0;

    virtual RState GetColorById(uint32_t id, uint32_t &outValue) = 0;

    virtual RState GetColorByName(const char *name, uint32_t &outValue) = 0;

    virtual RState GetProfileById(uint32_t id, std::string &outValue) = 0;

    virtual RState GetProfileByName(const char *name, std::string &outValue) = 0;

    virtual RState GetMediaById(uint32_t id, std::string &outValue) = 0;

    virtual RState GetMediaByName(const char *name, std::string &outValue) = 0;
};

ResourceManager *CreateResourceManager();
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif