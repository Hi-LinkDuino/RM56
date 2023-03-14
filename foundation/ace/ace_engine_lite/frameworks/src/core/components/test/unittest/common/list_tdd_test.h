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

#ifndef OHOS_ACELITE_TEST_LIST_H
#define OHOS_ACELITE_TEST_LIST_H

#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#else
#include <typeinfo.h>
#endif
#include "component_factory.h"
#include "test_common.h"

namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class ListTddTest : public testing::Test {
#else
class ListTddTest {
#endif
public:
    ListTddTest();
    ~ListTddTest(){}
    void SetUp();
    void TearDown();
    void ListTest001();
    void ListTest002();
    void ListTest003();
    void ListTest004();
    void RunTests();

    jerry_value_t objGlob;
    jerry_value_t objAttrs;
    jerry_value_t objStaticStyle;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_TEST_LIST_H
