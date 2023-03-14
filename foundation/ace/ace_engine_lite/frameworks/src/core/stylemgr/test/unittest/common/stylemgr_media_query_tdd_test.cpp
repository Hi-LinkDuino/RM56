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

#include "stylemgr_media_query_tdd_test.h"

#include "js_debugger_config.h"
#include "key_parser.h"
#include "product_adapter.h"
#include "scope_js_value.h"
#include "system_info.h"

namespace OHOS {
namespace ACELite {
namespace StyleSheetCSSData {
const char CSS_DATA_TEST_001[] =
    "{"
    "    \"classSelectors\": {"
    "      \"container\": {"
    "            \"width\": 400,"
    "            \"height\": 400"
    "        },"
    "        \"button\": {"
    "            \"width\": 300,"
    "            \"height\": 80"
    "        }"
    "    },"
    "    \"@media\": ["
    "        {"
    "            \"condition\": \"screen and (device-type: phone)\","
    "            \"classSelectors\": {"
    "                \"container\": {"
    "                    \"backgroundColor\": 8421504"
    "                },"
    "                \"containerNew\": {"
    "                    \"width\": 400,"
    "                    \"height\": 400"
    "                }"
    "            }"
    "        },"
    "        {"
    "            \"condition\": \"screen and (height: 1024)\","
    "            \"classSelectors\": {"
    "                \"button\": {"
    "                    \"borderTopWidth\": 5,"
    "                    \"borderRightWidth\": 5,"
    "                    \"borderBottomWidth\": 5,"
    "                    \"borderLeftWidth\": 5"
    "                }"
    "            }"
    "        }"
    "    ]"
    "}";

const char CSS_DATA_TEST_002[] =
    "{"
    "    \"classSelectors\": {"
    "      \"container\": {"
    "            \"width\": 390,"
    "            \"height\": 390,"
    "            \"justifyContent\": \"center\","
    "            \"alignItems\": \"center\","
    "            \"backgroundColor\": 32768"
    "        },"
    "        \"button\": {"
    "            \"width\": 300,"
    "            \"height\": 80,"
    "            \"borderTopWidth\": 2,"
    "            \"borderRightWidth\": 2,"
    "            \"borderBottomWidth\": 2,"
    "            \"borderLeftWidth\": 2,"
    "            \"color\": 16711680"
    "        }"
    "    },"
    "    \"@media\": ["
    "        {"
    "            \"condition\": \"screen and (device-type: smartVision)\","
    "            \"classSelectors\": {"
    "                \"container\": {"
    "                    \"backgroundColor\": 8421504"
    "                },"
    "                \"containerNew\": {"
    "                    \"width\": 400,"
    "                    \"height\": 400"
    "                }"
    "            }"
    "        }"
    "    ]"
    "}";

const char CSS_DATA_TEST_003[] =
    "{"
    "    \"classSelectors\": {"
    "      \"container\": {"
    "            \"width\": 390,"
    "            \"height\": 390"
    "        },"
    "        \"button\": {"
    "            \"width\": 300,"
    "            \"height\": 80"
    "        }"
    "    },"
    "    \"@media\": ["
    "        {"
    "            \"condition\": \"screen and (device-type: smartVision)\","
    "            \"classSelectors\": {"
    "                \"containerNew\": {"
    "                    \"width\": 400,"
    "                    \"height\": 400"
    "                }"
    "            }"
    "        }"
    "    ]"
    "}";

const char CSS_DATA_TEST_004[] =
    "{"
    "    \"classSelectors\": {"
    "      \"container\": {"
    "            \"width\": 390,"
    "            \"height\": 390,"
    "            \"justifyContent\": \"center\","
    "            \"alignItems\": \"center\","
    "            \"backgroundColor\": 32768"
    "        }"
    "    },"
    "    \"@media\": ["
    "        {"
    "            \"condition\": \"screen and (device-type: smartVision)\","
    "            \"classSelectors\": {"
    "                \"container\": {"
    "                    \"backgroundColor\": 215026"
    "                }"
    "            }"
    "        }"
    "    ]"
    "}";

const char CSS_DATA_TEST_005[] =
    "{"
    "    \"classSelectors\": {"
    "      \"container\": {"
    "            \"width\": 390,"
    "            \"height\": 390"
    "        }"
    "    },"
    "    \"@media\": ["
    "        {"
    "            \"condition\": \"screen and (device-type: smartVision)\","
    "            \"classSelectors\": {"
    "                \"container\": {"
    "                    \"backgroundColor\": 8421504"
    "                }"
    "            }"
    "        }"
    "    ]"
    "}";

const char CSS_DATA_TEST_006[] =
    "{"
    "    \"classSelectors\": {"
    "      \"container\": {"
    "            \"width\": 390,"
    "            \"height\": 390,"
    "            \"justifyContent\": \"center\","
    "            \"alignItems\": \"center\","
    "            \"backgroundColor\": 32768"
    "        },"
    "        \"button\": {"
    "            \"width\": 300,"
    "            \"height\": 80,"
    "            \"borderTopWidth\": 2,"
    "            \"borderRightWidth\": 2,"
    "            \"borderBottomWidth\": 2,"
    "            \"borderLeftWidth\": 2,"
    "            \"color\": 16711680"
    "        }"
    "    },"
    "    \"@media\": ["
    "        {"
    "            \"condition\": \"screen and (device-type: smartVision)\","
    "            \"classSelectors\": {"
    "                \"container\": {"
    "                    \"backgroundColor\": 8421504"
    "                },"
    "                \"button\": {"
    "                    \"width\": 400,"
    "                    \"height\": 400"
    "                }"
    "            }"
    "        }"
    "    ]"
    "}";

const char CSS_DATA_TEST_007[] =
    "{"
    "    \"classSelectors\": {"
    "      \"container\": {"
    "            \"width\": 390,"
    "            \"height\": 390,"
    "            \"backgroundColor\": 32768"
    "        }"
    "    },"
    "    \"@media\": ["
    "        {"
    "            \"condition\": \"screen and (device-type: smartVision)\","
    "            \"classSelectors\": {"
    "                \"container\": {"
    "                    \"backgroundColor\": 8421504"
    "                },"
    "                \"container\": {"
    "                    \"width\": 400,"
    "                    \"height\": 400"
    "                }"
    "            }"
    "        }"
    "    ]"
    "}";

const char CSS_DATA_TEST_008[] =
    "{"
    "    \"classSelectors\": {"
    "      \"container\": {"
    "            \"width\": 390,"
    "            \"height\": 390,"
    "            \"backgroundColor\": 32768"
    "        },"
    "        \"button\": {"
    "            \"width\": 300,"
    "            \"height\": 80,"
    "            \"borderTopWidth\": 2,"
    "            \"borderRightWidth\": 2,"
    "            \"borderBottomWidth\": 2,"
    "            \"borderLeftWidth\": 2,"
    "            \"color\": 16711680"
    "        }"
    "    },"
    "    \"@media\": ["
    "        {"
    "            \"condition\": \"screen and (device-type: smartVision)\","
    "            \"classSelectors\": {"
    "                \"container\": {"
    "                    \"backgroundColor\": 121301"
    "                }"
    "            }"
    "        },"
    "        {"
    "            \"condition\": \"screen and (height: 466)\","
    "            \"classSelectors\": {"
    "                \"button\": {"
    "                    \"borderTopWidth\": 5,"
    "                    \"borderRightWidth\": 5,"
    "                    \"borderBottomWidth\": 5,"
    "                    \"borderLeftWidth\": 5"
    "                }"
    "            }"
    "        }"
    "    ]"
    "}";

const char CSS_DATA_TEST_009[] =
    "{"
    "    \"classSelectors\": {"
    "      \"container\": {"
    "            \"width\": 390,"
    "            \"height\": 390,"
    "            \"backgroundColor\": 32768"
    "        }"
    "    },"
    "    \"@media\": ["
    "        {"
    "            \"condition\": \"screen and (device-type: smartVision)\","
    "            \"classSelectors\": {"
    "                \"container\": {"
    "                    \"backgroundColor\": 8421504"
    "                }"
    "            }"
    "        },"
    "        {"
    "            \"condition\": \"screen and (height: 960)\","
    "            \"classSelectors\": {"
    "                \"container\": {"
    "                    \"backgroundColor\": 16711680"
    "                }"
    "            }"
    "        }"
    "    ]"
    "}";

const char CSS_DATA_TEST_010[] =
    "{"
    "    \"@media\": ["
    "        {"
    "            \"condition\": \"screen and (device-type: smartVision)\","
    "            \"classSelectors\": {"
    "                \"container\": {"
    "                    \"backgroundColor\": 123456"
    "                }"
    "            }"
    "        }"
    "    ],"
    "    \"classSelectors\": {"
    "      \"container\": {"
    "            \"width\": 390,"
    "            \"height\": 390,"
    "            \"backgroundColor\": 8421504"
    "        }"
    "    }"
    "}";

const char CSS_DATA_TEST_011[] =
    "{"
    "    \"@media\": ["
    "        {"
    "            \"condition\": \"screen and (device-type: smartVision)\","
    "            \"classSelectors\": {"
    "                \"container\": {"
    "                    \"backgroundColor\": 8421504"
    "                },"
    "                \"containerNew\": {"
    "                    \"width\": 400,"
    "                    \"height\": 400"
    "                }"
    "            }"
    "        },"
    "        {"
    "            \"condition\": \"screen and (height: 1024)\","
    "            \"classSelectors\": {"
    "                \"button\": {"
    "                    \"borderTopWidth\": 5,"
    "                    \"borderRightWidth\": 5,"
    "                    \"borderBottomWidth\": 5,"
    "                    \"borderLeftWidth\": 5"
    "                }"
    "            }"
    "        }"
    "    ]"
    "}";

const char CSS_DATA_TEST_012[] =
    "{"
    "    \"classSelectors\": {"
    "      \"container\": {"
    "            \"width\": 400,"
    "            \"height\": 400"
    "        },"
    "        \"button\": {"
    "            \"width\": 300,"
    "            \"height\": 80"
    "        }"
    "    },"
    "    \"@media\": ["
    "        {"
    "            \"condition\": \"screen and (device-type: smartVision)\","
    "            \"classSelectors\": {"
    "                \"container\": {},"
    "                \"containerNew\": {"
    "                    \"width\": 400,"
    "                    \"height\": 400"
    "                }"
    "            }"
    "        }"
    "    ]"
    "}";
} // namespace StyleSheetCSSData

void StyleMgrMediaQueryTddTest::SetUp()
{
    SystemInfo::GetInstance().Initialize();
    Debugger::GetInstance().SetupJSContext();
    jerry_init(JERRY_INIT_EMPTY);
}

void StyleMgrMediaQueryTddTest::TearDown()
{
    jerry_cleanup();
    Debugger::GetInstance().ReleaseJSContext();
}

int StyleMgrMediaQueryTddTest::GetAppStyleCount(const AppStyleList *list) const
{
    if (list == nullptr) {
        return 0;
    }

    int count = 0;
    const AppStyle *style = list->GetFirst();
    while (style != nullptr) {
        count++;
        style = style->GetNext();
    }

    return count;
}

int StyleMgrMediaQueryTddTest::CalcAppStyleCount(const AppStyleSheet *list) const
{
    if (list == nullptr) {
        return 0;
    }
    return GetAppStyleCount(list->idSelectors_) + GetAppStyleCount(list->classSelectors_) +
           GetAppStyleCount(list->keyFrameSelectors_);
}

int StyleMgrMediaQueryTddTest::GetAppStyleItemCount(const AppStyle *list) const
{
    if (list == nullptr) {
        return 0;
    }

    int count = 0;
    const AppStyleItem *styleItem = list->GetFirst();
    while (styleItem != nullptr) {
        count++;
        styleItem = styleItem->GetNext();
    }

    return count;
}

const AppStyle *StyleMgrMediaQueryTddTest::GetStyleFromSelector(const AppStyleList *styleList,
                                                                const char *selectorName) const
{
    if (styleList == nullptr || selectorName == nullptr) {
        return nullptr;
    }
    const AppStyle *style = styleList->GetFirst();
    while (style != nullptr) {
        const char *styleName = style->GetStyleName();
        if (styleName != nullptr) {
            if (strcmp(styleName, selectorName) == 0) {
                return style;
            }
        }
        style = style->GetNext();
    }
    return nullptr;
}

const AppStyle *StyleMgrMediaQueryTddTest::GetStyleFromSelectorList(const AppStyleSheet *list,
                                                                    const char *selectorName) const
{
    if (list == nullptr || selectorName == nullptr) {
        return nullptr;
    }

    const AppStyle *selector = GetStyleFromSelector(list->classSelectors_, selectorName);
    selector = (selector != nullptr) ? selector : GetStyleFromSelector(list->idSelectors_, selectorName);
    selector = (selector != nullptr) ? selector : GetStyleFromSelector(list->keyFrameSelectors_, selectorName);

    return selector;
}

int StyleMgrMediaQueryTddTest::GetItemCountOfSelector(const AppStyleSheet *list, const char *selectorName) const
{
    if (list == nullptr || selectorName == nullptr) {
        return 0;
    }

    const AppStyle *selector = GetStyleFromSelectorList(list, selectorName);
    return GetAppStyleItemCount(selector);
}

const AppStyleItem *StyleMgrMediaQueryTddTest::GetStyleItemFromStyle(const AppStyle *style, const char *itemName) const
{
    if (style == nullptr || itemName == nullptr) {
        return nullptr;
    }
    uint16_t targetItemNameKey = KeyParser::ParseKeyId(itemName, strlen(itemName));
    if (targetItemNameKey == K_UNKNOWN) {
        return nullptr;
    }
    const AppStyleItem *styleItem = style->GetFirst();
    while (styleItem != nullptr) {
        if (styleItem->GetPropNameId() == targetItemNameKey) {
            return styleItem;
        }
        styleItem = styleItem->GetNext();
    }
    return nullptr;
}

const AppStyleItem *StyleMgrMediaQueryTddTest::GetStyleItemByName(const AppStyleSheet *list,
                                                                  const char *selectorName,
                                                                  const char *itemName) const
{
    if (list == nullptr || selectorName == nullptr || itemName == nullptr) {
        return nullptr;
    }

    const AppStyle *style = GetStyleFromSelectorList(list, selectorName);
    if (style == nullptr) {
        return nullptr;
    }
    const AppStyleItem *styleItem = GetStyleItemFromStyle(style, itemName);
    return styleItem;
}

/**
 * @tc.name: StyleMgrMediaQueryCSS001
 * @tc.desc: Verify the style sheet with media query block initialization
 */
HWTEST_F(StyleMgrMediaQueryTddTest, StyleMgrMediaQueryCSS001, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare test resource
     */
    jerry_value_t styleSheetObj =
        jerry_json_parse(reinterpret_cast<const jerry_char_t *>(StyleSheetCSSData::CSS_DATA_TEST_001),
                         strlen(StyleSheetCSSData::CSS_DATA_TEST_001));
    ScopeJSValue autoReleaseV(styleSheetObj);
    AppStyleManager styleManager;
    styleManager.InitStyleSheet(styleSheetObj);
    const AppStyleSheet *styleSheet = styleManager.GetStyleSheet();
    /**
     * @tc.steps: step2. check count
     */
    const int targetCount = 2;
    int count = CalcAppStyleCount(styleSheet);
    EXPECT_EQ(count, targetCount);
}

/**
 * @tc.name: StyleMgrMediaQueryCSS002
 * @tc.desc: Verify the style sheet with media query block initialization
 */
HWTEST_F(StyleMgrMediaQueryTddTest, StyleMgrMediaQueryCSS002, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare test resource
     */
    AppStyleManager styleMgr;
    jerry_value_t styleSheetObject =
        jerry_json_parse(reinterpret_cast<const jerry_char_t *>(StyleSheetCSSData::CSS_DATA_TEST_002),
                         strlen(StyleSheetCSSData::CSS_DATA_TEST_002));
    ScopeJSValue autoReleaseValue(styleSheetObject);
    /**
     * @tc.steps: step2. initialize style sheet list
     */
    styleMgr.InitStyleSheet(styleSheetObject);
    const AppStyleSheet *styleSheet = styleMgr.GetStyleSheet();
    /**
     * @tc.steps: step3. check count
     */
    int targetCount = 3;
    int count = CalcAppStyleCount(styleSheet);
    EXPECT_EQ(count, targetCount);
    /**
     * @tc.steps: step4. the matched media query styles should be added
     */
    const AppStyle *containerNewSelector = GetStyleFromSelectorList(styleSheet, "containerNew");
    EXPECT_NE(containerNewSelector, nullptr);

    int containerSelectorItemCount = GetItemCountOfSelector(styleSheet, "container");
    targetCount = 5;
    EXPECT_EQ(containerSelectorItemCount, targetCount);

    const AppStyleItem *backgroundColorItem = GetStyleItemByName(styleSheet, "container", "backgroundColor");
    EXPECT_NE(backgroundColorItem, nullptr);
    if (backgroundColorItem != nullptr) {
        int32_t targetValue = 8421504;
        int32_t backgroundColorValue = backgroundColorItem->GetNumValue();
        EXPECT_EQ(targetValue, backgroundColorValue);
    }
}

/**
 * @tc.name: StyleMgrMediaQueryCSS003
 * @tc.desc: Verify the style sheet with media query block initialization
 */
HWTEST_F(StyleMgrMediaQueryTddTest, StyleMgrMediaQueryCSS003, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare test resource
     */
    jerry_value_t styleSheetObject =
        jerry_json_parse(reinterpret_cast<const jerry_char_t *>(StyleSheetCSSData::CSS_DATA_TEST_003),
                         strlen(StyleSheetCSSData::CSS_DATA_TEST_003));
    ScopeJSValue scopeJSValue(styleSheetObject);
    AppStyleManager styleManagerInstance;
    styleManagerInstance.InitStyleSheet(styleSheetObject);
    const AppStyleSheet *styleSheet = styleManagerInstance.GetStyleSheet();
    /**
     * @tc.steps: step2. check total cound
     */
    const int count = 3;
    EXPECT_EQ(CalcAppStyleCount(styleSheet), count);
}

/**
 * @tc.name: StyleMgrMediaQueryCSS004
 * @tc.desc: Verify the style sheet with media query block initialization
 */
HWTEST_F(StyleMgrMediaQueryTddTest, StyleMgrMediaQueryCSS004, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare test resource
     */
    AppStyleManager appStyleManagerInner;
    jerry_value_t styleSheetObject =
        jerry_json_parse(reinterpret_cast<const jerry_char_t *>(StyleSheetCSSData::CSS_DATA_TEST_004),
                         strlen(StyleSheetCSSData::CSS_DATA_TEST_004));
    ScopeJSValue scopeValue(styleSheetObject);
    appStyleManagerInner.InitStyleSheet(styleSheetObject);
    const AppStyleSheet *styleSheet = appStyleManagerInner.GetStyleSheet();
    /**
     * @tc.steps: step2. check the result
     */
    const AppStyleItem *item = GetStyleItemByName(styleSheet, "container", "backgroundColor");
    EXPECT_NE(item, nullptr);
    if (item != nullptr) {
        int32_t targetValue = 215026;
        int32_t backgroundColorValue = item->GetNumValue();
        EXPECT_EQ(targetValue, backgroundColorValue);
    }
}

/**
 * @tc.name: StyleMgrMediaQueryCSS005
 * @tc.desc: Verify the style sheet with media query block initialization
 */
HWTEST_F(StyleMgrMediaQueryTddTest, StyleMgrMediaQueryCSS005, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare test resource
     */
    jerry_value_t styleSheetObject =
        jerry_json_parse(reinterpret_cast<const jerry_char_t *>(StyleSheetCSSData::CSS_DATA_TEST_005),
                         strlen(StyleSheetCSSData::CSS_DATA_TEST_005));
    ScopeJSValue jsValue(styleSheetObject); // auto release
    AppStyleManager styleManagerInstance;
    styleManagerInstance.InitStyleSheet(styleSheetObject);
    const AppStyleSheet *styleSheet = styleManagerInstance.GetStyleSheet();
    /**
     * @tc.steps: step2. check the result
     */
    int targetCount = 1;
    EXPECT_EQ(CalcAppStyleCount(styleSheet), targetCount);
    int containerSelectorItemCount = GetItemCountOfSelector(styleSheet, "container");
    targetCount = 3;
    EXPECT_EQ(containerSelectorItemCount, targetCount);
}

/**
 * @tc.name: StyleMgrMediaQueryCSS006
 * @tc.desc: Verify the style sheet with media query block initialization
 */
HWTEST_F(StyleMgrMediaQueryTddTest, StyleMgrMediaQueryCSS006, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare test resource
     */
    jerry_value_t cssObject =
        jerry_json_parse(reinterpret_cast<const jerry_char_t *>(StyleSheetCSSData::CSS_DATA_TEST_006),
                         strlen(StyleSheetCSSData::CSS_DATA_TEST_006));
    ScopeJSValue scopeValue(cssObject);
    AppStyleManager appStyleManager;
    appStyleManager.InitStyleSheet(cssObject);
    const AppStyleSheet *styleSheet = appStyleManager.GetStyleSheet();
    /**
     * @tc.steps: step2. check the result
     */
    int targetCount = 2;
    EXPECT_EQ(CalcAppStyleCount(styleSheet), targetCount);
    int containerItemCount = 5;
    EXPECT_EQ(GetItemCountOfSelector(styleSheet, "container"), containerItemCount);
    int buttonItemCount = 7;
    EXPECT_EQ(GetItemCountOfSelector(styleSheet, "button"), buttonItemCount);
}

/**
 * @tc.name: StyleMgrMediaQueryCSS007
 * @tc.desc: Verify the style sheet with media query block initialization
 */
HWTEST_F(StyleMgrMediaQueryTddTest, StyleMgrMediaQueryCSS007, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare test resource
     */
    jerry_value_t cssDataJSValue =
        jerry_json_parse(reinterpret_cast<const jerry_char_t *>(StyleSheetCSSData::CSS_DATA_TEST_007),
                         strlen(StyleSheetCSSData::CSS_DATA_TEST_007));
    ScopeJSValue autoReleaseValue(cssDataJSValue);
    AppStyleManager appStyleManager;
    appStyleManager.InitStyleSheet(cssDataJSValue);
    const AppStyleSheet *styleSheet = appStyleManager.GetStyleSheet();
    /**
     * @tc.steps: step2. check the result
     */
    int targetCount = 1;
    EXPECT_EQ(CalcAppStyleCount(styleSheet), targetCount);
    int containerItemCount = 3;
    EXPECT_EQ(GetItemCountOfSelector(styleSheet, "container"), containerItemCount);
    const AppStyleItem *widthStyleItem = GetStyleItemByName(styleSheet, "container", "width");
    EXPECT_NE(widthStyleItem, nullptr);
    if (widthStyleItem != nullptr) {
        int32_t targetValue = 400;
        int32_t widthValue = widthStyleItem->GetNumValue();
        EXPECT_EQ(targetValue, widthValue);
    }
}

/**
 * @tc.name: StyleMgrMediaQueryCSS008
 * @tc.desc: Verify the style sheet with media query block initialization
 */
HWTEST_F(StyleMgrMediaQueryTddTest, StyleMgrMediaQueryCSS008, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare test resource
     */
    const uint16_t screenSize = 466;
    ProductAdapter::SetScreenSize(screenSize, screenSize);
    SystemInfo::GetInstance().Initialize();
    jerry_value_t styleSheetObject =
        jerry_json_parse(reinterpret_cast<const jerry_char_t *>(StyleSheetCSSData::CSS_DATA_TEST_008),
                         strlen(StyleSheetCSSData::CSS_DATA_TEST_008));
    ScopeJSValue autoReleaseValue(styleSheetObject);
    AppStyleManager appStyleManagerObject;
    appStyleManagerObject.InitStyleSheet(styleSheetObject);
    const AppStyleSheet *styleSheet = appStyleManagerObject.GetStyleSheet();
    /**
     * @tc.steps: step2. check the result
     */
    int targetCount = 2;
    EXPECT_EQ(CalcAppStyleCount(styleSheet), targetCount);
    const AppStyleItem *backgroundColorStyleItem = GetStyleItemByName(styleSheet, "container", "backgroundColor");
    EXPECT_NE(backgroundColorStyleItem, nullptr);
    if (backgroundColorStyleItem != nullptr) {
        int32_t targetValue = 121301;
        int32_t backgroundColorValue = backgroundColorStyleItem->GetNumValue();
        EXPECT_EQ(targetValue, backgroundColorValue);
    }
    const AppStyleItem *borderWidthItem = GetStyleItemByName(styleSheet, "button", "borderTopWidth");
    EXPECT_NE(borderWidthItem, nullptr);
    if (borderWidthItem != nullptr) {
        int32_t targetValue = 5;
        int32_t borderWidthValue = borderWidthItem->GetNumValue();
        EXPECT_EQ(targetValue, borderWidthValue);
    }
}

/**
 * @tc.name: StyleMgrMediaQueryCSS009
 * @tc.desc: Verify the style sheet with media query block initialization
 */
HWTEST_F(StyleMgrMediaQueryTddTest, StyleMgrMediaQueryCSS009, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare test resource
     */
    const uint16_t screenWidth = 480;
    const uint16_t screenHeight = 960;
    ProductAdapter::SetScreenSize(screenWidth, screenHeight);
    SystemInfo::GetInstance().Initialize();
    jerry_value_t styleSheetObject =
        jerry_json_parse(reinterpret_cast<const jerry_char_t *>(StyleSheetCSSData::CSS_DATA_TEST_009),
                         strlen(StyleSheetCSSData::CSS_DATA_TEST_009));
    ScopeJSValue scopeValue(styleSheetObject);
    AppStyleManager styleMgr;
    styleMgr.InitStyleSheet(styleSheetObject);
    const AppStyleSheet *styleSheet = styleMgr.GetStyleSheet();
    /**
     * @tc.steps: step2. check the result
     */
    const AppStyleItem *backgroundColorItem = GetStyleItemByName(styleSheet, "container", "backgroundColor");
    EXPECT_NE(backgroundColorItem, nullptr);
    if (backgroundColorItem != nullptr) {
        int32_t targetColorValue = 16711680;
        int32_t backgroundColorValue = backgroundColorItem->GetNumValue();
        EXPECT_EQ(targetColorValue, backgroundColorValue);
    }
}

/**
 * @tc.name: StyleMgrMediaQueryCSS010
 * @tc.desc: Verify the style sheet with media query block initialization
 */
HWTEST_F(StyleMgrMediaQueryTddTest, StyleMgrMediaQueryCSS010, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare test resource
     */
    jerry_value_t styleListJSValue =
        jerry_json_parse(reinterpret_cast<const jerry_char_t *>(StyleSheetCSSData::CSS_DATA_TEST_010),
                         strlen(StyleSheetCSSData::CSS_DATA_TEST_010));
    ScopeJSValue scopeValue(styleListJSValue);
    AppStyleManager appStyleManager;
    appStyleManager.InitStyleSheet(styleListJSValue);
    const AppStyleSheet *styleSheet = appStyleManager.GetStyleSheet();
    /**
     * @tc.steps: step2. check the result
     */
    const AppStyleItem *backgroundColorItem = GetStyleItemByName(styleSheet, "container", "backgroundColor");
    EXPECT_NE(backgroundColorItem, nullptr);
    if (backgroundColorItem != nullptr) {
        int32_t targetColorValue = 123456;
        EXPECT_EQ(targetColorValue, backgroundColorItem->GetNumValue());
    }
}

/**
 * @tc.name: StyleMgrMediaQueryCSS011
 * @tc.desc: Verify the style sheet with media query block initialization
 */
HWTEST_F(StyleMgrMediaQueryTddTest, StyleMgrMediaQueryCSS011, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare test resource
     */
    const uint16_t screenSizeWidth = 960;
    const uint16_t screenSizeHeight = 960;
    ProductAdapter::SetScreenSize(screenSizeWidth, screenSizeHeight);
    AppStyleManager styleMgrInstance;
    jerry_value_t styleSheetObject =
        jerry_json_parse(reinterpret_cast<const jerry_char_t *>(StyleSheetCSSData::CSS_DATA_TEST_011),
                         strlen(StyleSheetCSSData::CSS_DATA_TEST_011));
    ScopeJSValue autoReleaseValue(styleSheetObject);
    styleMgrInstance.InitStyleSheet(styleSheetObject);
    const AppStyleSheet *styleSheet = styleMgrInstance.GetStyleSheet();
    /**
     * @tc.steps: step2. check the result
     */
    int targetCount = 2;
    EXPECT_EQ(CalcAppStyleCount(styleSheet), targetCount);
}

/**
 * @tc.name: StyleMgrMediaQueryCSS012
 * @tc.desc: Verify the style sheet with media query block initialization
 */
HWTEST_F(StyleMgrMediaQueryTddTest, StyleMgrMediaQueryCSS012, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare test resource
     */
    jerry_value_t cssData =
        jerry_json_parse(reinterpret_cast<const jerry_char_t *>(StyleSheetCSSData::CSS_DATA_TEST_012),
                         strlen(StyleSheetCSSData::CSS_DATA_TEST_012));
    ScopeJSValue autoReleaseValue(cssData);
    AppStyleManager appStyleManager;
    appStyleManager.InitStyleSheet(cssData);
    const AppStyleSheet *styleSheet = appStyleManager.GetStyleSheet();
    /**
     * @tc.steps: step2. check the result
     */
    int targetCount = 3;
    EXPECT_EQ(CalcAppStyleCount(styleSheet), targetCount);
    int containerItemCount = 2;
    EXPECT_EQ(GetItemCountOfSelector(styleSheet, "container"), containerItemCount);
}
} // namespace ACELite
} // namespace OHOS
