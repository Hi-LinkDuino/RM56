/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
 * @syscap SystemCapability.Communication.Bluetooth.Lite
 * @since 6
 */
export interface StartBLEScanOptions {
  interval: number;
  success: () => void;
  fail: (data: string, code: number) => void;
  complete: () => void;
}

/**
 * @syscap SystemCapability.Communication.Bluetooth.Lite
 * @since 6
 */
export interface StopBLEScanOptions {
  success: () => void;
  fail: (data: string, code: number) => void;
  complete: () => void;
}

/**
 * @syscap SystemCapability.Communication.Bluetooth.Lite
 * @since 6
 */
export interface BluetoothDevice {
  addrType: "public" | "random";
  addr: string;
  rssi: number;
  txpower: string;
  data: string;
}

/**
 * @syscap SystemCapability.Communication.Bluetooth.Lite
 * @since 6
 */
export interface BLEFoundResponse {
  devices: Array<BluetoothDevice>;
}

/**
 * @syscap SystemCapability.Communication.Bluetooth.Lite
 * @since 6
 */
export interface SubscribeBLEFoundOptions {
    success: (data: BLEFoundResponse) => void;
    fail: (data: string, code: number) => void;
}

export interface CallbackOptions {
  callback?: (status: number) => void;

  fail?: () => void;
}

/**
 * @syscap SystemCapability.Communication.Bluetooth.Lite
 * @since 6
 */
export default class Bluetooth {
  /**
   * Start BLE scan
   * @param options Options
   */
  static startBLEScan(options: StartBLEScanOptions): void;

  /**
   * Stop BLE scan
   * @param options Options
   */
  static stopBLEScan(options: StopBLEScanOptions): void;

  /**
   * Subscribe BLE found
   * @param options Options
   */
  static subscribeBLEFound(options: SubscribeBLEFoundOptions): void;

  /**
   * Stop the subscription of BLE found
   */
  static unsubscribeBLEFound(): void;

  static test_callback(options?: CallbackOptions): void;
  static on(data: string, callback: Callback): void;
  static off(data: string, callback?: Callback): void;
  static hangup():void;
  static answer():void;
  static bt_scan():void;
  static bt_scan_cancel():void;
  static bt_connect(data:string):void;
  static walkie_talkie_init():void;
  static walkie_talkie_deinit():void;
  static start_listen():void;
  static stop_listen():void;
  static start_talk():void;
  static stop_talk():void;
}
