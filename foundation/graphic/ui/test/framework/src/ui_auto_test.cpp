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

#include "ui_auto_test.h"

#include "compare_tools.h"
#include "components/root_view.h"
#include "components/ui_list.h"
#include "components/ui_view_group.h"
#include "dfx/event_injector.h"
#include "test_layout/ui_auto_test_basic_layout.h"
#include "test_render/ui_auto_test_render.h"
#include "ui_test_app.h"
#include "ui_test_group.h"

namespace OHOS {
void UIAutoTest::ResetMainMenu() const
{
    while (RootView::GetInstance()->GetChildById(UI_TEST_MAIN_LIST_ID) == nullptr) {
        ClickViewById(UI_TEST_BACK_BUTTON_ID);
    }
}

void UIAutoTest::EnterSubMenu(const char* id) const
{
    if (id == nullptr) {
        return;
    }
    UIView* view = RootView::GetInstance()->GetChildById(id);
    if (view == nullptr) {
        UIView* view = RootView::GetInstance()->GetChildById(UI_TEST_MAIN_LIST_ID);
        if (view == nullptr) {
            return;
        }
        ListNode<TestCaseInfo>* node = UITestGroup::GetTestCase().Begin();
        while (node != UITestGroup::GetTestCase().End()) {
            if ((node->data_.sliceId != nullptr) && (strcmp(id, node->data_.sliceId) == 0)) {
                UITestGroup::GetTestCase().PushFront(node->data_);
                UITestGroup::GetTestCase().Remove(node);
                break;
            }
            node = node->next_;
        }
        reinterpret_cast<UIList*>(view)->RefreshList();
        CompareTools::WaitSuspend();
    }
    ClickViewById(id);
}

void UIAutoTest::ClickViewById(const char* id) const
{
    if (id == nullptr) {
        return;
    }
    UIView* view = RootView::GetInstance()->GetChildById(id);
    if (view == nullptr) {
        return;
    }
    Point point;
    point.x = view->GetOrigRect().GetX();
    point.y = view->GetOrigRect().GetY();
    EventInjector::GetInstance()->SetClickEvent(point);
    CompareTools::WaitSuspend();
}

void UIAutoTest::DragViewToHead(const char* id) const
{
    if (id == nullptr) {
        return;
    }
    UIView* view = RootView::GetInstance()->GetChildById(id);
    if (view == nullptr) {
        return;
    }
    Point startPoint;
    startPoint.x = view->GetOrigRect().GetX();
    startPoint.y = view->GetOrigRect().GetY();

    Point endPoint;
    endPoint.x = 100; // 100 :end point x position;
    endPoint.y = 100; // 100 :end point y position;
    EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, 300); // 300: drag time
    CompareTools::WaitSuspend();
}

void UIAutoTest::CompareByBinary(const char* fileName) const
{
    if (fileName == nullptr) {
        return;
    }
    char filePath[DEFAULT_FILE_NAME_MAX_LENGTH] = {0};
    CompareTools::StrnCatPath(filePath, DEFAULT_FILE_NAME_MAX_LENGTH, fileName, strlen(fileName));
    if (CompareTools::CheckFileExist(filePath, sizeof(filePath))) {
        CompareTools::CompareFile(filePath, sizeof(filePath));
    } else {
        CompareTools::SaveFile(filePath, sizeof(filePath));
    }
}

void UIAutoTest::SetUpTestCase()
{
    AutoTestCaseGroup::AddTestCase(new UIAutoTestRender());
    AutoTestCaseGroup::AddTestCase(new UIAutoTestBasicLayout());
}
} // namespace OHOS
