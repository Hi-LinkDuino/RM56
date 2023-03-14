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

#include "core/components/tab_bar/render_tab_bar.h"

#include <algorithm>

#include "base/log/event_report.h"
#include "base/utils/system_properties.h"
#include "core/components/common/properties/alignment.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t GRADIENT_POINT_SIZE = 4;
constexpr Dimension FOCUS_ANIMATION_WIDTH = 2.0_vp;
constexpr Dimension OFFSET_FOR_FOCUS = 4.0_vp;
constexpr double DOUBLE_FACTOR = 2.0;

} // namespace

RenderTabBar::RenderTabBar() : RenderNode(true) {}

void RenderTabBar::Update(const RefPtr<Component>& component)
{
    RefPtr<TabBarComponent> tabBar = AceType::DynamicCast<TabBarComponent>(component);
    if (!tabBar) {
        LOGE("TabBarComponent is nullptr");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    tabsSize_ = static_cast<int32_t>(tabBar->GetChildren().size());
    auto tabController = tabBar->GetController();
    FlushIndex(tabController);
    if (initialUpdate_) {
        auto barIndicator = tabBar->GetIndicator();
        if (barIndicator) {
            indicator_ = barIndicator->CreateRenderNode();
            if (indicator_) {
                AddChild(indicator_);
                indicator_->Attach(GetContext());
                indicator_->Update(barIndicator);
                indicatorPadding_ = barIndicator->GetPadding();
                indicatorStyle_ = GetIndicatorStyle(barIndicator);
            }
        }
        initialUpdate_ = false;
    }
    InitScrollableOffset(tabBar->GetMode());
    mode_ = tabBar->GetMode();
    isVertical_ = tabBar->IsVertical();
    indicatorSize_ = tabBar->GetIndicatorSize();
    padding_ = tabBar->GetPadding();
    activeIndicatorMinWidth_ = tabBar->GetActiveIndicatorMinWidth();
    focusAnimationColor_ = tabBar->GetFocusAnimationColor();
    focusRadiusDimension_ = tabBar->GetFocusRadiusDimension();
    gradientWidth_ = tabBar->GetGradientWidth();
    barPosition_ = tabBar->GetBarPosition();
    SetTextDirection(tabBar->GetTextDirection());
    Initialize();
    MarkNeedLayout();
}

void RenderTabBar::FlushIndex(const RefPtr<TabController>& controller)
{
    if (!controller) {
        return;
    }
    int32_t index = 0;
    if (controller->IsIndexDefined()) {
        index = controller->GetIndex();
    } else {
        auto initialIndex = controller->GetInitialIndex();
        auto pendingIndex = controller->GetPendingIndex();
        if (initialUpdate_ && pendingIndex < 0) {
            index = initialIndex < 0 ? 0 : initialIndex;
        } else {
            index = pendingIndex < 0 ? 0 : pendingIndex;
        }
    }
    index_ = index < tabsSize_ ? index : tabsSize_ - 1;
    controller->SetIndexWithoutChangeContent(index_);
}

void RenderTabBar::PerformLayout()
{
    tabsSize_ = 0;
    const std::list<RefPtr<RenderNode>>& children = GetChildren();
    if (indicator_) {
        // At least one child(include indicator)
        if (children.size() <= 1) {
            return;
        }
        tabsSize_ = static_cast<int32_t>(children.size()) - 1;
    } else {
        if (children.empty()) {
            return;
        }
        tabsSize_ = static_cast<int32_t>(children.size());
    }

    index_ = std::clamp(index_, 0, std::max(0, tabsSize_ - 1));
    if (!IsRightToLeft()) {
        if (tabBarWidth_ > 0 && !NearEqual(tabBarWidth_, GetLayoutParam().GetMaxSize().Width())) {
            if (!isVertical_ && actualWidth_ > GetLayoutSize().Width() &&
                GreatNotEqual(
                    GetLayoutParam().GetMaxSize().Width(), actualWidth_ - std::abs(scrollableOffset_.GetX()))) {
                scrollableOffset_.SetX(scrollableOffset_.GetX() + GetLayoutParam().GetMaxSize().Width() - tabBarWidth_);
            }
        }
    }
    tabBarWidth_ = GetLayoutParam().GetMaxSize().Width();
    // Layout children and indicator
    LayoutChildren();
    UpdatePosition();
    Size layoutSize = GetLayoutParam().Constrain(GetLayoutParam().GetMaxSize());
    LOGD("RenderTabBar layoutSize: (%{public}s)", layoutSize.ToString().c_str());
    SetLayoutSize(layoutSize);
    ApplyGradientColor();
    if (isFirstLayout_) {
        if (index_ != 0) {
            SetIndex(index_, true);
        }
        isFirstLayout_ = false;
    }
}

void RenderTabBar::PerformLayoutChildren(const LayoutParam& innerLayoutParam)
{
    actualWidth_ = NormalizeToPx(padding_.Left());
    actualHeight_ = NormalizeToPx(padding_.Top());

    tabsWidth_.clear();
    tabsHeight_.clear();
    tabItemOffsets_.clear();
    if (!isVertical_ && scrollableOffset_.GetX() > tabBarWidth_) {
        scrollableOffset_.Reset();
    }
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("context is null");
        return;
    }

    if (isVertical_) {
        tabItemOffsets_.emplace_back(0.0, 0.0);
    } else {
        if (!IsRightToLeft()) {
            tabItemOffsets_.emplace_back(padding_.GetOffsetInPx(context->GetDipScale()));
        }
    }

    auto children = GetChildren();
    auto item = children.begin();
    // skip indicator
    if (indicator_) {
        ++item;
    }
    // First time layout all children
    for (; item != children.end(); ++item) {
        (*item)->Layout(innerLayoutParam);
        if (isVertical_) {
            tabsHeight_.push_back((*item)->GetLayoutSize().Height());
            actualHeight_ += tabsHeight_.back();
            tabItemOffsets_.emplace_back(0.0, actualHeight_);
        } else {
            tabsWidth_.push_back((*item)->GetLayoutSize().Width());
            actualWidth_ += tabsWidth_.back();
            if (IsRightToLeft()) {
                tabItemOffsets_.emplace_back(tabBarWidth_ - actualWidth_, 0.0);
            } else {
                tabItemOffsets_.emplace_back(actualWidth_, 0.0);
            }
        }
    }

    actualWidth_ += NormalizeToPx(padding_.Right());
    actualHeight_ += NormalizeToPx(padding_.Bottom());
}

