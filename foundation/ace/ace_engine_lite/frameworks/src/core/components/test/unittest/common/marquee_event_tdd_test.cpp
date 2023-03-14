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

#include "marquee_event_tdd_test.h"

namespace OHOS {
namespace ACELite {
const char  * const MarqueeEventTddTest::BUNDLE =
    "(function () { "
    "  return new ViewModel({ "
    "    render : function (vm) {"
    "      var _vm = vm || this;"
    "        return _c('stack', {"
    "          staticStyle : {width : 454,height : 454},"
    "        },"
    "        [_c('text', {"
    "           attrs : {value : 'marquee1'},"
    "           staticStyle : {left : 170,top : 30,width : 300,height : 40}}),"
    "         _c('marquee', {"
    "           attrs : {scrollamount : function () {return _vm.speed}, value : function () {return _vm.text}},"
    "           staticClass : ['textStyle'],"
    "           staticStyle : {left : 50, top : 230, width : 256, height : 60},"
    "           onBubbleEvents : {'click' : _vm.changeSpeed}})"
    "        ]);"
    "    },"
    "    styleSheet : {"
    "      classSelectors : {"
    "        'textStyle' : {"
    "          color : 16711680"
    "        }"
    "      }"
    "    },"
    "    data : {text : 'Hello JS, we are friends.', speed : 1, flag : true},"
    "    changeSpeed : function changeSpeed() {"
    "      if (this.flag) {"
    "        this.speed = 5;"
    "        this.flag = false; "
    "      } else { "
    "        this.speed = 50;"
    "        this.flag = true;"
    "      }"
    "    }"
    "  });"
    "})();";

const char * const MarqueeEventTddTest::FUNC_NAME = "changeSpeed";

void MarqueeEventTddTest::MarqueeEvent001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE, strlen(BUNDLE));

    // first check default speed
    JSValue value = JSObject::Get(page, "speed");
    double speed = JSNumber::Value(value);
    EXPECT_EQ((uint8_t)speed, 1);
    jerry_release_value(value);

    // speed value
    const uint8_t speedVal1 = 5;
    const uint8_t speedVal2 = 50;

    // click change speed
    JSValue ret = JSObject::Call(page, FUNC_NAME);
    value = JSObject::Get(page, "speed");
    speed = JSNumber::Value(value);
    if (jerry_value_is_error(ret)) {
        EXPECT_TRUE((uint8_t)speed == speedVal1);
        jerry_release_value(value);
        jerry_release_value(ret);
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }
    EXPECT_TRUE((uint8_t)speed == speedVal1);

    jerry_release_value(value);

    // click again
    ret = JSObject::Call(page, FUNC_NAME);
    value = JSObject::Get(page, "speed");
    speed = JSNumber::Value(value);
    if (jerry_value_is_error(ret)) {
        EXPECT_TRUE((uint8_t)speed == speedVal2);
        jerry_release_value(value);
        jerry_release_value(ret);
        TDD_CASE_END();
        DestroyPage(page);
        return;
    }
    EXPECT_TRUE((uint8_t)speed == speedVal2);

    jerry_release_value(value);

    // click again
    ret = JSObject::Call(page, FUNC_NAME);
    value = JSObject::Get(page, "speed");
    speed = JSNumber::Value(value);
    if (jerry_value_is_error(ret)) {
        EXPECT_TRUE((uint8_t)speed == speedVal1);
        jerry_release_value(value);
        jerry_release_value(ret);
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }
    EXPECT_TRUE((uint8_t)speed == speedVal1);

    jerry_release_value(value);
    jerry_release_value(ret);
    DestroyPage(page);
    TDD_CASE_END();
}

void MarqueeEventTddTest::RunTests()
{
    MarqueeEvent001();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: MarqueeEvent001
 * @tc.desc: Verify Marquee scrollamount.
 */
HWTEST_F(MarqueeEventTddTest, MarqueeEvent001, TestSize.Level0)
{
    MarqueeEventTddTest::MarqueeEvent001();
}
#endif // TDD_ASSERTIONS
} // namespace ACELite
} // namespace OHOS
