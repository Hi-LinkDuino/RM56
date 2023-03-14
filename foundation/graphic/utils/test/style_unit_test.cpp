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

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
class StyleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static Style* style_;
};
Style* StyleTest::style_ = nullptr;

void StyleTest::SetUpTestCase()
{
    if (style_ == nullptr) {
        style_ = new Style();
    }
}

void StyleTest::TearDownTestCase()
{
    if (style_ != nullptr) {
        delete style_;
        style_ = nullptr;
    }
}

/**
 * @tc.name: StyleSetStyle_001
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_001, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    EXPECT_EQ(style_->GetStyle(STYLE_BACKGROUND_COLOR), Color::Gray().full);
}

/**
 * @tc.name: StyleSetStyle_002
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_002, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    EXPECT_EQ(style_->GetStyle(STYLE_BACKGROUND_OPA), OPA_OPAQUE);
}

/**
 * @tc.name: StyleSetStyle_003
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_003, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_BORDER_RADIUS, 0);
    EXPECT_EQ(style_->GetStyle(STYLE_BORDER_RADIUS), 0);
}

/**
 * @tc.name: StyleSetStyle_004
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_004, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
    EXPECT_EQ(style_->GetStyle(STYLE_BORDER_COLOR), Color::Gray().full);
}

/**
 * @tc.name: StyleSetStyle_005
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_005, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_BORDER_OPA, OPA_OPAQUE);
    EXPECT_EQ(style_->GetStyle(STYLE_BORDER_OPA), OPA_OPAQUE);
}

/**
 * @tc.name: StyleSetStyle_006
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_006, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t BORDER_WIDTH = 1;
    style_->SetStyle(STYLE_BORDER_WIDTH, BORDER_WIDTH);
    EXPECT_EQ(style_->GetStyle(STYLE_BORDER_WIDTH), BORDER_WIDTH);
}

/**
 * @tc.name: StyleSetStyle_007
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_007, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_PADDING_LEFT, 0);
    EXPECT_EQ(style_->GetStyle(STYLE_PADDING_LEFT), 0);
}

/**
 * @tc.name: StyleSetStyle_008
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_008, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_PADDING_RIGHT, HORIZONTAL_RESOLUTION);
    EXPECT_EQ(style_->GetStyle(STYLE_PADDING_RIGHT), HORIZONTAL_RESOLUTION);
}

/**
 * @tc.name: StyleSetStyle_009
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_009, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_PADDING_TOP, 0);
    EXPECT_EQ(style_->GetStyle(STYLE_PADDING_TOP), 0);
}

/**
 * @tc.name: StyleSetStyle_010
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_010, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_PADDING_BOTTOM, VERTICAL_RESOLUTION);
    EXPECT_EQ(style_->GetStyle(STYLE_PADDING_BOTTOM), VERTICAL_RESOLUTION);
}

/**
 * @tc.name: StyleSetStyle_011
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_011, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_MARGIN_LEFT, 0);
    EXPECT_EQ(style_->GetStyle(STYLE_MARGIN_LEFT), 0);
}

/**
 * @tc.name: StyleSetStyle_012
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_012, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_MARGIN_RIGHT, HORIZONTAL_RESOLUTION);
    EXPECT_EQ(style_->GetStyle(STYLE_MARGIN_RIGHT), HORIZONTAL_RESOLUTION);
}

/**
 * @tc.name: StyleSetStyle_013
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_013, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_MARGIN_TOP, 0);
    EXPECT_EQ(style_->GetStyle(STYLE_MARGIN_TOP), 0);
}

/**
 * @tc.name: StyleSetStyle_014
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_014, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_MARGIN_BOTTOM, VERTICAL_RESOLUTION);
    EXPECT_EQ(style_->GetStyle(STYLE_MARGIN_BOTTOM), VERTICAL_RESOLUTION);
}

/**
 * @tc.name: StyleSetStyle_015
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_015, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    EXPECT_EQ(style_->GetStyle(STYLE_IMAGE_OPA), OPA_OPAQUE);
}

/**
 * @tc.name: StyleSetStyle_015
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_016, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);
    EXPECT_EQ(style_->GetStyle(STYLE_TEXT_COLOR), Color::Gray().full);
}

/**
 * @tc.name: StyleSetStyle_016
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_017, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_TEXT_FONT, 0);
    EXPECT_EQ(style_->GetStyle(STYLE_TEXT_FONT), 0);
}

/**
 * @tc.name: StyleSetStyle_017
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_018, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t letterSpace = 4;
    style_->SetStyle(STYLE_LETTER_SPACE, letterSpace);
    EXPECT_EQ(style_->GetStyle(STYLE_LETTER_SPACE), letterSpace);
}


/**
 * @tc.name: StyleSetStyle_018
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000FQNF0
 */
HWTEST_F(StyleTest, StyleSetStyle_019, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t lineHeight = 10;
    style_->SetStyle(STYLE_LINE_HEIGHT, lineHeight);
    EXPECT_EQ(style_->GetStyle(STYLE_LINE_HEIGHT), lineHeight);
}

/**
 * @tc.name: StyleSetStyle_019
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_020, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    EXPECT_EQ(style_->GetStyle(STYLE_TEXT_OPA), OPA_OPAQUE);
}

/**
 * @tc.name: StyleSetStyle_020
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_021, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_LINE_COLOR, Color::Gray().full);
    EXPECT_EQ(style_->GetStyle(STYLE_LINE_COLOR), Color::Gray().full);
}

/**
 * @tc.name: StyleSetStyle_021
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_022, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t lineWidth = 1;
    style_->SetStyle(STYLE_LINE_WIDTH, lineWidth);
    EXPECT_EQ(style_->GetStyle(STYLE_LINE_WIDTH), lineWidth);
}

/**
 * @tc.name: StyleSetStyle_022
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_023, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_LINE_OPA, OPA_OPAQUE);
    EXPECT_EQ(style_->GetStyle(STYLE_LINE_OPA), OPA_OPAQUE);
}

/**
 * @tc.name: StyleSetStyle_023
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(StyleTest, StyleSetStyle_024, TestSize.Level0)
{
    if (style_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    style_->SetStyle(STYLE_LINE_CAP, CapType::CAP_ROUND);
    EXPECT_EQ(style_->GetStyle(STYLE_LINE_CAP), CapType::CAP_ROUND);
}
} // namespace OHOS
