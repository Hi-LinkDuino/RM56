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

#include "frameworks/bridge/js_frontend/js_ace_page.h"

#include "base/utils/system_properties.h"
#include "core/components/focus_collaboration/focus_collaboration_component.h"
#include "core/components/page/page_component.h"
#include "core/components/page_transition/page_transition_component.h"

namespace OHOS::Ace::Framework {

JsAcePage::~JsAcePage()
{
    LOG_DESTROY();
    auto pipelineContext = pipelineContext_.Upgrade();
    if (!pipelineContext) {
        return;
    }

    auto accessibilityManager = pipelineContext->GetAccessibilityManager();

    auto taskExecutor = pipelineContext->GetTaskExecutor();
    if (!taskExecutor) {
        LOGE("taskExecutor not exists");
        return;
    }

    RefPtr<DOMDocument> domDoc;
    domDoc.Swap(domDoc_);
    auto weakDom = AceType::WeakClaim(AceType::RawPtr(domDoc));
    auto weakAcc = AceType::WeakClaim(AceType::RawPtr(accessibilityManager));
    taskExecutor->PostTask([weakDom, weakAcc] {
        auto domDoc = weakDom.Upgrade();
        auto accessibilityManager = weakAcc.Upgrade();
        if (domDoc && accessibilityManager) {
            accessibilityManager->ClearPageAccessibilityNodes(domDoc->GetRootNodeId());
        }
    }, TaskExecutor::TaskType::UI);

    // Release Dom and Components in UI thread
    RefPtr<PageTransitionComponent> pageTransition;
    pageTransition.Swap(pageTransition_);
    RefPtr<Component> component;
    component.Swap(component_);
    std::shared_ptr<JsPageRadioGroups> radioGroups;
    radioGroups.swap(radioGroups_);

    taskExecutor->PostSyncTask([&domDoc, &pageTransition, &component, &radioGroups]() {
        LOGI("release Dom and Components on UI thread");
        domDoc.Reset();
        pageTransition.Reset();
        component.Reset();
        radioGroups.reset();
    }, TaskExecutor::TaskType::UI);
}

RefPtr<PageComponent> JsAcePage::BuildPage(const std::string& url)
{
    CHECK_RUN_ON(UI);
    auto pageId = GetPageId();
    auto rootStack = domDoc_->GetRootStackComponent();
    auto rootComposedStack = domDoc_->GetRootComposedStack();
    auto focusCollaboration = AceType::MakeRefPtr<FocusCollaborationComponent>(true);

    if (container_.Upgrade()) {
        if (component_) {
            return AceType::MakeRefPtr<PageComponent>(pageId, url, component_);
        } else if (rootComposedStack) {
            return AceType::MakeRefPtr<PageComponent>(pageId, url, rootComposedStack);
        }
    }
    if (!pageTransition_) {
        pageTransition_ = AceType::MakeRefPtr<PageTransitionComponent>();
    }
    if ((!rootStack || !rootComposedStack) && !component_) {
        LOGW("Page[%{public}d] can't be loaded. no root component.", pageId);
        pageTransition_->SetContent(nullptr);
    } else {
        if (component_) {
            focusCollaboration->InsertChild(0, component_);
        } else if (rootComposedStack) {
            focusCollaboration->InsertChild(0, rootComposedStack);
        }
        pageTransition_->SetContent(focusCollaboration);
        if ((SystemProperties::GetDeviceType() == DeviceType::TV) && (!pageTransition_->GetIsSetOption())) {
            pageTransition_->SetSeparation(true);
            SwapBackgroundDecoration(pageTransition_);
        }
    }
    bool isDeclarative = false;
    auto context = pipelineContext_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        isDeclarative = true;
    }
    const std::string& cardComposeId = GetCardId();
    if (!cardComposeId.empty()) {
        return AceType::MakeRefPtr<PageComponent>(
            pageId, url, cardComposeId, isDeclarative ? std::move(pageTransition_) : pageTransition_);
    }
    return AceType::MakeRefPtr<PageComponent>(
        pageId, url, isDeclarative ? std::move(pageTransition_) : pageTransition_);
}

std::string JsAcePage::GetCardId() const
{
    std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(pageParams_);
    if (argsValue && argsValue->IsObject()) {
        // support old JSON structure as { "ref": value}
        if (!argsValue->GetString(DOM_TRANSITION_CARD_COMPOSEID).empty()) {
            return argsValue->GetString(DOM_TRANSITION_CARD_COMPOSEID);
        }

        // support new JSON structure as { "paramsData": { "ref": value } }
        const auto& paramsData = argsValue->GetObject(DOM_TRANSITION_CARD_PARAMS);
        if (paramsData->IsObject() && !paramsData->GetString(DOM_TRANSITION_CARD_COMPOSEID).empty()) {
            return paramsData->GetString(DOM_TRANSITION_CARD_COMPOSEID);
        }
    }
    return "";
}

