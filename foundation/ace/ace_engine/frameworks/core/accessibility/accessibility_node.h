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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ACCESSIBILITY_ACCESSIBILITY_NODE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ACCESSIBILITY_ACCESSIBILITY_NODE_H

#include <functional>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "base/geometry/axis.h"
#include "base/geometry/rect.h"
#include "base/memory/ace_type.h"
#include "base/utils/macros.h"
#include "core/accessibility/accessibility_utils.h"
#include "core/event/ace_event_handler.h"

namespace OHOS::Ace {

struct PositionInfo {
    double width = 0.0;
    double height = 0.0;
    double left = 0.0;
    double top = 0.0;
};

using ActionClickImpl = std::function<void()>;
using ActionLongClickImpl = std::function<void()>;
using ActionScrollForwardImpl = std::function<bool()>;
using ActionScrollBackwardImpl = std::function<bool()>;
using ActionFocusImpl = std::function<void()>;
using ActionUpdateIdsImpl = std::function<void()>;
using FocusChangeCallback = std::function<void(const std::string&)>;
using ActionAccessibilityFocusImpl = std::function<void(bool)>;

using NodeId = int32_t;
// If no insertion location is specified, new child will be added to the end of children list by default.
constexpr int32_t DEFAULT_INDEX = -1;
constexpr uint64_t DEFAULT_ACTIONS = std::numeric_limits<uint64_t>::max();

class ACE_EXPORT AccessibilityNode : public AceType {
    DECLARE_ACE_TYPE(AccessibilityNode, AceType);

public:
    using ChartValue = std::unordered_map<std::string, std::vector<std::pair<std::string, double>>>;

    AccessibilityNode(NodeId nodeId, const std::string& nodeName);
    ~AccessibilityNode() override = default;

    // node event action
    void SetActionClickImpl(const ActionClickImpl& actionClickImpl);
    bool ActionClick();
    void SetActionLongClickImpl(const ActionLongClickImpl& actionLongClickImpl);
    bool ActionLongClick();
    void SetActionScrollForward(const ActionScrollForwardImpl& actionScrollForwardImpl);
    bool ActionScrollForward();
    void SetActionScrollBackward(const ActionScrollBackwardImpl& actionScrollBackwardImpl);
    bool ActionScrollBackward();
    void SetActionFocusImpl(const ActionFocusImpl& actionFocusImpl);
    bool ActionFocus();
    void SetActionUpdateIdsImpl(const ActionUpdateIdsImpl& actionUpdateIdsImpl);
    void ActionUpdateIds();
    void SetActionAccessibilityFocusImpl(const ActionAccessibilityFocusImpl& actionAccessibilityFocusImpl);
    bool ActionAccessibilityFocus(bool isFocus);

    // node base
    void SetAttr(const std::vector<std::pair<std::string, std::string>>& attrs);
    // used for inspector node in PC preview
    void SetStyle(const std::vector<std::pair<std::string, std::string>>& styles);
    void AddEvent(int32_t pageId, const std::vector<std::string>& events);
    void AddNode(const RefPtr<AccessibilityNode>& node, int32_t slot = DEFAULT_INDEX);
    void RemoveNode(const RefPtr<AccessibilityNode>& node);
    void Mount(int32_t slot);

    void SetIsRootNode(bool isRootNode)
    {
        isRootNode_ = isRootNode;
    }

    bool IsRootNode() const
    {
        return isRootNode_;
    }

    void ResetChildList(std::list<RefPtr<AccessibilityNode>>& children)
    {
        children_.clear();
        children_.swap(children);
    }

    const std::list<RefPtr<AccessibilityNode>>& GetChildList() const
    {
        return children_;
    }

    NodeId GetParentId() const
    {
        auto parentNode = parentNode_.Upgrade();
        return parentNode ? parentNode->GetNodeId() : -1;
    }

    RefPtr<AccessibilityNode> GetParentNode() const
    {
        return parentNode_.Upgrade();
    }

    void SetParentNode(const RefPtr<AccessibilityNode>& parentNode);

    const std::string& GetTag() const
    {
        return tag_;
    }

