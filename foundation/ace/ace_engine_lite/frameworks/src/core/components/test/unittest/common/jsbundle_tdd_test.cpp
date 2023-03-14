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

#include "jsbundle_tdd_test.h"
#include "component.h"
#include "component_utils.h"
#include "js_app_context.h"
#include "js_app_environment.h"
#include "js_fwk_common.h"
#include "ui_label.h"
#include "ui_view_group.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {
JsbundleTddTest::JsbundleTddTest() {}
constexpr char FUNC_CREATE_ELEMENT[] = "_c";

constexpr char CLASS_FLEXLAYOUT[] = "N4OHOS10FlexLayoutE";
constexpr char CLASS_LABEL[] = "N4OHOS7UILabelE";

constexpr char TAG_DIV[] = "div";
constexpr char TAG_TEXT[] = "text";
constexpr char ARG_ATTRS[] = "attrs";
constexpr char ARG_STYLES[] = "staticStyle";
constexpr char ARG_VALUE[] = "value";
constexpr char ARG_WIDTH[] = "width";
constexpr char ARG_HEIGHT[] = "height";
constexpr char HELLO_MESSAGE[] = "message";

constexpr uint8_t ARG_LENGTH_2 = 2;
constexpr uint8_t ARG_LENGTH_3 = 3;
constexpr uint8_t ATTR_VAL_WIDTH = 100;
constexpr uint8_t ATTR_VAL_HEIGHT = 100;
void JsbundleTddTest::SetUp()
{
    JsAppEnvironment *env = JsAppEnvironment::GetInstance();
    env->InitJsFramework();
}

void JsbundleTddTest::TearDown()
{
    JsAppContext::GetInstance()->ReleaseStyles();
    JsAppEnvironment *env = JsAppEnvironment::GetInstance();
    env->Cleanup();
}

void JsbundleTddTest::CreateElementAttributeLength001()
{
    JSValue tag = JSString::Create(TAG_DIV);
    JSValue args[1] = {tag};
    JSValue el = JSGlobal::Call(FUNC_CREATE_ELEMENT, args, 1);
    if (JSUndefined::Is(el) || jerry_value_is_error(el)) {
        printf("CreateElementAttributeLength001 fail\n");
        EXPECT_FALSE(JSUndefined::Is(el) || jerry_value_is_error(el));
    } else {
        UIViewGroup *viewGroup = reinterpret_cast<UIViewGroup *>(ComponentUtils::GetViewFromBindingObject(el));
        if (viewGroup == nullptr || strcmp(CLASS_FLEXLAYOUT, typeid(*viewGroup).name())) {
            printf("CreateElementAttributeLength001 fail\n");
        } else {
            printf("CreateElementAttributeLength001 pass\n");
        }
        EXPECT_FALSE(viewGroup == nullptr || strcmp(CLASS_FLEXLAYOUT, typeid(*viewGroup).name()));
    }
    ReleaseJerryValue(el, tag, VA_ARG_END_FLAG);
}

void JsbundleTddTest::CreateElementAttributeLength002()
{
    const char *message = HELLO_MESSAGE;
    JSValue tag = JSString::Create(TAG_TEXT);
    JSValue options = JSObject::Create();
    JSValue attrs = JSObject::Create();
    JSValue value = JSString::Create(message);
    JSObject::Set(attrs, ARG_VALUE, value);
    JSObject::Set(options, ARG_ATTRS, attrs);
    JSValue args[ARG_LENGTH_2] = {tag, options};
    JSValue el = JSGlobal::Call(FUNC_CREATE_ELEMENT, args, ARG_LENGTH_2);
    if (JSUndefined::Is(el) || jerry_value_is_error(el)) {
        printf("CreateElementAttributeLength002 fail\n");
        EXPECT_FALSE(JSUndefined::Is(el) || jerry_value_is_error(el));
    } else {
        Component *textComponent = ComponentUtils::GetComponentFromBindingObject(el);
        if (textComponent != nullptr) {
            textComponent->OnViewAttached();
        }
        UILabel *label = reinterpret_cast<UILabel *>(ComponentUtils::GetViewFromBindingObject(el));
        if (label == nullptr || strcmp(CLASS_LABEL, typeid(*label).name()) || strcmp(message, label->GetText())) {
            printf("CreateElementAttributeLength002 fail\n");
        } else {
            printf("CreateElementAttributeLength002 pass\n");
        }
        EXPECT_FALSE(label == nullptr || strcmp(CLASS_LABEL, typeid(*label).name()) ||
                     (label->GetText() != nullptr && strcmp(message, label->GetText())));
    }
    ReleaseJerryValue(el, options, attrs, value, tag, VA_ARG_END_FLAG);
}

