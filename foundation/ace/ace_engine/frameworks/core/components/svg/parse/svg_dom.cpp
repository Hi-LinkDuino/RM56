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

#include "frameworks/core/components/svg/parse/svg_dom.h"

#include "base/utils/system_properties.h"
#include "frameworks/core/components/box/render_box.h"
#include "frameworks/core/components/svg/flutter_render_svg.h"
#include "frameworks/core/components/svg/parse/svg_animation.h"
#include "frameworks/core/components/svg/parse/svg_circle.h"
#include "frameworks/core/components/svg/parse/svg_clip_path.h"
#include "frameworks/core/components/svg/parse/svg_defs.h"
#include "frameworks/core/components/svg/parse/svg_ellipse.h"
#include "frameworks/core/components/svg/parse/svg_fe_colormatrix.h"
#include "frameworks/core/components/svg/parse/svg_fe_composite.h"
#include "frameworks/core/components/svg/parse/svg_fe_gaussianblur.h"
#include "frameworks/core/components/svg/parse/svg_fe_offset.h"
#include "frameworks/core/components/svg/parse/svg_filter.h"
#include "frameworks/core/components/svg/parse/svg_g.h"
#include "frameworks/core/components/svg/parse/svg_gradient.h"
#include "frameworks/core/components/svg/parse/svg_line.h"
#include "frameworks/core/components/svg/parse/svg_mask.h"
#include "frameworks/core/components/svg/parse/svg_path.h"
#include "frameworks/core/components/svg/parse/svg_pattern.h"
#include "frameworks/core/components/svg/parse/svg_polygon.h"
#include "frameworks/core/components/svg/parse/svg_rect.h"
#include "frameworks/core/components/svg/parse/svg_stop.h"
#include "frameworks/core/components/svg/parse/svg_svg.h"
#include "frameworks/core/components/svg/parse/svg_use.h"
#include "frameworks/core/components/svg/render_svg_base.h"
#include "frameworks/core/components/transform/transform_component.h"

#include <queue>

