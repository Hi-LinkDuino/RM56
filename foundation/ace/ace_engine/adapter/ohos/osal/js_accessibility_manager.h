/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_ACCESSIBILITY_JS_ACCESSIBILITY_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_ACCESSIBILITY_JS_ACCESSIBILITY_MANAGER_H

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "accessibility_element_operator.h"
#include "accessibility_state_event.h"

#include "core/accessibility/accessibility_manager.h"
#include "core/accessibility/accessibility_utils.h"
#include "frameworks/bridge/common/accessibility/accessibility_node_manager.h"

namespace OHOS::Ace::Framework {

class JsAccessibilityManager : public AccessibilityNodeManager {
    DECLARE_ACE_TYPE(JsAccessibilityManager, AccessibilityNodeManager);

public:
    JsAccessibilityManager() = default;
    virtual ~JsAccessibilityManager();

    // JsAccessibilityManager overrides functions.
    void InitializeCallback() override;
    void SendAccessibilityAsyncEvent(const AccessibilityEvent& accessibilityEvent) override;
    void DumpHandleEvent(const std::vector<std::string>& params) override;
    void DumpProperty(const std::vector<std::string>& params) override;
    void SetCardViewParams(const std::string& key, bool focus) override;
    void HandleComponentPostBinding() override;

    void UpdateViewScale();

    float GetScaleX() const
    {
        return scaleX_;
    }

    float GetScaleY() const
    {
        return scaleY_;
    }

    int GetWindowId() const
    {
        return windowId_;
    }

    void SetWindowId(const int windowId)
    {
        windowId_ = windowId;
    }

    bool IsRegister();
    void Register(bool state);
    virtual bool SubscribeStateObserver(const int eventType);
    virtual bool UnsubscribeStateObserver(const int eventType);
    virtual int RegisterInteractionOperation(const int windowId);
    virtual void DeregisterInteractionOperation();
    virtual bool SendAccessibilitySyncEvent(const AccessibilityEvent& accessibilityEvent);

    void SearchElementInfoByAccessibilityId(const long elementId, const int requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback, const int mode);
    void SearchElementInfosByText(const long elementId, const std::string& text, const int requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback);
    void FindFocusedElementInfo(const long elementId, const int focusType, const int requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback);
    void FocusMoveSearch(const long elementId, const int direction, const int requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback);
    void ExecuteAction(const long accessibilityId, const Accessibility::ActionType& action,
        const std::map<std::string, std::string> actionArguments, const int requestId,
        Accessibility::AccessibilityElementOperatorCallback& callback);
    bool ClearCurrentFocus();

private:
    void UpdateNodeChildIds(const RefPtr<AccessibilityNode>& node);

    class JsInteractionOperation : public Accessibility::AccessibilityElementOperator {
    public:
        virtual ~JsInteractionOperation() = default;
        // Accessibility override.
        void SearchElementInfoByAccessibilityId(const long elementId, const int requestId,
            Accessibility::AccessibilityElementOperatorCallback& callback, const int mode) override;
        void SearchElementInfosByText(const long elementId, const std::string& text, const int requestId,
            Accessibility::AccessibilityElementOperatorCallback& callback) override;
        void FindFocusedElementInfo(const long elementId, const int focusType, const int requestId,
            Accessibility::AccessibilityElementOperatorCallback& callback) override;
        void FocusMoveSearch(const long elementId, const int direction, const int requestId,
            Accessibility::AccessibilityElementOperatorCallback& callback) override;
        void ExecuteAction(const long elementId, const int action,
            const std::map<std::string, std::string> actionArguments, const int requestId,
            Accessibility::AccessibilityElementOperatorCallback& callback) override;
        void ClearFocus() override;
        void OutsideTouch() override;

        void SetHandler(const WeakPtr<JsAccessibilityManager>& js)
        {
            js_ = js;
        }

        const WeakPtr<JsAccessibilityManager>& GetHandler() const
        {
            return js_;
        }

    private:
        WeakPtr<JsAccessibilityManager> js_;
    };

    class JsAccessibilityStateObserver : public Accessibility::AccessibilityStateObserver {
    public:
        void OnStateChanged(const bool state) override;
        void SetHandler(const WeakPtr<JsAccessibilityManager>& js)
        {
            js_ = js;
        }

        const WeakPtr<JsAccessibilityManager>& GetHandler() const
        {
            return js_;
        }

    private:
        WeakPtr<JsAccessibilityManager> js_;
    };

    bool AccessibilityActionEvent(const Accessibility::ActionType& action,
        const std::map<std::string, std::string> actionArguments, const RefPtr<AccessibilityNode>& node,
        const RefPtr<PipelineContext>& context);
    bool RequestAccessibilityFocus(const RefPtr<AccessibilityNode>& node);

    bool ClearAccessibilityFocus(const RefPtr<AccessibilityNode>& node);

    void AddFocusableNode(std::list<RefPtr<AccessibilityNode>>& nodeList, const RefPtr<AccessibilityNode>& node);
    bool CanAccessibilityFocused(const RefPtr<AccessibilityNode>& node);
    RefPtr<AccessibilityNode> FindNodeInRelativeDirection(
        const std::list<RefPtr<AccessibilityNode>>& nodeList, RefPtr<AccessibilityNode>& node, const int direction);
    RefPtr<AccessibilityNode> FindNodeInAbsoluteDirection(
        const std::list<RefPtr<AccessibilityNode>>& nodeList, RefPtr<AccessibilityNode>& node, const int direction);
    RefPtr<AccessibilityNode> GetNextFocusableNode(
        const std::list<RefPtr<AccessibilityNode>>& nodeList, RefPtr<AccessibilityNode>& node);
    RefPtr<AccessibilityNode> GetPreviousFocusableNode(
        const std::list<RefPtr<AccessibilityNode>>& nodeList, RefPtr<AccessibilityNode>& node);
    bool CheckBetterRect(Rect nodeRect, const int direction, Rect itemRect, Rect tempBest);
    bool IsCandidateRect(Rect nodeRect, Rect itemRect, const int direction);
    bool OutrightBetter(Rect nodeRect, const int direction, Rect Rect1, Rect Rect2);
    bool CheckRectBeam(Rect nodeRect, Rect itemRect, const int direction);
    bool IsToDirectionOf(Rect nodeRect, Rect itemRect, const int direction);
    double MajorAxisDistanceToFarEdge(Rect nodeRect, Rect itemRect, const int direction);
    double MajorAxisDistance(Rect nodeRect, Rect itemRect, const int direction);
    double MinorAxisDistance(Rect nodeRect, Rect itemRect, const int direction);
    double GetWeightedDistanceFor(double majorAxisDistance, double minorAxisDistance);

    std::string callbackKey_;
    int windowId_ = 0;
    bool isReg_ = false;
    std::shared_ptr<JsAccessibilityStateObserver> stateObserver_ = nullptr;
    std::shared_ptr<JsInteractionOperation> interactionOperation_ = nullptr;
    float scaleX_ = 1.0f;
    float scaleY_ = 1.0f;
    NodeId currentFocusNodeId_ = -1;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_ACCESSIBILITY_JS_ACCESSIBILITY_MANAGER_H
