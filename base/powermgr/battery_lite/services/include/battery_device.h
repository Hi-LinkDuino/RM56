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

#ifndef BATTERY_DEVICE_H
#define BATTERY_DEVICE_H

#include "ohos_init.h"
#include "samgr_lite.h"
#include "service.h"
#include "feature.h"
#include <iproxy_server.h>
#include <iunknown.h>
#include <pthread.h>
#include <log.h>
#include "ibattery.h"


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define BATTERY_DEVICE "battery_device"
#define BATTERY_FEATURE "battery_feature"

#define BATTERY_ERROR_UNKNOWN (-1)
#define BATTERY_ERROR_INVALID_ID (-2)
#define BATTERY_ERROR_INVALID_PARAM (-3)
#define BATTERY_OK 0

#define TASK_CONFIG_STACK_SIZE 0x800
#define TASK_CONFIG_QUEUE_SIZE 20


typedef struct BatteeryDeviceFeatureApi {
    INHERIT_SERVER_IPROXY;
    int32_t (*GetSoc)(void);
    BatteryChargeState (*GetChargingStatus)(void);
    BatteryHealthState (*GetHealthStatus)(void);
    BatteryPluggedType (*GetPluggedType)(void);
    int32_t (*GetVoltage)(void);
    char* (*GetTechnology)(void);
    int32_t (*GetTemperature)(void);
    int (*TurnOnLed)(int red, int green, int blue);
    int (*TurnOffLed)(void);
    int (*SetLedColor)(int red, int green, int blue);
    int (*GetLedColor)(int *red, int *green, int *blue);
    void (*ShutDown)(void);
    void (*UpdateBatInfo)(BatInfo*);
} BatteeryDeviceFeatureApi;

typedef struct BatteryDevice {
    INHERIT_SERVICE;
    INHERIT_IUNKNOWNENTRY(BatteeryDeviceFeatureApi);
    Identity identity;
} BatteryDevice;

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // BATTERY_SRV_CLIENT_H