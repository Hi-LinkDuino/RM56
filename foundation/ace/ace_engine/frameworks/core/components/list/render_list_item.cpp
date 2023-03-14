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

#include "core/components/list/render_list_item.h"

#include "base/log/dump_log.h"
#include "base/utils/system_properties.h"
#include "core/common/frontend.h"
#include "core/common/vibrator/vibrator_proxy.h"
#include "core/components/box/render_box.h"
#include "core/components/flex/render_flex.h"
#include "core/components/list/render_list.h"
#include "core/components/list/render_list_item_group.h"
#include "core/components/list/tv_interactive_effect.h"
#ifdef WEARABLE_PRODUCT
#include "core/components/list/watch_interactive_effect.h"
#endif
#include "core/components/proxy/render_item_proxy.h"

namespace OHOS::Ace {
namespace {

const double HALF_SIZE = 0.5;
const double ITEM_DISTANCE_BASE = 76.0;
const double ITEM_DEFAULT_SCALE = 1.0;
const double ITEM_ZERO = 0.0;
const double ITEM_SCALE_BASE = 1.12;
const double ITEM_OPACITY_BASE = 1.0;
const double ITEM_RATIO = -0.34; // 0.78 - 1.12  // 0.66 - 1.0
const double DISTANCE_EPSILON = 1.0;
constexpr int32_t MIN_COMPATITABLE_VERSION = 5;
const double WATCH_SIZE = 466.0;

#ifdef WEARABLE_PRODUCT
const std::string& VIBRATOR_TYPE_WATCH_CROWN_STRENGTH1 = "watchhaptic.crown.strength1";
#endif

} // namespace

RenderListItem::RenderListItem()
{
    Initialize();
}

void RenderListItem::Initialize()
{
    clickRecognizer_ = AceType::MakeRefPtr<ClickRecognizer>();
    clickRecognizer_->SetOnClick([weakItem = AceType::WeakClaim(this)](const ClickInfo&) {
        auto item = weakItem.Upgrade();
        if (item) {
            item->HandleClicked();
        }
    });
    touchRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
    touchRecognizer_->SetOnTouchDown([weakItem = AceType::WeakClaim(this)](const TouchEventInfo&) {
        auto item = weakItem.Upgrade();
        if (item && item->GetSupportClick()) {
            item->PlayPressDownAnimation();
        }
    });
    touchRecognizer_->SetOnTouchUp([weakItem = AceType::WeakClaim(this)](const TouchEventInfo&) {
        auto item = weakItem.Upgrade();
        if (item && item->GetSupportClick()) {
            item->PlayPressUpAnimation();
        }
    });
    touchRecognizer_->SetOnTouchCancel([weakItem = AceType::WeakClaim(this)](const TouchEventInfo&) {
        auto item = weakItem.Upgrade();
        if (item && item->GetSupportClick()) {
            item->PlayPressUpAnimation();
        }
    });
}

void RenderListItem::PerformLayout()
{
    if (!GetChildren().empty()) {
        auto child = GetChildren().front();
        auto context = context_.Upgrade();
        if (NeedDivider() && context && context->GetIsDeclarative()) {
            auto layoutParam = GetLayoutParam();
            auto maxSize = layoutParam.GetMaxSize();
            auto dividerSize = IsListVertical() ? Size(0.0, NormalizeToPx(dividerHeight_))
                                                : Size(NormalizeToPx(dividerHeight_), 0.0);
            maxSize = maxSize - dividerSize;
            layoutParam.SetMaxSize(maxSize);
            child->Layout(layoutParam);
        } else {
            child->Layout(GetLayoutParam());
        }
        child->SetPosition(Offset::Zero());
        if (NeedDivider()) {
            auto dividerSize = IsListVertical() ? Size(0.0, NormalizeToPx(dividerHeight_))
                : Size(NormalizeToPx(dividerHeight_), 0.0);
            SetLayoutSize(dividerSize + child->GetLayoutSize());
        } else {
            SetLayoutSize(child->GetLayoutSize());
        }
        // update focus animation size
        focusAnimationRRect_.SetRect(Rect(Offset(0, 0), GetPaintSize() * TV_ITEM_SCALE));

        if (SystemProperties::GetDeviceType() == DeviceType::WATCH) {
            CalculateScaleFactorOnWatch();
        }
    }
}

void RenderListItem::CalculateScaleFactorOnWatch()
{
    if (isTitle_) {
        return;
    }

    auto context = GetContext().Upgrade();
    const static int32_t PLATFORM_VERSION_FIVE = 5;
    if (context && context->GetMinPlatformVersion() <= PLATFORM_VERSION_FIVE) {
        Offset itemCenter = GetGlobalOffset() + GetLayoutSize() * HALF_SIZE;
        double scale = ITEM_DEFAULT_SCALE;
        double viewScale = ITEM_DEFAULT_SCALE;
        auto pipelineContext = context_.Upgrade();
        if (pipelineContext) {
            scale = pipelineContext->GetDipScale();
            viewScale = pipelineContext->GetViewScale();
        }
        if (NearZero(scale) || NearZero(viewScale)) {
            LOGE("pipeline parameter is invalid");
            return;
        }
        auto distance = std::abs(itemCenter.GetY() - WATCH_SIZE / viewScale * HALF_SIZE);
        auto ratio = std::pow(distance, SQUARE) * ITEM_RATIO / std::pow(ITEM_DISTANCE_BASE * scale, SQUARE);
        scaleFactor_ = std::max(ITEM_ZERO, ratio + ITEM_SCALE_BASE);
        opacityFactor_ = std::max(ITEM_ZERO, ratio + ITEM_OPACITY_BASE);
    } else {
        auto renderList = GetRenderList();
        if (!renderList) {
            LOGE("Can not find parent render list");
            return;
        }

        double itemSize = renderList->IsVertical() ? GetLayoutSize().Height() : GetLayoutSize().Width();
        double itemCenter = GetPositionInList() + renderList->GetListPosition() + itemSize * HALF_SIZE;
        double scale = ITEM_DEFAULT_SCALE;
        auto pipelineContext = context_.Upgrade();
        if (pipelineContext) {
            scale = pipelineContext->GetDipScale();
        }
        if (NearZero(scale)) {
            LOGE("Pipeline parameter is invalid");
            return;
        }
        double viewPort = renderList->IsVertical() ? viewPort_.Height() : viewPort_.Width();
        auto distance = std::abs(itemCenter - viewPort * HALF_SIZE);
        if (NearZero(distance, DISTANCE_EPSILON)) {
            distance = 0.0;
        }
        auto ratio = std::pow(distance, SQUARE) * ITEM_RATIO / std::pow(ITEM_DISTANCE_BASE * scale, SQUARE);
        scaleFactor_ = std::max(ITEM_ZERO, ratio + ITEM_SCALE_BASE);
        opacityFactor_ = std::max(ITEM_ZERO, ratio + ITEM_OPACITY_BASE);
    }
}

void RenderListItem::Update(const RefPtr<Component>& component)
{
    itemComponent_ = component;
    auto item = AceType::DynamicCast<ListItemComponent>(component);
    if (item) {
        type_ = item->GetType();
        index_ = item->GetIndex();
        columnSpan_ = item->GetColumnSpan();
        op_ = item->GetOperation();
        flags_ = item->GetFlags();
        // update focus animation color
        focusAnimationColor_ = item->GetFocusAnimationColor();
        // update focus animation corner radius
        focusAnimationRRect_.SetCorner({ item->GetTopLeftRadius(), item->GetTopRightRadius(),
            item->GetBottomRightRadius(), item->GetBottomLeftRadius() });
        LOGD("[Focus][Dep:%{public}d] Update: focusAnimationRRect top left radius: %{public}.1lf", GetDepth(),
            focusAnimationRRect_.GetCorner().topLeftRadius.GetX().Value());

        needVibrate_ = item->NeedVibrate();
        auto context = context_.Upgrade();
        if (needVibrate_ && !vibrator_ && context) {
            vibrator_ = VibratorProxy::GetInstance().GetVibrator(context->GetTaskExecutor());
        }

        rotationVibrate_ = item->IsRotationVibrate();
        if (rotationVibrate_ && !vibrator_ && context) {
            vibrator_ = VibratorProxy::GetInstance().GetVibrator(context->GetTaskExecutor());
        }

        supportScale_ = item->GetSupportScale();
        supportOpacity_ = item->GetSupportOpacity();
        supportClick_ = item->GetSupportClick();
        isTitle_ = item->IsTitle();
        sticky_ = item->GetSticky();
        stickyMode_ = item->GetStickyMode();
        clickColor_ = item->GetClickColor();
        selfAlign_ = item->GetAlignSelf();
        Dimension radius = item->GetStickyRadius();
        if (radius.IsValid()) {
            stickyRadius_ = context ? context->NormalizeToPx(radius) : stickyRadius_;
        }

        if (!primary_ && item->GetPrimary()) {
            NotifyGroupPrimaryChange();
        }
        SetPrimary(item->GetPrimary());

        // list divider
        needDivider_ = item->NeedDivider();
        isActive_ = item->IsActive();
        dividerLength_ = item->GetDividerLength();
        dividerHeight_ = item->GetDividerHeight();
        dividerOrigin_ = item->GetDividerOrigin();
        dividerColor_ = item->GetDividerColor();
        auto onClickId = item->GetClickEventId();
        clickEvent_ = AceAsyncEvent<void()>::Create(onClickId, GetContext());
        clickRecognizer_->SetUseCatchMode(true);
        if (!onClickId.GetCatchMode()) {
            static const int32_t bubbleModeVersion = 6;
            auto pipeline = context_.Upgrade();
            if (pipeline && pipeline->GetMinPlatformVersion() >= bubbleModeVersion) {
                clickRecognizer_->SetUseCatchMode(false);
                return;
            }
        }
        stickyEvent_ = AceAsyncEvent<void(const std::string&)>::Create(item->GetStickyEventId(), GetContext());
        transitionEffect_ = item->GetTransitionEffect();
        UpdateAccessibilityAttr();
        MarkNeedLayout();
    }
}

void RenderListItem::NotifyGroupPrimaryChange()
{
    auto parent = GetParent().Upgrade();
    while (parent) {
        auto group = AceType::DynamicCast<RenderListItemGroup>(parent);
        if (group) {
            group->ItemPrimaryChange(GetIndex());
            break;
        }
        parent = parent->GetParent().Upgrade();
    }
}

double RenderListItem::GetPositionInList() const
{
    RefPtr<RenderNode> parentNode = GetParent().Upgrade();
    while (parentNode) {
        RefPtr<RenderList> listNode = AceType::DynamicCast<RenderList>(parentNode);
        if (listNode) {
            return listNode->GetItemPosition(index_);
        }
        parentNode = parentNode->GetParent().Upgrade();
    }
    return 0.0;
}

bool RenderListItem::IsItemCenter(bool isVertical, Size viewport)
{
    auto parent = GetParent().Upgrade();
    if (!parent) {
        return false;
    }
    Size itemSize = GetLayoutSize();
    Offset itemPosition = parent->GetPosition();
    double size = isVertical ? itemSize.Height() : itemSize.Width();
    double position = isVertical ? itemPosition.GetY() : itemPosition.GetX();
    double center = (isVertical ? viewport.Height() : viewport.Width()) / 2.0;
    return center > position && center < (position + size);
}

void RenderListItem::ResetFocusEffect()
{
#ifdef WEARABLE_PRODUCT
    if (SystemProperties::GetDeviceType() == DeviceType::WATCH) {
        focusController_ = AceType::MakeRefPtr<WatchInteractiveEffect>(GetContext());
#else
    if (SystemProperties::GetDeviceType() == DeviceType::TV) {
        focusController_ = AceType::MakeRefPtr<TVInteractiveEffect>(GetContext());
#endif
    } else {
        focusController_ = AceType::MakeRefPtr<InteractiveEffect>(GetContext());
    }
    focusController_->Initialize(GetThemeManager());
    focusController_->SetItemNode(AceType::WeakClaim(this));
    auto listNode = GetRenderList();
    if (listNode) {
        auto weakList = WeakPtr<RenderList>(listNode);
        focusController_->SetListNode(weakList);
    }
}

void RenderListItem::HandleItemEffect(bool isFromRotate)
{
    if (!focusController_) {
        ResetFocusEffect();
    }

    if (currentState_ != lastState_) {
#ifdef WEARABLE_PRODUCT
        if (needVibrate_ && lastState_ == ItemState::NEARBY && currentState_ == ItemState::FOCUS && vibrator_) {
            vibrator_->Vibrate(VIBRATOR_TYPE_WATCH_CROWN_STRENGTH1);
        }
        if (rotationVibrate_ && isFromRotate && vibrator_) {
            vibrator_->Vibrate(VIBRATOR_TYPE_WATCH_CROWN_STRENGTH1);
        }
#endif
        LOGD("item (%{public}d) change from %{public}d to %{public}d.", index_, lastState_, currentState_);
        lastState_ = currentState_;
        if (currentState_ != ItemState::NONE && currentState_ != ItemState::CLICK) {
            focusController_->ShowAnimation(currentState_);
        } else {
            // invalid focus
            LOGD("focus state invalid.");
        }
    }
    MarkNeedRender();
}

void RenderListItem::HandleClicked()
{
    if (clickEvent_) {
        clickEvent_();
    }

    if (primary_ && clicked_) {
        clicked_();
    }
}

void RenderListItem::PlayPressDownAnimation()
{
    if (!focusController_) {
        ResetFocusEffect();
    }
    pressAnimation_ = true;
    focusController_->TouchDownAnimation();
}

void RenderListItem::PlayPressUpAnimation()
{
    if (!focusController_) {
        ResetFocusEffect();
    }
    pressAnimation_ = false;
    focusController_->TouchUpAnimation();
}

void RenderListItem::OnCancelPressAnimation()
{
    if (!pressAnimation_) {
        return;
    }
    if (!focusController_) {
        ResetFocusEffect();
    }
    focusController_->CancelTouchAnimation();
}

void RenderListItem::HandleStickyEvent(bool sticky)
{
    std::string state = sticky ? "true" : "false";
    std::string param = std::string(R"("sticky",{"state":)").append(state).append("},null");
    if (stickyEvent_) {
        stickyEvent_(param);
    }
}

void RenderListItem::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!GetVisible() && !GetClonedBySticky()) {
        return;
    }
    // supportClick means show click effect
    bool supportClick = supportClick_;
    auto pipeline = context_.Upgrade();
    if (pipeline && pipeline->GetMinPlatformVersion() > MIN_COMPATITABLE_VERSION) {
        supportClick = true;
    }
    if ((!touchRecognizer_) || (!clickRecognizer_) || (!supportClick)) {
        return;
    }
    touchRecognizer_->SetCoordinateOffset(coordinateOffset);
    clickRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(touchRecognizer_);
    result.emplace_back(clickRecognizer_);
}

