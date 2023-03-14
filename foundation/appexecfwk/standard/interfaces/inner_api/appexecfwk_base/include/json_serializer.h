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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_JSON_SERIALIZER_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_JSON_SERIALIZER_H

#include "bundle_info.h"
#include "bundle_pack_info.h"
#include "bundle_user_info.h"
#include "common_event_info.h"
#include "distributed_ability_info.h"
#include "distributed_module_info.h"
#include "form_info.h"
#include "free_install/dispatch_info.h"
#include "free_install/install_result.h"
#include "free_install/target_ability_info.h"
#include "nlohmann/json.hpp"
#include "shortcut_info.h"

namespace OHOS {
namespace AppExecFwk {

/*
 * form_json and to_json is global static overload method, which need callback by json library,
 * and can not rename this function, so don't named according UpperCamelCase style
 */
void to_json(nlohmann::json &jsonObject, const CustomizeData &customizeData);
void from_json(const nlohmann::json &jsonObject, CustomizeData &customizeData);
void to_json(nlohmann::json &jsonObject, const MetaData &metaData);
void from_json(const nlohmann::json &jsonObject, MetaData &metaData);
void to_json(nlohmann::json &jsonObject, const Metadata &metadata);
void from_json(const nlohmann::json &jsonObject, Metadata &metadata);
void to_json(nlohmann::json &jsonObject, const AbilityInfo &abilityInfo);
void from_json(const nlohmann::json &jsonObject, AbilityInfo &abilityInfo);
void to_json(nlohmann::json &jsonObject, const ExtensionAbilityInfo &extensionInfo);
void from_json(const nlohmann::json &jsonObject, ExtensionAbilityInfo &extensionInfo);
void to_json(nlohmann::json &jsonObject, const ApplicationInfo &applicationInfo);
void from_json(const nlohmann::json &jsonObject, ApplicationInfo &applicationInfo);
void to_json(nlohmann::json &jsonObject, const BundleInfo &bundleInfo);
void from_json(const nlohmann::json &jsonObject, BundleInfo &bundleInfo);
void to_json(nlohmann::json &jsonObject, const ModuleInfo &moduleInfo);
void from_json(const nlohmann::json &jsonObject, ModuleInfo &moduleInfo);
void to_json(nlohmann::json &jsonObject, const FormInfo &formInfo);
void from_json(const nlohmann::json &jsonObject, FormInfo &formInfo);
void to_json(nlohmann::json &jsonObject, const ShortcutInfo &shortcutInfo);
void from_json(const nlohmann::json &jsonObject, ShortcutInfo &shortcutInfo);
void to_json(nlohmann::json &jsonObject, const CommonEventInfo &commonEvent);
void from_json(const nlohmann::json &jsonObject, CommonEventInfo &commonEvent);
void to_json(nlohmann::json &jsonObject, const HapModuleInfo &hapModuleInfo);
void from_json(const nlohmann::json &jsonObject, HapModuleInfo &hapModuleInfo);
void to_json(nlohmann::json& jsonObject, const BundleUserInfo& bundleUserInfo);
void from_json(const nlohmann::json& jsonObject, BundleUserInfo& bundleUserInfo);
void from_json(const nlohmann::json &jsonObject, ShortcutWant &shortcutWant);
void from_json(const nlohmann::json &jsonObject, Shortcut &shortcut);
void from_json(const nlohmann::json &jsonObject, ShortcutJson &shortcutJson);
void to_json(nlohmann::json &jsonObject, const RequestPermissionUsedScene &usedScene);
void from_json(const nlohmann::json &jsonObject, RequestPermissionUsedScene &usedScene);
void to_json(nlohmann::json &jsonObject, const RequestPermission &requestPermission);
void from_json(const nlohmann::json &jsonObject, RequestPermission &requestPermission);
void to_json(nlohmann::json& jsonObject, const DistributedAbilityInfo& distributedAbilityInfo);
void from_json(const nlohmann::json& jsonObject, DistributedAbilityInfo& distributedAbilityInfo);
void to_json(nlohmann::json& jsonObject, const DistributedModuleInfo& distributedModuleInfo);
void from_json(const nlohmann::json& jsonObject, DistributedModuleInfo& distributedModuleInfo);
void to_json(nlohmann::json &jsonObject, const TargetInfo &targetInfo);
void from_json(const nlohmann::json &jsonObject, TargetInfo &targetInfo);
void to_json(nlohmann::json &jsonObject, const Result &result);
void from_json(const nlohmann::json &jsonObject, Result &result);
void to_json(nlohmann::json &jsonObject, const Progress &progress);
void from_json(const nlohmann::json &jsonObject, Progress &progress);
void to_json(nlohmann::json &jsonObject, const TargetAbilityInfo &targetAbilityInfo);
void from_json(const nlohmann::json &jsonObject, TargetAbilityInfo &targetAbilityInfo);
void to_json(nlohmann::json &jsonObject, const InstallResult &installResult);
void from_json(const nlohmann::json &jsonObject, InstallResult &installResult);
void to_json(nlohmann::json &jsonObject, const DispatcherInfo &dispatcherInfo);
void from_json(const nlohmann::json &jsonObject, DispatcherInfo &dispatcherInfo);
void to_json(nlohmann::json &jsonObject, const Version &version);
void from_json(const nlohmann::json &jsonObject, Version &version);
void to_json(nlohmann::json &jsonObject, const PackageApp &app);
void from_json(const nlohmann::json &jsonObject, PackageApp &app);
void to_json(nlohmann::json &jsonObject, const ModuleAbilityInfo &abilityinfo);
void from_json(const nlohmann::json &jsonObject, ModuleAbilityInfo &abilityinfo);
void to_json(nlohmann::json &jsonObject, const AbilityFormInfo &abilityinfo);
void from_json(const nlohmann::json &jsonObject, AbilityFormInfo &abilityinfo);
void to_json(nlohmann::json &jsonObject, const ModuleDistro &distro);
void from_json(const nlohmann::json &jsonObject, ModuleDistro &distro);
void to_json(nlohmann::json &jsonObject, const ApiVersion &apiVersion);
void from_json(const nlohmann::json &jsonObject, ApiVersion &apiVersion);
void to_json(nlohmann::json &jsonObject, const PackageModule &packageModule);
void from_json(const nlohmann::json &jsonObject, PackageModule &packageModule);
void to_json(nlohmann::json &jsonObject, const Summary &summary);
void from_json(const nlohmann::json &jsonObject, Summary &summary);
void to_json(nlohmann::json &jsonObject, const Packages &packages);
void from_json(const nlohmann::json &jsonObject, Packages &packages);
void to_json(nlohmann::json &jsonObject, const BundlePackInfo &bundlePackInfo);
void from_json(const nlohmann::json &jsonObject, BundlePackInfo &bundlePackInfo);
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_JSON_SERIALIZER_H
