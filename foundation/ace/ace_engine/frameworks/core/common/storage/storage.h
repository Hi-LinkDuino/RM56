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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_STORAGE_STORAGE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_STORAGE_STORAGE_H

#include "base/memory/ace_type.h"
#include "base/thread/task_executor.h"

namespace OHOS::Ace {

class Storage : public AceType {
    DECLARE_ACE_TYPE(Storage, AceType);

public:
    ~Storage() override = default;

    virtual void Set(const std::string& key, const std::string& value) = 0;
    virtual std::string Get(const std::string& key) = 0;
    virtual void Clear() = 0;
    virtual void Delete(const std::string& key) = 0;

protected:
    explicit Storage(const RefPtr<TaskExecutor>& taskExecutor) : taskExecutor_(taskExecutor) {}
    RefPtr<TaskExecutor> taskExecutor_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_STORAGE_STORAGE_H
