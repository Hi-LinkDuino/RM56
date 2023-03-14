/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "napi_rdb_store.h"

#include <cinttypes>

#include "js_logger.h"
#include "js_utils.h"
#include "napi_async_proxy.h"
#include "napi_rdb_predicates.h"
#include "napi_result_set.h"
#include "rdb_errno.h"
#include "securec.h"

using namespace OHOS::NativeRdb;
using namespace OHOS::AppDataMgrJsKit;
using OHOS::DistributedRdb::SubscribeMode;
using OHOS::DistributedRdb::SubscribeOption;
using OHOS::DistributedRdb::SyncOption;
using OHOS::DistributedRdb::SyncResult;

namespace OHOS {
namespace RdbJsKit {
class RdbStoreContext : public NapiAsyncProxy<RdbStoreContext>::AysncContext {
public:
    RdbStoreContext() : AysncContext(), predicatesProxy(nullptr), valuesBucket(nullptr), rowId(0), enumArg(0)
    {
        valuesBucket = new ValuesBucket();
    }

    virtual ~RdbStoreContext()
    {
        auto *obj = reinterpret_cast<RdbStoreProxy *>(boundObj);
        if (obj != nullptr) {
            obj->Release(env);
        }
        delete valuesBucket;
    }

    void BindArgs(napi_env env, napi_value arg);
    void JSNumber2NativeType(std::shared_ptr<OHOS::NativeRdb::RdbStore> &rdbStore);
    std::string device;
    std::string tableName;
    std::vector<std::string> tablesName;
    std::string whereClause;
    std::vector<std::string> whereArgs;
    std::vector<std::string> selectionArgs;
    std::string sql;
    RdbPredicatesProxy *predicatesProxy;
    std::vector<std::string> columns;
    ValuesBucket *valuesBucket;
    std::map<std::string, ValueObject> numberMaps;
    std::vector<ValueObject> bindArgs;
    uint64_t rowId;
    std::vector<uint8_t> newKey;
    std::unique_ptr<AbsSharedResultSet> resultSet;
    std::unique_ptr<ResultSet> resultSet_value;
    std::string aliasName;
    std::string pathName;
    std::string destName;
    std::string srcName;
    int32_t enumArg;
    DistributedRdb::SyncResult syncResult;
};

static __thread napi_ref constructor_ = nullptr;

void RdbStoreContext::BindArgs(napi_env env, napi_value arg)
{
    bindArgs.clear();
    uint32_t arrLen = 0;
    napi_get_array_length(env, arg, &arrLen);
    if (arrLen == 0) {
        return;
    }
    for (size_t i = 0; i < arrLen; ++i) {
        napi_value element;
        napi_get_element(env, arg, i, &element);
        napi_valuetype type;
        napi_typeof(env, element, &type);
        switch (type) {
            case napi_boolean: {
                    bool value = false;
                    napi_status status = napi_get_value_bool(env, element, &value);
                    if (status == napi_ok) {
                        bindArgs.push_back(ValueObject(value));
                    }
                } break;
            case napi_number: {
                    double value;
                    napi_status status = napi_get_value_double(env, element, &value);
                    if (status == napi_ok) {
                        bindArgs.push_back(ValueObject(value));
                    }
                } break;
            case napi_null:
                bindArgs.push_back(ValueObject());
                break;
            case napi_string:
                bindArgs.push_back(ValueObject(JSUtils::Convert2String(env, element, JSUtils::DEFAULT_BUF_SIZE)));
                break;
            case napi_object:
                bindArgs.push_back(ValueObject(JSUtils::Convert2U8Vector(env, element)));
                break;
            default:
                break;
        }
    }
}

void RdbStoreContext::JSNumber2NativeType(std::shared_ptr<OHOS::NativeRdb::RdbStore> &rdbStore)
{
    std::unique_ptr<ResultSet> result = rdbStore->QueryByStep(std::string("SELECT * FROM ") + tableName + " LIMIT 1");
    result->GoToFirstRow();
    for (std::map<std::string, ValueObject>::iterator it = numberMaps.begin(); it != numberMaps.end(); ++it) {
        int index = -1;
        result->GetColumnIndex(it->first, index);
        ColumnType columnType = ColumnType::TYPE_FLOAT;
        result->GetColumnType(index, columnType);
        double value;
        it->second.GetDouble(value);
        switch (columnType) {
            case ColumnType::TYPE_FLOAT:
                LOG_DEBUG("JSNumber2NativeType type:float");
                valuesBucket->PutDouble(it->first, value);
                break;
            case ColumnType::TYPE_INTEGER:
                LOG_DEBUG("JSNumber2NativeType type:integer");
                valuesBucket->PutLong(it->first, int64_t(value));
                break;
            default:
                LOG_DEBUG("JSNumber2NativeType type:%{public}d", int(columnType));
                valuesBucket->PutDouble(it->first, value);
                break;
        }
    }
    result->Close();
    result = nullptr;
    numberMaps.clear();
    LOG_DEBUG("JSNumber2NativeType end");
}

RdbStoreProxy::RdbStoreProxy()
{
}

RdbStoreProxy::~RdbStoreProxy()
{
    LOG_DEBUG("RdbStoreProxy destructor");
}

void RdbStoreProxy::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor descriptors[] = {
        DECLARE_NAPI_FUNCTION("delete", Delete),
        DECLARE_NAPI_FUNCTION("update", Update),
        DECLARE_NAPI_FUNCTION("insert", Insert),
        DECLARE_NAPI_FUNCTION("querySql", QuerySql),
        DECLARE_NAPI_FUNCTION("query", Query),
        DECLARE_NAPI_FUNCTION("executeSql", ExecuteSql),
        DECLARE_NAPI_FUNCTION("replace", Replace),
        DECLARE_NAPI_FUNCTION("backup", Backup),
        DECLARE_NAPI_FUNCTION("count", Count),
        DECLARE_NAPI_FUNCTION("addAttach", Attach),
        DECLARE_NAPI_FUNCTION("beginTransaction", BeginTransaction),
        DECLARE_NAPI_FUNCTION("rollBack", RollBack),
        DECLARE_NAPI_FUNCTION("commit", Commit),
        DECLARE_NAPI_FUNCTION("queryByStep", QueryByStep),
        DECLARE_NAPI_GETTER_SETTER("version", GetVersion, SetVersion),
        DECLARE_NAPI_FUNCTION("markAsCommit", MarkAsCommit),
        DECLARE_NAPI_FUNCTION("endTransaction", EndTransaction),
        DECLARE_NAPI_FUNCTION("restore", ChangeDbFileForRestore),
        DECLARE_NAPI_FUNCTION("changeEncryptKey", ChangeEncryptKey),
        DECLARE_NAPI_GETTER("isInTransaction", IsInTransaction),
        DECLARE_NAPI_GETTER("isOpen", IsOpen),
        DECLARE_NAPI_GETTER("path", GetPath),
        DECLARE_NAPI_GETTER("isHoldingConnection", IsHoldingConnection),
        DECLARE_NAPI_GETTER("isReadOnly", IsReadOnly),
        DECLARE_NAPI_GETTER("isMemoryRdb", IsMemoryRdb),
        DECLARE_NAPI_FUNCTION("setDistributedTables", SetDistributedTables),
        DECLARE_NAPI_FUNCTION("obtainDistributedTableName", ObtainDistributedTableName),
        DECLARE_NAPI_FUNCTION("sync", Sync),
        DECLARE_NAPI_FUNCTION("on", OnEvent),
        DECLARE_NAPI_FUNCTION("off", OffEvent),
    };
    napi_value cons = nullptr;
    napi_define_class(env, "RdbStore", NAPI_AUTO_LENGTH, Initialize, nullptr,
        sizeof(descriptors) / sizeof(napi_property_descriptor), descriptors, &cons);

