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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_CONSTANTS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_CONSTANTS_H

#include <cstdint>

namespace OHOS::Ace {

enum class LineCap {
    BUTT,
    ROUND,
    SQUARE,
};

enum class ButtonType {
    NORMAL,
    CAPSULE,
    CIRCLE,
    TEXT,
    ARC,
    DOWNLOAD,
    ICON,
    CUSTOM,
};

// Flex Styles
enum class FlexDirection {
    ROW = 0,
    COLUMN,
    ROW_REVERSE,
    COLUMN_REVERSE,
};

enum class FlexAlign {
    AUTO,

    // align to the start of the axis, can be both used in MainAxisAlign and CrossAxisAlign.
    FLEX_START,

    // align to the center of the axis, can be both used in MainAxisAlign and CrossAxisAlign.
    CENTER,

    // align to the end of the axis, can be both used in MainAxisAlign and CrossAxisAlign.
    FLEX_END,

    // stretch the cross size, only used in CrossAxisAlign.
    STRETCH,

    // adjust the cross position according to the textBaseline, only used in CrossAxisAlign.
    BASELINE,

    // align the children on both ends, only used in MainAxisAlign.
    SPACE_BETWEEN,

    // align the child with equivalent space, only used in MainAxisAlign.
    SPACE_AROUND,

    // align the child with space evenly, only used in MainAxisAlign.
    SPACE_EVENLY,

    // User-defined space, only used in MainAxisAlign.
    SPACE_CUSOMIZATION,
};

enum class MainAxisSize {
    // default setting, fill the max size of the layout param. Only under MAX, mainAxisAlign and FlexItem are valid
    MAX,

    // make the size of row/column as large as its children's size.
    MIN,
};

enum class CrossAxisSize {
    // fill the max size of the layout param in cross size of row/column.
    MAX,

    // make the cross size of row/column as large as its children's size.
    MIN,
};

enum class FlexLayoutMode {
    // If children do not contains flex weight, use this mode. It is the default mode.
    FLEX_ITEM_MODE,

    // If all the children contains flex weight, use this mode.
    FLEX_WEIGHT_MODE,
};

// Box Style
enum class BoxFlex {
    // width and height do not extend to box's parent
    FLEX_NO,

    // width extends to box's parent, height does not extend to box's parent
    FLEX_X,

    // height extends to box's parent, width does not extend to box's parent
    FLEX_Y,

    // width and height extend to box's parent
    FLEX_XY,
};

enum class BoxSizing {
    // The width and height properties includes only the content. Border and padding are not included.
    CONTENT_BOX,

    // The width and height properties includes content, padding and border.
    BORDER_BOX,
};

// Stack Styles
enum class StackFit {
    // keep the child component's size as it is.
    KEEP,

    // keep the child component's size as the parent's.
    STRETCH,

    // use parent's layoutParam to layout the child
    INHERIT,

    // use first child's size as layoutPram max size.
    FIRST_CHILD,
};

enum class Overflow {
    // when the size overflows, clip the exceeding.
    CLIP,

    // when the size overflows, observe the exceeding.
    OBSERVABLE,

    // when the size overflows, scroll the exceeding.
    SCROLL,

