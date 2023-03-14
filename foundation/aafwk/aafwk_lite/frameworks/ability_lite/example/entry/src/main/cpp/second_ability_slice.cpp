/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "second_ability_slice.h"

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "root_view_helper.h"

namespace OHOS {
REGISTER_AS(SecondAbilitySlice)

constexpr static int BUTTON1_POSITION_X = 50;
constexpr static int BUTTON1_POSITION_Y = 50;
constexpr static int BUTTON_WIDTH = 200;
constexpr static int BUTTON_HEIGHT = 50;
constexpr static int ROOT_VIEW_POSITION_X = 0;
constexpr static int ROOT_VIEW_POSITION_Y = 0;
constexpr static int ROOT_VIEW_WIDTH = 300;
constexpr static int ROOT_VIEW_HEIGHT = 300;
constexpr static uint8_t ROOT_VIEW_OPACITY = 255;
constexpr static uint8_t FONT_ID = 10;

void SecondAbilitySlice::OnStart(const Want &want)
{
    printf("SecondAbilitySlice::OnStart - %s\n", reinterpret_cast<char *>(want.data));
    AbilitySlice::OnStart(want);

    auto button1 = new UILabel();
    button1->SetPosition(BUTTON1_POSITION_X, BUTTON1_POSITION_Y);
    button1->SetText("Next AA");
    button1->Resize(BUTTON_WIDTH, BUTTON_HEIGHT);
    button1->SetFontId(FONT_ID);
    button1->SetStyle(STYLE_TEXT_COLOR,  Color::Black().full);
    button1->SetStyle(STYLE_TEXT_OPA,  ROOT_VIEW_OPACITY);
    button1->SetStyle(STYLE_BACKGROUND_COLOR,  Color::Yellow().full);
    button1->SetStyle(STYLE_BACKGROUND_OPA,  ROOT_VIEW_OPACITY);

    rootView_ = RootView::GetWindowRootView();
    rootView_->SetPosition(ROOT_VIEW_POSITION_X, ROOT_VIEW_POSITION_Y);
    rootView_->Resize(ROOT_VIEW_WIDTH, ROOT_VIEW_HEIGHT);
    rootView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Olive().full);

    rootView_->Add(button1);

    SetUIContent(rootView_);
}

void SecondAbilitySlice::OnStop()
{
    printf("SecondAbilitySlice::OnStop\n");
    AbilitySlice::OnStop();
    DeleteViewChildren(rootView_);
}
}
