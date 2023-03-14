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

#include "core/components/multimodal/render_multimodal.h"

#include "core/components/multimodal/multimodal_component.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

RenderMultimodal::RenderMultimodal()
    : subscriptSwitchCallback_(std::bind(&RenderMultimodal::OnSubscriptSwitchChange, this, std::placeholders::_1))
{}

void RenderMultimodal::PerformLayout()
{
    Size layoutSize;
    const auto& children = GetChildren();
    if (!children.empty()) {
        auto child = children.front();
        child->Layout(GetLayoutParam());
        child->SetPosition(Offset::Zero());
        layoutSize = child->GetLayoutSize();
    }
    SetLayoutSize(layoutSize);
}

void RenderMultimodal::Update(const RefPtr<Component>& component)
{
    auto multiModalComponent = DynamicCast<MultimodalComponent>(component);
    ACE_DCHECK(multiModalComponent);
    clickCallback_ = AceAsyncEvent<void(const ClickInfo&)>::Create(multiModalComponent->GetOnClickId(), context_);
    pageId_ = multiModalComponent->GetPageId();
    if (!multimodalEventCallback_) {
        multimodalEventCallback_ = [weak = WeakClaim(this)](const AceMultimodalEvent&) {
            auto renderMultiModal = weak.Upgrade();
            if (renderMultiModal) {
                renderMultiModal->OnEventTrigger();
            }
        };
    }
    if (multiModalScene_.Invalid()) {
        auto pipelineContext = GetContext().Upgrade();
        if (!pipelineContext) {
            LOGW("the pipeline context is null");
            return;
        }
        auto multimodalManager = pipelineContext->GetMultiModalManager();
        if (!multimodalManager) {
            LOGW("the multimodal manager is null");
            return;
        }
        auto multiModalScene = multimodalManager->GetMultiModalScene(pageId_);
        multiModalScene->SubscribeSubscriptSwitchEvent(subscriptSwitchCallback_);
        multiModalScene_ = multiModalScene;
    }
    PrepareMultimodalEvent(multiModalComponent->GetMultimodalProperties());
    MarkNeedLayout();
}

void RenderMultimodal::PrepareMultimodalEvent(const CommonMultimodalAttribute& multimodalProperties)
{
    auto multiModalScene = multiModalScene_.Upgrade();
    if (!multiModalScene) {
        LOGE("fail to prepare multimodal event due to scene is null");
        return;
    }
    if (!multimodalProperties.voiceLabel.empty() &&
        (voiceEvent_.GetVoiceContent() != multimodalProperties.voiceLabel)) {
        multiModalScene->UnSubscribeVoiceEvent(voiceEvent_);
        voiceEvent_ = VoiceEvent(multimodalProperties.voiceLabel, multimodalProperties.scene);
        multiModalScene->SubscribeVoiceEvent(voiceEvent_, multimodalEventCallback_);
    }
    if (!multimodalProperties.useSubscript) {
        if (useSubscript_) {
            multiModalScene->UnSubscribeVoiceEvent(subscript_);
            useSubscript_ = false;
            if (useAutoSubscriptId_) {
                multiModalScene->RemoveSubscriptId(subscript_.GetVoiceContent());
                useAutoSubscriptId_ = false;
            }
        }
        return;
    }
    if (!multimodalProperties.subscriptLabel.empty() &&
        subscript_.GetVoiceContent() != multimodalProperties.subscriptLabel) {
        multiModalScene->UnSubscribeVoiceEvent(subscript_);
        if (useAutoSubscriptId_) {
            multiModalScene->RemoveSubscriptId(subscript_.GetVoiceContent());
            useAutoSubscriptId_ = false;
        }
        subscript_ = VoiceEvent(multimodalProperties.subscriptLabel, multimodalProperties.scene);
        multiModalScene->SubscribeVoiceEvent(subscript_, multimodalEventCallback_);
        return;
    }
    if (subscript_.GetVoiceContent().empty()) {
        useAutoSubscriptId_ = true;
        subscript_ = VoiceEvent(multiModalScene->GetAvailableSubscriptId(), SceneLabel::PAGE, true);
        subscript_.SetBadgeList({ { "1", multiModalScene->GetCurrentMaxSubscriptId() } });
        multiModalScene->SubscribeVoiceEvent(subscript_, multimodalEventCallback_);
    }
}

void RenderMultimodal::OnEventTrigger()
{
    if (clickCallback_) {
        clickCallback_(ClickInfo(-1));
    }
}

RenderMultimodal::~RenderMultimodal()
{
    auto multiModalScene = multiModalScene_.Upgrade();
    if (!multiModalScene) {
        LOGE("fail to destroy multimodal event due to multiModalScene is null");
        return;
    }
    if (!voiceEvent_.GetVoiceContent().empty()) {
        multiModalScene->UnSubscribeVoiceEvent(voiceEvent_);
    }
    if (!subscript_.GetVoiceContent().empty()) {
        multiModalScene->UnSubscribeVoiceEvent(subscript_);
        if (useAutoSubscriptId_) {
            multiModalScene->RemoveSubscriptId(subscript_.GetVoiceContent());
            useAutoSubscriptId_ = false;
        }
    }
    multiModalScene->UnSubscribeSubscriptSwitchEvent(subscriptSwitchCallback_);
}

void RenderMultimodal::OnHiddenChanged(bool hidden)
{
    auto multiModalScene = multiModalScene_.Upgrade();
    if (!multiModalScene) {
        LOGE("fail to destroy multimodal event due to multiModalScene is null");
        return;
    }
    // If it is hidden, release subscriptId and voiceEvent.
    if (!voiceEvent_.GetVoiceContent().empty()) {
        if (hidden) {
            multiModalScene->UnSubscribeVoiceEvent(voiceEvent_);
        } else {
            multiModalScene->SubscribeVoiceEvent(voiceEvent_, multimodalEventCallback_);
        }
    }
    if (!subscript_.GetVoiceContent().empty()) {
        if (hidden) {
            multiModalScene->UnSubscribeVoiceEvent(subscript_);
        } else {
            multiModalScene->SubscribeVoiceEvent(subscript_, multimodalEventCallback_);
        }
    }
}

void RenderMultimodal::OnSubscriptSwitchChange(bool isOn)
{
    if (isSubscriptShow_ != isOn) {
        isSubscriptShow_ = isOn;
        MarkNeedRender();
    }
}

} // namespace OHOS::Ace