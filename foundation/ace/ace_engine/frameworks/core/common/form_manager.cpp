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

#include "core/common/form_manager.h"

#include "unistd.h"

#include "base/log/log.h"

namespace OHOS::Ace {

FormManager::FormManager() {}

FormManager::~FormManager() {}

void FormManager::AddSubContainer(int64_t formId, const RefPtr<SubContainer>& subContainer)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto result = subContainerMap_.try_emplace(formId, subContainer);
    if (!result.second) {
        LOGW("already have subContainer of this instance");
    }
}

void FormManager::RemoveSubContainer(int64_t formId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    subContainerMap_.erase(formId);
}

RefPtr<SubContainer> FormManager::GetSubContainer(int64_t formId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto subContainer = subContainerMap_.find(formId);
    if (subContainer != subContainerMap_.end()) {
        return subContainer->second;
    } else {
        return nullptr;
    }
}

void FormManager::AddNonmatchedContainer(const std::string& cardKey, const RefPtr<SubContainer>& subContainer)
{
    std::lock_guard<std::mutex> lock(nonmatchedContainerMutex_);
    auto result = nonmatchedContainerMap_.try_emplace(cardKey, subContainer);
    if (!result.second) {
        LOGW("already have subContainer of this key: %{public}s", cardKey.c_str());
    }
}

RefPtr<SubContainer> FormManager::MatchSubContainerWithFormId(int64_t formId, const std::string& cardKey)
{
    std::lock_guard<std::mutex> lock(nonmatchedContainerMutex_);
    auto iter = nonmatchedContainerMap_.find(cardKey);
    if (iter == nonmatchedContainerMap_.end()) {
        LOGW("no subcontainer of key: %{private}s", cardKey.c_str());
        return nullptr;
    }
    auto subContainer = iter->second;
    AddSubContainer(formId, subContainer);
    nonmatchedContainerMap_.erase(iter);
    return subContainer;
}
void FormManager::Dump(const std::vector<std::string>& params)
{
#ifdef FORM_SUPPORTED
    std::unordered_map<std::string, RefPtr<SubContainer>> copied;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        copied = nonmatchedContainerMap_;
    }
    for (const auto& container : copied) {
        auto pipelineContext = container.second->GetPipelineContext();
        if (!pipelineContext) {
            LOGW("the pipeline context is nullptr, pa container");
            continue;
        }
        pipelineContext->GetTaskExecutor()->PostSyncTask(
            [params, container = container.second]() { container->Dump(params); }, TaskExecutor::TaskType::UI);
    }
#endif
}

} // namespace OHOS::Ace
