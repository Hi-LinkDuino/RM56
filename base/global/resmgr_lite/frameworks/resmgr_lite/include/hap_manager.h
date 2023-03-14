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
#ifndef OHOS_RESOURCE_MANAGER_HAPMANAGER_H
#define OHOS_RESOURCE_MANAGER_HAPMANAGER_H

#include "res_config_impl.h"
#include "hap_resource.h"
#include "res_desc.h"
#include "lock.h"

#include <locale_info.h>
#include <plural_format.h>
#include <vector>

namespace OHOS {
namespace Global {
namespace Resource {
class HapManager {
public:
    /**
     * The constructor of HapManager
     */
    explicit HapManager(ResConfigImpl *resConfig);

    /**
     * The destructor of HapManager
     */
    ~HapManager();

    /**
     * Update the resConfig
     * @param resConfig the resource config
     * @return SUCCESS if the resConfig updated success, else HAP_INIT_FAILED
     */
    RState UpdateResConfig(ResConfig &resConfig);

    /**
     * Get the resConfig
     * @param resConfig the resource config
     */
    void GetResConfig(ResConfig &resConfig);

    /**
     * Add resource path to hap paths
     * @param path the resource path
     * @return true if add resource path success, else false
     */
    bool AddResource(const char *path);

    /**
     * Find resource by resource id
     * @param id the resource id
     * @return the resources related to resource id
     */
    const IdItem *FindResourceById(uint32_t id);

    /**
     * Find resource by resource name
     * @param name the resource name
     * @param resType the resource type
     * @return the resources related to resource name
     */
    const IdItem *FindResourceByName(const char *name, const ResType resType);

    /**
     * Find best resource path by resource id
     * @param id the resource id
     * @return the best resource path
     */
    const HapResource::ValueUnderQualifierDir *FindQualifierValueById(uint32_t id);

    /**
     * Find best resource path by resource name
     * @param name the resource name
     * @param resType the resource type
     * @return the best resource path
     */
    const HapResource::ValueUnderQualifierDir *FindQualifierValueByName(const char *name, const ResType resType);

    /**
     * Get the language pluralRule related to quantity
     * @param quantity the language quantity
     * @return the language pluralRule related to quantity
     */
    std::string GetPluralRulesAndSelect(int quantity);

private:
    void UpdateResConfigImpl(ResConfigImpl &resConfig);

    void GetResConfigImpl(ResConfigImpl &resConfig);

    const HapResource::IdValues *GetResourceList(uint32_t ident) const;

    const HapResource::IdValues *GetResourceListByName(const char *name, const ResType resType) const;

    bool AddResourcePath(const char *path);

    // when resConfig_ updated we must call ReloadAll()
    RState ReloadAll();

    // app res config
    ResConfigImpl *resConfig_;

    // set of hap Resources
    std::vector<HapResource *> hapResources_;

    // set of loaded hap path
    std::vector<std::string> loadedHapPaths_;

    // key is language
    std::vector<std::pair<std::string, OHOS::I18N::PluralFormat *>> plurRulesCache_;

    Lock lock_;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif