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

#include "components/ui_box_progress.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const int16_t MAX = 500;
    const int16_t MIN = 100;
    const int16_t WIDTH = 100;
    const int16_t HEIGHT = 100;
}

class UIBoxProgressTest : public testing::Test {
public:
    UIBoxProgressTest() : boxProgress_(nullptr) {}
    virtual ~UIBoxProgressTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    UIBoxProgress* boxProgress_;
};

void UIBoxProgressTest::SetUpTestCase()
{
}

void UIBoxProgressTest::TearDownTestCase()
{
}

void UIBoxProgressTest::SetUp()
{
    if (boxProgress_ == nullptr) {
        boxProgress_ = new UIBoxProgress();
    }
}

void UIBoxProgressTest::TearDown()
{
    if (boxProgress_ != nullptr) {
        delete boxProgress_;
        boxProgress_ = nullptr;
    }
}

/**
 * @tc.name: UIBoxProgressGetViewType_001
 * @tc.desc: Verify GetViewType function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UIBoxProgressTest, UIBoxProgressGetViewType_001, TestSize.Level1)
{
    if (boxProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(boxProgress_->GetViewType(), UI_BOX_PROGRESS);
}

/**
 * @tc.name: UIBoxProgressSetDirection_001
 * @tc.desc: Verify SetDirection function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UIBoxProgressTest, UIBoxProgressSetDirection_001, TestSize.Level0)
{
    if (boxProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UIBoxProgress::Direction direction = UIBoxProgress::Direction::DIR_BOTTOM_TO_TOP;
    boxProgress_->SetDirection(direction);
    EXPECT_EQ(boxProgress_->GetDirection(), direction);
    direction = UIBoxProgress::Direction::DIR_LEFT_TO_RIGHT;
    boxProgress_->SetDirection(direction);
    EXPECT_EQ(boxProgress_->GetDirection(), direction);
    direction = UIBoxProgress::Direction::DIR_RIGHT_TO_LEFT;
    boxProgress_->SetDirection(direction);
    EXPECT_EQ(boxProgress_->GetDirection(), direction);
    direction = UIBoxProgress::Direction::DIR_TOP_TO_BOTTOM;
    boxProgress_->SetDirection(direction);
    EXPECT_EQ(boxProgress_->GetDirection(), direction);
}

/**
 * @tc.name: UIBoxProgressSetValidWidth_001
 * @tc.desc: Verify SetValidWidth function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UIBoxProgressTest, UIBoxProgressSetValidWidth_001, TestSize.Level1)
{
    if (boxProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const int16_t valueWidth = 10;
    boxProgress_->Resize(WIDTH, HEIGHT);
    EXPECT_EQ(boxProgress_->GetValidWidth(), WIDTH);
    boxProgress_->SetValidWidth(valueWidth);
    EXPECT_EQ(boxProgress_->GetValidWidth(), valueWidth);
}

/**
 * @tc.name: UIBoxProgressSetValidHeight_001
 * @tc.desc: Verify SetValidHeight function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UIBoxProgressTest, UIBoxProgressSetValidHeight_001, TestSize.Level1)
{
    if (boxProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const int16_t valueHeight = 10;
    boxProgress_->Resize(WIDTH, HEIGHT);
    EXPECT_EQ(boxProgress_->GetValidHeight(), HEIGHT);
    boxProgress_->SetValidHeight(valueHeight);
    EXPECT_EQ(boxProgress_->GetValidHeight(), valueHeight);
}

/**
 * @tc.name: UIBoxProgressSetValue_001
 * @tc.desc: Verify SetValue function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UIBoxProgressTest, UIBoxProgressSetValue_001, TestSize.Level1)
{
    if (boxProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    boxProgress_->SetRange(MAX, MIN);

    int16_t value = MIN - 1;
    boxProgress_->SetValue(value);
    EXPECT_EQ(boxProgress_->GetValue(), MIN);

    value = MAX + 1;
    boxProgress_->SetValue(value);
    EXPECT_EQ(boxProgress_->GetValue(), MAX);
}

/**
 * @tc.name: UIBoxProgressSetWidth_001
 * @tc.desc: Verify SetWidth function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UIBoxProgressTest, UIBoxProgressSetWidth_001, TestSize.Level1)
{
    if (boxProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }

    boxProgress_->SetWidth(WIDTH);
    EXPECT_EQ(boxProgress_->GetWidth(), WIDTH);
    EXPECT_EQ(boxProgress_->GetValidWidth(), WIDTH);
}

/**
 * @tc.name: UIBoxProgressSetHeight_001
 * @tc.desc: Verify SetHeight function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UIBoxProgressTest, UIBoxProgressSetHeight_001, TestSize.Level1)
{
    if (boxProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }

    boxProgress_->SetHeight(HEIGHT);
    EXPECT_EQ(boxProgress_->GetHeight(), HEIGHT);
    EXPECT_EQ(boxProgress_->GetValidHeight(), HEIGHT);
}
} // namespace OHOS
