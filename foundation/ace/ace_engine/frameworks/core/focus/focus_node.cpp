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

#include "core/focus/focus_node.h"

#include <algorithm>
#include <atomic>
#include <cinttypes>

#include "base/log/dump_log.h"
#include "base/log/log.h"
#include "core/common/ace_application_info.h"
#include "core/components/flex/flex_element.h"
#include "core/event/ace_event_handler.h"
#include "core/gestures/click_recognizer.h"
#include "core/pipeline/base/composed_element.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {
namespace {

inline RefPtr<RenderNode> GetRenderNode(const RefPtr<FocusNode>& node)
{
    auto element = AceType::DynamicCast<RenderElement>(node);
    if (!element) {
        auto composedElement = AceType::DynamicCast<ComposedElement>(node);
        if (composedElement) {
            auto child = composedElement->GetChildren().front();
            return child ? child->GetRenderNode() : nullptr;
        }
        return nullptr;
    }
    return element->GetRenderNode();
}

} // namespace

int32_t FocusNode::GenerateFocusIndex()
{
    static std::atomic<int32_t> counter { 1 };
    return counter.fetch_add(1, std::memory_order_relaxed);
}

bool FocusNode::HandleKeyEvent(const KeyEvent& keyEvent)
{
    if (!IsCurrentFocus()) {
        return false;
    }
    if (OnKeyEvent(keyEvent)) {
        return true;
    }

    auto element = AceType::DynamicCast<Element>(this);
    if (!element) {
        return false;
    }
    auto context = element->GetContext().Upgrade();
    if (!context) {
        return false;
    }

    switch (keyEvent.code) {
        case KeyCode::KEY_ENTER:
        case KeyCode::KEY_NUMPAD_ENTER:
        case KeyCode::KEY_DPAD_CENTER:
            if (keyEvent.action != KeyAction::DOWN) {
                return false;
            }
            if (context->GetIsDeclarative()) {
                OnClick(keyEvent);
            } else {
                OnClick();
            }
            return true;

        default:
            return false;
    }
}

void FocusNode::DumpFocus() {}

void FocusNode::DumpFocusTree(int32_t depth)
{
    if (DumpLog::GetInstance().GetDumpFile()) {
        DumpFocus();
        std::string information = AceType::TypeName(this);
        if (IsCurrentFocus()) {
            information += "(Node*)";
        } else {
            information += "(Node)";
        }

        if (!IsFocusable()) {
            information = "(-)" + information;
        }
        DumpLog::GetInstance().Print(depth, information, 0);
    }
}

bool FocusNode::RequestFocusImmediately()
{
    auto renderNode = GetRenderNode(AceType::Claim(this));
    if (renderNode) {
        auto context = renderNode->GetContext().Upgrade();
        if (context && context->IsJsCard()) {
            return false;
        }
    }
    if (IsCurrentFocus()) {
        return true;
    }

    if (!IsFocusable()) {
        return false;
    }

    currentFocus_ = true;
    UpdateAccessibilityFocusInfo();
    auto parent = GetParent().Upgrade();
    if (parent) {
        parent->SwitchFocus(AceType::Claim(this));
    }

    HandleFocus();
    return true;
}

void FocusNode::UpdateAccessibilityFocusInfo()
{
    auto renderNode = GetRenderNode(AceType::Claim(this));
    if (!renderNode) {
        LOGW("FocusNode renderNode is null.");
        return;
    }
    auto accessibilityNode = renderNode->GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }
    accessibilityNode->SetFocusedState(currentFocus_);
}

void FocusNode::LostFocus()
{
    if (IsCurrentFocus()) {
        currentFocus_ = false;
        UpdateAccessibilityFocusInfo();
        OnBlur();
    }
}

void FocusNode::LostSelfFocus()
{
    if (IsCurrentFocus()) {
        SetFocusable(false);
        SetFocusable(true);
    }
}

void FocusNode::RemoveSelf()
{
    auto parent = parent_.Upgrade();
    if (parent) {
        parent->RemoveChild(AceType::Claim(this));
    }
}

