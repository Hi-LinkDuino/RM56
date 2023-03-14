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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_DECLARATION_CONSTANTS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_DECLARATION_CONSTANTS_H

#include "core/components/declaration/badge/badge_declaration.h"
#include "core/components/declaration/button/button_declaration.h"
#include "core/components/declaration/clock/clock_declaration.h"
#include "core/components/declaration/common/attribute.h"
#include "core/components/declaration/common/event.h"
#include "core/components/declaration/common/method.h"
#include "core/components/declaration/common/style.h"
#include "core/components/declaration/div/div_declaration.h"
#include "core/components/declaration/image/image_animator_declaration.h"
#include "core/components/declaration/input/input_declaration.h"
#include "core/components/declaration/piece/piece_declaration.h"
#include "core/components/declaration/qrcode/qrcode_declaration.h"
#include "core/components/declaration/richtext/rich_text_declaration.h"
#include "core/components/declaration/search/search_declaration.h"
#include "core/components/declaration/side_bar/side_bar_declaration.h"
#include "core/components/declaration/span/span_declaration.h"
#include "core/components/declaration/svg/svg_animate_declaration.h"
#include "core/components/declaration/svg/svg_base_declaration.h"
#include "core/components/declaration/svg/svg_circle_declaration.h"
#include "core/components/declaration/svg/svg_declaration.h"
#include "core/components/declaration/svg/svg_ellipse_declaration.h"
#include "core/components/declaration/svg/svg_filter_declaration.h"
#include "core/components/declaration/svg/svg_fe_colormatrix_declaration.h"
#include "core/components/declaration/svg/svg_fe_composite_declaration.h"
#include "core/components/declaration/svg/svg_fe_flood_declaration.h"
#include "core/components/declaration/svg/svg_fe_func_declaration.h"
#include "core/components/declaration/svg/svg_fe_gaussianblur_declaration.h"
#include "core/components/declaration/svg/svg_fe_merge_declaration.h"
#include "core/components/declaration/svg/svg_fe_merge_node_declaration.h"
#include "core/components/declaration/svg/svg_fe_offset_declaration.h"
#include "core/components/declaration/svg/svg_gradient_declaration.h"
#include "core/components/declaration/svg/svg_line_declaration.h"
#include "core/components/declaration/svg/svg_mask_declaration.h"
#include "core/components/declaration/svg/svg_path_declaration.h"
#include "core/components/declaration/svg/svg_pattern_declaration.h"
#include "core/components/declaration/svg/svg_polygon_declaration.h"
#include "core/components/declaration/svg/svg_rect_declaration.h"
#include "core/components/declaration/svg/svg_stop_declaration.h"
#include "core/components/declaration/svg/svg_text_declaration.h"
#include "core/components/declaration/svg/svg_text_path_declaration.h"
#include "core/components/declaration/swiper/swiper_declaration.h"
#include "core/components/declaration/text/text_declaration.h"
#include "core/components/declaration/textfield/textfield_declaration.h"
#include "core/components/declaration/texttimer/texttimer_declaration.h"
#include "core/components/declaration/web/web_declaration.h"
#include "core/components/declaration/xcomponent/xcomponent_declaration.h"

namespace OHOS::Ace {

class DeclarationConstants {
public:
    // attributes, styles, events and methods supported default.
    static const std::vector<AttributeTag> DEFAULT_ATTRS;
    static const std::vector<StyleTag> DEFAULT_STYLES;
    static const std::vector<EventTag> DEFAULT_EVENTS;
    static const std::vector<MethodTag> DEFAULT_METHODS;