void RenderTabBar::LayoutChildren()
{
    // First time layout all children and update relative position.
    LayoutParam innerLayoutParam = MakeInnerLayoutParam();

    // First time layout all children
    PerformLayoutChildren(innerLayoutParam);

    if (mode_ == TabBarMode::FIXED_START && !isVertical_ && needUpdateOffset_) {
        double padding;
        if (index_ == 0) {
            padding = NormalizeToPx(Dimension(16, DimensionUnit::VP));
        } else {
            padding = NormalizeToPx(Dimension(24, DimensionUnit::VP));
        }
        if (!IsRightToLeft()) {
            scrollableOffset_.SetX(std::clamp(padding - tabItemOffsets_[index_].GetX(), -MaxScrollableWidth(), 0.0));
        } else {
            scrollableOffset_.SetX(std::clamp(padding - tabItemOffsets_[index_].GetX(), 0.0, MaxScrollableWidth()));
        }
    } else if (mode_ == TabBarMode::SCROLLABEL && actualWidth_ < GetLayoutParam().GetMaxSize().Width() &&
               !isVertical_) {
        // In scrollable mod: the sum of Tab's width can less then TabBar width
        double halfWidth = GetLayoutParam().GetMaxSize().Width() / DOUBLE_FACTOR;
        if (actualWidth_ < halfWidth) {
            // when items total width less than half of tab bar, make items average harf width
            double averageWidth = halfWidth / tabsSize_;
            innerLayoutParam.SetMinSize(Size(averageWidth, innerLayoutParam.GetMinSize().Height()));
            // relayout all children
            PerformLayoutChildren(innerLayoutParam);
        }
        if (IsRightToLeft()) {
            scrollableOffset_ = Offset((actualWidth_ - GetLayoutParam().GetMaxSize().Width()) / DOUBLE_FACTOR, 0.0);
        } else {
            scrollableOffset_ = Offset((GetLayoutParam().GetMaxSize().Width() - actualWidth_) / DOUBLE_FACTOR, 0.0);
        }
    } else if (mode_ == TabBarMode::SCROLLABEL && actualHeight_ < GetLayoutParam().GetMaxSize().Height() &&
               isVertical_) {
        // In scrollable mod: the sum of Tab's width can less then TabBar width
        scrollableOffset_ = Offset(0.0, (GetLayoutParam().GetMaxSize().Height() - actualHeight_) / DOUBLE_FACTOR);
    }
}

