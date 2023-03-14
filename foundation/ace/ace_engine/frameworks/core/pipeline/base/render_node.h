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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_RENDER_NODE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_RENDER_NODE_H

#include <list>

#include "base/geometry/dimension.h"
#include "base/geometry/rect.h"
#include "base/memory/ace_type.h"
#include "base/utils/macros.h"
#include "base/utils/system_properties.h"
#include "core/accessibility/accessibility_manager.h"
#include "core/animation/animatable_properties.h"
#include "core/animation/keyframe_animation.h"
#include "core/animation/property_animatable.h"
#include "core/common/draw_delegate.h"
#include "core/components/common/layout/align_declaration.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/layout/layout_param.h"
#include "core/components/common/properties/motion_path_option.h"
#include "core/components/common/properties/state_attributes.h"
#include "core/components/common/properties/text_style.h"
#include "core/components_v2/extensions/events/event_extensions.h"
#include "core/components_v2/inspector/inspector_node.h"
#include "core/event/axis_event.h"
#include "core/event/touch_event.h"
#include "core/gestures/drag_recognizer.h"
#include "core/pipeline/base/render_context.h"
#include "core/pipeline/base/render_layer.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

extern const Dimension FOCUS_BOUNDARY;

class Component;

// If no insertion location is specified, new child will be added to the end of children list by default.
constexpr int32_t DEFAULT_RENDER_NODE_SLOT = -1;
constexpr int32_t PRESS_DURATION = 100;
constexpr int32_t HOVER_DURATION = 250;
constexpr uint32_t FIND_MAX_COUNT = 64;

using HoverAndPressCallback = std::function<void(const Color&)>;
using Rosen::RSNode;

// RenderNode is the base class for different render backend, represent a render unit for render pipeline.
class ACE_EXPORT RenderNode : public PropertyAnimatable, public AnimatableProperties, public virtual AceType {
    DECLARE_ACE_TYPE(RenderNode, PropertyAnimatable, AceType);

public:
    using OpacityCallback = std::function<void(uint8_t)>;
    using SlipFactorSetting = std::function<void(double)>;
    ~RenderNode() override = default;

    static void MarkTreeRender(const RefPtr<RenderNode>& root, bool& meetHole, bool needFlush);

    static void MarkWholeRender(const WeakPtr<RenderNode>& nodeWeak, bool needFlush);

    void SetZIndex(int32_t zIndex)
    {
        zIndex_ = zIndex;
    }

    int32_t GetZIndex() const
    {
        return zIndex_;
    }

    void SetIsPercentSize(bool isPercentSize)
    {
        isPercentSize_ = isPercentSize;
    }

    bool GetIsPercentSize() const
    {
        return isPercentSize_;
    }

    void AddChild(const RefPtr<RenderNode>& child, int32_t slot = DEFAULT_RENDER_NODE_SLOT);

    void RemoveChild(const RefPtr<RenderNode>& child);

    void MovePosition(int32_t slot);

    void ClearChildren();

    virtual void MoveWhenOutOfViewPort(bool hasEffect);

    bool IsPointInBox(const TouchEvent& point);
    void Attach(const WeakPtr<PipelineContext>& context);

    // unmount from render tree
    void Unmount()
    {
        RefPtr<RenderNode> parent = parent_.Upgrade();
        if (parent) {
            parent->MarkNeedLayout();
            parent->RemoveChild(AceType::Claim(this));
        }
    }

    // Update node with attr, style, event, method and so on.
    // This method will call Update virtual function.
    void UpdateAll(const RefPtr<Component>& component);

    virtual void AddToScene() {}

    virtual void Update(const RefPtr<Component>& component) = 0;

    // Called when page context attached, subclass can initialize object which needs page context.
    virtual void OnAttachContext() {}

    virtual void FinishRender(const std::unique_ptr<DrawDelegate>& delegate, const Rect& dirty) {}

    virtual void UpdateTouchRect();

    void SetTouchRectList(std::vector<Rect>& touchRectList);
    bool CompareTouchRect(const Rect& parentTouchRect, const Rect& childTouchRect);
    void CompareTouchRectList(std::vector<Rect>& touchRectList,
        const std::vector<Rect>& childTouchRectList, const std::vector<Rect>& parentTouchRectList);

    bool NeedLayout() const
    {
        return needLayout_;
    }

    void SetNeedLayout(bool needLayout)
    {
        needLayout_ = needLayout;
    }

    void MarkNeedLayout(bool selfOnly = false, bool forceParent = false);

    /**
     * \brief check in mark need layout progress if parent need layout again.
     * \return true if need layout agagin.
     */
    virtual bool CheckIfNeedLayoutAgain()
    {
        return true;
    }

