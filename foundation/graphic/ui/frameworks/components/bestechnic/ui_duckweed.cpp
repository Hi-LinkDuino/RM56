/**
 * @file ui_duckweed.cpp
 * @author yongxianglai@bestechnic.com
 * @date 2023-02-23
 * @brief duckweed 消息实现
 * @since 1.0
 * @version 1.0
 */

#if (BES_FEATURE_DUCKWEED == 1)
#include "components/bestechnic/ui_duckweed.h"

#include "draw/draw_rect.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/graphic_types.h"
#include "font/ui_font.h"
#include "securec.h"
#include "draw/draw_utils.h"

namespace OHOS {

//OHOS::UIDuckweed public

UIDuckweed::UIDuckweed(): opacity_(OPA_OPAQUE)
{
    backgroundStyle_ = new Style();
    foregroundStyle_ = new Style();
}

UIDuckweed::~UIDuckweed()
{
    SetContent(nullptr);

    if (uiText_ != nullptr) {
        delete uiText_;
        uiText_ = nullptr;
    }

    if (backgroundStyle_ != nullptr) {
        delete backgroundStyle_;
        backgroundStyle_ = nullptr;
    }

    if (foregroundStyle_ != nullptr) {
        delete foregroundStyle_;
        foregroundStyle_ = nullptr;
    }
}

const Rect& UIDuckweed::GetRect()
{
    return rect_;
}

const DuckweedOption& UIDuckweed::GetOption()
{
    return option_;
}

void UIDuckweed::SetContent(const SpannableString* content)
{
    if (content_ != nullptr) {
        delete content_;
        content_ = nullptr;
    }
    if (content == nullptr) return;

    uint16_t len = static_cast<uint16_t>(strlen(content->text_) + 1); //length + 1，see ui_qrcode.cpp
    char* text = static_cast<char*>(UIMalloc(len));
    if (text != nullptr) {
        if (memcpy_s(text, len, content->text_, len) != 0) {
            SetContent(nullptr);
        }
    }

    content_ = new SpannableString(text);

    List<StyleSpan*> srcList = content->spanList_;

    if (srcList.Size() <= 0) return;

    ListNode<StyleSpan*>* pos = srcList.Head();
    while (pos != nullptr) {
        (content_->spanList_).PushBack(pos->data_);

        if (pos == srcList.Tail()) break;

        pos = pos->next_;
    }
}

void UIDuckweed::SetOption(const DuckweedOption& option)
{
    option_.Clone(option);
}

void UIDuckweed::SetOpacity(uint8_t opacity)
{
    opacity_ = opacity;
}

void UIDuckweed::Evaluate()
{
    InitUIText();

    if (uiText_ == nullptr || content_ == nullptr || content_->text_ == nullptr) return;

#if ENABLE_VECTOR_FONT
    uiText_->SetSpannableString(content_);
#else
    uiText_->SetText(content_->text_);
#endif //ENABLE_VECTOR_FONT

    uiText_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, option_.textSize);
    Rect rect;
    rect.SetRect(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    CalculatorRect(rect);
}

void UIDuckweed::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    if (content_ == nullptr) return;
    if (uiText_ == nullptr) return;

    Rect rect;
    if (!rect.Intersect(invalidatedArea, rect_)) return;

    DrawBackground(gfxDstBuffer, invalidatedArea, opacity_, rect);
    DrawForeground(gfxDstBuffer, invalidatedArea, opacity_, rect);
}

//OHOS::UIDuckweed private

void UIDuckweed::InitUIText()
{
    if (uiText_ != nullptr) return;

    uiText_ = new Text();
    if (uiText_ != nullptr) {
        uiText_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    }
}

void UIDuckweed::MatchHeightForContent(const uint16_t& width, uint16_t& height, 
                        uint16_t textWidth, const uint16_t& textHeight)
{
    while (textWidth > width) {
        height += textHeight;
        textWidth -= textHeight;
        if (height >= Screen::GetInstance().GetHeight() * 0.8f) return; // 此时屏幕已经显示不下，继续增加高度已经没有意义
    }
}

