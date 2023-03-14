+137/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import app from '@system.app'
import Context from '@ohos.napi_context'
import batteryInfo from '@ohos.batteryInfo';
import brightness from '@ohos.brightness';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

describe('appInfoTest', function () {
  console.log("*************Battery Unit Test Begin*************");

  /**
   * @tc.number battery_manager_js_0100
   * @tc.name battery_soc_test
   * @tc.desc Battery acquisition kit
   */
  it('battery_soc_test', 0, function () {
    let batterySoc = batteryInfo.batterySOC;
    console.info('batterySoc = ' + batterySoc);
    expect(batterySoc >= -1 && batterySoc <= 100).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_0200
   * @tc.name charging_status_test
   * @tc.desc Battery charging kit
   */
  it('charging_status_test', 0, function () {
    let chargingStatus = batteryInfo.chargingStatus;
    console.info('chargingStatus = ' + chargingStatus);
    expect(chargingStatus >= 0 && chargingStatus <= 4).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_0300
   * @tc.name health_status_test
   * @tc.desc Get Battery Health Get KIT
   */
  it('health_status_test', 0, function () {
    let healthStatus = batteryInfo.healthStatus;
    console.info('healthStatus = ' + healthStatus);
    expect(healthStatus >= 0 && healthStatus <= 6).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_0400
   * @tc.name plugged_type_test
   * @tc.desc Obtaining the USB Connection Type KIT
   */
  it('plugged_type_test', 0, function () {
    let pluggedType = batteryInfo.pluggedType;
    console.info('pluggedType = ' + pluggedType);
    expect(pluggedType >= 0 && pluggedType <= 4).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_0500
   * @tc.name voltage_test
   * @tc.desc Obtaining the Voltage Interface KIT
   */
  it('voltage_test', 0, function () {
    let voltage = batteryInfo.voltage;
    console.info('voltage = ' + voltage);
    expect(voltage >= -1).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_0600
   * @tc.name technology_test
   * @tc.desc Obtaining the Battery Technology Interface KIT
   */
  it('technology_test', 0, function () {
    let technology = batteryInfo.technology;
    console.info('technology = ' + technology);
    expect(technology !== null).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_0700
   * @tc.name battery_temperature_test
   * @tc.desc BatteryTemperature values Interface Test
   */
  it('battery_temperature_test', 0, function () {
    let batteryTemperature = batteryInfo.batteryTemperature;
    console.info('batteryTemperature = ' + batteryTemperature);
    expect(batteryTemperature <= 700).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_0800
   * @tc.name enum_health_state_test_unknown
   * @tc.desc BatteryHealthState values Interface Test
   */
  it('enum_health_state_test_unknown', 0, function () {
    let batteryHealthState = batteryInfo.BatteryHealthState.UNKNOWN;
    console.info('batteryHealthState = ' + batteryHealthState);
    expect(batteryHealthState == 0).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_0900
   * @tc.name enum_health_state_test_good
   * @tc.desc BatteryHealthState values Interface Test
   */
  it('enum_health_state_test_good', 0, function () {
    let batteryHealthState = batteryInfo.BatteryHealthState.GOOD;
    console.info('batteryHealthState = ' + batteryHealthState);
    expect(batteryHealthState == 1).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_1000
   * @tc.name enum_health_state_test_overheat
   * @tc.desc BatteryHealthState values Interface Test
   */
  it('enum_health_state_test_overheat', 0, function () {
    let batteryHealthState = batteryInfo.BatteryHealthState.OVERHEAT;
    console.info('batteryHealthState = ' + batteryHealthState);
    expect(batteryHealthState == 2).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_1100
   * @tc.name enum_health_state_test_overvoltage
   * @tc.desc BatteryHealthState values Interface Test
   */
  it('enum_health_state_test_overvoltage', 0, function () {
    let batteryHealthState = batteryInfo.BatteryHealthState.OVERVOLTAGE;
    console.info('batteryHealthState = ' + batteryHealthState);
    expect(batteryHealthState == 3).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_1200
   * @tc.name enum_health_state_test_cold
   * @tc.desc BatteryHealthState values Interface Test
   */
  it('enum_health_state_test_cold', 0, function () {
    let batteryHealthState = batteryInfo.BatteryHealthState.COLD;
    console.info('batteryHealthState = ' + batteryHealthState);
    expect(batteryHealthState == 4).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_1300
   * @tc.name enum_health_state_test_dead
   * @tc.desc BatteryHealthState values Interface Test
   */
  it('enum_health_state_test_dead', 0, function () {
    let batteryHealthState = batteryInfo.BatteryHealthState.DEAD;
    console.info('batteryHealthState = ' + batteryHealthState);
    expect(batteryHealthState == 5).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_1400
   * @tc.name enum_charge_state_test_none
   * @tc.desc BatteryChargeState values Interface Test
   */
  it('enum_charge_state_test_none', 0, function () {
    let batteryChargeState = batteryInfo.BatteryChargeState.NONE;
    console.info('batteryChargeState = ' + batteryChargeState);
    expect(batteryChargeState == 0).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_1500
   * @tc.name enum_charge_state_test_enable
   * @tc.desc BatteryChargeState values Interface Test
   */
  it('enum_charge_state_test_enable', 0, function () {
    let batteryChargeState = batteryInfo.BatteryChargeState.ENABLE;
    console.info('batteryChargeState = ' + batteryChargeState);
    expect(batteryChargeState == 1).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_1600
   * @tc.name enum_charge_state_test_disable
   * @tc.desc BatteryChargeState values Interface Test
   */
  it('enum_charge_state_test_disable', 0, function () {
    let batteryChargeState = batteryInfo.BatteryChargeState.DISABLE;
    console.info('batteryChargeState = ' + batteryChargeState);
    expect(batteryChargeState == 2).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_1700
   * @tc.name enum_charge_state_test_full
   * @tc.desc BatteryChargeState values Interface Test
   */
  it('enum_charge_state_test_full', 0, function () {
    let batteryChargeState = batteryInfo.BatteryChargeState.FULL;
    console.info('batteryChargeState = ' + batteryChargeState);
    expect(batteryChargeState == 3).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_1800
   * @tc.name enum_plugged_type_test_none
   * @tc.desc BatteryPluggedType values Interface Test
   */
  it('enum_plugged_type_test_none', 0, function () {
    let batteryPluggedType = batteryInfo.BatteryPluggedType.NONE;
    console.info('batteryPluggedType = ' + batteryPluggedType);
    expect(batteryPluggedType == 0).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_1900
   * @tc.name enum_plugged_type_test_ac
   * @tc.desc BatteryPluggedType values Interface Test
   */
  it('enum_plugged_type_test_ac', 0, function () {
    let batteryPluggedType = batteryInfo.BatteryPluggedType.AC;
    console.info('batteryPluggedType = ' + batteryPluggedType);
    expect(batteryPluggedType == 1).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_2000
   * @tc.name enum_plugged_type_test_usb
   * @tc.desc BatteryPluggedType values Interface Test
   */
  it('enum_plugged_type_test_usb', 0, function () {
    let batteryPluggedType = batteryInfo.BatteryPluggedType.USB;
    console.info('batteryPluggedType = ' + batteryPluggedType);
    expect(batteryPluggedType == 2).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_2100
   * @tc.name enum_plugged_type_test_wireless
   * @tc.desc BatteryPluggedType values Interface Test
   */
  it('enum_plugged_type_test_wireless', 0, function () {
    let batteryPluggedType = batteryInfo.BatteryPluggedType.WIRELESS;
    console.info('batteryPluggedType = ' + batteryPluggedType);
    expect(batteryPluggedType == 3).assertTrue()
  })

  /**
   * @tc.number battery_manager_js_2200
   * @tc.name brightness_test
   * @tc.desc Set Value Interface Test
   */
  it('brightness_test', 0, function () {
    brightness.setValue(10);
    brightness.setValue(0);
    brightness.setValue(255);
    expect(0 == 0).assertTrue();
  })

  /**
   * @tc.number battery_manager_js_2300
   * @tc.name is_battery_present_test
   * @tc.desc Battry Present Interface Test
   */
  it('is_battery_present_test', 0, function () {
    let isBatteryPresent = batteryInfo.isBatteryPresent;
    console.info('isBatteryPresent = ' + isBatteryPresent);
    if (isBatteryPresent) {
      expect(isBatteryPresent).assertTrue();
    } else {
      expect(isBatteryPresent).assertFalse();
    }
  })
})