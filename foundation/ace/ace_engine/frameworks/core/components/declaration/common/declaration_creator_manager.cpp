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

#include "core/components/declaration/common/declaration_creator_manager.h"

#include "core/components/declaration/badge/badge_declaration.h"
#include "core/components/declaration/button/button_declaration.h"
#include "core/components/declaration/canvas/canvas_declaration.h"
#include "core/components/declaration/clock/clock_declaration.h"
#include "core/components/declaration/div/div_declaration.h"
#include "core/components/declaration/image/image_animator_declaration.h"
#include "core/components/declaration/input/input_declaration.h"
#include "core/components/declaration/piece/piece_declaration.h"
#include "core/components/declaration/qrcode/qrcode_declaration.h"
#include "core/components/declaration/richtext/rich_text_declaration.h"
#include "core/components/declaration/search/search_declaration.h"
#include "core/components/declaration/span/span_declaration.h"
#include "core/components/declaration/svg/svg_animate_declaration.h"
#include "core/components/declaration/svg/svg_base_declaration.h"
#include "core/components/declaration/svg/svg_circle_declaration.h"
#include "core/components/declaration/svg/svg_declaration.h"
#include "core/components/declaration/svg/svg_ellipse_declaration.h"
#include "core/components/declaration/svg/svg_fe_colormatrix_declaration.h"
#include "core/components/declaration/svg/svg_fe_composite_declaration.h"
#include "core/components/declaration/svg/svg_fe_gaussianblur_declaration.h"
#include "core/components/declaration/svg/svg_fe_offset_declaration.h"
#include "core/components/declaration/svg/svg_filter_declaration.h"
#include "core/components/declaration/svg/svg_gradient_declaration.h"
#include "core/components/declaration/svg/svg_line_declaration.h"
#include "core/components/declaration/svg/svg_mask_declaration.h"
#include "core/components/declaration/svg/svg_path_declaration.h"
#include "core/components/declaration/svg/svg_polygon_declaration.h"
#include "core/components/declaration/svg/svg_rect_declaration.h"
#include "core/components/declaration/svg/svg_stop_declaration.h"
#include "core/components/declaration/svg/svg_text_declaration.h"
#include "core/components/declaration/svg/svg_text_path_declaration.h"
#include "core/components/declaration/swiper/swiper_declaration.h"
#include "core/components/declaration/text/text_declaration.h"
#include "core/components/declaration/textarea/textarea_declaration.h"
#include "core/components/declaration/web/web_declaration.h"
#include "core/components/declaration/xcomponent/xcomponent_declaration.h"

