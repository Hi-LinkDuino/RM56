/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "stack_tdd_test.h"
#include "js_app_context.h"
#include "js_debugger_config.h"
#include "stack_component.h"

namespace OHOS {
namespace ACELite {
StackTddTest::StackTddTest() : optionsObj_(UNDEFINED), attrsObj_(UNDEFINED), styleObj_(UNDEFINED)
{
    componentNameId_ = KeyParser::ParseKeyId("stack");
}

void StackTddTest::SetUp()
{
    Debugger::GetInstance().SetupJSContext();
    jerry_init(JERRY_INIT_EMPTY);

    optionsObj_ = jerry_get_global_object();
    attrsObj_ = jerry_create_object();
    JerrySetNamedProperty(optionsObj_, "attrs", attrsObj_);

    styleObj_ = jerry_create_object();
    JerrySetNamedProperty(optionsObj_, "staticStyle", styleObj_);
    rootComponentMock_.PrepareRootContainer();
}

void StackTddTest::TearDown()
{
    JsAppContext::GetInstance()->ReleaseStyles();
    ReleaseJerryValue(attrsObj_, styleObj_, optionsObj_, VA_ARG_END_FLAG);
    jerry_cleanup();
    Debugger::GetInstance().ReleaseJSContext();
}

Component *StackTddTest::GetRenderedComponent(uint16_t componentKeyId, jerry_value_t children) const
{
    Component *component = ComponentFactory::CreateComponent(componentKeyId, optionsObj_, children);
    rootComponentMock_.RenderComponent(*component);
    return component;
}

Component *StackTddTest::CreateChildComponent(jerry_value_t childOptions) const
{
    jerry_value_t children = jerry_create_null();
    Component *stack = ComponentFactory::CreateComponent(KeyParser::ParseKeyId("stack"), childOptions, children);
    stack->Render();
    jerry_release_value(children);
    return stack;
}

void StackTddTest::ReleaseComponent(Component *&component) const
{
    if (component != nullptr) {
        component->Release();
        delete component;
        component = nullptr;
    }
}

void StackTddTest::ComponentStackTest001()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set style
     */
    jerry_value_t children = jerry_create_null();
    Component *component = GetRenderedComponent(componentNameId_, children);
    EXPECT_TRUE(component != nullptr);
    CASE_END_IF_NULLPTR(component);
    UIViewGroup *stackView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step1. check childView = nullptr
     */
    EXPECT_TRUE((stackView != nullptr) && (stackView->GetChildrenHead() == nullptr));
    ReleaseComponent(component);
    jerry_release_value(children);
    TDD_CASE_END();
}

void StackTddTest::ComponentStackTest002()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. create child component
     */
    jerry_value_t childOptions = jerry_create_null();
    Component *child = CreateChildComponent(childOptions);
    EXPECT_TRUE(child != nullptr);
    CASE_END_IF_NULLPTR(child);
    /* *
     * @tc.steps: step2. set children object
     */
    const uint32_t childrenNum = 1;
    jerry_value_t children = jerry_create_array(childrenNum);
    jerry_release_value(jerry_set_property_by_index(children, 0, child->GetNativeElement()));
    /* *
     * @tc.steps: step3. create stack component
     */
    Component *component = GetRenderedComponent(componentNameId_, children);
    EXPECT_TRUE(component != nullptr);
    CASE_END_IF_NULLPTR(component);
    UIViewGroup *stackView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step3. check stackView != nullptr &&  childView != nullptr
     */
    EXPECT_TRUE((stackView != nullptr) && (stackView->GetChildrenHead() != nullptr));
    ReleaseComponent(child);
    ReleaseComponent(component);
    ReleaseJerryValue(childOptions, children, VA_ARG_END_FLAG);
    TDD_CASE_END();
}

