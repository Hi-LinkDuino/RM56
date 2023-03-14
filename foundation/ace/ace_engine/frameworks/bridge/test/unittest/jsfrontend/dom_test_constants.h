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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_TEST_UNITTEST_JSFRONTEND_DOM_TEST_CONSTANTS_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_TEST_UNITTEST_JSFRONTEND_DOM_TEST_CONSTANTS_H

#include <vector>

#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace::Framework {

const std::string CHILD_TAG = "child";
const std::string COMMON_EVENT = "commonEvent";
const std::string COMMON_STYLE = "commonStyle";
const std::string DOM_ATTR = "attr";
const std::string DOM_EVENT = "event";
const std::string DOM_STYLE = "style";
const std::string DOM_TAG = "tag";
constexpr int32_t PAGE_ID = 1;

// common style list
const std::vector<std::string> COMMON_STYLE_LIST = {
    DOM_WIDTH,
    DOM_HEIGHT,
    DOM_PADDING_TOP,
    DOM_PADDING_RIGHT,
    DOM_PADDING_BOTTOM,
    DOM_PADDING_LEFT,
    DOM_MARGIN_TOP,
    DOM_MARGIN_RIGHT,
    DOM_MARGIN_BOTTOM,
    DOM_MARGIN_LEFT,
    DOM_BACKGROUND_COLOR,
    DOM_FLEX_DIRECTION,
    DOM_FLEX_ROW,
    DOM_FLEX_COLUMN,
    DOM_FLEX,
    DOM_FLEX_GROW,
    DOM_FLEX_SHRINK,
    DOM_FLEX_BASIS,
    DOM_BORDER_TOP_WIDTH,
    DOM_BORDER_RIGHT_WIDTH,
    DOM_BORDER_BOTTOM_WIDTH,
    DOM_BORDER_LEFT_WIDTH,
    DOM_BORDER_STYLE,
    DOM_BORDER_TOP_COLOR,
    DOM_BORDER_RIGHT_COLOR,
    DOM_BORDER_BOTTOM_COLOR,
    DOM_BORDER_LEFT_COLOR,
    DOM_BORDER_TOP_LEFT_RADIUS,
    DOM_BORDER_TOP_RIGHT_RADIUS,
    DOM_BORDER_BOTTOM_LEFT_RADIUS,
    DOM_BORDER_BOTTOM_RIGHT_RADIUS,
    DOM_BACKGROUND_COLOR,
    DOM_BACKGROUND_IMAGE,
    DOM_BACKGROUND_IMAGE_SIZE,
    DOM_BACKGROUND_IMAGE_POSITION,
    DOM_BACKGROUND_IMAGE_REPEAT,
    DOM_FOCUSABLE,
    DOM_FLEX_WEIGHT,
    DOM_BOX_SHADOW_H,
    DOM_BOX_SHADOW_V,
    DOM_BOX_SHADOW_BLUR,
    DOM_BOX_SHADOW_SPREAD,
    DOM_BOX_SHADOW_COLOR,
    DOM_FILTER,
    DOM_BACKDROP_FILTER,
    DOM_TRANSITION_EFFECT,
    DOM_WINDOW_FILTER,
    DOM_MASK_IMAGE,
    DOM_MASK_POSITION,
    DOM_MASK_SIZE,
    DOM_TRANSFORM_ORIGIN,
    DOM_TRANSFORM,
    DOM_CLIP_PATH,
};

// common event list
const std::vector<std::string> COMMON_EVENT_LIST = {
    DOM_TOUCH_START,
    DOM_TOUCH_MOVE,
    DOM_TOUCH_CANCEL,
    DOM_TOUCH_END,
    DOM_CLICK,
    DOM_LONG_PRESS,
    DOM_FOCUS,
    DOM_BLUR,
    DOM_KEY
};

// div node style list
const std::vector<std::string> DIV_STYLE_LIST = {
    DOM_DISPLAY,
    DOM_GRID_TEMPLATE_COLUMNS,
    DOM_GRID_TEMPLATE_ROWS,
    DOM_GRID_COLUMN_GAP,
    DOM_GRID_ROW_GAP,
    DOM_GRID_COLUMN_START,
    DOM_GRID_COLUMN_END,
    DOM_GRID_ROW_START,
    DOM_GRID_ROW_END,
    DOM_FLEX_DIRECTION,
    DOM_JUSTIFY_CONTENT,
    DOM_ALIGN_ITEMS,
};

