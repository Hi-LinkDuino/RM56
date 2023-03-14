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

#include "core/components/list/render_list_item_group.h"

#include "core/animation/curve.h"
#include "core/animation/curve_animation.h"
#include "core/components/flex/render_flex.h"
#include "core/components/list/list_item_group_component.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {
namespace {

constexpr Dimension BOX_SIZE_WIDTH = 48.0_vp;
constexpr Dimension MARGIN_RIGHT_WIDTH = 2.0_vp;
constexpr int32_t ANIMATION_DURATION = 250;
constexpr float POSITION_ANIMATION_EXPAND_TIME = 0.8f;
constexpr float POSITION_ANIMATION_COLLAPSE_TIME = 0.333f;
constexpr float OPACITY_ANIMATION_EXPAND_TIME = 0.4f;
constexpr float OPACITY_ANIMATION_COLLAPSE_TIME = 0.5f;
constexpr float ROTATE_ANGLE = 180.0f;
constexpr float ROTATE_ANIMATION_EXPAND_TIME = 0.8f;
constexpr float ROTATE_ANIMATION_COLLAPSE_TIME = 0.667f;
const char LIST_ITEM_GROUP_EVENT_GROUPCLICK[] = "groupclick";
const char LIST_ITEM_GROUP_EVENT_GROUPCOLLAPSE[] = "groupcollapse";
const char LIST_ITEM_GROUP_EVENT_GROUPEXPAND[] = "groupexpand";

} // namespace

std::string RenderListItemGroup::MakeEventParam(const std::string& command)
{
    return std::string("\"").append(command).append("\", ").append(R"({"groupid": ")").append(groupId_).append("\"}");
}

void RenderListItemGroup::HandleClicked()
{
    ResetChildVisibleState();
    expand_ = !expand_;
    UpdateGroupComponentStatus(expand_);
    UpdateExpandStatusInList();
    SetAnimationStop();

    if (onClicked_) {
        LOGD("list item group: onclicked");
        onClicked_(MakeEventParam(LIST_ITEM_GROUP_EVENT_GROUPCLICK));
    }
    FireExpandEvent();
    MarkNeedLayout();
    LOGD("expand_: %{public}d", expand_);
}

void RenderListItemGroup::FireExpandEvent()
{
    if (expand_ && onExpand_) {
        LOGD("list item group: onexpand");
        onExpand_(MakeEventParam(LIST_ITEM_GROUP_EVENT_GROUPEXPAND));
    }

    if (!expand_ && onCollapse_) {
        LOGD("list item group: oncollapse");
        onCollapse_(MakeEventParam(LIST_ITEM_GROUP_EVENT_GROUPCOLLAPSE));
    }
}

LayoutParam RenderListItemGroup::MakeInnerLayoutParam() const
{
    LayoutParam innerLayout;
    Size maxSize;

    if (direction_ == FlexDirection::ROW) {
        maxSize = Size(Size::INFINITE_SIZE, GetLayoutParam().GetMaxSize().Height());
    } else {
        maxSize = Size(GetLayoutParam().GetMaxSize().Width(), Size::INFINITE_SIZE);
    }
    innerLayout.SetMaxSize(maxSize);
    return innerLayout;
}

void RenderListItemGroup::AddArrowImage(double mainSize)
{
    if (arrowImage_) {
        if (!GetChildren().empty() && GetChildren().back() != arrowImage_) {
            RemoveChild(arrowImage_);
            AddChild(arrowImage_, GetChildren().size());
        }
        LayoutParam innerLayout;
        double width = NormalizeToPx(imageSize_);
        innerLayout.SetFixedSize(Size(width, width));
        arrowImage_->Layout(innerLayout);

        Size primarySize = primary_->GetLayoutSize();
        double offsetX = 0.0;
        if (rightToLeft_) {
            if (direction_ == FlexDirection::COLUMN) {
                offsetX = NormalizeToPx(MARGIN_RIGHT_WIDTH);
            } else {
                offsetX = std::max(
                    mainSize - NormalizeToPx(BOX_SIZE_WIDTH + MARGIN_RIGHT_WIDTH + imageSize_) * FACTOR_HALF, 0.0);
            }
        } else {
            offsetX = std::max(
                primarySize.Width() - NormalizeToPx(BOX_SIZE_WIDTH + MARGIN_RIGHT_WIDTH + imageSize_) * FACTOR_HALF,
                0.0);
        }
        double offsetY = 0.0;
        auto renderBox = GetRenderBox(primary_);
        if (renderBox) {
            double marginTop = NormalizeToPx(renderBox->GetMargin().Top());
            double marginRight = NormalizeToPx(renderBox->GetMargin().Right());
            double boxPaintHeight = renderBox->GetPaintSize().Height();
            offsetY = marginTop + std::max((boxPaintHeight - width) * FACTOR_HALF, 0.0);
            offsetX = std::max(offsetX - marginRight, 0.0);
        } else {
            offsetY = std::max((primarySize.Height() - width) * FACTOR_HALF, 0.0);
        }
        arrowImage_->SetPosition(Offset(offsetX, offsetY));
        double rotateAngle = expand_ ? ROTATE_ANGLE : 0.0;
        arrowImage_->SetRotate(rotateAngle);
    }
}