void RenderTabBar::UpdatePosition()
{
    const std::list<RefPtr<RenderNode>>& children = GetChildren();

    // At least one child
    if (children.size() <= 1) {
        return;
    }
    auto item = children.begin();
    // skip indicator
    if (indicator_) {
        ++item;
    }
    // Second update relative position.
    for (int32_t i = 0; item != children.end(); ++item) {
        (*item)->SetPosition(scrollableOffset_ + tabItemOffsets_[i]);
        if (i == index_ && indicator_) {
            // update indicator layout and position
            indicator_->Layout(MakeIndicatorLayoutParam(*item));
            Offset offset = MakeIndicatorOffset(*item);
            indicator_->SetPosition(scrollableOffset_ + tabItemOffsets_[index_] + offset);
        }
        i++;
    }
}

void RenderTabBar::SetIndex(int32_t index, bool force)
{
    if (index < 0 || index >= tabsSize_) {
        LOGW("illegal index = %{public}d", index);
        return;
    }

    if (index_ != index || force) {
        if (mode_ == TabBarMode::FIXED_START && tabBarSizeAnimation_) {
            needUpdateOffset_ = true;
            auto tabBar = AceType::WeakClaim(this);
            tabBarSizeAnimation_->Start(tabBar, index_, index);
        }
        index_ = index;
        if (mode_ == TabBarMode::SCROLLABEL) {
            if (actualWidth_ > GetLayoutParam().GetMaxSize().Width() && !isVertical_) {
                // In scrollable mod: the select tab must in middle of tabBar
                Offset centerViewPort(GetLayoutParam().GetMaxSize().Width() / DOUBLE_FACTOR, 0.0);
                Offset centerTabItem = tabItemOffsets_[index_] + Offset(tabsWidth_[index_] / DOUBLE_FACTOR, 0.0);
                scrollableOffset_ = centerViewPort - centerTabItem;
                if (!IsRightToLeft()) {
                    scrollableOffset_.SetX(std::clamp(scrollableOffset_.GetX(), -MaxScrollableWidth(), 0.0));
                } else {
                    scrollableOffset_.SetX(std::clamp(scrollableOffset_.GetX(), 0.0, MaxScrollableWidth()));
                }
            }
            if (actualHeight_ > GetLayoutParam().GetMaxSize().Height() && isVertical_) {
                // In scrollable mod: the select tab must in middle of tabBar
                Offset centerViewPort(0.0, GetLayoutParam().GetMaxSize().Height() / DOUBLE_FACTOR);
                Offset centerTabItem = tabItemOffsets_[index_] + Offset(0.0, tabsHeight_[index_] / DOUBLE_FACTOR);
                scrollableOffset_ = centerViewPort - centerTabItem;
                scrollableOffset_.SetY(std::clamp(scrollableOffset_.GetY(), -MaxScrollableHeight(), 0.0));
            }
        }
        MarkNeedLayout();
    }
}

void RenderTabBar::Initialize()
{
    if (!clickRecognizer_) {
        clickRecognizer_ = AceType::MakeRefPtr<ClickRecognizer>();
        clickRecognizer_->SetOnClick([weakBar = AceType::WeakClaim(this)](const ClickInfo& info) {
            auto tabBar = weakBar.Upgrade();
            if (tabBar) {
                tabBar->HandleClickedEvent(info);
            }
        });
    }
    if (!scrollable_) {
        scrollable_ = AceType::MakeRefPtr<Scrollable>(
            [weak = AceType::WeakClaim(this)](double offset, int32_t source) {
                auto tabBar = weak.Upgrade();
                if (tabBar && (source != SCROLL_FROM_START)) {
                    return tabBar->HandleScrollablePosition(offset);
                } else {
                    return false;
                }
            },
            isVertical_ ? Axis::VERTICAL : Axis::HORIZONTAL);
        scrollable_->Initialize(GetContext());
        scrollable_->SetNodeId(GetAccessibilityNodeId());
        scrollable_->SetScrollableNode(AceType::WeakClaim(this));
    }

    if (!tabBarSizeAnimation_ && !indicator_) {
        tabBarSizeAnimation_ = AceType::MakeRefPtr<TabBarSizeAnimation>();
        tabBarSizeAnimation_->Initialize(GetContext());
    }
    InitAccessibilityEventListener();
}

