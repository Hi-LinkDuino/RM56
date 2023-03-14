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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_UTILS_H

#include <string>
#include <vector>

#include "core/components/common/layout/constants.h"
#include "core/components/common/layout/align_declaration.h"

namespace OHOS::Ace::V2 {

namespace {

const char* AXIS_TYPE[] = {
    "Axis.VERTICAL",
    "Axis.HORIZONTAL",
    "Axis.FREE",
    "Axis.NONE",
};

}

inline std::string ConvertStackFitToString(StackFit stackFit)
{
    std::string result = "";
    switch (stackFit) {
        case StackFit::STRETCH:
            result = "StackFit.Stretch";
            break;
        case StackFit::INHERIT:
            result = "StackFit.Inherit";
            break;
        case StackFit::FIRST_CHILD:
            result = "StackFit.FirstChild";
            break;
        case StackFit::KEEP:
        default:
            result = "StackFit.Keep";
            break;
    }
    return result;
}

inline std::string ConvertOverflowToString(Overflow overflow)
{
    std::string result = "";
    switch (overflow) {
        case Overflow::CLIP:
            result = "Overflow.Clip";
            break;
        case Overflow::SCROLL:
            result = "Overflow.Scroll";
            break;
        case Overflow::FORCE_CLIP:
            result = "Overflow.ForceClip";
            break;
        case Overflow::OBSERVABLE:
        default:
            result = "Overflow.Observable";
            break;
    }
    return result;
}

inline std::string ConvertBoolToString(bool flag)
{
    return flag ? "true" : "false";
}

inline std::string ConvertTabBarModeToString(TabBarMode barMode)
{
    std::string result = "";
    switch (barMode) {
        case TabBarMode::SCROLLABEL:
            result = "BarMode.Scrollable";
            break;
        case TabBarMode::FIXED_START:
            result = "BarMode.FixedStart";
            break;
        case TabBarMode::FIXED:
        default:
            result = "BarMode.Fixed";
            break;
    }
    return result;
}

inline std::string ConvertBarPositionToString(BarPosition barPosition)
{
    std::string result = "";
    switch (barPosition) {
        case BarPosition::END:
            result = "BarPosition.End";
            break;
        case BarPosition::START:
        default:
            result = "BarPosition.Start";
            break;
    }
    return result;
}

inline std::string ConvertFlexDirectionToStirng(FlexDirection direction)
    {
        if (direction == FlexDirection::ROW) {
            return "FlexDirection.Row";
        } else if (direction == FlexDirection::ROW_REVERSE) {
            return "FlexDirection.RowReverse";
        } else if (direction == FlexDirection::COLUMN) {
            return "FlexDirection.Column";
        } else {
            return "FlexDirection.ColumnReverse";
        }
    }

    inline std::string ConvertWrapDirectionToStirng(WrapDirection direction)
    {
        if (direction == WrapDirection::HORIZONTAL) {
            return "FlexDirection.Row";
        } else if (direction == WrapDirection::HORIZONTAL_REVERSE) {
            return "FlexDirection.RowReverse";
        } else if (direction == WrapDirection::VERTICAL) {
            return "FlexDirection.Column";
        } else {
            return "FlexDirection.ColumnReverse";
        }
    }

    inline std::string ConvertFlexAlignToStirng(FlexAlign align)
    {
        if (align == FlexAlign::FLEX_START) {
            return "FlexAlign.Start";
        } else if (align == FlexAlign::CENTER) {
            return "FlexAlign.Center";
        } else if (align == FlexAlign::FLEX_END) {
            return "FlexAlign.End";
        } else if (align == FlexAlign::SPACE_BETWEEN) {
            return "FlexAlign.SpaceBetween";
        } else if (align == FlexAlign::SPACE_AROUND) {
            return "FlexAlign.SpaceAround";
        } else if (align == FlexAlign::BASELINE) {
            return "FlexAlign.Baseline";
        } else if (align == FlexAlign::STRETCH) {
            return "FlexAlign.Stretch";
        } else {
            return "FlexAlign.SpaceEvenly";
        }
    }

    inline std::string ConvertWrapAlignmentToStirng(WrapAlignment align)
    {
        if (align == WrapAlignment::START) {
            return "FlexAlign.Start";
        } else if (align == WrapAlignment::CENTER) {
            return "FlexAlign.Center";
        } else if (align == WrapAlignment::END) {
            return "FlexAlign.End";
        } else if (align == WrapAlignment::SPACE_BETWEEN) {
            return "FlexAlign.SpaceBetween";
        } else if (align == WrapAlignment::SPACE_AROUND) {
            return "FlexAlign.SpaceAround";
        } else if (align == WrapAlignment::STRETCH) {
            return "FlexAlign.Stretch";
        } else if (align == WrapAlignment::BASELINE) {
            return "FlexAlign.Baseline";
        } else {
            return "FlexAlign.SpaceEvenly";
        }
    }

