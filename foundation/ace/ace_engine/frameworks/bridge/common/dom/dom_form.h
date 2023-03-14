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

#ifndef FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_FORM_H
#define FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_FORM_H

#include "base/utils/macros.h"
#include "frameworks/bridge/common/dom/dom_input.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMForm : public DOMNode {
    DECLARE_ACE_TYPE(DOMForm, DOMNode);

public:
    DOMForm(NodeId nodeId, const std::string& nodeName);
    ~DOMForm() override = default;

    void Submit();

    void Reset();

    RefPtr<Component> GetSpecializedComponent() override
    {
        return columnContainer_;
    }

    void PrepareSpecializedComponent() override;

    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;

    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;

    void AddInputChild(const WeakPtr<FormValue>& child)
    {
        formValueList_.emplace_back(child);
    }

protected:
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;

private:
    RefPtr<ColumnComponent> columnContainer_;
    std::list<WeakPtr<FormValue>> formValueList_;
    std::function<void(const std::string&)> onSubmit_;
    std::function<void()> onReset_;
    std::string formId_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_FORM_H
