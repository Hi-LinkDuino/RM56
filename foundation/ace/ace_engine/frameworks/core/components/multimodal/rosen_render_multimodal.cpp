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
#include "core/components/multimodal/rosen_render_multimodal.h"

#include <cstdint>

#include "flutter/lib/ui/text/font_collection.h"
#include "flutter/third_party/txt/src/txt/paragraph_builder.h"
#include "flutter/third_party/txt/src/txt/paragraph_style.h"

#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkPoint.h"
#include "third_party/skia/include/core/SkRRect.h"

#include "base/i18n/localization.h"
#include "base/utils/string_utils.h"
#include "core/components/calendar/rosen_render_calendar.h"
#include "core/components/font/constants_converter.h"
#include "core/components/font/flutter_font_collection.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {
namespace {
constexpr double DEFAULT_SIZE = 20.0;
constexpr double LIMIT_WIDTH = 18.0;
constexpr double CORNER_RADIUS = 8.0;
constexpr double FONT_SIZE = 14;
const char ELLIPSIS[] = "...";
} // namespace

void RosenRenderMultimodal::Paint(RenderContext& context, const Offset& offset)
{
    RenderNode::Paint(context, offset);

    if (!isSubscriptShow_) {
        return;
    }

    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (canvas == nullptr) {
        LOGE("Paint canvas is null");
        return;
    }

    auto width = NormalizeToPx(Dimension(DEFAULT_SIZE, DimensionUnit::VP));
    auto height = NormalizeToPx(Dimension(DEFAULT_SIZE, DimensionUnit::VP));
    auto corner = NormalizeToPx(Dimension(CORNER_RADIUS, DimensionUnit::VP));

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(Color::FromARGB(230, 0, 0, 0).GetValue());

    UpdateParagraph(offset, subscript_.GetVoiceContent());
    paragraph_->Layout(GetLayoutSize().Width());
    if (paragraph_->GetLongestLine() > NormalizeToPx(Dimension(LIMIT_WIDTH, DimensionUnit::VP))) {
        width = height + (paragraph_->GetLongestLine() - NormalizeToPx(Dimension(LIMIT_WIDTH, DimensionUnit::VP)));
    }

    SkVector radii[] = { { corner, corner }, { 0, 0 }, { corner, corner }, { 0, 0 } };
    SkRRect rrect;
    rrect.setRectRadii(SkRect::MakeXYWH(offset.GetX(), offset.GetY(), width, height), radii);
    canvas->drawRRect(rrect, paint);

    auto leftOffset = paragraph_->GetLongestLine() / 2;
    auto centerX = offset.GetX() + width / 2;
    auto centerY = offset.GetY() + height / 2;
    paragraph_->Paint(canvas, centerX - leftOffset, centerY - paragraph_->GetHeight() / 2);
}

void RosenRenderMultimodal::UpdateParagraph(const Offset& offset, const std::string& text)
{
    using namespace Constants;
    txt::ParagraphStyle style;
    style.max_lines = 1;
    style.ellipsis = StringUtils::Str8ToStr16(ELLIPSIS);
    auto fontCollection = FlutterFontCollection::GetInstance().GetFontCollection();
    if (!fontCollection) {
        LOGW("UpdateParagraph: fontCollection is null");
        return;
    }
    std::unique_ptr<txt::ParagraphBuilder> builder =
        txt::ParagraphBuilder::CreateTxtBuilder(style, fontCollection);
    txt::TextStyle txtStyle;
    txtStyle.font_size = NormalizeToPx(Dimension(FONT_SIZE, DimensionUnit::FP));
    txtStyle.color = Color::FromARGB(255, 255, 255, 255).GetValue();
    txtStyle.locale = Localization::GetInstance()->GetFontLocale();
    builder->PushStyle(txtStyle);
    builder->AddText(StringUtils::Str8ToStr16(text));
    paragraph_ = builder->Build();
}
} // namespace OHOS::Ace