    void SetTag(const std::string& tag)
    {
        tag_ = tag;
    }

    int32_t GetPageId() const
    {
        return pageId_;
    }

    void SetPageId(int32_t pageId)
    {
        pageId_ = pageId;
    }

    void SetPositionInfo(const PositionInfo& positionInfo);

    const EventMarker& GetAccessibilityEventMarker() const
    {
        return onAccessibilityEventId_;
    }

    const EventMarker& GetClickEventMarker() const
    {
        return onClickId_;
    }

    const EventMarker& GetLongPressEventMarker() const
    {
        return onLongPressId_;
    }

    const EventMarker& GetFocusEventMarker() const
    {
        return onFocusId_;
    }

    void SetFocusChangeEventMarker(const EventMarker& eventId);

    void OnFocusChange(bool isFocus);

    const EventMarker& GetBlurEventMarker() const
    {
        return onBlurId_;
    }

    // node attr need to barrierfree
    NodeId GetNodeId() const
    {
        return nodeId_;
    }

    const std::string& GetText() const
    {
        return text_;
    }

    void SetText(const std::string& text)
    {
        text_ = text;
    }

    const std::string& GetHintText() const
    {
        return hintText_;
    }

    void SetHintText(const std::string& hintText)
    {
        hintText_ = hintText;
    }

    const std::vector<int32_t>& GetChildIds() const
    {
        return childIds_;
    }

    void SetChildIds(const std::vector<int32_t>& ids)
    {
        childIds_ = ids;
    }

    double GetWidth() const
    {
        return rect_.Width();
    }

    void SetWidth(double width)
    {
        rect_.SetWidth(width);
    }

    double GetHeight() const
    {
        return rect_.Height();
    }

    void SetHeight(double height)
    {
        rect_.SetHeight(height);
    }

    double GetLeft() const
    {
        return rect_.Left();
    }

    void SetLeft(double left)
    {
        return rect_.SetLeft(left);
    }

    double GetTop() const
    {
        return rect_.Top();
    }

    void SetTop(double top)
    {
        return rect_.SetTop(top);
    }

    bool GetCheckedState() const
    {
        return isChecked_;
    }

    void SetCheckedState(bool state)
    {
        isChecked_ = state;
    }

    bool GetEnabledState() const
    {
        return isEnabled_;
    }

    void SetEnabledState(bool state)
    {
        isEnabled_ = state;
    }

    bool GetEditable() const
    {
        return isEditable_;
    }

    void SetEditable(bool editable)
    {
        isEditable_ = editable;
    }

    bool GetFocusedState() const
    {
        return isFocused_;
    }

    void SetFocusedState(bool state)
    {
        isFocused_ = state;
        OnFocusChange(isFocused_);
    }

    bool GetAccessibilityFocusedState() const
    {
        return isAccessibilityFocused_;
    }

    void SetAccessibilityFocusedState(bool state)
    {
        isAccessibilityFocused_ = state;
    }

    bool GetSelectedState() const
    {
        return isSelected_;
    }

    void SetSelectedState(bool state)
    {
        isSelected_ = state;
    }

    bool GetCheckableState() const
    {
        return isCheckable_;
    }

    void SetCheckableState(bool state)
    {
        isCheckable_ = state;
    }

    bool GetClickableState() const
    {
        return isClickable_;
    }

    void SetClickableState(bool state)
    {
        isClickable_ = state;
        SetSupportAction(AceAction::ACTION_CLICK, state);
    }

    bool GetFocusableState() const
    {
        return isFocusable_;
    }

    void SetFocusableState(bool state)
    {
        isFocusable_ = state;
    }

    bool GetScrollableState() const
    {
        return isScrollable_;
    }

    void SetScrollableState(bool state)
    {
        isScrollable_ = state;
    }

    bool GetLongClickableState() const
    {
        return isLongClickable_;
    }

    void SetLongClickableState(bool state)
    {
        isLongClickable_ = state;
        SetSupportAction(AceAction::ACTION_LONG_CLICK, state);
    }

