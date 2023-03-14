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

#ifndef OHOS_ACELITE_ROUTER_MODULE_TDD_TEST_H
#define OHOS_ACELITE_ROUTER_MODULE_TDD_TEST_H

#include "acelite_config.h"

#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#endif
#include "js_fwk_common.h"
#include "js_page_state_machine.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class RouterModuleTddTest : public testing::Test {
#else
class RouterModuleTddTest {
#endif
public:
    RouterModuleTddTest() = default;
    virtual ~RouterModuleTddTest() = default;
    void SetUp();
    void TearDown();
    void RouterModuleTest001();
    void RouterModuleTest002();
    void RouterModuleTest003();
    void RouterModuleTest004();
    void RouterModuleTest005();
    void RouterModuleTest006();
    void RouterModuleTest007();
    void RouterModuleTest008();
    void RouterModuleTest009();
    void RouterModuleTest010();
    void RouterModuleTest011();
    void RouterModuleTest012();
    void RouterModuleTest013();
    void ReleaseJSValue(JSValue &value);
    void HookViewModel(StateMachine &sm, const char *jsBundle, size_t bundleLength);
    void PrepareRouterStateMachine(StateMachine &sm, JSValue &routerParam);
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_ROUTER_MODULE_TDD_TEST_H
