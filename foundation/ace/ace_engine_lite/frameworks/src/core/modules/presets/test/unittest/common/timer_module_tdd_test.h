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

#ifndef TIMER_MODULE_TEST_H
#define TIMER_MODULE_TEST_H
#include "acelite_config.h"
#include "basic_tdd_test.h"
#include "test_common.h"

namespace OHOS {
namespace ACELite {
class TimerModuleTddTest : public BasicTddTest {
public:
    void SetTimeoutTest001();

    void SetIntervalTest002();

    void SetTimeoutTest003();

    void SetTimeoutTest004();

    void SetIntervalTest005();

    void SetTimeoutTest006();

    void SetIntervalTest007();

    void ClearTimeoutTest008();

    void ClearIntervalTest009();

    void TimerLimitTest010();

    char *TriggerTimer(const double xRate, const double yRate, const uint8_t sleepTime, const JSValue page) const;

#ifndef TDD_ASSERTIONS
    void RunTests();
#endif
};
}
}
#endif // TIMER_MODULE_TEST_H
