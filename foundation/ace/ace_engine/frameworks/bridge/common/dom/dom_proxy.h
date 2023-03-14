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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_PROXY_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_PROXY_H

#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMProxy : public DOMNode {
    DECLARE_ACE_TYPE(DOMProxy, DOMNode);

public:
    DOMProxy(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName) {}
    ~DOMProxy() override = default;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return nullptr;
    }

    void CompositeComponents() override {}

    void Mount(int32_t slot) override;
    void SetShowAttr(const std::string& showValue) override;
    void ConnectWith(const WeakPtr<DOMNode>& target);

    // proxy node does not need the following actions
    void SetAttr(const std::vector<std::pair<std::string, std::string>>& attrs) override {}
    void SetStyle(const std::vector<std::pair<std::string, std::string>>& styles) override {}
    void AddEvent(int32_t pageId, const std::vector<std::string>& events) override {}
    void UpdateStyleWithChildren() override;
    void SetIsIgnored(bool ignore);

private:
    WeakPtr<DOMNode> targetNode_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_PROXY_H
