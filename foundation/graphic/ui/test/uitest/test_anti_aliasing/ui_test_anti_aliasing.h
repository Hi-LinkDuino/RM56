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

#ifndef UI_TEST_ANTI_ALIASING_H
#define UI_TEST_ANTI_ALIASING_H

#include "components/ui_label.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestAntiAliasing : public UITest {
public:
    UITestAntiAliasing() {}
    ~UITestAntiAliasing() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    /**
     * @brief Test if rect anti-aliasing
     */
    void UIKit_Anti_Aliasing_Test_Rect_001();

    /**
     * @brief Test if circle anti-aliasing
     */
    void UIKit_Anti_Aliasing_Test_Circle_001();

    /**
     * @brief Test if line anti-aliasing
     */
    void UIKit_Anti_Aliasing_Test_Line_001();

private:
    UIViewGroup* CreateTestCaseGroup() const;
    UILabel* CreateTitleLabel() const;
    UIScrollView* container_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_ANTI_ALIASING_H
