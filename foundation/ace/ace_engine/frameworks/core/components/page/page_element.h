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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_PAGE_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_PAGE_ELEMENT_H

#include <functional>
#include <unordered_map>

#include "core/components/shared_transition/shared_transition_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace {

class SharedTransitionElement;
class TransformElement;

class PageElement : public ComposedElement, FocusGroup {
    DECLARE_ACE_TYPE(PageElement, ComposedElement, FocusGroup);

public:
    using SharedTransitionMap = std::unordered_map<ShareId, WeakPtr<SharedTransitionElement>>;
    using HiddenCallbackMap = std::unordered_map<int32_t, std::function<void(bool)>>;
    using CardTransitionMap = std::unordered_map<int32_t, WeakPtr<TransformElement>>;

    // Use this to store geometryTransition matched renderNodes and animation option
    struct GeometryTransitionInfo {
        AnimationOption sharedAnimationOption;
        WeakPtr<BoxElement> appearElement;
        WeakPtr<BoxElement> disappearElement;
        bool isNeedCreate;
    };
    using GeometryTransitionMap = std::unordered_map<std::string, GeometryTransitionInfo>;
    PageElement(int32_t pageId, const std::string& pageUrl, const ComposeId& id);
    PageElement(int32_t pageId, const std::string& pageUrl, const ComposeId& cardComposeId, const ComposeId& id);
    ~PageElement();

    int32_t GetPageId()
    {
        return pageId_;
    }

    std::string GetPageUrl() const
    {
        return pageUrl_;
    }

    void RemoveSharedTransition(const ShareId& shareId);
    void AddSharedTransition(const RefPtr<SharedTransitionElement>& shared);
    const SharedTransitionMap& GetSharedTransitionMap() const;

    void SetHidden(bool hidden);

    int32_t RegisterHiddenCallback(std::function<void(bool)>&& callback)
    {
        if (callback) {
            hiddenCallbackMap_.emplace(callbackId_, std::move(callback));
        }
        return callbackId_++;
    }

    void CancelHiddenCallback(int32_t callbackId)
    {
        hiddenCallbackMap_.erase(callbackId);
    }

    const ComposeId& GetCardComposeId() const
    {
        return cardComposeId_;
    }

    void RemoveCardTransition(int32_t retakeId);
    void AddCardTransition(const RefPtr<TransformElement>& transform);
    const CardTransitionMap& GetCardTransitionMap() const;

    void AddGeometryTransition(const std::string& id, WeakPtr<BoxElement>& boxElement, AnimationOption& option);
    const GeometryTransitionMap& GetGeometryTransition() const;
    void RemoveGeometryTransition(const std::string& id);
    void FinishCreateGeometryTransition(const std::string& id);
    void Dump() override;

protected:
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;

private:
    int32_t callbackId_ = 0;
    int32_t pageId_ = -1;
    std::string pageUrl_;
    ComposeId cardComposeId_;
    SharedTransitionMap sharedTransitionElementMap_;
    HiddenCallbackMap hiddenCallbackMap_;
    CardTransitionMap cardTransitionMap_;
    GeometryTransitionMap geometryTransitionMap_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_PAGE_ELEMENT_H