    void MarkNeedPredictLayout();

    void OnLayout();

    // deadline : The remaining time until the next vsync. (unit: microsecond)
    virtual void OnPredictLayout(int64_t deadline) {}

    virtual Size GetChildViewPort()
    {
        return viewPort_;
    }

    // Called by parent to perform layout.
    void Layout(const LayoutParam& layoutParam)
    {
        auto pipeline = context_.Upgrade();
        if (!pipeline) {
            LOGE("pipeline is null when layout");
            return;
        }

        bool dipScaleChange = !NearEqual(pipeline->GetDipScale(), dipScale_);
        dipScale_ = pipeline->GetDipScale();
        if (dipScaleChange || layoutParam_ != layoutParam) {
            layoutParam_ = layoutParam;
            layoutParamChanged_ = true;
            SetNeedLayout(true);
        }

        if (onChangeCallback_) {
            onChangeCallback_();
        }
        OnLayout();
    }

    // Called by parent to update layout param without PerformLayout.
    void SetLayoutParam(const LayoutParam& layoutParam)
    {
        if (layoutParam_ != layoutParam) {
            layoutParam_ = layoutParam;
            layoutParamChanged_ = true;
            MarkNeedLayout();
        }
    }

    const LayoutParam& GetLayoutParam() const
    {
        return layoutParam_;
    }

    // Each subclass should override this function for actual layout operation.
    virtual void PerformLayout() = 0;

    Offset GetPosition() const
    {
        return paintRect_.GetOffset();
    }

    void SetPosition(const Offset& offset);

    void SetAbsolutePosition(const Offset& offset)
    {
        SetPositionInternal(offset);
    }

    Size GetLayoutSize() const
    {
        return paintRect_.GetSize();
    }

    Rect GetRectWithShadow() const;

    void SetShadow(const Shadow& shadow)
    {
        shadow_ = shadow;
        hasShadow_ = true;
    }

    void SetLayoutSize(const Size& size);

    Rect GetRectBasedWindowTopLeft()
    {
        return Rect(GetGlobalOffset(), paintRect_.GetSize());
    }

    virtual const Rect& GetTouchRect()
    {
        if (needUpdateTouchRect_) {
            needUpdateTouchRect_ = false;
            UpdateTouchRect();
        }
        return touchRect_;
    }

    virtual const std::vector<Rect>& GetTouchRectList()
    {
        if (needUpdateTouchRect_) {
            needUpdateTouchRect_ = false;
            touchRectList_.clear();
            UpdateTouchRect();
        }
        return touchRectList_;
    }

    void ChangeTouchRectList(std::vector<Rect>& touchRectList)
    {
        touchRectList_ = touchRectList;
    }

    bool InTouchRectList(const Point& parentLocalPoint, const std::vector<Rect>& touchRectList) const
    {
        for (auto& rect : touchRectList) {
            if (rect.IsInRegion(parentLocalPoint)) {
                return true;
            }
        }
        return false;
    }

    virtual bool IsUseOnly()
    {
        return false;
    }

    virtual bool IsNotSiblingAddRecognizerToResult()
    {
        return true;
    }

    virtual Point GetTransformPoint(const Point& point)
    {
        return point;
    }

    virtual Rect GetTransformRect(const Rect& rect)
    {
        return rect;
    }

    const Rect& GetPaintRect() const;

    Rect GetTransitionPaintRect() const;

    Offset GetTransitionGlobalOffset() const;

    void SetPaintRect(const Rect& rect);

    void SetTouchRect(const Rect& rect)
    {
        touchRect_ = rect;
        needUpdateTouchRect_ = false;
    }

    void MarkNeedUpdateTouchRect(bool needUpdateTouchRect)
    {
        needUpdateTouchRect_ = needUpdateTouchRect;
    }

    virtual void OnChildAdded(const RefPtr<RenderNode>& child)
    {
        if (slipFactorSetting_) {
            child->SetSlipFactorSetting(slipFactorSetting_);
        }
    }

    virtual void OnChildRemoved(const RefPtr<RenderNode>& child) {}

    virtual void OnRemove() {}

    const std::string& GetAccessibilityText() const
    {
        return accessibilityText_;
    }

    void SetAccessibilityText(const std::string& accessibilityText)
    {
        accessibilityText_ = accessibilityText;
    }

    virtual void DumpTree(int32_t depth);

    virtual void Dump();

    enum class BridgeType { NONE, ROSEN, FLUTTER };

    virtual BridgeType GetBridgeType() const
    {
        return BridgeType::NONE;
    }

    void SetNeedRender(bool needRender)
    {
        needRender_ = needRender;
    }

