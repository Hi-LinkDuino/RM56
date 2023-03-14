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
#include "napi_rdb_store_helper.h"

#include <functional>

#include "js_ability.h"
#include "js_logger.h"
#include "js_utils.h"
#include "napi_async_proxy.h"
#include "napi_rdb_store.h"
#include "rdb_errno.h"
#include "rdb_open_callback.h"
#include "rdb_store_config.h"
#include "sqlite_database_utils.h"
#include "unistd.h"

using namespace OHOS::NativeRdb;
using namespace OHOS::AppDataMgrJsKit;

namespace OHOS {
namespace RdbJsKit {
class OpenCallback : public OHOS::NativeRdb::RdbOpenCallback {
public:
    OpenCallback() = default;

    OpenCallback(napi_env env, napi_value jsObj) : env_(env)
    {
        napi_create_reference(env, jsObj, 1, &ref_);
        napi_value property;
        napi_get_named_property(env_, jsObj, "onOpen", &property);
        napi_create_reference(env, property, 1, &onOpen_);
        napi_get_named_property(env_, jsObj, "onCreate", &property);
        napi_create_reference(env, property, 1, &onCreate_);
        napi_get_named_property(env_, jsObj, "onUpgrade", &property);
        napi_create_reference(env, property, 1, &onUpgrade_);
        napi_get_named_property(env_, jsObj, "onDowngrade", &property);
        napi_create_reference(env, property, 1, &onDowngrade_);
    }

    ~OpenCallback()
    {
        if (env_ != nullptr) {
            napi_delete_reference(env_, ref_);
            napi_delete_reference(env_, onOpen_);
            napi_delete_reference(env_, onCreate_);
            napi_delete_reference(env_, onUpgrade_);
            napi_delete_reference(env_, onDowngrade_);
        }
    }

    OpenCallback(const OpenCallback &obj) = delete;

    OpenCallback &operator=(const OpenCallback &obj) = delete;

    OpenCallback(OpenCallback &&obj) noexcept
    {
        operator=(std::move(obj));
    }

    OpenCallback &operator=(OpenCallback &&obj) noexcept
    {
        if (this == &obj) {
            return *this;
        }
        if (env_ != nullptr && ref_ != nullptr) {
            napi_delete_reference(env_, ref_);
        }
        env_ = obj.env_;
        ref_ = obj.ref_;
        onOpen_ = obj.onOpen_;
        onCreate_ = obj.onCreate_;
        onDowngrade_ = obj.onDowngrade_;
        onUpgrade_ = obj.onUpgrade_;
        callbacks_ = std::move(obj.callbacks_);
        obj.env_ = nullptr;
        obj.ref_ = nullptr;
        obj.onOpen_ = nullptr;
        obj.onCreate_ = nullptr;
        obj.onDowngrade_ = nullptr;
        obj.onUpgrade_ = nullptr;
        return *this;
    }

    int OnCreate(OHOS::NativeRdb::RdbStore &rdbStore) override
    {
        LOG_DEBUG("OnCreate Callback %{public}p", onCreate_);
        callbacks_.emplace_back([this]() -> int {
            napi_value self;
            napi_status status = napi_get_reference_value(env_, ref_, &self);
            if (status != napi_ok) {
                LOG_ERROR("OnCreate get self reference failed, code:%{public}d", status);
                return E_ERROR;
            }
            napi_value method;
            status = napi_get_reference_value(env_, onCreate_, &method);
            if (status != napi_ok) {
                LOG_ERROR("OnCreate get method reference failed, code:%{public}d", status);
                return E_ERROR;
            }
            LOG_DEBUG("OnCreate self:%{public}p, method:%{public}p", self, method);
            napi_value retValue = nullptr;
            status = napi_call_function(env_, self, method, 0, nullptr, &retValue);
            if (status != napi_ok) {
                LOG_ERROR("OnCreate call js method failed, code:%{public}d", status);
                return E_ERROR;
            }
            return E_OK;
        });
        return E_OK;
    }

