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

#include "acelite_config.h"
#include "chart_tdd_test.h"
#include "component_factory.h"
#include "js_app_environment.h"
#include "root_view.h"
#include "ui_chart.h"

namespace OHOS {
namespace ACELite {
const char * const BUNDLE1 =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container']\n"
    "            },[_c('chart',{\n"
    "                staticStyle:{width: '300px', height: '300px'},\n"
    "                attrs:{\n"
    "                    ref: 'chart',\n"
    "                    type: 'bar',\n"
    "                    datasets: [\n"
    "                        {\n"
    "                            fillColor: '#9c9c9c',\n"
    "                            data:[2478,2600,3000,3200,2500]\n"
    "                        }\n"
    "                    ],\n"
    "                    options:{\n"
    "                        xAxis: {\n"
    "                            min: 0,\n"
    "                            max: 10,\n"
    "                            axisTick: 5,\n"
    "                            display: false\n"
    "                        },\n"
    "                        yAxis: {\n"
    "                            min: 0,\n"
    "                            max: 5000,\n"
    "                            axisTick: 5,\n"
    "                            display: false\n"
    "                        }\n"
    "                    }\n"
    "                }\n"
    "            })]);\n"
    "        },\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: {\n"
    "                    width: '454px',\n"
    "                    height: '454px',\n"
    "                    justifyContent: 'center',\n"
    "                    alignItems: 'center'\n"
    "                }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

const char * const BUNDLE2 =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container']\n"
    "            },[_c('chart',{\n"
    "                staticStyle:{width: '300px', height: '300px'},\n"
    "                attrs:{\n"
    "                    ref: 'chart',\n"
    "                    type: 'line',\n"
    "                    datasets: [\n"
    "                        {\n"
    "                            fillColor: '#eeaaff',\n"
    "                            data:[2478,2600,3000,3200,2500]\n"
    "                        }\n"
    "                    ],\n"
    "                    options:{\n"
    "                        xAxis: {\n"
    "                            min: 0,\n"
    "                            max: 10,\n"
    "                            axisTick: 5,\n"
    "                            display: false\n"
    "                        },\n"
    "                        yAxis: {\n"
    "                            min: 0,\n"
    "                            max: 5000,\n"
    "                            axisTick: 5,\n"
    "                            display: false\n"
    "                        }\n"
    "                    }\n"
    "                }\n"
    "            })]);\n"
    "        },\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: {\n"
    "                    width: '454px',\n"
    "                    height: '454px',\n"
    "                    justifyContent: 'center',\n"
    "                    alignItems: 'center'\n"
    "                }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

const char * const BUNDLE3 =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container']\n"
    "            },[_c('chart',{\n"
    "                staticStyle:{width: '300px'},\n"
    "                dynamicStyle:{height: function height(){return _vm.heightVal}},\n"
    "                on: {\n"
    "                    click: _vm.changeHeight\n"
    "                },\n"
    "                attrs:{\n"
    "                    ref: 'chart',\n"
    "                    type: 'bar',\n"
    "                    datasets: [\n"
    "                        {\n"
    "                            fillColor: '#9c9c9c',\n"
    "                            data:[2478,2600,3000,3200,2500]\n"
    "                        }\n"
    "                    ],\n"
    "                    options:{\n"
    "                        xAxis: {\n"
    "                            min: 0,\n"
    "                            max: 10,\n"
    "                            axisTick: 5,\n"
    "                            display: false\n"
    "                        },\n"
    "                        yAxis: {\n"
    "                            min: 0,\n"
    "                            max: 5000,\n"
    "                            axisTick: 5,\n"
    "                            display: false\n"
    "                        }\n"
    "                    }\n"
    "                }\n"
    "            })]);\n"
    "        },\n"
    "        data:{heightVal: '200px'},\n"
    "        changeHeight: function changeHeight() {this.heightVal = '300px';},\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: {\n"
    "                    width: '454px',\n"
    "                    height: '454px',\n"
    "                    justifyContent: 'center',\n"
    "                    alignItems: 'center'\n"
    "                }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

const char * const BUNDLE4 =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container']\n"
    "            },[_c('chart',{\n"
    "                staticStyle:{width: '300px'},\n"
    "                dynamicStyle:{height: function height(){return _vm.heightVal}},\n"
    "                on: {\n"
    "                    click: _vm.changeHeight\n"
    "                },\n"
    "                attrs:{\n"
    "                    ref: 'chart',\n"
    "                    type: 'line',\n"
    "                    datasets: [\n"
    "                        {\n"
    "                            fillColor: '#eeaaff',\n"
    "                            data:[2478,2600,3000,3200,2500]\n"
    "                        }\n"
    "                    ],\n"
    "                    options:{\n"
    "                        xAxis: {\n"
    "                            min: 0,\n"
    "                            max: 10,\n"
    "                            axisTick: 5,\n"
    "                            display: false\n"
    "                        },\n"
    "                        yAxis: {\n"
    "                            min: 0,\n"
    "                            max: 5000,\n"
    "                            axisTick: 5,\n"
    "                            display: false\n"
    "                        }\n"
    "                    }\n"
    "                }\n"
    "            })]);\n"
    "        },\n"
    "        data:{heightVal: '200px'},\n"
    "        changeHeight: function changeHeight() {this.heightVal = '300px';},\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: {\n"
    "                    width: '454px',\n"
    "                    height: '454px',\n"
    "                    justifyContent: 'center',\n"
    "                    alignItems: 'center'\n"
    "                }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

void ChartTddTest::ComponentChartStyleValueTest001()
{
    int16_t defaultWidth = 300;
    int16_t defaultHeight = 300;
    VerifyWidthAndHeight(BUNDLE1, defaultWidth, defaultHeight);
}

void ChartTddTest::ComponentChartStyleValueTest002()
{
    int16_t defaultWidth = 300;
    int16_t defaultHeight = 300;
    VerifyWidthAndHeight(BUNDLE2, defaultWidth, defaultHeight);
}

void ChartTddTest::ComponentChartStyleValueTest003()
{
    int16_t changedHeight = 300;
    ChangeComponentHeight(BUNDLE3, changedHeight);
}

void ChartTddTest::ComponentChartStyleValueTest004()
{
    int16_t changedHeight = 300;
    ChangeComponentHeight(BUNDLE4, changedHeight);
}

void ChartTddTest::VerifyWidthAndHeight(const char *bundle, int16_t defaultWidth, int16_t defaultHeight)
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(bundle, strlen(bundle));
    UIChart *chart = reinterpret_cast<UIChart *>(GetViewByRef(page, "chart"));
    EXPECT_TRUE(chart != nullptr);
    if (chart != nullptr) {
        int16_t width = chart->GetWidth();
        EXPECT_TRUE(width == defaultWidth);
        int16_t height = chart->GetHeight();
        EXPECT_TRUE(height == defaultHeight);
        UIViewType viewType = chart->GetViewType();
        EXPECT_TRUE(viewType == UI_CHART);
    }
    DestroyPage(page);
    TDD_CASE_END();
}

void ChartTddTest::ChangeComponentHeight(const char *bundle, int16_t changedHeight)
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(bundle, strlen(bundle));
    UIChart *chart = reinterpret_cast<UIChart *>(GetViewByRef(page, "chart"));
    EXPECT_TRUE(chart != nullptr);
    if (chart != nullptr) {
        ClickByRef(page, "chart");
        int16_t height = chart->GetHeight();
        EXPECT_TRUE(height == changedHeight);
    }
    DestroyPage(page);
    TDD_CASE_END();
}