void RenderTabBar::InitAccessibilityEventListener()
{
    auto refNode = accessibilityNode_.Upgrade();
    if (!refNode) {
        return;
    }

    refNode->AddSupportAction(AceAction::ACTION_SCROLL_FORWARD);
    refNode->AddSupportAction(AceAction::ACTION_SCROLL_BACKWARD);
    refNode->AddSupportAction(AceAction::ACTION_CLICK);

    auto weakPtr = AceType::WeakClaim(this);
    refNode->SetActionClickImpl([weakPtr]() {
        auto tabBar = weakPtr.Upgrade();
        if (tabBar) {
            tabBar->AccessibilityClick();
            return true;
        }
        return false;
    });

    refNode->SetActionScrollForward([weakPtr]() {
        auto tabBar = weakPtr.Upgrade();
        if (tabBar) {
            tabBar->AccessibilityScroll(true);
            return true;
        }
        return false;
    });

    refNode->SetActionScrollBackward([weakPtr]() {
        auto tabBar = weakPtr.Upgrade();
        if (tabBar) {
            tabBar->AccessibilityScroll(false);
            return true;
        }
        return false;
    });
}

void RenderTabBar::AccessibilityScroll(bool isAdd)
{
    if (tabItemOffsets_.empty()) {
        return;
    }
    if (isAdd) {
        accessibilityIndex_++;
    } else {
        accessibilityIndex_--;
    }
}

void RenderTabBar::AccessibilityClick()
{
    if (callback_) {
        callback_(accessibilityIndex_);
    }
}

void RenderTabBar::HandleClickedEvent(const ClickInfo& info)
{
    LOGI("Click event x is %{public}lf", info.GetLocalLocation().GetX());
    if (tabItemOffsets_.empty()) {
        LOGW("tabItemOffsets is empty");
        return;
    }
    Offset local = info.GetLocalLocation() - scrollableOffset_;
    if (isVertical_) {
        auto clickRange = std::make_pair(tabItemOffsets_[0].GetY(), tabItemOffsets_[tabItemOffsets_.size() - 1].GetY());
        if (local.GetY() < clickRange.first || local.GetY() > clickRange.second) {
            LOGW("clicked (%{public}lf) position out of range [%{public}lf, %{public}lf]", local.GetY(),
                clickRange.first, clickRange.second);
            return;
        }
    } else {
        auto clickRange = std::make_pair(tabItemOffsets_[0].GetX(), tabItemOffsets_[tabItemOffsets_.size() - 1].GetX());
        if (!IsRightToLeft()) {
            if (local.GetX() < clickRange.first || local.GetX() > clickRange.second) {
                LOGW("clicked (%{public}lf) position out of range [%{public}lf, %{public}lf]", local.GetX(),
                    clickRange.first, clickRange.second);
                return;
            }
        } else {
            if (local.GetX() > tabBarWidth_ || local.GetX() < clickRange.second) {
                LOGW("clicked (%{public}lf) position out of range [%{public}lf, %{public}lf]", local.GetX(),
                    clickRange.first, clickRange.second);
                return;
            }
        }
    }
    auto pos = std::lower_bound(tabItemOffsets_.begin(), tabItemOffsets_.end(), local,
        [weakBar = AceType::WeakClaim(this)](const Offset& a, const Offset& b) {
            auto tabBar = weakBar.Upgrade();
            if (tabBar) {
                return tabBar->IsRightToLeft() ? a.GetX() > b.GetX()
                                               : (tabBar->isVertical_ ? a.GetY() < b.GetY() : a.GetX() < b.GetX());
            } else {
                return false;
            }
        });

    if (pos != tabItemOffsets_.end()) {
        int32_t index = IsRightToLeft() ? std::distance(tabItemOffsets_.begin(), pos)
                                        : std::distance(tabItemOffsets_.begin(), pos) - 1;
        if (index >= 0 && index < tabsSize_ && index != index_) {
            LOGD("selected tab bar index :%{public}d, current index :%{public}d", index, index_);
            if (callback_) {
                callback_(index);
            }
        }
    }
}