    int OnUpgrade(OHOS::NativeRdb::RdbStore &rdbStore, int oldVersion, int newVersion) override
    {
        LOG_DEBUG("OnUpgrade Callback %{public}p", onUpgrade_);
        callbacks_.emplace_back([this, oldVersion, newVersion]() -> int {
            napi_value self;
            napi_status status = napi_get_reference_value(env_, ref_, &self);
            if (status != napi_ok) {
                LOG_ERROR("OnUpgrade get self reference failed, code:%{public}d", status);
                return E_ERROR;
            }
            napi_value method;
            status = napi_get_reference_value(env_, onUpgrade_, &method);
            if (status != napi_ok) {
                LOG_ERROR("OnUpgrade get method reference failed, code:%{public}d", status);
                return E_ERROR;
            }
            LOG_DEBUG("OnUpgrade self:%{public}p, method:%{public}p", self, method);
            napi_value result[JSUtils::ASYNC_RST_SIZE] = { 0 };
            napi_get_undefined(env_, &result[0]);
            napi_create_object(env_, &result[1]);
            napi_value version;
            napi_create_int32(env_, newVersion, &version);
            napi_set_named_property(env_, result[1], "currentVersion", version);
            napi_create_int32(env_, oldVersion, &version);
            napi_set_named_property(env_, result[1], "targetVersion", version);
            napi_value retValue = nullptr;
            status = napi_call_function(env_, self, method, JSUtils::ASYNC_RST_SIZE, result, &retValue);
            if (status != napi_ok) {
                LOG_ERROR("OnUpgrade call js method failed, code:%{public}d", status);
                return E_ERROR;
            }
            return E_OK;
        });
        return E_OK;
    }

    int OnDowngrade(OHOS::NativeRdb::RdbStore &rdbStore, int oldVersion, int newVersion) override
    {
        LOG_DEBUG("OnDowngrade Callback %{public}p", onDowngrade_);
        callbacks_.emplace_back([this, oldVersion, newVersion]() -> int {
            napi_value self;
            napi_status status = napi_get_reference_value(env_, ref_, &self);
            if (status != napi_ok) {
                LOG_ERROR("OnDowngrade get self reference failed, code:%{public}d", status);
                return E_ERROR;
            }
            napi_value method;
            status = napi_get_reference_value(env_, onDowngrade_, &method);
            if (status != napi_ok) {
                LOG_ERROR("OnDowngrade get method reference failed, code:%{public}d", status);
                return E_ERROR;
            }
            napi_value result[JSUtils::ASYNC_RST_SIZE] = { 0 };
            napi_get_undefined(env_, &result[0]);
            napi_create_object(env_, &result[1]);
            napi_value version;
            napi_create_int32(env_, newVersion, &version);
            napi_set_named_property(env_, result[1], "currentVersion", version);
            napi_create_int32(env_, oldVersion, &version);
            napi_set_named_property(env_, result[1], "targetVersion", version);
            napi_value retValue = nullptr;
            status = napi_call_function(env_, self, method, JSUtils::ASYNC_RST_SIZE, result, &retValue);
            if (status != napi_ok) {
                LOG_ERROR("OnDowngrade call js method failed, code:%{public}d", status);
                return E_ERROR;
            }
            return E_OK;
        });
        return E_OK;
    }

    int OnOpen(OHOS::NativeRdb::RdbStore &rdbStore) override
    {
        LOG_DEBUG("OnOpen Callback %{public}p", onOpen_);
        callbacks_.emplace_back([this]() -> int {
            napi_value self;
            napi_status status = napi_get_reference_value(env_, ref_, &self);
            if (status != napi_ok) {
                LOG_ERROR("OnOpen get self reference failed, code:%{public}d", status);
                return E_ERROR;
            }
            napi_value method;
            status = napi_get_reference_value(env_, onOpen_, &method);
            if (status != napi_ok) {
                LOG_ERROR("OnOpen get method reference failed, code:%{public}d", status);
                return E_ERROR;
            }
            LOG_DEBUG("OnOpen self:%{public}p, method:%{public}p", self, method);
            napi_value retValue = nullptr;
            status = napi_call_function(env_, self, method, 0, nullptr, &retValue);
            if (status != napi_ok) {
                LOG_ERROR("OnOpen call js method failed, code:%{public}d", status);
                return E_ERROR;
            }
            return E_OK;
        });
        return E_OK;
    }

