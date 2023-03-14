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

#ifndef UI_TEST_OPACITY_H
#define UI_TEST_OPACITY_H

#include "components/root_view.h"
#include "components/text_adapter.h"
#include "components/ui_arc_label.h"
#include "components/ui_button.h"
#include "components/ui_image_animator.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_list.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"
namespace OHOS {
class UITestOpacity : public UITest {
public:
    UITestOpacity() {}
    ~UITestOpacity() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    /**
     * @brief Test UILabel
     */
    void UIKitOpacityTestUILabel001();

    /**
     * @brief Test UILabel exception value
     */
    void UIKitOpacityTestUILabel002();

    /**
     * @brief Test UIButton
     */
    void UIKitOpacityTestUIButton001();

    /**
     * @brief Test UIButton image
     */
    void UIKitOpacityTestUIButton002();

    /**
     * @brief Test UILabelButton
     */
    void UIKitOpacityTestUILabelButton001();

    /**
     * @brief Test UIArcLabel
     */
    void UIKitOpacityTestUIArcLabel001();

    /**
     * @brief Test UIImageView
     */
    void UIKitOpacityTestUIImageView001();

    /**
     * @brief Test UIImageAnimator
     */
    void UIKitOpacityTestUIImageAnimator001();

    /**
     * @brief Test UIView
     */
    void UIKitOpacityTestUIView001();

    /**
     * @brief Test UIViewGroup
     */
    void UIKitOpacityTestUIViewGroup001();

    /**
     * @brief Test UIList
     */
    void UIKitOpacityTestUIList001();

    /**
     * @brief Test UIScrollView
     */
    void UIKitOpacityTestUIScrollView001();

    /**
     * @brief Test UISwipeView
     */
    void UIKitOpacityTestUISwipeView001();

private:
    UIViewGroup* CreateTestCaseGroup(const char* title) const;
    UILabel* CreateTestCaseUILabel(const char* title, uint8_t opaScale) const;
    UIButton* CreateTestCaseUIButton(uint8_t opaScale) const;
    UILabelButton* CreateTestCaseUILabelButton(const char* title, uint8_t opaScale) const;
    UIArcLabel* CreateTestCaseUIArcLabel(const char* title, uint8_t opaScale) const;
    UIImageView* CreateTestCaseUIImageView(const char* path, uint8_t opaScale) const;
    UIImageAnimatorView* CreateTestCaseUIImageAnimator(const ImageAnimatorInfo imageAnimatorInfo[],
                                                       uint8_t opaScale) const;
    UIView* CreateTestCaseUIView(uint8_t opaScale) const;
    UIList* CreateTestCaseUIList(uint8_t opaScale) const;

    UIScrollView* container_ = nullptr;
};
}
#endif // UI_TEST_OPACITY_H
