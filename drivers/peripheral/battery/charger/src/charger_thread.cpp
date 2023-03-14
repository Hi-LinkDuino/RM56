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

#include "charger_thread.h"

#include <cinttypes>
#include <securec.h>
#include <parameters.h>
#include <linux/netlink.h>
#include "updater_ui.h"
#include "text_label.h"
#include "view.h"
#include "input_manager.h"
#include "input_type.h"
#include "power_mgr_client.h"
#include "battery_log.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
namespace {
constexpr int32_t SHUTDOWN_TIME_MS = 2000;
constexpr int32_t SEC_TO_MSEC = 1000;
constexpr int32_t NSEC_TO_MSEC = 1000000;
constexpr int32_t REBOOT_TIME = 2000;
constexpr int32_t BACKLIGHT_OFF_TIME_MS = 10000;
constexpr int32_t VIBRATE_TIME_MS = 75;
constexpr int32_t MAX_IMG_COUNT = 62;
constexpr int32_t MAX_IMG_NAME_SIZE = 255;
constexpr int32_t LOOP_TOP_PICTURES = 10;
}

struct KeyState {
    bool up;
    bool down;
    int64_t timestamp;
};

Frame* g_hosFrame = nullptr;
AnimationLabel* g_animationLabel = nullptr;
TextLabel* g_updateInfoLabel = nullptr;
struct KeyState g_keys[KEY_MAX + 1] = {};

static int64_t GetCurrentTime()
{
    timespec tm {};
    clock_gettime(CLOCK_MONOTONIC, &tm);
    return tm.tv_sec * SEC_TO_MSEC + (tm.tv_nsec / NSEC_TO_MSEC);
}

void ChargerThread::HandleStates()
{
    HandleChargingState();
    HandlePowerKeyState();
    HandleScreenState();
}

int32_t ChargerThread::UpdateWaitInterval()
{
    int64_t currentTime = GetCurrentTime();
    int64_t nextWait = INT64_MAX;
    int64_t timeout = INVALID;

    if (pluginWait_ != INVALID) {
        nextWait = pluginWait_ - currentTime;
    }

    if (keyWait_ != INVALID && keyWait_ < nextWait) {
        nextWait = keyWait_;
    }

    if (backlightWait_ != INVALID && backlightWait_ < nextWait) {
        nextWait = backlightWait_;
    }

    if (nextWait != INVALID && nextWait != INT64_MAX) {
        if (nextWait - currentTime > 0) {
            timeout = nextWait - currentTime;
        } else {
            timeout = 0;
        }
    }

    return static_cast<int32_t>(timeout);
}

void ChargerThread::AnimationInit()
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start init animation");
    int32_t screenH = 0;
    int32_t screenW = 0;
    auto* sfDev = new SurfaceDev(SurfaceDev::DevType::DRM_DEVICE);
    sfDev->GetScreenSize(screenW, screenH);
    View::BRGA888Pixel bgColor {0x00, 0x00, 0x00, 0xff};

    g_hosFrame = new Frame(screenW, screenH, View::PixelFormat::BGRA888, sfDev);
    g_hosFrame->SetBackgroundColor(&bgColor);

    g_animationLabel = new AnimationLabel(90, 240, 360, 960 >> 1, g_hosFrame);
    g_animationLabel->SetBackgroundColor(&bgColor);
    LoadImages(g_animationLabel);

    g_updateInfoLabel = new TextLabel(screenW / 3, 340, screenW / 3, 100, g_hosFrame);
    g_updateInfoLabel->SetOutLineBold(false, false);
    g_updateInfoLabel->SetBackgroundColor(&bgColor);

    BATTERY_HILOGD(FEATURE_CHARGING, "finish init animation");
}

void ChargerThread::LoadImages(AnimationLabel* animationLabel)
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start load images");
    char nameBuf[MAX_IMG_NAME_SIZE];
    for (int32_t i = 0; i < MAX_IMG_COUNT; i++) {
        if (memset_s(nameBuf, MAX_IMG_NAME_SIZE, 0, MAX_IMG_NAME_SIZE) != EOK) {
            BATTERY_HILOGW(FEATURE_CHARGING, "memset_s failed");
            return;
        }

        if (i < LOOP_TOP_PICTURES) {
            if (snprintf_s(nameBuf, MAX_IMG_NAME_SIZE, MAX_IMG_NAME_SIZE - 1,
                           "/system/etc/resources/loop0000%d.png", i) == -1) {
                BATTERY_HILOGW(FEATURE_CHARGING, "snprintf_s failed, index=%{public}d", i);
                return;
            }
        } else {
            if (snprintf_s(nameBuf, MAX_IMG_NAME_SIZE, MAX_IMG_NAME_SIZE - 1,
                           "/system/etc/resources/loop000%d.png", i) == -1) {
                BATTERY_HILOGW(FEATURE_CHARGING, "snprintf_s failed, index=%{public}d", i);
                return;
            }
        }

        animationLabel->AddImg(nameBuf);
    }
    animationLabel->AddStaticImg(nameBuf);
}