namespace OHOS::Ace {

DeclarationCreatorManager::DeclarationCreatorManager() = default;
DeclarationCreatorManager::~DeclarationCreatorManager() = default;

template<class T>
RefPtr<Declaration> DeclarationCreator(const std::string& tag)
{
    return AceType::MakeRefPtr<T>();
}

const RefPtr<Declaration> DeclarationCreatorManager::CreateDeclaration(const std::string& tag)
{
    static const LinearMapNode<std::function<RefPtr<Declaration>(const std::string&)>> declarationCreators[] = {
        { DOM_NODE_TAG_ANIMATE, DeclarationCreator<SvgAnimateDeclaration> },
        { DOM_NODE_TAG_ANIMATE_MOTION, DeclarationCreator<SvgAnimateDeclaration> },
        { DOM_NODE_TAG_ANIMATE_TRANSFORM, DeclarationCreator<SvgAnimateDeclaration> },
        { DOM_NODE_TAG_BADGE, DeclarationCreator<BadgeDeclaration> },
        { DOM_NODE_TAG_BUTTON, DeclarationCreator<ButtonDeclaration> },
        { DOM_NODE_TAG_CANVAS, DeclarationCreator<CanvasDeclaration> },
        { DOM_NODE_TAG_CIRCLE, DeclarationCreator<SvgCircleDeclaration> },
        { DOM_NODE_TAG_CLOCK, DeclarationCreator<ClockDeclaration> },
        { DOM_NODE_TAG_ELLIPSE, DeclarationCreator<SvgEllipseDeclaration> },
        { DOM_NODE_TAG_FE_COLORMATRIX, DeclarationCreator<SvgFeColorMatrixDeclaration> },
        { DOM_NODE_TAG_FE_COMPOSITE, DeclarationCreator<SvgFeCompositeDeclaration> },
        { DOM_NODE_TAG_FE_GAUSSIANBLUR, DeclarationCreator<SvgFeGaussianBlurDeclaration> },
        { DOM_NODE_TAG_FE_OFFSET, DeclarationCreator<SvgFeOffsetDeclaration> },
        { DOM_NODE_TAG_FILTER, DeclarationCreator<SvgFilterDeclaration> },
        { DOM_NODE_TAG_G, DeclarationCreator<SvgDeclaration> },
        { DOM_NODE_TAG_LINEAR_GRADIENT, DeclarationCreator<SvgGradientDeclaration> },
        { DOM_NODE_TAG_IMAGE_ANIMATOR, DeclarationCreator<ImageAnimatorDeclaration> },
        { DOM_NODE_TAG_LABEL, DeclarationCreator<TextDeclaration> },
        { DOM_NODE_TAG_LINE, DeclarationCreator<SvgLineDeclaration> },
        { DOM_NODE_TAG_MASK, DeclarationCreator<SvgMaskDeclaration> },
        { DOM_NODE_TAG_PATH, DeclarationCreator<SvgPathDeclaration> },
        { DOM_NODE_TAG_PIECE, DeclarationCreator<PieceDeclaration> },
        { DOM_NODE_TAG_POLYGON, DeclarationCreator<SvgPolygonDeclaration> },
        { DOM_NODE_TAG_POLYLINE, DeclarationCreator<SvgPolygonDeclaration> },
        { DOM_NODE_TAG_QRCODE, DeclarationCreator<QrcodeDeclaration> },
        { DOM_NODE_TAG_RADIAL_GRADIENT, DeclarationCreator<SvgGradientDeclaration> },
        { DOM_NODE_TAG_RECT, DeclarationCreator<SvgRectDeclaration> },
        { DOM_NODE_TAG_RICH_TEXT, DeclarationCreator<RichTextDeclaration> },
        { DOM_NODE_TAG_SPAN, DeclarationCreator<SpanDeclaration> },
        { DOM_NODE_TAG_STOP, DeclarationCreator<SvgStopDeclaration> },
        { DOM_NODE_TAG_SVG, DeclarationCreator<SvgDeclaration> },
        { DOM_NODE_TAG_SVG_TEXT, DeclarationCreator<SvgTextDeclaration> },
        { DOM_NODE_TAG_SWIPER, DeclarationCreator<SwiperDeclaration> },
        { DOM_NODE_TAG_TEXT, DeclarationCreator<TextDeclaration> },
        { DOM_NODE_TAG_TEXT_PATH, DeclarationCreator<SvgTextPathDeclaration> },
        { DOM_NODE_TAG_TSPAN, DeclarationCreator<SvgTextDeclaration> },
        { DOM_NODE_TAG_USE, DeclarationCreator<SvgDeclaration> },
        { DOM_NODE_TAG_WEB, DeclarationCreator<WebDeclaration> },
        { DOM_NODE_TAG_XCOMPONENT, DeclarationCreator<XComponentDeclaration> },
    };
    int64_t creatorIndex = BinarySearchFindIndex(declarationCreators, ArraySize(declarationCreators), tag.c_str());
    RefPtr<Declaration> declaration = AceType::MakeRefPtr<Declaration>();
    if (creatorIndex >= 0) {
        declaration = declarationCreators[creatorIndex].value(tag);
    }
    declaration->Init();
    return declaration;
}

} // namespace OHOS::Ace
