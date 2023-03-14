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

#include "arc_tdd_test.h"

namespace OHOS {
namespace ACELite {
void ArcTddTest::SetCircleType()
{
    JerrySetStringProperty(attrsObj_, "type", "arc");
}

int16_t ArcTddTest::GetPercentage(const UICircleProgress &circleProgress) const
{
    uint16_t delta = circleProgress.GetValue() - circleProgress.GetRangeMin();
    uint16_t rangeSize = circleProgress.GetRangeMax() - circleProgress.GetRangeMin();
    if (rangeSize == 0) {
        return 0;
    }

    uint8_t totalPercent = 100;
    uint16_t result = totalPercent * delta / rangeSize;
    if (result > totalPercent) {
        result = totalPercent;
    }
    return result;
}

void ArcTddTest::PrintExpectInfo(const ColorType colorType, int32_t lineColor) const
{
    EXPECT_TRUE(colorType.green == GetRGBColor(lineColor).green);
    EXPECT_TRUE(colorType.blue == GetRGBColor(lineColor).blue);
    EXPECT_TRUE(colorType.red == GetRGBColor(lineColor).red);
}

void ArcTddTest::ComponentProgressAttributeSetTest001()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();
    /**
    * @tc.steps: step1. set attribute percent = 50
    */
    int8_t percentValue = 50;
    JerrySetNumberProperty(attrsObj_, "percent", percentValue);

    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressAttributeSetTest001 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressAttributeSetTest001 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }

    /**
    * @tc.expected: step1. check percent = 50
    */
    EXPECT_TRUE(GetPercentage(*circleView) == percentValue);

    /**
    * @tc.steps: step2. set attribute percent = -60
    */
    int8_t percentValueNavigateNum = -60;
    UpdateNumAttributeOrStyleValue(circleComponent, "percent", percentValueNavigateNum, true);

   /**
    * @tc.expected: step2. check percent = 0
    */
    EXPECT_EQ(GetPercentage(*circleView), 0);

    /**
    * @tc.steps: step3. set attribute percent = -1
    */
    UpdateNumAttributeOrStyleValue(circleComponent, "percent", -1, true);

