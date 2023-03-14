/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "components/ui_label.h"
#include "font/ui_font.h"
#include "gfx_utils/graphic_log.h"
#include "themes/theme_manager.h"

namespace OHOS {
class LabelAnimator : public Animator, public AnimatorCallback {
public:
    LabelAnimator(uint16_t textX, uint16_t labelX, int16_t startPos, UIView* view)
        : Animator(this, view, 0, true),
          startPos_(startPos),
          textX_(textX),
          labelX_(labelX),
          offsetX_(startPos),
          waitCount_(ANIM_WAIT_COUNT),
          speed_(0),
          preRunTime_(0),
          decimal_(0)
    {
    }

    virtual ~LabelAnimator() {}

    int16_t GetStartPos() const
    {
        return startPos_;
    }

    void SetStartPos(int16_t pos)
    {
        startPos_ = pos;
    }

    void UpdateWidth(uint16_t textWidth, uint16_t labelWidth)
    {
        textX_ = textWidth;
        labelX_ = labelWidth;
        waitCount_ = ANIM_WAIT_COUNT;
        preRunTime_ = 0;
        decimal_ = 0;
        offsetX_ = startPos_;
        static_cast<UILabel*>(view_)->offsetX_ = offsetX_;
        view_->Invalidate();
    }

    void Callback(UIView* view) override
    {
        if (view == nullptr) {
            return;
        }

        uint32_t curTime = GetRunTime();
        if (waitCount_ > 0) {
            waitCount_--;
            preRunTime_ = curTime;
            return;
        }
        if (curTime == preRunTime_) {
            return;
        }
        uint32_t time = (curTime > preRunTime_) ? (curTime - preRunTime_) : (UINT32_MAX - preRunTime_ + curTime);
        // 1000: 1000 milliseconds is 1 second
        float floatStep = (static_cast<float>(time * speed_) / 1000) + decimal_;
        uint16_t integerStep = static_cast<uint16_t>(floatStep);
        decimal_ = floatStep - integerStep;
        preRunTime_ = curTime;

        if (integerStep != 0) {
            offsetX_ -= integerStep;
        } else {
            return;
        }
        offsetX_ = ((offsetX_ - labelX_) % (textX_ + labelX_)) + labelX_;
        static_cast<UILabel*>(view)->offsetX_ = offsetX_;
        view->Invalidate();
    }

