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

#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace {

// Use DOM_ROOT_NODE_ID_BASE plus page id as the root node id, to ensure it is unique,
// if not, composedElementMap_ in PipelineContext would have problem when doing page
// routing, such as pageA --> PageB --> PageA.
const NodeId DOM_ROOT_NODE_ID_BASE = 1000000;
const char DOM_DEFAULT_ROOT_NODE_ID[] = "_root";
// node tag defines
const char DOM_NODE_TAG_DIV[] = "div";
const char DOM_NODE_TAG_CALENDAR[] = "calendar";
const char DOM_NODE_TAG_GRID_COLUMN[] = "grid-col";
const char DOM_NODE_TAG_GRID_CONTAINER[] = "grid-container";
const char DOM_NODE_TAG_GRID_ROW[] = "grid-row";
const char DOM_NODE_TAG_SVG_TEXT[] = "svg-text";
const char DOM_NODE_TAG_TEXT[] = "text";
const char DOM_NODE_TAG_TEXT_PATH[] = "textpath";
const char DOM_NODE_TAG_TSPAN[] = "tspan";
const char DOM_NODE_TAG_PICKER_DIALOG[] = "picker";
const char DOM_NODE_TAG_PICKER_VIEW[] = "picker-view";
const char DOM_NODE_TAG_OPTION[] = "option";
const char DOM_NODE_TAG_POPUP[] = "popup";
const char DOM_NODE_TAG_PROGRESS[] = "progress";
const char DOM_NODE_TAG_SELECT[] = "select";
const char DOM_NODE_TAG_MASK[] = "mask";
const char DOM_NODE_TAG_MENU[] = "menu";
const char DOM_NODE_TAG_SLIDER[] = "slider";
const char DOM_NODE_TAG_SPAN[] = "span";
const char DOM_NODE_TAG_STACK[] = "stack";
const char DOM_NODE_TAG_SWIPER[] = "swiper";
const char DOM_NODE_TAG_SWITCH[] = "switch";
const char DOM_NODE_TAG_TABS[] = "tabs";
const char DOM_NODE_TAG_TAB_BAR[] = "tab-bar";
const char DOM_NODE_TAG_TAB_CONTENT[] = "tab-content";
const char DOM_NODE_TAG_REFRESH[] = "refresh";
const char DOM_NODE_TAG_IMAGE[] = "image";
const char DOM_NODE_TAG_LIST[] = "list";
const char DOM_NODE_TAG_LIST_ITEM[] = "list-item";
const char DOM_NODE_TAG_LIST_ITEM_GROUP[] = "list-item-group";
const char DOM_NODE_TAG_VIDEO[] = "video";
const char DOM_NODE_TAG_RATING[] = "rating";
const char DOM_NODE_TAG_MARQUEE[] = "marquee";
const char DOM_NODE_TAG_NAVIGATION_BAR[] = "navigation-bar";
const char DOM_NODE_TAG_NAVIGATION_MENU[] = "navigation-menu";
const char DOM_NODE_TAG_TEXTAREA[] = "textarea";
const char DOM_NODE_TAG_INPUT[] = "input";
const char DOM_NODE_TAG_LABEL[] = "label";
const char DOM_NODE_TAG_DIVIDER[] = "divider";
const char DOM_NODE_TAG_CANVAS[] = "canvas";
const char DOM_NODE_TAG_BUTTON[] = "button";
const char DOM_NODE_TAG_CHART[] = "chart";
const char DOM_NODE_TAG_DIALOG[] = "dialog";
const char DOM_NODE_TAG_SEARCH[] = "search";
const char DOM_NODE_TAG_IMAGE_ANIMATOR[] = "image-animator";
const char DOM_NODE_TAG_TOGGLE[] = "toggle";
const char DOM_NODE_TAG_CLOCK[] = "clock";
const char DOM_NODE_TAG_STEPPER[] = "stepper";
const char DOM_NODE_TAG_STEPPER_ITEM[] = "stepper-item";
const char DOM_NODE_TAG_PIECE[] = "piece";
const char DOM_NODE_TAG_BADGE[] = "badge";
const char DOM_NODE_TAG_PANEL[] = "panel";
const char DOM_NODE_TAG_TOOL_BAR[] = "toolbar";
const char DOM_NODE_TAG_TOOL_BAR_ITEM[] = "toolbar-item";
const char DOM_NODE_TAG_QRCODE[] = "qrcode";
const char DOM_NODE_TAG_SVG[] = "svg";
const char DOM_NODE_TAG_G[] = "g";
const char DOM_NODE_TAG_USE[] = "use";
const char DOM_NODE_TAG_DEFS[] = "defs";
const char DOM_NODE_TAG_SYMBOL[] = "symbol";
const char DOM_NODE_TAG_RECT[] = "rect";
const char DOM_NODE_TAG_ANIMATE[] = "animate";
const char DOM_NODE_TAG_ANIMATE_TRANSFORM[] = "animatetransform";
const char DOM_NODE_TAG_WEB[] = "web";
const char DOM_NODE_TAG_RICH_TEXT[] = "richtext";
const char DOM_NODE_TAG_XCOMPONENT[] = "xcomponent";
const char DOM_NODE_TAG_PATH[] = "path";
const char DOM_NODE_TAG_LINE[] = "line";
const char DOM_NODE_TAG_POLYLINE[] = "polyline";
const char DOM_NODE_TAG_POLYGON[] = "polygon";
const char DOM_NODE_TAG_CIRCLE[] = "circle";
const char DOM_NODE_TAG_ELLIPSE[] = "ellipse";
const char DOM_NODE_TAG_FORM[] = "form";
const char DOM_NODE_TAG_ANIMATE_MOTION[] = "animatemotion";
const char DOM_NODE_TAG_CAMERA[] = "camera";
const char DOM_NODE_TAG_STOP[] = "stop";
const char DOM_NODE_TAG_LINEAR_GRADIENT[] = "linearGradient";
const char DOM_NODE_TAG_RADIAL_GRADIENT[] = "radialGradient";
const char DOM_NODE_TAG_FILTER[] = "filter";
const char DOM_NODE_TAG_FE_COLORMATRIX[] = "fecolormatrix";
const char DOM_NODE_TAG_FE_COMPOSITE[] = "fecomposite";
const char DOM_NODE_TAG_FE_GAUSSIANBLUR[] = "fegaussianblur";
const char DOM_NODE_TAG_FE_OFFSET[] = "feoffset";

// common attribute constants
const char DOM_ID[] = "id";
const char DOM_SHARE_ID[] = "shareid";
const char DOM_SHOW[] = "show";
const char DOM_DIR[] = "dir";
const char DOM_AUTO_FOCUS[] = "autofocus";

// common persudo class style constants
const char DOM_PSEUDO_CLASS_SYMBOL[] = ":";
const char DOM_ACTIVE_PSEUDO_CLASS[] = ":active";
const char DOM_DISABLED_PSEUDO_CLASS[] = ":disabled";
const char DOM_FOCUS_PSEUDO_CLASS[] = ":focus";
const char DOM_CHECKED_PSEUDO_CLASS[] = ":checked";
const char DOM_WAITING_PSEUDO_CLASS[] = ":waiting";
const char DOM_HOVER_PSEUDO_CLASS[] = ":hover";