void RenderListItem::UpdateItemFocusRect(double scale)
{
    focusAnimationRRect_.SetRect(Rect(Offset(0.0, 0.0), GetPaintSize() * scale));
}

void RenderListItem::HandleFocusEvent(bool focus, bool isInGroup)
{
    if (SystemProperties::GetDeviceType() == DeviceType::WATCH) {
        return;
    }
    focused_ = focus;

    ShowFocusAnimation(focus, Rect(0.0, 0.0, 0.0, 0.0));

    if (focus) {
        ChangeStatus(RenderStatus::FOCUS);
        currentState_ = ItemState::FOCUS;
        if (isInGroup) {
            HandleItemEffect();
            return;
        }
        RefPtr<RenderNode> parentNode = GetParent().Upgrade();
        while (parentNode) {
            RefPtr<RenderList> listNode = AceType::DynamicCast<RenderList>(parentNode);
            if (listNode) {
                listNode->ListItemFocused(index_);
                break;
            }
            parentNode = parentNode->GetParent().Upgrade();
        }
    } else {
        ChangeStatus(RenderStatus::BLUR);
        currentState_ = ItemState::BLUR;
    }
    HandleItemEffect();
}

Offset RenderListItem::GetPaintOffset() const
{
    auto globalOffset = GetGlobalOffset();
    auto layoutSize = GetLayoutSize(); // size include margin
    auto paintSize = GetPaintSize();   // size exclude margin
    auto margin = GetMarginInPx();
    if (layoutSize > paintSize) {
        globalOffset.SetX(globalOffset.GetX() + margin.LeftPx());
        globalOffset.SetY(globalOffset.GetY() + margin.TopPx());
    }
    return globalOffset;
}