void ChartTddTest::RunTests()
{
    ComponentChartStyleValueTest001();
    ComponentChartStyleValueTest002();
    ComponentChartStyleValueTest003();
    ComponentChartStyleValueTest004();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name:ComponentChartStyleValueTest001
 * @tc.desc: Verify bar chart default style value.
 */
HWTEST_F(ChartTddTest, ChartStyle001, TestSize.Level1)
{
    ChartTddTest::ComponentChartStyleValueTest001();
}

/**
 * @tc.name:ComponentChartStyleValueTest002
 * @tc.desc: Verify line chart defaule style value.
 */
HWTEST_F(ChartTddTest, ChartStyle002, TestSize.Level1)
{
    ChartTddTest::ComponentChartStyleValueTest002();
}

/**
 * @tc.name:ComponentChartStyleValueTest003
 * @tc.desc: change bar chart height.
 */
HWTEST_F(ChartTddTest, ChartStyle003, TestSize.Level1)
{
    ChartTddTest::ComponentChartStyleValueTest003();
}

/**
 * @tc.name:ComponentChartStyleValueTest003
 * @tc.desc: change line chart height.
 */
HWTEST_F(ChartTddTest, ChartStyle004, TestSize.Level1)
{
    ChartTddTest::ComponentChartStyleValueTest004();
}

#endif
} // namespace ACELite
} // namespace OHOS
