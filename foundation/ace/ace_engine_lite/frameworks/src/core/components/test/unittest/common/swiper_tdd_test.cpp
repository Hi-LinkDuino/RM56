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
#include "swiper_tdd_test.h"
#include "js_app_environment.h"
#include "root_view.h"
#include "stylemgr/app_style_item.h"
#include "stylemgr/app_style_manager.h"
#include "swiper_component.h"

namespace OHOS {
namespace ACELite {
SwiperTddTest::SwiperTddTest()
    : optionsObj_(UNDEFINED),
      attrsObj_(UNDEFINED),
      styleObj_(UNDEFINED),
      eventObj_(UNDEFINED),
      childrenObj_(UNDEFINED),
      stack1_(nullptr),
      stack2_(nullptr),
      stack3_(nullptr) {}

void SwiperTddTest::SetUp()
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
    ReleaseJerryValue(attrsKey, styleKey, eventKey, ret1, ret2, ret3, VA_ARG_END_FLAG);

    const int childrenNum = 3;
    childrenObj_ = jerry_create_array(childrenNum);

    jerry_value_t childOption = jerry_create_object();
    jerry_value_t type = jerry_create_string((const jerry_char_t*)"stack");
    ret1 = jerry_set_property_by_index(childOption, 0, type);
    stack1_ = ComponentFactory::CreateComponent(KeyParser::ParseKeyId("div"), childOption, jerry_create_null());
    stack1_->Render();
    ret2 = jerry_set_property_by_index(childrenObj_, 0, stack1_->GetNativeElement());
    ReleaseJerryValue(childOption, ret1, ret2, VA_ARG_END_FLAG);

    childOption = jerry_create_object();
    ret1 = jerry_set_property_by_index(childOption, 1, type);
    stack2_ = ComponentFactory::CreateComponent(KeyParser::ParseKeyId("div"), childOption, jerry_create_null());
    stack2_->Render();
    ret2 = jerry_set_property_by_index(childrenObj_, 1, stack2_->GetNativeElement());
    ReleaseJerryValue(childOption, ret1, ret2, VA_ARG_END_FLAG);

    childOption = jerry_create_object();
    const int pIndex = 2;
    ret1 = jerry_set_property_by_index(childOption, pIndex, type);
    stack3_ = ComponentFactory::CreateComponent(KeyParser::ParseKeyId("div"), childOption, jerry_create_null());
    stack3_->Render();
    ret2 = jerry_set_property_by_index(childrenObj_, pIndex, stack3_->GetNativeElement());
    ReleaseJerryValue(childOption, ret1, ret2, type, VA_ARG_END_FLAG);
    rootComponentMock_.PrepareRootContainer();
}


void SwiperTddTest::TearDown()
{
    ReleaseJerryValue(attrsObj_, styleObj_, eventObj_, optionsObj_, VA_ARG_END_FLAG);
    JsAppContext::GetInstance()->ReleaseStyles();
    stack1_->Release();
    stack2_->Release();
    stack3_->Release();
}

void SwiperTddTest::ComponentSwiperAttributeSetTest001()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set index 2
     */
    jerry_value_t indexKey = jerry_create_string((const jerry_char_t *)"index");
    const int val = 2;
    jerry_value_t indexVal = jerry_create_number(val);
    jerry_value_t ret1 = jerry_set_property(attrsObj_, indexKey, indexVal);
    Component* component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    rootComponentMock_.RenderComponent(*component);

    /**
     * @tc.steps: step2. get native swiper object
     */
    UIView* view = component->GetComponentRootView();
    UISwipeView* swiperView = reinterpret_cast<UISwipeView *>(view);
    const int pageIndex = 2;
    if (swiperView->GetCurrentPage() == pageIndex) {
        printf("ComponentSwiperAttributeSetTest001 pass\n");
    } else {
        printf("ComponentSwiperAttributeSetTest001 fail\n");
    }
    EXPECT_EQ(swiperView->GetCurrentPage(), pageIndex);
    ReleaseJerryValue(indexVal, ret1, VA_ARG_END_FLAG);

