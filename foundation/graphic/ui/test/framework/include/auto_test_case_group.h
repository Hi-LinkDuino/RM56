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

#ifndef GRAPHIC_LITE_AUTO_TEST_CASE_GROUP_H
#define GRAPHIC_LITE_AUTO_TEST_CASE_GROUP_H

#include "gfx_utils/list.h"

namespace OHOS {
class AutoTestCaseGroup {
public:
    class AutoTestCase {
    public:
        AutoTestCase() {}
        virtual ~AutoTestCase() {}
        virtual void RunTestList() = 0;
        virtual void Reset() const = 0;
        virtual void ResetMainMenu() const = 0;
    };

    static List<AutoTestCase*>& GetTestCase();
    static void TearDownTestCase();
    static void AddTestCase(AutoTestCase* testCaseInfo);

private:
    static List<AutoTestCase*> testCaseList_;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_AUTO_TEST_CASE_GROUP_H
