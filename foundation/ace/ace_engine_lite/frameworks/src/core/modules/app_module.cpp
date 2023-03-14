/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "app_module.h"
#include "ace_log.h"
#include "js_app_context.h"
#if (FEATURE_SCREEN_ON_VISIBLE == 1)
#include "js_async_work.h"
#include "product_adapter.h"
#endif

using namespace OHOS::ACELite;

#include "hal_trace.h"

static JSIValue g_thisVal = NULL;
static JSIValue g_callback = NULL;
extern "C" void notify_update_bt_ui2(int flag)
{
    TRACE(0, "%s 11111 called===========g_callback=%#x", __func__,g_callback);
    JSIValue argv[ARGC_ONE] = {JSI::CreateNumber(flag)};
    JSI::CallFunction(g_callback, g_thisVal, argv, ARGC_ONE);
}

namespace OHOS {
namespace ACELite {
const char * const AppModule::FILE_MANIFEST = "manifest.json";
const char * const AppModule::KEY_APP_NAME = "appName";
const char * const AppModule::KEY_VERSION_NAME = "versionName";
const char * const AppModule::KEY_VERSION_CODE = "versionCode";

#if (FEATURE_SCREEN_ON_VISIBLE == 1)
const char * const AppModule::SCREEN_ON_VISIBLE_KEY = "visible";
const char * const AppModule::SCREEN_ON_VISIBLE_DATA = "data";
const char * const AppModule::SCREEN_ON_VISIBLE_CODE = "code";
const char * const AppModule::SCREEN_ON_VISIBLE_INVALID_PARAMETER = "Incorrect parameter";
const uint8_t AppModule::SCREEN_ON_VISIBLE_ERR = 202;

struct AsyncParams : public MemoryHeap {
    ACE_DISALLOW_COPY_AND_MOVE(AsyncParams);
    AsyncParams() : result(nullptr), callback(nullptr), context(nullptr) {}

    JSIValue result;
    JSIValue callback;
    JSIValue context;
};
#endif

JSIValue AppModule::GetInfo(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue result = JSI::CreateUndefined();

    cJSON *manifest = ReadManifest();
    if (manifest == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Fail to get the content of manifest.");
        return result;
    }

    cJSON *appName = cJSON_GetObjectItem(manifest, KEY_APP_NAME);
    cJSON *versionName = cJSON_GetObjectItem(manifest, KEY_VERSION_NAME);
    cJSON *versionCode = cJSON_GetObjectItem(manifest, KEY_VERSION_CODE);

    result = JSI::CreateObject();
    if (appName != nullptr && appName->type == cJSON_String) {
        JSI::SetStringProperty(result, KEY_APP_NAME, appName->valuestring);
    }
    if (versionName != nullptr && versionName->type == cJSON_String) {
        JSI::SetStringProperty(result, KEY_VERSION_NAME, versionName->valuestring);
    }
    if (versionCode != nullptr && versionCode->type == cJSON_Number) {
        JSI::SetNumberProperty(result, KEY_VERSION_CODE, versionCode->valuedouble);
    }
    cJSON_Delete(manifest);
    manifest = nullptr;
    return result;
}

cJSON *AppModule::ReadManifest()
{
    char *appPath = const_cast<char *>(JsAppContext::GetInstance()->GetCurrentAbilityPath());
    if ((appPath == nullptr) || !strlen(appPath)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Fail to get app information because the app path is null or empty.");
        return nullptr;
    }

    char *manifestPath = RelocateJSSourceFilePath(appPath, FILE_MANIFEST);
    if (manifestPath == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Fail to get app information because the manifest.json path is null.");
        return nullptr;
    }
    if (!strlen(manifestPath)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Fail to get app information because the manifest.json path is empty.");
        ace_free(manifestPath);
        manifestPath = nullptr;
        return nullptr;
    }

    uint32_t fileSize = 0;
    char *manifestContent = ReadFile(manifestPath, fileSize, false);

    ace_free(manifestPath);
    manifestPath = nullptr;

    if (manifestContent == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Fail to get app information because the manifest.json context is null.");
        return nullptr;
    }
    if (fileSize == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Fail to get app information because the manifest.json context is empty.");
        ace_free(manifestContent);
        manifestContent = nullptr;
        return nullptr;
    }

    cJSON *manifest = cJSON_Parse(manifestContent);

    ace_free(manifestContent);
    manifestContent = nullptr;

    return manifest;
}

JSIValue AppModule::Terminate(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    UNUSED(thisVal);
    UNUSED(args);
    UNUSED(argsNum);
    JsAppContext::GetInstance()->TerminateAbility();
    return JSI::CreateUndefined();
}
JSIValue AppModule::TestMothod(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    HILOG_ERROR("%s called===========", __func__);
    return JSI::CreateUndefined();
}
JSIValue AppModule::TestJSICallback(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    TRACE(0, "%s %d called===========", __func__, __LINE__);
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || (argsNum == 0) || (JSI::ValueIsUndefined(args[0]))) {
        return undefValue;
    }

