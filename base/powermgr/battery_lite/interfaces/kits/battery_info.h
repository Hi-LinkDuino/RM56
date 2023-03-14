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

#ifndef BATERY_INFO_H
#define BATERY_INFO_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum {
    /**
     * Battery is discharge.
     */
    CHARGE_STATE_NONE,
    /**
     * Battery is charging.
     */
    CHARGE_STATE_ENABLE,
    /**
     * Battery is not charging.
     */
    CHARGE_STATE_DISABLE,
    /**
     * Battery charge full.
     */
    CHARGE_STATE_FULL,
    /**
    * The bottom of the enum.
    */
    CHARGE_STATE_BUTT
} BatteryChargeState;

typedef enum {
    /**
     * Health Status: unknown.
     */
    HEALTH_STATE_UNKNOWN,
    /**
     * Health Status: good.
     */
    HEALTH_STATE_GOOD,
    /**
     * Health Status: over heat.
     */
    HEALTH_STATE_OVERHEAT,
    /**
     * Health Status: over voltage.
     */
    HEALTH_STATE_OVERVOLTAGE,
    /**
     * Health Status: COLD.
     */
    HEALTH_STATE_COLD,
    /**
     * Health Status: Dead.
     */
    HEALTH_STATE_DEAD,
    /**
    * The bottom of the enum.
    */
    HEALTH_STATE_BUTT
} BatteryHealthState;

typedef enum {
    /**
     * Power source is unplugged.
     */
    PLUGGED_TYPE_NONE,
    /**
     * Power source is an AC charger.
     */
    PLUGGED_TYPE_AC,
    /**
     * Power source is a USB DC charger.
     */
    PLUGGED_TYPE_USB,
    /**
     * Power source is wireless charger.
     */
    PLUGGED_TYPE_WIRELESS,
    /**
    * The bottom of the enum.
    */
    PLUGGED_TYPE_BUTT
} BatteryPluggedType;

int32_t GetBatSoc(void);
BatteryChargeState GetChargingStatus(void);
BatteryHealthState GetHealthStatus(void);
BatteryPluggedType GetPluggedType(void);
int32_t GetBatVoltage(void);
char* GetBatTechnology(void);
int32_t GetBatTemperature(void);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // BATERY_INFO_H