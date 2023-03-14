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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_SVG_PAINT_STATE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_SVG_PAINT_STATE_H

#include "base/memory/ace_type.h"
#include "core/components/common/properties/animatable_double.h"
#include "frameworks/core/components/common/layout/constants.h"
#include "frameworks/core/components/common/properties/color.h"
#include "frameworks/core/components/common/properties/decoration.h"
#include "frameworks/core/components/common/properties/paint_state.h"
#include "frameworks/core/components/common/properties/text_style.h"

namespace OHOS::Ace {

const char ATTR_NAME_FILL[] = "fill";
const char ATTR_NAME_STROKE[] = "stroke";
const char ATTR_NAME_STROKE_WIDTH[] = "stroke-width";
const char ATTR_NAME_MITER_LIMIT[] = "stroke-miterlimit";
const char ATTR_NAME_STROKE_DASHOFFSET[] = "stroke-dashoffset";
const char ATTR_NAME_FONT_SIZE[] = "font-size";
const char ATTR_NAME_FILL_OPACITY[] = "fill-opacity";
const char ATTR_NAME_STROKE_OPACITY[] = "stroke-opacity";
const char ATTR_NAME_LETTER_SPACING[] = "letter-spacing";
const char ANIMATOR_TYPE_MOTION[] = "motion";
const char ATTR_NAME_FILL_RULE_EVENODD[] = "evenodd";

class FillState {
public:
    void SetContextAndCallback(const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
    {
        color_.SetContextAndCallback(context, callback);
        opacity_.SetContextAndCallback(context, callback);
    }

    const AnimatableColor& GetColor() const
    {
        return color_;
    }

    /**
     * set fill color
     * @param color
     * @param isSelf if false the color value inherited from the parent node, otherwise the value is setted by self
     */
    void SetColor(const Color& color, bool isSelf = true, const AnimationOption& option = AnimationOption())
    {
        color_ = AnimatableColor(color, option);
        hasColor_ = isSelf;
    }

    std::optional<Gradient>& GetGradient()
    {
        return gradient_;
    }

    const std::optional<Gradient>& GetGradient() const
    {
        return gradient_;
    }

    void SetGradient(const Gradient& gradient, bool isSelf = true)
    {
        gradient_ = std::make_optional(gradient);
        hasGradient_ = isSelf;
    }

    void SetOpacity(double opacity, bool isSelf = true, const AnimationOption& option = AnimationOption())
    {
        opacity_ = AnimatableDouble(opacity, option);
        hasOpacity_ = isSelf;
    }

    AnimatableDouble GetOpacity() const
    {
        return opacity_;
    }

    void SetFillRule(const std::string& fillRule, bool isSelf = true)
    {
        fillRule_ = fillRule;
        hasFillRule_ = isSelf;
    }

    const std::string& GetFillRule() const
    {
        return fillRule_;
    }

    bool IsEvenodd() const
    {
        return fillRule_ == ATTR_NAME_FILL_RULE_EVENODD;
    }

    void Inherit(const FillState& parent)
    {
        if (!hasColor_) {
            color_ = parent.GetColor();
        }
        if (!hasOpacity_) {
            opacity_ = parent.GetOpacity();
        }
        if (!hasFillRule_) {
            fillRule_ = parent.GetFillRule();
        }
        if (!hasGradient_) {
            gradient_ = parent.GetGradient();
        }
    }

    bool HasColor() const
    {
        return hasColor_;
    }

    bool HasOpacity() const
    {
        return hasOpacity_;
    }

    void SetHref(const std::string& href)
    {
        href_ = href;
    }

    const std::string& GetHref() const
    {
        return href_;
    }

protected:
    AnimatableColor color_ = AnimatableColor(Color::BLACK);
    AnimatableDouble opacity_ = AnimatableDouble(1.0);
    std::string fillRule_;
    std::optional<Gradient> gradient_;
    bool hasColor_ = false;
    bool hasOpacity_ = false;
    bool hasFillRule_ = false;
    bool hasGradient_ = false;
    std::string href_;
};

class StrokeState {
public:
    void SetContextAndCallback(const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
    {
        lineWidth_.SetContextAndCallback(context, callback);
        color_.SetContextAndCallback(context, callback);
        opacity_.SetContextAndCallback(context, callback);
        strokeDashOffset_.SetContextAndCallback(context, callback);
    }

    const AnimatableColor& GetColor() const
    {
        return color_;
    }

    void SetColor(const Color& color, bool isSelf = true, const AnimationOption& option = AnimationOption())
    {
        color_ = AnimatableColor(color, option);
        hasColor_ = isSelf;
    }

    void SetOpacity(double opacity, bool isSelf = true, const AnimationOption& option = AnimationOption())
    {
        opacity_ = AnimatableDouble(opacity, option);
        hasOpacity_ = isSelf;
    }

    AnimatableDouble GetOpacity() const
    {
        return opacity_;
    }

    LineCapStyle GetLineCap() const
    {
        return lineCap_;
    }

    void SetLineCap(LineCapStyle lineCap, bool isSelf = true)
    {
        lineCap_ = lineCap;
        hasLineCap_ = isSelf;
    }

    LineJoinStyle GetLineJoin() const
    {
        return lineJoin_;
    }

    void SetLineJoin(LineJoinStyle lineJoin, bool isSelf = true)
    {
        lineJoin_ = lineJoin;
        hasLineJoin_ = isSelf;
    }

    const AnimatableDimension& GetLineWidth() const
    {
        return lineWidth_;
    }

    void SetLineWidth(Dimension lineWidth, bool isSelf = true, const AnimationOption& option = AnimationOption())
    {
        lineWidth_ = AnimatableDimension(lineWidth, option);
        hasLineWidth_ = isSelf;
    }

    double GetMiterLimit() const
    {
        return miterLimit_;
    }

    void SetMiterLimit(double miterLimit, bool isSelf = true)
    {
        miterLimit_ = miterLimit;
        hasMiterLimit_ = isSelf;
    }

    const LineDashParam& GetLineDash() const
    {
        return lineDash_;
    }

    void SetLineDash(const LineDashParam& lineDash, bool isSelf = true)
    {
        lineDash_ = lineDash;
        hasLineDash_ = isSelf;
    }

    void SetLineDashOffset(double offset, bool isSelf = true)
    {
        lineDash_.dashOffset = offset;
        hasDashOffset_ = isSelf;
    }

    void SetLineDash(const std::vector<double>& segments, bool isSelf = true)
    {
        lineDash_.lineDash = segments;
        hasLineDash_ = isSelf;
    }

    void SetStrokeDashOffset(const Dimension& offset, bool isSelf = true,
                             const AnimationOption& option = AnimationOption())
    {
        strokeDashOffset_ = AnimatableDimension(offset, option);
        hasStrokeDashOffset_ = isSelf;
    }

    void SetStrokeDashArray(const std::vector<Dimension>& segments, bool isSelf = true)
    {
        strokeDashArray_ = segments;
        hasStrokeDashArray_ = isSelf;
    }

    const AnimatableDimension& GetStrokeDashOffset() const
    {
        return strokeDashOffset_;
    }

    const std::vector<Dimension>& GetStrokeDashArray() const
    {
        return strokeDashArray_;
    }

    bool HasStroke() const
    {
        // The text outline is drawn only when stroke is set
        return color_ != Color::TRANSPARENT;
    }

    void Inherit(const StrokeState& strokeState)
    {
        if (!hasColor_) {
            color_ = strokeState.GetColor();
        }
        if (!hasOpacity_) {
            opacity_ = strokeState.GetOpacity();
        }
        if (!hasLineCap_) {
            lineCap_ = strokeState.GetLineCap();
        }
        if (!hasLineJoin_) {
            lineJoin_ = strokeState.GetLineJoin();
        }
        if (!hasLineWidth_) {
            lineWidth_ = strokeState.GetLineWidth();
        }
        if (!hasMiterLimit_) {
            miterLimit_ = strokeState.GetMiterLimit();
        }
        if (!hasLineDash_) {
            lineDash_.lineDash = strokeState.GetLineDash().lineDash;
        }
        if (!hasDashOffset_) {
            lineDash_.dashOffset = strokeState.GetLineDash().dashOffset;
        }
        if (!hasStrokeDashArray_) {
            strokeDashArray_ = strokeState.GetStrokeDashArray();
        }
        if (!hasStrokeDashOffset_) {
            strokeDashOffset_ = strokeState.GetStrokeDashOffset();
        }
    }

    bool HasColor() const
    {
        return hasColor_;
    }

    bool HasOpacity() const
    {
        return hasOpacity_;
    }

    bool HasLineWidth() const
    {
        return hasLineWidth_;
    }

    bool HasMiterLimit() const
    {
        return hasMiterLimit_;
    }

    bool HasDashOffset() const
    {
        return hasDashOffset_;
    }

    void SetHref(const std::string& href)
    {
        href_ = href;
    }

    const std::string& GetHref() const
    {
        return href_;
    }

private:
    AnimatableColor color_ = AnimatableColor(Color::TRANSPARENT);
    AnimatableDouble opacity_ = AnimatableDouble(1.0);
    LineCapStyle lineCap_ = LineCapStyle::BUTT;
    LineJoinStyle lineJoin_ = LineJoinStyle::MITER;
    AnimatableDimension lineWidth_ = AnimatableDimension(1.0);
    double miterLimit_ = 4.0;
    LineDashParam lineDash_;
    std::vector<Dimension> strokeDashArray_;
    AnimatableDimension strokeDashOffset_;
    std::string href_;
    bool hasColor_ = false;
    bool hasOpacity_ = false;
    bool hasLineCap_ = false;
    bool hasLineJoin_ = false;
    bool hasLineWidth_ = false;
    bool hasMiterLimit_ = false;
    bool hasLineDash_ = false;
    bool hasDashOffset_ = false;
    bool hasStrokeDashArray_ = false;
    bool hasStrokeDashOffset_ = false;
};

class SvgTextStyle {
public:
    void SetFontFamilies(const std::vector<std::string>& fontFamilies, bool isSelf = true)
    {
        hasFontFamilies_ = isSelf;
        textStyle_.SetFontFamilies(fontFamilies);
    }

