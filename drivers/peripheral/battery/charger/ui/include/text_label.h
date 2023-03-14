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

#ifndef CHARGER_UI_TEXT_LABLE_H
#define CHARGER_UI_TEXT_LABLE_H

#include <iostream>
#include <string>
#include <functional>
#include "frame.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
class TextLabel : public View {
using ClickCallback = std::function<void(int32_t id)>;
enum class FontType {
    DEFAULT_FONT,
};

enum class AlignmentMethod {
    ALIGN_CENTER,
    ALIGN_TO_LEFT,
    ALIGN_TO_TOP,
};
public:
    TextLabel(int32_t mStartX, int32_t mStartY, int32_t w, int32_t h, Frame* mparent);
    ~TextLabel() override = default;;
    void SetText(const char* str);
    void SetTextColor(BRGA888Pixel color);
    void SetFont(FontType fType);
    void SetOutLineBold(bool topBold, bool bottomBold);
    void SetTextAlignmentMethod(AlignmentMethod methodH, AlignmentMethod methodV);
    void OnDraw() override;
private:
    void InitFont();
    FILE* InitFontType();
    void DrawText();
    void DrawTextLoop(unsigned char ch, char* tmpBuf, int32_t textSx, int32_t textSy);
    void DrawOutline();
    void DrawFocus();

    ClickCallback callBack_;
    char textBuf_[512 + 1] {};
    Frame* parent_ {};

    AlignmentMethod fontAligMethodLevel_ = AlignmentMethod::ALIGN_TO_LEFT;
    AlignmentMethod fontAligMethodUpright_ = AlignmentMethod::ALIGN_CENTER;

    BRGA888Pixel outlineColor_ {};
    BRGA888Pixel actionBgColor_ {};
    BRGA888Pixel normalBgColor_ {};
    BRGA888Pixel textColor_ {};

    bool boldTopLine_ = false;
    bool boldBottomLine_ = false;

    FontType fontType_ { FontType::DEFAULT_FONT };
    char fontBuf_[4096 * 4096] {};
    uint32_t fontWidth_ = 0;
    uint32_t fontHeight_ = 0;
};
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif // CHARGER_UI_TEXT_LABLE_H