    napi_create_reference(env, cons, 1, &constructor_);
    LOG_DEBUG("Init RdbStoreProxy end");
}

napi_value RdbStoreProxy::Initialize(napi_env env, napi_callback_info info)
{
    napi_value self;
    NAPI_CALL(env, napi_get_cb_info(env, info, NULL, NULL, &self, nullptr));
    auto finalize = [](napi_env env, void *data, void *hint) {
        RdbStoreProxy *proxy = reinterpret_cast<RdbStoreProxy *>(data);
        if (proxy->ref_ != nullptr) {
            napi_delete_reference(env, proxy->ref_);
            proxy->ref_ = nullptr;
        }
        delete proxy;
    };
    auto *proxy = new RdbStoreProxy();
    napi_status status = napi_wrap(env, self, proxy, finalize, nullptr, &proxy->ref_);
    if (status != napi_ok) {
        LOG_ERROR("RdbStoreProxy::Initialize napi_wrap failed! code:%{public}d!", status);
        finalize(env, proxy, nullptr);
        return nullptr;
    }
    if (proxy->ref_ == nullptr) {
        napi_create_reference(env, self, 0, &proxy->ref_);
    }
    return self;
}

napi_value RdbStoreProxy::NewInstance(napi_env env, std::shared_ptr<OHOS::NativeRdb::RdbStore> value)
{
    napi_value cons;
    napi_status status = napi_get_reference_value(env, constructor_, &cons);
    if (status != napi_ok) {
        LOG_ERROR("RdbStoreProxy::NewInstance get constructor failed! code:%{public}d!", status);
        return nullptr;
    }

    napi_value instance = nullptr;
    status = napi_new_instance(env, cons, 0, nullptr, &instance);
    if (status != napi_ok) {
        LOG_ERROR("RdbStoreProxy::NewInstance napi_new_instance failed! code:%{public}d!", status);
        return nullptr;
    }

    RdbStoreProxy *proxy = nullptr;
    status = napi_unwrap(env, instance, reinterpret_cast<void **>(&proxy));
    if (proxy == nullptr) {
        LOG_ERROR("RdbStoreProxy::NewInstance native instance is nullptr! code:%{public}d!", status);
        return instance;
    }
    proxy->rdbStore_ = std::move(value);
    return instance;
}

RdbStoreProxy *RdbStoreProxy::GetNativeInstance(napi_env env, napi_value self)
{
    RdbStoreProxy *proxy = nullptr;
    napi_status status = napi_unwrap(env, self, reinterpret_cast<void **>(&proxy));
    if (proxy == nullptr) {
        LOG_ERROR("RdbStoreProxy::GetNativePredicates native instance is nullptr! code:%{public}d!", status);
        return nullptr;
    }
    uint32_t count = 0;
    {
        std::lock_guard<std::mutex> lock(proxy->mutex_);
        status = napi_reference_ref(env, proxy->ref_, &count);
    }
    if (status != napi_ok) {
        LOG_ERROR("RdbStoreProxy::GetNativePredicates napi_reference_ref(%{public}p) failed! code:%{public}d!, "
                  "count:%{public}u",
            proxy->ref_, status, count);
        return proxy;
    }
    return proxy;
}

void RdbStoreProxy::Release(napi_env env)
{
    uint32_t count = 0;
    napi_status status = napi_ok;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        status = napi_reference_unref(env, ref_, &count);
    }