    // default value of common attribute
    static const std::shared_ptr<CommonAttribute> DEFAULT_COMMON_ATTR;
    static const std::shared_ptr<CommonDisabledAttribute> DEFAULT_DISABLED_ATTR;
    static const std::shared_ptr<CommonFocusableAttribute> DEFAULT_FOCUSABLE_ATTR;
    static const std::shared_ptr<CommonTouchableAttribute> DEFAULT_TOUCHABLE_ATTR;
    static const std::shared_ptr<CommonDataAttribute> DEFAULT_DATA_ATTR;
    static const std::shared_ptr<CommonClickEffectAttribute> DEFAULT_CLICK_EFFECT_ATTR;
    static const std::shared_ptr<CommonRenderAttribute> DEFAULT_RENDER_ATTR;
    static const std::shared_ptr<CommonMultimodalAttribute> DEFAULT_MULTI_MODAL_ATTR;

    // default value of common style
    static const std::shared_ptr<CommonStyle> DEFAULT_COMMON_STYLE;
    static const std::shared_ptr<CommonSizeStyle> DEFAULT_SIZE_STYLE;
    static const std::shared_ptr<CommonMarginStyle> DEFAULT_MARGIN_STYLE;
    static const std::shared_ptr<CommonPaddingStyle> DEFAULT_PADDING_STYLE;
    static const std::shared_ptr<CommonBorderStyle> DEFAULT_BORDER_STYLE;
    static const std::shared_ptr<CommonBackgroundStyle> DEFAULT_BACKGROUND_STYLE;
    static const std::shared_ptr<CommonFlexStyle> DEFAULT_FLEX_STYLE;
    static const std::shared_ptr<CommonPositionStyle> DEFAULT_POSITION_STYLE;
    static const std::shared_ptr<CommonOpacityStyle> DEFAULT_OPACITY_STYLE;
    static const std::shared_ptr<CommonVisibilityStyle> DEFAULT_VISIBILITY_STYLE;
    static const std::shared_ptr<CommonDisplayStyle> DEFAULT_DISPLAY_STYLE;
    static const std::shared_ptr<CommonShadowStyle> DEFAULT_SHADOW_STYLE;
    static const std::shared_ptr<CommonOverflowStyle> DEFAULT_OVERFLOW_STYLE;
    static const std::shared_ptr<CommonFilterStyle> DEFAULT_FILTER_STYLE;
    static const std::shared_ptr<CommonAnimationStyle> DEFAULT_ANIMATION_STYLE;
    static const std::shared_ptr<CommonShareTransitionStyle> DEFAULT_SHARE_TRANSITION_STYLE;
    static const std::shared_ptr<CommonCardTransitionStyle> DEFAULT_CARD_TRANSITION_STYLE;
    static const std::shared_ptr<CommonPageTransitionStyle> DEFAULT_PAGE_TRANSITION_STYLE;
    static const std::shared_ptr<CommonClipPathStyle> DEFAULT_CLIP_PATH_STYLE;
    static const std::shared_ptr<CommonMaskStyle> DEFAULT_MASK_STYLE;
    static const std::shared_ptr<CommonImageStyle> DEFAULT_IMAGE_STYLE;

    // default value of common event
    static const std::shared_ptr<CommonRawEvent> DEFAULT_RAW_EVENT;
    static const std::shared_ptr<CommonGestureEvent> DEFAULT_GESTURE_EVENT;
    static const std::shared_ptr<CommonFocusEvent> DEFAULT_FOCUS_EVENT;
    static const std::shared_ptr<CommonKeyEvent> DEFAULT_KEY_EVENT;
    static const std::shared_ptr<CommonMouseEvent> DEFAULT_MOUSE_EVENT;
    static const std::shared_ptr<CommonSwipeEvent> DEFAULT_SWIPE_EVENT;
    static const std::shared_ptr<CommonAttachEvent> DEFAULT_ATTACH_EVENT;
    static const std::shared_ptr<CommonCrownEvent> DEFAULT_CROWN_EVENT;

    // default value of common event
    static const std::shared_ptr<CommonMethod> DEFAULT_METHOD;

    // default value of badge
    static const std::shared_ptr<BadgeAttribute> DEFAULT_BADGE_ATTR;
    static const std::shared_ptr<BadgeStyle> DEFAULT_BADGE_STYLE;
    static const std::shared_ptr<BadgeEvent> DEFAULT_BADGE_EVENT;