EdgePx RenderListItem::GetMarginInPx() const
{
    EdgePx marginInPx;
    auto children = GetChildren();
    if (children.empty()) {
        return marginInPx;
    }

    auto child = children.front();
    while (child) {
        auto box = AceType::DynamicCast<RenderBoxBase>(child);
        if (box) {
            auto boxChildren = box->GetChildren();
            if (boxChildren.empty()) {
                break;
            }
            auto boxChild = boxChildren.front();
            if (boxChild && AceType::DynamicCast<RenderFlex>(boxChild)) {
                marginInPx = box->GetMargin();
                break;
            }
        }
        children = child->GetChildren();
        if (children.empty()) {
            break;
        }
        child = children.front();
    }
    return marginInPx;
}

Size RenderListItem::GetPaintSize() const
{
    Size size = GetLayoutSize();
    auto margin = GetMarginInPx();
    return size - margin.GetLayoutSize();
}

Border RenderListItem::GetFocusBorder() const
{
    Border border;
    border.SetTopRightRadius(focusAnimationRRect_.GetCorner().topRightRadius);
    border.SetTopLeftRadius(focusAnimationRRect_.GetCorner().topLeftRadius);
    border.SetBottomLeftRadius(focusAnimationRRect_.GetCorner().bottomLeftRadius);
    border.SetBottomRightRadius(focusAnimationRRect_.GetCorner().bottomRightRadius);
    return border;
}

