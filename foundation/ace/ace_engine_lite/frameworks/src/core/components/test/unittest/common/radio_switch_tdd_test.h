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
#ifndef RADIO_SWITCH_TDD_TEST_H
#define RADIO_SWITCH_TDD_TEST_H
#include <cstdint>
#include "base_test.h"
namespace OHOS {
namespace ACELite {
class RadioSwitchTddTest : public BaseTest {
public:
    RadioSwitchTddTest();
    ~RadioSwitchTddTest() override {}
    void RunTests();
    void WidthRadioValueTest001();
    void WidthRadioValueTest002();
    void WidthSwitchValueTest003();
    void WidthSwitchValueTest004();
    void HeightRadioValueTest005();
    void HeightRadioValueTest006();
    void HeightSwitchValueTest007();
    void HeightSwitchValueTest008();

private:
    constexpr static uint32_t MAX_VALUE = 65555;
    constexpr static int16_t CHECK_LEN = MAX_VALUE - UINT16_MAX - 1;
    uint16_t radioComponentId_;
    uint16_t switchComponentId_;
};
}
}

#endif // RADIO_SWITCH_TDD_TEST_H