// common style constants
const char DOM_WIDTH[] = "width";
const char DOM_HEIGHT[] = "height";
const char DOM_TYPE[] = "type";
const char DOM_VALUE[] = "value";
const char DOM_START[] = "start";
const char DOM_END[] = "end";
const char DOM_TITLE[] = "title";
const char DOM_TOUCHABLE[] = "touchable";
const char DOM_DISABLED[] = "disabled";
const char DOM_PADDING[] = "padding";
const char DOM_PADDING_TOP[] = "paddingTop";
const char DOM_PADDING_RIGHT[] = "paddingRight";
const char DOM_PADDING_BOTTOM[] = "paddingBottom";
const char DOM_PADDING_LEFT[] = "paddingLeft";
const char DOM_PADDING_START[] = "paddingStart";
const char DOM_PADDING_END[] = "paddingEnd";
const char DOM_MARGIN[] = "margin";
const char DOM_MARGIN_TOP[] = "marginTop";
const char DOM_MARGIN_RIGHT[] = "marginRight";
const char DOM_MARGIN_BOTTOM[] = "marginBottom";
const char DOM_MARGIN_LEFT[] = "marginLeft";
const char DOM_MARGIN_START[] = "marginStart";
const char DOM_MARGIN_END[] = "marginEnd";
const char DOM_LAYOUT_IN_BOX[] = "layoutInBox";
const char DOM_BACKGROUND_COLOR[] = "backgroundColor";
const char DOM_BACKGROUND_IMAGE[] = "backgroundImage";
const char DOM_FADE_COLOR[] = "fadeColor";
const char DOM_BOX_SHADOW_H[] = "boxShadowH";
const char DOM_BOX_SHADOW_V[] = "boxShadowV";
const char DOM_BOX_SHADOW_BLUR[] = "boxShadowBlur";
const char DOM_BOX_SHADOW_SPREAD[] = "boxShadowSpread";
const char DOM_BOX_SHADOW_COLOR[] = "boxShadowColor";
const char DOM_FLEX_DIRECTION[] = "flexDirection";
const char DOM_FLEX_ROW[] = "row";
const char DOM_FLEX_COLUMN[] = "column";
const char DOM_FLEX[] = "flex";
const char DOM_FLEX_GROW[] = "flexGrow";
const char DOM_FLEX_SHRINK[] = "flexShrink";
const char DOM_FLEX_BASIS[] = "flexBasis";
const char DOM_FLEX_WEIGHT[] = "flexWeight";
const char DOM_ALIGN_SELF[] = "alignSelf";
const char DOM_MIN_WIDTH[] = "minWidth";
const char DOM_MIN_HEIGHT[] = "minHeight";
const char DOM_MAX_WIDTH[] = "maxWidth";
const char DOM_MAX_HEIGHT[] = "maxHeight";
const char DOM_DISPLAY_INDEX[] = "displayIndex";
const char DOM_ASPECT_RATIO[] = "aspectRatio";
const char DOM_OPACITY[] = "opacity";
const char DOM_APPEARING_DURATION[] = "appearingDuration";
const char DOM_DISPLAY[] = "display";
const char DOM_VISIBILITY[] = "visibility";
const char DOM_DISPLAY_GRID[] = "grid";
const char DOM_DISPLAY_FLEX[] = "flex";
const char DOM_DISPLAY_NONE[] = "none";
const char DOM_VISIBILITY_VISIBLE[] = "visible";
const char DOM_VISIBILITY_HIDDEN[] = "hidden";
const char DOM_BORDER[] = "border";
const char DOM_BORDER_WIDTH[] = "borderWidth";
const char DOM_BORDER_LEFT_WIDTH[] = "borderLeftWidth";
const char DOM_BORDER_TOP_WIDTH[] = "borderTopWidth";
const char DOM_BORDER_RIGHT_WIDTH[] = "borderRightWidth";
const char DOM_BORDER_BOTTOM_WIDTH[] = "borderBottomWidth";
const char DOM_BORDER_COLOR[] = "borderColor";
const char DOM_BORDER_LEFT_COLOR[] = "borderLeftColor";
const char DOM_BORDER_TOP_COLOR[] = "borderTopColor";
const char DOM_BORDER_RIGHT_COLOR[] = "borderRightColor";
const char DOM_BORDER_BOTTOM_COLOR[] = "borderBottomColor";
const char DOM_BORDER_STYLE[] = "borderStyle";
const char DOM_BORDER_LEFT_STYLE[] = "borderLeftStyle";
const char DOM_BORDER_TOP_STYLE[] = "borderTopStyle";
const char DOM_BORDER_RIGHT_STYLE[] = "borderRightStyle";
const char DOM_BORDER_BOTTOM_STYLE[] = "borderBottomStyle";
const char DOM_BORDER_RADIUS[] = "borderRadius";
const char DOM_BORDER_TOP_LEFT_RADIUS[] = "borderTopLeftRadius";
const char DOM_BORDER_TOP_RIGHT_RADIUS[] = "borderTopRightRadius";
const char DOM_BORDER_BOTTOM_RIGHT_RADIUS[] = "borderBottomRightRadius";
const char DOM_BORDER_BOTTOM_LEFT_RADIUS[] = "borderBottomLeftRadius";
const char DOM_BORDER_IMAGE[] = "borderImage";
const char DOM_BORDER_IMAGE_SOURCE[] = "borderImageSource";
const char DOM_BORDER_IMAGE_WIDTH[] = "borderImageWidth";
const char DOM_BORDER_IMAGE_SLICE[] = "borderImageSlice";
const char DOM_BORDER_IMAGE_OUTSET[] = "borderImageOutset";
const char DOM_BORDER_IMAGE_REPEAT[] = "borderImageRepeat";
const char DOM_POSITION[] = "position";
const char DOM_POSITION_RELATIVE[] = "relative";
const char DOM_POSITION_FIXED[] = "fixed";
const char DOM_POSITION_ABSOLUTE[] = "absolute";
const char DOM_POSITION_TOP[] = "top";
const char DOM_POSITION_RIGHT[] = "right";
const char DOM_POSITION_BOTTOM[] = "bottom";
const char DOM_POSITION_LEFT[] = "left";
const char DOM_POSITION_START[] = "start";
const char DOM_POSITION_END[] = "end";
const char DOM_BACKGROUND_IMAGE_SIZE[] = "backgroundSize";
const char DOM_BACKGROUND_IMAGE_SIZE_CONTAIN[] = "contain";
const char DOM_BACKGROUND_IMAGE_SIZE_COVER[] = "cover";
const char DOM_BACKGROUND_IMAGE_SIZE_AUTO[] = "auto";
const char DOM_BACKGROUND_IMAGE_POSITION[] = "backgroundPosition";
const char DOM_BACKGROUND_IMAGE_POSITION_LEFT[] = "left";
const char DOM_BACKGROUND_IMAGE_POSITION_CENTER[] = "center";
const char DOM_BACKGROUND_IMAGE_POSITION_RIGHT[] = "right";
const char DOM_BACKGROUND_IMAGE_POSITION_TOP[] = "top";
const char DOM_BACKGROUND_IMAGE_POSITION_BOTTOM[] = "bottom";
const char DOM_BACKGROUND_IMAGE_REPEAT[] = "backgroundRepeat";
const char DOM_BACKGROUND[] = "background";
const char DOM_GRADIENT_DIRECTION_TOP[] = "top";
const char DOM_GRADIENT_DIRECTION_RIGHT[] = "right";
const char DOM_GRADIENT_DIRECTION_BOTTOM[] = "bottom";
const char DOM_GRADIENT_DIRECTION_LEFT[] = "left";
const char DOM_GRADIENT_SIZE_CLOSEST_CORNER[] = "closest-corner";
const char DOM_GRADIENT_SIZE_CLOSEST_SIDE[] = "closest-side";
const char DOM_GRADIENT_SIZE_FARTHEST_CORNER[] = "farthest-corner";
const char DOM_GRADIENT_SIZE_FARTHEST_SIDE[] = "farthest-side";
const char DOM_GRADIENT_SHAPE_CIRCLE[] = "circle";
const char DOM_GRADIENT_SHAPE_ELLIPSE[] = "ellipse";
const char DOM_VALUES[] = "values";
const char DOM_GRADIENT_VALUES[] = "values";
const char DOM_GRADIENT_TYPE[] = "type";
const char DOM_GRADIENT_DIRECTIONS[] = "directions";
const char DOM_GRADIENT_SIZE[] = "size";
const char DOM_GRADIENT_POSITION[] = "position";
const char DOM_GRADIENT_SHAPE[] = "shape";
const char DOM_GRADIENT_ANGLE[] = "angle";
const char DOM_GRADIENT_ROTATION[] = "rotation";
const char DOM_LINEAR_GRADIENT[] = "linearGradient";
const char DOM_REPEATING_LINEAR_GRADIENT[] = "repeatingLinearGradient";
const char DOM_RADIAL_GRADIENT[] = "radialGradient";
const char DOM_REPEATING_RADIAL_GRADIENT[] = "repeatingRadialGradient";
const char DOM_SWEEP_GRADIENT[] = "sweepGradient";
const char DOM_REPEATING_SWEEP_GRADIENT[] = "repeatingSweepGradient";
const char DOM_TRANSFORM[] = "transform";
const char DOM_SCALE[] = "scale";
const char DOM_SCALE_X[] = "scaleX";
const char DOM_SCALE_Y[] = "scaleY";
const char DOM_SCALE_Z[] = "scaleZ";
const char DOM_SCALE_3D[] = "scale3d";
const char DOM_TRANSLATE[] = "translate";
const char DOM_TRANSLATE_X[] = "translateX";
const char DOM_TRANSLATE_Y[] = "translateY";
const char DOM_TRANSLATE_Z[] = "translateZ";
const char DOM_TRANSLATE_3D[] = "translate3d";
const char DOM_ROTATE[] = "rotate";
const char DOM_ROTATE_X[] = "rotateX";
const char DOM_ROTATE_Y[] = "rotateY";
const char DOM_ROTATE_Z[] = "rotateZ";
const char DOM_ROTATE_3D[] = "rotate3d";
const char DOM_SKEW[] = "skew";
const char DOM_SKEW_X[] = "skewX";
const char DOM_SKEW_Y[] = "skewY";
const char DOM_PERSPECTIVE[] = "perspective";
const char DOM_MATRIX[] = "matrix";
const char DOM_MATRIX_3D[] = "matrix3d";
const char DOM_FOCUSABLE[] = "focusable";
const char DOM_ANIMATION_WIDTH[] = "width";
const char DOM_ANIMATION_HEIGHT[] = "height";
const char DOM_ANIMATION_COLOR[] = "backgroundColor";
const char DOM_ANIMATION_OPACITY[] = "opacity";
const char DOM_ANIMATION_ITERATIONS[] = "iterations";
const char DOM_ANIMATION_DELAY_API[] = "delay";
const char DOM_ANIMATION_DURATION_API[] = "duration";
const char DOM_TRANSFORM_ORIGIN[] = "transformOrigin";
const char DOM_ANIMATION_NAME[] = "animationName";
const char DOM_ANIMATION_DELAY[] = "animationDelay";
const char DOM_ANIMATION_DURATION[] = "animationDuration";
const char DOM_ANIMATION_FILL[] = "fill";
const char DOM_ANIMATION_EASING[] = "easing";
const char DOM_ANIMATION_BEGIN[] = "begin";
const char DOM_ANIMATION_END[] = "end";
const char DOM_ANIMATION_ITERATION_COUNT[] = "animationIterationCount";
const char DOM_ANIMATION_TIMING_FUNCTION[] = "animationTimingFunction";
const char DOM_ANIMATION_FILL_MODE[] = "animationFillMode";
const char DOM_ANIMATION_DIRECTION[] = "animationDirection";
const char DOM_ANIMATION_DIRECTION_API[] = "direction";
const char DOM_ANIMATION_DIRECTION_NORMAL[] = "normal";
const char DOM_ANIMATION_DIRECTION_ALTERNATE[] = "alternate";
const char DOM_ANIMATION_DIRECTION_REVERSE[] = "reverse";
const char DOM_ANIMATION_DIRECTION_ALTERNATE_REVERSE[] = "alternate-reverse";
const char DOM_ANIMATION_PLAY_STATE[] = "animationPlayState";
const char DOM_ANIMATION_PLAY_STATE_RUNNING[] = "running";
const char DOM_ANIMATION_PLAY_STATE_PAUSED[] = "paused";
const char DOM_ANIMATION_PLAY_STATE_IDLE[] = "idle";
const char DOM_ANIMATION_PLAY_STATE_FINISHED[] = "finished";
const char DOM_ANIMATION_TIMING_FUNCTION_EASE[] = "ease";
const char DOM_ANIMATION_TIMING_FUNCTION_EASE_IN[] = "ease-in";
const char DOM_ANIMATION_TIMING_FUNCTION_EASE_OUT[] = "ease-out";
const char DOM_ANIMATION_TIMING_FUNCTION_EASE_IN_OUT[] = "ease-in-out";
const char DOM_ANIMATION_TIMING_FUNCTION_LINEAR[] = "linear";
const char DOM_ANIMATION_TIMING_FUNCTION_FAST_OUT_SLOW_IN[] = "fast-out-slow-in";
const char DOM_ANIMATION_TIMING_FUNCTION_LINEAR_OUT_SLOW_IN[] = "linear-out-slow-in";
const char DOM_ANIMATION_TIMING_FUNCTION_FAST_OUT_LINEAR_IN[] = "fast-out-linear-in";
const char DOM_ANIMATION_TIMING_FUNCTION_FRICTION[] = "friction";
const char DOM_ANIMATION_TIMING_FUNCTION_EXTREME_DECELERATION[] = "extreme-deceleration";
const char DOM_ANIMATION_TIMING_FUNCTION_SHARP[] = "sharp";
const char DOM_ANIMATION_TIMING_FUNCTION_RHYTHM[] = "rhythm";
const char DOM_ANIMATION_TIMING_FUNCTION_SMOOTH[] = "smooth";
const char DOM_ANIMATION_TIMING_FUNCTION_STEPS[] = "steps";
const char DOM_ANIMATION_TIMING_FUNCTION_CUBIC_BEZIER[] = "cubic-bezier";
const char DOM_ANIMATION_TIMING_FUNCTION_SPRING[] = "spring";
const char DOM_ANIMATION_FILL_MODE_NONE[] = "none";
const char DOM_ANIMATION_FILL_MODE_FORWARDS[] = "forwards";
const char DOM_ANIMATION_FILL_MODE_BACKWARDS[] = "backwards";
const char DOM_ANIMATION_FILL_MODE_BOTH[] = "both";
const char DOM_ANIMATION_NAME_TIME[] = "time";
const char DOM_ANIMATION_OFFSET[] = "offset";
const char DOM_TRANSFORM_ORIGIN_RIGHT_TOP[] = "right top";
const char DOM_TRANSFORM_ORIGIN_RIGHT_CENTER[] = "right center";
const char DOM_TRANSFORM_ORIGIN_RIGHT_BOTTOM[] = "right bottom";
const char DOM_TRANSFORM_ORIGIN_CENTER_TOP[] = "center top";
const char DOM_TRANSFORM_ORIGIN_CENTER_CENTER[] = "center center";
const char DOM_TRANSFORM_ORIGIN_CENTER_BOTTOM[] = "center bottom";
const char DOM_TRANSFORM_ORIGIN_LEFT_TOP[] = "left top";
const char DOM_TRANSFORM_ORIGIN_LEFT_CENTER[] = "left center";
const char DOM_TRANSFORM_ORIGIN_LEFT_BOTTOM[] = "left bottom";
const char DOM_FILTER[] = "filter";
const char DOM_BACKDROP_FILTER[] = "backdropFilter";
const char DOM_WINDOW_FILTER[] = "windowFilter";
const char DOM_CARET_COLOR[] = "caretColor";
const char DOM_CLIP_PATH[] = "clipPath";
const char DOM_MASK_IMAGE[] = "maskImage";
const char DOM_MASK_POSITION[] = "maskPosition";
const char DOM_MASK_SIZE[] = "maskSize";
const char DOM_IMAGE_FILL[] = "imageFill";
const char DOM_BOX_SIZING[] = "boxSizing";
const char DOM_AUTO[] = "auto";

