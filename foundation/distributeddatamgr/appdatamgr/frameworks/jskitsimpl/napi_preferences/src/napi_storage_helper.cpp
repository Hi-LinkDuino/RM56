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
#include "napi_storage_helper.h"

#include <linux/limits.h>

#include <string>

#include "js_logger.h"
#include "napi_async_proxy.h"
#include "napi_storage.h"
#include "preferences_errno.h"
#include "securec.h"

using namespace OHOS::NativePreferences;
using namespace OHOS::AppDataMgrJsKit;

namespace OHOS {
namespace PreferencesJsKit {
struct HelperAysncContext : NapiAsyncProxy<HelperAysncContext>::AysncContext {
    std::string path;
};

napi_value GetStorageSync(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    LOG_DEBUG("getPreferences %{public}zu", argc);

    char *path = new char[PATH_MAX];
    size_t pathLen = 0;
    napi_get_value_string_utf8(env, args[0], path, PATH_MAX, &pathLen);
    LOG_DEBUG("getPreferences: path %{public}s", path);
    delete[] path;

    napi_value instance = nullptr;
    NAPI_CALL(env, StorageProxy::NewInstance(env, args[0], &instance));
    LOG_DEBUG("getPreferences end");

    return instance;
}

void ParseString(const napi_env &env, const napi_value &value, HelperAysncContext *asyncContext)
{
    if (asyncContext == nullptr) {
        LOG_WARN("error input");
        return;
    }
    char *path = new char[PATH_MAX];
    size_t pathLen = 0;
    napi_get_value_string_utf8(env, value, path, PATH_MAX, &pathLen);
    asyncContext->path = path;
    delete[] path;
}

napi_value GetStorage(napi_env env, napi_callback_info info)
{
    NapiAsyncProxy<HelperAysncContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<HelperAysncContext>::InputParser> parsers;
    parsers.push_back(ParseString);
    proxy.ParseInputs(parsers);

    return proxy.DoAsyncWork(
        "GetStorage",
        [](HelperAysncContext *asyncContext) {
            int errCode = OK;
            OHOS::NativePreferences::PreferencesHelper::GetPreferences(asyncContext->path, errCode);
            LOG_DEBUG("GetPreferences return %{public}d", errCode);
            return errCode;
        },
        [](HelperAysncContext *asyncContext, napi_value &output) {
            napi_value path = nullptr;
            napi_create_string_utf8(asyncContext->env, asyncContext->path.c_str(), NAPI_AUTO_LENGTH, &path);
            auto ret = StorageProxy::NewInstance(asyncContext->env, path, &output);
            return (ret == napi_ok) ? OK : ERR;
        });
}

napi_status GetInputPath(napi_env env, napi_callback_info info, std::string &pathString)
{
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_status ret = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (ret != napi_ok) {
        return ret;
    }

    napi_valuetype valueType;
    ret = napi_typeof(env, args[0], &valueType);
    if (ret != napi_ok || valueType != napi_string) {
        return napi_invalid_arg;
    }

    char *path = new char[PATH_MAX];
    size_t pathLen = 0;
    ret = napi_get_value_string_utf8(env, args[0], path, PATH_MAX, &pathLen);
    pathString = path;
    delete[] path;
    return ret;
}

napi_value DeleteStorageSync(napi_env env, napi_callback_info info)
{
    std::string path;
    napi_status ret = GetInputPath(env, info, path);
    if (ret != napi_ok) {
        napi_throw_error(env, nullptr, "Input path error");
        return nullptr;
    }
    int errCode = PreferencesHelper::DeletePreferences(path);
    if (errCode != E_OK) {
        LOG_ERROR("deleteStorage failed %{public}d", errCode);
        napi_throw_error(env, std::to_string(errCode).c_str(), "deleteStorage failed");
    }
    LOG_DEBUG("deleteStorage end");

    return nullptr;
}

napi_value DeleteStorage(napi_env env, napi_callback_info info)
{
    NapiAsyncProxy<HelperAysncContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<HelperAysncContext>::InputParser> parsers;
    parsers.push_back(ParseString);
    proxy.ParseInputs(parsers);

    return proxy.DoAsyncWork(
        "DeleteStorage",
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

napi_value RemoveStorageFromCacheSync(napi_env env, napi_callback_info info)
{
    std::string path;
    napi_status ret = GetInputPath(env, info, path);
    if (ret != napi_ok) {
        napi_throw_error(env, nullptr, "Input path error");
        return nullptr;
    }

    int errCode = PreferencesHelper::RemovePreferencesFromCache(path);
    if (errCode != E_OK) {
        LOG_ERROR("removeStorageFromCache failed %{public}d", errCode);
        napi_throw_error(env, std::to_string(errCode).c_str(), "removeStorageFromCache failed");
    }
    LOG_DEBUG("removeStorageFromCache end");

    return nullptr;
}

napi_value RemoveStorageFromCache(napi_env env, napi_callback_info info)
{
    NapiAsyncProxy<HelperAysncContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<HelperAysncContext>::InputParser> parsers;
    parsers.push_back(ParseString);
    proxy.ParseInputs(parsers);

    return proxy.DoAsyncWork(
        "RemoveStorageFromCache",
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

napi_value InitPreferenceHelper(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getStorage", GetStorage),
        DECLARE_NAPI_FUNCTION("getStorageSync", GetStorageSync),
        DECLARE_NAPI_FUNCTION("deleteStorage", DeleteStorage),
        DECLARE_NAPI_FUNCTION("deleteStorageSync", DeleteStorageSync),
        DECLARE_NAPI_FUNCTION("removeStorageFromCache", RemoveStorageFromCache),
        DECLARE_NAPI_FUNCTION("removeStorageFromCacheSync", RemoveStorageFromCacheSync),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(*properties), properties));
    return exports;
}
} // namespace PreferencesJsKit
} // namespace OHOS
