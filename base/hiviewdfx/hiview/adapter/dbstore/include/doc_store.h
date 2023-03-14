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

#ifndef HIVIEW_ADAPTER_DBSTORE_INCLUDE_DOC_STORE_H
#define HIVIEW_ADAPTER_DBSTORE_INCLUDE_DOC_STORE_H

#include <functional>
#include <memory>
#include <vector>

#include "data_query.h"
#include "types.h"
#include "visibility.h"

namespace OHOS {
namespace HiviewDFX {
class DocDB;
class DocStore {
public:
    DOCSTORE_API DocStore() {}

    DOCSTORE_API ~DocStore() {}

    DOCSTORE_API int Put(const Entry &entry);

    DOCSTORE_API int PutBatch(const std::vector<Entry> &entries);

    DOCSTORE_API int Merge(const Entry &entry);

    DOCSTORE_API int Delete(const DataQuery &query);

    DOCSTORE_API int GetEntriesWithQuery(const DataQuery &query, std::vector<Entry> &entries) const;

    DOCSTORE_API int GetEntryDuringQuery(const DataQuery &query,
        std::function<int (int, const Entry&)> callback) const;
private:
    friend class StoreManager;
    std::shared_ptr<DocDB> dbPtr;
}; // DocStore
} // HiviewDFX
} // OHOS
#endif // HIVIEW_ADAPTER_DBSTORE_INCLUDE_DOC_STORE_H