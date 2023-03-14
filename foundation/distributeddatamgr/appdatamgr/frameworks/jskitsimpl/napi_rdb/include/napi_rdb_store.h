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

#ifndef RDB_JSKIT_NAPI_RDB_STORE_H
#define RDB_JSKIT_NAPI_RDB_STORE_H

#include <list>
#include <mutex>
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "rdb_helper.h"
#include "napi_rdb_store_observer.h"
#include "rdb_types.h"

namespace OHOS {
namespace RdbJsKit {
class RdbStoreProxy {
public:
    static void Init(napi_env env, napi_value exports);
    static napi_value NewInstance(napi_env env, std::shared_ptr<OHOS::NativeRdb::RdbStore> value);
    static RdbStoreProxy *GetNativeInstance(napi_env env, napi_value self);
    void Release(napi_env env);
    RdbStoreProxy();
    ~RdbStoreProxy();

private:
    static napi_value Initialize(napi_env env, napi_callback_info info);
    static napi_value Delete(napi_env env, napi_callback_info info);
    static napi_value Update(napi_env env, napi_callback_info info);
    static napi_value Insert(napi_env env, napi_callback_info info);
    static napi_value Query(napi_env env, napi_callback_info info);
    static napi_value QuerySql(napi_env env, napi_callback_info info);
    static napi_value ExecuteSql(napi_env env, napi_callback_info info);
    static napi_value Backup(napi_env env, napi_callback_info info);
    static napi_value Count(napi_env env, napi_callback_info info);
    static napi_value Replace(napi_env env, napi_callback_info info);
    static napi_value Attach(napi_env env, napi_callback_info info);
    static napi_value GetPath(napi_env env, napi_callback_info info);
    static napi_value IsMemoryRdb(napi_env env, napi_callback_info info);
    static napi_value IsHoldingConnection(napi_env env, napi_callback_info info);
    static napi_value IsReadOnly(napi_env env, napi_callback_info info);
    static napi_value BeginTransaction(napi_env env, napi_callback_info info);
    static napi_value RollBack(napi_env env, napi_callback_info info);
    static napi_value Commit(napi_env env, napi_callback_info info);
    static napi_value QueryByStep(napi_env env, napi_callback_info info);
    static napi_value IsInTransaction(napi_env env, napi_callback_info info);
    static napi_value IsOpen(napi_env env, napi_callback_info info);
    static napi_value GetVersion(napi_env env, napi_callback_info info);
    static napi_value SetVersion(napi_env env, napi_callback_info info);
    static napi_value MarkAsCommit(napi_env env, napi_callback_info info);
    static napi_value EndTransaction(napi_env env, napi_callback_info info);
    static napi_value ChangeDbFileForRestore(napi_env env, napi_callback_info info);
    static napi_value ChangeEncryptKey(napi_env env, napi_callback_info info);
    static napi_value SetDistributedTables(napi_env env, napi_callback_info info);
    static napi_value ObtainDistributedTableName(napi_env env, napi_callback_info info);
    static napi_value Sync(napi_env env, napi_callback_info info);
    static napi_value OnEvent(napi_env env, napi_callback_info info);
    static napi_value OffEvent(napi_env env, napi_callback_info info);

    static constexpr int MIN_ON_EVENT_ARG_NUM = 2;
    static constexpr int MAX_ON_EVENT_ARG_NUM = 5;

    void OnDataChangeEvent(napi_env env, size_t argc, napi_value* argv);
    void OffDataChangeEvent(napi_env env, size_t argc, napi_value* argv);

    std::mutex mutex_;
    std::shared_ptr<OHOS::NativeRdb::RdbStore> rdbStore_;
    napi_ref ref_ = nullptr;
    std::list<std::shared_ptr<NapiRdbStoreObserver>> observers_[DistributedRdb::SUBSCRIBE_MODE_MAX];
};
} // namespace RdbJsKit
} // namespace OHOS

#endif // RDB_JSKIT_NAPI_RDB_STORE_H