    bool GetIsMultiLine() const
    {
        return isMultiLine_;
    }

    void SetIsMultiLine(bool multiLine)
    {
        isMultiLine_ = multiLine;
    }

    bool GetIsPassword() const
    {
        return isPassword_;
    }

    void SetIsPassword(bool isPassword)
    {
        isPassword_ = isPassword;
    }

    std::unordered_set<AceAction> GetSupportAction(uint64_t enableActions = DEFAULT_ACTIONS) const;

    void AddSupportAction(AceAction action)
    {
        supportActions_ |= (1UL << static_cast<uint32_t>(action));
    }

    void SetSupportAction(AceAction action, bool isEnable)
    {
        isEnable ? supportActions_ |= (1UL << static_cast<uint32_t>(action))
                 : supportActions_ &= (~(0UL)) ^ (1UL << static_cast<uint32_t>(action));
    }

    const std::string& GetAccessibilityLabel() const
    {
        return accessibilityLabel_;
    }

    void SetAccessibilityLabel(const std::string& label)
    {
        accessibilityLabel_ = label;
    }

    const std::string& GetAccessibilityHint() const
    {
        return accessibilityHint_;
    }

    void SetAccessibilityHint(const std::string& hint)
    {
        accessibilityHint_ = hint;
    }

    const std::string& GetImportantForAccessibility() const
    {
        return importantForAccessibility_;
    }

    void SetImportantForAccessibility(const std::string& importance)
    {
        importantForAccessibility_ = importance;
    }

    size_t GetMaxTextLength() const
    {
        return maxTextLength_;
    }

    void SetMaxTextLength(size_t length)
    {
        maxTextLength_ = length;
    }

    int32_t GetTextSelectionStart() const
    {
        return textSelectionStart_;
    }

    void SetTextSelectionStart(int32_t start)
    {
        textSelectionStart_ = start;
    }

    int32_t GetTextSelectionEnd() const
    {
        return textSelectionEnd_;
    }

    void SetTextSelectionEnd(int32_t end)
    {
        textSelectionEnd_ = end;
    }

    const std::string& GetErrorText() const
    {
        return errorText_;
    }

    void SetErrorText(const std::string& errorText)
    {
        errorText_ = errorText;
    }

    const std::string& GetJsComponentId() const
    {
        return jsComponentId_;
    }

    void SetJsComponentId(const std::string& jsComponentId)
    {
        jsComponentId_ = jsComponentId;
    }

    bool GetAccessible() const
    {
        return accessible_;
    }

    void SetAccessible(bool accessible)
    {
        accessible_ = accessible;
    }

    AccessibilityValue GetAccessibilityValue() const
    {
        return accessibilityValue_;
    }

    void SetAccessibilityValue(double cur, double min = 0.0, double max = 0.0)
    {
        accessibilityValue_.current = cur;
        accessibilityValue_.min = min;
        accessibilityValue_.max = max;
    }

    const std::unique_ptr<ChartValue>& GetChartValue() const
    {
        return chartValue_;
    }

    void PutChartValue(const std::string& groupName, const std::vector<std::pair<std::string, double>>& values)
    {
        if (!chartValue_) {
            chartValue_ = std::make_unique<ChartValue>();
        }

        auto result = chartValue_->try_emplace(groupName, values);
        if (!result.second) {
            result.first->second = values;
        }
    }

    std::string GetInputType() const
    {
        return inputType_;
    }

    AceTextCategory GetTextInputType() const
    {
        return textInputType_;
    }

    void SetTextInputType(AceTextCategory type)
    {
        textInputType_ = type;
    }

    const AceCollectionInfo& GetCollectionInfo() const
    {
        return collectionInfo_;
    }

    void SetCollectionInfo(const AceCollectionInfo& collectionInfo)
    {
        collectionInfo_ = collectionInfo;
    }

    const AceCollectionItemInfo& GetCollectionItemInfo() const
    {
        return collectionItemInfo_;
    }

    void SetCollectionItemInfo(const AceCollectionItemInfo& collectionItemInfo)
    {
        collectionItemInfo_ = collectionItemInfo;
    }

