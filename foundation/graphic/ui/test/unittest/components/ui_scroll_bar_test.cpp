/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <climits>
#include <gtest/gtest.h>

#include "components/ui_arc_scroll_bar.h"
#include "components/ui_box_scroll_bar.h"
#include "components/ui_list.h"
#include "components/ui_scroll_view.h"
#include "engines/gfx/gfx_engine_manager.h"

using namespace testing::ext;

namespace OHOS {
class ScrollBarTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void ScrollBarTest::SetUpTestCase() {}

void ScrollBarTest::TearDownTestCase() {}

class TestArcScrollBarTest : public UIArcScrollBar {
public:
    int16_t GetWidth() const
    {
        return width_;
    }

    int16_t GetRadius() const
    {
        return radius_;
    }

    const Style* GetBackStyle() const
    {
        return backgroundStyle_;
    }

    const Style* GetForeStyle() const
    {
        return foregroundStyle_;
    }

    float GetProportion() const
    {
        return foregroundProportion_;
    }

    uint8_t GetOpacity() const
    {
        return opacity_;
    }
};

class TestBoxScrollBarTest : public UIBoxScrollBar {
public:
    int16_t GetWidth() const
    {
        return backgroundRect_.GetWidth();
    }

    int16_t GetHeight() const
    {
        return backgroundRect_.GetHeight();
    }

    const Style* GetBackStyle() const
    {
        return backgroundStyle_;
    }

    const Style* GetForeStyle() const
    {
        return foregroundStyle_;
    }

    float GetProportion() const
    {
        return foregroundProportion_;
    }

    uint8_t GetOpacity() const
    {
        return opacity_;
    }
};

class TestUIScrollView : public UIScrollView {
public:
    bool GetXScrollBarVisible() const
    {
        return xScrollBarVisible_;
    }

    bool GetYScrollBarVisible() const
    {
        return yScrollBarVisible_;
    }
};

class TestUIList : public UIList {
public:
    bool GetXScrollBarVisible() const
    {
        return xScrollBarVisible_;
    }

