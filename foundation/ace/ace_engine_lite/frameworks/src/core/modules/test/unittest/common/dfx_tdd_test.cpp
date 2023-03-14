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
#include "dfx_tdd_test.h"
#include <unistd.h>
#include "js_fwk_common.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {
const char * const DfxTddTest::BUNDLE1
                        = "(function () {"
                          "  return new ViewModel({"
                          "    render: function (vm) {"
                          "      var _vm = vm || this;"
                          "        return _c('swiper', {"
                          "          attrs : {index : 0,duration : 75},"
                          "          staticClass : ['container'], "
                          "          on : {'change' : _vm.swiperChange}} , ["
                          "            _c('stack', {"
                          "              staticClass : ['container']} , ["
                          "                _c('text', {"
                          "                  attrs : {value : function() {return _vm.textValue}},"
                          "                  staticClass : ['pm25-name'],"
                          "                  staticStyle : {color : 16711680},"
                          "                  on : {'click' : _vm.click1}"
                          "                 })"
                          "            ])"
                          "      ]);"
                          "    },"
                          "    styleSheet: {"
                          "      classSelectors: {"
                          "        'pm25-value': {"
                          "          textAlign: 'center',"
                          "          fontSize: 38,"
                          "          color: 15794175,"
                          "          height: 454,"
                          "          width: 454,"
                          "          top: 235"
                          "        },"
                          "        'pm25-name': {"
                          "          textAlign: 'center',"
                          "          color: 10667170,"
                          "          width: 454,"
                          "          height: 50,"
                          "          top: 285"
                          "        }"
                          "      }"
                          "    },"
                          "   data: {textValue: 'Hello World'},"
                          "   onInit: function onInit() {},"
                          "   onShow: function onShow() {},"
                          "   openDetail: function openDetail() {},"
                          "   click3: function click3() {"
                          "     var sum = num + 1;"
                          "     this.textValue = 'Hello Ace';"
                          "   },"
                          "   click2: function click2() {"
                          "     this.click3();"
                          "   },"
                          "   click1: function click1() {"
                          "     this.click2();"
                          "   }"
                          " });"
                          "})();";

const char * const DfxTddTest::BUNDLE2
                        = "(function () {"
                          "  return new ViewModel({"
                          "    render: function (vm) {"
                          "      var _vm = vm || this;"
                          "        return _c('swiper', {"
                          "          attrs : {index : 0,duration : 75},"
                          "          staticClass : ['container'], "
                          "          on : {'change' : _vm.swiperChange}} , ["
                          "            _c('stack', {"
                          "              staticClass : ['container']} , ["
                          "                _c('text', {"
                          "                  attrs : {value : function() {return _vm.textValue}},"
                          "                  staticClass : ['pm25-name'],"
                          "                  staticStyle : {color : 16711680}"
                          "                 })"
                          "            ])"
                          "      ]);"
                          "    },"
                          "    styleSheet: {"
                          "      classSelectors: {"
                          "        'pm25-value': {"
                          "          textAlign: 'center',"
                          "          fontSize: 38,"
                          "          color: 15794175,"
                          "          height: 454,"
                          "          width: 454,"
                          "          top: 235"
                          "        },"
                          "        'pm25-name': {"
                          "          textAlign: 'center',"
                          "          color: 10667170,"
                          "          width: 454,"
                          "          height: 50,"
                          "          top: 285"
                          "        }"
                          "      }"
                          "    },"
                          "   data: {textValue: 'Hello World'},"
                          "   onInit: function onInit() {},"
                          "   onShow: function onShow() {},"
                          "   openDetail: function openDetail() {},"
                          "   click3: function click3() {"
                          "     this.click4();"
                          "     this.textValue = 'Hello Ace';"
                          "   },"
                          "   click2: function click2() {"
                          "     this.click3();"
                          "   },"
                          "   click1: function click1() {"
                          "     this.click2();"
                          "   }"
                          " });"
                          "})();";

