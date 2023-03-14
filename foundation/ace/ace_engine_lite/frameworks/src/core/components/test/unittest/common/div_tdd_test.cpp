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

#include "div_tdd_test.h"
#include "base_test.h"
#include "div_component.h"
#include "js_app_environment.h"
#include "stylemgr/app_style_manager.h"

namespace OHOS {
namespace ACELite {
DivTddTest::DivTddTest() : BaseTest()
{
    uint8_t componentNameLength = 3;
    componentNameId_ = KeyParser::ParseKeyId("div", componentNameLength);
}

void DivTddTest::ComponentDivAttributeSetTest015()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set unsupported attribute aaa = "abc"
     */
    JerrySetStringProperty(attrsObj_, "aaa", "abc");

    /* *
     * @tc.steps: step2. set normal attribute width = "100"
     */
    JerrySetStringProperty(attrsObj_, "id", "div01");

    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step2. check width set correctly
     */
    if (!strcmp(divView->GetViewId(), "div01")) {
        printf("ComponentDivAttributeSetTest015 pass\n");
    } else {
        printf("ComponentDivAttributeSetTest015 fail\n");
    }
    EXPECT_STREQ(divView->GetViewId(), "div01");
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest008()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set style width = 0
     */
    JerrySetNumberProperty(styleObj_, "width", 0);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step1. check width = 0
     */
    if (divView->GetWidth() == 0) {
        printf("ComponentDivStyleSetTest008 pass\n");
    } else {
        printf("ComponentDivStyleSetTest008 fail\n");
    }
    EXPECT_EQ(divView->GetWidth(), 0);
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style width = 20
     */
    uint8_t width = 20;
    JerrySetNumberProperty(styleObj_, "width", width);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step2. check width = 20
     */
    if (divView->GetWidth() == width) {
        printf("ComponentDivStyleSetTest008 pass\n");
    } else {
        printf("ComponentDivStyleSetTest008 fail\n");
    }
    EXPECT_TRUE(divView->GetWidth() == width);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest009()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set width = -32768
     */
    int16_t width = -32768;
    JerrySetNumberProperty(styleObj_, "width", width);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step1. get width = 1
     */
    if (divView->GetWidth() == 1) {
        printf("ComponentDivStyleSetTest009 pass\n");
    } else {
        printf("ComponentDivStyleSetTest009 fail\n");
    }
    EXPECT_EQ(divView->GetWidth(), 1);
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set width = 32767
     */
    int16_t widthBoundaryPosNum = 32767;
    JerrySetNumberProperty(styleObj_, "width", widthBoundaryPosNum);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step2. get width = 32767
     */
    if (divView->GetWidth() == widthBoundaryPosNum) {
        printf("ComponentDivStyleSetTest009 pass\n");
    } else {
        printf("ComponentDivStyleSetTest009 fail\n");
    }
    EXPECT_TRUE(divView->GetWidth() == widthBoundaryPosNum);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest010()
{
    TDD_CASE_BEGIN();
    const char *styleName = "width";
    /* *
     * @tc.steps: step1. set width = -32769, set width = 3276888
     */
    int32_t width = -32769;
    JerrySetNumberProperty(styleObj_, styleName, width);

    int32_t widthLarge = 1000000;
    JerrySetNumberProperty(styleObj_, styleName, widthLarge);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style width = "10"
     */
    uint8_t widthNormal = 10;
    JerrySetNumberProperty(styleObj_, styleName, widthNormal);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;

    /* *
     * @tc.expected: step3. check system not crash(set normal value, and check it is setted correctly)
     */
    uint8_t widthIn = 5;
    JerrySetNumberProperty(styleObj_, styleName, widthIn);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    printf("%d\n", divView->GetWidth());
    if (divView->GetWidth() == widthIn) {
        printf("ComponentDivStyleSetTest010 pass\n");
    } else {
        printf("ComponentDivStyleSetTest010 fail\n");
    }
    EXPECT_TRUE(divView->GetWidth() == widthIn);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest011()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set style height = 0
     */
    JerrySetNumberProperty(styleObj_, "height", 0);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step1. check height = 0
     */
    if (divView->GetHeight() == 0) {
        printf("ComponentDivStyleSetTest011 pass\n");
    } else {
        printf("ComponentDivStyleSetTest011 fail\n");
    }
    EXPECT_EQ(divView->GetHeight(), 0);
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style height = 20
     */
    uint8_t height = 20;
    JerrySetNumberProperty(styleObj_, "height", height);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step2. check height = 20
     */
    if (divView->GetHeight() == height) {
        printf("ComponentDivStyleSetTest011 pass\n");
    } else {
        printf("ComponentDivStyleSetTest011 fail\n");
    }
    EXPECT_TRUE(divView->GetHeight() == height);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest012()
{
    TDD_CASE_BEGIN();
    const char *styleName = "height";
    /* *
     * @tc.steps: step1. set height = -32769, set height = 3276888
     */
    int32_t height = -32769;
    JerrySetNumberProperty(styleObj_, styleName, height);

    int32_t heightBoundary = 3276888;
    JerrySetNumberProperty(styleObj_, styleName, heightBoundary);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style height = "10"
     */
    JerrySetStringProperty(styleObj_, styleName, "10");
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;

    /* *
     * @tc.expected: step3. check system not crash(set normal value, and check it is setted correctly)
     */
    uint8_t heightNormal = 5;
    JerrySetNumberProperty(styleObj_, styleName, heightNormal);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    printf("%d\n", divView->GetHeight());
    if (divView->GetHeight() == heightNormal) {
        printf("ComponentDivStyleSetTest012 pass\n");
    } else {
        printf("ComponentDivStyleSetTest012 fail\n");
    }
    EXPECT_TRUE(divView->GetHeight() == heightNormal);
    delete (component);
    component = nullptr;

    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest013()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set style margin = 0
     */
    JerrySetNumberProperty(styleObj_, "margin", 0);

    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step1. check margin = 0
     */
    if (divView->GetStyle(STYLE_MARGIN_LEFT) == 0 && divView->GetStyle(STYLE_MARGIN_TOP) == 0 &&
        divView->GetStyle(STYLE_MARGIN_RIGHT) == 0 && divView->GetStyle(STYLE_MARGIN_BOTTOM) == 0) {
        printf("ComponentDivStyleSetTest013 pass\n");
    } else {
        printf("ComponentDivStyleSetTest013 fail\n");
    }
    EXPECT_EQ(divView->GetStyle(STYLE_MARGIN_LEFT), 0);
    EXPECT_EQ(divView->GetStyle(STYLE_MARGIN_TOP), 0);
    EXPECT_EQ(divView->GetStyle(STYLE_MARGIN_RIGHT), 0);
    EXPECT_EQ(divView->GetStyle(STYLE_MARGIN_BOTTOM), 0);
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style margin = 20
     */
    uint8_t margin = 20;
    JerrySetNumberProperty(styleObj_, "margin", margin);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step2. check margin = 20
     */
    if (divView->GetStyle(STYLE_MARGIN_LEFT) == margin && divView->GetStyle(STYLE_MARGIN_TOP) == margin &&
        divView->GetStyle(STYLE_MARGIN_RIGHT) == margin && divView->GetStyle(STYLE_MARGIN_BOTTOM) == margin) {
        printf("ComponentDivStyleSetTest013 pass\n");
    } else {
        printf("ComponentDivStyleSetTest013 fail\n");
    }
    EXPECT_TRUE(divView->GetStyle(STYLE_MARGIN_LEFT) == margin);
    EXPECT_TRUE(divView->GetStyle(STYLE_MARGIN_TOP) == margin);
    EXPECT_TRUE(divView->GetStyle(STYLE_MARGIN_RIGHT) == margin);
    EXPECT_TRUE(divView->GetStyle(STYLE_MARGIN_BOTTOM) == margin);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest014()
{
    TDD_CASE_BEGIN();
    const char *styleName = "margin";
    /* *
     * @tc.steps: step1. set margin = -32769, set margin = 3276888
     */
    int32_t margin = -32769;
    JerrySetNumberProperty(styleObj_, styleName, margin);

    int32_t marginBoundary = 3276888;
    JerrySetNumberProperty(styleObj_, styleName, marginBoundary);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;
    /* *
     * @tc.steps: step2. set style margin = "10"
     */
    JerrySetStringProperty(styleObj_, styleName, "10");
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;
    /* *
     * @tc.expected: step3. check system not crash(set normal value, and check it is setted correctly)
     */
    uint8_t marginNormal = 5;
    JerrySetNumberProperty(styleObj_, styleName, marginNormal);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    printf("%d\n", divView->GetHeight());
    if (divView->GetStyle(STYLE_MARGIN_LEFT) == marginNormal && divView->GetStyle(STYLE_MARGIN_TOP) == marginNormal &&
        divView->GetStyle(STYLE_MARGIN_RIGHT) == marginNormal &&
        divView->GetStyle(STYLE_MARGIN_BOTTOM) == marginNormal) {
        printf("ComponentDivStyleSetTest014 pass\n");
    } else {
        printf("ComponentDivStyleSetTest014 fail\n");
    }
    EXPECT_TRUE(divView->GetStyle(STYLE_MARGIN_LEFT) == marginNormal);
    EXPECT_TRUE(divView->GetStyle(STYLE_MARGIN_TOP) == marginNormal);
    EXPECT_TRUE(divView->GetStyle(STYLE_MARGIN_RIGHT) == marginNormal);
    EXPECT_TRUE(divView->GetStyle(STYLE_MARGIN_BOTTOM) == marginNormal);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest015()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set style borderWidth = 0
     */
    JerrySetNumberProperty(styleObj_, "borderWidth", 0);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step1. check borderWidth = 0
     */
    if (divView->GetStyle(STYLE_BORDER_WIDTH) == 0) {
        printf("ComponentDivStyleSetTest015 pass\n");
    } else {
        printf("ComponentDivStyleSetTest015 fail\n");
    }
    EXPECT_EQ(divView->GetStyle(STYLE_BORDER_WIDTH), 0);
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style borderWidth = 20
     */
    uint8_t borderWidth = 20;
    JerrySetNumberProperty(styleObj_, "borderWidth", borderWidth);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step2. check borderWidth = 20
     */
    if (divView->GetStyle(STYLE_BORDER_WIDTH) == borderWidth) {
        printf("ComponentDivStyleSetTest015 pass\n");
    } else {
        printf("ComponentDivStyleSetTest015 fail\n");
    }
    EXPECT_TRUE(divView->GetStyle(STYLE_BORDER_WIDTH) == borderWidth);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest016()
{
    TDD_CASE_BEGIN();
    const char *styleName = "borderWidth";
    /* *
     * @tc.steps: step1. set borderWidth = -32769, set borderWidth = 3276888
     */
    int32_t borderWidth = -32769;
    JerrySetNumberProperty(styleObj_, styleName, borderWidth);

    int32_t borderWidthBoundary = 3276888;
    JerrySetNumberProperty(styleObj_, styleName, borderWidthBoundary);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style borderWidth = "10"
     */
    JerrySetStringProperty(styleObj_, styleName, "10");
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;
    /* *
     * @tc.expected: step3. check system not crash(set normal value, and check it is setted correctly)
     */
    uint8_t borderWidthNormal = 5;
    JerrySetNumberProperty(styleObj_, styleName, borderWidthNormal);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    printf("%d\n", divView->GetHeight());
    if (divView->GetStyle(STYLE_BORDER_WIDTH) == borderWidthNormal) {
        printf("ComponentDivStyleSetTest016 pass\n");
    } else {
        printf("ComponentDivStyleSetTest016 fail\n");
    }
    EXPECT_TRUE(divView->GetStyle(STYLE_BORDER_WIDTH) == borderWidthNormal);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest017()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set style borderRadius = 0
     */
    JerrySetNumberProperty(styleObj_, "borderRadius", 0);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step1. check borderRadius = 0
     */
    if (divView->GetStyle(STYLE_BORDER_RADIUS) == 0) {
        printf("ComponentDivStyleSetTest017 pass\n");
    } else {
        printf("ComponentDivStyleSetTest017 fail\n");
    }
    EXPECT_EQ(divView->GetStyle(STYLE_BORDER_RADIUS), 0);
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style borderRadius = 20
     */
    uint8_t borderRadius = 20;
    JerrySetNumberProperty(styleObj_, "borderRadius", borderRadius);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step2. check borderRadius = 20
     */
    if (divView->GetStyle(STYLE_BORDER_RADIUS) == borderRadius) {
        printf("ComponentDivStyleSetTest017 pass\n");
    } else {
        printf("ComponentDivStyleSetTest017 fail\n");
    }
    EXPECT_TRUE(divView->GetStyle(STYLE_BORDER_RADIUS) == borderRadius);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest018()
{
    TDD_CASE_BEGIN();
    const char *styleName = "borderRadius";
    /* *
     * @tc.steps: step1. set borderRadius = -32769, set borderRadius = 3276888
     */
    uint32_t borderRadius = -32769;
    JerrySetNumberProperty(styleObj_, styleName, borderRadius);

    uint32_t borderRadiusBoundary = 3276888;
    JerrySetNumberProperty(styleObj_, styleName, borderRadiusBoundary);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style borderRadius = "10"
     */
    JerrySetStringProperty(styleObj_, styleName, "10");
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;
    /* *
     * @tc.expected: step3. check system not crash(set normal value, and check it is setted correctly)
     */
    uint8_t borderRadiusNormal = 5;
    JerrySetNumberProperty(styleObj_, styleName, borderRadiusNormal);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    printf("%d\n", divView->GetHeight());
    if (divView->GetStyle(STYLE_BORDER_RADIUS) == borderRadiusNormal) {
        printf("ComponentDivStyleSetTest018 pass\n");
    } else {
        printf("ComponentDivStyleSetTest018 fail\n");
    }
    EXPECT_TRUE(divView->GetStyle(STYLE_BORDER_RADIUS) == borderRadiusNormal);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest019()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set style left = 0
     */
    JerrySetNumberProperty(styleObj_, "left", 0);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step1. check left = 0
     */
    if (divView->GetX() == 0) {
        printf("ComponentDivStyleSetTest019 pass\n");
    } else {
        printf("ComponentDivStyleSetTest019 fail\n");
    }
    EXPECT_EQ(divView->GetX(), 0);
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style left = 20
     */
    uint8_t left = 20;
    JerrySetNumberProperty(styleObj_, "left", left);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step2. check left = 20
     */
    if (divView->GetX() == left) {
        printf("ComponentDivStyleSetTest019 pass\n");
    } else {
        printf("ComponentDivStyleSetTest019 fail\n");
    }
    EXPECT_TRUE(divView->GetX() == left);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest020()
{
    TDD_CASE_BEGIN();
    const char *styleName = "left";
    /* *
     * @tc.steps: step1. set left = -32769, set left = 3276888
     */
    int32_t left = -32769;
    JerrySetNumberProperty(styleObj_, styleName, left);

    int32_t leftBoundary = 3276888;
    JerrySetNumberProperty(styleObj_, styleName, leftBoundary);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style left = "10"
     */
    JerrySetStringProperty(styleObj_, styleName, "10");
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;
    /* *
     * @tc.expected: step3. check system not crash(set normal value, and check it is setted correctly)
     */
    uint8_t leftNormal = 5;
    JerrySetNumberProperty(styleObj_, styleName, leftNormal);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    printf("%d\n", divView->GetHeight());
    if (divView->GetX() == leftNormal) {
        printf("ComponentDivStyleSetTest020 pass\n");
    } else {
        printf("ComponentDivStyleSetTest020 fail\n");
    }
    EXPECT_TRUE(divView->GetX() == leftNormal);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest021()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set style top = 0
     */
    JerrySetNumberProperty(styleObj_, "top", 0);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step1. check top = 0
     */
    if (divView->GetY() == 0) {
        printf("ComponentDivStyleSetTest021 pass\n");
    } else {
        printf("ComponentDivStyleSetTest021 fail\n");
    }
    EXPECT_EQ(divView->GetY(), 0);
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style top = 20
     */
    uint8_t top = 20;
    JerrySetNumberProperty(styleObj_, "top", top);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step2. check top = 20
     */
    if (divView->GetY() == top) {
        printf("ComponentDivStyleSetTest021 pass\n");
    } else {
        printf("ComponentDivStyleSetTest021 fail\n");
    }
    EXPECT_TRUE(divView->GetY() == top);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest022()
{
    TDD_CASE_BEGIN();
    const char *styleName = "top";
    /* *
     * @tc.steps: step1. set top = -32769, set top = 3276888
     */
    int32_t top = -32769;
    JerrySetNumberProperty(styleObj_, styleName, top);

    int32_t topBoundary = 3276888;
    JerrySetNumberProperty(styleObj_, styleName, topBoundary);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style top = "10"
     */
    JerrySetStringProperty(styleObj_, styleName, "10");
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;
    /* *
     * @tc.expected: step3. check system not crash(set normal value, and check it is setted correctly)
     */
    uint8_t topNormal = 5;
    JerrySetNumberProperty(styleObj_, styleName, topNormal);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    printf("%d\n", divView->GetHeight());
    if (divView->GetY() == topNormal) {
        printf("ComponentDivStyleSetTest022 pass\n");
    } else {
        printf("ComponentDivStyleSetTest022 fail\n");
    }
    EXPECT_TRUE(divView->GetY() == topNormal);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest023()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set style borderColor = 0
     */
    JerrySetNumberProperty(styleObj_, "borderColor", 0);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    ColorType tempColor;
    tempColor.full = divView->GetStyle(STYLE_BORDER_COLOR);
    /* *
     * @tc.expected: step1. check borderColor = 0
     */
    if (tempColor.blue == Color::GetColorFromRGB(0, 0, 0).blue &&
        tempColor.red == Color::GetColorFromRGB(0, 0, 0).red &&
        tempColor.green == Color::GetColorFromRGB(0, 0, 0).green) {
        printf("ComponentDivStyleSetTest023 pass\n");
    } else {
        printf("ComponentDivStyleSetTest023 fail\n");
    }
    EXPECT_TRUE(tempColor.blue == Color::GetColorFromRGB(0, 0, 0).blue);
    EXPECT_TRUE(tempColor.red == Color::GetColorFromRGB(0, 0, 0).red);
    EXPECT_TRUE(tempColor.green == Color::GetColorFromRGB(0, 0, 0).green);
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style borderColor = 3158454
     */
    int32_t color = 3158454;
    JerrySetNumberProperty(styleObj_, "borderColor", color);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    tempColor.full = divView->GetStyle(STYLE_BORDER_COLOR);
    /* *
     * @tc.expected: step2. check borderColor = 3158454
     */
    if (tempColor.blue == GetRGBColor(color).blue && tempColor.red == GetRGBColor(color).red &&
        tempColor.green == GetRGBColor(color).green) {
        printf("ComponentDivStyleSetTest023 pass\n");
    } else {
        printf("ComponentDivStyleSetTest023 fail\n");
    }
    EXPECT_TRUE(tempColor.blue == GetRGBColor(color).blue);
    EXPECT_TRUE(tempColor.red == GetRGBColor(color).red);
    EXPECT_TRUE(tempColor.green == GetRGBColor(color).green);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest024()
{
    TDD_CASE_BEGIN();
    const char *styleName = "borderColor";
    /* *
     * @tc.steps: step1. set borderColor = -21474836481, set borderColor = 21474836481
     */
    int64_t borderColor = -21474836481;
    jerry_value_t numValue = jerry_create_number(borderColor);
    JerrySetNamedProperty(styleObj_, styleName, numValue);
    jerry_release_value(numValue);

    int64_t borderColorBoundary = 21474836481;
    JerrySetNumberProperty(styleObj_, styleName, borderColorBoundary);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style borderColor = "10"
     */
    JerrySetStringProperty(styleObj_, styleName, "10");
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;
    /* *
     * @tc.expected: step3. check system not crash(set normal value, and check it is setted correctly)
     */
    int32_t borderColorNormal = 2735506;
    JerrySetNumberProperty(styleObj_, styleName, borderColorNormal);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    ColorType tempColor;
    tempColor.full = divView->GetStyle(STYLE_BORDER_COLOR);
    printf("%d\n", divView->GetHeight());
    if (tempColor.blue == GetRGBColor(borderColorNormal).blue && tempColor.red == GetRGBColor(borderColorNormal).red &&
        tempColor.green == GetRGBColor(borderColorNormal).green) {
        printf("ComponentDivStyleSetTest024 pass\n");
    } else {
        printf("ComponentDivStyleSetTest024 fail\n");
    }
    EXPECT_TRUE(tempColor.blue == GetRGBColor(borderColorNormal).blue);
    EXPECT_TRUE(tempColor.red == GetRGBColor(borderColorNormal).red);
    EXPECT_TRUE(tempColor.green == GetRGBColor(borderColorNormal).green);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest025()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set style backgroundColor = 0
     */
    JerrySetNumberProperty(styleObj_, "backgroundColor", 0);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    ColorType tempColor;
    tempColor.full = divView->GetStyle(STYLE_BACKGROUND_COLOR);
    /* *
     * @tc.expected: step1. check backgroundColor = 0
     */
    if (tempColor.blue == GetRGBColor(0).blue && tempColor.red == GetRGBColor(0).red &&
        tempColor.green == GetRGBColor(0).green) {
        printf("ComponentDivStyleSetTest025 pass\n");
    } else {
        printf("ComponentDivStyleSetTest025 fail\n");
    }
    EXPECT_TRUE(tempColor.blue == GetRGBColor(0).blue);
    EXPECT_TRUE(tempColor.red == GetRGBColor(0).red);
    EXPECT_TRUE(tempColor.green == GetRGBColor(0).green);
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style backgroundColor = 3158454
     */
    int32_t color = 3158454;
    JerrySetNumberProperty(styleObj_, "backgroundColor", color);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    tempColor.full = divView->GetStyle(STYLE_BACKGROUND_COLOR);
    /* *
     * @tc.expected: step2. check backgroundColor = 3158454
     */
    if (tempColor.blue == GetRGBColor(color).blue && tempColor.red == GetRGBColor(color).red &&
        tempColor.green == GetRGBColor(color).green) {
        printf("ComponentDivStyleSetTest025 pass\n");
    } else {
        printf("ComponentDivStyleSetTest025 fail\n");
    }
    EXPECT_TRUE(tempColor.blue == GetRGBColor(color).blue);
    EXPECT_TRUE(tempColor.red == GetRGBColor(color).red);
    EXPECT_TRUE(tempColor.green == GetRGBColor(color).green);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest026()
{
    TDD_CASE_BEGIN();
    const char *styleName = "backgroundColor";
    /* *
     * @tc.steps: step1. set backgroundColor = -21474836481, set backgroundColor = 21474836481
     */
    int64_t bgColor = -21474836481;
    JerrySetNumberProperty(styleObj_, styleName, bgColor);

    int64_t bgColorBoundary = 21474836481;
    JerrySetNumberProperty(styleObj_, styleName, bgColorBoundary);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;

    /* *
     * @tc.steps: step2. set style backgroundColor = "10"
     */
    JerrySetStringProperty(styleObj_, styleName, "10");
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    delete (component);
    component = nullptr;

    /* *
     * @tc.expected: step3. check system not crash(set normal value, and check it is setted correctly)
     */
    int32_t bgColorNormal = 2735506;
    JerrySetNumberProperty(styleObj_, styleName, bgColorNormal);
    component = GetRenderedComponent(componentNameId_);
    divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    ColorType tempColor;
    tempColor.full = divView->GetStyle(STYLE_BACKGROUND_COLOR);
    printf("%d\n", divView->GetHeight());
    if (tempColor.blue == GetRGBColor(bgColorNormal).blue && tempColor.red == GetRGBColor(bgColorNormal).red &&
        tempColor.green == GetRGBColor(bgColorNormal).green) {
        printf("ComponentDivStyleSetTest026 pass\n");
    } else {
        printf("ComponentDivStyleSetTest026 fail\n");
    }
    EXPECT_TRUE(tempColor.blue == GetRGBColor(bgColorNormal).blue);
    EXPECT_TRUE(tempColor.red == GetRGBColor(bgColorNormal).red);
    EXPECT_TRUE(tempColor.green == GetRGBColor(bgColorNormal).green);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::ComponentDivStyleSetTest032()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set top = 50, set unsupport style, abc = 78978
     */
    uint8_t top = 50;
    double abc = 78978;
    JerrySetNumberProperty(styleObj_, "top", top);
    JerrySetNumberProperty(styleObj_, "abc", abc);
    Component *component = GetRenderedComponent(componentNameId_);
    UIViewGroup *divView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());

    /* *
     * @tc.steps: step2. check top = 50
     */
    if (divView->GetY() == top) {
        printf("ComponentDivStyleSetTest032 pass\n");
    } else {
        printf("ComponentDivStyleSetTest032 fail\n");
    }
    EXPECT_TRUE(divView->GetY() == top);
    delete (component);
    component = nullptr;
    TDD_CASE_END();
}

void DivTddTest::RunTests()
{
    ComponentDivAttributeSetTest015();
    ComponentDivStyleSetTest008();
    ComponentDivStyleSetTest009();
    ComponentDivStyleSetTest010();
    ComponentDivStyleSetTest011();
    ComponentDivStyleSetTest012();
    ComponentDivStyleSetTest013();
    ComponentDivStyleSetTest014();
    ComponentDivStyleSetTest015();
    ComponentDivStyleSetTest016();
    ComponentDivStyleSetTest017();
    ComponentDivStyleSetTest018();
    ComponentDivStyleSetTest019();
    ComponentDivStyleSetTest020();
    ComponentDivStyleSetTest021();
    ComponentDivStyleSetTest022();
    ComponentDivStyleSetTest023();
    ComponentDivStyleSetTest024();
    ComponentDivStyleSetTest025();
    ComponentDivStyleSetTest026();
    ComponentDivStyleSetTest032();
}

#ifdef TDD_ASSERTIONS
/* *
 * @tc.name: ComponentDivAttributeSetTest015
 * @tc.desc: Verify attribute(which is not support) will not affect normal attribute set.
 */
HWTEST_F(DivTddTest, divAttr015, TestSize.Level1)
{
    DivTddTest::ComponentDivAttributeSetTest015();
}

/* *
 * @tc.name: ComponentDivStyleSetTest008
 * @tc.desc: Verify width style can set nomally.
 */
HWTEST_F(DivTddTest, div008, TestSize.Level0)
{
    DivTddTest::ComponentDivStyleSetTest008();
}

/* *
 * @tc.name: ComponentDivStyleSetTest009
 * @tc.desc: Verify Common style width set boundary value.
 */
HWTEST_F(DivTddTest, div009, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest009();
}

/* *
 * @tc.name: ComponentDivStyleSetTest010
 * @tc.desc: Verify Common style width set value out of boundary.
 */
HWTEST_F(DivTddTest, div010, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest010();
}

/* *
 * @tc.name: ComponentDivStyleSetTest011
 * @tc.desc: Verify height style can set nomally.
 */
HWTEST_F(DivTddTest, div011, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest011();
}

/* *
 * @tc.name: ComponentDivStyleSetTest012
 * @tc.desc: Verify Common style height set value out of boundary.
 */
HWTEST_F(DivTddTest, div012, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest012();
}

/* *
 * @tc.name: ComponentDivStyleSetTest013
 * @tc.desc: Verify margin style can set nomally.
 */
HWTEST_F(DivTddTest, div013, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest013();
}

/* *
 * @tc.name: ComponentDivStyleSetTest014
 * @tc.desc: Verify Common style margin set value out of boundary.
 */
HWTEST_F(DivTddTest, div014, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest014();
}

/* *
 * @tc.name: ComponentDivStyleSetTest015
 * @tc.desc: Verify borderWidth style can set nomally.
 */
HWTEST_F(DivTddTest, div015, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest015();
}

/* *
 * @tc.name: ComponentDivStyleSetTest016
 * @tc.desc: Verify Common style borderWidth set value out of boundary.
 */
HWTEST_F(DivTddTest, div016, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest016();
}

/* *
 * @tc.name: ComponentDivStyleSetTest017
 * @tc.desc: Verify borderRadius style can set nomally.
 */
HWTEST_F(DivTddTest, div017, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest017();
}

/* *
 * @tc.name: ComponentDivStyleSetTest018
 * @tc.desc: Verify Common style borderRadius set value out of boundary.
 */
HWTEST_F(DivTddTest, div018, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest018();
}

/* *
 * @tc.name: ComponentDivStyleSetTest019
 * @tc.desc: Verify left style can set nomally.
 */
HWTEST_F(DivTddTest, div019, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest019();
}

/* *
 * @tc.name: ComponentDivStyleSetTest020
 * @tc.desc: Verify Common style left set value out of boundary.
 */
HWTEST_F(DivTddTest, div020, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest020();
}

/* *
 * @tc.name: ComponentDivStyleSetTest021
 * @tc.desc: Verify top style can set nomally.
 */
HWTEST_F(DivTddTest, div021, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest021();
}

/* *
 * @tc.name: ComponentDivStyleSetTest022
 * @tc.desc: Verify Common style top set value out of boundary.
 */
HWTEST_F(DivTddTest, div022, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest022();
}

/* *
 * @tc.name: ComponentDivStyleSetTest023
 * @tc.desc: Verify borderColor style can set nomally.
 */
HWTEST_F(DivTddTest, div023, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest023();
}

/* *
 * @tc.name: ComponentDivStyleSetTest024
 * @tc.desc: Verify Common style borderColor set value out of boundary.
 */
HWTEST_F(DivTddTest, div024, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest024();
}

/* *
 * @tc.name: ComponentDivStyleSetTest025
 * @tc.desc: Verify backgroundColor style can set nomally.
 */
HWTEST_F(DivTddTest, div025, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest025();
}

/* *
 * @tc.name: ComponentDivStyleSetTest026
 * @tc.desc: Verify Common style backgroundColor set value out of boundary.
 */
HWTEST_F(DivTddTest, div026, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest026();
}

/* *
 * @tc.name: ComponentDivStyleSetTest032
 * @tc.desc: Verify set style which is not support, system can run normally.
 */
HWTEST_F(DivTddTest, div032, TestSize.Level1)
{
    DivTddTest::ComponentDivStyleSetTest032();
}
#endif
} // namespace ACELite
} // namespace OHOS
