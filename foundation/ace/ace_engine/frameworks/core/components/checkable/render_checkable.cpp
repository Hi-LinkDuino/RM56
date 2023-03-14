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

#include "core/components/checkable/render_checkable.h"

#include "base/log/event_report.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/common/properties/color.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

void RenderCheckable::Update(const RefPtr<Component>& component)
{
    auto checkable = AceType::DynamicCast<CheckableComponent>(component);
    if (!checkable) {
        LOGW("component is null");
        return;
    }
    pointColor_ = checkable->GetPointColor().GetValue();
    activeColor_ = checkable->GetActiveColor().GetValue();
    pointColorInspector_ = checkable->GetPointColor();
    activeColorInspector_ = checkable->GetActiveColor();
    inactiveColor_ = checkable->GetInactiveColor().GetValue();
    inactivePointColor_ = checkable->GetInactivePointColor().GetValue();
    focusColor_ = checkable->GetFocusColor().GetValue();
    shadowColor_ = checkable->GetShadowColor().GetValue();
    shadowWidth_ = checkable->GetShadowWidth();
    defaultWidth_ = checkable->GetDefaultWidth();
    defaultHeight_ = checkable->GetDefaultHeight();
    aspectRatio_ = checkable->GetAspectRatio();
    backgroundSolid_ = checkable->IsBackgroundSolid();
    hotZoneHorizontalPadding_ = checkable->GetHotZoneHorizontalPadding();
    hotZoneVerticalPadding_ = checkable->GetHotZoneVerticalPadding();
    disabled_ = checkable->IsDisabled();
    auto clickId = checkable->GetClickEvent();
    auto catchMode = true;
    if (!clickId.IsEmpty()) {
        catchMode = clickId.GetCatchMode();
    }
    clickEvent_ = AceAsyncEvent<void()>::Create(clickId, context_);
    changeEvent_ = AceAsyncEvent<void(const std::string)>::Create(checkable->GetChangeEvent(), context_);
    valueChangeEvent_ = checkable->GetChangeEvent().GetUiStrFunction();
    domChangeEvent_ = AceAsyncEvent<void(const std::string&)>::Create(checkable->GetDomChangeEvent(), context_);
    needFocus_ = checkable->GetNeedFocus();
    if (checkable->GetOnChange()) {
        onChange_ = *checkable->GetOnChange();
    }
    if (checkable->GetOnClick()) {
        onClick_ = *checkable->GetOnClick();
    }
    InitClickRecognizer(catchMode);
    AddAccessibilityAction();
    MarkNeedLayout();
}

void RenderCheckable::AddAccessibilityAction()
{
    auto accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }
    accessibilityNode->AddSupportAction(AceAction::ACTION_CLICK);
    accessibilityNode->SetActionClickImpl([weakPtr = AceType::WeakClaim(this)]() {
        auto renderCheckable = weakPtr.Upgrade();
        if (renderCheckable) {
            renderCheckable->HandleClick();
        }
    });
}

void RenderCheckable::InitSize()
{
    // Get parent(box)'s layoutParam.GetMaxSize as self size.
    width_ = GetLayoutParam().GetMaxSize().Width();
    if (NearEqual(Size::INFINITE_SIZE, width_)) {
        width_ = NormalizeToPx(defaultWidth_);
    }
    height_ = GetLayoutParam().GetMaxSize().Height();
    if (NearEqual(Size::INFINITE_SIZE, height_)) {
        height_ = NormalizeToPx(defaultHeight_);
    }
}

void RenderCheckable::CalculateSize()
{
    // Fit hot zone to real size.
    double hotZoneHorizontalPadding = NormalizePercentToPx(hotZoneHorizontalPadding_, false);
    double hotZoneVerticalPadding = NormalizePercentToPx(hotZoneVerticalPadding_, true);
    auto defaultWidth = NormalizeToPx(defaultWidth_);
    auto defaultHeight = NormalizeToPx(defaultHeight_);
    if ((width_ < defaultWidth) && (!NearZero(defaultWidth))) {
        hotZoneHorizontalPadding *= width_ / defaultWidth;
    }
    if ((height_ < defaultHeight) && (!NearZero(defaultHeight))) {
        hotZoneVerticalPadding *= height_ / defaultHeight;
    }

    // Calculate draw size with hot zone and ratio of (width / height).
    double width = width_ - 2 * hotZoneHorizontalPadding;
    width = width > 0 ? width : 0;
    double height = height_ - 2 * hotZoneVerticalPadding;
    height = height > 0 ? height : 0;
    drawSize_ = Size(width, height);
    ApplyAspectRatio(drawSize_);
    paintPosition_ = Alignment::GetAlignPosition(Size(width_, height_), drawSize_, Alignment::CENTER);
}

