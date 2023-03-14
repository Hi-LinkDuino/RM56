/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "ui_auto_test_render.h"
#include "ui_test_render.h"

namespace OHOS {
void UIAutoTestRender::Reset() const
{
    ResetMainMenu();
    EnterSubMenu(UI_TEST_RENDER_ID);
}

void UIAutoTestRender::RunTestList()
{
    Reset();
    UIKitRenderTestRender001();
    UIKitRenderTestRenderMeasure001();
}

void UIAutoTestRender::UIKitRenderTestRender001() const
{
    const char* fileName = "ui_test_render_001.bmp";
    CompareByBinary(fileName);
}

void UIAutoTestRender::UIKitRenderTestRenderMeasure001() const
{
    ClickViewById(UI_TEST_RENDER_UPDATA_BUTTON_ID_01);
    const char* fileName = "ui_test_render_measure_001.bmp";
    CompareByBinary(fileName);
}
} // namespace OHOS