// transition style
const char DOM_TRANSITION_NAME[] = "transition";
const char DOM_TRANSITION_PROPERTY[] = "transitionProperty";
const char DOM_TRANSITION_PROPERTY_DURATION[] = "transitionPropertyDuration";
const char DOM_TRANSITION_PROPERTY_TIMING_FUNCTION[] = "transitionPropertyTimingFunction";
const char DOM_TRANSITION_PROPERTY_DELAY[] = "transitionPropertyDelay";
const char DOM_TRANSITION_BACKGROUND_COLOR[] = "background-color";
const char DOM_TRANSITION_ENTER[] = "transitionEnter";
const char DOM_TRANSITION_EXIT[] = "transitionExit";
const char DOM_TRANSITION_DURATION[] = "transitionDuration";
const char DOM_TRANSITION_TIMING_FUNCTION[] = "transitionTimingFunction";

// shared transition style
const char DOM_SHARED_TRANSITION_EFFECT[] = "sharedTransitionEffect";
const char DOM_SHARED_TRANSITION_NAME[] = "sharedTransitionName";
const char DOM_SHARED_TRANSITION_TIMING_FUNCTION[] = "sharedTransitionTimingFunction";

// display: grid style constants
const char DOM_GRID_TEMPLATE_COLUMNS[] = "gridTemplateColumns";
const char DOM_GRID_TEMPLATE_ROWS[] = "gridTemplateRows";
const char DOM_GRID_COLUMN_GAP[] = "gridColumnsGap";
const char DOM_GRID_ROW_GAP[] = "gridRowsGap";
const char DOM_GRID_COLUMN_START[] = "gridColumnStart";
const char DOM_GRID_COLUMN_END[] = "gridColumnEnd";
const char DOM_GRID_ROW_START[] = "gridRowStart";
const char DOM_GRID_ROW_END[] = "gridRowEnd";
const char DOM_GRID_AUTO_FLOW[] = "gridAutoFlow";

// div node style constants
const char DOM_ALIGN_CONTENT[] = "alignContent";
const char DOM_ALIGN_CONTENT_STRETCH[] = "stretch";
const char DOM_ALIGN_CONTENT_START[] = "flex-start";
const char DOM_ALIGN_CONTENT_CENTER[] = "center";
const char DOM_ALIGN_CONTENT_END[] = "flex-end";
const char DOM_ALIGN_CONTENT_BETWEEN[] = "space-between";
const char DOM_ALIGN_CONTENT_AROUND[] = "space-around";
const char DOM_ALIGN_ITEMS[] = "alignItems";
const char DOM_ALIGN_ITEMS_START[] = "flex-start";
const char DOM_ALIGN_ITEMS_CENTER[] = "center";
const char DOM_ALIGN_ITEMS_END[] = "flex-end";
const char DOM_ALIGN_ITEMS_STRETCH[] = "stretch";
const char DOM_ALIGN_ITEMS_BASELINE[] = "baseline";
const char DOM_JUSTIFY_CONTENT[] = "justifyContent";
const char DOM_JUSTIFY_CONTENT_START[] = "flex-start";
const char DOM_JUSTIFY_CONTENT_CENTER[] = "center";
const char DOM_JUSTIFY_CONTENT_END[] = "flex-end";
const char DOM_JUSTIFY_CONTENT_BETWEEN[] = "space-between";
const char DOM_JUSTIFY_CONTENT_AROUND[] = "space-around";
const char DOM_JUSTIFY_CONTENT_EVENLY[] = "space-evenly";
const char DOM_FLEX_WRAP[] = "flexWrap";
const char DOM_WRAP[] = "wrap";
const char DOM_DIV_CARD_TYPE[] = "card";
const char DOM_DIV_CARD_BLUR[] = "blureffect";
const char DOM_DIV_EVENT_REACH_TOP[] = "reachtop";
const char DOM_DIV_EVENT_REACH_BOTTOM[] = "reachbottom";
const char DOM_DIV_EVENT_REACH_START[] = "reachstart";
const char DOM_DIV_EVENT_REACH_END[] = "reachend";

// image attribute and style constants
const char DOM_SRC[] = "src";
const char DOM_IMAGE_FIT[] = "objectFit";
const char DOM_IMAGE_ALT[] = "alt";
const char DOM_IMAGE_MATCH_TEXT_DIRECTION[] = "matchTextDirection";
const char DOM_IMAGE_FIT_ORIGINAL_SIZE[] = "fitOriginalSize";
const char DOM_IMAGE_FILL_COLOR[] = "fill";
const char DOM_IMAGE_SYNC_LOAD[] = "syncload";
// objectPosition
const char DOM_IMAGE_POSITION[] = "objectPosition";
const char DOM_IMAGE_POSITION_LEFT[] = "left";
const char DOM_IMAGE_POSITION_CENTER[] = "center";
const char DOM_IMAGE_POSITION_RIGHT[] = "right";
const char DOM_IMAGE_POSITION_TOP[] = "top";
const char DOM_IMAGE_POSITION_BOTTOM[] = "bottom";

// image animator attribute and style constants
const char DOM_IMAGES[] = "images";
const char DOM_ITERATION[] = "iteration";
const char DOM_REVERSE[] = "reverse";
const char DOM_FIXEDSIZE[] = "fixedsize";
const char DOM_FILLMODE[] = "fillmode";
const char DOM_PREDECODE[] = "predecode";
const char DOM_IMAGE_ANIMATOR_START[] = "start";
const char DOM_IMAGE_ANIMATOR_STOP[] = "stop";
const char DOM_IMAGE_ANIMATOR_PAUSE[] = "pause";
const char DOM_IMAGE_ANIMATOR_RESUME[] = "resume";