const char * const DfxTddTest::BUNDLE3
                        = "(function () {"
                          "  return new ViewModel({"
                          "    render: function (vm) {"
                          "      var _vm = vm || this;"
                          "        return _c('swiper', {"
                          "          attrs : {index : 0,duration : 75},"
                          "          staticClass : ['container'], "
                          "          on : {'change' : _vm.swiperChange}} , ["
                          "            _c('stack', {"
                          "              staticClass : ['container']} , ["
                          "                _c('text', {"
                          "                  attrs : {value : function() {return _vm.textValue}},"
                          "                  staticClass : ['pm25-name'],"
                          "                  staticStyle : {color : 16711680}"
                          "                 })"
                          "            ])"
                          "      ]);"
                          "    },"
                          "    styleSheet: {"
                          "      classSelectors: {"
                          "        'pm25-value': {"
                          "          textAlign: 'center',"
                          "          fontSize: 38,"
                          "          color: 15794175,"
                          "          height: 454,"
                          "          width: 454,"
                          "          top: 235"
                          "        },"
                          "        'pm25-name': {"
                          "          textAlign: 'center',"
                          "          color: 10667170,"
                          "          width: 454,"
                          "          height: 50,"
                          "          top: 285"
                          "        }"
                          "      }"
                          "    },"
                          "   data: {textValue: 'Hello World'},"
                          "   onInit: function onInit() {},"
                          "   onShow: function onShow() {},"
                          "   openDetail: function openDetail() {},"
                          "   click3: function click3() {"
                          "     this.textValue = 'Hello Ace';"
                          "     console.log('Hello Ace.');"
                          "   },"
                          "   click2: function click2() {"
                          "     this.click3();"
                          "   },"
                          "   click1: function click1() {"
                          "     this.click2();"
                          "   }"
                          " });"
                          "})();";

const char * const DfxTddTest::BUNDLE4
                        = "(function () {"
                          "  return new ViewModel({"
                          "    render: function (vm) {"
                          "      var _vm = vm || this;"
                          "        return _c('swiper', {"
                          "          attrs : {index : 0,duration : 75},"
                          "          staticClass : ['container'], "
                          "          on : {'change' : _vm.swiperChange}} , ["
                          "            _c('stack', {"
                          "              staticClass : ['container']} , ["
                          "                _c('text', {"
                          "                  attrs : {value : function() {return _vm.textValue}},"
                          "                  staticClass : ['pm25-name'],"
                          "                  staticStyle : {color : 16711680}"
                          "                 })"
                          "            ])"
                          "      ]);"
                          "    },"
                          "    styleSheet: {"
                          "      classSelectors: {"
                          "        'pm25-value': {"
                          "          textAlign: 'center',"
                          "          fontSize: 38,"
                          "          color: 15794175,"
                          "          height: 454,"
                          "          width: 454,"
                          "          top: 235"
                          "        },"
                          "        'pm25-name': {"
                          "          textAlign: 'center',"
                          "          color: 10667170,"
                          "          width: 454,"
                          "          height: 50,"
                          "          top: 285"
                          "        }"
                          "      }"
                          "    },"
                          "   data: {textValue: 'Hello World'},"
                          "   onInit: function onInit() {},"
                          "   onShow: function onShow() {},"
                          "   openDetail: function openDetail() {},"
                          "   click32: function click32() {"
                          "     this,click33();"
                          "   },"
                          "   click31: function click31() {"
                          "     this.click32();"
                          "   },"
                          "   click30: function click30() {"
                          "     this.click31();"
                          "   },"
                          "   click29: function click29() {"
                          "     this,click30();"
                          "   },"
                          "   click28: function click28() {"
                          "     this.click29();"
                          "   },"
                          "   click27: function click27() {"
                          "     this.click28();"
                          "   },"
                          "   click26: function click26() {"
                          "     this,click27();"
                          "   },"
                          "   click25: function click25() {"
                          "     this.click26();"
                          "   },"
                          "   click24: function click24() {"
                          "     this,click25();"
                          "   },"
                          "   click23: function click23() {"
                          "     this.click24();"
                          "   },"
                          "   click22: function click22() {"
                          "     this.click23();"
                          "   },"
                          "   click21: function click21() {"
                          "     this,click22();"
                          "   },"
                          "   click20: function click20() {"
                          "     this.click21();"
                          "   },"
                          "   click19: function click19() {"
                          "     this.click20();"
                          "   },"
                          "   click18: function click18() {"
                          "     this,click19();"
                          "   },"
                          "   click17: function click17() {"
                          "     this.click18();"
                          "   },"
                          "   click16: function click16() {"
                          "     this.click17();"
                          "   },"
                          "   click15: function click15() {"
                          "     this,click16();"
                          "   },"
                          "   click14: function click14() {"
                          "     this.click15();"
                          "   },"
                          "   click13: function click13() {"
                          "     this.click14();"
                          "   },"
                          "   click12: function click12() {"
                          "     this,click13();"
                          "   },"
                          "   click11: function click11() {"
                          "     this.click12();"
                          "   },"
                          "   click10: function click10() {"
                          "     this.click11();"
                          "   },"
                          "   click9: function click9() {"
                          "     this,click10();"
                          "   },"
                          "   click8: function click8() {"
                          "     this.click9();"
                          "   },"
                          "   click7: function click7() {"
                          "     this.click8();"
                          "   },"
                          "   click6: function click6() {"
                          "     this,click7();"
                          "   },"
                          "   click5: function click5() {"
                          "     this.click6();"
                          "   },"
                          "   click4: function click4() {"
                          "     this.click5();"
                          "   },"
                          "   click3: function click3() {"
                          "     this,click4();"
                          "   },"
                          "   click2: function click2() {"
                          "     this.click3();"
                          "   },"
                          "   click1: function click1() {"
                          "     this.click2();"
                          "   }"
                          " });"
                          "})();";

