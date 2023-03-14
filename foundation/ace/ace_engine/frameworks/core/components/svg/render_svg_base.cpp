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

#include "frameworks/core/components/svg/render_svg_base.h"

#include "frameworks/core/animation/curve_animation.h"
#include "frameworks/core/components/svg/render_svg.h"
#include "frameworks/core/components/svg/render_svg_mask.h"
#include "frameworks/core/components/transform/render_transform.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t START_VALUE = 0;
constexpr int32_t END_VALUE = 1;
constexpr Dimension TRANSFORM_ORIGIN_DEFAULT = 0.5_pct;

const std::unordered_map<std::string, std::function<Color(RenderSvgBase&)>> COLOR_PROPER_GETTERS = {
    { ATTR_NAME_FILL, [](RenderSvgBase& base) -> Color { return base.GetFillState().GetColor(); } },
    { ATTR_NAME_STROKE, [](RenderSvgBase& base) -> Color { return base.GetStrokeState().GetColor(); } },
};

const std::unordered_map<std::string, std::function<Dimension(RenderSvgBase&)>> DIMENSION_PROPER_GETTERS = {
    { ATTR_NAME_STROKE_WIDTH, [](RenderSvgBase& base) -> Dimension { return base.GetStrokeState().GetLineWidth(); } },
    { ATTR_NAME_FONT_SIZE, [](RenderSvgBase& base) -> Dimension { return base.GetTextStyle().GetFontSize(); } },
};

const std::unordered_map<std::string, std::function<double(RenderSvgBase&)>> DOUBLE_PROPER_GETTERS = {
    { ATTR_NAME_FILL_OPACITY,
        [](RenderSvgBase& base) -> double { return base.GetFillState().GetOpacity().GetValue(); } },
    { ATTR_NAME_STROKE_OPACITY,
        [](RenderSvgBase& base) -> double { return base.GetStrokeState().GetOpacity().GetValue(); } },
    { ATTR_NAME_LETTER_SPACING,
        [](RenderSvgBase& base) -> double { return base.NormalizeToPx(base.GetTextStyle().GetLetterSpacing()); } },
    { ATTR_NAME_MITER_LIMIT, [](RenderSvgBase& base) -> double { return base.GetStrokeState().GetMiterLimit(); } },
    { ATTR_NAME_STROKE_DASHOFFSET,
        [](RenderSvgBase& base) -> double { return base.GetStrokeState().GetLineDash().dashOffset; } },
    { ATTR_NAME_OPACITY, [](RenderSvgBase& base) -> double { return base.GetOpacity() * (1.0 / UINT8_MAX); } },
};

const char SVG_TRANSFORM_ORIGIN_LEFT_BOTTOM[] = "left_bottom";
const char SVG_TRANSFORM_ORIGIN_LEFT_CENTER[] = "left_center";
const char SVG_TRANSFORM_ORIGIN_RIGHT_TOP[] = "right_top";
const char SVG_TRANSFORM_ORIGIN_RIGHT_CENTER[] = "right_center";
const char SVG_TRANSFORM_ORIGIN_RIGHT_BOTTOM[] = "right_bottom";
const char SVG_TRANSFORM_ORIGIN_CENTER_LEFT[] = "center_left";
const char SVG_TRANSFORM_ORIGIN_CENTER_RIGHT[] = "center_right";
const char SVG_TRANSFORM_ORIGIN_CENTER_CENTER[] = "center_center";
const char SVG_TRANSFORM_ORIGIN_CENTER_TOP[] = "center_top";
const char SVG_TRANSFORM_ORIGIN_CENTER_BOTTOM[] = "center_bottom";
const char SVG_TRANSFORM_ORIGIN_TOP_RIGHT[] = "top_right";
const char SVG_TRANSFORM_ORIGIN_TOP_CENTER[] = "top_center";
const char SVG_TRANSFORM_ORIGIN_BOTTOM_LEFT[] = "bottom_left";
const char SVG_TRANSFORM_ORIGIN_BOTTOM_CENTER[] = "bottom_center";
const char SVG_TRANSFORM_ORIGIN_BOTTOM_RIGHT[] = "bottom_right";

} // namespace

uint8_t OpacityDoubleToUint8(double opacity)
{
    return static_cast<uint8_t>(round(opacity * UINT8_MAX));
}

RefPtr<RenderSvg> FindRootSvgNode(RefPtr<RenderNode> parent, WeakPtr<RenderSvgBase>& rootSvgNode)
{
    auto root = AceType::DynamicCast<RenderSvg>(rootSvgNode.Upgrade());
    if (root != nullptr) {
        return root;
    }

    while (parent != nullptr) {
        root = AceType::DynamicCast<RenderSvg>(parent);
        if (root != nullptr && root->IsRoot()) {
            rootSvgNode = root;
            break;
        }
        parent = parent->GetParent().Upgrade();
    }
    return root;
}