void RenderTabBar::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!clickRecognizer_ || !scrollable_) {
        return;
    }
    clickRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(clickRecognizer_);
    scrollable_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(scrollable_);
}

bool RenderTabBar::HandleScrollablePosition(double value)
{
    if (IsScrollable()) {
        if (!isVertical_ && actualWidth_ > GetLayoutSize().Width()) {
            Offset delta(value, 0.0);
            scrollableOffset_ += delta;
            if (!IsRightToLeft()) {
                scrollableOffset_.SetX(std::clamp(scrollableOffset_.GetX(), -MaxScrollableWidth(), 0.0));
            } else {
                scrollableOffset_.SetX(std::clamp(scrollableOffset_.GetX(), 0.0, MaxScrollableWidth()));
            }
            needUpdateOffset_ = false;
        }
        if (isVertical_ && actualHeight_ > GetLayoutSize().Height()) {
            Offset delta(0.0, value);
            scrollableOffset_ += delta;
            scrollableOffset_.SetY(std::clamp(scrollableOffset_.GetY(), -MaxScrollableHeight(), 0.0));
        }

        if (!NearZero(value)) {
            MarkNeedLayout();
            return true;
        }
    }
    return false;
}

LayoutParam RenderTabBar::MakeInnerLayoutParam() const
{
    LayoutParam innerLayout = GetLayoutParam();
    if (mode_ == TabBarMode::FIXED) {
        double paddingHorizontal = NormalizeToPx(padding_.Left()) + NormalizeToPx(padding_.Right());
        double paddingVertical = NormalizeToPx(padding_.Top()) + NormalizeToPx(padding_.Bottom());
        double tabMinWidth = std::max(0.0, innerLayout.GetMinSize().Width() - paddingHorizontal);
        double tabMaxWidth = std::max(0.0, innerLayout.GetMaxSize().Width() - paddingHorizontal);
        double tabMinHeight = std::max(0.0, innerLayout.GetMinSize().Height() - paddingVertical);
        double tabMaxHeight = std::max(0.0, innerLayout.GetMaxSize().Height() - paddingVertical);
        if (isVertical_) {
            if (tabsSize_ > 1) {
                tabMinHeight = tabMinHeight / tabsSize_;
                tabMaxHeight = tabMaxHeight / tabsSize_;
            }
            innerLayout.SetMinSize(Size(std::max(innerLayout.GetMinSize().Width(), 0.0), std::max(tabMinHeight, 0.0)));
            innerLayout.SetMaxSize(
                Size(std::max(GetLayoutParam().GetMaxSize().Width(), 0.0), std::max(tabMaxHeight, 0.0)));
        } else {
            if (tabsSize_ > 1) {
                tabMinWidth = tabMinWidth / tabsSize_;
                tabMaxWidth = tabMaxWidth / tabsSize_;
            }
            innerLayout.SetMinSize(Size(std::max(tabMinWidth, 0.0), std::max(innerLayout.GetMinSize().Height(), 0.0)));
            innerLayout.SetMaxSize(
                Size(std::max(tabMaxWidth, 0.0), std::max(GetLayoutParam().GetMaxSize().Height(), 0.0)));
        }
    } else {
        if (isVertical_) {
            innerLayout.SetMinSize(
                Size(std::max(innerLayout.GetMinSize().Width(), 0.0), innerLayout.GetMinSize().Height()));
            innerLayout.SetMaxSize(Size(std::max(innerLayout.GetMaxSize().Width(), 0.0), Size::INFINITE_SIZE));
        } else {
            innerLayout.SetMinSize(
                Size(innerLayout.GetMinSize().Width(), std::max(innerLayout.GetMinSize().Height(), 0.0)));
            innerLayout.SetMaxSize(Size(Size::INFINITE_SIZE, std::max(innerLayout.GetMaxSize().Height(), 0.0)));
        }
    }
    return innerLayout;
}