void StackTddTest::ComponentStackTest003()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. create two children components
     */
    jerry_value_t childOptions = jerry_create_null();
    Component *child1 = CreateChildComponent(childOptions);
    EXPECT_TRUE(child1 != nullptr);
    CASE_END_IF_NULLPTR(child1);
    Component *child2 = CreateChildComponent(childOptions);
    EXPECT_TRUE(child2 != nullptr);
    CASE_END_IF_NULLPTR(child2);
    /* *
     * @tc.steps: step2. set children object
     */
    const uint32_t childrenNum = 2;
    jerry_value_t children = jerry_create_array(childrenNum);
    jerry_release_value(jerry_set_property_by_index(children, 0, child1->GetNativeElement()));
    jerry_release_value(jerry_set_property_by_index(children, 1, child2->GetNativeElement()));
    /* *
     * @tc.steps: step3. create stack component
     */
    Component *component = GetRenderedComponent(componentNameId_, children);
    EXPECT_TRUE(component != nullptr);
    CASE_END_IF_NULLPTR(component);
    UIViewGroup *stackView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    EXPECT_TRUE(stackView != nullptr);
    CASE_END_IF_NULLPTR(stackView);
    /* *
     * @tc.steps: step4. get children views
     */
    UIView *childView1 = stackView->GetChildrenHead();
    EXPECT_TRUE(childView1 != nullptr);
    CASE_END_IF_NULLPTR(childView1);
    UIView *childView2 = childView1->GetNextSibling();
    /* *
     * @tc.expected: step4. check two children views
     */
    EXPECT_TRUE((childView2 != nullptr) && (childView1 != childView2));

    ReleaseComponent(child1);
    ReleaseComponent(child2);
    ReleaseComponent(component);
    ReleaseJerryValue(childOptions, children, VA_ARG_END_FLAG);
    TDD_CASE_END();
}

void StackTddTest::ComponentStackTest004()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. create child attributes object
     */
    jerry_value_t childAttrsObj = jerry_create_object();
    JerrySetStringProperty(childAttrsObj, "id", "child01");
    jerry_value_t childOptions = jerry_create_object();
    JerrySetNamedProperty(childOptions, "attrs", childAttrsObj);
    Component *child = CreateChildComponent(childOptions);
    EXPECT_TRUE(child != nullptr);
    CASE_END_IF_NULLPTR(child);
    /* *
     * @tc.steps: step2. set child to children object
     */
    const uint32_t childrenNum = 1;
    jerry_value_t children = jerry_create_array(childrenNum);
    jerry_release_value(jerry_set_property_by_index(children, 0, child->GetNativeElement()));
    /* *
     * @tc.steps: step3. create stack component
     */
    Component *component = GetRenderedComponent(componentNameId_, children);
    EXPECT_TRUE(component != nullptr);
    CASE_END_IF_NULLPTR(component);
    UIViewGroup *stackView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    EXPECT_TRUE(stackView != nullptr);
    CASE_END_IF_NULLPTR(stackView);
    /* *
     * @tc.steps: step4. get child view
     */
    UIView *childView = stackView->GetChildrenHead();
    EXPECT_TRUE(childView != nullptr);
    CASE_END_IF_NULLPTR(childView);
    /* *
     * @tc.expected: step4. check the child view attribute
     */
    EXPECT_STREQ(childView->GetViewId(), "child01");

    ReleaseComponent(child);
    ReleaseComponent(component);
    ReleaseJerryValue(childAttrsObj, childOptions, children, VA_ARG_END_FLAG);
    TDD_CASE_END();
}

void StackTddTest::ComponentStackTest005()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. create child styles object, left = 5, top = 10
     */
    jerry_value_t childStylesObj = jerry_create_object();
    int16_t left = 5;
    int16_t top = 10;
    JerrySetNumberProperty(childStylesObj, "left", left);
    JerrySetNumberProperty(childStylesObj, "top", top);
    jerry_value_t childOptions = jerry_create_object();
    JerrySetNamedProperty(childOptions, "staticStyle", childStylesObj);
    Component *child = CreateChildComponent(childOptions);
    EXPECT_TRUE(child != nullptr);
    CASE_END_IF_NULLPTR(child);
    /* *
     * @tc.steps: step2. set child to children object
     */
    const uint32_t childrenNum = 1;
    jerry_value_t children = jerry_create_array(childrenNum);
    jerry_release_value(jerry_set_property_by_index(children, 0, child->GetNativeElement()));
    /* *
     * @tc.steps: step3. create stack component
     */
    Component *component = GetRenderedComponent(componentNameId_, children);
    EXPECT_TRUE(component != nullptr);
    CASE_END_IF_NULLPTR(component);
    UIViewGroup *stackView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    EXPECT_TRUE(stackView != nullptr);
    CASE_END_IF_NULLPTR(stackView);
    /* *
     * @tc.steps: step4. get child view
     */
    UIView *childView = stackView->GetChildrenHead();
    EXPECT_TRUE(childView != nullptr);
    CASE_END_IF_NULLPTR(childView);
    /* *
     * @tc.expected: step4. check the child view styles
     */
    EXPECT_TRUE((childView->GetX() == left) && (childView->GetY() == top));

    ReleaseComponent(child);
    ReleaseComponent(component);
    ReleaseJerryValue(childStylesObj, childOptions, children, VA_ARG_END_FLAG);
    TDD_CASE_END();
}