    bool GetYScrollBarVisible() const
    {
        return yScrollBarVisible_;
    }
};

HWTEST_F(ScrollBarTest, UIScrollBarSetXScrollBarVisible, TestSize.Level0)
{
    TestUIScrollView scrollView;
    TestUIList uiList;

    EXPECT_FALSE(scrollView.GetXScrollBarVisible());
    EXPECT_FALSE(uiList.GetXScrollBarVisible());
    BaseGfxEngine::GetInstance()->SetScreenShape(ScreenShape::CIRCLE);
    scrollView.SetXScrollBarVisible(true);
    uiList.SetXScrollBarVisible(true);
    EXPECT_FALSE(scrollView.GetXScrollBarVisible());
    EXPECT_FALSE(uiList.GetXScrollBarVisible());

    BaseGfxEngine::GetInstance()->SetScreenShape(ScreenShape::RECTANGLE);
    scrollView.SetXScrollBarVisible(true);
    uiList.SetXScrollBarVisible(true);
    EXPECT_TRUE(scrollView.GetXScrollBarVisible());
    EXPECT_TRUE(uiList.GetXScrollBarVisible());
}

HWTEST_F(ScrollBarTest, UIScrollBarSetYScrollBarVisible, TestSize.Level0)
{
    TestUIScrollView scrollView;
    TestUIList uiList;

    EXPECT_FALSE(scrollView.GetYScrollBarVisible());
    EXPECT_FALSE(uiList.GetYScrollBarVisible());
    scrollView.SetYScrollBarVisible(true);
    uiList.SetYScrollBarVisible(true);
    EXPECT_TRUE(scrollView.GetYScrollBarVisible());
    EXPECT_TRUE(uiList.GetYScrollBarVisible());
}

HWTEST_F(ScrollBarTest, UIScrollBarSetPosition, TestSize.Level0)
{
    constexpr int16_t VALID_POSITION = 5;
    constexpr int16_t VALID_LEN = 5;
    constexpr int16_t NEGATIVE_LEN = -5;
    constexpr int16_t ZERO_LEN = 0;

    TestArcScrollBarTest arcBar;
    arcBar.SetPosition(VALID_POSITION, VALID_POSITION, VALID_LEN, VALID_LEN);
    EXPECT_EQ(arcBar.GetWidth(), VALID_LEN);
    EXPECT_EQ(arcBar.GetRadius(), VALID_LEN);

    TestBoxScrollBarTest boxBar;
    boxBar.SetPosition(VALID_POSITION, VALID_POSITION, VALID_LEN, VALID_LEN);
    EXPECT_EQ(boxBar.GetWidth(), VALID_LEN);
    EXPECT_EQ(boxBar.GetHeight(), VALID_LEN);

    arcBar.SetPosition(VALID_POSITION, VALID_POSITION, VALID_LEN, NEGATIVE_LEN);
    EXPECT_NE(arcBar.GetRadius(), NEGATIVE_LEN);

    boxBar.SetPosition(VALID_POSITION, VALID_POSITION, VALID_LEN, NEGATIVE_LEN);
    EXPECT_NE(boxBar.GetHeight(), NEGATIVE_LEN);

    arcBar.SetPosition(VALID_POSITION, VALID_POSITION, ZERO_LEN, VALID_LEN);
    EXPECT_NE(arcBar.GetWidth(), ZERO_LEN);

    boxBar.SetPosition(VALID_POSITION, VALID_POSITION, ZERO_LEN, VALID_LEN);
    EXPECT_NE(boxBar.GetWidth(), ZERO_LEN);
}

HWTEST_F(ScrollBarTest, UIScrollBarGetBarStyle, TestSize.Level0)
{
    Style& defaultBackStyle = StyleDefault::GetScrollBarBackgroundStyle();
    Style& defaultForeStyle = StyleDefault::GetScrollBarForegroundStyle();

    TestArcScrollBarTest arcBar;
    TestBoxScrollBarTest boxBar;

    EXPECT_EQ(arcBar.GetBackStyle()->GetStyle(STYLE_LINE_COLOR), defaultBackStyle.GetStyle(STYLE_LINE_COLOR));
    EXPECT_EQ(arcBar.GetForeStyle()->GetStyle(STYLE_LINE_COLOR), defaultForeStyle.GetStyle(STYLE_LINE_COLOR));

    EXPECT_EQ(arcBar.GetBackStyle()->GetStyle(STYLE_LINE_OPA), defaultBackStyle.GetStyle(STYLE_LINE_OPA));
    EXPECT_EQ(arcBar.GetForeStyle()->GetStyle(STYLE_LINE_OPA), defaultForeStyle.GetStyle(STYLE_LINE_OPA));

    EXPECT_EQ(boxBar.GetBackStyle()->GetStyle(STYLE_BACKGROUND_COLOR),
              defaultBackStyle.GetStyle(STYLE_BACKGROUND_COLOR));
    EXPECT_EQ(boxBar.GetForeStyle()->GetStyle(STYLE_BACKGROUND_COLOR),
              defaultForeStyle.GetStyle(STYLE_BACKGROUND_COLOR));

    EXPECT_EQ(boxBar.GetBackStyle()->GetStyle(STYLE_BACKGROUND_OPA), defaultBackStyle.GetStyle(STYLE_BACKGROUND_OPA));
    EXPECT_EQ(boxBar.GetForeStyle()->GetStyle(STYLE_BACKGROUND_OPA), defaultForeStyle.GetStyle(STYLE_BACKGROUND_OPA));
}

HWTEST_F(ScrollBarTest, UIScrollBarSetForegroundProportion, TestSize.Level0)
{
    TestArcScrollBarTest arcBar;
    TestBoxScrollBarTest boxBar;

    float proportion[] = {0, 0.5, 1};
    for (int8_t i = 0; i < sizeof(proportion) / sizeof(float); i++) {
        arcBar.SetForegroundProportion(proportion[i]);
        EXPECT_EQ(arcBar.GetProportion(), proportion[i]);

        boxBar.SetForegroundProportion(proportion[i]);
        EXPECT_EQ(boxBar.GetProportion(), proportion[i]);
    }

    float invalidProportion[] = {1.5, -0.5};
    for (int8_t i = 0; i < sizeof(invalidProportion) / sizeof(float); i++) {
        arcBar.SetForegroundProportion(invalidProportion[i]);
        EXPECT_NE(arcBar.GetProportion(), invalidProportion[i]);

        boxBar.SetForegroundProportion(invalidProportion[i]);
        EXPECT_NE(boxBar.GetProportion(), invalidProportion[i]);
    }
}

HWTEST_F(ScrollBarTest, UIScrollBarSetOpacity, TestSize.Level0)
{
    TestArcScrollBarTest arcBar;
    TestBoxScrollBarTest boxBar;

    uint8_t opa[] = {0, 255};
    for (int8_t i = 0; i < sizeof(opa) / sizeof(uint8_t); i++) {
        arcBar.SetOpacity(opa[i]);
        EXPECT_EQ(arcBar.GetOpacity(), opa[i]);

        boxBar.SetOpacity(opa[i]);
        EXPECT_EQ(boxBar.GetOpacity(), opa[i]);
    }
}
} // namespace OHOS