LayoutParam RenderTabBar::MakeIndicatorLayoutParam(const RefPtr<RenderNode>& item) const
{
    LayoutParam innerLayout;
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("context is null");
        return innerLayout;
    }
    innerLayout.SetMinSize(Size(NormalizeToPx(activeIndicatorMinWidth_), 0.0));
    Size maxLayoutParam = GetLayoutParam().GetMaxSize();
    if (isVertical_) {
        innerLayout.SetMaxSize(Size(
            std::max(0.0, std::max(0.0, maxLayoutParam.Width()) - NormalizeToPx(FOCUS_ANIMATION_WIDTH) * DOUBLE_FACTOR),
            std::max(0.0, maxLayoutParam.Height()) - NormalizeToPx(FOCUS_ANIMATION_WIDTH) * DOUBLE_FACTOR));
    } else {
        innerLayout.SetMaxSize(
            Size(std::max(0.0, tabsWidth_[index_] - NormalizeToPx(FOCUS_ANIMATION_WIDTH) * DOUBLE_FACTOR),
                std::max(0.0, maxLayoutParam.Height()) - NormalizeToPx(FOCUS_ANIMATION_WIDTH) * DOUBLE_FACTOR));
    }

    if (indicatorSize_ == TabBarIndicatorType::LABEL) {
        auto childSize = GetTabItemChildLayoutSize(item);
        childSize += indicatorPadding_.GetLayoutSizeInPx(context->GetDipScale());

        innerLayout.SetMaxSize(Size(std::min(innerLayout.GetMaxSize().Width(), childSize.Width()),
            std::min(innerLayout.GetMaxSize().Height(), childSize.Height())));
    }
    return innerLayout;
}

Offset RenderTabBar::MakeIndicatorOffset(const RefPtr<RenderNode>& item) const
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("context is null");
        return Offset(0.0, 0.0);
    }
    Offset offset;
    if (isVertical_) {
        offset = Alignment::GetAlignPosition(Size(GetLayoutParam().GetMaxSize().Width(), tabsHeight_[index_]),
            indicator_->GetLayoutSize(), Alignment::CENTER);
    } else {
        offset = Alignment::GetAlignPosition(Size(tabsWidth_[index_], GetLayoutParam().GetMaxSize().Height()),
            indicator_->GetLayoutSize(), Alignment::CENTER);
    }
    if (indicatorStyle_ == TabBarIndicatorStyle::DEFAULT &&
        (!onFocused_ || SystemProperties::GetDeviceType() == DeviceType::PHONE)) {
        Size childSize = GetTabItemChildLayoutSize(item);
        offset +=
            Offset(0.0, childSize.Height() / DOUBLE_FACTOR) + indicatorPadding_.GetOffsetInPx(context->GetDipScale());
    }
    return offset;
}

double RenderTabBar::MaxScrollableWidth() const
{
    if (IsScrollable() && actualWidth_ > GetLayoutParam().GetMaxSize().Width() && !isVertical_) {
        return actualWidth_ - GetLayoutParam().GetMaxSize().Width();
    }
    return 0.0;
}

double RenderTabBar::MaxScrollableHeight() const
{
    if (IsScrollable() && actualHeight_ > GetLayoutParam().GetMaxSize().Height() && isVertical_) {
        return actualHeight_ - GetLayoutParam().GetMaxSize().Height();
    }
    return 0.0;
}

Size RenderTabBar::GetTabItemChildLayoutSize(const RefPtr<RenderNode>& item) const
{
    auto children = item->GetChildren();
    if (children.empty()) {
        return Size(0.0, 0.0);
    }
    return children.front()->GetLayoutSize();
}

void RenderTabBar::UpdateIndicatorStyle(const RefPtr<Component>& component)
{
    if (indicator_) {
        indicator_->Update(component);
        RefPtr<BoxComponent> boxComponent = AceType::DynamicCast<BoxComponent>(component);
        if (boxComponent) {
            indicatorStyle_ = GetIndicatorStyle(boxComponent);
            indicatorPadding_ = boxComponent->GetPadding();
        }
    }
}

void RenderTabBar::HandleFocusEvent(bool focus)
{
    LOGD("RenderTabBar HandleFocusEvent, focus is %{public}d", focus);
    onFocused_ = focus;
    if (!onFocused_) {
        auto context = context_.Upgrade();
        if (context) {
            context->CancelFocusAnimation();
        }
    }
}