void ChargerThread::UpdateAnimation(const int32_t& capacity)
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start update animation, capacity=%{public}d", capacity);
    AnimationLabel::needStop_ = false;

    struct FocusInfo info {false, false};
    struct Bold bold {false, false};
    View::BRGA888Pixel bgColor {0x00, 0x00, 0x00, 0xff};
    std::string displaySoc = "  " + std::to_string(capacity) + "%";
    TextLabelInit(g_updateInfoLabel, displaySoc, bold, info, bgColor);
    g_animationLabel->UpdateLoop();
}

void ChargerThread::CycleMatters()
{
    if (!started_) {
        started_ = true;
        backlightWait_ = GetCurrentTime() - 1;
    }

    provider_->ParseCapacity(&capacity_);
    provider_->ParseChargeState(&chargeState_);
    BATTERY_HILOGI(FEATURE_CHARGING, "chargeState_=%{public}d, capacity_=%{public}d", chargeState_, capacity_);

    UpdateEpollInterval(chargeState_);
}

void ChargerThread::UpdateBatteryInfo(void* arg, char* msg)
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start update battery info by uevent msg");
    std::unique_ptr<BatterydInfo> batteryInfo = std::make_unique<BatterydInfo>();
    if (batteryInfo == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "make_unique BatterydInfo return nullptr");
        return;
    }

    provider_->ParseUeventToBatterydInfo(msg, batteryInfo.get());

    capacity_ = batteryInfo->capacity_;
    chargeState_ = batteryInfo->chargeState_;

    HandleCapacity(capacity_);
    HandleTemperature(batteryInfo->temperature_);

    led_->UpdateLedColor(chargeState_, capacity_);
    if (backlight_->GetScreenState()) {
        UpdateAnimation(capacity_);
    }

    BATTERY_HILOGD(FEATURE_CHARGING, "finish update battery info");
}

void ChargerThread::UpdateBatteryInfo(void* arg)
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start update battery info by provider");
    int32_t temperature = 0;
    provider_->ParseTemperature(&temperature);
    provider_->ParseCapacity(&capacity_);
    provider_->ParseChargeState(&chargeState_);
    BATTERY_HILOGD(FEATURE_CHARGING, "temperature=%{public}d, capacity_=%{public}d, chargeState_=%{public}d",
        temperature, capacity_, chargeState_);

    HandleTemperature(temperature);
    HandleCapacity(capacity_);

    led_->UpdateLedColor(chargeState_, capacity_);
    if (backlight_->GetScreenState()) {
        UpdateAnimation(capacity_);
    }
}

void ChargerThread::HandleCapacity(const int32_t& capacity)
{
    auto lowCapacity = batteryConfig_->GetCapacityConf();
    BATTERY_HILOGD(FEATURE_CHARGING, "capacity=%{public}d, lowCapacity=%{public}d", capacity, lowCapacity);
    auto& powerMgrClient = OHOS::PowerMgr::PowerMgrClient::GetInstance();
    if ((capacity <= lowCapacity) &&
        ((chargeState_ == PowerSupplyProvider::CHARGE_STATE_NONE) ||
        (chargeState_ == PowerSupplyProvider::CHARGE_STATE_RESERVED))) {
        BATTERY_HILOGW(FEATURE_CHARGING, "low capacity, shutdown device");
        std::string reason = "LowCapacity";
        powerMgrClient.ShutDownDevice(reason);
    }
}

void ChargerThread::HandleTemperature(const int32_t& temperature)
{
    auto tempConf = batteryConfig_->GetTempConf();
    BATTERY_HILOGD(FEATURE_CHARGING, "temperature=%{public}d, tempConf.lower=%{public}d, tempConf.upper=%{public}d",
        temperature, tempConf.lower, tempConf.upper);

    auto& powerMgrClient = OHOS::PowerMgr::PowerMgrClient::GetInstance();
    if (((temperature <= tempConf.lower) || (temperature >= tempConf.upper)) &&
        (tempConf.lower != tempConf.upper)) {
        BATTERY_HILOGW(FEATURE_CHARGING, "temperature out of range, shutdown device");
        std::string reason = "TemperatureOutOfRange";
        powerMgrClient.ShutDownDevice(reason);
    }
}

