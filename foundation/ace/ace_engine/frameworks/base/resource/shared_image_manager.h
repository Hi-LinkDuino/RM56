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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_RESOURCE_SHARED_IMAGE_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_RESOURCE_SHARED_IMAGE_MANAGER_H

#include <mutex>
#include <set>
#include <unordered_map>
#include <vector>

#include "base/memory/ace_type.h"
#include "base/thread/cancelable_callback.h"
#include "base/thread/task_executor.h"

namespace OHOS::Ace {

using SharedImage = std::vector<uint8_t>;
using SharedImageMap = std::unordered_map<std::string, SharedImage>;

class ImageProviderLoader;
using ProviderMapToReload = std::unordered_map<std::string, std::set<WeakPtr<ImageProviderLoader>>>;

class SharedImageManager : public AceType {
    DECLARE_ACE_TYPE(SharedImageManager, AceType);

public:
    explicit SharedImageManager(const RefPtr<TaskExecutor>& taskExecutor) : taskExecutor_(taskExecutor) {}
    ~SharedImageManager() override = default;
    void AddSharedImage(const std::string& name, SharedImage&& sharedImage);
    void AddPictureNamesToReloadMap(std::string&& name);
    bool AddProviderToReloadMap(const std::string& name, const WeakPtr<ImageProviderLoader>& providerWp);
    bool FindImageInSharedImageMap(const std::string& name, const WeakPtr<ImageProviderLoader>& providerWp);

    const SharedImageMap& GetSharedImageMap() const
    {
        return sharedImageMap_;
    }

    bool Remove(const std::string& name)
    {
        int res = static_cast<int>(sharedImageMap_.erase(name));
        return (res != 0);
    }

    bool IsResourceToReload(const std::string& name)
    {
        std::lock_guard<std::mutex> lockProviderMap(providerMapMutex_);
        return providerMapToReload_.find(name) != providerMapToReload_.end();
    }

private:
    void PostDelayedTaskToClearImageData(const std::string& name, size_t dataSize);
    std::function<void()> GenerateClearImageDataCallback(const std::string& name, size_t dataSize);

    std::mutex sharedImageMapMutex_;
    std::mutex providerMapMutex_;
    std::mutex cancelableCallbackMapMutex_;
    SharedImageMap sharedImageMap_;
    ProviderMapToReload providerMapToReload_;
    std::function<void()> clearImageDataCallback_;
    std::unordered_map<std::string, CancelableCallback<void()>> cancelableCallbackMap_;
    RefPtr<TaskExecutor> taskExecutor_;
};

class ImageProviderLoader : public virtual AceType {
    DECLARE_ACE_TYPE(ImageProviderLoader, AceType);

public:
    ~ImageProviderLoader() override = default;
    virtual void UpdateData(const std::string& uri, const std::vector<uint8_t>& memData) = 0;

protected:
    ImageProviderLoader() = default;

    ACE_DISALLOW_COPY_AND_MOVE(ImageProviderLoader);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_RESOURCE_SHARED_IMAGE_MANAGER_H
