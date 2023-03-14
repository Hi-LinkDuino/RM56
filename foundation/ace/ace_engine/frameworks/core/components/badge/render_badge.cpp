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

#include "core/components/badge/render_badge.h"

#include "base/log/event_report.h"
#include "core/common/font_manager.h"
#include "core/components/common/properties/alignment.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

RenderBadge::RenderBadge()
{
    clickRecognizer_ = AceType::MakeRefPtr<ClickRecognizer>();
    clickRecognizer_->SetOnClick([wp = AceType::WeakClaim(this)](const ClickInfo&) {
        auto badge = wp.Upgrade();
        if (badge) {
            badge->HandleClickEvent();
        }
    });
}

RenderBadge::~RenderBadge()
{
    auto context = context_.Upgrade();
    if (context) {
        context->RemoveFontNode(WeakClaim(this));
        auto fontManager = context->GetFontManager();
        if (fontManager) {
            fontManager->RemoveVariationNode(WeakClaim(this));
        }
    }
}

void RenderBadge::HandleClickEvent()
{
    if (onClick_) {
        onClick_();
    }
}

void RenderBadge::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!clickRecognizer_) {
        return;
    }
    clickRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(clickRecognizer_);
}

void RenderBadge::Update(const RefPtr<Component>& component)
{
    badge_ = AceType::DynamicCast<BadgeComponent>(component);
    if (!badge_) {
        LOGE("Update error, badge component is null");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    showMessage_ = badge_->IsShowMessage();
    auto padding = badge_->GetPadding();
    auto badgeLabel = badge_->GetBadgeLabel();
    auto messageCount = badge_->GetMessageCount();
    auto countLimit = badge_->GetMaxCount();
    badgeChildInitialOffset_ = Offset(NormalizeToPx(padding.Left()), NormalizeToPx(padding.Top()));
    onClick_ = AceAsyncEvent<void()>::Create(badge_->GetClickEvent(), context_);
    auto catchMode = true;
    if (badge_->GetClickEvent().IsEmpty()) {
        catchMode = false;
    } else {
        catchMode = badge_->GetClickEvent().GetCatchMode();
    }
    static const int32_t bubbleModeVersion = 6;
    auto pipeline = context_.Upgrade();
    if (!catchMode) {
        if (pipeline && pipeline->GetMinPlatformVersion() >= bubbleModeVersion) {
            catchMode = false;
        } else {
            catchMode = true;
        }
    }
    clickRecognizer_->SetUseCatchMode(catchMode);
    textData_.clear();
    showMessage_ = ParseBadgeStatus(badgeLabel, messageCount, countLimit);
    badgeTextComponent_ = AceType::MakeRefPtr<TextComponent>(textData_);
    if (!badgeRenderText_) {
        InitialBadgeText();
    }
    UpdateBadgeText();
}

void RenderBadge::PerformLayout()
{
    if (!badge_) {
        return;
    }

    if (!GetChildren().front()) {
        SetLayoutSize(Size());
        showMessage_ = false;
        return;
    }

    auto context = context_.Upgrade();
    if (context) {
        dipScale_ = context->GetDipScale();
    }

    // child layout
    LayoutParam layoutParam = GetLayoutParam();
    Size minSize = layoutParam.GetMinSize();
    Size maxSize = layoutParam.GetMaxSize();
    LayoutParam innerLayoutParam = layoutParam;
    Size paddingSize = badge_->GetPadding().GetLayoutSizeInPx(dipScale_);
    innerLayoutParam.SetMinSize(minSize - paddingSize);
    innerLayoutParam.SetMaxSize(maxSize - paddingSize);
    double maxWidth = minSize.Width();
    double maxHeight = minSize.Height();
    if (!GetChildren().empty()) {
        auto child = GetChildren().front();
        child->Layout(innerLayoutParam);
        child->SetPosition(badgeChildInitialOffset_);
        maxWidth = std::max(maxWidth, child->GetLayoutSize().Width() + paddingSize.Width());
        maxHeight = std::max(maxHeight, child->GetLayoutSize().Height() + paddingSize.Height());
    }

    // calculate self layout size
    if (maxSize.IsInfinite()) {
        // same with child size
        badgeSize_ = Size(maxWidth, maxHeight);
    } else {
        badgeSize_ = maxSize;
    }
    if (!badgeSize_.IsValid()) {
        badgeSize_ = Size();
        showMessage_ = false;
    }
    SetLayoutSize(badgeSize_);
    width_ = badgeSize_.Width();
    height_ = badgeSize_.Height();
}

void RenderBadge::InitialBadgeText()
{
    badgeRenderText_ = AceType::DynamicCast<RenderText>(badgeTextComponent_->CreateRenderNode());
    LayoutParam innerLayout;
    innerLayout.SetMaxSize(Size(Size::INFINITE_SIZE, Size::INFINITE_SIZE));
    badgeRenderText_->Attach(GetContext());
    badgeRenderText_->Update(badgeTextComponent_);
    badgeRenderText_->SetLayoutParam(innerLayout);
}

bool RenderBadge::ParseBadgeStatus(const std::string& label, int64_t messageCount, int64_t countLimit)
{
    if (!label.empty()) {
        textData_ = label;
        return true;
    }
    if (messageCount < 0) {
        return true;
    }
    if (messageCount > 0) {
        if (messageCount > countLimit) {
            textData_ = std::to_string(countLimit) + '+';
        } else {
            textData_ = std::to_string(messageCount);
        }
        return true;
    }
    return false;
}

void RenderBadge::UpdateBadgeText()
{
    auto context = context_.Upgrade();
    if (context) {
        auto fontManager = context->GetFontManager();
        if (fontManager) {
            fontManager->AddVariationNode(WeakClaim(this));
        }
    }
    if (badge_) {
        textStyle_.SetTextColor(badge_->GetBadgeTextColor());
        textStyle_.SetFontSize(badge_->GetBadgeFontSize());
        textStyle_.SetAllowScale(false);
        badgeTextComponent_->SetData(textData_);
        badgeTextComponent_->SetTextStyle(textStyle_);
        badgeRenderText_->Update(badgeTextComponent_);
    }
}

} // namespace OHOS::Ace
