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

#include "ui_auto_test_basic_layout.h"
#include "ui_test_basic_layout.h"

namespace OHOS {
void UIAutoTestBasicLayout::Reset() const
{
    ResetMainMenu();
    EnterSubMenu(UI_TEST_BASIC_LAYOUT_ID);
}

void UIAutoTestBasicLayout::RunTestList()
{
    Reset();
    UIKitBasicLayoutTestMargin001();
    UIKitBasicLayoutTestPadding001();
    UIKitBasicLayoutTestCenterOfParent001();
    UIKitBasicLayoutTestLeftOfParent001();
    UIKitBasicLayoutTestRightOfParent001();
    UIKitBasicLayoutTestTopOfParent001();
    UIKitBasicLayoutTestBottomOfParent001();
    UIKitBasicLayoutTestLeftOfSibling001();
    UIKitBasicLayoutTestRightOfSibling001();
    UIKitBasicLayoutTestTopOfSibling001();
    UIKitBasicLayoutTestBottomOfSibling001();
    UIKitBasicLayoutTestHorCenterOfSibling001();
    UIKitBasicLayoutTestVerCenterOfSibling001();
    UIKitBasicLayoutTestLeftToSibling001();
    UIKitBasicLayoutTestRightToSibling001();
    UIKitBasicLayoutTestTopToSibling001();
    UIKitBasicLayoutTestBottomToSibling001();
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestMargin001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_MARGIN_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_LEFTOFPARENT_BTN_ID);
    const char* fileName = "ui_test_basic_layout_margin_top_left_001.bmp";
    CompareByBinary(fileName);
    ClickViewById(UI_TEST_BASIC_LAYOUT_RIGHTOFPARENT_BTN_ID);
    const char* fileName2 = "ui_test_basic_layout_margin_top_right_001.bmp";
    CompareByBinary(fileName2);
    ClickViewById(UI_TEST_BASIC_LAYOUT_BOTTOMOFPARENT_BTN_ID);
    const char* fileName3 = "ui_test_basic_layout_margin_bottom_right_001.bmp";
    CompareByBinary(fileName3);
    ClickViewById(UI_TEST_BASIC_LAYOUT_RIGHTOFPARENT_BTN_ID);
    const char* fileName4 = "ui_test_basic_layout_margin_bottom_left_001.bmp";
    CompareByBinary(fileName4);
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestPadding001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_PADDING_BTN_ID);
    const char* fileName = "ui_test_basic_layout_padding_001.bmp";
    CompareByBinary(fileName);
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestCenterOfParent001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_CENTEROFPARENT_BTN_ID);
    const char* fileName = "ui_test_basic_layout_center_of_parent_001.bmp";
    CompareByBinary(fileName);
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestLeftOfParent001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_LEFTOFPARENT_BTN_ID);
    const char* fileName = "ui_test_basic_layout_left_of_parent_001.bmp";
    CompareByBinary(fileName);
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestRightOfParent001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_RIGHTOFPARENT_BTN_ID);
    const char* fileName = "ui_test_basic_layout_right_of_parent_001.bmp";
    CompareByBinary(fileName);
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestTopOfParent001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_TOPOFPARENT_BTN_ID);
    const char* fileName = "ui_test_basic_layout_top_of_parent_001.bmp";
    CompareByBinary(fileName);
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestBottomOfParent001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_BOTTOMOFPARENT_BTN_ID);
    const char* fileName = "ui_test_basic_layout_bottom_of_parent_001.bmp";
    CompareByBinary(fileName);
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestLeftOfSibling001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_LEFTOFSIBLING_BTN_ID);
    const char* fileName = "ui_test_basic_layout_left_of_sibling_001.bmp";
    CompareByBinary(fileName);
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestRightOfSibling001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_RIGHTOFSIBLING_BTN_ID);
    const char* fileName = "ui_test_basic_layout_right_of_sibling_001.bmp";
    CompareByBinary(fileName);
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestTopOfSibling001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_TOPOFSIBLING_BTN_ID);
    const char* fileName = "ui_test_basic_layout_top_of_sibling_001.bmp";
    CompareByBinary(fileName);
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestBottomOfSibling001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_BOTTOMOFSIBLING_BTN_ID);
    const char* fileName = "ui_test_basic_layout_bottom_of_sibling_001.bmp";
    CompareByBinary(fileName);
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestHorCenterOfSibling001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_HORCENTEROFSIBLING_BTN_ID);
    const char* fileName = "ui_test_basic_layout_hor_center_of_sibling_001.bmp";
    CompareByBinary(fileName);
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestVerCenterOfSibling001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_VERCENTEROFSIBLING_BTN_ID);
    const char* fileName = "ui_test_basic_layout_ver_center_of_sibling_001.bmp";
    CompareByBinary(fileName);
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestLeftToSibling001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_LEFTTOSIBLING_BTN_ID);
    const char* fileName = "ui_test_basic_layout_left_to_sibling_001.bmp";
    CompareByBinary(fileName);
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestRightToSibling001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_RIGHTTOSIBLING_BTN_ID);
    const char* fileName = "ui_test_basic_layout_right_to_sibling_001.bmp";
    CompareByBinary(fileName);
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestTopToSibling001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_TOPTOSIBLING_BTN_ID);
    const char* fileName = "ui_test_basic_layout_top_to_sibling_001.bmp";
    CompareByBinary(fileName);
}

void UIAutoTestBasicLayout::UIKitBasicLayoutTestBottomToSibling001() const
{
    ClickViewById(UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    ClickViewById(UI_TEST_BASIC_LAYOUT_BOTTOMTOSIBLING_BTN_ID);
    const char* fileName = "ui_test_basic_layout_bottom_to_sibling_001.bmp";
    CompareByBinary(fileName);
}
} // namespace OHOS
