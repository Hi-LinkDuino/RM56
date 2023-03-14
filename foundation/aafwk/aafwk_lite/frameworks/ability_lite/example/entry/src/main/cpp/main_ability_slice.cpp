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

#include "main_ability_slice.h"

#include "ability_manager.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "next_ability_slice.h"
#include "root_view_helper.h"

namespace OHOS {
REGISTER_AS(MainAbilitySlice)

constexpr static int BUTTON1_POSITION_X = 50;
constexpr static int BUTTON1_POSITION_Y = 50;
constexpr static int BUTTON_WIDTH = 200;
constexpr static int BUTTON_HEIGHT = 30;
constexpr static int ROOT_VIEW_POSITION_X = 0;
constexpr static int ROOT_VIEW_POSITION_Y = 0;
constexpr static int ROOT_VIEW_WIDTH = 300;
constexpr static int ROOT_VIEW_HEIGHT = 500;
constexpr static uint8_t ROOT_VIEW_OPACITY = 255;
constexpr static uint8_t FONT_ID = 10;

int32_t IpcMsgHandler(int funcId, ElementName *elementName, SvcIdentity *serviceSid, void *storeArg)
{
    printf("IpcMsgHandler, funcId is %d, serviceSid is %p\n", funcId, serviceSid);
    printf("elementName is %s, %s\n", elementName->bundleName, elementName->abilityName);
    if (serviceSid == nullptr) {
        return 0;
    }
    printf("This is MainAbilitySlice\n");

    // push data
    IpcIo request;
    char data[IPC_IO_DATA_MAX];
    IpcIoInit(&request, data, IPC_IO_DATA_MAX, 0);
    IpcIoPushInt32(&request, 10);
    IpcIoPushInt32(&request, 6);

    // send and getReply
    IpcIo reply;
    uintptr_t ptr = 0;
    if (Transact(nullptr, *serviceSid, 0, &request, &reply, LITEIPC_FLAG_DEFAULT, &ptr) != LITEIPC_OK) {
        printf("transact error\n");
        return -1;
    }
    int result = IpcIoPopInt32(&reply);
    printf("execute add method, result is %d\n", result);
    if (ptr != 0) {
        FreeBuffer(nullptr, reinterpret_cast<void *>(ptr));
    }
    return 0;
}

void MainAbilitySlice::OnStart(const Want &want)
{
    AbilitySlice::OnStart(want);

    auto button1 = new UILabel();
    button1->SetPosition(BUTTON1_POSITION_X, BUTTON1_POSITION_Y);
    button1->SetText("鸿蒙应用框架 Main AS");
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

void MainAbilitySlice::OnStop()
{
    printf("MainAbilitySlice::OnStop\n");
    AbilitySlice::OnStop();
    DeleteViewChildren(rootView_);
}
}
