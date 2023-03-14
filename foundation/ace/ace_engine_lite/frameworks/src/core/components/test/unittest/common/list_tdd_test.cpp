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

#include "list_tdd_test.h"
#include "acelite_config.h"
#include "component_factory.h"
#include "js_app_environment.h"

namespace OHOS {
namespace ACELite {
ListTddTest::ListTddTest()
    : objGlob(UNDEFINED),
      objAttrs(UNDEFINED),
      objStaticStyle(UNDEFINED){}
void ListTddTest::SetUp()
{
    JsAppEnvironment* appJsEnv = JsAppEnvironment::GetInstance();
    appJsEnv->InitJsFramework();

    objGlob = jerry_get_global_object();

    objAttrs = jerry_create_object();
    JSObject::Set(objGlob, "attrs", objAttrs);

    objStaticStyle = jerry_create_object();
    JSObject::Set(objGlob, "staticStyle", objStaticStyle);
}

void ListTddTest::TearDown()
{
    JsAppContext::GetInstance()->ReleaseStyles();
    ReleaseJerryValue(objGlob, objAttrs, objStaticStyle, VA_ARG_END_FLAG);
}

void ListTddTest::ListTest001()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. setting flex-direction style = row.
     */
    JSObject::SetString(objStaticStyle, "flexDirection", "row");

    jerry_value_t children = jerry_create_null();
    uint16_t componentNameId = KeyParser::ParseKeyId("list", strlen("list"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    jerry_release_value(children);
    component->Render();

    UIList* listView = reinterpret_cast<UIList *>(component->GetComponentRootView());

#ifdef TDD_ASSERTIONS
    char className[] = "N4OHOS6UIListE";
#else
    char className[] = "class OHOS::UIList";
#endif

    /**
     * @tc.expected: step1. get flex-direction style = row.
     */
    if (strcmp(className, typeid(*listView).name()) == 0 &&
        reinterpret_cast<UIList *>(listView)->GetDirection() == UIList::HORIZONTAL) {
        printf("[Test Case] [ListTest001] PASSED\n");
    } else {
        printf("[Test Case] [ListTest001] FAILED\n");
    }
    EXPECT_TRUE(strcmp(className, typeid(*listView).name()) == 0 &&
        reinterpret_cast<UIList *>(listView)->GetDirection() == UIList::HORIZONTAL);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void ListTddTest::ListTest002()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. setting flex-direction style = column.
     */
    JSObject::SetString(objStaticStyle, "flexDirection", "column");

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("list", strlen("list"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    jerry_release_value(children);

    component->Render();

    UIViewGroup* listView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
#ifdef TDD_ASSERTIONS
    char className[] = "N4OHOS6UIListE";
#else
    char className[] = "class OHOS::UIList";
#endif

    static constexpr uint8_t vertical = 1;

    /**
     * @tc.expected: step1. get flex-direction style = column.
     */
    if (strcmp(className, typeid(*listView).name()) == 0 &&
        reinterpret_cast<UIList *>(listView)->GetDirection() == vertical) {
        printf("[Test Case] [ListTest002] PASSED\n");
    } else {
        printf("[Test Case] [ListTest002] FAILED\n");
    }
    EXPECT_TRUE(strcmp(className, typeid(*listView).name()) == 0 &&
        reinterpret_cast<UIList *>(listView)->GetDirection() == vertical);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void ListTddTest::ListTest003()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. setting not exist flex-direction style = abcd.
     */
    JSObject::SetString(objStaticStyle, "flexDirection", "abcd");

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("list", strlen("list"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    jerry_release_value(children);
    component->Render();

    UIViewGroup* listView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());

#ifdef TDD_ASSERTIONS
    char className[] = "N4OHOS6UIListE";
#else
    char className[] = "class OHOS::UIList";
#endif

    static constexpr uint8_t vertical = 1;

    /**
     * @tc.expected: step1. get flex-direction style = column.
     */
    if (strcmp(className, typeid(*listView).name()) == 0 &&
        reinterpret_cast<UIList *>(listView)->GetDirection() == vertical) {
        printf("[Test Case] [ListTest003] PASSED\n");
    } else {
        printf("[Test Case] [ListTest003] FAILED\n");
    }
    EXPECT_TRUE(strcmp(className, typeid(*listView).name()) == 0 &&
        reinterpret_cast<UIList *>(listView)->GetDirection() == vertical);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void ListTddTest::ListTest004()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. not setting flex-direction style.
     */
    JSObject::SetString(objStaticStyle, "width", "240");

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("list", strlen("list"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    jerry_release_value(children);
    component->Render();

    UIViewGroup* listView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());

#ifdef TDD_ASSERTIONS
    char className[] = "N4OHOS6UIListE";
#else
    char className[] = "class OHOS::UIList";
#endif

    static constexpr uint8_t vertical = 1;

    /**
     * @tc.expected: step1. get flex-direction style = column.
     */
    if (strcmp(className, typeid(*listView).name()) == 0 &&
        reinterpret_cast<UIList *>(listView)->GetDirection() == vertical) {
        printf("[Test Case] [ListTest004] PASSED\n");
    } else {
        printf("[Test Case] [ListTest004] FAILED\n");
    }
    EXPECT_TRUE(strcmp(className, typeid(*listView).name()) == 0 &&
        reinterpret_cast<UIList *>(listView)->GetDirection() == vertical);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void ListTddTest::RunTests()
{
    ListTest001();
    ListTest002();
    ListTest003();
    ListTest004();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: Component_List_Create_Test_001
 * @tc.desc: Verify list component support setting flex-direction style = row.
 */
HWTEST_F(ListTddTest, ListTest001, TestSize.Level1)
{
    ListTddTest::ListTest001();
}

/**
 * @tc.name: Component_List_Create_Test_002
 * @tc.desc: Verify list component support setting flex-direction style = column.
 */
HWTEST_F(ListTddTest, ListTest002, TestSize.Level1)
{
    ListTddTest::ListTest002();
}

/**
 * @tc.name: Component_List_Create_Test_003
 * @tc.desc: Verify list component support restore the default flex-direction=column if setting not exist type.
 */
HWTEST_F(ListTddTest, ListTest003, TestSize.Level1)
{
    ListTddTest::ListTest003();
}

/**
 * @tc.name: Component_List_Create_Test_004
 * @tc.desc: Verify list component support restore the default flex-direction=column if not setting it.
 */
HWTEST_F(ListTddTest, ListTest004, TestSize.Level0)
{
    ListTddTest::ListTest004();
}
#endif
}
}
