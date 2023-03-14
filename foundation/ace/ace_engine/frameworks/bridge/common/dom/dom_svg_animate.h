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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SVG_ANIMATE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SVG_ANIMATE_H

#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/core/components/svg/svg_animate_component.h"

namespace OHOS::Ace::Framework {

class ACE_EXPORT DOMSvgAnimate : public DOMNode {
    DECLARE_ACE_TYPE(DOMSvgAnimate, DOMNode);

public:
    DOMSvgAnimate(NodeId nodeId, const std::string& nodeName);
    ~DOMSvgAnimate() override = default;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return animateComponent_;
    }

protected:
    void PrepareSpecializedComponent() override;
    void SetAnimateAttrs();
    CalcMode ConvertCalcMode(const std::string& val) const;

    RefPtr<SvgAnimateComponent> animateComponent_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SVG_ANIMATE_H