Rect FindSvgViewBox(RefPtr<RenderNode> parent)
{
    while (parent != nullptr) {
        auto svg = AceType::DynamicCast<RenderSvg>(parent);
        if (svg != nullptr) {
            const auto& viewBox = svg->GetViewBox();
            if (!NearZero(viewBox.Width()) && !NearZero(viewBox.Height())) {
                return viewBox;
            }
            if (svg->IsRoot()) {
                break;
            }
        }
        parent = parent->GetParent().Upgrade();
    }

    return Rect();
}

RenderSvgBase::~RenderSvgBase()
{
    std::unordered_map<std::string, RefPtr<Animator>>::iterator it;
    for (it = animators_.begin(); it != animators_.end(); ++it) {
        if (!it->second) {
            LOGE("animator is null");
            continue;
        }
        if (!it->second->IsStopped()) {
            it->second->Stop();
        }
        it->second->ClearInterpolators();
    }
    animators_.clear();
}

void RenderSvgBase::PaintDirectly(RenderContext& context, const Offset& offset)
{
    for (const auto& item: GetChildren()) {
        auto child = AceType::DynamicCast<RenderSvgBase>(item);
        if (!child) {
            // find svg base node from box node child
            auto boxChild = item->GetFirstChild();
            while (!child && boxChild) {
                child = AceType::DynamicCast<RenderSvgBase>(boxChild);
                boxChild = boxChild->GetFirstChild();
            }
        }
        if (child) {
            Offset current = offset;
            if (!context.IsIntersectWith(child, current)) {
                continue;
            }
            // directly use matrix4 in PaintDirectly instead of streansform layer
            child->PaintDirectly(context, current);
            child->SetNeedRender(false);
        }
    }
}

double RenderSvgBase::ConvertDimensionToPx(const Dimension& value, double baseValue)
{
    if (value.Unit() == DimensionUnit::PERCENT) {
        return value.Value() * baseValue;
    } else if (value.Unit() == DimensionUnit::PX) {
        return value.Value();
    } else {
        return NormalizeToPx(value);
    }
}

double RenderSvgBase::ConvertDimensionToPx(const Dimension& value, LengthType type, bool isRoot)
{
    switch (value.Unit()) {
        case DimensionUnit::PERCENT: {
            Size viewPort = (svgViewPort_.IsValid() && !isRoot) ? svgViewPort_ : GetLayoutSize();
            if (type == LengthType::HORIZONTAL) {
                return value.Value() * viewPort.Width();
            }
            if (type == LengthType::VERTICAL) {
                return value.Value() * viewPort.Height();
            }
            if (type == LengthType::OTHER) {
                return value.Value() * sqrt(viewPort.Width() * viewPort.Height());
            }
            return 0.0;
        }
        case DimensionUnit::PX:
            return value.Value();
        default:
            return NormalizeToPx(value);
    }
}

static inline bool IsKeyWord(const std::string& value)
{
    static const std::set<std::string> keyWords = {"left", "right", "top", "bottom", "center"};
    return keyWords.find(value) != keyWords.end();
}

static inline bool IsValidHorizontalKeyWord(const std::string& value)
{
    static const std::set<std::string> keyWords = {"left", "right", "center"};
    return keyWords.find(value) != keyWords.end();
}

static inline bool IsValidVerticalKeyWord(const std::string& value)
{
    static const std::set<std::string> keyWords = {"top", "bottom", "center"};
    return keyWords.find(value) != keyWords.end();
}

static std::pair<Dimension, Dimension> FindInKeyWordsMap(const std::string& value)
{
    static const std::map<std::string, std::pair<Dimension, Dimension>> keyWordsMap = {
        { SVG_TRANSFORM_ORIGIN_LEFT_BOTTOM, { 0.0_pct, 1.0_pct } },
        { SVG_TRANSFORM_ORIGIN_LEFT_CENTER, { 0.0_pct, 0.5_pct } },
        { SVG_TRANSFORM_ORIGIN_RIGHT_TOP, { 1.0_pct, 0.0_pct } },
        { SVG_TRANSFORM_ORIGIN_RIGHT_CENTER, { 1.0_pct, 0.5_pct } },
        { SVG_TRANSFORM_ORIGIN_RIGHT_BOTTOM, { 1.0_pct, 1.0_pct } },
        { SVG_TRANSFORM_ORIGIN_CENTER_LEFT, { 0.0_pct, 0.5_pct } },
        { SVG_TRANSFORM_ORIGIN_CENTER_RIGHT, { 1.0_pct, 0.5_pct } },
        { SVG_TRANSFORM_ORIGIN_CENTER_CENTER, { 0.5_pct, 0.5_pct } },
        { SVG_TRANSFORM_ORIGIN_CENTER_TOP, { 0.5_pct, 0.0_pct } },
        { SVG_TRANSFORM_ORIGIN_CENTER_BOTTOM, { 0.5_pct, 1.0_pct } },
        { SVG_TRANSFORM_ORIGIN_TOP_RIGHT, { 1.0_pct, 0.0_pct } },
        { SVG_TRANSFORM_ORIGIN_TOP_CENTER, { 0.5_pct, 0.0_pct } },
        { SVG_TRANSFORM_ORIGIN_BOTTOM_LEFT, { 0.0_pct, 1.0_pct } },
        { SVG_TRANSFORM_ORIGIN_BOTTOM_CENTER, { 0.5_pct, 1.0_pct } },
        { SVG_TRANSFORM_ORIGIN_BOTTOM_RIGHT, { 1.0_pct, 1.0_pct } }
    };

    auto iter = keyWordsMap.find(value);
    if (iter != keyWordsMap.end()) {
        return iter->second;
    } else {
        return std::make_pair(Dimension(), Dimension());
    }
}