void FocusNode::SetFocusable(bool focusable)
{
    if (focusable_ == focusable) {
        return;
    }
    focusable_ = focusable;
    RefreshParentFocusable(FocusNode::IsFocusable());
    RefreshFocus();
}

void FocusNode::SetShow(bool show)
{
    show_ = show;
    if (!show) {
        RefreshFocus();
    }
}

void FocusNode::RefreshFocus()
{
    if (!IsCurrentFocus()) {
        return;
    }

    // lost current focus and request another focus
    auto parent = GetParent().Upgrade();

    // current node is root node
    if (!parent) {
        LostFocus();
        return;
    }
    while (!parent->IsFocusable()) {
        // parent node is root node
        if (!parent->GetParent().Upgrade()) {
            parent->LostFocus();
            return;
        }
        parent = parent->GetParent().Upgrade();
    }
    parent->LostFocus();
    parent->RequestFocusImmediately();
}

bool FocusNode::OnKeyEvent(const KeyEvent& keyEvent)
{
    auto element = AceType::DynamicCast<Element>(this);
    if (!element) {
        return false;
    }
    auto context = element->GetContext().Upgrade();
    if (!context) {
        return false;
    }
    if (context->GetIsDeclarative()) {
        auto info = std::make_shared<KeyEventInfo>(keyEvent);
        if (!onKeyEventCallback_) {
            return false;
        }
        LOGI("FocusNode::OnKeyEvent: Do key callback on %{public}s with key event{ Code(%{public}d), "
             "Action(%{public}d), "
             "SourceType(%{public}d), DeviceId(%{public}" PRId64 "), Time(%{public}lld) }. Return: %{public}d",
            AceType::TypeName(this), info->GetKeyCode(), info->GetKeyType(), info->GetSourceDevice(),
            info->GetDeviceId(), info->GetTimeStamp().time_since_epoch().count(), info->IsStopPropagation());
        onKeyEventCallback_(info);
        return info->IsStopPropagation();
    } else {
        if (onKeyCallback_) {
            return onKeyCallback_(keyEvent);
        }
    }
    return false;
}

void FocusNode::RefreshParentFocusable(bool focusable)
{
    // do nothing
}

void FocusNode::RequestFocus()
{
    if (IsCurrentFocus()) {
        return;
    }

    auto element = AceType::DynamicCast<Element>(this);
    if (!element) {
        return;
    }
    auto context = element->GetContext().Upgrade();
    if (context) {
        context->AddDirtyFocus(AceType::Claim(this));
    } else {
        LOGE("fail to add dirty focus due to context is null");
    }
}

void FocusNode::OnClick(const KeyEvent& event)
{
    if (onClickEventCallback_) {
        auto info = std::make_shared<ClickInfo>(-1);
        info->SetTimeStamp(event.timeStamp);
        info->SetGlobalLocation(
            Offset((GetRect().Left() + GetRect().Right()) / 2, (GetRect().Top() + GetRect().Bottom()) / 2));
        info->SetLocalLocation(
            Offset((GetRect().Right() - GetRect().Left()) / 2, (GetRect().Bottom() - GetRect().Top()) / 2));
        info->SetSourceDevice(static_cast<SourceType>(event.sourceType));
        info->SetDeviceId(event.deviceId);
        LOGI("FocusNode::OnClick: Do click callback on %{public}s with key event{ Global(%{public}f,%{public}f), "
             "Local(%{public}f,%{public}f), SourceType(%{public}d), DeviceId(%{public}" PRId64
             "), Time(%{public}lld) }",
            AceType::TypeName(this), info->GetGlobalLocation().GetX(), info->GetGlobalLocation().GetY(),
            info->GetLocalLocation().GetX(), info->GetLocalLocation().GetY(), info->GetSourceDevice(),
            info->GetDeviceId(), info->GetTimeStamp().time_since_epoch().count());
        onClickEventCallback_(info);
    }
}

