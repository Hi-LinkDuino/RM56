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

#include "horizonprogress_tdd_test.h"
#include "acelite_config.h"
#include "horizon_progress_component.h"
#include "js_app_environment.h"
#include "ui_box_progress.h"

namespace OHOS {
namespace ACELite {
HorizonProgressTddTest::HorizonProgressTddTest()
    : objGlob(UNDEFINED),
      objAttrs(UNDEFINED),
      objStaticStyle(UNDEFINED) {}

void HorizonProgressTddTest::SetUp()
{
    printf("ACELiteHorizonProgressTest setup");
    JsAppEnvironment* appJsEnv = JsAppEnvironment::GetInstance();
    appJsEnv->InitJsFramework();

    objGlob = jerry_get_global_object();

    jerry_value_t keyAttrs = jerry_create_string((const jerry_char_t*)"attrs");
    objAttrs = jerry_create_object();
    jerry_set_property(objGlob, keyAttrs, objAttrs);
    jerry_release_value(keyAttrs);

    jerry_value_t keyStaticStyle = jerry_create_string(reinterpret_cast<const jerry_char_t *>("staticStyle"));
    objStaticStyle = jerry_create_object();
    jerry_set_property(objGlob, keyStaticStyle, objStaticStyle);
    jerry_release_value(keyStaticStyle);
    rootComponentMock_.PrepareRootContainer();
}

void HorizonProgressTddTest::TearDown()
{
    JsAppContext::GetInstance()->ReleaseStyles();
}

void HorizonProgressTddTest::HorizonProgressTest001()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. setting progress type=abc.
     */
    jerry_value_t keyId = jerry_create_string((const jerry_char_t*)"type");
    jerry_value_t valueId = jerry_create_string((const jerry_char_t*)"abc");
    jerry_set_property(objAttrs, keyId, valueId);

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("progress", strlen("progress"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    rootComponentMock_.RenderComponent(*component);

    UIView* progressView = reinterpret_cast<UIView *>(component->GetComponentRootView());
    printf("==%s\n", typeid(*progressView).name());
#ifdef TDD_ASSERTIONS
    char className[] = "N4OHOS13UIBoxProgressE";
#else
    char className[] = "class OHOS::UIBoxProgress";
#endif

    /**
     * @tc.expected: step1. get progress type = horizon(UIBoxProgress).
     */
    if (strcmp(className, typeid(*progressView).name()) == 0) {
        printf("[Test Case] [HorizonProgressTest001] PASSED\n");
    } else {
        printf("[Test Case] [HorizonProgressTest001] FAILED\n");
    }
    EXPECT_EQ(strcmp(className, typeid(*progressView).name()), 0);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void HorizonProgressTddTest::HorizonProgressTest002()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. setting progress type=" ".
     */
    jerry_value_t keyId = jerry_create_string((const jerry_char_t*)"type");
    jerry_value_t valueId = jerry_create_string((const jerry_char_t*)" ");
    jerry_set_property(objAttrs, keyId, valueId);

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("progress", strlen("progress"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    rootComponentMock_.RenderComponent(*component);

    UIView* progressView = reinterpret_cast<UIView *>(component->GetComponentRootView());
    printf("==%s\n", typeid(*progressView).name());

#ifdef TDD_ASSERTIONS
    char className[] = "N4OHOS13UIBoxProgressE";
#else
    char className[] = "class OHOS::UIBoxProgress";
#endif
    /**
     * @tc.expected: step1. get progress type = horizon(UIBoxProgress).
     */
    if (strcmp(className, typeid(*progressView).name()) == 0) {
        printf("[Test Case] [HorizonProgressTest002] PASSED\n");
    } else {
        printf("[Test Case] [HorizonProgressTest002] FAILED\n");
    }
    EXPECT_EQ(strcmp(className, typeid(*progressView).name()), 0);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void HorizonProgressTddTest::HorizonProgressTest003()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. setting progress type=arc.
     */
    jerry_value_t keyId = jerry_create_string((const jerry_char_t*)"type");
    jerry_value_t valueId = jerry_create_string((const jerry_char_t*)"arc");
    jerry_set_property(objAttrs, keyId, valueId);

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("progress", strlen("progress"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    rootComponentMock_.RenderComponent(*component);

    UIView* progressView = component->GetComponentRootView();
    printf("==%s\n", typeid(*progressView).name());

#ifdef TDD_ASSERTIONS
    char className[] = "N4OHOS16UICircleProgressE";
#else
    char className[] = "class OHOS::UICircleProgress";
#endif
    /**
    * @tc.expected: step1. get progress type = circle(UICircleProgress).
    */
    if (strcmp(className, typeid(*progressView).name()) == 0) {
        printf("[Test Case] [HorizonProgressTest003] PASSED\n");
    } else {
        printf("[Test Case] [HorizonProgressTest003] FAILED\n");
    }
    EXPECT_EQ(strcmp(className, typeid(*progressView).name()), 0);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void HorizonProgressTddTest::HorizonProgressTest004()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. setting percent=-32768.
     */
    jerry_value_t typeId = jerry_create_string((const jerry_char_t*)"type");
    jerry_value_t typeValue = jerry_create_string((const jerry_char_t*)"horizontal");
    jerry_set_property(objAttrs, typeId, typeValue);
    jerry_release_value(typeId);
    jerry_release_value(typeValue);
    const int32_t percentNum = -32768;
    jerry_value_t percentId = jerry_create_string((const jerry_char_t*)"percent");
    jerry_value_t percentValue = jerry_create_number(percentNum);
    jerry_set_property(objAttrs, percentId, percentValue);
    jerry_release_value(percentId);
    jerry_release_value(percentValue);

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("progress", strlen("progress"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    rootComponentMock_.RenderComponent(*component);
    /**
     * @tc.expected: step1. progress component will not crash.
     */
    UIBoxProgress* progressView = reinterpret_cast<UIBoxProgress *>(component->GetComponentRootView());
    if (progressView->GetValue() != percentNum) {
        printf("[Test Case] [HorizonProgressTest004] PASSED\n");
    } else {
        printf("[Test Case] [HorizonProgressTest004] FAILED\n");
    }
    EXPECT_TRUE(progressView->GetValue() != percentNum);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void HorizonProgressTddTest::HorizonProgressTest005()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. setting percent=100.
     */
    jerry_value_t typeId = jerry_create_string((const jerry_char_t*)"type");
    jerry_value_t typeValue = jerry_create_string((const jerry_char_t*)"horizontal");
    jerry_set_property(objAttrs, typeId, typeValue);
    jerry_release_value(typeId);
    jerry_release_value(typeValue);

    jerry_value_t percentId = jerry_create_string((const jerry_char_t*)"percent");
    const int32_t percentNum = 100;
    jerry_value_t percentValue = jerry_create_number(percentNum);
    jerry_set_property(objAttrs, percentId, percentValue);
    jerry_release_value(percentId);
    jerry_release_value(percentValue);

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("progress", strlen("progress"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }

    rootComponentMock_.RenderComponent(*component);
    UIBoxProgress* progressView = reinterpret_cast<UIBoxProgress *>(component->GetComponentRootView());
    /**
     * @tc.expected: step1. get percent=100.
     */
    if (progressView->GetValue() == percentNum) {
        printf("[Test Case] [HorizonProgressTest005] PASSED\n");
    } else {
        printf("[Test Case] [HorizonProgressTest005] FAILED\n");
    }
    EXPECT_TRUE(progressView->GetValue() == percentNum);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void HorizonProgressTddTest::HorizonProgressTest006()
{
    TDD_CASE_BEGIN();
    jerry_value_t typeId = jerry_create_string((const jerry_char_t*)"type");
    jerry_value_t typeValue = jerry_create_string((const jerry_char_t*)"horizontal");
    jerry_set_property(objAttrs, typeId, typeValue);
    jerry_release_value(typeId);
    jerry_release_value(typeValue);

    /**
     * @tc.steps: step1. setting percent=32767.
     */
    jerry_value_t percentId = jerry_create_string((const jerry_char_t*)"percent");
    const int32_t percentNum = 32767;
    jerry_value_t percentValue = jerry_create_number(percentNum);
    jerry_set_property(objAttrs, percentId, percentValue);
    jerry_release_value(percentId);
    jerry_release_value(percentValue);

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("progress", strlen("progress"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    rootComponentMock_.RenderComponent(*component);

    /**
     * @tc.expected: step1. progress component will not crash.
     */
    UIBoxProgress* progressView = reinterpret_cast<UIBoxProgress *>(component->GetComponentRootView());
    if (progressView->GetValue() != percentNum) {
        printf("[Test Case] [HorizonProgressTest006] PASSED\n");
    } else {
        printf("[Test Case] [HorizonProgressTest006] FAILED\n");
    }
    EXPECT_TRUE(progressView->GetValue() != percentNum);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void HorizonProgressTddTest::HorizonProgressTest007()
{
    TDD_CASE_BEGIN();
    jerry_value_t typeId = jerry_create_string((const jerry_char_t*)"type");
    jerry_value_t typeValue = jerry_create_string((const jerry_char_t*)"horizontal");
    jerry_set_property(objAttrs, typeId, typeValue);
    jerry_release_value(typeId);
    jerry_release_value(typeValue);
    /**
     * @tc.steps: step1. setting percent=-32769.
     */
    jerry_value_t percentId = jerry_create_string((const jerry_char_t*)"percent");
    const int32_t percentNum = -32769;
    jerry_value_t percentValue = jerry_create_number(percentNum);
    jerry_set_property(objAttrs, percentId, percentValue);
    jerry_release_value(percentId);
    jerry_release_value(percentValue);

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("progress", strlen("progress"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    rootComponentMock_.RenderComponent(*component);

    /**
     * @tc.expected: step1. progress component will not crash.
     */
    UIBoxProgress* progressView = reinterpret_cast<UIBoxProgress *>(component->GetComponentRootView());
    if (progressView->GetValue() != percentNum) {
        printf("[Test Case] [HorizonProgressTest007] PASSED\n");
    } else {
        printf("[Test Case] [HorizonProgressTest007] FAILED\n");
    }
    EXPECT_TRUE(progressView->GetValue() != percentNum);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void HorizonProgressTddTest::HorizonProgressTest008()
{
    TDD_CASE_BEGIN();
    jerry_value_t typeId = jerry_create_string((const jerry_char_t*)"type");
    jerry_value_t typeValue = jerry_create_string((const jerry_char_t*)"horizontal");
    jerry_set_property(objAttrs, typeId, typeValue);
    jerry_release_value(typeId);
    jerry_release_value(typeValue);
    /**
     * @tc.steps: step1. setting percent=32768.
     */
    jerry_value_t percentId = jerry_create_string((const jerry_char_t*)"percent");
    const int32_t percentNum = 32768;
    jerry_value_t percentValue = jerry_create_number(percentNum);
    jerry_set_property(objAttrs, percentId, percentValue);
    jerry_release_value(percentId);
    jerry_release_value(percentValue);

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("progress", strlen("progress"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    rootComponentMock_.RenderComponent(*component);

    /**
     * @tc.expected: step1. progress component will not crash.
     */
    UIBoxProgress* progressView = reinterpret_cast<UIBoxProgress *>(component->GetComponentRootView());
    if (progressView->GetValue() != percentNum) {
        printf("[Test Case] [HorizonProgressTest008] PASSED\n");
    } else {
        printf("[Test Case] [HorizonProgressTest008] FAILED\n");
    }
    EXPECT_TRUE(progressView->GetValue() != percentNum);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void HorizonProgressTddTest::HorizonProgressTest009()
{
    TDD_CASE_BEGIN();
    jerry_value_t typeId = jerry_create_string((const jerry_char_t*)"type");
    jerry_value_t typeValue = jerry_create_string((const jerry_char_t*)"horizontal");
    jerry_set_property(objAttrs, typeId, typeValue);
    jerry_release_value(typeId);
    jerry_release_value(typeValue);

    /**
     * @tc.steps: step1. setting color=0x000000.
     */
    jerry_value_t colorKey = jerry_create_string((const jerry_char_t*)"color");
    // 0(0x000000)
    const int32_t percentNum = 0x000000;
    const int32_t redNum = 0x00;
    const int32_t greenNum = 0x00;
    const int32_t blueNum = 0x00;
    jerry_value_t colorValue = jerry_create_number(percentNum);
    jerry_set_property(objStaticStyle, colorKey, colorValue);

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("progress", strlen("progress"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    rootComponentMock_.RenderComponent(*component);

    /**
     * @tc.expected: step1. get color=0x000000.
     */
    UIBoxProgress* progressView = reinterpret_cast<UIBoxProgress *>(component->GetComponentRootView());
    ColorType tempColor;
    tempColor.full = progressView->GetForegroundStyle(STYLE_BACKGROUND_COLOR);
    if (tempColor.red == redNum &&
        tempColor.green == greenNum &&
        tempColor.blue == blueNum) {
        printf("[Test Case] [HorizonProgressTest009] PASSED\n");
    } else {
        printf("[Test Case] [HorizonProgressTest009] FAILED\n");
    }
    EXPECT_TRUE(tempColor.red == redNum);
    EXPECT_TRUE(tempColor.green == greenNum);
    EXPECT_TRUE(tempColor.blue == blueNum);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void HorizonProgressTddTest::HorizonProgressTest013()
{
    TDD_CASE_BEGIN();
    jerry_value_t typeId = jerry_create_string((const jerry_char_t*)"type");
    jerry_value_t typeValue = jerry_create_string((const jerry_char_t*)"horizontal");
    jerry_set_property(objAttrs, typeId, typeValue);
    jerry_release_value(typeId);
    jerry_release_value(typeValue);
    /**
     * @tc.steps: step1. setting color=0x80000000.
     */
    jerry_value_t colorKey = jerry_create_string((const jerry_char_t*)"color");
    // 2147483648(0x80000000)
    const int32_t percentNum = static_cast<int32_t>(2147483648);
    const int32_t redNum = 0x00;
    const int32_t greenNum = 0x00;
    const int32_t blueNum = 0x00;
    jerry_value_t colorValue = jerry_create_number(percentNum);
    jerry_set_property(objStaticStyle, colorKey, colorValue);

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("progress", strlen("progress"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    rootComponentMock_.RenderComponent(*component);

    /**
     * @tc.expected: step1. get color=0x000000.
     */
    UIBoxProgress* progressView = reinterpret_cast<UIBoxProgress *>(component->GetComponentRootView());
    ColorType tempColor;
    tempColor.full = progressView->GetForegroundStyle(STYLE_BACKGROUND_COLOR);
    if (tempColor.red == redNum &&
        tempColor.green == greenNum &&
        tempColor.blue == blueNum) {
        printf("[Test Case] [HorizonProgressTest013] PASSED\n");
    } else {
        printf("[Test Case] [HorizonProgressTest013] FAILED\n");
    }
    EXPECT_TRUE(tempColor.red == redNum);
    EXPECT_TRUE(tempColor.green == greenNum);
    EXPECT_TRUE(tempColor.blue == blueNum);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void HorizonProgressTddTest::HorizonProgressTest014()
{
    TDD_CASE_BEGIN();
    jerry_value_t typeId = jerry_create_string((const jerry_char_t*)"type");
    jerry_value_t typeValue = jerry_create_string((const jerry_char_t*)"horizontal");
    jerry_set_property(objAttrs, typeId, typeValue);
    jerry_release_value(typeId);
    jerry_release_value(typeValue);
    /**
     * @tc.steps: step1. setting strokeWidth=0.
     */
    jerry_value_t colorKey = jerry_create_string((const jerry_char_t*)"strokeWidth");
    const int32_t strokeWidth = 0;
    jerry_value_t colorValue = jerry_create_number(strokeWidth);
    jerry_set_property(objStaticStyle, colorKey, colorValue);

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("progress", strlen("progress"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    rootComponentMock_.RenderComponent(*component);
    /**
     * @tc.expected: step1. get strokeWidth=0.
     */
    UIBoxProgress* progressView = reinterpret_cast<UIBoxProgress *>(component->GetComponentRootView());
    if (static_cast<int32_t>(progressView->GetValidHeight()) == strokeWidth) {
        printf("[Test Case] [HorizonProgressTest014] PASSED\n");
    } else {
        printf("[Test Case] [HorizonProgressTest014] FAILED\n");
    }
    EXPECT_TRUE(progressView->GetValidHeight() == strokeWidth);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void HorizonProgressTddTest::HorizonProgressTest015()
{
    TDD_CASE_BEGIN();
    jerry_value_t typeId = jerry_create_string((const jerry_char_t*)"type");
    jerry_value_t typeValue = jerry_create_string((const jerry_char_t*)"horizontal");
    jerry_set_property(objAttrs, typeId, typeValue);
    jerry_release_value(typeId);
    jerry_release_value(typeValue);
    /**
     * @tc.steps: step1. setting strokeWidth=32767.
     */
    jerry_value_t colorKey = jerry_create_string((const jerry_char_t*)"strokeWidth");
    const int32_t strokeWidth = 32767;
    jerry_value_t colorValue = jerry_create_number(strokeWidth);
    jerry_set_property(objStaticStyle, colorKey, colorValue);

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("progress", strlen("progress"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    rootComponentMock_.RenderComponent(*component);
    /**
     * @tc.expected: step1. get strokeWidth=32767.
     */
    UIBoxProgress* progressView = reinterpret_cast<UIBoxProgress *>(component->GetComponentRootView());
    if (progressView->GetValidHeight() == strokeWidth) {
        printf("[Test Case] [HorizonProgressTest015] PASSED\n");
    } else {
        printf("[Test Case] [HorizonProgressTest015] FAILED\n");
    }
    EXPECT_TRUE(progressView->GetValidHeight() == strokeWidth);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void HorizonProgressTddTest::HorizonProgressTest016()
{
    TDD_CASE_BEGIN();
    jerry_value_t typeId = jerry_create_string((const jerry_char_t*)"type");
    jerry_value_t typeValue = jerry_create_string((const jerry_char_t*)"horizontal");
    jerry_set_property(objAttrs, typeId, typeValue);
    jerry_release_value(typeId);
    jerry_release_value(typeValue);
    /**
     * @tc.steps: step1. setting strokeWidth=32768.
     */
    jerry_value_t colorKey = jerry_create_string((const jerry_char_t*)"strokeWidth");
    const int32_t strokeWidth = 32768;
    jerry_value_t colorValue = jerry_create_number(strokeWidth);
    jerry_set_property(objStaticStyle, colorKey, colorValue);

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("progress", strlen("progress"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    rootComponentMock_.RenderComponent(*component);
    /**
     * @tc.expected: step1. progress component will not crash.
     */
    UIBoxProgress* progressView = reinterpret_cast<UIBoxProgress *>(component->GetComponentRootView());
    if (static_cast<int32_t>(progressView->GetValidHeight()) != strokeWidth) {
        printf("[Test Case] [HorizonProgressTest016] PASSED\n");
    } else {
        printf("[Test Case] [HorizonProgressTest016] FAILED\n");
    }
    EXPECT_TRUE(static_cast<int32_t>(progressView->GetValidHeight()) != strokeWidth);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void HorizonProgressTddTest::HorizonProgressTest017()
{
    TDD_CASE_BEGIN();
    jerry_value_t typeId = jerry_create_string((const jerry_char_t*)"type");
    jerry_value_t typeValue = jerry_create_string((const jerry_char_t*)"horizontal");
    jerry_set_property(objAttrs, typeId, typeValue);
    jerry_release_value(typeId);
    jerry_release_value(typeValue);
    /**
     * @tc.steps: step1. setting strokeWidth=-1.
     */
    jerry_value_t colorKey = jerry_create_string((const jerry_char_t*)"strokeWidth");
    const int32_t strokeWidth = -1;
    jerry_value_t colorValue = jerry_create_number(strokeWidth);
    jerry_set_property(objStaticStyle, colorKey, colorValue);

    jerry_value_t children = jerry_create_null();

    uint16_t componentNameId = KeyParser::ParseKeyId("progress", strlen("progress"));
    Component* component = ComponentFactory::CreateComponent(componentNameId, objGlob, children);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    rootComponentMock_.RenderComponent(*component);
    /**
     * @tc.expected: step1. get default strokeWidth=32.
     */
    UIBoxProgress* progressView = reinterpret_cast<UIBoxProgress *>(component->GetComponentRootView());
    if (progressView->GetValidHeight() == strokeWidth) {
        printf("[Test Case] [HorizonProgressTest017] PASSED\n");
    } else {
        printf("[Test Case] [HorizonProgressTest017] FAILED\n");
    }
    EXPECT_TRUE(progressView->GetValidHeight() == strokeWidth);
    component->Release();
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void HorizonProgressTddTest::RunTests()
{
    HorizonProgressTest001();
    HorizonProgressTest002();
    HorizonProgressTest003();
    HorizonProgressTest004();
    HorizonProgressTest005();
    HorizonProgressTest006();
    HorizonProgressTest007();
    HorizonProgressTest008();
    HorizonProgressTest009();
    HorizonProgressTest013();
    HorizonProgressTest014();
    HorizonProgressTest015();
    HorizonProgressTest016();
    HorizonProgressTest017();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: Component_HorizonProgress_Create_Test_001
 * @tc.desc: Verify progress component will restore the default progress type=horizontal if setting not exist type.
 */
HWTEST_F(HorizonProgressTddTest, horizon001, TestSize.Level1)
{
    HorizonProgressTddTest::HorizonProgressTest001();
}

/**
 * @tc.name: Component_HorizonProgress_Create_Test_002
 * @tc.desc: Verify progress component will restore the default progress type=horizontal if setting empty type.
 */
HWTEST_F(HorizonProgressTddTest, horizon002, TestSize.Level1)
{
    HorizonProgressTddTest::HorizonProgressTest002();
}

/**
 * @tc.name: Component_HorizonProgress_Create_Test_003
 * @tc.desc: Verify progress component setting normal progress attribute: type=arc successfully.
 */
HWTEST_F(HorizonProgressTddTest, horizon003, TestSize.Level1)
{
    HorizonProgressTddTest::HorizonProgressTest003();
}

/**
 * @tc.name: Component_HorizonProgress_Atrribute_Test_001
 * @tc.desc: Verify progress component will not crash when setting error attribute: percent=-32768.
 */
HWTEST_F(HorizonProgressTddTest, horizon004, TestSize.Level1)
{
    HorizonProgressTddTest::HorizonProgressTest004();
}

/**
 * @tc.name: Component_HorizonProgress_Atrribute_Test_002
 * @tc.desc: Verify progress component setting normal attribute: percent=100 successfully.
 */
HWTEST_F(HorizonProgressTddTest, horizon005, TestSize.Level0)
{
    HorizonProgressTddTest::HorizonProgressTest005();
}

/**
 * @tc.name: Component_HorizonProgress_Atrribute_Test_003
 * @tc.desc: Verify progress component will not crash when setting error attribute: percent=32767.
 */
HWTEST_F(HorizonProgressTddTest, horizon006, TestSize.Level1)
{
    HorizonProgressTddTest::HorizonProgressTest006();
}

/**
 * @tc.name: Component_HorizonProgress_Atrribute_Test_004
 * @tc.desc: Verify progress component will not crash when setting error attribute: percent=-32769.
 */
HWTEST_F(HorizonProgressTddTest, horizon007, TestSize.Level1)
{
    HorizonProgressTddTest::HorizonProgressTest007();
}

/**
 * @tc.name: Component_HorizonProgress_Atrribute_Test_005
 * @tc.desc: Verify progress component will not crash when setting error attribute: percent=32768.
 */
HWTEST_F(HorizonProgressTddTest, horizon008, TestSize.Level1)
{
    HorizonProgressTddTest::HorizonProgressTest008();
}

/**
 * @tc.name: Component_HorizonProgress_style_Test_001
 * @tc.desc: Verify progress component setting normal progress style: color=0x000000(0) successfully.
 */
HWTEST_F(HorizonProgressTddTest, horizon009, TestSize.Level1)
{
    HorizonProgressTddTest::HorizonProgressTest009();
}

/**
 * @tc.name: Component_HorizonProgress_style_Test_005
 * @tc.desc: Verify progress component will restore the default color=0x6B9AC7(7051975) if setting error value.
 */
HWTEST_F(HorizonProgressTddTest, horizon013, TestSize.Level0)
{
    HorizonProgressTddTest::HorizonProgressTest013();
}

/**
 * @tc.name: Component_HorizonProgress_style_Test_006
 * @tc.desc: Verify progress component setting normal progress style: strokeWidth=0 successfully.
 */
HWTEST_F(HorizonProgressTddTest, horizon014, TestSize.Level1)
{
    HorizonProgressTddTest::HorizonProgressTest014();
}

/**
 * @tc.name: Component_HorizonProgress_style_Test_007
 * @tc.desc: Verify progress component setting normal progress style: strokeWidth=32767 successfully.
 */
HWTEST_F(HorizonProgressTddTest, horizon015, TestSize.Level0)
{
    HorizonProgressTddTest::HorizonProgressTest015();
}

/**
 * @tc.name: Component_HorizonProgress_style_Test_008
 * @tc.desc: Verify progress component will not crash when setting error style: strokeWidth=32768.
 */
HWTEST_F(HorizonProgressTddTest, horizon016, TestSize.Level1)
{
    HorizonProgressTddTest::HorizonProgressTest016();
}

/**
 * @tc.name: Component_HorizonProgress_style_Test_009
 * @tc.desc: Verify progress component support setting error stroke width.
 */
HWTEST_F(HorizonProgressTddTest, horizon017, TestSize.Level1)
{
    HorizonProgressTddTest::HorizonProgressTest017();
}
#endif
} // ACELite
} // OHOS
