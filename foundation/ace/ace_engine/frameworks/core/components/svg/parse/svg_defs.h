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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_DEFS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_DEFS_H

#include "include/pathops/SkPathOps.h"

#include "frameworks/core/components/svg/parse/svg_mask.h"
#include "frameworks/core/components/svg/parse/svg_pattern.h"
#include "frameworks/core/components/svg/parse/svg_filter.h"

namespace OHOS::Ace {

class SvgDefs : public SvgNode {
    DECLARE_ACE_TYPE(SvgDefs, SvgNode);

public:
    SvgDefs() = default;
    ~SvgDefs() override = default;

    static RefPtr<SvgNode> Create()
    {
        return AceType::MakeRefPtr<SvgDefs>();
    }

    RefPtr<RenderNode> CreateRender(
        const LayoutParam& layoutParam, const RefPtr<SvgBaseDeclaration>& parent, bool useBox = true) override
    {
        auto svgContext = svgContext_.Upgrade();
        if (svgContext == nullptr) {
            return nullptr;
        }
        auto root = svgContext->GetSvgRoot();
        if (root == nullptr) {
            return nullptr;
        }
        for (auto& child : children_) {
            if (AceType::InstanceOf<SvgMask>(child) ||
                AceType::InstanceOf<SvgPattern>(child) ||
                AceType::InstanceOf<SvgFilter>(child)) {
                auto childRender = child->CreateRender(layoutParam, nullptr, useBox);
                if (childRender) {
                    root->AddChild(childRender, root->GetChildren().size());
                    child->Update(childRender);
                }
            }
        }
        return nullptr;
    }

    void AppendChild(const RefPtr<SvgNode>& child) override
    {
        children_.emplace_back(child);
    }

    SkPath AsPath(const Size& viewPort) const override
    {
        SkPath path;
        for (auto child : children_) {
            const SkPath childPath = child->AsPath(viewPort);
            Op(path, childPath, kUnion_SkPathOp, &path);
        }
        return path;
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_DEFS_H