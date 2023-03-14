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

#include "acelite_config.h"
#include "canvas_tdd_test.h"
#include "component_factory.h"
#include "js_app_environment.h"
#include "root_view.h"
#include "ui_canvas.h"

namespace OHOS {
namespace ACELite {
const char * const BUNDLE1 =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function render(vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('canvas', {\n"
    "        staticClass: ['container'],\n"
    "        attrs: {\n"
    "          ref: 'canvas1'\n"
    "        }\n"
    "      });\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          width: '454px',\n"
    "          height: '454px',\n"
    "          backgroundColor: '#ffff00'\n"
    "        }\n"
    "      }\n"
    "    }\n"
    "  });\n"
    "})();\n";

const char * const BUNDLE2 =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function render(vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('canvas', {\n"
    "        staticStyle:{width: '300px', backgroundColor: '#ffff00'},\n"
    "        dynamicStyle:{height: function height(){return _vm.heightVal}},\n"
    "        on: {\n"
    "            click: _vm.changeHeight\n"
    "        },\n"
    "        attrs: {\n"
    "          ref: 'canvas1'\n"
    "        }\n"
    "      });\n"
    "    },\n"
    "    changeHeight: function changeHeight() {\n"
    "        this.heightVal = '300px';\n"
    "    },\n"
    "    data:{heightVal: '200px'}\n"
    "  });\n"
    "})();\n";

void CanvasTddTest::ComponentCanvasDefaultValueTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE1, strlen(BUNDLE1));
    UICanvas *canvas = reinterpret_cast<UICanvas *>(GetViewByRef(page, "canvas1"));
    EXPECT_TRUE(canvas != nullptr);
    if (canvas != nullptr) {
        UIViewType viewType = canvas->GetViewType();
        EXPECT_TRUE(viewType == UI_CANVAS);
        int16_t defaultWidth = 454;
        int16_t defaultHeight = 454;
        EXPECT_TRUE(canvas->GetWidth() == defaultWidth);
        EXPECT_TRUE(canvas->GetHeight() == defaultHeight);
    }
    DestroyPage(page);
    TDD_CASE_END();
}

void CanvasTddTest::ComponentCanvasDefaultValueTest002()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE1, strlen(BUNDLE1));
    UICanvas *canvas = reinterpret_cast<UICanvas *>(GetViewByRef(page, "canvas1"));
    EXPECT_TRUE(canvas != nullptr);
    if (canvas != nullptr) {
        EXPECT_EQ(canvas->GetStartPosition().x, 0);
        EXPECT_EQ(canvas->GetStartPosition().y, 0);
    }
    DestroyPage(page);
    TDD_CASE_END();
}

void CanvasTddTest::ComponentCanvasChangeHeightTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE2, strlen(BUNDLE2));
    UICanvas *canvas = reinterpret_cast<UICanvas *>(GetViewByRef(page, "canvas1"));
    EXPECT_TRUE(canvas != nullptr);
    if (canvas != nullptr) {
        ClickByRef(page, "canvas1");
        int16_t changedHeight = 300;
        int16_t height = canvas->GetHeight();
        EXPECT_TRUE(height == changedHeight);
    }
    DestroyPage(page);
    TDD_CASE_END();
}

void CanvasTddTest::RunTests()
{
    ComponentCanvasDefaultValueTest001();
    ComponentCanvasDefaultValueTest002();
    ComponentCanvasChangeHeightTest001();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name:ComponentCanvasDefaultValueTest001
 * @tc.desc: Verify canvas default value.
 */
HWTEST_F(CanvasTddTest, CanvasDefault001, TestSize.Level1)
{
    CanvasTddTest::ComponentCanvasDefaultValueTest001();
}

/**
 * @tc.name:ComponentCanvasDefaultValueTest002
 * @tc.desc: Verify canvas default value.
 */
HWTEST_F(CanvasTddTest, CanvasDefault002, TestSize.Level1)
{
    CanvasTddTest::ComponentCanvasDefaultValueTest002();
}

/**
 * @tc.name:ComponentCanvasChangeHeightTest001
 * @tc.desc: Verify canvas change height.
 */
HWTEST_F(CanvasTddTest, CanvasChangeHeight001, TestSize.Level1)
{
    CanvasTddTest::ComponentCanvasChangeHeightTest001();
}

#endif
} // namespace ACELite
} // namespace OHOS