    if (status != napi_ok) {
        LOG_ERROR("RdbStoreProxy::Release napi_reference_unref failed! code:%{public}d!, count:%{public}u",
            status, count);
    }
}

void ParseThis(const napi_env &env, const napi_value &arg, RdbStoreContext *asyncContext)
{
    asyncContext->boundObj = RdbStoreProxy::GetNativeInstance(env, arg);
    LOG_DEBUG("ParseThis end");
}

void ParseTableName(const napi_env &env, const napi_value &arg, RdbStoreContext *asyncContext)
{
    asyncContext->tableName = JSUtils::Convert2String(env, arg, JSUtils::DEFAULT_BUF_SIZE);
    LOG_DEBUG("ParseTableName end");
}

void ParseDevice(const napi_env &env, const napi_value &arg, RdbStoreContext *asyncContext)
{
    asyncContext->device = JSUtils::Convert2String(env, arg, JSUtils::DEFAULT_BUF_SIZE);
    LOG_DEBUG("ParseDevice end");
}

void ParseTablesName(const napi_env &env, const napi_value &arg, RdbStoreContext *asyncContext)
{
    uint32_t arrLen = 0;
    napi_get_array_length(env, arg, &arrLen);
    if (arrLen == 0) {
        return;
    }
    for (uint32_t i = 0; i < arrLen; ++i) {
        napi_value element;
        napi_get_element(env, arg, i, &element);
        napi_valuetype type;
        napi_typeof(env, element, &type);
        if (type == napi_string) {
            std::string table = JSUtils::Convert2String(env, element, JSUtils::DEFAULT_BUF_SIZE);
            asyncContext->tablesName.push_back(table);
        }
    }
    LOG_DEBUG("ParseTablesName end");
}

void ParseEnumArg(const napi_env &env, const napi_value &arg, RdbStoreContext *asyncContext)
{
    napi_get_value_int32(env, arg, &asyncContext->enumArg);
    LOG_DEBUG("ParseEnumArg end");
}

void ParsePredicates(const napi_env &env, const napi_value &arg, RdbStoreContext *asyncContext)
{
    napi_unwrap(env, arg, reinterpret_cast<void **>(&asyncContext->predicatesProxy));
    asyncContext->tableName = asyncContext->predicatesProxy->GetPredicates()->GetTableName();
    LOG_DEBUG("ParsePredicates end");
}

void ParseNewKey(const napi_env &env, const napi_value &arg, RdbStoreContext *asyncContext)
{
    asyncContext->newKey = JSUtils::Convert2U8Vector(env, arg);
    LOG_DEBUG("ParseNewKey end");
}

void ParseDestName(const napi_env &env, const napi_value &arg, RdbStoreContext *asyncContext)
{
    asyncContext->destName = JSUtils::Convert2String(env, arg, E_EMPTY_FILE_NAME);
    LOG_DEBUG("DestName end");
}

void ParseSrcName(const napi_env &env, const napi_value &arg, RdbStoreContext *asyncContext)
{
    asyncContext->srcName = JSUtils::Convert2String(env, arg, E_EMPTY_TABLE_NAME);
    LOG_DEBUG("ParseSrcName end");
}

void ParseColumns(const napi_env &env, const napi_value &arg, RdbStoreContext *asyncContext)
{
    asyncContext->columns = JSUtils::Convert2StrVector(env, arg, JSUtils::DEFAULT_BUF_SIZE);
    LOG_DEBUG("ParseColumns end");
}

void ParseWhereClause(const napi_env &env, const napi_value &arg, RdbStoreContext *asyncContext)
{
    asyncContext->whereClause = JSUtils::Convert2String(env, arg, E_HAVING_CLAUSE_NOT_IN_GROUP_BY);
    LOG_DEBUG("ParseWhereClause end");
}

void ParseAlias(const napi_env &env, const napi_value &arg, RdbStoreContext *asyncContext)
{
    asyncContext->aliasName = JSUtils::Convert2String(env, arg, E_EMPTY_TABLE_NAME);
    LOG_DEBUG("ParseAlias end");
}

void ParsePath(const napi_env &env, const napi_value &arg, RdbStoreContext *asyncContext)
{
    asyncContext->pathName = JSUtils::Convert2String(env, arg, E_EMPTY_TABLE_NAME);
    LOG_DEBUG("ParsePath end");
}

void ParseWhereArgs(const napi_env &env, const napi_value &arg, RdbStoreContext *asyncContext)
{
    asyncContext->whereArgs = JSUtils::Convert2StrVector(env, arg, JSUtils::DEFAULT_BUF_SIZE);
    LOG_DEBUG("ParseWhereArgs end");
}

void ParseSelectionArgs(const napi_env &env, const napi_value &arg, RdbStoreContext *asyncContext)
{
    asyncContext->selectionArgs = JSUtils::Convert2StrVector(env, arg, JSUtils::DEFAULT_BUF_SIZE);
    LOG_DEBUG("ParseSelectionArgs end");
}

void ParseSql(const napi_env &env, const napi_value &arg, RdbStoreContext *asyncContext)
{
    asyncContext->sql = JSUtils::Convert2String(env, arg, JSUtils::DEFAULT_BUF_SIZE);
    LOG_DEBUG("ParseSql end");
}