    void MarkNeedRender(bool overlay = false);

    bool NeedRender() const
    {
        return needRender_;
    }

    void SetDepth(int32_t depth);

    int32_t GetDepth() const
    {
        return depth_;
    }

    PositionType GetPositionType() const
    {
        return positionParam_.type;
    }

    virtual const Dimension& GetLeft() const
    {
        return positionParam_.left.first;
    }

    virtual const Dimension& GetRight() const
    {
        return positionParam_.right.first;
    }

    virtual const Dimension& GetTop() const
    {
        return positionParam_.top.first;
    }

    virtual const Dimension& GetBottom() const
    {
        return positionParam_.bottom.first;
    }

    const Dimension& GetAnchorX() const
    {
        return positionParam_.anchor.first;
    }

    const Dimension& GetAnchorY() const
    {
        return positionParam_.anchor.second;
    }

    virtual bool HasLeft() const
    {
        return positionParam_.left.second;
    }

    virtual bool HasRight() const
    {
        return positionParam_.right.second;
    }

    virtual bool HasTop() const
    {
        return positionParam_.top.second;
    }

    virtual bool HasBottom() const
    {
        return positionParam_.bottom.second;
    }

    virtual void SetLeft(const Dimension& left) // add for animation
    {
        if (positionParam_.left.first != left) {
            positionParam_.left = std::make_pair(left, true);
            MarkNeedLayout();
        }
    }

    virtual void SetTop(const Dimension& top) // add for animation
    {
        if (positionParam_.top.first != top) {
            positionParam_.top = std::make_pair(top, true);
            MarkNeedLayout();
        }
    }

    virtual void SetRight(const Dimension& right) // add for animation
    {
        if (positionParam_.right.first != right) {
            positionParam_.top = std::make_pair(right, true);
            MarkNeedLayout();
        }
    }

    virtual void SetBottom(const Dimension& bottom) // add for animation
    {
        if (positionParam_.bottom.first != bottom) {
            positionParam_.bottom = std::make_pair(bottom, true);
            MarkNeedLayout();
        }
    }

    WeakPtr<RenderNode> GetParent() const
    {
        return parent_;
    }

    WeakPtr<PipelineContext> GetContext() const
    {
        return context_;
    }

    virtual RenderLayer GetRenderLayer()
    {
        return nullptr;
    }

    void SetVisible(bool visible, bool inRecursion = false)
    {
        if (visible_ != visible) {
            visible_ = visible;
            AddDirtyRenderBoundaryNode();
            OnVisibleChanged();
            if (!inRecursion && SystemProperties::GetRosenBackendEnabled()) {
                MarkParentNeedRender();
            }
        }
        for (auto& child : children_) {
            child->SetVisible(visible, true);
        }
    }

    virtual bool GetVisible() const
    {
        return visible_;
    }

    virtual void SetHidden(bool hidden, bool inRecursion = false)
    {
        if (hidden_ != hidden) {
            hidden_ = hidden;
            AddDirtyRenderBoundaryNode();
            OnHiddenChanged(hidden);
            if (!inRecursion && SystemProperties::GetRosenBackendEnabled()) {
                MarkParentNeedRender();
            }
            if (hidden_) {
                disableTouchEvent_ = true;
            } else {
                disableTouchEvent_ = false;
            }
        }
        for (auto& child : children_) {
            child->SetHidden(hidden, true);
        }
    }

    void SetSelfHidden(bool hidden)
    {
        if (hidden_ != hidden) {
            hidden_ = hidden;
            AddDirtyRenderBoundaryNode();
            OnHiddenChanged(hidden);
            if (SystemProperties::GetRosenBackendEnabled()) {
                MarkParentNeedRender();
            }
        }
    }

    bool GetHidden() const
    {
        return hidden_;
    }

    bool IsTakenBoundary() const
    {
        return takeBoundary_;
    }

    virtual bool IsRepaintBoundary() const
    {
        return IsHeadRenderNode();
    }

    virtual const std::list<RefPtr<RenderNode>>& GetChildren() const
    {
        return children_;
    }

    virtual void NotifyPaintFinish();

    virtual void RenderWithContext(RenderContext& context, const Offset& offset);
    virtual void Paint(RenderContext& context, const Offset& offset);
    virtual void PaintChild(const RefPtr<RenderNode>& child, RenderContext& context, const Offset& offset);

    virtual void OnPaintFinish() {}

    virtual bool TouchTest(const Point& globalPoint, const Point& parentLocalPoint, const TouchRestrict& touchRestrict,
        TouchTestResult& result);

    virtual void MouseTest(const Point& globalPoint, const Point& parentLocalPoint, MouseTestResult& result);

