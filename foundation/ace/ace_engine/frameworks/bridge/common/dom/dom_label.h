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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_LABEL_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_LABEL_H

#include "base/utils/label_target.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/component.h"
#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/common/dom/dom_text.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/js_frontend/frontend_delegate_impl.h"

namespace OHOS::Ace::Framework {

class DOMLabel : public DOMText {
    DECLARE_ACE_TYPE(DOMLabel, DOMText);

public:
    DOMLabel(NodeId nodeId, const std::string& nodeName);
    ~DOMLabel();

    void SetTargetNode(const RefPtr<DOMNode>& node)
    {
        auto labelTarget = AceType::DynamicCast<LabelTarget>(node->GetBoxComponent()->GetChild());
        if (!labelTarget) {
            LOGE("node component type is not label target");
            return;
        }
        trigger_ = labelTarget->GetTrigger();
    }

    RefPtr<LabelTrigger> GetLabelTrigger() const
    {
        return trigger_;
    }

    std::function<void(const std::string&)> GetEvent()
    {
        return event_;
    }

    RefPtr<Component> GetSpecializedComponent() override;

protected:
    void PrepareSpecializedComponent() override;

private:
    // click event
    std::list<RefPtr<DOMNode>> children_;
    RefPtr<TouchListenerComponent> labelComponent_;
    RefPtr<LabelTrigger> trigger_;
    std::function<void(const std::string&)> event_;
    EventMarker clickMarker_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_LABEL_H
