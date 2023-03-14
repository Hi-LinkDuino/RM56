/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_BACKEND_DELEGATE_H
#define FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_BACKEND_DELEGATE_H

#include <vector>

#include "base/json/json_util.h"
#include "base/memory/ace_type.h"
#include "base/utils/noncopyable.h"
#include "core/common/backend.h"
#include "core/event/ace_event_helper.h"
#include "frameworks/bridge/js_frontend/engine/common/group_js_bridge.h"

namespace OHOS::Ace {

class BackendDelegate : public AceType {
    DECLARE_ACE_TYPE(BackendDelegate, AceType);

public:
    BackendDelegate() = default;
    ~BackendDelegate() override = default;

    // posting js task from jsengine
    virtual void PostJsTask(std::function<void()>&& task) = 0;

    // posting js task from jsengine
    virtual void PostDelayedJsTask(std::function<void()>&& task, uint32_t delayTime) = 0;

    virtual BackendType GetType() const = 0;

    virtual SingleTaskExecutor GetAnimationJsTask() = 0;

    virtual bool GetAssetContent(const std::string& url, std::string& content) = 0;
    virtual bool GetAssetContent(const std::string& url, std::vector<uint8_t>& content) = 0;
    virtual bool GetResourceData(const std::string& fileUri, std::vector<uint8_t>& content, std::string& ami) = 0;
    virtual std::string GetAssetPath(const std::string& url) = 0;

    virtual void AddTaskObserver(std::function<void()>&& task) = 0;
    virtual void RemoveTaskObserver() = 0;

    virtual void SetCallBackResult(const std::string& callBackId, const std::string& result) = 0;

    virtual const RefPtr<GroupJsBridge>& GetGroupJsBridge() = 0;

    ACE_EXPORT void SetAssetManager(const RefPtr<AssetManager>& assetManager)
    {
        assetManager_ = assetManager;
    }

    ACE_EXPORT RefPtr<AssetManager> GetAssetManager() const
    {
        return assetManager_;
    }

protected:
    RefPtr<AssetManager> assetManager_;

    ACE_DISALLOW_COPY_AND_MOVE(BackendDelegate);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_BACKEND_DELEGATE_H