RefPtr<ComposedComponent> JsAcePage::BuildPagePatch(int32_t nodeId)
{
    CHECK_RUN_ON(UI);
    RefPtr<Component> dirtyComponent = domDoc_->GetComponentById(nodeId);
    if (!dirtyComponent) {
        LOGE("Node[%{public}d] can't be reached.", nodeId);
        return nullptr;
    }

    auto composedComponent = AceType::DynamicCast<ComposedComponent>(dirtyComponent);
    ACE_DCHECK(composedComponent);
    return composedComponent;
}

void JsAcePage::SwapBackgroundDecoration(const RefPtr<PageTransitionComponent>& transition)
{
    CHECK_RUN_ON(UI);
    if (!transition) {
        LOGW("swap background decoration failed. transition is null.");
        return;
    }

    auto rootNode = domDoc_->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE + GetPageId());
    if (!rootNode) {
        LOGW("swap background decoration failed. root node is null.");
        return;
    }

    auto box = rootNode->GetBoxComponent();
    if (!box) {
        LOGW("swap background decoration failed. box is null.");
        return;
    }

    auto decoration = box->GetBackDecoration();
    if (!decoration) {
        LOGW("swap background decoration failed. decoration is null.");
        return;
    }

    auto backgroundBox = AceType::MakeRefPtr<BoxComponent>();
    backgroundBox->SetBackDecoration(decoration);
    backgroundBox->SetWidth(box->GetWidthDimension().Value(), box->GetWidthDimension().Unit());
    backgroundBox->SetHeight(box->GetHeightDimension().Value(), box->GetHeightDimension().Unit());
    backgroundBox->SetFlex(BoxFlex::FLEX_XY);
    transition->SetBackground(backgroundBox);
    box->SetBackDecoration(nullptr);
}

RefPtr<BaseCanvasBridge> JsAcePage::GetBridgeById(NodeId nodeId)
{
    std::unique_lock<std::mutex> lock(bridgeMutex_);
    auto iter = canvasBridges_.find(nodeId);
    if (iter == canvasBridges_.end()) {
        LOGE("the canvas is not in the map");
        return nullptr;
    }
    return iter->second;
}

RefPtr<BaseCanvasBridge> JsAcePage::GetOffscreenCanvasBridgeById(int32_t bridgeId)
{
    auto iter = offscreenCanvasBridges_.find(bridgeId);
    if (iter == offscreenCanvasBridges_.end()) {
        LOGE("the canvas is not in the map");
        return nullptr;
    }
    return iter->second;
}

RefPtr<BaseXComponentBridge> JsAcePage::GetXComponentBridgeById(NodeId nodeId)
{
    auto iter = xcomponentBridges_.find(nodeId);
    if (iter == xcomponentBridges_.end()) {
        LOGE("the XComponent is not in the map");
        return nullptr;
    }
    return iter->second;
}

RefPtr<BaseAnimationBridge> JsAcePage::GetAnimationBridge(NodeId nodeId)
{
    std::unique_lock<std::mutex> lock(bridgeMutex_);
    auto bridge = animationBridges_.find(nodeId);
    if (bridge == animationBridges_.end()) {
        LOGW("the animation bridge is not in the map, nodeId: %{public}d", nodeId);
        return nullptr;
    }
    return bridge->second;
}

void JsAcePage::RemoveAnimationBridge(NodeId nodeId)
{
    RefPtr<BaseAnimationBridge> bridge;
    {
        std::unique_lock<std::mutex> lock(bridgeMutex_);
        auto pos = animationBridges_.find(nodeId);
        if (pos != animationBridges_.end()) {
            bridge.Swap(pos->second);
            animationBridges_.erase(pos);
        }
    }

    if (bridge) {
        auto pipelineContext = pipelineContext_.Upgrade();
        if (!pipelineContext) {
            LOGE("pipelineContext is nullptr");
            return;
        }
        auto taskExecutor = pipelineContext->GetTaskExecutor();
        if (!taskExecutor) {
            LOGE("taskExecutor is nullptr");
            return;
        }
        taskExecutor->PostSyncTask([&bridge]() { bridge.Reset(); }, TaskExecutor::TaskType::JS);
    }
}

void JsAcePage::AddAnimationBridge(NodeId nodeId, const RefPtr<BaseAnimationBridge>& animationBridge)
{
    if (!animationBridge) {
        LOGE("AddAnimationBridge failed. Animation bridge is null.");
        return;
    }
    std::unique_lock<std::mutex> lock(bridgeMutex_);
    animationBridges_[nodeId] = animationBridge;
}

void JsAcePage::AddAnimatorBridge(int32_t bridgeId, const RefPtr<BaseAnimationBridge>& animatorBridge)
{
    if (!animatorBridge) {
        LOGE("AddAnimationBridge failed. Animation bridge is null.");
        return;
    }
    auto animator = animatorBridge->JsGetAnimator();
    if (!animator) {
        LOGE("animator is null");
        return;
    }
    animator->AttachScheduler(pipelineContext_);
    std::unique_lock<std::mutex> lock(bridgeMutex_);
    animatorBridges_[bridgeId] = animatorBridge;
}

