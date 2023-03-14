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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_CONSTANTS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_CONSTANTS_H

#include <cstdint>

#include "frameworks/base/utils/macros.h"

namespace OHOS::Ace {

// base constants
extern const char CLASS_NAME[];
extern const char NAME[];
extern const char CHILD[];

// common constants
extern const char DISABLED[];
extern const char WIDTH[];
extern const char HEIGHT[];
extern const char ALIGN[];

// image constants
extern const char IMAGE_NAME[];
extern const char IMAGE_SRC[];
extern const char IMAGE_ALT[];
extern const char IMAGE_WIDTH[];
extern const char IMAGE_HEIGHT[];
extern const char IMAGE_ALIGNMENT[];
extern const char IMAGE_POSITION[];
extern const char IMAGE_POSITION_SIZE_X[];
extern const char IMAGE_POSITION_SIZE_Y[];
extern const char IMAGE_POSITION_VALUE_X[];
extern const char IMAGE_POSITION_VALUE_Y[];
extern const char IMAGE_SIZE[];
extern const char IMAGE_SIZE_VALUE[];
extern const char IMAGE_SIZE_TYPE[];
extern const char IMAGE_REPEAT[];
extern const char IMAGE_FIT[];
extern const char IMAGE_COLOR[];
extern const char IMAGE_ON_LOAD_SUCCESS[];
extern const char IMAGE_ON_LOAD_FAIL[];
extern const char IMAGE_ALT_BLANK[];
extern const uint32_t ALT_COLOR_GREY;

// text constants
extern const char TEXT_NAME[];
extern const char TEXT_DATA[];
extern const char TEXT_DIRECTION[];
extern const char TEXT_COLOR[];
extern const char TEXT_DECORATION[];
extern const char TEXT_FONT_WEIGHT[];
extern const char TEXT_FONT_SIZE[];
extern const char TEXT_FONT_STYLE[];
extern const char TEXT_BASE_LINE[];
extern const char TEXT_FONT_FAMILIES[];
extern const char TEXT_MAX_LINES[];
extern const char TEXT_ALIGN[];
extern const char TEXT_OVERFLOW[];
extern const char TEXT_WORD_SPACING[];
extern const char TEXT_LINE_HEIGHT[];
extern const char TEXT_LETTER_SPACING[];
extern const char TEXT_STYLE[];
extern const char TEXT_SPAN_NAME[];
extern const char TEXT_ADAPT[];
extern const char TEXT_ADAPT_MIN_FONTSIZE[];

// text style constants
extern const char TEXT_STYLE_NAME[];

// textField constants
extern const char TEXT_FIELD_NAME[];
extern const char TEXT_FIELD_TYPE[];
extern const char OBSCURE_TEXT[];
extern const char TEXT_FIELD_ENABLED[];
extern const char TEXT_FIELD_MAX_LENGTH[];
extern const char TEXT_FIELD_MAX_LINES[];
extern const char TEXT_FIELD_AUTO_FOCUS[];
extern const char TEXT_FIELD_ACTION_LABEL[];
extern const char TEXT_FIELD_ACTION[];
extern const char CURSOR_COLOR[];
extern const char TEXT_FIELD_PLACEHOLDER[];
extern const char TEXT_FIELD_PLACEHOLDER_COLOR[];
extern const char TEXT_FIELD_EXTEND[];
extern const char TEXT_FIELD_ICON_IMAGE[];
// events
extern const char TEXT_FIELD_ON_TEXT_CHANGE[];
extern const char TEXT_FIELD_ON_FINISH_INPUT[];
extern const char TEXT_FIELD_ON_TAP[];

// color constants
extern const char COLOR[];
extern const char COLOR_NAME[];
extern const char COLOR_VALUE[];
extern const char COLOR_RED[];
extern const char COLOR_GREEN[];
extern const char COLOR_BLUE[];
extern const char COLOR_ALPHA[];
extern const char COLOR_OPACITY[];

// shadow constants
extern const char SHADOW[];
extern const char OFFSET[];
extern const char HORIZONTAL_OFFSET[];
extern const char VERTICAL_OFFSET[];
extern const char RADIUS[];
extern const char SPREAD[];
extern const char ELEVATION[];

// compose constants
extern const char COMPOSE_NAME[];
extern const char COMPOSE_ID[];
extern const char COMPOSE_DATA[];

// box constants
extern const char BOX_NAME[];
extern const char BOX_BACK_DECORATION[];
extern const char BOX_FRONT_DECORATION[];

// flex constants
extern const char COLUMN_NAME[];
extern const char ROW_NAME[];
extern const char MAIN_AXIS_ALIGN[];
extern const char CROSS_AXIS_ALIGN[];
extern const char MAIN_AXIS_SIZE[];

// flex item constants
extern const char FLEX_ITEM_NAME[];
extern const char FLEX_GROW[];
extern const char FLEX_SHRINK[];
extern const char FLEX_BASIS[];

// stack constants
extern const char STACK_NAME[];
extern const char STACK_ALIGNMENT[];
extern const char STACK_FIT[];
extern const char STACK_OVERFLOW[];
extern const char STACK_MAIN_STACK_SIZE[];

// positioned constants
extern const char POSITIONED_NAME[];
extern const char POSITIONED_LEFT[];
extern const char POSITIONED_RIGHT[];
extern const char POSITIONED_HEIGHT[];
extern const char POSITIONED_WIDTH[];
extern const char POSITIONED_TOP[];
extern const char POSITIONED_BOTTOM[];

// list constants
extern const char LIST_NAME[];
extern const char LIST_COUNT[];
extern const char LIST_EVENT_REQUEST_ITEM[];
extern const char LIST_DIRECTION[];
extern const char LIST_CACHED_COUNT[];
extern const char LIST_BEGIN_INDEX[];
extern const char LIST_END_INDEX[];
extern const char LIST_REPEATED_LENGTH[];
extern const char LIST_INDEX_OFFSET[];
extern const char LIST_SCROLL_PAGE[];
extern const char LIST_POSITION[];
// list item constants
extern const char LIST_ITEM_NAME[];
extern const char LIST_ITEM_TYPE[];
// edge effect
extern const char EDGE_EFFECT[];
extern const char FADE_COLOR[];
// grid constants
extern const char GRID_NAME[];
extern const char GRID_WIDTH[];
extern const char GRID_HEIGHT[];
extern const char GRID_ITEM_COUNT[];
extern const char GRID_INITIAL_ITEM[];
extern const char GRID_COLUMN_COUNT[];
extern const char GRID_COLUMN_EXTENT[];
extern const char GRID_EVENT_REQUEST_ITEM[];
extern const char GRID_SCROLL_PAGE[];
extern const char GRID_DIRECTION[];
extern const char GRID_ALIGN[];
extern const char GRID_CACHED_COUNT[];
extern const char GRID_ITEM_NAME[];
extern const char GRID_COLUMN_SPAN[];
extern const char GRID_ROW_SPAN[];
extern const char GRID_ROW_COUNT[];
extern const char GRID_ROW_INDEX[];
extern const char GRID_COLUMN_INDEX[];
extern const char GRID_LAYOUT_NAME[];
extern const char GRID_LAYOUT_ITEM_NAME[];
extern const int32_t DEFAULT_DIRECTION; // FlexDirection::COLUMN
extern const int32_t DEFAULT_ALIGN;     // FlexAlign::STRETCH
ACE_EXPORT extern const int32_t DEFAULT_COLUMN_COUNT;
ACE_EXPORT extern const int32_t DEFAULT_COLUMN_EXTENT;
extern const int32_t DEFAULT_COLUMN_SPAN;
extern const int32_t DEFAULT_ITEM_COUNT;
extern const int32_t DEFAULT_INITIAL_ITEM;
extern const int32_t DEFAULT_CACHED_COUNT;
ACE_EXPORT extern const double DEFAULT_GRID_WIDTH;
ACE_EXPORT extern const double DEFAULT_GRID_HEIGHT;
// scrollBar
extern const char SCROLL_BAR_DISPLAY_MODE[];
extern const char SCROLL_BAR_SHAPE_MODE[];

// edge constants
extern const char PADDING_NAME[];
extern const char PADDING[];
extern const char MARGIN[];
extern const char EDGE_NAME[];
extern const char EDGE_VALUE[];
extern const char EDGE_LEFT[];
extern const char EDGE_TOP[];
extern const char EDGE_RIGHT[];
extern const char EDGE_BOTTOM[];

// border constants
extern const char BORDER_NAME[];
extern const char BORDER_VALUE[];
extern const char BORDER_LEFT[];
extern const char BORDER_TOP[];
extern const char BORDER_RIGHT[];
extern const char BORDER_BOTTOM[];
extern const char BORDER_RADIUS_VALUE[];
extern const char BORDER_TOP_LEFT[];
extern const char BORDER_TOP_RIGHT[];
extern const char BORDER_BOTTOM_LEFT[];
extern const char BORDER_BOTTOM_RIGHT[];

// borderEdge constants
extern const char BORDER_EDGE_NAME[];
extern const char BORDER_EDGE_WIDTH[];
extern const char BORDER_EDGE_COLOR[];
extern const char BORDER_EDGE_STYLE[];

// radius constants
extern const char RADIUS_NAME[];
extern const char RADIUS_VALUE[];
extern const char RADIUS_X[];
extern const char RADIUS_Y[];

// decoration constants
extern const char DECORATION_NAME[];
extern const char SHADOW_LIST[];
extern const char BACKGROUND_IMAGE[];
extern const char BACKGROUND_COLOR[];
extern const char DECORATION_BORDER[];
extern const char DECORATION_PADDING[];

// backgroundImage constants
extern const char BACKGROUND_IMAGE_NAME[];
extern const char BACKGROUND_SRC[];

// gradient constants
extern const char GRADIENT_NAME[];
extern const char GRADIENT_DIRECTION[];
extern const char GRADIENT_REPEAT[];
extern const char GRADIENT_ANGLE[];
extern const char GRADIENT_COLOR_LIST[];
extern const char GRADIENT_DIMENSION[];
extern const char GRADIENT_DIMENSION_VALUE[];
extern const char GRADIENT_DIMENSION_UNIT[];
extern const char GRADIENT_COLOR[];
extern const char GRADIENT_HAS_VALUE[];

// align constants
extern const char ALIGN_NAME[];
extern const char HORIZONTAL[];
extern const char VERTICAL[];
extern const char HORIZONTAL_VALUE_LEFT[];
extern const char HORIZONTAL_VALUE_CENTER[];
extern const char HORIZONTAL_VALUE_RIGHT[];
extern const char VERTICAL_VALUE_TOP[];
extern const char VERTICAL_VALUE_CENTER[];
extern const char VERTICAL_VALUE_BOTTOM[];

// tween constants
extern const char TWEEN_NAME[];
extern const char DURATION[];
extern const char DELAY[];
extern const char REVERSE[];
extern const char ITERATION[];
extern const char FILL_MODE[];
extern const char CURVE[];
extern const char TWEEN_OPTION[];
extern const char OPTION_SCALE[];
extern const char OPTION_SCALE_X[];
extern const char OPTION_SCALE_Y[];
extern const char OPTION_TRANSLATE[];
extern const char OPTION_TRANSLATE_X[];
extern const char OPTION_TRANSLATE_Y[];
extern const char OPTION_ROTATE_Z[];
extern const char OPTION_ROTATE_X[];
extern const char OPTION_ROTATE_Y[];

// slider constants
extern const char SLIDER_NAME[];
extern const char VALUE[];
extern const char SLIDER_MIN[];
extern const char SLIDER_MAX[];
extern const char SLIDER_STEP[];
extern const char SLIDER_SELECTED_COLOR[];
extern const char SLIDER_BAR_BACKGROUND_COLOR[];
extern const char SLIDER_BLOCK_COLOR[];

// swiper constants
extern const char SWIPER_NAME[];
extern const char SWIPER_ON_CHANGED[];
extern const char SWIPER_INDEX[];
extern const char SWIPER_SHOW_INDICATOR[];
extern const char SWIPER_DURATION[];
extern const char SWIPER_LOOP[];
extern const char SWIPER_AUTO_PLAY[];
extern const char SWIPER_AUTO_PLAY_INTERVAL[];
extern const char SWIPER_IS_VERTICAL[];
extern const char SWIPER_INDICATOR_COLOR[];
extern const char SWIPER_INDICATOR_SELECTED_COLOR[];
extern const char SWIPER_INDICATOR_SIZE[];
extern const char SWIPER_INDICATOR_SELECTED_SIZE[];
extern const char SWIPER_INDICATOR_TOP[];
extern const char SWIPER_INDICATOR_LEFT[];
extern const char SWIPER_INDICATOR_BOTTOM[];
extern const char SWIPER_INDICATOR_RIGHT[];
extern const char ON_MOVE_END_ID[];

// button constants
extern const char BUTTON_NAME[];
extern const char BUTTON_WIDTH[];
extern const char BUTTON_HEIGHT[];
extern const char BUTTON_RECT_RADIUS[];
extern const char BUTTON_TYPE[];
extern const char BUTTON_DISABLED_STATE[];
extern const char BUTTON_AUTO_FOCUS[];
extern const char BUTTON_BACKGROUND_COLOR[];
extern const char BUTTON_CLICKED_COLOR[];
extern const char BUTTON_DISABLED_COLOR[];
extern const char BUTTON_FOCUS_COLOR[];
extern const char BUTTON_CLICKED_EVENT_ID[];
extern const char BUTTON_BORDER[];
extern const char BUTTON_SHADOW[];

// raw event components.
extern const char TOUCH_LISTENER_NAME[];
extern const char ON_TOUCH_DOWN_EVENT[];
extern const char ON_TOUCH_MOVE_EVENT[];
extern const char ON_TOUCH_UP_EVENT[];
extern const char ON_TOUCH_CANCEL_EVENT[];

// gesture event components.
extern const char GESTURE_LISTENER_NAME[];
extern const char ON_CLICK_EVENT[];
extern const char ON_LONG_PRESS_EVENT[];
extern const char ON_HORIZONTAL_DRAG_START_EVENT[];
extern const char ON_HORIZONTAL_DRAG_UPDATE_EVENT[];
extern const char ON_HORIZONTAL_DRAG_END_EVENT[];
extern const char ON_HORIZONTAL_DRAG_CANCEL_EVENT[];
extern const char ON_VERTICAL_DRAG_START_EVENT[];
extern const char ON_VERTICAL_DRAG_UPDATE_EVENT[];
extern const char ON_VERTICAL_DRAG_END_EVENT[];
extern const char ON_VERTICAL_DRAG_CANCEL_EVENT[];
extern const char ON_FREE_DRAG_START_EVENT[];
extern const char ON_FREE_DRAG_UPDATE_EVENT[];
extern const char ON_FREE_DRAG_END_EVENT[];
extern const char ON_FREE_DRAG_CANCEL_EVENT[];

// focusable components
extern const char FOCUSABLE_NAME[];
extern const char ON_FOCUS_EVENT[];
extern const char ON_BLUR_EVENT[];
extern const char ON_KEY_EVENT[];
extern const char BOX_STYLE[];
extern const char FOCUSED_BOX_STYLE[];
extern const char AUTO_FOCUSED[];

// texture constants
extern const char TEXTURE_NAME[];
extern const char TEXTURE_SRC_ID[];
extern const char TEXTURE_SRC_WIDTH[];
extern const char TEXTURE_SRC_HEIGHT[];
extern const char TEXTURE_FIT[];

// display constants
extern const char DISPLAY_NAME[];
extern const char DISPLAY_VISIBLE[];
extern const char DISPLAY_OPACITY[];

// tab constants
extern const char TAB_BAR_NAME[];
extern const char TAB_CONTENT_NAME[];
extern const char TAB_NAME[];
extern const char TAB_TABS[];
extern const char TAB_INDEX[];
extern const char TAB_IS_VERTICAL[];
extern const char TAB_CONTROLLER_ID[];
extern const char TAB_INDICATOR[];
extern const char TAB_INDICATOR_WIDTH[];
extern const char TAB_CONTENT_ON_CHANGE[];
extern const char TAB_CONTENT_SCROLLABEL[];
extern const char TAB_INDICATOR_COLOR[];
extern const char TAB_INDICATOR_PADDING[];
extern const char TAB_INDICATOR_SIZE[];
extern const char TAB_LABEL_PADDING[];
extern const char TAB_MODE[];
extern const char TAB_ITEM_TEXT[];
extern const char TAB_ITEM_ICON[];

// dialog constants
extern const char DIALOG_NAME[];
extern const char DIALOG_TITLE[];
extern const char DIALOG_TITLE_PADDING[];
extern const char DIALOG_CONTENT[];
extern const char DIALOG_CONTENT_PADDING[];
extern const char DIALOG_BACKGROUND_COLOR[];
extern const char DIALOG_ACTIONS[];
extern const char DIALOG_AUTO_CANCEL[];

// scroll constants
extern const char SCROLL_NAME[];
extern const char SCROLL_DIRECTION[];
extern const char SCROLL_POSITION[];
extern const char SCROLL_PADDING[];
extern const char SCROLL_PAGE[];

// progress constants
extern const char PROGRESS_NAME[];
extern const char PROGRESS_SELECT_COLOR[];
extern const char PROGRESS_CACHED_COLOR[];
extern const char PROGRESS_BACKGROUND_COLOR[];
extern const char PROGRESS_VALUE[];
extern const char PROGRESS_CACHED_VALUE[];
extern const char PROGRESS_MAX[];
extern const char PROGRESS_MIN[];
extern const char PROGRESS_THICKNESS[];
extern const char PROGRESS_TYPE[];

// transform constants
extern const char TRANSFORM_NAME[];
extern const char SCALE_VALUE[];
extern const char ANGLE_VALUE[];
extern const char ORIGIN[];
extern const char TRANSLATE[];
extern const char SCALE[];
extern const char ROTATE[];
extern const char PARAM[];
extern const char PARAM_X[];
extern const char PARAM_Y[];
extern const char PARAM_Z[];
extern const char TRANSFORM[];
extern const char CENTER[];

// wrap constants
extern const char WRAP_NAME[];
extern const char DIRECTION[];
extern const char SPACING[];
extern const char CONTENT_SPACING[];
extern const char ALIGNMENT[];
extern const char MAIN_ALIGNMENT[];
extern const char CROSS_ALIGNMENT[];

// checkable constants
extern const char CHECKBOX_NAME[];
extern const char SWITCH_NAME[];
extern const char RADIO_NAME[];
extern const char CHECKABLE_VALUE[];
extern const char RADIO_GROUP_VALUE[];
extern const char CHECKABLE_ON_CHANGED_EVENT[];
extern const char CHECKABLE_POINT_COLOR[];
extern const char CHECKABLE_ACTIVE_COLOR[];
extern const char CHECKABLE_INACTIVE_COLOR[];
extern const char CHECKABLE_FOCUS_COLOR[];

// popup constants
extern const char POPUP_NAME[];
extern const char POPUP_COMPOSED_ID[];
extern const char POPUP_COMPOSED_NAME[];
extern const char POPUP_PLACEMENT[];
extern const char POPUP_MASK_COLOR[];
extern const char POPUP_ON_VISIBILITY_CHANGE[];

// marquee constants
extern const char MARQUEE_NAME[];
extern const char MARQUEE_VALUE[];
extern const char MARQUEE_DIRECTION[];
extern const char MARQUEE_DIRECTION_LEFT[];
extern const char MARQUEE_DIRECTION_RIGHT[];
extern const char MARQUEE_SCROLL_AMOUNT[];
extern const char MARQUEE_LOOP[];
extern const char MARQUEE_BOUNCE[];
extern const char MARQUEE_FINISH[];
extern const char MARQUEE_START[];

// rating constants
extern const char RATING_NAME[];
extern const char RATING_SCORE[];
extern const char RATING_FOREGROUND_SRC[];
extern const char RATING_SECONDARY_SRC[];
extern const char RATING_BACKGROUND_SRC[];
extern const char STEP_SIZE[];
extern const char STAR_NUM[];
extern const char RATING_WIDTH[];
extern const char RATING_HEIGHT[];
extern const char RATING_HORIZONTAL_PADDING[];
extern const char RATING_VERTICAL_PADDING[];
extern const char RATING_INDICATOR[];
extern const char RATING_ON_CHANGE[];
extern const int32_t DEFAULT_RATING_STAR_NUM;
extern const int32_t DEFAULT_INDICATOR_VALUE;
extern const int32_t DEFAULT_NO_INDICATOR_VALUE;
extern const uint32_t RATING_FOCUS_BOARD_COLOR;
extern const double DEFAULT_RATING_SCORE;
extern const double DEFAULT_RATING_WIDTH;
extern const double DEFAULT_RATING_HEIGHT;
extern const double DEFAULT_RATING_STEP_SIZE;
extern const double DEFAULT_RATING_HORIZONTAL_PADDING;
extern const double DEFAULT_RATING_VERTICAL_PADDING;

// loading progress constants
extern const char LOADING_PROGRESS_NAME[];
extern const char DIAMETER[];

// video constants
extern const char VIDEO_NAME[];
extern const char VIDEO_SRC[];
extern const char VIDEO_AUTOPLAY[];
extern const char VIDEO_POSTER[];
extern const char VIDEO_CONTROLS[];
extern const char VIDEO_MUTED[];
extern const char VIDEO_FIT[];
extern const char VIDEO_PREPARED_EVENT_ID[];
extern const char VIDEO_START_EVENT_ID[];
extern const char VIDEO_PAUSE_EVENT_ID[];
extern const char VIDEO_FINISH_EVENT_ID[];
extern const char VIDEO_ERROR_EVENT_ID[];
extern const char VIDEO_SEEKING_EVENT_ID[];
extern const char VIDEO_SEEKED_EVENT_ID[];
extern const char VIDEO_TIMEUPDATE_EVENT_ID[];
extern const char VIDEO_FULLSCREENCHANGE_EVENT_ID[];

// select constants
extern const char SELECT_NAME[];
extern const char SELECT_DISABLED[];
extern const char SELECT_SELECTED_COLOR[];
extern const char SELECT_CLICKED_COLOR[];
extern const char SELECT_DISABLED_COLOR[];
extern const char SELECT_ON_CHANGED[];
extern const char SELECT_TIP_TEXT[];
extern const char SELECT_OPTION_SIZE[];
extern const char SELECT_OPTIONS[];
extern const char SELECT_OPTION_ICON[];
extern const char SELECT_OPTION_VALUE[];
extern const char SELECT_OPTION_TEXT[];
extern const char SELECT_OPTION_SELECTED[];
extern const uint32_t SELECT_DEFAULT_OPTION_SIZE;
ACE_EXPORT extern const uint32_t SELECT_INVALID_INDEX;

// clip constants
extern const char CLIP_NAME[];

// divider constants
extern const char DIVIDER_NAME[];
extern const char DIVIDER_STROKE_WIDTH[];
extern const char DIVIDER_COLOR[];
extern const char DIVIDER_VERTICAL[];

// columnSplit constants
extern const double DEFAULT_SPLIT_HEIGHT;

// animator constants
extern const char START[];
extern const char PAUSE[];
extern const char STOP[];
extern const char RESUME[];
extern const char CANCEL[];

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_CONSTANTS_H
