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

#ifndef NATIVE_RDB_ABS_SHARED_RESULT_SET_H
#define NATIVE_RDB_ABS_SHARED_RESULT_SET_H

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "abs_result_set.h"
#include "message_parcel.h"
#include "parcel.h"
#include "shared_block.h"
#include "shared_result_set.h"

namespace OHOS {
namespace NativeRdb {
class AbsSharedResultSet : public AbsResultSet, public SharedResultSet {
public:
    AbsSharedResultSet();
    explicit AbsSharedResultSet(std::string name);
    virtual ~AbsSharedResultSet();
    int GetBlob(int columnIndex, std::vector<uint8_t> &blob) override;
    int GetString(int columnIndex, std::string &value) override;
    int GetInt(int columnIndex, int &value) override;
    int GetLong(int columnIndex, int64_t &value) override;
    int GetDouble(int columnIndex, double &value) override;
    int IsColumnNull(int columnIndex, bool &isNull) override;
    int GetColumnType(int columnIndex, ColumnType &columnType) override;
    int GoToRow(int position) override;
    int GetAllColumnNames(std::vector<std::string> &columnNames) override;
    int GetRowCount(int &count) override;
    AppDataFwk::SharedBlock *GetBlock() const override;
    bool OnGo(int oldRowIndex, int newRowIndex) override;
    void FillBlock(int startRowIndex, AppDataFwk::SharedBlock *block) override;
    virtual void SetBlock(AppDataFwk::SharedBlock *block);
    int Close() override;
    bool HasBlock() const;

protected:
    int CheckState(int columnIndex);
    void ClearBlock();
    void ClosedBlock();
    virtual void Finalize();

    friend class ISharedResultSetStub;
    friend class ISharedResultSetProxy;
    bool Unmarshalling(MessageParcel &parcel);
    bool Marshalling(MessageParcel &parcel);
    
private:
    // The default position of the cursor
    static const int INIT_POS = -1;
    static const size_t DEFAULT_BLOCK_SIZE = 2 * 1024 * 1024;

    // The SharedBlock owned by this AbsSharedResultSet
    AppDataFwk::SharedBlock *sharedBlock_  = nullptr;
};
} // namespace NativeRdb
} // namespace OHOS

#endif