    /**
     * @tc.steps: step3. set index = '2'; get index = 2
     */
    indexVal = jerry_create_string((const jerry_char_t *)"2");
    component->UpdateView(KeyParser::ParseKeyId("index"), indexVal);
    view = component->GetComponentRootView();
    swiperView = reinterpret_cast<UISwipeView *>(view);
    if (swiperView->GetCurrentPage() == pageIndex) {
        printf("ComponentSwiperAttributeSetTest001 pass\n");
    } else {
        printf("ComponentSwiperAttributeSetTest001 fail\n");
    }
    EXPECT_EQ(swiperView->GetCurrentPage(), pageIndex);
    component->Release();
    ReleaseJerryValue(indexVal, ret1, VA_ARG_END_FLAG);

    /**
     * @tc.steps: step4. set index = 0; get index = 0
     */
    indexVal = jerry_create_number(0);
    ret1 = jerry_set_property(attrsObj_, indexKey, indexVal);
    component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    rootComponentMock_.RenderComponent(*component);
    view = component->GetComponentRootView();
    swiperView = reinterpret_cast<UISwipeView *>(view);
    if (swiperView->GetCurrentPage() == 0) {
        printf("ComponentSwiperAttributeSetTest001 pass\n");
    } else {
        printf("ComponentSwiperAttributeSetTest001 fail\n");
    }
    EXPECT_EQ(swiperView->GetCurrentPage(), 0);
    component->Release();
    ReleaseJerryValue(indexKey, indexVal, ret1, VA_ARG_END_FLAG);
    TDD_CASE_END();
}

void SwiperTddTest::ComponentSwiperAttributeSetTest002()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. dont set index value
     */
    Component* component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    rootComponentMock_.RenderComponent(*component);

    /**
     * @tc.steps: step2. get native swiper object
     */
    UIView* view = component->GetComponentRootView();
    UISwipeView* swiperView = reinterpret_cast<UISwipeView *>(view);

    /**
     * @tc.steps: step3. get index = 0
     */
    if (swiperView->GetCurrentPage() == 0) {
        printf("ComponentSwiperAttributeSetTest002 pass\n");
    } else {
        printf("ComponentSwiperAttributeSetTest002 fail\n");
    }
    EXPECT_EQ(swiperView->GetCurrentPage(), 0);
    component->Release();

    /**
     * @tc.steps: step4. set index -1
     */
    jerry_value_t indexKey = jerry_create_string((const jerry_char_t*)"index");
    jerry_value_t indexVal = jerry_create_string((const jerry_char_t*)"abc123");
    jerry_value_t ret1 = jerry_set_property(attrsObj_, indexKey, indexVal);
    component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    rootComponentMock_.RenderComponent(*component);
    view = component->GetComponentRootView();
    swiperView = reinterpret_cast<UISwipeView *>(view);
    if (swiperView->GetCurrentPage() == 0) {
        printf("ComponentSwiperAttributeSetTest002 pass\n");
    } else {
        printf("ComponentSwiperAttributeSetTest002 fail\n");
    }
    EXPECT_EQ(swiperView->GetCurrentPage(), 0);
    component->Release();

    ReleaseJerryValue(indexKey, indexVal, ret1, VA_ARG_END_FLAG);

    TDD_CASE_END();
}

void SwiperTddTest::ComponentSwiperAttributeSetTest003()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set index bool
     */
    jerry_value_t indexKey = jerry_create_string((const jerry_char_t*)"index");
    jerry_value_t indexVal = jerry_create_boolean(true);
    jerry_value_t ret1 = jerry_set_property(attrsObj_, indexKey, indexVal);
    Component* component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    rootComponentMock_.RenderComponent(*component);

    /**
     * @tc.steps: step2. get native swiper object
     */
    UIView* view = component->GetComponentRootView();
    UISwipeView* swiperView = reinterpret_cast<UISwipeView *>(view);

    /**
     * @tc.steps: step3. get index = 0
     */
    if (swiperView->GetCurrentPage() == 0) {
        printf("ComponentSwiperAttributeSetTest003 pass\n");
    } else {
        printf("ComponentSwiperAttributeSetTest003 fail\n");
    }
    EXPECT_EQ(swiperView->GetCurrentPage(), 0);
    component->Release();

    ReleaseJerryValue(indexVal, indexKey, ret1, VA_ARG_END_FLAG);

    TDD_CASE_END();
}