void RenderListItem::ShowFocusAnimation(bool focus, const Rect& listRect, double scale)
{
    // paint focus animation
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("[Focus]Pipeline context is nullptr");
        return;
    }

    LOGD("Index:%{public}d focus:%{public}d %{public}s", index_, focus, GetPaintOffset().ToString().c_str());
    if (focus) {
        Size size = GetPaintSize();
        auto globalOffset = GetPaintOffset() + (size * (DEFAULT_SCALE - scale) * HALF_SIZE);
        if (listRect.IsValid()) {
            context->ShowFocusAnimation(focusAnimationRRect_, focusAnimationColor_, globalOffset, listRect);
            context->ShowShadow(focusAnimationRRect_, globalOffset);
        } else {
            context->ShowFocusAnimation(focusAnimationRRect_, focusAnimationColor_, globalOffset);
            context->ShowShadow(focusAnimationRRect_, globalOffset);
        }
    } else {
        context->CancelFocusAnimation();
        context->CancelShadow();
    }
}

void RenderListItem::UpdateAccessibilityAttr()
{
    auto refPtr = accessibilityNode_.Upgrade();
    if (!refPtr) {
        LOGD("Get accessibility node failed.");
        return;
    }

    refPtr->SetClickableState(true);
    refPtr->SetActionClickImpl([weakItem = AceType::WeakClaim(this)]() {
        auto item = weakItem.Upgrade();
        if (item) {
            LOGI("Trigger ActionClick by Accessibility(%{public}d).", item->index_);
            item->HandleClicked();
            item->OnGroupClicked();
        }
    });

    refPtr->SetFocusableState(true);
    refPtr->SetActionFocusImpl([weakItem = AceType::WeakClaim(this)]() {
        auto item = weakItem.Upgrade();
        if (item) {
            LOGI("Trigger ActionFocus by Accessibility(%{public}d).", item->index_);
            item->MoveToViewPort();
        }
    });
    refPtr->AddSupportAction(AceAction::ACTION_ACCESSIBILITY_FOCUS);
}

