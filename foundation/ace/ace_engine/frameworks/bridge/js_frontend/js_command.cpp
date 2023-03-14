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

#include "frameworks/bridge/js_frontend/js_command.h"

#include "base/log/event_report.h"
#include "base/log/log.h"
#include "frameworks/bridge/common/dom/dom_proxy.h"
#include "frameworks/bridge/common/dom/dom_search.h"
#include "frameworks/bridge/common/dom/dom_textarea.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/js_frontend/engine/common/js_engine_loader.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"

namespace OHOS::Ace::Framework {
namespace {

inline RefPtr<DOMNode> GetNodeFromPage(const RefPtr<JsAcePage>& page, NodeId nodeId)
{
    auto domDocument = page ? page->GetDomDocument() : nullptr;
    if (domDocument) {
        return domDocument->GetDOMNodeById(nodeId);
    }
    LOGE("Failed to get DOM document");
    EventReport::SendJsException(JsExcepType::GET_NODE_ERR);
    return nullptr;
}

inline RefPtr<AccessibilityManager> GetAccessibilityManager(const RefPtr<JsAcePage>& page)
{
    if (!page) {
        return nullptr;
    }
    auto pipelineContext = page->GetPipelineContext().Upgrade();
    if (!pipelineContext) {
        return nullptr;
    }
    return pipelineContext->GetAccessibilityManager();
}

inline void TrySaveTargetAndIdNode(const std::string& id, const std::string& target,
    const RefPtr<DOMDocument>& domDocument, const RefPtr<DOMNode>& node)
{
    if (!id.empty()) {
        domDocument->AddNodeWithId(id, node);
    }
    if (!target.empty()) {
        domDocument->AddNodeWithTarget(target, node);
    }
}

std::vector<std::string> g_declarationNodes =
{   DOM_NODE_TAG_BADGE,
    DOM_NODE_TAG_BUTTON,
    DOM_NODE_TAG_LABEL,
    DOM_NODE_TAG_PIECE,
    DOM_NODE_TAG_QRCODE,
    DOM_NODE_TAG_SPAN,
    DOM_NODE_TAG_SWIPER,
    DOM_NODE_TAG_TEXT,
    DOM_NODE_TAG_WEB,
    DOM_NODE_TAG_CLOCK,
    DOM_NODE_TAG_XCOMPONENT
};

} // namespace

void JsCommandDomElementOperator::UpdateForChart(const RefPtr<DOMNode>& node) const
{
    if (chartDatasets_ || chartOptions_ || segments_) {
        auto chart = AceType::DynamicCast<DOMChart>(node);
        if (chart) {
            if (chartDatasets_) {
                chart->SetChartAttrDatasets(*chartDatasets_);
            }
            if (chartOptions_) {
                chart->SetChartAttrOptions(*chartOptions_);
            }
            if (segments_) {
                chart->SetChartAttrSegments(*segments_);
            }
        }
    }
}

void JsCommandDomElementOperator::UpdateForImageAnimator(const RefPtr<DOMNode>& node) const
{
    if (images_) {
        auto imageAnimator = AceType::DynamicCast<DOMImageAnimator>(node);
        if (imageAnimator) {
            imageAnimator->SetImagesAttr(*images_);
        }
    }
}

void JsCommandDomElementOperator::UpdateForClock(const RefPtr<DOMNode>& node) const
{
    if (clockConfig_) {
        auto domClock = AceType::DynamicCast<DOMClock>(node);
        if (domClock) {
            domClock->SetClockConfig(*clockConfig_);
        }
    }
}

void JsCommandDomElementOperator::UpdateForBadge(const RefPtr<DOMNode>& node) const
{
    if (badgeConfig_) {
        auto domBadge = AceType::DynamicCast<DOMBadge>(node);
        if (domBadge) {
            domBadge->SetBadgeConfig(*badgeConfig_);
        }
    }
}

void JsCommandDomElementOperator::UpdateForStepperLabel(const RefPtr<DOMNode>& node) const
{
    if (stepperLabel_) {
        auto domStepperItem = AceType::DynamicCast<DOMStepperItem>(node);
        if (domStepperItem) {
            domStepperItem->SetLabel(*stepperLabel_);
        }
    }
}

void JsCommandDomElementOperator::UpdateForInput(const RefPtr<DOMNode>& node) const
{
    if (!node || !inputOptions_) {
        return;
    }

    auto input = AceType::DynamicCast<DOMInput>(node);
    if (input) {
        input->SetInputOptions(*inputOptions_);
        return;
    }
    auto textarea = AceType::DynamicCast<DOMTextarea>(node);
    if (textarea) {
        textarea->SetInputOptions(*inputOptions_);
        return;
    }
    auto search = AceType::DynamicCast<DOMSearch>(node);
    if (search) {
        search->SetInputOptions(*inputOptions_);
    }
}

RefPtr<DOMNode> JsCommandDomElementCreator::CreateDomNode(const RefPtr<JsAcePage>& page, NodeId parentNodeId) const
{
    if (!page) {
        return nullptr;
    }
    auto pageId = page->GetPageId();
    auto domDocument = page->GetDomDocument();
    ACE_DCHECK(domDocument);
    RefPtr<DOMNode> parentNode;
    if (parentNodeId != -1) {
        parentNode = domDocument->GetDOMNodeById(parentNodeId);
        if (!parentNode) {
            LOGE("Parent node %{private}d not exists", nodeId_);
            EventReport::SendJsException(JsExcepType::CREATE_NODE_ERR);
            return nullptr;
        }
    }

    std::string tagName = tagName_;
    if (parentNode && parentNode->HasSvgTag() && tagName_ == DOM_NODE_TAG_TEXT) {
        // the input tag of ace text and svg text is same.
        tagName = std::string(DOM_NODE_TAG_SVG_TEXT);
    }
    auto node = domDocument->CreateNodeWithId(tagName, nodeId_, itemIndex_);
    if (!node) {
        LOGE("Failed to create DOM node %{public}s", tagName.c_str());
        EventReport::SendJsException(JsExcepType::CREATE_NODE_ERR);
        return nullptr;
    }
    if (page->IsLiteStyle()) {
        node->AdjustParamInLiteMode();
    }

    // supplement: set svg tag by parentNode.hasSvgTag_ or tagName_
    node->SetParentNode(parentNode);

    TrySaveTargetAndIdNode(id_, target_, domDocument, node);
    node->SetShareId(shareId_);
    node->SetPipelineContext(pipelineContext_);
    node->SetIsCustomComponent(isCustomComponent_);
    node->SetBoxWrap(page->IsUseBoxWrap());
    node->InitializeStyle();
    auto declaration = node->GetDeclaration();
    if (declaration) {
        declaration->BindPipelineContext(pipelineContext_);
        declaration->InitializeStyle();
    }
    node->SetAttr(attrs_);

    if (animationStyles_) {
        node->SetAnimationStyle(*animationStyles_);
    }
    if (transitionEnter_) {
        node->SetIsTransition(true);
        node->SetIsEnter(true);
        node->SetAnimationStyle(*transitionEnter_);
    }
    if (transitionExit_) {
        node->SetIsTransition(true);
        node->SetIsEnter(false);
        node->SetAnimationStyle(*transitionExit_);
    }
    if (sharedTransitionName_) {
        node->SetSharedTransitionStyle(*sharedTransitionName_);
    }

    UpdateForChart(node);
    UpdateForImageAnimator(node);
    UpdateForClock(node);
    UpdateForBadge(node);
    UpdateForStepperLabel(node);
    UpdateForInput(node);
    node->SetStyle(styles_);
    node->AddEvent(pageId, events_);
    return node;
}

RefPtr<DOMNode> JsCommandDomElementCreator::CreateDomElement(const RefPtr<JsAcePage>& page) const
{
    if (!page) {
        return nullptr;
    }
    auto pageId = page->GetPageId();
    auto domDocument = page->GetDomDocument();
    ACE_DCHECK(domDocument);

    std::string tagName = tagName_;
    auto node = domDocument->CreateNodeWithId(tagName, nodeId_, -1);
    if (!node) {
        EventReport::SendJsException(JsExcepType::CREATE_NODE_ERR);
        return nullptr;
    }
    if (page->IsLiteStyle()) {
        node->AdjustParamInLiteMode();
    }
    node->SetBoxWrap(page->IsUseBoxWrap());

    TrySaveTargetAndIdNode(id_, target_, domDocument, node);
    node->SetShareId(shareId_);
    node->SetPipelineContext(pipelineContext_);
    node->SetIsCustomComponent(isCustomComponent_);
    node->InitializeStyle();
    node->SetAttr(attrs_);
    if (animationStyles_) {
        node->SetAnimationStyle(*animationStyles_);
    }
    if (transitionEnter_) {
        node->SetIsTransition(true);
        node->SetIsEnter(true);
        node->SetAnimationStyle(*transitionEnter_);
    }
    if (transitionExit_) {
        node->SetIsTransition(true);
        node->SetIsEnter(false);
        node->SetAnimationStyle(*transitionExit_);
    }
    if (sharedTransitionName_) {
        node->SetSharedTransitionStyle(*sharedTransitionName_);
    }

    UpdateForChart(node);
    UpdateForImageAnimator(node);
    UpdateForClock(node);
    UpdateForBadge(node);
    UpdateForStepperLabel(node);
    UpdateForInput(node);

    node->SetStyle(styles_);
    node->AddEvent(pageId, events_);
    return node;
}


void JsCommandDomElementCreator::MountDomNode(
    const RefPtr<DOMNode>& node, const RefPtr<DOMDocument>& domDocument, NodeId parentNodeId) const
{
    if (!node || !domDocument) {
        return;
    }
    // useProxyNode flag is used for the dom node which is out of its previous order.
    // For example, navigation bar is used to set outside the root div, thus there should be a proxy node in
    // its previous order.
    bool useProxyNode = false;
    bool isIgnored = false;
    if (tagName_ == DOM_NODE_TAG_NAVIGATION_BAR) {
        auto rootStack = domDocument->GetRootStackComponent();
        if (rootStack && !rootStack->HasNavigationBar()) {
            node->GenerateComponentNode();
            rootStack->SetNavigationBar(node->GetRootComponent());
            LOGD("added navigation bar node");
            useProxyNode = true;
            isIgnored = true;
        }
    } else if (node->GetPosition() == PositionType::FIXED) {
        const auto& rootStack = domDocument->GetRootStackComponent();
        if (rootStack) {
            rootStack->AppendChild(node->GetRootComponent());
            // mount node to root
            node->Mount(-1);
            ScheduleUpdateForFixedNode(domDocument);
            useProxyNode = true;
        }
    }
    LOGD("parent(=%{private}d) add child(=%{private}d) in slot(=%{private}d)", parentNodeId, nodeId_, itemIndex_);
    if (useProxyNode) {
        // mount proxy dom node to replace the position of original node
        auto proxy = CreateDomProxy(domDocument, parentNodeId);
        if (proxy) {
            proxy->ConnectWith(node);
            proxy->SetIsIgnored(isIgnored);
            proxy->Mount(itemIndex_);
        }
    } else {
        node->Mount(itemIndex_);
    }
}

RefPtr<DOMProxy> JsCommandDomElementCreator::CreateDomProxy(
    const RefPtr<DOMDocument>& domDocument, NodeId parentNodeId) const
{
    RefPtr<DOMNode> parentNode;
    if (parentNodeId != -1) {
        parentNode = domDocument->GetDOMNodeById(parentNodeId);
        if (!parentNode) {
            return nullptr;
        }
    }
    // generate proxy id in DomDocument
    auto proxy = domDocument->CreateProxyNodeWithId(tagName_, nodeId_);
    if (!proxy) {
        return nullptr;
    }

    proxy->SetParentNode(parentNode);
    proxy->SetPipelineContext(pipelineContext_);
    proxy->SetProxyNode(true);
    return proxy;
}

void JsCommandDomElementCreator::ScheduleUpdateForFixedNode(const RefPtr<DOMDocument>& domDocument) const
{
    const auto& rootComposedStack = domDocument->GetRootComposedStack();
    if (rootComposedStack) {
        rootComposedStack->MarkNeedUpdate();
        auto context = pipelineContext_.Upgrade();
        if (context) {
            context->ScheduleUpdate(rootComposedStack);
        }
    }
}

void JsCommandCreateDomBody::Execute(const RefPtr<JsAcePage>& page) const
{
    auto domDocument = page ? page->GetDomDocument() : nullptr;
    if (!domDocument) {
        LOGE("Failed to get DOM document");
        EventReport::SendJsException(JsExcepType::CREATE_DOM_BODY_ERR);
        return;
    }

    auto node = CreateDomNode(page);
    if (!node) {
        return;
    }

    auto transition = node->BuildTransitionComponent();
    page->SetPageTransition(transition);
    node->GenerateComponentNode();
    domDocument->SetPipelineContext(pipelineContext_);
    domDocument->SetUpRootComponent(node);

    if (tagName_ == DOM_NODE_TAG_OPTION) {
        return; // option of menu and select for popup do not need auto creating
    }

    // create root accessibility node
    auto accessibilityManager = GetAccessibilityManager(page);
    if (!accessibilityManager) {
        LOGW("accessibilityManager not exists");
        return;
    }

    accessibilityManager->SetRootNodeId(domDocument->GetRootNodeId());
    auto accessibilityNode = accessibilityManager->CreateAccessibilityNode(tagName_, nodeId_, -1, itemIndex_);
    if (!accessibilityNode) {
        LOGD("Failed to create accessibility node %{public}s", tagName_.c_str());
        return;
    }
    accessibilityManager->TrySaveTargetAndIdNode(id_, target_, accessibilityNode);
    accessibilityNode->SetAttr(attrs_);
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    accessibilityNode->SetStyle(styles_);
#endif
    accessibilityNode->AddEvent(page->GetPageId(), events_);
}

void JsCommandCreateDomElement::Execute(const RefPtr<JsAcePage>& page) const
{
    auto domDocument = page ? page->GetDomDocument() : nullptr;
    if (!domDocument) {
        LOGE("Failed to get DOM document");
        EventReport::SendJsException(JsExcepType::CREATE_NODE_ERR);
        return;
    }
    auto node = CreateDomElement(page);
    if (!node) {
        LOGE("node is nullptr");
        return;
    }
}
void JsCommandAddDomElement::Execute(const RefPtr<JsAcePage>& page) const
{
    auto domDocument = page ? page->GetDomDocument() : nullptr;
    if (!domDocument) {
        LOGE("Failed to get DOM document");
        EventReport::SendJsException(JsExcepType::CREATE_NODE_ERR);
        return;
    }

    auto node = CreateDomNode(page, parentNodeId_);
    if (!node) {
        return;
    }
    MountDomNode(node, domDocument, parentNodeId_);

    if (tagName_ == DOM_NODE_TAG_CANVAS) {
        auto bridge = JsEngineLoader::Get().CreateCanvasBridge();
        page->PushCanvasBridge(nodeId_, bridge);
    }

    if (tagName_ == DOM_NODE_TAG_XCOMPONENT) {
        auto bridge = JsEngineLoader::Get().CreateXComponentBridge();
        page->PushXComponentBridge(nodeId_, bridge);
    }

    page->PushNewNode(nodeId_, parentNodeId_);

    // create other accessibility node
    auto accessibilityManager = GetAccessibilityManager(page);
    if (!accessibilityManager) {
        LOGW("accessibilityManager not exists");
        return;
    }
    if (tagName_ == DOM_NODE_TAG_OPTION) {
        return; // option of menu and select for popup do not need auto creating
    }
    auto accessibilityNode =
        accessibilityManager->CreateAccessibilityNode(tagName_, nodeId_, parentNodeId_, itemIndex_);
    if (!accessibilityNode) {
        LOGD("Failed to create accessibility node %{public}s", tagName_.c_str());
        return;
    }
    accessibilityManager->TrySaveTargetAndIdNode(id_, target_, accessibilityNode);
    accessibilityNode->SetAttr(attrs_);
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    accessibilityNode->SetStyle(styles_);
    if (!animationStyles_) {
        return;
    }
    for (const auto& animationNameKeyframe : *animationStyles_.get()) {
        auto animationName = animationNameKeyframe.find(DOM_ANIMATION_NAME);
        if (animationName != animationNameKeyframe.end()) {
            std::vector<std::pair<std::string, std::string>> vector;
            vector.emplace_back(DOM_ANIMATION_NAME, animationName->second);
            accessibilityNode->SetStyle(vector);
        }
    }
#endif
    accessibilityNode->AddEvent(page->GetPageId(), events_);
}

void JsCommandRemoveDomElement::Execute(const RefPtr<JsAcePage>& page) const
{
    auto domDocument = page ? page->GetDomDocument() : nullptr;
    if (!domDocument) {
        LOGE("Failed to get DOM document");
        EventReport::SendJsException(JsExcepType::REMOVE_DOM_ELEMENT_ERR);
        return;
    }

    auto node = domDocument->GetDOMNodeById(nodeId_);
    if (!node) {
        LOGE("Node %{private}d not exists", nodeId_);
        EventReport::SendJsException(JsExcepType::REMOVE_DOM_ELEMENT_ERR);
        return;
    }

    auto parentNodeId = node->GetParentId();
    domDocument->RemoveNodes(node, true);
    page->PushDirtyNode(parentNodeId);

    // remove accessibility node and it's children
    auto accessibilityManager = GetAccessibilityManager(page);
    if (!accessibilityManager) {
        LOGW("accessibilityManager not exists");
        return;
    }
    auto accessibilityNode = accessibilityManager->GetAccessibilityNodeById(nodeId_);
    if (!accessibilityNode) {
        LOGE("Accessibility Node %{private}d not exists", nodeId_);
        return;
    }
    accessibilityManager->RemoveAccessibilityNodes(accessibilityNode);
}

void JsCommandAppendElement::Execute(const RefPtr<JsAcePage>& page) const
{
    auto domDocument = page ? page->GetDomDocument() : nullptr;
    if (!domDocument) {
        LOGE("Failed to get DOM document");
        EventReport::SendJsException(JsExcepType::CREATE_NODE_ERR);
        return;
    }
    auto node = GetNodeFromPage(page, nodeId_);
    if (!node) {
        LOGE("node is nullptr");
        return;
    }
    RefPtr<DOMNode> parentNode;
    int32_t parentNodeId = parentNodeId_;
    if (parentNodeId != -1) {
        parentNode = domDocument->GetDOMNodeById(parentNodeId);
        if (!parentNode) {
            LOGE("Parent node %{private}d not exists", nodeId_);
            EventReport::SendJsException(JsExcepType::CREATE_NODE_ERR);
        }
    }
    node->SetParentNode(parentNode);

    MountDomNode(node, domDocument, parentNodeId_);
    page->PushNewNode(nodeId_, parentNodeId_);
}

void JsCommandUpdateDomElementAttrs::Execute(const RefPtr<JsAcePage>& page) const
{
    auto node = GetNodeFromPage(page, nodeId_);
    if (!node) {
        LOGE("Node %{private}d not exists", nodeId_);
        EventReport::SendJsException(JsExcepType::UPDATE_DOM_ELEMENT_ERR);
        return;
    }
    if (page->IsLiteStyle()) {
        node->AdjustParamInLiteMode();
    }
    if (page->CheckShowCommandConsumed()) {
        auto showAttr = std::find_if(std::begin(attrs_), std::end(attrs_),
            [](const std::pair<std::string, std::string>& attr) { return attr.first == DOM_SHOW; });
        if (showAttr != std::end(attrs_)) {
            return;
        }
    }
    TrySaveTargetAndIdNode(id_, target_, page->GetDomDocument(), node);
    node->SetBoxWrap(page->IsUseBoxWrap());
    node->SetAttr(attrs_);
    node->SetShareId(shareId_);
    UpdateForChart(node);
    UpdateForImageAnimator(node);
    UpdateForClock(node);
    UpdateForBadge(node);
    UpdateForStepperLabel(node);
    UpdateForInput(node);

    node->GenerateComponentNode();
    page->PushDirtyNode(node->GetDirtyNodeId());

    // update accessibility node
    auto accessibilityManager = GetAccessibilityManager(page);
    if (!accessibilityManager) {
        LOGW("accessibilityManager not exists");
        return;
    }
    auto accessibilityNode = accessibilityManager->GetAccessibilityNodeById(nodeId_);
    if (!accessibilityNode) {
        LOGE("Accessibility Node %{private}d not exists", nodeId_);
        return;
    }
    accessibilityManager->TrySaveTargetAndIdNode(id_, target_, accessibilityNode);
    accessibilityNode->SetAttr(attrs_);
}

void JsCommandUpdateDomElementStyles::Execute(const RefPtr<JsAcePage>& page) const
{
    auto node = GetNodeFromPage(page, nodeId_);
    if (!node) {
        LOGE("Node %{private}d not exists", nodeId_);
        EventReport::SendJsException(JsExcepType::UPDATE_DOM_ELEMENT_ERR);
        return;
    }
    if (animationStyles_) {
        node->SetAnimationStyle(*animationStyles_);
    }
    node->SetStyle(styles_);
    node->GenerateComponentNode();
    page->PushDirtyNode(nodeId_);

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    // update accessibility node
    auto accessibilityManager = GetAccessibilityManager(page);
    if (!accessibilityManager) {
        LOGE("accessibilityManager not exists");
        return;
    }
    auto accessibilityNode = accessibilityManager->GetAccessibilityNodeById(nodeId_);
    if (!accessibilityNode) {
        LOGE("Accessibility Node %{private}d not exists", nodeId_);
        return;
    }
    accessibilityManager->TrySaveTargetAndIdNode(id_, target_, accessibilityNode);
    accessibilityNode->SetStyle(styles_);
#endif
}

void JsCommandCallDomElementMethod::Execute(const RefPtr<JsAcePage>& page) const
{
    auto node = GetNodeFromPage(page, nodeId_);
    if (!node) {
        LOGE("Node %{private}d not exists", nodeId_);
        return;
    }
    if (method_ == DOM_FOCUS) {
        page->UpdateShowAttr();
    }
    auto declaration = node->GetDeclaration();
    if (declaration &&
        std::find(g_declarationNodes.begin(), g_declarationNodes.end(), node->GetTag()) != g_declarationNodes.end()) {
        declaration->CallMethod(method_, param_);
    } else {
        node->CallMethod(method_, param_);
    }
}

void JsCommandContextOperation::Execute(const RefPtr<JsAcePage>& page) const
{
    if (!task_) {
        return;
    }
    auto canvas = AceType::DynamicCast<DOMCanvas>(GetNodeFromPage(page, nodeId_));
    if (!canvas) {
        LOGE("Node %{private}d not exists or not a canvas", nodeId_);
        return;
    }
    auto paintChild = AceType::DynamicCast<CustomPaintComponent>(canvas->GetSpecializedComponent());
    ACE_DCHECK(paintChild);
    auto pool = paintChild->GetTaskPool();
    if (!pool) {
        LOGE("canvas get pool failed");
        return;
    }
    task_(pool);
}

void JsCommandXComponentOperation::Execute(const RefPtr<JsAcePage>& page) const
{
    if (!task_) {
        return;
    }
    auto xcomponent = AceType::DynamicCast<DOMXComponent>(GetNodeFromPage(page, nodeId_));
    if (!xcomponent) {
        LOGE("Node %{private}d not exists or not a xcomponent", nodeId_);
        return;
    }
    auto child = AceType::DynamicCast<XComponentComponent>(xcomponent->GetSpecializedComponent());
    ACE_DCHECK(child);
    auto pool = child->GetTaskPool();
    if (!pool) {
        LOGE("xcomponent get pool failed");
        return;
    }
    task_(pool);
}

void JsCommandAnimation::Execute(const RefPtr<JsAcePage>& page) const
{
    if (!page) {
        LOGE("execute animation command failed. page is null.");
        return;
    }
    if (task_) {
        task_->AnimationBridgeTaskFunc(page, nodeId_);
    }
}

void JsCommandAnimator::Execute(const RefPtr<JsAcePage>& page) const
{
    if (!page) {
        LOGE("execute animation command failed. page is null.");
        return;
    }
    if (task_) {
        task_->AnimatorBridgeTaskFunc(page, bridgeId_);
    }
}

} // namespace OHOS::Ace::Framework