const char * const DfxTddTest::BUNDLE5
                        = "(function () {"
                          "  return new ViewModel( {"
                          "    render: function (vm) {"
                          "      var _vm = vm || this;"
                          "        return _c('stack', {"
                          "          staticStyle : {"
                          "            width : 444,"
                          "            height : 444,"
                          "            marginTop : 20,"
                          "            marginRight : 20,"
                          "            marginBottom : 20,"
                          "            marginLeft : 20,"
                          "            borderTopWidth : 5,"
                          "            borderRightWidth : 5,"
                          "            borderBottomWidth : 5,"
                          "            borderLeftWidth : 5,"
                          "            borderTopColor : 65280,"
                          "            borderRightColor : 65280,"
                          "            borderBottomColor : 65280,"
                          "            borderLeftColor : 65280,"
                          "            borderRadius : 10"
                          "          },"
                          "        },"
                          "        [_c('text',{"
                          "          attrs : {id : 'textid', value : 'get dom node'},"
                          "          staticStyle : {left : 70,top : 20,width : 400,height : 50},"
                          "          onBubbleEvents : {'click' : function (evt) {_vm.getDomNode}}}),"
                          "         _c('div', {"
                          "          staticClass : ['commonStyle']} ),"
                          "         _c('text', {"
                          "          attrs : {value : function () {return 'domId: ' + (_vm.domId)}},"
                          "          staticStyle : {left : 70,top : 320,width : 400,height : 40}})"
                          "        ])"
                          "      },"
                          "      styleSheet:{"
                          "        classSelectors: {"
                          "          commonStyle: {"
                          "            left:95,"
                          "            top:75,"
                          "            width:200,"
                          "            height:150,"
                          "            marginTop:10,"
                          "            marginRight:10,"
                          "            marginBottom:10,"
                          "            marginLeft:10,"
                          "            paddingTop:30,"
                          "            paddingRight:30,"
                          "            paddingBottom:30,"
                          "            paddingLeft:30,"
                          "            borderTopWidth:1,"
                          "            borderRightWidth:1,"
                          "            borderBottomWidth:1,"
                          "            borderLeftWidth:1,"
                          "            borderRadius:10,"
                          "            backgroundColor:16711680"
                          "          }"
                          "        }"
                          "      },"
                          "      data :{domId: ''},"
                          "      getDomNode: function getDomNode() {"
                          "        var dfx = requireNative('system.dfx');"
                          "        var info = dfx.dumpDomNode('textid');"
                          "        if (info == undefined) {"
                          "          console.log('dump dom node error');"
                          "          this.domId = 'error';"
                          "        } else {"
                          "          var jsonInfo = JSON.parse(info);"
                          "          this.domId = jsonInfo.id;"
                          "        }"
                          "      }"
                          "  })"
                          "})()";