    inline std::string ConvertWrapTextDecorationToStirng(TextDecoration decoration)
    {
        static const LinearEnumMapNode<TextDecoration, std::string> decorationTable[] = {
            { TextDecoration::NONE, "TextDecorationType.None" },
            { TextDecoration::UNDERLINE, "TextDecorationType.Underline" },
            { TextDecoration::OVERLINE, "TextDecorationType.Overline" },
            { TextDecoration::LINE_THROUGH, "TextDecorationType.LineThrough" },
            { TextDecoration::INHERIT, "TextDecorationType.Inherit" },
        };

        auto index = BinarySearchFindIndex(decorationTable, ArraySize(decorationTable), decoration);
        return index < 0 ? "TextDecorationType.None" : decorationTable[index].value;
    }

    inline std::string ConvertWrapTextCaseToStirng(TextCase textCase)
    {
        static const LinearEnumMapNode<TextCase, std::string> textCaseTable[] = {
            { TextCase::NORMAL, "TextCase.Normal" },
            { TextCase::LOWERCASE, "TextCase.LowerCase" },
            { TextCase::UPPERCASE, "TextCase.UpperCase" },
        };

        auto index = BinarySearchFindIndex(textCaseTable, ArraySize(textCaseTable), textCase);
        return index < 0 ? "TextCase.Normal" : textCaseTable[index].value;
    }

    inline std::string ConvertWrapImageFitToString(ImageFit imageFit)
    {
        static const LinearEnumMapNode<ImageFit, std::string> imageFitTable[] = {
            { ImageFit::COVER, "ImageFit.Cover" },
            { ImageFit::FILL, "ImageFit.Fill" },
            { ImageFit::CONTAIN, "ImageFit.Contain" },
            { ImageFit::FITWIDTH, "ImageFit.FitWidth" },
            { ImageFit::FITHEIGHT, "ImageFit.FitHeight" },
            { ImageFit::NONE, "ImageFit.None" },
            { ImageFit::SCALEDOWN, "ImageFit.ScaleDown" },
        };

        auto index = BinarySearchFindIndex(imageFitTable, ArraySize(imageFitTable), imageFit);
        return index < 0 ? "ImageFit.Cover" : imageFitTable[index].value;
    }

    inline std::string ConvertWrapImageRepeatToString(ImageRepeat repeat)
    {
        static const LinearEnumMapNode<ImageRepeat, std::string> imageRepeatTable[] = {
            { ImageRepeat::NOREPEAT, "ImageRepeat.NoRepeat" },
            { ImageRepeat::REPEAT, "ImageRepeat.XY" },
            { ImageRepeat::REPEATX, "ImageRepeat.X" },
            { ImageRepeat::REPEATY, "ImageRepeat.Y" },
        };

        auto index = BinarySearchFindIndex(imageRepeatTable, ArraySize(imageRepeatTable), repeat);
        return index < 0 ? "ImageRepeat.NoRepeat" : imageRepeatTable[index].value;
    }

    inline std::string ConvertWrapImageInterpolationToString(ImageInterpolation imageInterpolation)
    {
        static const LinearEnumMapNode<ImageInterpolation, std::string> imageInterpolationTable[] = {
            { ImageInterpolation::NONE, "ImageInterpolation.None" },
            { ImageInterpolation::LOW, "ImageInterpolation.Low" },
            { ImageInterpolation::MEDIUM, "ImageInterpolation.Medium" },
            { ImageInterpolation::HIGH, "ImageInterpolation.High" },
        };

        auto index =
            BinarySearchFindIndex(imageInterpolationTable, ArraySize(imageInterpolationTable), imageInterpolation);
        return index < 0 ? "ImageInterpolation.None" : imageInterpolationTable[index].value;
    }

    inline std::string ConvertWrapImageRenderModeToString(ImageRenderMode imageRenderMode)
    {
        static const LinearEnumMapNode<ImageRenderMode, std::string> imageRenderModeTable[] = {
            { ImageRenderMode::ORIGINAL, "ImageRenderMode.Original" },
            { ImageRenderMode::TEMPLATE, "ImageRenderMode.Template" },
        };

        auto index = BinarySearchFindIndex(imageRenderModeTable, ArraySize(imageRenderModeTable), imageRenderMode);
        return index < 0 ? "ImageRenderMode.Original" : imageRenderModeTable[index].value;
    }

