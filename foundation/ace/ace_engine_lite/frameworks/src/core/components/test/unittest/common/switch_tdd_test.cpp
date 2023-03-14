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

#include "switch_tdd_test.h"
#include "js_app_environment.h"
#include "root_view.h"
#include "switch_component.h"

namespace OHOS {
namespace ACELite {
SwitchTddTest::SwitchTddTest()
    : optionsObj_(0),
      attrsObj_(0),
      styleObj_(0),
      eventObj_(0),
      childrenObj_(0),
      componentKeyId_(KeyParser::ParseKeyId("switch"))
{
}

void SwitchTddTest::SetUp()
{
    JsAppEnvironment* appJsEnv = JsAppEnvironment::GetInstance();
    appJsEnv->InitJsFramework();

    optionsObj_ = jerry_get_global_object();

    jerry_value_t attrsKey = jerry_create_string((const jerry_char_t*)"attrs");
    attrsObj_ = jerry_create_object();
    jerry_value_t ret1 = jerry_set_property(optionsObj_, attrsKey, attrsObj_);

    jerry_value_t styleKey = jerry_create_string((const jerry_char_t*)"staticStyle");
    styleObj_ = jerry_create_object();
    jerry_value_t ret2 = jerry_set_property(optionsObj_, styleKey, styleObj_);

    jerry_value_t eventKey = jerry_create_string((const jerry_char_t*)"on");
    eventObj_ = jerry_create_object();
    jerry_value_t ret3 = jerry_set_property(optionsObj_, eventKey, eventObj_);

    ReleaseJerryValue(attrsKey, ret1, styleKey, ret2, eventKey, ret3, VA_ARG_END_FLAG);
}

void SwitchTddTest::TearDown()
{
    ReleaseJerryValue(attrsObj_, styleObj_, eventObj_, optionsObj_, VA_ARG_END_FLAG);
    JsAppContext::GetInstance()->ReleaseStyles();
}

void SwitchTddTest::ComponentSwitchAttributeSetTest001()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. do not set checked
     */
    Component* component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    component->Render();

    /**
     * @tc.steps: step2. get native switch object;get checked false
     */
    UIView* view = component->GetComponentRootView();
    UIToggleButton* toggleButton = reinterpret_cast<UIToggleButton *>(view);
    if (toggleButton->GetState() == false) {
        printf("ComponentSwitchAttributeSetTest001 pass\n");
    } else {
        printf("ComponentSwitchAttributeSetTest001 fail\n");
    }
    EXPECT_FALSE(toggleButton->GetState());
    component->Release();

    /**
     * @tc.steps: step3. set checked = true; get checked = true
     */
    jerry_value_t checkedKey = jerry_create_string((const jerry_char_t*)("checked"));
    jerry_value_t checkedVal = jerry_create_boolean(true);
    jerry_value_t ret1 = jerry_set_property(attrsObj_, checkedKey, checkedVal);
    component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    component->Render();
    view = component->GetComponentRootView();
    toggleButton = reinterpret_cast<UIToggleButton *>(view);
    if (toggleButton->GetState() == true) {
        printf("ComponentSwitchAttributeSetTest001 pass\n");
    } else {
        printf("ComponentSwitchAttributeSetTest001 fail\n");
    }
    EXPECT_TRUE(toggleButton->GetState());
    component->Release();
    jerry_release_value(checkedVal);

    /**
     * @tc.steps: step4. set checkedVal false; get checkedVal false
     */
    checkedVal = jerry_create_boolean(false);
    jerry_value_t ret2 = jerry_set_property(attrsObj_, checkedKey, checkedVal);
    component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    component->Render();
    view = component->GetComponentRootView();
    toggleButton = reinterpret_cast<UIToggleButton *>(view);
    if (toggleButton->GetState() == false) {
        printf("ComponentSwitchAttributeSetTest001 pass\n");
    } else {
        printf("ComponentSwitchAttributeSetTest001 fail\n");
    }
    EXPECT_FALSE(toggleButton->GetState());
    component->Release();

