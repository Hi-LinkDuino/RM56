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
  }
