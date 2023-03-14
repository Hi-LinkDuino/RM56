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

#ifndef UI_TEST_GROUP_H
#define UI_TEST_GROUP_H

#include "components/ui_view.h"
#include "gfx_utils/list.h"

namespace OHOS {
class UITest;
struct TestCaseInfo {
    const char* sliceId;
    UITest* testObj;
};

class UITestGroup {
public:
    static void SetUpTestCase();
    static List<TestCaseInfo>& GetTestCase();
    UITest* GetTestCase(const char* id);
    static void TearDownTestCase();
    static void AddTestCase(TestCaseInfo testCaseInfo);

private:
    static List<TestCaseInfo> testCaseList_;
};
} // namespace OHOS
#endif