void ParseValuesBucket(const napi_env &env, const napi_value &arg, RdbStoreContext *context)
{
    napi_value keys = 0;
    napi_get_property_names(env, arg, &keys);
    uint32_t arrLen = 0;
    napi_status status = napi_get_array_length(env, keys, &arrLen);
    if (status != napi_ok) {
        LOG_DEBUG("ValuesBucket errr");
        return;
    }
    for (size_t i = 0; i < arrLen; ++i) {
        napi_value key;
        status = napi_get_element(env, keys, i, &key);
        std::string keyStr = JSUtils::Convert2String(env, key, JSUtils::DEFAULT_BUF_SIZE);
        napi_value value;
        napi_get_property(env, arg, key, &value);
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, value, &valueType);
        if (valueType == napi_string) {
            std::string valueString = JSUtils::Convert2String(env, value, JSUtils::DEFAULT_BUF_SIZE);
            context->valuesBucket->PutString(keyStr, valueString);
            LOG_DEBUG("ValueObject type napi_string");
        } else if (valueType == napi_number) {
            double valueNumber;
            napi_get_value_double(env, value, &valueNumber);
            context->numberMaps.insert(std::make_pair(keyStr, ValueObject(valueNumber)));
            LOG_DEBUG("ValueObject type napi_number");
        } else if (valueType == napi_boolean) {
            bool valueBool = false;
            napi_get_value_bool(env, value, &valueBool);
            context->valuesBucket->PutBool(keyStr, valueBool);
            LOG_DEBUG("ValueObject type napi_boolean");
        } else if (valueType == napi_null) {
            context->valuesBucket->PutNull(keyStr);
            LOG_DEBUG("ValueObject type napi_null");
        } else if (valueType == napi_object) {
            context->valuesBucket->PutBlob(keyStr, JSUtils::Convert2U8Vector(env, value));
            LOG_DEBUG("ValueObject type napi_object");
        } else {
            LOG_WARN("valuesBucket error");
        }
    }
    LOG_DEBUG("ParseValuesBucket end");
}

