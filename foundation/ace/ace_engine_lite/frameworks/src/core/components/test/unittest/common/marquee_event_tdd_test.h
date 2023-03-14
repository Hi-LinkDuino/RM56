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
#ifndef ACELITE_MARQUEE_EVENT_TDD_TEST_H
#define ACELITE_MARQUEE_EVENT_TDD_TEST_H

#include "basic_tdd_test.h"

namespace OHOS {
namespace ACELite {
class MarqueeEventTddTest : public BasicTddTest {
public:
    MarqueeEventTddTest() : BasicTddTest() {}
    ~MarqueeEventTddTest() override {}
    void MarqueeEvent001();
    void RunTests();

private:
    static const char * const BUNDLE;
    static const char * const FUNC_NAME;
};
} // namespace ACELite
} // namespace OHOS

#endif // ACELITE_MARQUEE_EVENT_TDD_TEST_H