bool RenderListItemGroup::NeedRebuild() const
{
    auto children = GetChildren();
    if (children.empty()) {
        LOGE("list item group has no list item child");
        return false;
    }
    bool needRebuild = false;
    for (const auto& iter : children) {
        auto listItemNode = RenderListItem::GetRenderListItem(iter);
        if (listItemNode && listItemNode->GetPrimaryChange()) {
            needRebuild = true;
            listItemNode->SetPrimaryChange(false);
        }
    }
    return needRebuild;
}

void RenderListItemGroup::GetPrimaryItem()
{
    auto children = GetChildren();
    if (children.empty()) {
        LOGE("list item group has no list item child");
        primary_ = nullptr;
        return;
    }
    primary_ = nullptr;
    // Find primary item.
    for (const auto& iter : children) {
        auto listItemNode = RenderListItem::GetRenderListItem(iter);
        if (listItemNode) {
            if (listItemNode->GetPrimary()) {
                primary_ = iter;
                break;
            }
            if (!primary_) {
                primary_ = iter;
            }
        }
    }
    auto primaryListItem = RenderListItem::GetRenderListItem(primary_);
    if (!primaryListItem) {
        LOGE("list item group has no primary child");
        primary_ = nullptr;
        return;
    }
    if (!primaryListItem->GetPrimary()) {
        isDefaultPrimary_ = true;
    }
    primaryListItem->SetPrimary(true);
    primaryListItem->SetCurPrimary(true);
    primaryListItem->SetIndex(0);
    primaryListItem->RegisterClickedCallback([weakItemGroup = AceType::WeakClaim(this)]() {
        auto listItemGroup = weakItemGroup.Upgrade();
        if (listItemGroup) {
            listItemGroup->HandleClicked();
        }
    });
}

void RenderListItemGroup::LayoutExpandableList(double mainSize)
{
    double primarySize = 0.0;
    if (primary_) {
        primarySize = GetMainSize(primary_->GetLayoutSize());
    }
    if (NearEqual(mainSize, primarySize)) {
        ResetChildVisibleState();
        return;
    }
    double curSize = 0.0;
    auto children = GetChildren();
    auto rIter = children.rbegin();
    while (rIter != children.rend()) {
        auto child = *rIter++;
        if (child == primary_ || child == arrowImage_) {
            continue;
        }

        double childSize = GetMainSize(child->GetLayoutSize());
        if (childSize > mainSize - curSize - primarySize) {
            child->SetVisible(false);
            break;
        }
        child->SetVisible(true);
        if (rightToLeft_ && direction_ == FlexDirection::ROW) {
            child->SetPosition(MakeValue<Offset>(curSize, 0.0));
        } else {
            child->SetPosition(MakeValue<Offset>(mainSize - curSize - childSize, 0.0));
        }
        curSize += childSize;
        if (curSize + primarySize >= mainSize) {
            break;
        }
    }

    while (rIter != children.rend()) {
        auto child = *rIter++;
        if (child == primary_ || child == arrowImage_) {
            continue;
        }
        child->SetVisible(false);
    }

    MarkNeedLayout();
}

void RenderListItemGroup::SetAnimationStop()
{
    animating_ = false;
}

