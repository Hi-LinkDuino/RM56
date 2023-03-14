/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "js_app_context.h"
#include "ace_event_error_code.h"
#include "ace_log.h"
#if (defined(__LINUX__) || defined(__LITEOS_A__))
#include "ace_ability.h"
#endif
#if (FEATURE_API_VERSION == 1)
#include "bundle_manager.h"
#endif // FEATURE_API_VERSION
#include "component_factory.h"
#include "component_utils.h"
#include "fatal_handler.h"
#include "js_app_environment.h"
#include "js_profiler.h"
#include "platform_adapter.h"
#include "product_adapter.h"
#include "securec.h"
#include "string_util.h"
#include "task_manager.h"
#include "ui_view_group.h"
#if (OHOS_ACELITE_PRODUCT_WATCH != 1)
#include "ability_env.h"
#endif

namespace OHOS {
namespace ACELite {
constexpr char URI_PREFIX_DATA[] = "internal://app";
constexpr uint8_t URI_PREFIX_DATA_LENGTH = 14;
void JsAppContext::ClearContext()
{
    // reset current ability path and uuid
    ReleaseAbilityInfo();
}

// check byte code file snapshot version is OK with the current
void JsAppContext::CheckSnapshotVersion(const char *bcFileContent, uint32_t contentLength) const
{
    // this is part of engine struct definations
    typedef struct {
        uint32_t magic; // four byte magic number
        uint32_t version; // version number
    } JerrySnapshotHeaderT;
    if ((bcFileContent == nullptr) || (contentLength == 0) || (contentLength <= sizeof(JerrySnapshotHeaderT))) {
        return;
    }
    const uint8_t *snapshotData = reinterpret_cast<const uint8_t *>(bcFileContent);
    const JerrySnapshotHeaderT *headerP = reinterpret_cast<const JerrySnapshotHeaderT *>(snapshotData);
    // JERRY_SNAPSHOT_VERSION is defined in jerryscript-snapshot.h
    if (headerP->version != JERRY_SNAPSHOT_VERSION) {
        HILOG_ERROR(HILOG_MODULE_ACE, "invalid snapshot version[%{public}d]", headerP->version);
    }
}

/**
 * return value should be released by caller when it's not used
 */
jerry_value_t JsAppContext::Eval(char *fullPath, size_t fullPathLength, bool isAppEval) const
{
    if ((fullPath == nullptr) || (fullPathLength == 0)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to eval js code cause by empty JavaScript script.");
        ACE_ERROR_CODE_PRINT(EXCE_ACE_ROUTER_REPLACE_FAILED, EXCE_ACE_PAGE_INDEX_MISSING);
        return UNDEFINED;
    }

    uint32_t contentLength = 0;
    START_TRACING(PAGE_CODE_LOAD);
    bool isSnapshotMode = JsAppEnvironment::GetInstance()->IsSnapshotMode();
    char *jsCode = EvaluateFile(isSnapshotMode, contentLength, fullPath, fullPathLength);
    STOP_TRACING();
    if ((jsCode == nullptr) || (contentLength > FILE_CONTENT_LENGTH_MAX)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "empty js file or length is incorrect, eval user code failed");
        ACE_ERROR_CODE_PRINT(EXCE_ACE_ROUTER_REPLACE_FAILED, EXCE_ACE_PAGE_FILE_READ_FAILED);
        ACE_FREE(jsCode);
        return UNDEFINED;
    }

    START_TRACING(PAGE_CODE_EVAL);
    jerry_value_t viewModel = UNDEFINED;
    if (isSnapshotMode) {
        CheckSnapshotVersion(jsCode, contentLength);
        const uint32_t *snapshotContent = reinterpret_cast<const uint32_t *>(jsCode);
        viewModel = jerry_exec_snapshot(snapshotContent, contentLength, 0, 1);
    } else {
        const jerry_char_t *jsScript = reinterpret_cast<const jerry_char_t *>(jsCode);
        jerry_value_t retValue = jerry_parse(reinterpret_cast<const jerry_char_t *>(fullPath), fullPathLength,
                                             jsScript, contentLength, JERRY_PARSE_NO_OPTS);
        if (jerry_value_is_error(retValue)) {
            ACE_ERROR_CODE_PRINT(EXCE_ACE_ROUTER_REPLACE_FAILED, EXCE_ACE_PAGE_JS_EVAL_FAILED);
            PrintErrorMessage(retValue);
            // free js code buffer
            ace_free(jsCode);
            jsCode = nullptr;
            jerry_release_value(retValue);
            STOP_TRACING();
            return UNDEFINED;
        }
        viewModel = jerry_run(retValue);
        jerry_release_value(retValue);
    }

    STOP_TRACING();
    // free js code buffer
    ace_free(jsCode);
    jsCode = nullptr;

    if (jerry_value_is_error(viewModel)) {
        ACE_ERROR_CODE_PRINT(EXCE_ACE_ROUTER_REPLACE_FAILED, EXCE_ACE_PAGE_JS_EVAL_FAILED);
        PrintErrorMessage(viewModel);
        jerry_release_value(viewModel);
        return UNDEFINED;
    }

