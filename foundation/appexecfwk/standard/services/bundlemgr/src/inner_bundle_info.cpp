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

#include "inner_bundle_info.h"

#include <deque>
#include <regex>
#include "bundle_mgr_client.h"
#include "bundle_permission_mgr.h"
#include "common_profile.h"
#include "distributed_module_info.h"
#include "distributed_ability_info.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string APP_TYPE = "appType";
const std::string UID = "uid";
const std::string GID = "gid";
const std::string BASE_DATA_DIR = "baseDataDir";
const std::string BUNDLE_STATUS = "bundleStatus";
const std::string BASE_APPLICATION_INFO = "baseApplicationInfo";
const std::string BASE_BUNDLE_INFO = "baseBundleInfo";
const std::string BASE_ABILITY_INFO = "baseAbilityInfos";
const std::string INNER_MODULE_INFO = "innerModuleInfos";
const std::string SKILL_INFOS = "skillInfos";
const std::string USER_ID = "userId_";
const std::string APP_FEATURE = "appFeature";
const std::string CAN_UNINSTALL = "canUninstall";
const std::string MODULE_PACKAGE = "modulePackage";
const std::string MODULE_PATH = "modulePath";
const std::string MODULE_NAME = "moduleName";
const std::string MODULE_DESCRIPTION = "description";
const std::string MODULE_DESCRIPTION_ID = "descriptionId";
const std::string MODULE_LABEL = "label";
const std::string MODULE_LABEL_ID = "labelId";
const std::string MODULE_DESCRIPTION_INSTALLATION_FREE = "installationFree";
const std::string MODULE_IS_REMOVABLE = "isRemovable";
const std::string MODULE_UPGRADE_FLAG = "upgradeFlag";
const std::string MODULE_IS_ENTRY = "isEntry";
const std::string MODULE_METADATA = "metaData";
const std::string MODULE_COLOR_MODE = "colorMode";
const std::string MODULE_DISTRO = "distro";
const std::string MODULE_REQ_CAPABILITIES = "reqCapabilities";
const std::string MODULE_DATA_DIR = "moduleDataDir";
const std::string MODULE_RES_PATH = "moduleResPath";
const std::string MODULE_HAP_PATH = "hapPath";
const std::string MODULE_ABILITY_KEYS = "abilityKeys";
const std::string MODULE_SKILL_KEYS = "skillKeys";
const std::string MODULE_FORMS = "formInfos";
const std::string MODULE_SHORTCUT = "shortcutInfos";
const std::string MODULE_COMMON_EVENT = "commonEvents";
const std::string MODULE_MAIN_ABILITY = "mainAbility";
const std::string MODULE_ENTRY_ABILITY_KEY = "entryAbilityKey";
const std::string MODULE_DEPENDENCIES = "dependencies";
const std::string NEW_BUNDLE_NAME = "newBundleName";
const std::string MODULE_SRC_PATH = "srcPath";
const std::string MODULE_HASH_VALUE = "hashValue";
const std::string SCHEME_SEPARATOR = "://";
const std::string PORT_SEPARATOR = ":";
const std::string PATH_SEPARATOR = "/";
const std::string IS_PREINSTALL_APP = "isPreInstallApp";
const std::string INSTALL_MARK = "installMark";
const char WILDCARD = '*';
const std::string TYPE_WILDCARD = "*/*";
const std::string INNER_BUNDLE_USER_INFOS = "innerBundleUserInfos";
const std::string MODULE_PROCESS = "process";
const std::string MODULE_SRC_ENTRANCE = "srcEntrance";
const std::string MODULE_DEVICE_TYPES = "deviceTypes";
const std::string MODULE_VIRTUAL_MACHINE = "virtualMachine";
const std::string MODULE_UI_SYNTAX = "uiSyntax";
const std::string MODULE_PAGES = "pages";
const std::string MODULE_META_DATA = "metadata";
const std::string MODULE_REQUEST_PERMISSIONS = "requestPermissions";
const std::string MODULE_DEFINE_PERMISSIONS = "definePermissions";
const std::string MODULE_DEF_PERMS = "defPermissions";
const std::string MODULE_EXTENSION_KEYS = "extensionKeys";
const std::string MODULE_EXTENSION_SKILL_KEYS = "extensionSkillKeys";
const std::string MODULE_IS_MODULE_JSON = "isModuleJson";
const std::string MODULE_IS_STAGE_BASED_MODEL = "isStageBasedModel";
const std::string BUNDLE_IS_NEW_VERSION = "isNewVersion";
const std::string BUNDLE_IS_NEED_UPDATE = "upgradeFlag";
const std::string BUNDLE_BASE_EXTENSION_INFOS = "baseExtensionInfos";
const std::string BUNDLE_EXTENSION_SKILL_INFOS = "extensionSkillInfos";
const std::string BUNDLE_PACK_INFO = "bundlePackInfo";
const std::string ALLOWED_ACLS = "allowedAcls";
const std::string META_DATA_SHORTCUTS_NAME = "ohos.ability.shortcuts";
const std::string APP_INDEX = "appIndex";
const std::string BUNDLE_IS_SANDBOX_APP = "isSandboxApp";
const std::string BUNDLE_SANDBOX_PERSISTENT_INFO = "sandboxPersistentInfo";

const std::string NameAndUserIdToKey(const std::string &bundleName, int32_t userId)
{
    return bundleName + Constants::FILE_UNDERLINE + std::to_string(userId);
}
}  // namespace

bool Skill::Match(const OHOS::AAFwk::Want &want) const
{
    bool matchAction = MatchAction(want.GetAction());
    if (!matchAction) {
        APP_LOGD("Action does not match");
        return false;
    }
    bool matchEntities = MatchEntities(want.GetEntities());
    if (!matchEntities) {
        APP_LOGD("Entities does not match");
        return false;
    }
    bool matchUriAndType = MatchUriAndType(want.GetUriString(), want.GetType());
    if (!matchUriAndType) {
        APP_LOGD("Uri or Type does not match");
        return false;
    }
    return true;
}

bool Skill::MatchLauncher(const OHOS::AAFwk::Want &want) const
{
    bool matchAction = MatchAction(want.GetAction());
    if (!matchAction) {
        APP_LOGD("Action does not match");
        return false;
    }
    bool matchEntities = MatchEntities(want.GetEntities());
    if (!matchEntities) {
        APP_LOGD("Entities does not match");
        return false;
    }
    return true;
}

bool Skill::MatchAction(const std::string &action) const
{
    // config actions empty, no match
    if (actions.empty()) {
        return false;
    }
    // config actions not empty, param empty, match
    if (action.empty()) {
        return true;
    }
    // config actions not empty, param not empty, if config actions contains param action, match
    return std::find(actions.cbegin(), actions.cend(), action) != actions.cend();
}

bool Skill::MatchEntities(const std::vector<std::string> &paramEntities) const
{
    // param entities empty, match
    if (paramEntities.empty()) {
        return true;
    }
    // config entities empty, param entities not empty, not match
    if (entities.empty()) {
        return false;
    }
    // config entities not empty, param entities not empty, if every param entity in config entities, match
    std::vector<std::string>::size_type size = paramEntities.size();
    for (std::vector<std::string>::size_type i = 0; i < size; i++) {
        bool ret = std::find(entities.cbegin(), entities.cend(), paramEntities[i]) == entities.cend();
        if (ret) {
            return false;
        }
    }
    return true;
}

bool Skill::MatchUriAndType(const std::string &uriString, const std::string &type) const
{
    if (uriString.empty() && type.empty()) {
        // case1 : param uri empty, param type empty
        if (uris.empty()) {
            return true;
        }
        for (const SkillUri &skillUri : uris) {
            if (skillUri.scheme.empty() && skillUri.type.empty()) {
                return true;
            }
        }
        return false;
    }
    if (uris.empty()) {
        return false;
    }
    if (!uriString.empty() && type.empty()) {
        // case2 : param uri not empty, param type empty
        for (const SkillUri &skillUri : uris) {
            if (MatchUri(uriString, skillUri) && skillUri.type.empty()) {
                return true;
            }
        }
        return false;
    } else if (uriString.empty() && !type.empty()) {
        // case3 : param uri empty, param type not empty
        for (const SkillUri &skillUri : uris) {
            if (skillUri.scheme.empty() && MatchType(type, skillUri.type)) {
                return true;
            }
        }
        return false;
    } else {
        // case4 : param uri not empty, param type not empty
        for (const SkillUri &skillUri : uris) {
            if (MatchUri(uriString, skillUri) && MatchType(type, skillUri.type)) {
                return true;
            }
        }
        return false;
    }
}

bool Skill::MatchUri(const std::string &uriString, const SkillUri &skillUri) const
{
    if (skillUri.scheme.empty()) {
        return uriString.empty();
    }
    if (skillUri.host.empty()) {
        return uriString == skillUri.scheme;
    }
    std::string skillUriString;
    skillUriString.append(skillUri.scheme).append(SCHEME_SEPARATOR).append(skillUri.host);
    if (!skillUri.port.empty()) {
        skillUriString.append(PORT_SEPARATOR).append(skillUri.port);
    }
    if (skillUri.path.empty() && skillUri.pathStartWith.empty() && skillUri.pathRegex.empty()) {
        return uriString == skillUriString;
    }
    skillUriString.append(PATH_SEPARATOR);
    // if one of path, pathStartWith, pathRegex match, then match
    if (!skillUri.path.empty()) {
        // path match
        std::string pathUri(skillUriString);
        pathUri.append(skillUri.path);
        if (uriString == pathUri) {
            return true;
        }
    }
    if (!skillUri.pathStartWith.empty()) {
        // pathStartWith match
        std::string pathStartWithUri(skillUriString);
        pathStartWithUri.append(skillUri.pathStartWith);
        if (uriString.find(pathStartWithUri) == 0) {
            return true;
        }
    }
    if (!skillUri.pathRegex.empty()) {
        // pathRegex match
        std::string pathRegxUri(skillUriString);
        pathRegxUri.append(skillUri.pathRegex);
        try {
            std::regex regex(pathRegxUri);
            if (regex_match(uriString, regex)) {
                return true;
            }
        } catch(...) {
            APP_LOGE("regex error");
        }
    }
    return false;
}

