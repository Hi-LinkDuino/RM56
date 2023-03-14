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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_DOM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_DOM_H

#include <unordered_map>
#include <stack>
#include "src/xml/SkDOM.h"
#include "src/xml/SkXMLParser.h"
#include "src/xml/SkXMLWriter.h"

#include "base/log/log.h"
#include "base/memory/ace_type.h"
#include "core/animation/animator_group.h"
#include "frameworks/core/components/svg/parse/svg_context.h"
#include "frameworks/core/components/svg/parse/svg_node.h"
#include "frameworks/core/components/svg/parse/svg_style.h"

namespace OHOS::Ace {

struct SvgRadius {
    Radius topLeft;
    Radius topRight;
    Radius bottomLeft;
    Radius bottomRight;

    bool IsValid() const
    {
        return topLeft.IsValid() || topRight.IsValid() || bottomLeft.IsValid() || bottomRight.IsValid();
    }
};

class SvgDom : public AceType {
    DECLARE_ACE_TYPE(SvgDom, AceType);

public:
    explicit SvgDom(const WeakPtr<PipelineContext>& context);
    ~SvgDom() override;
    static RefPtr<SvgDom> CreateSvgDom(SkStream& svgStream, const WeakPtr<PipelineContext>& context,
        const std::optional<Color>& svgThemeColor);
    bool ParseSvg(SkStream& svgStream);
    void CreateRenderNode(ImageFit imageFit, const SvgRadius& svgRadius, bool useBox = true);
    void PaintDirectly(RenderContext& context, const Offset& offset);

    void SetContainerSize(const Size& size)
    {
        containerSize_ = size;
    }

    const Size& GetContainerSize() const
    {
        return containerSize_;
    }

    RefPtr<RenderNode> GetRootRenderNode() const
    {
        return renderNode_;
    }

    RefPtr<RenderNode> GetRootSvgRenderNode() const
    {
        return svgRoot_.Upgrade();
    }

    void SetFinishEvent(const EventMarker& finishEvent)
    {
        finishEvent_ = finishEvent;
    }

    void SetRootOpacity(int32_t alpha);
    void SetRootRotate(double rotate);
    void SetRadius(const SvgRadius& svgRadius);

private:
    void InitAnimatorGroup(const RefPtr<RenderNode>& node);
    void AddToAnimatorGroup(const RefPtr<RenderNode>& node, RefPtr<AnimatorGroup>& animatorGroup);

    RefPtr<SvgNode> TranslateSvgNode(const SkDOM& dom, const SkDOM::Node* xmlNode, const RefPtr<SvgNode>& parent);
    void ParseAttrs(const SkDOM& xmlDom, const SkDOM::Node* xmlNode, const RefPtr<SvgNode>& svgNode);
    void SetAttrValue(const std::string& name, const std::string& value, const RefPtr<SvgNode>& svgNode);
    void ParseIdAttr(const WeakPtr<SvgNode>& weakSvgNode, const std::string& value);
    void ParseFillAttr(const WeakPtr<SvgNode>& weakSvgNode, const std::string& value);
    void ParseClassAttr(const WeakPtr<SvgNode>& weakSvgNode, const std::string& value);
    void ParseStyleAttr(const WeakPtr<SvgNode>& weakSvgNode, const std::string& value);
    void ApplyImageFit(ImageFit imageFit, double& scaleX, double& scaleY);
    void ApplyFill(double& scaleX, double& scaleY);
    void ApplyContain(double& scaleX, double& scaleY);
    void ApplyCover(double& scaleX, double& scaleY);
    void SyncRSNode(const RefPtr<RenderNode>& renderNode);

    WeakPtr<PipelineContext> context_;
    RefPtr<SvgContext> svgContext_;
    RefPtr<SvgNode> root_;
    RefPtr<RenderNode> renderNode_;
    WeakPtr<RenderNode> svgRoot_;
    WeakPtr<RenderNode> clipBox_;
    WeakPtr<RenderNode> transform_;
    Size containerSize_;
    Size svgSize_;
    RefPtr<AnimatorGroup> animatorGroup_ = nullptr;
    EventMarker finishEvent_;
    std::optional<Color> fillColor_;
    PushAttr attrCallback_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_DOM_H