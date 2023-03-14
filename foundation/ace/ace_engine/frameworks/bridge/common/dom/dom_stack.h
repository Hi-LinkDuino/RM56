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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_STACK_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_STACK_H

#include "base/utils/macros.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/stack/stack_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace::Framework {

class DOMStack final : public DOMNode {
    DECLARE_ACE_TYPE(DOMStack, DOMNode);

public:
    DOMStack(NodeId nodeId, const std::string& nodeName);
    ~DOMStack() override = default;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return stackChild_;
    }

    const RefPtr<ScrollComponent>& GetScrollComponent() const override
    {
        return scroll_;
    }

protected:
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    void PrepareSpecializedComponent() override;
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    void CompositeComponents() override;

private:
    bool GetAxisOffset(const std::pair<std::string, std::string>& style);
    void InitScrollBar();

    enum AxisAlign {
        START = 0,
        CENTER = 1,
        END = 2,
    };

    // Nine patched table
    const Alignment AlignArray[3][3] { { Alignment::TOP_LEFT, Alignment::TOP_CENTER, Alignment::TOP_RIGHT },
        { Alignment::CENTER_LEFT, Alignment::CENTER, Alignment::CENTER_RIGHT },
        { Alignment::BOTTOM_LEFT, Alignment::BOTTOM_CENTER, Alignment::BOTTOM_RIGHT } };

    uint8_t mainAxisAlign_ { 0 };
    uint8_t crossAxisAlign_ { 0 };
    FlexDirection direction_ { FlexDirection::ROW };

    RefPtr<ScrollComponent> scroll_;
    RefPtr<StackComponent> stackChild_;
    StackFit stackFit_ { StackFit::KEEP };
    Alignment alignment_ { Alignment::CENTER };
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_STACK_H