    SetGlobalNamedProperty(isAppEval, viewModel);
    return viewModel;
}

char *JsAppContext::EvaluateFile(bool &isSnapshotMode,
                                 uint32_t &outLength,
                                 char *fullPath,
                                 size_t fullPathLength) const
{
    if (fullPath == nullptr || fullPathLength == 0) {
        return nullptr;
    }
    const uint8_t fileSuffixLength = 3; // file suffix is fixed, .js or .bc
    size_t filePathLen = strlen(fullPath);
    if ((filePathLen == 0) || (filePathLen != fullPathLength) || (fullPathLength < fileSuffixLength)) {
        return nullptr;
    }
    outLength = 0;
    char *jsCode = ReadFile(fullPath, outLength, isSnapshotMode);
    if ((jsCode != nullptr) && (outLength <= FILE_CONTENT_LENGTH_MAX)) {
        // read successfully
        return jsCode;
    }
    // make sure the memory is freed
    ACE_FREE(jsCode);

    const char * const anotherSuffx = isSnapshotMode ? ".js" : ".bc";
    // change file suffix to another mode file
    if (strcpy_s((fullPath + (fullPathLength - fileSuffixLength)), (fileSuffixLength + 1), anotherSuffx) != EOK) {
        return nullptr;
    }
    // snapshot mode changed to another
    isSnapshotMode = !isSnapshotMode;
    HILOG_ERROR(HILOG_MODULE_ACE, "JS mode changed unexpected [%{public}d]", isSnapshotMode);
    jsCode = ReadFile(fullPath, outLength, isSnapshotMode);
    return jsCode;
}

void JsAppContext::SetGlobalNamedProperty(bool isAppEval, jerry_value_t viewModel) const
{
    jerry_value_t globalObject = jerry_get_global_object();
    if (isAppEval) {
        JerrySetNamedProperty(globalObject, ATTR_APP, viewModel);
    } else {
        JerrySetNamedProperty(globalObject, ATTR_ROOT, viewModel);
    }
    jerry_release_value(globalObject);
}

jerry_value_t JsAppContext::Render(jerry_value_t viewModel) const
{
    if (jerry_value_is_error(viewModel)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to render app cause by render error.");
        return UNDEFINED;
    }

    if (jerry_value_is_undefined(viewModel)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Nothing to render as it is undefined.");
        return UNDEFINED;
    }

    jerry_value_t renderFunction = jerryx_get_property_str(viewModel, ATTR_RENDER);
    if (jerry_value_is_undefined(renderFunction)) {
        ACE_ERROR_CODE_PRINT(EXCE_ACE_ROUTER_REPLACE_FAILED, EXCE_ACE_PAGE_NO_RENDER_FUNC);
        return UNDEFINED;
    }
    jerry_value_t nativeElement = CallJSFunction(renderFunction, viewModel, nullptr, 0);
    if (jerry_value_is_undefined(nativeElement)) {
        ACE_ERROR_CODE_PRINT(EXCE_ACE_ROUTER_REPLACE_FAILED, EXCE_ACE_PAGE_RENDER_FAILED);
    }
    jerry_release_value(renderFunction);
    return nativeElement;
}

void JsAppContext::TerminateAbility() const
{
    if (currentToken_ == 0) {
        // if no running js ability, drop
        return;
    }
    FatalHandler::GetInstance().SetExitingFlag(true);
    Terminate(currentToken_);
}

void JsAppContext::SetCurrentAbilityInfo(const char * const abilityPath, const char * const bundleName, uint16_t token)
{
    // release old first
    ReleaseAbilityInfo();

    if (abilityPath != nullptr) {
        size_t abilityPathLen = strlen(abilityPath);
        if ((abilityPathLen > 0) && (abilityPathLen < PATH_LENGTH_MAX)) {
            currentAbilityPath_ = static_cast<char *>(ace_malloc(abilityPathLen + 1));
            if (currentAbilityPath_ == nullptr) {
                HILOG_ERROR(HILOG_MODULE_ACE, "malloc buffer for current ability path failed");
                return;
            }
            if (memcpy_s(currentAbilityPath_, abilityPathLen, abilityPath, abilityPathLen) != 0) {
                ace_free(currentAbilityPath_);
                currentAbilityPath_ = nullptr;
                return;
            }
            currentAbilityPath_[abilityPathLen] = 0;
        }
    }

    if (bundleName != nullptr) {
        size_t bundleNameLen = strlen(bundleName);
        if ((bundleNameLen > 0) && (bundleNameLen < NAME_LENGTH_MAX)) {
            currentBundleName_ = static_cast<char *>(ace_malloc(bundleNameLen + 1));
            if (currentBundleName_ == nullptr) {
                HILOG_ERROR(HILOG_MODULE_ACE, "malloc buffer for current uuid failed");
                return;
            }
            if (memcpy_s(currentBundleName_, bundleNameLen, bundleName, bundleNameLen) != 0) {
                ace_free(currentAbilityPath_);
                currentAbilityPath_ = nullptr;
                ace_free(currentBundleName_);
                currentBundleName_ = nullptr;
                return;
            }
            currentBundleName_[bundleNameLen] = 0;
        }
    }

    currentToken_ = token;
}

void JsAppContext::SetCurrentJsPath(const char * const jsPath)
{
    // release old first
    if (currentJsPath_) {
        ace_free(currentJsPath_);
        currentJsPath_ = nullptr;
    }

    if (jsPath != nullptr) {
        size_t jsPathLen = strlen(jsPath);
        if ((jsPathLen > 0) && (jsPathLen < PATH_LENGTH_MAX)) {
            currentJsPath_ = static_cast<char *>(ace_malloc(jsPathLen + 1));
            if (currentJsPath_ == nullptr) {
                HILOG_ERROR(HILOG_MODULE_ACE, "malloc buffer for current js path failed");
                return;
            }
            if (memcpy_s(currentJsPath_, jsPathLen, jsPath, jsPathLen) != 0) {
                ace_free(currentJsPath_);
                currentJsPath_ = nullptr;
                return;
            }
            currentJsPath_[jsPathLen] = '\0';
        }
    }
}

void JsAppContext::ReleaseAbilityInfo()
{
    if (currentBundleName_) {
        ace_free(currentBundleName_);
        currentBundleName_ = nullptr;
    }

    if (currentAbilityPath_) {
        ace_free(currentAbilityPath_);
        currentAbilityPath_ = nullptr;
    }

    if (currentJsPath_) {
        ace_free(currentJsPath_);
        currentJsPath_ = nullptr;
    }
}

char *JsAppContext::GetResourcePath(const char *uri) const
{
    if (uri == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "uri is null.");
        return nullptr;
    }
    size_t size = strlen(uri);
    if (size == 0 || size > NAME_LENGTH_MAX) {
        HILOG_ERROR(HILOG_MODULE_ACE, "uri is empty or too long.");
        return nullptr;
    }
    if (StringUtil::StartsWith(uri, URI_PREFIX_DATA)) {
        char *path = StringUtil::Slice(uri, URI_PREFIX_DATA_LENGTH);
        if (path == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "fail to get resource path.");
            return nullptr;
        }
#if (OHOS_ACELITE_PRODUCT_WATCH == 1)
        // no GetDataPath API provided on watch, contact the path by the product configuration insteadly
        char *relocatedPath = ProcessResourcePathByConfiguration(size, path);
#else
        const char *dataPath = GetDataPath();
        if (dataPath == nullptr || strlen(dataPath) == 0) {
            dataPath = currentBundleName_;
        }
        char *relocatedPath = RelocateResourceFilePath(dataPath, path);
#endif
        ACE_FREE(path);
        return relocatedPath;
    }
    return RelocateResourceFilePath(currentAbilityPath_, uri);
}

