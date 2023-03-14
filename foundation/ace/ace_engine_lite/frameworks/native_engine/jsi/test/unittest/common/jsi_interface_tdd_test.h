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

#ifndef ACELITE_JSI_INTERFACE_TDD_TEST_H
#define ACELITE_JSI_INTERFACE_TDD_TEST_H

#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#endif

#include "acelite_config.h"
#include "jerryscript.h"
#include "js_config.h"
#include "jsi.h"
#include "test_common.h"

namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class JsiInterfaceTddTest : public testing::Test {
#else
class JsiInterfaceTddTest {
#endif
public:
    JsiInterfaceTddTest();
    ~JsiInterfaceTddTest(){}
    void SetUp();
    void TearDown();
    static void OnDestroy(int8_t statusCode);
    static void OnTerminate(int8_t statusCode);
    static JSIValue Setter(const JSIValue thisVal, const JSIValue args[], uint8_t argsNum);
    static JSIValue Getter(const JSIValue thisVal, const JSIValue args[], uint8_t argsNum);
    static JSIValue Function(const JSIValue thisVal, const JSIValue args[], uint8_t argsNum);
    void JSIInterfaceTest001();
    void JSIInterfaceTest002();
    void JSIInterfaceTest003();
    void JSIInterfaceTest004();
    void JSIInterfaceTest005();
    void JSIInterfaceTest006();
    void JSIInterfaceTest007();
    void JSIInterfaceTest008();
    void JSIInterfaceTest009();
    void JSIInterfaceTest010();
#if IS_ENABLED(JS_FWK_SYMBOL)
    void JSIInterfaceTest011();
#endif // ENABLED(JS_FWK_SYMBOL)
    void JSIInterfaceTest012();
    void JSIInterfaceTest013();
    void JSIInterfaceTest014();
    void JSIInterfaceTest015();
    void JSIInterfaceTest016();
    void JSIInterfaceTest017();
#if IS_ENABLED(JS_FWK_TYPEDARRAY)
    void JSIInterfaceTest018();
    void JSIInterfaceTest019();
#endif // ENABLED(JS_FWK_TYPEDARRAY)
    void JSIInterfaceTest020();
    void JSIInterfaceTest021();
    void JSIInterfaceTest022();
    void JSIInterfaceTest023();
    void RunTests();
};
} // namespace ACELite
} // namespace OHOS

#endif // ACELITE_JSI_INTERFACE_TDD_TEST_H