void RenderListItemGroup::PerformLayout()
{
    LOGD("RenderListItemGroup::PerformLayout: %{public}d", animating_);
    if (animating_) {
        return;
    }

    GetPrimaryItem();
    if (!primary_) {
        ResetLayout();
        return;
    }

    auto forceLayout = false;
    if (NeedRebuild() && rebuild_) {
        rebuild_();
        forceLayout = true;
        GetPrimaryItem();
    }
    double curMainSize = GetMainSize(GetLayoutSize());
    auto layoutParam = MakeInnerLayoutParam();
    primary_->Layout(layoutParam);

    double mainSize = GetMainSize(primary_->GetLayoutSize());
    double crossSize = std::max(0.0, GetCrossSize(primary_->GetLayoutSize()));

    // Layout other items.
    int32_t index = 1;
    for (const auto& child : GetChildren()) {
        if (child == primary_ || child == arrowImage_) {
            continue;
        }
        auto listItemNode = RenderListItem::GetRenderListItem(child);
        if (listItemNode) {
            listItemNode->SetIndex(index++);
        }
        if (expand_ || IsExpanded()) {
            child->Layout(layoutParam);
            mainSize += GetMainSize(child->GetLayoutSize());
            crossSize = std::max(crossSize, GetCrossSize(primary_->GetLayoutSize()));
        }
    }
    AddArrowImage(mainSize);

    if (rightToLeft_ && direction_ == FlexDirection::ROW) {
        primary_->SetPosition(Offset(mainSize - GetMainSize(primary_->GetLayoutSize()), 0.0));
    } else {
        primary_->SetPosition(Offset::Zero());
    }

    double startPosition = curMainSize;
    double endPosition = mainSize;
    LOGD("startPosition: %{public}lf, endPosition: %{public}lf", startPosition, endPosition);
    if (!NearZero(startPosition) && !forceLayout) {
        if (NearEqual(startPosition, endPosition)) {
            ResetChildVisibleState();
            LOGE("equal return");
            return;
        }
        AnimationPerformLayout(crossSize, startPosition, endPosition);
        SetLayoutSize(MakeValue<Size>(startPosition, crossSize));
    } else {
        LayoutExpandableList(endPosition);
        SetLayoutSize(MakeValue<Size>(endPosition, crossSize));
        LOGD("layoutSize(no animation): %{public}s", GetLayoutSize().ToString().c_str());
    }
}

bool RenderListItemGroup::IsExpanded()
{
    bool flag = expand_;
    auto listParent = GetParent().Upgrade();
    RefPtr<RenderList> list = nullptr;
    while (listParent) {
        if (AceType::InstanceOf<RenderList>(listParent)) {
            list = AceType::DynamicCast<RenderList>(listParent);
            break;
        }
        listParent = listParent->GetParent().Upgrade();
    }
    if (list) {
        flag = expand_ || list->GetLayoutManager()->GetExpandStatus(GetIndex());
    }
    return flag;
}

void RenderListItemGroup::AnimationPerformLayout(double crossSize, double startPosition, double endPosition)
{
    if (!animator_->IsStopped()) {
        animator_->Stop();
    }
    animator_->ClearInterpolators();

    RefPtr<KeyframeAnimation<double>> positionAnimation =
        createPositionAnimation(crossSize, startPosition, endPosition);

    auto opacityAnimation = CreateOpacityAnimation();
    auto rotateAnimation = CreateRotateAnimation();

    animator_->AddInterpolator(positionAnimation);
    animator_->AddInterpolator(opacityAnimation);
    animator_->AddInterpolator(rotateAnimation);

    animator_->SetDuration(ANIMATION_DURATION);
    animator_->Play();
    animating_ = true;
    animator_->AddStopListener([weakItemGroup = AceType::WeakClaim(this)]() {
        auto itemGroup = weakItemGroup.Upgrade();
        itemGroup->SetAnimationStop();
        itemGroup->SetChildStretch(false);
        LOGD("layoutSize(animation): %{public}s", itemGroup->GetLayoutSize().ToString().c_str());
    });
}

