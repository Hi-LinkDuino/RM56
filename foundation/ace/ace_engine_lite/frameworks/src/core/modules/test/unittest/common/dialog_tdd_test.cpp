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

#include "dialog_tdd_test.h"
#include <unistd.h>
#include "event_injector.h"
#include "js_fwk_common.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {
const char * const DialogTddTest::BUNDLE1
                        = "(function () {"
                          "  return new ViewModel({"
                          "    render: function (vm) {"
                          "      var _vm = vm || this;"
                          "        return _c('stack', {"
                          "          staticStyle : {width: 454, height: 454},"
                          "        },"
                          "        [_c('text', {"
                          "          attrs : {value : function() {return _vm.msg}},"
                          "          }),"
                          "         _c('input', {"
                          "          attrs : {type: 'button', value: 'showDialog'},"
                          "          onBubbleEvents: {'click': _vm.showDialog}})"
                          "        ]"
                          "      )"
                          "    },"
                          "    data: {msg: 'dialog'},"
                          "    showDialog: function showDialog() {"
                          "      var self = this;"
                          "      self.msg = 'hello';"
                          "      var _system = {'default': requireNative('system.prompt')};"
                          "      _system['default'].showDialog({"
                          "      title: '弹框标题',"
                          "      message: '弹框显示内容',"
                          "      buttons: [{"
                          "        text: '确认',"
                          "        color: '#666666'"
                          "      }],"
                          "    success: function success(data) {"
                          "      switch (data.index) {"
                          "        case 0:"
                          "          self.msg = 'confirm';"
                          "          break;"
                          "      }"
                          "    }"
                          "  });"
                          "  }"
                          "})"
                          "})()";

const char * const DialogTddTest::BUNDLE2
                        = "(function () {"
                          "  return new ViewModel({"
                          "    render: function (vm) {"
                          "      var _vm = vm || this;"
                          "        return _c('stack', {"
                          "          staticStyle : {width: 454, height: 454},"
                          "        },"
                          "        [_c('text', {"
                          "          attrs : {value : function() {return _vm.msg}},"
                          "          }),"
                          "         _c('input', {"
                          "          attrs : {type: 'button', value: 'showDialog'},"
                          "          onBubbleEvents: {'click': _vm.showDialog}})"
                          "        ]"
                          "      )"
                          "    },"
                          "    data: {msg: 'dialog'},"
                          "    showDialog: function showDialog() {"
                          "      var self = this;"
                          "      self.msg = 'hello';"
                          "      var _system = {'default': requireNative('system.prompt')};"
                          "      _system['default'].showDialog({"
                          "      title: '弹框标题',"
                          "      message: '弹框显示内容',"
                          "      buttons: [{"
                          "        text: '确认',"
                          "        color: '#666666'"
                          "      }, {"
                          "        text: '取消',"
                          "        color: '#666666'"
                          "      }],"
                          "    success: function success(data) {"
                          "      switch (data.index) {"
                          "        case 0:"
                          "          self.msg = 'confirm';"
                          "          break;"
                          "        case 1:"
                          "          self.msg = 'cancel';"
                          "          break;"
                          "      }"
                          "    }"
                          "  });"
                          "  }"
                          "})"
                          "})()";

const char * const DialogTddTest::BUNDLE3
                        = "(function () {"
                          "  return new ViewModel({"
                          "    render: function (vm) {"
                          "      var _vm = vm || this;"
                          "        return _c('stack', {"
                          "          staticStyle : {width: 454, height: 454},"
                          "        },"
                          "        [_c('text', {"
                          "          attrs : {value : function() {return _vm.msg}},"
                          "          staticStyle : {left: 140, top: 0, width: 200, height: 40},"
                          "          }),"
                          "         _c('input', {"
                          "          attrs : {type: 'button', value: 'showDialog'},"
                          "          staticStyle : {left: 125, top: 230, width: 200, height: 50},"
                          "          onBubbleEvents: {'click': _vm.showDialog}})"
                          "        ]"
                          "      )"
                          "    },"
                          "    data: {msg: 'dialog'},"
                          "    showDialog: function showDialog() {"
                          "      var self = this;"
                          "      self.msg = 'hello';"
                          "      var _system = {'default': requireNative('system.prompt')};"
                          "      _system['default'].showDialog({"
                          "      title: '弹框标题',"
                          "      message: '弹框显示内容',"
                          "      buttons: [{"
                          "        text: '确认',"
                          "        color: '#666666'"
                          "      }, {"
                          "        text: '取消',"
                          "        color: '#666666'"
                          "      }, {"
                          "        text: '完成',"
                          "        color: '#666666'"
                          "      }],"
                          "    success: function success(data) {"
                          "      switch (data.index) {"
                          "        case 0:"
                          "          self.msg = 'confirm';"
                          "          break;"
                          "        case 1:"
                          "          self.msg = 'cancel';"
                          "          break;"
                          "        case 2:"
                          "          self.msg = 'complete';"
                          "          break;"
                          "      }"
                          "    }"
                          "  });"
                          "  }"
                          "})"
                          "})()";

