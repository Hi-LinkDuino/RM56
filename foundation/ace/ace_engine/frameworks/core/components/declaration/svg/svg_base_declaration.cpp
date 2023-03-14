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

#include "core/components/declaration/svg/svg_base_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {
namespace {

const char LINECAP_ROUND[] = "round";
const char LINECAP_SQUARE[] = "square";
const char LINEJOIN_BEVEL[] = "bevel";
const char LINEJOIN_ROUND[] = "round";

const char DOM_SVG_SRC_FILL_OPACITY[] = "fill-opacity";
const char DOM_SVG_SRC_FILL_RULE[] = "fill-rule";
const char DOM_SVG_SRC_STROKE_DASHARRAY[] = "stroke-dasharray";
const char DOM_SVG_SRC_STROKE_DASHOFFSET[] = "stroke-dashoffset";
const char DOM_SVG_SRC_STROKE_LINECAP[] = "stroke-linecap";
const char DOM_SVG_SRC_STROKE_LINEJOIN[] = "stroke-linejoin";
const char DOM_SVG_SRC_STROKE_MITERLIMIT[] = "stroke-miterlimit";
const char DOM_SVG_SRC_STROKE_OPACITY[] = "stroke-opacity";
const char DOM_SVG_SRC_STROKE_WIDTH[] = "stroke-width";
const char DOM_SVG_SRC_CLIP_PATH[] = "clip-path";
const char DOM_SVG_SRC_CLIP_RULE[] = "clip-rule";
const char DOM_SVG_SRC_TRANSFORM_ORIGIN[] = "transform-origin";

const LinearMapNode<Color> COLOR_TABLE[] = {
    { "aliceblue", Color(0xfff0f8ff) },
    { "antiquewhite", Color(0xfffaebd7) },
    { "aqua", Color(0xff00ffff) },
    { "aquamarine", Color(0xff7fffd4) },
    { "azure", Color(0xfff0ffff) },
    { "beige", Color(0xfff5f5dc) },
    { "bisque", Color(0xffffe4c4) },
    { "black", Color(0XFF000000) },
    { "blanchedalmond", Color(0xffffebcd) },
    { "blue", Color(0xff0000ff) },
    { "blueviolet", Color(0xff8a2be2) },
    { "brown", Color(0xffa52a2a) },
    { "burlywood", Color(0xffdeb887) },
    { "cadetblue", Color(0xff5f9ea0) },
    { "chartreuse", Color(0xff7fff00) },
    { "chocolate", Color(0xffd2691e) },
    { "coral", Color(0xffff7f50) },
    { "cornflowerblue", Color(0xff6495ed) },
    { "cornsilk", Color(0xfffff8dc) },
    { "crimson", Color(0xffdc143c) },
    { "cyan", Color(0xff00ffff) },
    { "darkblue", Color(0xff00008b) },
    { "darkcyan", Color(0xff008b8b) },
    { "darkgoldenrod", Color(0xffb8860b) },
    { "darkgray", Color(0xffa9a9a9) },
    { "darkgreen", Color(0xff006400) },
    { "darkgrey", Color(0xffa9a9a9) },
    { "darkkhaki", Color(0xffbdb76b) },
    { "darkmagenta", Color(0xff8b008b) },
    { "darkolivegreen", Color(0xff556b2f) },
    { "darkorange", Color(0xffff8c00) },
    { "darkorchid", Color(0xff9932cc) },
    { "darkred", Color(0xff8b0000) },
    { "darksalmon", Color(0xffe9967a) },
    { "darkseagreen", Color(0xff8fbc8f) },
    { "darkslateblue", Color(0xff483d8b) },
    { "darkslategray", Color(0xff2f4f4f) },
    { "darkslategrey", Color(0xff2f4f4f) },
    { "darkturquoise", Color(0xff00ced1) },
    { "darkviolet", Color(0xff9400D3) },
    { "deeppink", Color(0xffff1493) },
    { "deepskyblue", Color(0xff00bfff) },
    { "dimgray", Color(0xff696969) },
    { "dimgrey", Color(0xff696969) },
    { "dodgerblue", Color(0xff1e90ff) },
    { "firebrick", Color(0xffb22222) },
    { "floralwhite", Color(0xfffffaf0) },
    { "forestgreen", Color(0xff228b22) },
    { "fuchsia", Color(0xffff00ff) },
    { "gainsboro", Color(0xffdcdcdc) },
    { "ghostwhite", Color(0xfff8f8ff) },
    { "gold", Color(0xffffd700) },
    { "goldenrod", Color(0xffdaa520) },
    { "gray", Color(0xff808080) },
    { "green", Color(0xff008000) },
    { "greenyellow", Color(0xffadff2f) },
    { "grey", Color(0xff808080) },
    { "honeydew", Color(0xfff0fff0) },
    { "hotpink", Color(0xffff69b4) },
    { "indianred", Color(0xffcd5c5c) },
    { "indigo", Color(0xff4b0082) },
    { "ivory", Color(0xfffffff0) },
    { "khaki", Color(0xfff0e68c) },
    { "lavender", Color(0xffe6e6fa) },
    { "lavenderblush", Color(0xfffff0f5) },
    { "lawngreen", Color(0xff7cfc00) },
    { "lemonchiffon", Color(0xfffffacd) },
    { "lightblue", Color(0xffadd8e6) },
    { "lightcoral", Color(0xfff08080) },
    { "lightcyan", Color(0xffe0ffff) },
    { "lightgoldenrodyellow", Color(0xfffafad2) },
    { "lightgray", Color(0xffd3d3d3) },
    { "lightgreen", Color(0xff90ee90) },
    { "lightgrey", Color(0xffd3d3d3) },
    { "lightpink", Color(0xffffb6c1) },
    { "lightsalmon", Color(0xffffa07a) },
    { "lightseagreen", Color(0xff20b2aa) },
    { "lightskyblue", Color(0xff87cefa) },
    { "lightslategray", Color(0xff778899) },
    { "lightslategrey", Color(0xff778899) },
    { "lightsteelblue", Color(0xffb0c4de) },
    { "lightyellow", Color(0xffffffe0) },
    { "lime", Color(0xff00ff00) },
    { "limegreen", Color(0xff32cd32) },
    { "linen", Color(0xfffaf0e6) },
    { "magenta", Color(0xffff00ff) },
    { "maroon", Color(0xff800000) },
    { "mediumaquamarine", Color(0xff66cdaa) },
    { "mediumblue", Color(0xff0000cd) },
    { "mediumorchid", Color(0xffba55d3) },
    { "mediumpurple", Color(0xff9370db) },
    { "mediumseagreen", Color(0xff3cb371) },
    { "mediumslateblue", Color(0xff7b68ee) },
    { "mediumspringgreen", Color(0xff00fa9a) },
    { "mediumturquoise", Color(0xff48d1cc) },
    { "mediumvioletred", Color(0xffc71585) },
    { "midnightblue", Color(0xff191970) },
    { "mintcream", Color(0xfff5fffa) },
    { "mistyrose", Color(0xffffe4e1) },
    { "moccasin", Color(0xffffe4b5) },
    { "navajowhite", Color(0xffffdead) },
    { "navy", Color(0xff000080) },
    { "oldlace", Color(0xfffdf5e6) },
    { "olive", Color(0xff808000) },
    { "olivedrab", Color(0xff6b8e23) },
    { "orange", Color(0xffffa500) },
    { "orangered", Color(0xffff4500) },
    { "orchid", Color(0xffda70d6) },
    { "palegoldenrod", Color(0xffeee8aa) },
    { "palegreen", Color(0xff98fb98) },
    { "paleturquoise", Color(0xffafeeee) },
    { "palevioletred", Color(0xffdb7093) },
    { "papayawhip", Color(0xffffefd5) },
    { "peachpuff", Color(0xffffdab9) },
    { "peru", Color(0xffcd853f) },
    { "pink", Color(0xffffc0cb) },
    { "plum", Color(0xffdda0dd) },
    { "powderblue", Color(0xffb0e0e6) },
    { "purple", Color(0xff800080) },
    { "red", Color(0xffff0000) },
    { "rosybrown", Color(0xffbc8f8f) },
    { "royalblue", Color(0xff4169e1) },
    { "rebeccapurple", Color(0xff663399) },
    { "saddlebrown", Color(0xff8b4513) },
    { "salmon", Color(0xfffa8072) },
    { "sandybrown", Color(0xfff4a460) },
    { "seagreen", Color(0xff2e8b57) },
    { "seashell", Color(0xfffff5ee) },
    { "sienna", Color(0xffa0522d) },
    { "silver", Color(0xffc0c0c0) },
    { "skyblue", Color(0xff87ceeb) },
    { "slateblue", Color(0xff6a5acd) },
    { "slategray", Color(0xff708090) },
    { "slategrey", Color(0xff708090) },
    { "snow", Color(0xfffffafa) },
    { "springgreen", Color(0xff00ff7f) },
    { "steelblue", Color(0xff4682b4) },
    { "tan", Color(0xffd2b48c) },
    { "teal", Color(0xff008080) },
    { "thistle", Color(0xffd8bfd8) },
    { "tomato", Color(0xffff6347) },
    { "turquoise", Color(0xff40e0d0) },
    { "violet", Color(0xffee82ee) },
    { "wheat", Color(0xfff5deb3) },
    { "white", Color(0xffffffff) },
    { "whitesmoke", Color(0xfff5f5f5) },
    { "yellow", Color(0xffffff00) },
    { "yellowgreen", Color(0xff9acd32) },
};

} // namespace

