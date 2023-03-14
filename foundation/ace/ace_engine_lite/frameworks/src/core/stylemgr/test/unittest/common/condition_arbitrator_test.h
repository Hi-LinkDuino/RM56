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

#ifndef OHOS_ACELITE_CONDITION_ARBITRATOR_TEST_H
#define OHOS_ACELITE_CONDITION_ARBITRATOR_TEST_H
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
class ConditionArbitratorTest : public testing::Test {
#else
class ConditionArbitratorTest {
#endif
public:
    void SetUp();
    void TearDown() {}
    void MaxWidthTest001();
    void MinWidthTest002();
    void WidthTest003();
    void MaxHeightTest004();
    void MinHeightTest005();
    void HeightTest006();
    void MaxAspectRatioTest007();
    void MinAspectRatioTest008();
    void AspectRatioTest009();
    void AspectRatioTest010();
    void DeviceTypeTest011();
    void RoundScreenTest012();
    void CombineConditionTest013();
    void CombineConditionTest014();

#ifndef TDD_ASSERTIONS
    void RunTests();
#endif
};
}
}
#endif
