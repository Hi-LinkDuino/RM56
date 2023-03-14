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

#ifndef SIMULATOR_CONFIG_H
#define SIMULATOR_CONFIG_H

#include <QDir>
#include <QSettings>
#include <QString>

const QString CONFIG_FILE_PATH = QDir::currentPath() + "/qt.ini";
const QString CONFIG_KEY_JSBUNDLE_PATH = "JSBundlePath";
const QString CONFIG_KEY_JSHEAP_SIZE = "JSHeapSize";
const int16_t DEFAULT_JSHEAP_SIZE = 64; // KB
const int16_t MIN_JSHEAP_SIZE = 48; // KB
const int16_t MAX_JSHEAP_SIZE = 512; // KB
const int16_t DEFAULT_JSWINDOW_WIDTH = 960; // pixel
const int16_t DEFAULT_JSWINDOW_HEIGHT = 480; // pixel
const int16_t CHILD_PAGE_HEIGHT = 100; // pixel

class SimulatorConfig {
public:
    static SimulatorConfig& GetInstance();
    QString GetConfigValue(QString key) const;
    void SetConfigValue(QString key, QString value) const;

private:
    SimulatorConfig();
};

#endif // SIMULATOR_CONFIG_H
