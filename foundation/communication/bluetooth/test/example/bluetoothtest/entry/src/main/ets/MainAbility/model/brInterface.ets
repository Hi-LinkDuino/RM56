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

/**
 *BR Test Page Of Bluetooth test
 */

import prompt from '@system.prompt';
import BluetoothModel from './BluetoothModel'
import Router from '@system.router';
import BluetoothDeviceController from '../controller/BluetoothDeviceController';
import bluetooth from '@ohos.bluetooth';
import bt from '@ohos.bluetooth';

const PAGE_URI_DEVICE_FOUND_MODE = 'pages/subManualApiTest/subBrTest/deviceFound';

export function testInputBluetooth(): string {
  //  let ret = btApi.enableBluetooth();
  //  let btEnable = ret;
  //  AppStorage.SetOrCreate('bluetoothIsOn', btEnable);
  //  let message = "蓝牙使能执行结果：" + ret;
  let message = "testInputBluetooth"
  prompt.showToast({ message: message })
  return message
}

let btEnable = false

export function testEnableBluetooth() {
  let message = "enableBluetooth test"
  if (btEnable) {
    message = '蓝牙已经使能';
    prompt.showToast({ message: message })
    return message
  }
  let ret = BluetoothModel.enableBluetooth();
  btEnable = true;
  AppStorage.SetOrCreate('bluetoothIsOn', btEnable);
  message = "蓝牙使能执行结果：" + ret;
  prompt.showToast({ message: message })
  return message
}

export function testDisableBluetooth(): string {


  let message = "disableBluetooth test"
  //  if (!btEnable) {
  //    message = '蓝牙还未使能';
  //    prompt.showToast({message: message})
  //    return message
  //  }
  let ret = BluetoothModel.disableBluetooth();
  btEnable = false;
  AppStorage.SetOrCreate('bluetoothIsOn', btEnable);
  message = "蓝牙去使能执行结果：" + ret;
  prompt.showToast({ message: message })
  return message
}

export function testGetState(): string {
  let ret = BluetoothModel.getState();
  let stateBT = "getState test"
  switch (ret) {
    case 0:
      stateBT = 'STATE_OFF';
      break;
    case 1:
      stateBT = 'STATE_TURNING_ON';
      break;
    case 2:
      stateBT = 'STATE_ON';
      break;
    case 3:
      stateBT = 'STATE_TURNING_OFF';
      break;
    case 4:
      stateBT = 'STATE_BLE_TURNING_ON';
      break;
    case 5:
      stateBT = 'STATE_BLE_ON';
      break;
    case 6:
      stateBT = 'STATE_BLE_TURNING_OFF';
      break;
    default:
      stateBT = '未知状态';
      break;
  }
  let message = "当前蓝牙的状态是：" + stateBT;
  prompt.showToast({ message: message })
  return message
}

export function testGetBtConnectionState(): string {
  let ret = BluetoothModel.getBtConnectionState();
  let message = "当前连接状态是：" + ret;
  prompt.showToast({ message: message })
  return message
}

export function testSetLocalName(name: string): string {
  let ret = BluetoothModel.setLocalName(name);
  let message = "设置：" + ret + "设备名称为：" + name;
  prompt.showToast({ message: message })
  return message
}

export function testGetLocalName(): string {
  let ret = BluetoothModel.getLocalName();
  let message = "本设备名称是：" + ret;
  prompt.showToast({ message: message })
  return message
}

export function testSetBluetoothScanMode(mode: number, dur: number): {} {
  let retObj = { mod: 0, duration: -1 }
  let ret = BluetoothModel.setBluetoothScanMode(mode, dur);
  //  let ret = true   调试使用
  if (ret) {
    AppStorage.SetOrCreate('setScanModeRet', ret);
    retObj.mod = mode;
    retObj.duration = dur;
  } else {
    console.info("BluetoothModel.setBluetoothScanMode failed!")
  }
  console.info("BluetoothModel.setBluetoothScanMode success!", retObj)
  let message = ret ? "setBluetoothScanMode执行成功" : "setBluetoothScanMode执行失败"
  prompt.showToast({ message: message })
  return retObj
}

export function testGetBluetoothScanMode(): string {
  let ret = BluetoothModel.getBluetoothScanMode();
  let message = "模式: " + ret;
  prompt.showToast({ message: message })
  return message
}

export function testStartBluetoothDiscovery() {
  let message = "startBluetoothDiscovery test"
  let btEnable = AppStorage.Get('bluetoothIsOn')
  if (!btEnable) {
    message = '蓝牙还未使能';
    return message;
  } else {
    Router.push({ uri: PAGE_URI_DEVICE_FOUND_MODE });
    message = "蓝牙发现成功"
  }
  prompt.showToast({ message: message })
  return message
}

export function testStopBluetoothDiscovery(): string {
  let ret = BluetoothModel.stopBluetoothDiscovery();
  let message = "stopBluetoothDiscovery 执行：" + ret;
  prompt.showToast({ message: message })
  return message
}

export function testPairDevice(deviceId: string): string {
  let controller = new BluetoothDeviceController;
  let ret = bluetooth.pairDevice(deviceId);
  let message = ""
  controller.pair(deviceId, (pinCode: string) => {
    prompt.showToast({ message: pinCode })
    AppStorage.SetOrCreate('pairedMac', deviceId);
  }, () => {
    prompt.showToast({ message: "testPairDevice Failed" })
  })
  message = "配对：" + deviceId + ret ? "成功" : "失败"
  prompt.showToast({ message: message })
  return message
}