void RenderListItem::OnGroupClicked()
{
    if (!primary_ || !curPrimary_) {
        return; // Only trigger group click when current item is primary.
    }
    RefPtr<RenderNode> parent = GetParent().Upgrade();
    while (parent) {
        RefPtr<RenderListItemGroup> group = AceType::DynamicCast<RenderListItemGroup>(parent);
        if (group) {
            return group->HandleClicked();
        }
        parent = parent->GetParent().Upgrade();
    }
}

void RenderListItem::MoveToViewPort()
{
    RefPtr<RenderNode> parentNode = GetParent().Upgrade();
    while (parentNode) {
        RefPtr<RenderList> listNode = AceType::DynamicCast<RenderList>(parentNode);
        if (listNode) {
            return listNode->MoveItemToViewPort(GetPositionInList());
        }
        parentNode = parentNode->GetParent().Upgrade();
    }
}

RefPtr<RenderList> RenderListItem::GetRenderList() const
{
    auto parent = GetParent().Upgrade();
    while (parent) {
        auto parentNode = AceType::DynamicCast<RenderList>(parent);
        if (parentNode) {
            return parentNode;
        }
        parent = parent->GetParent().Upgrade();
    }
    return nullptr;
}

bool RenderListItem::NeedDivider()
{
    return needDivider_ && !IsLastItem();
}

