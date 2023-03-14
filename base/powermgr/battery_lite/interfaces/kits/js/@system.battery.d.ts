/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

/**
 * @sysCap SystemCapability.PowerManager.BatteryManager.Lite
 */

export interface BatterySocResponse {

    batterySoc: number;
}

/**
 *  @sysCap SystemCapability.PowerManager.BatteryManager.Lite
 */

export interface GetBatterySOC {

    success?: (data: BatterySocResponse) => void;

    fail?: (data: string, code: number) => void;

    complete?: () => void;
}

export interface BatteryChargingStatusResponse {

  chargingStatus: number;
}

/**
 *  @sysCap SystemCapability.PowerManager.BatteryManager.Lite
 */

export interface GetChargingStatus {
   success?: (data: BatteryChargingStatusResponse) => void;

   fail?: (data: string, code: number) => void;

   complete?: () => void;
}

/**
 *  @sysCap SystemCapability.PowerManager.BatteryManager.Lite
 */

export interface BatteryHealthStatusResponse {

  healthStatus: number;
}

/**
 *  @sysCap SystemCapability.PowerManager.BatteryManager.Lite
 */

export interface GetHealthStatus {

   success?: (data: BatteryHealthStatusResponse) => void;

   fail?: (data: string, code: number) => void;

   complete?: () => void;
}

/**
 *  @sysCap SystemCapability.PowerManager.BatteryManager.Lite
 */

export interface BatteryGetPluggedTypeResponse {

  pluggedType: number;
}

/**
 *  @sysCap SystemCapability.PowerManager.BatteryManager.Lite
 */

export interface GetPluggedType {

  success?: (data: BatteryGetPluggedTypeResponse) => void;

  fail?: (data: string, code: number) => void;

  complete?: () => void;
}

/**
 *  @sysCap SystemCapability.PowerManager.BatteryManager.Lite
 */

export interface BatteryGetVoltageResponse {

  voltage: number;
}

/**
 *  @sysCap SystemCapability.PowerManager.BatteryManager.Lite
 */

export interface GetVoltage {

  success?: (data: BatteryGetVoltageResponse) => void;

  fail?: (data: string, code: number) => void;

  complete?: () => void;
}

/**
 *  @sysCap SystemCapability.PowerManager.BatteryManager.Lite
 */

export interface BatteryTechnologyResponse {

  technology: string;
}

/**
 *  @sysCap SystemCapability.PowerManager.BatteryManager.Lite
 */

export interface GetTechnology {

  success?: (data: BatteryTechnologyResponse) => void;

  fail?: (data: string, code: number) => void;

  complete?: () => void;
}

/**
 *  @sysCap SystemCapability.PowerManager.BatteryManager.Lite
 */

export interface BatteryTemperatureResponse {

  temperature: number;
}

/**
 *  @sysCap SystemCapability.PowerManager.BatteryManager.Lite
 */

export interface GetTemperature {

  success?: (data: BatteryTemperatureResponse) => void;

  fail?: (data: string, code: number) => void;

  complete?: () => void;
}

/**
 *  @sysCap SystemCapability.PowerManager.BatteryManager.Lite
 */

export default class battery {

/**
 * Battery state of charge (SoC) of the current device, in percent.
 */

  static BatterySOC(options?: GetBatterySOC): void;
  
/**
  * Battery charging status of the current device.
  */

  static ChargingStatus(options?: GetChargingStatus): void;

/**
  * Battery health state of the current device.
  */
  
  static HealthStatus(options?: GetHealthStatus): void;

/**
  * Charger type of the current device.
  */
  
  static PluggedType(options?: GetPluggedType): void;

/**
  * Battery voltage of the current device.
  */
  
  static Voltage(options?: GetVoltage): void;

/**
  * Battery technology of the current device.
  */
  
  static Technology(options?: GetTechnology): void;
  
/**
  * Battery temperature of the current device.
  */

  static Temperature(options?: GetTemperature): void;
}