// list attribute、style and event constants
const char DOM_LIST_COLUMNS[] = "columns";
const char DOM_LIST_ITEM_EXTENT[] = "itemExtent";
const char DOM_LIST_EVENT_SCROLL[] = "scroll";
const char DOM_LIST_EVENT_SCROLL_BOTTOM[] = "scrollbottom";
const char DOM_LIST_EVENT_SCROLL_END[] = "scrollend";
const char DOM_LIST_EVENT_SCROLL_TOP[] = "scrolltop";
const char DOM_LIST_EVENT_SCROLL_TOUCH_UP[] = "scrolltouchup";
const char DOM_LIST_EVENT_INDEXER_CHANGE[] = "indexerchange";
const char DOM_LIST_ITEM_EVENT_STICKY[] = "sticky";
const char DOM_LIST_METHOD_EXPAND_GROUP[] = "expandGroup";
const char DOM_LIST_METHOD_COLLAPSE_GROUP[] = "collapseGroup";
const char DOM_LIST_METHOD_SCROLL_BY[] = "scrollBy";
const char DOM_LIST_METHOD_SCROLL_TO[] = "scrollTo";
const char DOM_LIST_METHOD_SCROLL_ARROW[] = "scrollArrow";
const char DOM_LIST_METHOD_SCROLL_TOP[] = "scrollTop";
const char DOM_LIST_METHOD_SCROLL_BOTTOM[] = "scrollBottom";
const char DOM_LIST_METHOD_SCROLL_PAGE[] = "scrollPage";
const char DOM_LIST_SCROLLPAGE[] = "scrollpage";
const char DOM_LIST_CHAIN_ANIMATION[] = "chainanimation";
const char DOM_LIST_INITIAL_INDEX[] = "initialindex";
const char DOM_LIST_INITIAL_OFFSET[] = "initialoffset";
const char DOM_LIST_UPDATE_EFFECT[] = "updateeffect";
const char DOM_SCROLL_EFFECT[] = "scrolleffect";
const char DOM_SCROLL_EFFECT_SPRING[] = "spring";
const char DOM_SCROLL_EFFECT_FADE[] = "fade";
const char DOM_SCROLL_EFFECT_NONE[] = "no";
const char DOM_LIST_INDEXER[] = "indexer";
const char DOM_LIST_INDEXER_MODE[] = "indexercircle";
const char DOM_LIST_INDEXER_BUBBLE[] = "indexerbubble";
const char DOM_LIST_INDEXER_MULTI[] = "indexermulti";
const char DOM_LIST_ITEM_SCALE[] = "itemscale";
const char DOM_LIST_ITEM_OPACITY[] = "itemopacity";
const char DOM_LIST_ITEM_CENTER[] = "itemcenter";
const char DOM_LIST_CENTER_LAYOUT[] = "centerlayout";
const char DOM_LIST_SCROLL_VIBRATE[] = "scrollvibrate";
const char DOM_LIST_ROTATION_VIBRATE[] = "rotationvibrate";
const char DOM_LIST_DIVIDER[] = "divider";
const char DOM_LIST_DIVIDER_COLOR[] = "dividerColor";
const char DOM_LIST_DIVIDER_HEIGHT[] = "dividerHeight";
const char DOM_LIST_DIVIDER_LENGTH[] = "dividerLength";
const char DOM_LIST_DIVIDER_ORIGIN[] = "dividerOrigin";
const char DOM_LIST_ATTR_SELECTED[] = "selected";
const char DOM_LIST_ACCESSIBILITY_DISABLED[] = "accessibilitydisabled";
const char DOM_SCROLL_SCROLLBAR[] = "scrollbar";
const char DOM_SCROLL_SCROLLBAR_OFF[] = "off";
const char DOM_SCROLL_SCROLLBAR_ON[] = "on";
const char DOM_SCROLL_SCROLLBAR_AUTO[] = "auto";
const char DOM_SCROLL_SHAPE_MODE[] = "shapemode";
const char DOM_SCROLL_SHAPE_MODE_RECT[] = "rect";
const char DOM_SCROLL_SHAPE_MODE_ROUND[] = "round";
const char DOM_SCROLL_SCROLLBAR_COLOR[] = "scrollbarColor";
const char DOM_SCROLL_SCROLLBAR_WIDTH[] = "scrollbarWidth";
const char DOM_SCROLL_OVER_SCROLL_EFFECT[] = "overscrollEffect";
const char DOM_SCROLL_SCROLLBAR_POSITION[] = "scrollbaroffset";
const char DOM_SCROLL_SCROLLBAR_OFFSET[] = "scrollbarOffset";

// list-items attribute and style constants
const char DOM_LISTITEM_COLUMN_SPAN[] = "columnSpan";
const char DOM_LISTITEM_TYPE[] = "type";
const char DOM_LISTITEM_STICKY[] = "sticky";
const char DOM_LISTITEM_STICKY_RADIUS[] = "stickyradius";
const char DOM_LISTITEM_CLICK_EFFECT[] = "clickeffect";
const char DOM_LISTITEM_IS_TITLE[] = "title";
const char DOM_LISTITEM_INDEX_KEY[] = "section";
const char DOM_LISTITEM_PRIMARY[] = "primary";
const char DOM_LISTITEM_CARD_TYPE[] = "card";
const char DOM_LISTITEM_CARD_BLUR[] = "blureffect";
const char DOM_LISTITEM_ACTIVE[] = "active";
const char DOM_LISTITEM_CLICK_COLOR[] = "clickColor";
const char DOM_LISTITEM_KEY[] = "key";

// list-items-group attribute and style constants
const char DOM_LIST_ITEM_GROUP_EVENT_GROUPCLICK[] = "groupclick";
const char DOM_LIST_ITEM_GROUP_EVENT_GROUPCOLLAPSE[] = "groupcollapse";
const char DOM_LIST_ITEM_GROUP_EVENT_GROUPEXPAND[] = "groupexpand";

// option attribute style and event constants
const char DOM_OPTION_SELECTED[] = "selected";
const char DOM_OPTION_VALUE[] = "value";
const char DOM_OPTION_ICON[] = "icon";
const char DOM_OPTION_ACTION[] = "action";
const char DOM_OPTION_CONTENT[] = "content";
const char DOM_OPTION_TEXT_DECORATION[] = "textDecoration";

// text attribute and style constants
const char DOM_TEXT_VALUE[] = "value";
const char DOM_TEXT_COLOR[] = "color";
const char DOM_TEXT_LINES[] = "lines";
const char DOM_TEXT_MAX_LINES[] = "maxLines";
const char DOM_TEXT_FONT_SIZE[] = "fontSize";
const char DOM_TEXT_MIN_FONT_SIZE[] = "minFontSize";
const char DOM_TEXT_MAX_FONT_SIZE[] = "maxFontSize";
const char DOM_TEXT_FONT_SIZE_STEP[] = "fontSizeStep";
const char DOM_TEXT_PREFER_FONT_SIZES[] = "preferFontSizes";
const char DOM_TEXT_FONT_WEIGHT[] = "fontWeight";
const char DOM_TEXT_DECORATION[] = "textDecoration";
const char DOM_TEXT_DECORATION_COLOR[] = "textDecorationColor";
const char DOM_TEXT_FONT_STYLE[] = "fontStyle";
const char DOM_TEXT_ALLOW_SCALE[] = "allowScale";
const char DOM_TEXT_ALIGN[] = "textAlign";
const char DOM_TEXT_CASE[] = "textCase";
const char DOM_TEXT_BASELINE_OFFSET[] = "baselineOffset";
const char DOM_TEXT_OVERFLOW[] = "textOverflow";
const char DOM_OVERFLOW_STYLE[] = "overflow";
const char DOM_TEXT_LINE_HEIGHT[] = "lineHeight";
const char DOM_TEXT_LETTER_SPACING[] = "letterSpacing";
const char DOM_TEXT_FONT_FAMILY[] = "fontFamily";
const char DOM_ELLIPSIS[] = "ellipsis";
const char DOM_CLIP[] = "clip";
const char DOM_LEFT[] = "left";
const char DOM_RIGHT[] = "right";
const char DOM_CENTER[] = "center";
const char DOM_TEXT_DECORATION_UNDERLINE[] = "underline";
const char DOM_TEXT_DECORATION_OVERLINE[] = "overline";
const char DOM_TEXT_DECORATION_LINETHROUGH[] = "line-through";
const char DOM_TEXT_DECORATION_INHERIT[] = "inherit";
const char DOM_TEXT_DECORATION_NONE[] = "none";
const char DOM_TEXT_FONT_STYLE_ITALIC[] = "italic";
const char DOM_TEXT_FONT_STYLE_NORMAL[] = "normal";
const char DOM_TEXT_WORD_BREAK[] = "wordBreak";
const char DOM_TEXT_FONT_VARIANT[] = "fontVariant";
const char DOM_TEXT_FONT_FEATURE_SETTINGS[] = "fontFeatureSettings";
const char DOM_TEXT_ADAPT_HEIGHT[] = "adaptHeight";
const char DOM_TEXT_VERTICAL_ALIGN[] = "textValign";
const char DOM_TOP[] = "top";
const char DOM_MIDDLE[] = "middle";
const char DOM_BOTTOM[] = "bottom";
const char DOM_TEXT_INDENT[] = "textIndent";
const char DOM_TEXT_WORD_SPACING[] = "wordSpacing";
const char DOM_TEXT_WHITE_SPACE[] = "whiteSpace";
const char DOM_WHITE_SPACE_NORMAL[] = "normal";
const char DOM_WHITE_SPACE_PRELINE[] = "pre-line";
const char DOM_WHITE_SPACE_PREWRAP[] = "pre-wrap";
const char DOM_WHITE_SPACE_NOWRAP[] = "nowrap";
const char DOM_WHITE_SPACE_PRE[] = "pre";
const char DOM_WHITE_SPACE_INHERIT[] = "inherit";

// popup attribute and style constants
const char DOM_TARGET[] = "target";
const char DOM_PLACEMENT[] = "placement";
const char DOM_MASK_COLOR[] = "maskColor";
const char DOM_ARROW_OFFSET[] = "arrowoffset";
const char DOM_KEEP_ALIVE[] = "keepalive";
const char DOM_CLICKABLE[] = "clickable";
const char DOM_VISIBILITY_CHANGE[] = "visibilitychange";

// progress attribute and style constants
const char DOM_PROGRESS_COLOR[] = "color";
const char DOM_PROGRESS_DIAMETER[] = "diameter";
const char DOM_PROGRESS_BUBBLE_RADIUS[] = "bubbleRadius";
const char DOM_PROGRESS_SECONDARY_COLOR[] = "secondaryColor";
const char DOM_PROGRESS_BACKGROUND_COLOR[] = "backgroundColor";
const char DOM_PROGRESS_STROKE_WIDTH[] = "strokeWidth";
const char DOM_PROGRESS_TYPE[] = "type";
const char DOM_PROGRESS_TYPE_HORIZONTAL[] = "horizontal";
const char DOM_PROGRESS_TYPE_CIRCULAR[] = "circular";
const char DOM_PROGRESS_TYPE_BUBBLE[] = "bubble";
const char DOM_PROGRESS_TYPE_RING[] = "ring";
const char DOM_PROGRESS_TYPE_SCALE[] = "scale-ring";
const char DOM_PROGRESS_TYPE_MOON[] = "eclipse";
const char DOM_PROGRESS_TYPE_ARC[] = "arc";
const char DOM_PROGRESS_PERCENT[] = "percent";
const char DOM_PROGRESS_SECONDARY_PERCENT[] = "secondarypercent";
const char DOM_CIRCULAR_WIDTH[] = "width";
const char DOM_CIRCULAR_HEIGHT[] = "height";
const char DOM_CENTER_X[] = "centerX";
const char DOM_CENTER_Y[] = "centerY";
const char DOM_PROGRESS_RADIUS[] = "radius";
const char DOM_START_DEGREE[] = "startAngle";
const char DOM_SWEEP_DEGREE[] = "totalAngle";
const char DOM_RING_CLOCKWISH_DIRECTION[] = "clockwise";
const char DOM_SCALE_WIDTH[] = "scaleWidth";
const char DOM_SCALE_NUMBER[] = "scaleNumber";
const char DOM_DIRECTION[] = "direction";
const char DOM_START_TO_END[] = "startToEnd";