    /**
    * @tc.expected: step3. check percent = 0
    */
    EXPECT_EQ(GetPercentage(*circleView), 0);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::ComponentProgressAttributeSetTest002()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();
    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressAttributeSetTest002 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressAttributeSetTest002 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }
    /**
    * @tc.steps: step4. set attribute percent = 0
    */
    UpdateNumAttributeOrStyleValue(circleComponent, "percent", 0, true);

    /**
    * @tc.expected: step4. check percent = 0
    */
    EXPECT_EQ(GetPercentage(*circleView), 0);

    /**
    * @tc.steps: step5. set attribute percent = 100
    */
    int8_t percentValueFull = 100;
    UpdateNumAttributeOrStyleValue(circleComponent, "percent", percentValueFull, true);

    /**
    * @tc.expected: step5. check percent = 100
    */
    EXPECT_TRUE(GetPercentage(*circleView) == percentValueFull);

    /**
    * @tc.steps: step6. set attribute percent = 101
    */
    int8_t percentValueBoundary = 101;
    UpdateNumAttributeOrStyleValue(circleComponent, "percent", percentValueBoundary, true);

    /**
    * @tc.expected: step6. check percent = 100
    */
    uint8_t maxPercent = 100;
    EXPECT_TRUE(GetPercentage(*circleView) == maxPercent);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::ComponentProgressAttributeSetTest003()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();
    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressAttributeSetTest003 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressAttributeSetTest003 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }
    /**
    * @tc.steps: step6. set attribute percent = 200
    */
    uint8_t percentValueOver = 200;
    UpdateNumAttributeOrStyleValue(circleComponent, "percent", percentValueOver, true);

    /**
    * @tc.expected: step6. check percent = 100
    */
    uint8_t maxPercent = 100;
    EXPECT_TRUE(GetPercentage(*circleView) == maxPercent);

    /**
    * @tc.steps: step6. set attribute percent = "abc"
    */
    UpdateCharAttributeOrStyleValue(circleComponent, "percent", "abc", true);

    /**
    * @tc.expected: step6. check percent = "abc"
    */
    EXPECT_EQ(GetPercentage(*circleView), 0);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::ComponentProgressStyleSetTest001()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();

    const char* attributeName = "centerX";

    /**
    * @tc.steps: step1. set style centerX = 227
    */
    uint8_t centerX = 227;
    JerrySetNumberProperty(styleObj_, attributeName, centerX);

    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressStyleSetTest001 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressStyleSetTest001 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }

    /**
    * @tc.expected: step1. check centerX = 227
    */
    EXPECT_TRUE(circleView->GetCenterPosition().x == centerX);

    /**
    * @tc.steps: step2. set attribute centerX = -32768
    */
    int16_t centerXBoundary = -32768;
    UpdateNumAttributeOrStyleValue(circleComponent, attributeName, centerXBoundary, false);

    /**
    * @tc.expected: step2. check centerX = -32768
    */
    EXPECT_TRUE(circleView->GetCenterPosition().x == centerXBoundary);

    /**
    * @tc.steps: step3. set attribute centerX = 32765
    */
    int16_t centerXBoundaryIn = 32765;
    UpdateNumAttributeOrStyleValue(circleComponent, attributeName, centerXBoundaryIn, false);

    /**
    * @tc.expected: step3. check centerX = 32765
    */
    EXPECT_TRUE(circleView->GetCenterPosition().x == centerXBoundaryIn);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::ComponentProgressStyleSetTest002()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();

    const char* attributeName = "centerX";

    /**
    * @tc.steps: step1. set style centerX = -32769
    */
    int32_t centerX = -32769;
    JerrySetNumberProperty(styleObj_, attributeName, centerX);

    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressStyleSetTest002 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressStyleSetTest002 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }

    /**
    * @tc.expected: step1. set normal value 20, and check centerX = 22 (use normal value set to check whether system
    * run normally)
    */
    uint8_t centerXNormal = 20;
    UpdateNumAttributeOrStyleValue(circleComponent, "centerX", centerXNormal, false);
    EXPECT_TRUE(circleView->GetCenterPosition().x == centerXNormal);

    /**
    * @tc.steps: step2. set style centerX = -32766
    */
    int16_t centerXBoundary = -32766;
    UpdateNumAttributeOrStyleValue(circleComponent, "centerX", centerXBoundary, false);

    /**
    * @tc.steps: step3. set style centerX = "*,"
    */
    UpdateCharAttributeOrStyleValue(circleComponent, "centerX", "*,", false);

    /**
    * @tc.expected: step3. check centerX = 0 (use normal value set to check whether system run normally)
    */
    EXPECT_EQ(circleView->GetCenterPosition().x, 0);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::ComponentProgressStyleSetTest003()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();

    const char* styleName = "centerY";

    /**
    * @tc.steps: step1. set style centerY = 227
    */
    uint8_t centerY = 227;
    JerrySetNumberProperty(styleObj_, styleName, centerY);

    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressStyleSetTest003 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressStyleSetTest003 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }

    /**
    * @tc.expected: step1. check centerY = 227
    */
    EXPECT_TRUE(circleView->GetCenterPosition().y == centerY);

    /**
    * @tc.steps: step2. set attribute centerY = -32768
    */
    int16_t centerYBoundary = -32768;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, centerYBoundary, false);

    /**
    * @tc.expected: step2. check centerY = -32768
    */
    EXPECT_TRUE(circleView->GetCenterPosition().y == centerYBoundary);

    /**
    * @tc.steps: step3. set attribute centerY = 32765
    */
    int16_t centerYBoundaryIn = 32765;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, centerYBoundaryIn, false);

    /**
    * @tc.expected: step3. check centerY = 32765
    */
    EXPECT_TRUE(circleView->GetCenterPosition().y == centerYBoundaryIn);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::ComponentProgressStyleSetTest004()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();

    const char* styleName = "centerY";

    /**
    * @tc.steps: step1. set style centerY = -32769
    */
    int32_t centerY = -32769;
    JerrySetNumberProperty(styleObj_, styleName, centerY);

    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressStyleSetTest003 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressStyleSetTest003 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }

    /**
    * @tc.expected: step1. set normal value 20, and check centerY = 22 (use normal value set to check whether system
    * run normally)
    */
    uint8_t centerYNormal = 20;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, centerYNormal, false);
    EXPECT_TRUE(circleView->GetCenterPosition().y == centerYNormal);

    /**
    * @tc.steps: step2. set style centerY = -32766
    */
    int16_t centerYBoundary = -32766;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, centerYBoundary, false);

    /**
    * @tc.steps: step3. set style centerY = "abc"
    */
    UpdateCharAttributeOrStyleValue(circleComponent, styleName, "abc", false);

    /**
    * @tc.expected: step3. check centerY = 0 (use normal value set to check whether system run normally)
    */
    EXPECT_EQ(circleView->GetCenterPosition().y, 0);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::ComponentProgressStyleSetTest005()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();

    const char* styleName = "radius";

    /**
    * @tc.steps: step1. set style radius = 227
    */
    uint8_t radius = 227;
    JerrySetNumberProperty(styleObj_, styleName, radius);

    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressStyleSetTest005 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressStyleSetTest005 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }

    /**
    * @tc.expected: step1. check radius = 227
    */
    EXPECT_TRUE(circleView->GetRadius() == radius);

    /**
    * @tc.steps: step2. set style radius = 0
    */
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, 0, false);

    /**
    * @tc.expected: step2. check radius = 0
    */
    EXPECT_EQ(circleView->GetRadius(), 0);

    /**
    * @tc.steps: step3. set style radius = 65534
    */
    uint16_t radiusBoundary = 65534;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, radiusBoundary, false);

    /**
    * @tc.expected: step3. check radius = 65534
    */
    EXPECT_TRUE(circleView->GetRadius() == radiusBoundary);

    /**
    * @tc.steps: step4. set style radius = "this is a string"
    */
    UpdateCharAttributeOrStyleValue(circleComponent, styleName, "this is a string", false);

    /**
    * @tc.expected: step4. check radius = 0
    */
    EXPECT_TRUE(circleView->GetRadius() == 0);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::ComponentProgressStyleSetTest006()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();

    const char* styleName = "radius";

    /**
    * @tc.steps: step1. set style radius = -1
    */
    JerrySetNumberProperty(styleObj_, styleName, -1);

    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressStyleSetTest006 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressStyleSetTest006 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }

    /**
    * @tc.steps: step2. set style radius = 655355
    */
    int32_t radius = 655355;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, radius, false);

    /**
    * @tc.expected: step2. set normal value 20, and check radius = 20 (use normal value set to check whether system run
    * normally)
    */
    uint8_t radiusNormal = 20;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, radiusNormal, false);
    EXPECT_TRUE(circleView->GetRadius() == radiusNormal);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::ComponentProgressStyleSetTest008()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();

    const char* styleName = "startAngle";

    /**
    * @tc.steps: step1. set style startAngle = 0
    */
    JerrySetNumberProperty(styleObj_, styleName, 0);

    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressStyleSetTest008 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressStyleSetTest008 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }

    /**
    * @tc.expected: step1. check startAngle = 0
    */
    EXPECT_EQ(circleView->GetStartAngle(), 0);

    /**
    * @tc.steps: step2. set style startAngle = 360
    */
    int16_t startAngle = 360;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, startAngle, false);

    /**
    * @tc.expected: step2. check startAngle = 360
    */
    EXPECT_TRUE(circleView->GetStartAngle() == startAngle);

    /**
    * @tc.steps: step3. set style startAngle = 361
    */
    int16_t startAngleBoundary = 361;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, startAngleBoundary, false);

    /**
    * @tc.expected: step3. check startAngle = 360
    */
    EXPECT_TRUE(circleView->GetStartAngle() == startAngle);

    /**
    * @tc.steps: step4. set style startAngle = -1
    */
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, -1, false);

    /**
    * @tc.expected: step4. check startAngle = 0
    */
    EXPECT_EQ(circleView->GetStartAngle(), 0);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::ComponentProgressStyleSetTest009()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();

    const char* styleName = "startAngle";

    /**
    * @tc.steps: step1. set unnormal style startAngle = -32769
    */
    int32_t startAngle = -32769;
    JerrySetNumberProperty(styleObj_, styleName, startAngle);

    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressStyleSetTest009 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressStyleSetTest009 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }

    /**
    * @tc.steps: step2. set unnormal style startAngle = 326669
    */
    int32_t startAngleLarger = 326669;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, startAngleLarger, false);

    /**
    * @tc.steps: step3. set style startAngle = 30
    */
    uint8_t startAngleNormal = 30;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, startAngleNormal, false);

    /**
    * @tc.expected: step3. check startAngle = 30
    */
    EXPECT_TRUE(circleView->GetStartAngle() == startAngleNormal);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::ComponentProgressStyleSetTest010()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();

    const char* styleName = "totalAngle";

    int16_t startAngle = 30;
    JerrySetNumberProperty(styleObj_, "startAngle", startAngle);

    /**
    * @tc.steps: step1. set style totalAngle = -360
    */
    int16_t totalAngle = -360;
    JerrySetNumberProperty(styleObj_, styleName, totalAngle);

    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressStyleSetTest010 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressStyleSetTest010 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }

    /**
    * @tc.expected: step1. check endAngle = -330
    */
    int16_t endAngle = -330;
    EXPECT_TRUE(circleView->GetEndAngle() == endAngle);

    /**
    * @tc.steps: step2. set style totalAngle = -361
    */
    int16_t totalAngleBoundary = -361;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, totalAngleBoundary, false);

    /**
    * @tc.expected: step2. check endAngle = -330
    */
    EXPECT_TRUE(circleView->GetEndAngle() == endAngle);

    /**
    * @tc.steps: step3. set style totalAngle = 360
    */
    int16_t totalAngleBoundaryIn = 360;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, totalAngleBoundaryIn, false);

    /**
    * @tc.expected: step3. check endAngle = 390
    */
    int16_t endAngleUpdate = 390;
    EXPECT_TRUE(circleView->GetEndAngle() == endAngleUpdate);

    /**
    * @tc.steps: step4. set style totalAngle = 90
    */
    uint8_t totalAngleNormal = 90;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, totalAngleNormal, false);

    /**
    * @tc.expected: step4. check endAngle = 120
    */
    uint8_t endAngleReNew = 120;
    EXPECT_TRUE(circleView->GetEndAngle() == endAngleReNew);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::ComponentProgressStyleSetTest011()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();

    const char* styleName = "totalAngle";
    uint8_t startAngle = 30;
    JerrySetNumberProperty(styleObj_, "startAngle", startAngle);

    /**
    * @tc.steps: step1. set unnormal style totaldAngle = -32769
    */
    int32_t totalAngle = -32769;
    JerrySetNumberProperty(styleObj_, styleName, totalAngle);

    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressStyleSetTest011 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressStyleSetTest011 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }

    /**
    * @tc.steps: step2. set unnormal style totalAngle = 326669
    */
    int32_t totalAngleBoundary = 326669;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, totalAngleBoundary, false);

    /**
    * @tc.steps: step3. set style totalAngle = 30
    */
    uint8_t totalAngleNormal = 30;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, totalAngleNormal, false);

    /**
    * @tc.expected: step3. check endAngle = 60
    */
    uint8_t endAngle = 60;
    EXPECT_TRUE(circleView->GetEndAngle() == endAngle);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::ComponentProgressStyleSetTest012()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();

    const char* styleName = "color";

    /**
    * @tc.steps: step1. set style lineColor = 227
    */
    int32_t lineColor = 227;
    JerrySetNumberProperty(styleObj_, styleName, lineColor);

    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressStyleSetTest012 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressStyleSetTest012 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }
    ColorType tempColor;
    tempColor.full = circleView->GetForegroundStyle(STYLE_LINE_COLOR);

    /**
    * @tc.expected: step1. check lineColor = 227
    */
    PrintExpectInfo(tempColor, lineColor);

    /**
    * @tc.steps: step2. set style lineColor = -2147483648
    */
    int32_t lineColorBoundary = INT_MIN;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, lineColorBoundary, false);
    tempColor.full = circleView->GetForegroundStyle(STYLE_LINE_COLOR);

    /**
    * @tc.expected: step2. check lineColor = -2147483648
    */
    PrintExpectInfo(tempColor, lineColorBoundary);

    /**
    * @tc.steps: step3. set style lineColor = 2147483647
    */
    int32_t lineColorBoundaryPosNum = 2147483647;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, lineColorBoundaryPosNum, false);
    tempColor.full = circleView->GetForegroundStyle(STYLE_LINE_COLOR);

    /**
    * @tc.expected: step3. check lineColor = 2147483647
    */
    PrintExpectInfo(tempColor, lineColorBoundaryPosNum);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::ComponentProgressStyleSetTest013()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();

    const char* styleName = "color";

    /**
    * @tc.steps: step1. set unnormal style lineColor = -2147483648
    */
    int32_t lineColor = INT_MIN;
    JerrySetNumberProperty(styleObj_, styleName, lineColor);

    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressStyleSetTest013 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressStyleSetTest013 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }

    /**
    * @tc.steps: step2. set unnormal style lineColor = 2147483648
    */
    int32_t lineColorBoundary = static_cast<int32_t>(2147483648);
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, lineColorBoundary, false);

    /**
    * @tc.steps: step3. set style lineColor = 30
    */
    uint8_t lineColorNormal = 30;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, lineColorNormal, false);
    ColorType tempColor;
    tempColor.full = circleView->GetForegroundStyle(STYLE_LINE_COLOR);

    /**
    * @tc.expected: step3. check lineColor = 30
    */
    PrintExpectInfo(tempColor, lineColorNormal);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::ComponentProgressStyleSetTest014()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();

    const char* styleName = "strokeWidth";

    /**
    * @tc.steps: step1. set style lineWidth = 227
    */
    int16_t lineWidth = 227;
    JerrySetNumberProperty(styleObj_, styleName, lineWidth);

    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressStyleSetTest014 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressStyleSetTest014 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }

    /**
    * @tc.expected: step1. check lineWidth = 227
    */
    EXPECT_TRUE(circleView->GetForegroundStyle(STYLE_LINE_WIDTH) == lineWidth);

    /**
    * @tc.steps: step2. set style lineWidth = -32768
    */
    int16_t lineWidthBoundary = -32768;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, lineWidthBoundary, false);

    /**
    * @tc.expected: step2. check lineWidth = -32768
    */
    EXPECT_TRUE(circleView->GetForegroundStyle(STYLE_LINE_WIDTH) == lineWidthBoundary);

    /**
    * @tc.steps: step3. set style lineWidth = 32767
    */
    int16_t lineWidthBoundaryPosNum = 32767;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, lineWidthBoundaryPosNum, false);

    /**
    * @tc.expected: step3. check lineWidth = 32767
    */
    EXPECT_TRUE(circleView->GetForegroundStyle(STYLE_LINE_WIDTH) == lineWidthBoundaryPosNum);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::ComponentProgressStyleSetTest015()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("progress");
    SetCircleType();

    const char* styleName = "strokeWidth";

    /**
    * @tc.steps: step1. set unnormal style lineWidth = -32769
    */
    int32_t lineWidth = -32769;
    JerrySetNumberProperty(styleObj_, styleName, lineWidth);

    Component* circleComponent = reinterpret_cast<CircleProgressComponent *>(GetRenderedComponent(componentNameId_));
    if (circleComponent == nullptr) {
        printf("ComponentProgressStyleSetTest015 fail\n");
        TDD_CASE_END();
        return;
    }
    UICircleProgress* circleView = reinterpret_cast<UICircleProgress *>(circleComponent->GetComponentRootView());
    if (circleView == nullptr) {
        printf("ComponentProgressStyleSetTest015 fail\n");
        delete(circleComponent);
        circleComponent = nullptr;
        TDD_CASE_END();
        return;
    }

    /**
    * @tc.steps: step2. set unnormal style lineWidth = 326669
    */
    int32_t lineWidthBoundary = 326669;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, lineWidthBoundary, false);

    /**
    * @tc.steps: step3. set style lineWidth = 30
    */
    uint8_t lineWidthNormal = 30;
    UpdateNumAttributeOrStyleValue(circleComponent, styleName, lineWidthNormal, false);

    /**
    * @tc.expected: step3. check lineWidth = 30
    */
    EXPECT_TRUE(circleView->GetForegroundStyle(STYLE_LINE_WIDTH) == lineWidthNormal);
    delete(circleComponent);
    circleComponent = nullptr;
    TDD_CASE_END();
}

