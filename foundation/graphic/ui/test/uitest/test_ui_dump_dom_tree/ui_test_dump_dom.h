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

#ifndef GRAPHIC_LITE_UI_TEST_DUMP_DOM_H
#define GRAPHIC_LITE_UI_TEST_DUMP_DOM_H

#include "graphic_config.h"

#if ENABLE_DEBUG
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "layout/grid_layout.h"
#include "ui_test.h"

namespace OHOS {
class UITestDumpDomTree : public UITest {
public:
    UITestDumpDomTree() {}
    ~UITestDumpDomTree() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void CreateButtons(GridLayout* layout);

    void UIKit_TestDumpDom_001();
    void UIKit_TestDumpDom_002();
    void UIKit_TestDumpDom_003();
    void UIKit_TestDumpDom_004();
    void UIKit_TestDumpDom_005();
    void UIKit_TestDumpDom_006();
    UILabelButton* GetLabelButton(const char* buttonname);

private:
    UIScrollView* container_ = nullptr;
    UIView::OnClickListener* clickDumpDomListener1_ = nullptr;
    UIView::OnClickListener* clickDumpDomListener2_ = nullptr;
    UIView::OnClickListener* clickDumpDomListener3_ = nullptr;
    UIView::OnClickListener* clickDumpDomListener4_ = nullptr;
    UIView::OnClickListener* clickDumpDomListener5_ = nullptr;
    UIView::OnClickListener* clickDumpDomListener6_ = nullptr;
    UIView::OnClickListener* clickDumpDomListener7_ = nullptr;
    UIView::OnClickListener* clickDumpDomListener8_ = nullptr;
    UIView::OnClickListener* clickDumpDomListener9_ = nullptr;
    UIView::OnClickListener* clickDumpDomListener10_ = nullptr;
    UIView::OnClickListener* clickDumpDomListener11_ = nullptr;
    UIView::OnClickListener* clickDumpDomListener12_ = nullptr;
};
}
#endif // ENABLE_DEBUG
#endif // GRAPHIC_LITE_UI_TEST_DUMP_DOM_H