// chart attribute and style constants
const char DOM_CHART_TYPE[] = "type";
const char DOM_CHART_TYPE_BAR[] = "bar";
const char DOM_CHART_TYPE_LINE[] = "line";
const char DOM_CHART_TYPE_GAUGE[] = "gauge";
const char DOM_CHART_TYPE_LOADING[] = "loading";
const char DOM_CHART_TYPE_PROGRESS[] = "progress";
const char DOM_CHART_TYPE_RAINBOW[] = "rainbow";
const char DOM_COLORS_ARRAY[] = "colors";
const char DOM_WEIGHTS_ARRAY[] = "weights";
const char DOM_EFFECTS_ON[] = "effects";
const char DOM_AUTO_SCALE[] = "autoscale";
const char DOM_CHART_ANIMATION_DURATION[] = "animationduration";

// slider attribute and style constants
const char DOM_MIN[] = "min";
const char DOM_MAX[] = "max";
const char DOM_STEP[] = "step";
const char DOM_COLOR[] = "color";
const char DOM_SELECTED_COLOR[] = "selectedColor";
const char DOM_BLOCK_COLOR[] = "blockColor";
const char DOM_MIN_ICON[] = "minicon";
const char DOM_MAX_ICON[] = "maxicon";
const char DOM_CONTINUOUS[] = "continuous";
const char DOM_INTERMITTENT[] = "intermittent";
const char DOM_SLIDER_MODE[] = "mode";
const char DOM_SHOW_TIPS[] = "showtips";
const char DOM_SHOW_STEPS[] = "showsteps";
const char DOM_OUTSET[] = "outset";
const char DOM_INSET[] = "inset";

// stepper attribute style and event constants
const char DOM_STEPPER_INDEX[] = "index";
const char DOM_STEPPER_LABEL[] = "label";
const char DOM_STEPPER_LEFT_LABEL[] = "prevLabel";
const char DOM_STEPPER_RIGHT_LABEL[] = "nextLabel";
const char DOM_STEPPER_INITIAL_STATUS[] = "status";
const char DOM_STEPPER_TEXT_COLOR[] = "color";
const char DOM_STEPPER_FONT_SIZE[] = "fontSize";
const char DOM_STEPPER_FONT_STYLE[] = "fontStyle";
const char DOM_STEPPER_FONT_WEIGHT[] = "fontWeight";
const char DOM_STEPPER_FONT_FAMILY[] = "fontFamily";
const char DOM_STEPPER_TEXT_DECORATION[] = "textDecoration";
const char DOM_STEPPER_EVENT_FINISH[] = "finish";
const char DOM_STEPPER_EVENT_SKIP[] = "skip";
const char DOM_STEPPER_EVENT_CHANGE[] = "change";
const char DOM_STEPPER_EVENT_NEXT[] = "next";
const char DOM_STEPPER_EVENT_BACK[] = "back";
const char DOM_STEPPER_SET_RIGHT_BUTTON_STATUS[] = "setNextButtonStatus";

// stepper-item attribute style and event constants
const char DOM_STEPPER_ITEM_EVENT_APPEAR[] = "appear";
const char DOM_STEPPER_ITEM_EVENT_DISAPPEAR[] = "disappear";

// swiper attribute and style constants
const char DOM_INDEX[] = "index";
const char DOM_SWIPER_ANIMATION_OPACITY[] = "animationopacity";
const char DOM_AUTOPLAY[] = "autoplay";
const char DOM_CACHED_SIZE[] = "cachedSize";
const char DOM_DISPLAY_MODE[] = "displaymode";
const char DOM_DISPLAY_MODE_AUTO_LINEAR[] = "autoLinear";
const char DOM_INTERVAL[] = "interval";
const char DOM_INDICATOR[] = "indicator";
const char DOM_LOOP[] = "loop";
const char DOM_DURATION[] = "duration";
const char DOM_VERTICAL[] = "vertical";
const char DOM_ANIMATION_CURVE[] = "animationCurve";
const char DOM_INDICATOR_COLOR[] = "indicatorColor";
const char DOM_INDICATOR_MASK[] = "indicatormask";
const char DOM_INDICATOR_DISABLED[] = "indicatordisabled";
const char DOM_INDICATOR_SELECTEDCOLOR[] = "indicatorSelectedColor";
const char DOM_INDICATOR_SIZE[] = "indicatorSize";
const char DOM_INDICATOR_TOP[] = "indicatorTop";
const char DOM_INDICATOR_LEFT[] = "indicatorLeft";
const char DOM_INDICATOR_RIGHT[] = "indicatorRight";
const char DOM_INDICATOR_BOTTOM[] = "indicatorBottom";
const char DOM_METHOD_SWIPE_TO[] = "swipeTo";
const char DOM_METHOD_APPEND[] = "append";
const char DOM_METHOD_SHOW_PREVIOUS[] = "showPrevious";
const char DOM_METHOD_SHOW_NEXT[] = "showNext";
const char DOM_DIGITAL_INDICATOR[] = "digital";
const char DOM_PREVIOUS_MARGIN[] = "previousMargin";
const char DOM_NEXT_MARGIN[] = "nextMargin";

// switch attribute and style constants
const char DOM_CHECKED[] = "checked";
const char DOM_SHOW_TEXT[] = "showtext";
const char DOM_TEXT_ON[] = "texton";
const char DOM_TEXT_OFF[] = "textoff";
const char DOM_TEXT_PADDING[] = "textPadding";
const char DOM_TEXT_ON_COLOR[] = "textonColor";
const char DOM_TEXT_OFF_COLOR[] = "textoffColor";
const char DOM_TRACK_COLOR_ON[] = "trackColorOn";
const char DOM_TRACK_COLOR_OFF[] = "trackColorOff";

// tabs attribute and style constants
const char DOM_TAB_BAR_MODE[] = "mode";
const char DOM_TAB_CONTENT_SCROLLABEL[] = "scrollable";

// refresh attribute and style constants
const char DOM_REFRESH[] = "refresh";
const char DOM_REFRESH_OFFSET[] = "offset";
const char DOM_REFRESH_REFRESHING[] = "refreshing";
const char DOM_REFRESH_TYPE[] = "type";
const char DOM_REFRESH_LASTTIME[] = "lasttime";
const char DOM_REFRESH_FRICTION[] = "friction";
const char DOM_REFRESH_BACKGROUND_COLOR[] = "backgroundColor";
const char DOM_REFRESH_PROGRESS_COLOR[] = "progressColor";
const char DOM_REFRESH_EVENT_PULL_DOWN[] = "pulldown";
const char DOM_REFRESH_TIME_OFFSET[] = "timeoffset";

// rating attribute and style constants
const char DOM_STAR_NUM[] = "numstars";
const char DOM_RATING_SCORE[] = "rating";
const char DOM_RATING_STEP[] = "stepsize";
const char DOM_RATING_INDICATOR[] = "indicator";
const char DOM_BACKGROUND_SRC[] = "starBackground";
const char DOM_FOREGROUND_SRC[] = "starForeground";
const char DOM_SECONDARY_SRC[] = "starSecondary";
const char DOM_RTL_FLIP[] = "rtlFlip";

// marquee attribute and style constants
const char DOM_MARQUEE_LOOP[] = "loop";
const char DOM_MARQUEE_SCROLL_AMOUNT[] = "scrollamount";
const char DOM_MARQUEE_DIRECTION[] = "direction";
const char DOM_MARQUEE_DIRECTION_LEFT[] = "left";
const char DOM_MARQUEE_DIRECTION_RIGHT[] = "right";
const char DOM_MARQUEE_VALUE[] = "value";
const char DOM_MARQUEE_COLOR[] = "color";
const char DOM_MARQUEE_FONT_SIZE[] = "fontSize";
const char DOM_MARQUEE_FONT_WEIGHT[] = "fontWeight";
const char DOM_MARQUEE_FONT_FAMILY[] = "fontFamily";
const char DOM_MARQUEE_EVENT_BOUNCE[] = "bounce";
const char DOM_MARQUEE_EVENT_FINISH[] = "finish";
const char DOM_MARQUEE_EVENT_START[] = "start";
const char DOM_MARQUEE_METHOD_START[] = "start";
const char DOM_MARQUEE_METHOD_STOP[] = "stop";

// navigationBar attribute and style constants
const char DOM_NAVIGATION_BAR_METHOD_SHOW[] = "show";
const char DOM_NAVIGATION_BAR_METHOD_HIDE[] = "hide";
const char DOM_NAVIGATION_BAR_TYPE[] = "type";
const char DOM_NAVIGATION_BAR_HEADER[] = "header";
const char DOM_NAVIGATION_BAR_TITLE[] = "title";
const char DOM_NAVIGATION_BAR_SUBTITLE[] = "subtitle";
const char DOM_NAVIGATION_BAR_BACK_ENABLED[] = "backenabled";
const char DOM_NAVIGATION_BAR_LOGO[] = "logo";
const char DOM_NAVIGATION_BAR_START_ICON[] = "starticon";
const char DOM_NAVIGATION_BAR_END_ICON[] = "endicon";
const char DOM_NAVIGATION_BAR_TITLE_COLOR[] = "titleColor";
const char DOM_NAVIGATION_BAR_SUBTITLE_COLOR[] = "subtitleColor";
const char DOM_NAVIGATION_BAR_EVENT_BACK_CLICK[] = "backclick";
const char DOM_NAVIGATION_BAR_EVENT_START_CLICK[] = "startclick";
const char DOM_NAVIGATION_BAR_EVENT_END_CLICK[] = "endclick";