void RenderTabBar::OnPaintFinish()
{
    auto context = context_.Upgrade();
    if (!context || !onFocused_) {
        return;
    }

    auto deviceType = SystemProperties::GetDeviceType();
    if (deviceType == DeviceType::TV) {
        if (indicator_) {
            context->ShowFocusAnimation(RRect::MakeRRect(Rect(Offset(0, 0), indicator_->GetLayoutSize()),
                                            Radius(NormalizeToPx(focusRadiusDimension_))),
                focusAnimationColor_, indicator_->GetGlobalOffset());
        }
    } else {
        int32_t focusedChildIndex = indicator_ ? index_ + 1 : index_;
        auto focusedItem = GetChildByIndex(focusedChildIndex);
        if (!focusedItem) {
            return;
        }

        auto layoutSize = focusedItem->GetLayoutSize();
        auto position = focusedItem->GetGlobalOffset();

        double offsetForFocus = NormalizeToPx(OFFSET_FOR_FOCUS);
        Offset offset = Offset(offsetForFocus, offsetForFocus);
        layoutSize -= Size(offsetForFocus * 2.0, offsetForFocus * 2.0);
        context->ShowFocusAnimation(
            RRect::MakeRRect(Rect(offset, layoutSize), Radius(NormalizeToPx(focusRadiusDimension_))),
            focusAnimationColor_, position + offset);
    }
}

void RenderTabBar::ApplyGradientColor()
{
    auto parent = GetParent().Upgrade();
    RefPtr<RenderBox> box = AceType::DynamicCast<RenderBox>(parent);
    if (box) {
        Color colorA = box->GetColor();
        Color colorB = colorA.ChangeAlpha(0);

        double viewWidth = GetLayoutSize().Width();
        double gradientWidthPx = NormalizeToPx(gradientWidth_);

        // only box's width is big enough will add gradient
        if (viewWidth > DOUBLE_FACTOR * gradientWidthPx && !isVertical_) {
            auto frontDecoration = box->GetFrontDecoration();
            if (!frontDecoration) {
                frontDecoration = AceType::MakeRefPtr<Decoration>();
            }
            auto backDecoration = box->GetBackDecoration();
            if (backDecoration) {
                frontDecoration->SetBorder(backDecoration->GetBorder());
            }
            Gradient gradient = Gradient();
            gradient.SetDirection(GradientDirection::RIGHT);
            std::vector<GradientColor> gradientColors = std::vector<GradientColor>(GRADIENT_POINT_SIZE);

            gradientColors[0].SetColor(colorA);
            gradientColors[0].SetDimension(Dimension(0.0, DimensionUnit::PX));

            gradientColors[1].SetColor(colorB);
            gradientColors[1].SetDimension(Dimension(gradientWidthPx, DimensionUnit::PX));

            gradientColors[2].SetColor(colorB);
            gradientColors[2].SetDimension(Dimension(viewWidth - gradientWidthPx, DimensionUnit::PX));

            gradientColors[3].SetColor(colorA);
            gradientColors[3].SetDimension(Dimension(viewWidth, DimensionUnit::PX));

            for (const auto& gradientColor : gradientColors) {
                gradient.AddColor(gradientColor);
            }

            frontDecoration->SetGradient(gradient);

            box->SetFrontDecoration(frontDecoration);
        }
    }
}

TabBarIndicatorStyle RenderTabBar::GetIndicatorStyle(const RefPtr<BoxComponent>& component) const
{
    RefPtr<TabBarIndicatorComponent> indicatorComponent = AceType::DynamicCast<TabBarIndicatorComponent>(component);
    if (indicatorComponent) {
        return indicatorComponent->GetIndicatorStyle();
    }
    return TabBarIndicatorStyle::CUSTOM;
}

void RenderTabBar::InitScrollableOffset(TabBarMode mode)
{
    if (mode != mode_) {
        scrollableOffset_.Reset();
    }
}

RefPtr<RenderNode> RenderTabBar::GetChildByIndex(int32_t index) const
{
    int32_t size = static_cast<int32_t>(GetChildren().size());
    if (index < 0 || index >= size) {
        return nullptr;
    }
    auto pos = GetChildren().begin();
    std::advance(pos, index);
    return *pos;
}

} // namespace OHOS::Ace