bool Skill::MatchType(const std::string &type, const std::string &skillUriType) const
{
    // type is not empty
    if (skillUriType.empty()) {
        return false;
    }
    if (type == TYPE_WILDCARD || skillUriType == TYPE_WILDCARD) {
        // param is */* or config is */*
        return true;
    }
    bool paramTypeRegex = type.back() == WILDCARD;
    if (paramTypeRegex) {
        // param is string/*
        std::string prefix = type.substr(0, type.length() - 1);
        return skillUriType.find(prefix) == 0;
    }
    bool typeRegex = skillUriType.back() == WILDCARD;
    if (typeRegex) {
        // config is string/*
        std::string prefix = skillUriType.substr(0, skillUriType.length() - 1);
        return type.find(prefix) == 0;
    } else {
        return type == skillUriType;
    }
}

InnerBundleInfo::InnerBundleInfo()
{
    APP_LOGD("inner bundle info instance is created");
}

InnerBundleInfo::~InnerBundleInfo()
{
    APP_LOGD("inner bundle info instance is destroyed");
}

void to_json(nlohmann::json &jsonObject, const Distro &distro)
{
    jsonObject = nlohmann::json {
            {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DELIVERY_WITH_INSTALL, distro.deliveryWithInstall},
            {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_MODULE_NAME, distro.moduleName},
            {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_MODULE_TYPE, distro.moduleType},
            {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_MODULE_INSTALLATION_FREE, distro.installationFree}
    };
}

void to_json(nlohmann::json &jsonObject, const DefinePermission &definePermission)
{
    jsonObject = nlohmann::json {
        {Profile::DEFINEPERMISSION_NAME, definePermission.name},
        {Profile::DEFINEPERMISSION_GRANT_MODE, definePermission.grantMode},
        {Profile::DEFINEPERMISSION_AVAILABLE_LEVEL, definePermission.availableLevel},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_AVAILABLESCOPE, definePermission.availableScope},
        {Profile::DEFINEPERMISSION_PROVISION_ENABLE, definePermission.provisionEnable},
        {Profile::DEFINEPERMISSION_DISTRIBUTED_SCENE_ENABLE, definePermission.distributedSceneEnable},
        {Profile::LABEL, definePermission.label},
        {Profile::LABEL_ID, definePermission.labelId},
        {Profile::DESCRIPTION, definePermission.description},
        {Profile::DESCRIPTION_ID, definePermission.descriptionId}
    };
}

void to_json(nlohmann::json &jsonObject, const InnerModuleInfo &info)
{
    jsonObject = nlohmann::json {
        {MODULE_PACKAGE, info.modulePackage},
        {MODULE_NAME, info.moduleName},
        {MODULE_PATH, info.modulePath},
        {MODULE_DATA_DIR, info.moduleDataDir},
        {MODULE_RES_PATH, info.moduleResPath},
        {MODULE_IS_ENTRY, info.isEntry},
        {MODULE_METADATA, info.metaData},
        {MODULE_COLOR_MODE, info.colorMode},
        {MODULE_DISTRO, info.distro},
        {MODULE_DESCRIPTION, info.description},
        {MODULE_DESCRIPTION_ID, info.descriptionId},
        {MODULE_LABEL, info.label},
        {MODULE_LABEL_ID, info.labelId},
        {MODULE_DESCRIPTION_INSTALLATION_FREE, info.installationFree},
        {MODULE_IS_REMOVABLE, info.isRemovable},
        {MODULE_UPGRADE_FLAG, info.upgradeFlag},
        {MODULE_REQ_CAPABILITIES, info.reqCapabilities},
        {MODULE_ABILITY_KEYS, info.abilityKeys},
        {MODULE_SKILL_KEYS, info.skillKeys},
        {MODULE_MAIN_ABILITY, info.mainAbility},
        {MODULE_ENTRY_ABILITY_KEY, info.entryAbilityKey},
        {MODULE_SRC_PATH, info.srcPath},
        {MODULE_HASH_VALUE, info.hashValue},
        {MODULE_PROCESS, info.process},
        {MODULE_SRC_ENTRANCE, info.srcEntrance},
        {MODULE_DEVICE_TYPES, info.deviceTypes},
        {MODULE_VIRTUAL_MACHINE, info.virtualMachine},
        {MODULE_UI_SYNTAX, info.uiSyntax},
        {MODULE_PAGES, info.pages},
        {MODULE_META_DATA, info.metadata},
        {MODULE_REQUEST_PERMISSIONS, info.requestPermissions},
        {MODULE_DEFINE_PERMISSIONS, info.definePermissions},
        {MODULE_DEF_PERMS, info.defPermissions},
        {MODULE_EXTENSION_KEYS, info.extensionKeys},
        {MODULE_EXTENSION_SKILL_KEYS, info.extensionSkillKeys},
        {MODULE_IS_MODULE_JSON, info.isModuleJson},
        {MODULE_IS_STAGE_BASED_MODEL, info.isStageBasedModel},
        {MODULE_DEPENDENCIES, info.dependencies},
        {MODULE_HAP_PATH, info.hapPath}
    };
}

void to_json(nlohmann::json &jsonObject, const SkillUri &uri)
{
    jsonObject = nlohmann::json {
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_SCHEME, uri.scheme},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_HOST, uri.host},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_PORT, uri.port},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_PATH, uri.path},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_PATHSTARTWITH, uri.pathStartWith},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_PATHREGEX, uri.pathRegex},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_TYPE, uri.type}
    };
}

void to_json(nlohmann::json &jsonObject, const Skill &skill)
{
    jsonObject = nlohmann::json {
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_ACTIONS, skill.actions},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_ENTITIES, skill.entities},
        {ProfileReader::BUNDLE_MODULE_PROFILE_KEY_URIS, skill.uris}
    };
}

void to_json(nlohmann::json &jsonObject, const InstallMark &installMark)
{
    jsonObject = nlohmann::json {
        {ProfileReader::BUNDLE_INSTALL_MARK_BUNDLE, installMark.bundleName},
        {ProfileReader::BUNDLE_INSTALL_MARK_PACKAGE, installMark.packageName},
        {ProfileReader::BUNDLE_INSTALL_MARK_STATUS, installMark.status}
    };
}

void to_json(nlohmann::json &jsonObject, const SandboxAppPersistentInfo &sandboxPersistentInfo)
{
    jsonObject = nlohmann::json {
        {ProfileReader::BUNDLE_SANDBOX_PERSISTENT_ACCESS_TOKEN_ID, sandboxPersistentInfo.accessTokenId},
        {ProfileReader::BUNDLE_SANDBOX_PERSISTENT_APP_INDEX, sandboxPersistentInfo.appIndex},
        {ProfileReader::BUNDLE_SANDBOX_PERSISTENT_USER_ID, sandboxPersistentInfo.userId}
    };
}

void InnerBundleInfo::ToJson(nlohmann::json &jsonObject) const
{
    jsonObject[APP_TYPE] = appType_;
    jsonObject[BASE_DATA_DIR] = baseDataDir_;
    jsonObject[BUNDLE_STATUS] = bundleStatus_;
    jsonObject[ALLOWED_ACLS] = allowedAcls_;
    jsonObject[BASE_APPLICATION_INFO] = baseApplicationInfo_;
    jsonObject[BASE_BUNDLE_INFO] = baseBundleInfo_;
    jsonObject[BASE_ABILITY_INFO] = baseAbilityInfos_;
    jsonObject[INNER_MODULE_INFO] = innerModuleInfos_;
    jsonObject[SKILL_INFOS] = skillInfos_;
    jsonObject[USER_ID] = userId_;
    jsonObject[APP_FEATURE] = appFeature_;
    jsonObject[MODULE_FORMS] = formInfos_;
    jsonObject[MODULE_SHORTCUT] = shortcutInfos_;
    jsonObject[NEW_BUNDLE_NAME] = newBundleName_;
    jsonObject[MODULE_COMMON_EVENT] = commonEvents_;
    jsonObject[INSTALL_MARK] = mark_;
    jsonObject[INNER_BUNDLE_USER_INFOS] = innerBundleUserInfos_;
    jsonObject[BUNDLE_IS_NEW_VERSION] = isNewVersion_;
    jsonObject[BUNDLE_IS_NEED_UPDATE] = upgradeFlag_;
    jsonObject[BUNDLE_BASE_EXTENSION_INFOS] = baseExtensionInfos_;
    jsonObject[BUNDLE_EXTENSION_SKILL_INFOS] = extensionSkillInfos_;
    jsonObject[BUNDLE_PACK_INFO] = bundlePackInfo_;
    jsonObject[APP_INDEX] = appIndex_;
    jsonObject[BUNDLE_IS_SANDBOX_APP] = isSandboxApp_;
    jsonObject[BUNDLE_SANDBOX_PERSISTENT_INFO] = sandboxPersistentInfo_;
}