void JsbundleTddTest::CreateElementAttributeLength003()
{
    const char *message = HELLO_MESSAGE;
    JSValue textTag = JSString::Create(TAG_TEXT);
    JSValue textOptions = JSObject::Create();
    JSValue textAttrs = JSObject::Create();
    JSValue textValue = JSString::Create(message);
    JSObject::Set(textAttrs, ARG_VALUE, textValue);
    JSObject::Set(textOptions, ARG_ATTRS, textAttrs);
    JSValue textArgs[ARG_LENGTH_2] = {textTag, textOptions};
    JSValue textEl = JSGlobal::Call(FUNC_CREATE_ELEMENT, textArgs, ARG_LENGTH_2);

    JSValue divTag = JSString::Create(TAG_DIV);
    JSValue divChildren = JSArray::Create(0);
    JSArray::Push(divChildren, textEl);
    JSValue divArgs[ARG_LENGTH_2] = {divTag, divChildren};
    JSValue divEl = JSGlobal::Call(FUNC_CREATE_ELEMENT, divArgs, ARG_LENGTH_2);
    if (JSUndefined::Is(divEl) || jerry_value_is_error(divEl)) {
        printf("CreateElementAttributeLength003 fail\n");
        EXPECT_FALSE(JSUndefined::Is(divEl) || jerry_value_is_error(divEl));
    } else {
        Component *textComponent = ComponentUtils::GetComponentFromBindingObject(textEl);
        Component *divComponent = ComponentUtils::GetComponentFromBindingObject(divEl);
        UIViewGroup *viewGroup = reinterpret_cast<UIViewGroup *>(ComponentUtils::GetViewFromBindingObject(divEl));
        if (textComponent == nullptr || divComponent == nullptr ||
            viewGroup == nullptr || strcmp(CLASS_FLEXLAYOUT, typeid(*viewGroup).name())) {
            printf("CreateElementAttributeLength003 fail\n");
            EXPECT_FALSE(viewGroup == nullptr || strcmp(CLASS_FLEXLAYOUT, typeid(*viewGroup).name()));
        } else {
            ConstrainedParameter divContentParameter = {ATTR_VAL_WIDTH, ATTR_VAL_HEIGHT};
            Component::BuildViewTree(textComponent, divComponent, divContentParameter);
            UILabel *label = reinterpret_cast<UILabel *>(viewGroup->GetChildrenHead());
            EXPECT_FALSE(label == nullptr || strcmp(CLASS_LABEL, typeid(*label).name()) ||
                         (label->GetText() != nullptr && strcmp(message, label->GetText())));
        }
    }
    ReleaseJerryValue(divEl, divChildren, divTag, textEl, textOptions, textAttrs, textValue, textTag, VA_ARG_END_FLAG);
}

