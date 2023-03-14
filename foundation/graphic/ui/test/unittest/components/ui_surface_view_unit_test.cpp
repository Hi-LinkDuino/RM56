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

#include "components/ui_surface_view.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const int16_t INIT_POS_X = 10;
    const int16_t INIT_POS_Y = 20;
    const int16_t INIT_WIDTH = 50;
    const int16_t INIT_HEIGHT = 30;
}

class UISurfaceViewTest : public testing::Test {
public:
    UISurfaceViewTest() : surface_(nullptr) {}
    virtual ~UISurfaceViewTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    UISurfaceView* surface_;
};

void UISurfaceViewTest::SetUpTestCase()
{
}

void UISurfaceViewTest::TearDownTestCase()
{
}

void UISurfaceViewTest::SetUp()
{
    if (surface_ == nullptr) {
        surface_ = new UISurfaceView();
    }
}

void UISurfaceViewTest::TearDown()
{
    if (surface_ != nullptr) {
        delete surface_;
        surface_ = nullptr;
    }
}
/**
 * @tc.name: UISurfaceViewSetPosition_001
 * @tc.desc: Verify SetPosition function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQJ
 */
HWTEST_F(UISurfaceViewTest, UISurfaceViewSetPosition_001, TestSize.Level0)
{
    if (surface_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    surface_->SetPosition(INIT_POS_X, INIT_POS_Y);
    EXPECT_EQ(surface_->GetX(), INIT_POS_X);
    EXPECT_EQ(surface_->GetY(), INIT_POS_Y);

    surface_->SetPosition(INIT_POS_X + 1, INIT_POS_Y + 1, INIT_WIDTH, INIT_HEIGHT);
    EXPECT_EQ(surface_->GetX(), INIT_POS_X + 1);
    EXPECT_EQ(surface_->GetY(), INIT_POS_Y + 1);
    EXPECT_EQ(surface_->GetWidth(), INIT_WIDTH);
    EXPECT_EQ(surface_->GetHeight(), INIT_HEIGHT);
}

/**
 * @tc.name: UISurfaceViewResize_001
 * @tc.desc: Verify Resize function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQJ
 */
HWTEST_F(UISurfaceViewTest, UISurfaceViewResize_001, TestSize.Level1)
{
    if (surface_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    surface_->Resize(INIT_WIDTH, INIT_HEIGHT);
    EXPECT_EQ(surface_->GetWidth(), INIT_WIDTH);
    EXPECT_EQ(surface_->GetHeight(), INIT_HEIGHT);
}

/**
 * @tc.name: UISurfaceViewSetX_001
 * @tc.desc: Verify SetX function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQJ
 */
HWTEST_F(UISurfaceViewTest, UISurfaceViewSetX_001, TestSize.Level1)
{
    if (surface_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    surface_->SetX(INIT_POS_X);
    EXPECT_EQ(surface_->GetX(), INIT_POS_X);
}

/**
 * @tc.name: UISurfaceViewSetY_001
 * @tc.desc: Verify SetY function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQJ
 */
HWTEST_F(UISurfaceViewTest, UISurfaceViewSetY_001, TestSize.Level1)
{
    if (surface_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    surface_->SetY(INIT_POS_Y);
    EXPECT_EQ(surface_->GetY(), INIT_POS_Y);
}

/**
 * @tc.name: UISurfaceViewSetWidth_001
 * @tc.desc: Verify SetWidth function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQJ
 */
HWTEST_F(UISurfaceViewTest, UISurfaceViewSetWidth_001, TestSize.Level1)
{
    if (surface_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    surface_->SetWidth(INIT_WIDTH);
    EXPECT_EQ(surface_->GetWidth(), INIT_WIDTH);
}

/**
 * @tc.name: UISurfaceViewSetHeight_001
 * @tc.desc: Verify SetHeight function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQJ
 */
HWTEST_F(UISurfaceViewTest, UISurfaceViewSetHeight_001, TestSize.Level1)
{
    if (surface_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    surface_->SetHeight(INIT_HEIGHT);
    EXPECT_EQ(surface_->GetHeight(), INIT_HEIGHT);
}

/**
 * @tc.name: UISurfaceViewSetVisible_001
 * @tc.desc: Verify SetVisible function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQJ
 */
HWTEST_F(UISurfaceViewTest, UISurfaceViewSetVisible_001, TestSize.Level1)
{
    if (surface_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    bool visible = surface_->IsVisible();
    surface_->SetVisible(!visible);
    EXPECT_EQ(surface_->IsVisible(), !visible);
}
} // namespace OHOS