void JsAcePage::RemoveAnimatorBridge(int32_t bridgeId)
{
    std::unique_lock<std::mutex> lock(bridgeMutex_);
    animatorBridges_.erase(bridgeId);
}

RefPtr<BaseAnimationBridge> JsAcePage::GetAnimatorBridge(int32_t bridgeId)
{
    std::unique_lock<std::mutex> lock(bridgeMutex_);
    auto bridge = animatorBridges_.find(bridgeId);
    if (bridge == animatorBridges_.end()) {
        LOGW("the animation bridge is not in the map, nodeId: %{public}d", bridgeId);
        return nullptr;
    }
    return bridge->second;
}

RefPtr<Curve> JsAcePage::GetCurve(const std::string& curveString)
{
    CHECK_RUN_ON(JS);
    auto curveIter = curves_.find(curveString);
    if (curveIter == curves_.end()) {
        LOGW("the animation curve is not in the map");
        return nullptr;
    }
    return curveIter->second;
}

void JsAcePage::RemoveCurve(const std::string& curveString)
{
    CHECK_RUN_ON(JS);
    curves_.erase(curveString);
}

void JsAcePage::AddCurve(const std::string& curveString, const RefPtr<Curve>& curve)
{
    CHECK_RUN_ON(JS);
    if (!curve) {
        LOGE("AddCurve failed. Animation curve is null.");
        return;
    }
    curves_[curveString] = curve;
}

void JsAcePage::AddAnimatorInfo(const std::string animatorId, const RefPtr<AnimatorInfo>& animatorInfo)
{
    if (!animatorInfo) {
        LOGE("AddAnimation failed. Animation is null.");
        return;
    }
    auto animator = animatorInfo->GetAnimator();
    if (!animator) {
        LOGE("animator is null");
        return;
    }
    animator->AttachScheduler(pipelineContext_);
    animatorInfos_[animatorId] = animatorInfo;
}

void JsAcePage::RemoveAnimatorInfo(const std::string& animatorId)
{
    animatorInfos_.erase(animatorId);
}

RefPtr<AnimatorInfo> JsAcePage::GetAnimatorInfo(const std::string& animatorId)
{
    auto bridge = animatorInfos_.find(animatorId);
    if (bridge == animatorInfos_.end()) {
        LOGW("the animation bridge is not in the map, animatorId: %{public}s", animatorId.c_str());
        return nullptr;
    }
    return bridge->second;
}

void JsAcePage::PushCanvasBridge(NodeId nodeId, const RefPtr<BaseCanvasBridge>& bridge)
{
    if (!bridge) {
        LOGE("PushCanvasBridge failed. Canvas bridge is null.");
        return;
    }
    std::unique_lock<std::mutex> lock(bridgeMutex_);
    canvasBridges_[nodeId] = bridge;
}

void JsAcePage::PushOffscreenCanvasBridge(int32_t bridgeId, const RefPtr<BaseCanvasBridge>& bridge)
{
    offscreenCanvasBridges_[bridgeId] = bridge;
}

void JsAcePage::PushXComponentBridge(NodeId nodeId, const RefPtr<BaseXComponentBridge>& bridge)
{
    if (!bridge) {
        LOGE("PushXComponentBridge failed. XComponent bridge is null.");
        return;
    }
    xcomponentBridges_[nodeId] = bridge;
}

void JsAcePage::AddNodeEvent(int32_t nodeId, const std::string& actionType, const std::string& eventAction)
{
    std::unique_lock<std::mutex> lock(eventMutex_);
    nodeEvent_[nodeId][actionType] = eventAction;
}

std::string JsAcePage::GetNodeEventAction(int32_t nodeId, const std::string& actionType)
{
    // in error case just use empty string.
    std::unique_lock<std::mutex> lock(eventMutex_);
    return nodeEvent_[nodeId][actionType];
}

std::shared_ptr<JsPageRadioGroups> JsAcePage::GetRadioGroups()
{
    return radioGroups_;
}

void JsAcePage::OnJsEngineDestroy()
{
    std::unique_lock<std::mutex> lock(bridgeMutex_);
    for (auto&& [id, bridge] : animationBridges_) {
        if (bridge) {
            bridge->OnJsEngineDestroy();
        }
    }
    for (auto&& [id, bridge] : canvasBridges_) {
        if (bridge) {
            bridge->OnJsEngineDestroy();
        }
    }
    for (auto&& [id, bridge] : xcomponentBridges_) {
        if (bridge) {
            bridge->OnJsEngineDestroy();
        }
    }
    for (auto&& [id, bridge] : animatorBridges_) {
        if (bridge) {
            bridge->OnJsEngineDestroy();
        }
    }
    for (auto&& [id, info] : animatorInfos_) {
        if (info) {
            info->OnJsEngineDestroy();
        }
    }
}

} // namespace OHOS::Ace::Framework
