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

#ifndef UI_TEST_ARC_LABEL_H
#define UI_TEST_ARC_LABEL_H

#include "components/ui_arc_label.h"
#include "components/ui_label.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestArcLabel : public UITest {
public:
    UITestArcLabel() {}
    ~UITestArcLabel() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    /**
     * @brief Test display arc text from 0 to 270 INSIDE
     */
    void UIKit_UIArcLabel_Test_Display_001();

    /**
     * @brief Test display arc text from 270 to 0 INSIDE
     */
    void UIKit_UIArcLabel_Test_Display_002();

    /**
     * @brief Test display arc text from 0 to 270 OUTSIDE
     */
    void UIKit_UIArcLabel_Test_Display_003();

    /**
     * @brief Test display arc text from 270 to 0 OUTSIDE
     */
    void UIKit_UIArcLabel_Test_Display_004();

    /**
     * @brief Test arc text align LEFT
     */
    void UIKit_UIArcLabel_Test_Align_001();

    /**
     * @brief Test arc text align CENTER
     */
    void UIKit_UIArcLabel_Test_Align_002();

    /**
     * @brief Test arc text align RIGHT
     */
    void UIKit_UIArcLabel_Test_Align_003();

private:
    UIScrollView* container_ = nullptr;

    void InnerTestTitle(const char* title, int16_t x, int16_t y) const;

    // for direction and orientation
    void TestArcLabelDisplay(const char* title, const int16_t startAngle, const int16_t endAngle,
        const UIArcLabel::TextOrientation orientation, int16_t x, int16_t y);
};
} // namespace OHOS
#endif // UI_TEST_ARC_LABEL_H