RefPtr<KeyframeAnimation<double>> RenderListItemGroup::createPositionAnimation(
    double crossSize, double startPosition, double endPosition)
{
    float secondKeyTime = expand_ ? POSITION_ANIMATION_EXPAND_TIME : POSITION_ANIMATION_COLLAPSE_TIME;
    double secondKeyValue = expand_ ? endPosition : startPosition;
    RefPtr<Curve> firstCurve;
    if (expand_) {
        firstCurve = Curves::FAST_OUT_LINEAR_IN;
    } else {
        firstCurve = Curves::LINEAR;
    }
    RefPtr<Curve> secondCurve;
    if (expand_) {
        secondCurve = Curves::LINEAR;
    } else {
        secondCurve = Curves::FRICTION;
    }
    auto positionKeyframe1 = MakeRefPtr<Keyframe<double>>(0.0f, startPosition);
    auto positionKeyframe2 = MakeRefPtr<Keyframe<double>>(secondKeyTime, secondKeyValue);
    positionKeyframe2->SetCurve(firstCurve);
    auto positionKeyframe3 = MakeRefPtr<Keyframe<double>>(1.0f, endPosition);
    positionKeyframe3->SetCurve(secondCurve);
    auto positionAnimation = MakeRefPtr<KeyframeAnimation<double>>();
    positionAnimation->AddKeyframe(positionKeyframe1);
    positionAnimation->AddKeyframe(positionKeyframe2);
    positionAnimation->AddKeyframe(positionKeyframe3);
    positionAnimation->AddListener([weakItemGroup = WeakClaim(this), crossSize](double value) {
        auto itemGroup = weakItemGroup.Upgrade();
        if (itemGroup) {
            itemGroup->LayoutExpandableList(value);
            itemGroup->SetLayoutSize(itemGroup->MakeValue<Size>(value, crossSize));
        }
    });
    return positionAnimation;
}

RefPtr<KeyframeAnimation<int32_t>> RenderListItemGroup::CreateOpacityAnimation()
{
    int32_t startOpacity = expand_ ? 0 : UINT8_MAX;
    int32_t endOpacity = UINT8_MAX - startOpacity;
    double secondKeyTime = expand_ ? OPACITY_ANIMATION_EXPAND_TIME : OPACITY_ANIMATION_COLLAPSE_TIME;
    int32_t secondKeyValue = expand_ ? startOpacity : endOpacity;
    RefPtr<Curve> firstCurve;
    if (expand_) {
        firstCurve = Curves::LINEAR;
    } else {
        firstCurve = Curves::FAST_OUT_LINEAR_IN;
    }
    RefPtr<Curve> secondCurve;
    if (expand_) {
        secondCurve = Curves::FRICTION;
    } else {
        secondCurve = Curves::LINEAR;
    }
    auto opacityKeyframe1 = MakeRefPtr<Keyframe<int32_t>>(0.0f, startOpacity);
    auto opacityKeyframe2 = MakeRefPtr<Keyframe<int32_t>>(secondKeyTime, secondKeyValue);
    opacityKeyframe2->SetCurve(firstCurve);
    auto opacityKeyframe3 = MakeRefPtr<Keyframe<int32_t>>(1.0f, endOpacity);
    opacityKeyframe3->SetCurve(secondCurve);
    auto opacityAnimation = MakeRefPtr<KeyframeAnimation<int32_t>>();
    opacityAnimation->AddKeyframe(opacityKeyframe1);
    opacityAnimation->AddKeyframe(opacityKeyframe2);
    opacityAnimation->AddKeyframe(opacityKeyframe3);
    opacityAnimation->AddListener([weakItemGroup = WeakClaim(this)](int32_t value) {
        auto itemGroup = weakItemGroup.Upgrade();
        if (itemGroup) {
            itemGroup->SetChildOpacity(value);
        }
    });
    return opacityAnimation;
}

void RenderListItemGroup::SetChildOpacity(int32_t opacity)
{
    auto children = GetChildren();
    auto rIter = children.rbegin();
    while (rIter != children.rend()) {
        auto child = *rIter++;
        if (child == primary_ || child == arrowImage_) {
            continue;
        }
        auto listItem = RenderListItem::GetRenderListItem(child);
        if (listItem) {
            listItem->SetStretch(true);
            listItem->SetExpandOpacity(opacity);
            listItem->MarkNeedRender();
        }
    }
    MarkNeedLayout();
}

void RenderListItemGroup::SetChildStretch(bool isStretch)
{
    auto children = GetChildren();
    auto rIter = children.rbegin();
    while (rIter != children.rend()) {
        auto child = *rIter++;
        if (child == primary_ || child == arrowImage_) {
            continue;
        }
        auto listItem = RenderListItem::GetRenderListItem(child);
        if (listItem) {
            listItem->SetStretch(isStretch);
        }
    }
}