    bool GetShown() const
    {
        return shown_;
    }

    bool GetVisible() const
    {
        return visible_;
    }

    void SetVisible(bool visible)
    {
        visible_ = visible;
    }

    const Rect& GetRect() const
    {
        return rect_;
    }

    void SetRect(const Rect& rect)
    {
        isValidRect_ = rect.IsValid();
        if (isValidRect_) {
            rect_ = rect;
        }
    }

    const Rect& GetGlobalRect()
    {
        return globalRect_;
    }

    void SetGlobalRect(const Rect& rect)
    {
        globalRect_ = rect;
    }

    void ClearRect()
    {
        rect_ = Rect(0, 0, 0, 0);
    }

    bool IsValidRect() const
    {
        return isValidRect_;
    }

    bool GetClicked() const
    {
        return isClicked_;
    }

    void SetClicked(bool clicked)
    {
        isClicked_ = clicked;
    }

    void SetMarginSize(const Size& marginSize)
    {
        marginSize_ = marginSize;
    }

    Size GetMarginSize() const
    {
        return marginSize_;
    }

    const std::vector<std::pair<std::string, std::string>>& GetAttrs() const
    {
        return attrs_;
    }

    const std::vector<std::pair<std::string, std::string>>& GetStyles() const
    {
        return styles_;
    }

    void SetClipFlagToChild(bool clipFlag)
    {
        for (auto& child : children_) {
            child->SetClipFlagToChild(clipFlag);
        }
        clipFlag_ = clipFlag;
    }

    bool GetClipFlag()
    {
        return clipFlag_;
    }

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    // used for inspector node in PC preview
    bool GetClearRectInfoFlag() const
    {
        return isClearRectInfo_;
    }

    // used for inspector node in PC preview
    void SetClearRectInfoFlag(bool isClearRectInfo)
    {
        isClearRectInfo_ = isClearRectInfo;
    }

    // used for inspector node in PC preview
    void SetScaleToChild(double scale)
    {
        for (auto& child : children_) {
            child->SetScaleToChild(scale);
        }
        SetScale(scale);
    }

    // used for inspector node in PC preview
    void SetScaleCenterToChild(Offset center)
    {
        for (auto& child : children_) {
            child->SetScaleCenterToChild(center);
        }
        SetScaleCenter(center);
    }

    // used for inspector node in PC preview
    double GetScale()
    {
        return scale_;
    }

    // used for inspector node in PC preview
    void SetScale(double scale)
    {
        scale_ = scale;
        SetIsAnimationNode(true);
    }

    // used for inspector node in PC preview
    void SetScaleCenter(Offset center)
    {
        scaleCenter_ = center;
    }

    // used for inspector node in PC preview
    Offset GetScaleCenter()
    {
        return scaleCenter_;
    }

    // used for inspector node in PC preview
    void SetTranslateOffsetToChild(const Offset& offset)
    {
        for (auto& child : children_) {
            child->SetTranslateOffsetToChild(offset);
        }
        SetTranslateOffset(offset);
    }

    void SetTranslateOffset(const Offset& offset)
    {
        translateOffset_ = offset;
        SetIsAnimationNode(true);
    }

    Offset GetTranslateOffset() const
    {
        return translateOffset_;
    }

    // used for inspector node in PC preview
    void SetRotateToChild(const double& angle, const RotateAxis& Axis)
    {
        for (auto& child : children_) {
            child->SetRotateToChild(angle, Axis);
        }
        SetRotateAngle(angle);
        SetRotateAxis(Axis);
    }

    void SetRotateAngle(const double& angle)
    {
        rotateAngle_ = angle;
        SetIsAnimationNode(true);
    }

    double GetRotateAngle() const
    {
        return rotateAngle_;
    }

    void SetRotateAxis(const RotateAxis& Axis)
    {
        rotateAxis_ = Axis;
    }

    RotateAxis GetRotateAxis(RotateAxis Axis) const
    {
        return rotateAxis_;
    }

    bool IsAnimationNode() const
    {
        return isAnimationNode_;
    }

