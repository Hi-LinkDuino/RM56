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

#ifndef POWERMGR_POWER_MANAGE_H
#define POWERMGR_POWER_MANAGE_H

#include <ohos_types.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum {
    SUSPEND_DEVICE_REASON_MIN = 0,
    SUSPEND_DEVICE_REASON_APPLICATION = SUSPEND_DEVICE_REASON_MIN,
    SUSPEND_DEVICE_REASON_DEVICE_ADMIN = 1,
    SUSPEND_DEVICE_REASON_TIMEOUT = 2,
    SUSPEND_DEVICE_REASON_LID_SWITCH = 3,
    SUSPEND_DEVICE_REASON_POWER_BUTTON = 4,
    SUSPEND_DEVICE_REASON_HDMI = 5,
    SUSPEND_DEVICE_REASON_SLEEP_BUTTON = 6,
    SUSPEND_DEVICE_REASON_ACCESSIBILITY = 7,
    SUSPEND_DEVICE_REASON_FORCE_SUSPEND = 8,
    SUSPEND_DEVICE_REASON_MAX = SUSPEND_DEVICE_REASON_FORCE_SUSPEND,
} SuspendDeviceType;

typedef enum {
    WAKEUP_DEVICE_UNKNOWN = 0,
    WAKEUP_DEVICE_POWER_BUTTON = 1,
    WAKEUP_DEVICE_APPLICATION = 2,
    WAKEUP_DEVICE_PLUGGED_IN = 3,
    WAKEUP_DEVICE_GESTURE = 4,
    WAKEUP_DEVICE_CAMERA_LAUNCH = 5,
    WAKEUP_DEVICE_WAKE_KEY = 6,
    WAKEUP_DEVICE_WAKE_MOTION = 7,
    WAKEUP_DEVICE_HDMI = 8,
    WAKEUP_DEVICE_LID = 9,
} WakeupDeviceType;

/**
 * Suspend device.
 *
 * @param reason The reason why will you suspend the device.
 * @param suspendImmed The flag indicating whether the system will suspend immediately.
 */
void SuspendDevice(SuspendDeviceType reason, BOOL suspendImmed);

/**
 * Wake up the device.
 *
 * @param reason The reason for waking up the device.
 * @param details Details of the wakeup reason.
 */
void WakeupDevice(WakeupDeviceType reason, const char* details);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // POWERMGR_POWER_MANAGE_H