// image node attribute list
const std::vector<std::string> IMAGE_ATTR_LIST = {
    DOM_SRC,
    DOM_IMAGE_ALT,
};

// image node style list
const std::vector<std::string> IMAGE_STYLE_LIST = {
    DOM_IMAGE_FIT,
};

// image node event list
const std::vector<std::string> IMAGE_EVENT_LIST = {
    DOM_ERROR,
    DOM_COMPLETE,
};

// imageanimator node attribute list
const std::vector<std::string> IMAGE_ANIMATOR_ATTR_LIST = {
    DOM_IMAGES,
    DOM_ITERATION,
    DOM_REVERSE,
    DOM_FIXEDSIZE,
};

// text node style list
const std::vector<std::string> TEXT_STYLE_LIST = {
    DOM_TEXT_MAX_LINES,
    DOM_TEXT_COLOR,
    DOM_TEXT_LETTER_SPACING,
    DOM_TEXT_FONT_SIZE,
    DOM_TEXT_FONT_STYLE,
    DOM_TEXT_FONT_WEIGHT,
    DOM_TEXT_DECORATION,
    DOM_TEXT_ALIGN,
    DOM_TEXT_LINE_HEIGHT,
    DOM_TEXT_OVERFLOW,
    DOM_TEXT_FONT_FAMILY,
    DOM_TEXT_LINES,
    DOM_TEXT_MIN_FONT_SIZE,
    DOM_TEXT_MAX_FONT_SIZE,
    DOM_TEXT_FONT_SIZE_STEP,
    DOM_TEXT_PREFER_FONT_SIZES,
    DOM_TEXT_FONT_VARIANT,
};

// text node attribute list
const std::vector<std::string> TEXT_ATTR_LIST = {
    DOM_VALUE,
};

// svg text node attribute list
const std::vector<std::string> SVG_TEXT_ATTR_LIST = {
    DOM_VALUE,
    DOM_SVG_X,
    DOM_SVG_Y,
    DOM_SVG_DX,
    DOM_SVG_DY,
    DOM_SVG_ROTATE,
    DOM_SVG_TEXT_LENGTH,
    DOM_SVG_LENGTH_ADJUST,
    DOM_SVG_STROKE,
    DOM_SVG_STROKE_OPACITY,
    DOM_SVG_STROKE_WIDTH,
    DOM_SVG_FONT_SIZE,
    DOM_SVG_FILL,
    DOM_SVG_FILL_OPACITY,
};

// svg textpath node attribute list
const std::vector<std::string> SVG_TEXT_PATH_ATTR_LIST = {
    DOM_VALUE,
    DOM_SVG_ATTR_PATH,
    DOM_SVG_START_OFFSET,
    DOM_SVG_TEXT_LENGTH,
    DOM_SVG_LENGTH_ADJUST,
    DOM_SVG_STROKE,
    DOM_SVG_STROKE_OPACITY,
    DOM_SVG_STROKE_WIDTH,
    DOM_SVG_FONT_SIZE,
    DOM_SVG_FILL,
    DOM_SVG_FILL_OPACITY,
};

// svg tspan node attribute list
const std::vector<std::string> SVG_TSPAN_ATTR_LIST = {
    DOM_VALUE,
    DOM_SVG_X,
    DOM_SVG_Y,
    DOM_SVG_DX,
    DOM_SVG_DY,
    DOM_SVG_ROTATE,
    DOM_SVG_TEXT_LENGTH,
    DOM_SVG_LENGTH_ADJUST,
    DOM_SVG_STROKE,
    DOM_SVG_STROKE_OPACITY,
    DOM_SVG_STROKE_WIDTH,
    DOM_SVG_FONT_SIZE,
    DOM_SVG_FILL,
    DOM_SVG_FILL_OPACITY,
};

// progress node style and attribute list
const std::vector<std::string> PROGRESS_STYLE_LIST = {
    DOM_PROGRESS_COLOR,
    DOM_PROGRESS_STROKE_WIDTH,
};

const std::vector<std::string> PROGRESS_ATTR_LIST = {
    DOM_PROGRESS_TYPE,
    DOM_PROGRESS_PERCENT,
    DOM_PROGRESS_SECONDARY_PERCENT,
};

