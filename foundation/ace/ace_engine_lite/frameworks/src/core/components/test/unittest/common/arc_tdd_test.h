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

#ifndef OHOS_ACELITE_TEST_ARC_H
#define OHOS_ACELITE_TEST_ARC_H

#include "base_test.h"
#include "circle_progress_component.h"
#include "ui_abstract_progress.h"
#include "ui_circle_progress.h"

namespace OHOS {
namespace ACELite {
class ArcTddTest : public BaseTest {
public:
    void SetCircleType();
    int16_t GetPercentage(const UICircleProgress &circleProgress) const;
    void PrintExpectInfo(const ColorType colorType, int32_t lineColor) const;
    void ComponentProgressAttributeSetTest001();
    void ComponentProgressAttributeSetTest002();
    void ComponentProgressAttributeSetTest003();
    void ComponentProgressStyleSetTest001();
    void ComponentProgressStyleSetTest002();
    void ComponentProgressStyleSetTest003();
    void ComponentProgressStyleSetTest004();
    void ComponentProgressStyleSetTest005();
    void ComponentProgressStyleSetTest006();
    void ComponentProgressStyleSetTest008();
    void ComponentProgressStyleSetTest009();
    void ComponentProgressStyleSetTest010();
    void ComponentProgressStyleSetTest011();
    void ComponentProgressStyleSetTest012();
    void ComponentProgressStyleSetTest013();
    void ComponentProgressStyleSetTest014();
    void ComponentProgressStyleSetTest015();
    void RunTests();
};
}
}
#endif // OHOS_ACELITE_TEST_ARC_H
