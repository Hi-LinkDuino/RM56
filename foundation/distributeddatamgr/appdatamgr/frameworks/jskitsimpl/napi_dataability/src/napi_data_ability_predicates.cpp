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

#include "napi_data_ability_predicates.h"

#include "js_logger.h"
#include "js_utils.h"
#include "napi_async_proxy.h"

using namespace OHOS::NativeRdb;
using namespace OHOS::AppDataMgrJsKit;

namespace OHOS {
namespace DataAbilityJsKit {
static __thread napi_ref constructor_ = nullptr;

void DataAbilityPredicatesProxy::Init(napi_env env, napi_value exports)
{
    LOG_INFO("Init DataAbilityPredicatesProxy");
    napi_property_descriptor descriptors[] = {
        DECLARE_NAPI_FUNCTION("equalTo", EqualTo),
        DECLARE_NAPI_FUNCTION("notEqualTo", NotEqualTo),
        DECLARE_NAPI_FUNCTION("beginWrap", BeginWrap),
        DECLARE_NAPI_FUNCTION("endWrap", EndWrap),
        DECLARE_NAPI_FUNCTION("or", Or),
        DECLARE_NAPI_FUNCTION("and", And),
        DECLARE_NAPI_FUNCTION("contains", Contains),
        DECLARE_NAPI_FUNCTION("beginsWith", BeginsWith),
        DECLARE_NAPI_FUNCTION("endsWith", EndsWith),
        DECLARE_NAPI_FUNCTION("isNull", IsNull),
        DECLARE_NAPI_FUNCTION("isNotNull", IsNotNull),
        DECLARE_NAPI_FUNCTION("like", Like),
        DECLARE_NAPI_FUNCTION("glob", Glob),
        DECLARE_NAPI_FUNCTION("between", Between),
        DECLARE_NAPI_FUNCTION("notBetween", NotBetween),
        DECLARE_NAPI_FUNCTION("greaterThan", GreaterThan),
        DECLARE_NAPI_FUNCTION("lessThan", LessThan),
        DECLARE_NAPI_FUNCTION("greaterThanOrEqualTo", GreaterThanOrEqualTo),
        DECLARE_NAPI_FUNCTION("lessThanOrEqualTo", LessThanOrEqualTo),
        DECLARE_NAPI_FUNCTION("orderByAsc", OrderByAsc),
        DECLARE_NAPI_FUNCTION("orderByDesc", OrderByDesc),
        DECLARE_NAPI_FUNCTION("distinct", Distinct),
        DECLARE_NAPI_FUNCTION("limitAs", Limit),
        DECLARE_NAPI_FUNCTION("offsetAs", Offset),
        DECLARE_NAPI_FUNCTION("groupBy", GroupBy),
        DECLARE_NAPI_FUNCTION("indexedBy", IndexedBy),
        DECLARE_NAPI_FUNCTION("in", In),
        DECLARE_NAPI_FUNCTION("notIn", NotIn),
        DECLARE_NAPI_FUNCTION("clear", Clear),
        DECLARE_NAPI_FUNCTION("isRawSelection", IsRawSelection),
        DECLARE_NAPI_GETTER_SETTER("whereClause", GetWhereClause, SetWhereClause),
        DECLARE_NAPI_GETTER_SETTER("whereArgs", GetWhereArgs, SetWhereArgs),
        DECLARE_NAPI_GETTER_SETTER("order", GetOrder, SetOrder),
        DECLARE_NAPI_GETTER("limit", GetLimit),
        DECLARE_NAPI_GETTER("offset", GetOffset),
        DECLARE_NAPI_GETTER("isDistinct", IsDistinct),
        DECLARE_NAPI_GETTER("group", GetGroup),
        DECLARE_NAPI_GETTER("index", GetIndex),
        DECLARE_NAPI_GETTER("isNeedAnd", IsNeedAnd),
        DECLARE_NAPI_GETTER("isSorted", IsSorted),
    };

    napi_value cons;
    NAPI_CALL_RETURN_VOID(env, napi_define_class(env, "DataAbilityPredicates", NAPI_AUTO_LENGTH, New, nullptr,
                                   sizeof(descriptors) / sizeof(napi_property_descriptor), descriptors, &cons));

    NAPI_CALL_RETURN_VOID(env, napi_create_reference(env, cons, 1, &constructor_));

    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, exports, "DataAbilityPredicates", cons));
    LOG_DEBUG("Init DataAbilityPredicatesProxy end");
}

