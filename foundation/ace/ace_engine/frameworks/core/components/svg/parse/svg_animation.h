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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_ANIMATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_ANIMATION_H

#include "frameworks/core/components/svg/parse/svg_node.h"
#include "frameworks/core/components/svg/svg_animate_component.h"

namespace OHOS::Ace {

class SvgAnimation : public SvgNode {
    DECLARE_ACE_TYPE(SvgAnimation, SvgNode);

public:
    explicit SvgAnimation(SvgAnimateType svgAnimateType);
    ~SvgAnimation() override = default;
    static RefPtr<SvgNode> CreateAnimate();
    static RefPtr<SvgNode> CreateAnimateTransform();
    static RefPtr<SvgNode> CreateAnimateMotion();
    void SetAttr(const std::string& name, const std::string& value) override;

    RefPtr<Component> GetComponent() const override
    {
        return component_;
    }

private:
    RefPtr<SvgAnimateComponent> component_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_ANIMATION_H