void from_json(const nlohmann::json &jsonObject, InnerModuleInfo &info)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_PACKAGE,
        info.modulePackage,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_NAME,
        info.moduleName,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_PATH,
        info.modulePath,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_DATA_DIR,
        info.moduleDataDir,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_HAP_PATH,
        info.hapPath,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_RES_PATH,
        info.moduleResPath,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        MODULE_IS_ENTRY,
        info.isEntry,
        JsonType::BOOLEAN,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<MetaData>(jsonObject,
        jsonObjectEnd,
        MODULE_METADATA,
        info.metaData,
        JsonType::OBJECT,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<ModuleColorMode>(jsonObject,
        jsonObjectEnd,
        MODULE_COLOR_MODE,
        info.colorMode,
        JsonType::NUMBER,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Distro>(jsonObject,
        jsonObjectEnd,
        MODULE_DISTRO,
        info.distro,
        JsonType::OBJECT,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_DESCRIPTION,
        info.description,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        MODULE_DESCRIPTION_ID,
        info.descriptionId,
        JsonType::NUMBER,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_LABEL,
        info.label,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        MODULE_LABEL_ID,
        info.labelId,
        JsonType::NUMBER,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_MAIN_ABILITY,
        info.mainAbility,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_ENTRY_ABILITY_KEY,
        info.entryAbilityKey,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_SRC_PATH,
        info.srcPath,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_HASH_VALUE,
        info.hashValue,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        MODULE_DESCRIPTION_INSTALLATION_FREE,
        info.installationFree,
        JsonType::BOOLEAN,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::map<std::string, bool>>(jsonObject,
        jsonObjectEnd,
        MODULE_IS_REMOVABLE,
        info.isRemovable,
        JsonType::OBJECT,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        MODULE_UPGRADE_FLAG,
        info.upgradeFlag,
        JsonType::NUMBER,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        MODULE_REQ_CAPABILITIES,
        info.reqCapabilities,
        JsonType::ARRAY,
        false,
        ProfileReader::parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        MODULE_ABILITY_KEYS,
        info.abilityKeys,
        JsonType::ARRAY,
        false,
        ProfileReader::parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        MODULE_SKILL_KEYS,
        info.skillKeys,
        JsonType::ARRAY,
        false,
        ProfileReader::parseResult,
        ArrayType::STRING);
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_PROCESS,
        info.process,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_SRC_ENTRANCE,
        info.srcEntrance,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        MODULE_DEVICE_TYPES,
        info.deviceTypes,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_VIRTUAL_MACHINE,
        info.virtualMachine,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_UI_SYNTAX,
        info.uiSyntax,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_PAGES,
        info.pages,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<Metadata>>(jsonObject,
        jsonObjectEnd,
        MODULE_META_DATA,
        info.metadata,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<RequestPermission>>(jsonObject,
        jsonObjectEnd,
        MODULE_REQUEST_PERMISSIONS,
        info.requestPermissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<DefinePermission>>(jsonObject,
        jsonObjectEnd,
        MODULE_DEFINE_PERMISSIONS,
        info.definePermissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<DefinePermission>>(jsonObject,
        jsonObjectEnd,
        MODULE_DEF_PERMS,
        info.defPermissions,
        JsonType::ARRAY,
        false,
        ProfileReader::parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        MODULE_EXTENSION_KEYS,
        info.extensionKeys,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        MODULE_EXTENSION_SKILL_KEYS,
        info.extensionSkillKeys,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        MODULE_IS_MODULE_JSON,
        info.isModuleJson,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        MODULE_IS_STAGE_BASED_MODEL,
        info.isStageBasedModel,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        MODULE_DEPENDENCIES,
        info.dependencies,
        JsonType::ARRAY,
        false,
        ProfileReader::parseResult,
        ArrayType::STRING);
    if (parseResult != ERR_OK) {
        APP_LOGE("read InnerModuleInfo from database error, error code : %{public}d", parseResult);
    }
}

void from_json(const nlohmann::json &jsonObject, SkillUri &uri)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_MODULE_PROFILE_KEY_SCHEME,
        uri.scheme,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    // these are not required fields.
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_MODULE_PROFILE_KEY_HOST,
        uri.host,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_MODULE_PROFILE_KEY_PORT,
        uri.port,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_MODULE_PROFILE_KEY_PATH,
        uri.path,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_MODULE_PROFILE_KEY_PATHSTARTWITH,
        uri.pathStartWith,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_MODULE_PROFILE_KEY_PATHREGX,
        uri.pathRegex,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_MODULE_PROFILE_KEY_PATHREGEX,
        uri.pathRegex,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_MODULE_PROFILE_KEY_TYPE,
        uri.type,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, Skill &skill)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_MODULE_PROFILE_KEY_ACTIONS,
        skill.actions,
        JsonType::ARRAY,
        false,
        ProfileReader::parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_MODULE_PROFILE_KEY_ENTITIES,
        skill.entities,
        JsonType::ARRAY,
        false,
        ProfileReader::parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<SkillUri>>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_MODULE_PROFILE_KEY_URIS,
        skill.uris,
        JsonType::ARRAY,
        false,
        ProfileReader::parseResult,
        ArrayType::OBJECT);
}

void from_json(const nlohmann::json &jsonObject, Distro &distro)
{
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DELIVERY_WITH_INSTALL,
        distro.deliveryWithInstall,
        JsonType::BOOLEAN,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_MODULE_PROFILE_KEY_MODULE_NAME,
        distro.moduleName,
        JsonType::STRING,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_MODULE_PROFILE_KEY_MODULE_TYPE,
        distro.moduleType,
        JsonType::STRING,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    // mustFlag decide by distro.moduleType
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_MODULE_PROFILE_KEY_MODULE_INSTALLATION_FREE,
        distro.installationFree,
        JsonType::BOOLEAN,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, InstallMark &installMark)
{
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_INSTALL_MARK_BUNDLE,
        installMark.bundleName,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_INSTALL_MARK_PACKAGE,
        installMark.packageName,
        JsonType::STRING,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_INSTALL_MARK_STATUS,
        installMark.status,
        JsonType::NUMBER,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, SandboxAppPersistentInfo &sandboxPersistentInfo)
{
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<uint32_t>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_SANDBOX_PERSISTENT_ACCESS_TOKEN_ID,
        sandboxPersistentInfo.accessTokenId,
        JsonType::NUMBER,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_SANDBOX_PERSISTENT_APP_INDEX,
        sandboxPersistentInfo.appIndex,
        JsonType::NUMBER,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_SANDBOX_PERSISTENT_USER_ID,
        sandboxPersistentInfo.userId,
        JsonType::NUMBER,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, DefinePermission &definePermission)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        Profile::DEFINEPERMISSION_NAME,
        definePermission.name,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        Profile::DEFINEPERMISSION_GRANT_MODE,
        definePermission.grantMode,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        Profile::DEFINEPERMISSION_AVAILABLE_LEVEL,
        definePermission.availableLevel,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_AVAILABLESCOPE,
        definePermission.availableScope,
        JsonType::ARRAY,
        false,
        ProfileReader::parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        Profile::DEFINEPERMISSION_PROVISION_ENABLE,
        definePermission.provisionEnable,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        Profile::DEFINEPERMISSION_DISTRIBUTED_SCENE_ENABLE,
        definePermission.distributedSceneEnable,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        Profile::LABEL,
        definePermission.label,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        Profile::LABEL_ID,
        definePermission.labelId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        Profile::DESCRIPTION,
        definePermission.description,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        Profile::DESCRIPTION_ID,
        definePermission.descriptionId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    if (parseResult != ERR_OK) {
        APP_LOGE("read DefinePermission from database error, error code : %{public}d", parseResult);
    }
}

int32_t InnerBundleInfo::FromJson(const nlohmann::json &jsonObject)
{
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<Constants::AppType>(jsonObject,
        jsonObjectEnd,
        APP_TYPE,
        appType_,
        JsonType::NUMBER,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int>(jsonObject,
        jsonObjectEnd,
        UID,
        uid_,
        JsonType::NUMBER,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int>(jsonObject,
        jsonObjectEnd,
        GID,
        gid_,
        JsonType::NUMBER,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        ALLOWED_ACLS,
        allowedAcls_,
        JsonType::ARRAY,
        false,
        ProfileReader::parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BASE_DATA_DIR,
        baseDataDir_,
        JsonType::STRING,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<BundleStatus>(jsonObject,
        jsonObjectEnd,
        BUNDLE_STATUS,
        bundleStatus_,
        JsonType::NUMBER,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<BundleInfo>(jsonObject,
        jsonObjectEnd,
        BASE_BUNDLE_INFO,
        baseBundleInfo_,
        JsonType::OBJECT,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<ApplicationInfo>(jsonObject,
        jsonObjectEnd,
        BASE_APPLICATION_INFO,
        baseApplicationInfo_,
        JsonType::OBJECT,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::map<std::string, AbilityInfo>>(jsonObject,
        jsonObjectEnd,
        BASE_ABILITY_INFO,
        baseAbilityInfos_,
        JsonType::OBJECT,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::map<std::string, InnerModuleInfo>>(jsonObject,
        jsonObjectEnd,
        INNER_MODULE_INFO,
        innerModuleInfos_,
        JsonType::OBJECT,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::map<std::string, std::vector<Skill>>>(jsonObject,
        jsonObjectEnd,
        SKILL_INFOS,
        skillInfos_,
        JsonType::OBJECT,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int>(jsonObject,
        jsonObjectEnd,
        USER_ID,
        userId_,
        JsonType::NUMBER,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APP_FEATURE,
        appFeature_,
        JsonType::STRING,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::map<std::string, std::vector<FormInfo>>>(jsonObject,
        jsonObjectEnd,
        MODULE_FORMS,
        formInfos_,
        JsonType::OBJECT,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::map<std::string, ShortcutInfo>>(jsonObject,
        jsonObjectEnd,
        MODULE_SHORTCUT,
        shortcutInfos_,
        JsonType::OBJECT,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::map<std::string, CommonEventInfo>>(jsonObject,
        jsonObjectEnd,
        MODULE_COMMON_EVENT,
        commonEvents_,
        JsonType::OBJECT,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        NEW_BUNDLE_NAME,
        newBundleName_,
        JsonType::STRING,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<InstallMark>(jsonObject,
        jsonObjectEnd,
        INSTALL_MARK,
        mark_,
        JsonType::OBJECT,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    int32_t isOldVersion = ERR_OK;
    GetValueIfFindKey<std::map<std::string, InnerBundleUserInfo>>(jsonObject,
        jsonObjectEnd,
        INNER_BUNDLE_USER_INFOS,
        innerBundleUserInfos_,
        JsonType::OBJECT,
        true,
        isOldVersion,
        ArrayType::NOT_ARRAY);
    int32_t ret = ProfileReader::parseResult;
    // need recover parse result to ERR_OK
    ProfileReader::parseResult = ERR_OK;
    if (ret == ERR_OK && isOldVersion == ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP) {
        // To be compatible with the old database,
        // if the old data does not have bundleUserInfos,
        // the default user information needs to be constructed.
        BuildDefaultUserInfo();
    }
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_IS_NEW_VERSION,
        isNewVersion_,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_IS_NEED_UPDATE,
        upgradeFlag_,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::map<std::string, ExtensionAbilityInfo>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_BASE_EXTENSION_INFOS,
        baseExtensionInfos_,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::map<std::string, std::vector<Skill>>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_EXTENSION_SKILL_INFOS,
        extensionSkillInfos_,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<BundlePackInfo>(jsonObject,
        jsonObjectEnd,
        BUNDLE_PACK_INFO,
        bundlePackInfo_,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int>(jsonObject,
        jsonObjectEnd,
        APP_INDEX,
        appIndex_,
        JsonType::NUMBER,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_IS_SANDBOX_APP,
        isSandboxApp_,
        JsonType::BOOLEAN,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<SandboxAppPersistentInfo>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_SANDBOX_PERSISTENT_INFO,
        sandboxPersistentInfo_,
        JsonType::ARRAY,
        false,
        ProfileReader::parseResult,
        ArrayType::OBJECT);
    if (parseResult != ERR_OK) {
        APP_LOGE("read InnerBundleInfo from database error, error code : %{public}d", parseResult);
        return parseResult;
    }
    return ret;
}

void InnerBundleInfo::BuildDefaultUserInfo()
{
    APP_LOGD("BuildDefaultUserInfo: bundleName: %{public}s.",
        baseApplicationInfo_.bundleName.c_str());
    InnerBundleUserInfo defaultInnerBundleUserInfo;
    defaultInnerBundleUserInfo.bundleUserInfo.userId = GetUserId();
    defaultInnerBundleUserInfo.uid = uid_;
    defaultInnerBundleUserInfo.gids.emplace_back(gid_);
    defaultInnerBundleUserInfo.installTime = baseBundleInfo_.installTime;
    defaultInnerBundleUserInfo.updateTime = baseBundleInfo_.updateTime;
    defaultInnerBundleUserInfo.bundleName = baseApplicationInfo_.bundleName;
    defaultInnerBundleUserInfo.bundleUserInfo.enabled = baseApplicationInfo_.enabled;
    AddInnerBundleUserInfo(defaultInnerBundleUserInfo);
}

std::optional<HapModuleInfo> InnerBundleInfo::FindHapModuleInfo(const std::string &modulePackage, int32_t userId) const
{
    auto it = innerModuleInfos_.find(modulePackage);
    if (it == innerModuleInfos_.end()) {
        APP_LOGE("can not find module %{public}s", modulePackage.c_str());
        return std::nullopt;
    }
    HapModuleInfo hapInfo;
    hapInfo.name = it->second.modulePackage;
    hapInfo.moduleName = it->second.moduleName;
    hapInfo.description = it->second.description;
    hapInfo.descriptionId = it->second.descriptionId;
    hapInfo.label = it->second.label;
    hapInfo.labelId = it->second.labelId;
    hapInfo.mainAbility = it->second.mainAbility;
    hapInfo.srcPath = it->second.srcPath;
    hapInfo.hapPath = it->second.hapPath;
    hapInfo.supportedModes = baseApplicationInfo_.supportedModes;
    hapInfo.reqCapabilities = it->second.reqCapabilities;
    hapInfo.colorMode = it->second.colorMode;
    hapInfo.isRemovable = it->second.isRemovable;
    hapInfo.upgradeFlag = it->second.upgradeFlag;

    hapInfo.bundleName = baseApplicationInfo_.bundleName;
    hapInfo.mainElementName = it->second.mainAbility;
    hapInfo.pages = it->second.pages;
    hapInfo.process = it->second.process;
    hapInfo.resourcePath = it->second.moduleResPath;
    hapInfo.srcEntrance = it->second.srcEntrance;
    hapInfo.uiSyntax = it->second.uiSyntax;
    hapInfo.virtualMachine = it->second.virtualMachine;
    hapInfo.deliveryWithInstall = it->second.distro.deliveryWithInstall;
    hapInfo.installationFree = it->second.distro.installationFree;
    hapInfo.isModuleJson = it->second.isModuleJson;
    hapInfo.isStageBasedModel = it->second.isStageBasedModel;
    std::string moduleType = it->second.distro.moduleType;
    if (moduleType == Profile::MODULE_TYPE_ENTRY) {
        hapInfo.moduleType = ModuleType::ENTRY;
    } else if (moduleType == Profile::MODULE_TYPE_FEATURE) {
        hapInfo.moduleType = ModuleType::FEATURE;
    } else if (moduleType == Profile::MODULE_TYPE_HAR) {
        hapInfo.moduleType = ModuleType::HAR;
    } else {
        hapInfo.moduleType = ModuleType::UNKNOWN;
    }
    std::string key;
    key.append(".").append(modulePackage).append(".");
    for (const auto &extension : baseExtensionInfos_) {
        if (extension.first.find(key) != std::string::npos) {
            hapInfo.extensionInfos.emplace_back(extension.second);
        }
    }
    hapInfo.metadata = it->second.metadata;
    bool first = false;
    for (auto &ability : baseAbilityInfos_) {
        if (ability.first.find(key) != std::string::npos) {
            if (!first) {
                hapInfo.iconPath = ability.second.iconPath;
                hapInfo.deviceTypes = ability.second.deviceTypes;
                first = true;
            }
            auto &abilityInfo = hapInfo.abilityInfos.emplace_back(ability.second);
            GetApplicationInfo(
                ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, userId, abilityInfo.applicationInfo);
        }
    }
    hapInfo.dependencies = it->second.dependencies;
    return hapInfo;
}

std::optional<AbilityInfo> InnerBundleInfo::FindAbilityInfo(
    const std::string &bundleName, const std::string &moduleName,
    const std::string &abilityName, int32_t userId) const
{
    for (const auto &ability : baseAbilityInfos_) {
        auto abilityInfo = ability.second;
        if ((abilityInfo.bundleName == bundleName) && (abilityInfo.name == abilityName) &&
            (moduleName.empty() || (abilityInfo.moduleName == moduleName))) {
            GetApplicationInfo(
                ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, userId, abilityInfo.applicationInfo);
            return abilityInfo;
        }
    }
    return std::nullopt;
}

std::optional<std::vector<AbilityInfo>> InnerBundleInfo::FindAbilityInfos(
    const std::string &bundleName, int32_t userId) const
{
    std::vector<AbilityInfo> abilitys;

    if (bundleName.empty()) {
        return std::nullopt;
    }

    for (const auto &ability : baseAbilityInfos_) {
        auto abilityInfo = ability.second;
        if ((abilityInfo.bundleName == bundleName)) {
            GetApplicationInfo(
                ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, userId, abilityInfo.applicationInfo);
            abilitys.emplace_back(abilityInfo);
        }
    }
    if (!abilitys.empty()) {
        return abilitys;
    }

    return std::nullopt;
}

std::optional<ExtensionAbilityInfo> InnerBundleInfo::FindExtensionInfo(
    const std::string &bundleName, const std::string &moduleName, const std::string &extensionName) const
{
    for (const auto &extension : baseExtensionInfos_) {
        if ((extension.second.bundleName == bundleName) && (extension.second.name == extensionName) &&
            (moduleName.empty() || (extension.second.moduleName == moduleName))) {
            return extension.second;
        }
    }
    return std::nullopt;
}

std::optional<std::vector<ExtensionAbilityInfo>> InnerBundleInfo::FindExtensionInfos(
    const std::string &bundleName) const
{
    std::vector<ExtensionAbilityInfo> extensions;

    if (bundleName.empty()) {
        return std::nullopt;
    }

    for (const auto &extension : baseExtensionInfos_) {
        if ((extension.second.bundleName == bundleName)) {
            extensions.emplace_back(extension.second);
        }
    }
    if (!extensions.empty()) {
        return extensions;
    }

    return std::nullopt;
}

void InnerBundleInfo::FindAbilityInfosForClone(const std::string &bundleName,
    const std::string &abilityName, int32_t userId, std::vector<AbilityInfo> &abilitys)
{
    if (bundleName.empty()) {
        return;
    }

    for (auto &ability : baseAbilityInfos_) {
        APP_LOGE("FindAbilityInfosForClonekey = %{public}s", ability.first.c_str());
        auto abilityInfo = ability.second;
        if ((abilityInfo.bundleName == bundleName && (abilityInfo.name == abilityName))) {
            GetApplicationInfo(
                ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, userId, abilityInfo.applicationInfo);
            abilitys.emplace_back(abilityInfo);
        }
    }
    return;
}

bool InnerBundleInfo::AddModuleInfo(const InnerBundleInfo &newInfo)
{
    if (newInfo.currentPackage_.empty()) {
        APP_LOGE("current package is empty");
        return false;
    }
    if (FindModule(newInfo.currentPackage_)) {
        APP_LOGE("current package %{public}s is exist", currentPackage_.c_str());
        return false;
    }
    AddInnerModuleInfo(newInfo.innerModuleInfos_);
    AddModuleAbilityInfo(newInfo.baseAbilityInfos_);
    AddModuleSkillInfo(newInfo.skillInfos_);
    AddModuleExtensionInfos(newInfo.baseExtensionInfos_);
    AddModuleExtensionSkillInfos(newInfo.extensionSkillInfos_);
    AddModuleFormInfo(newInfo.formInfos_);
    AddModuleShortcutInfo(newInfo.shortcutInfos_);
    AddModuleCommonEvent(newInfo.commonEvents_);
    return true;
}

void InnerBundleInfo::UpdateBaseBundleInfo(const BundleInfo &bundleInfo, bool isEntry)
{
    baseBundleInfo_.name = bundleInfo.name;

    baseBundleInfo_.versionCode = bundleInfo.versionCode;
    baseBundleInfo_.versionName = bundleInfo.versionName;
    baseBundleInfo_.minCompatibleVersionCode = bundleInfo.minCompatibleVersionCode;

    baseBundleInfo_.compatibleVersion = bundleInfo.compatibleVersion;
    baseBundleInfo_.targetVersion = bundleInfo.targetVersion;

    baseBundleInfo_.isKeepAlive = bundleInfo.isKeepAlive;
    baseBundleInfo_.singleton = bundleInfo.singleton;
    baseBundleInfo_.isPreInstallApp = bundleInfo.isPreInstallApp;

    baseBundleInfo_.vendor = bundleInfo.vendor;
    baseBundleInfo_.releaseType = bundleInfo.releaseType;
    if (!baseBundleInfo_.isNativeApp) {
        baseBundleInfo_.isNativeApp = bundleInfo.isNativeApp;
    }

    if (isEntry) {
        baseBundleInfo_.mainEntry = bundleInfo.mainEntry;
        baseBundleInfo_.entryModuleName = bundleInfo.entryModuleName;
    }
}

void InnerBundleInfo::UpdateBaseApplicationInfo(const ApplicationInfo &applicationInfo)
{
    baseApplicationInfo_.name = applicationInfo.name;
    baseApplicationInfo_.bundleName = applicationInfo.bundleName;

    baseApplicationInfo_.versionCode = applicationInfo.versionCode;
    baseApplicationInfo_.versionName = applicationInfo.versionName;
    baseApplicationInfo_.minCompatibleVersionCode = applicationInfo.minCompatibleVersionCode;

    baseApplicationInfo_.apiCompatibleVersion = applicationInfo.apiCompatibleVersion;
    baseApplicationInfo_.apiTargetVersion = applicationInfo.apiTargetVersion;

    baseApplicationInfo_.iconPath = applicationInfo.iconPath;
    baseApplicationInfo_.iconId = applicationInfo.iconId;
    baseApplicationInfo_.label = applicationInfo.label;
    baseApplicationInfo_.labelId = applicationInfo.labelId;
    baseApplicationInfo_.description = applicationInfo.description;
    baseApplicationInfo_.descriptionId = applicationInfo.descriptionId;

    baseApplicationInfo_.keepAlive = applicationInfo.keepAlive;
    baseApplicationInfo_.removable = applicationInfo.removable;
    baseApplicationInfo_.singleton = applicationInfo.singleton;
    baseApplicationInfo_.userDataClearable = applicationInfo.userDataClearable;
    baseApplicationInfo_.accessible = applicationInfo.accessible;

    if (!baseApplicationInfo_.isSystemApp) {
        baseApplicationInfo_.isSystemApp = applicationInfo.isSystemApp;
    }
    if (!baseApplicationInfo_.isLauncherApp) {
        baseApplicationInfo_.isLauncherApp = applicationInfo.isLauncherApp;
    }

    baseApplicationInfo_.apiReleaseType = applicationInfo.apiReleaseType;
    baseApplicationInfo_.debug = applicationInfo.debug;
    baseApplicationInfo_.deviceId = applicationInfo.deviceId;
    baseApplicationInfo_.distributedNotificationEnabled = applicationInfo.distributedNotificationEnabled;
    baseApplicationInfo_.entityType = applicationInfo.entityType;
    baseApplicationInfo_.process = applicationInfo.process;
    baseApplicationInfo_.supportedModes = applicationInfo.supportedModes;
    baseApplicationInfo_.vendor = applicationInfo.vendor;

    if (baseApplicationInfo_.nativeLibraryPath.empty()) {
        baseApplicationInfo_.nativeLibraryPath = applicationInfo.nativeLibraryPath;
        baseApplicationInfo_.cpuAbi = applicationInfo.cpuAbi;
    }
}

void InnerBundleInfo::UpdateModuleInfo(const InnerBundleInfo &newInfo)
{
    if (newInfo.currentPackage_.empty()) {
        APP_LOGE("no package in new info");
        return;
    }

    RemoveModuleInfo(newInfo.currentPackage_);
    AddInnerModuleInfo(newInfo.innerModuleInfos_);
    AddModuleAbilityInfo(newInfo.baseAbilityInfos_);
    AddModuleSkillInfo(newInfo.skillInfos_);
    AddModuleExtensionInfos(newInfo.baseExtensionInfos_);
    AddModuleExtensionSkillInfos(newInfo.extensionSkillInfos_);
    AddModuleFormInfo(newInfo.formInfos_);
    AddModuleShortcutInfo(newInfo.shortcutInfos_);
    AddModuleCommonEvent(newInfo.commonEvents_);
}

void InnerBundleInfo::RemoveModuleInfo(const std::string &modulePackage)
{
    auto it = innerModuleInfos_.find(modulePackage);
    if (it == innerModuleInfos_.end()) {
        APP_LOGE("The module(%{public}s) infomation does not exist", modulePackage.c_str());
        return;
    }

    auto oldModuleInfo = it->second;
    if (oldModuleInfo.isEntry) {
        baseBundleInfo_.mainEntry.clear();
        baseBundleInfo_.entryModuleName.clear();
    }
    innerModuleInfos_.erase(it);
    std::string key;
    key.append(".").append(modulePackage).append(".");
    for (auto it = shortcutInfos_.begin(); it != shortcutInfos_.end();) {
        if (it->first.find(key) != std::string::npos) {
            shortcutInfos_.erase(it++);
        } else {
            ++it;
        }
    }

    for (auto it = commonEvents_.begin(); it != commonEvents_.end();) {
        if (it->first.find(key) != std::string::npos) {
            commonEvents_.erase(it++);
        } else {
            ++it;
        }
    }

    // delete old abilityInfos
    for (auto abilityKey : oldModuleInfo.abilityKeys) {
        auto abilityItem = baseAbilityInfos_.find(abilityKey);
        if (abilityItem == baseAbilityInfos_.end()) {
            continue;
        }

        baseAbilityInfos_.erase(abilityItem);
        formInfos_.erase(abilityKey);
    }

    // delete old skillInfos
    for (auto skillKey : oldModuleInfo.skillKeys) {
        auto skillItem = skillInfos_.find(skillKey);
        if (skillItem == skillInfos_.end()) {
            continue;
        }

        skillInfos_.erase(skillItem);
    }

    // delete old extensionInfos
    for (auto extensionKey : oldModuleInfo.extensionKeys) {
        auto extensionItem = baseExtensionInfos_.find(extensionKey);
        if (extensionItem == baseExtensionInfos_.end()) {
            continue;
        }

        baseExtensionInfos_.erase(extensionItem);
    }

    // delete old extensionSkillInfos
    for (auto extensionSkillKey : oldModuleInfo.extensionSkillKeys) {
        auto extensionSkillItem = extensionSkillInfos_.find(extensionSkillKey);
        if (extensionSkillItem == extensionSkillInfos_.end()) {
            continue;
        }

        extensionSkillInfos_.erase(extensionSkillItem);
    }
}

std::string InnerBundleInfo::ToString() const
{
    nlohmann::json j;
    ToJson(j);
    return j.dump();
}

void InnerBundleInfo::GetApplicationInfo(int32_t flags, int32_t userId, ApplicationInfo &appInfo) const
{
    InnerBundleUserInfo innerBundleUserInfo;
    if (!GetInnerBundleUserInfo(userId, innerBundleUserInfo)) {
        APP_LOGE("can not find userId %{public}d when get applicationInfo", userId);
        return;
    }

    appInfo = baseApplicationInfo_;

    appInfo.accessTokenId = innerBundleUserInfo.accessTokenId;
    appInfo.enabled = innerBundleUserInfo.bundleUserInfo.enabled;
    appInfo.uid = innerBundleUserInfo.uid;

    for (const auto &info : innerModuleInfos_) {
        ModuleInfo moduleInfo;
        moduleInfo.moduleName = info.second.moduleName;
        moduleInfo.moduleSourceDir = info.second.modulePath;
        appInfo.moduleInfos.emplace_back(moduleInfo);
        appInfo.moduleSourceDirs.emplace_back(info.second.modulePath);
        if (info.second.isEntry) {
            appInfo.entryDir = info.second.modulePath;
        }
        if ((static_cast<uint32_t>(flags) & GET_APPLICATION_INFO_WITH_PERMISSION) ==
            GET_APPLICATION_INFO_WITH_PERMISSION) {
            std::transform(info.second.requestPermissions.begin(),
                info.second.requestPermissions.end(),
                std::back_inserter(appInfo.permissions),
                [](const auto &p) { return p.name; });
        }
        if ((static_cast<uint32_t>(flags) & GET_APPLICATION_INFO_WITH_METADATA) == GET_APPLICATION_INFO_WITH_METADATA) {
            bool isModuleJson = info.second.isModuleJson;
            if (!isModuleJson && info.second.metaData.customizeData.size() > 0) {
                appInfo.metaData[info.second.moduleName] = info.second.metaData.customizeData;
            }
            if (isModuleJson && info.second.metadata.size() > 0) {
                appInfo.metadata[info.second.moduleName] = info.second.metadata;
            }
        }
    }
    if (!appInfo.permissions.empty()) {
        RemoveDuplicateName(appInfo.permissions);
    }
}

void InnerBundleInfo::GetBundleInfo(int32_t flags, BundleInfo &bundleInfo, int32_t userId) const
{
    InnerBundleUserInfo innerBundleUserInfo;
    if (!GetInnerBundleUserInfo(userId, innerBundleUserInfo)) {
        APP_LOGE("can not find userId %{public}d when GetBundleInfo", userId);
        return;
    }

    bundleInfo = baseBundleInfo_;

    bundleInfo.uid = innerBundleUserInfo.uid;
    if (!innerBundleUserInfo.gids.empty()) {
        bundleInfo.gid = innerBundleUserInfo.gids[0];
    }
    bundleInfo.installTime = innerBundleUserInfo.installTime;
    bundleInfo.updateTime = innerBundleUserInfo.updateTime;

    GetApplicationInfo(ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, bundleInfo.applicationInfo);
    for (const auto &info : innerModuleInfos_) {
        if ((static_cast<uint32_t>(flags) & GET_BUNDLE_WITH_REQUESTED_PERMISSION)
            == GET_BUNDLE_WITH_REQUESTED_PERMISSION) {
            std::transform(info.second.requestPermissions.begin(),
                info.second.requestPermissions.end(),
                std::back_inserter(bundleInfo.reqPermissions),
                [](const auto &p) { return p.name; });
            std::transform(info.second.definePermissions.begin(),
                info.second.definePermissions.end(),
                std::back_inserter(bundleInfo.defPermissions),
                [](const auto &p) { return p.name; });
            std::transform(info.second.defPermissions.begin(),
                info.second.defPermissions.end(),
                std::back_inserter(bundleInfo.defPermissions),
                [](const auto &p) { return p.name; });
        }
        bundleInfo.hapModuleNames.emplace_back(info.second.modulePackage);
        auto hapmoduleinfo = FindHapModuleInfo(info.second.modulePackage, userId);
        if (hapmoduleinfo) {
            GetModuleWithHashValue(flags, info.second.modulePackage, *hapmoduleinfo);
            bundleInfo.hapModuleInfos.emplace_back(*hapmoduleinfo);
            bundleInfo.moduleNames.emplace_back(info.second.moduleName);
            bundleInfo.moduleDirs.emplace_back(info.second.modulePath);
            bundleInfo.modulePublicDirs.emplace_back(info.second.moduleDataDir);
            bundleInfo.moduleResPaths.emplace_back(info.second.moduleResPath);
        } else {
            APP_LOGE("can not find hapmoduleinfo %{public}s", info.second.moduleName.c_str());
        }
    }
    if ((static_cast<uint32_t>(flags) & GET_BUNDLE_WITH_REQUESTED_PERMISSION)
        == GET_BUNDLE_WITH_REQUESTED_PERMISSION) {
        if (!bundleInfo.reqPermissions.empty()) {
            RemoveDuplicateName(bundleInfo.reqPermissions);
        }
        if (!bundleInfo.defPermissions.empty()) {
            RemoveDuplicateName(bundleInfo.defPermissions);
        }
        if (!BundlePermissionMgr::GetRequestPermissionStates(bundleInfo)) {
            APP_LOGE("get request permission state failed");
        }
        bundleInfo.reqPermissionDetails = GetAllRequestPermissions();
    }
    GetBundleWithAbilities(flags, bundleInfo, userId);
    GetBundeleWithExtension(flags, bundleInfo, userId);
}

void InnerBundleInfo::GetModuleWithHashValue(
    int32_t flags, const std::string &modulePackage, HapModuleInfo &hapModuleInfo) const
{
    if (!(static_cast<uint32_t>(flags) & GET_BUNDLE_WITH_HASH_VALUE)) {
        return;
    }

    auto it = innerModuleInfos_.find(modulePackage);
    if (it == innerModuleInfos_.end()) {
        APP_LOGE("can not find module %{public}s", modulePackage.c_str());
        return;
    }

    hapModuleInfo.hashValue = it->second.hashValue;
}

void InnerBundleInfo::GetBundleWithAbilities(int32_t flags, BundleInfo &bundleInfo, int32_t userId) const
{
    APP_LOGD("bundleName:%{public}s userid:%{public}d", bundleInfo.name.c_str(), userId);
    if (static_cast<uint32_t>(flags) & GET_BUNDLE_WITH_ABILITIES) {
        for (auto &ability : baseAbilityInfos_) {
            bool isEnabled = IsAbilityEnabled(ability.second, userId);
            if (!(static_cast<uint32_t>(flags) & GET_ABILITY_INFO_WITH_DISABLE)
                && !isEnabled) {
                APP_LOGW("%{public}s is disabled,", ability.second.name.c_str());
                continue;
            }
            AbilityInfo abilityInfo = ability.second;
            abilityInfo.enabled = isEnabled;
            bundleInfo.abilityInfos.emplace_back(abilityInfo);
        }
    }
}

void InnerBundleInfo::GetBundeleWithExtension(int32_t flags, BundleInfo &bundleInfo, int32_t userId) const
{
    APP_LOGD("get bundleInfo with extensionInfo begin");
    if ((static_cast<uint32_t>(flags) & GET_BUNDLE_WITH_EXTENSION_INFO) == GET_BUNDLE_WITH_EXTENSION_INFO) {
        for (const auto &extensionInfo : baseExtensionInfos_) {
            if (!extensionInfo.second.enabled) {
                continue;
            }
            ExtensionAbilityInfo info = extensionInfo.second;
            bundleInfo.extensionInfos.emplace_back(info);
        }
    }
    APP_LOGD("get bundleInfo with extensionInfo end");
}

bool InnerBundleInfo::CheckSpecialMetaData(const std::string &metaData) const
{
    if (isNewVersion_) {
        for (const auto &moduleInfo : innerModuleInfos_) {
            for (const auto &data : moduleInfo.second.metadata) {
                if (metaData == data.name) {
                    return true;
                }
            }
        }
        return false;
    }
    // old version
    for (const auto &moduleInfo : innerModuleInfos_) {
        for (const auto &data : moduleInfo.second.metaData.customizeData) {
            if (metaData == data.name) {
                return true;
            }
        }
    }
    return false;
}

void InnerBundleInfo::GetFormsInfoByModule(const std::string &moduleName, std::vector<FormInfo> &formInfos) const
{
    for (const auto &data : formInfos_) {
        for (auto &form : data.second) {
            if (form.moduleName == moduleName) {
                formInfos.emplace_back(form);
            }
        }
    }
}

void InnerBundleInfo::GetFormsInfoByApp(std::vector<FormInfo> &formInfos) const
{
    for (const auto &data : formInfos_) {
        for (auto &form : data.second) {
            formInfos.emplace_back(form);
        }
    }
}

void InnerBundleInfo::GetShortcutInfos(std::vector<ShortcutInfo> &shortcutInfos) const
{
    if (isNewVersion_) {
        AbilityInfo abilityInfo;
        GetMainAbilityInfo(abilityInfo);
        if (!abilityInfo.resourcePath.empty() && abilityInfo.metadata.size() > 0) {
            std::vector<std::string> rawJson;
            BundleMgrClient bundleMgrClient;
            bool ret = bundleMgrClient.GetResConfigFile(abilityInfo, META_DATA_SHORTCUTS_NAME, rawJson);
            if (!ret) {
                APP_LOGD("GetResConfigFile return false");
                return;
            }
            if (rawJson.size() == 0) {
                APP_LOGD("rawJson size 0. skip.");
                return;
            }
            nlohmann::json jsonObject = nlohmann::json::parse(rawJson[0], nullptr, false);
            if (jsonObject.is_discarded()) {
                APP_LOGE("shortcuts json invalid");
                return;
            }
            ShortcutJson shortcutJson = jsonObject.get<ShortcutJson>();
            for (const Shortcut &item : shortcutJson.shortcuts) {
                ShortcutInfo shortcutInfo;
                shortcutInfo.id = item.shortcutId;
                shortcutInfo.bundleName = abilityInfo.bundleName;
                shortcutInfo.icon = item.icon;
                shortcutInfo.label = item.label;
                shortcutInfo.iconId = item.iconId;
                shortcutInfo.labelId = item.labelId;
                for (const ShortcutWant &shortcutWant : item.wants) {
                    ShortcutIntent shortcutIntent;
                    shortcutIntent.targetBundle = shortcutWant.bundleName;
                    shortcutIntent.targetClass = shortcutWant.abilityName;
                    shortcutInfo.intents.emplace_back(shortcutIntent);
                }
                shortcutInfos.emplace_back(shortcutInfo);
            }
        }
        return;
    }
    for (const auto &shortcut : shortcutInfos_) {
        shortcutInfos.emplace_back(shortcut.second);
    }
}

void InnerBundleInfo::GetCommonEvents(const std::string &eventKey, std::vector<CommonEventInfo> &commonEvents) const
{
    CommonEventInfo item;
    for (const auto &commonEvent : commonEvents_) {
        for (const auto &event : commonEvent.second.events) {
            if (event == eventKey) {
                item = commonEvent.second;
                item.uid = GetUid(GetUserId());
                commonEvents.emplace_back(item);
                break;
            }
        }
    }
}

std::optional<InnerModuleInfo> InnerBundleInfo::GetInnerModuleInfoByModuleName(const std::string &moduleName) const
{
    for (const auto &innerModuleInfo : innerModuleInfos_) {
        APP_LOGD("info.moduleName = %{public}s, moduleName= %{public}s",
            innerModuleInfo.second.moduleName.c_str(), moduleName.c_str());
        if (innerModuleInfo.second.moduleName == moduleName) {
            return innerModuleInfo.second;
        }
    }
    return std::nullopt;
}

void InnerBundleInfo::GetModuleNames(std::vector<std::string> &moduleNames) const
{
    for (const auto &innerModuleInfo : innerModuleInfos_) {
        moduleNames.emplace_back(innerModuleInfo.second.moduleName);
    }
}

void InnerBundleInfo::ResetBundleState(int32_t userId)
{
    if (userId == Constants::ALL_USERID) {
        for (auto& innerBundleUserInfo : innerBundleUserInfos_) {
            innerBundleUserInfo.second.bundleUserInfo.Reset();
        }

        return;
    }

    auto& key = NameAndUserIdToKey(GetBundleName(), userId);
    if (innerBundleUserInfos_.find(key) == innerBundleUserInfos_.end()) {
        APP_LOGD("no this user %{public}s", key.c_str());
        return;
    }

    innerBundleUserInfos_.at(key).bundleUserInfo.Reset();
}

void InnerBundleInfo::RemoveInnerBundleUserInfo(int32_t userId)
{
    auto& key = NameAndUserIdToKey(GetBundleName(), userId);
    auto infoItem = innerBundleUserInfos_.find(key);
    if (infoItem == innerBundleUserInfos_.end()) {
        return;
    }

    auto result = innerBundleUserInfos_.erase(key);
    if (result == 0) {
        APP_LOGE("remove userId:%{public}d key:%{public}s info failed", userId, key.c_str());
    }
    for (auto &innerModuleInfo : innerModuleInfos_) {
        DeleteModuleRemovable(innerModuleInfo.second.moduleName, userId);
    }
}

void InnerBundleInfo::AddInnerBundleUserInfo(
    const InnerBundleUserInfo& innerBundleUserInfo)
{
    auto& key = NameAndUserIdToKey(
        GetBundleName(), innerBundleUserInfo.bundleUserInfo.userId);
    auto infoItem = innerBundleUserInfos_.find(key);
    if (infoItem == innerBundleUserInfos_.end()) {
        innerBundleUserInfos_.emplace(key, innerBundleUserInfo);
        return;
    }

    innerBundleUserInfos_[key] = innerBundleUserInfo;
}

bool InnerBundleInfo::GetInnerBundleUserInfo(
    int32_t userId, InnerBundleUserInfo& innerBundleUserInfo) const
{
    if (userId == Constants::NOT_EXIST_USERID) {
        return true;
    }

    if (userId == Constants::ALL_USERID) {
        if (innerBundleUserInfos_.empty()) {
            return false;
        }

        innerBundleUserInfo = innerBundleUserInfos_.begin()->second;
        return true;
    }

    auto& key = NameAndUserIdToKey(GetBundleName(), userId);
    auto infoItem = innerBundleUserInfos_.find(key);
    if (infoItem == innerBundleUserInfos_.end()) {
        APP_LOGD("no this user %{public}s", key.c_str());
        return false;
    }

    innerBundleUserInfo = infoItem->second;
    return true;
}

bool InnerBundleInfo::HasInnerBundleUserInfo(int32_t userId) const
{
    if (userId == Constants::ALL_USERID) {
        return !innerBundleUserInfos_.empty();
    }

    auto& key = NameAndUserIdToKey(GetBundleName(), userId);
    auto infoItem = innerBundleUserInfos_.find(key);
    return infoItem != innerBundleUserInfos_.end();
}

void InnerBundleInfo::SetBundleInstallTime(const int64_t time, int32_t userId)
{
    auto& key = NameAndUserIdToKey(GetBundleName(), userId);
    auto infoItem = innerBundleUserInfos_.find(key);
    if (infoItem == innerBundleUserInfos_.end()) {
        return;
    }

    infoItem->second.installTime = time;
    infoItem->second.updateTime = time;
}

void InnerBundleInfo::SetAccessTokenId(uint32_t accessToken, const int32_t userId)
{
    auto& key = NameAndUserIdToKey(GetBundleName(), userId);
    auto infoItem = innerBundleUserInfos_.find(key);
    if (infoItem == innerBundleUserInfos_.end()) {
        return;
    }

    infoItem->second.accessTokenId = accessToken;
}

void InnerBundleInfo::SetBundleUpdateTime(const int64_t time, int32_t userId)
{
    auto& key = NameAndUserIdToKey(GetBundleName(), userId);
    auto infoItem = innerBundleUserInfos_.find(key);
    if (infoItem == innerBundleUserInfos_.end()) {
        return;
    }

    infoItem->second.updateTime = time;
}

bool InnerBundleInfo::IsAbilityEnabled(const AbilityInfo &abilityInfo, int32_t userId) const
{
    APP_LOGD("IsAbilityEnabled bundleName:%{public}s, userId:%{public}d", abilityInfo.bundleName.c_str(), userId);
    if (userId == Constants::NOT_EXIST_USERID) {
        return true;
    }
    auto& key = NameAndUserIdToKey(abilityInfo.bundleName, userId);
    auto infoItem = innerBundleUserInfos_.find(key);
    if (infoItem == innerBundleUserInfos_.end()) {
        APP_LOGE("innerBundleUserInfos find key:%{public}s, error", key.c_str());
        return false;
    }
    auto disabledAbilities = infoItem->second.bundleUserInfo.disabledAbilities;
    if (std::find(disabledAbilities.begin(), disabledAbilities.end(), abilityInfo.name) != disabledAbilities.end()) {
        return false;
    } else {
        return true;
    }
}

bool InnerBundleInfo::SetAbilityEnabled(const std::string &bundleName,
                                        const std::string &moduleName,
                                        const std::string &abilityName,
                                        bool isEnabled,
                                        int32_t userId)
{
    APP_LOGD("SetAbilityEnabled :%{public}s, %{public}s, %{public}s, %{public}d",
        bundleName.c_str(), moduleName.c_str(), abilityName.c_str(), userId);
    for (auto &ability : baseAbilityInfos_) {
        if ((ability.second.bundleName == bundleName) && (ability.second.name == abilityName) &&
            (moduleName.empty() || (ability.second.moduleName == moduleName))) {
            auto &key = NameAndUserIdToKey(bundleName, userId);
            auto infoItem = innerBundleUserInfos_.find(key);
            if (infoItem == innerBundleUserInfos_.end()) {
                APP_LOGE("SetAbilityEnabled find innerBundleUserInfo failed");
                return false;
            }
            auto iter = std::find(infoItem->second.bundleUserInfo.disabledAbilities.begin(),
                                  infoItem->second.bundleUserInfo.disabledAbilities.end(),
                                  abilityName);
            if (iter != infoItem->second.bundleUserInfo.disabledAbilities.end()) {
                if (isEnabled) {
                    infoItem->second.bundleUserInfo.disabledAbilities.erase(iter);
                }
            } else {
                if (!isEnabled) {
                    infoItem->second.bundleUserInfo.disabledAbilities.push_back(abilityName);
                }
            }
            return true;
        }
    }
    return false;
}

void InnerBundleInfo::RemoveDuplicateName(std::vector<std::string> &name) const
{
    std::sort(name.begin(), name.end());
    auto iter = std::unique(name.begin(), name.end());
    name.erase(iter, name.end());
}

std::vector<DefinePermission> InnerBundleInfo::GetAllDefinePermissions() const
{
    std::vector<DefinePermission> definePermissions;
    for (const auto &info : innerModuleInfos_) {
        std::transform(info.second.definePermissions.begin(),
            info.second.definePermissions.end(),
            std::back_inserter(definePermissions),
            [](const auto &p) { return p; });
    }
    if (!definePermissions.empty()) {
        std::sort(definePermissions.begin(), definePermissions.end(),
            [](DefinePermission defPermA, DefinePermission defPermB) {
                return defPermA.name < defPermB.name;
            });
        auto iter = std::unique(definePermissions.begin(), definePermissions.end(),
            [](DefinePermission defPermA, DefinePermission defPermB) {
                return defPermA.name == defPermB.name;
            });
        definePermissions.erase(iter, definePermissions.end());
    }
    return definePermissions;
}

std::vector<RequestPermission> InnerBundleInfo::GetAllRequestPermissions() const
{
    std::vector<RequestPermission> requestPermissions;
    for (const auto &info : innerModuleInfos_) {
        std::transform(info.second.requestPermissions.begin(),
            info.second.requestPermissions.end(),
            std::back_inserter(requestPermissions),
            [](const auto &p) { return p; });
    }
    if (!requestPermissions.empty()) {
        std::sort(requestPermissions.begin(), requestPermissions.end(),
            [](RequestPermission reqPermA, RequestPermission reqPermB) {
                return reqPermA.name < reqPermB.name;
            });
        auto iter = std::unique(requestPermissions.begin(), requestPermissions.end(),
            [](RequestPermission reqPermA, RequestPermission reqPermB) {
                return reqPermA.name == reqPermB.name;
            });
        requestPermissions.erase(iter, requestPermissions.end());
    }
    return requestPermissions;
}

void InnerBundleInfo::SetApplicationEnabled(bool enabled, int32_t userId)
{
    auto& key = NameAndUserIdToKey(GetBundleName(), userId);
    auto infoItem = innerBundleUserInfos_.find(key);
    if (infoItem == innerBundleUserInfos_.end()) {
        return;
    }

    infoItem->second.bundleUserInfo.enabled = enabled;
}

bool InnerBundleInfo::IsBundleRemovable(int32_t userId) const
{
    APP_LOGD("userId is %{public}d", userId);
    if (IsPreInstallApp()) {
        APP_LOGE("PreInstallApp should not be cleaned");
        return false;
    }
    for (const auto &innerModuleInfo : innerModuleInfos_) {
        if (!IsModuleRemovable(innerModuleInfo.second.moduleName, userId)) {
            APP_LOGE("not all haps should be cleaned");
            return false;
        }
    }
    APP_LOGD("this bundle should be cleaned");
    return true;
}

bool InnerBundleInfo::IsUserExistModule(const std::string &moduleName, int32_t userId) const
{
    std::string stringUserId = "";
    stringUserId.append(std::to_string(userId));
    APP_LOGD("userId:%{public}d moduleName:%{public}s", userId, moduleName.c_str());
    auto modInfoItem = GetInnerModuleInfoByModuleName(moduleName);
    if (!modInfoItem) {
        APP_LOGE("get InnerModuleInfo by moduleName(%{public}s) failed", moduleName.c_str());
        return false;
    }
    auto item = modInfoItem->isRemovable.find(stringUserId);
    if (item == modInfoItem->isRemovable.end()) {
        APP_LOGE("userId:%{public}d has not moduleName:%{public}s", userId, moduleName.c_str());
        return false;
    }
    APP_LOGD("userId:%{public}d exist moduleName:%{public}s", userId, moduleName.c_str());
    return true;
}

bool InnerBundleInfo::IsModuleRemovable(const std::string &moduleName, int32_t userId) const
{
    std::string stringUserId = "";
    stringUserId.append(std::to_string(userId));
    APP_LOGD("userId:%{public}d moduleName:%{public}s", userId, moduleName.c_str());
    auto modInfoItem = GetInnerModuleInfoByModuleName(moduleName);
    if (!modInfoItem) {
        APP_LOGE("get InnerModuleInfo by moduleName(%{public}s) failed", moduleName.c_str());
        return false;
    }
    auto item = modInfoItem->isRemovable.find(stringUserId);
    if (item == modInfoItem->isRemovable.end()) {
        APP_LOGE("userId:%{public}d has not moduleName:%{public}s", userId, moduleName.c_str());
        return false;
    }
    bool ret = item->second;
    APP_LOGD("userId:%{public}d, moduleName:%{public}s, ret:%{public}d,", userId, moduleName.c_str(), ret);
    return ret;
}

bool InnerBundleInfo::AddModuleRemovableInfo(
    InnerModuleInfo &info, const std::string &stringUserId, bool isEnable) const
{
    auto item = info.isRemovable.find(stringUserId);
    if (item == info.isRemovable.end()) {
        auto result = info.isRemovable.try_emplace(stringUserId, isEnable);
        if (!result.second) {
            APP_LOGE("add userId:%{public}s isRemovable:%{public}d failed", stringUserId.c_str(), isEnable);
            return false;
        } else {
            APP_LOGD("add userId:%{public}s isRemovable:%{public}d into map", stringUserId.c_str(), isEnable);
            return true;
        }
    }
    item->second = isEnable;
    APP_LOGD("set userId:%{public}s isEnable:%{public}d ok", stringUserId.c_str(), isEnable);
    return true;
}

bool InnerBundleInfo::SetModuleRemovable(const std::string &moduleName, bool isEnable, int32_t userId)
{
    std::string stringUserId = "";
    stringUserId.append(std::to_string(userId));
    APP_LOGD("userId:%{public}d moduleName:%{public}s isEnable:%{public}d", userId, moduleName.c_str(), isEnable);
    for (auto &innerModuleInfo : innerModuleInfos_) {
        if (innerModuleInfo.second.moduleName == moduleName) {
            return AddModuleRemovableInfo(innerModuleInfo.second, stringUserId, isEnable);
        }
    }
    return false;
}

void InnerBundleInfo::DeleteModuleRemovableInfo(InnerModuleInfo &info, const std::string &stringUserId)
{
    auto item = info.isRemovable.find(stringUserId);
    if (item != info.isRemovable.end()) {
        auto result = info.isRemovable.erase(stringUserId);
        if (result == 0) {
            APP_LOGE("del userId:%{public}s from map failed", stringUserId.c_str());
            return;
        }
        APP_LOGD("del userId:%{public}s from map", stringUserId.c_str());
        return;
    }
}

void InnerBundleInfo::DeleteModuleRemovable(const std::string &moduleName, int32_t userId)
{
    std::string stringUserId = "";
    stringUserId.append(std::to_string(userId));
    APP_LOGD("userId:%{public}d moduleName:%{public}s", userId, moduleName.c_str());
    for (auto &innerModuleInfo : innerModuleInfos_) {
        if (innerModuleInfo.second.moduleName == moduleName) {
            DeleteModuleRemovableInfo(innerModuleInfo.second, stringUserId);
            return;
        }
    }
}

bool InnerBundleInfo::SetModuleUpgradeFlag(std::string moduleName, int32_t upgradeFlag)
{
    upgradeFlag_ = upgradeFlag;
    return true;
}

int32_t InnerBundleInfo::GetModuleUpgradeFlag(std::string moduleName) const
{
    return upgradeFlag_;
}

int32_t InnerBundleInfo::GetResponseUserId(int32_t requestUserId) const
{
    if (innerBundleUserInfos_.empty()) {
        APP_LOGE("Get responseUserId failed due to user map is empty.");
        return Constants::INVALID_USERID;
    }

    if (requestUserId == Constants::ANY_USERID) {
        return innerBundleUserInfos_.begin()->second.bundleUserInfo.userId;
    }

    if (HasInnerBundleUserInfo(requestUserId)) {
        return requestUserId;
    }

    if (requestUserId < Constants::START_USERID) {
        APP_LOGE("requestUserId(%{public}d) less than start userId.", requestUserId);
        return Constants::INVALID_USERID;
    }

    int32_t responseUserId = Constants::INVALID_USERID;
    for (const auto &innerBundleUserInfo : innerBundleUserInfos_) {
        if (innerBundleUserInfo.second.bundleUserInfo.userId < Constants::START_USERID) {
            responseUserId = innerBundleUserInfo.second.bundleUserInfo.userId;
            break;
        }
    }

    APP_LOGD("requestUserId(%{public}d) and responseUserId(%{public}d).", requestUserId, responseUserId);
    return responseUserId;
}

void InnerBundleInfo::GetUriPrefixList(std::vector<std::string> &uriPrefixList,
    const std::string &excludeModule) const
{
    std::vector<std::string> uriList;
    for (const auto &abilityInfoPair : baseAbilityInfos_) {
        if (abilityInfoPair.second.uri.empty()) {
            continue;
        }
        if (!excludeModule.empty() && abilityInfoPair.first.find(excludeModule) == 0) {
            continue;
        }
        uriList.emplace_back(abilityInfoPair.second.uri);
    }
    for (const auto &extensionInfoPair : baseExtensionInfos_) {
        if (extensionInfoPair.second.uri.empty()) {
            continue;
        }
        if (!excludeModule.empty() && extensionInfoPair.first.find(excludeModule) == 0) {
            continue;
        }
        uriList.emplace_back(extensionInfoPair.second.uri);
    }
    for (const std::string &uri : uriList) {
        size_t schemePos = uri.find(Constants::URI_SEPARATOR);
        if (schemePos == uri.npos) {
            continue;
        }
        size_t cutPos = uri.find(Constants::SEPARATOR, schemePos + Constants::URI_SEPARATOR_LEN);
        std::string uriPrefix;
        if (cutPos == uri.npos) {
            uriPrefix = uri;
        } else {
            uriPrefix = uri.substr(0, cutPos);
        }
        uriPrefixList.emplace_back(uriPrefix);
    }
}

void InnerBundleInfo::GetUriPrefixList(std::vector<std::string> &uriPrefixList, int32_t userId,
    const std::string &excludeModule) const
{
    auto& key = NameAndUserIdToKey(GetBundleName(), userId);
    auto infoItem = innerBundleUserInfos_.find(key);
    if (infoItem == innerBundleUserInfos_.end()) {
        return;
    }
    GetUriPrefixList(uriPrefixList, excludeModule);
}

std::vector<std::string> InnerBundleInfo::GetDependentModuleNames(const std::string &moduleName) const
{
    std::vector<std::string> dependentModuleNames;
    auto it = innerModuleInfos_.find(moduleName);
    if (it == innerModuleInfos_.end()) {
        APP_LOGE("GetDependentModuleNames can not find module %{public}s", moduleName.c_str());
        return dependentModuleNames;
    }
    dependentModuleNames = it->second.dependencies;
    return dependentModuleNames;
}

std::vector<std::string> InnerBundleInfo::GetAllDependentModuleNames(const std::string &moduleName) const
{
    std::deque<std::string> moduleDeque;
    std::vector<std::string> dependentModuleNames = GetDependentModuleNames(moduleName);
    for (auto &st : dependentModuleNames) {
        moduleDeque.push_back(st);
    }
    std::vector<std::string> res;
    while (!moduleDeque.empty()) {
        std::string name = moduleDeque.front();
        moduleDeque.pop_front();
        if (std::find(res.begin(), res.end(), name) == res.end()) {
            res.push_back(name);
            dependentModuleNames = GetDependentModuleNames(name);
            for (auto &st : dependentModuleNames) {
                moduleDeque.push_back(st);
            }
        }
    }
    return res;
}

std::string InnerBundleInfo::GetMainAbility() const
{
    AbilityInfo abilityInfo;
    GetMainAbilityInfo(abilityInfo);
    return abilityInfo.name;
}

void InnerBundleInfo::GetMainAbilityInfo(AbilityInfo &abilityInfo) const
{
    for (const auto& item : innerModuleInfos_) {
        const std::string& key = item.second.entryAbilityKey;
        if (!key.empty() && (baseAbilityInfos_.count(key) != 0)) {
            abilityInfo = baseAbilityInfos_.at(key);
            if (item.second.isEntry) {
                return;
            }
        }
    }
}

bool InnerBundleInfo::HasEntry() const
{
    for (const auto& item : innerModuleInfos_) {
        if (item.second.isEntry) {
            return true;
        }
    }
    return false;
}
}  // namespace AppExecFwk
}  // namespace OHOS
