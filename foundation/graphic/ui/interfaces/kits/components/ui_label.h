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

/**
 * @addtogroup UI_Components
 * @{
 *
 * @brief Defines UI components such as buttons, texts, images, lists, and progress bars.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file ui_label.h
 *
 * @brief Declares a <b>UILabel</b> class that represents a label.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_LABEL_H
#define GRAPHIC_LITE_UI_LABEL_H

#include "animator/animator.h"
#include "common/text.h"
#include "components/ui_view.h"
#if ENABLE_VECTOR_FONT
#include "common/spannable_string.h"
#endif

namespace OHOS {
/**
 * @brief Defines the functions for presenting a label in a specified area, setting the style and background color
 *        of a label, and setting the display mode of a long label text.
 *
 * @since 1.0
 * @version 1.0
 */
class UILabel : public UIView {
public:
    /**
     * @brief Enumerates the display modes of a long text.
     */
    enum LineBreakMode : uint8_t {
        /**
         * The label size is adaptive to the text size.
         */
        LINE_BREAK_ADAPT = 0,
        /**
         * The height of this label remains unchanged, and the width is adaptive to the text size.
         */
        LINE_BREAK_STRETCH,
        /**
         * The width of this label remains unchanged, and the height is adaptive to the text size.
         * The text switches to the next line if the text exceeds the maximum label width.
         */
        LINE_BREAK_WRAP,
        /**
         * The width and height of this label remain unchanged.
         * If this text is too long, ellipsis will be used at the end.
         */
        LINE_BREAK_ELLIPSIS,
        /**
         * The width and height of this label remain unchanged.
         * If this text is too long, it will be rolled to display.
         */
        LINE_BREAK_MARQUEE,
        /**
         * The width and height of this label remain unchanged.
         * If this text is too long, it will be cropped to display.
         */
        LINE_BREAK_CLIP,
        /**
         * Maximum value of the line break mode, which is used for validity check.
         */
        LINE_BREAK_MAX,
    };

    /**
     * @brief A constructor used to create a <b>UILabel</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    UILabel();

    /**
     * @brief A destructor used to delete the <b>UILabel</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~UILabel();

    /**
     * @brief Obtains the view type.
     *
     * @return Returns <b>UI_LABEL</b>, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_LABEL;
    }

    /**
     * @brief Obtains the width of this label.
     *
     * @return Returns the label width.
     * @since 1.0
     * @version 1.0
     */
    int16_t GetWidth() override;

    /**
     * @brief Obtains the height of this label.
     *
     * @return Returns the label height.
     * @since 1.0
     * @version 1.0
     */
    int16_t GetHeight() override;

    /**
     * @brief Sets the view style.
     * @param style Indicates the view style.
     * @since 1.0
     * @version 1.0
     */
    void SetStyle(Style& style) override
    {
        UIView::SetStyle(style);
    }

    /**
     * @brief Sets a style.
     *
     * @param key Indicates the key of the style to set.
     * @param value Indicates the value matching the key.
     * @since 1.0
     * @version 1.0
     */
    void SetStyle(uint8_t key, int64_t value) override;

    /**
     * @brief Checks whether this label needs to be covered before drawing it.
     *
     * @param invalidatedArea Indicates the area to draw.
     * @return Returns <b>true</b> if this label needs to be covered; returns <b> false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool OnPreDraw(Rect& invalidatedArea) const override
    {
        return false;
    }

    /**
     * @brief Draws this label.
     *
     * @param invalidatedArea Indicates the area to draw.
     * @since 1.0
     * @version 1.0
     */
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    /**
     * @brief Sets the text content for this label.
     *
     * @param text Indicates the pointer to the text content.
     * @since 1.0
     * @version 1.0
     */
    void SetText(const char* text);

#if ENABLE_VECTOR_FONT
    /**
     * @brief Sets the SpannableString content for this label.
     *
     * @param text Indicates the pointer to the text content.
     */
    void SetText(const SpannableString* text);

#endif
    /**
     * @brief Obtains the text of this label.
     *
     * @return Returns the text.
     * @since 1.0
     * @version 1.0
     */
    const char* GetText() const
    {
        return (labelText_ == nullptr) ? nullptr : labelText_->GetText();
    }

    /**
     * @brief Sets the line break mode for this text.
     *
     * @param lineBreakMode Indicates the line break mode to set.
     * @since 1.0
     * @version 1.0
     */
    void SetLineBreakMode(const uint8_t lineBreakMode);

    /**
     * @brief Obtains the line break mode of this text.
     *
     * @return Returns the line break mode.
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetLineBreakMode() const
    {
        return lineBreakMode_;
    }

    /**
     * @brief Sets the color for this text.
     *
     * @param color Indicates the text color to set.
     * @since 1.0
     * @version 1.0
     */
    void SetTextColor(ColorType color)
    {
        useTextColor_ = true;
        textColor_ = color;
    }

    /**
     * @brief Obtains the color of this text.
     *
     * @return Returns the text color.
     * @since 1.0
     * @version 1.0
     */
    ColorType GetTextColor() const
    {
        return useTextColor_ ? textColor_ : GetStyleConst().textColor_;
    }

