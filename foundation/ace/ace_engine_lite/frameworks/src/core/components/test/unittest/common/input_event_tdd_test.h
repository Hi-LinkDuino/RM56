/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef ACELITE_INPUT_EVENT_TDD_TEST_H
#define ACELITE_INPUT_EVENT_TDD_TEST_H
#include "basic_tdd_test.h"
namespace OHOS {
namespace ACELite {
class InputEventTddTest : public BasicTddTest {
public:
    void ClickPosition(const double xRate, const double yRate) const;
    void LongPressPosition(const double xRate, const double yRate) const;
    UIView *GetComponent(JSValue page, const char *name) const;
    void InputBindingTest001();
    void InputBindingTest001Extra(JSValue page);
    void InputEventTest002();
    void ChangeEventTest003();
    void CheckboxEventTest004();
    void ChangeEventTest005();
    void InputIfTest006();
    void InputShowTest007();
    void InputForTest008();
    void RadioTest009();
    void InputTest010();
    void CheckboxChangeClick011();
    void RadioChangeClick012();
};
}
}
#endif
