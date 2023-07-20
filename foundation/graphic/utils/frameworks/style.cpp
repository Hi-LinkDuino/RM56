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

#include "gfx_utils/style.h"

namespace OHOS {
Style StyleDefault::defaultStyle_;
Style StyleDefault::brightStyle_;
Style StyleDefault::brightColorStyle_;
Style StyleDefault::buttonReleasedStyle_;
Style StyleDefault::buttonPressedStyle_;
Style StyleDefault::buttonInactiveStyle_;
Style StyleDefault::labelStyle_;
Style StyleDefault::backgroundTransparentStyle_;
Style StyleDefault::progressBackgroundStyle_;
Style StyleDefault::progressForegroundStyle_;
Style StyleDefault::sliderKnobStyle_;
Style StyleDefault::pickerBackgroundStyle_;
Style StyleDefault::pickerHighlightStyle_;
Style StyleDefault::scrollBarBackgroundStyle_;
Style StyleDefault::scrollBarForegroundStyle_;

Style::Style()
    : bgColor_(Color::Black()),
      bgOpa_(OPA_OPAQUE),
      borderOpa_(OPA_OPAQUE),
      borderWidth_(0),
      borderRadius_(0),
      radiusDirection_(0),
      borderColor_(Color::Black()),
      paddingLeft_(0),
      paddingRight_(0),
      paddingTop_(0),
      paddingBottom_(0),
      marginLeft_(0),
      marginRight_(0),
      marginTop_(0),
      marginBottom_(0),
      imageOpa_(OPA_OPAQUE),
      textOpa_(OPA_OPAQUE),
      font_(16), // 16 : default font size
      lineSpace_(2), // 2 : default line space
      letterSpace_(0),
      lineHeight_(0),
      textColor_(Color::White()),
      lineColor_(Color::GetColorFromRGB(0x20, 0x20, 0x20)),
      lineOpa_(OPA_OPAQUE),
      lineCap_(CapType::CAP_NONE),
      lineWidth_(2), // 2 : default line width
      useRadiuRepLineWidth(false)
{
}

int64_t Style::GetStyle(uint8_t key) const
{
    switch (key) {
        case STYLE_BACKGROUND_COLOR:
            return bgColor_.full;
        case STYLE_BACKGROUND_OPA:
            return bgOpa_;
        case STYLE_RADIUS_DIRECTION:
            return radiusDirection_;
        case STYLE_BORDER_RADIUS:
            return borderRadius_;
        case STYLE_BORDER_COLOR:
            return borderColor_.full;
        case STYLE_BORDER_OPA:
            return borderOpa_;
        case STYLE_BORDER_WIDTH:
            return borderWidth_;
        case STYLE_PADDING_LEFT:
            return paddingLeft_;
        case STYLE_PADDING_RIGHT:
            return paddingRight_;
        case STYLE_PADDING_TOP:
            return paddingTop_;
        case STYLE_PADDING_BOTTOM:
            return paddingBottom_;
        case STYLE_MARGIN_LEFT:
            return marginLeft_;
        case STYLE_MARGIN_RIGHT:
            return marginRight_;
        case STYLE_MARGIN_TOP:
            return marginTop_;
        case STYLE_MARGIN_BOTTOM:
            return marginBottom_;
        case STYLE_IMAGE_OPA:
            return imageOpa_;
        case STYLE_TEXT_COLOR:
            return textColor_.full;
        case STYLE_TEXT_FONT:
            return font_;
        case STYLE_LINE_SPACE:
            return lineSpace_;
        case STYLE_LETTER_SPACE:
            return letterSpace_;
        case STYLE_LINE_HEIGHT:
            return lineHeight_;
        case STYLE_TEXT_OPA:
            return textOpa_;
        case STYLE_LINE_COLOR:
            return lineColor_.full;
        case STYLE_LINE_WIDTH:
            return lineWidth_;
        case STYLE_LINE_OPA:
            return lineOpa_;
        case STYLE_LINE_CAP:
            return lineCap_;
        default:
            return 0;
    }
}
void Style::TranslateValueToType(int16_t radiusDirection){
    if(radiusDirection ==1){
        radiusDirection_ = RadiusDirectionType::RADIUS_DIRECTION_LEFT;
    }else if (radiusDirection ==2)
    {
        radiusDirection_ = RadiusDirectionType::RADIUS_DIRECTION_RIGHT;
    }else if (radiusDirection ==3)
    {
        radiusDirection_ = RadiusDirectionType::RADIUS_DIRECTION_TOP;
    }else if (radiusDirection ==4)
    {
        radiusDirection_ = RadiusDirectionType::RADIUS_DIRECTION_BOTTOM;
    }
}

void Style::SetStyle(uint8_t key, int64_t value)
{
    switch (key) {
        case STYLE_BACKGROUND_COLOR:
            bgColor_.full = value;
            break;
        case STYLE_BACKGROUND_OPA:
            bgOpa_ = value;
            break;
        case STYLE_RADIUS_DIRECTION:
            TranslateValueToType(value);
            break;
        case STYLE_BORDER_RADIUS:
            borderRadius_ = value;
            break;
        case STYLE_BORDER_COLOR:
            borderColor_.full = value;
            break;
        case STYLE_BORDER_OPA:
            borderOpa_ = value;
            break;
        case STYLE_BORDER_WIDTH:
            borderWidth_ = value;
            break;
        case STYLE_PADDING_LEFT:
            paddingLeft_ = value;
            break;
        case STYLE_PADDING_RIGHT:
            paddingRight_ = value;
            break;
        case STYLE_PADDING_TOP:
            paddingTop_ = value;
            break;
        case STYLE_PADDING_BOTTOM:
            paddingBottom_ = value;
            break;
        case STYLE_MARGIN_LEFT:
            marginLeft_ = value;
            break;
        case STYLE_MARGIN_RIGHT:
            marginRight_ = value;
            break;
        case STYLE_MARGIN_TOP:
            marginTop_ = value;
            break;
        case STYLE_MARGIN_BOTTOM:
            marginBottom_ = value;
            break;
        case STYLE_IMAGE_OPA:
            imageOpa_ = value;
            break;
        case STYLE_TEXT_COLOR:
            textColor_.full = value;
            break;
        case STYLE_TEXT_FONT:
            font_ = value;
            break;
        case STYLE_LINE_SPACE:
            lineSpace_ = value;
            break;
        case STYLE_LETTER_SPACE:
            letterSpace_ = value;
            break;
        case STYLE_LINE_HEIGHT:
            lineHeight_ = value;
            break;
        case STYLE_TEXT_OPA:
            textOpa_ = value;
            break;
        case STYLE_LINE_COLOR:
            lineColor_.full = value;
            break;
        case STYLE_LINE_WIDTH:
            lineWidth_ = value;
            break;
        case STYLE_LINE_OPA:
            lineOpa_ = value;
            break;
        case STYLE_LINE_CAP:
            lineCap_ = value;
            break;
        default:
            break;
    }
}

void StyleDefault::Init()
{
    InitStyle();
    InitButtonStyle();
    InitLabelStyle();
    InitBackgroundTransparentStyle();
    InitProgressStyle();
    InitPickerStyle();
    InitScrollBarStyle();
}

void StyleDefault::InitStyle()
{
    brightStyle_ = defaultStyle_;
    brightStyle_.SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    brightStyle_.SetStyle(STYLE_BORDER_RADIUS, 3); // 3: rect radius
    brightStyle_.SetStyle(STYLE_BORDER_COLOR, Color::GetColorFromRGB(0x40, 0x40, 0x40).full);
    brightStyle_.SetStyle(STYLE_BORDER_WIDTH, 3); // 3: rect border width
    brightStyle_.SetStyle(STYLE_BORDER_OPA, 76); // 76: default opa
    brightStyle_.SetStyle(STYLE_TEXT_COLOR, Color::GetColorFromRGB(0x20, 0x20, 0x20).full);
    brightStyle_.SetStyle(STYLE_LINE_COLOR, Color::GetColorFromRGB(0x20, 0x20, 0x20).full);

    brightColorStyle_ = brightStyle_;
    brightColorStyle_.SetStyle(STYLE_TEXT_COLOR, Color::GetColorFromRGB(0xe0, 0xe0, 0xe0).full);
    brightColorStyle_.SetStyle(STYLE_LINE_COLOR, Color::GetColorFromRGB(0xc0, 0xc0, 0xc0).full);
    brightColorStyle_.SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0x6b, 0x9a, 0xc7).full);
    brightColorStyle_.SetStyle(STYLE_BORDER_COLOR, Color::GetColorFromRGB(0x15, 0x2c, 0x42).full);
}