void StackTddTest::ComponentStackTest006()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. create child styles object, width = 100, height = 50
     */
    jerry_value_t childStylesObj = jerry_create_object();
    int16_t width = 100;
    int16_t height = 50;
    JerrySetNumberProperty(childStylesObj, "width", width);
    JerrySetNumberProperty(childStylesObj, "height", height);
    jerry_value_t childOptions = jerry_create_object();
    JerrySetNamedProperty(childOptions, "staticStyle", childStylesObj);
    Component *child = CreateChildComponent(childOptions);
    EXPECT_TRUE(child != nullptr);
    CASE_END_IF_NULLPTR(child);
    /* *
     * @tc.steps: step2. set child to children object
     */
    const uint32_t childrenNum = 1;
    jerry_value_t children = jerry_create_array(childrenNum);
    jerry_release_value(jerry_set_property_by_index(children, 0, child->GetNativeElement()));
    /* *
     * @tc.steps: step3. create stack component
     */
    Component *component = GetRenderedComponent(componentNameId_, children);
    EXPECT_TRUE(component != nullptr);
    CASE_END_IF_NULLPTR(component);
    UIViewGroup *stackView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    EXPECT_TRUE(stackView != nullptr);
    CASE_END_IF_NULLPTR(stackView);
    /* *
     * @tc.steps: step4. get child view
     */
    UIView *childView = stackView->GetChildrenHead();
    EXPECT_TRUE(childView != nullptr);
    CASE_END_IF_NULLPTR(childView);
    /* *
     * @tc.expected: step4. check the child view styles
     */
    EXPECT_TRUE((childView->GetWidth() == width) && (childView->GetHeight() == height));

    ReleaseComponent(child);
    ReleaseComponent(component);
    ReleaseJerryValue(childStylesObj, childOptions, children, VA_ARG_END_FLAG);
    TDD_CASE_END();
}

void StackTddTest::RunTests()
{
    ComponentStackTest001();
    ComponentStackTest002();
    ComponentStackTest003();
    ComponentStackTest004();
    ComponentStackTest005();
    ComponentStackTest006();
}

#ifdef TDD_ASSERTIONS
/* *
 * @tc.name: ComponentStackTest001
 * @tc.desc: Verify no child component set.
 */
HWTEST_F(StackTddTest, test001, TestSize.Level1)
{
    StackTddTest::ComponentStackTest001();
}

/* *
 * @tc.name: ComponentStackTest002
 * @tc.desc: Verify child component can be set.
 */
HWTEST_F(StackTddTest, test002, TestSize.Level0)
{
    StackTddTest::ComponentStackTest002();
}

/* *
 * @tc.name: ComponentStackTest003
 * @tc.desc: Verify multiple children components can be set.
 */
HWTEST_F(StackTddTest, test003, TestSize.Level0)
{
    StackTddTest::ComponentStackTest003();
}

/* *
 * @tc.name: ComponentStackTest004
 * @tc.desc: Verify attribute of child component can be set correctly.
 */
HWTEST_F(StackTddTest, test004, TestSize.Level1)
{
    StackTddTest::ComponentStackTest004();
}

/* *
 * @tc.name: ComponentStackTest005
 * @tc.desc: Verify left and top styles of child component can be set correctly.
 */
HWTEST_F(StackTddTest, test005, TestSize.Level1)
{
    StackTddTest::ComponentStackTest005();
}

/* *
 * @tc.name: ComponentStackTest006
 * @tc.desc: Verify width and height styles of child component can be set correctly.
 */
HWTEST_F(StackTddTest, test006, TestSize.Level1)
{
    StackTddTest::ComponentStackTest006();
}
#endif
} // namespace ACELite
} // namespace OHOS
