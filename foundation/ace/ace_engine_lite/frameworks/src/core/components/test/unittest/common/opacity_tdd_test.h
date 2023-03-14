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
#ifndef OHOS_ACELITE_TEST_OPACITY_H
#define OHOS_ACELITE_TEST_OPACITY_H

#include "base_test.h"

namespace OHOS {
namespace ACELite {
class OpacityTddTest : public BaseTest {
public:
    OpacityTddTest();
    ~OpacityTddTest() override {};
    void SetOpacityTest001();
    void SetOpacityTest002();
    void SetOpacityTest003();
    void SetOpacityTest004();
    void SetOpacityTest005();
    void SetOpacityTest006();
    void SetOpacityTest007();
    void RunTests();

private:
    void SetOpaScale(jerry_value_t styleObj, double inputOpacityValue);
    UIView* GetContainerView(uint16_t componentKeyId, jerry_value_t options, jerry_value_t children) const;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_TEST_TEXT_H