// slider node style and attribute list
const std::vector<std::string> SLIDER_ATTR_LIST = {
    DOM_MIN,
    DOM_MAX,
    DOM_STEP,
    DOM_VALUE,
    DOM_SHOW_TIPS,
    DOM_SHOW_STEPS,
    DOM_SLIDER_MODE,
};
const std::vector<std::string> SLIDER_STYLE_LIST = {
    DOM_COLOR,
    DOM_SELECTED_COLOR,
    DOM_BLOCK_COLOR,
    DOM_PADDING_LEFT,
    DOM_PADDING_RIGHT,
};
const std::vector<std::string> SLIDER_EVENT_LIST = {
    DOM_CHANGE,
};
// swiper node style and attribute list
const std::vector<std::string> SWIPER_ATTR_LIST = {
    DOM_INDEX,
    DOM_AUTOPLAY,
    DOM_INTERVAL,
    DOM_INDICATOR,
    DOM_LOOP,
    DOM_DURATION,
    DOM_VERTICAL,
};
const std::vector<std::string> SWIPER_STYLE_LIST = {
    DOM_INDICATOR_COLOR,
    DOM_INDICATOR_SELECTEDCOLOR,
    DOM_INDICATOR_SIZE,
    DOM_INDICATOR_TOP,
    DOM_INDICATOR_LEFT,
    DOM_INDICATOR_RIGHT,
    DOM_INDICATOR_BOTTOM,
};
const std::vector<std::string> SWIPER_EVENT_LIST = {
    DOM_CHANGE,
};

// refresh node style and attribute list
const std::vector<std::string> REFRESH_ATTR_LIST = {
    DOM_REFRESH_OFFSET,
    DOM_REFRESH_REFRESHING,
    DOM_REFRESH_TYPE,
    DOM_REFRESH_LASTTIME,
};

const std::vector<std::string> REFRESH_STYLE_LIST = {
    DOM_REFRESH_BACKGROUND_COLOR,
    DOM_REFRESH_PROGRESS_COLOR,
};

const std::vector<std::string> REFRESH_EVENT_LIST = {
    DOM_REFRESH,
};

// switch node attribute, style and event list
const std::vector<std::string> SWITCH_ATTR_LIST = {
    DOM_CHECKED,
    DOM_DISABLED,
    DOM_SHOW_TEXT,
    DOM_TEXT_ON,
    DOM_TEXT_OFF,
};
const std::vector<std::string> SWITCH_STYLE_LIST = {
    DOM_TEXT_FONT_SIZE,
    DOM_TEXT_FONT_WEIGHT,
    DOM_TEXT_FONT_FAMILY,
    DOM_TEXT_DECORATION,
    DOM_TEXT_LETTER_SPACING,
    DOM_TEXT_FONT_STYLE,
    DOM_TEXT_PADDING,
    DOM_TEXT_ON_COLOR,
    DOM_TEXT_OFF_COLOR,
    DOM_TRACK_COLOR_ON,
    DOM_TRACK_COLOR_OFF,
};
const std::vector<std::string> SWITCH_EVENT_LIST = {
    DOM_CHANGE
};

// span node style list
const std::vector<std::string> SPAN_STYLE_LIST = {
    DOM_TEXT_COLOR,
    DOM_TEXT_FONT_SIZE,
    DOM_TEXT_FONT_STYLE,
    DOM_TEXT_FONT_WEIGHT,
    DOM_TEXT_DECORATION,
    DOM_TEXT_FONT_FAMILY,
    DOM_TEXT_FONT_VARIANT,
};

// span node attribute list
const std::vector<std::string> SPAN_ATTR_LIST = {
    DOM_VALUE,
};

// popup node attribute list
const std::vector<std::string> POPUP_ATTR_LIST = {
    DOM_TARGET,
    DOM_PLACEMENT,
};

// popup node style list
const std::vector<std::string> POPUP_STYLE_LIST = {
    DOM_MASK_COLOR,
};

// popup node event list
const std::vector<std::string> POPUP_EVENT_LIST = {
    DOM_VISIBILITY_CHANGE,
};

// list node attribute list
const std::vector<std::string> LIST_ATTR_LIST = {
    DOM_SCROLL_EFFECT,
    DOM_SCROLL_SCROLLBAR,
    DOM_SCROLL_SHAPE_MODE,
    DOM_LIST_UPDATE_EFFECT,
};