void StyleDefault::InitButtonStyle()
{
    /* Button released style */
    buttonReleasedStyle_ = defaultStyle_;
    buttonReleasedStyle_.SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0x00, 0x7d, 0xff).full);
    buttonReleasedStyle_.SetStyle(STYLE_BORDER_RADIUS, 200); // 200: button radius
    buttonReleasedStyle_.SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    buttonReleasedStyle_.SetStyle(STYLE_BORDER_COLOR, Color::GetColorFromRGB(0x0b, 0x19, 0x28).full);
    buttonReleasedStyle_.SetStyle(STYLE_BORDER_WIDTH, 0);
    buttonReleasedStyle_.SetStyle(STYLE_BORDER_OPA, 178); // 178: default opa

    /* Button pressed style */
    buttonPressedStyle_ = buttonReleasedStyle_;
    buttonPressedStyle_.SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0x19, 0x96, 0xff).full);
    buttonPressedStyle_.SetStyle(STYLE_BORDER_COLOR, Color::GetColorFromRGB(0x0b, 0x19, 0x28).full);

    /* Button inactive style */
    buttonInactiveStyle_ = buttonReleasedStyle_;
    buttonInactiveStyle_.SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0x61, 0x7e, 0x9d).full);
    buttonInactiveStyle_.SetStyle(STYLE_BORDER_COLOR, Color::GetColorFromRGB(0x90, 0x90, 0x90).full);
}