void FocusGroup::AddChild(const RefPtr<FocusNode>& focusNode)
{
    // Already belong to any focus scope.
    if (!focusNode || !focusNode->GetParent().Invalid()) {
        return;
    }

    auto it = std::find(focusNodes_.begin(), focusNodes_.end(), focusNode);
    if (it == focusNodes_.end()) {
        focusNodes_.emplace_back(focusNode);
        focusNode->SetParent(AceType::WeakClaim(this));
    }
}

void FocusGroup::AddChild(const RefPtr<FocusNode>& focusNode, const RefPtr<FocusNode>& nextFocusNode)
{
    // Already belong to any focus scope.
    if (!focusNode || !focusNode->GetParent().Invalid()) {
        return;
    }

    auto it = std::find(focusNodes_.begin(), focusNodes_.end(), focusNode);
    auto pos = std::find(focusNodes_.begin(), focusNodes_.end(), nextFocusNode);
    if (it == focusNodes_.end()) {
        focusNodes_.insert(pos, focusNode);
        focusNode->SetParent(AceType::WeakClaim(this));
    }
}

void FocusGroup::DumpFocusTree(int32_t depth)
{
    if (DumpLog::GetInstance().GetDumpFile()) {
        DumpFocus();
        std::string information = AceType::TypeName(this);
        if (IsCurrentFocus()) {
            information += "(Scope*)";
        } else {
            information += "(Scope)";
        }

        if (!IsFocusable()) {
            information = "(-)" + information;
        }
        DumpLog::GetInstance().Print(depth, information, focusNodes_.size());
    }

    for (const auto& item : focusNodes_) {
        item->DumpFocusTree(depth + 1);
    }
}

void FocusGroup::RemoveChild(const RefPtr<FocusNode>& focusNode)
{
    // Not belong to this focus scope.
    if (!focusNode || focusNode->GetParent() != this) {
        return;
    }

    if (focusNode->IsCurrentFocus()) {
        // Try to goto next focus, otherwise goto previous focus.
        if (!GoToNextFocus(true) && !GoToNextFocus(false)) {
            itLastFocusNode_ = focusNodes_.end();
        }
        focusNode->LostFocus();
    } else {
        if (itLastFocusNode_ != focusNodes_.end() && (*itLastFocusNode_) == focusNode) {
            itLastFocusNode_ = focusNodes_.end();
        }
    }

    auto it = std::find(focusNodes_.begin(), focusNodes_.end(), focusNode);
    if (it == focusNodes_.end()) {
        return;
    }
    focusNodes_.erase(it);
    focusNode->SetParent(nullptr);
}

void FocusGroup::SwitchFocus(const RefPtr<FocusNode>& focusNode)
{
    auto it = std::find(focusNodes_.begin(), focusNodes_.end(), focusNode);
    ACE_DCHECK(it != focusNodes_.end());

    auto itFocusNode = itLastFocusNode_;
    itLastFocusNode_ = it;

    if (IsCurrentFocus()) {
        if (itFocusNode != focusNodes_.end() && itFocusNode != it) {
            (*itFocusNode)->LostFocus();
        }
    } else {
        RequestFocusImmediately();
    }
}

bool FocusGroup::IsFocusable() const
{
    if (!FocusNode::IsFocusable()) {
        return false;
    }
    if (focusNodes_.size() > 0) {
        return std::any_of(focusNodes_.begin(), focusNodes_.end(),
            [](const RefPtr<FocusNode>& focusNode) { return focusNode->IsFocusable(); });
    } else {
        return true;
    }
}

bool FocusGroup::GoToNextFocus(bool reverse, const Rect& rect)
{
    if (focusNodes_.empty()) {
        return false;
    }
    auto itNewFocusNode = itLastFocusNode_;
    if (itNewFocusNode == focusNodes_.end()) {
        itNewFocusNode = focusNodes_.begin();
    }
    if (reverse) {
        if (itNewFocusNode == focusNodes_.begin()) {
            itNewFocusNode = focusNodes_.end();
            return false;
        } else {
            --itNewFocusNode;
        }
        while (itNewFocusNode != focusNodes_.begin()) {
            if (TryRequestFocus(*itNewFocusNode, rect)) {
                return true;
            }
            --itNewFocusNode;
        }
        if (itNewFocusNode == focusNodes_.begin()) {
            if (TryRequestFocus(*itNewFocusNode, rect)) {
                return true;
            }
        }
    } else {
        if (itNewFocusNode != focusNodes_.end()) {
            ++itNewFocusNode;
        }
        while (itNewFocusNode != focusNodes_.end()) {
            if (TryRequestFocus(*itNewFocusNode, rect)) {
                return true;
            }
            ++itNewFocusNode;
        }
    }

    return false;
}

