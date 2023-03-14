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

#ifndef OHOS_ACELITE_TEST_DFX_H
#define OHOS_ACELITE_TEST_DFX_H

#include "basic_tdd_test.h"

namespace OHOS {
namespace ACELite {
class DfxTddTest : public BasicTddTest {
public:
    DfxTddTest() : BasicTddTest() {}
    ~DfxTddTest() override {}

    void DfxTest001();
    void DfxTest002();
    void DfxTest003();
    void DfxTest004();
    void DfxTest005();
    void DfxTest006();
    void DfxTest007();
    void RunTests();
private:
    static const char * const BUNDLE1;
    static const char * const BUNDLE2;
    static const char * const BUNDLE3;
    static const char * const BUNDLE4;
    static const char * const BUNDLE5;
    static const char * const BUNDLE6;
    static const char * const BUNDLE7;
    static const char * const FUNC_NAME;
    static const char * const DOM_NODE_FUNC_NAME;
    static const char * const DOM_TREE_FUNC_NAME;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_TEST_DFX_H
