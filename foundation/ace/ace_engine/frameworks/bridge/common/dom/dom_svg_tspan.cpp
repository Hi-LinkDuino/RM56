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

#include "frameworks/bridge/common/dom/dom_svg_tspan.h"

#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/core/components/svg/svg_tspan_component.h"

namespace OHOS::Ace::Framework {

DOMSvgTspan::DOMSvgTspan(NodeId nodeId, const std::string& nodeName) : DOMSvgText(nodeId, nodeName) {}

void DOMSvgTspan::PrepareSpecializedComponent()
{
    if (!textComponent_) {
        textComponent_ = AceType::MakeRefPtr<SvgTspanComponent>();
    }
    DOMSvgText::PrepareSpecializedComponent();
}

} // namespace OHOS::Ace::Framework