void SwiperTddTest::ComponentSwiperAttributeSetTest004()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set index 3
     */
    jerry_value_t indexKey = jerry_create_string((const jerry_char_t*)"index");
    const int val = 3;
    jerry_value_t indexVal = jerry_create_number(val);
    jerry_value_t ret1 = jerry_set_property(attrsObj_, indexKey, indexVal);
    Component* component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    rootComponentMock_.RenderComponent(*component);

    /**
     * @tc.steps: step2. get native swiper object
     */
    UIView* view = component->GetComponentRootView();
    UISwipeView* swiperView = reinterpret_cast<UISwipeView *>(view);

    /**
     * @tc.steps: step3. get index = 0
     */
    if (swiperView->GetCurrentPage() == 0) {
        printf("ComponentSwiperAttributeSetTest004 pass\n");
    } else {
        printf("ComponentSwiperAttributeSetTest004 fail\n");
    }
    EXPECT_EQ(swiperView->GetCurrentPage(), 0);
    component->Release();
    ReleaseJerryValue(indexVal, ret1, VA_ARG_END_FLAG);

    /**
     * @tc.steps: step4. set index -1
     */
    indexVal = jerry_create_number(-1);
    ret1 = jerry_set_property(attrsObj_, indexKey, indexVal);
    component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    rootComponentMock_.RenderComponent(*component);
    view = component->GetComponentRootView();
    swiperView = reinterpret_cast<UISwipeView *>(view);
    if (swiperView->GetCurrentPage() == 0) {
        printf("ComponentSwiperAttributeSetTest004 pass\n");
    } else {
        printf("ComponentSwiperAttributeSetTest004 fail\n");
    }
    EXPECT_EQ(swiperView->GetCurrentPage(), 0);
    component->Release();

    ReleaseJerryValue(indexKey, indexVal, ret1, VA_ARG_END_FLAG);

    TDD_CASE_END();
}

void SwiperTddTest::ComponentSwiperAttributeSetTest005()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set index -32769
     */
    jerry_value_t indexKey = jerry_create_string((const jerry_char_t*)"index");
    const int val1 = -32769;
    jerry_value_t indexVal = jerry_create_number(val1);
    jerry_value_t ret1 = jerry_set_property(attrsObj_, indexKey, indexVal);
    Component* component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    rootComponentMock_.RenderComponent(*component);

    /**
     * @tc.steps: step2. get native swiper object
     */
    UIView* view = component->GetComponentRootView();
    UISwipeView* swiperView = reinterpret_cast<UISwipeView *>(view);

    /**
     * @tc.steps: step3. get index = 0
     */
    if (swiperView->GetCurrentPage() == 0) {
        printf("ComponentSwiperAttributeSetTest005 pass\n");
    } else {
        printf("ComponentSwiperAttributeSetTest005 fail\n");
    }
    EXPECT_EQ(swiperView->GetCurrentPage(), 0);
    component->Release();
    ReleaseJerryValue(indexVal, ret1, VA_ARG_END_FLAG);

    /**
     * @tc.steps: step4. set index 32768 get index 0
     */
    const int val2 = 32768;
    indexVal = jerry_create_number(val2);
    ret1 = jerry_set_property(attrsObj_, indexKey, indexVal);
    component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    rootComponentMock_.RenderComponent(*component);
    view = component->GetComponentRootView();
    swiperView = reinterpret_cast<UISwipeView *>(view);
    if (swiperView->GetCurrentPage() == 0) {
        printf("ComponentSwiperAttributeSetTest005 pass\n");
    } else {
        printf("ComponentSwiperAttributeSetTest005 fail\n");
    }
    EXPECT_EQ(swiperView->GetCurrentPage(), 0);
    component->Release();

    ReleaseJerryValue(indexKey, indexVal, ret1, VA_ARG_END_FLAG);

    TDD_CASE_END();
}