void UIDuckweed::CalculatorRect(Rect& rect)
{
    if (uiText_ == nullptr) return;

    uiText_->ReMeasureTextSize(rect, *foregroundStyle_);
    Point textWH = uiText_->GetTextSize();

    Screen &screen = Screen::GetInstance();
    uint16_t scrW = screen.GetWidth();
    uint16_t scrH = screen.GetHeight();

    //由消息内容决定宽度
    uint16_t w = textWH.x + (option_.padding << 1); //左右都需要有间距
    // 检查内容高度，决定消息高度
    //有可能宽度不足以容纳全部内容，需要换行
    uint16_t h = MATH_MIN(scrH * 0.8f + 0.5f, static_cast<float>(textWH.y + (option_.padding << 1))); //上下都需要有间距

    uint16_t minTop = scrW * 0.05f + 0.5f;
    uint16_t left, top, right, bottom;
    if (screen.GetScreenShape() == ScreenShape::CIRCLE) {
        //圆形屏幕因为不同位置上可显示的区域不同，精确地计算任意位置显示区域会比较麻烦
        //TOP：    top=4%，left>=25%，width<=50%
        //CENTER：top>=4%，left>=15%，width<=70%
        //BOTTOM：top=96%，left>=25%，width<=50%
        uint16_t halfScrW = (scrW >> 1) + 0.5f;
        uint16_t halfHalfScrW = (scrW >> 2) + 0.5f;
        switch (option_.side) {
            case TOP:
                w = MATH_MIN(halfScrW, w);
                MatchHeightForContent(w, h, textWH.x, textWH.y);
                left = MATH_MAX(halfHalfScrW, (scrW - w) >> 1);
                top = scrH * 0.05f + 0.5f;
                right = left + w;
                break;
            case BOTTOM:
                w = MATH_MIN(halfScrW, w);
                MatchHeightForContent(w, h, textWH.x, textWH.y);
                left = MATH_MAX(halfHalfScrW, (scrW - w) >> 1);
                top = MATH_MAX(minTop, static_cast<uint16_t>(scrH * 0.95f + 0.5f - h));
                right = MATH_MIN(halfHalfScrW * 3, left + w);
                break;
            case CENTER:
            default:
                w = MATH_MIN(static_cast<uint16_t>(scrW * 0.7f + 0.5f), w);
                MatchHeightForContent(w, h, textWH.x, textWH.y);
                left = MATH_MAX(scrW * 0.15f, ((scrW - w) >> 1) + 0.5f);
                top = MATH_MAX(minTop, (scrH - h) >> 1);
                right = MATH_MIN(scrW * 0.85f, left + w);
                break;
        }
    } else {
        w = MATH_MIN(scrW * 0.8f + 0.5f, w);
        MatchHeightForContent(w, h, textWH.x, textWH.y);
        left = MATH_MAX(scrW * 0.15f, ((scrW - w) >> 1) + 0.5f);
        switch (option_.side) {
            case TOP:
                top = scrH * 0.05f + 0.5f;
                break;
            case BOTTOM:
                top = MATH_MAX(minTop, static_cast<uint16_t>(scrH * 0.95f + 0.5f - h));
                break;
            case CENTER:
            default:
                top = MATH_MAX(minTop, (scrH - h) >> 1);
                break;
        }
        right = MATH_MIN(static_cast<uint16_t>(scrW * 0.85f + 0.5f), left + w);
    }

    bottom = MATH_MIN(static_cast<uint16_t>(scrH * 0.95f + 0.5f), top + h);
    rect_.SetRect(left, top, right, bottom);
}

void UIDuckweed::DrawBackground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa, Rect& rect)
{
    backgroundStyle_->SetStyle(STYLE_BACKGROUND_COLOR, option_.bgColor);
    backgroundStyle_->SetStyle(STYLE_BACKGROUND_OPA, option_.bgOpacity);
    backgroundStyle_->SetStyle(STYLE_BORDER_RADIUS, rect.GetHeight() >> 1);
    backgroundStyle_->SetStyle(STYLE_BORDER_COLOR, option_.bgColor);
    backgroundStyle_->SetStyle(STYLE_BORDER_OPA, option_.bgOpacity);

    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, rect, rect, *backgroundStyle_, backgroundOpa);
}

void UIDuckweed::DrawForeground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa, Rect& rect)
{
    if (uiText_ == nullptr) return;

    foregroundStyle_->SetStyle(STYLE_TEXT_COLOR, option_.textColor);
    foregroundStyle_->SetStyle(STYLE_TEXT_OPA, option_.textOpacity);

    Rect textRect;
    textRect.SetX(rect.GetLeft());
    textRect.SetY(rect.GetTop());
    textRect.SetWidth(MATH_MAX(0, rect.GetWidth()));
    textRect.SetHeight(MATH_MAX(0, rect.GetHeight()));
    uiText_->ReMeasureTextSize(textRect, *foregroundStyle_);

    uint16_t ellipsisIndex = uiText_->GetEllipsisIndex(textRect, *foregroundStyle_);

    uiText_->OnDraw(gfxDstBuffer, textRect, textRect, textRect, 0, *foregroundStyle_, ellipsisIndex, backgroundOpa);
}

} // namespace OHOS
#endif //BES_FEATURE_DUCKWEED