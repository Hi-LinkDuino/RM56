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
#include "event_bubble_tdd_test.h"
#include <unistd.h>
#include "flex_layout.h"
#include "ui_list.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {
const char BUNDLE_CLICK_BUBBBLE[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'], onBubbleEvents: { click: _vm.handleClick3 } }, [\n"
    "        _c('div', { staticClass: ['pane'], onBubbleEvents: { click: _vm.handleClick2 } }, [\n"
    "          _c('input', {\n"
    "            staticClass: ['button'],\n"
    "            attrs: { type: 'button', value: 'CLICK ME' },\n"
    "            onBubbleEvents: { click: _vm.handleClick1 } \n"
    "          })\n"
    "        ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          backgroundColor: '#f00',\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        pane: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          width: '70%',\n"
    "          height: '70%'\n"
    "        },\n"
    "        button: {\n"
    "          width: 240\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      logs: []\n"
    "    },\n"
    "    handleClick1: function () {\n"
    "      this.logs.push('handleClick1');\n"
    "    },\n"
    "    handleClick2: function () {\n"
    "      this.logs.push('handleClick2');\n"
    "    },\n"
    "    handleClick3: function () {\n"
    "      this.logs.push('handleClick3');\n"
    "    }\n"
    "  });\n"
    "})();";

const char BUNDLE_LONG_PRESS_BUBBLE[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'], onBubbleEvents: { longpress: _vm.handleLongPress3 } }, "
    "[\n"
    "        _c('div', { staticClass: ['pane'], onBubbleEvents: { longpress: _vm.handleLongPress2 } }, [\n"
    "          _c('input', {\n"
    "            staticClass: ['button'],\n"
    "            attrs: { type: 'button', value: 'LONG PRESS ME' },\n"
    "            onBubbleEvents: { longpress: _vm.handleLongPress1 } \n"
    "          })\n"
    "        ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        pane: {\n"
    "          backgroundColor: '#f00',\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          width: '70%',\n"
    "          height: '70%'\n"
    "        },\n"
    "        button: {\n"
    "          width: 300\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      logs: []\n"
    "    },\n"
    "    handleLongPress1: function () {\n"
    "      this.logs.push('handleLongPress1');\n"
    "    },\n"
    "    handleLongPress2: function () {\n"
    "      this.logs.push('handleLongPress2');\n"
    "    },\n"
    "    handleLongPress3: function () {\n"
    "      this.logs.push('handleLongPress3');\n"
    "    }\n"
    "  });\n"
    "})();";

const char BUNDLE_SWIPE_BUBBLE[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'], onBubbleEvents: { swipe: _vm.handleSwipe3 } }, [\n"
    "        _c('div', { staticClass: ['pane'], onBubbleEvents: { swipe: _vm.handleSwipe2 } }, [\n"
    "          _c('div', {\n"
    "            staticClass: ['box'],\n"
    "            onBubbleEvents: { swipe: _vm.handleSwipe1 } \n"
    "          })\n"
    "        ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        pane: {\n"
    "          backgroundColor: '#f00',\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          width: '70%',\n"
    "          height: '70%'\n"
    "        },\n"
    "        box: {\n"
    "          width: 200,\n"
    "          height: 200,\n"
    "          backgroundColor: '#0f0'\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      logs: []\n"
    "    },\n"
    "    handleSwipe1: function () {\n"
    "      this.logs.push('handleSwipe1');\n"
    "    },\n"
    "    handleSwipe2: function () {\n"
    "      this.logs.push('handleSwipe2');\n"
    "    },\n"
    "    handleSwipe3: function () {\n"
    "      this.logs.push('handleSwipe3');\n"
    "    }\n"
    "  });\n"
    "})();";

const char BUNDLE_CLICK_DEFAULT_BUBBLE[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'], onBubbleEvents: { click: _vm.handleClick3 } }, [\n"
    "        _c('div', { staticClass: ['pane'] }, [\n"
    "          _c('input', {\n"
    "            staticClass: ['button'],\n"
    "            attrs: { type: 'button', value: 'CLICK ME' },\n"
    "            onBubbleEvents: { click: _vm.handleClick1 } \n"
    "          })\n"
    "        ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          backgroundColor: '#f00',\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        pane: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          width: '70%',\n"
    "          height: '70%'\n"
    "        },\n"
    "        button: {\n"
    "          width: 240\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      logs: []\n"
    "    },\n"
    "    handleClick1: function () {\n"
    "      this.logs.push('handleClick1');\n"
    "    },\n"
    "    handleClick2: function () {\n"
    "      this.logs.push('handleClick2');\n"
    "    },\n"
    "    handleClick3: function () {\n"
    "      this.logs.push('handleClick3');\n"
    "    }\n"
    "  });\n"
    "})();";

const char BUNDLE_LONG_PRESS_DEFAULT_BUBBLE[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'], onBubbleEvents: { longpress: _vm.handleLongPress3 } }, "
    "[\n"
    "        _c('div', { staticClass: ['pane'] }, [\n"
    "          _c('input', {\n"
    "            staticClass: ['button'],\n"
    "            attrs: { type: 'button', value: 'LONG PRESS ME' },\n"
    "            onBubbleEvents: { longpress: _vm.handleLongPress1 } \n"
    "          })\n"
    "        ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        pane: {\n"
    "          backgroundColor: '#f00',\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          width: '70%',\n"
    "          height: '70%'\n"
    "        },\n"
    "        button: {\n"
    "          width: 300\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      logs: []\n"
    "    },\n"
    "    handleLongPress1: function () {\n"
    "      this.logs.push('handleLongPress1');\n"
    "    },\n"
    "    handleLongPress2: function () {\n"
    "      this.logs.push('handleLongPress2');\n"
    "    },\n"
    "    handleLongPress3: function () {\n"
    "      this.logs.push('handleLongPress3');\n"
    "    }\n"
    "  });\n"
    "})();";

const char BUNDLE_SWIPE_DEFAULT_BUBBLE[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'], onBubbleEvents: { swipe: _vm.handleSwipe3 } }, [\n"
    "        _c('div', { staticClass: ['pane'] }, [\n"
    "          _c('div', {\n"
    "            staticClass: ['box'],\n"
    "            onBubbleEvents: { swipe: _vm.handleSwipe1 } \n"
    "          })\n"
    "        ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        pane: {\n"
    "          backgroundColor: '#f00',\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          width: '70%',\n"
    "          height: '70%'\n"
    "        },\n"
    "        box: {\n"
    "          width: 200,\n"
    "          height: 200,\n"
    "          backgroundColor: '#0f0'\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      logs: []\n"
    "    },\n"
    "    handleSwipe1: function () {\n"
    "      this.logs.push('handleSwipe1');\n"
    "    },\n"
    "    handleSwipe2: function () {\n"
    "      this.logs.push('handleSwipe2');\n"
    "    },\n"
    "    handleSwipe3: function () {\n"
    "      this.logs.push('handleSwipe3');\n"
    "    }\n"
    "  });\n"
    "})();";

const char BUNDLE_CLICK_STOP_PROPAGATION[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'], onBubbleEvents: { click: _vm.handleClick3 } }, [\n"
    "        _c('div', { staticClass: ['pane'], catchBubbleEvents: { click: _vm.handleClick2 } }, [\n"
    "          _c('input', {\n"
    "            staticClass: ['button'],\n"
    "            attrs: { type: 'button', value: 'CLICK ME' },\n"
    "            onBubbleEvents: { click: _vm.handleClick1 }\n"
    "          })\n"
    "        ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        pane: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          width: '70%',\n"
    "          height: '70%'\n"
    "        },\n"
    "        button: {\n"
    "          width: 240\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      logs: []\n"
    "    },\n"
    "    handleClick1: function () {\n"
    "      this.logs.push('handleClick1');\n"
    "    },\n"
    "    handleClick2: function () {\n"
    "      this.logs.push('handleClick2');\n"
    "    },\n"
    "    handleClick3: function () {\n"
    "      this.logs.push('handleClick3');\n"
    "    }\n"
    "  });\n"
    "})();";

const char BUNDLE_LONG_PRESS_STOP_PROPAGATION[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'], onBubbleEvents: { longpress: _vm.handleLongPress3 } }, "
    "[\n"
    "        _c('div', { staticClass: ['pane'], catchBubbleEvents: { longpress: _vm.handleLongPress2 } }, [\n"
    "          _c('input', {\n"
    "            staticClass: ['button'],\n"
    "            attrs: { type: 'button', value: 'LONG PRESS ME' },\n"
    "            onBubbleEvents: { longpress: _vm.handleLongPress1 }\n"
    "          })\n"
    "        ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        pane: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          width: '70%',\n"
    "          height: '70%'\n"
    "        },\n"
    "        button: {\n"
    "          width: 240\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      logs: []\n"
    "    },\n"
    "    handleLongPress1: function () {\n"
    "      this.logs.push('handleLongPress1');\n"
    "    },\n"
    "    handleLongPress2: function () {\n"
    "      this.logs.push('handleLongPress2');\n"
    "    },\n"
    "    handleLongPress3: function () {\n"
    "      this.logs.push('handleLongPress3');\n"
    "    }\n"
    "  });\n"
    "})();";

const char BUNDLE_SWIPE_STOP_PROPAGATION[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'], onBubbleEvents: { swipe: _vm.handleSwipe3 } }, [\n"
    "        _c('div', { staticClass: ['pane'], catchBubbleEvents: { swipe: _vm.handleSwipe2 } }, [\n"
    "          _c('div', {\n"
    "            staticClass: ['box'],\n"
    "            onBubbleEvents: { swipe: _vm.handleSwipe1 } \n"
    "          })\n"
    "        ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        pane: {\n"
    "          backgroundColor: '#f00',\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          width: '70%',\n"
    "          height: '70%'\n"
    "        },\n"
    "        box: {\n"
    "          width: 200,\n"
    "          height: 200,\n"
    "          backgroundColor: '#0f0'\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      logs: []\n"
    "    },\n"
    "    handleSwipe1: function () {\n"
    "      this.logs.push('handleSwipe1');\n"
    "    },\n"
    "    handleSwipe2: function () {\n"
    "      this.logs.push('handleSwipe2');\n"
    "    },\n"
    "    handleSwipe3: function () {\n"
    "      this.logs.push('handleSwipe3');\n"
    "    }\n"
    "  });\n"
    "})();";

const char BUNDLE_CLICK_DELEGATION[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'], onBubbleEvents: { click: _vm.handleClick } }, [\n"
    "        _c('input', { staticClass: ['button'], attrs: { type: 'button', ref: 'red', value: 'red' } }),\n"
    "        _c('input', { staticClass: ['button'], attrs: { type: 'button', ref: 'green', value: 'green' } }),\n"
    "        _c('input', { staticClass: ['button'], attrs: { type: 'button', ref: 'blue', value: 'blue' } })\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%',\n"
    "        },\n"
    "        button: {\n"
    "          borderRadius: 0,\n"
    "          width: 150\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      bgColor: '#f00'\n"
    "    },\n"
    "    handleClick: function(event) {\n"
    "      if (event.target === this.$refs.red) {\n"
    "        this.bgColor = '#f00';\n"
    "      } else if (event.target === this.$refs.green) {\n"
    "        this.bgColor = '#0f0';\n"
    "      } else if (event.target === this.$refs.blue) {\n"
    "        this.bgColor = '#00f';\n"
    "      } else {\n"
    "        console.log('Unknown element.');\n"
    "      }\n"
    "    }\n"
    "  });\n"
    "})();";

const char BUNDLE_LONG_PRESS_DELEGATION[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'], onBubbleEvents: { longpress: _vm.handleLongPress } }, [\n"
    "        _c('input', { staticClass: ['button'], attrs: { type: 'button', ref: 'red', value: 'red' } }),\n"
    "        _c('input', { staticClass: ['button'], attrs: { type: 'button', ref: 'green', value: 'green' } }),\n"
    "        _c('input', { staticClass: ['button'], attrs: { type: 'button', ref: 'blue', value: 'blue' } })\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%',\n"
    "        },\n"
    "        button: {\n"
    "          borderRadius: 0,\n"
    "          width: 150\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      bgColor: '#f00'\n"
    "    },\n"
    "    handleLongPress: function(event) {\n"
    "      if (event.target === this.$refs.red) {\n"
    "        this.bgColor = '#f00';\n"
    "      } else if (event.target === this.$refs.green) {\n"
    "        this.bgColor = '#0f0';\n"
    "      } else if (event.target === this.$refs.blue) {\n"
    "        this.bgColor = '#00f';\n"
    "      } else {\n"
    "        console.log('Unknown element.');\n"
    "      }\n"
    "    }\n"
    "  });\n"
    "})();";

const char BUNDLE_SWIPE_DELEGATION[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'], onBubbleEvents: { swipe: _vm.handleSwipe } }, [\n"
    "        _c('div', { staticClass: ['box', 'box--red'], attrs: { ref: 'red' } }, [\n"
    "          _c('text', { staticClass: ['title'], attrs: { value: 'red'} })\n"
    "        ]),\n"
    "        _c('div', { staticClass: ['box', 'box--green'], attrs: { ref: 'green' } }, [\n"
    "          _c('text', { staticClass: ['title'], attrs: { value: 'green'} })\n"
    "        ]),\n"
    "        _c('div', { staticClass: ['box', 'box--blue'], attrs: { ref: 'blue' } }, [\n"
    "          _c('text', { staticClass: ['title'], attrs: { value: 'blue'} })\n"
    "        ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        box: {\n"
    "          width: '50%',\n"
    "          height: 100,\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center'\n"
    "        },\n"
    "        'box--red': {\n"
    "          backgroundColor: '#f00'\n"
    "        },\n"
    "        'box--green': {\n"
    "          backgroundColor: '#0f0'\n"
    "        },\n"
    "        'box--blue': {\n"
    "          backgroundColor: '#00f'\n"
    "        },\n"
    "        title: {\n"
    "          color: '#fff'\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      text: 'none'\n"
    "    },\n"
    "    handleSwipe: function(event) {\n"
    "      if (event.target === this.$refs.red) {\n"
    "        this.text = 'red';\n"
    "      } else if (event.target === this.$refs.green) {\n"
    "        this.text = 'green';\n"
    "      } else if (event.target === this.$refs.blue) {\n"
    "        this.text = 'blue';\n"
    "      } else {\n"
    "        console.log('Unknown element.');\n"
    "      }\n"
    "    }\n"
    "  });\n"
    "})();";

const char BUNDLE_CLICK_PRESSURE[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "       _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "        _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "         _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "          _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "           _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "            _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "             _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "              _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "               _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                 _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                  _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                   _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                    _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                     _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                      _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                       _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                        _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                         _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                          _c('text', { attrs: { value: function() {  return _vm.count; } } })\n"
    "                         ])\n"
    "                        ])\n"
    "                       ])\n"
    "                      ])\n"
    "                     ])\n"
    "                    ])\n"
    "                   ])\n"
    "                  ])\n"
    "                 ])\n"
    "                ])\n"
    "               ])\n"
    "              ])\n"
    "             ])\n"
    "            ])\n"
    "           ])\n"
    "          ])\n"
    "         ])\n"
    "        ])\n"
    "       ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        box: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      count: 0\n"
    "    },\n"
    "    handle: function() {\n"
    "      this.count++;\n"
    "    }\n"
    "  });\n"
    "})();";

const char BUNDLE_LONG_PRESS_PRESSURE[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "       _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "        _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "         _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "          _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "           _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "            _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "             _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "              _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "               _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "                _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "                 _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "                  _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "                   _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "                    _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "                     _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "                      _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "                       _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "                        _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "                         _c('div', { staticClass: ['box'], onBubbleEvents: { longpress: _vm.handle } }, [\n"
    "                          _c('text', { attrs: { value: function() {  return _vm.count; } } })\n"
    "                         ])\n"
    "                        ])\n"
    "                       ])\n"
    "                      ])\n"
    "                     ])\n"
    "                    ])\n"
    "                   ])\n"
    "                  ])\n"
    "                 ])\n"
    "                ])\n"
    "               ])\n"
    "              ])\n"
    "             ])\n"
    "            ])\n"
    "           ])\n"
    "          ])\n"
    "         ])\n"
    "        ])\n"
    "       ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        box: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      count: 0\n"
    "    },\n"
    "    handle: function() {\n"
    "      this.count++;\n"
    "    }\n"
    "  });\n"
    "})();";

const char BUNDLE_SWIPE_PRESSURE[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "       _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "        _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "         _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "          _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "           _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "            _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "             _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "              _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "               _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "                _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "                 _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "                  _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "                   _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "                    _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "                     _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "                      _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "                       _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "                        _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "                         _c('div', { staticClass: ['box'], onBubbleEvents: { swipe: _vm.handle } }, [\n"
    "                          _c('text', { attrs: { value: function() {  return _vm.count; } } })\n"
    "                         ])\n"
    "                        ])\n"
    "                       ])\n"
    "                      ])\n"
    "                     ])\n"
    "                    ])\n"
    "                   ])\n"
    "                  ])\n"
    "                 ])\n"
    "                ])\n"
    "               ])\n"
    "              ])\n"
    "             ])\n"
    "            ])\n"
    "           ])\n"
    "          ])\n"
    "         ])\n"
    "        ])\n"
    "       ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        box: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      count: 0\n"
    "    },\n"
    "    handle: function() {\n"
    "      this.count++;\n"
    "    }\n"
    "  });\n"
    "})();";
const char BUNDLE_CLICK_PRESSURE2[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "       _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "        _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "         _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "          _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "           _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "            _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "             _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "              _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "               _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                 _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                  _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                   _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                    _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                     _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                      _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                       _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                        _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                         _c('div', { staticClass: ['box'], onBubbleEvents: { click: _vm.handle } }, [\n"
    "                          _c('text', { attrs: { value: function() {  return _vm.count; } } })\n"
    "                         ])\n"
    "                        ])\n"
    "                       ])\n"
    "                      ])\n"
    "                     ])\n"
    "                    ])\n"
    "                   ])\n"
    "                  ])\n"
    "                 ])\n"
    "                ])\n"
    "               ])\n"
    "              ])\n"
    "             ])\n"
    "            ])\n"
    "           ])\n"
    "          ])\n"
    "         ])\n"
    "        ])\n"
    "       ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        box: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      count: 0\n"
    "    },\n"
    "    handle: function() {\n"
    "      this.count++;\n"
    "    }\n"
    "  });\n"
    "})();";

const char BUBBLE_PREVENT_DOUBLE_SWIPE[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c(\n"
    "        'div',\n"
    "        {\n"
    "          staticClass: ['container'],\n"
    "          onBubbleEvents: { swipe: _vm.handleDivSwipe },\n"
    "          attrs: { ref: 'box' }\n"
    "        },\n"
    "        [\n"
    "          _c('text', { attrs: { value: 'Header' } }),\n"
    "          _c(\n"
    "            'list',\n"
    "            {\n"
    "              staticClass: ['list'],\n"
    "              catchBubbleEvents: { swipe: _vm.handleListSwipe },\n"
    "              attrs: { ref: 'list' }\n"
    "            },\n"
    "            [\n"
    "              _l(\n"
    "                function () {\n"
    "                  return _vm.dataSource;\n"
    "                },\n"
    "                function (item, $idx) {\n"
    "                  return _c('list-item', { staticClass: ['list-item'] }, [\n"
    "                    _c('text', {\n"
    "                      staticClass: ['title'],\n"
    "                      attrs: {\n"
    "                        value: function () {\n"
    "                          return item.title;\n"
    "                        }\n"
    "                      }\n"
    "                    })\n"
    "                  ]);\n"
    "                }\n"
    "              )\n"
    "            ]\n"
    "          ),\n"
    "          _c('text', { attrs: { value: 'Footer' } }),\n"
    "        ]\n"
    "      );\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '200%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        list: {\n"
    "          backgroundColor: 'blanchedalmond',\n"
    "          width: '80%',\n"
    "          height: 400\n"
    "        },\n"
    "        'list-item': {\n"
    "          width: '100%',\n"
    "          height: 200,\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center'\n"
    "        },\n"
    "      },\n"
    "    },\n"
    "    data: {\n"
    "      dataSource: [\n"
    "        { title: 'Item 1' },\n"
    "        { title: 'Item 2' },\n"
    "        { title: 'Item 3' },\n"
    "        { title: 'Item 4' },\n"
    "        { title: 'Item 5' }\n"
    "      ],\n"
    "      divSwipe: false,\n"
    "      listSwipe: false\n"
    "    },\n"
    "    handleDivSwipe: function () {\n"
    "      this.divSwipe = true;\n"
    "    },\n"
    "    handleListSwipe: function () {\n"
    "      this.listSwipe = true;\n"
    "    }\n"
    "  });\n"
    "})();\n";

constexpr uint8_t NUM_TWO = 2;
constexpr uint8_t NUM_THREE = 3;

void EventBubbleTddTest::EventBubbleTest001()
{
    TDD_CASE_BEGIN();

    JSValue page = CreatePage(BUNDLE_CLICK_BUBBBLE, strlen(BUNDLE_CLICK_BUBBBLE));
    const char *propLogs = "logs";
    JSValue logs = JSObject::Get(page, propLogs);
    EXPECT_EQ(JSArray::Length(logs), 0);
    const uint16_t centerX = GetHorizontalResolution() / NUM_TWO;
    const uint16_t centerY = GetVerticalResolution() / NUM_TWO;
    Click(centerX, centerY);
    EXPECT_EQ(JSArray::Length(logs), NUM_THREE);
    char *content = JSArray::GetString(logs, 0);
    EXPECT_STREQ(content, "handleClick1");
    free(content);
    content = JSArray::GetString(logs, 1);
    EXPECT_STREQ(content, "handleClick2");
    free(content);
    content = JSArray::GetString(logs, NUM_TWO);
    EXPECT_STREQ(content, "handleClick3");
    free(content);
    content = nullptr;
    JSRelease(logs);
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::EventBubbleTest002()
{
    TDD_CASE_BEGIN();

    JSValue page = CreatePage(BUNDLE_LONG_PRESS_BUBBLE, strlen(BUNDLE_LONG_PRESS_BUBBLE));
    const char *propLogs = "logs";
    JSValue logs = JSObject::Get(page, propLogs);
    EXPECT_EQ(JSArray::Length(logs), 0);
    const uint16_t centerX = GetHorizontalResolution() / NUM_TWO;
    const uint16_t centerY = GetVerticalResolution() / NUM_TWO;
    LongPress(centerX, centerY);
    EXPECT_EQ(JSArray::Length(logs), NUM_THREE);
    char *content = JSArray::GetString(logs, 0);
    EXPECT_STREQ(content, "handleLongPress1");
    free(content);
    content = JSArray::GetString(logs, 1);
    EXPECT_STREQ(content, "handleLongPress2");
    free(content);
    content = JSArray::GetString(logs, NUM_TWO);
    EXPECT_STREQ(content, "handleLongPress3");
    free(content);
    content = nullptr;
    JSRelease(logs);
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::EventBubbleTest003()
{
    TDD_CASE_BEGIN();

    JSValue page = CreatePage(BUNDLE_SWIPE_BUBBLE, strlen(BUNDLE_SWIPE_BUBBLE));
    const char *propLogs = "logs";
    JSValue logs = JSObject::Get(page, propLogs);
    EXPECT_EQ(JSArray::Length(logs), 0);
    const uint16_t centerX = GetHorizontalResolution() / NUM_TWO;
    const uint16_t centerY = GetVerticalResolution() / NUM_TWO;
    constexpr uint16_t halfDistance = 50;
    const uint16_t startX = centerX - halfDistance;
    const uint16_t endX = centerX + halfDistance;
    Swipe(startX, centerY, endX, centerY);
    EXPECT_EQ(JSArray::Length(logs), NUM_THREE);
    char *content = JSArray::GetString(logs, 0);
    EXPECT_STREQ(content, "handleSwipe1");
    free(content);
    content = JSArray::GetString(logs, 1);
    EXPECT_STREQ(content, "handleSwipe2");
    free(content);
    content = JSArray::GetString(logs, NUM_TWO);
    EXPECT_STREQ(content, "handleSwipe3");
    free(content);
    content = nullptr;
    JSRelease(logs);
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::EventBubbleTest004()
{
    TDD_CASE_BEGIN();

    JSValue page = CreatePage(BUNDLE_CLICK_DEFAULT_BUBBLE, strlen(BUNDLE_CLICK_DEFAULT_BUBBLE));
    const char *propLogs = "logs";
    JSValue logs = JSObject::Get(page, propLogs);
    EXPECT_EQ(JSArray::Length(logs), 0);
    const uint16_t centerX = GetHorizontalResolution() / NUM_TWO;
    const uint16_t centerY = GetVerticalResolution() / NUM_TWO;
    Click(centerX, centerY);
    EXPECT_EQ(JSArray::Length(logs), NUM_TWO);
    char *content = JSArray::GetString(logs, 0);
    EXPECT_STREQ(content, "handleClick1");
    free(content);
    content = JSArray::GetString(logs, 1);
    EXPECT_STREQ(content, "handleClick3");
    free(content);
    content = nullptr;
    JSRelease(logs);
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::EventBubbleTest005()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_LONG_PRESS_DEFAULT_BUBBLE, strlen(BUNDLE_LONG_PRESS_DEFAULT_BUBBLE));
    const char *propLogs = "logs";
    JSValue logs = JSObject::Get(page, propLogs);
    EXPECT_EQ(JSArray::Length(logs), 0);
    const uint16_t centerX = GetHorizontalResolution() / NUM_TWO;
    const uint16_t centerY = GetVerticalResolution() / NUM_TWO;
    LongPress(centerX, centerY);
    EXPECT_EQ(JSArray::Length(logs), NUM_TWO);
    char *content = JSArray::GetString(logs, 0);
    EXPECT_STREQ(content, "handleLongPress1");
    free(content);
    content = JSArray::GetString(logs, 1);
    EXPECT_STREQ(content, "handleLongPress3");
    free(content);
    content = nullptr;
    JSRelease(logs);
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::EventBubbleTest006()
{
    TDD_CASE_BEGIN();

    JSValue page = CreatePage(BUNDLE_SWIPE_DEFAULT_BUBBLE, strlen(BUNDLE_SWIPE_DEFAULT_BUBBLE));
    const char *propLogs = "logs";
    JSValue logs = JSObject::Get(page, propLogs);
    EXPECT_EQ(JSArray::Length(logs), 0);
    const uint16_t centerX = GetHorizontalResolution() / NUM_TWO;
    const uint16_t centerY = GetVerticalResolution() / NUM_TWO;
    constexpr uint16_t halfDistance = 50;
    const uint16_t startX = centerX - halfDistance;
    const uint16_t endX = centerX + halfDistance;
    Swipe(startX, centerY, endX, centerY);
    EXPECT_EQ(JSArray::Length(logs), NUM_TWO);
    char *content = JSArray::GetString(logs, 0);
    EXPECT_STREQ(content, "handleSwipe1");
    free(content);
    content = JSArray::GetString(logs, 1);
    EXPECT_STREQ(content, "handleSwipe3");
    free(content);
    content = nullptr;
    JSRelease(logs);
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::EventBubbleTest007()
{
    TDD_CASE_BEGIN();

    JSValue page = CreatePage(BUNDLE_CLICK_STOP_PROPAGATION, strlen(BUNDLE_CLICK_STOP_PROPAGATION));
    const char *propLogs = "logs";
    JSValue logs = JSObject::Get(page, propLogs);
    EXPECT_EQ(JSArray::Length(logs), 0);
    const uint16_t centerX = GetHorizontalResolution() / NUM_TWO;
    const uint16_t centerY = GetVerticalResolution() / NUM_TWO;
    Click(centerX, centerY);
    EXPECT_EQ(JSArray::Length(logs), NUM_TWO);
    char *content = JSArray::GetString(logs, 0);
    EXPECT_STREQ(content, "handleClick1");
    free(content);
    content = JSArray::GetString(logs, 1);
    EXPECT_STREQ(content, "handleClick2");
    free(content);
    content = nullptr;
    JSRelease(logs);
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::EventBubbleTest008()
{
    TDD_CASE_BEGIN();

    JSValue page = CreatePage(BUNDLE_LONG_PRESS_STOP_PROPAGATION, strlen(BUNDLE_LONG_PRESS_STOP_PROPAGATION));
    const char *propLogs = "logs";
    JSValue logs = JSObject::Get(page, propLogs);
    EXPECT_EQ(JSArray::Length(logs), 0);
    const uint16_t centerX = GetHorizontalResolution() / NUM_TWO;
    const uint16_t centerY = GetVerticalResolution() / NUM_TWO;
    LongPress(centerX, centerY);
    EXPECT_EQ(JSArray::Length(logs), NUM_TWO);
    char *content = JSArray::GetString(logs, 0);
    EXPECT_STREQ(content, "handleLongPress1");
    free(content);
    content = JSArray::GetString(logs, 1);
    EXPECT_STREQ(content, "handleLongPress2");
    free(content);
    content = nullptr;
    JSRelease(logs);
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::EventBubbleTest009()
{
    TDD_CASE_BEGIN();

    JSValue page = CreatePage(BUNDLE_SWIPE_STOP_PROPAGATION, strlen(BUNDLE_SWIPE_STOP_PROPAGATION));
    const char *propLogs = "logs";
    JSValue logs = JSObject::Get(page, propLogs);
    EXPECT_EQ(JSArray::Length(logs), 0);
    const uint16_t centerX = GetHorizontalResolution() / NUM_TWO;
    const uint16_t centerY = GetVerticalResolution() / NUM_TWO;
    constexpr uint16_t halfDistance = 50;
    const uint16_t startX = centerX - halfDistance;
    const uint16_t endX = centerX + halfDistance;
    Swipe(startX, centerY, endX, centerY);
    EXPECT_EQ(JSArray::Length(logs), NUM_TWO);
    char *content = JSArray::GetString(logs, 0);
    EXPECT_STREQ(content, "handleSwipe1");
    free(content);
    content = JSArray::GetString(logs, 1);
    EXPECT_STREQ(content, "handleSwipe2");
    free(content);
    content = nullptr;
    JSRelease(logs);
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::EventBubbleTest010()
{
    TDD_CASE_BEGIN();

    JSValue page = CreatePage(BUNDLE_CLICK_DELEGATION, strlen(BUNDLE_CLICK_DELEGATION));
    const char *propBgColor = "bgColor";
    const char *bgRed = "#f00";
    const char *bgGreen = "#0f0";
    const char *bgBlue = "#00f";
    const uint16_t centerX = GetHorizontalResolution() / NUM_TWO;
    const uint16_t centerY = GetVerticalResolution() / NUM_TWO;
    constexpr uint16_t distance = 100;
    // click green button
    Click(centerX, centerY);
    char *content = JSObject::GetString(page, propBgColor);
    EXPECT_STREQ(content, bgGreen);
    free(content);
    // click blue button
    Click(centerX + distance, centerY);
    content = JSObject::GetString(page, propBgColor);
    EXPECT_STREQ(content, bgBlue);
    free(content);
    // click red button
    Click(centerX - distance, centerY);
    content = JSObject::GetString(page, propBgColor);
    EXPECT_STREQ(content, bgRed);
    free(content);
    content = nullptr;
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::EventBubbleTest011()
{
    TDD_CASE_BEGIN();

    JSValue page = CreatePage(BUNDLE_LONG_PRESS_DELEGATION, strlen(BUNDLE_LONG_PRESS_DELEGATION));
    // click green button
    const uint16_t centerX = GetHorizontalResolution() / NUM_TWO;
    const uint16_t centerY = GetVerticalResolution() / NUM_TWO;
    constexpr uint16_t distance = 100;
    LongPress(centerX, centerY);
    const char *propBgColor = "bgColor";
    char *content = JSObject::GetString(page, propBgColor);
    EXPECT_STREQ(content, "#0f0");
    free(content);
    // click blue button
    LongPress(centerX + distance, centerY);
    content = JSObject::GetString(page, propBgColor);
    EXPECT_STREQ(content, "#00f");
    free(content);
    // click red button
    LongPress(centerX - distance, centerY);
    content = JSObject::GetString(page, propBgColor);
    EXPECT_STREQ(content, "#f00");
    free(content);
    content = nullptr;
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::EventBubbleTest012()
{
    TDD_CASE_BEGIN();

    JSValue page = CreatePage(BUNDLE_SWIPE_DELEGATION, strlen(BUNDLE_SWIPE_DELEGATION));
    const char *propText = "text";
    const char *textRed = "red";
    const char *textGreen = "green";
    const char *textBlue = "blue";
    const uint16_t greenX = GetHorizontalResolution() / NUM_TWO;
    const uint16_t greenY = GetVerticalResolution() / NUM_TWO;
    constexpr uint16_t boxHeight = 100;
    constexpr uint16_t halfDistance = 50;
    const uint16_t startX = greenX - halfDistance;
    const uint16_t endX = greenX + halfDistance;
    const uint16_t redY = greenY - boxHeight;
    const uint16_t blueY = greenY + boxHeight;
    Swipe(startX, redY, endX, redY);
    char *content = JSObject::GetString(page, propText);
    EXPECT_STREQ(content, textRed);
    free(content);
    Swipe(startX, greenY, endX, greenY);
    content = JSObject::GetString(page, propText);
    EXPECT_STREQ(content, textGreen);
    free(content);
    Swipe(startX, blueY, endX, blueY);
    content = JSObject::GetString(page, propText);
    EXPECT_STREQ(content, textBlue);
    free(content);
    content = nullptr;
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::EventBubbleTest013()
{
    TDD_CASE_BEGIN();

    JSValue page = CreatePage(BUNDLE_CLICK_PRESSURE, strlen(BUNDLE_CLICK_PRESSURE));
    const char *propCount = "count";
    const uint16_t centerX = GetHorizontalResolution() / NUM_TWO;
    const uint16_t centerY = GetVerticalResolution() / NUM_TWO;
    constexpr uint8_t diffCount = 20;
    Click(centerX, centerY);
    uint32_t count = static_cast<uint32_t>(JSObject::GetNumber(page, propCount));
    EXPECT_EQ(count, diffCount);
    Click(centerX, centerY);
    count = static_cast<uint32_t>(JSObject::GetNumber(page, propCount));
    EXPECT_EQ(count, diffCount * NUM_TWO);
    Click(centerX, centerY);
    count = static_cast<uint32_t>(JSObject::GetNumber(page, propCount));
    EXPECT_EQ(count, diffCount * NUM_THREE);
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::EventBubbleTest014()
{
    TDD_CASE_BEGIN();

    JSValue page = CreatePage(BUNDLE_LONG_PRESS_PRESSURE, strlen(BUNDLE_LONG_PRESS_PRESSURE));
    const char *propCount = "count";
    const uint16_t centerX = GetHorizontalResolution() / NUM_TWO;
    const uint16_t centerY = GetVerticalResolution() / NUM_TWO;
    constexpr uint8_t diffCount = 20;
    LongPress(centerX, centerY);
    uint32_t count = static_cast<uint32_t>(JSObject::GetNumber(page, propCount));
    EXPECT_EQ(count, diffCount);
    LongPress(centerX, centerY);
    count = static_cast<uint32_t>(JSObject::GetNumber(page, propCount));
    EXPECT_EQ(count, diffCount * NUM_TWO);
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::EventBubbleTest015()
{
    TDD_CASE_BEGIN();

    JSValue page = CreatePage(BUNDLE_SWIPE_PRESSURE, strlen(BUNDLE_SWIPE_PRESSURE));
    const char *propCount = "count";
    const uint16_t centerX = GetHorizontalResolution() / NUM_TWO;
    const uint16_t centerY = GetVerticalResolution() / NUM_TWO;
    constexpr uint16_t halfDistance = 50;
    const uint16_t startX = centerX - halfDistance;
    const uint16_t endX = centerX + halfDistance;
    constexpr uint8_t diffCount = 20;
    Swipe(startX, centerY, endX, centerY);
    uint32_t count = static_cast<uint32_t>(JSObject::GetNumber(page, propCount));
    EXPECT_EQ(count, diffCount);
    Swipe(startX, centerY, endX, centerY);
    count = static_cast<uint32_t>(JSObject::GetNumber(page, propCount));
    EXPECT_EQ(count, diffCount * NUM_TWO);
    Swipe(startX, centerY, endX, centerY);
    count = static_cast<uint32_t>(JSObject::GetNumber(page, propCount));
    EXPECT_EQ(count, diffCount * NUM_THREE);
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::EventBubbleTest016()
{
    TDD_CASE_BEGIN();

    JSValue page = CreatePage(BUNDLE_CLICK_PRESSURE2, strlen(BUNDLE_CLICK_PRESSURE2));
    const char *propCount = "count";
    const uint16_t centerX = GetHorizontalResolution() / NUM_TWO;
    const uint16_t centerY = GetVerticalResolution() / NUM_TWO;
    constexpr uint8_t times = 10;
    constexpr uint8_t diffCount = 20;
    for (uint8_t idx = 0; idx < times; ++idx) {
        Click(centerX, centerY);
    }
    uint32_t count = static_cast<uint32_t>(JSObject::GetNumber(page, propCount));
    EXPECT_EQ(count, diffCount * times);
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::EventBubbleTest017()
{
    TDD_CASE_BEGIN();

    JSValue page = CreatePage(BUBBLE_PREVENT_DOUBLE_SWIPE, strlen(BUBBLE_PREVENT_DOUBLE_SWIPE));
    FlexLayout *container = reinterpret_cast<FlexLayout *>(GetViewByRef(page, "box"));
    EXPECT_TRUE(container != nullptr);
    if (container != nullptr) {
        int16_t prevContainerY = container->GetOrigRect().GetY();
        UIList *list = reinterpret_cast<UIList *>(GetViewByRef(page, "list"));
        EXPECT_TRUE(list != nullptr);
        if (list != nullptr) {
            Rect rect = list->GetOrigRect();
            int16_t diffY = 100;
            int16_t x = rect.GetLeft() + rect.GetWidth() / NUM_TWO;
            int16_t startY = rect.GetTop() + rect.GetHeight() - diffY;
            int16_t endY = rect.GetTop() + diffY;
            Swipe(x, startY, x, endY);
            EXPECT_FALSE(JSObject::GetBoolean(page, "divSwipe"));
            EXPECT_TRUE(JSObject::GetBoolean(page, "listSwipe"));
            EXPECT_EQ(container->GetOrigRect().GetY(), prevContainerY);
        }
    }
    DestroyPage(page);
    TDD_CASE_END();
}

void EventBubbleTddTest::RunTests()
{
    EventBubbleTest001();
    EventBubbleTest002();
    EventBubbleTest003();
    EventBubbleTest004();
    EventBubbleTest005();
    EventBubbleTest006();
    EventBubbleTest007();
    EventBubbleTest008();
    EventBubbleTest009();
    EventBubbleTest010();
    EventBubbleTest011();
    EventBubbleTest012();
    EventBubbleTest013();
    EventBubbleTest014();
    EventBubbleTest015();
    EventBubbleTest016();
    EventBubbleTest017();
}

#ifdef TDD_ASSERTIONS
/* *
 * @tc.name: EventBubbleTest001
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest001, TestSize.Level0)
{
    EventBubbleTddTest::EventBubbleTest001();
}

/* *
 * @tc.name: EventBubbleTest002
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest002, TestSize.Level0)
{
    EventBubbleTddTest::EventBubbleTest002();
}

/* *
 * @tc.name: EventBubbleTest003
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest003, TestSize.Level1)
{
    EventBubbleTddTest::EventBubbleTest003();
}

/* *
 * @tc.name: EventBubbleTest004
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest004, TestSize.Level1)
{
    EventBubbleTddTest::EventBubbleTest004();
}

/* *
 * @tc.name: EventBubbleTest005
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest005, TestSize.Level1)
{
    EventBubbleTddTest::EventBubbleTest005();
}

/* *
 * @tc.name: EventBubbleTest006
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest006, TestSize.Level1)
{
    EventBubbleTddTest::EventBubbleTest006();
}

/* *
 * @tc.name: EventBubbleTest007
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest007, TestSize.Level1)
{
    EventBubbleTddTest::EventBubbleTest007();
}

/* *
 * @tc.name: EventBubbleTest008
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest008, TestSize.Level1)
{
    EventBubbleTddTest::EventBubbleTest008();
}

/* *
 * @tc.name: EventBubbleTest009
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest009, TestSize.Level1)
{
    EventBubbleTddTest::EventBubbleTest009();
}

/* *
 * @tc.name: EventBubbleTest010
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest010, TestSize.Level1)
{
    EventBubbleTddTest::EventBubbleTest010();
}

/* *
 * @tc.name: EventBubbleTest011
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest011, TestSize.Level1)
{
    EventBubbleTddTest::EventBubbleTest011();
}

/* *
 * @tc.name: EventBubbleTest012
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest012, TestSize.Level1)
{
    EventBubbleTddTest::EventBubbleTest012();
}

/* *
 * @tc.name: EventBubbleTest013
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest013, TestSize.Level1)
{
    EventBubbleTddTest::EventBubbleTest013();
}

/* *
 * @tc.name: EventBubbleTest014
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest014, TestSize.Level1)
{
    EventBubbleTddTest::EventBubbleTest014();
}

/* *
 * @tc.name: EventBubbleTest015
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest015, TestSize.Level1)
{
    EventBubbleTddTest::EventBubbleTest015();
}

/* *
 * @tc.name: EventBubbleTest016
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest016, TestSize.Level1)
{
    EventBubbleTddTest::EventBubbleTest016();
}

/* *
 * @tc.name: EventBubbleTest017
 * @tc.desc: Verify event bubble.
 */
HWTEST_F(EventBubbleTddTest, EventBubbleTest017, TestSize.Level1)
{
    EventBubbleTddTest::EventBubbleTest017();
}

#endif // TDD_ASSERTIONS
} // namespace ACELite
} // namespace OHOS