const char * const DfxTddTest::BUNDLE6
                        = "(function () {"
                          "  return new ViewModel( {"
                          "    render: function (vm) {"
                          "      var _vm = vm || this;"
                          "        return _c('stack', {"
                          "          staticStyle : {"
                          "            width : 444,"
                          "            height : 444,"
                          "            marginTop : 20,"
                          "            marginRight : 20,"
                          "            marginBottom : 20,"
                          "            marginLeft : 20,"
                          "            borderTopWidth : 5,"
                          "            borderRightWidth : 5,"
                          "            borderBottomWidth : 5,"
                          "            borderLeftWidth : 5,"
                          "            borderTopColor : 65280,"
                          "            borderRightColor : 65280,"
                          "            borderBottomColor : 65280,"
                          "            borderLeftColor : 65280,"
                          "            borderRadius : 10"
                          "          },"
                          "        },"
                          "        [_c('text',{"
                          "          attrs : {id : 'textid', value : 'get dom tree default'},"
                          "          staticStyle : {left : 50,top : 20,width : 400,height : 50},"
                          "          onBubbleEvents : {'click' : function (evt) {_vm.getDomTree}}}),"
                          "         _c('div', {"
                          "          staticClass : ['commonStyle']} ),"
                          "         _c('text', {"
                          "          attrs : {value : function () {return _vm.msg}},"
                          "          staticStyle : {left : 70,top : 320,width : 400,height : 40}})"
                          "        ])"
                          "      },"
                          "      styleSheet:{"
                          "        classSelectors: {"
                          "          commonStyle: {"
                          "            left:95,"
                          "            top:75,"
                          "            width:200,"
                          "            height:150,"
                          "            marginTop:10,"
                          "            marginRight:10,"
                          "            marginBottom:10,"
                          "            marginLeft:10,"
                          "            paddingTop:30,"
                          "            paddingRight:30,"
                          "            paddingBottom:30,"
                          "            paddingLeft:30,"
                          "            borderTopWidth:1,"
                          "            borderRightWidth:1,"
                          "            borderBottomWidth:1,"
                          "            borderLeftWidth:1,"
                          "            borderRadius:10,"
                          "            backgroundColor:16711680"
                          "          }"
                          "        }"
                          "      },"
                          "      data :{msg: ''},"
                          "      getDomTree: function getDomTree() {"
                          "        var dfx = requireNative('system.dfx');"
                          "        if (dfx.dumpDomTree()) {"
                          "          this.msg = 'success';"
                          "        } else {"
                          "          this.msg = 'fail';"
                          "        }"
                          "      }"
                          "  })"
                          "})()";

