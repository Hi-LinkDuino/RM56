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

#include "auto_test_case_group.h"
#include "graphic_config.h"

namespace OHOS {
List<AutoTestCaseGroup::AutoTestCase*> AutoTestCaseGroup::testCaseList_;

List<AutoTestCaseGroup::AutoTestCase*>& AutoTestCaseGroup::GetTestCase()
{
    return testCaseList_;
}

void AutoTestCaseGroup::TearDownTestCase()
{
    ListNode<AutoTestCase*>* node = testCaseList_.Begin();
    while (node != testCaseList_.End()) {
        delete node->data_;
        node->data_ = nullptr;
        node = node->next_;
    }
    testCaseList_.Clear();
}

void AutoTestCaseGroup::AddTestCase(AutoTestCase* testCaseInfo)
{
    if (testCaseInfo != nullptr) {
        testCaseList_.PushBack(testCaseInfo);
    }
}
} // namespace OHOS
