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

#ifndef DATAABILITY_JSKIT_NAPI_DATA_ABILITY_PREDICATES_H
#define DATAABILITY_JSKIT_NAPI_DATA_ABILITY_PREDICATES_H

#include <memory>
#include "data_ability_predicates.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace DataAbilityJsKit {
class DataAbilityPredicatesProxy {
public:
    static void Init(napi_env env, napi_value exports);
    static napi_value NewInstance(napi_env env, std::shared_ptr<NativeRdb::DataAbilityPredicates> value);
    static std::shared_ptr<NativeRdb::DataAbilityPredicates> GetNativePredicates(
        const napi_env &env, const napi_value &arg);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

    DataAbilityPredicatesProxy();
    std::shared_ptr<NativeRdb::DataAbilityPredicates> GetPredicates() const;

private:
    ~DataAbilityPredicatesProxy();

    static napi_value New(napi_env env, napi_callback_info info);
    static std::shared_ptr<NativeRdb::DataAbilityPredicates> GetNativePredicates(napi_env env, napi_callback_info info);
    static napi_value EqualTo(napi_env env, napi_callback_info info);
    static napi_value NotEqualTo(napi_env env, napi_callback_info info);
    static napi_value BeginWrap(napi_env env, napi_callback_info info);
    static napi_value EndWrap(napi_env env, napi_callback_info info);
    static napi_value Or(napi_env env, napi_callback_info info);
    static napi_value And(napi_env env, napi_callback_info info);
    static napi_value Contains(napi_env env, napi_callback_info info);
    static napi_value BeginsWith(napi_env env, napi_callback_info info);
    static napi_value EndsWith(napi_env env, napi_callback_info info);
    static napi_value IsNull(napi_env env, napi_callback_info info);
    static napi_value IsNotNull(napi_env env, napi_callback_info info);
    static napi_value Like(napi_env env, napi_callback_info info);
    static napi_value Glob(napi_env env, napi_callback_info info);
    static napi_value Between(napi_env env, napi_callback_info info);
    static napi_value NotBetween(napi_env env, napi_callback_info info);
    static napi_value GreaterThan(napi_env env, napi_callback_info info);
    static napi_value LessThan(napi_env env, napi_callback_info info);
    static napi_value GreaterThanOrEqualTo(napi_env env, napi_callback_info info);
    static napi_value LessThanOrEqualTo(napi_env env, napi_callback_info info);
    static napi_value OrderByAsc(napi_env env, napi_callback_info info);
    static napi_value OrderByDesc(napi_env env, napi_callback_info info);
    static napi_value Distinct(napi_env env, napi_callback_info info);
    static napi_value Limit(napi_env env, napi_callback_info info);
    static napi_value Offset(napi_env env, napi_callback_info info);
    static napi_value GroupBy(napi_env env, napi_callback_info info);
    static napi_value IndexedBy(napi_env env, napi_callback_info info);
    static napi_value In(napi_env env, napi_callback_info info);
    static napi_value NotIn(napi_env env, napi_callback_info info);
    static napi_value Clear(napi_env env, napi_callback_info info);
    static napi_value IsRawSelection(napi_env env, napi_callback_info info);
    static napi_value GetWhereClause(napi_env env, napi_callback_info info);
    static napi_value SetWhereClause(napi_env env, napi_callback_info info);
    static napi_value GetWhereArgs(napi_env env, napi_callback_info info);
    static napi_value SetWhereArgs(napi_env env, napi_callback_info info);
    static napi_value GetOrder(napi_env env, napi_callback_info info);
    static napi_value SetOrder(napi_env env, napi_callback_info info);
    static napi_value GetLimit(napi_env env, napi_callback_info info);
    static napi_value GetOffset(napi_env env, napi_callback_info info);
    static napi_value IsDistinct(napi_env env, napi_callback_info info);
    static napi_value GetGroup(napi_env env, napi_callback_info info);
    static napi_value GetIndex(napi_env env, napi_callback_info info);
    static napi_value IsNeedAnd(napi_env env, napi_callback_info info);
    static napi_value IsSorted(napi_env env, napi_callback_info info);
    std::shared_ptr<NativeRdb::DataAbilityPredicates> predicates_;
    napi_env env_;
    napi_ref wrapper_;
};
} // namespace DataAbilityJsKit
} // namespace OHOS

EXTERN_C_START
__attribute__((visibility("default"))) napi_value NAPI_OHOS_Data_DataAbilityJsKit_DataAbilityPredicatesProxy_NewInstance(
    napi_env env, OHOS::NativeRdb::DataAbilityPredicates *predicates);

__attribute__((visibility("default"))) OHOS::NativeRdb::DataAbilityPredicates *
NAPI_OHOS_Data_DataAbilityJsKit_DataAbilityPredicatesProxy_GetNativeObject(
    const napi_env &env, const napi_value &arg);
EXTERN_C_END
#endif // DATAABILITY_JSKIT_NAPI_DATA_ABILITY_PREDICATES_H