std::pair<Dimension, Dimension> RenderSvgBase::CreateTransformOrigin(const std::string& transformOrigin) const
{
    static const std::map<std::string, Dimension> keyMap = {
        {"left", 0.0_pct},
        {"right", 1.0_pct},
        {"center", 0.5_pct},
        {"top", 0.0_pct},
        {"bottom", 1.0_pct}
    };
    Dimension x;
    Dimension y;
    std::vector<std::string> values;
    StringUtils::SplitStr(transformOrigin, " ", values);
    if (values.size() == 2) {
        if (IsKeyWord(values[0]) && IsKeyWord(values[1])) {
            return FindInKeyWordsMap(values[0] + "_" + values[1]);
        } else if (IsValidHorizontalKeyWord(values[0])) {
            x = keyMap.at(values[0]);
            y = StringUtils::StringToDimension(values[1]);
        } else if (IsValidVerticalKeyWord(values[1])) {
            x = StringUtils::StringToDimension(values[0]);
            y = keyMap.at(values[1]);
        } else {
            x = StringUtils::StringToDimension(values[0]);
            y = StringUtils::StringToDimension(values[1]);
        }
    } else if (values.size() == 1) {
        if (IsValidHorizontalKeyWord(values[0])) {
            x = keyMap.at(values[0]);
            y = TRANSFORM_ORIGIN_DEFAULT;
        } else if (IsValidVerticalKeyWord(values[0])) {
            x = TRANSFORM_ORIGIN_DEFAULT;
            y = keyMap.at(values[0]);
        } else {
            x = StringUtils::StringToDimension(values[0]);
            y = TRANSFORM_ORIGIN_DEFAULT;
        }
    }
    return std::make_pair(x, y);
}

Offset RenderSvgBase::GetTransformOffset(bool isRoot)
{
    double x = ConvertDimensionToPx(transformOrigin_.first, LengthType::HORIZONTAL, isRoot);
    double y = ConvertDimensionToPx(transformOrigin_.second, LengthType::VERTICAL, isRoot);
    return Offset(x, y) + GetTransitionGlobalOffset();
}

void RenderSvgBase::SetPresentationAttrs(const RefPtr<SvgBaseDeclaration>& baseDeclaration)
{
    if (baseDeclaration) {
        opacity_ = OpacityDoubleToUint8(baseDeclaration->GetOpacity());
        fillState_ = baseDeclaration->GetFillState();
        strokeState_ = baseDeclaration->GetStrokeState();
        textStyle_ = baseDeclaration->GetSvgTextStyle();
        transform_ = baseDeclaration->GetTransform();
        if (IsSvgNode()) {
            transformAttrs_ = SvgTransform::CreateMap(transform_);
        }
        transformOrigin_ = CreateTransformOrigin(baseDeclaration->GetTransformOrigin());
        maskId_ = ParseIdFromUrl(baseDeclaration->GetMaskId());
        filterId_ = ParseIdFromUrl(baseDeclaration->GetFilterId());
        id_ = baseDeclaration->GetId();
    }
}

void RenderSvgBase::SetPresentationAttrs(
    const RefPtr<Component>& component, const RefPtr<SvgBaseDeclaration>& baseDeclaration)
{
    SetPresentationAttrs(baseDeclaration);
    if (!id_.empty() && component) {
        // href used by svg tag 'use'
        AddComponentHrefToRoot(id_, component);
        if (baseDeclaration) {
            AddDeclarationHrefToRoot(id_, baseDeclaration);
        }
        return;
    }
}

void RenderSvgBase::PrepareTransformAnimation(const RefPtr<SvgAnimate>& svgAnimate, double originalValue)
{
    if (!svgAnimate->GetValues().empty()) {
        PrepareTransformFrameAnimation(svgAnimate, originalValue);
    } else {
        PrepareTransformValueAnimation(svgAnimate, originalValue);
    }
}

void RenderSvgBase::PrepareTransformValueAnimation(const RefPtr<SvgAnimate>& svgAnimate, double originalValue)
{
    std::vector<float> fromVec;
    std::vector<float> toVec;
    std::string type;
    if (!svgAnimate->GetValuesRange(fromVec, toVec, type)) {
        LOGE("invalid animate info of type %{public}s", type.c_str());
        return;
    }

    std::function<void(double)> callback;
    callback = [weak = AceType::WeakClaim(this), type, fromVec, toVec](double value) {
        auto svgBase = weak.Upgrade();
        if (!svgBase) {
            LOGE("svgBase is null");
            return;
        }
        if (!svgBase->SetTransformProperty(type, fromVec, toVec, value)) {
            LOGE("no the property: %{public}s", type.c_str());
            return;
        }
        svgBase->OnNotifyRender();
    };
    CreatePropertyAnimation(svgAnimate, originalValue, std::move(callback));
}