    const std::vector<std::string>& GetFontFamilies() const
    {
        return textStyle_.GetFontFamilies();
    }

    void SetFontSize(const Dimension& fontSize, bool isSelf = true)
    {
        textStyle_.SetFontSize(fontSize);
        hasFontSize_ = isSelf;
    }

    const Dimension& GetFontSize() const
    {
        return textStyle_.GetFontSize();
    }

    void SetFontStyle(FontStyle fontStyle, bool isSelf = true)
    {
        textStyle_.SetFontStyle(fontStyle);
        hasFontStyle_ = isSelf;
    }

    FontStyle GetFontStyle() const
    {
        return textStyle_.GetFontStyle();
    }

    void SetFontWeight(FontWeight fontWeight, bool isSelf = true)
    {
        textStyle_.SetFontWeight(fontWeight);
        hasFontWeight_ = isSelf;
    }

    FontWeight GetFontWeight() const
    {
        return textStyle_.GetFontWeight();
    }

    void SetLetterSpacing(const Dimension& letterSpacing, bool isSelf = true)
    {
        textStyle_.SetLetterSpacing(letterSpacing);
        hasLetterSpacing_ = isSelf;
    }

    const Dimension& GetLetterSpacing() const
    {
        return textStyle_.GetLetterSpacing();
    }

