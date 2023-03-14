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

#ifndef HIVIEW_BASE_EVENT_STORE_STORE_MGR_PROXY_H
#define HIVIEW_BASE_EVENT_STORE_STORE_MGR_PROXY_H

#include <memory>
#include <string>

#include "singleton.h"
#include "store_manager.h"

namespace OHOS {
namespace HiviewDFX {
class StoreMgrProxy : public OHOS::DelayedRefSingleton<StoreMgrProxy> {
public:
    StoreMgrProxy() {}
    ~StoreMgrProxy() {}

    std::shared_ptr<DocStore> GetDocStore(const std::string& db);
    int BackupDocStore(const std::string& db, const std::string &backupFile);
    int CloseDocStore(const std::string& db);
    int DeleteDocStore(const std::string& db);
private:
    StoreManager storeManager_;
}; // StoreInsManager
} // HiviewDFX
} // OHOS
#endif // HIVIEW_BASE_EVENT_STORE_STORE_MGR_PROXY_H