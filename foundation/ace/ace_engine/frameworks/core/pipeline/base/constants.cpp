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

#include "core/pipeline/base/constants.h"

#include "base/utils/macros.h"

namespace OHOS::Ace {

// base constants
const char CLASS_NAME[] = "className";
const char NAME[] = "name";
const char CHILD[] = "child";

// common constants
const char DISABLED[] = "disabled";
const char WIDTH[] = "width";
const char HEIGHT[] = "height";
const char ALIGN[] = "align";

// image constants
const char IMAGE_NAME[] = "Image";
const char IMAGE_SRC[] = "src";
const char IMAGE_ALT[] = "alt";
const char IMAGE_WIDTH[] = "width";
const char IMAGE_HEIGHT[] = "height";
const char IMAGE_ALIGNMENT[] = "alignment";
const char IMAGE_POSITION[] = "imagePosition";
const char IMAGE_POSITION_SIZE_X[] = "positionX";
const char IMAGE_POSITION_SIZE_Y[] = "positionY";
const char IMAGE_POSITION_VALUE_X[] = "dx";
const char IMAGE_POSITION_VALUE_Y[] = "dy";
const char IMAGE_SIZE[] = "imageSize";
const char IMAGE_SIZE_VALUE[] = "value";
const char IMAGE_SIZE_TYPE[] = "type";
const char IMAGE_REPEAT[] = "imageRepeat";
const char IMAGE_FIT[] = "imageFit";
const char IMAGE_COLOR[] = "color";
const char IMAGE_ON_LOAD_SUCCESS[] = "onLoadSuccess";
const char IMAGE_ON_LOAD_FAIL[] = "onLoadFail";
const char IMAGE_ALT_BLANK[] = "blank";
const uint32_t ALT_COLOR_GREY = 0xFFCCCCCC;

// text constants
const char TEXT_NAME[] = "Text";
const char TEXT_DATA[] = "value";
const char TEXT_DIRECTION[] = "textDirection";
const char TEXT_COLOR[] = "color";
const char TEXT_DECORATION[] = "textDecoration";
const char TEXT_FONT_WEIGHT[] = "fontWeight";
const char TEXT_FONT_SIZE[] = "fontSize";
const char TEXT_FONT_STYLE[] = "fontStyle";
const char TEXT_BASE_LINE[] = "textBaseline";
const char TEXT_FONT_FAMILIES[] = "fontFamilies";
const char TEXT_MAX_LINES[] = "textMaxLines";
const char TEXT_ALIGN[] = "textAlign";
const char TEXT_OVERFLOW[] = "textOverflow";
const char TEXT_WORD_SPACING[] = "wordSpacing";
const char TEXT_LINE_HEIGHT[] = "lineHeight";
const char TEXT_LETTER_SPACING[] = "letterSpacing";
const char TEXT_STYLE[] = "textStyle";
const char TEXT_SPAN_NAME[] = "TextSpan";
const char TEXT_ADAPT[] = "textAdapt";
const char TEXT_ADAPT_MIN_FONTSIZE[] = "textAdaptMinSize";

// text style constants
const char TEXT_STYLE_NAME[] = "TextStyle";

// textField constants
const char TEXT_FIELD_NAME[] = "TextField";
const char TEXT_FIELD_TYPE[] = "keyboardType";
const char OBSCURE_TEXT[] = "obscure";
const char TEXT_FIELD_ENABLED[] = "enabled";
const char TEXT_FIELD_MAX_LENGTH[] = "maxLength";
const char TEXT_FIELD_MAX_LINES[] = "maxLines";
const char TEXT_FIELD_AUTO_FOCUS[] = "autoFocus";
const char TEXT_FIELD_ACTION_LABEL[] = "actionLabel";
const char TEXT_FIELD_ACTION[] = "action";
const char CURSOR_COLOR[] = "cursorColor";
const char TEXT_FIELD_PLACEHOLDER[] = "placeholder";
const char TEXT_FIELD_PLACEHOLDER_COLOR[] = "placeholderColor";
const char TEXT_FIELD_EXTEND[] = "extend";
const char TEXT_FIELD_ICON_IMAGE[] = "iconImage";
// events
const char TEXT_FIELD_ON_TEXT_CHANGE[] = "onTextChange";
const char TEXT_FIELD_ON_FINISH_INPUT[] = "onFinishInput";
const char TEXT_FIELD_ON_TAP[] = "onTap";

// color constants
const char COLOR[] = "color";
const char COLOR_NAME[] = "Color";
const char COLOR_VALUE[] = "value";
const char COLOR_RED[] = "red";
const char COLOR_GREEN[] = "green";
const char COLOR_BLUE[] = "blue";
const char COLOR_ALPHA[] = "alpha";
const char COLOR_OPACITY[] = "opacity";

// shadow constants
const char SHADOW[] = "shadow";
const char OFFSET[] = "offset";
const char HORIZONTAL_OFFSET[] = "dx";
const char VERTICAL_OFFSET[] = "dy";
const char RADIUS[] = "blurRadius";
const char SPREAD[] = "spreadRadius";
const char ELEVATION[] = "elevation";

// compose constants
const char COMPOSE_NAME[] = "Compose";
const char COMPOSE_ID[] = "composeId";
const char COMPOSE_DATA[] = "componentData";

// box constants
const char BOX_NAME[] = "Box";
const char BOX_BACK_DECORATION[] = "backDecoration";
const char BOX_FRONT_DECORATION[] = "frontDecoration";

// flex constants
const char COLUMN_NAME[] = "Column";
const char ROW_NAME[] = "Row";
const char MAIN_AXIS_ALIGN[] = "mainAxisAlign";
const char CROSS_AXIS_ALIGN[] = "crossAxisAlign";
const char MAIN_AXIS_SIZE[] = "mainAxisSize";

// flex item constants
const char FLEX_ITEM_NAME[] = "FlexItem";
const char FLEX_GROW[] = "flexGrow";
const char FLEX_SHRINK[] = "flexShrink";
const char FLEX_BASIS[] = "flexBasis";

// stack constants
const char STACK_NAME[] = "Stack";
const char STACK_ALIGNMENT[] = "alignment";
const char STACK_FIT[] = "stackFit";
const char STACK_OVERFLOW[] = "overflow";
const char STACK_MAIN_STACK_SIZE[] = "mainStackSize";

// positioned constants
const char POSITIONED_NAME[] = "Positioned";
const char POSITIONED_LEFT[] = "left";
const char POSITIONED_RIGHT[] = "right";
const char POSITIONED_HEIGHT[] = "height";
const char POSITIONED_WIDTH[] = "width";
const char POSITIONED_TOP[] = "top";
const char POSITIONED_BOTTOM[] = "bottom";

// list constants
const char LIST_NAME[] = "List";
const char LIST_COUNT[] = "count";
const char LIST_EVENT_REQUEST_ITEM[] = "requestitem";
const char LIST_DIRECTION[] = "direction";
const char LIST_CACHED_COUNT[] = "cachedcount";
const char LIST_BEGIN_INDEX[] = "beginindex";
const char LIST_END_INDEX[] = "endindex";
const char LIST_REPEATED_LENGTH[] = "repeatedlength";
const char LIST_INDEX_OFFSET[] = "indexoffset";
const char LIST_SCROLL_PAGE[] = "scrollPage";
const char LIST_POSITION[] = "listPosition";
// list item constants
const char LIST_ITEM_NAME[] = "ListItem";
const char LIST_ITEM_TYPE[] = "itemType";
// edge effect
const char EDGE_EFFECT[] = "edgeEffect";
const char FADE_COLOR[] = "fadeColor";

// grid constants
const char GRID_NAME[] = "Grid";
const char GRID_WIDTH[] = "width";
const char GRID_HEIGHT[] = "height";
const char GRID_ITEM_COUNT[] = "itemCount";
const char GRID_INITIAL_ITEM[] = "initialItem";
const char GRID_COLUMN_COUNT[] = "columnCount";
const char GRID_COLUMN_EXTENT[] = "columnExtent";
const char GRID_EVENT_REQUEST_ITEM[] = "requestitem";
const char GRID_SCROLL_PAGE[] = "scrollPage";
const char GRID_DIRECTION[] = "direction";
const char GRID_ALIGN[] = "align";
const char GRID_CACHED_COUNT[] = "cachedcount";
const char GRID_ITEM_NAME[] = "GridItem";
const char GRID_COLUMN_SPAN[] = "columnSpan";
const char GRID_ROW_SPAN[] = "rowSpan";
const char GRID_ROW_COUNT[] = "rowCount";
const char GRID_ROW_INDEX[] = "rowIndex";
const char GRID_COLUMN_INDEX[] = "columnIndex";
const char GRID_LAYOUT_NAME[] = "GridLayout";
const char GRID_LAYOUT_ITEM_NAME[] = "GridLayoutItem";
const int32_t DEFAULT_DIRECTION = 1; // FlexDirection::COLUMN
const int32_t DEFAULT_ALIGN = 4;     // FlexAlign::STRETCH
const int32_t DEFAULT_COLUMN_COUNT = 0;
const int32_t DEFAULT_COLUMN_EXTENT = 0;
const int32_t DEFAULT_COLUMN_SPAN = 1;
const int32_t DEFAULT_ITEM_COUNT = 0;
const int32_t DEFAULT_INITIAL_ITEM = 0;
const int32_t DEFAULT_CACHED_COUNT = 10;
const double DEFAULT_GRID_WIDTH = -1.0;
const double DEFAULT_GRID_HEIGHT = -1.0;

// scrollBar
extern const char SCROLL_BAR_DISPLAY_MODE[] = "scrollbar";
extern const char SCROLL_BAR_SHAPE_MODE[] = "shapemode";

// edge constants
const char PADDING_NAME[] = "Padding";
const char PADDING[] = "padding";
const char MARGIN[] = "margin";
const char EDGE_NAME[] = "Edge";
const char EDGE_VALUE[] = "value";
const char EDGE_LEFT[] = "left";
const char EDGE_TOP[] = "top";
const char EDGE_RIGHT[] = "right";
const char EDGE_BOTTOM[] = "bottom";

// border constants
const char BORDER_NAME[] = "Border";
const char BORDER_VALUE[] = "value";
const char BORDER_LEFT[] = "left";
const char BORDER_TOP[] = "top";
const char BORDER_RIGHT[] = "right";
const char BORDER_BOTTOM[] = "bottom";
const char BORDER_RADIUS_VALUE[] = "radius";
const char BORDER_TOP_LEFT[] = "topLeftRadius";
const char BORDER_TOP_RIGHT[] = "topRightRadius";
const char BORDER_BOTTOM_LEFT[] = "bottomLeftRadius";
const char BORDER_BOTTOM_RIGHT[] = "bottomRightRadius";

// borderEdge constants
const char BORDER_EDGE_NAME[] = "BorderEdge";
const char BORDER_EDGE_WIDTH[] = "width";
const char BORDER_EDGE_COLOR[] = "color";
const char BORDER_EDGE_STYLE[] = "style";

// radius constants
const char RADIUS_NAME[] = "Radius";
const char RADIUS_VALUE[] = "value";
const char RADIUS_X[] = "x";
const char RADIUS_Y[] = "y";

// decoration constants
const char DECORATION_NAME[] = "Decoration";
const char SHADOW_LIST[] = "shadowList";
const char BACKGROUND_IMAGE[] = "backgroundImage";
const char BACKGROUND_COLOR[] = "color";
const char DECORATION_BORDER[] = "border";
const char DECORATION_PADDING[] = "padding";

// backgroundImage constants
const char BACKGROUND_IMAGE_NAME[] = "BackgroundImage";
const char BACKGROUND_SRC[] = "src";

// gradient constants
const char GRADIENT_NAME[] = "Gradient";
const char GRADIENT_DIRECTION[] = "direction";
const char GRADIENT_REPEAT[] = "repeat";
const char GRADIENT_ANGLE[] = "angle";
const char GRADIENT_COLOR_LIST[] = "colorList";
const char GRADIENT_DIMENSION[] = "dimension";
const char GRADIENT_DIMENSION_VALUE[] = "value";
const char GRADIENT_DIMENSION_UNIT[] = "unit";
const char GRADIENT_COLOR[] = "color";
const char GRADIENT_HAS_VALUE[] = "hasValue";

// align constants
const char ALIGN_NAME[] = "Align";
const char HORIZONTAL[] = "horizontal";
const char VERTICAL[] = "vertical";
const char HORIZONTAL_VALUE_LEFT[] = "left";
const char HORIZONTAL_VALUE_CENTER[] = "center";
const char HORIZONTAL_VALUE_RIGHT[] = "right";
const char VERTICAL_VALUE_TOP[] = "top";
const char VERTICAL_VALUE_CENTER[] = "center";
const char VERTICAL_VALUE_BOTTOM[] = "bottom";

// tween constants
const char TWEEN_NAME[] = "Tween";
const char DURATION[] = "duration";
const char DELAY[] = "delay";
const char REVERSE[] = "reverse";
const char ITERATION[] = "iteration";
const char FILL_MODE[] = "fill-mode";
const char CURVE[] = "curve";
const char TWEEN_OPTION[] = "option";
const char OPTION_SCALE[] = "scale";
const char OPTION_SCALE_X[] = "scaleX";
const char OPTION_SCALE_Y[] = "scaleY";
const char OPTION_TRANSLATE[] = "translate";
const char OPTION_TRANSLATE_X[] = "translateX";
const char OPTION_TRANSLATE_Y[] = "translateY";
const char OPTION_ROTATE_Z[] = "rotateZ";
const char OPTION_ROTATE_X[] = "rotateX";
const char OPTION_ROTATE_Y[] = "rotateY";

// slider constants
const char SLIDER_NAME[] = "Slider";
const char VALUE[] = "value";
const char SLIDER_MIN[] = "min";
const char SLIDER_MAX[] = "max";
const char SLIDER_STEP[] = "step";
const char SLIDER_SELECTED_COLOR[] = "barSelectedColor";
const char SLIDER_BAR_BACKGROUND_COLOR[] = "barBackgroundColor";
const char SLIDER_BLOCK_COLOR[] = "blockColor";
const char ON_MOVE_END_ID[] = "onMoveEndId";

// swiper constants
const char SWIPER_NAME[] = "Swiper";
const char SWIPER_ON_CHANGED[] = "onChanged";
const char SWIPER_INDEX[] = "index";
const char SWIPER_SHOW_INDICATOR[] = "showIndicator";
const char SWIPER_DURATION[] = "duration";
const char SWIPER_LOOP[] = "loop";
const char SWIPER_AUTO_PLAY[] = "autoPlay";
const char SWIPER_AUTO_PLAY_INTERVAL[] = "autoPlayInterval";
const char SWIPER_IS_VERTICAL[] = "isVertical";
const char SWIPER_INDICATOR_COLOR[] = "indicatorColor";
const char SWIPER_INDICATOR_SELECTED_COLOR[] = "indicatorSelectedColor";
const char SWIPER_INDICATOR_SIZE[] = "indicatorSize";
const char SWIPER_INDICATOR_SELECTED_SIZE[] = "indicatorSelectedSize";
const char SWIPER_INDICATOR_TOP[] = "indicatorTop";
const char SWIPER_INDICATOR_LEFT[] = "indicatorLeft";
const char SWIPER_INDICATOR_BOTTOM[] = "indicatorBottom";
const char SWIPER_INDICATOR_RIGHT[] = "indicatorRight";

// button constants
const char BUTTON_NAME[] = "Button";
const char BUTTON_WIDTH[] = "width";
const char BUTTON_HEIGHT[] = "height";
const char BUTTON_RECT_RADIUS[] = "rrectRadius";
const char BUTTON_TYPE[] = "type";
const char BUTTON_DISABLED_STATE[] = "disabledState";
const char BUTTON_AUTO_FOCUS[] = "autoFocus";
const char BUTTON_BACKGROUND_COLOR[] = "backgroundColor";
const char BUTTON_CLICKED_COLOR[] = "clickedColor";
const char BUTTON_DISABLED_COLOR[] = "disabledColor";
const char BUTTON_FOCUS_COLOR[] = "focusColor";
const char BUTTON_CLICKED_EVENT_ID[] = "buttonClickedEventId";
const char BUTTON_BORDER[] = "buttonBorder";
const char BUTTON_SHADOW[] = "buttonShadow";

// raw event components.
const char TOUCH_LISTENER_NAME[] = "TouchListener";
const char ON_TOUCH_DOWN_EVENT[] = "onTouchDown";
const char ON_TOUCH_MOVE_EVENT[] = "onTouchMove";
const char ON_TOUCH_UP_EVENT[] = "onTouchUp";
const char ON_TOUCH_CANCEL_EVENT[] = "onTouchCancel";

// gesture event components.
const char GESTURE_LISTENER_NAME[] = "GestureListener";
const char ON_CLICK_EVENT[] = "onClick";
const char ON_LONG_PRESS_EVENT[] = "onLongPress";
const char ON_HORIZONTAL_DRAG_START_EVENT[] = "onHorizontalDragStart";
const char ON_HORIZONTAL_DRAG_UPDATE_EVENT[] = "onHorizontalDragUpdate";
const char ON_HORIZONTAL_DRAG_END_EVENT[] = "onHorizontalDragEnd";
const char ON_HORIZONTAL_DRAG_CANCEL_EVENT[] = "onHorizontalDragCancel";
const char ON_VERTICAL_DRAG_START_EVENT[] = "onVerticalDragStart";
const char ON_VERTICAL_DRAG_UPDATE_EVENT[] = "onVerticalDragUpdate";
const char ON_VERTICAL_DRAG_END_EVENT[] = "onVerticalDragEnd";
const char ON_VERTICAL_DRAG_CANCEL_EVENT[] = "onVerticalDragCancel";
const char ON_FREE_DRAG_START_EVENT[] = "onFreeDragStart";
const char ON_FREE_DRAG_UPDATE_EVENT[] = "onFreeDragUpdate";
const char ON_FREE_DRAG_END_EVENT[] = "onFreeDragEnd";
const char ON_FREE_DRAG_CANCEL_EVENT[] = "onFreeDragCancel";

// focusable components
const char FOCUSABLE_NAME[] = "Focusable";
const char ON_FOCUS_EVENT[] = "onFocus";
const char ON_BLUR_EVENT[] = "onBlur";
const char ON_KEY_EVENT[] = "onKey";
const char BOX_STYLE[] = "style";
const char FOCUSED_BOX_STYLE[] = "focusedStyle";
const char AUTO_FOCUSED[] = "autoFocused";

// texture constants
const char TEXTURE_NAME[] = "Texture";
const char TEXTURE_SRC_ID[] = "id";
const char TEXTURE_SRC_WIDTH[] = "srcwidth";
const char TEXTURE_SRC_HEIGHT[] = "srcheight";
const char TEXTURE_FIT[] = "texturefit";

// display constants
const char DISPLAY_NAME[] = "Display";
const char DISPLAY_VISIBLE[] = "visible";
const char DISPLAY_OPACITY[] = "opacity";

// tab constants
const char TAB_BAR_NAME[] = "Tabbar";
const char TAB_CONTENT_NAME[] = "Tabcontent";
const char TAB_NAME[] = "Tab";
const char TAB_TABS[] = "tabs";
const char TAB_INDEX[] = "index";
const char TAB_IS_VERTICAL[] = "vertical";
const char TAB_CONTROLLER_ID[] = "controllerId";
const char TAB_INDICATOR[] = "indicator";
const char TAB_INDICATOR_WIDTH[] = "indicatorWidth";
const char TAB_CONTENT_ON_CHANGE[] = "onChange";
const char TAB_CONTENT_SCROLLABEL[] = "scrollable";
const char TAB_INDICATOR_COLOR[] = "indicatorColor";
const char TAB_INDICATOR_PADDING[] = "indicatorPadding";
const char TAB_INDICATOR_SIZE[] = "indicatorSize";
const char TAB_LABEL_PADDING[] = "labelPadding";
const char TAB_MODE[] = "mode";
const char TAB_ITEM_TEXT[] = "text";
const char TAB_ITEM_ICON[] = "icon";

// dialog constants
const char DIALOG_NAME[] = "Dialog";
const char DIALOG_TITLE[] = "title";
const char DIALOG_TITLE_PADDING[] = "titlePadding";
const char DIALOG_CONTENT[] = "content";
const char DIALOG_CONTENT_PADDING[] = "contentPadding";
const char DIALOG_BACKGROUND_COLOR[] = "backgroundColor";
const char DIALOG_ACTIONS[] = "actions";
const char DIALOG_AUTO_CANCEL[] = "autoCancel";

// scroll constants
const char SCROLL_NAME[] = "Scroll";
const char SCROLL_DIRECTION[] = "direction";
const char SCROLL_POSITION[] = "scrollPosition";
const char SCROLL_PADDING[] = "scrollPadding";
const char SCROLL_PAGE[] = "scrollPage";

// progress constants
const char PROGRESS_NAME[] = "Progress";
const char PROGRESS_SELECT_COLOR[] = "selectColor";
const char PROGRESS_CACHED_COLOR[] = "cachedColor";
const char PROGRESS_BACKGROUND_COLOR[] = "trackColor";
const char PROGRESS_VALUE[] = "value";
const char PROGRESS_CACHED_VALUE[] = "cachedValue";
const char PROGRESS_MAX[] = "max";
const char PROGRESS_MIN[] = "min";
const char PROGRESS_THICKNESS[] = "thickness";
const char PROGRESS_TYPE[] = "type";

// transform constants
const char TRANSFORM_NAME[] = "Transform";
const char SCALE_VALUE[] = "scaleValue";
const char ANGLE_VALUE[] = "angleValue";
const char ORIGIN[] = "origin";
const char TRANSLATE[] = "translate";
ACE_EXPORT const char SCALE[] = "scale";
const char ROTATE[] = "rotate";
const char PARAM[] = "parameter";
const char PARAM_X[] = "px";
const char PARAM_Y[] = "py";
const char PARAM_Z[] = "pz";
const char TRANSFORM[] = "transform";
const char CENTER[] = "center";

// wrap constants
const char WRAP_NAME[] = "Wrap";
const char DIRECTION[] = "direction";
const char SPACING[] = "space";
const char CONTENT_SPACING[] = "contentSpace";
const char ALIGNMENT[] = "alignment";
const char MAIN_ALIGNMENT[] = "mainAlignment";
const char CROSS_ALIGNMENT[] = "crossAlignment";

// checkable constants
const char CHECKBOX_NAME[] = "Checkbox";
const char SWITCH_NAME[] = "Switch";
const char RADIO_NAME[] = "Radio";
const char CHECKABLE_VALUE[] = "value";
const char RADIO_GROUP_VALUE[] = "groupValue";
const char CHECKABLE_ON_CHANGED_EVENT[] = "onChanged";
const char CHECKABLE_POINT_COLOR[] = "pointColor";
const char CHECKABLE_ACTIVE_COLOR[] = "activeColor";
const char CHECKABLE_INACTIVE_COLOR[] = "inactiveColor";
const char CHECKABLE_FOCUS_COLOR[] = "focusColor";

// popup constants
const char POPUP_NAME[] = "Popup";
const char POPUP_COMPOSED_ID[] = "id";
const char POPUP_COMPOSED_NAME[] = "name";
const char POPUP_PLACEMENT[] = "placement";
const char POPUP_MASK_COLOR[] = "maskColor";
const char POPUP_ON_VISIBILITY_CHANGE[] = "onVisibilityChange";

// marquee constants
const char MARQUEE_NAME[] = "Marquee";
const char MARQUEE_VALUE[] = "value";
const char MARQUEE_DIRECTION[] = "direction";
const char MARQUEE_DIRECTION_LEFT[] = "left";
const char MARQUEE_DIRECTION_RIGHT[] = "right";
const char MARQUEE_SCROLL_AMOUNT[] = "scrollAmount";
const char MARQUEE_LOOP[] = "loop";
const char MARQUEE_BOUNCE[] = "bounce";
const char MARQUEE_FINISH[] = "finish";
const char MARQUEE_START[] = "start";

// rating constants
const char RATING_NAME[] = "Rating";
const char RATING_SCORE[] = "ratingScore";
const char RATING_FOREGROUND_SRC[] = "foregroundSrc";
const char RATING_SECONDARY_SRC[] = "secondarySrc";
const char RATING_BACKGROUND_SRC[] = "backgroundSrc";
const char STEP_SIZE[] = "stepSize";
const char STAR_NUM[] = "starNum";
const char RATING_WIDTH[] = "width";
const char RATING_HEIGHT[] = "height";
const char RATING_HORIZONTAL_PADDING[] = "paddingHorizontal";
const char RATING_VERTICAL_PADDING[] = "paddingVertical";
const char RATING_INDICATOR[] = "isIndicator";
const char RATING_ON_CHANGE[] = "onChange";
const int32_t DEFAULT_RATING_STAR_NUM = 5;
const int32_t DEFAULT_INDICATOR_VALUE = 0;
const int32_t DEFAULT_NO_INDICATOR_VALUE = 0;
const uint32_t RATING_FOCUS_BOARD_COLOR = 0xE6FFFFFF;
const double DEFAULT_RATING_SCORE = 0.0;
const double DEFAULT_RATING_WIDTH = -1.0;
const double DEFAULT_RATING_HEIGHT = -1.0;
const double DEFAULT_RATING_STEP_SIZE = 0.5;
const double DEFAULT_RATING_HORIZONTAL_PADDING = 0.0;
const double DEFAULT_RATING_VERTICAL_PADDING = 0.0;

// loading progress constants
const char LOADING_PROGRESS_NAME[] = "LoadingProgress";
const char DIAMETER[] = "diameter";

// video constants
const char VIDEO_NAME[] = "Video";
const char VIDEO_SRC[] = "src";
const char VIDEO_AUTOPLAY[] = "autoplay";
const char VIDEO_POSTER[] = "poster";
const char VIDEO_CONTROLS[] = "controls";
const char VIDEO_MUTED[] = "muted";
const char VIDEO_FIT[] = "videofit";
const char VIDEO_PREPARED_EVENT_ID[] = "onPreparedId";
const char VIDEO_START_EVENT_ID[] = "onStartId";
const char VIDEO_PAUSE_EVENT_ID[] = "onPauseId";
const char VIDEO_FINISH_EVENT_ID[] = "onFinishId";
const char VIDEO_ERROR_EVENT_ID[] = "onErrorId";
const char VIDEO_SEEKING_EVENT_ID[] = "onSeekingId";
const char VIDEO_SEEKED_EVENT_ID[] = "onSeekedId";
const char VIDEO_TIMEUPDATE_EVENT_ID[] = "onTimeUpdateId";
const char VIDEO_FULLSCREENCHANGE_EVENT_ID[] = "onFullScreenChangeId";

// select constants
const char SELECT_NAME[] = "Select";
const char SELECT_DISABLED[] = "disabled";
const char SELECT_SELECTED_COLOR[] = "selectedColor";
const char SELECT_CLICKED_COLOR[] = "clickedColor";
const char SELECT_DISABLED_COLOR[] = "disabledColor";
const char SELECT_ON_CHANGED[] = "onChanged";
const char SELECT_TIP_TEXT[] = "tipText";
const char SELECT_OPTION_SIZE[] = "optionSize";
const char SELECT_OPTIONS[] = "options";
const char SELECT_OPTION_ICON[] = "icon";
const char SELECT_OPTION_VALUE[] = "value";
const char SELECT_OPTION_TEXT[] = "text";
const char SELECT_OPTION_SELECTED[] = "selected";
const uint32_t SELECT_DEFAULT_OPTION_SIZE = static_cast<uint32_t>(INT32_MAX);
const uint32_t SELECT_INVALID_INDEX = 0xFFFFFFFF;

// clip constants
const char CLIP_NAME[] = "Clip";

// divider constants
const char DIVIDER_NAME[] = "Divider";
const char DIVIDER_STROKE_WIDTH[] = "strokeWidth";
const char DIVIDER_COLOR[] = "color";
const char DIVIDER_VERTICAL[] = "vertical";

// columnSplit constants
const double DEFAULT_SPLIT_HEIGHT = 2.0;

// animator constants
const char START[] = "start";
const char PAUSE[] = "pause";
const char STOP[] = "stop";
const char RESUME[] = "resume";
const char CANCEL[] = "cancel";

} // namespace OHOS::Ace