// list node style list
const std::vector<std::string> LIST_STYLE_LIST = {
    DOM_FADE_COLOR,
    DOM_LIST_ITEM_EXTENT,
};

// tabs node attribute list
const std::vector<std::string> TABS_ATTR_LIST = {
    TAB_INDEX,
    TAB_IS_VERTICAL,
};

// tab-bar node attribute list
const std::vector<std::string> TABBAR_ATTR_LIST = {
    DOM_TAB_BAR_MODE,
};

// tab-content node attribute list
const std::vector<std::string> TABCONTENT_ATTR_LIST = {
    DOM_TAB_CONTENT_SCROLLABEL,
};

// video node attribute, style and event list
const std::vector<std::string> VIDEO_ATTR_LIST = {
    DOM_VIDEO_MUTED,
    DOM_VIDEO_SRC,
    DOM_VIDEO_AUTOPLAY,
    DOM_VIDEO_POSTER,
    DOM_VIDEO_CONTROLS,
};
const std::vector<std::string> VIDEO_STYLE_LIST = {
    DOM_VIDEO_FIT,
};
const std::vector<std::string> VIDEO_EVENT_LIST = {
    DOM_VIDEO_EVENT_PREPARED,
    DOM_VIDEO_EVENT_START,
    DOM_VIDEO_EVENT_PAUSE,
    DOM_VIDEO_EVENT_FINISH,
    DOM_VIDEO_EVENT_ERROR,
    DOM_VIDEO_EVENT_SEEKING,
    DOM_VIDEO_EVENT_SEEKED,
    DOM_VIDEO_EVENT_TIMEUPDATE,
    DOM_VIDEO_EVENT_FULLSCREENCHANGE,
};
const std::vector<std::string> RATING_ATTR_LIST = {
    DOM_STAR_NUM,
    DOM_RATING_SCORE,
    DOM_RATING_STEP,
    DOM_RATING_INDICATOR,
};
const std::vector<std::string> RATING_STYLE_LIST = {
    DOM_BACKGROUND_SRC,
    DOM_FOREGROUND_SRC,
    DOM_SECONDARY_SRC,
    DOM_WIDTH,
    DOM_HEIGHT,
};
const std::vector<std::string> RATING_EVENT_LIST = {
    DOM_CHANGE,
};

// marquee node attribute, style and event list
const std::vector<std::string> MARQUEE_ATTR_LIST = {
    DOM_MARQUEE_VALUE,
    DOM_MARQUEE_LOOP,
    DOM_MARQUEE_SCROLL_AMOUNT,
    DOM_MARQUEE_DIRECTION,
};
const std::vector<std::string> MARQUEE_STYLE_LIST = {
    DOM_MARQUEE_COLOR,
    DOM_MARQUEE_FONT_SIZE,
    DOM_MARQUEE_FONT_WEIGHT,
    DOM_MARQUEE_FONT_FAMILY,
};
const std::vector<std::string> MARQUEE_EVENT_LIST = {
    DOM_MARQUEE_EVENT_BOUNCE,
    DOM_MARQUEE_EVENT_FINISH,
    DOM_MARQUEE_EVENT_START,
};

// textarea node style list
const std::vector<std::string> TEXTAREA_STYLE_LIST = {
    DOM_TEXTAREA_COLOR,
    DOM_TEXTAREA_FONT_SIZE,
    DOM_TEXTAREA_FONT_WEIGHT,
    DOM_TEXTAREA_PLACEHOLDER_COLOR,
    DOM_CARET_COLOR,
    DOM_TEXTAREA_FONT_FAMILY,
    DOM_TEXTAREA_CURSOR_COLOR,
};

// textarea node attribute list
const std::vector<std::string> TEXTAREA_ATTR_LIST = {
    DOM_DISABLED,
    DOM_TEXTAREA_OBSCURE,
    DOM_TEXTAREA_VALUE,
    DOM_TEXTAREA_PLACEHOLDER,
    DOM_TEXTAREA_MAXLENGTH,
    DOM_TEXTAREA_MAXLINES,
    DOM_TEXTAREA_OBSCURE,
    DOM_TEXTAREA_EXTEND,
    DOM_ICON_SRC,
    DOM_INPUT_SELECTED_START,
    DOM_INPUT_SELECTED_END,
    DOM_INPUT_SOFT_KEYBOARD_ENABLED,
};