void ChargerThread::SetKeyWait(struct KeyState& key, int64_t timeout)
{
    int64_t nextMoment = key.timestamp + timeout;
    if (keyWait_ == INVALID || nextMoment < keyWait_) {
        keyWait_ = nextMoment;
    }
}

void ChargerThread::HandleChargingState()
{
    int64_t now = GetCurrentTime();
    BATTERY_HILOGD(FEATURE_CHARGING, "chargeState_=%{public}d, now=%{public}" PRId64 "", chargeState_, now);
    auto& powerMgrClient = OHOS::PowerMgr::PowerMgrClient::GetInstance();

    if ((chargeState_ == PowerSupplyProvider::CHARGE_STATE_NONE) ||
        (chargeState_ == PowerSupplyProvider::CHARGE_STATE_RESERVED)) {
        if (pluginWait_ == INVALID) {
            BATTERY_HILOGD(FEATURE_CHARGING, "wait plugin");
            backlightWait_ = now - 1;
            backlight_->TurnOnScreen();
            led_->TurnOffLed();
            AnimationLabel::needStop_ = true;
            pluginWait_ = now + SHUTDOWN_TIME_MS;
        } else if (now >= pluginWait_) {
            BATTERY_HILOGI(FEATURE_CHARGING, "shutdown device, pluginWait_=%{public}" PRId64 "", pluginWait_);
            std::string reason = "charger unplugged";
            powerMgrClient.ShutDownDevice(reason);
        } else {
            BATTERY_HILOGD(FEATURE_CHARGING, "ShutDownDevice timer already in scheduled.");
        }
    } else {
        if (pluginWait_ != INVALID) {
            BATTERY_HILOGI(FEATURE_CHARGING, "update capacity_=%{public}d", capacity_);
            backlightWait_ = now - 1;
            backlight_->TurnOnScreen();
            led_->UpdateLedColor(chargeState_, capacity_);
            AnimationLabel::needStop_ = true;
            UpdateAnimation(capacity_);
        }
        pluginWait_ = INVALID;
    }
}

void ChargerThread::HandleScreenState()
{
    if (backlightWait_ != INVALID && GetCurrentTime() > backlightWait_ + BACKLIGHT_OFF_TIME_MS) {
        BATTERY_HILOGI(FEATURE_CHARGING, "turn off screen");
        backlight_->TurnOffScreen();
        AnimationLabel::needStop_ = true;
        backlightWait_ = INVALID;
    }
}

void ChargerThread::SetKeyState(int32_t code, int32_t value, int64_t now)
{
    BATTERY_HILOGD(FEATURE_CHARGING, "now=%{public}" PRId64 "", now);
    bool down;
    if (!!value) {
        down = true;
    } else {
        down = false;
    }

    if (code > KEY_MAX) {
        return;
    }

    if (g_keys[code].down == down) {
        return;
    }

    if (down) {
        g_keys[code].timestamp = now;
    }

    g_keys[code].down = down;
    g_keys[code].up = true;
}

void ChargerThread::HandlePowerKeyState()
{
    auto now = GetCurrentTime();
    BATTERY_HILOGD(FEATURE_CHARGING, "now=%{public}" PRId64 "", now);
    HandlePowerKey(KEY_POWER, now);

    BATTERY_HILOGD(FEATURE_CHARGING, "keyWait_=%{public}" PRId64 "", keyWait_);
    if (keyWait_ != INVALID && now > keyWait_) {
        keyWait_ = INVALID;
    }
}

void ChargerThread::HandlePowerKey(int32_t keycode, int64_t now)
{
    auto& powerMgrClient = OHOS::PowerMgr::PowerMgrClient::GetInstance();
    KeyState key = g_keys[keycode];
    if (keycode == KEY_POWER) {
        if (key.down) {
            BATTERY_HILOGD(FEATURE_CHARGING, "power key down");
            int64_t rebootTime = key.timestamp + REBOOT_TIME;
            if (now >= rebootTime) {
                BATTERY_HILOGD(FEATURE_CHARGING, "reboot machine");
                backlight_->TurnOffScreen();
                AnimationLabel::needStop_ = true;
                vibrate_->HandleVibration(VIBRATE_TIME_MS);
                std::string reason = "Reboot";
                powerMgrClient.RebootDevice(reason);
            } else {
                SetKeyWait(key, REBOOT_TIME);
                backlight_->TurnOnScreen();
                AnimationLabel::needStop_ = true;
                UpdateAnimation(capacity_);
                backlightWait_ = now - 1;
                BATTERY_HILOGD(FEATURE_CHARGING, "turn on the screen");
            }
        } else {
            if (key.up) {
                BATTERY_HILOGD(FEATURE_CHARGING, "power key up");
                backlight_->TurnOnScreen();
                AnimationLabel::needStop_ = true;
                UpdateAnimation(capacity_);
                backlightWait_ = now - 1;
            }
        }
    }
    key.up = false;
}

