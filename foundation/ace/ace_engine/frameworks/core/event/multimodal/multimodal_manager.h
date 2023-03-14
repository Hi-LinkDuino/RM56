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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_MULTIMODAL_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_MULTIMODAL_MANAGER_H

#include "base/memory/ace_type.h"
#include "core/event/multimodal/multimodal_scene.h"
#include "core/event/multimodal/multimodal_subscriber.h"

namespace OHOS::Ace {

class PipelineContext;

class MultiModalManager final : public AceType {
    DECLARE_ACE_TYPE(MultiModalManager, AceType)

public:
    MultiModalManager() = default;
    ~MultiModalManager() override = default;

    void SetMultimodalSubscriber(const RefPtr<MultimodalSubscriber>& subscriber);
    void OpenChannel(const RefPtr<PipelineContext>& context);

    RefPtr<MultiModalScene> GetMultiModalScene(int32_t pageId);
    RefPtr<MultiModalScene> GetCurrentMultiModalScene();

    void DumpMultimodalScene();

    void PushActiveScene(int32_t newPageId);
    void PopActiveScene(int32_t poppedPageId, int32_t incomingPageId);
    void ReplaceActiveScene(int32_t newPageId, int32_t replaceId);
    void RemoveInactiveScene(int32_t pageId);

private:
    void RemoveMultimodalScene(int32_t pageId);

    RefPtr<MultimodalSubscriber> subscriber_;
    std::unordered_map<int32_t, RefPtr<MultiModalScene>> multiModalScenes_;
    int32_t activeSceneId_ = -1;
    bool isRegistered_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_MULTIMODAL_MANAGER_H