const char * const DialogTddTest::BUNDLE4
                        = "(function () {"
                          "  return new ViewModel({"
                          "    render: function (vm) {"
                          "      var _vm = vm || this;"
                          "        return _c('stack', {"
                          "          staticStyle : {width: 454, height: 454},"
                          "        },"
                          "        [_c('text', {"
                          "          attrs : {value : function() {return _vm.msg}},"
                          "          staticStyle : {left: 140, top: 0, width: 200, height: 40},"
                          "          }),"
                          "         _c('input', {"
                          "          attrs : {type: 'button', value: 'showDialog'},"
                          "          staticStyle : {left: 125, top: 230, width: 200, height: 50},"
                          "          onBubbleEvents: {'click': _vm.showDialog}})"
                          "        ]"
                          "      )"
                          "    },"
                          "    data: {msg: 'dialog'},"
                          "    showDialog: function showDialog() {"
                          "      var self = this;"
                          "      self.msg = 'hello';"
                          "      var _system = {'default': requireNative('system.prompt')};"
                          "      _system['default'].showDialog({"
                          "      title: '弹框标题',"
                          "      message: '弹框显示内容',"
                          "      buttons: [{"
                          "        text: '确认',"
                          "        color: '#666666'"
                          "      }, {"
                          "        text: '取消',"
                          "        color: '#666666'"
                          "      }, {"
                          "        text: '完成',"
                          "        color: '#666666'"
                          "      }, {"
                          "        text: 'button4',"
                          "        color: '#666666'"
                          "      }],"
                          "    success: function success(data) {"
                          "      switch (data.index) {"
                          "        case 0:"
                          "          self.msg = 'confirm';"
                          "          break;"
                          "        case 1:"
                          "          self.msg = 'cancel';"
                          "          break;"
                          "        case 2:"
                          "          self.msg = 'complete';"
                          "          break;"
                          "        case 3:"
                          "          self.msg = 'useless';"
                          "          break;"
                          "      }"
                          "    }"
                          "  });"
                          "  }"
                          "})"
                          "})()";

const char * const DialogTddTest::BUNDLE5
                        = "(function () {"
                          "  return new ViewModel({"
                          "    render: function (vm) {"
                          "      var _vm = vm || this;"
                          "        return _c('stack', {"
                          "          staticStyle : {width: 454, height: 454},"
                          "        },"
                          "        [_c('text', {"
                          "          attrs : {value : function() {return _vm.msg}},"
                          "          }),"
                          "         _c('input', {"
                          "          attrs : {type: 'button', value: 'showDialog'},"
                          "          onBubbleEvents: {'click': _vm.showDialog}})"
                          "        ]"
                          "      )"
                          "    },"
                          "    data: {msg: 'dialog'},"
                          "    showDialog: function showDialog() {"
                          "      var self = this;"
                          "      self.msg = 'HelloAce';"
                          "      var _system = {'default': requireNative('system.prompt')};"
                          "      _system['default'].showDialog({"
                          "      title: '弹框标题'"
                          "    });"
                          "  }"
                          "})"
                          "})()";

const char * const DialogTddTest::METHOD = "showDialog";

const char * const DialogTddTest::CONFIRM = "confirm";