void RenderSvgBase::PrepareTransformFrameAnimation(const RefPtr<SvgAnimate>& svgAnimate, double originalValue)
{
    std::vector<std::vector<float>> frames;
    std::string type;
    if (!svgAnimate->GetFrames(frames, type)) {
        LOGE("invalid animate keys info of type %{public}s", type.c_str());
        return;
    }
    if (frames.size() <= 1) {
        LOGE("invalid frames numbers %{public}s", type.c_str());
        return;
    }

    // set indices instead of frames
    std::vector<std::string> indices;
    uint32_t size = svgAnimate->GetValues().size();
    for (uint32_t i = 0; i < size; i++) {
        indices.emplace_back(std::to_string(i));
    }
    auto instance = AceType::MakeRefPtr<SvgAnimate>();
    svgAnimate->Copy(instance);
    instance->SetValues(indices);

    std::function<void(double)> callback;
    callback = [weak = AceType::WeakClaim(this), type, frames](double value) {
        auto svgBase = weak.Upgrade();
        if (!svgBase) {
            LOGE("svgBase is null");
            return;
        }
        // use index and rate to locate frame and position
        uint32_t index = (uint32_t)value;
        double rate = value - index;
        if (index >= frames.size() - 1) {
            index = frames.size() - 2;
            rate = 1.0;
        }
        if (!svgBase->SetTransformProperty(type, frames[index], frames[index + 1], rate)) {
            LOGE("no the property: %{public}s", type.c_str());
            return;
        }
        svgBase->OnNotifyRender();
    };
    CreatePropertyAnimation(instance, originalValue, std::move(callback));
}

template<typename T>
void RenderSvgBase::PreparePresentationAnimation(const RefPtr<SvgAnimate>& svgAnimate, const T& originalValue)
{
    std::function<void(T)> callback;
    callback = [weakRect = AceType::WeakClaim(this), attrName = svgAnimate->GetAttributeName()](T value) {
        auto svgBase = weakRect.Upgrade();
        if (!svgBase) {
            LOGE("svgBase is null");
            return;
        }
        if (!svgBase->SetPresentationProperty(attrName, value)) {
            LOGE("no the property: %{public}s", attrName.c_str());
            return;
        }

        // notify render node to paint.
        // if tspan has changed, should notify parent node of text or textpath.
        svgBase->OnNotifyRender();

        if (svgBase->IsSvgNode()) {
            svgBase->ChangeChildInheritValue(svgBase, attrName, value);
        }
    };
    CreatePropertyAnimation(svgAnimate, originalValue, std::move(callback));
}

template<typename T>
void RenderSvgBase::ChangeChildInheritValue(const RefPtr<RenderNode>& svgBase, const std::string& attrName, T value)
{
    if (!svgBase) {
        LOGE("ChangeChildInheritValue failed, svgBase is null");
        return;
    }
    auto renderChildren = svgBase->GetChildren();
    for (const auto& item : renderChildren) {
        if (!item->GetVisible()) {
            continue;
        }
        auto child = AceType::DynamicCast<RenderSvgBase>(item);
        if (child && !child->IsSelfValue(attrName) && !child->HasAnimator(attrName)) {
            if (child->SetPresentationProperty(attrName, value, false)) {
                child->MarkNeedRender(true);
            }
        }
        ChangeChildInheritValue(item, attrName, value);
    }
}

bool RenderSvgBase::PrepareBaseAnimation(const RefPtr<SvgAnimate>& svgAnimate)
{
    auto attrName = svgAnimate->GetAttributeName();
    if (COLOR_PROPER_GETTERS.find(attrName) != COLOR_PROPER_GETTERS.end()) {
        Color originalValue = COLOR_PROPER_GETTERS.find(attrName)->second(*this);
        PreparePresentationAnimation(svgAnimate, originalValue);
    } else if (DIMENSION_PROPER_GETTERS.find(attrName) != DIMENSION_PROPER_GETTERS.end()) {
        Dimension originalValue = DIMENSION_PROPER_GETTERS.find(attrName)->second(*this);
        PreparePresentationAnimation(svgAnimate, originalValue);
    } else if (DOUBLE_PROPER_GETTERS.find(attrName) != DOUBLE_PROPER_GETTERS.end()) {
        double originalValue = DOUBLE_PROPER_GETTERS.find(attrName)->second(*this);
        PreparePresentationAnimation(svgAnimate, originalValue);
    } else if (attrName.find(TRANSFORM) != std::string::npos) {
        double originalValue = 0.0;
        PrepareTransformAnimation(svgAnimate, originalValue);
    } else {
        return false;
    }
    return true;
}