using namespace Framework;

void SvgBaseDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_BASE_ATTR);
}

bool SvgBaseDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    return SetPresentationAttr(attr);
}

bool SvgBaseDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    return SetPresentationAttr(style);
}

bool SvgBaseDeclaration::SetPresentationAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgBaseDeclaration&)> svgBaseAttrs[] = {
        { DOM_SVG_SRC_CLIP_PATH,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                auto value = StringUtils::TrimStr(val);
                if (value.find("url(") == 0) {
                    auto src = std::regex_replace(value,
                        std::regex(R"(^url\(\s*['"]?\s*#([^()]+?)\s*['"]?\s*\)$)"), "$1");
                    attrs.clipState.SetHref(src);
                }
            } },
        { DOM_SVG_SRC_CLIP_RULE,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.clipState.SetClipRule(val);
            } },
        { DOM_CLIP_PATH,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                auto value = StringUtils::TrimStr(val);
                if (value.find("url(") == 0) {
                    auto src = std::regex_replace(value,
                        std::regex(R"(^url\(\s*['"]?\s*#([^()]+?)\s*['"]?\s*\)$)"), "$1");
                    attrs.clipState.SetHref(src);
                }
            } },
        { DOM_SVG_CLIP_RULE,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.clipState.SetClipRule(val);
            } },
        { DOM_SVG_FILL,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                auto value = StringUtils::TrimStr(val);
                if (value.find("url(") == 0) {
                    auto src = std::regex_replace(value,
                        std::regex(R"(^url\(\s*['"]?\s*#([^()]+?)\s*['"]?\s*\)$)"), "$1");
                    attrs.fillState.SetHref(src);
                } else {
                    Color fill = (val == VALUE_NONE ? Color::TRANSPARENT : declaration.GetColor(val));
                    attrs.fillState.SetColor(fill);
                }
            } },
        { DOM_SVG_SRC_FILL_OPACITY,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.fillState.SetOpacity(std::clamp(StringToDouble(val), 0.0, 1.0));
            } },
        { DOM_SVG_SRC_FILL_RULE,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.fillState.SetFillRule(val);
            } },
        { DOM_SVG_FILL_OPACITY,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.fillState.SetOpacity(std::clamp(StringToDouble(val), 0.0, 1.0));
            } },
        { DOM_SVG_FILL_RULE,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.fillState.SetFillRule(val);
            } },
        { DOM_SVG_FILTER,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.filterId = val;
            } },
        { DOM_SVG_FONT_SIZE,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                Dimension fontSize = StringToDimension(val);
                if (GreatOrEqual(fontSize.Value(), 0.0)) {
                    attrs.textStyle.SetFontSize(fontSize);
                }
            } },
        { DOM_SVG_HREF,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                auto value = StringUtils::TrimStr(val);
                if (value.substr(0, 1) == "#") {
                    attrs.href = value.substr(1);
                }
            } },
        { DOM_SVG_MASK,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.maskId = val;
            } },
        { DOM_SVG_OPACITY,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.hasOpacity = true;
                attrs.opacity = std::clamp(StringToDouble(val), 0.0, 1.0);
            } },
        { DOM_SVG_STROKE,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                auto value = StringUtils::TrimStr(val);
                if (value.find("url(") == 0) {
                    auto src = std::regex_replace(value,
                        std::regex(R"(^url\(\s*['"]?\s*#([^()]+?)\s*['"]?\s*\)$)"), "$1");
                    attrs.strokeState.SetHref(src);
                } else {
                    Color stroke = (val == VALUE_NONE ? Color::TRANSPARENT : declaration.GetColor(val));
                    attrs.strokeState.SetColor(stroke);
                }
            } },
        { DOM_SVG_SRC_STROKE_DASHARRAY,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                if (val.empty()) {
                    return;
                }
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                std::vector<double> lineDashVector;
                StringUtils::StringSpliter(val, ' ', lineDashVector);
                attrs.strokeState.SetLineDash(lineDashVector);
            } },
        { DOM_SVG_SRC_STROKE_DASHOFFSET,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.strokeState.SetLineDashOffset(StringToDouble(val));
            } },
        { DOM_SVG_SRC_STROKE_LINECAP,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.strokeState.SetLineCap(declaration.GetLineCapStyle(val));
            } },
        { DOM_SVG_SRC_STROKE_LINEJOIN,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.strokeState.SetLineJoin(declaration.GetLineJoinStyle(val));
            } },
        { DOM_SVG_SRC_STROKE_MITERLIMIT,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                double miterLimit = StringToDouble(val);
                if (GreatOrEqual(miterLimit, 1.0)) {
                    attrs.strokeState.SetMiterLimit(miterLimit);
                }
            } },
        { DOM_SVG_SRC_STROKE_OPACITY,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.strokeState.SetOpacity(std::clamp(StringToDouble(val), 0.0, 1.0));
            } },
        { DOM_SVG_SRC_STROKE_WIDTH,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                Dimension lineWidth = StringToDimension(val);
                if (GreatOrEqual(lineWidth.Value(), 0.0)) {
                    attrs.strokeState.SetLineWidth(lineWidth);
                }
            } },
        { DOM_SVG_STROKE_DASHARRAY,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                if (val.empty()) {
                    return;
                }
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                std::vector<double> lineDashVector;
                StringUtils::StringSpliter(val, ' ', lineDashVector);
                attrs.strokeState.SetLineDash(lineDashVector);
            } },
        { DOM_SVG_STROKE_DASHOFFSET,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.strokeState.SetLineDashOffset(StringToDouble(val));
            } },
        { DOM_SVG_STROKE_LINECAP,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.strokeState.SetLineCap(declaration.GetLineCapStyle(val));
            } },
        { DOM_SVG_STROKE_LINEJOIN,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.strokeState.SetLineJoin(declaration.GetLineJoinStyle(val));
            } },
        { DOM_SVG_STROKE_MITERLIMIT,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                double miterLimit = StringToDouble(val);
                if (GreatOrEqual(miterLimit, 1.0)) {
                    attrs.strokeState.SetMiterLimit(miterLimit);
                }
            } },
        { DOM_SVG_STROKE_OPACITY,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.strokeState.SetOpacity(std::clamp(StringToDouble(val), 0.0, 1.0));
            } },
        { DOM_SVG_STROKE_WIDTH,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                Dimension lineWidth = StringToDimension(val);
                if (GreatOrEqual(lineWidth.Value(), 0.0)) {
                    attrs.strokeState.SetLineWidth(lineWidth);
                }
            } },
        { DOM_TRANSFORM,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.transform = val;
            } },
        { DOM_SVG_SRC_TRANSFORM_ORIGIN,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                attrs.transformOrigin = val;
            } },
        { DOM_SVG_XLINK_HREF,
            [](const std::string& val, SvgBaseDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
                auto value = StringUtils::TrimStr(val);
                if (value.substr(0, 1) == "#") {
                    attrs.href = value.substr(1);
                }
            } }
    };
    auto attrIter = BinarySearchFindIndex(svgBaseAttrs, ArraySize(svgBaseAttrs), attr.first.c_str());
    if (attrIter != -1) {
        svgBaseAttrs[attrIter].value(attr.second, *this);
        return true;
    }
    return false;
}

