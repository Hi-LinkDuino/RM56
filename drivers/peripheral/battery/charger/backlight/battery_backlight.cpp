/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "battery_backlight.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "errors.h"
#include "battery_log.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
namespace {
constexpr uint32_t BACKLIGHT_ON = 128;
constexpr uint32_t BACKLIGHT_OFF = 0;
constexpr uint32_t MKDIR_WAIT_TIME = 1;
std::vector<std::string> g_backlightNodeNames;
const std::string BACKLIGHT_BASE_PATH = "/sys/class/leds";
std::string g_backlightNode = "backlight";
}


BatteryBacklight::BatteryBacklight()
{
    InitDefaultSysfs();
}

void BatteryBacklight::TraversalBacklightNode()
{
    std::string::size_type idx;

    for (auto iter = g_backlightNodeNames.begin(); iter != g_backlightNodeNames.end(); ++iter) {
        idx = iter->find(g_backlightNode);
        if (idx == std::string::npos) {
            BATTERY_HILOGW(FEATURE_CHARGING, "not found backlight node, use default");
        } else {
            g_backlightNode = *iter;
            BATTERY_HILOGD(FEATURE_CHARGING, "backlight node is %{public}s", iter->c_str());
        }
    }
}

int32_t BatteryBacklight::InitBacklightSysfs()
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start init backlight sysfs");
    DIR* dir = nullptr;
    struct dirent* entry = nullptr;
    int32_t index = 0;
    const int32_t MAX_SIZE = 64;

    dir = opendir(BACKLIGHT_BASE_PATH.c_str());
    if (dir == nullptr) {
        BATTERY_HILOGW(FEATURE_CHARGING, "backlight base path is not exist");
        return ERR_INVALID_VALUE;
    }

    while (true) {
        entry = readdir(dir);
        if (entry == nullptr) {
            break;
        }

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (entry->d_type == DT_DIR || entry->d_type == DT_LNK) {
            BATTERY_HILOGI(FEATURE_CHARGING, "init backlight info of %{public}s", entry->d_name);
            if (index >= MAX_SIZE) {
                BATTERY_HILOGE(FEATURE_CHARGING, "too many backlight types");
                break;
            }
            g_backlightNodeNames.emplace_back(entry->d_name);
            index++;
        }
    }

    TraversalBacklightNode();
    BATTERY_HILOGD(FEATURE_CHARGING, "backlight index is %{public}d", index);
    closedir(dir);
    BATTERY_HILOGD(FEATURE_CHARGING, "finish init backlight sysfs");
    return ERR_OK;
}

void BatteryBacklight::TurnOnScreen()
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start turn on screen");
    HandleBacklight(BACKLIGHT_ON);
    screenOn_ = true;
}

void BatteryBacklight::TurnOffScreen()
{
    BATTERY_HILOGI(FEATURE_CHARGING, "start turn off screen");
    HandleBacklight(BACKLIGHT_OFF);
    screenOn_ = false;
}

bool BatteryBacklight::GetScreenState() const
{
    BATTERY_HILOGI(FEATURE_CHARGING, "screen state: %{public}d", screenOn_);
    return screenOn_;
}

void BatteryBacklight::CreateFile(const std::string& path, const std::string& content)
{
    std::ofstream stream(path.c_str());
    if (!stream.is_open()) {
        BATTERY_HILOGD(FEATURE_CHARGING, "Cannot create file %{public}s", path.c_str());
        return;
    }
    stream << content.c_str() << std::endl;
    stream.close();
}

void BatteryBacklight::InitDefaultSysfs() const
{
    std::string brightnessPath = "/data";
    if (access(brightnessPath.c_str(), 0) == -1) {
        mkdir(brightnessPath.c_str(), S_IRWXU);
        sleep(MKDIR_WAIT_TIME);
    }

    BATTERY_HILOGI(FEATURE_CHARGING, "create default brightness path");
    CreateFile("/data/brightness", "127");
}

void BatteryBacklight::InitDevicePah(std::string& path)
{
    if (access(path.c_str(), F_OK) == 0) {
        BATTERY_HILOGI(FEATURE_CHARGING, "system backlight path exist");
        return;
    } else {
        BATTERY_HILOGI(FEATURE_CHARGING, "create mock backlight path");
        path = "/data/brightness";
        return;
    }

    BATTERY_HILOGI(FEATURE_CHARGING, "exit");
}

int32_t BatteryBacklight::HandleBacklight(uint32_t backlight)
{
    FILE* fp = nullptr;
    int32_t ret = -1;
    std::string devicePath = BACKLIGHT_BASE_PATH + "/" + g_backlightNode + "/" + "brightness";
    BATTERY_HILOGD(FEATURE_CHARGING, "backlight devicePath is %{public}s", devicePath.c_str());
    InitDevicePah(devicePath);

    BATTERY_HILOGD(FEATURE_CHARGING, "backlight value is %{public}d", backlight);
    fp = fopen(devicePath.c_str(), "w");
    if (fp != nullptr) {
        ret = fprintf(fp, "%u\n", backlight);
        fclose(fp);
    }
    if (ret <= 0) {
        BATTERY_HILOGW(FEATURE_CHARGING, "failed to set backlight path=%{public}s.", devicePath.c_str());
    }
    return ret;
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