template<typename T>
bool RenderSvgBase::CreatePropertyAnimation(
    const RefPtr<SvgAnimate>& svgAnimate, const T& originalValue, std::function<void(T)>&& callback)
{
    if (!svgAnimate) {
        LOGE("create property animation failed, svgAnimate is null");
        return false;
    }
    auto animatorIter = animators_.find(svgAnimate->GetAttributeName());
    if (animatorIter != animators_.end()) {
        if (!animatorIter->second->IsStopped()) {
            animatorIter->second->Stop();
        }
        animatorIter->second->ClearInterpolators();
        auto animator = animatorIter->second;
        if (!svgAnimate->CreatePropertyAnimate(std::move(callback), originalValue, animator)) {
            animators_.erase(animatorIter);
        }
    } else {
        auto animator = AceType::MakeRefPtr<Animator>(context_);
        if (svgAnimate->CreatePropertyAnimate(std::move(callback), originalValue, animator)) {
            animators_.emplace(svgAnimate->GetAttributeName(), animator);
        }
    }
    return true;
}

bool RenderSvgBase::PrepareAnimateMotion(const RefPtr<SvgAnimate>& svgAnimate)
{
    if (!svgAnimate || svgAnimate->GetSvgAnimateType() != SvgAnimateType::MOTION) {
        LOGE("create motion animation failed, svgAnimate is null");
        return false;
    }
    std::function<void(double)> callback;
    callback = [weak = AceType::WeakClaim(this), path = svgAnimate->GetPath(), rotate = svgAnimate->GetRotate()](
                   double value) {
        auto sharp = weak.Upgrade();
        if (!sharp) {
            LOGE("sharp is null");
            return;
        }
        sharp->UpdateMotion(path, rotate, value);
        sharp->MarkNeedRender(true);
    };

    auto animatorIter = animators_.find(ANIMATOR_TYPE_MOTION);
    if (animatorIter != animators_.end()) {
        if (!animatorIter->second->IsStopped()) {
            animatorIter->second->Stop();
        }
        animatorIter->second->ClearInterpolators();
        auto animator = animatorIter->second;
        if (!svgAnimate->CreateMotionAnimate(std::move(callback), animator)) {
            animators_.erase(animatorIter);
        }
    } else {
        auto animator = AceType::MakeRefPtr<Animator>(context_);
        if (svgAnimate->CreateMotionAnimate(std::move(callback), animator)) {
            animators_.emplace(ANIMATOR_TYPE_MOTION, animator);
        }
    }
    return true;
}

void RenderSvgBase::PrepareWeightAnimate(const RefPtr<SvgAnimate>& svgAnimate, std::vector<std::string>& valueVector,
    const std::string& originalValue, bool& isBy)
{
    if (!svgAnimate->GetValues().empty()) {
        valueVector = svgAnimate->GetValues();
        valueVector.insert(valueVector.begin(), originalValue);
        std::vector<std::string> newValues;
        uint32_t size = svgAnimate->GetValues().size();
        for (uint32_t i = 0; i < size; i++) {
            newValues.emplace_back(std::to_string(i));
        }
        svgAnimate->SetValues(newValues);
    } else {
        std::string from = svgAnimate->GetFrom().empty() ? originalValue : svgAnimate->GetFrom();
        if (!svgAnimate->GetTo().empty()) {
            valueVector.emplace_back(from);
            valueVector.emplace_back(svgAnimate->GetTo());
            svgAnimate->SetFrom(std::to_string(START_VALUE));
            svgAnimate->SetTo(std::to_string(END_VALUE));
        } else if (!svgAnimate->GetBy().empty()) {
            valueVector.emplace_back(from);
            valueVector.emplace_back(svgAnimate->GetBy());
            svgAnimate->SetFrom(std::to_string(START_VALUE));
            svgAnimate->SetTo(std::to_string(END_VALUE));
            isBy = true;
        } else {
            if (from == originalValue) {
                return;
            }
            valueVector.emplace_back(originalValue);
            valueVector.emplace_back(from);
            svgAnimate->SetFrom(std::to_string(START_VALUE));
            svgAnimate->SetTo(std::to_string(END_VALUE));
        }
    }
}

template<typename T>
bool RenderSvgBase::SetPresentationProperty(const std::string& attrName, const T& val, bool isSelf)
{
    return false;
}

template<>
bool RenderSvgBase::SetPresentationProperty(const std::string& attrName, const Color& val, bool isSelf)
{
    if (attrName == ATTR_NAME_FILL) {
        fillState_.SetColor(val, isSelf);
    } else if (attrName == ATTR_NAME_STROKE) {
        strokeState_.SetColor(val, isSelf);
    } else {
        return false;
    }
    return true;
}

template<>
bool RenderSvgBase::SetPresentationProperty(const std::string& attrName, const Dimension& val, bool isSelf)
{
    if (attrName == ATTR_NAME_STROKE_WIDTH) {
        strokeState_.SetLineWidth(val, isSelf);
    } else if (attrName == ATTR_NAME_FONT_SIZE) {
        textStyle_.SetFontSize(val, isSelf);
    } else {
        return false;
    }
    return true;
}