void SwiperTddTest::ComponentSwiperAttributeSetTest013()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. do not set vertical
     */
    Component* component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    rootComponentMock_.RenderComponent(*component);

    /**
     * @tc.steps: step2. get native swiper object
     */
    UIView* view = component->GetComponentRootView();
    UISwipeView* swiperView = reinterpret_cast<UISwipeView *>(view);

    /**
     * @tc.steps: step3. get direction = 0
     */
    if (swiperView->GetDirection() == 0) {
        printf("ComponentSwiperAttributeSetTest013 pass\n");
    } else {
        printf("ComponentSwiperAttributeSetTest013 fail\n");
    }
    EXPECT_EQ(swiperView->GetDirection(), 0);
    component->Release();

    /**
     * @tc.steps: step4. set vert true get vert 1
     */
    jerry_value_t vertKey = jerry_create_string((const jerry_char_t*)"vertical");
    jerry_value_t vertVal = jerry_create_boolean(true);
    jerry_value_t ret1 = jerry_set_property(attrsObj_, vertKey, vertVal);
    component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    rootComponentMock_.RenderComponent(*component);
    view = component->GetComponentRootView();
    swiperView = reinterpret_cast<UISwipeView *>(view);
    if (swiperView->GetDirection() == 1) {
        printf("ComponentSwiperAttributeSetTest013 pass\n");
    } else {
        printf("ComponentSwiperAttributeSetTest013 fail\n");
    }
    EXPECT_EQ(swiperView->GetDirection(), 1);
    component->Release();
    ReleaseJerryValue(vertVal, ret1, VA_ARG_END_FLAG);

    /**
     * @tc.steps: step5. set vert true get vert 0
     */
    vertVal = jerry_create_boolean(false);
    ret1 = jerry_set_property(attrsObj_, vertKey, vertVal);
    component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    rootComponentMock_.RenderComponent(*component);
    view = component->GetComponentRootView();
    swiperView = reinterpret_cast<UISwipeView *>(view);
    if (swiperView->GetDirection() == 0) {
        printf("ComponentSwiperAttributeSetTest013 pass\n");
    } else {
        printf("ComponentSwiperAttributeSetTest013 fail\n");
    }
    EXPECT_EQ(swiperView->GetDirection(), 0);
    component->Release();

    ReleaseJerryValue(vertKey, vertVal, ret1, VA_ARG_END_FLAG);

    TDD_CASE_END();
}

void SwiperTddTest::ComponentSwiperAttributeSetTest014()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. do not set vertical
     */
    jerry_value_t vertKey = jerry_create_string((const jerry_char_t*)"vertical");
    const int val = 100;
    jerry_value_t vertVal = jerry_create_number(val);
    jerry_value_t ret1 = jerry_set_property(attrsObj_, vertKey, vertVal);
    Component* component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    rootComponentMock_.RenderComponent(*component);

    /**
     * @tc.steps: step2. get native swiper object
     */
    UIView* view = component->GetComponentRootView();
    UISwipeView* swiperView = reinterpret_cast<UISwipeView *>(view);

    /**
     * @tc.steps: step3. get direction = 0
     */
    if (swiperView->GetDirection() == 0) {
        printf("ComponentSwiperAttributeSetTest014 pass\n");
    } else {
        printf("ComponentSwiperAttributeSetTest014 fail\n");
    }
    EXPECT_EQ(swiperView->GetDirection(), 0);
    component->Release();

    ReleaseJerryValue(vertKey, vertVal, ret1, VA_ARG_END_FLAG);

    TDD_CASE_END();
}