void ArcTddTest::RunTests()
{
    ComponentProgressAttributeSetTest001();
    ComponentProgressStyleSetTest001();
    ComponentProgressStyleSetTest002();
    ComponentProgressStyleSetTest003();
    ComponentProgressStyleSetTest004();
    ComponentProgressStyleSetTest005();
    ComponentProgressStyleSetTest006();
    ComponentProgressStyleSetTest008();
    ComponentProgressStyleSetTest009();
    ComponentProgressStyleSetTest010();
    ComponentProgressStyleSetTest011();
    ComponentProgressStyleSetTest012();
    ComponentProgressStyleSetTest013();
    ComponentProgressStyleSetTest014();
    ComponentProgressStyleSetTest015();
}

#ifdef TDD_ASSERTIONS
/**
* @tc.name: ComponentProgressAttributeSetTest001
* @tc.desc: Verify circleprogress percent attribute can set normally.
*/
HWTEST_F(ArcTddTest, arcAttr001, TestSize.Level0)
{
    ArcTddTest::ComponentProgressAttributeSetTest001();
}

/**
* @tc.name: ComponentProgressAttributeSetTest002
* @tc.desc: Verify circleprogress percent attribute can set normally.
*/
HWTEST_F(ArcTddTest, arcAttr002, TestSize.Level1)
{
    ArcTddTest::ComponentProgressAttributeSetTest002();
}