bool FocusGroup::OnKeyEvent(const KeyEvent& keyEvent)
{
    ACE_DCHECK(IsCurrentFocus());
    if (itLastFocusNode_ != focusNodes_.end() && (*itLastFocusNode_)->HandleKeyEvent(keyEvent)) {
        return true;
    }

    if (FocusNode::OnKeyEvent(keyEvent)) {
        return true;
    }

    if (keyEvent.action != KeyAction::DOWN) {
        return false;
    }

    if (!CalculatePosition()) {
        return false;
    }

    LOGD("Position information: Node is %{public}s, X: %{public}lf Y: %{public}lf W: %{public}lf H: %{public}lf",
        AceType::TypeName(this), GetRect().Left(), GetRect().Top(), GetRect().Width(), GetRect().Height());

    OnFocusMove(keyEvent.code);
    switch (keyEvent.code) {
        case KeyCode::TV_CONTROL_UP:
            LOGI("FocusGroup::OnKeyEvent: RequestNextFocus 'UP' by KeyCode(%{public}d)", keyEvent.code);
            return RequestNextFocus(true, true, GetRect());
        case KeyCode::TV_CONTROL_DOWN:
            LOGI("FocusGroup::OnKeyEvent: RequestNextFocus 'DOWN' by KeyCode(%{public}d)", keyEvent.code);
            return RequestNextFocus(true, false, GetRect());
        case KeyCode::TV_CONTROL_LEFT:
            LOGI("FocusGroup::OnKeyEvent: RequestNextFocus 'LEFT' by KeyCode(%{public}d)", keyEvent.code);
            return RequestNextFocus(false, !AceApplicationInfo::GetInstance().IsRightToLeft(), GetRect());
        case KeyCode::TV_CONTROL_RIGHT:
            LOGI("FocusGroup::OnKeyEvent: RequestNextFocus 'RIGHT' by KeyCode(%{public}d)", keyEvent.code);
            return RequestNextFocus(false, AceApplicationInfo::GetInstance().IsRightToLeft(), GetRect());
        case KeyCode::KEY_TAB:
            if (keyEvent.pressedCodes.size() == 1) {
                LOGI("FocusGroup::OnKeyEvent: RequestNextFocus 'TAB' by KeyCode(%{public}d)", keyEvent.code);
                return RequestNextFocus(false, false, GetRect()) || RequestNextFocus(true, false, GetRect());
            } else {
                LOGI("FocusGroup::OnKeyEvent: RequestNextFocus 'SHIFT-TAB' by KeyCode(%{public}d)", keyEvent.code);
                if (keyEvent.IsKey({ KeyCode::KEY_SHIFT_LEFT, KeyCode::KEY_TAB }) ||
                    keyEvent.IsKey({ KeyCode::KEY_SHIFT_RIGHT, KeyCode::KEY_TAB })) {
                    return RequestNextFocus(false, true, GetRect()) || RequestNextFocus(true, true, GetRect());
                }
            }
        default:
            return false;
    }
}

bool FocusGroup::CalculatePosition()
{
    if (itLastFocusNode_ == focusNodes_.end()) {
        return false;
    }

    Rect childRect;
    if (!CalculateRect(*itLastFocusNode_, childRect)) {
        return false;
    }

    if ((*itLastFocusNode_)->IsChild()) {
        auto renderNode = GetRenderNode(*itLastFocusNode_);
        if (!renderNode) {
            return false;
        }

        Rect rect(childRect.GetOffset(), renderNode->GetLayoutSize());
        (*itLastFocusNode_)->SetRect(rect);
        SetRect(rect);
    } else {
        SetRect((*itLastFocusNode_)->GetRect() + childRect.GetOffset());
    }

    return true;
}

