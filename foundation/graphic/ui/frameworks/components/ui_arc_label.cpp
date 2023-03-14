/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "components/ui_arc_label.h"

#include "common/typed_text.h"
#include "draw/draw_label.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "font/ui_font.h"
#include "themes/theme_manager.h"

namespace OHOS {
UIArcLabel::UIArcLabel()
    : arcLabelText_(nullptr),
      needRefresh_(false),
      textSize_({0, 0}),
      radius_(0),
      startAngle_(0),
      endAngle_(0),
      arcCenter_({0, 0}),
      orientation_(TextOrientation::INSIDE),
      arcTextInfo_{0}
{
    Theme* theme = ThemeManager::GetInstance().GetCurrent();
    style_ = (theme != nullptr) ? &(theme->GetLabelStyle()) : &(StyleDefault::GetLabelStyle());
}

UIArcLabel::~UIArcLabel()
{
    if (arcLabelText_ != nullptr) {
        delete arcLabelText_;
        arcLabelText_ = nullptr;
    }
}

void UIArcLabel::SetStyle(uint8_t key, int64_t value)
{
    UIView::SetStyle(key, value);
    RefreshArcLabel();
}

void UIArcLabel::SetText(const char* text)
{
    if (text == nullptr) {
        return;
    }
    InitArcLabelText();
    arcLabelText_->SetText(text);
    if (arcLabelText_->IsNeedRefresh()) {
        RefreshArcLabel();
    }
}

const char* UIArcLabel::GetText() const
{
    return (arcLabelText_ == nullptr) ? nullptr : arcLabelText_->GetText();
}

void UIArcLabel::SetAlign(UITextLanguageAlignment horizontalAlign)
{
    InitArcLabelText();
    arcLabelText_->SetAlign(horizontalAlign, TEXT_ALIGNMENT_TOP);
    if (arcLabelText_->IsNeedRefresh()) {
        RefreshArcLabel();
    }
}

UITextLanguageAlignment UIArcLabel::GetHorAlign()
{
    InitArcLabelText();
    return arcLabelText_->GetHorAlign();
}

UITextLanguageDirect UIArcLabel::GetDirect()
{
    InitArcLabelText();
    return arcLabelText_->GetDirect();
}

void UIArcLabel::SetFontId(uint8_t fontId)
{
    InitArcLabelText();
    arcLabelText_->SetFontId(fontId);
    if (arcLabelText_->IsNeedRefresh()) {
        RefreshArcLabel();
    }
}

uint8_t UIArcLabel::GetFontId()
{
    InitArcLabelText();
    return arcLabelText_->GetFontId();
}

void UIArcLabel::SetFont(const char* name, uint8_t size)
{
    if (name == nullptr) {
        return;
    }
    InitArcLabelText();
    arcLabelText_->SetFont(name, size);
    if (arcLabelText_->IsNeedRefresh()) {
        RefreshArcLabel();
    }
}

void UIArcLabel::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    InitArcLabelText();
    const char* text = arcLabelText_->GetText();
    if ((text == nullptr) || (radius_ == 0)) {
        return;
    }
    Rect trunc = invalidatedArea;
    OpacityType opa = GetMixOpaScale();
    UIView::OnDraw(gfxDstBuffer, invalidatedArea);
    DrawArcText(gfxDstBuffer, trunc, opa);
}

void UIArcLabel::DrawArcText(BufferInfo& gfxDstBuffer, const Rect& mask, OpacityType opaScale)
{
    Point center;
    center.x = arcTextInfo_.arcCenter.x + GetRect().GetX();
    center.y = arcTextInfo_.arcCenter.y + GetRect().GetY();
    InitArcLabelText();
    DrawLabel::DrawArcText(gfxDstBuffer, mask, arcLabelText_->GetText(), center, arcLabelText_->GetFontId(),
                           arcLabelText_->GetFontSize(), arcTextInfo_, orientation_, *style_, opaScale);
}

void UIArcLabel::RefreshArcLabel()
{
    Invalidate();
    if (!needRefresh_) {
        needRefresh_ = true;
    }
}

void UIArcLabel::ReMeasure()
{
    if (!needRefresh_) {
        return;
    }
    needRefresh_ = false;
    InitArcLabelText();

    MeasureArcTextInfo();
    Rect textRect =
        TypedText::GetArcTextRect(arcLabelText_->GetText(), arcLabelText_->GetFontId(), arcLabelText_->GetFontSize(),
                                  arcCenter_, style_->letterSpace_, orientation_, arcTextInfo_);
    int16_t arcTextWidth = textRect.GetWidth();
    int16_t arcTextHeight = textRect.GetHeight();

    SetPosition(textRect.GetX(), textRect.GetY());
    Resize(arcTextWidth, arcTextHeight);
    arcTextInfo_.arcCenter.x = arcCenter_.x - GetX() + style_->borderWidth_ + style_->paddingLeft_;
    arcTextInfo_.arcCenter.y = arcCenter_.y - GetY() + style_->borderWidth_ + style_->paddingTop_;
    textSize_.x = arcTextWidth;
    textSize_.y = arcTextHeight;
    Invalidate();
}

void UIArcLabel::MeasureArcTextInfo()
{
    const char* text = arcLabelText_->GetText();
    if (text == nullptr) {
        return;
    }
    uint16_t letterHeight = UIFont::GetInstance()->GetHeight(arcLabelText_->GetFontId(), arcLabelText_->GetFontSize());
    arcTextInfo_.radius = ((orientation_ == TextOrientation::INSIDE) ? radius_ : (radius_ - letterHeight));
    if (arcTextInfo_.radius == 0) {
        return;
    }

    uint16_t arcAngle;
    if (startAngle_ < endAngle_) {
        arcAngle = endAngle_ - startAngle_;
        arcTextInfo_.direct = TEXT_DIRECT_LTR; // Clockwise
        arcLabelText_->SetDirect(TEXT_DIRECT_LTR);
    } else {
        arcAngle = startAngle_ - endAngle_;
        arcTextInfo_.direct = TEXT_DIRECT_RTL; // Counterclockwise
        arcLabelText_->SetDirect(TEXT_DIRECT_RTL);
    }
    // calculate max arc length
    float maxLength = static_cast<float>((UI_PI * radius_ * arcAngle) / SEMICIRCLE_IN_DEGREE);
    arcTextInfo_.lineStart = 0;
    arcTextInfo_.lineEnd = TypedText::GetNextLine(&text[arcTextInfo_.lineStart], arcLabelText_->GetFontId(),
                                                  arcLabelText_->GetFontSize(), style_->letterSpace_,
                                                  static_cast<int16_t>(maxLength));
    arcTextInfo_.startAngle = startAngle_ % CIRCLE_IN_DEGREE;
    int16_t actLength =
        TypedText::GetTextWidth(&text[arcTextInfo_.lineStart], arcLabelText_->GetFontId(), arcLabelText_->GetFontSize(),
                                arcTextInfo_.lineEnd - arcTextInfo_.lineStart, style_->letterSpace_);
    if ((arcLabelText_->GetHorAlign() != TEXT_ALIGNMENT_LEFT) && (actLength < maxLength)) {
        float gapLength = maxLength - actLength;
        if (arcLabelText_->GetHorAlign() == TEXT_ALIGNMENT_CENTER) {
            gapLength = gapLength / 2; // 2: half
        }
        arcTextInfo_.startAngle += TypedText::GetAngleForArcLen(gapLength, letterHeight, arcTextInfo_.radius,
                                                                arcTextInfo_.direct, orientation_);
    }
}
} // namespace OHOS