/**
* @tc.name: ComponentProgressAttributeSetTest003
* @tc.desc: Verify circleprogress percent attribute can set normally.
*/
HWTEST_F(ArcTddTest, arcAttr003, TestSize.Level1)
{
    ArcTddTest::ComponentProgressAttributeSetTest003();
}

/**
* @tc.name: ComponentProgressStyleSetTest001
* @tc.desc: Verify circleprogress centerX style can set normally.
*/
HWTEST_F(ArcTddTest, arcStyle001, TestSize.Level1)
{
    ArcTddTest::ComponentProgressStyleSetTest001();
}

/**
* @tc.name: ComponentProgressStyleSetTest002
* @tc.desc: Verify circleprogress centerX style can deal unNormally value.
*/
HWTEST_F(ArcTddTest, arcStyle002, TestSize.Level1)
{
    ArcTddTest::ComponentProgressStyleSetTest002();
}

/**
* @tc.name: ComponentProgressStyleSetTest003
* @tc.desc: Verify circleprogress centerY style can set normally.
*/
HWTEST_F(ArcTddTest, arcStyle003, TestSize.Level1)
{
    ArcTddTest::ComponentProgressStyleSetTest003();
}

/**
* @tc.name: ComponentProgressStyleSetTest004
* @tc.desc: Verify circleprogress centerY style can deal unNormally value.
*/
HWTEST_F(ArcTddTest, arcStyle004, TestSize.Level1)
{
    ArcTddTest::ComponentProgressStyleSetTest004();
}

