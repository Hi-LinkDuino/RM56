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


#ifndef NATIVE_RDB_SQLITE_SHARED_RESULT_SET_H
#define NATIVE_RDB_SQLITE_SHARED_RESULT_SET_H

#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include "rdb_store_impl.h"
#include "sqlite_statement.h"
#include "shared_block.h"
#include "abs_shared_result_set.h"
#include "value_object.h"

namespace OHOS {
namespace NativeRdb {
class SqliteSharedResultSet : public AbsSharedResultSet {
public:
    SqliteSharedResultSet(std::shared_ptr<RdbStoreImpl> rdbSreImpl, std::string path, std::string sql,
        const std::vector<std::string> &selectionArgVec);
    ~SqliteSharedResultSet() override;
    int GetAllColumnNames(std::vector<std::string> &columnNames) override;
    int Close() override;
    int GetRowCount(int &count) override;
    bool OnGo(int oldPosition, int newPosition) override;
    void SetBlock(AppDataFwk::SharedBlock *block) override;
    std::shared_ptr<RdbStore> GetRdbStore() const;
    int PickFillBlockStartPosition(int resultSetPosition, int blockCapacity) const;
    void SetFillBlockForwardOnly(bool isOnlyFillResultSetBlockInput);

protected:
    void Finalize() override;

private:
    int PrepareStep();
    int CheckSession();
    void FillSharedBlock(int requiredPos);

private:
    // The specified value is -1 when there is no data
    static const int NO_COUNT = -1;
    // The pick position of the shared block for search
    static const int PICK_POS = 3;
    // The number of rows that can fit in the shared block, 0 if unknown
    int resultSetBlockCapacity;
    // Controls fetching of rows relative to requested position
    bool isOnlyFillResultSetBlock;
    std::mutex sessionMutex;
    std::shared_ptr<RdbStoreImpl> rdbStoreImpl;
    std::string qrySql;
    std::vector<std::string> selectionArgVec;
    std::shared_ptr<SqliteStatement> sqliteStatement;
    std::thread::id tid;
    // The number of rows in the cursor
    int rowNum;
};
} // namespace NativeRdb
} // namespace OHOS

#endif