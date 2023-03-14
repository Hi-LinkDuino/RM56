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

#include "core/components/side_bar/render_side_bar_container.h"

#include "base/geometry/dimension.h"
#include "base/geometry/offset.h"
#include "base/geometry/size.h"
#include "base/log/ace_trace.h"
#include "base/utils/system_properties.h"
#include "core/components/box/render_box.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/layout/layout_param.h"
#include "core/components/image/image_component.h"
#include "core/components/image/render_image.h"
#include "core/components/side_bar/side_bar_container_component.h"
#include "core/gestures/long_press_recognizer.h"
#include "core/gestures/pan_recognizer.h"
#include "core/gestures/sequenced_recognizer.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/position_layout_utils.h"

namespace OHOS::Ace {

namespace {

constexpr int32_t DEFAULT_FINGERS = 1;
constexpr int32_t DEFAULT_DURATION = 50;
constexpr int32_t DEFAULT_DISTANCE = 0;
constexpr int32_t DEFAULT_MIX_CHILDREN_SIZE = 3;
constexpr Dimension DEFAULT_DRAG_REGION = 20.0_vp;

} // namespace

RefPtr<RenderNode> RenderSideBarContainer::Create()
{
    return AceType::MakeRefPtr<RenderSideBarContainer>();
}

void RenderSideBarContainer::Update(const RefPtr<Component>& component)
{
    sideBar_ = AceType::DynamicCast<SideBarContainerComponent>(component);
    if (!sideBar_) {
        return;
    }

    auto width = sidebarWidth_;
    showSideBar_ = sideBar_->GetIsShow();
    if (sideBar_->GetSideBarContainerType() == SideBarContainerType::EMBED) {
        style_ = "SideBarContainerType.Embed";
    } else {
        style_ = "SideBarContainerType.Overlay";
    }
    buttonLeft_ = sideBar_->GetButtonLeft();
    buttonTop_ = sideBar_->GetButtonTop();
    buttonWidth_ = sideBar_->GetButtonWidth();
    buttonHeight_ = sideBar_->GetButtonHeight();
    iconShow_ = sideBar_->GetShowIcon();
    iconHidden_ = sideBar_->GetHiddenIcon();
    iconSwitch_ = sideBar_->GetSwitchIcon();
    showControlButton_ = sideBar_->GetShowControlButton();

    auto minWidth = sideBar_->GetSideBarMinWidth();
    auto maxWidth = sideBar_->GetSideBarMaxWidth();
    if (!isInitialized_ && sideBar_->IsSideBarwidthDefined()) {
        width = sideBar_->GetSideBarWidth();
    }
    exceptRegion_.SetRect(SystemProperties::Vp2Px(sideBar_->GetButtonLeft()),
        SystemProperties::Vp2Px(sideBar_->GetButtonTop()), SystemProperties::Vp2Px(sideBar_->GetButtonWidth()),
        SystemProperties::Vp2Px(sideBar_->GetButtonHeight()));
    if (width < minWidth || width > maxWidth || minWidth > maxWidth) {
        LOGE("the minSideBarWidth or maxSideBarWidth is illegal, use default value");
    } else {
        sidebarWidth_ = width;
        minSidebarWidth_ = minWidth;
        maxSidebarWidth_ = maxWidth;
    }
    if (!isInitialized_) {
        status_ = sideBar_->GetSideBarStatus();
        pendingStatus_ = status_;
        curPosition_ = -sidebarWidth_;
        InitializeDragAndAnimation();
        isInitialized_ = true;
    }

    auto weak = AceType::WeakClaim(this);
    sideBar_->SetClickedFunction([weak] {
        auto container = weak.Upgrade();
        if (container) {
            container->DoSideBarAnimation();
        }
    });

    RenderStack::Update(component);
}

void RenderSideBarContainer::InitializeDragAndAnimation()
{
    // update drag recognizer.
    auto context = GetContext();
    auto weak = AceType::WeakClaim(this);
    PanDirection panDirection = { .type = PanDirection::HORIZONTAL };
    auto longPressRecognizer =
        AceType::MakeRefPtr<LongPressRecognizer>(context, DEFAULT_DURATION, DEFAULT_FINGERS, false);
    auto panRecognizer = AceType::MakeRefPtr<PanRecognizer>(context, DEFAULT_FINGERS, panDirection, DEFAULT_DISTANCE);
    panRecognizer->SetOnActionStart([weak](const GestureEvent& info) {
        auto container = weak.Upgrade();
        if (container) {
            container->HandleDragStart();
        }
    });
    panRecognizer->SetOnActionUpdate([weak](const GestureEvent& info) {
        auto container = weak.Upgrade();
        if (container) {
            container->HandleDragUpdate(info.GetOffsetX());
        }
    });
    panRecognizer->SetOnActionEnd([weak](const GestureEvent& info) {
        auto container = weak.Upgrade();
        if (container) {
            container->HandleDragEnd();
        }
    });
    panRecognizer->SetOnActionCancel([weak]() {
        auto container = weak.Upgrade();
        if (container) {
            container->HandleDragEnd();
        }
    });
    std::vector<RefPtr<GestureRecognizer>> recognizers { longPressRecognizer, panRecognizer };
    dragRecognizer_ = AceType::MakeRefPtr<OHOS::Ace::SequencedRecognizer>(GetContext(), recognizers);
    dragRecognizer_->SetIsExternalGesture(true);
    animationController_ = AceType::MakeRefPtr<SideBarAnimationController>(GetContext());
    animationController_->SetRenderSideBarContainer(weak);
}

void RenderSideBarContainer::OnStatusChanged(RenderStatus renderStatus)
{
    if (renderStatus == RenderStatus::FOCUS) {
        isFocus_ = true;
    } else if (renderStatus == RenderStatus::BLUR) {
        isFocus_ = false;
    }
}

void RenderSideBarContainer::UpdateElementPosition(double offset)
{
    curPosition_ = Dimension(SystemProperties::Px2Vp(offset), DimensionUnit::VP);
    SetChildrenStatus();
}

double RenderSideBarContainer::GetSidebarWidth() const
{
    return sidebarWidth_.ConvertToPx();
}

double RenderSideBarContainer::GetSlidePosition() const
{
    return curPosition_.ConvertToPx();
}

bool RenderSideBarContainer::TouchTest(const Point& globalPoint, const Point& parentLocalPoint,
    const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (GetDisableTouchEvent() || IsDisabled()) {
        return false;
    }

    auto paintRect = GetPaintRect();
    auto exceptRegion = Rect(paintRect.GetOffset() + exceptRegion_.GetOffset(), exceptRegion_.GetSize());
    auto dragRect = Rect(paintRect.GetOffset() + Offset((sidebarWidth_ - DEFAULT_DRAG_REGION).ConvertToPx(), 0),
        Size(2 * DEFAULT_DRAG_REGION.ConvertToPx(), paintRect.Height()));
    auto touchRect = GetTransformRect(dragRect);
    auto point = GetTransformPoint(parentLocalPoint);
    if (touchRect.IsInRegion(point) && !GetTransformRect(exceptRegion).IsInRegion(point)) {
        const auto localPoint = point - GetPaintRect().GetOffset();
        const auto coordinateOffset = globalPoint - localPoint;
        dragRecognizer_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(dragRecognizer_);
        return true;
    }

    return RenderNode::TouchTest(globalPoint, parentLocalPoint, touchRestrict, result);
}

void RenderSideBarContainer::DoSideBarAnimation()
{
    if (!animationController_) {
        return;
    }

    if (isFocus_) {
        auto context = GetContext().Upgrade();
        if (context) {
            context->CancelFocusAnimation();
        }
    }

    animationController_->SetAnimationStopCallback([weak = AceType::WeakClaim(this)]() {
        auto container = weak.Upgrade();
        if (container) {
            container->isAnimation_ = false;
            container->status_ = container->pendingStatus_;
            if (container->sideBar_->GetOnChange()) {
                (*container->sideBar_->GetOnChange())(container->status_ == SideStatus::SHOW);
            }
            container->UpdateRenderImage();
        }
    });
    if (status_ == SideStatus::SHOW) {
        pendingStatus_ = SideStatus::HIDDEN;
    } else {
        pendingStatus_ = SideStatus::SHOW;
    }

    isAnimation_ = true;
    animationController_->PlaySideBarContainerToAnimation(pendingStatus_);
    status_ = SideStatus::CHANGING;
    UpdateRenderImage();
}

void RenderSideBarContainer::SetChildrenStatus()
{
    auto children = GetChildren();
    if (children.size() < DEFAULT_MIX_CHILDREN_SIZE) {
        return;
    }

    auto begin = children.begin();
    RefPtr<RenderNode>& content = *begin;
    RefPtr<RenderNode>& sideBar = *(++begin);
    RefPtr<RenderNode>& imageBox = *(++begin);
    auto box = imageBox->GetFirstChild();
    renderImage_ = box ? box->GetFirstChild() : nullptr;

    static Dimension miniWidthToHide = 520.0_vp;
    auto layoutSize = GetLayoutSize();
    if (status_ == SideStatus::SHOW) {
        curPosition_ = 0.0_vp;
    }
    auto autoHide = GetLayoutSize().Width() <= miniWidthToHide.ConvertToPx();
    if (status_ == SideStatus::HIDDEN) {
        curPosition_ = -sidebarWidth_;
    }
    if (status_ == SideStatus::AUTO) {
        if (autoHide) {
            curPosition_ = -sidebarWidth_;
            status_ = SideStatus::HIDDEN;
        } else {
            curPosition_ = 0.0_vp;
            status_ = SideStatus::SHOW;
        }
    }
    if (sideBar_->GetSideBarContainerType() == SideBarContainerType::EMBED) {
        content->SetLeft(sidebarWidth_ + curPosition_);
        auto fixedSize = layoutSize.MinusWidth((sidebarWidth_ + curPosition_).ConvertToPx());
        content->Layout(LayoutParam(fixedSize, fixedSize));
    } else {
        content->SetLeft(Dimension(0));
        content->Layout(LayoutParam(layoutSize, layoutSize));
    }
    sideBar->SetLeft(curPosition_);
    auto fixedSize = Size(sidebarWidth_.ConvertToPx(), layoutSize.Height());
    sideBar->Layout(LayoutParam(fixedSize, fixedSize));
    MarkNeedRender();
}

void RenderSideBarContainer::UpdateRenderImage()
{
    auto renderImage = DynamicCast<RenderImage>(renderImage_.Upgrade());
    if (!renderImage) {
        LOGE("sidebar control button image error");
        return;
    }
    auto imageComponent = AceType::MakeRefPtr<ImageComponent>();
    if (status_ == SideStatus::SHOW) {
        if (sideBar_->GetShowIcon().empty()) {
            imageComponent->SetResourceId(InternalResource::ResourceId::SIDE_BAR);
        } else {
            imageComponent->SetSrc(sideBar_->GetShowIcon());
        }
    }
    if (status_ == SideStatus::HIDDEN) {
        if (sideBar_->GetHiddenIcon().empty()) {
            imageComponent->SetResourceId(InternalResource::ResourceId::SIDE_BAR);
        } else {
            imageComponent->SetSrc(sideBar_->GetHiddenIcon());
        }
    }
    if (status_ == SideStatus::CHANGING) {
        if (sideBar_->GetSwitchIcon().empty()) {
            imageComponent->SetResourceId(InternalResource::ResourceId::SIDE_BAR);
        } else {
            imageComponent->SetSrc(sideBar_->GetSwitchIcon());
        }
    }
    imageComponent->SetUseSkiaSvg(false);
    imageComponent->SetImageFit(ImageFit::FILL);
    renderImage->Update(imageComponent);
}

void RenderSideBarContainer::HandleDragUpdate(double xOffset)
{
    if (isAnimation_) {
        return;
    }
    if (status_ != SideStatus::SHOW) {
        return;
    }
    auto sideBarLine = preSidebarWidth_.ConvertToPx() + xOffset;
    auto minValue = minSidebarWidth_.ConvertToPx();
    auto maxValue = maxSidebarWidth_.ConvertToPx();
    if (sideBarLine > minValue && sideBarLine < maxValue) {
        sidebarWidth_ = Dimension(SystemProperties::Px2Vp(sideBarLine), DimensionUnit::VP);
        SetChildrenStatus();
        return;
    }
    if (sideBarLine > maxValue) {
        sidebarWidth_ = maxSidebarWidth_;
        SetChildrenStatus();
        return;
    }
    if (sideBarLine > minValue - DEFAULT_DRAG_REGION.ConvertToPx()) {
        sidebarWidth_ = minSidebarWidth_;
        SetChildrenStatus();
        return;
    }
    sidebarWidth_ = minSidebarWidth_;
    DoSideBarAnimation();
}

void RenderSideBarContainer::HandleDragStart()
{
    if (isAnimation_) {
        return;
    }
    if (status_ != SideStatus::SHOW) {
        return;
    }
    preSidebarWidth_ = sidebarWidth_;
}

void RenderSideBarContainer::HandleDragEnd()
{
    if (isAnimation_) {
        return;
    }
    if (status_ != SideStatus::SHOW) {
        return;
    }
    preSidebarWidth_ = sidebarWidth_;
}

} // namespace OHOS::Ace
