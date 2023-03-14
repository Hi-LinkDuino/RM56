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

#ifndef UI_TEST_FONT_H
#define UI_TEST_FONT_H

#include "components/ui_scroll_view.h"
#include "font/ui_font.h"
#include "ui_test.h"

namespace OHOS {
class UITestFont : public UITest {
public:
    UITestFont() {}
    ~UITestFont() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    /**
     * @brief Test single line Chinese display
     */
    void UIKitFontTestDisplay001();

    /**
     * @brief Test multiline Chinese display
     */
    void UIKitFontTestDisplay002();

    /**
     * @brief Test single line English display
     */
    void UIKitFontTestDisplay003();

    /**
     * @brief Test multiline English display
     */
    void UIKitFontTestDisplay004();

    /**
     * @brief Test single line Chinese and English display
     */
    void UIKitFontTestDisplay005();

    /**
     * @brief Test multiline Chinese and English display
     */
    void UIKitFontTestDisplay006();

    /**
     * @brief Test single line text color display
     */
    void UIKitFontTestDisplay007();

    /**
     * @brief Test multiline line text color display
     */
    void UIKitFontTestDisplay008();

    /**
     * @brief Test text rotate
     */
    void UIKitFontTestDisplay009();

    /**
     * @brief Test text rotate
     */
    void UIKitFontTestDisplay010();

    /**
     * @brief Test font baseline alignment
     */
    void UIKitFontTestBaseline001();

    /**
     * @brief Test font baseline alignment
     */
    void UIKitFontTestBaseline002();

    /**
     * @brief Test font lineheight alignment
     */
    void UIKitFontTestLineHeight001();

    /**
     * @brief Test font lineheight alignment
     */
    void UIKitFontTestLineHeight002();

#if ENABLE_MULTI_FONT
    /**
     * @brief Test multilingual display
     */
    void UIKitFontMultiLanguage001();

    /**
     * @brief Test multilingual display
     */
    void UIKitFontMultiLanguage002();
#endif // ENABLE_MULTI_FONT

    /**
     * @brief Test font background color display
     */
    void UIKitFontTestBackgroundColor();

    /**
     * @brief Test font foreground color display
     */
    void UIKitFontTestForegroundColor();

    /**
     * @brief Test font line background color display
     */
    void UIKitFontTestLineBackgroundSpan();

    /**
     * @brief Test font absolute size display
     */
    void UIKitFontTestAbsoluteSizeSpan();

    /**
     * @brief Test font relative size display
     */
    void UIKitFontTestRelativeSizeSpan();

    /**
     * @brief Test line Emoji display
     */
    void UIKitFontTestDisplayEmoji001();

    /**
     * @brief Test Emoji and Text display
     */
    void UIKitFontTestDisplayEmoji002();

    /**
     * @brief Test Emoji and Text line break display
     */
    void UIKitFontTestDisplayEmoji003();
private:
    UIScrollView* container_ = nullptr;
    void InnerTestTitle(const char* title);
};
} // namespace OHOS
#endif // UI_TEST_FONT_H
