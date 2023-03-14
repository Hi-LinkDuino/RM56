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

#include "timer_module_tdd_test.h"
#include "js_fwk_common.h"

namespace OHOS {
namespace ACELite {
const char SET_TIMEOUT_001[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('stack', {staticClass: ['stack']}, [\n"
    "        _c('input', {'attrs': {'type': 'button','value': 'startTimer'},\n"
    "                     'staticClass': ['button'],\n"
    "                     'staticStyle': {'left': '30%','top': '25%'},\n"
    "                     'onBubbleEvents' : {'click' : _vm.startTimer}}),\n"
    "        _c('text', {'attrs' : {'value' : function () {return _vm.value}},\n"
    "                    'staticClass' : ['text']})\n"
    "   ]);\n"
    "   },\n"
    "   styleSheet: {\n"
    "      classSelectors: {\n"
    "        stack: {\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          left: 0,\n"
    "          right: 0,\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "      button: {\n"
    "      width:'25%',\n"
    "      height:'20%',\n"
    "        },\n"
    "      text: {\n"
    "      width:'50%',\n"
    "      fontSize:38,\n"
    "      textAlign: 'center',\n"
    "      left:'20%',\n"
    "      top: '50%',\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      value: 'timer'\n"
    "    },\n"
    "    startTimer: function() {\n"
    "      var _this = this;\n"
    "      setTimeout(function() {\n"
    "        _this.value = 'once_timer_success';\n"
    "        }, 100);\n"
    "    }\n"
    "  });\n"
    "})();\n";

const char INTERVAL_TEST_002[] =
    "(function () {\n"
    "var index = 0;\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('stack', {staticClass: ['stack']}, [\n"
    "        _c('input', {'attrs': {'type': 'button','value': 'startTimer'},\n"
    "                     'staticClass': ['button'],\n"
    "                     'staticStyle': {'left': '25%','top': '20%'},\n"
    "                     'onBubbleEvents' : {'click' : _vm.startTimer}}),\n"
    "        _c('text', {'attrs' : {'value' : function () {return _vm.value}},\n"
    "                    'staticClass' : ['text']})\n"
    "   ]);\n"
    "   },\n"
    "   styleSheet: {\n"
    "      classSelectors: {\n"
    "        stack: {\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          left: 0,\n"
    "          right: 0,\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "      button: {\n"
    "      width:'25%',\n"
    "      height:'20%',\n"
    "        },\n"
    "      text: {\n"
    "      width:'50%',\n"
    "      fontSize:38,\n"
    "      textAlign: 'center',\n"
    "      left:'20%',\n"
    "      top: '50%',\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      value: 'timer'\n"
    "    },\n"
    "    startTimer: function() {\n"
    "      var _this = this;\n"
    "      var id = setInterval(function() {"
    "        if (index >5) {\n"
    "            _this.value = 'clearInterval';\n"
    "            clearInterval(id);\n"
    "          }\n"
    "        index++;\n"
    "        _this.value = index;\n"
    "        }, 100);\n"
    "    }\n"
    "  });\n"
    "})();\n";

const char INTERVAL_TEST_003[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('stack', {staticClass: ['stack']}, [\n"
    "        _c('input', {'attrs': {'type': 'button','value': 'startTimer'},\n"
    "                     'staticClass': ['button'],\n"
    "                     'staticStyle': {'left': '25%','top': '20%'},\n"
    "                     'onBubbleEvents' : {'click' : _vm.startTimer}}),\n"
    "        _c('text', {'attrs' : {'value' : function () {return _vm.value}},\n"
    "                    'staticClass' : ['text']})\n"
    "   ]);\n"
    "   },\n"
    "   styleSheet: {\n"
    "      classSelectors: {\n"
    "        stack: {\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          left: 0,\n"
    "          right: 0,\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "      button: {\n"
    "      width:'25%',\n"
    "      height:'20%',\n"
    "        },\n"
    "      text: {\n"
    "      width:'50%',\n"
    "      fontSize:38,\n"
    "      textAlign: 'center',\n"
    "      left:'20%',\n"
    "      top: '50%',\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      value: 'timer'\n"
    "    },\n"
    "    startTimer: function() {\n"
    "      var _this = this;\n"
    "      var id = setInterval(function() {\n"
    "         clearTimeout(id);\n"
    "         _this.value = 'clearSuccess';\n"
    "        }, 200);\n"
    "    }\n"
    "  });\n"
    "})();\n";

const char TIME_OUT_004[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('stack', {staticClass: ['stack']}, [\n"
    "        _c('input', {'attrs': {type: 'button',value: 'triggerONo'},\n"
    "                     'staticClass': ['button'],\n"
    "                     'staticStyle': {'left': '5%','top': '10%'},\n"
    "                     'onBubbleEvents' : {'click' : _vm.triggerNo}\n"
    "           }),\n"
    "       _c('input', {'attrs': {'type':'button', value:'One Arg'},\n"
    "                    'staticClass':['button'],\n"
    "                    'staticStyle':{'left':'35%', 'top':'10%'},\n"
    "                    'onBubbleEvents':{'click':_vm.triggerOne}\n"
    "          }),\n"
    "       _c('input', {'attrs':{'type':'button', value:'Two Arg'},\n"
    "                    'staticClass':['button'],\n"
    "                    'staticStyle': {'left':'65%', 'top':'10%'},\n"
    "                    'onBubbleEvents':{'click':_vm.triggerTwo}\n"
    "           }),\n"
    "        _c('text', {'attrs' : {'value' : function () {return _vm.value}},\n"
    "                    'staticClass' : ['text']\n"
    "           })\n"
    "   ]);\n"
    "   },\n"
    "   styleSheet: {\n"
    "      classSelectors: {\n"
    "        stack: {\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          left: 0,\n"
    "          right: 0,\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "      button: {\n"
    "      width:'25%',\n"
    "      height:'20%',\n"
    "        },\n"
    "      text: {\n"
    "      width:'50%',\n"
    "      fontSize:38,\n"
    "      textAlign: 'center',\n"
    "      left:'20%',\n"
    "      top: '50%',\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      value: 'timer'\n"
    "    },\n"
    "    triggerOne: function() {\n"
    "      var _this = this;\n"
    "      var id = setTimeout(function(a) {\n"
    "        _this.value = a;\n"
    "        }, 200, 'One Arg');\n"
    "    },\n"
    "   triggerNo: function(a) {\n"
    "       var _this = this;\n"
    "       setTimeout(function() {\n"
    "         _this.value = a;\n"
    "       }, 200);\n"
    "   },\n"
    "   triggerTwo: function(a) {\n"
    "       var _this = this;"
    "       setTimeout(function(a) {\n"
    "           _this.value = a;\n"
    "         }, 200, 'Two Arg', 'One Arg');\n"
    "   }\n"
    "  });\n"
    "})();\n";

const char ARG_TEST_005[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('stack', {staticClass: ['stack']}, [\n"
    "        _c('input', {'attrs': {type: 'button',value: 'triggerNo'},\n"
    "                     'staticClass': ['button'],\n"
    "                     'staticStyle': {'left': '5%','top': '10%'},\n"
    "                     'onBubbleEvents' : {'click' : _vm.triggerNo}\n"
    "           }),\n"
    "       _c('input', {'attrs': {'type':'button', value:'One Arg'},\n"
    "                    'staticClass':['button'],\n"
    "                    'staticStyle':{'left':'35%', 'top':'10%'},\n"
    "                    'onBubbleEvents':{'click':_vm.triggerOne}\n"
    "          }),\n"
    "       _c('input', {'attrs':{'type':'button', value:'Two Arg'},\n"
    "                    'staticClass':['button'],\n"
    "                    'staticStyle': {'left':'65%', 'top':'10%'},\n"
    "                    'onBubbleEvents':{'click':_vm.triggerTwo}\n"
    "           }),\n"
    "        _c('text', {'attrs' : {'value' : function () {return _vm.value}},\n"
    "                    'staticClass' : ['text']\n"
    "           })\n"
    "   ]);\n"
    "   },\n"
    "   styleSheet: {\n"
    "      classSelectors: {\n"
    "        stack: {\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          left: 0,\n"
    "          right: 0,\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "      button: {\n"
    "      width:'25%',\n"
    "      height:'20%',\n"
    "        },\n"
    "      text: {\n"
    "      width:'50%',\n"
    "      fontSize:38,\n"
    "      textAlign: 'center',\n"
    "      left:'20%',\n"
    "      top: '50%',\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      value: 'timer'\n"
    "    },\n"
    "    triggerOne: function() {\n"
    "      var _this = this;\n"
    "      var id = setInterval(function(a) {\n"
    "        _this.value = a;\n"
    "        clearInterval(id);\n"
    "        }, 200, 'Interval One');\n"
    "    },\n"
    "   triggerNo: function(a) {\n"
    "       var _this = this;\n"
    "       var id = setInterval(function(a) {\n"
    "         _this.value = a;\n"
    "         clearInterval(id);\n"
    "       }, 200);\n"
    "   },\n"
    "   triggerTwo: function(a) {\n"
    "       var _this = this;\n"
    "       var id = setInterval(function(a) {\n"
    "           _this.value = a;\n"
    "           clearInterval(id);\n"
    "         }, 200, 'Interval Two', 'Interval One');\n"
    "   }\n"
    "  });\n"
    "})();\n";

const char CLEAR_TIME_OUT_006[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('stack', {staticClass: ['stack']}, [\n"
    "        _c('input', {'attrs': {'type': 'button','value': 'setTimeout'},\n"
    "                     'staticClass': ['button'],\n"
    "                     'staticStyle': {'left': '25%','top': '20%'},\n"
    "                     'onBubbleEvents' : {'click' : _vm.checkSetTimeout}}),\n"
    "        _c('text', {'attrs' : {'value' : function () {return _vm.value}},\n"
    "                    'staticClass' : ['text']})\n"
    "   ]);\n"
    "   },\n"
    "   styleSheet: {\n"
    "      classSelectors: {\n"
    "        stack: {\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          left: 0,\n"
    "          right: 0,\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "      button: {\n"
    "      width:'25%',\n"
    "      height:'20%',\n"
    "        },\n"
    "      text: {\n"
    "      width:'50%',\n"
    "      fontSize:38,\n"
    "      textAlign: 'center',\n"
    "      left:'20%',\n"
    "      top: '50%',\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      value: 'timer'\n"
    "    },\n"
    "    checkSetTimeout: function() {\n"
    "      var _this = this;\n"
    "      var id = setTimeout(function() {\n"
    "          _this.value = 'trigger start';\n"
    "          clearTimeout(id);\n"
    "          _this.value = 'trigger success';\n"
    "        }, 200);\n"
    "    }\n"
    "  });\n"
    "})();\n";

const char INTERVAL_TEST_007[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('stack', {staticClass: ['stack']}, [\n"
    "        _c('input', {'attrs': {'type': 'button','value': 'setInterval'},\n"
    "                     'staticClass': ['button'],\n"
    "                     'staticStyle': {'left': '25%','top': '20%'},\n"
    "                     'onBubbleEvents' : {'click' : _vm.checkSetInterval}}),\n"
    "        _c('text', {'attrs' : {'value' : function () {return _vm.value}},\n"
    "                    'staticClass' : ['text']})\n"
    "   ]);\n"
    "   },\n"
    "   styleSheet: {\n"
    "      classSelectors: {\n"
    "        stack: {\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          left: 0,\n"
    "          right: 0,\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "      button: {\n"
    "      width:'25%',\n"
    "      height:'20%',\n"
    "        },\n"
    "      text: {\n"
    "      width:'50%',\n"
    "      fontSize:38,\n"
    "      textAlign: 'center',\n"
    "      left:'20%',\n"
    "      top: '50%',\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      value: 'timer'\n"
    "    },\n"
    "    checkSetInterval: function() {\n"
    "      var _this = this;\n"
    "      var id = setInterval(function() {"
    "          _this.value = 'trigger interval start';\n"
    "          clearInterval(id);\n"
    "          _this.value = 'trigger interval success';\n"
    "        }, 200);\n"
    "    }\n"
    "  });\n"
    "})();\n";

const char CLEAR_TIME_OUT_008[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('stack', {staticClass: ['stack']}, [\n"
    "        _c('input', {'attrs': {'type': 'button','value': 'clearInterval'},\n"
    "                     'staticClass': ['button'],\n"
    "                     'staticStyle': {'left': '30%','top': '15%'},\n"
    "                     'onBubbleEvents' : {'click' : _vm.testClearInterval}}),\n"
    "       _c('input', {'attrs':{'type':'button', value:'clearTimeout'},\n"
    "                    'staticClass': ['button'],\n"
    "                    'staticStyle': {'left':'30%', 'top':'45%'},\n"
    "                    'onBubbleEvents': {'click': _vm.testClearTimeout}}),\n"
    "        _c('text', {'attrs' : {'value' : function () {return _vm.value}},\n"
    "                    'staticClass' : ['text']})\n"
    "   ]);\n"
    "   },\n"
    "   styleSheet: {\n"
    "      classSelectors: {\n"
    "        stack: {\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          left: 0,\n"
    "          right: 0,\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "      button: {\n"
    "      width:'40%',\n"
    "      height:'20%',\n"
    "        },\n"
    "      text: {\n"
    "      width:'50%',\n"
    "      fontSize:38,\n"
    "      textAlign: 'center',\n"
    "      left:'20%',\n"
    "      top: '70%',\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      value: 'timer'\n"
    "    },\n"
    "    testClearInterval: function() {\n"
    "       clearInterval(1);\n"
    "       console.log('testClearInterval');\n"
    "       this.value = 'clearInterval';\n"
    "    },\n"
    "   testClearTimeout: function() {\n"
    "       clearTimeout(1);\n"
    "       console.log('clearSuccess');\n"
    "       this.value = 'clearSuccess';\n"
    "    }"
    "  });\n"
    "})();\n";

const char BUNDLE_09[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('stack', {staticClass: ['stack']}, [\n"
    "        _c('input', {'attrs': {'type': 'button','value': 'minTime'},\n"
    "                     'staticClass': ['button'],\n"
    "                     'staticStyle': {'left': '30%','top': '15%'},\n"
    "                     'onBubbleEvents' : {'click' : _vm.testMinTime}}),\n"
    "       _c('input', {'attrs':{'type':'button', value:'maxTime'},\n"
    "                    'staticClass': ['button'],\n"
    "                    'staticStyle': {'left':'60%', 'top':'15%'},\n"
    "                    'onBubbleEvents': {'click': _vm.testMaxTime}}),\n"
    "       _c('input', {'attrs':{'type':'button', value:'minInterval'},\n"
    "                    'staticClass': ['button'],\n"
    "                    'staticStyle': {'left':'30%', 'top':'40%'},\n"
    "                    'onBubbleEvents': {'click': _vm.testMinInterval}}),\n"
    "       _c('input', {'attrs':{'type':'button', value:'maxInterval'},\n"
    "                    'staticClass': ['button'],\n"
    "                    'staticStyle': {'left':'60%', 'top':'40%'},\n"
    "                    'onBubbleEvents': {'click': _vm.testMaxInterval}}),\n"
    "        _c('text', {'attrs' : {'value' : function () {return _vm.value}},\n"
    "                    'staticClass' : ['text']})\n"
    "   ]);\n"
    "   },\n"
    "   styleSheet: {\n"
    "      classSelectors: {\n"
    "        stack: {\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          left: 0,\n"
    "          right: 0,\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "      button: {\n"
    "      width:'25%',\n"
    "      height:'20%',\n"
    "        },\n"
    "      text: {\n"
    "      width:'50%',\n"
    "      fontSize:38,\n"
    "      textAlign: 'center',\n"
    "      left:'20%',\n"
    "      top: '50%',\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      value: 'timer'\n"
    "    },\n"
    "    testMinTime: function() {\n"
    "       var _this = this;"
    "       setTimeout(function() {\n"
    "           _this.value = 'min';"
    "         }, -2);\n"
    "    },\n"
    "   testMaxTime: function() {\n"
    "       var _this = this;\n"
    "       setTimeout(function() {\n"
    "         _this.value = 'max';\n"
    "       }, 4294967297);"
    "    },"
    "   testMinInterval: function() {\n"
    "       var _this = this;\n"
    "       var id = setInterval(function() {\n"
    "              _this.value = 'min';\n"
    "              clearInterval(id);\n"
    "           }, -1);\n"
    "    },\n"
    "   testMaxInterval: function() {\n"
    "       var _this = this;\n"
    "       var id = setInterval(function() {\n"
    "               _this.value = 'max';\n"
    "               clearInterval(id);\n"
    "           }, 4294967295);\n"
    "   }\n"
    "  });\n"
    "})();\n";

const char BUNDLE_10[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('stack', {staticClass: ['stack']}, [\n"
    "        _c('input', {'attrs': {'type': 'button','value': 'maxTimer'},\n"
    "                     'staticClass': ['button'],\n"
    "                     'staticStyle': {'left': '30%','top': '25%'},\n"
    "                     'onBubbleEvents' : {'click' : _vm.startMaxTimer}}),\n"
    "        _c('text', {'attrs' : {'value' : function () {return _vm.value}},\n"
    "                    'staticClass' : ['text']})\n"
    "   ]);\n"
    "   },\n"
    "   styleSheet: {\n"
    "      classSelectors: {\n"
    "        stack: {\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          left: 0,\n"
    "          right: 0,\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "      button: {\n"
    "      width:'25%',\n"
    "      height:'20%',\n"
    "        },\n"
    "      text: {\n"
    "      width:'50%',\n"
    "      fontSize:38,\n"
    "      textAlign: 'center',\n"
    "      left:'20%',\n"
    "      top: '50%',\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      value: 'timer'\n"
    "    },\n"
    "    startMaxTimer: function() {\n"
    "      var _this = this;\n"
    "      setTimeout(function() {\n"
    "          _this.value = 'timer1';\n"
    "        }, 100);\n"
    "      setTimeout(function() {\n"
    "           _this.value = 'timer2';\n"
    "       }, 200);\n"
    "      setTimeout(function() {\n"
    "           _this.value = 'timer3';\n"
    "       }, 300);\n"
    "      setTimeout(function() {\n"
    "           _this.value = 'timer4';\n"
    "       }, 400);\n"
    "    }\n"
    "  });\n"
    "})();\n";

char *TimerModuleTddTest::TriggerTimer(const double xRate,
                                       const double yRate,
                                       const uint8_t sleepSeconds,
                                       const JSValue page) const
{
    const uint8_t offset = 50;
    const uint16_t centerX = GetHorizontalResolution() * xRate + offset;
    const uint16_t centerY = GetVerticalResolution() * yRate + offset;
    Click(centerX, centerY);
    sleep(sleepSeconds);
    if (!JSObject::Has(page, "value")) {
        return nullptr;
    }
    JSValue value = JSObject::Get(page, "value");
    char *content = nullptr;
    if (!JSUndefined::Is(value)) {
        content = JSObject::GetString(page, "value");
    }
    JSRelease(value);
    return content;
}

/**
 * @tc.name: SetTimeoutTest001
 * @tc.desc: test setTimeout
 */
HWTEST_F(TimerModuleTddTest, SetTimeoutTest001, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.read js file, setTimeout_test001.js, and eval the js page
     * @tc.expected: step1. the js file eval success
     */
    JSValue page = CreatePage(SET_TIMEOUT_001, strlen(SET_TIMEOUT_001));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps:step2.click startTimer button to trigger timer event and sleep 400 ms
     * @tc.expected: step2.the value of data is once_timer_success
     */
    const double xrate = 0.25;
    const double yrate = 0.2;
    const uint8_t sleepTime = 1;
    char *content = TriggerTimer(xrate, yrate, sleepTime, page);
    EXPECT_TRUE((content != nullptr) && (!strcmp(content, "once_timer_success")));
    ACE_FREE(content);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: SetIntervalTest001
 * @tc.desc: test set interval
 */
HWTEST_F(TimerModuleTddTest, SetIntervalTest002, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /*
     * @tc.steps:step1.eval js file setInterval_test002.js
     * @tc.expected:step1.eval js file success
     */
    JSValue page = CreatePage(INTERVAL_TEST_002, strlen(INTERVAL_TEST_002));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps:step2.click startTimer button to trigger timer event and sleep 400 ms
     * @tc.expected: step2.the value of data is interval_timer_success
     */
    const double xRate = 0.25;
    const double yRate = 0.2;
    const uint8_t sleepTime = 2;
    char *content = TriggerTimer(xRate, yRate, sleepTime, page);
    EXPECT_TRUE((content != nullptr) && (!strcmp(content, "7")));
    ACE_FREE(content);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: SetTimeoutTest003
 * @tc.desc: test clearTimeout
 */
HWTEST_F(TimerModuleTddTest, SetTimeoutTest003, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1.eval js file setTimeout_test003.js
     * @tc.expected:step1.eval js file success
     */
    JSValue page = CreatePage(INTERVAL_TEST_003, strlen(INTERVAL_TEST_003));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps:step2.click startTimer button and sleep 400 ms
     * @tc.expected: step2.the value of data is cleaSuccess
     */
    const double xrate = 0.25;
    const double yrate = 0.2;
    const uint8_t sleepTime = 1;
    char *content = TriggerTimer(xrate, yrate, sleepTime, page);
    EXPECT_TRUE((content != nullptr) && (!strcmp(content, "clearSuccess")));
    ACE_FREE(content);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: SetTimeoutTest004
 * @tc.desc: test setTimeout args
 */
HWTEST_F(TimerModuleTddTest, SetTimeoutTest004, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.eval js file setTimeout_test004.js
     * @tc.expected:step1.eval js file success
     */
    JSValue page = CreatePage(TIME_OUT_004, strlen(TIME_OUT_004));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps:step2.click the noArg button
     * @tc.expected:step2.the value attribute is timer
     */
    double xRate = 0.05;
    const double yRate = 0.1;
    const uint8_t sleepTime = 1;
    char *content = TriggerTimer(xRate, yRate, sleepTime, page);
    EXPECT_FALSE(content == nullptr);

    /**
     * @tc.steps:step3.click the oneArg button
     * @tc.expected:step3.the value attribute is One Arg
     */
    xRate = 0.35;
    content = TriggerTimer(xRate, yRate, sleepTime, page);
    EXPECT_TRUE((content != nullptr) && (!strcmp(content, "One Arg")));
    ACE_FREE(content);

    /**
     * @tc.steps:step3.click two args button
     * @tc.steps:step3.the value attribute is Two Arg
     */
    xRate = 0.65;
    content = TriggerTimer(xRate, yRate, sleepTime, page);
    EXPECT_TRUE((content != nullptr) && (!strcmp(content, "Two Arg")));
    ACE_FREE(content);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: SetIntervalTest005
 * @tc.desc: test setInterval args
 */
HWTEST_F(TimerModuleTddTest, SetIntervalTest005, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.eval js file setInterval_test005.js
     * @tc.expected:step1.eval js file success
     */
    JSValue page = CreatePage(ARG_TEST_005, strlen(ARG_TEST_005));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps:step2.click the noArg button
     * @tc.expected:step2.the value attribute is timer
     */
    double xRate = 0.05;
    const double yRate = 0.1;
    const uint8_t sleepTime = 1;
    char *content = TriggerTimer(xRate, yRate, sleepTime, page);
    EXPECT_TRUE(content == nullptr);

    /**
     * @tc.steps:step3.click the oneArg button
     * @tc.expected:step3.the value attribute is One Arg
     */
    xRate = 0.35;
    content = TriggerTimer(xRate, yRate, sleepTime, page);
    EXPECT_TRUE((content != nullptr) && (!strcmp(content, "Interval One")));
    ACE_FREE(content);

    /**
     * @tc.steps:step3.click two args button
     * @tc.steps:step3.the value attribute is Two Arg
     */
    xRate = 0.65;
    content = TriggerTimer(xRate, yRate, sleepTime, page);
    EXPECT_TRUE((content != nullptr) && (!strcmp(content, "Interval Two")));
    ACE_FREE(content);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: SetTimeoutTest006
 * @tc.desc: test setTimeout and clearTimeout api
 */
HWTEST_F(TimerModuleTddTest, SetTimeoutTest006, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.eval js file setTimeout_test006.js
     * @tc.expected:step1.eval js file success
     */
    JSValue page = CreatePage(CLEAR_TIME_OUT_006, strlen(CLEAR_TIME_OUT_006));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps:step2.click setTimeout button
     * @tc.expected:step2.the value attribute in data is trigger success
     */
    const double xRate = 0.25;
    const double yRate = 0.2;
    const uint8_t sleepTime = 1;
    char *content = TriggerTimer(xRate, yRate, sleepTime, page);
    EXPECT_FALSE((content == nullptr) || (strcmp(content, "trigger success")));
    ACE_FREE(content);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: SetIntervalTest007
 * @tc.desc: test setInterval and clearInterval api
 */
HWTEST_F(TimerModuleTddTest, SetIntervalTest007, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.eval js file setInterval_test007.js
     * @tc.expected:step1.eval js file success
     */
    JSValue page = CreatePage(INTERVAL_TEST_007, strlen(INTERVAL_TEST_007));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps:step2.click setTimeout button
     * @tc.expected:step2.the value attribute in data is trigger interval success
     */
    const double xRate = 0.25;
    const double yRate = 0.2;
    const uint8_t sleepTime = 1;
    char *content = TriggerTimer(xRate, yRate, sleepTime, page);
    EXPECT_FALSE((content == nullptr) || (strcmp(content, "trigger interval success")));
    ACE_FREE(content);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: ClearTimeoutTest008
 * @tc.desc: test delete not exists timer
 */
HWTEST_F(TimerModuleTddTest, ClearTimeoutTest008, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.eval js page clearInterval_test008.js
     * @tc.expected:step1.eval js page success
     */
    JSValue page = CreatePage(CLEAR_TIME_OUT_008, strlen(CLEAR_TIME_OUT_008));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps:step2.click the clearInterval button to clear not exist timer
     * @tc.expected:step2.the value attribute in data is clearSuccess
     */
    const double xRate = 0.3;
    double yRate = 0.15;
    const uint8_t sleepTime = 1;
    char *content = TriggerTimer(xRate, yRate, sleepTime, page);
    EXPECT_FALSE((content == nullptr) || (strcmp(content, "clearInterval")));
    ACE_FREE(content);

    /**
     * @tc.steps:step2.click the clearTimeout button to clear not exist timer
     * @tc.expected: the value attribute in data is clearSuccess
     */
    yRate = 0.45;
    content = TriggerTimer(xRate, yRate, sleepTime, page);
    EXPECT_FALSE((content == nullptr) || (strcmp(content, "clearSuccess")));
    ACE_FREE(content);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: ClearIntervalTest009
 * @tc.desc: check boundary value in setTimeout and setInterval
 */
HWTEST_F(TimerModuleTddTest, ClearIntervalTest009, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.eval js page clearInterval_test009.js
     * @tc.expected:step1.eval js page success
     */
    JSValue page = CreatePage(BUNDLE_09, strlen(BUNDLE_09));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps:step2.click minTime button and trigger timer
     * @tc.expected:step2.the value attribute in data is min
     */
    double xRate = 0.3;
    double yRate = 0.15;
    const uint8_t sleepTime = 1;
    char *content = TriggerTimer(xRate, yRate, sleepTime, page);
    EXPECT_FALSE((content == nullptr) || (strcmp(content, "min")));
    ACE_FREE(content);

    /**
     * @tc.steps:step3.click maxTime button and trigger timer
     * @tc.expected:step3.the value attribute in data is max
     */
    xRate = 0.6;
    content = TriggerTimer(xRate, yRate, sleepTime, page);
    EXPECT_FALSE((content == nullptr) || (strcmp(content, "max")));
    ACE_FREE(content);

    /**
     * @tc.steps:step4.click the minInterval button to trigger interval timer
     * @tc.expected:step4.the value attribue in data is min
     */
    xRate = 0.3;
    yRate = 0.4;
    content = TriggerTimer(xRate, yRate, sleepTime, page);
    EXPECT_FALSE((content == nullptr) || (strcmp(content, "min")));
    ACE_FREE(content);

    /**
     * @tc.steps:step5.click the maxInterval button to trigger interval timer
     * @tc.expected:step5.the value attribute in data is max
     */
    xRate = 0.6;
    content = TriggerTimer(xRate, yRate, sleepTime, page);
    EXPECT_FALSE((content == nullptr) || (strcmp(content, "max")));
    ACE_FREE(content);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: ClearIntervalTest009
 * @tc.desc: test the timer limit
 */
HWTEST_F(TimerModuleTddTest, TimerLimitTest010, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.eval js file timerLimit_test010.js
     * @tc.expected:step1.eval js file success
     */
    JSValue page = CreatePage(BUNDLE_10, strlen(BUNDLE_10));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps:step2.click timer button and trigger timer
     * @tc.expected:step2.the value attribute in data is timer4
     */
    const double xRate = 0.3;
    const double yRate = 0.25;
    const uint8_t sleepTime = 1;
    char *content = TriggerTimer(xRate, yRate, sleepTime, page);
    EXPECT_FALSE((content == nullptr) || (strcmp(content, "timer4")));
    ACE_FREE(content);
    DestroyPage(page);
    TDD_CASE_END();
}

#ifndef TDD_ASSERTIONS
void TimerModuleTddTest::RunTests()
{
    SetTimeoutTest001();
    SetIntervalTest002();
    SetTimeoutTest003();
    SetTimeoutTest004();
    SetIntervalTest005();
    SetTimeoutTest006();
    SetIntervalTest007();
    ClearTimeoutTest008();
    ClearIntervalTest009();
    TimerLimitTest010();
}
#endif
}
}