template<>
bool RenderSvgBase::SetPresentationProperty(const std::string& attrName, const double& val, bool isSelf)
{
    if (attrName == ATTR_NAME_FILL_OPACITY) {
        fillState_.SetOpacity(val, isSelf);
    } else if (attrName == ATTR_NAME_STROKE_OPACITY) {
        strokeState_.SetOpacity(val, isSelf);
    } else if (attrName == ATTR_NAME_LETTER_SPACING) {
        textStyle_.SetLetterSpacing(Dimension(val), isSelf);
    } else if (attrName == ATTR_NAME_MITER_LIMIT) {
        strokeState_.SetMiterLimit(val, isSelf);
    } else if (attrName == ATTR_NAME_STROKE_DASHOFFSET) {
        strokeState_.SetLineDashOffset(val, isSelf);
    } else if (attrName == ATTR_NAME_OPACITY) {
        opacity_ = static_cast<uint8_t>(round(val * UINT8_MAX));
    } else {
        return false;
    }
    return true;
}

bool RenderSvgBase::SetTransformProperty(
    const std::string& type, const std::vector<float>& from, const std::vector<float>& to, double value)
{
    return SvgTransform::SetProperty(type, from, to, value, animateTransformAttrs_);
}

bool RenderSvgBase::IsSelfValue(const std::string& attrName)
{
    if (attrName == ATTR_NAME_FILL_OPACITY) {
        return fillState_.HasOpacity();
    } else if (attrName == ATTR_NAME_STROKE_OPACITY) {
        return strokeState_.HasOpacity();
    } else if (attrName == ATTR_NAME_LETTER_SPACING) {
        return textStyle_.HasLetterSpacing();
    } else if (attrName == ATTR_NAME_MITER_LIMIT) {
        return strokeState_.HasMiterLimit();
    } else if (attrName == ATTR_NAME_STROKE_DASHOFFSET) {
        return strokeState_.HasDashOffset();
    } else if (attrName == ATTR_NAME_STROKE_WIDTH) {
        return strokeState_.HasLineWidth();
    } else if (attrName == ATTR_NAME_FONT_SIZE) {
        return textStyle_.HasFontSize();
    } else if (attrName == ATTR_NAME_FILL) {
        return fillState_.HasColor();
    } else if (attrName == ATTR_NAME_STROKE) {
        return strokeState_.HasColor();
    } else {
        return true;
    }
}

bool RenderSvgBase::HasAnimator(const std::string& attrName)
{
    return !animators_.empty() && animators_.find(attrName) != animators_.end();
}

void RenderSvgBase::PrepareAnimation(const std::list<RefPtr<Component>>& componentChildren)
{
    for (const auto& childComponent : componentChildren) {
        auto svgAnimate = AceType::DynamicCast<SvgAnimate>(childComponent);
        if (!svgAnimate) {
            LOGE("animateComponent is null");
            continue;
        }
        if (!PrepareAnimateMotion(svgAnimate)) {
            PreparePropertyAnimation(svgAnimate);
        }
    }
}

void RenderSvgBase::AddComponentHrefToRoot(const std::string& id, const RefPtr<Component>& component)
{
    if (!id.empty() && component != nullptr) {
        auto rootSvg = FindRootSvgNode(GetParent().Upgrade(), rootSvgNode_);
        if (rootSvg != nullptr) {
            rootSvg->AddHrefComponent(id, component);
        }
    }
}

void RenderSvgBase::AddDeclarationHrefToRoot(const std::string& id, const RefPtr<SvgBaseDeclaration>& declaration)
{
    if (!id.empty() && declaration != nullptr) {
        auto rootSvg = FindRootSvgNode(GetParent().Upgrade(), rootSvgNode_);
        if (rootSvg != nullptr) {
            rootSvg->AddHrefDeclaration(id, declaration);
        }
    }
}

void RenderSvgBase::AddHrefToRoot(const std::string& id, const RefPtr<RenderSvgBase>& node)
{
    if (!id.empty() && node != nullptr) {
        auto rootSvg = FindRootSvgNode(GetParent().Upgrade(), rootSvgNode_);
        if (rootSvg != nullptr) {
            rootSvg->AddHrefNode(id, node);
        }
    }
}

void RenderSvgBase::AddMaskToRoot(const std::string& id, const RefPtr<RenderSvgBase>& mask)
{
    return AddHrefToRoot(id, mask);
}

void RenderSvgBase::AddPatternToRoot(const std::string& id, const RefPtr<RenderSvgBase>& pattern)
{
    return AddHrefToRoot(id, pattern);
}

void RenderSvgBase::AddFilterToRoot(const std::string& id, const RefPtr<RenderSvgBase>& filter)
{
    return AddHrefToRoot(id, filter);
}

RefPtr<Component> RenderSvgBase::GetComponentHrefFromRoot(const std::string& id)
{
    if (!id.empty()) {
        auto rootSvg = FindRootSvgNode(GetParent().Upgrade(), rootSvgNode_);
        if (rootSvg != nullptr) {
            return rootSvg->GetHrefComponent(id);
        }
    }
    return nullptr;
}