    void SetAnimatorSpeed(uint16_t animSpeed)
    {
        speed_ = animSpeed;
        decimal_ = 0;
    }

private:
    static constexpr uint8_t ANIM_WAIT_COUNT = 50;
    int16_t startPos_;
    uint16_t textX_;
    uint16_t labelX_;
    int16_t offsetX_;
    uint16_t waitCount_;
    uint16_t speed_;
    uint32_t preRunTime_;
    float decimal_;
};

UILabel::UILabel()
    : labelText_(nullptr),
      needRefresh_(false),
      useTextColor_(false),
      hasAnimator_(false),
      lineBreakMode_(LINE_BREAK_ELLIPSIS),
      ellipsisIndex_(Text::TEXT_ELLIPSIS_END_INV),
      offsetX_(0),
      textColor_(Color::White()),
      animator_{nullptr}
{
    Theme* theme = ThemeManager::GetInstance().GetCurrent();
    Style& style = (theme != nullptr) ? (theme->GetLabelStyle()) : (StyleDefault::GetLabelStyle());
    UIView::SetStyle(style);
    animator_.speed = DEFAULT_ANIMATOR_SPEED;
}

UILabel::~UILabel()
{
    if (hasAnimator_) {
        delete animator_.animator;
        animator_.animator = nullptr;
        hasAnimator_ = false;
    }
    if (labelText_ != nullptr) {
        delete labelText_;
        labelText_ = nullptr;
    }
}

void UILabel::InitLabelText()
{
    if (labelText_ == nullptr) {
        labelText_ = new Text();
    }
}

int16_t UILabel::GetWidth()
{
    InitLabelText();
    if (needRefresh_ && labelText_->IsExpandWidth()) {
        ReMeasure();
    }
    return UIView::GetWidth();
}

int16_t UILabel::GetHeight()
{
    InitLabelText();
    if (needRefresh_ && labelText_->IsExpandHeight()) {
        ReMeasure();
    }
    return UIView::GetHeight();
}

void UILabel::SetStyle(uint8_t key, int64_t value)
{
    UIView::SetStyle(key, value);
    RefreshLabel();
}

void UILabel::SetText(const char* text)
{
    InitLabelText();
    labelText_->SetText(text);
    if (labelText_->IsNeedRefresh()) {
        RefreshLabel();
    }
}

#if ENABLE_VECTOR_FONT
void UILabel::SetText(const SpannableString* text)
{
    InitLabelText();
    labelText_->SetSpannableString(text);
    if (labelText_->IsNeedRefresh()) {
        RefreshLabel();
    }
}
#endif

void UILabel::SetAbsoluteSizeSpan(int16_t start, int16_t end, uint8_t size)
{
    if (labelText_ == nullptr) {
        return;
    }
    labelText_->SetAbsoluteSizeSpan(start, end, size);
    if (labelText_->IsNeedRefresh()) {
        RefreshLabel();
    }
}

void UILabel::SetRelativeSizeSpan(int16_t start, int16_t end, float size)
{
    if (labelText_ == nullptr) {
        return;
    }
    labelText_->SetRelativeSizeSpan(start, end, size);
    if (labelText_->IsNeedRefresh()) {
        RefreshLabel();
    }
}

void UILabel::SetLineBreakMode(const uint8_t lineBreakMode)
{
    InitLabelText();
    if ((lineBreakMode >= LINE_BREAK_MAX) || (lineBreakMode_ == lineBreakMode)) {
        return;
    }
    lineBreakMode_ = lineBreakMode;
    if ((lineBreakMode_ == LINE_BREAK_ADAPT) || (lineBreakMode_ == LINE_BREAK_STRETCH) ||
        (lineBreakMode_ == LINE_BREAK_MARQUEE)) {
        labelText_->SetExpandWidth(true);
    } else {
        labelText_->SetExpandWidth(false);
    }
    if ((lineBreakMode_ == LINE_BREAK_ADAPT) || (lineBreakMode_ == LINE_BREAK_WRAP)) {
        labelText_->SetExpandHeight(true);
    } else {
        labelText_->SetExpandHeight(false);
    }
    if (lineBreakMode_ != LINE_BREAK_MARQUEE) {
        offsetX_ = 0;
        if (hasAnimator_) {
            animator_.animator->Stop();
        }
    }
    RefreshLabel();
}

void UILabel::SetAlign(UITextLanguageAlignment horizontalAlign, UITextLanguageAlignment verticalAlign)
{
    InitLabelText();
    labelText_->SetAlign(horizontalAlign, verticalAlign);
    if (labelText_->IsNeedRefresh()) {
        RefreshLabel();
    }
}

void UILabel::SetFontId(uint8_t fontId)
{
    InitLabelText();
    labelText_->SetFontId(fontId);
    if (labelText_->IsNeedRefresh()) {
        RefreshLabel();
    }
}

void UILabel::SetFont(const char* name, uint8_t size)
{
    InitLabelText();
    labelText_->SetFont(name, size);
    if (labelText_->IsNeedRefresh()) {
        RefreshLabel();
    }
}

uint16_t UILabel::GetTextWidth()
{
    InitLabelText();
    if (labelText_->IsNeedRefresh()) {
        ReMeasure();
    }
    return labelText_->GetTextSize().x;
}

uint16_t UILabel::GetTextHeight()
{
    InitLabelText();
    if (labelText_->IsNeedRefresh()) {
        ReMeasure();
    }
    return labelText_->GetTextSize().y;
}

void UILabel::SetWidth(int16_t width)
{
    if (GetWidth() != width) {
        UIView::SetWidth(width);
        RefreshLabel();
    }
}

void UILabel::SetHeight(int16_t height)
{
    if (GetHeight() != height) {
        UIView::SetHeight(height);
        RefreshLabel();
    }
}

void UILabel::RefreshLabel()
{
    Invalidate();
    ellipsisIndex_ = Text::TEXT_ELLIPSIS_END_INV;
    if (!needRefresh_) {
        needRefresh_ = true;
    }
}

void UILabel::ReMeasure()
{
    if (!needRefresh_) {
        return;
    }
    needRefresh_ = false;
    InitLabelText();
    Style style = GetStyleConst();
    style.textColor_ = GetTextColor();
    bool flag = false;
    if ((transMap_ != nullptr) && !transMap_->IsInvalid()) {
        transMap_->SetInvalid(true);
        flag = true;
    }
    labelText_->ReMeasureTextSize(GetContentRect(), style);
    Point textSize = labelText_->GetTextSize();
    switch (lineBreakMode_) {
        case LINE_BREAK_ADAPT:
            Resize(textSize.x, textSize.y);
            break;
        case LINE_BREAK_STRETCH:
            SetWidth(textSize.x);
            break;
        case LINE_BREAK_WRAP:
            SetHeight(textSize.y);
            break;
        case LINE_BREAK_ELLIPSIS:
            ellipsisIndex_ = labelText_->GetEllipsisIndex(GetContentRect(), style);
            labelText_->ReMeasureTextWidthInEllipsisMode(GetContentRect(), style, ellipsisIndex_);
            break;
        case LINE_BREAK_MARQUEE:
            RemeasureForMarquee(textSize.x);
            break;
        default:
            break;
    }
    if ((transMap_ != nullptr) && flag) {
        transMap_->SetInvalid(false);
    }
}

void UILabel::RemeasureForMarquee(int16_t textWidth)
{
    int16_t rectWidth = GetWidth();
    if (textWidth > rectWidth) {
        offsetX_ = GetRollStartPos();
        if (labelText_->GetDirect() == TEXT_DIRECT_RTL) {
            labelText_->SetAlign(TEXT_ALIGNMENT_RIGHT);
        } else {
            labelText_->SetAlign(TEXT_ALIGNMENT_LEFT);
        }
        if (hasAnimator_) {
            static_cast<LabelAnimator*>(animator_.animator)->UpdateWidth(textWidth, rectWidth);
        } else {
            LabelAnimator* animator = new LabelAnimator(textWidth, rectWidth, offsetX_, this);
            if (animator == nullptr) {
                GRAPHIC_LOGE("new LabelAnimator fail");
                return;
            }
            animator->SetAnimatorSpeed(animator_.speed);
            animator_.animator = animator;
            hasAnimator_ = true;
        }
        animator_.animator->Start();
    } else {
        offsetX_ = 0;
        if (hasAnimator_) {
            animator_.animator->Stop();
        }
    }
}

void UILabel::SetRollStartPos(int16_t pos)
{
    if (hasAnimator_) {
        static_cast<LabelAnimator*>(animator_.animator)->SetStartPos(pos);
    } else {
        animator_.pos = pos;
    }
}

int16_t UILabel::GetRollStartPos() const
{
    return hasAnimator_ ? static_cast<LabelAnimator*>(animator_.animator)->GetStartPos() : animator_.pos;
}

void UILabel::SetRollSpeed(uint16_t speed)
{
    if (hasAnimator_) {
        static_cast<LabelAnimator*>(animator_.animator)->SetAnimatorSpeed(speed);
    } else {
        animator_.speed = speed;
    }
}

void UILabel::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    InitLabelText();
    UIView::OnDraw(gfxDstBuffer, invalidatedArea);
    Style style = GetStyleConst();
    style.textColor_ = GetTextColor();
    OpacityType opa = GetMixOpaScale();
    labelText_->OnDraw(gfxDstBuffer, invalidatedArea, GetOrigRect(),
                       GetContentRect(), offsetX_, style, ellipsisIndex_, opa);
}
} // namespace OHOS