    void SetIsAnimationNode(bool IsAnimationNode)
    {
        isAnimationNode_ = IsAnimationNode;
    }

    int32_t GetZIndex()
    {
        return zIndex_;
    }

    void SetZIndex(int32_t index)
    {
        zIndex_ = index;
    }

    // only panel has ZIndex,others components is default value 0
    void SetZIndexToChild(int32_t index)
    {
        for (auto& child : children_) {
            child->SetZIndexToChild(index);
        }
        SetZIndex(index);
    }

    void UpdateRectWithChildRect()
    {
        if (children_.empty()) {
            return;
        }
        SetRect(children_.front()->GetRect());
    }
#endif

protected:
    // inner use, don't need to barrierfree
    NodeId nodeId_ = -1;
    int32_t pageId_ = -1;
    bool isRootNode_ = false;
    std::string inputType_;
    WeakPtr<AccessibilityNode> parentNode_;
    std::list<RefPtr<AccessibilityNode>> children_;
    ActionClickImpl actionClickImpl_;
    ActionLongClickImpl actionLongClickImpl_;
    ActionScrollForwardImpl actionScrollForwardImpl_;
    ActionScrollBackwardImpl actionScrollBackwardImpl_;
    ActionFocusImpl actionFocusImpl_;
    ActionUpdateIdsImpl actionUpdateIdsImpl_;
    ActionAccessibilityFocusImpl actionAccessibilityFocusIdsImpl_;
    EventMarker onAccessibilityEventId_;
    EventMarker onClickId_;
    EventMarker onLongPressId_;
    EventMarker onFocusId_;
    EventMarker onBlurId_;
    FocusChangeCallback focusChangeEventId_;

private:
    void SetOperableInfo();

    // node attr need to barrierfree
    size_t maxTextLength_ = 0;
    int32_t textSelectionStart_ = 0;
    int32_t textSelectionEnd_ = 0;
    std::string tag_;
    std::string text_;
    std::string hintText_;
    std::string errorText_;
    std::string jsComponentId_;
    std::string accessibilityLabel_;
    std::string accessibilityHint_;
    std::string importantForAccessibility_;
    AceTextCategory textInputType_ { AceTextCategory::INPUT_TYPE_DEFAULT };
    std::vector<int32_t> childIds_;
    uint64_t supportActions_ = 0;
    std::unique_ptr<ChartValue> chartValue_;

    Rect globalRect_;
    Rect rect_;
    Size marginSize_;
    union {
        struct {
            bool isValidRect_ : 1;
            bool isChecked_ : 1;
            bool isEditable_ : 1;
            bool isEnabled_ : 1;
            bool accessible_ : 1;
            bool isFocused_ : 1;
            bool isSelected_ : 1;
            bool isCheckable_ : 1;
            bool isClickable_ : 1;
            bool isFocusable_ : 1;
            bool isScrollable_ : 1;
            bool isLongClickable_ : 1;
            bool isMultiLine_ : 1;
            bool isPassword_ : 1;
            bool visible_ : 1;
            bool shown_ : 1;
            bool isClicked_ : 1;
            bool isAccessibilityFocused_ : 1;
        };
        uint64_t bits_ = 0;
    };
    AccessibilityValue accessibilityValue_;
    AceCollectionInfo collectionInfo_;
    AceCollectionItemInfo collectionItemInfo_;

    std::vector<std::pair<std::string, std::string>> attrs_;
    std::vector<std::pair<std::string, std::string>> styles_;
    bool clipFlag_ = false;
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    // used for inspector node in PC preview
    bool isClearRectInfo_ = false;
    // focus scale or translateScale for inspector node in PC preview
    double scale_ = 1.0;
    Offset scaleCenter_ { 0.0, 0.0 };
    Offset translateOffset_ { 0.0, 0.0 };
    double rotateAngle_ = 0.0;
    RotateAxis rotateAxis_ = RotateAxis::AXIS_Z;
    bool isAnimationNode_ = false;
    int32_t zIndex_ = 0;
#endif
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ACCESSIBILITY_ACCESSIBILITY_NODE_H
