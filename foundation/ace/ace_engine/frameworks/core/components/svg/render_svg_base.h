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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_BASE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_BASE_H

#include "frameworks/core/animation/animator.h"
#include "frameworks/core/animation/keyframe_animation.h"
#include "frameworks/core/animation/svg_animate.h"
#include "frameworks/core/components/common/properties/svg_paint_state.h"
#include "frameworks/core/components/declaration/svg/svg_base_declaration.h"
#include "frameworks/core/components/svg/svg_animate_component.h"
#include "frameworks/core/components/svg/svg_transform.h"
#include "frameworks/core/pipeline/base/render_node.h"

namespace OHOS::Ace {

const char ATTR_NAME_WIDTH[] = "width";
const char ATTR_NAME_HEIGHT[] = "height";
const char ATTR_NAME_X[] = "x";
const char ATTR_NAME_Y[] = "y";
const char ATTR_NAME_RX[] = "rx";
const char ATTR_NAME_RY[] = "ry";
const char ATTR_NAME_OPACITY[] = "opacity";

enum class LengthType {
    HORIZONTAL,
    VERTICAL,
    OTHER,
};

class RenderSvgBase : public RenderNode {
    DECLARE_ACE_TYPE(RenderSvgBase, RenderNode);

public:
    ~RenderSvgBase() override;

    virtual void PaintDirectly(RenderContext& context, const Offset& offset);

    virtual void UpdateMotion(const std::string& path, const std::string& rotate, double percent) {}

    virtual bool PrepareSelfAnimation(const RefPtr<SvgAnimate>& SvgAnimate)
    {
        return false;
    }

    // return paint bounds of svg element path
    virtual Rect GetPaintBounds(const Offset& offset)
    {
        return GetPaintRect();
    }

    const FillState& GetFillState() const
    {
        return fillState_;
    }

    const StrokeState GetStrokeState() const
    {
        return strokeState_;
    }

    const SvgTextStyle GetTextStyle() const
    {
        return textStyle_;
    }

    const ClipState GetClipState() const
    {
        return clipState_;
    }

    bool IsSvgNode() const
    {
        return isSvgNode_;
    }

    bool NeedTransform() const
    {
        return (!animateTransformAttrs_.empty() || !transform_.empty());
    }

    const std::unordered_map<std::string, RefPtr<Animator>>& GetAnimators() const
    {
        return animators_;
    }

    void SetSvgRoot(const RefPtr<RenderSvgBase>& svgRoot)
    {
        rootSvgNode_ = svgRoot;
    }

    RefPtr<Component> GetComponentHrefFromRoot(const std::string& id);
    RefPtr<SvgBaseDeclaration> GetDeclarationHrefFromRoot(const std::string& id);
    RefPtr<RenderSvgBase> GetHrefFromRoot(const std::string& id);
    RefPtr<RenderSvgBase> GetMaskFromRoot(const std::string& id);
    RefPtr<RenderSvgBase> GetPatternFromRoot(const std::string& id);
    RefPtr<RenderSvgBase> GetFilterFromRoot(const std::string& id);

    void Inherit(const RefPtr<SvgBaseDeclaration>& parent, const RefPtr<SvgBaseDeclaration>& self);

protected:
    bool PrepareBaseAnimation(const RefPtr<SvgAnimate>& animateComponent);
    template<typename T>
    bool CreatePropertyAnimation(
        const RefPtr<SvgAnimate>& component, const T& originalValue, std::function<void(T value)>&& callback);
    template<typename T>
    bool SetPresentationProperty(const std::string& attrName, const T& val, bool isSelf = true);

    bool SetTransformProperty(
        const std::string& type, const std::vector<float>& from, const std::vector<float>& to, double value);

    double ConvertDimensionToPx(const Dimension& value, double baseValue);

    double ConvertDimensionToPx(const Dimension& value, LengthType type, bool isRoot = false);

    void SetPresentationAttrs(const RefPtr<SvgBaseDeclaration>& baseDeclaration);
    void SetPresentationAttrs(const RefPtr<Component>& component, const RefPtr<SvgBaseDeclaration>& baseDeclaration);

    void PrepareWeightAnimate(const RefPtr<SvgAnimate>& svgAnimate, std::vector<std::string>& valueVector,
        const std::string& originalValue, bool& isBy);

    void PrepareAnimation(const std::list<RefPtr<Component>>& componentChildren);
    bool PreparePropertyAnimation(const RefPtr<SvgAnimate>& svgAnimate);

    std::tuple<const Matrix4, float, float> GetRawTransformInfo();
    const Matrix4 GetTransformMatrix4();
    const Matrix4 UpdateTransformMatrix4();

    void AddComponentHrefToRoot(const std::string& id, const RefPtr<Component>& component);
    void AddDeclarationHrefToRoot(const std::string& id, const RefPtr<SvgBaseDeclaration>& declaration);
    void AddHrefToRoot(const std::string& id, const RefPtr<RenderSvgBase>& node);
    void AddMaskToRoot(const std::string& id, const RefPtr<RenderSvgBase>& mask);
    void AddPatternToRoot(const std::string& id, const RefPtr<RenderSvgBase>& pattern);
    void AddFilterToRoot(const std::string& id, const RefPtr<RenderSvgBase>& filter);
    const Rect GetViewBoxFromRoot();

    void PaintMaskLayer(RenderContext& context, const Offset& svg, const Offset& current);
    void PaintMaskLayer(RenderContext& context, const Offset& svg, const Rect& bounds); // used for tspan on textpath
    void UpdateGradient(FillState& fillState);
    std::pair<Dimension, Dimension> CreateTransformOrigin(const std::string& transformOrigin_) const;
    Offset GetTransformOffset(bool isRoot = false);

    FillState fillState_;
    StrokeState strokeState_;
    SvgTextStyle textStyle_;
    ClipState clipState_;
    std::unordered_map<std::string, RefPtr<Animator>> animators_;
    bool isSvgNode_ = false;
    Size svgViewPort_;
    std::string maskId_;
    std::string filterId_;
    std::string transform_;
    std::pair<Dimension, Dimension> transformOrigin_;
    std::map<std::string, std::vector<float>> transformAttrs_;
    std::map<std::string, std::vector<float>> animateTransformAttrs_;
    std::optional<TransformInfo> transformInfo_ = std::nullopt;
    WeakPtr<RenderSvgBase> rootSvgNode_ = nullptr;
    std::optional<Rect> svgViewBox_ = std::nullopt; // which contains viewBox
    std::string id_;

    virtual void OnNotifyRender()
    {
        MarkNeedRender(true);
    }

private:
    template<typename T>
    void PreparePresentationAnimation(const RefPtr<SvgAnimate>& svgAnimate, const T& originalValue);
    bool PrepareAnimateMotion(const RefPtr<SvgAnimate>& svgAnimate);
    void PrepareTransformAnimation(const RefPtr<SvgAnimate>& svgAnimate, double originalValue);
    void PrepareTransformValueAnimation(const RefPtr<SvgAnimate>& svgAnimate, double originalValue);
    void PrepareTransformFrameAnimation(const RefPtr<SvgAnimate>& svgAnimate, double originalValue);
    template<typename T>
    void ChangeChildInheritValue(const RefPtr<RenderNode>& svgBase, const std::string& attrName, T value);
    bool IsSelfValue(const std::string& attrName);
    bool HasAnimator(const std::string& attrName);
    std::string ParseIdFromUrl(const std::string& url);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_BASE_H