    virtual bool MouseHoverTest(const Point& parentLocalPoint);

    virtual bool MouseDetect(const Point& globalPoint, const Point& parentLocalPoint, MouseHoverTestList& result,
        WeakPtr<RenderNode>& hoverNode);

    virtual bool AxisDetect(const Point& globalPoint, const Point& parentLocalPoint, WeakPtr<RenderNode>& axisNode,
        const AxisDirection direction);

    virtual void HandleMouseHoverEvent(const MouseState mouseState) {}

    virtual bool HandleMouseEvent(const MouseEvent& event)
    {
        return false;
    }

    virtual void HandleAxisEvent(const AxisEvent& event) {}

    virtual bool RotationMatchTest(const RefPtr<RenderNode>& requestRenderNode);

    virtual bool RotationTest(const RotationEvent& event);

    virtual bool RotationTestForward(const RotationEvent& event);

    virtual double GetBaselineDistance(TextBaseline textBaseline);

    virtual Size GetContentSize();

    virtual bool ScrollPageByChild(Offset& delta, int32_t source);

    // Change render nodes' status
    void ChangeStatus(RenderStatus renderStatus)
    {
        // Myself status should be changed and function achieved by derived class which is component
        OnStatusChanged(renderStatus);

        // Deep traversal
        for (auto& child : children_) {
            child->ChangeStatus(renderStatus);
        }
    }

    virtual void OnStatusStyleChanged(VisualState state);

    Offset GetOffsetFromOrigin(const Offset& offset) const;

    virtual Offset GetGlobalOffset() const;

    virtual Offset GetGlobalOffsetExternal() const;

    // Whether |rect| is in the paint rect of render tree recursively.
    bool IsVisible(const Rect& rect, bool totally = false) const;

    void SetOnChangeCallback(std::function<void()>&& onChangeCallback)
    {
        onChangeCallback_ = std::move(onChangeCallback);
    }

    void SetDisableTouchEvent(bool disableTouchEvent)
    {
        disableTouchEvent_ = disableTouchEvent;
    }

    bool GetDisableTouchEvent() const
    {
        return disableTouchEvent_;
    }

    virtual bool IsChildrenTouchEnable()
    {
        return true;
    }

    void SetTextDirection(TextDirection textDirection)
    {
        textDirection_ = textDirection;
    }

    TextDirection GetTextDirection() const
    {
        return textDirection_;
    }

    // Transfer any other dimension unit to logical px.
    // NOTE: context_ MUST be initialized before call this method.
    double NormalizeToPx(Dimension dimension) const;

    // Mainly use this function to convert Percent to Px. Do not call this function in Update().
    double NormalizePercentToPx(const Dimension& dimension, bool isVertical, bool referSelf = false) const;

    // for accessibility
    void SetAccessibilityNode(const WeakPtr<AccessibilityNode>& accessibilityNode)
    {
        accessibilityNode_ = accessibilityNode;
    }

    const WeakPtr<AccessibilityNode>& GetAccessibilityNode() const
    {
        return accessibilityNode_;
    }

    int32_t GetAccessibilityNodeId() const
    {
        auto accessibilityNode = accessibilityNode_.Upgrade();
        if (accessibilityNode) {
            return accessibilityNode->GetNodeId();
        }
        return 0;
    }

    void ClearAccessibilityRect()
    {
        auto node = accessibilityNode_.Upgrade();
        if (node) {
            node->ClearRect();
        }
        for (auto& child : children_) {
            child->ClearAccessibilityRect();
        }
    }
    void SetAccessibilityRect(const Rect& rect);

    void SetNeedUpdateAccessibility(bool needUpdate)
    {
        needUpdateAccessibility_ = needUpdate;
        for (auto& child : children_) {
            child->SetNeedUpdateAccessibility(needUpdate);
        }
    }

    void SetAccessibilityVisible(bool visible)
    {
        auto node = accessibilityNode_.Upgrade();
        if (node) {
            node->SetVisible(visible);
        }
        for (auto& child : children_) {
            child->SetAccessibilityVisible(visible);
        }
    }

    RefPtr<RenderNode> GetLastChild() const;

    RefPtr<RenderNode> GetFirstChild() const;

    Offset GetOffsetToStage() const
    {
        auto offset = GetGlobalOffset();
        auto context = GetContext().Upgrade();
        if (context) {
            offset = offset - context->GetStageRect().GetOffset();
        }
        return offset;
    }

    Offset GetOffsetToPage() const;

    double GetFlexWeight() const
    {
        return flexWeight_;
    }

    int32_t GetDisplayIndex() const
    {
        return displayIndex_;
    }

    OpacityCallback GetOpacityCallback(int32_t domId);