void RenderListItemGroup::Update(const RefPtr<Component>& component)
{
    auto theme = GetTheme<ListItemTheme>();
    if (theme) {
        imageSize_ = theme->GetGroupImageSize();
    }

    if (!animator_) {
        animator_ = AceType::MakeRefPtr<Animator>(GetContext());
    }

    itemGroup_ = component;
    auto itemGroup = AceType::DynamicCast<ListItemGroupComponent>(itemGroup_);
    if (itemGroup) {
        groupId_ = itemGroup->GetGroupId();
        expand_ = itemGroup->GetExpand();
        onClicked_ = AceAsyncEvent<void(const std::string&)>::Create(itemGroup->GetOnClicked(), GetContext());
        onCollapse_ = AceAsyncEvent<void(const std::string&)>::Create(itemGroup->GetOnCollapse(), GetContext());
        onExpand_ = AceAsyncEvent<void(const std::string&)>::Create(itemGroup->GetOnExpand(), GetContext());

        if (initialUpdate_) {
            InitialImage();
            initialUpdate_ = false;
        }

        SetAnimationStop();
        MarkNeedLayout();
    }
    LOGD("RenderListItemGroup::Update");
    RenderListItem::Update(component);
    InitAccessibilityEventListener();
}

void RenderListItemGroup::InitAccessibilityEventListener()
{
    const auto& accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }
    accessibilityNode->AddSupportAction(AceAction::ACTION_CLICK);
    accessibilityNode->SetActionClickImpl([weakPtr = WeakClaim(this)]() {
        const auto& list = weakPtr.Upgrade();
        if (list) {
            list->HandleClicked();
        }
    });
}

void RenderListItemGroup::InitialImage()
{
    InternalResource::ResourceId resourceId = SystemProperties::GetDeviceType() == DeviceType::WATCH
                                                  ? InternalResource::ResourceId::WATCH_DOWN_ARROW_SVG
                                                  : InternalResource::ResourceId::DOWN_ARROW_SVG;
    double rotateAngle = expand_ ? ROTATE_ANGLE : 0.0;
    auto imageComponent = AceType::MakeRefPtr<ImageComponent>(resourceId);
    imageComponent->SetWidth(imageSize_);
    imageComponent->SetHeight(imageSize_);

    arrowImage_ = AceType::DynamicCast<RenderImage>(imageComponent->CreateRenderNode());
    arrowImage_->SetRotate(rotateAngle);
    arrowImage_->Attach(GetContext());
    arrowImage_->Update(imageComponent);
}

void RenderListItemGroup::ChangeDirection(FlexDirection direction)
{
    if (direction_ != direction) {
        LOGD("direction has change, reset item group layoutSize and primary position");
        direction_ = direction;
        expand_ = false;
        UpdateExpandStatusInList();
        ResetLayout();
    }
}

void RenderListItemGroup::ResetLayout()
{
    SetLayoutSize(Size(0.0, 0.0));
    if (primary_) {
        primary_->SetPosition(Offset::Zero());
    }
}

void RenderListItemGroup::UpdateExpandStatusInList()
{
    auto parentNode = GetParent().Upgrade();
    while (parentNode) {
        RefPtr<RenderList> listNode = AceType::DynamicCast<RenderList>(parentNode);
        if (listNode) {
            listNode->SetGroupState(GetIndex(), expand_);
            break;
        }
        parentNode = parentNode->GetParent().Upgrade();
    }
}

RefPtr<RenderBox> RenderListItemGroup::GetRenderBox(const RefPtr<RenderNode>& renderNode)
{
    RefPtr<RenderNode> box = renderNode;
    while (box) {
        auto& children = box->GetChildren();
        if (children.empty()) {
            return nullptr;
        }
        if (AceType::InstanceOf<RenderBox>(box) && AceType::InstanceOf<RenderFlex>(children.front())) {
            return AceType::DynamicCast<RenderBox>(box);
        }
        box = children.front();
    }
    return nullptr;
}