    inline std::string ConvertWrapTextAlignToString(TextAlign textAlign)
    {
        static const LinearEnumMapNode<TextAlign, std::string> textAlignTable[] = {
            { TextAlign::LEFT, "TextAlign.Left" },
            { TextAlign::RIGHT, "TextAlign.Right" },
            { TextAlign::CENTER, "TextAlign.Center" },
            { TextAlign::JUSTIFY, "TextAlign.Justify" },
            { TextAlign::START, "TextAlign.Start" },
            { TextAlign::END, "TextAlign.End" },
        };

        auto index = BinarySearchFindIndex(textAlignTable, ArraySize(textAlignTable), textAlign);
        return index < 0 ? "TextAlign.Start" : textAlignTable[index].value;
    }

    inline std::string ConvertWrapTextOverflowToString(TextOverflow textOverflow)
    {
        static const LinearEnumMapNode<TextOverflow, std::string> textOverflowTable[] = {
            { TextOverflow::CLIP, "TextOverflow.Clip" },
            { TextOverflow::ELLIPSIS, "TextOverflow.Ellipsis" },
            { TextOverflow::NONE, "TextOverflow.None" },
        };

        auto index = BinarySearchFindIndex(textOverflowTable, ArraySize(textOverflowTable), textOverflow);
        return index < 0 ? "TextAlign.Start" : textOverflowTable[index].value;
    }

    inline std::string ConvertWrapFontStyleToStirng(FontStyle fontStyle)
    {
        static const LinearEnumMapNode<FontStyle, std::string> fontStyleTable[] = {
            { FontStyle::NORMAL, "FontStyle.Normal" },
            { FontStyle::ITALIC, "FontStyle.Italic" },
        };

        auto index = BinarySearchFindIndex(fontStyleTable, ArraySize(fontStyleTable), fontStyle);
        return index < 0 ? "FontStyle.Normal" : fontStyleTable[index].value;
    }

    inline std::string ConvertWrapFontWeightToStirng(FontWeight fontWeight)
    {
        static const LinearEnumMapNode<FontWeight, std::string> fontWeightTable[] = {
            { FontWeight::W100, "FontWeight.100" },
            { FontWeight::W200, "FontWeight.200" },
            { FontWeight::W300, "FontWeight.300" },
            { FontWeight::W400, "FontWeight.400" },
            { FontWeight::W500, "FontWeight.500" },
            { FontWeight::W600, "FontWeight.600" },
            { FontWeight::W700, "FontWeight.700" },
            { FontWeight::W800, "FontWeight.800" },
            { FontWeight::W900, "FontWeight.900" },
            { FontWeight::BOLD, "FontWeight.Bold" },
            { FontWeight::NORMAL, "FontWeight.Normal" },
            { FontWeight::BOLDER, "FontWeight.Bolder" },
            { FontWeight::LIGHTER, "FontWeight.Lighter" },
            { FontWeight::MEDIUM, "FontWeight.Medium" },
            { FontWeight::REGULAR, "FontWeight.Regular" },
        };

        auto index = BinarySearchFindIndex(fontWeightTable, ArraySize(fontWeightTable), fontWeight);
        return index < 0 ? "FontWeight.Normal" : fontWeightTable[index].value;
    }

    inline std::string ConvertColorToString(Color color)
    {
        return color.ColorToString();
    }

    inline std::string ConvertAxisToString(Axis axis)
    {
        return AXIS_TYPE[static_cast<int32_t>(axis)];
    }

    inline  std::string ConvertSideToString(AlignDeclaration::Edge edge)
    {
        if (edge == AlignDeclaration::Edge::TOP) {
            return "Edge::Top";
        } else if (edge == AlignDeclaration::Edge::CENTER) {
            return "Edge::Center";
        } else if (edge == AlignDeclaration::Edge::BOTTOM) {
            return "Edge::Bottom";
        } else if (edge == AlignDeclaration::Edge::BASELINE) {
            return "Edge::Baseline";
        } else if (edge == AlignDeclaration::Edge::START) {
            return "Edge::Start";
        } else if (edge == AlignDeclaration::Edge::MIDDLE) {
            return "Edge::Middle";
        } else if (edge == AlignDeclaration::Edge::END) {
            return "Edge::End";
        } else {
            return "Edge::Center";
        }
    }
} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_UTILS_H