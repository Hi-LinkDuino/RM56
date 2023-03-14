/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
// @ts-nocheck
/**
 * BLE Test Page Of Bluetooth test
 */
import prompt from '@system.prompt';
import BluetoothModel from './BluetoothModel'
import bluetooth from '@ohos.bluetooth';

let gattServerInstance = null;
export function testCreateGattServerDevice():string{
  let message = "createGattServerDevice test"
  let btState = bluetooth.getState();
  if (btState == bluetooth.BluetoothState.STATE_ON) {
    message = "createGattServer starts";
    if (!gattServerInstance) {
     gattServerInstance = BluetoothModel.createGattServer();
     message = "createGattServer: succeeds.";
    } else {
     message = "GattServer existed!";
    }
  } else if (btState == bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
  } else {
    message = "switching,please wait!";
  }
  prompt.showToast({message: message})
  return message
}

let gattClientDeviceInstance = null
export function testCreateGattClientDevice():string{
  let message = "test createGattClientDevice"
  let btState = bluetooth.getState();
  let peripheralDeviceId = AppStorage.Get('peripheralDeviceId');
  if (btState == bluetooth.BluetoothState.STATE_ON) {
    message = "createGattClient start"
    if (!gattClientDeviceInstance) {
      gattClientDeviceInstance = BluetoothModel.createGattClient(peripheralDeviceId);
      message = "createGattClient:succeeds.";
    } else {
      message = "gattClient existed!";
    }
  } else if (btState == bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
  } else {
    message = "switching,please wait!";
  }
  prompt.showToast({message: message})
  return message
}

export function testGetConnectedBLEDevices():string{
  let connectedBleDevices = BluetoothModel.getConnectedBLEDevices();
  let message = "getConnectedBLEDevices" + JSON.stringify(connectedBleDevices);
  console.log("getConnectedBLEDevices" +JSON.stringify(connectedBleDevices))
  prompt.showToast({message: message})
  return message
}

export function testStartBLEScan():string {
  let btState = bluetooth.getState();
  let message ="test startBLEScan"
  if (btState == bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    return message;
  }
  let txtScanFilter_deviceId = AppStorage.Get('txtScanFilterDeviceId');
  let txtScanFilter_name = AppStorage.Get('txtScanFilterName');
  let txtScanFilter_serviceUuid = AppStorage.Get('txtScanFilterServiceUuid');
  let txtScanOptions_interval = AppStorage.Get('txtScanOptions_interval');
  let txtScanOptions_dutyMode = AppStorage.Get('txtScanOptionsDutyMode');
  let txtScanOptions_matchMode = AppStorage.Get('txtScanOptionsMatchMode');
  let cbxBleScanFilter = AppStorage.Get('cbxBleScanFilter');
  let cbxBleScanOptions = AppStorage.Get('cbxBleScanOptions');
  let curScanFilters = [];
  let bleScanFilter = {
    deviceId: txtScanFilter_deviceId,
    name: txtScanFilter_name,
    serviceUuid: txtScanFilter_serviceUuid
  }
  let curScanOptions = {
    interval:  parseInt(txtScanOptions_interval),
    dutyMode:  parseInt(txtScanOptions_dutyMode),
    matchMode: parseInt(txtScanOptions_matchMode)
  }
  if (cbxBleScanFilter) {
    curScanFilters.push(bleScanFilter);
  }

  if (!cbxBleScanOptions) {
    if (cbxBleScanFilter) {
      BluetoothModel.startBLEScan(curScanFilters);
      message = "startBLEScan Filters:" + JSON.stringify(curScanFilters);
    } else {
      BluetoothModel.startBLEScan(null);
      message = "startBLEScan(null)";
    }
  } else {
    BluetoothModel.startBLEScan(curScanFilters, curScanOptions);
    message = "startBLEScan Filters:"+JSON.stringify(curScanFilters) + "\n" + "Options:"+JSON.stringify(curScanOptions);
  }
  prompt.showToast({message: message})
  return message
}

export function testStopBLEScan():string {
  BluetoothModel.stopBLEScan();
  let message = "stopBLEScan OK!";
  prompt.showToast({message: message})
  return message
}

let bleDeviceFind = false
export function testOnBLEDeviceFind():string {
  let message = "onBLEDeviceFind test"
  if (bleDeviceFind) {
    BluetoothModel.unsubscribeBLEDeviceFind();
    message = '关闭BLE查找状态变化'
    prompt.showToast({ message: message })
  } else {
    BluetoothModel.subscribeBLEDeviceFind((deviceItems: Array<bluetooth.ScanResult>) => {
      LogUtil.log("BLE Test:" + 'available ble devices changed.');
      let message = ""
      let availBleDeviceIds = []
      deviceItems?.forEach(deviceItem => {
        let newDevice = deviceItem.deviceId;
        message += "mac:" + newDevice;
        LogUtil.log("BLE Test:" + 'available bluetooth new device : ' + newDevice);
        availBleDeviceIds.push(newDevice);
      })
      AppStorage.SetOrCreate('bluetoothAvailableDevices', availBleDeviceIds);
      prompt.showToast({message: message})
    })
    message = "打开BLE查找状态变化"
    prompt.showToast({ message: message })
  }
  bleDeviceFind = !bleDeviceFind
  return message
}
