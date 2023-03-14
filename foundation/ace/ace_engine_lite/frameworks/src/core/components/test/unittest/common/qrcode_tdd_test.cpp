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
#include "component_factory.h"
#include "js_app_environment.h"
#include "qrcode_tdd_test.h"
#include "root_view.h"
#include "ui_qrcode.h"

namespace OHOS {
namespace ACELite {
const char * const BUNDLE_ATTRIBUTE1 =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container']\n"
    "            },[_c('qrcode',{\n"
    "                staticStyle:{width: '300px', height: '300px'},\n"
    "                attrs:{\n"
    "                    ref: 'qrcode',\n"
    "                    value: 'Hello World!'\n"
    "                }\n"
    "            })]);\n"
    "        },\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: {\n"
    "                    width: '454px',\n"
    "                    height: '454px',\n"
    "                    justifyContent: 'center',\n"
    "                    alignItems: 'center'\n"
    "                }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

const char * const BUNDLE_ATTRIBUTE2 =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container'],\n"
    "                attrs: {ref: 'div'},\n"
    "                on: {\n"
    "                    click: _vm.changeHeight\n"
    "                }\n"
    "            },[_c('qrcode', {\n"
    "                staticStyle: {width: '300px'},\n"
    "                dynamicStyle: {height: function height() {return _vm.heightVal}},\n"
    "                attrs: {\n"
    "                    value: 'Hello World!',\n"
    "                    ref: 'qrcode'\n"
    "                }\n"
    "            })]);\n"
    "        },\n"
    "        data:{heightVal: '300px'},\n"
    "        changeHeight: function changeHeight() {this.heightVal = '400px';},\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: {\n"
    "                    width: '454px',\n"
    "                    height: '454px',\n"
    "                    justifyContent: 'center',\n"
    "                    alignItems: 'center'\n"
    "                }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

void QRCodeTddTest::ComponentQrcodeStyleValueTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_ATTRIBUTE1, strlen(BUNDLE_ATTRIBUTE1));
    UIQrcode *qrcode = reinterpret_cast<UIQrcode *>(GetViewByRef(page, "qrcode"));
    EXPECT_TRUE(qrcode != nullptr);
    if (qrcode != nullptr) {
        int16_t defaultWidth = 300;
        int16_t defaultHeight = 300;
        int16_t width = qrcode->GetWidth();
        printf("width:%d", width);
        EXPECT_TRUE(width == defaultWidth);
        int16_t height = qrcode->GetHeight();
        printf("height:%d", height);
        EXPECT_TRUE(height == defaultHeight);
    }
    DestroyPage(page);
    TDD_CASE_END();
}

void QRCodeTddTest::ComponentQrcodeStyleValueTest002()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_ATTRIBUTE2, strlen(BUNDLE_ATTRIBUTE2));
    UIQrcode *qrcode = reinterpret_cast<UIQrcode *>(GetViewByRef(page, "qrcode"));
    EXPECT_TRUE(qrcode != nullptr);
    if (qrcode != nullptr) {
        ClickByRef(page, "div");
        int16_t defaultHeight = 400;
        int16_t height = qrcode->GetHeight();
        EXPECT_TRUE(height == defaultHeight);
    }
    DestroyPage(page);
    TDD_CASE_END();
}

void QRCodeTddTest::ComponentQrcodeAttributeValueTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_ATTRIBUTE1, strlen(BUNDLE_ATTRIBUTE1));
    UIQrcode *qrcode = reinterpret_cast<UIQrcode *>(GetViewByRef(page, "qrcode"));
    EXPECT_TRUE(qrcode != nullptr);
    if (qrcode != nullptr) {
        UIViewType viewType = qrcode->GetViewType();
        EXPECT_TRUE(viewType == UI_QRCODE);
    }
    DestroyPage(page);
    TDD_CASE_END();
}

void QRCodeTddTest::RunTests()
{
    ComponentQrcodeStyleValueTest001();
    ComponentQrcodeStyleValueTest002();
    ComponentQrcodeAttributeValueTest001();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name:ComponentQrcodeStyleValueTest001
 * @tc.desc: Verify qrcode default style.
 */
HWTEST_F(QRCodeTddTest, QrcodeStyle001, TestSize.Level1)
{
    QRCodeTddTest::ComponentQrcodeStyleValueTest001();
}

/**
 * @tc.name:ComponentQrcodeStyleValueTest002
 * @tc.desc: Verify change qrcode height.
 */
HWTEST_F(QRCodeTddTest, QrcodeStyle002, TestSize.Level0)
{
    QRCodeTddTest::ComponentQrcodeStyleValueTest002();
}

/**
 * @tc.name:ComponentQrcodeAttributeValueTest001
 * @tc.desc: Verify qrcode viewtype.
 */
HWTEST_F(QRCodeTddTest, QrcodeAttr001, TestSize.Level1)
{
    QRCodeTddTest::ComponentQrcodeAttributeValueTest001();
}
#endif
} // namespace ACELite
} // namespace OHOS