char *JsAppContext::ProcessResourcePathByConfiguration(size_t origUriLength, const char *slicedFilePath) const
{
    const char *appDataRoot = ProductAdapter::GetPrivateDataRootPath();
    if (appDataRoot == nullptr || origUriLength == 0 || slicedFilePath == nullptr) {
        return nullptr;
    }
    size_t rootPathLen = strlen(appDataRoot);
    if (rootPathLen == 0) {
        return nullptr;
    }
    size_t dataPathSize = rootPathLen + strlen(currentBundleName_) + origUriLength - URI_PREFIX_DATA_LENGTH + 1;
    char *dataPath = StringUtil::Malloc(dataPathSize);
    if (dataPath == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "fail to malloc data path buffer.");
        return nullptr;
    }
    const char *fmtStr = "%s%s";
    if (appDataRoot[rootPathLen - 1] != RESOURCE_SEPARATOR) {
        fmtStr = "%s/%s";
    }
    if (sprintf_s(dataPath, dataPathSize + 1, fmtStr, appDataRoot, currentBundleName_) < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "fail to get resource path.");
        ACE_FREE(dataPath);
        return nullptr;
    }
    char *relocatedPath = RelocateResourceFilePath(dataPath, slicedFilePath);
    ACE_FREE(dataPath);
    return relocatedPath;
}

void JsAppContext::LoadApiVersion()
{
#if (FEATURE_API_VERSION == 1)
    BundleInfo bundle = {0};
    uint8_t retCode = GetBundleInfo(currentBundleName_, false, &bundle);
    if (retCode != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "fail to get api version.");
        return;
    }
    compatibleApi_ = bundle.compatibleApi;
    targetApi_ = bundle.targetApi;
#else
    const int32_t currentApiVersion = 6;
    compatibleApi_ = currentApiVersion;
    targetApi_ = currentApiVersion;
#endif
}

int32_t JsAppContext::GetCompatibleApi() const
{
    return compatibleApi_;
}

void JsAppContext::SetCompatibleApi(int32_t compatibleApi)
{
    compatibleApi_ = compatibleApi;
}

int32_t JsAppContext::GetTargetApi() const
{
    return targetApi_;
}

void JsAppContext::SetTargetApi(int32_t targetApi)
{
    targetApi_ = targetApi;
}
} // namespace ACELite
} // namespace OHOS
