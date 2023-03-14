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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SVG_ANIMATION_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SVG_ANIMATION_H

#include "frameworks/bridge/common/dom/dom_svg_animate.h"

namespace OHOS::Ace::Framework {

class ACE_EXPORT DOMSvgAnimateMotion final : public DOMSvgAnimate {
    DECLARE_ACE_TYPE(DOMSvgAnimateMotion, DOMSvgAnimate);

public:
    DOMSvgAnimateMotion(NodeId nodeId, const std::string& nodeName) : DOMSvgAnimate(nodeId, nodeName) {}
    ~DOMSvgAnimateMotion() override = default;

protected:
    void PrepareSpecializedComponent() override;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SVG_ANIMATION_H
