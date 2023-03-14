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

#ifndef RDB_JSKIT_NAPI_RDB_PREDICATES_H
#define RDB_JSKIT_NAPI_RDB_PREDICATES_H

#include <memory>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "rdb_predicates.h"

namespace OHOS {
namespace RdbJsKit {
class RdbPredicatesProxy {
public:
    static void Init(napi_env env, napi_value exports);
    static napi_value NewInstance(napi_env env, std::shared_ptr<NativeRdb::RdbPredicates> value);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

    explicit RdbPredicatesProxy(std::string &tableName);
    std::shared_ptr<NativeRdb::RdbPredicates> GetPredicates() const;
    
private:
    ~RdbPredicatesProxy();

    static napi_value New(napi_env env, napi_callback_info info);
    static std::shared_ptr<NativeRdb::RdbPredicates> GetNativePredicates(napi_env env, napi_callback_info info);

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
    static napi_value Using(napi_env env, napi_callback_info info);
    static napi_value Clear(napi_env env, napi_callback_info info);
    static napi_value CrossJoin(napi_env env, napi_callback_info info);
    static napi_value LeftOuterJoin(napi_env env, napi_callback_info info);
    static napi_value GetJoinCount(napi_env env, napi_callback_info info);
    static napi_value SetJoinConditions(napi_env env, napi_callback_info info);
    static napi_value SetJoinTableNames(napi_env env, napi_callback_info info);
    static napi_value SetJoinTypes(napi_env env, napi_callback_info info);
    static napi_value SetJoinCount(napi_env env, napi_callback_info info);
    static napi_value GetJoinTypes(napi_env env, napi_callback_info info);
    static napi_value GetJoinTableNames(napi_env env, napi_callback_info info);
    static napi_value GetJoinConditions(napi_env env, napi_callback_info info);
    static napi_value InnerJoin(napi_env env, napi_callback_info info);
    static napi_value On(napi_env env, napi_callback_info info);
    static napi_value InDevices(napi_env env, napi_callback_info info);
    static napi_value InAllDevices(napi_env env, napi_callback_info info);

    std::shared_ptr<NativeRdb::RdbPredicates> predicates_;
    napi_env env_;
    napi_ref wrapper_;
};
} // namespace RdbJsKit
} // namespace OHOS

#endif // RDB_JSKIT_NAPI_RDB_PREDICATES_H
