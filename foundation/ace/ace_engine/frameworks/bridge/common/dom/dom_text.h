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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TEXT_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TEXT_H

#include "core/components/text/text_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/common/dom/dom_span.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace::Framework {

class DOMText : public DOMNode {
    DECLARE_ACE_TYPE(DOMText, DOMNode);

public:
    DOMText(NodeId nodeId, const std::string& nodeName);
    ~DOMText() override = default;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return textChild_;
    }

protected:
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    void PrepareSpecializedComponent() override;
    void ResetInitializedStyle() override;

    RefPtr<TextComponent> textChild_;

private:
    void CheckAndSetSpanStyle(const RefPtr<DOMSpan>& dmoSpan, TextStyle& spanStyle);
    void SetBoxAlignForText();
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TEXT_H
