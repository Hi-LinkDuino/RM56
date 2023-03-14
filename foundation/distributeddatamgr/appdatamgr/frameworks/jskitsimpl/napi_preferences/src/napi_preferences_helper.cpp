/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "napi_preferences_helper.h"

#include <linux/limits.h>

#include <string>

#include "js_ability.h"
#include "js_logger.h"
#include "js_utils.h"
#include "napi_async_proxy.h"
#include "napi_preferences.h"
#include "preferences_errno.h"
#include "securec.h"

using namespace OHOS::NativePreferences;
using namespace OHOS::AppDataMgrJsKit;

namespace OHOS {
namespace PreferencesJsKit {
struct HelperAysncContext : NapiAsyncProxy<HelperAysncContext>::AysncContext {
    std::string path;
    std::shared_ptr<Context> context;
};

void ParseContext(const napi_env &env, const napi_value &object, HelperAysncContext *asyncContext)
{
    auto context = JSAbility::GetContext(env, object);
    NAPI_ASSERT_RETURN_VOID(env, context != nullptr, "ParseContext get context failed.");
    asyncContext->context = context;
}

void ParseName(const napi_env &env, const napi_value &value, HelperAysncContext *asyncContext)
{
    LOG_DEBUG("ParseName start");
    NAPI_ASSERT_RETURN_VOID(env, asyncContext->context != nullptr, "ParseName context is null.");
    std::string name = JSUtils::Convert2String(env, value, JSUtils::DEFAULT_BUF_SIZE);
    NAPI_ASSERT_RETURN_VOID(env, !name.empty(), "Get preferences name empty.");

    size_t pos = name.find_first_of('/');
    NAPI_ASSERT_RETURN_VOID(env, pos == std::string::npos, "A name without path should be input.");
    std::string databaseDir = asyncContext->context->GetDatabaseDir();
    asyncContext->path = databaseDir.append("/").append(name);
    LOG_DEBUG("ParseName: path %{public}s", asyncContext->path.c_str());
}

napi_value GetPreferences(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("GetPreferences start");
    NapiAsyncProxy<HelperAysncContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<HelperAysncContext>::InputParser> parsers;
    parsers.push_back(ParseContext);
    parsers.push_back(ParseName);
    proxy.ParseInputs(parsers);

    return proxy.DoAsyncWork(
        "GetPreferences",
        [](HelperAysncContext *asyncContext) {
            int errCode = OK;
            OHOS::NativePreferences::PreferencesHelper::GetPreferences(asyncContext->path, errCode);
            LOG_DEBUG("GetPreferences return %{public}d", errCode);
            return errCode;
        },
        [](HelperAysncContext *asyncContext, napi_value &output) {
            napi_value path = nullptr;
            napi_create_string_utf8(asyncContext->env, asyncContext->path.c_str(), NAPI_AUTO_LENGTH, &path);
            auto ret = PreferencesProxy::NewInstance(asyncContext->env, path, &output);
            return (ret == napi_ok) ? OK : ERR;
        });
}

napi_value DeletePreferences(napi_env env, napi_callback_info info)
{
    NapiAsyncProxy<HelperAysncContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<HelperAysncContext>::InputParser> parsers;
    parsers.push_back(ParseContext);
    parsers.push_back(ParseName);
    proxy.ParseInputs(parsers);

    return proxy.DoAsyncWork(
        "DeletePreferences",
        [](HelperAysncContext *asyncContext) {
            int errCode = PreferencesHelper::DeletePreferences(asyncContext->path);
            LOG_DEBUG("DeletePreferences return %{public}d", errCode);
            return errCode;
        },
        [](HelperAysncContext *asyncContext, napi_value &output) {
            napi_get_undefined(asyncContext->env, &output);
            return OK;
        });
}

napi_value RemovePreferencesFromCache(napi_env env, napi_callback_info info)
{
    NapiAsyncProxy<HelperAysncContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<HelperAysncContext>::InputParser> parsers;
    parsers.push_back(ParseContext);
    parsers.push_back(ParseName);
    proxy.ParseInputs(parsers);

    return proxy.DoAsyncWork(
        "RemovePreferencesFromCache",
        [](HelperAysncContext *asyncContext) {
            int errCode = PreferencesHelper::RemovePreferencesFromCache(asyncContext->path);
            LOG_DEBUG("RemovePreferencesFromCache return %{public}d", errCode);
            return errCode;
        },
        [](HelperAysncContext *asyncContext, napi_value &output) {
            napi_get_undefined(asyncContext->env, &output);
            return OK;
        });
}

napi_value InitPreferencesHelper(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getPreferences", GetPreferences),
        DECLARE_NAPI_FUNCTION("deletePreferences", DeletePreferences),
        DECLARE_NAPI_FUNCTION("removePreferencesFromCache", RemovePreferencesFromCache),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(*properties), properties));
    return exports;
}
} // namespace PreferencesJsKit
} // namespace OHOS
