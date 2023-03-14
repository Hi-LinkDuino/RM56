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
#ifndef OHOS_ACELITE_TEST_EVENT_BUBBLE_H
#define OHOS_ACELITE_TEST_EVENT_BUBBLE_H

#include "basic_tdd_test.h"

namespace OHOS {
namespace ACELite {
class EventBubbleTddTest : public BasicTddTest {
public:
    EventBubbleTddTest() : BasicTddTest() {}
    ~EventBubbleTddTest() override {}

    void EventBubbleTest001();
    void EventBubbleTest002();
    void EventBubbleTest003();
    void EventBubbleTest004();
    void EventBubbleTest005();
    void EventBubbleTest006();
    void EventBubbleTest007();
    void EventBubbleTest008();
    void EventBubbleTest009();
    void EventBubbleTest010();
    void EventBubbleTest011();
    void EventBubbleTest012();
    void EventBubbleTest013();
    void EventBubbleTest014();
    void EventBubbleTest015();
    void EventBubbleTest016();
    void EventBubbleTest017();
    void RunTests();
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_TEST_EVENT_BUBBLE_H