const char * const DfxTddTest::BUNDLE7
                        = "(function () {"
                          "  return new ViewModel( {"
                          "    render: function (vm) {"
                          "      var _vm = vm || this;"
                          "        return _c('stack', {"
                          "          staticStyle : {"
                          "            width : 444,"
                          "            height : 444,"
                          "            marginTop : 20,"
                          "            marginRight : 20,"
                          "            marginBottom : 20,"
                          "            marginLeft : 20,"
                          "            borderTopWidth : 5,"
                          "            borderRightWidth : 5,"
                          "            borderBottomWidth : 5,"
                          "            borderLeftWidth : 5,"
                          "            borderTopColor : 65280,"
                          "            borderRightColor : 65280,"
                          "            borderBottomColor : 65280,"
                          "            borderLeftColor : 65280,"
                          "            borderRadius : 10"
                          "          },"
                          "        },"
                          "        [_c('text',{"
                          "          attrs : {id : 'textid', value : 'get dom tree default'},"
                          "          staticStyle : {left : 50,top : 20,width : 400,height : 50},"
                          "          onBubbleEvents : {'click' : function (evt) {_vm.getDomTree}}}),"
                          "         _c('div', {"
                          "          staticClass : ['commonStyle']} ),"
                          "         _c('text', {"
                          "          attrs : {value : function () {return _vm.msg}},"
                          "          staticStyle : {left : 70,top : 320,width : 400,height : 40}})"
                          "        ])"
                          "      },"
                          "      styleSheet:{"
                          "        classSelectors: {"
                          "          commonStyle: {"
                          "            left:95,"
                          "            top:75,"
                          "            width:200,"
                          "            height:150,"
                          "            marginTop:10,"
                          "            marginRight:10,"
                          "            marginBottom:10,"
                          "            marginLeft:10,"
                          "            paddingTop:30,"
                          "            paddingRight:30,"
                          "            paddingBottom:30,"
                          "            paddingLeft:30,"
                          "            borderTopWidth:1,"
                          "            borderRightWidth:1,"
                          "            borderBottomWidth:1,"
                          "            borderLeftWidth:1,"
                          "            borderRadius:10,"
                          "            backgroundColor:16711680"
                          "          }"
                          "        }"
                          "      },"
                          "      data :{msg: ''},"
                          "      getDomTree: function getDomTree() {"
                          "        var dfx = requireNative('system.dfx');"
                          "        var treeid = 'textid';"
                          "        if (dfx.dumpDomTree(treeid)) {"
                          "          this.msg = 'success';"
                          "        } else {"
                          "          this.msg = 'fail';"
                          "        }"
                          "      }"
                          "  })"
                          "})()";


const char * const DfxTddTest::FUNC_NAME = "click1";

const char * const DfxTddTest::DOM_NODE_FUNC_NAME = "getDomNode";

const char * const DfxTddTest::DOM_TREE_FUNC_NAME = "getDomTree";

void DfxTddTest::DfxTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE1, strlen(BUNDLE1));
    JSValue ret = JSObject::Call(page, FUNC_NAME);
    if (!jerry_value_is_error(ret)) {
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }

    // trace out error information if the result contains error
    jerry_value_t errValue = jerry_get_value_from_error(ret, false);
    jerry_value_t errStrVal = jerry_value_to_string(errValue);
    jerry_release_value(errValue);
    jerry_size_t errStrSize = jerry_get_utf8_string_size(errStrVal);
    jerry_char_t *errStrBuffer = static_cast<jerry_char_t *>(ace_malloc(sizeof(jerry_char_t) * (errStrSize + 1)));
    if (errStrBuffer == nullptr) {
        jerry_release_value(errStrVal);
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }
    jerry_size_t stringEnd = jerry_string_to_utf8_char_buffer(errStrVal, errStrBuffer, errStrSize);
    errStrBuffer[stringEnd] = '\0';
    EXPECT_STREQ(reinterpret_cast<char *>(errStrBuffer), "ReferenceError: num is not defined");
    ace_free(errStrBuffer);
    errStrBuffer = nullptr;
    jerry_release_value(errStrVal);

    JSValue value = JSObject::Get(page, "textValue");
    char *valueStr = JSString::Value(value);
    if (valueStr != nullptr) {
        EXPECT_STREQ(valueStr, "Hello World");
        ace_free(valueStr);
        valueStr = nullptr;
    }
    jerry_release_value(value);
    DestroyPage(page);
    TDD_CASE_END();
}