    // default value of button
    static const std::shared_ptr<ButtonAttribute> DEFAULT_BUTTON_ATTR;
    static const std::shared_ptr<ButtonStyle> DEFAULT_BUTTON_STYLE;
    static const std::shared_ptr<ButtonEvent> DEFAULT_BUTTON_EVENT;
    static const std::shared_ptr<ButtonMethod> DEFAULT_BUTTON_METHOD;

    // default value of div
    static const std::shared_ptr<DivAttribute> DEFAULT_DIV_ATTR;
    static const std::shared_ptr<DivStyle> DEFAULT_DIV_STYLE;

    // default value of input
    static const std::shared_ptr<InputAttribute> DEFAULT_INPUT_ATTR;

    // default value of piece
    static const std::shared_ptr<PieceAttribute> DEFAULT_PIECE_ATTR;
    static const std::shared_ptr<PieceStyle> DEFAULT_PIECE_STYLE;
    static const std::shared_ptr<PieceEvent> DEFAULT_PIECE_EVENT;

    // default value of qrcode
    static const std::shared_ptr<QrcodeAttribute> DEFAULT_QRCODE_ATTR;
    static const std::shared_ptr<QrcodeStyle> DEFAULT_QRCODE_STYLE;

    // default value of search
    static const std::shared_ptr<SearchAttribute> DEFAULT_SEARCH_ATTR;
    static const std::shared_ptr<SearchStyle> DEFAULT_SEARCH_STYLE;
    static const std::shared_ptr<SearchEvent> DEFAULT_SEARCH_EVENT;
    static const std::shared_ptr<SearchMethod> DEFAULT_SEARCH_METHOD;

    // default value of side bar
    static const std::shared_ptr<SideBarContainerAttribute> DEFAULT_SIDE_BAR_ATTR;
    static const std::shared_ptr<RegionEvent> DEFAULT_REGION_EVENT;

    // default value of span
    static const std::shared_ptr<SpanAttribute> DEFAULT_SPAN_ATTR;
    static const std::shared_ptr<SpanStyle> DEFAULT_SPAN_STYLE;

    // default value of swiper
    static const std::shared_ptr<SwiperAttribute> DEFAULT_SWIPER_ATTR;
    static const std::shared_ptr<SwiperStyle> DEFAULT_SWIPER_STYLE;
    static const std::shared_ptr<SwiperEvent> DEFAULT_SWIPER_EVENT;
    static const std::shared_ptr<SwiperMethod> DEFAULT_SWIPER_METHOD;

    // default value of text
    static const std::shared_ptr<TextSpecializedAttribute> DEFAULT_TEXT_ATTR;
    static const std::shared_ptr<TextSpecializedStyle> DEFAULT_TEXT_STYLE;

    // default value of textfield
    static const std::shared_ptr<TextFieldAttribute> DEFAULT_TEXTFIELD_ATTR;
    static const std::shared_ptr<TextFieldStyle> DEFAULT_TEXTFIELD_STYLE;
    static const std::shared_ptr<TextFieldEvent> DEFAULT_TEXTFIELD_EVENT;
    static const std::shared_ptr<TextFieldMethod> DEFAULT_TEXTFIELD_METHOD;

    // default value of texttimer
    static const std::shared_ptr<TextTimerAttribute> DEFAULT_TEXTTIMER_ATTR;
    static const std::shared_ptr<TextTimerStyle> DEFAULT_TEXTTIMER_STYLE;
    static const std::shared_ptr<TextTimerEvent> DEFAULT_TEXTTIMER_EVENT;
    static const std::shared_ptr<TextTimerMethod> DEFAULT_TEXTTIMER_METHOD;