    void SetTextDecoration(TextDecoration textDecoration, bool isSelf = true)
    {
        textStyle_.SetTextDecoration(textDecoration);
        hasTextDecoration_ = isSelf;
    }

    TextDecoration GetTextDecoration() const
    {
        return textStyle_.GetTextDecoration();
    }

    const TextStyle& GetTextStyle() const
    {
        return textStyle_;
    }

    void Inherit(const SvgTextStyle& parent)
    {
        if (!hasFontFamilies_) {
            textStyle_.SetFontFamilies(parent.GetFontFamilies());
        }
        if (!hasFontSize_) {
            textStyle_.SetFontSize(parent.GetFontSize());
        }
        if (!hasFontStyle_) {
            textStyle_.SetFontStyle(parent.GetFontStyle());
        }
        if (!hasFontWeight_) {
            textStyle_.SetFontWeight(parent.GetFontWeight());
        }
        if (!hasLetterSpacing_) {
            textStyle_.SetLetterSpacing(parent.GetLetterSpacing());
        }
        if (!hasTextDecoration_) {
            textStyle_.SetTextDecoration(parent.GetTextDecoration());
        }
    }

    bool HasLetterSpacing() const
    {
        return hasLetterSpacing_;
    }

    bool HasFontSize() const
    {
        return hasFontSize_;
    }

private:
    TextStyle textStyle_;
    bool hasFontFamilies_ = false;
    bool hasFontSize_ = false;
    bool hasFontStyle_ = false;
    bool hasFontWeight_ = false;
    bool hasLetterSpacing_ = false;
    bool hasTextDecoration_ = false;
};

class ClipState {
public:
    void SetClipRule(const std::string& clipRule, bool isSelf = true)
    {
        clipRule_ = clipRule;
        hasClipRule_ = isSelf;
    }

    const std::string& GetClipRule() const
    {
        return clipRule_;
    }

    bool IsEvenodd() const
    {
        return clipRule_ == ATTR_NAME_FILL_RULE_EVENODD;
    }

    void SetHref(const std::string& href, bool isSelf = true)
    {
        href_ = href;
        hasHref_ = isSelf;
    }

    const std::string& GetHref() const
    {
        return href_;
    }

    void Inherit(const ClipState& clipState)
    {
        if (!hasClipRule_) {
            clipRule_ = clipState.GetClipRule();
        }
        if (!hasHref_) {
            href_ = clipState.GetHref();
        }
    }

private:
    std::string clipRule_;
    std::string href_;
    bool hasClipRule_ = false;
    bool hasHref_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_SVG_PAINT_STATE_H