    virtual bool SupportOpacity();

    void GetDomOpacityCallbacks(int32_t domId, std::list<OpacityCallback>& result);

    int32_t GetNodeId() const;

    uint8_t GetOpacity() const;

    virtual void UpdateOpacity(uint8_t opacity);

    bool InterceptTouchEvent() const
    {
        return interceptTouchEvent_;
    }

    void SetInterceptTouchEvent(bool interceptTouchEvent)
    {
        interceptTouchEvent_ = interceptTouchEvent;
    }

    void DispatchCancelPressAnimation()
    {
        OnCancelPressAnimation();
        for (const auto& child : children_) {
            child->DispatchCancelPressAnimation();
        }
    }

    virtual WeakPtr<RenderNode> CheckAxisNode()
    {
        return nullptr;
    }
    virtual WeakPtr<RenderNode> CheckHoverNode()
    {
        return nullptr;
    }
    virtual void MouseHoverEnterTest() {}
    virtual void MouseHoverExitTest() {}
    virtual void AnimateMouseHoverEnter() {}
    virtual void AnimateMouseHoverExit() {}
    virtual void OnCancelPressAnimation() {}
    virtual void OnMouseHoverEnterAnimation() {}
    virtual void OnMouseHoverExitAnimation() {}
    virtual void OnMouseClickDownAnimation() {}
    virtual void OnMouseClickUpAnimation() {}
    virtual void StopMouseHoverAnimation() {}
    virtual bool IsAxisScrollable(AxisDirection direction)
    {
        return false;
    }

    virtual void OnVisibleChanged() {}

    void CreateMouseAnimation(RefPtr<KeyframeAnimation<Color>>& animation, const Color& from, const Color& to);
    void SetHoverAndPressCallback(const HoverAndPressCallback& callback)
    {
        hoveAndPressCallback_ = callback;
    }

    Color GetEventEffectColor() const
    {
        return eventEffectColor_;
    }

    void UpdateWindowBlurRRect(bool clear = false);

    void WindowBlurTest();

    virtual RRect GetWindowBlurRRect() const
    {
        return RRect::MakeRRect(Rect(Offset::Zero(), GetLayoutSize()), Radius(0.0));
    }

    RRect GetGlobalWindowBlurRRect(std::vector<RRect>& coords) const;

    void MarkNeedWindowBlur(bool flag)
    {
        if (needWindowBlur_ != flag) {
            needWindowBlur_ = flag;
            if (!needWindowBlur_) {
                UpdateWindowBlurProgress(0.0f);
                SetWindowBlurStyle(WindowBlurStyle::STYLE_BACKGROUND_SMALL_LIGHT);
            }
            OnWindowBlurChanged();
        }
    }

    bool NeedWindowBlur() const
    {
        return needWindowBlur_;
    }

    void UpdateWindowBlurProgress(float progress)
    {
        windowBlurProgress_ = progress;
    }

    float GetWindowBlurProgress() const
    {
        return windowBlurProgress_;
    }

    void SetWindowBlurStyle(WindowBlurStyle style)
    {
        windowBlurStyle_ = style;
    }

    WindowBlurStyle GetWindowBlurStyle() const
    {
        return windowBlurStyle_;
    }

    void SetSlipFactorSetting(const SlipFactorSetting& slipFactorSetting)
    {
        slipFactorSetting_ = slipFactorSetting;
    }

    bool IsInfiniteLayout() const
    {
        return GetLayoutSize().Width() > INT32_MAX || GetLayoutSize().Height() > INT32_MAX;
    }

    bool IsIgnored() const
    {
        return isIgnored_;
    }

    void SetIsIgnored(bool ignore)
    {
        isIgnored_ = ignore;
    }

    void SetGlobalPoint(const Point& point)
    {
        globalPoint_ = point;
    }

    const Point& GetGlobalPoint()
    {
        return globalPoint_;
    }

    void SetCoordinatePoint(const Point& point)
    {
        coordinatePoint_ = point;
    }

    const Point& GetCoordinatePoint()
    {
        return coordinatePoint_;
    }

    bool IsTouchable() const
    {
        return touchable_;
    }

    bool IsDisabled() const
    {
        return disabled_;
    }

    virtual void OnAppShow()
    {
        isAppOnShow_ = true;
        for (const auto& child : children_) {
            child->OnAppShow();
        }
    }

    virtual void OnAppHide()
    {
        isAppOnShow_ = false;
        for (const auto& child : children_) {
            child->OnAppHide();
        }
    }

    bool IsAppShow()
    {
        return isAppOnShow_;
    }

