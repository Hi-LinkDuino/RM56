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

#include "battery_led.h"

#include <fstream>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "file_ex.h"
#include "errors.h"
#include "power_supply_provider.h"
#include "battery_log.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
namespace {
constexpr int32_t CAPACITY_FULL = 100;
constexpr int32_t MKDIR_WAIT_TIME = 1;
constexpr int32_t LED_COLOR_GREEN = 2;
constexpr int32_t LED_COLOR_RED = 4;
constexpr int32_t LED_COLOR_YELLOW = 6;
std::vector<std::string> g_ledsNodeNames;
const std::string LEDS_BASE_PATH = "/sys/class/leds";
std::string g_redLedsNode = "red";
std::string g_greenLedsNode = "green";
std::string g_blueLedsNode = "blue";
}

void BatteryLed::TraversalNode()
{
    std::string::size_type idx;

    for (auto iter = g_ledsNodeNames.begin(); iter != g_ledsNodeNames.end(); ++iter) {
        idx = iter->find(g_redLedsNode);
        if (idx == std::string::npos) {
            BATTERY_HILOGD(FEATURE_CHARGING, "not found red leds node");
        } else {
            g_redLedsNode = *iter;
            BATTERY_HILOGD(FEATURE_CHARGING, "red leds node is %{public}s", iter->c_str());
        }

        idx = iter->find(g_greenLedsNode);
        if (idx == std::string::npos) {
            BATTERY_HILOGD(FEATURE_CHARGING, "not found green leds node");
        } else {
            g_greenLedsNode = *iter;
            BATTERY_HILOGD(FEATURE_CHARGING, "green leds node is %{public}s", iter->c_str());
        }

        idx = iter->find(g_blueLedsNode);
        if (idx == std::string::npos) {
            BATTERY_HILOGD(FEATURE_CHARGING, "not found blue leds node");
        } else {
            g_blueLedsNode = *iter;
            BATTERY_HILOGD(FEATURE_CHARGING, "blue leds node is %{public}s", iter->c_str());
        }
    }
}

int32_t BatteryLed::InitLedsSysfs()
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start init leds sysfs");
    DIR* dir = nullptr;
    struct dirent* entry = nullptr;
    int32_t index = 0;
    const int32_t MAX_SIZE = 64;

    dir = opendir(LEDS_BASE_PATH.c_str());
    if (dir == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "leds base path is not exist");
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
            BATTERY_HILOGI(FEATURE_CHARGING, "init leds info of %{public}s", entry->d_name);
            if (index >= MAX_SIZE) {
                BATTERY_HILOGE(FEATURE_CHARGING, "too many leds types");
                break;
            }
            g_ledsNodeNames.emplace_back(entry->d_name);
            index++;
        }
    }

    TraversalNode();
    BATTERY_HILOGD(FEATURE_CHARGING, "leds index is %{public}d", index);
    closedir(dir);

    BATTERY_HILOGD(FEATURE_CHARGING, "finish init leds sysfs");
    return ERR_OK;
}

void BatteryLed::TurnOffLed()
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start turn off led");
    WriteLedInfoToSys(0, 0, 0);
}

void BatteryLed::UpdateLedColor(int32_t chargeState, int32_t capacity)
{
    if ((chargeState == PowerSupplyProvider::CHARGE_STATE_NONE) ||
        (chargeState == PowerSupplyProvider::CHARGE_STATE_RESERVED)) {
        BATTERY_HILOGD(FEATURE_CHARGING, "not in charging state, turn off led");
        TurnOffLed();
        return;
    }

    std::unique_ptr<BatteryConfig> batteryConfig = std::make_unique<BatteryConfig>();
    if (batteryConfig == nullptr) {
        BATTERY_HILOGW(FEATURE_CHARGING, "make_unique BatteryConfig return nullptr");
        return;
    }
    batteryConfig->Init();

    auto ledConf = batteryConfig->GetLedConf();
    for (auto it = ledConf.begin(); it != ledConf.end(); ++it) {
        BATTERY_HILOGD(FEATURE_CHARGING, "capacity=%{public}d, ledConf.begin()=%{public}d, ledConf.end()=%{public}d",
            capacity, it->capacityBegin, it->capacityEnd);
        if ((capacity >= it->capacityBegin) && (capacity < it->capacityEnd)) {
            switch (it->color) {
                case (LED_COLOR_GREEN): {
                    BATTERY_HILOGD(FEATURE_CHARGING, "led color display green");
                    WriteLedInfoToSys(0, it->brightness, 0);
                    break;
                }
                case (LED_COLOR_RED): {
                    BATTERY_HILOGD(FEATURE_CHARGING, "led color display red");
                    WriteLedInfoToSys(it->brightness, 0, 0);
                    break;
                }
                case (LED_COLOR_YELLOW): {
                    BATTERY_HILOGD(FEATURE_CHARGING, "led color display yellow");
                    WriteLedInfoToSys(it->brightness, it->brightness, 0);
                    break;
                }
                default: {
                    BATTERY_HILOGD(FEATURE_CHARGING, "led color display error.");
                    break;
                }
            }
            break;
        }

        if (capacity == CAPACITY_FULL) {
            BATTERY_HILOGD(FEATURE_CHARGING, "led color display green");
            WriteLedInfoToSys(0, it->brightness, 0);
            break;
        }
    }
}