void StyleDefault::InitLabelStyle()
{
    labelStyle_ = defaultStyle_;
    labelStyle_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
}

void StyleDefault::InitBackgroundTransparentStyle()
{
    backgroundTransparentStyle_ = defaultStyle_;
    backgroundTransparentStyle_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
}

void StyleDefault::InitPickerStyle()
{
    pickerBackgroundStyle_ = defaultStyle_;
    pickerBackgroundStyle_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    pickerBackgroundStyle_.SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);

    pickerHighlightStyle_ = defaultStyle_;
    pickerHighlightStyle_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    pickerHighlightStyle_.SetStyle(STYLE_TEXT_FONT, 18); // 18: font value
    pickerHighlightStyle_.SetStyle(STYLE_TEXT_COLOR, Color::GetColorFromRGB(0x45, 0xa5, 0xff).full);
}

void StyleDefault::InitProgressStyle()
{
    progressBackgroundStyle_ = defaultStyle_;
    progressBackgroundStyle_.SetStyle(STYLE_BORDER_WIDTH, 0);
    progressBackgroundStyle_.SetStyle(STYLE_BORDER_RADIUS, 0);

    progressForegroundStyle_ = brightStyle_;
    progressForegroundStyle_.SetStyle(STYLE_BORDER_WIDTH, 0);
    progressForegroundStyle_.SetStyle(STYLE_BORDER_RADIUS, 0);
    progressForegroundStyle_.SetStyle(STYLE_LINE_COLOR, Color::GetColorFromRGB(0x45, 0xa5, 0xff).full);

    sliderKnobStyle_ = brightColorStyle_;
    sliderKnobStyle_.SetStyle(STYLE_BORDER_WIDTH, 0);
}

void StyleDefault::InitScrollBarStyle()
{
    scrollBarBackgroundStyle_.SetStyle(STYLE_BACKGROUND_COLOR,
        Color::GetColorFromRGB(0x7f, 0x7f, 0x7f).full); // 0x7f: default color
    scrollBarBackgroundStyle_.SetStyle(STYLE_BACKGROUND_OPA, 128); // 128: default opa
    scrollBarBackgroundStyle_.SetStyle(STYLE_LINE_COLOR,
        Color::GetColorFromRGB(0x7f, 0x7f, 0x7f).full); // 0x7f: default color
    scrollBarBackgroundStyle_.SetStyle(STYLE_LINE_OPA, 128); // 128: default opa
    scrollBarBackgroundStyle_.SetStyle(STYLE_LINE_WIDTH, 8); // 8: default width

    scrollBarForegroundStyle_.SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    scrollBarForegroundStyle_.SetStyle(STYLE_BACKGROUND_OPA, 168); // 168: default opa
    scrollBarForegroundStyle_.SetStyle(STYLE_LINE_COLOR, Color::White().full);
    scrollBarForegroundStyle_.SetStyle(STYLE_LINE_OPA, 168); // 168: default opa
    scrollBarForegroundStyle_.SetStyle(STYLE_LINE_WIDTH, 6); // 6: default width
}
} // namespace OHOS
