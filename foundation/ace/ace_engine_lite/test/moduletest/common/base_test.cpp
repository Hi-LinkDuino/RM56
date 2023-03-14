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

#include "base_test.h"

#include "js_debugger_config.h"

namespace OHOS {
namespace ACELite {
BaseTest::BaseTest() : globObj_(0), attrsObj_(0), styleObj_(0), componentNameId_(0) {}

void BaseTest::SetUp(void)
{
    Debugger::GetInstance().SetupJSContext();
    jerry_init(JERRY_INIT_EMPTY);

    globObj_ = jerry_get_global_object();
    attrsObj_ = jerry_create_object();
    JerrySetNamedProperty(globObj_, "attrs", attrsObj_);

    styleObj_ = jerry_create_object();
    JerrySetNamedProperty(globObj_, "staticStyle", styleObj_);
    rootComponentMock_.PrepareRootContainer();
}

void BaseTest::TearDown()
{
    JsAppContext::GetInstance()->ReleaseStyles();
    jerry_release_value(attrsObj_);
    jerry_release_value(styleObj_);
    jerry_release_value(globObj_);
    jerry_cleanup();
    Debugger::GetInstance().ReleaseJSContext();
}

Component *BaseTest::GetRenderedComponent(uint16_t componentKeyId) const
{
    jerry_value_t children = jerry_create_null();
    Component *component = ComponentFactory::CreateComponent(componentKeyId, globObj_, children);
    rootComponentMock_.RenderComponent(*component);
    jerry_release_value(children);
    return component;
}

ColorType BaseTest::GetRGBColor(int32_t colorIntValue) const
{
    uint32_t colorValue = colorIntValue;
    uint8_t red8 = uint8_t((colorValue & TEXT_RED_COLOR_MASK) >> RED_COLOR_START_BIT);
    uint8_t green8 = uint8_t((colorValue & TEXT_GREEN_COLOR_MASK) >> GREEN_COLOR_START_BIT);
    uint8_t blue8 = uint8_t((colorValue & TEXT_BLUE_COLOR_MASK));
    return Color::GetColorFromRGB(red8, green8, blue8);
}

void BaseTest::UpdateNumAttributeOrStyleValue(Component *component,
                                              const char *attributeName,
                                              const int32_t newNumValue,
                                              const bool isToSetAttribute) const
{
    if (component == nullptr) {
        HILOG_WARN(HILOG_MODULE_ACE, "UpdateNumAttributeOrStyleValue component is null\n");
        return;
    }
    jerry_value_t attrName = jerry_create_string(reinterpret_cast<const jerry_char_t *>(attributeName));
    jerry_value_t attrValue = jerry_create_number(newNumValue);
    if (isToSetAttribute) {
        jerry_set_property(attrsObj_, attrName, attrValue);
    } else {
        jerry_set_property(styleObj_, attrName, attrValue);
    }
    component->UpdateView(KeyParser::ParseKeyId(attributeName), attrValue);
    jerry_release_value(attrName);
    jerry_release_value(attrValue);
}

void BaseTest::UpdateCharAttributeOrStyleValue(Component *component,
                                               const char *attributeName,
                                               const char *newCharValue,
                                               const bool isToSetAttribute) const
{
    if (component == nullptr) {
        HILOG_WARN(HILOG_MODULE_ACE, "UpdateCharAttributeOrStyleValue component is null\n");
        return;
    }
    jerry_value_t attrName = jerry_create_string(reinterpret_cast<const jerry_char_t *>(attributeName));
    jerry_value_t attrValue = jerry_create_string(reinterpret_cast<const jerry_char_t *>(newCharValue));
    if (isToSetAttribute) {
        jerry_set_property(attrsObj_, attrName, attrValue);
    } else {
        jerry_set_property(styleObj_, attrName, attrValue);
    }
    component->UpdateView(KeyParser::ParseKeyId(attributeName), attrValue);
    jerry_release_value(attrName);
    jerry_release_value(attrValue);
}

uint16_t BaseTest::SetCompnentNameId(const char *componentName)
{
    if (componentName == nullptr) {
        HILOG_WARN(HILOG_MODULE_ACE, "null component name\n");
        return K_UNKNOWN;
    }
    uint8_t maxLength = 9;
    char *tarComponentName = reinterpret_cast<char *>(malloc(maxLength));
    if (tarComponentName == nullptr) {
        HILOG_WARN(HILOG_MODULE_ACE, "alloc memory fail\n");
        return K_UNKNOWN;
    }
    tarComponentName[0] = '\0';
    bool copyRes = false;
    if (!strcmp(componentName, "progress")) {
        if (strcpy_s(tarComponentName, maxLength, "progress") == 0)
            copyRes = true;
    } else if (!strcmp(componentName, "chart")) {
        if (strcpy_s(tarComponentName, maxLength, "chart") == 0)
            copyRes = true;
    } else if (!strcmp(componentName, "marquee")) {
        if (strcpy_s(tarComponentName, maxLength, "marquee") == 0)
            copyRes = true;
    }

    if (copyRes) {
        componentNameId_ = KeyParser::ParseKeyId(tarComponentName, strlen(tarComponentName));
    } else {
        componentNameId_ = K_UNKNOWN;
    }
    free(tarComponentName);
    tarComponentName = nullptr;
    return componentNameId_;
}
} // namespace ACELite
} // namespace OHOS
