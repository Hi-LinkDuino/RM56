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

#ifndef NATIVE_RDB_RDB_HELPER_H
#define NATIVE_RDB_RDB_HELPER_H

#include <memory>
#include <string>
#include <mutex>
#include "rdb_open_callback.h"
#include "rdb_store.h"
#include "rdb_store_config.h"

namespace OHOS {
namespace NativeRdb {

class RdbHelper final {
public:
    static std::shared_ptr<RdbStore> GetRdbStore(
        const RdbStoreConfig &config, int version, RdbOpenCallback &openCallback, int &errCode);
    static int DeleteRdbStore(const std::string &path);
    static void ClearCache();

private:
    static int ProcessOpenCallback(
        RdbStore &rdbStore, const RdbStoreConfig &config, int version, RdbOpenCallback &openCallback);
    static std::mutex mutex_;
    static std::map<std::string, std::shared_ptr<RdbStore>> storeCache_;
};

} // namespace NativeRdb
} // namespace OHOS
#endif
