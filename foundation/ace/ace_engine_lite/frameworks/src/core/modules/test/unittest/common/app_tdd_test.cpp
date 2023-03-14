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

#include "app_tdd_test.h"
#include "js_app_context.h"
#include "product_adapter.h"

namespace OHOS {
namespace ACELite {
const char * const BUNDLE01 =
    "(function () {\n"
    "  var app = requireNative('system.app');\n"
    "  return new ViewModel({\n"
    "    render: function render(vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', {\n"
    "        staticClass: ['container']\n"
    "      }, [_c('input', {\n"
    "        staticClass: ['btn'],\n"
    "        attrs: {\n"
    "          ref: 'btn',\n"
    "          type: 'button',\n"
    "          value: 'GET'\n"
    "        },\n"
    "        on: {\n"
    "          click: _vm.handleClick\n"
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
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      info: ''\n"
    "    },\n"
    "    handleClick: function handleClick() {\n"
    "      this.info = JSON.stringify(app.getInfo());\n"
    "    }\n"
    "  });\n"
    "})();\n";

const char * const BUNDLE02 =
    "(function () {\n"
    "  var app = requireNative('system.app');\n"
    "  return new ViewModel({\n"
    "    render: function render(vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', {\n"
    "        staticClass: ['container']\n"
    "      }, [_c('input', {\n"
    "        staticClass: ['btn'],\n"
    "        attrs: {\n"
    "          ref: 'btn',\n"
    "          type: 'button',\n"
    "          value: 'Terminate'\n"
    "        },\n"
    "        on: {\n"
    "          click: _vm.exit\n"
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
    "        btn: {\n"
    "          width: '80%'\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    exit: function exit() {\n"
    "      app.terminate();\n"
    "    }\n"
    "  });\n"
    "})();\n";

bool exited = false;

void TerminateInterface(uint32_t token, bool forceStop)
{
    UNUSED(forceStop);
    UNUSED(token);
    exited = true;
}

void AppTddTest::AppTddTest01()
{
    TDD_CASE_BEGIN();
    const uint8_t token = 255;
#ifdef __LINUX__
    JsAppContext::GetInstance()->SetCurrentAbilityInfo("/storage/data", "com.example.test", token);
#else
    JsAppContext::GetInstance()->SetCurrentAbilityInfo("/user/data", "com.example.test", token);
#endif
    JSValue page = CreatePage(BUNDLE01, strlen(BUNDLE01));
    ClickByRef(page, "btn");
    char *info = JSObject::GetString(page, "info");
    EXPECT_STREQ(info, "{\"appName\":\"com.example.test\",\"versionName\":\"1.0.0\",\"versionCode\":1000000}");
    ACE_FREE(info);
    DestroyPage(page);
    TDD_CASE_END();
}

void AppTddTest::AppTddTest02()
{
    TDD_CASE_BEGIN();
    ProductAdapter::RegTerminatingHandler(TerminateInterface);
    JSValue page = CreatePage(BUNDLE02, strlen(BUNDLE02));
    ClickByRef(page, "btn");
    EXPECT_TRUE(exited);
    DestroyPage(page);
    TDD_CASE_END();
}

void AppTddTest::RunTests()
{
    AppTddTest::AppTddTest01();
    AppTddTest::AppTddTest02();
}
} // namespace ACELite
} // namespace OHOS
