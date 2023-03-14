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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_RENDER_TAB_CONTENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_RENDER_TAB_CONTENT_H

#include <functional>
#include <unordered_map>

#include "base/memory/ace_type.h"
#include "core/animation/animation.h"
#include "core/animation/animator.h"
#include "core/components/tab_bar/tab_content_component.h"
#include "core/gestures/drag_recognizer.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderTabContent : public RenderNode {
    DECLARE_ACE_TYPE(RenderTabContent, RenderNode)

public:
    RenderTabContent();
    ~RenderTabContent() override = default;

    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    bool IsUseOnly() override;

    using UpdateIndexFunc = std::function<void(int32_t index)>;
    void RegisterCallback(const UpdateIndexFunc& callback)
    {
        callback_ = callback;
    }

    void RegisterRequireCallback(const UpdateIndexFunc& callback)
    {
        requireCallback_ = callback;
    }

    void ChangeScroll(int32_t index, bool fromController = false);

    void UpdateDragPosition(int32_t index);

    void AddChildContent(int32_t index, const RefPtr<RenderNode>& child)
    {
        LOGI("AddChildContent: %{public}d", index);
        auto iter = contentMap_.begin();
        while (iter != contentMap_.end()) {
            if (iter->second == child) {
                if (iter->first != index) {
                    // index has changed, clear map.
                    contentMap_.clear();
                }
                break;
            }
            iter++;
        }
        contentMap_[index] = child;
        requestedIndex_ = index;
        forceUpdate_ = true;
    }

    void RemoveChildContent(int32_t index)
    {
        LOGI("RemoveChildContent: %{public}d", index);
        if (contentMap_.find(index) != contentMap_.end()) {
            contentMap_.clear();
            forceUpdate_ = true;
        }
    }

    void RemoveChildContent(const RefPtr<RenderNode>& child)
    {
        LOGI("RemoveChildContent by content");
        auto iter = contentMap_.begin();
        while (iter != contentMap_.end()) {
            if (iter->second == child) {
                iter->second->SetHidden(true);
                contentMap_.clear();
                forceUpdate_ = true;
                break;
            }
            iter++;
        }
    }

    // Used by declarative element to flush index after performBuild.
    void FlushIndex();

    int32_t GetCurrentIndex() const
    {
        return currentIndex_;
    }

    const std::unordered_map<int32_t, RefPtr<RenderNode>>& GetChildContents() const
    {
        return contentMap_;
    }

    void FireDomChangeEvent(int32_t index) const;

    bool IsScrollable() const
    {
        return scrollable_;
    }

private:
    // for handle scroll tabContent
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    void HandleDragStart();
    void HandleDragUpdate(double offset);
    void HandleDragEnd();

    void Initialize(const WeakPtr<PipelineContext>& context);
    void FireContentChangeEvent() const;

    // used to scroll TabContent and update the position
    void ScrollContents(int32_t newIndex, bool isLinkBar, bool fromController = false);
    void UpdateScrollPosition(double dragDelta);
    void UpdateChildPosition(double offset, int32_t currentIndex, int32_t newIndex, bool needChange);
    void HandleStopListener(int32_t newIndex, bool needChange, bool FromController = false);
    void HandleStartListener(int32_t newIndex, bool needChange, bool isLinkBar);
    void SetHiddenChild();

    double GetOffset(double offset);

    static double GetFriction(double percentage);

    // if tab is vertical, offset changed on Y axis
    // if tab is horizontal, offset changed on X axis
    Offset GetMainAxisOffset(double offset) const
    {
        return isVertical_ ? Offset(0.0, offset) : Offset(offset, 0.0);
    }

    bool IsRightToLeft() const
    {
        return GetTextDirection() == TextDirection::RTL && !isVertical_;
    }
    // used to get the previous index or the next index
    int32_t GetPrevIndex() const;
    int32_t GetNextIndex() const;

    // dragRecognizer and the animation
    RefPtr<DragRecognizer> dragDetector_;
    RefPtr<Animation<double>> translate_;
    RefPtr<Animator> animator_;

    // map for contents
    std::unordered_map<int32_t, RefPtr<RenderNode>> contentMap_;

    // onChange event
    std::function<void(const std::string&)> changeEvent_;
    std::function<void(uint32_t)> domChangeEvent_;
    std::function<void(const std::shared_ptr<BaseEventInfo>&)> onChangeEvent_;

    bool isAnimationAdded_ = false; // whether the animation is added
    bool scrollable_ = true;        // the default value is true
    bool isInAnimation_ = false;    // whether it is in animation
    bool isDragging_ = false;       // whether it is dragging
    bool isInitialized_ = false;
    bool isVertical_ = false; // whether the tab is vertical
    bool forceUpdate_ = false;

    int32_t contentCount_ = 0;    // the count of content
    int32_t currentIndex_ = 0;    // the index of current tab
    int32_t requestedIndex_ = -1; // the next requested index

    float scrollDuration_ = 0.0f; // the duration of the scroll animation
    double contentWidth_ = 0.0;   // the width of tab content
    double contentHeight_ = 0.0;  // the height of tab content
    double prevOffset_ = 0.0;     // the offset of previous item
    double nextOffset_ = 0.0;     // the offset of next item
    double scrollOffset_ = 0.0;   // the offset when the content is scrolling

    // callbacks when updating the index
    UpdateIndexFunc callback_;
    UpdateIndexFunc requireCallback_;

    RefPtr<TabController> controller_;
    bool useInitialIndex_ = true;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_RENDER_TAB_CONTENT_H
