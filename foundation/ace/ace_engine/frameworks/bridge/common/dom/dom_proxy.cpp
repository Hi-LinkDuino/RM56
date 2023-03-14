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

#include "frameworks/bridge/common/dom/dom_proxy.h"

namespace OHOS::Ace::Framework {

void DOMProxy::Mount(int32_t slot)
{
    auto parentNode = parentNode_.Upgrade();
    if (!parentNode) {
        return;
    }
    rootComponent_->SetChild(boxComponent_);
    rootComponent_->MarkNeedUpdate();

    parentNode->AddNode(AceType::Claim(this), slot);
    OnMounted(parentNode);
}

void DOMProxy::SetShowAttr(const std::string& showValue)
{
    auto targetNode = targetNode_.Upgrade();
    if (!targetNode) {
        return;
    }
    auto targetDeclaration = targetNode->GetDeclaration();
    if (targetDeclaration) {
        targetDeclaration->SetHasDisplayStyle(true);
    }
    targetNode->SetShowAttr(showValue);
    targetNode->GetRootComponent()->SetUpdateType(UpdateType::ALL);
    targetNode->GenerateComponentNode();
    auto pipeline = pipelineContext_.Upgrade();
    if (pipeline) {
        pipeline->ScheduleUpdate(targetNode->GetRootComponent());
    }
}

void DOMProxy::ConnectWith(const WeakPtr<DOMNode>& target)
{
    targetNode_ = target;
}

void DOMProxy::UpdateStyleWithChildren()
{
    auto target = targetNode_.Upgrade();
    if (!target) {
        return;
    }
    target->UpdateStyleWithChildren();
}

void DOMProxy::SetIsIgnored(bool ignore)
{
    if (boxComponent_) {
        boxComponent_->SetIsIgnored(ignore);
    }
}

} // namespace OHOS::Ace::Framework
