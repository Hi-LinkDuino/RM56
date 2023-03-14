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

#include "frameworks/bridge/common/dom/dom_svg_base.h"

#include "frameworks/bridge/common/dom/dom_svg.h"
#include "frameworks/bridge/common/dom/dom_svg_g.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMSvgBase::DOMSvgBase(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName) {}

void DOMSvgBase::InheritAttrs(const RefPtr<DOMNode>& parentNode)
{
    if (!AceType::InstanceOf<DOMSvg>(parentNode) && !AceType::InstanceOf<DOMSvgG>(parentNode)) {
        LOGE("parentNode is null");
        return;
    }
    auto declaration = AceType::DynamicCast<SvgBaseDeclaration>(declaration_);
    if (!declaration) {
        LOGE("declaration is null");
        return;
    }
    declaration->Inherit(parentNode->GetDeclaration());
}

} // namespace OHOS::Ace::Framework
