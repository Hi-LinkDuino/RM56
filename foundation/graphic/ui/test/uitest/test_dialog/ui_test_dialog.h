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
#ifndef UI_TEST_DIALOG_H
#define UI_TEST_DIALOG_H

#include "components/ui_dialog.h"
#include "components/ui_scroll_view.h"
#include "graphic_config.h"
#include "ui_test.h"

#if ENABLE_WINDOW
namespace OHOS {
class TestUIDialogButtonListener;
class TestUIDialogOnCancelListener;

class UITestDialog : public UITest, public UIView::OnClickListener {
public:
    UITestDialog() {}
    ~UITestDialog() {}
    void SetUp() override;
    void TearDown() override;
    void InnerTestTitle(const char* title);
    const UIView* GetTestView() override;

    bool OnClick(UIView &view, const ClickEvent& event) override;
    bool ClickExpand(UIView &view, const ClickEvent& event);

    void UIKitDialogTest001();
    void UIKitDialogTest002();
    void UIKitDialogTest003();
    void UIKitDialogTest004();
    void UIKitDialogTest005();
    void UIKitDialogTest006();
    void UIKitDialogTest007();
    void UIKitDialogTest008();
    void UIKitDialogTest009();
    void UIKitDialogTest010();

private:
    UIScrollView* container_ = nullptr;
    UILabel* label_ = nullptr;
    UILabelButton* button1_ = nullptr;
    UILabelButton* button2_ = nullptr;
    UILabelButton* button3_ = nullptr;
    UILabelButton* button4_ = nullptr;
    UILabelButton* button5_ = nullptr;
    UILabelButton* button6_ = nullptr;
    UILabelButton* button7_ = nullptr;
    UILabelButton* button8_ = nullptr;
    UILabelButton* button9_ = nullptr;
    UILabelButton* button10_ = nullptr;
    UIDialog* dialog_ = nullptr;
    TestUIDialogButtonListener* listener1_ = nullptr;
    TestUIDialogButtonListener* listener2_ = nullptr;
    TestUIDialogButtonListener* listener3_ = nullptr;
    TestUIDialogOnCancelListener* listener_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_DIALOG_H
#endif // ENABLE_WINDOW
