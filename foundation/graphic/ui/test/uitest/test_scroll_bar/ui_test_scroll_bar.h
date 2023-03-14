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

#ifndef UI_TEST_SCROLL_BAR_H
#define UI_TEST_SCROLL_BAR_H

#include "components/text_adapter.h"
#include "components/ui_image_view.h"
#include "components/ui_list.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestScrollBar : public UITest {
public:
    UITestScrollBar() {}
    ~UITestScrollBar() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void UIKit_ScrollBar_TEST_Scroll_view_001();
    void UIKit_ScrollBar_TEST_List_001();

private:
    UIViewGroup* container_ = nullptr;
    UIScrollView* scrollView_ = nullptr;
    UIList* list_ = nullptr;
    UIImageView* foreImg1_ = nullptr;
    UIImageView* foreImg2_ = nullptr;
    List<const char*>* adapterData_ = nullptr;
    TextAdapter* adapter_ = nullptr;
}; // namespace OHOS
}
#endif // UI_TEST_SCROLL_BAR_H
