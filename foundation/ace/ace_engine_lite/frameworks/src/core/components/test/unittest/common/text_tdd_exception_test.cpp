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

#include "text_tdd_exception_test.h"
#include "acelite_config.h"
#include "component_factory.h"
#include "js_app_environment.h"
#include "root_view.h"
#include "text_component.h"
#include "ui_label.h"

namespace OHOS {
namespace ACELite {
TextTddExceptionTest::TextTddExceptionTest():BaseTest()
{
    componentNameId_ = KeyParser::ParseKeyId("text");
}
void TextTddExceptionTest::ComponentTextStyleSetExceptionTest007()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set exception color value = "helloworld"
     */
    const char* expectColorStrValue = "helloWorld";
    const char* colorStr = "color";
    jerry_value_t textKey = jerry_create_string((const jerry_char_t*)colorStr);
    jerry_value_t textColorValue = jerry_create_string((const jerry_char_t*)expectColorStrValue);
    jerry_set_property(styleObj_, textKey, textColorValue);
    /**
     * @tc.expected: step2. console print exception info
     */
    Component* textComponent = reinterpret_cast<TextComponent *>(GetRenderedComponent(componentNameId_));
    jerry_release_value(textKey);
    jerry_release_value(textColorValue);

    /**
     * @tc.expected: step3. update textOverflow with exception value "exception"
     * console print exception info
     */
    const char* textOverflow = "textOverflow";
    const char* expectOverflowException = "exception";
    UpdateCharAttributeOrStyleValue(textComponent, textOverflow, expectOverflowException, false);

    /**
     * @tc.expected: step4. update letterSpacing with exception value "helloWorld"
     * console print exception info
     */
    const char* letterSpacingStr = "letterSpacing";
    const char* expectletterSpacingStrValue = "helloWorld";
    UpdateCharAttributeOrStyleValue(textComponent, letterSpacingStr, expectletterSpacingStrValue, false);

    /**
     * @tc.expected: step5. update textAlign with exception value "exception"
     * console print exception info
     */
    const char* textAlign = "textAlign";
    const char* expectALignException = "exception";
    UpdateCharAttributeOrStyleValue(textComponent, textAlign, expectALignException, false);

    TDD_CASE_END();
}

void TextTddExceptionTest::RunTests()
{
    ComponentTextStyleSetExceptionTest007();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name:Component_Text_StyleSet_Color_Test_002
 * @tc.desc: Verify text color/textOverFlow/textAlign/letterSpace exception value.
 */
HWTEST_F(TextTddExceptionTest, textStyle007, TestSize.Level0)
{
    TextTddExceptionTest::ComponentTextStyleSetExceptionTest007();
}
#endif
} // namespace ACELite
} // namespace OHOS
