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

#ifndef UI_AUTO_TEST_BASIC_LAYOUT_H
#define UI_AUTO_TEST_BASIC_LAYOUT_H

#include "ui_auto_test.h"

namespace OHOS {
class UIAutoTestBasicLayout : public UIAutoTest {
public:
    UIAutoTestBasicLayout() {}
    ~UIAutoTestBasicLayout() {}

    void Reset() const;
    void RunTestList();

    void UIKitBasicLayoutTestMargin001() const;
    void UIKitBasicLayoutTestPadding001() const;
    void UIKitBasicLayoutTestCenterOfParent001() const;
    void UIKitBasicLayoutTestLeftOfParent001() const;
    void UIKitBasicLayoutTestRightOfParent001() const;
    void UIKitBasicLayoutTestTopOfParent001() const;
    void UIKitBasicLayoutTestBottomOfParent001() const;
    void UIKitBasicLayoutTestLeftOfSibling001() const;
    void UIKitBasicLayoutTestRightOfSibling001() const;
    void UIKitBasicLayoutTestTopOfSibling001() const;
    void UIKitBasicLayoutTestBottomOfSibling001() const;
    void UIKitBasicLayoutTestHorCenterOfSibling001() const;
    void UIKitBasicLayoutTestVerCenterOfSibling001() const;
    void UIKitBasicLayoutTestLeftToSibling001() const;
    void UIKitBasicLayoutTestRightToSibling001() const;
    void UIKitBasicLayoutTestTopToSibling001() const;
    void UIKitBasicLayoutTestBottomToSibling001() const;
};
} // namespace OHOS
#endif // UI_AUTO_TEST_BASIC_LAYOUT_H