/**
* @tc.name: ComponentProgressStyleSetTest005
* @tc.desc: Verify circleprogress radius style can set normally.
*/
HWTEST_F(ArcTddTest, arcStyle005, TestSize.Level1)
{
    ArcTddTest::ComponentProgressStyleSetTest005();
}

/**
* @tc.name: ComponentProgressStyleSetTest006
* @tc.desc: Verify circleprogress radius style can deal unNormally value.
*/
HWTEST_F(ArcTddTest, arcStyle006, TestSize.Level1)
{
    ArcTddTest::ComponentProgressStyleSetTest006();
}

/**
* @tc.name: ComponentProgressStyleSetTest008
* @tc.desc: Verify circleprogress startAngle style can set normal value.
*/
HWTEST_F(ArcTddTest, arcStyle008, TestSize.Level1)
{
    ArcTddTest::ComponentProgressStyleSetTest008();
}

/**
* @tc.name: ComponentProgressStyleSetTest009
* @tc.desc: Verify circleprogress startAngle style can deal unNormal value.
*/
HWTEST_F(ArcTddTest, arcStyle009, TestSize.Level1)
{
    ArcTddTest::ComponentProgressStyleSetTest009();
}

/**
* @tc.name: ComponentProgressStyleSetTest010
* @tc.desc: Verify circleprogress totalAngle style can set normal value.
*/
HWTEST_F(ArcTddTest, arcStyle010, TestSize.Level1)
{
    ArcTddTest::ComponentProgressStyleSetTest010();
}