bool RenderListItem::IsLastItem()
{
    auto renderList = GetRenderList();
    if (renderList) {
        int32_t maxCount = renderList->GetMaxCount();
        if (GetIndex() == maxCount - 1) {
            return true;
        }
    }
    return false;
}

bool RenderListItem::IsListVertical()
{
    auto renderList = GetRenderList();
    if (renderList) {
        FlexDirection listDirection = renderList->GetDirection();
        return listDirection == FlexDirection::COLUMN || listDirection == FlexDirection::COLUMN_REVERSE;
    }
    return true;
}

void RenderListItem::Dump()
{
    DumpLog::GetInstance().AddDesc(std::string("index: ").append(std::to_string(GetIndex())));
}

void RenderListItem::RunCardTransitionAnimation(double shiftHeight)
{
    auto renderList = GetRenderList();
    if (!renderList) {
        LOGE("list render is null");
        return;
    }
    makeCardTransition_ = true;
    renderList->SetMakeCardTransition(makeCardTransition_);
    if (transitionEffect_ == TransitionEffect::UNFOLD) {
        renderList->SetShiftHeight(shiftHeight);
    } else {
        renderList->SetShiftHeight(0.0);
    }
    renderList->MarkNeedRender();
}

void RenderListItem::StopCardTransitionAnimation()
{
    makeCardTransition_ = false;
    auto renderList = GetRenderList();
    if (renderList) {
        renderList->SetMakeCardTransition(makeCardTransition_);
        renderList->SetShiftHeight(0.0);
        renderList->MarkNeedRender();
    }
}

RRect RenderListItem::GetRRect() const
{
    auto child = GetFirstChild();
    while (child) {
        auto childNode = AceType::DynamicCast<RenderBox>(child);
        if (childNode) {
            auto margin = childNode->GetMargin();
            auto rrect = RRect(Rect(childNode->GetGlobalOffset() + margin.GetOffset(),
                childNode->GetLayoutSize() - margin.GetLayoutSize()));
            if (childNode->GetBackDecoration()) {
                auto border = childNode->GetBackDecoration()->GetBorder();
                rrect.SetCorner({ border.TopLeftRadius(), border.TopRightRadius(),
                    border.BottomLeftRadius(), border.BottomRightRadius() });
            }
            return rrect;
        }
        child = child->GetFirstChild();
    }
    return RRect();
}

} // namespace OHOS::Ace