// textarea node event list
const std::vector<std::string> TEXTAREA_EVENT_LIST = {
    DOM_CHANGE,
};

// input node style list
const std::vector<std::string> INPUT_STYLE_LIST = {
    DOM_INPUT_COLOR,
    DOM_INPUT_FONT_SIZE,
    DOM_INPUT_FONT_WEIGHT,
    DOM_INPUT_CURSOR_COLOR,
    DOM_INPUT_PLACEHOLDER_COLOR,
    DOM_INPUT_WIDTH,
    DOM_INPUT_HEIGHT,
    DOM_INPUT_FONT_FAMILY,
    DOM_INPUT_RECT_RADIUS,
    DOM_INPUT_BACKGROUND_COLOR,
    DOM_INPUT_CLICKED_COLOR,
    DOM_INPUT_DISABLE_COLOR,
    DOM_INPUT_FOCUS_COLOR,
};

// input node attribute list
const std::vector<std::string> INPUT_ATTR_LIST = {
    DOM_DISABLED,
    DOM_INPUT_TYPE,
    DOM_INPUT_CHECKED,
    DOM_INPUT_NAME,
    DOM_INPUT_VALUE,
    DOM_INPUT_PLACEHOLDER,
    DOM_INPUT_MAXLENGTH,
    DOM_INPUT_ENTERKEYTYPE,
    DOM_INPUT_OBSCURE,
    DOM_INPUT_AUTO_FOCUS,
    DOM_ICON_SRC,
};

// input node event list
const std::vector<std::string> INPUT_EVENT_LIST = {
    DOM_CHANGE,
    DOM_CLICK,
    DOM_FOCUS,
    DOM_INPUT_EVENT_NAME,
    DOM_INPUT_EVENT_VALUE,
    DOM_INPUT_EVENT_CHECKED,
    DOM_INPUT_EVENT_ENTERKEYCLICK,
};

// label node attribute list
const std::vector<std::string> LABEL_ATTR_LIST = {
    DOM_VALUE,
    DOM_TARGET,
};

// input node style list
const std::vector<std::string> DIVIDER_STYLE_LIST = {
    DOM_DIVIDER_STROKE_WIDTH,
    DIVIDER_COLOR,
    DOM_DIVIDER_LINE_CAP,
};

// input node attribute list
const std::vector<std::string> DIVIDER_ATTR_LIST = {
    DOM_DIVIDER_VERTICAL,
};

// button node attribute list
const std::vector<std::string> BUTTON_ATTR_LIST = {
    DOM_DISABLED,
    DOM_BUTTON_TYPE,
    DOM_BUTTON_WAITING,
    DOM_BUTTON_AUTO_FOCUS,
    DOM_BUTTON_TEXT_DATA,
    DOM_BUTTON_ICON,
};

// button node style list
const std::vector<std::string> BUTTON_STYLE_LIST = {
    DOM_BUTTON_MIN_WIDTH,
    DOM_BUTTON_WIDTH,
    DOM_BUTTON_HEIGHT,
    DOM_BUTTON_TEXT_COLOR,
    DOM_BUTTON_FONT_SIZE,
    DOM_BUTTON_FONT_WEIGHT,
    DOM_BUTTON_FONT_FAMILY,
    DOM_BUTTON_RRECT_RADIUS,
    DOM_BUTTON_DEFAULT_COLOR,
    DOM_BUTTON_CLICKED_COLOR,
    DOM_BUTTON_DISABLE_COLOR,
    DOM_BUTTON_FOCUS_COLOR,
    BUTTON_SHADOW,
    DOM_BUTTON_BORDER_WIDTH,
    DOM_BUTTON_BORDER_COLOR,
    DOM_BUTTON_PROGRESS_DIAMETER,
    DOM_BUTTON_PROGRESS_COLOR,
    DOM_BUTTON_INNER_PADDING,
};

const std::vector<std::string> SEARCH_ATTR_LIST = {
    DOM_SEARCH_ICON,
    DOM_SEARCH_HINT,
    DOM_SEARCH_VALUE,
    DOM_INPUT_SELECTED_START,
    DOM_INPUT_SELECTED_END,
    DOM_INPUT_SOFT_KEYBOARD_ENABLED,
};