RefPtr<SvgBaseDeclaration> RenderSvgBase::GetDeclarationHrefFromRoot(const std::string& id)
{
    if (!id.empty()) {
        auto rootSvg = FindRootSvgNode(GetParent().Upgrade(), rootSvgNode_);
        if (rootSvg != nullptr) {
            return rootSvg->GetHrefDeclaration(id);
        }
    }
    return nullptr;
}

RefPtr<RenderSvgBase> RenderSvgBase::GetHrefFromRoot(const std::string& id)
{
    if (!id.empty()) {
        auto rootSvg = FindRootSvgNode(GetParent().Upgrade(), rootSvgNode_);
        if (rootSvg != nullptr) {
            return rootSvg->GetHrefNode(id);
        }
    }
    return nullptr;
}

RefPtr<RenderSvgBase> RenderSvgBase::GetMaskFromRoot(const std::string& id)
{
    return GetHrefFromRoot(id);
}

RefPtr<RenderSvgBase> RenderSvgBase::GetPatternFromRoot(const std::string& id)
{
    return GetHrefFromRoot(id);
}

RefPtr<RenderSvgBase> RenderSvgBase::GetFilterFromRoot(const std::string& id)
{
    return GetHrefFromRoot(id);
}

void RenderSvgBase::Inherit(const RefPtr<SvgBaseDeclaration>& parent, const RefPtr<SvgBaseDeclaration>& self)
{
    if (!parent || !self) {
        LOGD("parent or self declaration is null");
        return;
    }
    if (!self->HasOpacity()) {
        if (parent->HasOpacity()) {
            opacity_ = OpacityDoubleToUint8(parent->GetOpacity());
        }
    }
    fillState_.Inherit(parent->GetFillState());
    strokeState_.Inherit(parent->GetStrokeState());
    textStyle_.Inherit(parent->GetSvgTextStyle());
    clipState_.Inherit(parent->GetClipState());
}

const Rect RenderSvgBase::GetViewBoxFromRoot()
{
    if (svgViewBox_ == std::nullopt) {
        svgViewBox_ = FindSvgViewBox(GetParent().Upgrade());
    }

    if (svgViewBox_ != std::nullopt) {
        auto viewBox = svgViewBox_.value();
        if (!NearZero(viewBox.Width()) && !NearZero(viewBox.Height())) {
            return viewBox;
        }
    }

    // Use svg box bounds instead of view box when it is null.
    return GetPaintRect();
}

void RenderSvgBase::PaintMaskLayer(RenderContext& context, const Offset& svg, const Offset& current)
{
    if (!maskId_.empty()) {
        RefPtr<RenderSvgMask> renderMask = AceType::DynamicCast<RenderSvgMask>(GetMaskFromRoot(maskId_));
        if (renderMask != nullptr) {
            renderMask->PaintMaskLayer(context, svg, renderMask->IsDefalutMaskUnits() ?
                GetPaintBounds(current) : GetViewBoxFromRoot());
        }
    }
}

void RenderSvgBase::PaintMaskLayer(RenderContext& context, const Offset& svg, const Rect& bounds)
{
    if (!maskId_.empty()) {
        RefPtr<RenderSvgMask> renderMask = AceType::DynamicCast<RenderSvgMask>(GetMaskFromRoot(maskId_));
        if (renderMask != nullptr) {
            renderMask->PaintMaskLayer(context, svg, renderMask->IsDefalutMaskUnits() ?
                bounds : GetViewBoxFromRoot());
        }
    }
}

bool RenderSvgBase::PreparePropertyAnimation(const RefPtr<SvgAnimate>& svgAnimate)
{
    if (svgAnimate->GetSvgAnimateType() != SvgAnimateType::ANIMATE &&
        svgAnimate->GetSvgAnimateType() != SvgAnimateType::TRANSFORM) {
        return false;
    }
    if (!PrepareSelfAnimation(svgAnimate)) {
        PrepareBaseAnimation(svgAnimate);
    }
    return true;
}

std::tuple<const Matrix4, float, float> RenderSvgBase::GetRawTransformInfo()
{
    transformInfo_ = (!animateTransformAttrs_.empty()) ? SvgTransform::CreateMatrix4(animateTransformAttrs_)
                                                       : SvgTransform::CreateTransformInfo(transform_);
    float pivotX = 0.5;
    float pivotY = 0.5;
    if (transformInfo_->hasRotateCenter && GetLayoutSize().IsValid()) {
        pivotX = transformInfo_->rotateCenter.GetX() / GetLayoutSize().Width();
        pivotY = transformInfo_->rotateCenter.GetY() / GetLayoutSize().Height();
    }
    return {transformInfo_->matrix4, pivotX, pivotY};
}

const Matrix4 RenderSvgBase::GetTransformMatrix4()
{
    transformInfo_ = (!animateTransformAttrs_.empty()) ? SvgTransform::CreateMatrix4(animateTransformAttrs_)
                                                       : SvgTransform::CreateTransformInfo(transform_);
    if (transformInfo_->hasRotateCenter) {
        transformInfo_->matrix4 = RenderTransform::GetTransformByOffset(
            transformInfo_->matrix4, transformInfo_->rotateCenter);
    }
    return RenderTransform::GetTransformByOffset(transformInfo_->matrix4, GetTransformOffset());
}

