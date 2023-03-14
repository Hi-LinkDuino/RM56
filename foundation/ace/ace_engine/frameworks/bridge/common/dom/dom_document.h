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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_DOCUMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_DOCUMENT_H

#include <string>
#include <unordered_map>

#include "base/memory/ace_type.h"
#include "base/utils/macros.h"
#include "core/components/stack/stack_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/common/dom/dom_proxy.h"

namespace OHOS::Ace::Framework {

// DOMDocument is owned by JsAcePage, to maintain the component structures.
// As the [Component] can't completely one-one correspondence with Ace [Component],
// so the basic idea is:
// Each AcePage corresponding to only one DOMDocument, each DomDocument contains some [Node]s,
// each [Node] maybe corresponding only one Ace [Component], like Text/Image/..., or a
// [ComposedComponent] with some other components.
class ACE_EXPORT DOMDocument final : public virtual AceType {
    DECLARE_ACE_TYPE(DOMDocument, AceType);

public:
    explicit DOMDocument(int32_t pageId) : rootNodeId_(DOM_ROOT_NODE_ID_BASE + pageId) {}
    ~DOMDocument() override;

    RefPtr<DOMNode> CreateNodeWithId(const std::string& tag, NodeId nodeId, int32_t itemIndex = -1);

    RefPtr<DOMProxy> CreateProxyNodeWithId(const std::string& tag, NodeId nodeId);

    RefPtr<DOMNode> GetDOMNodeById(NodeId nodeId) const;

    void RemoveNodes(const RefPtr<DOMNode>& node, bool scheduleUpdate);

    void SetUpRootComponent(const RefPtr<DOMNode>& node);

    RefPtr<Component> GetComponentById(NodeId nodeId) const
    {
        auto domNode = GetDOMNodeById(nodeId);
        return (domNode == nullptr) ? nullptr : domNode->GetRootComponent();
    }

    const RefPtr<StackComponent>& GetRootStackComponent() const
    {
        return rootStackComponent_;
    }

    const RefPtr<ComposedComponent>& GetRootComposedStack() const
    {
        return rootComposedStack_;
    }

    void AddNodeWithId(const std::string& key, const RefPtr<DOMNode>& domNode);
    void AddNodeWithTarget(const std::string& key, const RefPtr<DOMNode>& domNode);
    void HandleComponentPostBinding();
    void HandlePageLoadFinish();

    int32_t GetRootNodeId() const
    {
        return rootNodeId_;
    }

    void SetPipelineContext(const WeakPtr<PipelineContext>& pipelineContext)
    {
        pipelineContext_ = pipelineContext;
    }

    const std::unordered_set<NodeId>& GetProxyRelatedNodes() const
    {
        return proxyRelatedNode_;
    }

private:
    NodeId rootNodeId_ = DOM_ROOT_NODE_ID_BASE;

    RefPtr<StackComponent> rootStackComponent_;
    RefPtr<ComposedComponent> rootComposedStack_;
    std::unordered_map<NodeId, RefPtr<DOMNode>> domNodes_;
    WeakPtr<PipelineContext> pipelineContext_;
    std::unordered_set<NodeId>  proxyRelatedNode_;

    // Below id and target vector are for special cases: popup and label node in Js.
    // Both of them must be assigned a 'target' attribute, which is corresponding to a DOMNode
    // that must be assigned 'id' attribute.
    // And the popup/label component in backend must be as the parent of the component that is
    // included in 'id' DOMNode.
    // However, the sequence(position in JS bundle) of the 'target' and 'id' DOMNode is uncertain,
    // and even not 1 to 1, we need save them and binding the relation after whole page loading done.
    std::vector<std::pair<std::string, WeakPtr<DOMNode>>> nodeWithIdVec_;
    std::vector<std::pair<std::string, WeakPtr<DOMNode>>> nodeWithTargetVec_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_DOCUMENT_H
