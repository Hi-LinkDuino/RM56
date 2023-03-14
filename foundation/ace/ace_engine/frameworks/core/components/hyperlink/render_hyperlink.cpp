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

#include "core/components/hyperlink/render_hyperlink.h"

#include "base/log/event_report.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/hyperlink/hyperlink_component.h"
#include "core/components/image/image_component.h"
#include "core/components/page/page_target.h"
#include "core/components/text/text_component.h"
#include "core/event/ace_event_helper.h"
#include "core/pipeline/base/component_group.h"
#include "core/pipeline/base/multi_composed_component.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

RenderHyperlink::RenderHyperlink()
{
    Initialize();
}

RefPtr<RenderNode> RenderHyperlink::Create()
{
    return AceType::MakeRefPtr<RenderHyperlink>();
}

void RenderHyperlink::Initialize()
{
    auto wp = AceType::WeakClaim(this);
    clickRecognizer_ = AceType::MakeRefPtr<ClickRecognizer>();
    clickRecognizer_->SetOnClick([wp](const ClickInfo& info) {
        auto hyperlink = wp.Upgrade();
        if (!hyperlink) {
            return;
        }
        hyperlink->JumpToAddress();
    });
}

void RenderHyperlink::Update(const RefPtr<Component>& component)
{
    hyperlinkComponent_ = AceType::DynamicCast<HyperlinkComponent>(component);
    if (!hyperlinkComponent_) {
        LOGE("update error, hyperlink is null.");
        return;
    }
    address_ = hyperlinkComponent_->GetAddress();
    color_ = hyperlinkComponent_->GetColor();
    SetImageChildColor(hyperlinkComponent_);
    if (!hyperlinkResources_) {
        hyperlinkResources_ = AceType::MakeRefPtr<HyperlinkResources>(context_);
        hyperlinkResources_->CreatePlatformResource(context_);
    }
    MarkNeedLayout();
}

void RenderHyperlink::PerformLayout()
{
    if (!GetChildren().empty()) {
        const auto& child = GetChildren().front();
        child->Layout(GetLayoutParam());
        auto childSize = child->GetLayoutSize();
        SetLayoutSize(childSize);
    }
}

void RenderHyperlink::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!clickRecognizer_) {
        return;
    }
    clickRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(clickRecognizer_);
}

void RenderHyperlink::JumpToAddress()
{
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    LOGW("[Engine Log] Unable to use the Hyperlink in the Previewer. "
         "Perform this operation on the emulator or a real device instead.");
    return;
#endif
    auto context = context_.Upgrade();
    if (context) {
        context->HyperlinkStartAbility(address_);
    }
}

void RenderHyperlink::SetImageChildColor(const RefPtr<Component> node)
{
    if (node == nullptr) {
        return;
    }
    auto componentGroup = AceType::DynamicCast<ComponentGroup>(node);
    auto multiComposedComponent = AceType::DynamicCast<MultiComposedComponent>(node);
    auto composedComponent = AceType::DynamicCast<ComposedComponent>(node);
    auto singleChildComponent = AceType::DynamicCast<SoleChildComponent>(node);
    auto imageComponent = AceType::DynamicCast<ImageComponent>(node);
    auto textComponent = AceType::DynamicCast<TextComponent>(node);
    if (imageComponent) {
        imageComponent->SetImageFill(GetColor());
        return;
    }
    if (textComponent) {
        auto textStyle = textComponent->GetTextStyle();
        textStyle.SetTextColor(GetColor());
        textComponent->SetTextStyle(std::move(textStyle));
        return;
    }
    if (componentGroup) {
        std::list<RefPtr<Component>> children = componentGroup->GetChildren();
        for (const auto& child : children) {
            SetImageChildColor(child);
        }
    } else if (multiComposedComponent) {
        std::list<RefPtr<Component>> children = multiComposedComponent->GetChildren();
        for (const auto& child : children) {
            SetImageChildColor(child);
        }
    } else if (composedComponent) {
        auto child = composedComponent->GetChild();
        SetImageChildColor(child);
    } else if (singleChildComponent) {
        auto child = singleChildComponent->GetChild();
        SetImageChildColor(child);
    }
}

} // namespace OHOS::Ace