void RenderCheckable::InitClickRecognizer(bool catchMode)
{
    if (!disabled_ && !clickRecognizer_) {
        clickRecognizer_ = AceType::MakeRefPtr<ClickRecognizer>();
        clickRecognizer_->SetOnClick([weak = AceType::WeakClaim(this)](const ClickInfo& info) {
            auto renderCheckable = weak.Upgrade();
            if (renderCheckable) {
                renderCheckable->HandleClick();
            }
        });
        static const int32_t bubbleModeVersion = 6;
        auto pipeline = context_.Upgrade();
        if (!catchMode && pipeline && pipeline->GetMinPlatformVersion() >= bubbleModeVersion) {
            clickRecognizer_->SetUseCatchMode(false);
        } else {
            clickRecognizer_->SetUseCatchMode(true);
        }
    } else if (disabled_ && clickRecognizer_) {
        clickRecognizer_ = nullptr;
    }
}

void RenderCheckable::ApplyAspectRatio(Size& drawSize) const
{
    // Protect from drawSize.Height() being zero.
    double drawAreaAspectRatio = Size::INFINITE_SIZE;
    if (!drawSize.IsValid()) {
        return;
    }
    drawAreaAspectRatio = drawSize.Width() / drawSize.Height();
    if (drawAreaAspectRatio > aspectRatio_) {
        drawSize.SetWidth(drawSize.Height() * aspectRatio_);
    } else if (!NearZero(aspectRatio_)) {
        drawSize.SetHeight(drawSize.Width() / aspectRatio_);
    }
}

void RenderCheckable::PerformLayout()
{
    InitSize();
    CalculateSize();
    Size constrainSize = GetLayoutParam().Constrain(Size(width_, height_));
    SetLayoutSize(constrainSize);
}

void RenderCheckable::OnStatusChanged(RenderStatus renderStatus)
{
    onFocus_ = renderStatus == RenderStatus::FOCUS;
    auto context = context_.Upgrade();
    if (context && context->GetRenderFocusAnimation() && (renderStatus == RenderStatus::BLUR)) {
        context->GetRenderFocusAnimation()->CancelFocusAnimation();
    }
    UpdateUIStatus();
    MarkNeedRender();
}

void RenderCheckable::HandleClick()
{
    auto result = UpdateChangedResult();
    if (!result.empty()) {
        MarkNeedRender();
        auto resultForChangeEvent = std::string(R"("change",{"checked":)").append(result.append("},null"));
        OnHandleChangedResult(resultForChangeEvent);
        if (changeEvent_) {
            changeEvent_(resultForChangeEvent);
        }
        if (valueChangeEvent_) {
            valueChangeEvent_(result);
        }
    }
    if (onChange_) {
        onChange_(checked_);
    }

    if (clickEvent_) {
        clickEvent_();
    }
}

void RenderCheckable::OnHandleChangedResult(const std::string& result)
{
    if (domChangeEvent_) {
        domChangeEvent_(result);
    }
}

std::string RenderCheckable::UpdateChangedResult()
{
    LOGD("handle click");
    checked_ = !checked_;
    UpdateUIStatus();

    return checked_ ? "true" : "false";
}

void RenderCheckable::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    LOGD("on touch test hit");
    if (dragRecognizer_) {
        dragRecognizer_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(dragRecognizer_);
    }
    if (clickRecognizer_) {
        clickRecognizer_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(clickRecognizer_);
    }
}

void RenderCheckable::OnMouseHoverEnterTest()
{
    MarkNeedRender();
}

void RenderCheckable::OnMouseHoverExitTest()
{
    MarkNeedRender();
}

void RenderCheckable::RequestFocusBorder(const Offset& focusOffset, const Size& focusSize, double borderRadius)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Pipeline context upgrade fail!");
        return;
    }
    if (!context->GetRenderFocusAnimation()) {
        LOGE("focusAnimation is null!");
        EventReport::SendRenderException(RenderExcepType::RENDER_ANIMATION_ERR);
        return;
    }
    context->ShowFocusAnimation(RRect::MakeRRect(Rect(Offset(), focusSize), borderRadius, borderRadius), Color::BLUE,
        focusOffset + GetGlobalOffset());
}

} // namespace OHOS::Ace
