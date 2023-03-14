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

#include "auto_test_app.h"
#include "auto_test_case_group.h"
#include "compare_tools.h"
#include "dfx/event_injector.h"
#if ENABLE_WINDOW
#include "window/window.h"
#endif

namespace OHOS {
AutoTestApp* AutoTestApp::GetInstance()
{
    static AutoTestApp instance;
    return &instance;
}

void AutoTestApp::Start()
{
    EventInjector::GetInstance()->RegisterEventInjector(EventDataType::POINT_TYPE);
    EventInjector::GetInstance()->RegisterEventInjector(EventDataType::KEY_TYPE);

#if ENABLE_WINDOW
    Window* window = RootView::GetInstance()->GetBoundWindow();
    if (window != nullptr) {
        EventInjector::GetInstance()->SetWindowId(window->GetWindowId());
    }
#endif
    CompareTools::WaitSuspend();

    ListNode<AutoTestCaseGroup::AutoTestCase*>* node = AutoTestCaseGroup::GetTestCase().Begin();
    while (node != AutoTestCaseGroup::GetTestCase().End()) {
        node->data_->RunTestList();
        node->data_->ResetMainMenu();
        node = node->next_;
    }
}

AutoTestApp::~AutoTestApp()
{
    if (EventInjector::GetInstance()->IsEventInjectorRegistered(EventDataType::POINT_TYPE)) {
        EventInjector::GetInstance()->UnregisterEventInjector(EventDataType::POINT_TYPE);
    }
    if (EventInjector::GetInstance()->IsEventInjectorRegistered(EventDataType::KEY_TYPE)) {
        EventInjector::GetInstance()->UnregisterEventInjector(EventDataType::KEY_TYPE);
    }
    AutoTestCaseGroup::TearDownTestCase();
}
} // namespace OHOS