const Matrix4 RenderSvgBase::UpdateTransformMatrix4()
{
    if (transformInfo_ != std::nullopt) {
        return RenderTransform::GetTransformByOffset(transformInfo_->matrix4, GetTransformOffset());
    } else {
        return Matrix4::CreateIdentity();
    }
}

void RenderSvgBase::UpdateGradient(FillState& fillState)
{
    auto& gradient = fillState.GetGradient();
    if (!gradient) {
        return;
    }
    auto bounds = GetPaintBounds(Offset());
    auto width = bounds.Width();
    auto height = bounds.Height();
    if (gradient->GetType() == GradientType::LINEAR) {
        const auto& linearGradient = gradient->GetLinearGradient();
        auto gradientInfo = LinearGradientInfo();
        auto x1 = linearGradient.x1 ? ConvertDimensionToPx(linearGradient.x1.value(), width) : 0.0;
        gradientInfo.x1 = x1 + bounds.Left();
        auto y1 = linearGradient.y1 ? ConvertDimensionToPx(linearGradient.y1.value(), height) : 0.0;
        gradientInfo.y1 = y1 + bounds.Top();
        auto x2 = ConvertDimensionToPx((linearGradient.x2 ? linearGradient.x2.value() : 1.0_pct), width);
        gradientInfo.x2 = x2 + bounds.Left();
        auto y2 = linearGradient.y2 ? ConvertDimensionToPx(linearGradient.y2.value(), height) : 0.0;
        gradientInfo.y2 = y2 + bounds.Top();
        gradient->SetLinearGradientInfo(gradientInfo);
    }
    if (gradient->GetType() == GradientType::RADIAL) {
        const auto& radialGradient = gradient->GetRadialGradient();
        auto gradientInfo = RadialGradientInfo();
        Dimension radialHorizontalSize = Dimension(
            radialGradient.radialHorizontalSize.value().Value(), radialGradient.radialHorizontalSize.value().Unit());
        gradientInfo.r =
            ConvertDimensionToPx(radialGradient.radialHorizontalSize ? radialHorizontalSize :
                0.5_pct, sqrt(width * height));
        Dimension radialCenterX = Dimension(
            radialGradient.radialCenterX.value().Value(), radialGradient.radialCenterX.value().Unit());
        gradientInfo.cx =
            ConvertDimensionToPx(radialGradient.radialCenterX ? radialCenterX : 0.5_pct, width) + bounds.Left();
        Dimension radialCenterY = Dimension(
            radialGradient.radialCenterY.value().Value(), radialGradient.radialCenterY.value().Unit());
        gradientInfo.cy =
            ConvertDimensionToPx(radialGradient.radialCenterY ? radialCenterY : 0.5_pct, height) + bounds.Top();
        if (radialGradient.fRadialCenterX && radialGradient.fRadialCenterX->IsValid()) {
            gradientInfo.fx = ConvertDimensionToPx(radialGradient.fRadialCenterX.value(), width) + bounds.Left();
        } else {
            gradientInfo.fx = gradientInfo.cx;
        }
        if (radialGradient.fRadialCenterY && radialGradient.fRadialCenterY->IsValid()) {
            gradientInfo.fy = ConvertDimensionToPx(radialGradient.fRadialCenterY.value(), height) + bounds.Top();
        } else {
            gradientInfo.fy = gradientInfo.cy;
        }
        gradient->SetRadialGradientInfo(gradientInfo);
    }
}

std::string RenderSvgBase::ParseIdFromUrl(const std::string& url)
{
    if (url.size() > 6) {
        std::string::size_type start = url.find("url(#");
        if (start != std::string::npos) {
            start += std::strlen("url(#");
            std::string::size_type end = url.find_first_of(')', start);
            if (end != std::string::npos) {
                return url.substr(start, end - start);
            }
        }
    }
    return "";
}

template bool RenderSvgBase::CreatePropertyAnimation(
    const RefPtr<SvgAnimate>& svgAnimate, const Color& originalValue, std::function<void(Color)>&& callback);
template bool RenderSvgBase::CreatePropertyAnimation(
    const RefPtr<SvgAnimate>& svgAnimate, const Dimension& originalValue, std::function<void(Dimension)>&& callback);
template bool RenderSvgBase::CreatePropertyAnimation(
    const RefPtr<SvgAnimate>& svgAnimate, const double& originalValue, std::function<void(double)>&& callback);
template void RenderSvgBase::PreparePresentationAnimation(const RefPtr<SvgAnimate>& svgAnimate, const Dimension& value);
template void RenderSvgBase::PreparePresentationAnimation(const RefPtr<SvgAnimate>& svgAnimate, const Color& value);
template void RenderSvgBase::PreparePresentationAnimation(const RefPtr<SvgAnimate>& svgAnimate, const double& value);

} // namespace OHOS::Ace