const char * const DialogTddTest::CANCEL = "cancel";

const char * const DialogTddTest::COMPLETE = "complete";

const char * const DialogTddTest::MSG = "msg";

const char * const DialogTddTest::DEFAULT = "dialog";

bool DialogTddTest::CallPageMethod(JSValue page, const char * const prop) const
{
    JSValue ret = JSObject::Call(page, prop);
    if (jerry_value_is_error(ret)) {
        jerry_release_value(ret);
        return false;
    }
    jerry_release_value(ret);
    return true;
}

void DialogTddTest::DialogTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE1, strlen(BUNDLE1));

    // step1: get default value and compare
    char *val = JSObject::GetString(page, MSG);
    EXPECT_STREQ(val, DEFAULT);
    ACE_FREE(val);
    if (!CallPageMethod(page, METHOD)) {
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }

    // step2: set dialog window id and click
    const int16_t positionX1 = 470;
    const int16_t positionY1 = 250;
    Window *window = RootView::GetInstance()->GetBoundWindow();
    EventInjector::GetInstance()->SetWindowId(window->GetWindowId() + 1);
    Click(positionX1, positionY1);

    // step3: get confirm value and compare
    val = JSObject::GetString(page, MSG);
    EXPECT_STREQ(val, CONFIRM);
    ACE_FREE(val);
    DestroyPage(page);
    TDD_CASE_END();
}

void DialogTddTest::DialogTest002()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE2, strlen(BUNDLE2));

    // step1: get default value and compare
    char *val = JSObject::GetString(page, MSG);
    EXPECT_STREQ(val, DEFAULT);
    ACE_FREE(val);
    if (!CallPageMethod(page, METHOD)) {
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }

    // step2: set dialog window id and click
    const int16_t positionX1 = 460;
    const int16_t positionY1 = 274;
    Window *window = RootView::GetInstance()->GetBoundWindow();
    EventInjector::GetInstance()->SetWindowId(window->GetWindowId() + 1);
    Click(positionX1, positionY1);

    // step3: get confirm value and compare
    val = JSObject::GetString(page, MSG);
    EXPECT_STREQ(val, CONFIRM);
    ACE_FREE(val);

    // step4: release previous result and call dialog method again
    if (!CallPageMethod(page, METHOD)) {
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }

    // step5: set dialog window id and click
    const int16_t positionX2 = 500;
    const int16_t positionY2 = 274;
    EventInjector::GetInstance()->SetWindowId(window->GetWindowId() + 1);
    Click(positionX2, positionY2);

    // step6: get cancel value and compare
    val = JSObject::GetString(page, MSG);
    EXPECT_STREQ(val, CANCEL);
    ACE_FREE(val);
    DestroyPage(page);
    TDD_CASE_END();
}

void DialogTddTest::DialogTest003()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE3, strlen(BUNDLE3));

    // step1: get default value and compare
    char *val = JSObject::GetString(page, MSG);
    EXPECT_STREQ(val, DEFAULT);
    ACE_FREE(val);
    if (!CallPageMethod(page, METHOD)) {
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }

    // step2: set dialog window id and click
    const int16_t positionX1 = 420;
    const int16_t positionY1 = 270;
    Window *window = RootView::GetInstance()->GetBoundWindow();
    EventInjector::GetInstance()->SetWindowId(window->GetWindowId() + 1);
    Click(positionX1, positionY1);

    // step3: get confirm value and compare
    val = JSObject::GetString(page, MSG);
    EXPECT_STREQ(val, CONFIRM);
    ACE_FREE(val);

    // step4: release previous result and call dialog method again
    if (!CallPageMethod(page, METHOD)) {
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }

    // step5: set dialog window id and click
    const int16_t positionX2 = 470;
    const int16_t positionY2 = 270;
    EventInjector::GetInstance()->SetWindowId(window->GetWindowId() + 1);
    Click(positionX2, positionY2);

    // step6: get cancel value and compare
    val = JSObject::GetString(page, MSG);
    EXPECT_STREQ(val, CANCEL);
    ACE_FREE(val);

    // step7: release previous result and call dialog method again
    if (!CallPageMethod(page, METHOD)) {
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }

    // step8: set dialog window id and click
    const int16_t positionX3 = 530;
    const int16_t positionY3 = 270;
    EventInjector::GetInstance()->SetWindowId(window->GetWindowId() + 1);
    Click(positionX3, positionY3);

    // step9: get complete value and compare
    val = JSObject::GetString(page, MSG);
    EXPECT_STREQ(val, COMPLETE);
    ACE_FREE(val);
    DestroyPage(page);
    TDD_CASE_END();
}

