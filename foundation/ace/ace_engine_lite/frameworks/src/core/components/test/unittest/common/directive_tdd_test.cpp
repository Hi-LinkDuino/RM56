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
#include "directive_tdd_test.h"
#include "flex_layout.h"
#include "ui_label.h"

namespace OHOS {
namespace ACELite {
const char * const BUNDLE01 =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function render(vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', {\n"
    "        staticClass: ['container']\n"
    "      }, [_i(function () {\n"
    "        return _vm.condition;\n"
    "      }, function () {\n"
    "        return _c('text', {\n"
    "          attrs: {\n"
    "            value: 'Hello world',\n"
    "            ref: 'text'\n"
    "          },\n"
    "          staticClass: ['title']\n"
    "        });\n"
    "      }), _c('input', {\n"
    "        staticClass: ['btn'],\n"
    "        attrs: {\n"
    "          ref: 'btn',\n"
    "          type: 'button',\n"
    "          value: function value() {\n"
    "            return _vm.condition ? 'hide' : 'show';\n"
    "          }\n"
    "        },\n"
    "        on: {\n"
    "          click: _vm.toggle\n"
    "        }\n"
    "      })]);\n"
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
    "        title: {\n"
    "          textAlign: 'center',\n"
    "          width: '80%'\n"
    "        },\n"
    "        btn: {\n"
    "          borderRadius: 0,\n"
    "          width: '80%'\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      condition: true\n"
    "    },\n"
    "    toggle: function toggle() {\n"
    "      this.condition = !this.condition;\n"
    "    }\n"
    "  });\n"
    "})();\n";

const char * const BUNDLE02 =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function render(vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', {\n"
    "        staticClass: ['container']\n"
    "      }, [_c('div', {\n"
    "        staticClass: ['btn-box']\n"
    "      }, [_c('input', {\n"
    "        staticClass: ['btn'],\n"
    "        attrs: {\n"
    "          ref: 'addBtn',\n"
    "          type: 'button',\n"
    "          value: 'ADD'\n"
    "        },\n"
    "        on: {\n"
    "          click: _vm.addElement\n"
    "        }\n"
    "      }), _c('input', {\n"
    "        staticClass: ['btn'],\n"
    "        attrs: {\n"
    "          ref: 'popBtn',\n"
    "          type: 'button',\n"
    "          value: 'POP'\n"
    "        },\n"
    "        on: {\n"
    "          click: _vm.popElement\n"
    "        }\n"
    "      })]), _c('div', {\n"
    "        staticClass: ['text-box'],\n"
    "        attrs: {\n"
    "          ref: 'textBox'\n"
    "        }\n"
    "      }, [_l(function () {\n"
    "        return _vm.dataSource;\n"
    "      }, function (item) {\n"
    "        return _c('text', {\n"
    "          attrs: {\n"
    "            ref: 'text',\n"
    "            value: function value() {\n"
    "              return item;\n"
    "            }\n"
    "          },\n"
    "          staticClass: ['text']\n"
    "        });\n"
    "      })])]);\n"
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
    "        'btn-box': {\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100px',\n"
    "          width: '100%'\n"
    "        },\n"
    "        btn: {\n"
    "          borderRadius: 0,\n"
    "          width: '120px'\n"
    "        },\n"
    "        'text-box': {\n"
    "          flexWrap: 'wrap',\n"
    "          height: '354px',\n"
    "          width: '100%'\n"
    "        },\n"
    "        text: {\n"
    "          textAlgin: 'center',\n"
    "          width: '100px'\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      dataSource: ['1', '2', '3']\n"
    "    },\n"
    "    addElement: function addElement() {\n"
    "      this.dataSource.push(this.dataSource.length + 1 + '');\n"
    "    },\n"
    "    popElement: function popElement() {\n"
    "      this.dataSource.splice(this.dataSource.length - 1, 1);\n"
    "    }\n"
    "  });\n"
    "})();\n";

const char * const BUNDLE03 =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function render(vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', {\n"
    "        staticClass: ['container']\n"
    "      }, [_c('text', {\n"
    "        staticClass: ['title'],\n"
    "        attrs: {\n"
    "          ref: 'text',\n"
    "          show: false,\n"
    "          value: function value() {\n"
    "            return 'Hello ' + _vm.message;\n"
    "          }\n"
    "        }\n"
    "      })]);\n"
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
    "        title: {\n"
    "          textAlign: 'center',\n"
    "          width: '80%',\n"
    "          display: 'flex'\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      message: 'OpenHarmony'\n"
    "    }\n"
    "  });\n"
    "})();\n";

void DirectiveTddTest::RunTests()
{
    DirectiveTest001();
    DirectiveTest002();
    DirectiveTest003();
}

void DirectiveTddTest::DirectiveTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE01, strlen(BUNDLE01));
    UILabel *label = reinterpret_cast<UILabel *>(GetViewByRef(page, "text"));
    EXPECT_TRUE(label != nullptr);
    if (label != nullptr) {
        ClickByRef(page, "btn");
        label = reinterpret_cast<UILabel *>(GetViewByRef(page, "text"));
        EXPECT_TRUE(label == nullptr);
        ClickByRef(page, "btn");
        label = reinterpret_cast<UILabel *>(GetViewByRef(page, "text"));
        EXPECT_TRUE(label != nullptr);
    }
    DestroyPage(page);
    TDD_CASE_END();
}

void DirectiveTddTest::DirectiveTest002()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE02, strlen(BUNDLE02));
    FlexLayout *textBox = reinterpret_cast<FlexLayout *>(GetViewByRef(page, "textBox"));
    EXPECT_TRUE(textBox != nullptr);
    if (textBox != nullptr) {
        uint8_t size = GetChildrenSize(*textBox);
        constexpr uint8_t initCount = 3;
        constexpr uint8_t addedCount = 4;
        EXPECT_TRUE(size == initCount);
        ClickByRef(page, "addBtn");
        size = GetChildrenSize(*textBox);
        EXPECT_TRUE(size == addedCount);
        ClickByRef(page, "popBtn");
        size = GetChildrenSize(*textBox);
        EXPECT_TRUE(size == initCount);
    }
    DestroyPage(page);
    TDD_CASE_END();
}

void DirectiveTddTest::DirectiveTest003()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE03, strlen(BUNDLE03));
    UILabel *text = reinterpret_cast<UILabel *>(GetViewByRef(page, "text"));
    EXPECT_TRUE(text != nullptr);
    if (text != nullptr) {
        EXPECT_FALSE(text->IsVisible());
    }
    DestroyPage(page);
    TDD_CASE_END();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: DirectiveTest001
 * @tc.desc: test id directive
 */
HWTEST_F(DirectiveTddTest, DirectiveTest001, TestSize.Level1)
{
    DirectiveTddTest::DirectiveTest001();
}

/**
 * @tc.name: DirectiveTest002
 * @tc.desc: test for directive
 */
HWTEST_F(DirectiveTddTest, DirectiveTest002, TestSize.Level0)
{
    DirectiveTddTest::DirectiveTest002();
}

/**
 * @tc.name: DirectiveTest003
 * @tc.desc: test show directive
 */
HWTEST_F(DirectiveTddTest, DirectiveTest003, TestSize.Level0)
{
    DirectiveTddTest::DirectiveTest003();
}
#endif
} // namespace ACELite
} // namespace OHOS
