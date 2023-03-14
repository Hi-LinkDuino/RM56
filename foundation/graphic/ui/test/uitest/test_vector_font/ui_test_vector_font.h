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

#ifndef UI_TEST_VECTOR_FONT_H
#define UI_TEST_VECTOR_FONT_H

#include "graphic_config.h"
#if ENABLE_VECTOR_FONT
#include "ui_test.h"
#include "components/ui_scroll_view.h"
#include "font/ui_font.h"
#include <string>

namespace OHOS {
class UITestVectorFont : public UITest {
public:
    UITestVectorFont() {}
    ~UITestVectorFont() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void Font_FontEngine_Vector_Test_CH_Lang_001();
    void Font_FontEngine_Vector_Test_CH_Lang_002();
    void Font_FontEngine_Vector_Test_JA_Lang_001();
    void Font_FontEngine_Vector_Test_JA_Lang_002();
    void Font_FontEngine_Vector_Test_KR_Lang_001();
    void Font_FontEngine_Vector_Test_VI_Lang_001();
#if ENABLE_MULTI_FONT
    /**
     * @brief Test JA language
     */
    void Font_FontEngine_MultiLanguage_Test_JA_Lang_001();

    /**
     * @brief Test UnRegister JA language
     */
    void Font_FontEngine_MultiLanguage_Test_JA_Lang_002();

    /**
     * @brief Test KR language
     */
    void Font_FontEngine_MultiLanguage_Test_KR_Lang_001();

    /**
     * @brief Test VI language
     */
    void Font_FontEngine_MultiLanguage_Test_VI_Lang_001();

    /**
     * @brief Test CS language
     */
    void Font_FontEngine_MultiLanguage_Test_CS_Lang_001();

    /**
     * @brief Test EL language
     */
    void Font_FontEngine_MultiLanguage_Test_EL_Lang_001();

    /**
     * @brief Test RO language
     */
    void Font_FontEngine_MultiLanguage_Test_RO_Lang_001();
#endif

#if ENABLE_VECTOR_FONT
    void TestDrawText_ITALY_BOLD();
#endif
private:
    UIScrollView* container_ = nullptr;
    void InnerTestTitle(const char* title);
    int16_t xPos = 50; // 50: x initial position
    int16_t yPos = 5; // 5 : y initial position
};
} // namespace OHOS
#endif // ENABLE_VECTOR_FONT
#endif // UI_TEST_VECTOR_FONT_H
