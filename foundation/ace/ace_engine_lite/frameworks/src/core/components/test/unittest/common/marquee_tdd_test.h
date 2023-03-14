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
#ifndef ACELITE_MARQUEE_TDD_TEST_H
#define ACELITE_MARQUEE_TDD_TEST_H

#include "base_test.h"

namespace OHOS {
namespace ACELite {
class MarqueeTddTest : public BaseTest {
public:
    MarqueeTddTest()
    {
        const char *componentName = "marquee";
        SetCompnentNameId(const_cast<char *>(componentName));
    }
    ~MarqueeTddTest() override {};
    void ComponentMarqueeStyleSetTest006();
    void ComponentMarqueeStyleSetTest007();
    void ComponentMarqueeStyleSetTest008();
    void ComponentMarqueeStyleSetTest009();
    void ComponentMarqueeStyleSetTest010();
    void ComponentMarqueeStyleSetTest011();
    void ComponentMarqueeStyleSetTest012();
    void ComponentMarqueeStyleSetTest013();
    void RunTests();
};
} // namespace ACELite
} // namespace OHOS

#endif // ACELITE_MARQUEE_TDD_TEST_H