    ReleaseJerryValue(checkedKey, checkedVal, ret1, ret2, VA_ARG_END_FLAG);

    TDD_CASE_END();
}

void SwitchTddTest::ComponentSwitchAttributeSetTest002()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set checked true
     */
    jerry_value_t checkedKey = jerry_create_string((const jerry_char_t*)("checked"));
    jerry_value_t checkedVal = jerry_create_string((const jerry_char_t*)("abc123"));
    jerry_value_t ret1 = jerry_set_property(attrsObj_, checkedKey, checkedVal);
    Component* component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    component->Render();

    /**
     * @tc.steps: step2. get native switch object
     */
    UIView* view = component->GetComponentRootView();
    UIToggleButton* toggleButton = reinterpret_cast<UIToggleButton *>(view);

    /**
     * @tc.steps: step3. get checked val
     */
    if (toggleButton->GetState() == true) {
        printf("ComponentSwitchAttributeSetTest002 pass\n");
    } else {
        printf("ComponentSwitchAttributeSetTest002 fail\n");
    }
    EXPECT_TRUE(toggleButton->GetState());
    component->Release();

    ReleaseJerryValue(checkedKey, checkedVal, ret1, VA_ARG_END_FLAG);

    TDD_CASE_END();
}

void SwitchTddTest::ComponentSwitchAttributeSetTest003()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set id "switch", show = true
     */
    jerry_value_t idKey = jerry_create_string((const jerry_char_t*)("id"));
    jerry_value_t idVal = jerry_create_string((const jerry_char_t*)("switch"));
    jerry_value_t ret1 = jerry_set_property(attrsObj_, idKey, idVal);
    jerry_value_t showKey = jerry_create_string((const jerry_char_t*)("show"));
    jerry_value_t showVal = jerry_create_boolean(false);
    jerry_value_t ret2 = jerry_set_property(attrsObj_, showKey, showVal);
    Component* component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    component->Render();

    /**
     * @tc.steps: step2. get native swiper object
     */
    UIView* view = component->GetComponentRootView();
    UIToggleButton* toggleButton = reinterpret_cast<UIToggleButton *>(view);

    /**
     * @tc.steps: step3. get id, show
     */
    if (!strcmp(toggleButton->GetViewId(), "switch") && toggleButton->IsVisible() == false) {
        printf("ComponentSwitchAttributeSetTest003 pass\n");
    } else {
        printf("ComponentSwitchAttributeSetTest003 fail\n");
    }
    EXPECT_TRUE((!strcmp(toggleButton->GetViewId(), "switch")) && (toggleButton->IsVisible() == false));

    component->Release();

    ReleaseJerryValue(idKey, idVal, showKey, showVal, ret1, ret2, VA_ARG_END_FLAG);

    TDD_CASE_END();
}

void SwitchTddTest::RunTests()
{
    ComponentSwitchAttributeSetTest001();
    ComponentSwitchAttributeSetTest002();
    ComponentSwitchAttributeSetTest003();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: ComponentSwitchAttributeSetTest001
 * @tc.desc: Verify attribute checked set normal value.
 */
HWTEST_F(SwitchTddTest, SwitchAttr001, TestSize.Level1)
{
    SwitchTddTest::ComponentSwitchAttributeSetTest001();
}

/**
 * @tc.name: ComponentSwitchAttributeSetTest002
 * @tc.desc: Verify attribute checked set abc123.
 */
HWTEST_F(SwitchTddTest, SwitchAttr002, TestSize.Level1)
{
    SwitchTddTest::ComponentSwitchAttributeSetTest002();
}

/**
 * @tc.name: ComponentSwitchAttributeSetTest003
 * @tc.desc: Verify attribute id, show.
 */
HWTEST_F(SwitchTddTest, SwitchAttr003, TestSize.Level0)
{
    SwitchTddTest::ComponentSwitchAttributeSetTest003();
}
#endif // TDD_ASSERTIONS
} // ACELite
} // OHOS
