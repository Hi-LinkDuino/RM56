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

#include "core/components/ability_component/render_ability_component.h"

namespace OHOS::Ace {
namespace {

constexpr char TYPE_ABILITY_COMPONENT[] = "abilitycomponent";

} // namespace

RefPtr<RenderNode> RenderAbilityComponent::Create()
{
    return AceType::MakeRefPtr<RenderAbilityComponent>();
}

void RenderAbilityComponent::Update(const RefPtr<Component>& component)
{
    auto abilityComponent = AceType::DynamicCast<AbilityComponent>(component);
    if (!abilityComponent) {
        LOGE("component is not valid");
        return;
    }

    if (!delegate_) {
        delegate_ = AceType::MakeRefPtr<AbilityComponentDelegate>(abilityComponent, context_, TYPE_ABILITY_COMPONENT);
    }
}

void RenderAbilityComponent::PerformLayout()
{
    SetLayoutSize(GetLayoutParam().GetMaxSize());
}

void RenderAbilityComponent::OnGlobalPositionChanged()
{
    UpdateRenderRect(true);
}

void RenderAbilityComponent::OnSizeChanged()
{
    UpdateRenderRect(false);
}

void RenderAbilityComponent::UpdateRenderRect(bool isPositionChanged)
{
    if (!hasPositionInited_ && !isPositionChanged) {
        return;
    }

    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext || !delegate_) {
        LOGE("context or delegate is null");
        return;
    }

    Rect renderRect = Rect(GetGlobalOffset(), GetLayoutSize()) * pipelineContext->GetViewScale();
    if (!hasPositionInited_) {
        hasPositionInited_ = true;
        delegate_->ReleasePlatformResource();
        delegate_->CreatePlatformResource(context_, renderRect);
    } else {
        delegate_->UpdateRenderRect(renderRect);
    }
}

void RenderAbilityComponent::ClearRenderObject()
{
    delegate_.Reset();
    hasPositionInited_ = false;
}

} // namespace OHOS::Ace