void BatteryLed::WriteLedInfoToSys(int32_t redBrightness, int32_t greenBrightness, int32_t blueBrightness)
{
    std::string redLedPath = LEDS_BASE_PATH + "/" + g_redLedsNode + "/" + "brightness";
    std::string greenLedPath = LEDS_BASE_PATH + "/" + g_greenLedsNode + "/" + "brightness";
    std::string blueLedPath = LEDS_BASE_PATH + "/" + g_blueLedsNode + "/" + "brightness";
    if (!FileExists(LEDS_BASE_PATH)) {
        InitMockLedFile(redLedPath, greenLedPath, blueLedPath);
    }
    BATTERY_HILOGD(FEATURE_CHARGING, "redLedPath is %{public}s, greenLedPath is %{public}s, blueLedPath is %{public}s",
        redLedPath.c_str(), greenLedPath.c_str(), blueLedPath.c_str());

    WriteLedNode(redLedPath, redBrightness);
    WriteLedNode(greenLedPath, greenBrightness);
    WriteLedNode(blueLedPath, blueBrightness);
}

void BatteryLed::CreateMockLedFile(const std::string &path, const std::string& content)
{
    std::ofstream stream(path.c_str());
    if (!stream.is_open()) {
        BATTERY_HILOGD(FEATURE_CHARGING, "cannot create file %{public}s", path.c_str());
        return;
    }
    stream << content.c_str() << std::endl;
    stream.close();
}

void BatteryLed::InitMockLedFile(std::string& redPath, std::string& greenPath, std::string& bluePath)
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start init mock led files");
    const std::string MOCK_LEDS_PATH = "/data/local/tmp/leds";
    const std::string RED_LED_PATH = "/data/local/tmp/leds/mock_led:red";
    const std::string RED_LED_BRIGHTNESS_PATH = "/data/local/tmp/leds/mock_led:red/brightness";
    const std::string GREEN_LED_PATH = "/data/local/tmp/leds/mock_led:green";
    const std::string GREEN_LED_BRIGHTNESS_PATH = "/data/local/tmp/leds/mock_led:green/brightness";
    const std::string BLUE_LED_PATH = "/data/local/tmp/leds/mock_led:blue";
    const std::string BLUE_LED_BRIGHTNESS_PATH = "/data/local/tmp/leds/mock_led:blue/brightness";

    if (!FileExists(MOCK_LEDS_PATH)) {
        int32_t ret = mkdir(MOCK_LEDS_PATH.c_str(), S_IRWXU);
        if (ret < 0) {
            BATTERY_HILOGD(FEATURE_CHARGING, "create leds path fail.");
            return;
        }
        sleep(MKDIR_WAIT_TIME);
    }

    BATTERY_HILOGD(FEATURE_CHARGING, "create mock led path and file");
    CreateMockLedPath(RED_LED_PATH);
    CreateMockLedPath(GREEN_LED_PATH);
    CreateMockLedPath(BLUE_LED_PATH);

    CreateMockLedFile(RED_LED_BRIGHTNESS_PATH, "0");
    CreateMockLedFile(GREEN_LED_BRIGHTNESS_PATH, "0");
    CreateMockLedFile(BLUE_LED_BRIGHTNESS_PATH, "0");
    redPath = RED_LED_BRIGHTNESS_PATH;
    greenPath = GREEN_LED_BRIGHTNESS_PATH;
    bluePath = BLUE_LED_BRIGHTNESS_PATH;
}

void BatteryLed::CreateMockLedPath(const std::string& mockPath)
{
    if (FileExists(mockPath)) {
        BATTERY_HILOGD(FEATURE_CHARGING, "mock led path exists, mockPath=%{public}s", mockPath.c_str());
        return;
    }
    int32_t ret = mkdir(mockPath.c_str(), S_IRWXU);
    if (ret < 0) {
        BATTERY_HILOGW(FEATURE_CHARGING, "create led mock path fail");
        return;
    }
    sleep(MKDIR_WAIT_TIME);
}

void BatteryLed::WriteLedNode(std::string& ledPath, int32_t brightness)
{
    FILE* file = fopen(ledPath.c_str(), "w");
    if (file == nullptr) {
        BATTERY_HILOGW(FEATURE_CHARGING, "led file open failed, path=%{public}s", ledPath.c_str());
        return;
    }
    int32_t ret = fprintf(file, "%d\n", brightness);
    if (ret < 0) {
        BATTERY_HILOGW(FEATURE_CHARGING, "led file fprintf failed, path=%{public}s", ledPath.c_str());
    }
    ret = fclose(file);
    if (ret < 0) {
        BATTERY_HILOGW(FEATURE_CHARGING, "led file close failed, path=%{public}s", ledPath.c_str());
    }
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