napi_value DataAbilityPredicatesProxy::New(napi_env env, napi_callback_info info)
{
    napi_value new_target;
    NAPI_CALL(env, napi_get_new_target(env, info, &new_target));
    bool is_constructor = (new_target != nullptr);

    napi_value thiz;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thiz, nullptr));

    if (is_constructor) {
        auto *proxy = new DataAbilityPredicatesProxy();
        proxy->env_ = env;
        NAPI_CALL(env, napi_wrap(env, thiz, proxy, DataAbilityPredicatesProxy::Destructor, nullptr, &proxy->wrapper_));
        LOG_INFO("DataAbilityPredicatesProxy::New constructor ref:%{public}p", proxy->wrapper_);
        return thiz;
    }

    napi_value cons;
    NAPI_CALL(env, napi_get_reference_value(env, constructor_, &cons));

    napi_value output;
    NAPI_CALL(env, napi_new_instance(env, cons, 0, nullptr, &output));

    return output;
}

napi_value DataAbilityPredicatesProxy::NewInstance(
    napi_env env, std::shared_ptr<NativeRdb::DataAbilityPredicates> value)
{
    napi_value cons;
    napi_status status = napi_get_reference_value(env, constructor_, &cons);
    if (status != napi_ok) {
        LOG_ERROR("DataAbilityPredicatesProxy get constructor failed! napi_status:%{public}d!", status);
        return nullptr;
    }

    napi_value instance = nullptr;
    status = napi_new_instance(env, cons, 0, nullptr, &instance);
    if (status != napi_ok) {
        LOG_ERROR("DataAbilityPredicatesProxy napi_new_instance failed! napi_status:%{public}d!", status);
        return nullptr;
    }

    DataAbilityPredicatesProxy *proxy = nullptr;
    status = napi_unwrap(env, instance, reinterpret_cast<void **>(&proxy));
    if (status != napi_ok) {
        LOG_ERROR("DataAbilityPredicatesProxy native instance is nullptr! napi_status:%{public}d!", status);
        return instance;
    }
    proxy->predicates_ = std::move(value);
    return instance;
}

std::shared_ptr<NativeRdb::DataAbilityPredicates> DataAbilityPredicatesProxy::GetNativePredicates(
    const napi_env &env, const napi_value &arg)
{
    LOG_DEBUG("GetNativePredicates on called.");
    if (arg == nullptr) {
        LOG_ERROR("DataAbilityPredicatesProxy arg is null.");
        return nullptr;
    }
    DataAbilityPredicatesProxy *proxy = nullptr;
    napi_unwrap(env, arg, reinterpret_cast<void **>(&proxy));
    return proxy->predicates_;
}

void DataAbilityPredicatesProxy::Destructor(napi_env env, void *nativeObject, void *)
{
    DataAbilityPredicatesProxy *proxy = static_cast<DataAbilityPredicatesProxy *>(nativeObject);
    delete proxy;
}

DataAbilityPredicatesProxy::~DataAbilityPredicatesProxy()
{
    napi_delete_reference(env_, wrapper_);
}

DataAbilityPredicatesProxy::DataAbilityPredicatesProxy()
    : predicates_(new DataAbilityPredicates()), env_(nullptr), wrapper_(nullptr)
{
}

