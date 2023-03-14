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

#include "core/components/declaration/common/declaration_constants.h"

namespace OHOS::Ace {

// attributes, styles, events and methods supported default.
const std::vector<AttributeTag> DeclarationConstants::DEFAULT_ATTRS { AttributeTag::COMMON_ATTR,
    AttributeTag::COMMON_DISABLED_ATTR, AttributeTag::COMMON_FOCUSABLE_ATTR, AttributeTag::COMMON_TOUCHABLE_ATTR,
    AttributeTag::COMMON_DATA_ATTR, AttributeTag::COMMON_CLICK_EFFECT_ATTR, AttributeTag::COMMON_RENDER_ATTR,
    AttributeTag::COMMON_MULTIMODAL_ATTR };
const std::vector<StyleTag> DeclarationConstants::DEFAULT_STYLES { StyleTag::COMMON_STYLE, StyleTag::COMMON_SIZE_STYLE,
    StyleTag::COMMON_MARGIN_STYLE, StyleTag::COMMON_PADDING_STYLE, StyleTag::COMMON_BORDER_STYLE,
    StyleTag::COMMON_BACKGROUND_STYLE, StyleTag::COMMON_FLEX_STYLE, StyleTag::COMMON_POSITION_STYLE,
    StyleTag::COMMON_OPACITY_STYLE, StyleTag::COMMON_VISIBILITY_STYLE, StyleTag::COMMON_DISPLAY_STYLE,
    StyleTag::COMMON_SHADOW_STYLE, StyleTag::COMMON_OVERFLOW_STYLE, StyleTag::COMMON_FILTER_STYLE,
    StyleTag::COMMON_ANIMATION_STYLE, StyleTag::COMMON_SHARE_TRANSITION_STYLE, StyleTag::COMMON_CARD_TRANSITION_STYLE,
    StyleTag::COMMON_PAGE_TRANSITION_STYLE, StyleTag::COMMON_CLIP_PATH_STYLE, StyleTag::COMMON_MASK_STYLE,
    StyleTag::COMMON_IMAGE_STYLE };
const std::vector<EventTag> DeclarationConstants::DEFAULT_EVENTS { EventTag::COMMON_RAW_EVENT,
    EventTag::COMMON_GESTURE_EVENT, EventTag::COMMON_FOCUS_EVENT, EventTag::COMMON_KEY_EVENT,
    EventTag::COMMON_REMOTE_MESSAGE_GRESURE_EVENT,
    EventTag::COMMON_MOUSE_EVENT, EventTag::COMMON_SWIPE_EVENT, EventTag::COMMON_ATTACH_EVENT,
    EventTag::COMMON_CROWN_EVENT };
const std::vector<MethodTag> DeclarationConstants::DEFAULT_METHODS { MethodTag::COMMON_METHOD };

// default common attribute
const std::shared_ptr<CommonAttribute> DeclarationConstants::DEFAULT_COMMON_ATTR = std::make_shared<CommonAttribute>();
const std::shared_ptr<CommonDisabledAttribute> DeclarationConstants::DEFAULT_DISABLED_ATTR =
    std::make_shared<CommonDisabledAttribute>();
const std::shared_ptr<CommonFocusableAttribute> DeclarationConstants::DEFAULT_FOCUSABLE_ATTR =
    std::make_shared<CommonFocusableAttribute>();
const std::shared_ptr<CommonTouchableAttribute> DeclarationConstants::DEFAULT_TOUCHABLE_ATTR =
    std::make_shared<CommonTouchableAttribute>();
const std::shared_ptr<CommonDataAttribute> DeclarationConstants::DEFAULT_DATA_ATTR =
    std::make_shared<CommonDataAttribute>();
const std::shared_ptr<CommonClickEffectAttribute> DeclarationConstants::DEFAULT_CLICK_EFFECT_ATTR =
    std::make_shared<CommonClickEffectAttribute>();
const std::shared_ptr<CommonRenderAttribute> DeclarationConstants::DEFAULT_RENDER_ATTR =
    std::make_shared<CommonRenderAttribute>();
const std::shared_ptr<CommonMultimodalAttribute> DeclarationConstants::DEFAULT_MULTI_MODAL_ATTR =
    std::make_shared<CommonMultimodalAttribute>();

// default common style
const std::shared_ptr<CommonStyle> DeclarationConstants::DEFAULT_COMMON_STYLE = std::make_shared<CommonStyle>();
const std::shared_ptr<CommonSizeStyle> DeclarationConstants::DEFAULT_SIZE_STYLE = std::make_shared<CommonSizeStyle>();
const std::shared_ptr<CommonMarginStyle> DeclarationConstants::DEFAULT_MARGIN_STYLE =
    std::make_shared<CommonMarginStyle>();
const std::shared_ptr<CommonPaddingStyle> DeclarationConstants::DEFAULT_PADDING_STYLE =
    std::make_shared<CommonPaddingStyle>();
const std::shared_ptr<CommonBorderStyle> DeclarationConstants::DEFAULT_BORDER_STYLE =
    std::make_shared<CommonBorderStyle>();
const std::shared_ptr<CommonBackgroundStyle> DeclarationConstants::DEFAULT_BACKGROUND_STYLE =
    std::make_shared<CommonBackgroundStyle>();
const std::shared_ptr<CommonFlexStyle> DeclarationConstants::DEFAULT_FLEX_STYLE = std::make_shared<CommonFlexStyle>();
const std::shared_ptr<CommonPositionStyle> DeclarationConstants::DEFAULT_POSITION_STYLE =
    std::make_shared<CommonPositionStyle>();
const std::shared_ptr<CommonOpacityStyle> DeclarationConstants::DEFAULT_OPACITY_STYLE =
    std::make_shared<CommonOpacityStyle>();
const std::shared_ptr<CommonVisibilityStyle> DeclarationConstants::DEFAULT_VISIBILITY_STYLE =
    std::make_shared<CommonVisibilityStyle>();
const std::shared_ptr<CommonDisplayStyle> DeclarationConstants::DEFAULT_DISPLAY_STYLE =
    std::make_shared<CommonDisplayStyle>();
const std::shared_ptr<CommonShadowStyle> DeclarationConstants::DEFAULT_SHADOW_STYLE =
    std::make_shared<CommonShadowStyle>();
const std::shared_ptr<CommonOverflowStyle> DeclarationConstants::DEFAULT_OVERFLOW_STYLE =
    std::make_shared<CommonOverflowStyle>();
const std::shared_ptr<CommonFilterStyle> DeclarationConstants::DEFAULT_FILTER_STYLE =
    std::make_shared<CommonFilterStyle>();
const std::shared_ptr<CommonAnimationStyle> DeclarationConstants::DEFAULT_ANIMATION_STYLE =
    std::make_shared<CommonAnimationStyle>();
const std::shared_ptr<CommonShareTransitionStyle> DeclarationConstants::DEFAULT_SHARE_TRANSITION_STYLE =
    std::make_shared<CommonShareTransitionStyle>();
const std::shared_ptr<CommonCardTransitionStyle> DeclarationConstants::DEFAULT_CARD_TRANSITION_STYLE =
    std::make_shared<CommonCardTransitionStyle>();
const std::shared_ptr<CommonPageTransitionStyle> DeclarationConstants::DEFAULT_PAGE_TRANSITION_STYLE =
    std::make_shared<CommonPageTransitionStyle>();
const std::shared_ptr<CommonClipPathStyle> DeclarationConstants::DEFAULT_CLIP_PATH_STYLE =
    std::make_shared<CommonClipPathStyle>();
const std::shared_ptr<CommonMaskStyle> DeclarationConstants::DEFAULT_MASK_STYLE =
    std::make_shared<CommonMaskStyle>();
const std::shared_ptr<CommonImageStyle> DeclarationConstants::DEFAULT_IMAGE_STYLE =
    std::make_shared<CommonImageStyle>();

// default common event
const std::shared_ptr<CommonRawEvent> DeclarationConstants::DEFAULT_RAW_EVENT = std::make_shared<CommonRawEvent>();
const std::shared_ptr<CommonGestureEvent> DeclarationConstants::DEFAULT_GESTURE_EVENT =
    std::make_shared<CommonGestureEvent>();
const std::shared_ptr<CommonFocusEvent> DeclarationConstants::DEFAULT_FOCUS_EVENT =
    std::make_shared<CommonFocusEvent>();
const std::shared_ptr<CommonKeyEvent> DeclarationConstants::DEFAULT_KEY_EVENT = std::make_shared<CommonKeyEvent>();
const std::shared_ptr<CommonMouseEvent> DeclarationConstants::DEFAULT_MOUSE_EVENT =
    std::make_shared<CommonMouseEvent>();
const std::shared_ptr<CommonSwipeEvent> DeclarationConstants::DEFAULT_SWIPE_EVENT =
    std::make_shared<CommonSwipeEvent>();
const std::shared_ptr<CommonAttachEvent> DeclarationConstants::DEFAULT_ATTACH_EVENT =
    std::make_shared<CommonAttachEvent>();
const std::shared_ptr<CommonCrownEvent> DeclarationConstants::DEFAULT_CROWN_EVENT =
    std::make_shared<CommonCrownEvent>();

// default common method
const std::shared_ptr<CommonMethod> DeclarationConstants::DEFAULT_METHOD = std::make_shared<CommonMethod>();

// default value of badge
const std::shared_ptr<BadgeAttribute> DeclarationConstants::DEFAULT_BADGE_ATTR = std::make_shared<BadgeAttribute>();
const std::shared_ptr<BadgeStyle> DeclarationConstants::DEFAULT_BADGE_STYLE = std::make_shared<BadgeStyle>();
const std::shared_ptr<BadgeEvent> DeclarationConstants::DEFAULT_BADGE_EVENT = std::make_shared<BadgeEvent>();

// default value of button
const std::shared_ptr<ButtonAttribute> DeclarationConstants::DEFAULT_BUTTON_ATTR = std::make_shared<ButtonAttribute>();
const std::shared_ptr<ButtonStyle> DeclarationConstants::DEFAULT_BUTTON_STYLE = std::make_shared<ButtonStyle>();
const std::shared_ptr<ButtonEvent> DeclarationConstants::DEFAULT_BUTTON_EVENT = std::make_shared<ButtonEvent>();
const std::shared_ptr<ButtonMethod> DeclarationConstants::DEFAULT_BUTTON_METHOD = std::make_shared<ButtonMethod>();

// default value of div
const std::shared_ptr<DivAttribute> DeclarationConstants::DEFAULT_DIV_ATTR = std::make_shared<DivAttribute>();
const std::shared_ptr<DivStyle> DeclarationConstants::DEFAULT_DIV_STYLE = std::make_shared<DivStyle>();

// default value of input
const std::shared_ptr<InputAttribute> DeclarationConstants::DEFAULT_INPUT_ATTR = std::make_shared<InputAttribute>();

// default value of piece
const std::shared_ptr<PieceAttribute> DeclarationConstants::DEFAULT_PIECE_ATTR = std::make_shared<PieceAttribute>();
const std::shared_ptr<PieceStyle> DeclarationConstants::DEFAULT_PIECE_STYLE = std::make_shared<PieceStyle>();
const std::shared_ptr<PieceEvent> DeclarationConstants::DEFAULT_PIECE_EVENT = std::make_shared<PieceEvent>();

// default value of qrcode
const std::shared_ptr<QrcodeAttribute> DeclarationConstants::DEFAULT_QRCODE_ATTR = std::make_shared<QrcodeAttribute>();
const std::shared_ptr<QrcodeStyle> DeclarationConstants::DEFAULT_QRCODE_STYLE = std::make_shared<QrcodeStyle>();

// default value of search
const std::shared_ptr<SearchAttribute> DeclarationConstants::DEFAULT_SEARCH_ATTR = std::make_shared<SearchAttribute>();
const std::shared_ptr<SearchStyle> DeclarationConstants::DEFAULT_SEARCH_STYLE = std::make_shared<SearchStyle>();
const std::shared_ptr<SearchEvent> DeclarationConstants::DEFAULT_SEARCH_EVENT = std::make_shared<SearchEvent>();
const std::shared_ptr<SearchMethod> DeclarationConstants::DEFAULT_SEARCH_METHOD = std::make_shared<SearchMethod>();

// default value of side bar
const std::shared_ptr<SideBarContainerAttribute> DeclarationConstants::DEFAULT_SIDE_BAR_ATTR =
    std::make_shared<SideBarContainerAttribute>();
const std::shared_ptr<RegionEvent> DeclarationConstants::DEFAULT_REGION_EVENT = std::make_shared<RegionEvent>();

// default value of span
const std::shared_ptr<SpanAttribute> DeclarationConstants::DEFAULT_SPAN_ATTR = std::make_shared<SpanAttribute>();
const std::shared_ptr<SpanStyle> DeclarationConstants::DEFAULT_SPAN_STYLE = std::make_shared<SpanStyle>();

// default value of swiper
const std::shared_ptr<SwiperAttribute> DeclarationConstants::DEFAULT_SWIPER_ATTR = std::make_shared<SwiperAttribute>();
const std::shared_ptr<SwiperStyle> DeclarationConstants::DEFAULT_SWIPER_STYLE = std::make_shared<SwiperStyle>();
const std::shared_ptr<SwiperEvent> DeclarationConstants::DEFAULT_SWIPER_EVENT = std::make_shared<SwiperEvent>();
const std::shared_ptr<SwiperMethod> DeclarationConstants::DEFAULT_SWIPER_METHOD = std::make_shared<SwiperMethod>();

// default value of text
const std::shared_ptr<TextSpecializedAttribute> DeclarationConstants::DEFAULT_TEXT_ATTR =
    std::make_shared<TextSpecializedAttribute>();
const std::shared_ptr<TextSpecializedStyle> DeclarationConstants::DEFAULT_TEXT_STYLE =
    std::make_shared<TextSpecializedStyle>();

// default value of textfield
const std::shared_ptr<TextFieldAttribute> DeclarationConstants::DEFAULT_TEXTFIELD_ATTR =
    std::make_shared<TextFieldAttribute>();
const std::shared_ptr<TextFieldStyle> DeclarationConstants::DEFAULT_TEXTFIELD_STYLE =
    std::make_shared<TextFieldStyle>();
const std::shared_ptr<TextFieldEvent> DeclarationConstants::DEFAULT_TEXTFIELD_EVENT =
    std::make_shared<TextFieldEvent>();
const std::shared_ptr<TextFieldMethod> DeclarationConstants::DEFAULT_TEXTFIELD_METHOD =
    std::make_shared<TextFieldMethod>();

// default value of texttimer
const std::shared_ptr<TextTimerAttribute> DeclarationConstants::DEFAULT_TEXTTIMER_ATTR =
    std::make_shared<TextTimerAttribute>();
const std::shared_ptr<TextTimerStyle> DeclarationConstants::DEFAULT_TEXTTIMER_STYLE =
    std::make_shared<TextTimerStyle>();
const std::shared_ptr<TextTimerEvent> DeclarationConstants::DEFAULT_TEXTTIMER_EVENT =
    std::make_shared<TextTimerEvent>();
const std::shared_ptr<TextTimerMethod> DeclarationConstants::DEFAULT_TEXTTIMER_METHOD =
    std::make_shared<TextTimerMethod>();

// default value of svg
const std::shared_ptr<SvgAnimateAttribute> DeclarationConstants::DEFAULT_SVG_ANIMATE_ATTR =
    std::make_shared<SvgAnimateAttribute>();
const std::shared_ptr<SvgAttribute> DeclarationConstants::DEFAULT_SVG_ATTR = std::make_shared<SvgAttribute>();
const std::shared_ptr<SvgBaseAttribute> DeclarationConstants::DEFAULT_SVG_BASE_ATTR =
    std::make_shared<SvgBaseAttribute>();
const std::shared_ptr<SvgCircleAttribute> DeclarationConstants::DEFAULT_SVG_CIRCLE_ATTR =
    std::make_shared<SvgCircleAttribute>();
const std::shared_ptr<SvgEllipseAttribute> DeclarationConstants::DEFAULT_SVG_ELLIPSE_ATTR =
    std::make_shared<SvgEllipseAttribute>();
const std::shared_ptr<SvgLineAttribute> DeclarationConstants::DEFAULT_SVG_LINE_ATTR =
    std::make_shared<SvgLineAttribute>();
const std::shared_ptr<SvgMaskAttribute> DeclarationConstants::DEFAULT_SVG_MASK_ATTR =
    std::make_shared<SvgMaskAttribute>();
const std::shared_ptr<SvgPathAttribute> DeclarationConstants::DEFAULT_SVG_PATH_ATTR =
    std::make_shared<SvgPathAttribute>();
const std::shared_ptr<SvgPatternAttribute> DeclarationConstants::DEFAULT_SVG_PATTERN_ATTR =
    std::make_shared<SvgPatternAttribute>();
const std::shared_ptr<SvgPolygonAttribute> DeclarationConstants::DEFAULT_SVG_POLYGON_ATTR =
    std::make_shared<SvgPolygonAttribute>();
const std::shared_ptr<SvgRectAttribute> DeclarationConstants::DEFAULT_SVG_RECT_ATTR =
    std::make_shared<SvgRectAttribute>();
const std::shared_ptr<SvgTextAttribute> DeclarationConstants::DEFAULT_SVG_TEXT_ATTR =
    std::make_shared<SvgTextAttribute>();
const std::shared_ptr<SvgTextPathAttribute> DeclarationConstants::DEFAULT_SVG_TEXT_PATH_ATTR =
    std::make_shared<SvgTextPathAttribute>();
const std::shared_ptr<SvgStopAttribute> DeclarationConstants::DEFAULT_SVG_STOP_ATTR =
    std::make_shared<SvgStopAttribute>();
const std::shared_ptr<SvgGradientAttribute> DeclarationConstants::DEFAULT_SVG_GRADIENT_ATTR =
    std::make_shared<SvgGradientAttribute>();
const std::shared_ptr<SvgFilterAttribute> DeclarationConstants::DEFAULT_SVG_FILTER_ATTR =
    std::make_shared<SvgFilterAttribute>();
const std::shared_ptr<SvgFeColorMatrixAttribute> DeclarationConstants::DEFAULT_SVG_FE_COLORMATRIX_ATTR =
    std::make_shared<SvgFeColorMatrixAttribute>();
const std::shared_ptr<SvgFeAttribute> DeclarationConstants::DEFAULT_SVG_FE_COMPONENT_TRANSFER_ATTR =
    std::make_shared<SvgFeAttribute>();
const std::shared_ptr<SvgFeCompositeAttribute> DeclarationConstants::DEFAULT_SVG_FE_COMPOSITE_ATTR =
    std::make_shared<SvgFeCompositeAttribute>();
const std::shared_ptr<SvgFeOffsetAttribute> DeclarationConstants::DEFAULT_SVG_FE_OFFSET_ATTR =
    std::make_shared<SvgFeOffsetAttribute>();
const std::shared_ptr<SvgFeFloodAttribute> DeclarationConstants::DEFAULT_SVG_FE_FLOOD_ATTR =
    std::make_shared<SvgFeFloodAttribute>();
const std::shared_ptr<SvgFeFuncAttribute> DeclarationConstants::DEFAULT_SVG_FE_FUNC_ATTR =
    std::make_shared<SvgFeFuncAttribute>();
const std::shared_ptr<SvgFeGaussianBlurAttribute> DeclarationConstants::DEFAULT_SVG_FE_GAUSSIAN_BLUR_ATTR =
    std::make_shared<SvgFeGaussianBlurAttribute>();
const std::shared_ptr<SvgFeMergeAttribute> DeclarationConstants::DEFAULT_SVG_FE_MERGE_ATTR =
    std::make_shared<SvgFeMergeAttribute>();
const std::shared_ptr<SvgFeMergeNodeAttribute> DeclarationConstants::DEFAULT_SVG_FE_MERGE_NODE_ATTR =
    std::make_shared<SvgFeMergeNodeAttribute>();

// default value of image_animator
const std::shared_ptr<ImageAnimatorAttribute> DeclarationConstants::DEFAULT_IMAGE_ANIMATOR_ATTR =
    std::make_shared<ImageAnimatorAttribute>();

// default value of web
const std::shared_ptr<WebAttribute> DeclarationConstants::DEFAULT_WEB_ATTR = std::make_shared<WebAttribute>();
const std::shared_ptr<WebEvent> DeclarationConstants::DEFAULT_WEB_EVENT = std::make_shared<WebEvent>();
const std::shared_ptr<WebMethod> DeclarationConstants::DEFAULT_WEB_METHOD = std::make_shared<WebMethod>();

// default value of richtext
const std::shared_ptr<RichTextAttribute> DeclarationConstants::DEFAULT_RICH_TEXT_ATTR =
    std::make_shared<RichTextAttribute>();
const std::shared_ptr<RichTextEvent> DeclarationConstants::DEFAULT_RICH_TEXT_EVENT =
    std::make_shared<RichTextEvent>();

// default value of clock
const std::shared_ptr<ClockAttribute> DeclarationConstants::DEFAULT_CLOCK_ATTR = std::make_shared<ClockAttribute>();
const std::shared_ptr<ClockStyle> DeclarationConstants::DEFAULT_CLOCK_STYLE = std::make_shared<ClockStyle>();
const std::shared_ptr<ClockEvent> DeclarationConstants::DEFAULT_CLOCK_EVENT = std::make_shared<ClockEvent>();

// default value of xcomponent
const std::shared_ptr<XComponentAttribute> DeclarationConstants::DEFAULT_XCOMPONENT_ATTR =
    std::make_shared<XComponentAttribute>();
const std::shared_ptr<XComponentEvent> DeclarationConstants::DEFAULT_XCOMPONENT_EVENT =
    std::make_shared<XComponentEvent>();
} // namespace OHOS::Ace
