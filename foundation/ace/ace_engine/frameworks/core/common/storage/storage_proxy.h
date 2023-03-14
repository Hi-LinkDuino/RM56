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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_STORAGE_STORAGE_PROXY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_STORAGE_STORAGE_PROXY_H

#include "base/utils/singleton.h"
#include "core/common/storage/storage_interface.h"

namespace OHOS::Ace {

class ACE_EXPORT StorageProxy : public StorageInterface {
public:
    ACE_EXPORT static StorageProxy* GetInstance();
    void SetDelegate(std::unique_ptr<StorageInterface>&& delegate);
    RefPtr<Storage> GetStorage(const RefPtr<TaskExecutor>& taskExecutor) const override;
    StorageProxy() = default;
    ~StorageProxy() = default;

private:
    std::unique_ptr<StorageInterface> delegate_;
    static StorageProxy* inst_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_STORAGE_STORAGE_PROXY_H