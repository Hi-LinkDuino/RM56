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

#include "font/ui_font.h"
#if ENABLE_VECTOR_FONT
#include "font/ui_font_vector.h"
#else
#include "font/ui_font_bitmap.h"
#endif
#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    constexpr uint8_t FONT_ERROR_RET = 0xFF;
    constexpr int8_t INVALID_RET = -1;
    constexpr uint8_t FONT_ID = 0xFF;
    constexpr uint8_t FONT_BPP = 8;
}
class UIFontTest : public testing::Test {
public:
    UIFontTest() {}
    virtual ~UIFontTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    static BaseFont* font_;
};

BaseFont* UIFontTest::font_ = nullptr;

void UIFontTest::SetUpTestCase()
{
    if (font_ == nullptr) {
#if ENABLE_VECTOR_FONT
        font_ = new UIFontVector();
#else
        font_ = new UIFontBitmap();
#endif
    }
}

void UIFontTest::TearDownTestCase()
{
    if (font_ != nullptr) {
        delete font_;
        font_ = nullptr;
    }
}

/**
 * @tc.name: Graphic_Font_Test_GetInstance_001
 * @tc.desc: Verify UIFont::GetInstance function, not nullptr.
 * @tc.type: FUNC
 * @tc.require: SR000F3PEK
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_GetInstance_001, TestSize.Level0)
{
    UIFont* font = UIFont::GetInstance();
    EXPECT_NE(font, nullptr);
}

/**
 * @tc.name: Graphic_Font_Test_IsVectorFont_001
 * @tc.desc: Verify IsVectorFont function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000FQNFQ
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_IsVectorFont_001, TestSize.Level1)
{
#if ENABLE_VECTOR_FONT
    bool ret = UIFont::GetInstance()->IsVectorFont();
    EXPECT_EQ(ret, true);
#else
    bool ret = UIFont::GetInstance()->IsVectorFont();
    EXPECT_EQ(ret, false);
#endif
}

/**
 * @tc.name: Graphic_Font_Test_SetFontPath_001
 * @tc.desc: Verify SetFontPath function, nullptr test.
 * @tc.type: FUNC
 * @tc.require: SR000FQNFQ
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_SetFontPath_001, TestSize.Level1)
{
    int8_t ret = UIFont::GetInstance()->SetFontPath(nullptr, BaseFont::FontType::DYNAMIC_FONT);
    EXPECT_EQ(ret, INVALID_RET);
}

/**
 * @tc.name: Graphic_Font_Test_SetFontPath_002
 * @tc.desc: Verify SetFontPath function, empty string test.
 * @tc.type: FUNC
 * @tc.require: SR000FQNFQ
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_SetFontPath_002, TestSize.Level1)
{
    int8_t ret = UIFont::GetInstance()->SetFontPath("", BaseFont::FontType::DYNAMIC_FONT);
#if ENABLE_VECTOR_FONT
    EXPECT_EQ(ret, 0);
#else
    EXPECT_EQ(ret, INVALID_RET);
#endif
}

/**
 * @tc.name: Graphic_Font_Test_GetFontId_001
 * @tc.desc: Verify GetFontId function, nullptr test.
 * @tc.type: FUNC
 * @tc.require: AR000FQNFR
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_GetFontId_001, TestSize.Level1)
{
    uint8_t ret = UIFont::GetInstance()->GetFontId(nullptr);
    EXPECT_EQ(ret, UIFontBuilder::GetInstance()->GetTotalFontId());
}

/**
 * @tc.name: Graphic_Font_Test_GetFontId_002
 * @tc.desc: Verify GetFontId function, empty string test.
 * @tc.type: FUNC
 * @tc.require: AR000FQNFR
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_GetFontId_002, TestSize.Level1)
{
    uint8_t id = UIFont::GetInstance()->GetFontId("", 0);
    EXPECT_EQ(id, UIFontBuilder::GetInstance()->GetTotalFontId());
}

/**
 * @tc.name: Graphic_Font_Test_GetHeight_001
 * @tc.desc: Verify GetHeight function, abnormal value test.
 * @tc.type: FUNC
 * @tc.require: AR000FQNFR
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_GetHeight_001, TestSize.Level1)
{
    uint16_t height = UIFont::GetInstance()->GetHeight(FONT_ID, 0);
#if ENABLE_VECTOR_FONT
    EXPECT_EQ(height, 0);
#else
    EXPECT_EQ(height, static_cast<uint16_t>(INVALID_RET));
#endif
}

/**
 * @tc.name: Graphic_Font_Test_GetWidth_001
 * @tc.desc: Verify GetWidth function, abnormal value test.
 * @tc.type: FUNC
 * @tc.require: AR000FQNFR
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_GetWidth_001, TestSize.Level1)
{
    uint16_t width = UIFont::GetInstance()->GetWidth(0, 0, 0, 0);
    EXPECT_EQ(width, 0);
}

/**
 * @tc.name: Graphic_Font_Test_GetBitmap_001
 * @tc.desc: Verify GetBitmap function, abnormal value test.
 * @tc.type: FUNC
 * @tc.require: AR000FQNFR
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_GetBitmap_001, TestSize.Level1)
{
    GlyphNode node;
    uint8_t* bitmap = UIFont::GetInstance()->GetBitmap(0, node, 0, 0, 0);
    EXPECT_EQ(bitmap, nullptr);
}

/**
 * @tc.name: Graphic_Font_Test_GetFontHeader_001
 * @tc.desc: Verify GetFontHeader function, abnormal value test.
 * @tc.type: FUNC
 * @tc.require: AR000FQNFR
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_GetFontHeader_001, TestSize.Level1)
{
    FontHeader header;
    int8_t res = UIFont::GetInstance()->GetFontHeader(header, FONT_ID, 0);
    EXPECT_EQ(res, INVALID_RET);
}

/**
 * @tc.name: Graphic_Font_Test_GetFontWeight_001
 * @tc.desc: Verify GetFontWeight function, abnormal value test.
 * @tc.type: FUNC
 * @tc.require: AR000FQNFR
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_GetFontWeight_001, TestSize.Level1)
{
    uint8_t fontWeight = UIFont::GetInstance()->GetFontWeight(FONT_ID);
#if ENABLE_VECTOR_FONT
    EXPECT_EQ(fontWeight, FONT_BPP);
#else
    EXPECT_EQ(fontWeight, 0);
#endif
}

/**
 * @tc.name: Graphic_Font_Test_GetFontInfo_001
 * @tc.desc: Verify GetFontInfo function, abnormal value test.
 * @tc.type: FUNC
 * @tc.require: AR000FQNFR
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_GetFontInfo_001, TestSize.Level1)
{
    const UITextLanguageFontParam* fontParam = UIFont::GetInstance()->GetFontInfo(FONT_ID);
    EXPECT_EQ(fontParam, nullptr);
}

/**
 * @tc.name: Graphic_Font_Test_GetShapingFontId_001
 * @tc.desc: Verify GetShapingFontId function, abnormal value test.
 * @tc.type: FUNC
 * @tc.require: AR000FQNFR
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_GetShapingFontId_001, TestSize.Level1)
{
    uint8_t ttfId = 0;
    uint32_t script = 0;
    uint8_t fontId = UIFont::GetInstance()->GetShapingFontId("", ttfId, script, FONT_ID, 0);
    EXPECT_EQ(fontId, 0);
}

/**
 * @tc.name: Graphic_Font_Test_RegisterFontInfo_001
 * @tc.desc: Verify UIFont::RegisterFontInfo function, error font name.
 * @tc.type: FUNC
 * @tc.require: AR000F3R7C
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_RegisterFontInfo_001, TestSize.Level1)
{
    uint8_t ret = UIFont::GetInstance()->RegisterFontInfo("error");
#if ENABLE_VECTOR_FONT
    EXPECT_EQ(ret, FONT_ERROR_RET);
#else
    EXPECT_EQ(ret, 0);
#endif
}

/**
 * @tc.name: Graphic_Font_Test_RegisterFontInfo_002
 * @tc.desc: Verify UIFont::RegisterFontInfo function, error font file path.
 * @tc.type: FUNC
 * @tc.require: AR000F3R7C
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_RegisterFontInfo_002, TestSize.Level0)
{
    uint8_t ret = UIFont::GetInstance()->RegisterFontInfo("ui-font.ttf");
#if ENABLE_VECTOR_FONT
    EXPECT_EQ(ret, FONT_ERROR_RET);
#else
    EXPECT_EQ(ret, 0);
#endif
}

/**
 * @tc.name: Graphic_Font_Test_UnregisterFontInfo_001
 * @tc.desc: Verify UIFont::UnregisterFontInfo function, error font name.
 * @tc.type: FUNC
 * @tc.require: AR000F3R7C
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_UnregisterFontInfo_001, TestSize.Level1)
{
    uint8_t ret = UIFont::GetInstance()->UnregisterFontInfo("error font name");
#if ENABLE_VECTOR_FONT
    EXPECT_EQ(ret, FONT_ERROR_RET);
#else
    EXPECT_EQ(ret, 0);
#endif
}

/**
 * @tc.name: Graphic_Font_Test_UnregisterFontInfo_002
 * @tc.desc: Verify UIFont::UnregisterFontInfo function, unregister fontsTable.
 * @tc.type: FUNC
 * @tc.require: AR000F3R7C
 */
HWTEST_F(UIFontTest, Graphic_Font_Test_UnregisterFontInfo_002, TestSize.Level0)
{
    const UITextLanguageFontParam* fontsTable = nullptr;
    uint8_t ret = UIFont::GetInstance()->UnregisterFontInfo(fontsTable, 0);
    EXPECT_EQ(ret, 0);
}
} // namespace OHOS
