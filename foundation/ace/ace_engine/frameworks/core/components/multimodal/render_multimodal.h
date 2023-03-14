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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MULTIMODAL_RENDER_MULTIMODAL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MULTIMODAL_RENDER_MULTIMODAL_H

#include "core/components/declaration/common/attribute.h"
#include "core/event/multimodal/multimodal_properties.h"
#include "core/event/multimodal/multimodal_scene.h"
#include "core/gestures/click_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderMultimodal : public RenderNode {
    DECLARE_ACE_TYPE(RenderMultimodal, RenderNode)

public:
    RenderMultimodal();
    ~RenderMultimodal() override;

    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;

    void PerformLayout() override;

protected:
    void OnHiddenChanged(bool hidden) override;

    bool isSubscriptShow_ = false;
    VoiceEvent voiceEvent_;
    VoiceEvent subscript_;

private:
    void PrepareMultimodalEvent(const CommonMultimodalAttribute& multimodalProperties);
    void OnEventTrigger();
    void OnSubscriptSwitchChange(bool isOn);

    ClickCallback clickCallback_;
    MultimodalEventCallback multimodalEventCallback_;
    WeakPtr<MultiModalScene> multiModalScene_;
    EventCallback<void(bool)> subscriptSwitchCallback_;
    int32_t pageId_ = -1;
    bool useSubscript_ = false;
    bool useAutoSubscriptId_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MULTIMODAL_RENDER_MULTIMODAL_H
