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

#ifndef OHOS_ACELITE_TEST_DIALOG_H
#define OHOS_ACELITE_TEST_DIALOG_H

#include "basic_tdd_test.h"

namespace OHOS {
namespace ACELite {
class DialogTddTest : public BasicTddTest {
public:
    DialogTddTest() : BasicTddTest() {}
    ~DialogTddTest() override {}

    void DialogTest001();
    void DialogTest002();
    void DialogTest003();
    void DialogTest004();
    void DialogTest005();
    void RunTests();

private:
    bool CallPageMethod(JSValue page, const char * const prop) const;
    static const char * const BUNDLE1;
    static const char * const BUNDLE2;
    static const char * const BUNDLE3;
    static const char * const BUNDLE4;
    static const char * const BUNDLE5;
    static const char * const METHOD;
    static const char * const CONFIRM;
    static const char * const CANCEL;
    static const char * const COMPLETE;
    static const char * const MSG;
    static const char * const DEFAULT;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_TEST_DIALOG_H