napi_value RdbStoreProxy::Insert(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::Insert start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    parsers.push_back(ParseTableName);
    parsers.push_back(ParseValuesBucket);
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "Insert",
        [](RdbStoreContext *context) {
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            int64_t rowId = 0;
            LOG_DEBUG("RdbStoreProxy::Insert Async");
            context->JSNumber2NativeType(obj->rdbStore_);
            int errCode = obj->rdbStore_->Insert(rowId, context->tableName, *(context->valuesBucket));
            context->rowId = rowId;
            LOG_DEBUG("RdbStoreProxy::Insert errCode is: %{public}d", errCode);
            return errCode;
        },
        [](RdbStoreContext *context, napi_value &output) {
            napi_status status = napi_create_int64(context->env, context->rowId, &output);
            LOG_DEBUG("RdbStoreProxy::Insert end");
            return (status == napi_ok) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::Delete(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::Delete start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    parsers.push_back(ParsePredicates);
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "Delete",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::Delete Async");
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            int temp = 0;
            int errCode = obj->rdbStore_->Delete(temp, *(context->predicatesProxy->GetPredicates()));
            context->rowId = temp;
            LOG_DEBUG("RdbStoreProxy::Delete errCode is: %{public}d", errCode);
            return errCode;
        },
        [](RdbStoreContext *context, napi_value &output) {
            napi_status status = napi_create_int64(context->env, context->rowId, &output);
            LOG_DEBUG("RdbStoreProxy::Delete end");
            return (status == napi_ok) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::Update(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::Update start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    parsers.push_back(ParseValuesBucket);
    parsers.push_back(ParsePredicates);
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "Update",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::Update Async");
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            int temp = 0;
            context->JSNumber2NativeType(obj->rdbStore_);
            int errCode =
                obj->rdbStore_->Update(temp, *(context->valuesBucket), *(context->predicatesProxy->GetPredicates()));
            context->rowId = temp;
            LOG_DEBUG("RdbStoreProxy::Update errCode is: %{public}d", errCode);
            return errCode;
        },
        [](RdbStoreContext *context, napi_value &output) {
            napi_status status = napi_create_int64(context->env, context->rowId, &output);
            LOG_DEBUG("RdbStoreProxy::Update end");
            return (status == napi_ok) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::Query(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::Query start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    parsers.push_back(ParsePredicates);
    parsers.push_back(ParseColumns);
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "Query",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::Query Async");
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            context->resultSet = obj->rdbStore_->Query(*(context->predicatesProxy->GetPredicates()), context->columns);
            LOG_DEBUG("RdbStoreProxy::Query result is nullptr ? %{public}d", (context->resultSet == nullptr));
            return (context->resultSet != nullptr) ? OK : ERR;
        },
        [](RdbStoreContext *context, napi_value &output) {
            output = ResultSetProxy::NewInstance(
                context->env, std::shared_ptr<AbsSharedResultSet>(context->resultSet.release()));
            LOG_DEBUG("RdbStoreProxy::Query end");
            return (output != nullptr) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::QuerySql(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::QuerySql start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    parsers.push_back(ParseSql);
    parsers.push_back(ParseSelectionArgs);
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "QuerySql",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::QuerySql Async");
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            context->resultSet = obj->rdbStore_->QuerySql(context->sql, context->selectionArgs);
            LOG_DEBUG("RdbStoreProxy::QuerySql is nullptr ? %{public}d", (context->resultSet == nullptr));
            return (context->resultSet != nullptr) ? OK : ERR;
        },
        [](RdbStoreContext *context, napi_value &output) {
            output = ResultSetProxy::NewInstance(
                context->env, std::shared_ptr<AbsSharedResultSet>(context->resultSet.release()));
            LOG_DEBUG("RdbStoreProxy::QuerySql end");
            return (output != nullptr) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::ExecuteSql(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::ExecuteSql start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    parsers.push_back(ParseSql);
    parsers.push_back(
        [](const napi_env &env, const napi_value &arg, RdbStoreContext *ctx) { ctx->BindArgs(env, arg); });
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "ExecuteSql",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::ExecuteSql Async");
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            int errCode = obj->rdbStore_->ExecuteSql(context->sql, context->bindArgs);
            LOG_DEBUG("RdbStoreProxy::ExecuteSql errCode is: %{public}d", errCode);
            return errCode;
        },
        [](RdbStoreContext *context, napi_value &output) {
            napi_status status = napi_get_undefined(context->env, &output);
            LOG_DEBUG("RdbStoreProxy::ExecuteSql end");
            return (status == napi_ok) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::Count(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::Count start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    parsers.push_back(ParsePredicates);
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "Count",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::Count Async");
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            std::int64_t temp = 0;
            int errCode = obj->rdbStore_->Count(temp, *(context->predicatesProxy->GetPredicates()));
            context->rowId = temp;
            LOG_DEBUG("RdbStoreProxy::Count errCode is: %{public}d", errCode);
            return errCode;
        },
        [](RdbStoreContext *context, napi_value &output) {
            napi_status status = napi_create_int64(context->env, context->rowId, &output);
            LOG_DEBUG("RdbStoreProxy::Count end");
            return (status == napi_ok) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::Replace(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::Replace start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    parsers.push_back(ParseTableName);
    parsers.push_back(ParseValuesBucket);
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "Replace",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::Replace Async");
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            int64_t rowId = 0;
            context->JSNumber2NativeType(obj->rdbStore_);
            int errCode = obj->rdbStore_->Replace(rowId, context->tableName, *(context->valuesBucket));
            context->rowId = rowId;
            LOG_DEBUG("RdbStoreProxy::Replace errCode is:%{public}d", errCode);
            return errCode;
        },
        [](RdbStoreContext *context, napi_value &output) {
            napi_status status = napi_create_int64(context->env, context->rowId, &output);
            LOG_DEBUG("RdbStoreProxy::Replace end");
            return (status == napi_ok) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::Backup(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::Backup start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    parsers.push_back(ParseTableName);
    parsers.push_back(ParseNewKey);
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "Backup",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::Backup Async");
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            int errCode = obj->rdbStore_->Backup(context->tableName, context->newKey);
            LOG_DEBUG("RdbStoreProxy::Backup errCode is: %{public}d", errCode);
            return (errCode == E_OK) ? OK : ERR;
        },
        [](RdbStoreContext *context, napi_value &output) {
            napi_status status = napi_get_undefined(context->env, &output);
            LOG_DEBUG("RdbStoreProxy::Backup end");
            return (status == napi_ok) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::Attach(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::Attach start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    parsers.push_back(ParseAlias);
    parsers.push_back(ParsePath);
    parsers.push_back(ParseNewKey);
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "Attach",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::Attach Async");
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            int errCode = obj->rdbStore_->Attach(context->aliasName, context->pathName, context->newKey);
            LOG_ERROR("RdbStoreProxy::Attach errCode is:%{public}d ", errCode);
            return (errCode != E_OK) ? OK : ERR;
        },
        [](RdbStoreContext *context, napi_value &output) {
            napi_status status = napi_get_undefined(context->env, &output);
            LOG_DEBUG("RdbStoreProxy::Attach end");
            return (status == napi_ok) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::IsHoldingConnection(napi_env env, napi_callback_info info)
{
    napi_value thisObj = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisObj, nullptr);
    RdbStoreProxy *rdbStoreProxy = GetNativeInstance(env, thisObj);
    NAPI_ASSERT(env, rdbStoreProxy != nullptr, "RdbStoreProxy is nullptr");
    bool out = rdbStoreProxy->rdbStore_->IsHoldingConnection();
    LOG_DEBUG("RdbStoreProxy::IsHoldingConnection out is : %{public}d", out);
    return JSUtils::Convert2JSValue(env, out);
}

napi_value RdbStoreProxy::IsReadOnly(napi_env env, napi_callback_info info)
{
    napi_value thisObj = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisObj, nullptr);
    RdbStoreProxy *rdbStoreProxy = GetNativeInstance(env, thisObj);
    NAPI_ASSERT(env, rdbStoreProxy != nullptr, "RdbStoreProxy is nullptr");
    bool out = rdbStoreProxy->rdbStore_->IsReadOnly();
    LOG_DEBUG("RdbStoreProxy::IsReadOnly out is : %{public}d", out);
    return JSUtils::Convert2JSValue(env, out);
}

napi_value RdbStoreProxy::IsMemoryRdb(napi_env env, napi_callback_info info)
{
    napi_value thisObj = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisObj, nullptr);
    RdbStoreProxy *rdbStoreProxy = GetNativeInstance(env, thisObj);
    NAPI_ASSERT(env, rdbStoreProxy != nullptr, "RdbStoreProxy is nullptr");
    bool out = rdbStoreProxy->rdbStore_->IsMemoryRdb();
    LOG_DEBUG("RdbStoreProxy::IsMemoryRdb out is : %{public}d", out);
    return JSUtils::Convert2JSValue(env, out);
}

napi_value RdbStoreProxy::GetPath(napi_env env, napi_callback_info info)
{
    napi_value thisObj = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisObj, nullptr);
    RdbStoreProxy *rdbStoreProxy = GetNativeInstance(env, thisObj);
    NAPI_ASSERT(env, rdbStoreProxy != nullptr, "RdbStoreProxy is nullptr");
    std::string path = rdbStoreProxy->rdbStore_->GetPath();
    LOG_DEBUG("RdbStoreProxy::GetPath path is empty ? %{public}d", path.empty());
    return JSUtils::Convert2JSValue(env, path);
}

napi_value RdbStoreProxy::BeginTransaction(napi_env env, napi_callback_info info)
{
    napi_value thisObj = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisObj, nullptr));
    RdbStoreProxy *rdbStoreProxy = GetNativeInstance(env, thisObj);
    NAPI_ASSERT(env, rdbStoreProxy != nullptr, "RdbStoreProxy is nullptr");
    int errCode = rdbStoreProxy->rdbStore_->BeginTransaction();
    NAPI_ASSERT(env, errCode == E_OK, "call BeginTransaction failed");
    rdbStoreProxy->Release(env);
    LOG_DEBUG("RdbStoreProxy::BeginTransaction end, errCode is:%{public}d", errCode);
    return nullptr;
}

napi_value RdbStoreProxy::RollBack(napi_env env, napi_callback_info info)
{
    napi_value thisObj = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisObj, nullptr));
    RdbStoreProxy *rdbStoreProxy = GetNativeInstance(env, thisObj);
    NAPI_ASSERT(env, rdbStoreProxy != nullptr, "RdbStoreProxy is nullptr");
    int errCode = rdbStoreProxy->rdbStore_->RollBack();
    NAPI_ASSERT(env, errCode == E_OK, "call RollBack failed");
    rdbStoreProxy->Release(env);
    LOG_DEBUG("RdbStoreProxy::RollBack end, errCode is:%{public}d", errCode);
    return nullptr;
}

napi_value RdbStoreProxy::Commit(napi_env env, napi_callback_info info)
{
    napi_value thisObj = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisObj, nullptr));
    RdbStoreProxy *rdbStoreProxy = GetNativeInstance(env, thisObj);
    NAPI_ASSERT(env, rdbStoreProxy != nullptr, "RdbStoreProxy is nullptr");
    int errCode = rdbStoreProxy->rdbStore_->Commit();
    NAPI_ASSERT(env, errCode == E_OK, "call Commit failed");
    rdbStoreProxy->Release(env);
    LOG_DEBUG("RdbStoreProxy::Commit end, errCode is:%{public}d", errCode);
    return nullptr;
}

napi_value RdbStoreProxy::QueryByStep(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::QueryByStep start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    parsers.push_back(ParseSql);
    parsers.push_back(ParseColumns);
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "QueryByStep",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::QueryByStep Async");
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            context->resultSet_value = obj->rdbStore_->QueryByStep(context->sql, context->columns);
            LOG_ERROR("RdbStoreProxy::QueryByStep is nullptr ? %{public}d ", context->resultSet_value == nullptr);
            return (context->resultSet_value != nullptr) ? OK : ERR;
        },
        [](RdbStoreContext *context, napi_value &output) {
            napi_status status = napi_get_undefined(context->env, &output);
            LOG_DEBUG("RdbStoreProxy::QueryByStep end");
            return (status == napi_ok) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::IsInTransaction(napi_env env, napi_callback_info info)
{
    napi_value thisObj = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisObj, nullptr);
    RdbStoreProxy *rdbStoreProxy = GetNativeInstance(env, thisObj);
    NAPI_ASSERT(env, rdbStoreProxy != nullptr, "RdbStoreProxy is nullptr");
    bool out = rdbStoreProxy->rdbStore_->IsInTransaction();
    LOG_DEBUG("RdbStoreProxy::IsInTransaction out is : %{public}d", out);
    return JSUtils::Convert2JSValue(env, out);
}

napi_value RdbStoreProxy::IsOpen(napi_env env, napi_callback_info info)
{
    napi_value thisObj = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisObj, nullptr);
    RdbStoreProxy *rdbStoreProxy = GetNativeInstance(env, thisObj);
    NAPI_ASSERT(env, rdbStoreProxy != nullptr, "RdbStoreProxy is nullptr");
    bool out = rdbStoreProxy->rdbStore_->IsOpen();
    LOG_DEBUG("RdbStoreProxy::IsOpen out is : %{public}d", out);
    return JSUtils::Convert2JSValue(env, out);
}