export function testUnPairedDevice(deviceId: string): string {
  //  let deviceId = "123123"
  let ret = BluetoothModel.unpairDevice(deviceId);
  let message = " 设备 " + deviceId + " 断开：" + ret;
  prompt.showToast({ message: message })
  return message
}

export function testGetRemoteDeviceName(deviceId: string): string {
  let ret = BluetoothModel.getDeviceName(deviceId);
  let message = '远程设备名:' + ret;
  prompt.showToast({ message: message })
  return message
}

export function testGetRemoteDeviceClass(deviceId: string): string {
  //  let deviceId= "12312fsdf";
  let ret = BluetoothModel.getDeviceType(deviceId);
  let message = '远程class名:' + ret;
  prompt.showToast({ message: message })
  return message
}

export function testGetPairedDevices(): string {
  let ret = BluetoothModel.getPairedDeviceIds();
  let message = "getPairedDevices test"
  console.log("BluetoothModel.getPairedDeviceIds:" + ret)
  if (ret.length == 0) {
    message = '当前没有任何配对设备';
  } else {
    message = "设备为：" + ret[0]
    AppStorage.SetOrCreate('pairedMac', ret[0]);
  }
  prompt.showToast({ message: message })
  return message
}


export function testGetProfileConnState1(profileId: number): string {
  let ret = BluetoothModel.getProfileConnState(profileId);
  let message = ret
  prompt.showToast({ message: message })
  console.log("br.testGetProfileConnState1:" + ret)
  return message
}

export function testGetProfileConnState2(profileId: number, deviceId: string): string {
  let ret = bt.getProfile(profileId).getDeviceState(deviceId);
  let stateProfile = ""
  console.log('testGetProfileConnState2 state is ' + ret);
  switch (ret) {
    case 0:
      stateProfile = 'STATE_DISCONNECTED';
      break;
    case 1:
      stateProfile = 'STATE_CONNECTING';
      break;
    case 2:
      stateProfile = 'STATE_CONNECTED';
      break;
    case 3:
      stateProfile = 'STATE_DISCONNECTING';
      break;
    default:
      stateProfile = '未知';
      break;
  }
  let message = stateProfile;
  prompt.showToast({ message: message })
  return message
}

let stateChange = false

export function testOnStateChange(): string {
  let message = "onStateChange test"
  console.log('testOnStateChange test');
  if (stateChange) {
    bluetooth.off('stateChange', () => {
      prompt.showToast({ message: "on.stateChange监听已关闭" })
    })
    message = "关闭注册状态变化"
    console.log('testOffStateChange test');
  } else {
    bluetooth.on('stateChange', (data: bluetooth.BluetoothState) => {
      let btStateMessage = ""
      switch (data) {
        case 0:
          btStateMessage += 'STATE_OFF';
          break;
        case 1:
          btStateMessage += 'STATE_TURNING_ON';
          break;
        case 2:
          btStateMessage += 'STATE_ON';
          break;
        case 3:
          btStateMessage += 'STATE_TURNING_OFF';
          break;
        case 4:
          btStateMessage += 'STATE_BLE_TURNING_ON';
          break;
        case 5:
          btStateMessage += 'STATE_BLE_ON';
          break;
        case 6:
          btStateMessage += 'STATE_BLE_TURNING_OFF';
          break;
        default:
          btStateMessage += '未知状态';
          break;
      }
      prompt.showToast({ message: "状态:" + btStateMessage })
    })
    message = "打开注册状态变化"
    prompt.showToast({ message: message })
  }
  stateChange = !stateChange
  return message
}

let pinRequired = false

export function testOnPinRequired(): string {
  let message = "onPinRequired test"
  console.log('testOnPinRequired test');
  if (pinRequired) {
    bluetooth.off('pinRequired', () => {
      prompt.showToast({ message: "on.pinRequired监听已关闭" })
    })
    message = "关闭注册pin码变化"
  } else {
    bluetooth.on('pinRequired', (data: bluetooth.PinRequiredParam) => {
      prompt.showToast({ message: "mac:" + data.deviceId + "pin:" + data.pinCode })
    })
    message = "打开注册pin码变化"
  }
  pinRequired = !pinRequired
  return message
}

let bondStateFlag = false

export function testOnBondStateChange(): string {
  let message = "onBondStateChange test"
  console.log('testOnBondStateChange test');
  if (bondStateFlag) {
    bluetooth.off('bondStateChange', () => {
      prompt.showToast({ message: "on.bondStateChange监听已关闭" })
    })
    message = "关闭注册绑定状态变化"
  } else {
    bluetooth.on('bondStateChange', (data: bluetooth.BondStateParam) => {
      prompt.showToast({ message: "mac:" + data.deviceId + "state:" + data.state })
    })
    message = "打开注册绑定状态变化"
  }
  bondStateFlag = !bondStateFlag
  return message
}

export function subscribeBluetoothDeviceFind(): string {
  let message = "subscribeBluetoothDeviceFind test"
  console.info("subscribeBluetoothDeviceFind")
  BluetoothModel.subscribeBluetoothDeviceFind((deviceIds: Array<string>) => {
    console.log('available bluetooth devices changed.');
    deviceIds?.forEach(async(deviceId)=> {
      console.log('available bluetooth find [' + deviceId + '] ')
      let message = ('available bluetooth find [' + deviceId + ']  ')
      prompt.showToast({ message: message })
      AppStorage.SetOrCreate('bluetoothAvailableDevices', message);
    })
  });
  prompt.showToast({ message: message })
  return message
}