namespace OHOS::Ace {

namespace {

const char DOM_SVG_STYLE[] = "style";
const char DOM_SVG_CLASS[] = "class";

} // namespace

static const LinearMapNode<RefPtr<SvgNode> (*)()> TAG_FACTORIES[] = {
    { "animate", []() -> RefPtr<SvgNode> { return SvgAnimation::CreateAnimate(); } },
    { "animateMotion", []() -> RefPtr<SvgNode> { return SvgAnimation::CreateAnimateMotion(); } },
    { "animateTransform", []() -> RefPtr<SvgNode> { return SvgAnimation::CreateAnimateTransform(); } },
    { "circle", []() -> RefPtr<SvgNode> { return SvgCircle::Create(); } },
    { "clipPath", []() -> RefPtr<SvgNode> { return SvgClipPath::Create(); } },
    { "defs", []() -> RefPtr<SvgNode> { return SvgDefs::Create(); } },
    { "ellipse", []() -> RefPtr<SvgNode> { return SvgEllipse::Create(); } },
    { "feColorMatrix", []() -> RefPtr<SvgNode> { return SvgFeColorMatrix::Create(); } },
    { "feComposite", []() -> RefPtr<SvgNode> { return SvgFeComposite::Create(); } },
    { "feGaussianBlur", []() -> RefPtr<SvgNode> { return SvgFeGaussianBlur::Create(); } },
    { "feOffset", []() -> RefPtr<SvgNode> { return SvgFeOffset::Create(); } },
    { "filter", []() -> RefPtr<SvgNode> { return SvgFilter::Create(); } },
    { "g", []() -> RefPtr<SvgNode> { return SvgG::Create(); } },
    { "line", []() -> RefPtr<SvgNode> { return SvgLine::Create(); } },
    { "linearGradient", []() -> RefPtr<SvgNode> { return SvgGradient::CreateLinearGradient(); } },
    { "mask", []() -> RefPtr<SvgNode> { return SvgMask::Create(); } },
    { "path", []() -> RefPtr<SvgNode> { return SvgPath::Create(); } },
    { "pattern", []() -> RefPtr<SvgNode> { return SvgPattern::Create(); } },
    { "polygon", []() -> RefPtr<SvgNode> { return SvgPolygon::CreatePolygon(); } },
    { "polyline", []() -> RefPtr<SvgNode> { return SvgPolygon::CreatePolyline(); } },
    { "radialGradient", []() -> RefPtr<SvgNode> { return SvgGradient::CreateRadialGradient(); } },
    { "rect", []() -> RefPtr<SvgNode> { return SvgRect::Create(); } },
    { "stop", []() -> RefPtr<SvgNode> { return SvgStop::Create(); } },
    { "style", []() -> RefPtr<SvgNode> { return SvgStyle::Create(); } },
    { "svg", []() -> RefPtr<SvgNode> { return SvgSvg::Create(); } },
    { "use", []() -> RefPtr<SvgNode> { return SvgUse::Create(); } },
};

SvgDom::SvgDom(const WeakPtr<PipelineContext>& context) : context_(context)
{
    attrCallback_ = [weakSvgDom = AceType::WeakClaim(this)](const std::string& styleName,
                    const std::pair<std::string, std::string>& attrPair) {
        auto svgDom = weakSvgDom.Upgrade();
        if (!svgDom) {
            LOGE("svg dom is null");
            return;
        }
        if (svgDom->svgContext_) {
            svgDom->svgContext_->PushStyle(styleName, attrPair);
        }
    };
}

SvgDom::~SvgDom()
{
    if (animatorGroup_) {
        animatorGroup_->Stop();
    }
    root_ = nullptr;
    renderNode_ = nullptr;
}

RefPtr<SvgDom> SvgDom::CreateSvgDom(SkStream& svgStream, const WeakPtr<PipelineContext>& context,
    const std::optional<Color>& svgThemeColor)
{
    RefPtr<SvgDom> svgDom = AceType::MakeRefPtr<SvgDom>(context);
    if (svgThemeColor) {
        svgDom->fillColor_ = svgThemeColor;
    }
    bool ret = svgDom->ParseSvg(svgStream);
    if (ret) {
        return svgDom;
    }
    return nullptr;
}

bool SvgDom::ParseSvg(SkStream& svgStream)
{
    SkDOM xmlDom;
    if (!xmlDom.build(svgStream)) {
        return false;
    }
    if (svgContext_ == nullptr) {
        svgContext_ = AceType::MakeRefPtr<SvgContext>();
    }
    root_ = TranslateSvgNode(xmlDom, xmlDom.getRootNode(), nullptr);
    if (root_ == nullptr) {
        return false;
    }
    auto svg = AceType::DynamicCast<SvgSvg>(root_);
    if (svg == nullptr) {
        return false;
    }
    svg->MarkIsRoot(true);
    svgSize_ = svg->GetSize();
    return true;
}

RefPtr<SvgNode> SvgDom::TranslateSvgNode(const SkDOM& dom, const SkDOM::Node* xmlNode,
    const RefPtr<SvgNode>& parent)
{
    const char* element = dom.getName(xmlNode);
    if (dom.getType(xmlNode) == SkDOM::kText_Type) {
        if (parent == nullptr) {
            return nullptr;
        }
        if (AceType::InstanceOf<SvgStyle>(parent)) {
            SvgStyle::ParseCssStyle(element, attrCallback_);
        } else {
            parent->SetText(element);
        }
    }

    auto elementIter = BinarySearchFindIndex(TAG_FACTORIES, ArraySize(TAG_FACTORIES), element);
    if (elementIter == -1) {
        return nullptr;
    }
    RefPtr<SvgNode> node = TAG_FACTORIES[elementIter].value();
    if (!node) {
        return nullptr;
    }
    node->SetContext(context_, svgContext_);
    ParseAttrs(dom, xmlNode, node);
    for (auto* child = dom.getFirstChild(xmlNode, nullptr); child; child = dom.getNextSibling(child)) {
        const auto& childNode = TranslateSvgNode(dom, child, node);
        if (childNode) {
            node->AppendChild(childNode);
        }
    }
    return node;
}

void SvgDom::ParseAttrs(const SkDOM& xmlDom, const SkDOM::Node* xmlNode, const RefPtr<SvgNode>& svgNode)
{
    const char *name = nullptr;
    const char *value = nullptr;
    SkDOM::AttrIter attrIter(xmlDom, xmlNode);
    while ((name = attrIter.next(&value))) {
        SetAttrValue(name, value, svgNode);
    }
}

void SvgDom::ParseIdAttr(const WeakPtr<SvgNode>& weakSvgNode, const std::string& value)
{
    auto svgNode = weakSvgNode.Upgrade();
    if (!svgNode) {
        LOGE("ParseIdAttr failed, svgNode is null");
        return;
    }
    svgNode->SetNodeId(value);
    svgNode->SetAttr(DOM_ID, value);
    svgContext_->Push(value, svgNode);
}

void SvgDom::ParseFillAttr(const WeakPtr<SvgNode>& weakSvgNode, const std::string& value)
{
    auto svgNode = weakSvgNode.Upgrade();
    if (!svgNode) {
        LOGE("ParseFillAttr failed, svgNode is null");
        return;
    }
    if (fillColor_) {
        std::stringstream stream;
        stream << std::hex << fillColor_.value().GetValue();
        std::string newValue(stream.str());
        svgNode->SetAttr(DOM_SVG_FILL, "#" + newValue);
    } else {
        svgNode->SetAttr(DOM_SVG_FILL, value);
    }
}

void SvgDom::ParseClassAttr(const WeakPtr<SvgNode>& weakSvgNode, const std::string& value)
{
    auto svgNode = weakSvgNode.Upgrade();
    if (!svgNode) {
        LOGE("ParseClassAttr failed, svgNode is null");
        return;
    }
    std::vector<std::string> styleNameVector;
    StringUtils::SplitStr(value, " ", styleNameVector);
    for (const auto& styleName : styleNameVector) {
        auto attrMap = svgContext_->GetAttrMap(styleName);
        if (attrMap.empty()) {
            continue;
        }
        for (const auto& attr: attrMap) {
            svgNode->SetAttr(attr.first, attr.second);
        }
    }
}

void SvgDom::ParseStyleAttr(const WeakPtr<SvgNode>& weakSvgNode, const std::string& value)
{
    auto svgNode = weakSvgNode.Upgrade();
    if (!svgNode) {
        LOGE("ParseStyleAttr failed, svgNode is null");
        return;
    }
    std::vector<std::string> attrPairVector;
    StringUtils::SplitStr(value, ";", attrPairVector);
    for (const auto& attrPair : attrPairVector) {
        std::vector<std::string> attrVector;
        StringUtils::SplitStr(attrPair, ":", attrVector);
        if (attrVector.size() == 2) {
            svgNode->SetAttr(attrVector[0], attrVector[1]);
        }
    }
}

void SvgDom::SetAttrValue(const std::string& name, const std::string& value, const RefPtr<SvgNode>& svgNode)
{
    static const LinearMapNode<void (*)(const std::string&, const WeakPtr<SvgNode>&, SvgDom&)> attrs[] = {
        { DOM_SVG_CLASS,
            [](const std::string& val, const WeakPtr<SvgNode>& svgNode, SvgDom& svgDom) {
                svgDom.ParseClassAttr(svgNode, val);
            } },
        { DOM_SVG_FILL,
            [](const std::string& val, const WeakPtr<SvgNode>& svgNode, SvgDom& svgDom) {
                svgDom.ParseFillAttr(svgNode, val);
            } },
        { DOM_ID,
            [](const std::string& val, const WeakPtr<SvgNode>& svgNode, SvgDom& svgDom) {
                svgDom.ParseIdAttr(svgNode, val);
            } },
        { DOM_SVG_STYLE,
            [](const std::string& val, const WeakPtr<SvgNode>& svgNode, SvgDom& svgDom) {
                svgDom.ParseStyleAttr(svgNode, val);
            } },
    };
    if (value.empty()) {
        return;
    }
    auto attrIter = BinarySearchFindIndex(attrs, ArraySize(attrs), name.c_str());
    if (attrIter != -1) {
        attrs[attrIter].value(value, svgNode, *this);
        return;
    }
    svgNode->SetAttr(name, value);
}

void SvgDom::PaintDirectly(RenderContext& context, const Offset& offset)
{
    auto svgRoot = AceType::DynamicCast<FlutterRenderSvg>(svgRoot_.Upgrade());
    if (svgRoot) {
        svgRoot->PaintDirectly(context, offset);
    } else {
        LOGD("paint fail as svg root is null");
    }
}

void SvgDom::CreateRenderNode(ImageFit imageFit, const SvgRadius& svgRadius, bool useBox)
{
    auto svg = AceType::DynamicCast<SvgSvg>(root_);
    if (svg == nullptr) {
        return;
    }
    Size size;
    if (svgSize_.IsValid() && !svgSize_.IsInfinite()) {
        size = svgSize_;
    } else {
        size = containerSize_;
    }
    auto renderSvg = svg->CreateRender(LayoutParam(size, Size(0.0, 0.0)), nullptr, useBox);
    if (renderSvg) {
        InitAnimatorGroup(renderSvg);
        double scaleX = 1.0;
        double scaleY = 1.0;
        ApplyImageFit(imageFit, scaleX, scaleY);
        auto transformComponent = AceType::MakeRefPtr<TransformComponent>();
        transformComponent->Scale(scaleX, scaleY);
        auto renderTransform = transformComponent->CreateRenderNode();
        renderTransform->Attach(context_);
        SyncRSNode(renderTransform);
        renderTransform->Update(transformComponent);
        renderTransform->AddChild(renderSvg);
        renderTransform->Layout(LayoutParam(containerSize_, Size(0.0, 0.0)));
        svgRoot_ = renderSvg;
        transform_ = renderTransform;

        auto clipBox = AceType::MakeRefPtr<BoxComponent>();
        clipBox->SetWidth(containerSize_.Width());
        clipBox->SetHeight(containerSize_.Height());
        clipBox->SetOverflow(Overflow::FORCE_CLIP);
        if (svgRadius.IsValid()) {
            RefPtr<Decoration> decoration = AceType::MakeRefPtr<Decoration>();
            Border border;
            border.SetTopLeftRadius(svgRadius.topLeft);
            border.SetTopRightRadius(svgRadius.topRight);
            border.SetBottomLeftRadius(svgRadius.bottomLeft);
            border.SetBottomRightRadius(svgRadius.bottomRight);
            decoration->SetBorder(border);
            clipBox->SetBackDecoration(decoration);
        }
        auto renderBox = clipBox->CreateRenderNode();
        renderBox->Attach(context_);
        SyncRSNode(renderBox);
        renderBox->Update(clipBox);
        renderBox->AddChild(renderTransform);
        renderBox->Layout(LayoutParam(containerSize_, Size(0.0, 0.0)));
        clipBox_ = renderBox;

        auto boxComponent = AceType::MakeRefPtr<BoxComponent>();
        boxComponent->SetWidth(containerSize_.Width());
        boxComponent->SetHeight(containerSize_.Height());
        renderNode_ = boxComponent->CreateRenderNode();
        renderNode_->Attach(context_);
        SyncRSNode(renderNode_);
        renderNode_->Update(boxComponent);
        renderNode_->AddChild(renderBox);
        renderNode_->Layout(LayoutParam(containerSize_, Size(0.0, 0.0)));
    }
}

void SvgDom::SetRootOpacity(int32_t alpha)
{
    auto root = AceType::DynamicCast<FlutterRenderSvg>(svgRoot_.Upgrade());
    if (root) {
        root->SetRootOpacity(alpha);
    }
}

void SvgDom::SetRootRotate(double rotate)
{
    auto root = AceType::DynamicCast<FlutterRenderSvg>(svgRoot_.Upgrade());
    if (root) {
        root->SetRootRotate(rotate);
    }
}

void SvgDom::SetRadius(const SvgRadius& svgRadius)
{
    if (!svgRadius.IsValid()) {
        return;
    }
    auto clipBox = AceType::DynamicCast<RenderBox>(clipBox_.Upgrade());
    if (!clipBox) {
        LOGW("clip box is nullptr");
        return;
    }

    Border border;
    border.SetTopLeftRadius(svgRadius.topLeft);
    border.SetTopRightRadius(svgRadius.topRight);
    border.SetBottomLeftRadius(svgRadius.bottomLeft);
    border.SetBottomRightRadius(svgRadius.bottomRight);
    auto backDecoration = AceType::MakeRefPtr<Decoration>();
    if (backDecoration) {
        backDecoration->SetBorder(border);
        clipBox->SetBackDecoration(backDecoration);
    }
}

void SvgDom::ApplyImageFit(ImageFit imageFit, double& scaleX, double& scaleY)
{
    switch (imageFit) {
        case ImageFit::FILL:
            ApplyFill(scaleX, scaleY);
            break;
        case ImageFit::NONE:
            break;
        case ImageFit::COVER:
            ApplyCover(scaleX, scaleY);
            break;
        case ImageFit::CONTAIN:
            ApplyContain(scaleX, scaleY);
            break;
        case ImageFit::SCALEDOWN:
            if (svgSize_ > containerSize_ || svgSize_ == containerSize_) {
                ApplyContain(scaleX, scaleY);
            }
            break;
        default:
            break;
    }
}

void SvgDom::ApplyFill(double& scaleX, double& scaleY)
{
    if (!svgSize_.IsValid()) {
        return;
    }
    scaleX = containerSize_.Width() / svgSize_.Width();
    scaleY = containerSize_.Height() / svgSize_.Height();
}

void SvgDom::ApplyContain(double& scaleX, double& scaleY)
{
    if (!svgSize_.IsValid()) {
        return;
    }
    if (Size::CalcRatio(svgSize_) > Size::CalcRatio(containerSize_)) {
        scaleX = containerSize_.Width() / svgSize_.Width();
        scaleY = scaleX;
    } else {
        scaleX = containerSize_.Height() / svgSize_.Height();
        scaleY = scaleX;
    }
}

void SvgDom::ApplyCover(double& scaleX, double& scaleY)
{
    if (!svgSize_.IsValid()) {
        return;
    }
    if (Size::CalcRatio(svgSize_) > Size::CalcRatio(containerSize_)) {
        scaleX = containerSize_.Height() / svgSize_.Height();
        scaleY = scaleX;
    } else {
        scaleX = containerSize_.Width() / svgSize_.Width();
        scaleY = scaleX;
    }
}

void SvgDom::InitAnimatorGroup(const RefPtr<RenderNode>& node)
{
    animatorGroup_ = AceType::MakeRefPtr<AnimatorGroup>();
    if (!animatorGroup_) {
        return;
    }
    AddToAnimatorGroup(node, animatorGroup_);
    auto finishEvent = AceAsyncEvent<void()>::Create(finishEvent_, context_);
    if (finishEvent) {
        animatorGroup_->AddStopListener(
            [asyncEvent = finishEvent] { asyncEvent(); });
    }
    animatorGroup_->Play();
}

void SvgDom::AddToAnimatorGroup(const RefPtr<RenderNode>& node, RefPtr<AnimatorGroup>& animatorGroup)
{
    if (!animatorGroup) {
        return;
    }

    std::queue<RefPtr<RenderNode>> queue;
    queue.push(node);
    while (!queue.empty()) {
        auto renderNode = queue.front();
        queue.pop();
        auto svgBase = AceType::DynamicCast<RenderSvgBase>(renderNode);
        if (svgBase) {
            auto animators = svgBase->GetAnimators();
            for (auto& p : animators) {
                animatorGroup->AddAnimator(p.second);
            }
        }
        if (renderNode) {
            auto children = renderNode->GetChildren();
            for (auto& child : children) {
                queue.push(child);
            }
        }
    }
}

void SvgDom::SyncRSNode(const RefPtr<RenderNode>& renderNode)
{
    if (!SystemProperties::GetRosenBackendEnabled() || !renderNode) {
        return;
    }
    renderNode->SyncRSNodeBoundary(true, true);
}

} // namespace OHOS::Ace
