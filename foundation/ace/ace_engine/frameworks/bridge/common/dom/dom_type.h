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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TYPE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TYPE_H

#include <cstdint>

#include "base/utils/macros.h"

namespace OHOS::Ace {

using NodeId = int32_t;
ACE_EXPORT extern const NodeId DOM_ROOT_NODE_ID_BASE;
ACE_EXPORT extern const char DOM_DEFAULT_ROOT_NODE_ID[];
// node tag defines
ACE_EXPORT extern const char DOM_NODE_TAG_CALENDAR[];
ACE_EXPORT extern const char DOM_NODE_TAG_DIV[];
ACE_EXPORT extern const char DOM_NODE_TAG_GRID_COLUMN[];
ACE_EXPORT extern const char DOM_NODE_TAG_GRID_CONTAINER[];
ACE_EXPORT extern const char DOM_NODE_TAG_GRID_ROW[];
ACE_EXPORT extern const char DOM_NODE_TAG_SVG_TEXT[];
ACE_EXPORT extern const char DOM_NODE_TAG_TEXT[];
ACE_EXPORT extern const char DOM_NODE_TAG_TEXT_PATH[];
ACE_EXPORT extern const char DOM_NODE_TAG_TSPAN[];
ACE_EXPORT extern const char DOM_NODE_TAG_PICKER_DIALOG[];
ACE_EXPORT extern const char DOM_NODE_TAG_PICKER_VIEW[];
ACE_EXPORT extern const char DOM_NODE_TAG_OPTION[];
ACE_EXPORT extern const char DOM_NODE_TAG_POPUP[];
ACE_EXPORT extern const char DOM_NODE_TAG_PROGRESS[];
ACE_EXPORT extern const char DOM_NODE_TAG_SELECT[];
ACE_EXPORT extern const char DOM_NODE_TAG_MASK[];
ACE_EXPORT extern const char DOM_NODE_TAG_MENU[];
ACE_EXPORT extern const char DOM_NODE_TAG_SLIDER[];
ACE_EXPORT extern const char DOM_NODE_TAG_SPAN[];
ACE_EXPORT extern const char DOM_NODE_TAG_STACK[];
ACE_EXPORT extern const char DOM_NODE_TAG_SWIPER[];
ACE_EXPORT extern const char DOM_NODE_TAG_SWITCH[];
ACE_EXPORT extern const char DOM_NODE_TAG_TABS[];
ACE_EXPORT extern const char DOM_NODE_TAG_TAB_BAR[];
ACE_EXPORT extern const char DOM_NODE_TAG_TAB_CONTENT[];
ACE_EXPORT extern const char DOM_NODE_TAG_REFRESH[];
ACE_EXPORT extern const char DOM_NODE_TAG_IMAGE[];
ACE_EXPORT extern const char DOM_NODE_TAG_LIST[];
ACE_EXPORT extern const char DOM_NODE_TAG_LIST_ITEM[];
ACE_EXPORT extern const char DOM_NODE_TAG_LIST_ITEM_GROUP[];
ACE_EXPORT extern const char DOM_NODE_TAG_VIDEO[];
ACE_EXPORT extern const char DOM_NODE_TAG_RATING[];
ACE_EXPORT extern const char DOM_NODE_TAG_MARQUEE[];
ACE_EXPORT extern const char DOM_NODE_TAG_NAVIGATION_BAR[];
ACE_EXPORT extern const char DOM_NODE_TAG_NAVIGATION_MENU[];
ACE_EXPORT extern const char DOM_NODE_TAG_TEXTAREA[];
ACE_EXPORT extern const char DOM_NODE_TAG_INPUT[];
ACE_EXPORT extern const char DOM_NODE_TAG_LABEL[];
ACE_EXPORT extern const char DOM_NODE_TAG_DIVIDER[];
ACE_EXPORT extern const char DOM_NODE_TAG_CANVAS[];
ACE_EXPORT extern const char DOM_NODE_TAG_BUTTON[];
ACE_EXPORT extern const char DOM_NODE_TAG_CHART[];
ACE_EXPORT extern const char DOM_NODE_TAG_DIALOG[];
ACE_EXPORT extern const char DOM_NODE_TAG_SEARCH[];
ACE_EXPORT extern const char DOM_NODE_TAG_IMAGE_ANIMATOR[];
ACE_EXPORT extern const char DOM_NODE_TAG_TOGGLE[];
ACE_EXPORT extern const char DOM_NODE_TAG_CLOCK[];
ACE_EXPORT extern const char DOM_NODE_TAG_STEPPER[];
ACE_EXPORT extern const char DOM_NODE_TAG_STEPPER_ITEM[];
ACE_EXPORT extern const char DOM_NODE_TAG_PIECE[];
ACE_EXPORT extern const char DOM_NODE_TAG_BADGE[];
ACE_EXPORT extern const char DOM_NODE_TAG_PANEL[];
ACE_EXPORT extern const char DOM_NODE_TAG_TOOL_BAR[];
ACE_EXPORT extern const char DOM_NODE_TAG_TOOL_BAR_ITEM[];
ACE_EXPORT extern const char DOM_NODE_TAG_QRCODE[];
ACE_EXPORT extern const char DOM_NODE_TAG_SVG[];
ACE_EXPORT extern const char DOM_NODE_TAG_G[];
ACE_EXPORT extern const char DOM_NODE_TAG_USE[];
ACE_EXPORT extern const char DOM_NODE_TAG_DEFS[];
ACE_EXPORT extern const char DOM_NODE_TAG_SYMBOL[];
ACE_EXPORT extern const char DOM_NODE_TAG_RECT[];
ACE_EXPORT extern const char DOM_NODE_TAG_ANIMATE[];
ACE_EXPORT extern const char DOM_NODE_TAG_ANIMATE_TRANSFORM[];
ACE_EXPORT extern const char DOM_NODE_TAG_WEB[];
ACE_EXPORT extern const char DOM_NODE_TAG_RICH_TEXT[];
ACE_EXPORT extern const char DOM_NODE_TAG_XCOMPONENT[];
ACE_EXPORT extern const char DOM_NODE_TAG_PATH[];
ACE_EXPORT extern const char DOM_NODE_TAG_LINE[];
ACE_EXPORT extern const char DOM_NODE_TAG_POLYLINE[];
ACE_EXPORT extern const char DOM_NODE_TAG_POLYGON[];
ACE_EXPORT extern const char DOM_NODE_TAG_CIRCLE[];
ACE_EXPORT extern const char DOM_NODE_TAG_FORM[];
ACE_EXPORT extern const char DOM_NODE_TAG_ELLIPSE[];
ACE_EXPORT extern const char DOM_NODE_TAG_ANIMATE_MOTION[];
ACE_EXPORT extern const char DOM_NODE_TAG_CAMERA[];
ACE_EXPORT extern const char DOM_NODE_TAG_STOP[];
ACE_EXPORT extern const char DOM_NODE_TAG_LINEAR_GRADIENT[];
ACE_EXPORT extern const char DOM_NODE_TAG_RADIAL_GRADIENT[];
ACE_EXPORT extern const char DOM_NODE_TAG_FILTER[];
ACE_EXPORT extern const char DOM_NODE_TAG_FE_COLORMATRIX[];
ACE_EXPORT extern const char DOM_NODE_TAG_FE_COMPOSITE[];
ACE_EXPORT extern const char DOM_NODE_TAG_FE_GAUSSIANBLUR[];
ACE_EXPORT extern const char DOM_NODE_TAG_FE_OFFSET[];

// common attribute constants
ACE_EXPORT extern const char DOM_ID[];
ACE_EXPORT extern const char DOM_SHARE_ID[];
ACE_EXPORT extern const char DOM_SHOW[];
ACE_EXPORT extern const char DOM_DIR[];
ACE_EXPORT extern const char DOM_AUTO_FOCUS[];

// common persudo class style constants
ACE_EXPORT extern const char DOM_PSEUDO_CLASS_SYMBOL[];
ACE_EXPORT extern const char DOM_ACTIVE_PSEUDO_CLASS[];
ACE_EXPORT extern const char DOM_DISABLED_PSEUDO_CLASS[];
ACE_EXPORT extern const char DOM_FOCUS_PSEUDO_CLASS[];
ACE_EXPORT extern const char DOM_CHECKED_PSEUDO_CLASS[];
ACE_EXPORT extern const char DOM_WAITING_PSEUDO_CLASS[];
ACE_EXPORT extern const char DOM_HOVER_PSEUDO_CLASS[];

// common style constants
ACE_EXPORT extern const char DOM_WIDTH[];
ACE_EXPORT extern const char DOM_HEIGHT[];
ACE_EXPORT extern const char DOM_TYPE[];
ACE_EXPORT extern const char DOM_VALUE[];
ACE_EXPORT extern const char DOM_START[];
ACE_EXPORT extern const char DOM_END[];
ACE_EXPORT extern const char DOM_TITLE[];
ACE_EXPORT extern const char DOM_TOUCHABLE[];
ACE_EXPORT extern const char DOM_DISABLED[];
ACE_EXPORT extern const char DOM_PADDING[];
ACE_EXPORT extern const char DOM_PADDING_TOP[];
ACE_EXPORT extern const char DOM_PADDING_RIGHT[];
ACE_EXPORT extern const char DOM_PADDING_BOTTOM[];
ACE_EXPORT extern const char DOM_PADDING_LEFT[];
ACE_EXPORT extern const char DOM_PADDING_START[];
ACE_EXPORT extern const char DOM_PADDING_END[];
ACE_EXPORT extern const char DOM_MARGIN[];
ACE_EXPORT extern const char DOM_MARGIN_TOP[];
ACE_EXPORT extern const char DOM_MARGIN_RIGHT[];
ACE_EXPORT extern const char DOM_MARGIN_BOTTOM[];
ACE_EXPORT extern const char DOM_MARGIN_LEFT[];
ACE_EXPORT extern const char DOM_MARGIN_START[];
ACE_EXPORT extern const char DOM_MARGIN_END[];
ACE_EXPORT extern const char DOM_LAYOUT_IN_BOX[];
ACE_EXPORT extern const char DOM_BACKGROUND_COLOR[];
ACE_EXPORT extern const char DOM_BACKGROUND_IMAGE[];
ACE_EXPORT extern const char DOM_FADE_COLOR[];
ACE_EXPORT extern const char DOM_BOX_SHADOW_H[];
ACE_EXPORT extern const char DOM_BOX_SHADOW_V[];
ACE_EXPORT extern const char DOM_BOX_SHADOW_BLUR[];
ACE_EXPORT extern const char DOM_BOX_SHADOW_SPREAD[];
ACE_EXPORT extern const char DOM_BOX_SHADOW_COLOR[];
ACE_EXPORT extern const char DOM_FLEX_DIRECTION[];
ACE_EXPORT extern const char DOM_FLEX_ROW[];
ACE_EXPORT extern const char DOM_FLEX_COLUMN[];
ACE_EXPORT extern const char DOM_FLEX[];
ACE_EXPORT extern const char DOM_FLEX_GROW[];
ACE_EXPORT extern const char DOM_FLEX_SHRINK[];
ACE_EXPORT extern const char DOM_FLEX_BASIS[];
ACE_EXPORT extern const char DOM_FLEX_WEIGHT[];
ACE_EXPORT extern const char DOM_ALIGN_SELF[];
ACE_EXPORT extern const char DOM_MIN_WIDTH[];
ACE_EXPORT extern const char DOM_MIN_HEIGHT[];
ACE_EXPORT extern const char DOM_MAX_WIDTH[];
ACE_EXPORT extern const char DOM_MAX_HEIGHT[];
ACE_EXPORT extern const char DOM_DISPLAY_INDEX[];
ACE_EXPORT extern const char DOM_ASPECT_RATIO[];
ACE_EXPORT extern const char DOM_OPACITY[];
ACE_EXPORT extern const char DOM_APPEARING_DURATION[];
ACE_EXPORT extern const char DOM_DISPLAY[];
ACE_EXPORT extern const char DOM_VISIBILITY[];
ACE_EXPORT extern const char DOM_DISPLAY_GRID[];
ACE_EXPORT extern const char DOM_DISPLAY_FLEX[];
ACE_EXPORT extern const char DOM_DISPLAY_NONE[];
ACE_EXPORT extern const char DOM_VISIBILITY_VISIBLE[];
ACE_EXPORT extern const char DOM_VISIBILITY_HIDDEN[];
ACE_EXPORT extern const char DOM_BORDER[];
ACE_EXPORT extern const char DOM_BORDER_WIDTH[];
ACE_EXPORT extern const char DOM_BORDER_LEFT_WIDTH[];
ACE_EXPORT extern const char DOM_BORDER_TOP_WIDTH[];
ACE_EXPORT extern const char DOM_BORDER_RIGHT_WIDTH[];
ACE_EXPORT extern const char DOM_BORDER_BOTTOM_WIDTH[];
ACE_EXPORT extern const char DOM_BORDER_COLOR[];
ACE_EXPORT extern const char DOM_BORDER_LEFT_COLOR[];
ACE_EXPORT extern const char DOM_BORDER_TOP_COLOR[];
ACE_EXPORT extern const char DOM_BORDER_RIGHT_COLOR[];
ACE_EXPORT extern const char DOM_BORDER_BOTTOM_COLOR[];
ACE_EXPORT extern const char DOM_BORDER_STYLE[];
ACE_EXPORT extern const char DOM_BORDER_LEFT_STYLE[];
ACE_EXPORT extern const char DOM_BORDER_TOP_STYLE[];
ACE_EXPORT extern const char DOM_BORDER_RIGHT_STYLE[];
ACE_EXPORT extern const char DOM_BORDER_BOTTOM_STYLE[];
ACE_EXPORT extern const char DOM_BORDER_RADIUS[];
ACE_EXPORT extern const char DOM_BORDER_TOP_LEFT_RADIUS[];
ACE_EXPORT extern const char DOM_BORDER_TOP_RIGHT_RADIUS[];
ACE_EXPORT extern const char DOM_BORDER_BOTTOM_RIGHT_RADIUS[];
ACE_EXPORT extern const char DOM_BORDER_BOTTOM_LEFT_RADIUS[];
ACE_EXPORT extern const char DOM_BORDER_IMAGE[];
ACE_EXPORT extern const char DOM_BORDER_IMAGE_SOURCE[];
ACE_EXPORT extern const char DOM_BORDER_IMAGE_WIDTH[];
ACE_EXPORT extern const char DOM_BORDER_IMAGE_SLICE[];
ACE_EXPORT extern const char DOM_BORDER_IMAGE_OUTSET[];
ACE_EXPORT extern const char DOM_BORDER_IMAGE_REPEAT[];
ACE_EXPORT extern const char DOM_POSITION[];
ACE_EXPORT extern const char DOM_POSITION_RELATIVE[];
ACE_EXPORT extern const char DOM_POSITION_FIXED[];
ACE_EXPORT extern const char DOM_POSITION_ABSOLUTE[];
ACE_EXPORT extern const char DOM_POSITION_TOP[];
ACE_EXPORT extern const char DOM_POSITION_RIGHT[];
ACE_EXPORT extern const char DOM_POSITION_BOTTOM[];
ACE_EXPORT extern const char DOM_POSITION_LEFT[];
ACE_EXPORT extern const char DOM_POSITION_START[];
ACE_EXPORT extern const char DOM_POSITION_END[];
ACE_EXPORT extern const char DOM_BACKGROUND_IMAGE_SIZE[];
ACE_EXPORT extern const char DOM_BACKGROUND_IMAGE_SIZE_CONTAIN[];
ACE_EXPORT extern const char DOM_BACKGROUND_IMAGE_SIZE_COVER[];
ACE_EXPORT extern const char DOM_BACKGROUND_IMAGE_SIZE_AUTO[];
ACE_EXPORT extern const char DOM_BACKGROUND_IMAGE_POSITION[];
ACE_EXPORT extern const char DOM_BACKGROUND_IMAGE_POSITION_LEFT[];
ACE_EXPORT extern const char DOM_BACKGROUND_IMAGE_POSITION_CENTER[];
ACE_EXPORT extern const char DOM_BACKGROUND_IMAGE_POSITION_RIGHT[];
ACE_EXPORT extern const char DOM_BACKGROUND_IMAGE_POSITION_TOP[];
ACE_EXPORT extern const char DOM_BACKGROUND_IMAGE_POSITION_BOTTOM[];
ACE_EXPORT extern const char DOM_BACKGROUND_IMAGE_REPEAT[];
ACE_EXPORT extern const char DOM_BACKGROUND[];
ACE_EXPORT extern const char DOM_GRADIENT_DIRECTION_TOP[];
ACE_EXPORT extern const char DOM_GRADIENT_DIRECTION_RIGHT[];
ACE_EXPORT extern const char DOM_GRADIENT_DIRECTION_BOTTOM[];
ACE_EXPORT extern const char DOM_GRADIENT_DIRECTION_LEFT[];
ACE_EXPORT extern const char DOM_GRADIENT_SIZE_CLOSEST_CORNER[];
ACE_EXPORT extern const char DOM_GRADIENT_SIZE_CLOSEST_SIDE[];
ACE_EXPORT extern const char DOM_GRADIENT_SIZE_FARTHEST_CORNER[];
ACE_EXPORT extern const char DOM_GRADIENT_SIZE_FARTHEST_SIDE[];
ACE_EXPORT extern const char DOM_GRADIENT_SHAPE_CIRCLE[];
ACE_EXPORT extern const char DOM_GRADIENT_SHAPE_ELLIPSE[];
ACE_EXPORT extern const char DOM_VALUES[];
ACE_EXPORT extern const char DOM_GRADIENT_VALUES[];
ACE_EXPORT extern const char DOM_GRADIENT_TYPE[];
ACE_EXPORT extern const char DOM_GRADIENT_DIRECTIONS[];
ACE_EXPORT extern const char DOM_GRADIENT_SIZE[];
ACE_EXPORT extern const char DOM_GRADIENT_POSITION[];
ACE_EXPORT extern const char DOM_GRADIENT_SHAPE[];
ACE_EXPORT extern const char DOM_GRADIENT_ANGLE[];
ACE_EXPORT extern const char DOM_GRADIENT_ROTATION[];
ACE_EXPORT extern const char DOM_LINEAR_GRADIENT[];
ACE_EXPORT extern const char DOM_REPEATING_LINEAR_GRADIENT[];
ACE_EXPORT extern const char DOM_RADIAL_GRADIENT[];
ACE_EXPORT extern const char DOM_REPEATING_RADIAL_GRADIENT[];
ACE_EXPORT extern const char DOM_SWEEP_GRADIENT[];
ACE_EXPORT extern const char DOM_REPEATING_SWEEP_GRADIENT[];
ACE_EXPORT extern const char DOM_TRANSFORM[];
ACE_EXPORT extern const char DOM_SCALE[];
ACE_EXPORT extern const char DOM_SCALE_X[];
ACE_EXPORT extern const char DOM_SCALE_Y[];
ACE_EXPORT extern const char DOM_SCALE_Z[];
ACE_EXPORT extern const char DOM_SCALE_3D[];
ACE_EXPORT extern const char DOM_TRANSLATE[];
ACE_EXPORT extern const char DOM_TRANSLATE_X[];
ACE_EXPORT extern const char DOM_TRANSLATE_Y[];
ACE_EXPORT extern const char DOM_TRANSLATE_Z[];
ACE_EXPORT extern const char DOM_TRANSLATE_3D[];
ACE_EXPORT extern const char DOM_ROTATE[];
ACE_EXPORT extern const char DOM_ROTATE_X[];
ACE_EXPORT extern const char DOM_ROTATE_Y[];
ACE_EXPORT extern const char DOM_ROTATE_Z[];
ACE_EXPORT extern const char DOM_ROTATE_3D[];
ACE_EXPORT extern const char DOM_SKEW[];
ACE_EXPORT extern const char DOM_SKEW_X[];
ACE_EXPORT extern const char DOM_SKEW_Y[];
ACE_EXPORT extern const char DOM_PERSPECTIVE[];
ACE_EXPORT extern const char DOM_MATRIX[];
ACE_EXPORT extern const char DOM_MATRIX_3D[];
ACE_EXPORT extern const char DOM_FOCUSABLE[];
ACE_EXPORT extern const char DOM_ANIMATION_WIDTH[];
ACE_EXPORT extern const char DOM_ANIMATION_HEIGHT[];
ACE_EXPORT extern const char DOM_ANIMATION_COLOR[];
ACE_EXPORT extern const char DOM_ANIMATION_OPACITY[];
ACE_EXPORT extern const char DOM_ANIMATION_ITERATIONS[];
ACE_EXPORT extern const char DOM_ANIMATION_DELAY_API[];
ACE_EXPORT extern const char DOM_ANIMATION_DURATION_API[];
ACE_EXPORT extern const char DOM_TRANSFORM_ORIGIN[];
ACE_EXPORT extern const char DOM_ANIMATION_NAME[];
ACE_EXPORT extern const char DOM_ANIMATION_DELAY[];
ACE_EXPORT extern const char DOM_ANIMATION_DURATION[];
ACE_EXPORT extern const char DOM_ANIMATION_FILL[];
ACE_EXPORT extern const char DOM_ANIMATION_EASING[];
ACE_EXPORT extern const char DOM_ANIMATION_BEGIN[];
ACE_EXPORT extern const char DOM_ANIMATION_END[];
ACE_EXPORT extern const char DOM_ANIMATION_ITERATION_COUNT[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION[];
ACE_EXPORT extern const char DOM_ANIMATION_FILL_MODE[];
ACE_EXPORT extern const char DOM_ANIMATION_DIRECTION[];
ACE_EXPORT extern const char DOM_ANIMATION_DIRECTION_API[];
ACE_EXPORT extern const char DOM_ANIMATION_DIRECTION_NORMAL[];
ACE_EXPORT extern const char DOM_ANIMATION_DIRECTION_ALTERNATE[];
ACE_EXPORT extern const char DOM_ANIMATION_DIRECTION_REVERSE[];
ACE_EXPORT extern const char DOM_ANIMATION_DIRECTION_ALTERNATE_REVERSE[];
ACE_EXPORT extern const char DOM_ANIMATION_PLAY_STATE[];
ACE_EXPORT extern const char DOM_ANIMATION_PLAY_STATE_RUNNING[];
ACE_EXPORT extern const char DOM_ANIMATION_PLAY_STATE_PAUSED[];
ACE_EXPORT extern const char DOM_ANIMATION_PLAY_STATE_IDLE[];
ACE_EXPORT extern const char DOM_ANIMATION_PLAY_STATE_FINISHED[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION_EASE[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION_EASE_IN[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION_EASE_OUT[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION_EASE_IN_OUT[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION_LINEAR[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION_FAST_OUT_SLOW_IN[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION_LINEAR_OUT_SLOW_IN[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION_FAST_OUT_LINEAR_IN[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION_FRICTION[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION_EXTREME_DECELERATION[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION_SHARP[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION_RHYTHM[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION_SMOOTH[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION_STEPS[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION_CUBIC_BEZIER[];
ACE_EXPORT extern const char DOM_ANIMATION_TIMING_FUNCTION_SPRING[];
ACE_EXPORT extern const char DOM_ANIMATION_FILL_MODE_NONE[];
ACE_EXPORT extern const char DOM_ANIMATION_FILL_MODE_FORWARDS[];
ACE_EXPORT extern const char DOM_ANIMATION_FILL_MODE_BACKWARDS[];
ACE_EXPORT extern const char DOM_ANIMATION_FILL_MODE_BOTH[];
ACE_EXPORT extern const char DOM_ANIMATION_NAME_TIME[];
ACE_EXPORT extern const char DOM_ANIMATION_OFFSET[];
ACE_EXPORT extern const char DOM_TRANSFORM_ORIGIN_RIGHT_TOP[];
ACE_EXPORT extern const char DOM_TRANSFORM_ORIGIN_RIGHT_CENTER[];
ACE_EXPORT extern const char DOM_TRANSFORM_ORIGIN_RIGHT_BOTTOM[];
ACE_EXPORT extern const char DOM_TRANSFORM_ORIGIN_CENTER_TOP[];
ACE_EXPORT extern const char DOM_TRANSFORM_ORIGIN_CENTER_CENTER[];
ACE_EXPORT extern const char DOM_TRANSFORM_ORIGIN_CENTER_BOTTOM[];
ACE_EXPORT extern const char DOM_TRANSFORM_ORIGIN_LEFT_TOP[];
ACE_EXPORT extern const char DOM_TRANSFORM_ORIGIN_LEFT_CENTER[];
ACE_EXPORT extern const char DOM_TRANSFORM_ORIGIN_LEFT_BOTTOM[];
ACE_EXPORT extern const char DOM_FILTER[];
ACE_EXPORT extern const char DOM_BACKDROP_FILTER[];
ACE_EXPORT extern const char DOM_WINDOW_FILTER[];
ACE_EXPORT extern const char DOM_CARET_COLOR[];
ACE_EXPORT extern const char DOM_CLIP_PATH[];
ACE_EXPORT extern const char DOM_MASK_IMAGE[];
ACE_EXPORT extern const char DOM_MASK_POSITION[];
ACE_EXPORT extern const char DOM_MASK_SIZE[];
ACE_EXPORT extern const char DOM_IMAGE_FILL[];
ACE_EXPORT extern const char DOM_BOX_SIZING[];
ACE_EXPORT extern const char DOM_AUTO[];

// transition style
ACE_EXPORT extern const char DOM_TRANSITION_NAME[];
ACE_EXPORT extern const char DOM_TRANSITION_PROPERTY[];
ACE_EXPORT extern const char DOM_TRANSITION_PROPERTY_DURATION[];
ACE_EXPORT extern const char DOM_TRANSITION_PROPERTY_TIMING_FUNCTION[];
ACE_EXPORT extern const char DOM_TRANSITION_PROPERTY_DELAY[];
ACE_EXPORT extern const char DOM_TRANSITION_BACKGROUND_COLOR[];
ACE_EXPORT extern const char DOM_TRANSITION_ENTER[];
ACE_EXPORT extern const char DOM_TRANSITION_EXIT[];
ACE_EXPORT extern const char DOM_TRANSITION_DURATION[];
ACE_EXPORT extern const char DOM_TRANSITION_TIMING_FUNCTION[];

// shared transition style
ACE_EXPORT extern const char DOM_SHARED_TRANSITION_EFFECT[];
ACE_EXPORT extern const char DOM_SHARED_TRANSITION_NAME[];
ACE_EXPORT extern const char DOM_SHARED_TRANSITION_TIMING_FUNCTION[];

// display: grid style constants
ACE_EXPORT extern const char DOM_GRID_TEMPLATE_COLUMNS[];
ACE_EXPORT extern const char DOM_GRID_TEMPLATE_ROWS[];
ACE_EXPORT extern const char DOM_GRID_COLUMN_GAP[];
ACE_EXPORT extern const char DOM_GRID_ROW_GAP[];
ACE_EXPORT extern const char DOM_GRID_COLUMN_START[];
ACE_EXPORT extern const char DOM_GRID_COLUMN_END[];
ACE_EXPORT extern const char DOM_GRID_ROW_START[];
ACE_EXPORT extern const char DOM_GRID_ROW_END[];
ACE_EXPORT extern const char DOM_GRID_AUTO_FLOW[];

// div node style constants
ACE_EXPORT extern const char DOM_ALIGN_CONTENT[];
ACE_EXPORT extern const char DOM_ALIGN_CONTENT_STRETCH[];
ACE_EXPORT extern const char DOM_ALIGN_CONTENT_START[];
ACE_EXPORT extern const char DOM_ALIGN_CONTENT_CENTER[];
ACE_EXPORT extern const char DOM_ALIGN_CONTENT_END[];
ACE_EXPORT extern const char DOM_ALIGN_CONTENT_BETWEEN[];
ACE_EXPORT extern const char DOM_ALIGN_CONTENT_AROUND[];
ACE_EXPORT extern const char DOM_ALIGN_ITEMS[];
ACE_EXPORT extern const char DOM_ALIGN_ITEMS_START[];
ACE_EXPORT extern const char DOM_ALIGN_ITEMS_CENTER[];
ACE_EXPORT extern const char DOM_ALIGN_ITEMS_END[];
ACE_EXPORT extern const char DOM_ALIGN_ITEMS_STRETCH[];
ACE_EXPORT extern const char DOM_ALIGN_ITEMS_BASELINE[];
ACE_EXPORT extern const char DOM_JUSTIFY_CONTENT[];
ACE_EXPORT extern const char DOM_JUSTIFY_CONTENT_START[];
ACE_EXPORT extern const char DOM_JUSTIFY_CONTENT_CENTER[];
ACE_EXPORT extern const char DOM_JUSTIFY_CONTENT_END[];
ACE_EXPORT extern const char DOM_JUSTIFY_CONTENT_BETWEEN[];
ACE_EXPORT extern const char DOM_JUSTIFY_CONTENT_AROUND[];
ACE_EXPORT extern const char DOM_JUSTIFY_CONTENT_EVENLY[];
ACE_EXPORT extern const char DOM_FLEX_WRAP[];
ACE_EXPORT extern const char DOM_WRAP[];
ACE_EXPORT extern const char DOM_DIV_CARD_TYPE[];
ACE_EXPORT extern const char DOM_DIV_CARD_BLUR[];
ACE_EXPORT extern const char DOM_DIV_EVENT_REACH_TOP[];
ACE_EXPORT extern const char DOM_DIV_EVENT_REACH_BOTTOM[];
ACE_EXPORT extern const char DOM_DIV_EVENT_REACH_START[];
ACE_EXPORT extern const char DOM_DIV_EVENT_REACH_END[];

// image attribute and style constants
ACE_EXPORT extern const char DOM_SRC[];
ACE_EXPORT extern const char DOM_TOP[];
ACE_EXPORT extern const char DOM_IMAGE_FIT[];
ACE_EXPORT extern const char DOM_IMAGE_ALT[];
ACE_EXPORT extern const char DOM_IMAGE_MATCH_TEXT_DIRECTION[];
ACE_EXPORT extern const char DOM_IMAGE_FIT_ORIGINAL_SIZE[];
ACE_EXPORT extern const char DOM_IMAGE_FILL_COLOR[];
ACE_EXPORT extern const char DOM_IMAGE_SYNC_LOAD[];
// objectPosition
ACE_EXPORT extern const char DOM_IMAGE_POSITION[];
ACE_EXPORT extern const char DOM_IMAGE_POSITION_LEFT[];
ACE_EXPORT extern const char DOM_IMAGE_POSITION_CENTER[];
ACE_EXPORT extern const char DOM_IMAGE_POSITION_RIGHT[];
ACE_EXPORT extern const char DOM_IMAGE_POSITION_TOP[];
ACE_EXPORT extern const char DOM_IMAGE_POSITION_BOTTOM[];

// image animator attribute and stytle constants
ACE_EXPORT extern const char DOM_IMAGES[];
ACE_EXPORT extern const char DOM_ITERATION[];
ACE_EXPORT extern const char DOM_REVERSE[];
ACE_EXPORT extern const char DOM_FIXEDSIZE[];
ACE_EXPORT extern const char DOM_FILLMODE[];
ACE_EXPORT extern const char DOM_PREDECODE[];
ACE_EXPORT extern const char DOM_IMAGE_ANIMATOR_START[];
ACE_EXPORT extern const char DOM_IMAGE_ANIMATOR_STOP[];
ACE_EXPORT extern const char DOM_IMAGE_ANIMATOR_PAUSE[];
ACE_EXPORT extern const char DOM_IMAGE_ANIMATOR_RESUME[];

// list attribute、style and event constants
ACE_EXPORT extern const char DOM_LIST_COLUMNS[];
ACE_EXPORT extern const char DOM_LIST_ITEM_EXTENT[];
ACE_EXPORT extern const char DOM_LIST_EVENT_SCROLL[];
ACE_EXPORT extern const char DOM_LIST_EVENT_SCROLL_BOTTOM[];
ACE_EXPORT extern const char DOM_LIST_EVENT_SCROLL_END[];
ACE_EXPORT extern const char DOM_LIST_EVENT_SCROLL_TOP[];
ACE_EXPORT extern const char DOM_LIST_EVENT_SCROLL_TOUCH_UP[];
ACE_EXPORT extern const char DOM_LIST_EVENT_INDEXER_CHANGE[];
ACE_EXPORT extern const char DOM_LIST_ITEM_EVENT_STICKY[];
ACE_EXPORT extern const char DOM_LIST_METHOD_EXPAND_GROUP[];
ACE_EXPORT extern const char DOM_LIST_METHOD_COLLAPSE_GROUP[];
ACE_EXPORT extern const char DOM_LIST_METHOD_SCROLL_BY[];
ACE_EXPORT extern const char DOM_LIST_METHOD_SCROLL_TO[];
ACE_EXPORT extern const char DOM_LIST_METHOD_SCROLL_ARROW[];
ACE_EXPORT extern const char DOM_LIST_METHOD_SCROLL_TOP[];
ACE_EXPORT extern const char DOM_LIST_METHOD_SCROLL_BOTTOM[];
ACE_EXPORT extern const char DOM_LIST_METHOD_SCROLL_PAGE[];
ACE_EXPORT extern const char DOM_LIST_SCROLLPAGE[];
ACE_EXPORT extern const char DOM_LIST_CHAIN_ANIMATION[];
ACE_EXPORT extern const char DOM_LIST_INITIAL_INDEX[];
ACE_EXPORT extern const char DOM_LIST_INITIAL_OFFSET[];
ACE_EXPORT extern const char DOM_LIST_UPDATE_EFFECT[];
ACE_EXPORT extern const char DOM_LIST_INDEXER[];
ACE_EXPORT extern const char DOM_LIST_INDEXER_MODE[];
ACE_EXPORT extern const char DOM_LIST_INDEXER_BUBBLE[];
ACE_EXPORT extern const char DOM_LIST_INDEXER_MULTI[];
ACE_EXPORT extern const char DOM_LIST_ITEM_SCALE[];
ACE_EXPORT extern const char DOM_LIST_ITEM_OPACITY[];
ACE_EXPORT extern const char DOM_LIST_ITEM_CENTER[];
ACE_EXPORT extern const char DOM_LIST_CENTER_LAYOUT[];
ACE_EXPORT extern const char DOM_LIST_SCROLL_VIBRATE[];
ACE_EXPORT extern const char DOM_LIST_ROTATION_VIBRATE[];
ACE_EXPORT extern const char DOM_LIST_DIVIDER[];
ACE_EXPORT extern const char DOM_LIST_DIVIDER_COLOR[];
ACE_EXPORT extern const char DOM_LIST_DIVIDER_HEIGHT[];
ACE_EXPORT extern const char DOM_LIST_DIVIDER_LENGTH[];
ACE_EXPORT extern const char DOM_LIST_DIVIDER_ORIGIN[];
ACE_EXPORT extern const char DOM_LIST_ATTR_SELECTED[];
ACE_EXPORT extern const char DOM_LIST_ACCESSIBILITY_DISABLED[];
ACE_EXPORT extern const char DOM_SCROLL_EFFECT[];
ACE_EXPORT extern const char DOM_SCROLL_EFFECT_SPRING[];
ACE_EXPORT extern const char DOM_SCROLL_EFFECT_FADE[];
ACE_EXPORT extern const char DOM_SCROLL_EFFECT_NONE[];
ACE_EXPORT extern const char DOM_SCROLL_SCROLLBAR[];
ACE_EXPORT extern const char DOM_SCROLL_SCROLLBAR_OFF[];
ACE_EXPORT extern const char DOM_SCROLL_SCROLLBAR_ON[];
ACE_EXPORT extern const char DOM_SCROLL_SCROLLBAR_AUTO[];
ACE_EXPORT extern const char DOM_SCROLL_SHAPE_MODE[];
ACE_EXPORT extern const char DOM_SCROLL_SHAPE_MODE_RECT[];
ACE_EXPORT extern const char DOM_SCROLL_SHAPE_MODE_ROUND[];
ACE_EXPORT extern const char DOM_SCROLL_SCROLLBAR_COLOR[];
ACE_EXPORT extern const char DOM_SCROLL_SCROLLBAR_WIDTH[];
ACE_EXPORT extern const char DOM_SCROLL_OVER_SCROLL_EFFECT[];
ACE_EXPORT extern const char DOM_SCROLL_SCROLLBAR_POSITION[];
ACE_EXPORT extern const char DOM_SCROLL_SCROLLBAR_OFFSET[];

// list-items attribute and style constants
ACE_EXPORT extern const char DOM_LISTITEM_COLUMN_SPAN[];
ACE_EXPORT extern const char DOM_LISTITEM_TYPE[];
ACE_EXPORT extern const char DOM_LISTITEM_STICKY[];
ACE_EXPORT extern const char DOM_LISTITEM_STICKY_RADIUS[];
ACE_EXPORT extern const char DOM_LISTITEM_CLICK_EFFECT[];
ACE_EXPORT extern const char DOM_LISTITEM_IS_TITLE[];
ACE_EXPORT extern const char DOM_LISTITEM_INDEX_KEY[];
ACE_EXPORT extern const char DOM_LISTITEM_PRIMARY[];
ACE_EXPORT extern const char DOM_LISTITEM_CARD_TYPE[];
ACE_EXPORT extern const char DOM_LISTITEM_CARD_BLUR[];
ACE_EXPORT extern const char DOM_LISTITEM_ACTIVE[];
ACE_EXPORT extern const char DOM_LISTITEM_CLICK_COLOR[];
ACE_EXPORT extern const char DOM_LISTITEM_KEY[];

// list-items-group attribute and style constants
ACE_EXPORT extern const char DOM_LIST_ITEM_GROUP_EVENT_GROUPCLICK[];
ACE_EXPORT extern const char DOM_LIST_ITEM_GROUP_EVENT_GROUPCOLLAPSE[];
ACE_EXPORT extern const char DOM_LIST_ITEM_GROUP_EVENT_GROUPEXPAND[];

// option attribute style and event constants
ACE_EXPORT extern const char DOM_OPTION_SELECTED[];
ACE_EXPORT extern const char DOM_OPTION_VALUE[];
ACE_EXPORT extern const char DOM_OPTION_ICON[];
ACE_EXPORT extern const char DOM_OPTION_ACTION[];
ACE_EXPORT extern const char DOM_OPTION_CONTENT[];
ACE_EXPORT extern const char DOM_OPTION_TEXT_DECORATION[];

// text attribute and style constants
ACE_EXPORT extern const char DOM_TEXT_VALUE[];
ACE_EXPORT extern const char DOM_TEXT_COLOR[];
ACE_EXPORT extern const char DOM_TEXT_LINES[];
ACE_EXPORT extern const char DOM_TEXT_MAX_LINES[];
ACE_EXPORT extern const char DOM_TEXT_FONT_SIZE[];
ACE_EXPORT extern const char DOM_TEXT_MIN_FONT_SIZE[];
ACE_EXPORT extern const char DOM_TEXT_MAX_FONT_SIZE[];
ACE_EXPORT extern const char DOM_TEXT_FONT_SIZE_STEP[];
ACE_EXPORT extern const char DOM_TEXT_PREFER_FONT_SIZES[];
ACE_EXPORT extern const char DOM_TEXT_FONT_WEIGHT[];
ACE_EXPORT extern const char DOM_TEXT_DECORATION[];
ACE_EXPORT extern const char DOM_TEXT_DECORATION_COLOR[];
ACE_EXPORT extern const char DOM_TEXT_FONT_STYLE[];
ACE_EXPORT extern const char DOM_TEXT_ALLOW_SCALE[];
ACE_EXPORT extern const char DOM_TEXT_ALIGN[];
ACE_EXPORT extern const char DOM_TEXT_CASE[];
ACE_EXPORT extern const char DOM_TEXT_BASELINE_OFFSET[];
ACE_EXPORT extern const char DOM_TEXT_OVERFLOW[];
ACE_EXPORT extern const char DOM_OVERFLOW_STYLE[];
ACE_EXPORT extern const char DOM_TEXT_LINE_HEIGHT[];
ACE_EXPORT extern const char DOM_TEXT_LETTER_SPACING[];
ACE_EXPORT extern const char DOM_TEXT_FONT_FAMILY[];
ACE_EXPORT extern const char DOM_ELLIPSIS[];
ACE_EXPORT extern const char DOM_CLIP[];
ACE_EXPORT extern const char DOM_LEFT[];
ACE_EXPORT extern const char DOM_RIGHT[];
ACE_EXPORT extern const char DOM_CENTER[];
ACE_EXPORT extern const char DOM_TEXT_DECORATION_UNDERLINE[];
ACE_EXPORT extern const char DOM_TEXT_DECORATION_OVERLINE[];
ACE_EXPORT extern const char DOM_TEXT_DECORATION_LINETHROUGH[];
ACE_EXPORT extern const char DOM_TEXT_DECORATION_INHERIT[];
ACE_EXPORT extern const char DOM_TEXT_DECORATION_NONE[];
ACE_EXPORT extern const char DOM_TEXT_FONT_STYLE_ITALIC[];
ACE_EXPORT extern const char DOM_TEXT_FONT_STYLE_NORMAL[];
ACE_EXPORT extern const char DOM_TEXT_WORD_BREAK[];
ACE_EXPORT extern const char DOM_TEXT_FONT_VARIANT[];
ACE_EXPORT extern const char DOM_TEXT_FONT_FEATURE_SETTINGS[];
ACE_EXPORT extern const char DOM_TEXT_ADAPT_HEIGHT[];
ACE_EXPORT extern const char DOM_TEXT_VERTICAL_ALIGN[];
ACE_EXPORT extern const char DOM_TOP[];
ACE_EXPORT extern const char DOM_MIDDLE[];
ACE_EXPORT extern const char DOM_BOTTOM[];
ACE_EXPORT extern const char DOM_TEXT_INDENT[];
ACE_EXPORT extern const char DOM_TEXT_WORD_SPACING[];
ACE_EXPORT extern const char DOM_TEXT_WHITE_SPACE[];
ACE_EXPORT extern const char DOM_WHITE_SPACE_NORMAL[];
ACE_EXPORT extern const char DOM_WHITE_SPACE_PRELINE[];
ACE_EXPORT extern const char DOM_WHITE_SPACE_PREWRAP[];
ACE_EXPORT extern const char DOM_WHITE_SPACE_NOWRAP[];
ACE_EXPORT extern const char DOM_WHITE_SPACE_PRE[];
ACE_EXPORT extern const char DOM_WHITE_SPACE_INHERIT[];

// popup attribute and style constants
ACE_EXPORT extern const char DOM_TARGET[];
ACE_EXPORT extern const char DOM_PLACEMENT[];
ACE_EXPORT extern const char DOM_MASK_COLOR[];
ACE_EXPORT extern const char DOM_ARROW_OFFSET[];
ACE_EXPORT extern const char DOM_KEEP_ALIVE[];
ACE_EXPORT extern const char DOM_CLICKABLE[];
ACE_EXPORT extern const char DOM_VISIBILITY_CHANGE[];

// progress attribute and style constants
ACE_EXPORT extern const char DOM_PROGRESS_COLOR[];
ACE_EXPORT extern const char DOM_PROGRESS_DIAMETER[];
ACE_EXPORT extern const char DOM_PROGRESS_BUBBLE_RADIUS[];
ACE_EXPORT extern const char DOM_PROGRESS_SECONDARY_COLOR[];
ACE_EXPORT extern const char DOM_PROGRESS_BACKGROUND_COLOR[];
ACE_EXPORT extern const char DOM_PROGRESS_STROKE_WIDTH[];
ACE_EXPORT extern const char DOM_PROGRESS_TYPE[];
ACE_EXPORT extern const char DOM_PROGRESS_TYPE_HORIZONTAL[];
ACE_EXPORT extern const char DOM_PROGRESS_TYPE_CIRCULAR[];
ACE_EXPORT extern const char DOM_PROGRESS_TYPE_BUBBLE[];
ACE_EXPORT extern const char DOM_PROGRESS_TYPE_MOON[];
ACE_EXPORT extern const char DOM_PROGRESS_TYPE_RING[];
ACE_EXPORT extern const char DOM_PROGRESS_TYPE_SCALE[];
ACE_EXPORT extern const char DOM_PROGRESS_TYPE_ARC[];
ACE_EXPORT extern const char DOM_PROGRESS_PERCENT[];
ACE_EXPORT extern const char DOM_PROGRESS_SECONDARY_PERCENT[];
ACE_EXPORT extern const char DOM_CIRCULAR_WIDTH[];
ACE_EXPORT extern const char DOM_CIRCULAR_HEIGHT[];
ACE_EXPORT extern const char DOM_CENTER_X[];
ACE_EXPORT extern const char DOM_CENTER_Y[];
ACE_EXPORT extern const char DOM_PROGRESS_RADIUS[];
ACE_EXPORT extern const char DOM_START_DEGREE[];
ACE_EXPORT extern const char DOM_SWEEP_DEGREE[];
ACE_EXPORT extern const char DOM_RING_CLOCKWISH_DIRECTION[];
ACE_EXPORT extern const char DOM_SCALE_WIDTH[];
ACE_EXPORT extern const char DOM_SCALE_NUMBER[];
ACE_EXPORT extern const char DOM_DIRECTION[];
ACE_EXPORT extern const char DOM_START_TO_END[];

// chart attribute and style constants
ACE_EXPORT extern const char DOM_CHART_TYPE[];
ACE_EXPORT extern const char DOM_CHART_TYPE_BAR[];
ACE_EXPORT extern const char DOM_CHART_TYPE_LINE[];
ACE_EXPORT extern const char DOM_CHART_TYPE_GAUGE[];
ACE_EXPORT extern const char DOM_CHART_TYPE_PROGRESS[];
ACE_EXPORT extern const char DOM_CHART_TYPE_LOADING[];
ACE_EXPORT extern const char DOM_CHART_TYPE_RAINBOW[];
ACE_EXPORT extern const char DOM_COLORS_ARRAY[];
ACE_EXPORT extern const char DOM_WEIGHTS_ARRAY[];
ACE_EXPORT extern const char DOM_EFFECTS_ON[];
ACE_EXPORT extern const char DOM_AUTO_SCALE[];
ACE_EXPORT extern const char DOM_CHART_ANIMATION_DURATION[];

// slider attribute and style constants
ACE_EXPORT extern const char DOM_MIN[];
ACE_EXPORT extern const char DOM_MAX[];
ACE_EXPORT extern const char DOM_STEP[];
ACE_EXPORT extern const char DOM_COLOR[];
ACE_EXPORT extern const char DOM_SELECTED_COLOR[];
ACE_EXPORT extern const char DOM_BLOCK_COLOR[];
ACE_EXPORT extern const char DOM_MIN_ICON[];
ACE_EXPORT extern const char DOM_MAX_ICON[];
ACE_EXPORT extern const char DOM_CONTINUOUS[];
ACE_EXPORT extern const char DOM_INTERMITTENT[];
ACE_EXPORT extern const char DOM_SLIDER_MODE[];
ACE_EXPORT extern const char DOM_SHOW_TIPS[];
ACE_EXPORT extern const char DOM_SHOW_STEPS[];
ACE_EXPORT extern const char DOM_OUTSET[];
ACE_EXPORT extern const char DOM_INSET[];

// stepper attribute style  event and method constants
ACE_EXPORT extern const char DOM_STEPPER_INDEX[];
ACE_EXPORT extern const char DOM_STEPPER_LABEL[];
ACE_EXPORT extern const char DOM_STEPPER_LEFT_LABEL[];
ACE_EXPORT extern const char DOM_STEPPER_RIGHT_LABEL[];
ACE_EXPORT extern const char DOM_STEPPER_INITIAL_STATUS[];
ACE_EXPORT extern const char DOM_STEPPER_TEXT_COLOR[];
ACE_EXPORT extern const char DOM_STEPPER_FONT_SIZE[];
ACE_EXPORT extern const char DOM_STEPPER_FONT_STYLE[];
ACE_EXPORT extern const char DOM_STEPPER_FONT_WEIGHT[];
ACE_EXPORT extern const char DOM_STEPPER_FONT_FAMILY[];
ACE_EXPORT extern const char DOM_STEPPER_TEXT_DECORATION[];
ACE_EXPORT extern const char DOM_STEPPER_EVENT_FINISH[];
ACE_EXPORT extern const char DOM_STEPPER_EVENT_SKIP[];
ACE_EXPORT extern const char DOM_STEPPER_EVENT_CHANGE[];
ACE_EXPORT extern const char DOM_STEPPER_EVENT_NEXT[];
ACE_EXPORT extern const char DOM_STEPPER_EVENT_BACK[];
ACE_EXPORT extern const char DOM_STEPPER_SET_RIGHT_BUTTON_STATUS[];

// stepper-item attribute style and event constants
ACE_EXPORT extern const char DOM_STEPPER_ITEM_EVENT_APPEAR[];
ACE_EXPORT extern const char DOM_STEPPER_ITEM_EVENT_DISAPPEAR[];

// swiper attribute and style constants
ACE_EXPORT extern const char DOM_INDEX[];
ACE_EXPORT extern const char DOM_SWIPER_ANIMATION_OPACITY[];
ACE_EXPORT extern const char DOM_AUTOPLAY[];
ACE_EXPORT extern const char DOM_CACHED_SIZE[];
ACE_EXPORT extern const char DOM_DISPLAY_MODE[];
ACE_EXPORT extern const char DOM_DISPLAY_MODE_AUTO_LINEAR[];
ACE_EXPORT extern const char DOM_INTERVAL[];
ACE_EXPORT extern const char DOM_INDICATOR[];
ACE_EXPORT extern const char DOM_LOOP[];
ACE_EXPORT extern const char DOM_DURATION[];
ACE_EXPORT extern const char DOM_VERTICAL[];
ACE_EXPORT extern const char DOM_ANIMATION_CURVE[];
ACE_EXPORT extern const char DOM_INDICATOR_COLOR[];
ACE_EXPORT extern const char DOM_INDICATOR_MASK[];
ACE_EXPORT extern const char DOM_INDICATOR_DISABLED[];
ACE_EXPORT extern const char DOM_INDICATOR_SELECTEDCOLOR[];
ACE_EXPORT extern const char DOM_INDICATOR_SIZE[];
ACE_EXPORT extern const char DOM_INDICATOR_TOP[];
ACE_EXPORT extern const char DOM_INDICATOR_LEFT[];
ACE_EXPORT extern const char DOM_INDICATOR_RIGHT[];
ACE_EXPORT extern const char DOM_INDICATOR_BOTTOM[];
ACE_EXPORT extern const char DOM_METHOD_SWIPE_TO[];
ACE_EXPORT extern const char DOM_METHOD_APPEND[];
ACE_EXPORT extern const char DOM_METHOD_SHOW_PREVIOUS[];
ACE_EXPORT extern const char DOM_METHOD_SHOW_NEXT[];
ACE_EXPORT extern const char DOM_DIGITAL_INDICATOR[];
ACE_EXPORT extern const char DOM_PREVIOUS_MARGIN[];
ACE_EXPORT extern const char DOM_NEXT_MARGIN[];

// switch attribute and style constants
ACE_EXPORT extern const char DOM_CHECKED[];
ACE_EXPORT extern const char DOM_SHOW_TEXT[];
ACE_EXPORT extern const char DOM_TEXT_ON[];
ACE_EXPORT extern const char DOM_TEXT_OFF[];
ACE_EXPORT extern const char DOM_TEXT_PADDING[];
ACE_EXPORT extern const char DOM_TEXT_ON_COLOR[];
ACE_EXPORT extern const char DOM_TEXT_OFF_COLOR[];
ACE_EXPORT extern const char DOM_TRACK_COLOR_ON[];
ACE_EXPORT extern const char DOM_TRACK_COLOR_OFF[];

// tabs attribute and style constants
ACE_EXPORT extern const char DOM_TAB_BAR_MODE[];
ACE_EXPORT extern const char DOM_TAB_CONTENT_SCROLLABEL[];

// refresh attribute and style constants
ACE_EXPORT extern const char DOM_REFRESH[];
ACE_EXPORT extern const char DOM_REFRESH_OFFSET[];
ACE_EXPORT extern const char DOM_REFRESH_REFRESHING[];
ACE_EXPORT extern const char DOM_REFRESH_TYPE[];
ACE_EXPORT extern const char DOM_REFRESH_LASTTIME[];
ACE_EXPORT extern const char DOM_REFRESH_FRICTION[];
ACE_EXPORT extern const char DOM_REFRESH_BACKGROUND_COLOR[];
ACE_EXPORT extern const char DOM_REFRESH_PROGRESS_COLOR[];
ACE_EXPORT extern const char DOM_REFRESH_EVENT_PULL_DOWN[];
ACE_EXPORT extern const char DOM_REFRESH_TIME_OFFSET[];

// rating attribute and style constants
ACE_EXPORT extern const char DOM_STAR_NUM[];
ACE_EXPORT extern const char DOM_RATING_SCORE[];
ACE_EXPORT extern const char DOM_RATING_STEP[];
ACE_EXPORT extern const char DOM_RATING_INDICATOR[];
ACE_EXPORT extern const char DOM_BACKGROUND_SRC[];
ACE_EXPORT extern const char DOM_FOREGROUND_SRC[];
ACE_EXPORT extern const char DOM_SECONDARY_SRC[];
ACE_EXPORT extern const char DOM_RTL_FLIP[];

// marquee attribute and style constants
ACE_EXPORT extern const char DOM_MARQUEE_LOOP[];
ACE_EXPORT extern const char DOM_MARQUEE_SCROLL_AMOUNT[];
ACE_EXPORT extern const char DOM_MARQUEE_DIRECTION[];
ACE_EXPORT extern const char DOM_MARQUEE_DIRECTION_LEFT[];
ACE_EXPORT extern const char DOM_MARQUEE_DIRECTION_RIGHT[];
ACE_EXPORT extern const char DOM_MARQUEE_VALUE[];
ACE_EXPORT extern const char DOM_MARQUEE_COLOR[];
ACE_EXPORT extern const char DOM_MARQUEE_FONT_SIZE[];
ACE_EXPORT extern const char DOM_MARQUEE_FONT_WEIGHT[];
ACE_EXPORT extern const char DOM_MARQUEE_FONT_FAMILY[];
ACE_EXPORT extern const char DOM_MARQUEE_EVENT_BOUNCE[];
ACE_EXPORT extern const char DOM_MARQUEE_EVENT_FINISH[];
ACE_EXPORT extern const char DOM_MARQUEE_EVENT_START[];
ACE_EXPORT extern const char DOM_MARQUEE_METHOD_START[];
ACE_EXPORT extern const char DOM_MARQUEE_METHOD_STOP[];

// navigationBar attribute and style constants
ACE_EXPORT extern const char DOM_NAVIGATION_BAR_METHOD_SHOW[];
ACE_EXPORT extern const char DOM_NAVIGATION_BAR_METHOD_HIDE[];
ACE_EXPORT extern const char DOM_NAVIGATION_BAR_TYPE[];
ACE_EXPORT extern const char DOM_NAVIGATION_BAR_HEADER[];
ACE_EXPORT extern const char DOM_NAVIGATION_BAR_TITLE[];
ACE_EXPORT extern const char DOM_NAVIGATION_BAR_SUBTITLE[];
ACE_EXPORT extern const char DOM_NAVIGATION_BAR_BACK_ENABLED[];
ACE_EXPORT extern const char DOM_NAVIGATION_BAR_LOGO[];
ACE_EXPORT extern const char DOM_NAVIGATION_BAR_START_ICON[];
ACE_EXPORT extern const char DOM_NAVIGATION_BAR_END_ICON[];
ACE_EXPORT extern const char DOM_NAVIGATION_BAR_TITLE_COLOR[];
ACE_EXPORT extern const char DOM_NAVIGATION_BAR_SUBTITLE_COLOR[];
ACE_EXPORT extern const char DOM_NAVIGATION_BAR_EVENT_BACK_CLICK[];
ACE_EXPORT extern const char DOM_NAVIGATION_BAR_EVENT_START_CLICK[];
ACE_EXPORT extern const char DOM_NAVIGATION_BAR_EVENT_END_CLICK[];

// common event list
ACE_EXPORT extern const char DOM_CHANGE[];
ACE_EXPORT extern const char DOM_TOUCH_START[];
ACE_EXPORT extern const char DOM_TOUCH_MOVE[];
ACE_EXPORT extern const char DOM_TOUCH_CANCEL[];
ACE_EXPORT extern const char DOM_TOUCH_END[];
ACE_EXPORT extern const char DOM_CAPTURE_TOUCH_START[];
ACE_EXPORT extern const char DOM_CAPTURE_TOUCH_MOVE[];
ACE_EXPORT extern const char DOM_CAPTURE_TOUCH_END[];
ACE_EXPORT extern const char DOM_CAPTURE_TOUCH_CANCEL[];
ACE_EXPORT extern const char DOM_CATCH_BUBBLE_TOUCH_START[];
ACE_EXPORT extern const char DOM_CATCH_BUBBLE_TOUCH_MOVE[];
ACE_EXPORT extern const char DOM_CATCH_BUBBLE_TOUCH_CANCEL[];
ACE_EXPORT extern const char DOM_CATCH_BUBBLE_TOUCH_END[];
ACE_EXPORT extern const char DOM_CATCH_CAPTURE_TOUCH_START[];
ACE_EXPORT extern const char DOM_CATCH_CAPTURE_TOUCH_MOVE[];
ACE_EXPORT extern const char DOM_CATCH_CAPTURE_TOUCH_CANCEL[];
ACE_EXPORT extern const char DOM_CATCH_CAPTURE_TOUCH_END[];
ACE_EXPORT extern const char DOM_CLICK[];
ACE_EXPORT extern const char DOM_DRAG_START[];
ACE_EXPORT extern const char DOM_DRAG[];
ACE_EXPORT extern const char DOM_DRAG_END[];
ACE_EXPORT extern const char DOM_DRAG_ENTER[];
ACE_EXPORT extern const char DOM_DRAG_OVER[];
ACE_EXPORT extern const char DOM_DRAG_LEAVE[];
ACE_EXPORT extern const char DOM_DRAG_DROP[];
ACE_EXPORT extern const char DOM_CATCH_BUBBLE_CLICK[];
ACE_EXPORT extern const char DOM_DOUBLE_CLICK[];
ACE_EXPORT extern const char DOM_LONG_PRESS[];
ACE_EXPORT extern const char DOM_PINCH_START[];
ACE_EXPORT extern const char DOM_PINCH_UPDATE[];
ACE_EXPORT extern const char DOM_PINCH_END[];
ACE_EXPORT extern const char DOM_PINCH_CANCEL[];
ACE_EXPORT extern const char DOM_COMPLETE[];
ACE_EXPORT extern const char DOM_ERROR[];
ACE_EXPORT extern const char DOM_FOCUS[];
ACE_EXPORT extern const char DOM_BLUR[];
ACE_EXPORT extern const char DOM_KEY[];
ACE_EXPORT extern const char DOM_SELECTED[];
ACE_EXPORT extern const char DOM_CANCEL[];
ACE_EXPORT extern const char DOM_HIDE[];
ACE_EXPORT extern const char DOM_MOUSE[];
ACE_EXPORT extern const char DOM_HOVER[];
ACE_EXPORT extern const char DOM_SWIPE[];
ACE_EXPORT extern const char DOM_ROTATION[];
ACE_EXPORT extern const char DOM_ANIMATION_FINISH[];

// video constants
ACE_EXPORT extern const char DOM_VIDEO_MUTED[];
ACE_EXPORT extern const char DOM_VIDEO_SRC[];
ACE_EXPORT extern const char DOM_VIDEO_AUTOPLAY[];
ACE_EXPORT extern const char DOM_VIDEO_POSTER[];
ACE_EXPORT extern const char DOM_VIDEO_CONTROLS[];
ACE_EXPORT extern const char DOM_VIDEO_FIT[];
ACE_EXPORT extern const char DOM_VIDEO_LOOP[];
ACE_EXPORT extern const char DOM_VIDEO_START_TIME[];
ACE_EXPORT extern const char DOM_VIDEO_EVENT_PREPARED[];
ACE_EXPORT extern const char DOM_VIDEO_EVENT_START[];
ACE_EXPORT extern const char DOM_VIDEO_EVENT_PAUSE[];
ACE_EXPORT extern const char DOM_VIDEO_EVENT_STOP[];
ACE_EXPORT extern const char DOM_VIDEO_EVENT_FINISH[];
ACE_EXPORT extern const char DOM_VIDEO_EVENT_ERROR[];
ACE_EXPORT extern const char DOM_VIDEO_EVENT_SEEKING[];
ACE_EXPORT extern const char DOM_VIDEO_EVENT_SEEKED[];
ACE_EXPORT extern const char DOM_VIDEO_EVENT_TIMEUPDATE[];
ACE_EXPORT extern const char DOM_VIDEO_EVENT_FULLSCREENCHANGE[];
ACE_EXPORT extern const char DOM_VIDEO_METHOD_START[];
ACE_EXPORT extern const char DOM_VIDEO_METHOD_PAUSE[];
ACE_EXPORT extern const char DOM_VIDEO_METHOD_STOP[];
ACE_EXPORT extern const char DOM_VIDEO_METHOD_SEEK_TO[];
ACE_EXPORT extern const char DOM_VIDEO_METHOD_REQUEST_FULLSCREEN[];
ACE_EXPORT extern const char DOM_VIDEO_METHOD_EXIT_FULLSCREEN[];
ACE_EXPORT extern const char DOM_VIDEO_SPEED[];
ACE_EXPORT extern const char DOM_VIDEO_DIRECTION[];
ACE_EXPORT extern const char DOM_VIDEO_POSITION[];
ACE_EXPORT extern const char DOM_VIDEO_POSITION_LEFT[];
ACE_EXPORT extern const char DOM_VIDEO_POSITION_CENTER[];
ACE_EXPORT extern const char DOM_VIDEO_POSITION_RIGHT[];
ACE_EXPORT extern const char DOM_VIDEO_POSITION_TOP[];
ACE_EXPORT extern const char DOM_VIDEO_POSITION_BOTTOM[];

// textarea attribute and style constants
ACE_EXPORT extern const char DOM_TEXTAREA_VALUE[];
ACE_EXPORT extern const char DOM_TEXTAREA_PLACEHOLDER[];
ACE_EXPORT extern const char DOM_TEXTAREA_MAXLENGTH[];
ACE_EXPORT extern const char DOM_TEXTAREA_MAXLINES[];
ACE_EXPORT extern const char DOM_TEXTAREA_OBSCURE[];
ACE_EXPORT extern const char DOM_TEXTAREA_PLACEHOLDER_COLOR[];
ACE_EXPORT extern const char DOM_TEXTAREA_CURSOR_COLOR[];
ACE_EXPORT extern const char DOM_TEXTAREA_COLOR[];
ACE_EXPORT extern const char DOM_TEXTAREA_FONT_SIZE[];
ACE_EXPORT extern const char DOM_TEXTAREA_FONT_WEIGHT[];
ACE_EXPORT extern const char DOM_TEXTAREA_FONT_FAMILY[];
ACE_EXPORT extern const char DOM_TEXTAREA_METHOD_FOCUS[];
ACE_EXPORT extern const char DOM_TEXTAREA_EXTEND[];
ACE_EXPORT extern const char DOM_TEXTAREA_SHOW_COUNTER[];

// input attribute and style extern constants
ACE_EXPORT extern const char DOM_INPUT_TYPE[];
ACE_EXPORT extern const char DOM_INPUT_TYPE_EMAIL[];
ACE_EXPORT extern const char DOM_INPUT_TYPE_DATE[];
ACE_EXPORT extern const char DOM_INPUT_TYPE_TIME[];
ACE_EXPORT extern const char DOM_INPUT_TYPE_NUMBER[];
ACE_EXPORT extern const char DOM_INPUT_TYPE_PASSWORD[];
ACE_EXPORT extern const char DOM_INPUT_CHECKED[];
ACE_EXPORT extern const char DOM_INPUT_NAME[];
ACE_EXPORT extern const char DOM_INPUT_VALUE[];
ACE_EXPORT extern const char DOM_INPUT_PLACEHOLDER[];
ACE_EXPORT extern const char DOM_INPUT_MAXLENGTH[];
ACE_EXPORT extern const char DOM_INPUT_ENTERKEYTYPE[];
ACE_EXPORT extern const char DOM_INPUT_PLACEHOLDER_COLOR[];
ACE_EXPORT extern const char DOM_INPUT_CURSOR_COLOR[];
ACE_EXPORT extern const char DOM_INPUT_COLOR[];
ACE_EXPORT extern const char DOM_INPUT_FONT_SIZE[];
ACE_EXPORT extern const char DOM_INPUT_FONT_WEIGHT[];
ACE_EXPORT extern const char DOM_INPUT_WIDTH[];
ACE_EXPORT extern const char DOM_INPUT_HEIGHT[];
ACE_EXPORT extern const char DOM_INPUT_FONT_FAMILY[];
ACE_EXPORT extern const char DOM_INPUT_OBSCURE[];
ACE_EXPORT extern const char DOM_INPUT_AUTO_FOCUS[];
ACE_EXPORT extern const char DOM_INPUT_RECT_RADIUS[];
ACE_EXPORT extern const char DOM_INPUT_BACKGROUND_COLOR[];
ACE_EXPORT extern const char DOM_INPUT_CLICKED_COLOR[];
ACE_EXPORT extern const char DOM_INPUT_DISABLE_COLOR[];
ACE_EXPORT extern const char DOM_INPUT_FOCUS_COLOR[];
ACE_EXPORT extern const char DOM_INPUT_METHOD_FOCUS[];
ACE_EXPORT extern const char DOM_ICON_SRC[];
ACE_EXPORT extern const char DOM_SHOW_ICON_SRC[];
ACE_EXPORT extern const char DOM_HIDE_ICON_SRC[];
ACE_EXPORT extern const char DOM_INPUT_SHOW_PASSWORD_ICON[];
ACE_EXPORT extern const char DOM_INPUT_SHOW_COUNTER[];
ACE_EXPORT extern const char DOM_INPUT_OPTION_ICON[];
ACE_EXPORT extern const char DOM_INPUT_OPTION_CONTENT[];
ACE_EXPORT extern const char DOM_INPUT_SELECTED_START[];
ACE_EXPORT extern const char DOM_INPUT_SELECTED_END[];
ACE_EXPORT extern const char DOM_INPUT_SOFT_KEYBOARD_ENABLED[];
ACE_EXPORT extern const char DOM_INPUT_METHOD_SHOW_ERROR[];
ACE_EXPORT extern const char DOM_INPUT_METHOD_DELETE[];
ACE_EXPORT extern const char DOM_INPUT_METHOD_INSERT[];

// input event extern constants
ACE_EXPORT extern const char DOM_INPUT_EVENT_NAME[];
ACE_EXPORT extern const char DOM_INPUT_EVENT_VALUE[];
ACE_EXPORT extern const char DOM_INPUT_EVENT_CHECKED[];
ACE_EXPORT extern const char DOM_INPUT_EVENT_ENTERKEYCLICK[];
ACE_EXPORT extern const char DOM_INPUT_EVENT_OPTION_SELECT[];
ACE_EXPORT extern const char DOM_INPUT_EVENT_TRANSLATE[];
ACE_EXPORT extern const char DOM_INPUT_EVENT_SHARE[];
ACE_EXPORT extern const char DOM_INPUT_EVENT_SEARCH[];
ACE_EXPORT extern const char DOM_INPUT_EVENT_SELECT_CHANGE[];

// divider style extern constants
ACE_EXPORT extern const char DOM_DIVIDER_VERTICAL[];
ACE_EXPORT extern const char DOM_DIVIDER_STROKE_WIDTH[];
ACE_EXPORT extern const char DOM_DIVIDER_LINE_CAP[];

// canvas
ACE_EXPORT extern const char DOM_CANVAS_METHOD_GET_CONTEXT[];

// button attribute and style constants
ACE_EXPORT extern const char DOM_BUTTON_TYPE[];
ACE_EXPORT extern const char DOM_BUTTON_HEIGHT[];
ACE_EXPORT extern const char DOM_BUTTON_WIDTH[];
ACE_EXPORT extern const char DOM_BUTTON_MIN_WIDTH[];
ACE_EXPORT extern const char DOM_BUTTON_RRECT_RADIUS[];
ACE_EXPORT extern const char DOM_BUTTON_DEFAULT_COLOR[];
ACE_EXPORT extern const char DOM_BUTTON_CLICKED_COLOR[];
ACE_EXPORT extern const char DOM_BUTTON_DISABLE_COLOR[];
ACE_EXPORT extern const char DOM_BUTTON_FOCUS_COLOR[];
ACE_EXPORT extern const char DOM_BUTTON_AUTO_FOCUS[];
ACE_EXPORT extern const char DOM_BUTTON_TEXT_DATA[];
ACE_EXPORT extern const char DOM_BUTTON_TEXT_COLOR[];
ACE_EXPORT extern const char DOM_BUTTON_TEXT_DISABLE_COLOR[];
ACE_EXPORT extern const char DOM_BUTTON_FONT_SIZE[];
ACE_EXPORT extern const char DOM_BUTTON_FONT_WEIGHT[];
ACE_EXPORT extern const char DOM_BUTTON_FONT_FAMILY[];
ACE_EXPORT extern const char DOM_BUTTON_FONT_STYLE[];
ACE_EXPORT extern const char DOM_BUTTON_BORDER_WIDTH[];
ACE_EXPORT extern const char DOM_BUTTON_BORDER_COLOR[];
ACE_EXPORT extern const char DOM_BUTTON_WAITING[];
ACE_EXPORT extern const char DOM_BUTTON_ICON[];
ACE_EXPORT extern const char DOM_BUTTON_ICON_WIDTH[];
ACE_EXPORT extern const char DOM_BUTTON_ICON_HEIGHT[];
ACE_EXPORT extern const char DOM_BUTTON_ICON_DIRECTION[];
ACE_EXPORT extern const char DOM_BUTTON_PROGRESS_DIAMETER[];
ACE_EXPORT extern const char DOM_BUTTON_PROGRESS_COLOR[];
ACE_EXPORT extern const char DOM_BUTTON_PROGRESS_FOCUS_COLOR[];
ACE_EXPORT extern const char DOM_BUTTON_INNER_PADDING[];
ACE_EXPORT extern const char DOM_BUTTON_METHOD_SET_PROGRESS[];

// picker attribute and style constants
ACE_EXPORT extern const char DOM_PICKER_SPLIT_ITEM;
ACE_EXPORT extern const char DOM_PICKER_SPLIT_ARRAY;
ACE_EXPORT extern const char DOM_PICKER_CONTAIN_SECOND[];
ACE_EXPORT extern const char DOM_PICKER_RANGE[];
ACE_EXPORT extern const char DOM_PICKER_COLUMN_CHANGE[];
ACE_EXPORT extern const char DOM_PICKER_COLUMN_COUNT[];
ACE_EXPORT extern const char DOM_PICKER_HOUR24[];
ACE_EXPORT extern const char DOM_PICKER_LUNAR[];
ACE_EXPORT extern const char DOM_PICKER_SHOW_LUNAR[];
ACE_EXPORT extern const char DOM_PICKER_TYPE_TIME[];
ACE_EXPORT extern const char DOM_PICKER_TYPE_DATE[];
ACE_EXPORT extern const char DOM_PICKER_TYPE_TEXT[];
ACE_EXPORT extern const char DOM_PICKER_TYPE_MULTITEXT[];
ACE_EXPORT extern const char DOM_PICKER_TYPE_DATETIME[];
ACE_EXPORT extern const char DOM_PICKER_TEXT_COLOR[];
ACE_EXPORT extern const char DOM_PICKER_SELECT_COLOR[];
ACE_EXPORT extern const char DOM_PICKER_SELECT_SIZE[];
ACE_EXPORT extern const char DOM_PICKER_SELECT_FAMILIES[];
ACE_EXPORT extern const char DOM_PICKER_FOCUS_COLOR[];
ACE_EXPORT extern const char DOM_PICKER_FOCUS_SIZE[];
ACE_EXPORT extern const char DOM_PICKER_FOCUS_FAMILIES[];
ACE_EXPORT extern const char DOM_PICKER_PREFIX[];
ACE_EXPORT extern const char DOM_PICKER_SUFFIX[];
ACE_EXPORT extern const char DOM_PICKER_DISAPPEAR_COLOR[];
ACE_EXPORT extern const char DOM_PICKER_DISAPPEAR_FONT_SIZE[];
ACE_EXPORT extern const char DOM_PICKER_COLUMN_HEIGHT[];
ACE_EXPORT extern const char DOM_PICKER_VIBRATE[];

// calendar
ACE_EXPORT extern const char DOM_CALENDAR_DATA[];
ACE_EXPORT extern const char DOM_CALENDAR_DATE[];
ACE_EXPORT extern const char DOM_CALENDAR_DATE_ADAPTER[];
ACE_EXPORT extern const char DOM_CALENDAR_DIRECTION[];
ACE_EXPORT extern const char DOM_CALENDAR_METHOD_GO_TO[];
ACE_EXPORT extern const char DOM_CALENDAR_EVENT_SELECTED_CHANGE[];
ACE_EXPORT extern const char DOM_CALENDAR_SHOW_HOLIDAY[];
ACE_EXPORT extern const char DOM_CALENDAR_SHOW_LUNAR[];
ACE_EXPORT extern const char DOM_CALENDAR_CARD_CALENDAR[];
ACE_EXPORT extern const char DOM_CALENDAR_START_DAY_OF_WEEK[];
ACE_EXPORT extern const char DOM_CALENDAR_TYPE[];
ACE_EXPORT extern const char DOM_CALENDAR_EVENT_REQUEST_DATA[];
ACE_EXPORT extern const char DOM_CALENDAR_OFF_DAYS[];
ACE_EXPORT extern const char DOM_CALENDAR_WORK_DAYS[];
ACE_EXPORT extern const char DOM_CALENDAR_HOLIDAYS[];

// dialog
ACE_EXPORT extern const char DOM_DIALOG_METHOD_SHOW[];
ACE_EXPORT extern const char DOM_DIALOG_METHOD_CLOSE[];
ACE_EXPORT extern const char DOM_DIALOG_EVENT_CANCEL[];
ACE_EXPORT extern const char DOM_DIALOG_STYLE_DRAGABLE[];

// multi modal event
ACE_EXPORT extern const char DOM_VOICE_LABEL[];
ACE_EXPORT extern const char DOM_SUBSCRIPT_LABEL[];
ACE_EXPORT extern const char DOM_SUBSCRIPT_FLAG[];
ACE_EXPORT extern const char DOM_SCENE_LABEL[];

// search constants
ACE_EXPORT extern const char DOM_SEARCH_ICON[];
ACE_EXPORT extern const char DOM_SEARCH_HINT[];
ACE_EXPORT extern const char DOM_SUBMIT[];
ACE_EXPORT extern const char DOM_SEARCH_VALUE[];
ACE_EXPORT extern const char DOM_SEARCH_BUTTON[];

// toggle constants
ACE_EXPORT extern const char DOM_TOGGLE_BACKGROUND_COLOR[];
ACE_EXPORT extern const char DOM_TOGGLE_CHECKED_COLOR[];
ACE_EXPORT extern const char DOM_TOGGLE_CHECKED_STATE[];
ACE_EXPORT extern const char DOM_TOGGLE_TEXT_COLOR[];
ACE_EXPORT extern const char DOM_TOGGLE_HEIGHT[];
ACE_EXPORT extern const char DOM_TOGGLE_WIDTH[];

// clock constants
// clock style
ACE_EXPORT extern const char DOM_DIGIT_FONT_FAMILY[];
// clock attr
ACE_EXPORT extern const char DOM_HOURS_WEST[];
ACE_EXPORT extern const char DOM_CLOCK_CONFIG[];
ACE_EXPORT extern const char DOM_DIGIT_COLOR[];
ACE_EXPORT extern const char DOM_DIGIT_COLOR_NIGHT[];
ACE_EXPORT extern const char DOM_CLOCK_FACE_SOURCE[];
ACE_EXPORT extern const char DOM_CLOCK_FACE_SOURCE_NIGHT[];
ACE_EXPORT extern const char DOM_HOUR_HAND_SOURCE[];
ACE_EXPORT extern const char DOM_HOUR_HAND_SOURCE_NIGHT[];
ACE_EXPORT extern const char DOM_MINUTE_HAND_SOURCE[];
ACE_EXPORT extern const char DOM_MINUTE_HAND_SOURCE_NIGHT[];
ACE_EXPORT extern const char DOM_SECOND_HAND_SOURCE[];
ACE_EXPORT extern const char DOM_SECOND_HAND_SOURCE_NIGHT[];
ACE_EXPORT extern const char DOM_SHOW_DIGIT[];
ACE_EXPORT extern const char DOM_DIGIT_RADIUS_RATIO[];
ACE_EXPORT extern const char DOM_DIGIT_SIZE_RATIO[];

// piece attribute and style constants
ACE_EXPORT extern const char DOM_PIECE_CONTENT[];
ACE_EXPORT extern const char DOM_PIECE_ICON[];
ACE_EXPORT extern const char DOM_PIECE_CLOSABLE[];
ACE_EXPORT extern const char DOM_PIECE_EVENT_CLOSE[];

// badge constants
ACE_EXPORT extern const char DOM_BADGE_CONFIG[];
ACE_EXPORT extern const char DOM_BADGE_COUNT[];
ACE_EXPORT extern const char DOM_BADGE_LABEL[];
ACE_EXPORT extern const char DOM_BADGE_PLACEMENT[];
ACE_EXPORT extern const char DOM_BADGE_VISIBLE[];
ACE_EXPORT extern const char DOM_BADGE_MAX_COUNT[];
ACE_EXPORT extern const char DOM_BADGE_COLOR[];
ACE_EXPORT extern const char DOM_BADGE_CIRCLE_SIZE[];
ACE_EXPORT extern const char DOM_BADGE_TEXT_FONT_SIZE[];
ACE_EXPORT extern const char DOM_BADGE_TEXT_COLOR[];

// grid container/row/columns
ACE_EXPORT extern const char DOM_GRID_CONTAINER_COLUMNS[];
ACE_EXPORT extern const char DOM_GRID_CONTAINER_SIZE_TYPE[];
ACE_EXPORT extern const char DOM_GRID_CONTAINER_GUTTER_WIDTH[];
ACE_EXPORT extern const char DOM_GRID_CONTAINER_GUTTER[];
ACE_EXPORT extern const char DOM_GRID_CONTAINER_TEMPLATE[];
ACE_EXPORT extern const char DOM_GRID_TEMPLATE_TYPE_NORMAL[];
ACE_EXPORT extern const char DOM_GRID_TEMPLATE_TYPE_GRID[];
ACE_EXPORT extern const char DOM_GRID_CONTAINER_GET_COLUMNS[];
ACE_EXPORT extern const char DOM_GRID_CONTAINER_GET_COLUMN_WIDTH[];
ACE_EXPORT extern const char DOM_GRID_CONTAINER_GET_GUTTER_WIDTH[];
ACE_EXPORT extern const char DOM_GRID_CONTAINER_GET_SIZE_TYPE[];
ACE_EXPORT extern const char DOM_GRID_COLUMN_SPAN[];
ACE_EXPORT extern const char DOM_GRID_COLUMN_OFFSET[];

ACE_EXPORT extern const char DOM_GRID_COLUMN_TYPE[];
ACE_EXPORT extern const char DOM_GRID_SIZE_TYPE_XS[];
ACE_EXPORT extern const char DOM_GRID_SIZE_TYPE_SM[];
ACE_EXPORT extern const char DOM_GRID_SIZE_TYPE_MD[];
ACE_EXPORT extern const char DOM_GRID_SIZE_TYPE_LG[];

ACE_EXPORT extern const char DOM_COLUMN_TYPE_NONE[];
ACE_EXPORT extern const char DOM_COLUMN_TYPE_BUBBLE[];
ACE_EXPORT extern const char DOM_COLUMN_TYPE_BUTTON[];
ACE_EXPORT extern const char DOM_COLUMN_TYPE_CARD[];
ACE_EXPORT extern const char DOM_COLUMN_TYPE_CONTENT[];

ACE_EXPORT extern const char DOM_CLICK_EFFECT[];
ACE_EXPORT extern const char DOM_SPRING_EFFECT[];
// panel attribute and style constants
ACE_EXPORT extern const char DOM_PANEL_METHOD_SHOW[];
ACE_EXPORT extern const char DOM_PANEL_METHOD_CLOSE[];
ACE_EXPORT extern const char DOM_PANEL_ATTR_TYPE[];
ACE_EXPORT extern const char DOM_PANEL_ATTR_MODE[];
ACE_EXPORT extern const char DOM_PANEL_ATTR_DRAG_BAR[];
ACE_EXPORT extern const char DOM_PANEL_ATTR_MIN_HEIGHT[];
ACE_EXPORT extern const char DOM_PANEL_ATTR_HALF_HEIGHT[];
ACE_EXPORT extern const char DOM_PANEL_ATTR_FULL_HEIGHT[];
ACE_EXPORT extern const char DOM_PANEL_EVENT_SIZE_CHANGED[];

// tool-bar-item constants
ACE_EXPORT extern const char DOM_TOOL_BAR_ITEM_ICON[];
ACE_EXPORT extern const char DOM_TOOL_BAR_ITEM_VALUE[];
ACE_EXPORT extern const char DOM_TOOL_BAR_ITEM_TEXT_COLOR[];
ACE_EXPORT extern const char DOM_TOOL_BAR_ITEM_COLOR[];
ACE_EXPORT extern const char DOM_TOOL_BAR_ITEM_FONT_SIZE[];
ACE_EXPORT extern const char DOM_TOOL_BAR_ITEM_FONT_STYLE[];
ACE_EXPORT extern const char DOM_TOOL_BAR_ITEM_FONT_WEIGHT[];
ACE_EXPORT extern const char DOM_TOOL_BAR_ITEM_TEXT_DECORATION[];
ACE_EXPORT extern const char DOM_TOOL_BAR_ITEM_FONT_FAMILY[];
ACE_EXPORT extern const char DOM_TOOL_BAR_ITEM_ALLOW_SCALE[];

// card transition
ACE_EXPORT extern const char DOM_TRANSITION_EFFECT[];
ACE_EXPORT extern const char DOM_TRANSITION_CARD_COMPOSEID[];
ACE_EXPORT extern const char DOM_TRANSITION_CARD_PARAMS[];

// qrcode
ACE_EXPORT extern const char DOM_QRCODE_BACKGROUND_COLOR[];
ACE_EXPORT extern const char DOM_QRCODE_COLOR[];
ACE_EXPORT extern const char DOM_QRCODE_HEIGHT[];
ACE_EXPORT extern const char DOM_QRCODE_WIDTH[];
ACE_EXPORT extern const char DOM_QRCODE_TYPE[];
ACE_EXPORT extern const char DOM_QRCODE_VALUE[];

// svg
ACE_EXPORT extern const char DOM_SVG_ID[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_BEGIN[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_DUR[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_END[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_MIN[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_MAX[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_RESTART[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_REPEAT_COUNT[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_REPEAT_DUR[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_FILL[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_CALC_MODE[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_VALUES[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_KEY_TIMES[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_KEY_SPLINES[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_FROM[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_TO[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_BY[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_ATTRIBUTE_NAME[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_ADDITIVE[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_ACCUMULATE[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_TYPE[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_KEY_POINTS[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_PATH[];
ACE_EXPORT extern const char DOM_SVG_ANIMATION_ROTATE[];
ACE_EXPORT extern const char DOM_SVG_ATTR_PATH[];
ACE_EXPORT extern const char DOM_SVG_START_OFFSET[];
ACE_EXPORT extern const char DOM_SVG_FILL[];
ACE_EXPORT extern const char DOM_SVG_FILL_OPACITY[];
ACE_EXPORT extern const char DOM_SVG_FILL_RULE[];
ACE_EXPORT extern const char DOM_SVG_FILTER[];
ACE_EXPORT extern const char DOM_SVG_FONT_FAMILY[];
ACE_EXPORT extern const char DOM_SVG_FONT_SIZE[];
ACE_EXPORT extern const char DOM_SVG_FONT_STYLE[];
ACE_EXPORT extern const char DOM_SVG_FONT_WEIGHT[];
ACE_EXPORT extern const char DOM_SVG_LETTER_SPACING[];
ACE_EXPORT extern const char DOM_SVG_MASK[];
ACE_EXPORT extern const char DOM_SVG_TEXT_DECORATION[];
ACE_EXPORT extern const char DOM_SVG_TEXT_LENGTH[];
ACE_EXPORT extern const char DOM_SVG_STROKE[];
ACE_EXPORT extern const char DOM_SVG_STROKE_DASHARRAY[];
ACE_EXPORT extern const char DOM_SVG_STROKE_DASHOFFSET[];
ACE_EXPORT extern const char DOM_SVG_STROKE_LINECAP[];
ACE_EXPORT extern const char DOM_SVG_STROKE_LINEJOIN[];
ACE_EXPORT extern const char DOM_SVG_STROKE_MITERLIMIT[];
ACE_EXPORT extern const char DOM_SVG_STROKE_OPACITY[];
ACE_EXPORT extern const char DOM_SVG_STROKE_WIDTH[];
ACE_EXPORT extern const char DOM_SVG_OPACITY[];
ACE_EXPORT extern const char DOM_SVG_VIEW_BOX[];
ACE_EXPORT extern const char DOM_SVG_X[];
ACE_EXPORT extern const char DOM_SVG_Y[];
ACE_EXPORT extern const char DOM_SVG_WIDTH[];
ACE_EXPORT extern const char DOM_SVG_HEIGHT[];
ACE_EXPORT extern const char DOM_SVG_DX[];
ACE_EXPORT extern const char DOM_SVG_DY[];
ACE_EXPORT extern const char DOM_SVG_RX[];
ACE_EXPORT extern const char DOM_SVG_RY[];
ACE_EXPORT extern const char DOM_SVG_D[];
ACE_EXPORT extern const char DOM_SVG_X1[];
ACE_EXPORT extern const char DOM_SVG_X2[];
ACE_EXPORT extern const char DOM_SVG_Y1[];
ACE_EXPORT extern const char DOM_SVG_Y2[];
ACE_EXPORT extern const char DOM_SVG_POINTS[];
ACE_EXPORT extern const char DOM_SVG_CX[];
ACE_EXPORT extern const char DOM_SVG_CY[];
ACE_EXPORT extern const char DOM_SVG_R[];
ACE_EXPORT extern const char DOM_SVG_ROTATE[];
ACE_EXPORT extern const char DOM_SVG_LENGTH_ADJUST[];
ACE_EXPORT extern const char DOM_SVG_MIRROR[];
ACE_EXPORT extern const char DOM_SVG_MASK_CONTENT_UNITS[];
ACE_EXPORT extern const char DOM_SVG_MASK_UNITS[];
ACE_EXPORT extern const char DOM_SVG_PATTERN_CONTENT_UNITS[];
ACE_EXPORT extern const char DOM_SVG_PATTERN_UNITS[];
ACE_EXPORT extern const char DOM_SVG_PATTERN_TRANSFORM[];
ACE_EXPORT extern const char DOM_SVG_OFFSET[];
ACE_EXPORT extern const char DOM_SVG_STOP_COLOR[];
ACE_EXPORT extern const char DOM_SVG_STOP_OPACITY[];
ACE_EXPORT extern const char DOM_SVG_GRADIENT_TRANSFORM[];
ACE_EXPORT extern const char DOM_SVG_SPREAD_METHOD[];
ACE_EXPORT extern const char DOM_SVG_FX[];
ACE_EXPORT extern const char DOM_SVG_FY[];
ACE_EXPORT extern const char DOM_SVG_FR[];
ACE_EXPORT extern const char DOM_SVG_HREF[];
ACE_EXPORT extern const char DOM_SVG_XLINK_HREF[];
ACE_EXPORT extern const char DOM_SVG_CLIP_RULE[];
ACE_EXPORT extern const char DOM_SVG_FE_IN[];
ACE_EXPORT extern const char DOM_SVG_FE_IN2[];
ACE_EXPORT extern const char DOM_SVG_FE_RESULT[];
ACE_EXPORT extern const char DOM_SVG_FE_COLOR_INTERPOLATION_FILTERS[];
ACE_EXPORT extern const char DOM_SVG_FE_TYPE[];
ACE_EXPORT extern const char DOM_SVG_FE_VALUES[];
ACE_EXPORT extern const char DOM_SVG_FE_FLOOD_COLOR[];
ACE_EXPORT extern const char DOM_SVG_FE_FLOOD_OPACITY[];
ACE_EXPORT extern const char DOM_SVG_FE_EDGE_MODE[];
ACE_EXPORT extern const char DOM_SVG_FE_STD_DEVIATION[];
ACE_EXPORT extern const char DOM_SVG_FE_AMPLITUDE[];
ACE_EXPORT extern const char DOM_SVG_FE_EXPONENT[];
ACE_EXPORT extern const char DOM_SVG_FE_INTERCEPT[];
ACE_EXPORT extern const char DOM_SVG_FE_SLOPE[];
ACE_EXPORT extern const char DOM_SVG_FE_TABLE_VALUES[];
ACE_EXPORT extern const char DOM_SVG_FE_K1[];
ACE_EXPORT extern const char DOM_SVG_FE_K2[];
ACE_EXPORT extern const char DOM_SVG_FE_K3[];
ACE_EXPORT extern const char DOM_SVG_FE_K4[];
ACE_EXPORT extern const char DOM_SVG_FE_OPERATOR_TYPE[];


// web
ACE_EXPORT extern const char DOM_PAGESTART[];
ACE_EXPORT extern const char DOM_PAGEFINISH[];
ACE_EXPORT extern const char DOM_PAGEERROR[];
ACE_EXPORT extern const char DOM_WEB_WEBSRC[];
ACE_EXPORT extern const char DOM_METHOD_RELOAD[];
ACE_EXPORT extern const char DOM_WEB_MESSAGE[];

// richtext
ACE_EXPORT extern const char DOM_RICH_TEXT_DATA[];
ACE_EXPORT extern const char DOM_LOAD_START[];
ACE_EXPORT extern const char DOM_LOAD_COMPLETE[];

// xcomponent
ACE_EXPORT extern const char DOM_XCOMPONENT_NAME[];
ACE_EXPORT extern const char DOM_XCOMPONENT_TYPE[];
ACE_EXPORT extern const char DOM_XCOMPONENT_LIBRARYNAME[];
ACE_EXPORT extern const char DOM_XCOMPONENT_INIT[];
ACE_EXPORT extern const char DOM_XCOMPONENT_DESTROY[];
// z-index
ACE_EXPORT extern const char DOM_ZINDEX[];

// camera
ACE_EXPORT extern const char DOM_TAKE_PHOTO[];
ACE_EXPORT extern const char DOM_CAMERA_FLASH[];
ACE_EXPORT extern const char DOM_CAMERA_DEVICE_POSITION[];
ACE_EXPORT extern const char DOM_CAMERA_START_RECORD[];
ACE_EXPORT extern const char DOM_CAMERA_CLOSE_RECORDER[];

// form
ACE_EXPORT extern const char DOM_FORM_EVENT_SUBMIT[];
ACE_EXPORT extern const char DOM_FORM_EVENT_RESET[];

// crown
ACE_EXPORT extern const char DOM_CROWN_ROTATE[];

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TYPE_H
