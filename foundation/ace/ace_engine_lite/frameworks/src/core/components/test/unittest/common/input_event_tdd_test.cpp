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

#include "input_event_tdd_test.h"
#include "ui_label_button.h"

namespace OHOS {
namespace ACELite {
const char BUNDLE1[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "        return _c('stack', {staticClass: ['stack']}, [\n"
    "          _c('input', {'attrs': {'type': 'button', 'value': function() {return _vm.value}, 'ref':'button'},\n"
    "                       'staticClass': ['button'],\n"
    "                       'staticStyle': {'left': '30%',\n"
    "                                       'top': '5%',\n"
    "                                       'fontSize': function() { return _vm.fontSize;},\n"
    "                                       'fontFamily': function() {return _vm.fontFamily;},\n"
    "                                       'color': function() {return _vm.color;}},\n"
    "                        'onBubbleEvents' : {'click' : _vm.triggerNo}\n"
    "         }),\n"
    "          _c('input', {'attrs': {'type':'button', value:'value'},\n"
    "                       'staticClass':['button'],\n"
    "                       'staticStyle':{'left':'30%', 'top':'25%'},\n"
    "                       'onBubbleEvents':{'click':_vm.changeValue}\n"
    "          }),\n"
    "          _c('input', {'attrs':{'type':'button', 'value': 'fontFamily'},\n"
    "                       'staticClass':['button'],\n"
    "                      'staticStyle': {'left':'30%', 'top':'45%'},\n"
    "                      'onBubbleEvents':{'click':_vm.changeFontFamily}\n"
    "         }),\n"
    "         _c('input', {'attrs' : {'type': 'button','value' : 'fontSize'},\n"
    "                 'staticClass': ['button'],\n"
    "                 'staticStyle': {'left': '30%', 'top':'65%'},\n"
    "                 'onBubbleEvents':{'click':_vm.changeFontSize}\n"
    "        }),\n"
    "     _c('input', {'attrs': {'type': 'button', 'value': 'color'},\n"
    "                  'staticClass': ['button'],\n"
    "                  'staticStyle': {'left': '30%', 'top': '85%'},\n"
    "                  'onBubbleEvents':{'click': _vm.changeColor}\n"
    "         })\n"
    " ]);\n"
    " },\n"
    " styleSheet: {\n"
    "    classSelectors: {\n"
    "      stack: {\n"
    "        justifyContent: 'center',\n"
    "        alignItems: 'center',\n"
    "        left: 0,\n"
    "        right: 0,\n"
    "        height: '100%',\n"
    "        width: '100%'\n"
    "      },\n"
    "      button: {\n"
    "        width:'50%',\n"
    "        height:'15%',\n"
    "      }\n"
    "    }\n"
    "  },\n"
    "  data: {\n"
    "    value: 'chinese',\n"
    "    fontSize: 30,\n"
    "    fontFamily: 'HYQiHei-65S.otf',\n"
    "    color: 180\n"
    "  },\n"
    "  changeValue:function() {\n"
    "    if(this.value == 'chinese') {\n"
    "        this.value = 'english';\n"
    "    } else {\n"
    "        this.value = 'chinese';\n"
    "    }\n"
    "  },\n"
    "  changeFontSize:function() {\n"
    "    if(this.fontSize == 30) {\n"
    "        this.fontSize = 38;\n"
    "    } else {\n"
    "        this.fontSize = 30;\n"
    "    }\n"
    "  },\n"
    "  changeFontFamily:function () {\n"
    "    this.fontFamily == 'HYQiHei-65S.otf';\n"
    "  },\n"
    "  changeColor:function() {\n"
    "    if(this.color == 220) {\n"
    "        this.color = 80;\n"
    "    } else {\n"
    "        this.color = 220;\n"
    "    }\n"
    "  }\n"
    "});\n"
    "})();\n";

const char BUNDLE2[] =
    "(function () {\n"
    "    return new ViewModel({\n"
    "      render: function (vm) {\n"
    "        var _vm = vm || this;\n"
    "        return _c('stack', {staticClass: ['stack']}, [\n"
    "          _c('input', {'attrs': {type: 'button', 'value': 'clickOneArg'},\n"
    "                       'staticClass': ['button'],\n"
    "                       'staticStyle': {'left': '30%',\n"
    "                                       'top': '20%'},\n"
    "                       'onBubbleEvents' : {'click' : _vm.clickOneArg, 'longpress': _vm.pressOneArg}\n"
    "             }),\n"
    "         _c('input', {'attrs': {'type':'button', value:'value'},\n"
    "                      'staticClass':['button'],\n"
    "                      'staticStyle':{'left':'30%', 'top':'45%'},\n"
    "                      'onBubbleEvents':{'click':_vm.clickNoArg, 'longpress': _vm.pressNoArg}\n"
    "            }),\n"
    "          _c('text', {'attrs': {'value': function() {return _vm.value}},\n"
    "                      'staticClass': ['text']\n"
    "             })\n"
    "     ]);\n"
    "     },\n"
    "     styleSheet: {\n"
    "        classSelectors: {\n"
    "          stack: {\n"
    "            justifyContent: 'center',\n"
    "            alignItems: 'center',\n"
    "            left: 0,\n"
    "            right: 0,\n"
    "            height: '100%',\n"
    "            width: '100%'},\n"
    "          button: {\n"
    "            width:'60%',\n"
    "            height:'15%',\n"
    "          },\n"
    "          text: {\n"
    "            width:'50%',\n"
    "            fontSize:38,\n"
    "            textAlign:'center',\n"
    "            left:'20%',\n"
    "            top:'70%'\n"
    "          }\n"
    "        }\n"
    "      },\n"
    "      data: {\n"
    "        value: 'value'\n"
    "      },\n"
    "      clickOneArg: function(a) {\n"
    "               this.value = 'clickOne';\n"
    "      },\n"
    "      pressOneArg: function(a) {\n"
    "               this.value = 'pressOne';\n"
    "      },\n"
    "      clickNoArg: function() {\n"
    "          this.value = 'clickNo';\n"
    "      },\n"
    "      pressNoArg: function() {\n"
    "          this.value = 'pressNo';\n"
    "      }\n"
    "    });\n"
    "  })();\n";

const char BUNDLE3[] =
    "(function () {\n"
    "    return new ViewModel({\n"
    "      render: function (vm) {\n"
    "        var _vm = vm || this;\n"
    "        return _c('stack', {staticClass: ['stack']}, [\n"
    "          _c('input', {'attrs': {'type': 'checkbox', 'ref': 'checkbox',\n"
    "                       'checked': function() {return _vm.checked;}},\n"
    "                       'staticClass': ['checkbox'],\n"
    "                       'staticStyle': {'left': '30%',\n"
    "                                       'top': '20%'},\n"
    "                       'onBubbleEvents' : {'change' : _vm.changeEvent}\n"
    "             }),\n"
    "          _c('text', {'attrs': {'value': function() {return _vm.value;}},\n"
    "                      'staticClass': ['text'],\n"
    "                      'onBubbleEvents':{'click': _vm.changeChecked}\n"
    "             })\n"
    "     ]);\n"
    "     },\n"
    "     styleSheet: {\n"
    "        classSelectors: {\n"
    "          stack: {\n"
    "            justifyContent: 'center',\n"
    "            alignItems: 'center',\n"
    "            left: 0,\n"
    "            right: 0,\n"
    "            height: '100%',\n"
    "            width: '100%'}\n"
    "        },\n"
    "        checkbox: {\n"
    "            width: '15%',\n"
    "            height: '15%'\n"
    "        },\n"
    "        text: {\n"
    "            width: '50%',\n"
    "            fontSize:38,\n"
    "            textAlign:'center',\n"
    "            left: '20%',\n"
    "            top: '40%'\n"
    "        }\n"
    "      },\n"
    "      data: {\n"
    "        value: 'show',\n"
    "        checked: true\n"
    "      },\n"
    "      changeEvent: function(e) {\n"
    "          if (e.checked) {\n"
    "              this.value = 'false';\n"
    "          } else {\n"
    "              this.value = 'true';\n"
    "          }\n"
    "      },\n"
    "      changeChecked: function() {\n"
    "          this.checked = !this.checked;\n"
    "      }\n"
    "    });\n"
    "  })();\n";

const char BUNDLE4[] =
    "(function () {\n"
    "return new ViewModel({\n"
    "  render: function (vm) {\n"
    "    var _vm = vm || this;\n"
    "    return _c('stack', {staticClass: ['stack']}, [\n"
    "      _c('input', {'attrs': {'type': 'checkbox', 'checked': function() {return _vm.checked;}, 'ref':'checkbox'},\n"
    "                   'staticClass': ['checkbox'],\n"
    "                   'staticStyle': {'left': '40%', 'top': '10%'},\n"
    "                   'onBubbleEvents': {'change': _vm.changeEvent, 'click': _vm.clickEvent,\n"
    "                                      'longpress': _vm.pressEvent}\n"
    "         }),\n"
    "      _c('text', {'attrs': {'value': function() { return _vm.click; }},\n"
    "                  'staticClass': ['text'],\n"
    "                  'staticStyle': {'left': '20%', 'top': '30%'},\n"
    "        }),\n"
    "      _c('text', {'attrs': {'value': function() {return _vm.press;}},\n"
    "                  'staticClass': ['text'],\n"
    "                  'staticStyle': {'left': '20%', 'top': '50%'}\n"
    "        }),\n"
    "      _c('text', {'attrs': {'value': function() { return _vm.change;}},\n"
    "                  'staticClass': ['text'],\n"
    "                  'staticStyle': {'left': '20%', 'top': '70%'}\n"
    "        })\n"
    " ]);\n"
    " },\n"
    " styleSheet: {\n"
    "    classSelectors: {\n"
    "      stack: {\n"
    "        justifyContent: 'center',\n"
    "        alignItems: 'center',\n"
    "        left: 0,\n"
    "        right: 0,\n"
    "        height: '100%',\n"
    "        width: '100%'}\n"
    "    },\n"
    "    checkbox: {\n"
    "        width: '15%',\n"
    "        height: '15%'\n"
    "    },\n"
    "    text: {\n"
    "        width: '50%',\n"
    "        fontSize:38,\n"
    "        textAlign:'center'\n"
    "    }\n"
    "  },\n"
    "  data: {\n"
    "    click: 'click',\n"
    "    press: 'press',\n"
    "    change: 'change'\n"
    "  },\n"
    "  clickEvent: function() {\n"
    "    if (this.click == 'click') {\n"
    "      this.click = 'clicked';\n"
    "    } else {\n"
    "      this.click = 'click';\n"
    "    }\n"
    "  },\n"
    "  changeEvent: function(e) {\n"
    "    if (e.checked) {\n"
    "      this.change = 'true';\n"
    "    } else {\n"
    "      this.change = 'false';\n"
    "    }\n"
    "  },\n"
    "  pressEvent: function () {\n"
    "    if (this.press == 'press') {\n"
    "      this.press = 'pressed';\n"
    "    } else {\n"
    "      this.press = 'press';\n"
    "    }\n"
    "  }\n"
    "});\n"
    "})();\n";

const char BUNDLE5[] =
    "(function () {\n"
    "return new ViewModel({\n"
    "  render: function (vm) {\n"
    "    var _vm = vm || this;\n"
    "    return _c('stack', {staticClass: ['stack']}, [\n"
    "      _c('input', {'attrs': {'type': 'checkbox', 'checked': function() {return _vm.checked;}, 'ref':'checkbox'},\n"
    "                   'staticClass': ['checkbox'],\n"
    "                   'staticStyle': {'left': '30%',\n"
    "                                   'top': '30%'},\n"
    "                   'onBubbleEvents' : {'change' : _vm.changeEvent}\n"
    "         }),\n"
    "      _c('text', {'attrs': {'value': function() {return _vm.value;}},\n"
    "                  'staticClass': ['text'],\n"
    "                  'staticStyle': {'left': '30%', 'top': '60%'}\n"
    "         })\n"
    " ]);\n"
    " },\n"
    " styleSheet: {\n"
    "    classSelectors: {\n"
    "      stack: {\n"
    "        justifyContent: 'center',\n"
    "        alignItems: 'center',\n"
    "        left: 0,\n"
    "        right: 0,\n"
    "        height: '100%',\n"
    "        width: '100%'},\n"
    "    checkbox: {\n"
    "        width: '50%',\n"
    "        height: '15%'\n"
    "    },\n"
    "    text: {\n"
    "        width: '50%',\n"
    "        fontSize:38,\n"
    "        textAlign:'center',\n"
    "        left: '30%',\n"
    "        top: '70%'\n"
    "    }\n"
    "  }\n"
    "  },\n"
    "  data: {\n"
    "    checked: true,\n"
    "    value: 'show'\n"
    "  },\n"
    "  changeEvent: function(e) {\n"
    "    this.checked = !this.checked;\n"
    "    if (e.checked) {\n"
    "        this.value = 'true';\n"
    "    } else {\n"
    "        this.value = 'false';\n"
    "    }\n"
    "  }\n"
    "});\n"
    "})();\n";

const char BUNDLE6[] =
    "(function () {\n"
    "return new ViewModel({\n"
    "  render: function (vm) {\n"
    "    var _vm = vm || this;\n"
    "    return _c('div', {'staticClass' : ['container']} ,\n"
    "                [_i((function () { return _vm.buttonIfStatus; }),\n"
    "                    function() {return _c('input',\n"
    "                    {'attrs' : {'type' : 'button','value' : 'ifStatus','ref':'button'},\n"
    "                    'staticClass' : ['button']} )}),\n"
    "                _c('input', {'attrs' : {'type' : 'button','value' : 'changeIf'},\n"
    "                             'onBubbleEvents' : {'click' : _vm.changeButtonIf},\n"
    "                             'staticClass' : ['button']}),\n"
    "                _i((function () { return _vm.checkboxIfStatus; }),\n"
    "                    function() {return _c('input',\n"
    "                      {'attrs' : {'type' : 'checkbox', 'ref':'checkbox'},\n"
    "                       'staticClass' : ['checkbox']} )}),\n"
    "                    _c('input', {'attrs' : {'type' : 'button','value' : 'changeIf'},\n"
    "                                 'onBubbleEvents' : {'click' : _vm.changeCheckboxIf},\n"
    "                                 'staticClass' : ['button']}),\n"
    "                _i((function () { return _vm.radioIfStatus; }),\n"
    "                  function() {return _c('input',{\n"
    "                    'attrs' : {'type' : 'radio', 'ref':'radio'},\n"
    "                    'staticClass' : ['radio']} )}),\n"
    "                _c('input', {'attrs' : {'type' : 'button','value' : 'changeIf'},\n"
    "                            'onBubbleEvents' : {'click' : _vm.changeRadioIf},\n"
    "                            'staticClass' : ['button']})]\n"
    "    );\n"
    " },\n"
    " styleSheet: {\n"
    "    classSelectors: {\n"
    "    container: {\n"
    "        width: '100%',\n"
    "        height: '100%',\n"
    "        left: '0',\n"
    "        top: '0',\n"
    "        flexDirection: 'column'\n"
    "      },\n"
    "    button: {\n"
    "        width: '50%',\n"
    "        height: '10%',\n"
    "        marginLeft: '30%',\n"
    "        marginTop: '5%'\n"
    "    },\n"
    "    checkbox: {\n"
    "      marginTop: '5%',\n"
    "      marginLeft: '30%'\n"
    "    },\n"
    "    radio: {\n"
    "      marginTop: '5%',\n"
    "      marginLeft: '30%'\n"
    "    }\n"
    "  }\n"
    "},\n"
    "data: {\n"
    "  buttonIfStatus: true,\n"
    "  checkboxIfStatus: true,\n"
    "  radioIfStatus: true\n"
    "},\n"
    "changeButtonIf: function() {\n"
    "  if (this.buttonIfStatus) {\n"
    "    this.buttonIfStatus = false;\n"
    "  } else {\n"
    "    this.buttonIfStatus = true;\n"
    "  }\n"
    "},\n"
    "changeCheckboxIf: function() {\n"
    "  if (this.checkboxIfStatus) {\n"
    "    this.checkboxIfStatus = false;\n"
    "  } else {\n"
    "    this.checkboxIfStatus = true;\n"
    "  }\n"
    "},\n"
    "changeRadioIf: function() {\n"
    "  if (this.radioIfStatus) {\n"
    "    this.radioIfStatus = false;\n"
    "  } else {\n"
    "    this.radioIfStatus = true;\n"
    "  }\n"
    "}\n"
    "});\n"
    "})();\n";

const char BUNDLE7[] =
    "(function () {\n"
    "return new ViewModel({\n"
    "  render: function (vm) {\n"
    "    var _vm = vm || this;\n"
    "    return _c('div', {'staticClass' : ['container']} ,[\n"
    "            _c('input', {\n"
    "                'attrs' : {'type' : 'button','value' : 'ifStatus',\n"
    "                'show': function() {return _vm.buttonStatus;}, 'ref':'button'},\n"
    "                'staticClass' : ['button']\n"
    "              }),\n"
    "            _c('input', {'attrs' : {'type' : 'button','value' : 'changeShow'},\n"
    "                'onBubbleEvents' : {'click' : _vm.changeButtonShow},\n"
    "                'staticClass' : ['button']}),\n"
    "            _c('input', {'attrs' : {'type' : 'checkbox', 'show': function() { return _vm.checkboxStatus; },\n"
    "                         'ref': 'checkbox'},\n"
    "                'staticClass' : ['checkbox']}),\n"
    "            _c('input', {'attrs' : {'type' : 'button', 'value': 'show'},\n"
    "                'onBubbleEvents' : {'click' : _vm.changeCheckboxShow},\n"
    "                'staticClass' : ['button']}),\n"
    "            _c('input',{ 'attrs' : {'type' : 'radio', 'show': function() { return _vm.radioStatus; },\n"
    "                                    'ref': 'radio'},\n"
    "                'staticClass' : ['radio']}),\n"
    "            _c('input', {'attrs' : {'type' : 'button','value' : 'changeShow'},\n"
    "                'onBubbleEvents' : {'click' : _vm.changeRadioShow},\n"
    "                'staticClass' : ['button']})]\n"
    "    );\n"
    " },\n"
    " styleSheet: {\n"
    "    classSelectors: {\n"
    "    container: {\n"
    "        width: '100%',\n"
    "        height: '100%',\n"
    "        left: '0',\n"
    "        top: '0',\n"
    "        flexDirection: 'column'\n"
    "      },\n"
    "    button: {\n"
    "        width: '50%',\n"
    "        height: '10%',\n"
    "        marginLeft: '30%',\n"
    "        marginTop: '5%'\n"
    "    },\n"
    "    checkbox: {\n"
    "      marginTop: '5%',\n"
    "      marginLeft: '30%',\n"
    "      width: '10%',\n"
    "      height: '10%'\n"
    "    },\n"
    "    radio: {\n"
    "      marginTop: '5%',\n"
    "      marginLeft: '30%',\n"
    "      width: '10%',\n"
    "      height: '10%'\n"
    "    }\n"
    "  }\n"
    "},\n"
    "data: {\n"
    "  buttonStatus: true,\n"
    "  checkboxStatus: true,\n"
    "  radioStatus: true\n"
    "},\n"
    "changeButtonShow: function() {\n"
    "  if (this.buttonStatus) {\n"
    "    this.buttonStatus = false;\n"
    "  } else {\n"
    "    this.buttonStatus = true;\n"
    "  }\n"
    "},\n"
    "changeCheckboxShow: function() {\n"
    "  if (this.checkboxStatus) {\n"
    "    this.checkboxStatus = false;\n"
    "  } else {\n"
    "    this.checkboxStatus = true;\n"
    "  }\n"
    "},\n"
    "changeRadioShow: function() {\n"
    "  if (this.radioStatus) {\n"
    "    this.radioStatus = false;\n"
    "  } else {\n"
    "    this.radioStatus = true;\n"
    "  }\n"
    "}\n"
    "});\n"
    "})();\n";

const char BUNDLE8[] =
    "(function () {\n"
    "return new ViewModel({\n"
    "  render: function (vm) {\n"
    "    var _vm = vm || this;\n"
    "    return _c('stack', {'staticClass' : ['stack']} ,[\n"
    "            _c('div', {'staticClass': ['container'],\n"
    "                       'staticStyle': {'left': '10%', 'top': '5%'}}, [\n"
    "              _l(function() { return _vm.buttonData; },\n"
    "                 function($item, $idx) {\n"
    "                   return _c('input', {'attrs': {'value': function() {return $item;},\n"
    "                                       'ref': function() {return $item;}},\n"
    "                             'staticClass': ['stackButton'],\n"
    "                             'staticStyle': {'marginLeft': '20%'}});\n"
    "                  })]),\n"
    "            _c('input', {'attrs' : {'type' : 'button','value' : 'changeFor'},\n"
    "                'onBubbleEvents' : {'click' : _vm.changeButtonFor},\n"
    "                'staticClass' : ['button'],\n"
    "                'staticStyle': {'left': '30%', 'top': '30%'}}),\n"
    "            _c('div', {'staticClass': ['container'],\n"
    "                       'staticStyle': {'left': '10%', 'top': '40%'}},[\n"
    "              _l(function() { return _vm.checkboxData;},\n"
    "                 function($item, $idx) {\n"
    "                   return _c('input', {'attrs': {'ref': function() {return $item;}, 'type': 'checkbox'},\n"
    "                             'staticStyle': {'marginLeft': '20%', 'marginTop': '5%'}});\n"
    "                })]),\n"
    "            _c('input', {'attrs' : {'type' : 'button', 'value': 'changeFor'},\n"
    "                'onBubbleEvents' : {'click' : _vm.changeCheckboxFor},\n"
    "                'staticClass' : ['button'],\n"
    "                'staticStyle': {'left': '30%', 'top': '60%'}}),\n"
    "            _c('div', {'staticClass': ['container'],\n"
    "                       'staticStyle': {'left': '10%', 'top': '70%'}}, [\n"
    "              _l(function() {return _vm.radioData;},\n"
    "               function($item, $idx) {\n"
    "                 return _c('input', {'attrs': {'name': function() { return $item; }, 'type': 'radio',\n"
    "                                       'ref': function() {return $item;}},\n"
    "                                     'staticStyle': {'marginLeft': '20%', 'marginTop': '5%'}});\n"
    "               })\n"
    "            ]),\n"
    "            _c('input', {'attrs' : {'type' : 'button','value' : 'changeFor'},\n"
    "                'onBubbleEvents' : {'click' : _vm.changeRadioFor},\n"
    "                'staticClass' : ['button'],\n"
    "                'staticStyle': {'left': '30%', 'top': '80%'}})]\n"
    "    );\n"
    " },\n"
    " styleSheet: {\n"
    "    classSelectors: {\n"
    "    stack: {\n"
    "        width: '100%',\n"
    "        height: '100%',\n"
    "        left: '0',\n"
    "        top: '0'\n"
    "      },\n"
    "    container: {\n"
    "      width: '100%',\n"
    "      height: '20%'\n"
    "    },\n"
    "    button: {\n"
    "        width: '50%',\n"
    "        height: '10%',\n"
    "        marginLeft: '30%',\n"
    "        marginTop: '5%'\n"
    "    }\n"
    "  }\n"
    "},\n"
    "data: {\n"
    "  buttonData: ['data1', 'data2'],\n"
    "  checkboxData: ['checkbox1', 'checkbox2'],\n"
    "  radioData: ['name1', 'name2']\n"
    "},\n"
    "changeButtonFor: function() {\n"
    "  if (this.buttonData.length == 2) {\n"
    "    this.buttonData = ['data1'];\n"
    "  } else {\n"
    "    this.buttonData = ['data1', 'data2'];\n"
    "  }\n"
    "},\n"
    "changeCheckboxFor: function() {\n"
    "  if (this.checkboxData.length == 2) {\n"
    "    this.checkboxData = ['checkbox1'];\n"
    "  } else {\n"
    "    this.checkboxData = ['checkbox1', 'checkbox2'];\n"
    "  }\n"
    "},\n"
    "changeRadioFor: function() {\n"
    "  if (this.radioData.length == 2) {\n"
    "    this.radioData = ['name1'];\n"
    "  } else {\n"
    "    this.radioData = ['name1', 'name2'];\n"
    "  }\n"
    "}\n"
    "});\n"
    "})();\n";

const char BUNDLE9[] =
    "    (function () {\n"
    "return new ViewModel({\n"
    "  render: function (vm) {\n"
    "    var _vm = vm || this;\n"
    "    return _c('stack', {'staticClass' : ['stack']} ,\n"
    "                [_c('input', {'attrs': {'name': 'name', 'ref': 'radio1', 'type':'radio'},\n"
    "                              'staticStyle':{'left':'10%', 'top':'30%', 'width': '15%', 'height': '15%'}}),\n"
    "                 _c('input', {'attrs': {'name': 'name', 'ref': 'radio2', 'type': 'radio'},\n"
    "                              'staticStyle': {'left': '30%', 'top': '30%', 'width': '15%', 'height': '15%'}}),\n"
    "                _c('input', {'attrs': {'name': 'name1', 'ref': 'radio3','type': 'radio'},\n"
    "                              'staticStyle': {'left': '60%', 'top': '30%', 'width': '15%', 'height': '15%'}})\n"
    "                ]);\n"
    " },\n"
    " styleSheet: {\n"
    "    classSelectors: {\n"
    "    stack: {\n"
    "        width: '100%',\n"
    "        height: '100%',\n"
    "        left: '0',\n"
    "        top: '0',\n"
    "        flexDirection: 'column'\n"
    "      }\n"
    "  }\n"
    "},\n"
    "data: {}\n"
    "});\n"
    "})();\n";

const char BUNDLE10[] =
    "(function () {\n"
    "return new ViewModel({\n"
    "  render: function (vm) {\n"
    "    var _vm = vm || this;\n"
    "    return _c('stack', {staticClass: ['stack']}, [\n"
    "      _c('text', {'attrs': {'value': function() {return 'Hello ' + _vm.value;}},\n"
    "                  'staticClass': ['text']\n"
    "         }),\n"
    "      _c('input', {'attrs': {'type': 'button', 'name': 'input1', 'value': 'button',\n"
    "                             'checked': true, 'ref':'button'},\n"
    "                  'staticClass': ['button'],\n"
    "                  'staticStyle': {'left': '30%', 'top': '40%'},\n"
    "                  'onBubbleEvents': {'click': _vm.clickEvent, 'longpress': _vm.pressEvent}\n"
    "                }),\n"
    "      _c('input', {'attrs': {'type': 'checkbox', 'checked': false, 'value': 'check',\n"
    "                             'name': 'input2', 'ref':'checkbox'},\n"
    "                   'staticClass': ['checkbox'],\n"
    "                   'staticStyle': {'left': '30%', 'top': '60%'},\n"
    "                   'onBubbleEvents': {'change': _vm.changeCheckbox}}),\n"
    "      _c('input', {'attrs': {'type': 'radio', 'checked': false, 'value': 'radio', 'name': 'input3',\n"
    "                             'ref': 'radio'},\n"
    "                   'staticClass': ['radio'],\n"
    "                   'staticStyle': {'left': '30%', 'top': '80%'},\n"
    "                   'onBubbleEvents': {'click': _vm.changeRadio}})\n"
    " ]);\n"
    " },\n"
    " styleSheet: {\n"
    "    classSelectors: {\n"
    "      stack: {\n"
    "        justifyContent: 'center',\n"
    "        alignItems: 'center',\n"
    "        left: 0,\n"
    "        right: 0,\n"
    "        height: '100%',\n"
    "        width: '100%'}\n"
    "    },\n"
    "    button: {\n"
    "        width: '30%',\n"
    "        height: '20%'\n"
    "    },\n"
    "    text: {\n"
    "        width: '80%',\n"
    "        fontSize:38,\n"
    "        textAlign:'center',\n"
    "        left: '30%',\n"
    "        top: '10%'\n"
    "    },\n"
    "    checkbox: {\n"
    "        width: '50%',\n"
    "        height: '20%'\n"
    "    },\n"
    "    radio: {\n"
    "        width: '20%',\n"
    "        height: '20%'\n"
    "    }\n"
    "  },\n"
    "  data: {\n"
    "    value: 'World'\n"
    "  },\n"
    "  clickEvent: function() {\n"
    "      this.value = '12331';\n"
    "  },\n"
    "  pressEvent: function() {\n"
    "      this.value = 'China';\n"
    "  },\n"
    "  changeCheckbox: function() {\n"
    "      this.value = this.value + 11;\n"
    "  },\n"
    "  changeRadio: function() {\n"
    "      this.value = this.value + '!';\n"
    "  }\n"
    "});\n"
    "})();\n";

const char BUNDLE11[] =
    "(function() {\n"
    "return new ViewModel({\n"
    "render:function (vm) {\n"
    "  var _vm = vm || this;\n"
    "  return _c('stack', {'staticClass' : ['stack']} , [\n"
    "    _c('input', {'attrs' : {'type' : 'checkbox'},\n"
    "      'staticStyle' : {'left' : '20%','top' : '20%'},\n"
    "      'onBubbleEvents' : {'click' : _vm.clickOnly}} ),\n"
    "    _c('input', {'attrs' : {'type' : 'checkbox'},\n"
    "      'staticStyle' : {'left' : '20%','top' : '40%','width' : '15%','height' : '15%'},\n"
    "      'onBubbleEvents' : {'change' : _vm.changeOnly}}),\n"
    "    _c('input', {'attrs' : {'type' : 'checkbox'},\n"
    "      'staticStyle' : {'left' : '20%','top' : '60%','width' : '15%','height' : '15%'},\n"
    "      'onBubbleEvents' : {'click' : _vm.clickOnly,'change' : _vm.changeOnly}}),\n"
    "    _c('text', {'attrs' : {'value' : function () {return (_vm.click) + ' ' + (_vm.change)}},\n"
    "      'staticClass' : ['text']} )\n"
    "      ])},\n"
    "styleSheet: {\n"
    "  classSelectors : {\n"
    "    'stack' : {\n"
    "      'width' : '100%',\n"
    "      'height' : '100%'\n"
    "    },\n"
    "    'text' : {\n"
    "      'width' : '40%',\n"
    "      'top' : '80%',\n"
    "      'left' : '30%'\n"
    "    }\n"
    "  },\n"
    "},\n"
    "data : {\n"
    "  'click' : 'click',\n"
    "  'change' : 'none'\n"
    "},\n"
    "clickOnly: function() {\n"
    "  if (this.click == 'click') {\n"
    "    this.click = 'clicked';\n"
    "  } else {\n"
    "    this.click = 'click';\n"
    "  }\n"
    "},\n"
    "changeOnly: function(e) {\n"
    "  if (e.checked) {\n"
    "    this.change = 'changed';\n"
    "  } else {\n"
    "    this.change = 'change';\n"
    "  }\n"
    "}\n"
    "});\n"
    "})();\n";

const char BUNDLE12[] =
    "(function() {\n"
    "return new ViewModel({\n"
    "render:function (vm) {\n"
    "  var _vm = vm || this;\n"
    "  return _c('stack', {'staticClass' : ['stack']} , [\n"
    "    _c('input', {'attrs' : {'type' : 'radio'},\n"
    "      'staticStyle' : {'left' : '20%','top' : '20%'},\n"
    "      'onBubbleEvents' : {'click' : _vm.clickOnly}} ),\n"
    "    _c('input', {'attrs' : {'type' : 'radio'},\n"
    "      'staticStyle' : {'left' : '20%','top' : '40%','width' : '15%','height' : '15%'},\n"
    "      'onBubbleEvents' : {'change' : _vm.changeOnly}}),\n"
    "    _c('input', {'attrs' : {'type' : 'radio'},\n"
    "      'staticStyle' : {'left' : '20%','top' : '60%','width' : '15%','height' : '15%'},\n"
    "      'onBubbleEvents' : {'click' : _vm.clickOnly,'change' : _vm.changeOnly}}),\n"
    "    _c('text', {'attrs' : {'value' : function () {return (_vm.click) + ' ' + (_vm.change)}},\n"
    "      'staticClass' : ['text']} )\n"
    "      ])},\n"
    "styleSheet: {\n"
    "  classSelectors : {\n"
    "    'stack' : {\n"
    "      'width' : '100%',\n"
    "      'height' : '100%'\n"
    "    },\n"
    "    'text' : {\n"
    "      'width' : '40%',\n"
    "      'top' : '80%',\n"
    "      'left' : '30%'\n"
    "    }\n"
    "  },\n"
    "},\n"
    "data : {\n"
    "  'click' : 'click',\n"
    "  'change' : 'none'\n"
    "},\n"
    "clickOnly: function() {\n"
    "  if (this.click == 'click') {\n"
    "    this.click = 'clicked';\n"
    "  } else {\n"
    "    this.click = 'click';\n"
    "  }\n"
    "},\n"
    "changeOnly: function(e) {\n"
    "  if (e.checked) {\n"
    "    this.change = 'changed';\n"
    "  } else {\n"
    "    this.change = 'change';\n"
    "  }\n"
    "}\n"
    "});\n"
    "})();\n";

void InputEventTddTest::ClickPosition(const double xRate, const double yRate) const
{
    // compute the position
    const uint8_t offset = 10;
    const uint16_t x = GetHorizontalResolution() * xRate + offset;
    const uint16_t y = GetVerticalResolution() * yRate + offset;
    Click(x, y);
}

void InputEventTddTest::LongPressPosition(const double xRate, const double yRate) const
{
    // compute the position
    const uint8_t offset = 10;
    const uint16_t x = GetHorizontalResolution() * xRate + offset;
    const uint16_t y = GetVerticalResolution() * yRate + offset;
    LongPress(x, y);
}

UIView *InputEventTddTest::GetComponent(JSValue page, const char *name) const
{
    JSValue refs = JSObject::Get(page, "$refs");
    JSValue targetRef = JSObject::Get(refs, name);
    if (JSUndefined::Is(targetRef)) {
        return nullptr;
    }
    UIView *target = ComponentUtils::GetViewFromBindingObject(targetRef);
    JSRelease(refs);
    JSRelease(targetRef);
    return target;
}

/**
 * @tc.name:InputBindingTest001
 * @tc.desc:test the input binding data
 */
HWTEST_F(InputEventTddTest, InputBindingTest001, TestSize.Level0)
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps:step1.eval the first js bundle
     * @tc.expected:step1.eval js bundle success
     */
    JSValue page = CreatePage(BUNDLE1, strlen(BUNDLE1));
    EXPECT_FALSE(JSUndefined::Is(page));
    if (JSUndefined::Is(page)) {
        TDD_CASE_END();
        return;
    }

    /**
     * @tc.steps: step2.click the position(0.3, 0.25)
     * @tc.expected: step2.the text value of UILabelButton is chinese
     */
    const double xRate = 0.3;
    double yRate = 0.25;
    ClickPosition(xRate, yRate);
    UILabelButton *button = reinterpret_cast<UILabelButton *>(GetComponent(page, "button"));
    EXPECT_FALSE(button == nullptr);
    if (button == nullptr) {
        DestroyPage(page);
        TDD_CASE_END();
        return;
    }
    const char *textValue = button->GetText();
    EXPECT_FALSE((textValue == nullptr) || (strcmp(textValue, "english")));

    /**
     * @tc.steps: step3.click the position(0.3, 0.45)
     * @tc.expected: step3.the attribute fontFamily is 'HYQiHei-65S.otf'
     */
    yRate = 0.45;
    ClickPosition(xRate, yRate);
    char *fontFamily = JSObject::GetString(page, "fontFamily");
    EXPECT_FALSE((fontFamily == nullptr) || (strcmp(fontFamily, "HYQiHei-65S.otf")));
    ACE_FREE(fontFamily);

    InputBindingTest001Extra(page);
}

void InputEventTddTest::InputBindingTest001Extra(JSValue page)
{
    /**
     * @tc.steps: step4.click the fontSize button
     * @tc.expected: step4.the fontSize attribute value is 38
     */
    const double xRate = 0.3;
    double yRate = 0.65;
    ClickPosition(xRate, yRate);
    int8_t fontSize = JSObject::GetNumber(page, "fontSize");
    const int8_t targetFontSize = 38;
    EXPECT_TRUE(fontSize == targetFontSize);

    /**
     * @tc.steps:step5.click the color button
     * @tc.expected:step5.the color value is 220
     */
    double newYRate = 0.85;
    ClickPosition(xRate, newYRate);
    int16_t colorValue = JSObject::GetNumber(page, "color");
    const int16_t targetColor = 220;
    EXPECT_TRUE(colorValue == targetColor);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: InputEventTest002
 * @tc.desc: test the click and longpress event
 */
HWTEST_F(InputEventTddTest, InputEventTest002, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.eval the js bundle2
     * @tc.expected: step1.eval js success
     */
    JSValue page = CreatePage(BUNDLE2, strlen(BUNDLE2));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps:step2.click the one arg button
     * @tc.expected: step2.the value attribute is clickOne
     */
    const double xRate = 0.5;
    double yRate = 0.3;
    ClickPosition(xRate, yRate);
    char *textValue = JSObject::GetString(page, "value");
    EXPECT_FALSE((textValue == nullptr) || (strcmp(textValue, "clickOne")));
    ACE_FREE(textValue);

    /**
     * @tc.steps: step3.longpress the one arg button
     * @tc.expected: step3.the value of attribute is pressOne
     */
    LongPressPosition(xRate, yRate);
    textValue = JSObject::GetString(page, "value");
    EXPECT_FALSE((textValue == nullptr) || (strcmp(textValue, "pressOne")));
    ACE_FREE(textValue);

    /**
     * @tc.steps: step4.click the no arg button
     * @tc.expected: step4.the value of attribute is 'clickNo'
     */
    yRate = 0.5;
    ClickPosition(xRate, yRate);
    textValue = JSObject::GetString(page, "value");
    EXPECT_FALSE((textValue == nullptr) || (strcmp(textValue, "clickNo")));
    ACE_FREE(textValue);

    /**
     * @tc.steps: step5.longpress the no arg button
     * @tc.expected: step5.the value of attribute is 'pressNo'
     */
    LongPressPosition(xRate, yRate);
    textValue = JSObject::GetString(page, "value");
    EXPECT_FALSE((textValue == nullptr) || (strcmp(textValue, "pressNo")));
    ACE_FREE(textValue);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name:ChangeEventTest003
 * @tc.desc:test the change event in checkbox
 */
HWTEST_F(InputEventTddTest, ChangeEventTest003, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1.eval the js bundle3
     * @tc.expected: step1.eval js success
     */
    JSValue page = CreatePage(BUNDLE3, strlen(BUNDLE3));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps: step2.click the checkbox button
     * @tc.expected: step2.the checkbox state is selected, the value attribute is true
     */
    double xRate = 0.3;
    double yRate = 0.2;
    ClickPosition(xRate, yRate);
    UICheckBox *checkbox = reinterpret_cast<UICheckBox *>(GetComponent(page, "checkbox"));
    EXPECT_TRUE((checkbox != nullptr) && (checkbox->GetState() == UICheckBox::UNSELECTED));
    char *value = JSObject::GetString(page, "value");
    EXPECT_FALSE((value == nullptr) || (strcmp(value, "true")));
    ACE_FREE(value);

    /**
     * @tc.steps: step3.click the text button
     * @tc.expected: step3.the checkbox state is unselected, the value attribute is true
     */
    xRate = 0.2;
    yRate = 0.4;
    ClickPosition(xRate, yRate);
    EXPECT_TRUE((checkbox != nullptr) && (checkbox->GetState() == UICheckBox::UICheckBoxState::UNSELECTED));
    value = JSObject::GetString(page, "value");
    EXPECT_FALSE((value == nullptr) || (strcmp(value, "true")));
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: CheckboxEventTest004
 * @tc.desc: test the longpress, click and change event
 */
HWTEST_F(InputEventTddTest, CheckboxEventTest004, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1.eval js bundle4
     * @tc.expected: step1.eval js success
     */
    JSValue page = CreatePage(BUNDLE4, strlen(BUNDLE4));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps: step2.click the checkbox to trigger the click and change event
     * @tc.expected: step2.the status of checkbox is selected,
     * the click,press change attribute is clicked, press and true
     */
    const double xRate = 0.4;
    const double yRate = 0.1;
    ClickPosition(xRate, yRate);
    UICheckBox *checkbox = reinterpret_cast<UICheckBox *>(GetComponent(page, "checkbox"));
    EXPECT_TRUE((checkbox != nullptr) && (checkbox->GetState() == UICheckBox::UICheckBoxState::SELECTED));
    char *clickValue = JSObject::GetString(page, "click");
    EXPECT_FALSE((clickValue == nullptr) || (strcmp(clickValue, "clicked")));
    ACE_FREE(clickValue);
    char *pressValue = JSObject::GetString(page, "press");
    EXPECT_FALSE((pressValue == nullptr) || (strcmp(pressValue, "press")));
    ACE_FREE(pressValue);
    char *changeValue = JSObject::GetString(page, "change");
    EXPECT_FALSE((changeValue == nullptr) || (strcmp(changeValue, "true")));
    ACE_FREE(changeValue);

    /**
     * @tc.steps: step3.longpress the checkbox to trigger the click and change event
     * @tc.expected:step3.the state of checkbox is selected, and the click, press,
     * change attribute is clicked, pressed and true
     */
    LongPressPosition(xRate, yRate);
    EXPECT_TRUE((checkbox != nullptr) && (checkbox->GetState() == UICheckBox::UICheckBoxState::SELECTED));
    clickValue = JSObject::GetString(page, "click");
    EXPECT_FALSE((clickValue == nullptr) || (strcmp(clickValue, "clicked")));
    ACE_FREE(clickValue);
    pressValue = JSObject::GetString(page, "press");
    EXPECT_FALSE((pressValue == nullptr) || (strcmp(pressValue, "pressed")));
    ACE_FREE(pressValue);
    changeValue = JSObject::GetString(page, "change");
    EXPECT_FALSE((changeValue == nullptr) || (strcmp(changeValue, "true")));
    ACE_FREE(changeValue);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: ChangeEventTest005
 * @tc.desc: test the looper change event
 */
HWTEST_F(InputEventTddTest, ChangeEventTest005, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1.eval js bundle5 to create the page
     * @tc.expected: step1.eval js success
     */
    JSValue page = CreatePage(BUNDLE5, strlen(BUNDLE5));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps: step2.get the state of checkbox
     * @tc.expected: step2.the state of checkbox is unselected
     */
    UICheckBox *checkbox = reinterpret_cast<UICheckBox *>(GetComponent(page, "checkbox"));
    EXPECT_TRUE((checkbox != nullptr) && (checkbox->GetState() == UICheckBox::UICheckBoxState::SELECTED));

    /**
     * @tc.steps: step3. clcik the checkbox button to trigger the change event
     * @tc.expected: step3.the state of checbox is unselected, the value attribute is false
     */
    const double xRate = 0.3;
    const double yRate = 0.3;
    ClickPosition(xRate, yRate);
    EXPECT_TRUE((checkbox != nullptr) && (checkbox->GetState() == UICheckBox::UICheckBoxState::UNSELECTED));
    char *textValue = JSObject::GetString(page, "value");
    EXPECT_FALSE((textValue == nullptr) || (strcmp(textValue, "false")));
    ACE_FREE(textValue);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: InputIfTest006
 * @tc.expected: test the if attribute
 */
HWTEST_F(InputEventTddTest, InputIfTest006, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1.eval the if test bundle
     * @tc.expected: step1.eval the js success
     */
    JSValue page = CreatePage(BUNDLE6, strlen(BUNDLE6));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps:step2.click the change buttonIf button
     * @tc.expected: step2.the first button is disappear
     */
    double xRate = 0.3;
    double yRate = 0.2;
    const uint8_t sleepTime = 50;
    ClickPosition(xRate, yRate);
    usleep(sleepTime);
    UIView *view = GetComponent(page, "button");
    EXPECT_TRUE(view == nullptr);

    /**
     * @tc.steps: step3.click the change buttonIf button
     * tc.expected: step3.the first button is shown
     */
    xRate = 0.3;
    yRate = 0.05;
    ClickPosition(xRate, yRate);
    usleep(sleepTime);
    view = GetComponent(page, "button");
    EXPECT_FALSE(view == nullptr);

    /**
     * @tc.steps: step4.click the changeCheckbox button
     * @tc.expected: step4.the checkbox is disappear
     */
    yRate = 0.5;
    ClickPosition(xRate, yRate);
    usleep(sleepTime);
    view = GetComponent(page, "checkbox");
    EXPECT_TRUE(view == nullptr);

    /**
     * @tc.steps: step5.click the changeCheckbox button
     * @tc.expected: step5. the checkbox is shown
     */
    yRate = 0.35;
    ClickPosition(xRate, yRate);
    usleep(sleepTime);
    view = GetComponent(page, "checkbox");
    EXPECT_FALSE(view == nullptr);

    /**
     * @tc.steps: step6.click the changeRadio button
     * @tc.steps: step6.the radio is disappear
     */
    yRate = 0.8;
    ClickPosition(xRate, yRate);
    usleep(sleepTime);
    view = GetComponent(page, "radio");
    EXPECT_TRUE(view == nullptr);

    /**
     * @tc.expected: step7.click the changeRadio button
     * @tc.exected: step7.the radio button is shown
     */
    yRate = 0.65;
    ClickPosition(xRate, yRate);
    usleep(sleepTime);
    view = GetComponent(page, "radio");
    EXPECT_FALSE(view == nullptr);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: InputShowTest007
 * @tc.expected: test the if attribute
 */
HWTEST_F(InputEventTddTest, InputShowTest007, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1.eval the showTest bundle
     * @tc.expected: step1.eval js success
     */
    JSValue page = CreatePage(BUNDLE7, strlen(BUNDLE7));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps: step2.click the changeButtonShow button
     * @tc.expected: step2.the visible attribute of button is false
     */
    const double xRate = 0.3;
    double yRate = 0.2;
    ClickPosition(xRate, yRate);
    UILabelButton *button = reinterpret_cast<UILabelButton *>(GetComponent(page, "button"));
    EXPECT_TRUE((button != nullptr) && (!button->IsVisible()));

    /**
     * @tc.steps: step3.click the changeButton button
     * @tc.expected: step3.the visiblity of button is true
     */
    yRate = 0.05;
    ClickPosition(xRate, yRate);
    EXPECT_TRUE((button != nullptr) && (button->IsVisible()));

    /**
     * @tc.steps: step4.click the changeCheckboxButton button
     * @tc.expected: step4.the visibility of checkbox is false
     */
    yRate = 0.5;
    ClickPosition(xRate, yRate);
    UICheckBox *checkbox = reinterpret_cast<UICheckBox *>(GetComponent(page, "checkbox"));
    EXPECT_TRUE((checkbox != nullptr) && (!checkbox->IsVisible()));

    /**
     * @tc.steps: step5.click the changeCheckboxButton button
     * @tc.expected: step5.the visibility of checkbox is true
     */
    yRate = 0.35;
    ClickPosition(xRate, yRate);
    EXPECT_TRUE((checkbox != nullptr) && (checkbox->IsVisible()));

    /**
     * @tc.steps: step6.click the changeRadioShow button
     * @tc.expected: step6.the visibility of radio is false
     */
    yRate = 0.8;
    ClickPosition(xRate, yRate);
    UIRadioButton *radio = reinterpret_cast<UIRadioButton *>(GetComponent(page, "radio"));
    EXPECT_TRUE((radio != nullptr) && (!radio->IsVisible()));

    /**
     * @tc.steps: step7.click the changeRadioShow button
     * @tc.expected: step7.the visibility of radio is true
     */
    yRate = 0.65;
    ClickPosition(xRate, yRate);
    EXPECT_TRUE((radio != nullptr) && (radio->IsVisible()));
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: InputForTest008
 * @tc.desc: test the for attribute in input
 */
HWTEST_F(InputEventTddTest, InputForTest008, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1.eval the for test js
     * @tc.expected: step1.eval js success
     */
    JSValue page = CreatePage(BUNDLE8, strlen(BUNDLE8));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps: step2.click the 'changeButtonFor' button
     * @tc.steps: step2. the button2 is disappear
     */
    const double xRate = 0.3;
    double yRate = 0.3;
    const uint8_t sleepTime = 50;
    ClickPosition(xRate, yRate);
    usleep(sleepTime);
    UIView *view1 = GetComponent(page, "data1");
    EXPECT_FALSE(view1 == nullptr);
    UIView *view2 = GetComponent(page, "data2");
    EXPECT_TRUE(view2 != nullptr);

    /**
     * @tc.steps:step3.click the "changeButtonFor" button
     * @tc.expected: step3.the button2 is shown
     */
    ClickPosition(xRate, yRate);
    usleep(sleepTime);
    view1 = GetComponent(page, "data1");
    EXPECT_FALSE(view1 == nullptr);
    view2 = GetComponent(page, "data2");
    EXPECT_FALSE(view2 == nullptr);

    /**
     * @tc.steps: step4.click the "changeCheckboxFor" button
     * @tc.expected: step4.the checkbox2 is disappear
     */
    yRate = 0.6;
    ClickPosition(xRate, yRate);
    usleep(sleepTime);
    view1 = GetComponent(page, "checkbox1");
    EXPECT_FALSE(view1 == nullptr);
    view2 = GetComponent(page, "checkbox2");
    EXPECT_TRUE(view2 != nullptr);

    /**
     * @tc.steps: step5.click the "changeCheckboxFor" button
     * @tc.expected: step5.the checkbox2 is appear
     */
    ClickPosition(xRate, yRate);
    usleep(sleepTime);
    view1 = GetComponent(page, "checkbox1");
    EXPECT_FALSE(view1 == nullptr);
    view2 = GetComponent(page, "checkbox2");
    EXPECT_FALSE(view2 == nullptr);

    /**
     * @tc.steps: step6.click the "changeRadioFor" button
     * @tc.expected: step6.the radio2 is disappear
     */
    yRate = 0.8;
    ClickPosition(xRate, yRate);
    usleep(sleepTime);
    view1 = GetComponent(page, "name1");
    EXPECT_FALSE(view1 == nullptr);
    view2 = GetComponent(page, "name2");
    EXPECT_TRUE(view2 != nullptr);

    /**
     * @tc.steps: step7.click the "changeRadioFor" button
     * @tc.expected: step7.the radio2 button is appear
     */
    ClickPosition(xRate, yRate);
    usleep(sleepTime);
    view1 = GetComponent(page, "name1");
    EXPECT_FALSE(view1 == nullptr);
    view2 = GetComponent(page, "name2");
    EXPECT_FALSE(view2 == nullptr);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name:RadioTest009
 * @tc.name: test the name attribute in radio
 */
HWTEST_F(InputEventTddTest, RadioTest009, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1.eval js bundle
     * @tc.expected: step1.eval js bundle success
     */
    JSValue page = CreatePage(BUNDLE9, strlen(BUNDLE9));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps: step2.click the first radioButton
     * @tc.expected: step2.the first radio state is selected,
     * the second radio state is unselected, the third radio state is unselected
     */
    double xRate = 0.1;
    const double yRate = 0.3;
    ClickPosition(xRate, yRate);
    UIRadioButton *radio1 = reinterpret_cast<UIRadioButton *>(GetComponent(page, "radio1"));
    EXPECT_TRUE((radio1 != nullptr) && (radio1->GetState() == UICheckBox::UICheckBoxState::SELECTED));
    UIRadioButton *radio2 = reinterpret_cast<UIRadioButton *>(GetComponent(page, "radio2"));
    EXPECT_TRUE((radio2 != nullptr) && (radio2->GetState() == UICheckBox::UICheckBoxState::UNSELECTED));
    UIRadioButton *radio3 = reinterpret_cast<UIRadioButton *>(GetComponent(page, "radio3"));
    EXPECT_TRUE((radio3 != nullptr) && (radio3->GetState() == UICheckBox::UICheckBoxState::UNSELECTED));

    /**
     * @tc.steps: step3.click the last radiobutton
     * @tc.expected: step3.the last button of radio state is selected
     */
    xRate = 0.6;
    ClickPosition(xRate, yRate);
    EXPECT_TRUE((radio1 != nullptr) && (radio1->GetState() == UICheckBox::UICheckBoxState::SELECTED));
    EXPECT_TRUE((radio2 != nullptr) && (radio2->GetState() == UICheckBox::UICheckBoxState::UNSELECTED));
    EXPECT_TRUE((radio3 != nullptr) && (radio3->GetState() == UICheckBox::UICheckBoxState::SELECTED));

    /**
     * @tc.steps: step4.click the second radioButton
     * @tc.expected: step4.the second button of radio state is selected
     */
    xRate = 0.3;
    ClickPosition(xRate, yRate);
    EXPECT_TRUE((radio1 != nullptr) && (radio1->GetState() == UICheckBox::UICheckBoxState::UNSELECTED));
    EXPECT_TRUE((radio2 != nullptr) && (radio2->GetState() == UICheckBox::UICheckBoxState::SELECTED));
    EXPECT_TRUE((radio3 != nullptr) && (radio3->GetState() == UICheckBox::UICheckBoxState::SELECTED));
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: InputTest010
 * @tc.desc: test the different event in input
 */
HWTEST_F(InputEventTddTest, InputTest010, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1.eval the event test bundle
     * @tc.expected: step1.eval js success
     */
    JSValue page = CreatePage(BUNDLE10, strlen(BUNDLE10));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps: step2.click the button
     * @tc.expected: step2.the value attribute value is 12331
     */
    const double xRate = 0.3;
    double yRate = 0.4;
    ClickPosition(xRate, yRate);
    char *value = JSObject::GetString(page, "value");
    EXPECT_FALSE((value == nullptr) || (strcmp(value, "12331")));
    ACE_FREE(value);

    /**
     * @tc.steps: step3.longpress the button
     * @tc.expected: step3.the value attribute is China
     */
    LongPressPosition(xRate, yRate);
    value = JSObject::GetString(page, "value");
    EXPECT_FALSE((value == nullptr) || (strcmp(value, "China")));
    ACE_FREE(value);

    /**
     * @tc.steps: step4.click the checkbox
     * @tc.expected: step4.the value attribute is Hello China11
     */
    yRate = 0.6;
    ClickPosition(xRate, yRate);
    value = JSObject::GetString(page, "value");
    EXPECT_FALSE((value == nullptr) || (strcmp(value, "China11")));
    ACE_FREE(value);

    /**
     * @tc.steps: step5.click the radio
     * @tc.expected: step5.the value attribute is Hello China11!
     */
    yRate = 0.8;
    ClickPosition(xRate, yRate);
    value = JSObject::GetString(page, "value");
    EXPECT_FALSE((value == nullptr) || (strcmp(value, "China11!")));
    ACE_FREE(value);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: CheckboxChangeClick011
 * @tc.desc: test the click and change event in checkbox
 */
HWTEST_F(InputEventTddTest, CheckboxChangeClick011, TestSize.Level0)
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps:step1.eval js bundle
     * @tc.expected:step1.eval js success
     */
    JSValue page = CreatePage(BUNDLE11, strlen(BUNDLE11));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps:step2.click the first checkbox
     * @tc.expected:step2.the click value is clicked, the change value is none
     */
    const double xRate = 0.2;
    double yRate = 0.2;
    ClickPosition(xRate, yRate);
    char *click = JSObject::GetString(page, "click");
    EXPECT_FALSE((click == nullptr) || (strcmp(click, "clicked")));
    ACE_FREE(click);
    char *change = JSObject::GetString(page, "change");
    EXPECT_FALSE((change == nullptr) || (strcmp(change, "none")));
    ACE_FREE(change);

    /**
     * @tc.steps: step3.click the second checkbox
     * @tc.expected:step3.the click value is clicked, the change value is changed
     */
    yRate = 0.4;
    ClickPosition(xRate, yRate);
    click = JSObject::GetString(page, "click");
    EXPECT_FALSE((click == nullptr) || (strcmp(click, "clicked")));
    ACE_FREE(click);
    change = JSObject::GetString(page, "change");
    EXPECT_FALSE((change == nullptr) || (strcmp(change, "changed")));
    ACE_FREE(change);

    /**
     * @tc.steps: step4.click the third checkbox
     * @tc.expected: step5.the click value is click, the change value is changed
     */
    yRate = 0.6;
    ClickPosition(xRate, yRate);
    click = JSObject::GetString(page, "click");
    EXPECT_FALSE((click == nullptr) || (strcmp(click, "click")));
    ACE_FREE(click);
    change = JSObject::GetString(page, "change");
    EXPECT_FALSE((change == nullptr) || (strcmp(change, "changed")));
    ACE_FREE(change);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: RadioChangeClick012
 * @tc.desc: test click and change event in radio
 */
HWTEST_F(InputEventTddTest, RadioChangeClick012, TestSize.Level1)
{
    /**
     * @tc.steps:step1.eval js bundle
     * @tc.expected:step1.eval js success
     */
    JSValue page = CreatePage(BUNDLE12, strlen(BUNDLE12));
    EXPECT_FALSE(JSUndefined::Is(page));

    /**
     * @tc.steps:step2.click the first radio
     * @tc.expected:step2.the click value is clicked, the change value is none
     */
    const double xRate = 0.2;
    double yRate = 0.2;
    ClickPosition(xRate, yRate);
    char *click = JSObject::GetString(page, "click");
    EXPECT_FALSE((click == nullptr) || (strcmp(click, "clicked")));
    ACE_FREE(click);
    char *change = JSObject::GetString(page, "change");
    EXPECT_FALSE((change == nullptr) || (strcmp(change, "none")));
    ACE_FREE(change);

    /**
     * @tc.steps: step3.click the second radio
     * @tc.expected:step3.the click value is clicked, the change value is changed
     */
    yRate = 0.4;
    ClickPosition(xRate, yRate);
    click = JSObject::GetString(page, "click");
    EXPECT_FALSE((click == nullptr) || (strcmp(click, "clicked")));
    ACE_FREE(click);
    change = JSObject::GetString(page, "change");
    EXPECT_FALSE((change == nullptr) || (strcmp(change, "changed")));
    ACE_FREE(change);

    /**
     * @tc.steps: step4.click the third radio
     * @tc.expected: step5.the click value is click, the change value is changed
     */
    yRate = 0.6;
    ClickPosition(xRate, yRate);
    click = JSObject::GetString(page, "click");
    EXPECT_FALSE((click == nullptr) || (strcmp(click, "click")));
    ACE_FREE(click);
    change = JSObject::GetString(page, "change");
    EXPECT_FALSE((change == nullptr) || (strcmp(change, "changed")));
    ACE_FREE(change);
    DestroyPage(page);
    TDD_CASE_END();
}
}
}