    // default value of svg
    static const std::shared_ptr<SvgAnimateAttribute> DEFAULT_SVG_ANIMATE_ATTR;
    static const std::shared_ptr<SvgAttribute> DEFAULT_SVG_ATTR;
    static const std::shared_ptr<SvgBaseAttribute> DEFAULT_SVG_BASE_ATTR;
    static const std::shared_ptr<SvgCircleAttribute> DEFAULT_SVG_CIRCLE_ATTR;
    static const std::shared_ptr<SvgEllipseAttribute> DEFAULT_SVG_ELLIPSE_ATTR;
    static const std::shared_ptr<SvgLineAttribute> DEFAULT_SVG_LINE_ATTR;
    static const std::shared_ptr<SvgMaskAttribute> DEFAULT_SVG_MASK_ATTR;
    static const std::shared_ptr<SvgPathAttribute> DEFAULT_SVG_PATH_ATTR;
    static const std::shared_ptr<SvgPatternAttribute> DEFAULT_SVG_PATTERN_ATTR;
    static const std::shared_ptr<SvgPolygonAttribute> DEFAULT_SVG_POLYGON_ATTR;
    static const std::shared_ptr<SvgRectAttribute> DEFAULT_SVG_RECT_ATTR;
    static const std::shared_ptr<SvgTextAttribute> DEFAULT_SVG_TEXT_ATTR;
    static const std::shared_ptr<SvgTextPathAttribute> DEFAULT_SVG_TEXT_PATH_ATTR;
    static const std::shared_ptr<SvgStopAttribute> DEFAULT_SVG_STOP_ATTR;
    static const std::shared_ptr<SvgGradientAttribute> DEFAULT_SVG_GRADIENT_ATTR;
    static const std::shared_ptr<SvgFilterAttribute> DEFAULT_SVG_FILTER_ATTR;
    static const std::shared_ptr<SvgFeColorMatrixAttribute> DEFAULT_SVG_FE_COLORMATRIX_ATTR;
    static const std::shared_ptr<SvgFeAttribute> DEFAULT_SVG_FE_COMPONENT_TRANSFER_ATTR;
    static const std::shared_ptr<SvgFeCompositeAttribute> DEFAULT_SVG_FE_COMPOSITE_ATTR;
    static const std::shared_ptr<SvgFeOffsetAttribute> DEFAULT_SVG_FE_OFFSET_ATTR;
    static const std::shared_ptr<SvgFeFuncAttribute> DEFAULT_SVG_FE_FUNC_ATTR;
    static const std::shared_ptr<SvgFeFloodAttribute> DEFAULT_SVG_FE_FLOOD_ATTR;
    static const std::shared_ptr<SvgFeGaussianBlurAttribute> DEFAULT_SVG_FE_GAUSSIAN_BLUR_ATTR;
    static const std::shared_ptr<SvgFeMergeAttribute> DEFAULT_SVG_FE_MERGE_ATTR;
    static const std::shared_ptr<SvgFeMergeNodeAttribute> DEFAULT_SVG_FE_MERGE_NODE_ATTR;

    // default value of image_animator
    static const std::shared_ptr<ImageAnimatorAttribute> DEFAULT_IMAGE_ANIMATOR_ATTR;

    // default value of web
    static const std::shared_ptr<WebAttribute> DEFAULT_WEB_ATTR;
    static const std::shared_ptr<WebEvent> DEFAULT_WEB_EVENT;
    static const std::shared_ptr<WebMethod> DEFAULT_WEB_METHOD;

    // default value of richtext
    static const std::shared_ptr<RichTextAttribute> DEFAULT_RICH_TEXT_ATTR;
    static const std::shared_ptr<RichTextEvent> DEFAULT_RICH_TEXT_EVENT;

    // default value of clock
    static const std::shared_ptr<ClockAttribute> DEFAULT_CLOCK_ATTR;
    static const std::shared_ptr<ClockStyle> DEFAULT_CLOCK_STYLE;
    static const std::shared_ptr<ClockEvent> DEFAULT_CLOCK_EVENT;

    // default value of xcomponent
    static const std::shared_ptr<XComponentAttribute> DEFAULT_XCOMPONENT_ATTR;
    static const std::shared_ptr<XComponentEvent> DEFAULT_XCOMPONENT_EVENT;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_DECLARATION_CONSTANTS_H
