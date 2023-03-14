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

#ifndef SHARED_BLOCK_SERIALIZER_INFO_H
#define SHARED_BLOCK_SERIALIZER_INFO_H

#include <sqlite3sym.h>

#include "shared_block.h"

namespace OHOS {
namespace NativeRdb {
#ifdef __cplusplus
extern "C" {
#endif
class SharedBlockSerializerInfo {
public:
    SharedBlockSerializerInfo(AppDataFwk::SharedBlock *sharedBlock, int numColumns, int startPos);
    ~SharedBlockSerializerInfo();
    int AddRow(int addedRows);
    int Reset(int startPos);
    int Finish(int addedRows, int totalRows);
    int PutString(int row, int column, const char *text, int sizeIncludingNull);
    int PutLong(int row, int column, sqlite3_int64 value);
    int PutDouble(int row, int column, double value);
    int PutBlob(int row, int column, const void *blob, int len);
    int PutNull(int row, int column);
    int PutOther(int row, int column);
    int GetTotalRows() const;
    int GetAddedRows() const;
    int GetStartPos() const;
private:
    AppDataFwk::SharedBlock *sharedBlock_;
    int anumColumns;
    int atotalRows;
    int astartPos;
    int raddedRows;
    bool risFull;
};

struct Sqlite3SharedBlockMethods {
    int iVersion;
    void *pContext;
    int countAllRows;
    int startPos;
    int requiredPos;
    int (*xAddRow)(void *pCtx, int addedRows);
    int (*xReset)(void *pCtx, int startPos);
    int (*xFinish)(void *pCtx, int addedRows, int totalRows);
    int (*xPutString)(void *pCtx, int addedRows, int column, const char *text, int len);
    int (*xPutLong)(void *pCtx, int addedRows, int column, sqlite3_int64 value);
    int (*xPutDouble)(void *pCtx, int addedRows, int column, double value);
    int (*xPutBlob)(void *pCtx, int addedRows, int column, const void *blob, int len);
    int (*xPutNull)(void *pCtx, int addedRows, int column);
    int (*xPutOther)(void *pCtx, int addedRows, int column);
    // Additional methods may be added in future releases
};

static const int SQLITE_DBCONFIG_SET_SHAREDBLOCK = 2004;

static const int SQLITE_DBCONFIG_USE_SHAREDBLOCK = 2005;
#ifdef __cplusplus
}
#endif
} // namespace NativeRdb
} // namespace OHOS

#endif