    template<typename T>
    RefPtr<T> GetTheme() const
    {
        auto context = context_.Upgrade();
        if (!context) {
            return nullptr;
        }
        auto themeManager = context->GetThemeManager();
        if (!themeManager) {
            return nullptr;
        }
        return themeManager->GetTheme<T>();
    }

    virtual bool HasEffectiveTransform() const;

    virtual void OnTransition(TransitionType type, int32_t id) {}

    bool IsDisappearing();
    virtual bool HasDisappearingTransition(int32_t nodeId);
    void NotifyTransition(TransitionType type, int32_t nodeId);
    virtual void SetPendingAppearingTransition() {}

    Rect GetDirtyRect() const;
    std::function<void(const DragUpdateInfo&)> onDomDragEnter_ = nullptr;
    std::function<void(const DragUpdateInfo&)> onDomDragOver_ = nullptr;
    std::function<void(const DragUpdateInfo&)> onDomDragLeave_ = nullptr;
    std::function<void(const DragEndInfo&)> onDomDragDrop_ = nullptr;
    virtual bool GetAlignDeclarationOffset(AlignDeclarationPtr alignDeclarationPtr, Offset& offset) const;

    // Each subclass override this to return touch target object which is used to receive touch event.
    // For convenience, it is recommended to return directly to the corresponding gesture recognizer.
    // Sees gestures directory.
    // Uses coordinateOffset for recognizer to calculate the local location of the touch point.
    // Uses touchRestrict for restrict gesture recognition in some sense.
    virtual void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
    {}
    virtual void OnPreDraw() {}

    template<typename T>
    RefPtr<T> FindChildNodeOfClass(const Point& globalPoint, const Point& parentLocalPoint)
    {
        Point transformPoint = GetTransformPoint(parentLocalPoint);
        if (!InTouchRectList(transformPoint, GetTouchRectList())) {
            return nullptr;
        }

        // Calculates the local point location in this node.
        const auto localPoint = transformPoint - GetPaintRect().GetOffset();
        const auto& children = GetChildren();
        for (auto iter = children.rbegin(); iter != children.rend(); ++iter) {
            auto& child = *iter;
            if (!child->GetVisible()) {
                continue;
            }

            if (child->InterceptTouchEvent()) {
                continue;
            }

            auto target = child->FindChildNodeOfClass<T>(globalPoint, localPoint);
            if (target) {
                return target;
            }
        }

        for (auto& rect : GetTouchRectList()) {
            if (touchable_ && rect.IsInRegion(transformPoint)) {
                RefPtr<RenderNode> renderNode = AceType::Claim<RenderNode>(this);
                if (AceType::InstanceOf<T>(renderNode)) {
                    return AceType::DynamicCast<T>(renderNode);
                }
            }
        }
        return nullptr;
    }

    template<typename T>
    RefPtr<T> FindTargetRenderNode(const RefPtr<PipelineContext> context, const GestureEvent& info)
    {
        if (!context) {
            return nullptr;
        }

        auto pageRenderNode = context->GetLastPageRender();
        if (!pageRenderNode) {
            return nullptr;
        }

        return pageRenderNode->FindChildNodeOfClass<T>(info.GetGlobalPoint(), info.GetGlobalPoint());
    }

    template<class T>
    RefPtr<T> FindChildOfClass(const RefPtr<RenderNode>& parent)
    {
        // BFS to find child in tree.
        uint32_t findCount = 0;
        auto searchQueue = parent->GetChildren(); // copy children to a queue
        while (++findCount <= FIND_MAX_COUNT && !searchQueue.empty()) {
            const auto child = searchQueue.front();
            searchQueue.pop_front();
            if (!child) {
                continue;
            }
            if (AceType::InstanceOf<T>(child)) {
                return AceType::DynamicCast<T>(child);
            }
            searchQueue.insert(searchQueue.end(), child->GetChildren().begin(), child->GetChildren().end());
        }
        return RefPtr<T>();
    }

    void SaveExplicitAnimationOption(const AnimationOption& option);

    const AnimationOption& GetExplicitAnimationOption() const;

    void ClearExplicitAnimationOption();

    void ClearDisappearingNode(RefPtr<RenderNode> child);

    void CreateLayoutTransition();

    void CreatePathAnimation();

    bool IsExclusiveEventForChild() const
    {
        return exclusiveEventForChild_;
    }
    void SetExclusiveEventForChild(bool exclusiveEventForChild)
    {
        exclusiveEventForChild_ = exclusiveEventForChild;
    }

    void MarkUpdateType(const RefPtr<Component>& component);

    virtual RefPtr<Component> GetComponent()
    {
        return nullptr;
    }