const std::vector<std::string> SEARCH_STYLE_LIST = {
    DOM_TEXT_FONT_SIZE,
    DOM_TEXT_FONT_WEIGHT,
    DOM_TEXT_FONT_FAMILY,
    DOM_COLOR,
    DOM_INPUT_PLACEHOLDER_COLOR,
    DOM_CARET_COLOR,
    DOM_PADDING_LEFT,
    DOM_PADDING_TOP,
    DOM_PADDING_RIGHT,
    DOM_PADDING_BOTTOM,
    DOM_PADDING_START,
    DOM_PADDING_END,
    DOM_BACKGROUND_COLOR,
};

const std::vector<std::string> SEARCH_EVENT_LIST = {
    DOM_CHANGE,
    DOM_SUBMIT
};

// piece node attribute list
const std::vector<std::string> PIECE_ATTR_LIST = {
    DOM_PIECE_CONTENT,
    DOM_PIECE_CLOSABLE,
    DOM_PIECE_ICON,
};

// piece node event list
const std::vector<std::string> PIECE_EVENT_LIST = {
    DOM_PIECE_EVENT_CLOSE,
};

const std::vector<std::string> DIV_ATTR_LIST = {
    DOM_CLICK_EFFECT,
};

// svg node attribute list
const std::vector<std::string> SVG_ATTR_LIST = {
    DOM_SVG_VIEW_BOX,
    DOM_SVG_X,
    DOM_SVG_Y,
    DOM_SVG_WIDTH,
    DOM_SVG_HEIGHT,
    DOM_SVG_FILL,
    DOM_SVG_FILL_OPACITY,
    DOM_SVG_STROKE,
    DOM_SVG_STROKE_DASHARRAY,
    DOM_SVG_STROKE_DASHOFFSET,
    DOM_SVG_STROKE_LINECAP,
    DOM_SVG_STROKE_LINEJOIN,
    DOM_SVG_STROKE_MITERLIMIT,
    DOM_SVG_STROKE_OPACITY,
    DOM_SVG_STROKE_WIDTH,
    DOM_SVG_OPACITY,
};

const std::vector<std::string> SVG_RECT_ATTR_LIST = {
    DOM_SVG_X,
    DOM_SVG_Y,
    DOM_SVG_WIDTH,
    DOM_SVG_HEIGHT,
    DOM_SVG_RX,
    DOM_SVG_RY,
    DOM_SVG_FILL,
    DOM_SVG_FILL_OPACITY,
    DOM_SVG_STROKE,
    DOM_SVG_STROKE_DASHARRAY,
    DOM_SVG_STROKE_DASHOFFSET,
    DOM_SVG_STROKE_LINECAP,
    DOM_SVG_STROKE_LINEJOIN,
    DOM_SVG_STROKE_MITERLIMIT,
    DOM_SVG_STROKE_OPACITY,
    DOM_SVG_STROKE_WIDTH,
    DOM_SVG_OPACITY,
};

const std::vector<std::string> SVG_CIRCLE_ATTR_LIST = {
    DOM_SVG_CX,
    DOM_SVG_CY,
    DOM_SVG_R,
    DOM_SVG_FILL,
    DOM_SVG_FILL_OPACITY,
    DOM_SVG_STROKE,
    DOM_SVG_STROKE_DASHARRAY,
    DOM_SVG_STROKE_DASHOFFSET,
    DOM_SVG_STROKE_LINECAP,
    DOM_SVG_STROKE_LINEJOIN,
    DOM_SVG_STROKE_MITERLIMIT,
    DOM_SVG_STROKE_OPACITY,
    DOM_SVG_STROKE_WIDTH,
    DOM_SVG_OPACITY,
};

const std::vector<std::string> SVG_ELLIPSE_ATTR_LIST = {
    DOM_SVG_CX,
    DOM_SVG_CY,
    DOM_SVG_RX,
    DOM_SVG_RY,
    DOM_SVG_FILL,
    DOM_SVG_FILL_OPACITY,
    DOM_SVG_STROKE,
    DOM_SVG_STROKE_DASHARRAY,
    DOM_SVG_STROKE_DASHOFFSET,
    DOM_SVG_STROKE_LINECAP,
    DOM_SVG_STROKE_LINEJOIN,
    DOM_SVG_STROKE_MITERLIMIT,
    DOM_SVG_STROKE_OPACITY,
    DOM_SVG_STROKE_WIDTH,
    DOM_SVG_OPACITY,
};

