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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_TABBAR_TAB_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_TABBAR_TAB_TEST_UTILS_H

#include "core/event/key_event.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/render_element.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {
namespace {

const KeyEvent RIGHT_KEY { KeyCode::TV_CONTROL_RIGHT, KeyAction::UP };
const KeyEvent LEFT_KEY { KeyCode::TV_CONTROL_LEFT, KeyAction::UP };
const KeyEvent UP_KEY { KeyCode::TV_CONTROL_UP, KeyAction::UP };
const KeyEvent DOWN_KEY { KeyCode::TV_CONTROL_DOWN, KeyAction::UP };

} // namespace

class MockRenderNode : public RenderNode {
    DECLARE_ACE_TYPE(MockRenderNode, RenderNode);

public:
    explicit MockRenderNode(bool takeBoundary = false) : RenderNode(takeBoundary) {}
    ~MockRenderNode() override = default;

    void PerformLayout() override {}
    void Update(const RefPtr<Component>& component) override {}

    void MockSetParent()
    {
        SetParent(nullptr);
    }
};

class ContainerElement : public FocusGroup, public RenderElement {
    DECLARE_ACE_TYPE(ContainerElement, FocusGroup, RenderElement);

public:
    void Create()
    {
        RefPtr<MockRenderNode> temp = AceType::MakeRefPtr<MockRenderNode>();
        temp->MockSetParent();
        renderNode_ = temp;
    }

    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override
    {
        return GoToNextFocus(reverse);
    }
};

class FocusElement : public BoxElement, public FocusNode {
    DECLARE_ACE_TYPE(FocusElement, BoxElement, FocusNode);
};

class FocusComponet : public BoxComponent {
    DECLARE_ACE_TYPE(FocusComponet, BoxComponent)

public:
    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<FocusElement>();
    };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_TABBAR_TAB_TEST_UTILS_H