    virtual void NotifySizeTransition(const AnimationOption& option, Size fromSize, Size toSize, int32_t nodeId);
    void CreateGeometryTransitionFrom(const RefPtr<RenderNode>& targetNode, AnimationOption& sharedOption);
    void CreateGeometryTransitionTo(const RefPtr<RenderNode>& targetNode, AnimationOption& sharedOption);
    void SetIsPaintGeometryTransition(bool isPaintGeometryTransition);
    void SetPaintOutOfParent(bool isPaintOutOfParent);
    bool IsPaintOutOfParent();
    void UpdatePosition();

    void SetHasSubWindow(bool hasSubWindow)
    {
        hasSubWindow_ = hasSubWindow;
    }

    bool GetHasSubWindow() const
    {
        return hasSubWindow_;
    }

    // mark JSview boundary, create/destroy RSNode if need
    void SyncRSNodeBoundary(bool isHead, bool isTail);
    void SyncRSNode(std::shared_ptr<RSNode> rsNode);
    const std::shared_ptr<RSNode>& GetRSNode() const
    {
        return rsNode_;
    }
    // sync geometry properties to ROSEN backend
    virtual void SyncGeometryProperties();

    bool IsResponseRegion() const
    {
        return isResponseRegion_;
    }

    double GetPxValue(double standard, const Dimension& value);

    const std::vector<Rect>& GetResponseRegionList() const
    {
        return responseRegionList_;
    }

    const WeakPtr<V2::InspectorNode>& GetInspectorNode() const
    {
        return inspector_;
    }

    void SetInspectorNode(const RefPtr<V2::InspectorNode>& inspectorNode)
    {
        inspector_ = inspectorNode;
    }

    virtual void SetNeedClip(bool needClip)
    {
        needClip_ = needClip;
    }

    bool GetNeedClip()
    {
        return needClip_;
    }

    RefPtr<RenderNode> FindDropChild(const Point& globalPoint, const Point& parentLocalPoint);
    static constexpr size_t DEFAULT_INDEX = -1;

    virtual std::string ProvideRestoreInfo()
    {
        return "";
    }

    void SetRestoreInfo(const std::string& restoreInfo)
    {
        restoreInfo_ = restoreInfo;
    }

    const std::string& GetRestoreInfo() const
    {
        return restoreInfo_;
    }

protected:
    explicit RenderNode(bool takeBoundary = false);
    virtual void ClearRenderObject();

    virtual void OnMouseTestHit(const Offset& coordinateOffset, MouseTestResult& result) {}
    virtual void OnMouseHoverEnterTest() {}
    virtual void OnMouseHoverExitTest() {}
    void SendAccessibilityEvent(const std::string& eventType);

    void PrepareLayout();

    void SetParent(const WeakPtr<RenderNode>& parent)
    {
        parent_ = parent;
    }

    void TakeBoundary(bool taken = true)
    {
        takeBoundary_ = taken;
    }

    bool IsLayoutParamChanged() const
    {
        return layoutParamChanged_;
    }

    virtual void OnGlobalPositionChanged()
    {
        MarkNeedSyncGeometryProperties();
        if (IsTailRenderNode()) {
            return;
        }
        for (const auto& child : children_) {
            if (child) {
                child->OnGlobalPositionChanged();
            }
        }
    };
    virtual void OnPositionChanged() {}
    virtual void OnSizeChanged() {}
    virtual void OnRenderFinish(RenderContext& context) {}
    virtual void OnStatusChanged(RenderStatus renderStatus) {}
    virtual void OnHiddenChanged(bool hidden) {}
    virtual void OnWindowBlurChanged() {}
    virtual bool MarkNeedRenderSpecial();

    double GetHighestChildBaseline(TextBaseline baseline);
    double GetFirstChildBaseline(TextBaseline baseline);
    Size GetLargestChildContentSize();
    void UpdateAccessibilityPosition();
    void CheckIfNeedUpdateTouchRect();

    RefPtr<ThemeManager> GetThemeManager() const
    {
        auto context = context_.Upgrade();
        if (!context) {
            return nullptr;
        }
        return context->GetThemeManager();
    }

    uint32_t GetUpdateType()
    {
        return updateType_;
    }

    virtual std::shared_ptr<RSNode> CreateRSNode() const;
    virtual void OnRSTransition(TransitionType type) {}
    // JSview boundary, all nodes in [head, tail] share the same RSNode
    bool IsHeadRenderNode() const
    {
#ifdef ENABLE_ROSEN_BACKEND
        return SystemProperties::GetRosenBackendEnabled() ? isHeadRenderNode_ : false;
#else
        return false;
#endif
    }
    bool IsTailRenderNode() const
    {
        return isTailRenderNode_;
    }
    Offset GetPaintOffset() const;
    virtual bool HasGeometryProperties() const
    {
        return IsTailRenderNode();
    }
    void MarkNeedSyncGeometryProperties();