void DialogTddTest::DialogTest004()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE4, strlen(BUNDLE4));

    // step1: get default value and compare
    char *val = JSObject::GetString(page, MSG);
    EXPECT_STREQ(val, DEFAULT);
    ACE_FREE(val);
    if (!CallPageMethod(page, METHOD)) {
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }

    // step2: set dialog window id and click
    const int16_t positionX1 = 420;
    const int16_t positionY1 = 270;
    Window *window = RootView::GetInstance()->GetBoundWindow();
    EventInjector::GetInstance()->SetWindowId(window->GetWindowId() + 1);
    Click(positionX1, positionY1);

    // step3: get confirm value and compare
    val = JSObject::GetString(page, MSG);
    EXPECT_STREQ(val, CONFIRM);
    ACE_FREE(val);

    // step4: release previous result and call dialog method again
    if (!CallPageMethod(page, METHOD)) {
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }

    // step5: set dialog window id and click
    const int16_t positionX2 = 470;
    const int16_t positionY2 = 270;
    EventInjector::GetInstance()->SetWindowId(window->GetWindowId() + 1);
    Click(positionX2, positionY2);

    // step6: get cancel value and compare
    val = JSObject::GetString(page, MSG);
    EXPECT_STREQ(val, CANCEL);
    ACE_FREE(val);

    // step7: release previous result and call dialog method again
    if (!CallPageMethod(page, METHOD)) {
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }

    // step8: set dialog window id and click
    const int16_t positionX3 = 530;
    const int16_t positionY3 = 270;
    EventInjector::GetInstance()->SetWindowId(window->GetWindowId() + 1);
    Click(positionX3, positionY3);

    // step9: get complete value and compare
    val = JSObject::GetString(page, MSG);
    EXPECT_STREQ(val, COMPLETE);
    ACE_FREE(val);
    DestroyPage(page);
    TDD_CASE_END();
}

void DialogTddTest::DialogTest005()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE5, strlen(BUNDLE5));

    // step1: get default value and compare
    char *val = JSObject::GetString(page, MSG);
    EXPECT_STREQ(val, DEFAULT);
    ACE_FREE(val);
    if (!CallPageMethod(page, METHOD)) {
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }

    // step2: set dialog window id and click
    const int16_t position = 10;
    const char *dismissiontVal = "HelloAce";
    Window *window = RootView::GetInstance()->GetBoundWindow();
    EventInjector::GetInstance()->SetWindowId(window->GetWindowId() + 1);
    Click(position, position);

    // step3: get dismisson value and compare
    val = JSObject::GetString(page, MSG);
    EXPECT_STREQ(val, dismissiontVal);
    ACE_FREE(val);
    DestroyPage(page);
    TDD_CASE_END();
}

void DialogTddTest::RunTests()
{
    DialogTest001();
    DialogTest002();
    DialogTest003();
    DialogTest004();
    DialogTest005();
}

#ifdef TDD_ASSERTIONS
HWTEST_F(DialogTddTest, DialogTest001, TestSize.Level1)
{
    DialogTddTest::DialogTest001();
}

HWTEST_F(DialogTddTest, DialogTest002, TestSize.Level1)
{
    DialogTddTest::DialogTest002();
}

HWTEST_F(DialogTddTest, DialogTest003, TestSize.Level1)
{
    DialogTddTest::DialogTest003();
}

HWTEST_F(DialogTddTest, DialogTest004, TestSize.Level0)
{
    DialogTddTest::DialogTest004();
}

HWTEST_F(DialogTddTest, DialogTest005, TestSize.Level1)
{
    DialogTddTest::DialogTest005();
}
#endif // TDD_ASSERTIONS
} // namespace ACELite
} // namespace OHOS
