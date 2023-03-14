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

#ifndef HIVIEW_ADAPTER_DBSTORE_INCLUDE_SOTRE_MANAGER_H
#define HIVIEW_ADAPTER_DBSTORE_INCLUDE_SOTRE_MANAGER_H

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "doc_store.h"
#include "visibility.h"

namespace OHOS {
namespace HiviewDFX {
class Option {
public:
    enum Flag {
        RDONLY,
        TRUNC,
        NO_TRIM_ON_CLOSE
    };

    std::string db;
    Flag flag;
};

class StoreManager {
public:
    DOCSTORE_API StoreManager() {}

    DOCSTORE_API ~StoreManager() {}

    DOCSTORE_API std::shared_ptr<DocStore> GetDocStore(const Option& option);

    DOCSTORE_API int OnlineBackupDocStore(const Option& option, const std::string &bakFile);

    DOCSTORE_API int CloseDocStore(const Option& option);

    DOCSTORE_API int DeleteDocStore(const Option& option);
private:
    int InnerCloseDocStore(const Option& option);
    std::mutex mutex_;
    std::map<std::string, std::shared_ptr<DocStore>> stores_;
}; // StoreManager
} // HiviewDFX
} // OHOS
#endif // HIVIEW_ADAPTER_DBSTORE_INCLUDE_SOTRE_MANAGER_H