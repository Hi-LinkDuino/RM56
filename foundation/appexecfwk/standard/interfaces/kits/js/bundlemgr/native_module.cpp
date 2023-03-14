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
#include <pthread.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>

#include "app_log_wrapper.h"
#include "bundle_mgr.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace AppExecFwk {
EXTERN_C_START
/*
 * function for module exports
 */
static napi_value Init(napi_env env, napi_value exports)
{
    napi_value nAbilityType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nAbilityType));
    CreateAbilityTypeObject(env, nAbilityType);

    napi_value nAbilitySubType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nAbilitySubType));
    CreateAbilitySubTypeObject(env, nAbilitySubType);

    napi_value nDisplayOrientation = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nDisplayOrientation));
    CreateDisplayOrientationObject(env, nDisplayOrientation);

    napi_value nLaunchMode = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nLaunchMode));
    CreateLaunchModeObject(env, nLaunchMode);

    napi_value nModuleUpdateFlag = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nModuleUpdateFlag));
    CreateModuleUpdateFlagObject(env, nModuleUpdateFlag);

    napi_value nFormType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nFormType));
    CreateFormTypeObject(env, nFormType);

    napi_value nColorMode = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nColorMode));
    CreateColorModeObject(env, nColorMode);

    napi_value nGrantStatus = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nGrantStatus));
    CreateGrantStatusObject(env, nGrantStatus);

    napi_value nModuleRemoveFlag = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nModuleRemoveFlag));
    CreateModuleRemoveFlagObject(env, nModuleRemoveFlag);

    napi_value nSignatureCompareResult = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nSignatureCompareResult));
    CreateSignatureCompareResultObject(env, nSignatureCompareResult);

    napi_value nShortcutExistence = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nShortcutExistence));
    CreateShortcutExistenceObject(env, nShortcutExistence);

    napi_value nQueryShortCutFlag = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nQueryShortCutFlag));
    CreateQueryShortCutFlagObject(env, nShortcutExistence);

    napi_value nExtensionAbilityType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nExtensionAbilityType));
    CreateExtensionAbilityTypeObject(env, nExtensionAbilityType);

    napi_value nExtensionFlag = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nExtensionFlag));
    CreateExtensionFlagObject(env, nExtensionFlag);

    napi_value nBundleFlag = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nBundleFlag));
    CreateBundleFlagObject(env, nBundleFlag);

    napi_value nInstallErrorCode = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nInstallErrorCode));
    CreateInstallErrorCodeObject(env, nInstallErrorCode);
    /*
     * Propertise define
     */
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getAllApplicationInfo", GetApplicationInfos),
        DECLARE_NAPI_FUNCTION("getApplicationInfos", GetApplicationInfos),
        DECLARE_NAPI_FUNCTION("getApplicationInfo", GetApplicationInfo),
        DECLARE_NAPI_FUNCTION("getAllBundleInfo", GetBundleInfos),
        DECLARE_NAPI_FUNCTION("getBundleInfos", GetBundleInfos),
        DECLARE_NAPI_FUNCTION("getBundleInfo", GetBundleInfo),
        DECLARE_NAPI_FUNCTION("getBundlePackInfo", GetBundlePackInfo),
        DECLARE_NAPI_FUNCTION("getBundleGids", GetBundleGids),
        DECLARE_NAPI_FUNCTION("getBundleArchiveInfo", GetBundleArchiveInfo),
        DECLARE_NAPI_FUNCTION("getLaunchWantForBundle", GetLaunchWantForBundle),
        DECLARE_NAPI_FUNCTION("getPermissionDef", GetPermissionDef),
        DECLARE_NAPI_FUNCTION("queryAbilityByWant", QueryAbilityInfos),
        DECLARE_NAPI_FUNCTION("getBundleInstaller", GetBundleInstaller),
        DECLARE_NAPI_FUNCTION("getFormsInfoByModule", GetFormsInfoByModule),
        DECLARE_NAPI_FUNCTION("getFormsInfo", GetFormsInfoByApp),
        DECLARE_NAPI_FUNCTION("getAllFormsInfo", GetAllFormsInfo),
        DECLARE_NAPI_FUNCTION("cleanBundleCacheFiles", ClearBundleCache),
        DECLARE_NAPI_FUNCTION("setApplicationEnabled", SetApplicationEnabled),
        DECLARE_NAPI_FUNCTION("setAbilityEnabled", SetAbilityEnabled),
        DECLARE_NAPI_FUNCTION("isAbilityEnabled", IsAbilityEnabled),
        DECLARE_NAPI_FUNCTION("isApplicationEnabled", IsApplicationEnabled),
        DECLARE_NAPI_FUNCTION("isModuleRemovable", IsModuleRemovable),
        DECLARE_NAPI_FUNCTION("setModuleUpgradeFlag", SetModuleUpgradeFlag),
        DECLARE_NAPI_FUNCTION("queryExtensionAbilityInfosByWant", QueryExtensionInfoByWant),
        DECLARE_NAPI_FUNCTION("getNameForUid", GetNameForUid),
        DECLARE_NAPI_FUNCTION("getAbilityInfo", GetAbilityInfo),
        DECLARE_NAPI_FUNCTION("getAbilityLabel", GetAbilityLabel),