napi_value RdbStoreProxy::GetVersion(napi_env env, napi_callback_info info)
{
    napi_value thisObj = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisObj, nullptr);
    RdbStoreProxy *rdbStoreProxy = GetNativeInstance(env, thisObj);
    NAPI_ASSERT(env, rdbStoreProxy != nullptr, "RdbStoreProxy is nullptr");
    int32_t getVersion = 0;
    int out = rdbStoreProxy->rdbStore_->GetVersion(getVersion);
    LOG_DEBUG("RdbStoreProxy::GetVersion out is : %{public}d", out);
    return JSUtils::Convert2JSValue(env, out);
}

napi_value RdbStoreProxy::SetVersion(napi_env env, napi_callback_info info)
{
    napi_value thiz;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc == 1, "RdbStoreProxy::SetVersion Invalid argvs!");
    RdbStoreProxy *rdbStoreProxy = GetNativeInstance(env, thiz);
    NAPI_ASSERT(env, rdbStoreProxy != nullptr, "RdbStoreProxy is nullptr");
    int32_t setVersion = 0;
    napi_get_value_int32(env, args[0], &setVersion);
    int out = rdbStoreProxy->rdbStore_->SetVersion(setVersion);
    LOG_DEBUG("RdbStoreProxy::SetVersion out is : %{public}d", out);
    return thiz;
}