void JsbundleTddTest::CreateElementAttributeLength004()
{
    const char *message = HELLO_MESSAGE;
    JSValue textTag = JSString::Create(TAG_TEXT);
    JSValue textOptions = JSObject::Create();
    JSValue textAttrs = JSObject::Create();
    JSValue textValue = JSString::Create(message);
    JSObject::Set(textAttrs, ARG_VALUE, textValue);
    JSObject::Set(textOptions, ARG_ATTRS, textAttrs);
    JSValue textArgs[ARG_LENGTH_2] = {textTag, textOptions};
    JSValue textEl = JSGlobal::Call(FUNC_CREATE_ELEMENT, textArgs, ARG_LENGTH_2);

    JSValue divTag = JSString::Create(TAG_DIV);
    JSValue divOptions = JSObject::Create();
    JSValue divStyles = JSObject::Create();
    JSValue divWidth = JSNumber::Create(ATTR_VAL_WIDTH);
    JSValue divHeight = JSNumber::Create(ATTR_VAL_HEIGHT);
    JSObject::Set(divStyles, ARG_WIDTH, divWidth);
    JSObject::Set(divStyles, ARG_HEIGHT, divHeight);
    JSObject::Set(divOptions, ARG_STYLES, divStyles);
    JSValue divChildren = JSArray::Create(0);
    JSArray::Push(divChildren, textEl);
    JSValue divArgs[ARG_LENGTH_3] = {divTag, divOptions, divChildren};
    JSValue divEl = JSGlobal::Call(FUNC_CREATE_ELEMENT, divArgs, ARG_LENGTH_3);
    if (JSUndefined::Is(divEl) || jerry_value_is_error(divEl)) {
        printf("CreateElementAttributeLength004 fail\n");
        EXPECT_FALSE(JSUndefined::Is(divEl) || jerry_value_is_error(divEl));
    } else {
        Component *textComponent = ComponentUtils::GetComponentFromBindingObject(textEl);
        Component *divComponent = ComponentUtils::GetComponentFromBindingObject(divEl);
        UIViewGroup *viewGroup = reinterpret_cast<UIViewGroup *>(ComponentUtils::GetViewFromBindingObject(divEl));
        if (textComponent == nullptr || divComponent == nullptr ||
            viewGroup == nullptr || strcmp(CLASS_FLEXLAYOUT, typeid(*viewGroup).name())) {
            printf("CreateElementAttributeLength004 fail\n");
            EXPECT_FALSE(viewGroup == nullptr || strcmp(CLASS_FLEXLAYOUT, typeid(*viewGroup).name()));
        } else {
            ConstrainedParameter divContentParameter = {ATTR_VAL_WIDTH, ATTR_VAL_HEIGHT};
            Component::BuildViewTree(textComponent, divComponent, divContentParameter);
            UILabel *label = reinterpret_cast<UILabel *>(viewGroup->GetChildrenHead());
            if (label == nullptr || strcmp(CLASS_LABEL, typeid(*label).name()) || strcmp(message, label->GetText())) {
                printf("CreateElementAttributeLength004 fail\n");
            } else {
                printf("CreateElementAttributeLength004 pass\n");
            }
            EXPECT_FALSE(label == nullptr || strcmp(CLASS_LABEL, typeid(*label).name()) ||
                         ((label->GetText() != nullptr) && (strcmp(message, label->GetText()))));
        }
    }
    ReleaseJerryValue(divEl, divChildren, divOptions, divStyles, divWidth, divHeight, divTag, textEl, textOptions,
                      textAttrs, textValue, textTag, VA_ARG_END_FLAG);
}

void JsbundleTddTest::CreateElementTagValidation005()
{
    JSValue tag = JSNumber::Create(1);
    JSValue args[1] = {tag};
    JSValue el = JSGlobal::Call(FUNC_CREATE_ELEMENT, args, 1);
    if (!JSUndefined::Is(el)) {
        printf("CreateElementTagValidation005 fail\n");
    } else {
        printf("CreateElementTagValidation005 pass\n");
    }
    EXPECT_TRUE(JSUndefined::Is(el));
    ReleaseJerryValue(el, tag, VA_ARG_END_FLAG);
}

void JsbundleTddTest::CreateElementTagValidation006()
{
    JSValue tag = JSString::Create("customized-component");
    JSValue args[1] = {tag};
    JSValue el = JSGlobal::Call(FUNC_CREATE_ELEMENT, args, 1);
    if (!JSUndefined::Is(el)) {
        printf("CreateElementTagValidation006 fail\n");
    } else {
        printf("CreateElementTagValidation006 pass\n");
    }
    EXPECT_TRUE(JSUndefined::Is(el));
    ReleaseJerryValue(el, tag, VA_ARG_END_FLAG);
}

void JsbundleTddTest::CreateElementTagValidation007()
{
    JSValue tag = JSString::Create(TAG_DIV);
    JSValue args[1] = {tag};
    JSValue el = JSGlobal::Call(FUNC_CREATE_ELEMENT, args, 1);
    if (JSUndefined::Is(el)) {
        printf("CreateElementTagValidation007 fail\n");
    } else {
        printf("CreateElementTagValidation007 pass\n");
    }
    EXPECT_FALSE(JSUndefined::Is(el));
    ReleaseJerryValue(el, tag, VA_ARG_END_FLAG);
}

void JsbundleTddTest::CreateElement2ndAttributeValidation008()
{
    JSValue tag = JSString::Create(TAG_TEXT);
    JSValue invalidOptions = JSNumber::Create(1);
    JSValue args[ARG_LENGTH_2] = {tag, invalidOptions};
    JSValue el = JSGlobal::Call(FUNC_CREATE_ELEMENT, args, ARG_LENGTH_2);
    if (JSUndefined::Is(el)) {
        printf("CreateElement2ndAttributeValidation008 fail\n");
    } else {
        printf("CreateElement2ndAttributeValidation008 pass\n");
    }
    EXPECT_FALSE(JSUndefined::Is(el));
    ReleaseJerryValue(el, invalidOptions, tag, VA_ARG_END_FLAG);
}

