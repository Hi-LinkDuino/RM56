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

#include "frameworks/bridge/common/dom/dom_xcomponent.h"

#include "base/log/log.h"
#include "core/components/declaration/xcomponent/xcomponent_declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace::Framework {
DOMXComponent::DOMXComponent(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    xComponentChild_ = AceType::MakeRefPtr<XComponentComponent>(nodeName);
    xComponentChild_->SetNodeId(nodeId);
}

void DOMXComponent::PrepareSpecializedComponent()
{
    auto xcomponentDeclaration = AceType::DynamicCast<XComponentDeclaration>(declaration_);
    if (!xcomponentDeclaration) {
        return;
    }

    auto& commonAttr = declaration_->MaybeResetAttribute<CommonAttribute>(AttributeTag::COMMON_ATTR);
    if (commonAttr.IsValid()) {
        xcomponentDeclaration->SetXComponentId(commonAttr.id);
    }

    xComponentChild_->SetDeclaration(xcomponentDeclaration);
}

#ifdef OHOS_STANDARD_SYSTEM
uint64_t DOMXComponent::GetSurfaceId() const
{
    if (!xComponentChild_) {
        LOGE("GetSurfaceId failed, The xComponent is not created.");
        return 0;
    }
    const auto& controller = xComponentChild_->GetXComponentController();
    if (!controller) {
        LOGE("GetSurfaceId failed, controller is null.");
        return 0;
    }
    auto surfaceId = controller->GetSurfaceId();
    return surfaceId;
}
#endif
} // namespace OHOS::Ace::Framework