// common event list
const char DOM_CHANGE[] = "change";
const char DOM_TOUCH_START[] = "touchstart";
const char DOM_TOUCH_MOVE[] = "touchmove";
const char DOM_TOUCH_CANCEL[] = "touchcancel";
const char DOM_TOUCH_END[] = "touchend";
const char DOM_CAPTURE_TOUCH_START[] = "capturetouchstart";
const char DOM_CAPTURE_TOUCH_MOVE[] = "capturetouchmove";
const char DOM_CAPTURE_TOUCH_CANCEL[] = "capturetouchcancel";
const char DOM_CAPTURE_TOUCH_END[] = "capturetouchend";
const char DOM_CATCH_BUBBLE_TOUCH_START[] = "catchbubbletouchstart";
const char DOM_CATCH_BUBBLE_TOUCH_MOVE[] = "catchbubbletouchmove";
const char DOM_CATCH_BUBBLE_TOUCH_CANCEL[] = "catchbubbletouchcancel";
const char DOM_CATCH_BUBBLE_TOUCH_END[] = "catchbubbletouchend";
const char DOM_CATCH_CAPTURE_TOUCH_START[] = "catchcapturetouchstart";
const char DOM_CATCH_CAPTURE_TOUCH_MOVE[] = "catchcapturetouchmove";
const char DOM_CATCH_CAPTURE_TOUCH_CANCEL[] = "catchcapturetouchcancel";
const char DOM_CATCH_CAPTURE_TOUCH_END[] = "catchcapturetouchend";
const char DOM_CLICK[] = "click";
const char DOM_DRAG_START[] = "dragstart";
const char DOM_DRAG[] = "drag";
const char DOM_DRAG_END[] = "dragend";
const char DOM_DRAG_ENTER[] = "dragenter";
const char DOM_DRAG_OVER[] = "dragover";
const char DOM_DRAG_LEAVE[] = "dragleave";
const char DOM_DRAG_DROP[] = "drop";
const char DOM_CATCH_BUBBLE_CLICK[] = "catchbubbleclick";
const char DOM_DOUBLE_CLICK[] = "doubleclick";
const char DOM_LONG_PRESS[] = "longpress";
const char DOM_PINCH_START[] = "pinchstart";
const char DOM_PINCH_UPDATE[] = "pinchupdate";
const char DOM_PINCH_END[] = "pinchend";
const char DOM_PINCH_CANCEL[] = "pinchcancel";
const char DOM_COMPLETE[] = "complete";
const char DOM_ERROR[] = "error";
const char DOM_FOCUS[] = "focus";
const char DOM_BLUR[] = "blur";
const char DOM_KEY[] = "key";
const char DOM_SELECTED[] = "selected";
const char DOM_CANCEL[] = "cancel";
const char DOM_HIDE[] = "hide";
const char DOM_MOUSE[] = "mouse";
const char DOM_HOVER[] = "hover";
const char DOM_SWIPE[] = "swipe";
const char DOM_ROTATION[] = "rotation";
const char DOM_ANIMATION_FINISH[] = "animationfinish";

// video constants
const char DOM_VIDEO_MUTED[] = "muted";
const char DOM_VIDEO_SRC[] = "src";
const char DOM_VIDEO_AUTOPLAY[] = "autoplay";
const char DOM_VIDEO_POSTER[] = "poster";
const char DOM_VIDEO_CONTROLS[] = "controls";
const char DOM_VIDEO_FIT[] = "objectFit";
const char DOM_VIDEO_LOOP[] = "loop";
const char DOM_VIDEO_START_TIME[] = "starttime";
const char DOM_VIDEO_EVENT_PREPARED[] = "prepared";
const char DOM_VIDEO_EVENT_START[] = "start";
const char DOM_VIDEO_EVENT_PAUSE[] = "pause";
const char DOM_VIDEO_EVENT_STOP[] = "stop";
const char DOM_VIDEO_EVENT_FINISH[] = "finish";
const char DOM_VIDEO_EVENT_ERROR[] = "error";
const char DOM_VIDEO_EVENT_SEEKING[] = "seeking";
const char DOM_VIDEO_EVENT_SEEKED[] = "seeked";
const char DOM_VIDEO_EVENT_TIMEUPDATE[] = "timeupdate";
const char DOM_VIDEO_EVENT_FULLSCREENCHANGE[] = "fullscreenchange";
const char DOM_VIDEO_METHOD_START[] = "start";
const char DOM_VIDEO_METHOD_PAUSE[] = "pause";
const char DOM_VIDEO_METHOD_STOP[] = "stop";
const char DOM_VIDEO_METHOD_SEEK_TO[] = "setCurrentTime";
const char DOM_VIDEO_METHOD_REQUEST_FULLSCREEN[] = "requestFullscreen";
const char DOM_VIDEO_METHOD_EXIT_FULLSCREEN[] = "exitFullscreen";
const char DOM_VIDEO_SPEED[] = "speed";
const char DOM_VIDEO_DIRECTION[] = "direction";
const char DOM_VIDEO_POSITION[] = "objectPosition";
const char DOM_VIDEO_POSITION_LEFT[] = "left";
const char DOM_VIDEO_POSITION_CENTER[] = "center";
const char DOM_VIDEO_POSITION_RIGHT[] = "right";
const char DOM_VIDEO_POSITION_TOP[] = "top";
const char DOM_VIDEO_POSITION_BOTTOM[] = "bottom";

// textarea attribute and style constants
const char DOM_TEXTAREA_VALUE[] = "value";
const char DOM_TEXTAREA_PLACEHOLDER[] = "placeholder";
const char DOM_TEXTAREA_MAXLENGTH[] = "maxlength";
const char DOM_TEXTAREA_MAXLINES[] = "maxlines";
const char DOM_TEXTAREA_OBSCURE[] = "obscure";
const char DOM_INPUT_AUTO_FOCUS[] = "autoFocus";
const char DOM_TEXTAREA_PLACEHOLDER_COLOR[] = "placeholderColor";
const char DOM_TEXTAREA_CURSOR_COLOR[] = "cursorColor";
const char DOM_TEXTAREA_COLOR[] = "color";
const char DOM_TEXTAREA_FONT_SIZE[] = "fontSize";
const char DOM_TEXTAREA_FONT_WEIGHT[] = "fontWeight";
const char DOM_TEXTAREA_FONT_FAMILY[] = "fontFamily";
const char DOM_INPUT_RECT_RADIUS[] = "rectRadius";
const char DOM_INPUT_BACKGROUND_COLOR[] = "backgroundColor";
const char DOM_INPUT_CLICKED_COLOR[] = "clickedColor";
const char DOM_INPUT_DISABLE_COLOR[] = "disabledColor";
const char DOM_INPUT_FOCUS_COLOR[] = "focusColor";
const char DOM_TEXTAREA_METHOD_FOCUS[] = "focus";
const char DOM_TEXTAREA_EXTEND[] = "extend";
const char DOM_TEXTAREA_SHOW_COUNTER[] = "showcounter";
const char DOM_INPUT_SHOW_COUNTER[] = "showcounter";
const char DOM_INPUT_METHOD_SHOW_ERROR[] = "showError";
const char DOM_INPUT_METHOD_DELETE[] = "delete";
const char DOM_INPUT_METHOD_INSERT[] = "insert";

// input attribute and style constants
const char DOM_INPUT_TYPE[] = "type";
const char DOM_INPUT_TYPE_EMAIL[] = "email";
const char DOM_INPUT_TYPE_DATE[] = "date";
const char DOM_INPUT_TYPE_TIME[] = "time";
const char DOM_INPUT_TYPE_NUMBER[] = "number";
const char DOM_INPUT_TYPE_PASSWORD[] = "password";
const char DOM_INPUT_CHECKED[] = "checked";
const char DOM_INPUT_NAME[] = "name";
const char DOM_INPUT_VALUE[] = "value";
const char DOM_INPUT_PLACEHOLDER[] = "placeholder";
const char DOM_INPUT_MAXLENGTH[] = "maxlength";
const char DOM_INPUT_ENTERKEYTYPE[] = "enterkeytype";
const char DOM_INPUT_OBSCURE[] = "obscure";
const char DOM_INPUT_PLACEHOLDER_COLOR[] = "placeholderColor";
const char DOM_INPUT_CURSOR_COLOR[] = "cursorColor";
const char DOM_INPUT_COLOR[] = "color";
const char DOM_INPUT_FONT_SIZE[] = "fontSize";
const char DOM_INPUT_FONT_WEIGHT[] = "fontWeight";
const char DOM_INPUT_WIDTH[] = "width";
const char DOM_INPUT_HEIGHT[] = "height";
const char DOM_INPUT_FONT_FAMILY[] = "fontFamily";
const char DOM_INPUT_METHOD_FOCUS[] = "focus";
const char DOM_ICON_SRC[] = "headericon";
const char DOM_SHOW_ICON_SRC[] = "showicon";
const char DOM_HIDE_ICON_SRC[] = "hideicon";
const char DOM_INPUT_SHOW_PASSWORD_ICON[] = "showpasswordicon";
const char DOM_INPUT_OPTION_ICON[] = "icon";
const char DOM_INPUT_OPTION_CONTENT[] = "content";
const char DOM_INPUT_SELECTED_START[] = "selectedstart";
const char DOM_INPUT_SELECTED_END[] = "selectedend";
const char DOM_INPUT_SOFT_KEYBOARD_ENABLED[] = "softkeyboardenabled";

// input event constants
const char DOM_INPUT_EVENT_NAME[] = "name";
const char DOM_INPUT_EVENT_VALUE[] = "value";
const char DOM_INPUT_EVENT_CHECKED[] = "checked";
const char DOM_INPUT_EVENT_ENTERKEYCLICK[] = "enterkeyclick";
const char DOM_INPUT_EVENT_OPTION_SELECT[] = "optionselect";
const char DOM_INPUT_EVENT_TRANSLATE[] = "translate";
const char DOM_INPUT_EVENT_SHARE[] = "share";
const char DOM_INPUT_EVENT_SEARCH[] = "search";
const char DOM_INPUT_EVENT_SELECT_CHANGE[] = "selectchange";

// divider style extern constants
const char DOM_DIVIDER_VERTICAL[] = "vertical";
const char DOM_DIVIDER_STROKE_WIDTH[] = "strokeWidth";
const char DOM_DIVIDER_LINE_CAP[] = "lineCap";

// canvas
const char DOM_CANVAS_METHOD_GET_CONTEXT[] = "getContext";

// button attribute and style constants
const char DOM_BUTTON_TYPE[] = "type";
const char DOM_BUTTON_HEIGHT[] = "height";
const char DOM_BUTTON_WIDTH[] = "width";
const char DOM_BUTTON_MIN_WIDTH[] = "minWidth";
const char DOM_BUTTON_RRECT_RADIUS[] = "radius";
const char DOM_BUTTON_DEFAULT_COLOR[] = "backgroundColor";
const char DOM_BUTTON_CLICKED_COLOR[] = "clickedColor";
const char DOM_BUTTON_DISABLE_COLOR[] = "disabledColor";
const char DOM_BUTTON_FOCUS_COLOR[] = "focusColor";
const char DOM_BUTTON_AUTO_FOCUS[] = "autofocus";
const char DOM_BUTTON_TEXT_DATA[] = "value";
const char DOM_BUTTON_TEXT_COLOR[] = "textColor";
const char DOM_BUTTON_TEXT_DISABLE_COLOR[] = "disabledTextColor";
const char DOM_BUTTON_FONT_SIZE[] = "fontSize";
const char DOM_BUTTON_FONT_WEIGHT[] = "fontWeight";
const char DOM_BUTTON_FONT_FAMILY[] = "fontFamily";
const char DOM_BUTTON_FONT_STYLE[] = "fontStyle";
const char DOM_BUTTON_BORDER_WIDTH[] = "borderWidth";
const char DOM_BUTTON_BORDER_COLOR[] = "borderColor";
const char DOM_BUTTON_WAITING[] = "waiting";
const char DOM_BUTTON_ICON[] = "icon";
const char DOM_BUTTON_ICON_WIDTH[] = "iconWidth";
const char DOM_BUTTON_ICON_HEIGHT[] = "iconHeight";
const char DOM_BUTTON_ICON_DIRECTION[] = "changeIconDirection";
const char DOM_BUTTON_PROGRESS_DIAMETER[] = "diameter";
const char DOM_BUTTON_PROGRESS_COLOR[] = "progressColor";
const char DOM_BUTTON_PROGRESS_FOCUS_COLOR[] = "progressFocusColor";
const char DOM_BUTTON_INNER_PADDING[] = "innerPadding";
const char DOM_BUTTON_METHOD_SET_PROGRESS[] = "setProgress";