void ChargerThread::HandleInputEvent(const struct input_event* iev)
{
    input_event ev {};
    ev.type = iev->type;
    ev.code = iev->code;
    ev.value = iev->value;
    BATTERY_HILOGD(FEATURE_CHARGING, "ev.type=%{public}d, ev.code=%{public}d, ev.value=%{public}d",
        ev.type, ev.code, ev.value);

    if (ev.type != EV_KEY) {
        return;
    }
    SetKeyState(ev.code, ev.value, GetCurrentTime());
}

void ChargerThread::EventPkgCallback(const EventPackage** pkgs, uint32_t count, uint32_t devIndex)
{
    (void)devIndex;
    BATTERY_HILOGD(FEATURE_CHARGING, "start key event callback");
    if (pkgs == nullptr || *pkgs == nullptr) {
        BATTERY_HILOGW(FEATURE_CHARGING, "pkgs or *pkgs is nullptr");
        return;
    }
    for (uint32_t i = 0; i < count; i++) {
        struct input_event ev = {
            .type = static_cast<__u16>(pkgs[i]->type),
            .code = static_cast<__u16>(pkgs[i]->code),
            .value = pkgs[i]->value,
        };
        HandleInputEvent(&ev);
    }
}

void ChargerThread::InitInput()
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start init input");
    IInputInterface* inputInterface = nullptr;
    int32_t ret = GetInputInterface(&inputInterface);
    if (ret != INPUT_SUCCESS) {
        BATTERY_HILOGW(FEATURE_CHARGING, "get input driver interface failed, ret=%{public}d", ret);
        return;
    }

    const uint32_t DEVICE_INDEX = 1;
    ret = inputInterface->iInputManager->OpenInputDevice(DEVICE_INDEX);
    if (ret != INPUT_SUCCESS) {
        BATTERY_HILOGD(FEATURE_CHARGING, "open device failed, index=%{public}u, ret=%{public}d", DEVICE_INDEX, ret);
        return;
    }

    uint32_t devType = InputDevType::INDEV_TYPE_UNKNOWN;
    ret = inputInterface->iInputController->GetDeviceType(DEVICE_INDEX, &devType);
    if (ret != INPUT_SUCCESS) {
        BATTERY_HILOGW(FEATURE_CHARGING, "get device type failed, index=%{public}u, ret=%{public}d", DEVICE_INDEX, ret);
        return;
    }

    InputEventCb callback = {
        .EventPkgCallback = EventPkgCallback
    };
    ret = inputInterface->iInputReporter->RegisterReportCallback(DEVICE_INDEX, &callback);
    if (ret != INPUT_SUCCESS) {
        BATTERY_HILOGW(FEATURE_CHARGING, "register callback failed, index=%{public}u, ret=%{public}d", DEVICE_INDEX,
                       ret);
        return;
    }

    BATTERY_HILOGD(FEATURE_CHARGING, "finish init input, index=%{public}u, type=%{public}u", DEVICE_INDEX, devType);
}

void ChargerThread::Init()
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start init charger thread");
    batteryConfig_ = std::make_unique<BatteryConfig>();
    if (batteryConfig_ == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "make_unique BatteryConfig return nullptr");
        return;
    }
    batteryConfig_->Init();

    provider_ = std::make_unique<PowerSupplyProvider>();
    if (provider_ == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "make_unique PowerSupplyProvider return nullptr");
        return;
    }
    provider_->InitBatteryPath();
    provider_->InitPowerSupplySysfs();

    vibrate_ = std::make_unique<BatteryVibrate>();
    if (vibrate_ == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "make_unique BatteryVibrate return nullptr");
        return;
    }

    if (!vibrate_->InitVibration()) {
        BATTERY_HILOGW(FEATURE_CHARGING, "InitVibration failed, vibration does not work");
    }

    backlight_ = std::make_unique<BatteryBacklight>();
    if (backlight_ == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "make_unique BatteryBacklight return nullptr");
        return;
    }
    backlight_->InitBacklightSysfs();
    backlight_->TurnOnScreen();

    led_ = std::make_unique<BatteryLed>();
    if (led_ == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "make_unique BatteryLed return nullptr");
        return;
    }
    led_->InitLedsSysfs();
    led_->TurnOffLed();

    AnimationInit();
    InitInput();
}

void ChargerThread::Run(void* service)
{
    BATTERY_HILOGI(FEATURE_CHARGING, "start run charger thread");
    Init();

    std::make_unique<std::thread>(&ChargerThread::LoopingThreadEntry, this, service)->join();
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
