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

#include "components/ui_time_picker.h"
#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
class UITimePickerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static UITimePicker* timePicker_;
};

UITimePicker* UITimePickerTest::timePicker_ = nullptr;

void UITimePickerTest::SetUpTestCase(void)
{
    if (timePicker_ == nullptr) {
        timePicker_ = new UITimePicker();
    }
}

void UITimePickerTest::TearDownTestCase(void)
{
    if (timePicker_ != nullptr) {
        delete timePicker_;
        timePicker_ = nullptr;
    }
}
/**
 * @tc.name: UITimePickerGetViewType_001
 * @tc.desc: Verify GetViewType function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ7
 */
HWTEST_F(UITimePickerTest, UITimePickerGetViewType_001, TestSize.Level1)
{
    if (timePicker_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(timePicker_->GetViewType(), UI_TIME_PICKER);
}

/**
 * @tc.name: UITimePickerSetWidth_001
 * @tc.desc: Verify SetWidth function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ7
 */
HWTEST_F(UITimePickerTest, UITimePickerSetWidth_001, TestSize.Level0)
{
    if (timePicker_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t width = 50;

    timePicker_->SetWidth(width);
    EXPECT_EQ(timePicker_->GetWidth(), width);
}

/**
 * @tc.name: UITimePickerSetHeight_001
 * @tc.desc: Verify SetHeight function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ7
 */
HWTEST_F(UITimePickerTest, UITimePickerSetHeight_001, TestSize.Level0)
{
    if (timePicker_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t height = 50;

    timePicker_->SetHeight(height);
    EXPECT_EQ(timePicker_->GetHeight(), height);
}
} // namespace OHOS
