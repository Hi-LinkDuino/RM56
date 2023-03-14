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

import prompt from '@system.prompt';
import BluetoothModel from './BluetoothModel'
import bluetooth from '@ohos.bluetooth';

//export function testSetSppBluetoothScanMode(mode: number, dur: number):String{
//  let message = "蓝牙还未使能"
////  let btEnable = AppStorage.get('bluetoothIsOn')
////  if (!btEnable) {
////    message = '蓝牙还未使能';
////    return;
////  }.
//  let ret = BluetoothModel.setBluetoothScanMode(4, 0);
//  if (ret) {
//    message = "设置成功"
//  } else {
//    message = "设置失败"
//  }
//  prompt.showToast({message: message})
//  return message
//}
let clientNumber = -1
export function testSppClientConnectSafe():String{
  let message = "sppClientConnectSafe test"
  console.info('testSppClientConnectSafe create! ');
  let uuid =AppStorage.Get('inputUuid')
  let deviceId =AppStorage.Get('inputDeviceId')
  console.info('bluetoothModel sppConnect: ' + deviceId);
  bluetooth.sppConnect(deviceId, {
    uuid: uuid,
    secure: true,
    type: 0
  }, (code, socketID) => {
    console.log("spp client code:"+code.code)
    if (code.code != 0) {
      message = "客户端连接失败!";
      console.info('bluetoothModel sppConnect callback socketNumber(safe mode): ' + code.code);
      prompt.showToast({message: message})
      return message;
    }
    acceptClientSocket(socketID);
    console.info('bluetoothModel sppConnect callback socketNumber(safe mode): ' + socketID);
    message = "客户端连接成功!";
    AppStorage.SetOrCreate("clientNumber",clientNumber)
    prompt.showToast({message: message})
    return message
  })
  prompt.showToast({message: message})
  return message
}

export function testSppClientConnectUnSafe():String{
  let message = "sppClientConnectUnSafe test"
  console.info('testSppClientConnectUnSafe create! ');
  let uuid =AppStorage.Get('inputUuid')
  let deviceId =AppStorage.Get('inputDeviceId')
  console.info('bluetoothModel sppConnect: ' + deviceId);
  bluetooth.sppConnect(deviceId, {
    uuid: uuid,
    secure: false,
    type: 0
  }, (code, socketID) => {
    if (code.code != 0) {
      message = "客户端连接失败!";
      console.info('bluetoothModel sppConnect callback socketNumber(non-safe mode): ' + socketID);
      prompt.showToast({message: message})
      return message;
    }
    acceptClientSocket(socketID);
    console.info('bluetoothModel sppConnect callback socketNumber(non-safe mode): ' + socketID);
    message = "客户端连接成功!";
    prompt.showToast({message: message})
    return message
  });
  prompt.showToast({message: message})
  return message
}

export function testSppClientWrite():String{
  let message = "sppClientWrite test"
  console.info( 'bluetoothModel sppWrite clientNumber: ' + JSON.stringify(clientNumber));
  let writeClientData =AppStorage.Get("writeClientData")
  let writeArrayBuffer = strToArrayBuffer(writeClientData)
  const sppWriteSuccess = bluetooth.sppWrite(clientNumber, writeArrayBuffer);
  AppStorage.SetOrCreate("clientNumber",clientNumber)
  if (sppWriteSuccess) {
    console.info('sppWrite successfully');
    message = "写入成功 ";
  } else {
    console.info('sppWrite failed');
    message = "写入失败";
  }
  prompt.showToast({message: message})
  return message
}

export function testSppClientReadOn():String{
  let message = "sppClientReadOn test"
  bluetooth.on('sppRead', clientNumber, (dataBuffer: ArrayBuffer) => {
    const data = new Uint8Array(dataBuffer);
    console.info('spp bluetooth read is: ' + data);
    if(data != null) {
      message = "读取成功" + JSON.stringify(data);
    } else {
      message = "读取失败";
    }
    AppStorage.SetOrCreate("dataReadMessage",message)
    prompt.showToast({message: message})
    return message
  });
  prompt.showToast({message: message})
  return message
}

export function testSppClientReadOff():String{
  let message = "sppClientReadOff test"
  bluetooth.off('sppRead', clientNumber, (dataBuffer: ArrayBuffer) => {
    const data = new Uint8Array(dataBuffer);
    console.info('spp bluetooth read is: ' + data);
    if(data != null) {
      message = "读取成功" + JSON.stringify(data);
    } else {
      message = "读取失败";
    }
    AppStorage.SetOrCreate("dataReadMessage",message)
    prompt.showToast({message: message})
    return message
  });
  prompt.showToast({message: message})
  return message
}

export function testSppClientClose():String{
  let message = "sppClientClose test"
  sppClientClose()
  prompt.showToast({message: message})
  return message
}

export function acceptClientSocket(number) {
  console.info('spp clientSocket socketID: ' + number);
  // 获取的clientNumber用作服务端后续读/写操作socket的id。
  clientNumber = number;
  AppStorage.SetOrCreate("clientNumber",clientNumber)
}

export function sppClientClose() {
  console.info('bluetoothModel sppCloseClientSocket clientNumber: ' + JSON.stringify(clientNumber))
  bluetooth.sppCloseClientSocket(clientNumber);
  let message = '关闭成功 clientNumber' + clientNumber;
  prompt.showToast({message: message})
  return message
}

export function strToArrayBuffer (str) {
  let array = new Uint8Array(str.length);
  for(var i = 0; i < str.length; i++) {
    array[i] = str.charCodeAt(i);
  }
  return array.buffer
}

export function dataRead(dataBuffer) {
  const data = new Uint8Array(dataBuffer);
  console.log('spp bluetooth read is: ' + data);
  if(data != null) {
    this.dataReadMessage = "读取成功" + JSON.stringify(data);
  } else {
    this.dataReadMessage = "读取失败";
  }
}