    /**
     * @brief Sets the alignment mode for this text.
     *
     * @param horizontalAlign Indicates the horizontal alignment mode to set,
     *                        which can be {@link TEXT_ALIGNMENT_LEFT},
     *                        {@link TEXT_ALIGNMENT_CENTER}, or {@link TEXT_ALIGNMENT_RIGHT}.
     * @param verticalAlign Indicates the vertical alignment mode to set, which can be
     *                      {@link TEXT_ALIGNMENT_TOP} (default mode), {@link TEXT_ALIGNMENT_CENTER},
     *                      or {@link TEXT_ALIGNMENT_BOTTOM}.
     * @since 1.0
     * @version 1.0
     */
    void SetAlign(UITextLanguageAlignment horizontalAlign,
        UITextLanguageAlignment verticalAlign = TEXT_ALIGNMENT_TOP);

    /**
     * @brief Obtains the horizontal alignment mode.
     *
     * @return Returns the horizontal alignment mode.
     * @since 1.0
     * @version 1.0
     */
    UITextLanguageAlignment GetHorAlign()
    {
        InitLabelText();
        return labelText_->GetHorAlign();
    }

    /**
     * @brief Obtains the vertical alignment mode.
     *
     * @return Returns the vertical alignment mode.
     * @since 1.0
     * @version 1.0
     */
    UITextLanguageAlignment GetVerAlign()
    {
        InitLabelText();
        return labelText_->GetVerAlign();
    }

    /**
     * @brief Sets the direction for this text.
     *
     * @return direct Returns the text direction, as defined in {@link UITextLanguageDirect}.
     * @since 1.0
     * @version 1.0
     */
    void SetDirect(UITextLanguageDirect direct)
    {
        InitLabelText();
        labelText_->SetDirect(direct);
    }

    /**
     * @brief Obtains the direction of this text.
     *
     * @return Returns the text direction, as defined in {@link UITextLanguageDirect}.
     * @since 1.0
     * @version 1.0
     */
    UITextLanguageDirect GetDirect()
    {
        InitLabelText();
        return labelText_->GetDirect();
    }

    /**
     * @brief Sets the font ID for this label.
     *
     * @param fontId Indicates the font ID composed of font name and size.
     * @since 1.0
     * @version 1.0
     */
    void SetFontId(uint8_t fontId);

    /**
     * @brief Obtains the font ID composed of font name and size.
     *
     * @return Returns the front ID of this label.
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetFontId()
    {
        InitLabelText();
        return labelText_->GetFontId();
    }

    /**
     * @brief Sets the font for this label.
     *
     * @param name Indicates the pointer to the font name.
     * @param size Indicates the font size to set.
     * @since 1.0
     * @version 1.0
     */
    void SetFont(const char* name, uint8_t size);

    /**
     * @brief Sets the scroll speed for this text.
     *
     * @param speed Indicates the scroll speed to set.
     * @since 1.0
     * @version 1.0
     */
    void SetRollSpeed(uint16_t speed);

    /**
     * @brief Obtains the width of this text.
     *
     * @return Returns the text width.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetTextWidth();

    /**
     * @brief Obtains the height of this text.
     *
     * @return Returns the text height.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetTextHeight();

    /**
     * @brief Sets the position where this text starts to roll.
     *
     * @param pos Indicates the position to set.
     * @since 1.0
     * @version 1.0
     */
    void SetRollStartPos(int16_t pos);

    /**
     * @brief Obtains the position where this text starts to roll.
     *
     * @return Returns the position where this text starts to roll.
     * @since 1.0
     * @version 1.0
     */
    int16_t GetRollStartPos() const;

    /**
     * @brief Sets the width for this label.
     *
     * @param width Indicates the width to set.
     * @since 1.0
     * @version 1.0
     */
    void SetWidth(int16_t width) override;

    /**
     * @brief Sets the height for this label.
     *
     * @param height Indicates the height to set.
     * @since 1.0
     * @version 1.0
     */
    void SetHeight(int16_t height) override;

    void ReMeasure() override;

    void SetSupportBaseLine(bool baseLine)
    {
        InitLabelText();
        labelText_->SetSupportBaseLine(baseLine);
    }

    void SetBackgroundColorSpan(ColorType backgroundColor, int16_t start, int16_t end)
    {
        labelText_->SetBackgroundColorSpan(backgroundColor, start, end);
    }

    void SetForegroundColorSpan(ColorType fontColor, int16_t start, int16_t end)
    {
        labelText_->SetForegroundColorSpan(fontColor, start, end);
    }

    void SetLineBackgroundSpan(ColorType lineBackgroundColor, int16_t start, int16_t end)
    {
        labelText_->SetLineBackgroundSpan(lineBackgroundColor, start, end);
    }

    void SetAbsoluteSizeSpan(int16_t start, int16_t end, uint8_t size);
    void SetRelativeSizeSpan(int16_t start, int16_t end, float size);

protected:
    Text* labelText_;
    void RefreshLabel();
    virtual void InitLabelText();

private:
    friend class LabelAnimator;

    void RemeasureForMarquee(int16_t textWidth);

    bool needRefresh_ : 1;
    bool useTextColor_ : 1;
    bool hasAnimator_ : 1;
    uint8_t lineBreakMode_ : 4;
    uint16_t ellipsisIndex_;
    int16_t offsetX_;
    ColorType textColor_;

    static constexpr uint16_t DEFAULT_ANIMATOR_SPEED = 35;
    union {
        Animator* animator;
        struct {
            uint16_t speed;
            int16_t pos;
        };
    } animator_;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_LABEL_H
