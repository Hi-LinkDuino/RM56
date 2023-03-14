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
import bluetooth from '@ohos.bluetooth';

let serverNumber: number = -1;
export function testSppServerCreateSafe():String{
  let message = "sppServerCreateSafe test"
  console.info('spp server socketId(safe mode) create! ');
  let uuid =AppStorage.Get('inputUuid')
  if (uuid == '') {
    message = 'uuid为空';
    prompt.showToast({message: message})
    return message
  }
  // 创建蓝牙服务器侦听套接字, 返回服务端ID
  bluetooth.sppListen('BluetoothDevice', {uuid: uuid, secure: true, type: 0}, (code,serverSocketID) => {
    console.info('bluetooth error code: ' + code.code);
    if (code.code == 0) {
      console.info('spp server: serverSocketID(safe mode) - ' + serverSocketID);
      serverNumber = serverSocketID;
      message = "创建server成功";
//      AppStorage.SetOrCreate("serverNumber",serverNumber)
      prompt.showToast({message: message})
      return message
    }
  });
  prompt.showToast({message: message})
  return message
}


export function testSppServerCreateUnSafe():String{
  let message = "sppServerCreateUnSafe test"
  let uuid =AppStorage.Get('inputUuid')
  console.info('spp server socketId(non-safe mode) create! ');
  if (uuid == '') {
    message = 'uuid为空';
    prompt.showToast({message: message})
    return message
  }
  // 创建蓝牙服务器侦听套接字, 返回服务端ID
  bluetooth.sppListen('BluetoothDevice', {uuid: uuid, secure: false, type: 0}, (code,serverSocketID) => {
    console.info('bluetooth error code: ' + code.code);
    if (code.code == 0) {
      console.info('spp server: serverSocketID(non-safe mode) ' + serverSocketID);
      serverNumber = serverSocketID;
      message = "创建server成功";
      prompt.showToast({message: message})
      return message
    }
  });
  prompt.showToast({message: message})
  return message
}

let clientNumber: number = -1;
export function testSppServerAccept():String {
  let message = "testSppServerAccept test"
//  serverNumber = AppStorage.Get("serverNumber")
  console.info('testSppServerAccept test')
  console.info('testSppServerAccept serverNumber ' + serverNumber)
  bluetooth.sppAccept(serverNumber, (code, clientSocketID) => {
    if (code.code == 0) {
      console.info('spp server: clientSocketID - ' + clientSocketID);
      console.info('test serverNumber ' + serverNumber);
      clientNumber = clientSocketID;
//      AppStorage.SetOrCreate("clientNumber",clientNumber)
      message = "接受客户端成功";
      bluetooth.on('sppRead', clientNumber,
        (dataBuffer: ArrayBuffer) => {
          const data = new Uint8Array(dataBuffer);
          console.info('spp bluetooth read is: ' + data);
          if(data != null) {
            message = "读取成功" + JSON.stringify(data);
          } else {
            message = "读取失败";
          }
          AppStorage.SetOrCreate("dataReadMessage",message)
        })
    }
    prompt.showToast({ message: message })
    return message
  })
  prompt.showToast({ message: message })
  return message
}

export function testSppServerWrite():String{
  let message = "sppServerWrite test"
  console.info(MODEL + 'sppWrite clientNumber: ' + JSON.stringify(clientNumber));
  let writeServerData =AppStorage.Get("writeServerData")
  let writeArrayBuffer = strToArrayBuffer(writeServerData)
  const sppWriteSuccess = bluetooth.sppWrite(clientNumber, writeArrayBuffer);
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


export function testSppServerReadOn():String{
  let message = "sppServerReadOn test"
  bluetooth.on('sppRead', clientNumber,
    (dataBuffer: ArrayBuffer) => {
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
    }
  );
  prompt.showToast({message: message})
  return message
}
export function testSppServerReadOff():String{
  let message = "sppServerReadOff test"
  bluetooth.off('sppRead', clientNumber,
    (dataBuffer: ArrayBuffer) => {
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
    }
  );
  prompt.showToast({message: message})
  return message
}

//关闭服务端serverSocket
let MODEL = "bluetoothModel:"
export function testSppCloseServerSocket():String{
  let message = "sppServerClose test"
  console.info(MODEL + 'sppCloseServerSocket serverNumber: ' + serverNumber)
  bluetooth.sppCloseServerSocket(serverNumber);
  message = "关闭服务端";
  prompt.showToast({message: message})
  return message
}

//关闭服务端accept回调的clientSocket
export function testSppCloseClientSocket():String{
  let message = "sppCloseClientSocket test"
  console.info(MODEL + 'sppCloseClientSocket clientNumber: ' + clientNumber)
  bluetooth.sppCloseClientSocket(clientNumber);
  message = "关闭客户端";
  prompt.showToast({message: message})
  return message
}

function strToArrayBuffer (str) {
  let array = new Uint8Array(str.length);
  for(var i = 0; i < str.length; i++) {
    array[i] = str.charCodeAt(i);
  }
  return array.buffer
}