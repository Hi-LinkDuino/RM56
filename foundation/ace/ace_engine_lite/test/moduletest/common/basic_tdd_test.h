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

#ifndef OHOS_ACELITE_BASIC_TDD_TEST_CONSTANTS_H
#define OHOS_ACELITE_BASIC_TDD_TEST_CONSTANTS_H

#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#else
#include <typeinfo.h>
#endif
#include "component.h"
#include "dock/screen_device_proxy.h"
#include "test_common.h"
#include "window/window.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class BasicTddTest : public testing::Test {
#else
class BasicTddTest {
#endif
public:
    BasicTddTest();
    virtual ~BasicTddTest();
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

protected:
    JSValue RunJS(const char *bundle, const uint32_t size) const;
    JSValue CreatePage(const char *bundle, const uint32_t size);
    void DestroyPage(JSValue page);
    UIView *GetViewByRef(JSValue page, const char *ref) const;
    void ClickByRef(JSValue page, const char *ref, uint8_t sleepTicks = 50) const;
    void LongPressByRef(JSValue page, const char *ref, uint8_t sleepTicks = 200) const;
    void Click(const UIView *view, uint8_t sleepTicks = 50) const;
    void LongPress(const UIView *view, uint8_t sleepTicks = 200) const;
    void Click(int16_t x, int16_t y, uint8_t sleepTicks = 50) const;
    void LongPress(int16_t x, int16_t y, uint8_t sleepTicks = 200) const;
    void Swipe(int16_t startX, int16_t startY, int16_t endX, int16_t endY, uint8_t sleepTicks = 150) const;
    uint8_t GetChildrenSize(const UIViewGroup &grop) const;

private:
    static void *TickHandler(void *args);
    static ScreenDevice *display_;
    static Window *window_;
    static pthread_t tickThread_;
    static bool isTickTaskRunning_;
    void CallLifecycle(JSValue page, const char *lifecycle) const;

    Component *rootComponent_;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_BASIC_TDD_TEST_CONSTANTS_H