RefPtr<KeyframeAnimation<double>> RenderListItemGroup::CreateRotateAnimation()
{
    double secondKeyTime = expand_ ? ROTATE_ANIMATION_EXPAND_TIME : ROTATE_ANIMATION_COLLAPSE_TIME;
    auto rotateKeyframe1 = MakeRefPtr<Keyframe<double>>(0.0f, 0.0f);
    auto rotateKeyframe2 = MakeRefPtr<Keyframe<double>>(secondKeyTime, ROTATE_ANGLE);
    rotateKeyframe2->SetCurve(Curves::FRICTION);
    auto rotateKeyframe3 = MakeRefPtr<Keyframe<double>>(1.0f, ROTATE_ANGLE);
    rotateKeyframe3->SetCurve(Curves::FRICTION);
    auto rotateAnimation = MakeRefPtr<KeyframeAnimation<double>>();
    rotateAnimation->AddKeyframe(rotateKeyframe1);
    rotateAnimation->AddKeyframe(rotateKeyframe2);
    rotateAnimation->AddKeyframe(rotateKeyframe3);
    rotateAnimation->AddListener([weakItemGroup = WeakClaim(this)](double value) {
        auto itemGroup = weakItemGroup.Upgrade();
        if (itemGroup) {
            itemGroup->RotateArrow(value);
        }
    });
    return rotateAnimation;
}

void RenderListItemGroup::RotateArrow(double angle)
{
    if (arrowImage_) {
        double rotateAngle = expand_ ? angle : ROTATE_ANGLE - angle;
        arrowImage_->SetRotate(rotateAngle);
        arrowImage_->MarkNeedRender();
    }
}

void RenderListItemGroup::UpdateGroupComponentStatus(bool expand)
{
    // Restore the state on component for recycle and rebuild.
    auto itemGroup = AceType::DynamicCast<ListItemGroupComponent>(itemGroup_);
    if (itemGroup) {
        itemGroup->SetExpand(expand);
    }
}

void RenderListItemGroup::ResetChildVisibleState()
{
    for (const auto& child : GetChildren()) {
        if (child == primary_ || child == arrowImage_) {
            child->SetVisible(true);
            continue;
        }
        child->SetVisible(expand_);
    }
}

RefPtr<RenderList> RenderListItemGroup::GetRenderList()
{
    auto parentNode = GetParent().Upgrade();
    while (parentNode) {
        RefPtr<RenderList> listNode = AceType::DynamicCast<RenderList>(parentNode);
        if (listNode) {
            return listNode;
        }
        parentNode = parentNode->GetParent().Upgrade();
    }
    return nullptr;
}

void RenderListItemGroup::ShowFocusAnimation()
{
    RefPtr<RenderListItem> focusItem;
    if (!expand_) {
        focusItem = RenderListItem::GetRenderListItem(primary_);
        if (focusItem) {
            focusItem->SetFocused(true);
            focusItem->ShowFocusAnimation(true, Rect(0.0, 0.0, 0.0, 0.0));
        }
    }
    for (const auto& child : GetChildren()) {
        if (child == arrowImage_) {
            continue;
        }
        focusItem = RenderListItem::GetRenderListItem(child);
        if (focusItem && focusItem->IsFocused()) {
            if (expand_) {
                focusItem->ShowFocusAnimation(true, Rect(0.0, 0.0, 0.0, 0.0));
                break;
            }
            if (child != primary_) {
                focusItem->SetFocused(false);
            }
            break;
        }
    }
}

int32_t RenderListItemGroup::GetNextFocusIndex(int32_t lastFocusIndex, bool vertical, bool reverse)
{
    KeyDirection key = DIRECTION_MAP.at(rightToLeft_).at(direction_).at(vertical).at(reverse);
    switch (key) {
        case KeyDirection::UP:
            return lastFocusIndex - 1;
        case KeyDirection::DOWN: {
            return lastFocusIndex + 1;
        }
        default:
            break;
    }
    return -1;
}

void RenderListItemGroup::ItemPrimaryChange(int32_t index)
{
    if (isDefaultPrimary_) {
        for (auto child : GetChildren()) {
            auto listItem = RenderListItem::GetRenderListItem(child);
            if (listItem && listItem->GetPrimary()) {
                listItem->SetPrimary(false);
                isDefaultPrimary_ = false;
                break;
            }
        }
    }
}

} // namespace OHOS::Ace
