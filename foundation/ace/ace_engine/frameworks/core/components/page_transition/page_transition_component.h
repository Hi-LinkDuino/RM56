/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_TRANSITION_PAGE_TRANSITION_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_TRANSITION_PAGE_TRANSITION_COMPONENT_H

#include <stack>

#include "core/components/box/box_component.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/page_transition/page_transition_element.h"
#include "core/components/page_transition/page_transition_info.h"
#include "core/components/stack/stack_component.h"
#include "core/components/tween/tween_component.h"

namespace OHOS::Ace {

class ACE_EXPORT PageTransitionComponent : public StackComponent {
    DECLARE_ACE_TYPE(PageTransitionComponent, StackComponent);

public:
    PageTransitionComponent()
        : StackComponent(Alignment::TOP_LEFT, StackFit::INHERIT, Overflow::CLIP, std::list<RefPtr<Component>>())
    {}

    ~PageTransitionComponent() override = default;

    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<PageTransitionElement>();
    }

    RefPtr<RenderNode> CreateRenderNode() override;

    // set page background box. set background, width and height in BoxComponent and set it
    void SetBackground(const RefPtr<BoxComponent>& background)
    {
        if (!background) {
            LOGE("background is empty when set back decoration.");
            return;
        }
        background_ = background;
    }

    // get the user's background box
    const RefPtr<BoxComponent>& GetBackground() const
    {
        return background_;
    }

    // set page content component, usually begin with StackComponent
    void SetContent(const RefPtr<Component>& content)
    {
        if (!content) {
            LOGE("content is empty when set content.");
            return;
        }
        content_ = content;
    }

    // get the user's content box
    const RefPtr<Component>& GetContent() const
    {
        return content_;
    }

    void SetContentTransitionOption(const TweenOption& in, const TweenOption& out)
    {
        isSetOption_ = true;
        contentInOption_ = in;
        contentOutOption_ = out;
    }

    const TweenOption& GetContentTransitionInOption() const
    {
        return contentInOption_;
    }

    const TweenOption& GetContentTransitionOutOption() const
    {
        return contentOutOption_;
    }

    // element only support page now
    static bool HasTransitionComponent(const RefPtr<Component>& component)
    {
        // first try with page element.
        RefPtr<PageComponent> pageComponent = AceType::DynamicCast<PageComponent>(component);
        if (!pageComponent) {
            LOGW("Page is null.");
            return false;
        }
        return AceType::InstanceOf<PageTransitionComponent>(pageComponent->GetChild());
    }

    void SetSeparation(bool isSeparate)
    {
        isSeparation_ = isSeparate;
    }

    bool GetSeparation() const
    {
        return isSeparation_;
    }

    bool GetIsSetOption() const
    {
        return isSetOption_;
    }

    void PushPageTransition(const RefPtr<PageTransition>& pageTransition)
    {
        pageTransitionStack_.push(pageTransition);
    }

    void PopPageTransition()
    {
        if (pageTransitionStack_.empty()) {
            return;
        }
        auto pageTransition = pageTransitionStack_.top();
        pageTransitionStack_.pop();
        if (pageTransition) {
            PageTransition::ProcessPageTransitionType(pageTransition);
            pageTransitions_[pageTransition->GetType()] = pageTransition;
        }
    }

    void ClearPageTransitionStack()
    {
        while (!pageTransitionStack_.empty()) {
            auto pageTransition = pageTransitionStack_.top();
            if (pageTransition) {
                PageTransition::ProcessPageTransitionType(pageTransition);
                pageTransitions_[pageTransition->GetType()] = pageTransition;
            }
            pageTransitionStack_.pop();
        }
    }

    void ClearPageTransition()
    {
        ClearPageTransitionStack();
        pageTransitions_.clear();
    }

    RefPtr<PageTransition> GetTopPageTransition()
    {
        if (pageTransitionStack_.empty()) {
            return nullptr;
        }
        return pageTransitionStack_.top();
    }

    const std::unordered_map<PageTransitionType, RefPtr<PageTransition>>& GetPageTransitions() const
    {
        return pageTransitions_;
    }

private:
    bool isSetOption_ = false;
    bool isSeparation_ = false;       // true means that the content of the page is separated from the background
    RefPtr<Component> content_;       // page content component, will be wrapped with animation component
    RefPtr<BoxComponent> background_; // page background box, will be wrapped with animation component
    TweenOption contentInOption_;
    TweenOption contentOutOption_;
    std::unordered_map<PageTransitionType, RefPtr<PageTransition>> pageTransitions_;
    std::stack<RefPtr<PageTransition>> pageTransitionStack_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_TRANSITION_PAGE_TRANSITION_COMPONENT_H
