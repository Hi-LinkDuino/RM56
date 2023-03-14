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

#ifndef OHOS_ACELITE_TEST_JSBUNDLE_H
#define OHOS_ACELITE_TEST_JSBUNDLE_H

#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#else
#include <typeinfo.h>
#endif

#include "test_common.h"

namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class JsbundleTddTest : public testing::Test {
#else
class JsbundleTddTest {
#endif
public:
    JsbundleTddTest();
    ~JsbundleTddTest(){}

    void SetUp();
    void TearDown();
    void CreateElementAttributeLength001();
    void CreateElementAttributeLength002();
    void CreateElementAttributeLength003();
    void CreateElementAttributeLength004();
    void CreateElementTagValidation005();
    void CreateElementTagValidation006();
    void CreateElementTagValidation007();
    void CreateElement2ndAttributeValidation008();
    void CreateElement2ndAttributeValidation009();
    void CreateElement2ndAttributeValidation010();
    void CreateElement2ndAttributeValidation011();
    void CreateElementChildrenValidation012();
    void CreateElementChildrenValidation013();
    void RunTests();
};
} // namespace ACELite
} // namesapce OHOS

#endif // OHOS_ACELITE_TEST_JSBUNDLE_H