void DfxTddTest::DfxTest002()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE2, strlen(BUNDLE2));
    JSValue errorValue = JSObject::Call(page, FUNC_NAME);
    if (!jerry_value_is_error(errorValue)) {
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }
    const uint16_t stackMsgMaxLength = 256;
    const uint8_t exceptLength = 3;
    const char *stack = "stack";
    jerry_value_t stackStr = jerry_create_string((const jerry_char_t *) stack);
    jerry_value_t errorVal = jerry_get_value_from_error(errorValue, false);
    jerry_value_t backtraceVal = jerry_get_property(errorVal, stackStr);
    ReleaseJerryValue(stackStr, errorVal, VA_ARG_END_FLAG);
    if (jerry_value_is_error(backtraceVal) || !(jerry_value_is_array(backtraceVal))) {
        jerry_release_value(backtraceVal);
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }
    uint32_t length = jerry_get_array_length(backtraceVal);
    EXPECT_EQ(length, exceptLength);
    jerry_char_t *errStrBuffer = static_cast<jerry_char_t *>(ace_malloc(sizeof(jerry_char_t) * (stackMsgMaxLength)));
    if (errStrBuffer == nullptr) {
        jerry_release_value(backtraceVal);
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }
    jerry_value_t itemVal = jerry_get_property_by_index(backtraceVal, 0);
    jerry_size_t strSize = 0;
    if (!jerry_value_is_error(itemVal) && jerry_value_is_string(itemVal)) {
        strSize = jerry_get_utf8_string_size(itemVal);
    }
    jerry_size_t stringEnd = jerry_string_to_utf8_char_buffer(itemVal, errStrBuffer, strSize);
    errStrBuffer[stringEnd] = '\0';
    EXPECT_STREQ(reinterpret_cast<char *>(errStrBuffer), "click3(), ");
    ace_free(errStrBuffer);
    errStrBuffer = nullptr;
    JSValue value = JSObject::Get(page, "textValue");
    char *valueStr = JSString::Value(value);
    if (valueStr != nullptr) {
        EXPECT_STREQ(valueStr, "Hello World");
        ace_free(valueStr);
        valueStr = nullptr;
    }
    ReleaseJerryValue(itemVal, backtraceVal, value, VA_ARG_END_FLAG);
    DestroyPage(page);
    TDD_CASE_END();
}

void DfxTddTest::DfxTest003()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE3, strlen(BUNDLE3));
    JSValue ret = JSObject::Call(page, FUNC_NAME);
    if (jerry_value_is_error(ret)) {
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }
    JSValue value = JSObject::Get(page, "textValue");
    char *valueStr = JSString::Value(value);
    if (valueStr != nullptr) {
        EXPECT_STREQ(valueStr, "Hello Ace");
        ace_free(valueStr);
        valueStr = nullptr;
    }
    jerry_release_value(value);
    DestroyPage(page);
    TDD_CASE_END();
}

void DfxTddTest::DfxTest004()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE4, strlen(BUNDLE4));
    JSValue ret = JSObject::Call(page, FUNC_NAME);
    if (jerry_value_is_error(ret)) {
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }
    const uint8_t exceptLength = 32;
    const char *stack = "stack";

    jerry_value_t stackStr = jerry_create_string((const jerry_char_t *) stack);
    jerry_value_t errorVal = jerry_get_value_from_error(ret, false);

    jerry_value_t backtraceVal = jerry_get_property(errorVal, stackStr);
    jerry_release_value(stackStr);
    jerry_release_value(errorVal);

    if (jerry_value_is_error(backtraceVal) || !(jerry_value_is_array(backtraceVal))) {
        jerry_release_value(backtraceVal);
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }

    uint32_t length = jerry_get_array_length(backtraceVal);
    EXPECT_EQ(length, exceptLength);
    jerry_release_value(backtraceVal);
    DestroyPage(page);
    TDD_CASE_END();
}