std::shared_ptr<NativeRdb::DataAbilityPredicates> DataAbilityPredicatesProxy::GetNativePredicates(
    napi_env env, napi_callback_info info)
{
    DataAbilityPredicatesProxy *predicatesProxy = nullptr;
    napi_value thiz;
    napi_get_cb_info(env, info, nullptr, nullptr, &thiz, nullptr);
    napi_unwrap(env, thiz, reinterpret_cast<void **>(&predicatesProxy));
    return predicatesProxy->predicates_;
}

napi_value DataAbilityPredicatesProxy::EqualTo(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::EqualTo on called.");
    napi_value thiz;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::EqualTo Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);
    GetNativePredicates(env, info)->EqualTo(field, value);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::NotEqualTo(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::NotEqualTo on called.");
    napi_value thiz;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::NotEqualTo Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);
    GetNativePredicates(env, info)->NotEqualTo(field, value);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::BeginWrap(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::BeginWrap on called.");
    napi_value thiz;
    napi_get_cb_info(env, info, nullptr, nullptr, &thiz, nullptr);
    GetNativePredicates(env, info)->BeginWrap();
    return thiz;
}

napi_value DataAbilityPredicatesProxy::EndWrap(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::EndWrap on called.");
    napi_value thiz;
    napi_get_cb_info(env, info, nullptr, nullptr, &thiz, nullptr);
    GetNativePredicates(env, info)->EndWrap();
    return thiz;
}

napi_value DataAbilityPredicatesProxy::Or(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Or on called.");
    napi_value thiz;
    napi_get_cb_info(env, info, nullptr, nullptr, &thiz, nullptr);
    GetNativePredicates(env, info)->Or();
    return thiz;
}

napi_value DataAbilityPredicatesProxy::And(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::And on called.");
    napi_value thiz;
    napi_get_cb_info(env, info, nullptr, nullptr, &thiz, nullptr);
    GetNativePredicates(env, info)->And();
    return thiz;
}