/**
* @tc.name: ComponentProgressStyleSetTest011
* @tc.desc: Verify circleprogress totalAngle style can deal unNormal value.
*/
HWTEST_F(ArcTddTest, arcStyle011, TestSize.Level1)
{
    ArcTddTest::ComponentProgressStyleSetTest011();
}

/**
* @tc.name: ComponentProgressStyleSetTest012
* @tc.desc: Verify circleprogress lineColor style can set normal value.
*/
HWTEST_F(ArcTddTest, arcStyle012, TestSize.Level1)
{
    ArcTddTest::ComponentProgressStyleSetTest012();
}

/**
* @tc.name: ComponentProgressStyleSetTest013
* @tc.desc: Verify circleprogress lineColor style can deal unNormal value.
*/
HWTEST_F(ArcTddTest, arcStyle013, TestSize.Level1)
{
    ArcTddTest::ComponentProgressStyleSetTest013();
}

/**
* @tc.name: ComponentProgressStyleSetTest014
* @tc.desc: Verify circleprogress lineWidth style can set normal value.
*/
HWTEST_F(ArcTddTest, arcStyle014, TestSize.Level1)
{
    ArcTddTest::ComponentProgressStyleSetTest014();
}

/**
* @tc.name: ComponentProgressStyleSetTest015
* @tc.desc: Verify circleprogress lineWidth style can deal unNormal value.
*/
HWTEST_F(ArcTddTest, arcStyle015, TestSize.Level1)
{
    ArcTddTest::ComponentProgressStyleSetTest015();
}
#endif
}
}