void DfxTddTest::DfxTest005()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE5, strlen(BUNDLE5));
    JSValue ret = JSObject::Call(page, DOM_NODE_FUNC_NAME);
    JSValue domId = JSObject::Get(page, "domId");
    uint16_t keyLength = 0;
    char *keyName = MallocStringOf(domId, &keyLength);
    const char *textId = "textid";
    if (jerry_value_is_error(ret)) {
        EXPECT_STREQ(keyName, textId);
        ACE_FREE(keyName);
        jerry_release_value(domId);
        jerry_release_value(ret);
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }
    EXPECT_STREQ(keyName, textId);
    ACE_FREE(keyName);
    jerry_release_value(domId);
    jerry_release_value(ret);
    DestroyPage(page);
    TDD_CASE_END();
}

void DfxTddTest::DfxTest006()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE6, strlen(BUNDLE6));
    JSValue ret = JSObject::Call(page, DOM_TREE_FUNC_NAME);
    JSValue msg = JSObject::Get(page, "msg");
    uint16_t len = 0;
    char *msgStr = MallocStringOf(msg, &len);
    const char *result = "success";
    if (jerry_value_is_error(ret)) {
        EXPECT_STREQ(msgStr, result);
        ACE_FREE(msgStr);
        jerry_release_value(msg);
        jerry_release_value(ret);
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }
    EXPECT_STREQ(msgStr, result);
    ACE_FREE(msgStr);
    jerry_release_value(msg);
    jerry_release_value(ret);
    DestroyPage(page);
    TDD_CASE_END();
}

void DfxTddTest::DfxTest007()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE7, strlen(BUNDLE7));
    JSValue ret = JSObject::Call(page, DOM_TREE_FUNC_NAME);
    JSValue msg = JSObject::Get(page, "msg");
    uint16_t len = 0;
    char *msgStr = MallocStringOf(msg, &len);
    const char *result = "success";
    if (jerry_value_is_error(ret)) {
        EXPECT_STREQ(msgStr, result);
        ACE_FREE(msgStr);
        jerry_release_value(msg);
        jerry_release_value(ret);
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }
    EXPECT_STREQ(msgStr, result);
    ACE_FREE(msgStr);
    jerry_release_value(msg);
    jerry_release_value(ret);
    DestroyPage(page);
    TDD_CASE_END();
}

void DfxTddTest::RunTests()
{
    DfxTest001();
    DfxTest002();
    DfxTest003();
    DfxTest004();
    DfxTest005();
    DfxTest006();
    DfxTest007();
}

#ifdef TDD_ASSERTIONS
/* *
 * @tc.name: DfxTest001
 * @tc.desc: Verify error code.
 */
HWTEST_F(DfxTddTest, DfxTest001, TestSize.Level0)
{
    DfxTddTest::DfxTest001();
}

/* *
 * @tc.name: DfxTest002
 * @tc.desc: Verify error message.
 */
HWTEST_F(DfxTddTest, DfxTest002, TestSize.Level0)
{
    DfxTddTest::DfxTest002();
}

/* *
 * @tc.name: DfxTest003
 * @tc.desc: Verify normal process.
 */
HWTEST_F(DfxTddTest, DfxTest003, TestSize.Level1)
{
    DfxTddTest::DfxTest003();
}

/* *
 * @tc.name: DfxTest004
 * @tc.desc: Verify pressure test.
 */
HWTEST_F(DfxTddTest, DfxTest004, TestSize.Level1)
{
    DfxTddTest::DfxTest004();
}

/* *
 * @tc.name: DfxTest005
 * @tc.desc: Verify dump dom id.
 */
HWTEST_F(DfxTddTest, DfxTest005, TestSize.Level1)
{
    DfxTddTest::DfxTest005();
}

/* *
 * @tc.name: DfxTest006
 * @tc.desc: Verify dump default dom tree.
 */
HWTEST_F(DfxTddTest, DfxTest006, TestSize.Level1)
{
    DfxTddTest::DfxTest006();
}

/* *
 * @tc.name: DfxTest007
 * @tc.desc: Verify dump dom tree by id.
 */
HWTEST_F(DfxTddTest, DfxTest007, TestSize.Level1)
{
    DfxTddTest::DfxTest007();
}
#endif // TDD_ASSERTIONS
}
} // namespace ACELite