void JsbundleTddTest::CreateElement2ndAttributeValidation009()
{
    JSValue tag = JSString::Create(TAG_TEXT);
    JSValue options = JSObject::Create();
    JSValue attrs = JSObject::Create();
    JSValue value = JSString::Create(HELLO_MESSAGE);
    JSObject::Set(attrs, ARG_VALUE, value);
    JSValue args[ARG_LENGTH_2] = {tag, options};
    JSValue el = JSGlobal::Call(FUNC_CREATE_ELEMENT, args, ARG_LENGTH_2);
    if (JSUndefined::Is(el)) {
        printf("CreateElement2ndAttributeValidation009 fail\n");
    } else {
        printf("CreateElement2ndAttributeValidation009 pass\n");
    }
    EXPECT_FALSE(JSUndefined::Is(el));
    ReleaseJerryValue(el, options, attrs, value, tag, VA_ARG_END_FLAG);
}

void JsbundleTddTest::CreateElement2ndAttributeValidation010()
{
    JSValue tag = JSString::Create(TAG_TEXT);
    JSValue children = JSArray::Create(0);
    JSValue args[ARG_LENGTH_2] = {tag, children};
    JSValue el = JSGlobal::Call(FUNC_CREATE_ELEMENT, args, ARG_LENGTH_2);
    if (JSUndefined::Is(el)) {
        printf("CreateElement2ndAttributeValidation010 fail\n");
    } else {
        printf("CreateElement2ndAttributeValidation010 pass\n");
    }
    EXPECT_FALSE(JSUndefined::Is(el));
    ReleaseJerryValue(el, children, tag, VA_ARG_END_FLAG);
}

void JsbundleTddTest::CreateElement2ndAttributeValidation011()
{
    JSValue textTag = JSString::Create(TAG_TEXT);
    JSValue textArgs[1] = {textTag};
    JSValue textEl = JSGlobal::Call(FUNC_CREATE_ELEMENT, textArgs, 1);

    JSValue divTag = JSString::Create(TAG_DIV);
    JSValue divChildren = JSArray::Create(0);
    JSArray::Push(divChildren, textEl);
    JSValue divArgs[ARG_LENGTH_2] = {divTag, divChildren};
    JSValue divEl = JSGlobal::Call(FUNC_CREATE_ELEMENT, divArgs, ARG_LENGTH_2);
    if (JSUndefined::Is(divEl)) {
        printf("CreateElement2ndAttributeValidation011 fail\n");
    } else {
        printf("CreateElement2ndAttributeValidation011 pass\n");
    }
    EXPECT_FALSE(JSUndefined::Is(divEl));
    ReleaseJerryValue(divEl, divChildren, divTag, textEl, textTag, VA_ARG_END_FLAG);
}

void JsbundleTddTest::CreateElementChildrenValidation012()
{
    JSValue tag = JSString::Create(TAG_DIV);
    JSValue options = JSObject::Create();
    JSValue children = JSArray::Create(0);
    JSValue args[ARG_LENGTH_3] = {tag, options, children};

    JSValue el = JSGlobal::Call(FUNC_CREATE_ELEMENT, args, ARG_LENGTH_3);
    if (JSUndefined::Is(el)) {
        printf("CreateElementChildrenValidation012 fail\n");
    } else {
        printf("CreateElementChildrenValidation012 pass\n");
    }
    EXPECT_FALSE(JSUndefined::Is(el));
    ReleaseJerryValue(el, children, options, tag, VA_ARG_END_FLAG);
}

void JsbundleTddTest::CreateElementChildrenValidation013()
{
    JSValue tag = JSString::Create(TAG_DIV);
    JSValue options = JSObject::Create();
    JSValue children = JSArray::Create(0);
    JSValue number = JSNumber::Create(1);
    JSArray::Push(children, number);
    JSValue args[ARG_LENGTH_3] = {tag, options, children};
    JSValue el = JSGlobal::Call(FUNC_CREATE_ELEMENT, args, ARG_LENGTH_3);
    if (JSUndefined::Is(el)) {
        printf("CreateElementChildrenValidation013 fail\n");
    } else {
        printf("CreateElementChildrenValidation013 pass\n");
    }
    EXPECT_FALSE(JSUndefined::Is(el));
    ReleaseJerryValue(el, children, number, options, tag, VA_ARG_END_FLAG);
}

