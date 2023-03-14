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

#include "core/event/multimodal/multimodal_manager.h"

#include "base/log/dump_log.h"
#include "base/log/log.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

RefPtr<MultiModalScene> MultiModalManager::GetMultiModalScene(int32_t pageId)
{
    auto iter = multiModalScenes_.find(pageId);
    if (iter == multiModalScenes_.end()) {
        auto newScene = multiModalScenes_.try_emplace(pageId, MakeRefPtr<MultiModalScene>(pageId, subscriber_));
        return newScene.first->second;
    }
    return iter->second;
}

void MultiModalManager::PushActiveScene(int32_t newPageId)
{
    if (activeSceneId_ == newPageId) {
        return;
    }
    if (activeSceneId_ > 0) {
        GetMultiModalScene(activeSceneId_)->Hide();
    }
    activeSceneId_ = newPageId;
}

void MultiModalManager::PopActiveScene(int32_t poppedPageId, int32_t incomingPageId)
{
    RemoveMultimodalScene(poppedPageId);
    if (activeSceneId_ == incomingPageId) {
        return;
    }
    GetMultiModalScene(incomingPageId)->Resume();
    activeSceneId_ = incomingPageId;
}

RefPtr<MultiModalScene> MultiModalManager::GetCurrentMultiModalScene()
{
    if (activeSceneId_ < 0) {
        return nullptr;
    }
    return GetMultiModalScene(activeSceneId_);
}

void MultiModalManager::ReplaceActiveScene(int32_t newPageId, int32_t replaceId)
{
    RemoveMultimodalScene(replaceId);
    if (activeSceneId_ == newPageId) {
        return;
    }
    activeSceneId_ = newPageId;
}

void MultiModalManager::RemoveMultimodalScene(int32_t pageId)
{
    multiModalScenes_.erase(pageId);
}

void MultiModalManager::RemoveInactiveScene(int32_t pageId)
{
    if (activeSceneId_ == pageId) {
        LOGW("fail to remove scene due to it is active");
        return;
    }
    RemoveMultimodalScene(pageId);
}

void MultiModalManager::SetMultimodalSubscriber(const RefPtr<MultimodalSubscriber>& subscriber)
{
    if (subscriber) {
        subscriber_ = subscriber;
    }
}

void MultiModalManager::OpenChannel(const RefPtr<PipelineContext>& context)
{
    if (!subscriber_) {
        LOGE("fail to get subscriber to register callback");
        return;
    }
    if (isRegistered_) {
        return;
    }
    auto callback = [weak = WeakClaim(this), weakContext = WeakPtr<PipelineContext>(context)](
                        const AceMultimodalEvent& event) {
        auto manager = weak.Upgrade();
        if (!manager) {
            LOGE("fail to get manager to notify event");
            return;
        }
        auto context = weakContext.Upgrade();
        if (!context) {
            LOGE("fail to get context to notify event");
            return;
        }
        auto scene = manager->GetCurrentMultiModalScene();
        if (!scene) {
            LOGE("fail to get current multi modal scene");
            return;
        }
        auto weakScene = AceType::WeakClaim(AceType::RawPtr(scene));
        context->GetTaskExecutor()->PostSyncTask([event, weakScene]() {
            auto scene = weakScene.Upgrade();
            if (scene == nullptr) {
                LOGE("scene is null!");
                return;
            }
            scene->OnNotifyMultimodalEvent(event);
        }, TaskExecutor::TaskType::UI);
    };
    subscriber_->RegisterCallback(callback, [weak = WeakClaim(this)]() {
        auto manager = weak.Upgrade();
        if (!manager) {
            LOGE("fail to get manager to notify event");
            return;
        }
        manager->isRegistered_ = true;
    });
}

void MultiModalManager::DumpMultimodalScene()
{
    if (DumpLog::GetInstance().GetDumpFile()) {
        DumpLog::GetInstance().AddDesc("ActiveScene: ", activeSceneId_);
        DumpLog::GetInstance().AddDesc("totalScene: ", multiModalScenes_.size());
        DumpLog::GetInstance().AddDesc("registered: ", isRegistered_);
        DumpLog::GetInstance().Print(0, GetTypeName(), multiModalScenes_.size());
    }
    GetCurrentMultiModalScene()->Dump();
}

} // namespace OHOS::Ace