// picker attribute and style constants
const char DOM_PICKER_SPLIT_ITEM = '\t';
const char DOM_PICKER_SPLIT_ARRAY = '\n';
const char DOM_PICKER_CONTAIN_SECOND[] = "containsecond";
const char DOM_PICKER_RANGE[] = "range";
const char DOM_PICKER_COLUMN_CHANGE[] = "columnchange";
const char DOM_PICKER_COLUMN_COUNT[] = "columns";
const char DOM_PICKER_HOUR24[] = "hours";
const char DOM_PICKER_LUNAR[] = "lunarswitch";
const char DOM_PICKER_SHOW_LUNAR[] = "lunar";
const char DOM_PICKER_TYPE_TIME[] = "time";
const char DOM_PICKER_TYPE_DATE[] = "date";
const char DOM_PICKER_TYPE_TEXT[] = "text";
const char DOM_PICKER_TYPE_MULTITEXT[] = "multi-text";
const char DOM_PICKER_TYPE_DATETIME[] = "datetime";
const char DOM_PICKER_TEXT_COLOR[] = "textColor";
const char DOM_PICKER_SELECT_COLOR[] = "selectedColor";
const char DOM_PICKER_SELECT_SIZE[] = "selectedFontSize";
const char DOM_PICKER_SELECT_FAMILIES[] = "selectedFontFamily";
const char DOM_PICKER_FOCUS_COLOR[] = "focusColor";
const char DOM_PICKER_FOCUS_SIZE[] = "focusFontSize";
const char DOM_PICKER_FOCUS_FAMILIES[] = "focusFontFamily";
const char DOM_PICKER_PREFIX[] = "indicatorprefix";
const char DOM_PICKER_SUFFIX[] = "indicatorsuffix";
const char DOM_PICKER_DISAPPEAR_COLOR[] = "disappearColor";
const char DOM_PICKER_DISAPPEAR_FONT_SIZE[] = "disappearFontSize";
const char DOM_PICKER_COLUMN_HEIGHT[] = "columnHeight";
const char DOM_PICKER_VIBRATE[] = "vibrate";

// calendar
const char DOM_CALENDAR_DATA[] = "calendardata";
const char DOM_CALENDAR_DATE[] = "date";
const char DOM_CALENDAR_DATE_ADAPTER[] = "dateadapter";
const char DOM_CALENDAR_DIRECTION[] = "direction";
const char DOM_CALENDAR_METHOD_GO_TO[] = "goto";
const char DOM_CALENDAR_EVENT_SELECTED_CHANGE[] = "selectedchange";
const char DOM_CALENDAR_SHOW_HOLIDAY[] = "showholiday";
const char DOM_CALENDAR_SHOW_LUNAR[] = "showlunar";
const char DOM_CALENDAR_CARD_CALENDAR[] = "cardcalendar";
const char DOM_CALENDAR_START_DAY_OF_WEEK[] = "startdayofweek";
const char DOM_CALENDAR_TYPE[] = "type";
const char DOM_CALENDAR_EVENT_REQUEST_DATA[] = "requestdata";
const char DOM_CALENDAR_OFF_DAYS[] = "offdays";
const char DOM_CALENDAR_WORK_DAYS[] = "workdays";
const char DOM_CALENDAR_HOLIDAYS[] = "holidays";

// dialog
const char DOM_DIALOG_METHOD_SHOW[] = "show";
const char DOM_DIALOG_METHOD_CLOSE[] = "close";
const char DOM_DIALOG_EVENT_CANCEL[] = "cancel";
const char DOM_DIALOG_STYLE_DRAGABLE[] = "dragable";

// multi modal event
const char DOM_VOICE_LABEL[] = "voicelabel";
const char DOM_SUBSCRIPT_LABEL[] = "subscriptlabel";
const char DOM_SUBSCRIPT_FLAG[] = "subscriptflag";
const char DOM_SCENE_LABEL[] = "scenelabel";

// search constants
const char DOM_SEARCH_HINT[] = "hint";
const char DOM_SEARCH_ICON[] = "icon";
const char DOM_SUBMIT[] = "submit";
const char DOM_SEARCH_VALUE[] = "value";
const char DOM_SEARCH_BUTTON[] = "searchbutton";

// toggle constants
const char DOM_TOGGLE_BACKGROUND_COLOR[] = "backgroundColor";
const char DOM_TOGGLE_CHECKED_COLOR[] = "checkedColor";
const char DOM_TOGGLE_CHECKED_STATE[] = "checked";
const char DOM_TOGGLE_TEXT_COLOR[] = "textColor";
const char DOM_TOGGLE_HEIGHT[] = "height";
const char DOM_TOGGLE_WIDTH[] = "width";

// clock constants
// clock style
const char DOM_DIGIT_FONT_FAMILY[] = "fontFamily";
// clock attr
const char DOM_HOURS_WEST[] = "hourswest";
const char DOM_CLOCK_CONFIG[] = "clockconfig";
const char DOM_DIGIT_COLOR[] = "digitColor";
const char DOM_DIGIT_COLOR_NIGHT[] = "digitColorNight";
const char DOM_DIGIT_RADIUS_RATIO[] = "digitRadiusRatio";
const char DOM_DIGIT_SIZE_RATIO[] = "digitSizeRatio";
const char DOM_CLOCK_FACE_SOURCE[] = "face";
const char DOM_CLOCK_FACE_SOURCE_NIGHT[] = "faceNight";
const char DOM_HOUR_HAND_SOURCE[] = "hourHand";
const char DOM_HOUR_HAND_SOURCE_NIGHT[] = "hourHandNight";
const char DOM_MINUTE_HAND_SOURCE[] = "minuteHand";
const char DOM_MINUTE_HAND_SOURCE_NIGHT[] = "minuteHandNight";
const char DOM_SECOND_HAND_SOURCE[] = "secondHand";
const char DOM_SECOND_HAND_SOURCE_NIGHT[] = "secondHandNight";
const char DOM_SHOW_DIGIT[] = "showdigit";

// piece constants
const char DOM_PIECE_CONTENT[] = "content";
const char DOM_PIECE_ICON[] = "icon";
const char DOM_PIECE_CLOSABLE[] = "closable";
const char DOM_PIECE_EVENT_CLOSE[] = "close";

// badge constants
const char DOM_BADGE_CONFIG[] = "config";
const char DOM_BADGE_COUNT[] = "count";
const char DOM_BADGE_LABEL[] = "label";
const char DOM_BADGE_PLACEMENT[] = "placement";
const char DOM_BADGE_VISIBLE[] = "visible";
const char DOM_BADGE_MAX_COUNT[] = "maxcount";
const char DOM_BADGE_COLOR[] = "badgeColor";
const char DOM_BADGE_CIRCLE_SIZE[] = "badgeSize";
const char DOM_BADGE_TEXT_FONT_SIZE[] = "textSize";
const char DOM_BADGE_TEXT_COLOR[] = "textColor";

// grid container/row/columns
const char DOM_GRID_CONTAINER_COLUMNS[] = "columns";
const char DOM_GRID_CONTAINER_GUTTER_WIDTH[] = "gutterwidth";
const char DOM_GRID_CONTAINER_SIZE_TYPE[] = "sizetype";
const char DOM_GRID_CONTAINER_GUTTER[] = "gutter";
const char DOM_GRID_CONTAINER_TEMPLATE[] = "gridtemplate";
const char DOM_GRID_TEMPLATE_TYPE_NORMAL[] = "normal";
const char DOM_GRID_TEMPLATE_TYPE_GRID[] = "grid";
const char DOM_GRID_CONTAINER_GET_COLUMNS[] = "getColumns";
const char DOM_GRID_CONTAINER_GET_COLUMN_WIDTH[] = "getColumnWidth";
const char DOM_GRID_CONTAINER_GET_GUTTER_WIDTH[] = "getGutterWidth";
const char DOM_GRID_CONTAINER_GET_SIZE_TYPE[] = "getSizeType";
const char DOM_GRID_COLUMN_SPAN[] = "span";
const char DOM_GRID_COLUMN_OFFSET[] = "offset";
const char DOM_GRID_COLUMN_TYPE[] = "columntype";
const char DOM_GRID_SIZE_TYPE_XS[] = "xs";
const char DOM_GRID_SIZE_TYPE_SM[] = "sm";
const char DOM_GRID_SIZE_TYPE_MD[] = "md";
const char DOM_GRID_SIZE_TYPE_LG[] = "lg";

const char DOM_COLUMN_TYPE_NONE[] = "none";
const char DOM_COLUMN_TYPE_BUBBLE[] = "bubble";
const char DOM_COLUMN_TYPE_BUTTON[] = "button";
const char DOM_COLUMN_TYPE_CARD[] = "card";
const char DOM_COLUMN_TYPE_CONTENT[] = "content";

const char DOM_CLICK_EFFECT[] = "clickEffect";
const char DOM_SPRING_EFFECT[] = "springeffect";
// panel constants
const char DOM_PANEL_METHOD_SHOW[] = "show";
const char DOM_PANEL_METHOD_CLOSE[] = "close";
const char DOM_PANEL_ATTR_TYPE[] = "type";
const char DOM_PANEL_ATTR_MODE[] = "mode";
const char DOM_PANEL_ATTR_DRAG_BAR[] = "dragbar";
const char DOM_PANEL_ATTR_MIN_HEIGHT[] = "miniheight";
const char DOM_PANEL_ATTR_HALF_HEIGHT[] = "halfheight";
const char DOM_PANEL_ATTR_FULL_HEIGHT[] = "fullheight";
const char DOM_PANEL_EVENT_SIZE_CHANGED[] = "sizechange";

