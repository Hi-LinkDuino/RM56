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

#ifndef OHOS_ACELITE_TEST_HORIZON_H
#define OHOS_ACELITE_TEST_HORIZON_H

#include "component_factory.h"
#include "root_component_mock.h"
#include "test_common.h"

#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#else
#include <typeinfo.h>
#endif

namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class HorizonProgressTddTest : public testing::Test {
#else
class HorizonProgressTddTest {
#endif
public:
    HorizonProgressTddTest();
    ~HorizonProgressTddTest(){}
    void SetUp();
    void TearDown();
    void HorizonProgressTest001();
    void HorizonProgressTest002();
    void HorizonProgressTest003();
    void HorizonProgressTest004();
    void HorizonProgressTest005();
    void HorizonProgressTest006();
    void HorizonProgressTest007();
    void HorizonProgressTest008();
    void HorizonProgressTest009();
    void HorizonProgressTest013();
    void HorizonProgressTest014();
    void HorizonProgressTest015();
    void HorizonProgressTest016();
    void HorizonProgressTest017();
    void RunTests();

    jerry_value_t objGlob;
    jerry_value_t objAttrs;
    jerry_value_t objStaticStyle;

private:
    RootComponentMock rootComponentMock_;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_TEST_HORIZON_H