    g_thisVal = thisVal;
    g_callback = JSI::GetNamedProperty(args[0], CB_CALLBACK);
    TRACE(0, "%s %d called===========g_callback=%#x", __func__,__LINE__,g_callback);
    return JSI::CreateUndefined();
}

#if (FEATURE_SCREEN_ON_VISIBLE == 1)
JSIValue AppModule::ScreenOnVisible(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || (argsNum == 0) || (JSI::ValueIsUndefined(args[0]))) {
        ProductAdapter::SetScreenOnVisible(false);
        return undefValue;
    }

    JSIValue visibleInput = JSI::GetNamedProperty(args[0], SCREEN_ON_VISIBLE_KEY);
    bool visible = false;
    if (!JSI::ValueIsUndefined(visibleInput)) {
        visible = JSI::ValueIsBoolean(visibleInput) ? JSI::ValueToBoolean(visibleInput) : false;
    }

    bool ret = ProductAdapter::SetScreenOnVisible(visible);
    if (ret) {
        OnSetActionSuccess(thisVal, args);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "Fail to set screen visible property");
        OnSetActionFail(thisVal, args);
    }
    OnSetActionComplete(thisVal, args);
    JSI::ReleaseValueList(visibleInput);
    return undefValue;
}

void AppModule::OnSetActionSuccess(const JSIValue thisVal, const JSIValue *args)
{
    JSIValue callback = JSI::GetNamedProperty(args[0], CB_SUCCESS);
    if ((!JSI::ValueIsUndefined(callback)) && JSI::ValueIsFunction(callback)) {
        AsyncCallFunction(thisVal, callback, nullptr);
    } else {
        JSI::ReleaseValue(callback);
    }
}

void AppModule::OnSetActionFail(const JSIValue thisVal, const JSIValue *args)
{
    JSIValue fail = JSI::GetNamedProperty(args[0], CB_FAIL);
    if ((!JSI::ValueIsUndefined(fail)) && JSI::ValueIsFunction(fail)) {
        JSIValue result = JSI::CreateObject();
        JSI::SetStringProperty(result, SCREEN_ON_VISIBLE_DATA, SCREEN_ON_VISIBLE_INVALID_PARAMETER);
        JSI::SetNumberProperty(result, SCREEN_ON_VISIBLE_CODE, SCREEN_ON_VISIBLE_ERR);
        AsyncCallFunction(thisVal, fail, result);
    } else {
        JSI::ReleaseValue(fail);
    }
}

void AppModule::OnSetActionComplete(const JSIValue thisVal, const JSIValue *args)
{
    JSIValue callback = JSI::GetNamedProperty(args[0], CB_COMPLETE);
    if ((!JSI::ValueIsUndefined(callback)) && JSI::ValueIsFunction(callback)) {
        AsyncCallFunction(thisVal, callback, nullptr);
    } else {
        JSI::ReleaseValue(callback);
    }
}

void AppModule::AsyncCallFunction(const JSIValue thisVal, const JSIValue callback, const JSIValue result)
{
    AsyncParams *params = new AsyncParams();
    if (params == nullptr) {
        JSI::ReleaseValueList(result, callback);
        return;
    }

    params->result = result;
    params->callback = callback;
    params->context = JSI::AcquireValue(thisVal);
    if (!JsAsyncWork::DispatchAsyncWork(Execute, static_cast<void *>(params))) {
        JSI::ReleaseValueList(params->result, params->callback, params->context);
        delete params;
        params = nullptr;
    }
}

void AppModule::Execute(void *data)
{
    AsyncParams *params = static_cast<AsyncParams *>(data);
    if (params == nullptr) {
        return;
    }

    JSIValue callback = params->callback;
    JSIValue result = params->result;
    JSIValue context = params->context;
    if (result == nullptr) {
        // complete callback and success callback do not need parameter
        JSI::CallFunction(callback, context, nullptr, 0);
        JSI::ReleaseValueList(callback, context);
    } else {
        // fail callback need error reason and error code
        JSIValue argv[ARGC_TWO] = {
            JSI::GetNamedProperty(result, SCREEN_ON_VISIBLE_DATA),
            JSI::GetNamedProperty(result, SCREEN_ON_VISIBLE_CODE)
        };
        JSI::CallFunction(callback, context, argv, ARGC_TWO);
        JSI::ReleaseValueList(callback, result, context);
    }

    delete params;
    params = nullptr;
}
#endif // FEATURE_SCREEN_ON_VISIBLE
} // namespace ACELite
} // namespace OHOS