void JsbundleTddTest::RunTests()
{
    TDD_CASE_BEGIN();
    CreateElementAttributeLength001();
    CreateElementAttributeLength002();
    CreateElementAttributeLength003();
    CreateElementAttributeLength004();
    CreateElementTagValidation005();
    CreateElementTagValidation006();
    CreateElementTagValidation007();
    CreateElement2ndAttributeValidation008();
    CreateElement2ndAttributeValidation009();
    CreateElement2ndAttributeValidation010();
    CreateElement2ndAttributeValidation011();
    CreateElementChildrenValidation012();
    CreateElementChildrenValidation013();
    TDD_CASE_END();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: CreateElement_Attribute_Length_001
 * @tc.desc: Verify the _c function support analysising UIView attribute.
 */
HWTEST_F(JsbundleTddTest, attrlen001, TestSize.Level1)
{
    JsbundleTddTest::CreateElementAttributeLength001();
}

/**
 * @tc.name: CreateElement_Attribute_Length_002
 * @tc.desc: Verify the _c function support analysising UILabel attribute.
 */
HWTEST_F(JsbundleTddTest, attrlen002, TestSize.Level1)
{
    JsbundleTddTest::CreateElementAttributeLength002();
}

/**
 * @tc.name: CreateElement_Attribute_Length_003
 * @tc.desc: Verify the _c function support analysising simple UIViewGroup attribute.
 */
HWTEST_F(JsbundleTddTest, attrlen003, TestSize.Level1)
{
    JsbundleTddTest::CreateElementAttributeLength003();
}

/**
 * @tc.name: CreateElement_Attribute_Length_004
 * @tc.desc: Verify the _c function support analysising complex UIViewGroup attribute.
 */
HWTEST_F(JsbundleTddTest, attrlen004, TestSize.Level0)
{
    JsbundleTddTest::CreateElementAttributeLength004();
}

/**
 * @tc.name: CreateElement_Tag_Validation_005
 * @tc.desc: Verify the _c function support handling error parameter(-1).
 */
HWTEST_F(JsbundleTddTest, tagViali005, TestSize.Level1)
{
    JsbundleTddTest::CreateElementTagValidation005();
}

/**
 * @tc.name: CreateElement_Tag_Validation_006
 * @tc.desc: Verify the _c function support handling error parameter('customized-component').
 */
HWTEST_F(JsbundleTddTest, tagViali006, TestSize.Level1)
{
    JsbundleTddTest::CreateElementTagValidation006();
}

/**
 * @tc.name: CreateElement_Tag_Validation_007
 * @tc.desc: Verify the _c function support handling legal parameter('div').
 */
HWTEST_F(JsbundleTddTest, tagViali007, TestSize.Level1)
{
    JsbundleTddTest::CreateElementTagValidation007();
}

/**
 * @tc.name: CreateElement_2ndAttribute_Validation_008
 * @tc.desc: Verify the _c function support handling legal parameter('text', 1).
 */
HWTEST_F(JsbundleTddTest, attrVali008, TestSize.Level1)
{
    JsbundleTddTest::CreateElement2ndAttributeValidation008();
}

/**
 * @tc.name: CreateElement_2ndAttribute_Validation_009
 * @tc.desc: Verify the _c function support handling legal parameter('text', { attrs: { value: 'Hello world' } }).
 */
HWTEST_F(JsbundleTddTest, attrVali009, TestSize.Level0)
{
    JsbundleTddTest::CreateElement2ndAttributeValidation009();
}

/**
 * @tc.name: CreateElement_2ndAttribute_Validation_010
 * @tc.desc: Verify the _c function support handling legal parameter('text', []).
 */
HWTEST_F(JsbundleTddTest, attrVali010, TestSize.Level1)
{
    JsbundleTddTest::CreateElement2ndAttributeValidation010();
}

/**
 * @tc.name: CreateElement_2ndAttribute_Validation_011
 * @tc.desc: Verify the _c function support handling legal parameter('div', [_c('text')]).
 */
HWTEST_F(JsbundleTddTest, attrVali011, TestSize.Level1)
{
    JsbundleTddTest::CreateElement2ndAttributeValidation011();
}

/**
 * @tc.name: CreateElement_Children_Validation_012
 * @tc.desc: Verify the _c function support handling legal parameter('div', {}, []).
 */
HWTEST_F(JsbundleTddTest, ChildVali012, TestSize.Level1)
{
    JsbundleTddTest::CreateElementChildrenValidation012();
}

/**
 * @tc.name: CreateElement_Children_Validation_013
 * @tc.desc: Verify the _c function support handling legal parameter('div', {}, [1]).
 */
HWTEST_F(JsbundleTddTest, ChildVali013, TestSize.Level1)
{
    JsbundleTddTest::CreateElementChildrenValidation013();
}
#endif
} // namespace ACELite
} // namespace OHOS