// tool-bar-item constants
const char DOM_TOOL_BAR_ITEM_ICON[] = "icon";
const char DOM_TOOL_BAR_ITEM_VALUE[] = "value";
const char DOM_TOOL_BAR_ITEM_TEXT_COLOR[] = "textColor";
const char DOM_TOOL_BAR_ITEM_COLOR[] = "color";
const char DOM_TOOL_BAR_ITEM_FONT_SIZE[] = "fontSize";
const char DOM_TOOL_BAR_ITEM_FONT_STYLE[] = "fontStyle";
const char DOM_TOOL_BAR_ITEM_FONT_WEIGHT[] = "fontWeight";
const char DOM_TOOL_BAR_ITEM_TEXT_DECORATION[] = "textDecoration";
const char DOM_TOOL_BAR_ITEM_FONT_FAMILY[] = "fontFamily";
const char DOM_TOOL_BAR_ITEM_ALLOW_SCALE[] = "allowScale";

// card transition
const char DOM_TRANSITION_EFFECT[] = "transitionEffect";
const char DOM_TRANSITION_CARD_COMPOSEID[] = "ref";
const char DOM_TRANSITION_CARD_PARAMS[] = "paramsData";

// qrcode
const char DOM_QRCODE_BACKGROUND_COLOR[] = "backgroundColor";
const char DOM_QRCODE_COLOR[] = "color";
const char DOM_QRCODE_HEIGHT[] = "height";
const char DOM_QRCODE_WIDTH[] = "width";
const char DOM_QRCODE_TYPE[] = "type";
const char DOM_QRCODE_VALUE[] = "value";

// svg
const char DOM_SVG_ID[] = "svgId";
const char DOM_SVG_ANIMATION_BEGIN[] = "begin";
const char DOM_SVG_ANIMATION_DUR[] = "dur";
const char DOM_SVG_ANIMATION_END[] = "end";
const char DOM_SVG_ANIMATION_MIN[] = "min";
const char DOM_SVG_ANIMATION_MAX[] = "max";
const char DOM_SVG_ANIMATION_RESTART[] = "restart";
const char DOM_SVG_ANIMATION_REPEAT_COUNT[] = "repeatcount";
const char DOM_SVG_ANIMATION_REPEAT_DUR[] = "repeatdur";
const char DOM_SVG_ANIMATION_FILL[] = "fill";
const char DOM_SVG_ANIMATION_CALC_MODE[] = "calcmode";
const char DOM_SVG_ANIMATION_VALUES[] = "values";
const char DOM_SVG_ANIMATION_KEY_TIMES[] = "keytimes";
const char DOM_SVG_ANIMATION_KEY_SPLINES[] = "keysplines";
const char DOM_SVG_ANIMATION_FROM[] = "from";
const char DOM_SVG_ANIMATION_TO[] = "to";
const char DOM_SVG_ANIMATION_BY[] = "by";
const char DOM_SVG_ANIMATION_ATTRIBUTE_NAME[] = "attributename";
const char DOM_SVG_ANIMATION_ADDITIVE[] = "additive";
const char DOM_SVG_ANIMATION_ACCUMULATE[] = "accumulate";
const char DOM_SVG_ANIMATION_TYPE[] = "type";
const char DOM_SVG_ANIMATION_KEY_POINTS[] = "keypoints";
const char DOM_SVG_ANIMATION_PATH[] = "path";
const char DOM_SVG_ANIMATION_ROTATE[] = "rotate";
const char DOM_SVG_ATTR_PATH[] = "path";
const char DOM_SVG_START_OFFSET[] = "startoffset";
const char DOM_SVG_FILL[] = "fill";
const char DOM_SVG_FILL_OPACITY[] = "fillOpacity";
const char DOM_SVG_FILL_RULE[] = "fillRule";
const char DOM_SVG_FILTER[] = "filter";
const char DOM_SVG_FONT_FAMILY[] = "fontFamily";
const char DOM_SVG_FONT_SIZE[] = "fontSize";
const char DOM_SVG_FONT_STYLE[] = "fontStyle";
const char DOM_SVG_FONT_WEIGHT[] = "fontWeight";
const char DOM_SVG_LETTER_SPACING[] = "letterSpacing";
const char DOM_SVG_TEXT_DECORATION[] = "textDecoration";
const char DOM_SVG_TEXT_LENGTH[] = "textlength";
const char DOM_SVG_MASK[] = "mask";
const char DOM_SVG_STROKE[] = "stroke";
const char DOM_SVG_STROKE_DASHARRAY[] = "strokeDasharray";
const char DOM_SVG_STROKE_DASHOFFSET[] = "strokeDashoffset";
const char DOM_SVG_STROKE_LINECAP[] = "strokeLinecap";
const char DOM_SVG_STROKE_LINEJOIN[] = "strokeLinejoin";
const char DOM_SVG_STROKE_MITERLIMIT[] = "strokeMiterlimit";
const char DOM_SVG_STROKE_OPACITY[] = "strokeOpacity";
const char DOM_SVG_STROKE_WIDTH[] = "strokeWidth";
const char DOM_SVG_OPACITY[] = "opacity";
const char DOM_SVG_VIEW_BOX[] = "viewbox";
const char DOM_SVG_X[] = "x";
const char DOM_SVG_Y[] = "y";
const char DOM_SVG_WIDTH[] = "width";
const char DOM_SVG_HEIGHT[] = "height";
const char DOM_SVG_DX[] = "dx";
const char DOM_SVG_DY[] = "dy";
const char DOM_SVG_RX[] = "rx";
const char DOM_SVG_RY[] = "ry";
const char DOM_SVG_D[] = "d";
const char DOM_SVG_X1[] = "x1";
const char DOM_SVG_X2[] = "x2";
const char DOM_SVG_Y1[] = "y1";
const char DOM_SVG_Y2[] = "y2";
const char DOM_SVG_POINTS[] = "points";
const char DOM_SVG_CX[] = "cx";
const char DOM_SVG_CY[] = "cy";
const char DOM_SVG_R[] = "r";
const char DOM_SVG_ROTATE[] = "rotate";
const char DOM_SVG_LENGTH_ADJUST[] = "lengthadjust";
const char DOM_SVG_MIRROR[] = "automirror";
const char DOM_SVG_MASK_CONTENT_UNITS[] = "maskcontentunits";
const char DOM_SVG_MASK_UNITS[] = "maskunits";
const char DOM_SVG_PATTERN_CONTENT_UNITS[] = "patterncontentunits";
const char DOM_SVG_PATTERN_UNITS[] = "patternunits";
const char DOM_SVG_PATTERN_TRANSFORM[] = "patterntransform";
const char DOM_SVG_OFFSET[] = "offset";
const char DOM_SVG_STOP_COLOR[] = "stopColor";
const char DOM_SVG_STOP_OPACITY[] = "stopOpacity";
const char DOM_SVG_GRADIENT_TRANSFORM[] = "gradienttransform";
const char DOM_SVG_SPREAD_METHOD[] = "spreadmethod";
const char DOM_SVG_FX[] = "fx";
const char DOM_SVG_FY[] = "fy";
const char DOM_SVG_FR[] = "fr";
const char DOM_SVG_HREF[] = "href";
const char DOM_SVG_XLINK_HREF[] = "xlink:href";
const char DOM_SVG_CLIP_RULE[] = "clipRule";
const char DOM_SVG_FE_IN[] = "in";
const char DOM_SVG_FE_IN2[] = "in2";
const char DOM_SVG_FE_RESULT[] = "result";
const char DOM_SVG_FE_COLOR_INTERPOLATION_FILTERS[] = "color-interpolation-filters";
const char DOM_SVG_FE_TYPE[] = "type";
const char DOM_SVG_FE_VALUES[] = "values";
const char DOM_SVG_FE_FLOOD_COLOR[] = "flood-color";
const char DOM_SVG_FE_FLOOD_OPACITY[] = "flood-opacity";
const char DOM_SVG_FE_EDGE_MODE[] = "edgemode";
const char DOM_SVG_FE_STD_DEVIATION[] = "stddeviation";
const char DOM_SVG_FE_AMPLITUDE[] = "amplitude";
const char DOM_SVG_FE_EXPONENT[] = "exponent";
const char DOM_SVG_FE_INTERCEPT[] = "intercept";
const char DOM_SVG_FE_SLOPE[] = "slope";
const char DOM_SVG_FE_TABLE_VALUES[] = "tablevalues";
const char DOM_SVG_FE_K1[] = "k1";
const char DOM_SVG_FE_K2[] = "k2";
const char DOM_SVG_FE_K3[] = "k3";
const char DOM_SVG_FE_K4[] = "k4";
const char DOM_SVG_FE_OPERATOR_TYPE[] = "operator";

// web
const char DOM_PAGESTART[] = "pagestart";
const char DOM_PAGEFINISH[] = "pagefinish";
const char DOM_PAGEERROR[] = "error";
const char DOM_WEB_WEBSRC[] = "src";
const char DOM_METHOD_RELOAD[] = "reload";
const char DOM_WEB_MESSAGE[] = "message";

// rich-text
const char DOM_RICH_TEXT_DATA[] = "value";
const char DOM_LOAD_START[] = "start";
const char DOM_LOAD_COMPLETE[] = "complete";

// xcomponent
const char DOM_XCOMPONENT_NAME[] = "name";
const char DOM_XCOMPONENT_TYPE[] = "type";
const char DOM_XCOMPONENT_LIBRARYNAME[] = "libraryname";
const char DOM_XCOMPONENT_INIT[] = "load";
const char DOM_XCOMPONENT_DESTROY[] = "destroy";

// z-index
const char DOM_ZINDEX[] = "zIndex";

// camera
const char DOM_TAKE_PHOTO[] = "takePhoto";
const char DOM_CAMERA_FLASH[] = "flash";
const char DOM_CAMERA_DEVICE_POSITION[] = "deviceposition";
const char DOM_CAMERA_START_RECORD[] = "startRecorder";
const char DOM_CAMERA_CLOSE_RECORDER[] = "closeRecorder";

// form
const char DOM_FORM_EVENT_SUBMIT[] = "submit";
const char DOM_FORM_EVENT_RESET[] = "reset";

// crown
const char DOM_CROWN_ROTATE[] = "rotate";

} // namespace OHOS::Ace
