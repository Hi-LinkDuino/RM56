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

#include <iostream>

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <Qt>

#include "ability_manager_interface.h"
#include "child_widget.h"
#include "graphic_startup.h"
#include "js_ability.h"
#include "js_debugger_config.h"
#include "main_widget.h"
#include "monitor.h"
#include "product_adapter.h"
#include "simulator_config.h"

namespace  {
    void InitUIkit()
    {
        OHOS::GraphicStartUp::Init();
        OHOS::Monitor::GetInstance()->InitHal();
        OHOS::Monitor::GetInstance()->InitFontEngine();
        OHOS::Monitor::GetInstance()->InitImageDecodeAbility();
    }

    void SetJSDebuggerConfig(int32_t defaultHeapSize)
    {
        const int16_t bytes = 1024;
        OHOS::ACELite::DebuggerConfig jsDebuggerConfig;
        jsDebuggerConfig.startDebuggerServer = false;
        jsDebuggerConfig.snapshotMode = false;
        jsDebuggerConfig.heapSize = defaultHeapSize * bytes;
        OHOS::ACELite::Debugger::GetInstance().ConfigEngineDebugger(jsDebuggerConfig);
    }

    void InitPage(OHOS::MainWidget *mainWidget, int16_t jsWindowHeight, int16_t jsWindowWidth, int16_t childPageHeight)
    {
        QString jsBundlePath = SimulatorConfig::GetInstance().GetConfigValue(CONFIG_KEY_JSBUNDLE_PATH);
        if (jsBundlePath.isNull() || jsBundlePath.isEmpty()) {
            jsBundlePath = "";
        } else {
            QFileInfo *file = new QFileInfo(jsBundlePath);
            if (file->exists() == false) {
                jsBundlePath = "";
            }
        }
        int16_t defaultHeapSize = DEFAULT_JSHEAP_SIZE; // KB
        QString jsHeapSize = SimulatorConfig::GetInstance().GetConfigValue(CONFIG_KEY_JSHEAP_SIZE);
        if (!jsHeapSize.isNull() && !jsHeapSize.isEmpty()) {
            int tempSize = jsHeapSize.toInt();
            if (tempSize >= MIN_JSHEAP_SIZE && tempSize <= MAX_JSHEAP_SIZE) {
                defaultHeapSize = tempSize;
            }
        }
        ChildWidget *childWidget = new ChildWidget(mainWidget, jsBundlePath, QString::number(defaultHeapSize));
        childWidget->setGeometry(QRect(0, jsWindowHeight, jsWindowWidth, childPageHeight));
        SetJSDebuggerConfig(defaultHeapSize);
        childWidget->StartApp(jsBundlePath.toStdString().c_str());
    }
}

static void TerminateInterface(uint32_t token, bool forceStop)
{
    (void)(forceStop);
    TerminateAbility(token);
}

static void RegisterTerminateHandler()
{
    OHOS::ACELite::ProductAdapter::RegTerminatingHandler(TerminateInterface);
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    InitUIkit();
    // do some product adapter mock
    RegisterTerminateHandler();
    OHOS::MainWidget mainWidget;
    mainWidget.setWindowTitle("ACE Simulator");
    mainWidget.setFixedSize(DEFAULT_JSWINDOW_WIDTH, DEFAULT_JSWINDOW_HEIGHT + CHILD_PAGE_HEIGHT);
    InitPage(&mainWidget, DEFAULT_JSWINDOW_HEIGHT, DEFAULT_JSWINDOW_WIDTH, CHILD_PAGE_HEIGHT);
    mainWidget.show();
    return app.exec();
}