void SvgBaseDeclaration::Inherit(const RefPtr<Declaration>& parent)
{
    auto svgDeclaration = AceType::DynamicCast<SvgBaseDeclaration>(parent);
    if (!svgDeclaration) {
        LOGD("parent declaration is null");
        return;
    }
    auto& attrs = MaybeResetAttribute<SvgBaseAttribute>(AttributeTag::SPECIALIZED_ATTR);
    if (!attrs.IsValid()) {
        LOGE("attrs is invalid");
        return;
    }
    if (!attrs.hasOpacity) {
        if (svgDeclaration->HasOpacity()) {
            attrs.opacity = svgDeclaration->GetOpacity();
        } else {
            attrs.opacity = 1.0; // defualt opacity is 1.0
        }
    }
    attrs.fillState.Inherit(svgDeclaration->GetFillState());
    attrs.strokeState.Inherit(svgDeclaration->GetStrokeState());
    attrs.textStyle.Inherit(svgDeclaration->GetSvgTextStyle());
    attrs.clipState.Inherit(svgDeclaration->GetClipState());
}

LineCapStyle SvgBaseDeclaration::GetLineCapStyle(const std::string& val) const
{
    if (val == LINECAP_ROUND) {
        return LineCapStyle::ROUND;
    } else if (val == LINECAP_SQUARE) {
        return LineCapStyle::SQUARE;
    } else {
        return LineCapStyle::BUTT;
    }
}

LineJoinStyle SvgBaseDeclaration::GetLineJoinStyle(const std::string& val) const
{
    if (val == LINEJOIN_BEVEL) {
        return LineJoinStyle::BEVEL;
    } else if (val == LINEJOIN_ROUND) {
        return LineJoinStyle::ROUND;
    } else {
        return LineJoinStyle::MITER;
    }
}

Color SvgBaseDeclaration::GetColor(const std::string& value) const
{
    int64_t colorIndex = BinarySearchFindIndex(COLOR_TABLE, ArraySize(COLOR_TABLE), value.c_str());
    if (colorIndex != -1) {
        return COLOR_TABLE[colorIndex].value;
    }
    return Color::FromString(value);
}

} // namespace OHOS::Ace