#ifdef BUNDLE_FRAMEWORK_GRAPHICS
        DECLARE_NAPI_FUNCTION("getAbilityIcon", GetAbilityIcon),
#endif
        DECLARE_NAPI_PROPERTY("AbilityType", nAbilityType),
        DECLARE_NAPI_PROPERTY("AbilitySubType", nAbilitySubType),
        DECLARE_NAPI_PROPERTY("DisplayOrientation", nDisplayOrientation),
        DECLARE_NAPI_PROPERTY("LaunchMode", nLaunchMode),
        DECLARE_NAPI_PROPERTY("ModuleUpdateFlag", nModuleUpdateFlag),
        DECLARE_NAPI_PROPERTY("FormType", nFormType),
        DECLARE_NAPI_PROPERTY("ColorMode", nColorMode),
        DECLARE_NAPI_PROPERTY("GrantStatus", nGrantStatus),
        DECLARE_NAPI_PROPERTY("ModuleRemoveFlag", nModuleRemoveFlag),
        DECLARE_NAPI_PROPERTY("SignatureCompareResult", nSignatureCompareResult),
        DECLARE_NAPI_PROPERTY("ShortcutExistence", nShortcutExistence),
        DECLARE_NAPI_PROPERTY("QueryShortCutFlag", nQueryShortCutFlag),
        DECLARE_NAPI_PROPERTY("InstallErrorCode", nInstallErrorCode),
        DECLARE_NAPI_PROPERTY("ExtensionAbilityType", nExtensionAbilityType),
        DECLARE_NAPI_PROPERTY("BundleFlag", nBundleFlag),
        DECLARE_NAPI_PROPERTY("ExtensionFlag", nExtensionFlag)
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));

    napi_value m_classBundleInstaller;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("install", Install),
        DECLARE_NAPI_FUNCTION("recover", Recover),
        DECLARE_NAPI_FUNCTION("uninstall", Uninstall),
    };
    NAPI_CALL(env,
        napi_define_class(env,
            "BundleInstaller",
            NAPI_AUTO_LENGTH,
            BundleInstallerConstructor,
            nullptr,
            sizeof(properties) / sizeof(*properties),
            properties,
            &m_classBundleInstaller));
    napi_create_reference(env, m_classBundleInstaller, 1, &g_classBundleInstaller);
    APP_LOGI("-----Init end------");
    return exports;
}
EXTERN_C_END

/*
 * Module define
 */
static napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "bundle",
    .nm_priv = ((void *)0),
    .reserved = {0}
};
/*
 * Module register function
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&_module);
}
}  // namespace AppExecFwk
}  // namespace OHOS