    bool hasSubWindow_ = false;
    bool needClip_ = false;
    WeakPtr<PipelineContext> context_;
    Size viewPort_;
    Point globalPoint_;
    Point coordinatePoint_;
    WeakPtr<V2::InspectorNode> inspector_;
    WeakPtr<AccessibilityNode> accessibilityNode_;

    Rect touchRect_;                  // Self touch rect
    std::vector<Rect> touchRectList_; // Self and all children touch rect
    std::vector<DimensionRect> responseRegion_;
    std::vector<Rect> responseRegionList_;
    PositionParam positionParam_;
    uint8_t opacity_ = 255;
    Shadow shadow_;

    float windowBlurProgress_ = 0.0f;
    WindowBlurStyle windowBlurStyle_ = WindowBlurStyle::STYLE_BACKGROUND_SMALL_LIGHT;
    bool touchable_ = true;
    bool interceptTouchEvent_ = false;
    bool needWindowBlur_ = false;
    bool needUpdateAccessibility_ = true;
    bool disabled_ = false;
    bool isResponseRegion_ = false;
    HoverAnimationType hoverAnimationType_ = HoverAnimationType::AUTO;
    int32_t minPlatformVersion_ = 0;

    MouseState mouseState_ = MouseState::NONE;
    SlipFactorSetting slipFactorSetting_;

    HoverAndPressCallback hoveAndPressCallback_;

    // hover or press color
    Color eventEffectColor_ = Color::TRANSPARENT;
    std::function<void(const std::string&)> onLayoutReady_;

    bool isAppOnShow_ = true;
    AnimationOption nonStrictOption_; // clear after transition done
    MotionPathOption motionPathOption_;
    RefPtr<V2::EventExtensions> eventExtensions_;

    // Compute multiSelect zone
    Rect ComputeSelectedZone(const Offset& startOffset, const Offset& endOffset);

private:
    void AddDirtyRenderBoundaryNode()
    {
        if (visible_ && !hidden_ && IsRepaintBoundary()) {
            auto pipelineContext = context_.Upgrade();
            if (pipelineContext == nullptr) {
                return;
            }
            pipelineContext->AddDirtyRenderNode(AceType::Claim(this));
        }
    }

    void SetPositionInternal(const Offset& offset);
    bool InLayoutTransition() const;
    // Sync view hierarchy to RSNode
    void RSNodeAddChild(const RefPtr<RenderNode>& child);
    void MarkParentNeedRender() const;

    std::list<RefPtr<RenderNode>> hoverChildren_;
    std::list<RefPtr<RenderNode>> children_;
    std::string accessibilityText_;
    LayoutParam layoutParam_;
    Rect paintRect_;
    WeakPtr<RenderNode> parent_;
    int32_t depth_ = 0;
    bool needRender_ = false;
    bool needLayout_ = false;
    bool visible_ = true;
    bool takeBoundary_ = false;
    bool layoutParamChanged_ = false;
    bool pendingDispatchLayoutReady_ = false;
    bool disableTouchEvent_ = false;
    bool needUpdateTouchRect_ = false;
    bool hasShadow_ = false;

    double flexWeight_ = 0.0;
    int32_t displayIndex_ = 1;

    double dipScale_ = 0.0;

    TextDirection textDirection_ { TextDirection::LTR };
    Offset selfOffset_ { 0, 0 };

    bool hidden_ = false;
    bool isIgnored_ = false;
    std::function<void()> onChangeCallback_;
    std::list<RefPtr<RenderNode>> disappearingNodes_;
    AnimatableDimension paintX_;
    AnimatableDimension paintY_;
    AnimatableDimension paintW_;
    AnimatableDimension paintH_;
    Size transitionPaintRectSize_;
    Rect nonStrictPaintRect_;
    bool isFirstSizeAssign_ = true;
    bool isFirstPositionAssign_ = true;

    // for container, this flag controls only the last child in touch area is consuming event.
    bool exclusiveEventForChild_ = false;
    int32_t zIndex_ = 0;
    bool isPercentSize_ = false;
    uint32_t updateType_ = 0;

    std::shared_ptr<RSNode> rsNode_ = nullptr;
    bool isHeadRenderNode_ = false;
    bool isTailRenderNode_ = false;

    bool isPaintGeometryTransition_ = false;
    bool isPaintOutOfParent_ = false;

    std::string restoreInfo_;

    ACE_DISALLOW_COPY_AND_MOVE(RenderNode);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_RENDER_NODE_H