napi_value RdbStoreProxy::MarkAsCommit(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::MarkAsCommit start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "MarkAsCommit",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::MarkAsCommit Async");
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            int errCode = obj->rdbStore_->MarkAsCommit();
            LOG_ERROR("RdbStoreProxy::MarkAsCommit errCode is : %{public}d", errCode);
            return (errCode == E_OK) ? OK : ERR;
        },
        [](RdbStoreContext *context, napi_value &output) {
            napi_status status = napi_get_undefined(context->env, &output);
            LOG_DEBUG("RdbStoreProxy::MarkAsCommit end");
            return (status == napi_ok) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::EndTransaction(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::EndTransaction start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "EndTranscation",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::EndTransaction Async");
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            int errCode = obj->rdbStore_->EndTransaction();
            LOG_DEBUG("RdbStoreProxy::EndTransaction errCode is : %{public}d", errCode);
            return (errCode != E_OK) ? OK : ERR;
        },
        [](RdbStoreContext *context, napi_value &output) {
            napi_status status = napi_get_undefined(context->env, &output);
            LOG_DEBUG("RdbStoreProxy::EndTransaction end");
            return (status == napi_ok) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::ChangeDbFileForRestore(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::ChangeDbFileForRestore start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    parsers.push_back(ParseDestName);
    parsers.push_back(ParseSrcName);
    parsers.push_back(ParseNewKey);
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "Restore",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::ChangeDbFileForRestore Async");
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            int errCode = 0;
            errCode = obj->rdbStore_->ChangeDbFileForRestore(context->destName, context->srcName, context->newKey);
            LOG_DEBUG("RdbStoreProxy::ChangeDbFileForRestore errCode is : %{public}d", errCode);
            return (errCode != E_OK) ? OK : ERR;
        },

        [](RdbStoreContext *context, napi_value &output) {
            napi_status status = napi_get_undefined(context->env, &output);
            LOG_DEBUG("RdbStoreProxy::ChangeDbFileForRestore end");
            return (status == napi_ok) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::ChangeEncryptKey(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::ChangeEncryptKey start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    parsers.push_back(ParseNewKey);
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "ChangeEncryptKey",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::ChangeEncryptKey Async");
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            int errCode = obj->rdbStore_->ChangeEncryptKey(context->newKey);
            LOG_DEBUG("RdbStoreProxy::ChangeEncryptKey errCode is : %{public}d", errCode);
            return (errCode == E_OK) ? OK : ERR;
        },
        [](RdbStoreContext *context, napi_value &output) {
            napi_status status = napi_get_undefined(context->env, &output);
            LOG_DEBUG("RdbStoreProxy::ChangeEncryptKey end");
            return (status == napi_ok) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::SetDistributedTables(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::SetDistributedTables start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    parsers.push_back(ParseTablesName);
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "SetDistributedTables",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::SetDistributedTables Async");
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            bool res = obj->rdbStore_->SetDistributedTables(context->tablesName);
            LOG_DEBUG("RdbStoreProxy::SetDistributedTables res is : %{public}d", res);
            return res ? OK : ERR;
        },
        [](RdbStoreContext *context, napi_value &output) {
            napi_status status = napi_get_undefined(context->env, &output);
            LOG_DEBUG("RdbStoreProxy::SetDistributedTables end");
            return (status == napi_ok) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::ObtainDistributedTableName(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::ObtainDistributedTableName start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    parsers.push_back(ParseDevice);
    parsers.push_back(ParseTableName);
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "ObtainDistributedTableName",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::ObtainDistributedTableName Async");
            RdbStoreProxy *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            auto name = obj->rdbStore_->ObtainDistributedTableName(context->device, context->tableName);
            LOG_INFO("RdbStoreProxy::ObtainDistributedTableName name is empty ? : %{public}d", name.empty());
            context->tableName = name;
            return name.empty() ? ERR : OK;
        },
        [](RdbStoreContext *context, napi_value &output) {
            napi_status status = napi_create_string_utf8(
                context->env, context->tableName.c_str(), context->tableName.length(), &output);
            LOG_DEBUG("RdbStoreProxy::ObtainDistributedTableName end");
            return (status == napi_ok) ? OK : ERR;
        });
}

napi_value RdbStoreProxy::Sync(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("RdbStoreProxy::Sync start");
    NapiAsyncProxy<RdbStoreContext> proxy;
    proxy.Init(env, info);
    std::vector<NapiAsyncProxy<RdbStoreContext>::InputParser> parsers;
    parsers.push_back(ParseEnumArg);
    parsers.push_back(ParsePredicates);
    proxy.ParseInputs(parsers, ParseThis);
    return proxy.DoAsyncWork(
        "Sync",
        [](RdbStoreContext *context) {
            LOG_DEBUG("RdbStoreProxy::Sync Async");
            auto *obj = reinterpret_cast<RdbStoreProxy *>(context->boundObj);
            SyncOption option;
            option.mode = static_cast<DistributedRdb::SyncMode>(context->enumArg);
            option.isBlock = true;
            bool res = obj->rdbStore_->Sync(option, *context->predicatesProxy->GetPredicates(),
                [context](const SyncResult &result) { context->syncResult = result; });
            LOG_INFO("RdbStoreProxy::Sync res is : %{public}d", res);
            return res ? OK : ERR;
        },
        [](RdbStoreContext *context, napi_value &output) {
            output = JSUtils::Convert2JSValue(context->env, context->syncResult);
            LOG_DEBUG("RdbStoreProxy::Sync end");
            return (output != nullptr) ? OK : ERR;
        });
}

void RdbStoreProxy::OnDataChangeEvent(napi_env env, size_t argc, napi_value *argv)
{
    napi_valuetype type;
    napi_typeof(env, argv[0], &type);
    if (type != napi_number) {
        LOG_ERROR("RdbStoreProxy::OnDataChangeEvent: first argument is not number");
        return;
    }
    int32_t mode = SubscribeMode::SUBSCRIBE_MODE_MAX;
    napi_get_value_int32(env, argv[0], &mode);
    if (mode < 0 || mode >= SubscribeMode::SUBSCRIBE_MODE_MAX) {
        LOG_ERROR("RdbStoreProxy::OnDataChangeEvent: first argument value is invalid");
        return;
    }
    LOG_INFO("RdbStoreProxy::OnDataChangeEvent: mode=%{public}d", mode);

    napi_typeof(env, argv[1], &type);
    if (type != napi_function) {
        LOG_ERROR("RdbStoreProxy::OnDataChangeEvent: second argument is not function");
        return;
    }

    std::lock_guard<std::mutex> lockGuard(mutex_);
    for (const auto &observer : observers_[mode]) {
        if (*observer == argv[1]) {
            LOG_ERROR("RdbStoreProxy::OnDataChangeEvent: duplicate subscribe");
            return;
        }
    }
    SubscribeOption option;
    option.mode = static_cast<SubscribeMode>(mode);
    auto observer = std::make_shared<NapiRdbStoreObserver>(env, argv[1]);
    if (!rdbStore_->Subscribe(option, observer.get())) {
        LOG_ERROR("RdbStoreProxy::OnDataChangeEvent: subscribe failed");
        return;
    }
    observers_[mode].push_back(observer);
    LOG_ERROR("RdbStoreProxy::OnDataChangeEvent: subscribe success");
}

void RdbStoreProxy::OffDataChangeEvent(napi_env env, size_t argc, napi_value *argv)
{
    napi_valuetype type;
    napi_typeof(env, argv[0], &type);
    if (type != napi_number) {
        LOG_ERROR("RdbStoreProxy::OffDataChangeEvent: first argument is not number");
        return;
    }
    int32_t mode = SubscribeMode::SUBSCRIBE_MODE_MAX;
    napi_get_value_int32(env, argv[0], &mode);
    if (mode < 0 || mode >= SubscribeMode::SUBSCRIBE_MODE_MAX) {
        LOG_ERROR("RdbStoreProxy::OffDataChangeEvent: first argument value is invalid");
        return;
    }
    LOG_INFO("RdbStoreProxy::OffDataChangeEvent: mode=%{public}d", mode);

    napi_typeof(env, argv[1], &type);
    if (type != napi_function) {
        LOG_ERROR("RdbStoreProxy::OffDataChangeEvent: second argument is not function");
        return;
    }

    SubscribeOption option;
    option.mode = static_cast<SubscribeMode>(mode);
    std::lock_guard<std::mutex> lockGuard(mutex_);
    for (auto it = observers_[mode].begin(); it != observers_[mode].end(); it++) {
        if (**it == argv[1]) {
            rdbStore_->UnSubscribe(option, it->get());
            observers_[mode].erase(it);
            LOG_INFO("RdbStoreProxy::OffDataChangeEvent: unsubscribe success");
            return;
        }
    }
    LOG_INFO("RdbStoreProxy::OffDataChangeEvent: not found");
}

napi_value RdbStoreProxy::OnEvent(napi_env env, napi_callback_info info)
{
    size_t argc = MAX_ON_EVENT_ARG_NUM;
    napi_value argv[MAX_ON_EVENT_ARG_NUM] {};
    napi_value self = nullptr;
    if (napi_get_cb_info(env, info, &argc, argv, &self, nullptr) != napi_ok) {
        LOG_ERROR("RdbStoreProxy::OnEvent: get args failed");
        return nullptr;
    }
    bool invalid_condition = argc < MIN_ON_EVENT_ARG_NUM || argc > MAX_ON_EVENT_ARG_NUM || self == nullptr;
    NAPI_ASSERT(env, !invalid_condition, "RdbStoreProxy::OnEvent: invalid args");

    auto proxy = RdbStoreProxy::GetNativeInstance(env, self);
    NAPI_ASSERT(env, proxy != nullptr, "RdbStoreProxy::OnEvent: invalid args");

    std::string event = JSUtils::Convert2String(env, argv[0]);
    if (event == "dataChange") {
        proxy->OnDataChangeEvent(env, argc - 1, argv + 1);
    }

    proxy->Release(env);
    LOG_ERROR("RdbStoreProxy::OnEvent end");
    return nullptr;
}

napi_value RdbStoreProxy::OffEvent(napi_env env, napi_callback_info info)
{
    size_t argc = MAX_ON_EVENT_ARG_NUM;
    napi_value argv[MAX_ON_EVENT_ARG_NUM] {};
    napi_value self = nullptr;
    if (napi_get_cb_info(env, info, &argc, argv, &self, nullptr) != napi_ok) {
        LOG_ERROR("RdbStoreProxy::OffEvent: get args failed");
        return nullptr;
    }
    bool invalid_condition = argc < MIN_ON_EVENT_ARG_NUM || argc > MAX_ON_EVENT_ARG_NUM || self == nullptr;
    NAPI_ASSERT(env, !invalid_condition, "RdbStoreProxy::OffEvent: invalid args");

    auto proxy = RdbStoreProxy::GetNativeInstance(env, self);
    NAPI_ASSERT(env, proxy != nullptr, "RdbStoreProxy::OffEvent: invalid args");

    std::string event = JSUtils::Convert2String(env, argv[0]);
    if (event == "dataChange") {
        proxy->OffDataChangeEvent(env, argc - 1, argv + 1);
    }

    proxy->Release(env);
    LOG_ERROR("RdbStoreProxy::OffEvent end");
    return nullptr;
}
} // namespace RdbJsKit
} // namespace OHOS