napi_value DataAbilityPredicatesProxy::Contains(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Contains on called.");
    napi_value thiz;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::Contains Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);

    GetNativePredicates(env, info)->Contains(field, value);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::BeginsWith(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::BeginsWith on called.");
    napi_value thiz;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::BeginsWith Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);
    GetNativePredicates(env, info)->BeginsWith(field, value);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::EndsWith(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::EndsWith on called.");
    napi_value thiz;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::EndsWith Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);
    GetNativePredicates(env, info)->EndsWith(field, value);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::IsNull(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::IsNull on called.");
    napi_value thiz;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::IsNull Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    GetNativePredicates(env, info)->IsNull(field);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::IsNotNull(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::IsNotNull on called.");
    napi_value thiz;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::IsNotNull Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    GetNativePredicates(env, info)->IsNotNull(field);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::Like(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Like on called.");
    napi_value thiz;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::Like Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);
    GetNativePredicates(env, info)->Like(field, value);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::Glob(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Glob on called.");
    napi_value thiz;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::Glob Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);
    GetNativePredicates(env, info)->Glob(field, value);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::Between(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Between on called.");
    napi_value thiz;
    size_t argc = 3;
    napi_value args[3] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::Between Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string low = JSUtils::ConvertAny2String(env, args[1]);
    std::string high = JSUtils::ConvertAny2String(env, args[2]);
    GetNativePredicates(env, info)->Between(field, low, high);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::NotBetween(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::NotBetween on called.");
    napi_value thiz;
    size_t argc = 3;
    napi_value args[3] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::NotBetween Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string low = JSUtils::ConvertAny2String(env, args[1]);
    std::string high = JSUtils::ConvertAny2String(env, args[2]);
    GetNativePredicates(env, info)->NotBetween(field, low, high);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::GreaterThan(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::GreaterThan on called.");
    napi_value thiz;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::GreaterThan Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);
    GetNativePredicates(env, info)->GreaterThan(field, value);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::LessThan(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::LessThan on called.");
    napi_value thiz;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::LessThan Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);
    GetNativePredicates(env, info)->LessThan(field, value);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::GreaterThanOrEqualTo(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::GreaterThanOrEqualTo on called.");
    napi_value thiz;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::GreaterThanOrEqualTo Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);
    GetNativePredicates(env, info)->GreaterThanOrEqualTo(field, value);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::LessThanOrEqualTo(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::LessThanOrEqualTo on called.");
    napi_value thiz;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::LessThanOrEqualTo Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::string value = JSUtils::ConvertAny2String(env, args[1]);
    GetNativePredicates(env, info)->LessThanOrEqualTo(field, value);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::OrderByAsc(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::OrderByAsc on called.");
    napi_value thiz;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::OrderByAsc Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    GetNativePredicates(env, info)->OrderByAsc(field);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::OrderByDesc(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::OrderByDesc on called.");
    napi_value thiz;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::OrderByDesc Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    GetNativePredicates(env, info)->OrderByDesc(field);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::Distinct(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Distinct on called.");
    napi_value thiz;
    napi_get_cb_info(env, info, nullptr, nullptr, &thiz, nullptr);
    GetNativePredicates(env, info)->Distinct();
    return thiz;
}

napi_value DataAbilityPredicatesProxy::Limit(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Limit on called.");
    napi_value thiz;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::Limit Invalid argvs!");
    int32_t limit = 0;
    napi_get_value_int32(env, args[0], &limit);
    GetNativePredicates(env, info)->Limit(limit);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::Offset(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Offset on called.");
    napi_value thiz;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::Offset Invalid argvs!");
    int32_t offset = 0;
    napi_get_value_int32(env, args[0], &offset);
    GetNativePredicates(env, info)->Offset(offset);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::GroupBy(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::GroupBy on called.");
    napi_value thiz;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::GroupBy Invalid argvs!");
    std::vector<std::string> fields = JSUtils::Convert2StrVector(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    GetNativePredicates(env, info)->GroupBy(fields);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::IndexedBy(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::IndexedBy on called.");
    napi_value thiz;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::IndexedBy Invalid argvs!");
    std::string indexName = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    GetNativePredicates(env, info)->IndexedBy(indexName);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::In(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::In on called.");
    napi_value thiz;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::In Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::vector<std::string> values = JSUtils::Convert2StrVector(env, args[1], JSUtils::DEFAULT_BUF_SIZE);
    GetNativePredicates(env, info)->In(field, values);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::NotIn(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::NotIn on called.");
    napi_value thiz;
    size_t argc = 2;
    napi_value args[2] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::NotIn Invalid argvs!");
    std::string field = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    std::vector<std::string> values = JSUtils::Convert2StrVector(env, args[1], JSUtils::DEFAULT_BUF_SIZE);
    GetNativePredicates(env, info)->NotIn(field, values);
    return thiz;
}

napi_value DataAbilityPredicatesProxy::Clear(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::Clear on called.");
    napi_value thiz = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thiz, nullptr);
    GetNativePredicates(env, info)->Clear();
    return thiz;
}

napi_value DataAbilityPredicatesProxy::IsRawSelection(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("DataAbilityPredicatesProxy::IsRawSelection on called.");
    napi_value thiz = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thiz, nullptr);
    bool out = GetNativePredicates(env, info)->IsRawSelection();
    return JSUtils::Convert2JSValue(env, out);
}

std::shared_ptr<NativeRdb::DataAbilityPredicates> DataAbilityPredicatesProxy::GetPredicates() const
{
    return this->predicates_;
}

napi_value DataAbilityPredicatesProxy::GetWhereClause(napi_env env, napi_callback_info info)
{
    auto ret = GetNativePredicates(env, info)->GetWhereClause();
    return JSUtils::Convert2JSValue(env, ret);
}

napi_value DataAbilityPredicatesProxy::SetWhereClause(napi_env env, napi_callback_info info)
{
    napi_value thiz;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::SetWhereClause Invalid argvs!");

    std::string whereClause = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    GetNativePredicates(env, info)->SetWhereClause(whereClause);

    return thiz;
}

napi_value DataAbilityPredicatesProxy::GetWhereArgs(napi_env env, napi_callback_info info)
{
    auto ret = GetNativePredicates(env, info)->GetWhereArgs();
    return JSUtils::Convert2JSValue(env, ret);
}

napi_value DataAbilityPredicatesProxy::SetWhereArgs(napi_env env, napi_callback_info info)
{
    napi_value thiz;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::SetWhereArgs Invalid argvs!");

    std::vector<std::string> whereArgs = JSUtils::Convert2StrVector(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    GetNativePredicates(env, info)->SetWhereArgs(whereArgs);

    return thiz;
}

napi_value DataAbilityPredicatesProxy::GetOrder(napi_env env, napi_callback_info info)
{
    auto ret = GetNativePredicates(env, info)->GetOrder();
    return JSUtils::Convert2JSValue(env, ret);
}

napi_value DataAbilityPredicatesProxy::SetOrder(napi_env env, napi_callback_info info)
{
    napi_value thiz;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    napi_get_cb_info(env, info, &argc, args, &thiz, nullptr);
    NAPI_ASSERT(env, argc > 0, "DataAbilityPredicatesProxy::SetOrder Invalid argvs!");

    std::string order = JSUtils::Convert2String(env, args[0], JSUtils::DEFAULT_BUF_SIZE);
    GetNativePredicates(env, info)->SetOrder(order);

    return thiz;
}

napi_value DataAbilityPredicatesProxy::GetLimit(napi_env env, napi_callback_info info)
{
    return JSUtils::Convert2JSValue(env, GetNativePredicates(env, info)->GetLimit());
}

napi_value DataAbilityPredicatesProxy::GetOffset(napi_env env, napi_callback_info info)
{
    return JSUtils::Convert2JSValue(env, GetNativePredicates(env, info)->GetOffset());
}

napi_value DataAbilityPredicatesProxy::IsDistinct(napi_env env, napi_callback_info info)
{
    return JSUtils::Convert2JSValue(env, GetNativePredicates(env, info)->IsDistinct());
}

napi_value DataAbilityPredicatesProxy::GetGroup(napi_env env, napi_callback_info info)
{
    auto ret = GetNativePredicates(env, info)->GetGroup();
    return JSUtils::Convert2JSValue(env, ret);
}

napi_value DataAbilityPredicatesProxy::GetIndex(napi_env env, napi_callback_info info)
{
    auto ret = GetNativePredicates(env, info)->GetIndex();
    return JSUtils::Convert2JSValue(env, ret);
}

napi_value DataAbilityPredicatesProxy::IsNeedAnd(napi_env env, napi_callback_info info)
{
    return JSUtils::Convert2JSValue(env, GetNativePredicates(env, info)->IsNeedAnd());
}

napi_value DataAbilityPredicatesProxy::IsSorted(napi_env env, napi_callback_info info)
{
    return JSUtils::Convert2JSValue(env, GetNativePredicates(env, info)->IsSorted());
}
} // namespace DataAbilityJsKit
} // namespace OHOS

EXTERN_C_START
__attribute__((visibility("default"))) napi_value NAPI_OHOS_Data_DataAbilityJsKit_DataAbilityPredicatesProxy_NewInstance(
    napi_env env, OHOS::NativeRdb::DataAbilityPredicates *predicates)
{
    return OHOS::DataAbilityJsKit::DataAbilityPredicatesProxy::NewInstance(
        env, std::shared_ptr<OHOS::NativeRdb::DataAbilityPredicates>(predicates));
}

__attribute__((visibility("default"))) OHOS::NativeRdb::DataAbilityPredicates *
NAPI_OHOS_Data_DataAbilityJsKit_DataAbilityPredicatesProxy_GetNativeObject(const napi_env &env, const napi_value &arg)
{
    auto predicates = OHOS::DataAbilityJsKit::DataAbilityPredicatesProxy::GetNativePredicates(env, arg);
    return predicates.get();
}
EXTERN_C_END