    // force clip the exceeding.
    FORCE_CLIP,
};

enum class MainStackSize {
    MAX,
    MIN,
    NORMAL,
    LAST_CHILD_HEIGHT,
    MATCH_CHILDREN,
    MAX_X,
    MAX_Y
};

enum class MainSwiperSize {
    MAX,
    MAX_X,
    MAX_Y,
    MIN,
    AUTO
};

enum class PositionType {
    RELATIVE = 0,
    FIXED,
    ABSOLUTE,
    OFFSET, // percetage layout based on RELATIVE
    SEMI_RELATIVE, // absolute offset based on RELATIVE
};

enum class TextAlign {
    LEFT,
    RIGHT,
    CENTER,
    /*
        render the text to fit the size of the text container by adding space
    */
    JUSTIFY,
    /*
        align the text from the start of the text container

        For Direction.ltr, from left side
        For Direction.rtl, from right side
    */
    START,
    /*
        align the text from the end of the text container

        For Direction.ltr, from right side
        For Direction.rtl, from left side
    */
    END,
};


enum class WhiteSpace {
    NORMAL,
    PRE,
    PRE_WRAP,
    NOWRAP,
    PRE_LINE,
    INHERIT,
};

enum class TextOverflow {
    CLIP,
    ELLIPSIS,
    NONE,
};

enum class TextDirection {
    LTR,
    RTL,
    INHERIT,
    AUTO,
};

enum class TextDecoration {
    NONE,
    UNDERLINE,
    OVERLINE,
    LINE_THROUGH,
    INHERIT,
};

enum class MarqueeDirection {
    LEFT,
    RIGHT,
};

enum class ImageRepeat {
    NOREPEAT = 0,
    REPEATX,
    REPEATY,
    REPEAT,
};

enum class ImageFit {
    FILL,
    CONTAIN,
    COVER,
    FITWIDTH,
    FITHEIGHT,
    NONE,
    SCALEDOWN,
};

enum class ImageRenderMode {
    ORIGINAL = 0,
    TEMPLATE,
};

enum class ImageInterpolation {
    NONE = 0,
    LOW,
    MEDIUM,
    HIGH,
};

enum class EdgeEffect {
    SPRING = 0,
    FADE,
    NONE,
};

enum class BorderStyle {
    SOLID,
    DASHED,
    DOTTED,
    NONE,
};

enum class BorderImageRepeat {
    SPACE,
    STRETCH,
    REPEAT,
    ROUND,
};

enum class BorderImageDirection {
    TOP,
    LEFT,
    RIGHT,
    BOTTOM,
};

enum class SrcType {
    UNSUPPORTED = -1,
    FILE = 0,
    ASSET,
    NETWORK,
    MEMORY,
    BASE64,
    INTERNAL, // internal cached file resource
    RESOURCE,
    DATA_ABILITY,
    RESOURCE_ID, // default resource which src is internal resource id
    PIXMAP,
};

enum class WrapAlignment {
    START,
    CENTER,
    END,
    SPACE_AROUND,
    SPACE_BETWEEN,
    STRETCH,
    SPACE_EVENLY,
    BASELINE,
};

enum class WrapDirection {
    HORIZONTAL,
    VERTICAL,
    HORIZONTAL_REVERSE,
    VERTICAL_REVERSE,
};

enum class FlexWrap {
    NO_WRAP,
    WRAP,
    WRAP_REVERSE,
};

enum class KeyDirection {
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
};

const ImageRepeat IMAGE_REPEATS[] = {
    ImageRepeat::REPEAT,
    ImageRepeat::REPEATX,
    ImageRepeat::REPEATY,
    ImageRepeat::NOREPEAT,
};

enum class WindowModal : int32_t {
    NORMAL = 0,
    SEMI_MODAL = 1,
    SEMI_MODAL_FULL_SCREEN = 2,
    DIALOG_MODAL = 3,
    CONTAINER_MODAL = 4,
    FIRST_VALUE = NORMAL,
    LAST_VALUE = CONTAINER_MODAL,
};

enum class WindowMode : uint32_t {
    WINDOW_MODE_UNDEFINED = 0,
    WINDOW_MODE_FULLSCREEN = 1,
    WINDOW_MODE_SPLIT_PRIMARY = 100,
    WINDOW_MODE_SPLIT_SECONDARY,
    WINDOW_MODE_FLOATING,
    WINDOW_MODE_PIP
};

enum class PanelType {
    MINI_BAR,
    FOLDABLE_BAR,
    TEMP_DISPLAY,
};

enum class PanelMode {
    MINI,
    HALF,
    FULL,
    AUTO,
};

enum class ColorScheme : int32_t {
    SCHEME_LIGHT = 0,
    SCHEME_TRANSPARENT = 1,
    SCHEME_DARK = 2,
    FIRST_VALUE = SCHEME_LIGHT,
    LAST_VALUE = SCHEME_DARK,
};

enum class RenderStatus : int32_t {
    UNKNOWN = -1,
    DEFAULT = 0,
    ACTIVITY = 1,
    FOCUS = 2,
    BLUR = 3,
    DISABLE = 4,
    WAITING = 5,
};

enum class BadgePosition {
    RIGHT_TOP,
    RIGHT,
    LEFT,
};

enum class QrcodeType {
    RECT,
    CIRCLE,
};

enum class AnimationCurve {
    FRICTION,
    STANDARD,
};

enum class WindowBlurStyle {
    STYLE_BACKGROUND_SMALL_LIGHT = 100,
    STYLE_BACKGROUND_MEDIUM_LIGHT = 101,
    STYLE_BACKGROUND_LARGE_LIGHT = 102,
    STYLE_BACKGROUND_XLARGE_LIGHT = 103,
    STYLE_BACKGROUND_SMALL_DARK = 104,
    STYLE_BACKGROUND_MEDIUM_DARK = 105,
    STYLE_BACKGROUND_LARGE_DARK = 106,
    STYLE_BACKGROUND_XLARGE_DARK = 107,
};

enum class DisplayType {
    NO_SETTING = 0,
    FLEX,
    GRID,
    NONE,
};

enum class VisibilityType {
    NO_SETTING = 0,
    VISIBLE,
    HIDDEN,
};

enum class RefreshType {
    AUTO,
    PULL_DOWN,
};

enum class TabBarMode {
    FIXED,
    SCROLLABEL,
    FIXED_START,
};

enum class ShowInNavigationBar {
    SHOW = 0,
    POPUP,
};

enum class HorizontalAlign {
    START = 1,
    CENTER,
    END,
};

enum class VerticalAlign {
    TOP = 1,
    CENTER,
    BOTTOM,
    BASELINE,
    NONE,
};

enum class BarPosition {
    START,
    END,
};

enum class CalendarType {
    NORMAL = 0,
    SIMPLE,
};

enum class SideBarContainerType {
    EMBED,
    OVERLAY
};

inline constexpr uint32_t STATE_NORMAL = 0;
inline constexpr uint32_t STATE_PRESSED = 1;
inline constexpr uint32_t STATE_FOCUS = 1 << 1;
inline constexpr uint32_t STATE_CHECKED = 1 << 2;
inline constexpr uint32_t STATE_DISABLED = 1 << 3;
inline constexpr uint32_t STATE_WAITING = 1 << 4;
inline constexpr uint32_t STATE_HOVERED = 1 << 5;
inline constexpr uint32_t STATE_ACTIVE = 1 << 6;

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_CONSTANTS_H