void SwiperTddTest::ComponentSwiperAttributeSetTest019()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set id,visibility.
     */
    jerry_value_t idKey = jerry_create_string((const jerry_char_t*)"id");
    jerry_value_t idVal = jerry_create_string((const jerry_char_t*)"swiper");
    jerry_value_t ret1 = jerry_set_property(attrsObj_, idKey, idVal);
    jerry_value_t visibilityKey = jerry_create_string((const jerry_char_t*)"show");
    jerry_value_t visibilityVal = jerry_create_boolean(false);
    jerry_value_t ret2 = jerry_set_property(attrsObj_, visibilityKey, visibilityVal);
    Component* component = ComponentFactory::CreateComponent(componentKeyId_, optionsObj_, childrenObj_);
    rootComponentMock_.RenderComponent(*component);

    /**
     * @tc.steps: step2. get native swiper object
     */
    UIView* view = component->GetComponentRootView();
    UISwipeView* swiperView = reinterpret_cast<UISwipeView *>(view);

    /**
     * @tc.steps: step3. get attribute value
     */
    if (!strcmp(swiperView->GetViewId(), "swiper") && swiperView->IsVisible() == false) {
        printf("ComponentSwiperAttributeSetTest019 pass\n");
    } else {
        printf("ComponentSwiperAttributeSetTest019 fail\n");
    }
    EXPECT_TRUE((!strcmp(swiperView->GetViewId(), "swiper")) && (swiperView->IsVisible() == false));
    component->Release();

    ReleaseJerryValue(idKey, idVal, ret1, visibilityKey, visibilityVal, ret2, VA_ARG_END_FLAG);

    TDD_CASE_END();
}

void SwiperTddTest::RunTests()
{
    ComponentSwiperAttributeSetTest001();
    ComponentSwiperAttributeSetTest002();
    ComponentSwiperAttributeSetTest003();
    ComponentSwiperAttributeSetTest004();
    ComponentSwiperAttributeSetTest005();
    ComponentSwiperAttributeSetTest013();
    ComponentSwiperAttributeSetTest014();
    ComponentSwiperAttributeSetTest019();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: ComponentSwiperAttributeSetTest001
 * @tc.desc: Verify attribute index set normal value.
 */
HWTEST_F(SwiperTddTest, SwiperAttr001, TestSize.Level1)
{
    SwiperTddTest::ComponentSwiperAttributeSetTest001();
}

/**
 * @tc.name: ComponentSwiperAttributeSetTest002
 * @tc.desc: Verify attribute index set wrong string value.
 */
HWTEST_F(SwiperTddTest, SwiperAttr002, TestSize.Level1)
{
    SwiperTddTest::ComponentSwiperAttributeSetTest002();
}

/**
 * @tc.name: ComponentSwiperAttributeSetTest003
 * @tc.desc: Verify attribute index set bool value.
 */
HWTEST_F(SwiperTddTest, SwiperAttr003, TestSize.Level1)
{
    SwiperTddTest::ComponentSwiperAttributeSetTest003();
}

/**
 * @tc.name: ComponentSwiperAttributeSetTest004
 * @tc.desc: Verify attribute index out of size.
 */
HWTEST_F(SwiperTddTest, SwiperAttr004, TestSize.Level1)
{
    SwiperTddTest::ComponentSwiperAttributeSetTest004();
}

/**
 * @tc.name: ComponentSwiperAttributeSetTest005
 * @tc.desc: Verify attribute index out of int16.
 */
HWTEST_F(SwiperTddTest, SwiperAttr005, TestSize.Level1)
{
    SwiperTddTest::ComponentSwiperAttributeSetTest005();
}

/**
 * @tc.name: ComponentSwiperAttributeSetTest013
 * @tc.desc: Verify attribute vertical set correct.
 */
HWTEST_F(SwiperTddTest, SwiperAttr013, TestSize.Level1)
{
    SwiperTddTest::ComponentSwiperAttributeSetTest013();
}

/**
 * @tc.name: ComponentSwiperAttributeSetTest014
 * @tc.desc: Verify attribute vertical input wrong type.
 */
HWTEST_F(SwiperTddTest, SwiperAttr014, TestSize.Level1)
{
    SwiperTddTest::ComponentSwiperAttributeSetTest014();
}

/**
 * @tc.name: ComponentSwiperAttributeSetTest019
 * @tc.desc: Verify attribute id,visibility.
 */
HWTEST_F(SwiperTddTest, SwiperAttr019, TestSize.Level1)
{
    SwiperTddTest::ComponentSwiperAttributeSetTest019();
}
#endif
} // ACELite
} // OHOS
