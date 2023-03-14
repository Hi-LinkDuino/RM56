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

#include <QApplication>

#include "graphic_config.h"
#include "main_widget.h"
#include "monitor.h"

extern void RunApp();
int main(int argc, char* argv[])
{
    QApplication uitest(argc, argv);
    OHOS::GraphicStartUp::Init();
    OHOS::Monitor::GetInstance()->InitHal();
    OHOS::Monitor::GetInstance()->InitFontEngine();
    OHOS::Monitor::GetInstance()->InitImageDecodeAbility();
    RunApp();
    OHOS::MainWidget mainWidget;
    mainWidget.resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    mainWidget.show();
    return uitest.exec();
}