void FocusGroup::OnFocus()
{
    if (focusNodes_.empty()) {
        return;
    }

    auto itFocusNode = itLastFocusNode_;
    do {
        if (itLastFocusNode_ == focusNodes_.end()) {
            itLastFocusNode_ = focusNodes_.begin();
            if (itLastFocusNode_ == itFocusNode) {
                break;
            }
        }
        if ((*itLastFocusNode_)->RequestFocusImmediately()) {
            FocusNode::OnFocus();
            return;
        }
    } while ((++itLastFocusNode_) != itFocusNode);

    // Not found any focusable node, clear focus.
    itLastFocusNode_ = focusNodes_.end();
}

void FocusGroup::OnBlur()
{
    FocusNode::OnBlur();

    if (itLastFocusNode_ != focusNodes_.end() && *itLastFocusNode_) {
        (*itLastFocusNode_)->LostFocus();
    }
}

void FocusGroup::SetShow(bool show)
{
    FocusNode::SetShow(show);
    RefreshParentFocusable(FocusNode::IsFocusable());
}

bool FocusGroup::TryRequestFocus(const RefPtr<FocusNode>& focusNode, const Rect& rect)
{
    if (rect.IsValid()) {
        Rect childRect;
        if (!CalculateRect(focusNode, childRect) ||
            !focusNode->AcceptFocusByRectOfLastFocus(rect - childRect.GetOffset())) {
            return false;
        }
    }
    return focusNode->RequestFocusImmediately();
}

bool FocusGroup::AcceptFocusByRectOfLastFocus(const Rect& rect)
{
    if (focusNodes_.empty()) {
        return false;
    }

    auto itFocusNode = itLastFocusNode_;
    do {
        if (itLastFocusNode_ == focusNodes_.end()) {
            itLastFocusNode_ = focusNodes_.begin();
            if (itLastFocusNode_ == itFocusNode) {
                break;
            }
        }
        Rect childRect;
        if (!CalculateRect(*itLastFocusNode_, childRect)) {
            continue;
        }

        if ((*itLastFocusNode_)->AcceptFocusByRectOfLastFocus(rect - childRect.GetOffset())) {
            return true;
        }
    } while ((++itLastFocusNode_) != itFocusNode);

    return false;
}

bool FocusGroup::CalculateRect(const RefPtr<FocusNode>& node, Rect& rect)
{
    auto renderNode = GetRenderNode(AceType::Claim(this));
    if (!renderNode) {
        return false;
    }
    Offset nowOffset = renderNode->GetOffsetFromOrigin(Offset());

    renderNode = GetRenderNode(node);
    if (!renderNode) {
        return false;
    }
    Offset childOffset = renderNode->GetOffsetFromOrigin(Offset());
    rect.SetRect(childOffset - nowOffset, renderNode->GetLayoutSize());
    return true;
}

void FocusGroup::RefreshParentFocusable(bool focusable)
{
    for (auto& item : focusNodes_) {
        if (focusable != item->IsParentFocusable()) {
            item->SetParentFocusable(focusable);
            item->RefreshParentFocusable(item->FocusNode::IsFocusable());
        }
    }
}

void FocusGroup::RebuildChild(std::list<RefPtr<FocusNode>>&& rebuildFocusNodes)
{
    if (rebuildFocusNodes.empty()) {
        return;
    }

    focusNodes_ = std::move(rebuildFocusNodes);
    itLastFocusNode_ = focusNodes_.end();
    if (!IsCurrentFocus()) {
        return;
    }

    auto it = focusNodes_.begin();
    while (it != focusNodes_.end()) {
        if ((*it)->IsCurrentFocus()) {
            itLastFocusNode_ = it;
            return;
        }
        ++it;
    }

    LostFocus();
    itLastFocusNode_ = focusNodes_.end();
    RequestFocusImmediately();
}

} // namespace OHOS::Ace