const std::vector<std::string> SVG_PATH_ATTR_LIST = {
    DOM_SVG_D,
    DOM_SVG_FILL,
    DOM_SVG_FILL_OPACITY,
    DOM_SVG_STROKE,
    DOM_SVG_STROKE_DASHARRAY,
    DOM_SVG_STROKE_DASHOFFSET,
    DOM_SVG_STROKE_LINECAP,
    DOM_SVG_STROKE_LINEJOIN,
    DOM_SVG_STROKE_MITERLIMIT,
    DOM_SVG_STROKE_OPACITY,
    DOM_SVG_STROKE_WIDTH,
    DOM_SVG_OPACITY,
};

const std::vector<std::string> SVG_LINE_ATTR_LIST = {
    DOM_SVG_X1,
    DOM_SVG_X2,
    DOM_SVG_Y1,
    DOM_SVG_Y2,
    DOM_SVG_FILL,
    DOM_SVG_FILL_OPACITY,
    DOM_SVG_STROKE,
    DOM_SVG_STROKE_DASHARRAY,
    DOM_SVG_STROKE_DASHOFFSET,
    DOM_SVG_STROKE_LINECAP,
    DOM_SVG_STROKE_LINEJOIN,
    DOM_SVG_STROKE_MITERLIMIT,
    DOM_SVG_STROKE_OPACITY,
    DOM_SVG_STROKE_WIDTH,
    DOM_SVG_OPACITY,
};

const std::vector<std::string> SVG_POLYLINE_ATTR_LIST = {
    DOM_SVG_POINTS,
    DOM_SVG_FILL,
    DOM_SVG_FILL_OPACITY,
    DOM_SVG_STROKE,
    DOM_SVG_STROKE_DASHARRAY,
    DOM_SVG_STROKE_DASHOFFSET,
    DOM_SVG_STROKE_LINECAP,
    DOM_SVG_STROKE_LINEJOIN,
    DOM_SVG_STROKE_MITERLIMIT,
    DOM_SVG_STROKE_OPACITY,
    DOM_SVG_STROKE_WIDTH,
    DOM_SVG_OPACITY,
};

const std::vector<std::string> SVG_POLYGON_ATTR_LIST = {
    DOM_SVG_POINTS,
    DOM_SVG_FILL,
    DOM_SVG_FILL_OPACITY,
    DOM_SVG_STROKE,
    DOM_SVG_STROKE_DASHARRAY,
    DOM_SVG_STROKE_DASHOFFSET,
    DOM_SVG_STROKE_LINECAP,
    DOM_SVG_STROKE_LINEJOIN,
    DOM_SVG_STROKE_MITERLIMIT,
    DOM_SVG_STROKE_OPACITY,
    DOM_SVG_STROKE_WIDTH,
    DOM_SVG_OPACITY,
};

const std::vector<std::string> SVG_ANIMATE_ATTR_LIST = {
    DOM_SVG_ANIMATION_BEGIN,
    DOM_SVG_ANIMATION_DUR,
    DOM_SVG_ANIMATION_END,
    DOM_SVG_ANIMATION_REPEAT_COUNT,
    DOM_SVG_ANIMATION_FILL,
    DOM_SVG_ANIMATION_CALC_MODE,
    DOM_SVG_ANIMATION_VALUES,
    DOM_SVG_ANIMATION_KEY_TIMES,
    DOM_SVG_ANIMATION_KEY_SPLINES,
    DOM_SVG_ANIMATION_FROM,
    DOM_SVG_ANIMATION_TO,
    DOM_SVG_ANIMATION_ATTRIBUTE_NAME,
};

const std::vector<std::string> SVG_ANIMATE_MOTION_ATTR_LIST = {
    DOM_SVG_ANIMATION_BEGIN,
    DOM_SVG_ANIMATION_DUR,
    DOM_SVG_ANIMATION_END,
    DOM_SVG_ANIMATION_REPEAT_COUNT,
    DOM_SVG_ANIMATION_FILL,
    DOM_SVG_ANIMATION_CALC_MODE,
    DOM_SVG_ANIMATION_KEY_TIMES,
    DOM_SVG_ANIMATION_KEY_SPLINES,
    DOM_SVG_ANIMATION_KEY_POINTS,
    DOM_SVG_ANIMATION_PATH,
    DOM_SVG_ANIMATION_ROTATE,
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_TEST_UNITTEST_JSFRONTEND_DOM_TEST_CONSTANTS_H