    void DelayNotify()
    {
        for (auto &callback : callbacks_) {
            callback();
        }
    }

private:
    napi_env env_ = nullptr;
    napi_ref ref_ = nullptr;
    napi_ref onOpen_ = nullptr;
    napi_ref onCreate_ = nullptr;
    napi_ref onUpgrade_ = nullptr;
    napi_ref onDowngrade_ = nullptr;
    std::vector<std::function<int(void)>> callbacks_;
};

class HelperRdbContext : public NapiAsyncProxy<HelperRdbContext>::AysncContext {
public:
    HelperRdbContext() : AysncContext(), config(""), version(0), openCallback(), proxy(nullptr)
    {
    }
    RdbStoreConfig config;
    int32_t version;
    OpenCallback openCallback;
    std::shared_ptr<RdbStore> proxy;
    std::shared_ptr<Context> context;
};

void ParseContext(const napi_env &env, const napi_value &object, HelperRdbContext *asyncContext)
{
    LOG_DEBUG("ParseContext begin");
    auto context = JSAbility::GetContext(env, object);
    NAPI_ASSERT_RETURN_VOID(env, context != nullptr, "ParseContext get context failed.");
    asyncContext->context = context;
    LOG_DEBUG("ParseContext end");
}

void ParseStoreConfig(const napi_env &env, const napi_value &object, HelperRdbContext *asyncContext)
{
    LOG_DEBUG("ParseStoreConfig begin");
    napi_value value;
    napi_get_named_property(env, object, "name", &value);
    NAPI_ASSERT_RETURN_VOID(env, value != nullptr, "no database name found in config.");
    std::string name = JSUtils::Convert2String(env, value, JSUtils::DEFAULT_BUF_SIZE);
    LOG_DEBUG("ParseStoreConfig: name %{public}s", name.c_str());
    NAPI_ASSERT_RETURN_VOID(env, !name.empty(), "Get database name empty.");
    asyncContext->config.SetName(std::move(name));

    value = nullptr;
    napi_get_named_property(env, object, "encryptKey", &value);
    if (value != nullptr) {
        asyncContext->config.SetEncryptKey(JSUtils::Convert2U8Vector(env, value));
    }

    if (asyncContext->context == nullptr) {
        ParseContext(env, nullptr, asyncContext); // when no context as arg got from application.
    }
    std::string databaseDir = asyncContext->context->GetDatabaseDir();
    LOG_DEBUG("ParseStoreConfig: databaseDir %{public}s", databaseDir.c_str());
    int errorCode = E_OK;
    std::string realPath = SqliteDatabaseUtils::GetDefaultDatabasePath(databaseDir, name, errorCode);
    LOG_DEBUG("ParseStoreConfig: realPath %{public}s", realPath.c_str());
    NAPI_ASSERT_RETURN_VOID(env, errorCode == E_OK, "Get database real path failed.");
    asyncContext->config.SetPath(realPath);
    asyncContext->config.SetBundleName(asyncContext->context->GetBundleName());
    LOG_DEBUG("ParseStoreConfig end");
}

void ParsePath(const napi_env &env, const napi_value &arg, HelperRdbContext *asyncContext)
{
    LOG_DEBUG("ParsePath begin");
    std::string path = JSUtils::Convert2String(env, arg, JSUtils::DEFAULT_BUF_SIZE);
    NAPI_ASSERT_RETURN_VOID(env, !path.empty(), "Get database name empty.");
    size_t pos = path.find_first_of('/');
    NAPI_ASSERT_RETURN_VOID(env, pos == std::string::npos, "A name without path should be input.");

    if (asyncContext->context == nullptr) {
        ParseContext(env, nullptr, asyncContext); // when no context as arg got from application.
    }
    std::string databaseDir = asyncContext->context->GetDatabaseDir();
    int errorCode = E_OK;
    std::string realPath = SqliteDatabaseUtils::GetDefaultDatabasePath(databaseDir, path, errorCode);
    NAPI_ASSERT_RETURN_VOID(env, errorCode == E_OK, "Get database real path failed.");
    asyncContext->config.SetPath(realPath);
    LOG_DEBUG("ParsePath end");
}

void ParseVersion(const napi_env &env, const napi_value &arg, HelperRdbContext *asyncContext)
{
    napi_get_value_int32(env, arg, &asyncContext->version);
    LOG_DEBUG("ParseVersion end");
}

class DefaultOpenCallback : public RdbOpenCallback {
public:
    int OnCreate(RdbStore &rdbStore) override
    {
        return E_OK;
    }
    int OnUpgrade(RdbStore &rdbStore, int oldVersion, int newVersion) override
    {
        return E_OK;
    }
};

napi_value GetRdbStore(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbJsKit::GetRdbStore start");
    NapiAsyncProxy<HelperRdbContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<HelperRdbContext>::InputParser> parsers;

    if (JSAbility::CheckContext(env, info)) {
        parsers.push_back(ParseContext);
    }
    parsers.push_back(ParseStoreConfig);
    parsers.push_back(ParseVersion);
    proxy.ParseInputs(parsers);
    return proxy.DoAsyncWork(
        "getRdbStore",
        [](HelperRdbContext *context) {
            LOG_DEBUG("RdbJsKit::GetRdbStore Async");
            int errCode = OK;
            DefaultOpenCallback callback;
            context->proxy = RdbHelper::GetRdbStore(context->config, context->version, callback, errCode);
            if (errCode != E_OK) {
                LOG_DEBUG("GetRdbStore failed %{public}d", errCode);
            }
            return (errCode == E_OK) ? OK : ERR;
        },
        [](HelperRdbContext *context, napi_value &output) {
            output = RdbStoreProxy::NewInstance(context->env, context->proxy);
            context->openCallback.DelayNotify();
            LOG_DEBUG("RdbJsKit::GetRdbStore end");
            return (output != nullptr) ? OK : ERR;
        });
}

napi_value DeleteRdbStore(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbJsKit::DeleteRdbStore start");
    NapiAsyncProxy<HelperRdbContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<HelperRdbContext>::InputParser> parsers;

    if (JSAbility::CheckContext(env, info)) {
        parsers.push_back(ParseContext);
    }
    parsers.push_back(ParsePath);
    proxy.ParseInputs(parsers);
    return proxy.DoAsyncWork(
        "deleteRdbStore",
        [](HelperRdbContext *context) {
            int ret = RdbHelper::DeleteRdbStore(context->config.GetPath());
            return (ret == E_OK) ? OK : ERR;
        },
        [](HelperRdbContext *context, napi_value &output) {
            napi_status status = napi_create_int64(context->env, E_OK, &output);
            LOG_DEBUG("RdbJsKit::DeleteRdbStore end");
            return (status == napi_ok) ? OK : ERR;
        });
}

napi_value InitRdbHelper(napi_env env, napi_value exports)
{
    LOG_INFO("RdbJsKit::InitRdbHelper begin");
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getRdbStore", GetRdbStore),
        DECLARE_NAPI_FUNCTION("deleteRdbStore", DeleteRdbStore),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(*properties), properties));
    LOG_INFO("RdbJsKit::InitRdbHelper end");
    return exports;
}
} // namespace RdbJsKit
} // namespace OHOS
