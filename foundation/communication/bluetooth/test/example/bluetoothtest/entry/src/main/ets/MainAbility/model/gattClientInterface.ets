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
 * GattClient Test Of Bluetooth test
 */

import prompt from '@system.prompt';
import bluetooth from '@ohos.bluetooth';
import BluetoothModel from './BluetoothModel'

let gattClientInstance = null
export function testCreateGattClient():String {
  let message = "createGattClient test "
  let peripheralDeviceId = AppStorage.Get('peripheralDeviceId');
  let btState = bluetooth.getState();
  if (btState === bluetooth.BluetoothState.STATE_ON) {
    if (gattClientInstance) {
      message = "GattClient exists!";
      prompt.showToast({ message: message })
      return message;
    }
    //注册回调
    bluetooth.BLE.on("BLEDeviceFind", (scanResult) => {
      scanResult.forEach(device => {
        var devId = device.deviceId; //00:1A:7D:DA:71:15
        var rssi = device.rssi;
        var data = device.data;
        prompt.showToast({ message: "findDevice:"+devId })
        if (devId == peripheralDeviceId) {
          bluetooth.BLE.stopBLEScan();
          // create ble client instance
          if (!gattClientInstance) {
            gattClientInstance = BluetoothModel.createGattClient(devId);
            message = "createGattClient succeeds ";
          }
        }
      })
    })
    if (peripheralDeviceId && peripheralDeviceId.length > 15) {
      gattClientInstance = BluetoothModel.createGattClient(peripheralDeviceId.toUpperCase());
      if (gattClientInstance) {
        message = "createGattClient: " + peripheralDeviceId;
      } else {
        message = "createGattClient:fail ";
      }
    } else {
      message = "peripheralDeviceId invalid! ";
    }
    prompt.showToast({ message: message })
  } else if (btState === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({ message: message })
  } else {
    message = "switching,please wait"
    prompt.showToast({ message: message })
  }
  return message
}

let bleConnectionState = 0;
export function testConnect():String {
  let message = "connect test";
  let btState = bluetooth.getState();
  if (btState === bluetooth.BluetoothState.STATE_ON) {
    if (gattClientInstance) {
      if(bleConnectionState == 0) {
        let ret = gattClientInstance.connect();
        message = "GattClient.connect: " + ret;
        bleConnectionState = 2
      } else {
        message = "this device is not disconnected!";
      }
    } else {
      message = "NO_GATT_CLIENT_OBJECT";
    }
  } else if (btState === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
  } else {
    message = "switching,please wait!";
  }
  prompt.showToast({message: message})
  return message
}

export function testDisconnect():String{
  let message = "test disconnect";
  let btState = bluetooth.getState();
  if (btState == bluetooth.BluetoothState.STATE_ON) {
    if(!gattClientInstance) {
      message = "NO_GATT_CLIENT_OBJECT";
      prompt.showToast({message: message})
      return message;
    }
    if (bleConnectionState == 2) {
      let ret = gattClientInstance.disconnect();
      message = "GattClient.disconnect: " + ret;
    } else {
      message = " BLE is not connected and no need to disconnect it ";
    }
  } else if (btState == bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
  } else {
    message = "switching,please wait!";
  }
  prompt.showToast({message: message})
  return message
}

let BLEConnectionStateChange = false
export function testOnBLEConnectionStateChange():String{
  let message = "onBLEConnectionStateChange test";
  let btStatus = bluetooth.getState();
    if (btStatus === bluetooth.BluetoothState.STATE_OFF) {
      message = "BT is not enabled!";
      prompt.showToast({ message: message })
      return message;
    }
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({ message: message })
    return message;
  }
  message = " HAVE_GATT_CLIENT_OBJECT "
  if (BLEConnectionStateChange) {
    gattClientInstance.off('BLEConnectionStateChange', () => {
      prompt.showToast({message: "off:BLEConnectionStateChange"})
    })
    message = "关闭BLE连接状态变化"
  } else {
    gattClientInstance.on('BLEConnectionStateChange', (bleConnectionState) => {
      switch (bleConnectionState) {
        case bluetooth.ProfileConnectionState.STATE_DISCONNECTED:
          bleConnectionStateInfo = "DISCONNECTED";
          break;
        case bluetooth.ProfileConnectionState.STATE_CONNECTING:
          bleConnectionStateInfo = "CONNECTING";
          break;
        case bluetooth.ProfileConnectionState.STATE_CONNECTED:
          bleConnectionStateInfo = "STATE_CONNECTED";
          break;
        case bluetooth.ProfileConnectionState.STATE_DISCONNECTING:
          bleConnectionStateInfo = "STATE_DISCONNECTING";
          break;
        default:
          bleConnectionStateInfo = "UNDEFINED";
          break;
      }
      AppStorage.SetOrCreate('bleConnectionStateInfo', bleConnectionStateInfo);
      prompt.showToast({ message: "状态:" + bleConnectionStateInfo })
    })
    message = "打开BLE连接状态变化"
  }
  BLEConnectionStateChange = !BLEConnectionStateChange
  prompt.showToast({message: message})
  return message
}

export function testGattClientClose():String{
  let message = "gattClientClose test"
  let btState = bluetooth.getState();
  if (btState == bluetooth.BluetoothState.STATE_ON) {
    if (!gattClientInstance) {
      message = "NO_GATT_CLIENT_OBJECT";
      prompt.showToast({message: message})
      return message;
    }
    let ret = gattClientInstance.close();
    if (ret) {
      gattClientInstance = null;
    }
    message = "GattClient.close:" + ret;
  } else if (btState == bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
  } else {
    message = "switching,please wait!";
  }
  prompt.showToast({message: message})
  return message
}

export function testGetServicesCallback():String{
  let message = "getServicesCallback test"
  let btState = bluetooth.getState();
  if (btState == bluetooth.BluetoothState.STATE_ON) {
    if (!gattClientInstance) {
      message = "NO_GATT_CLIENT_OBJECT";
      prompt.showToast({message: message})
      return message;
    }
    gattClientInstance.getServices((code, gattServices) => {
      if (code.code != 0) {
        message = "getServices(call back) error code:" + code.code;
        prompt.showToast({message: message})
        return message;
      } else {
        message += "getServices(Callback):invoke succeeds! \n "
        message += "bluetooth services size is " + gattServices.length + '\n';
        for (let i = 0; i < gattServices.length; i++) {
          message += 'bluetooth serviceUuid is ' + gattServices[i].serviceUuid + '\n';
        }
        prompt.showToast({message: message})
        AppStorage.SetOrCreate("getServices", message)
        console.info("getServices(Callback)", message)
      }
    })
  } else if (btState == bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
  } else {
    message = "switching,please wait!";
  }
  prompt.showToast({message: message})
  return message
}

export function testGetServicesPromise():String{
  let message = "getServicesPromise test"
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({message: message})
    return message;
  }
  message = "getServices promise:starts";
  let btState = bluetooth.getState();
  if (btState == bluetooth.BluetoothState.STATE_ON) {
    gattClientInstance.getServices().then(gattServices => {
      message = "getServicesPromise successfully:\n"
      message = "bluetooth services size is " + gattServices.length;
      for (let i = 0; i < gattServices.length; i++) {
        message += 'bluetooth serviceUuid is ' + gattServices[i].serviceUuid + '\n';
      }
      prompt.showToast({message: message})
    })
    message = "getServices(promise):invoke succeeds!";
  } else if (btState == bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
  } else {
    message = "switching,please wait!";
  }
  prompt.showToast({message: message})
  return message
}

export function testReadCharacteristicValueCallback():String{
  let serviceUUID = AppStorage.Get('serviceUUID');
  let characteristicUUID = AppStorage.Get('characteristicUUID');
  let message = "readCharacteristicValueCallback test"
  let btState = bluetooth.getState();
  if (btState === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({message: message})
    return message;
  }
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({message: message})
    return message;
  }
  let bleCharacteristicDataIn = {
    serviceUuid: serviceUUID,
    characteristicUuid: characteristicUUID
  };
  gattClientInstance.readCharacteristicValue(bleCharacteristicDataIn, (err, bleCharacteristicDataOut) => {
    if (err.code != 0) {
      message = "readCharacteristicValue error code:" + err.code + ",id:"+serviceUUID;
      console.info(message)
      prompt.showToast({message: message})
      return message;
    }
    message = "readCharacteristicValue callback:";
    message += 'bluetooth characteristic uuid:' + bleCharacteristicDataOut.characteristicUuid + "\n";
    let value = new Uint8Array(bleCharacteristicDataOut.characteristicValue);
    for (let i = 0; i < bleCharacteristicDataOut.characteristicValue.byteLength; i++) {
      message += 'bluetooth characteristic value: ' + value[i];
    }
    AppStorage.SetOrCreate("read_CharacteristicValue",message)
    prompt.showToast({message: message})
  });
  message = 'readCharacteristicValue(callback):invoke succeeds!';
  prompt.showToast({message: message})
  return message
}

export function testReadCharacteristicValuePromise():String{
  let serviceUUID = AppStorage.Get('serviceUUID');
  let characteristicUUID = AppStorage.Get('characteristicUUID');
  let message = "readCharacteristicValuePromise test"
  let btState = bluetooth.getState();
  if (btState === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({message: message})
    return message;
  }
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({message: message})
    return message;
  }
  let bleCharacteristicDataIn = {
    serviceUuid: serviceUUID,
    characteristicUuid: characteristicUUID
  };
  gattClientInstance.readCharacteristicValue(bleCharacteristicDataIn).then((bleCharacteristicDataOut) => {
    message = "readCharacteristicValue promise:\n"
    message += 'bluetooth characteristic uuid:' + bleCharacteristicDataOut.characteristicUuid + "\n";
    let value = new Uint8Array(bleCharacteristicDataOut.characteristicValue);
    for (let i = 0; i < bleCharacteristicDataOut.characteristicValue.byteLength; i++) {
      message += 'bluetooth characteristic value: ' + value[i];
    }
    prompt.showToast({message: message})
  });
  message = 'readCharacteristicValue(promise):invoke succeeds!';
  prompt.showToast({message: message})
  return message
}

export function testReadDescriptorValueCallback():String{
  let serviceUUID = AppStorage.Get('serviceUUID');
  let characteristicUUID = AppStorage.Get('characteristicUUID');
  let descriptorUUID = AppStorage.Get('descriptorUUID');
  let message = "readDescriptorValueCallback test"
  let btState = bluetooth.getState();
  if (btState === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({message: message})
    return message;
  }
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({message: message})
    return message;
  }
  let bufferDesc = new ArrayBuffer(8);
  let descV = new Uint8Array(bufferDesc);
  descV[0] = 88;
  let descriptorIn = {

    serviceUuid: serviceUUID,
    characteristicUuid: characteristicUUID,
    descriptorUuid: descriptorUUID,
    descriptorValue: bufferDesc
  };
  gattClientInstance.readDescriptorValue(descriptorIn, (err, descriptorOut) => {
    let message = ""
    if (err.code != 0) {
      message = "readDescriptorValue error:" + err.code.toString()
      prompt.showToast({message: message})
      return message;
    }
    message = 'readDescriptorValue(callback):descriptor uuid=' + descriptorOut.descriptorUuid + "\n";
    let value = new Uint8Array(descriptorOut.descriptorValue);
    for (let i = 0; i < descriptorOut.descriptorValue.byteLength; i++) {
      message += 'descriptor value: ' + value[i] + "\n";
    }
    prompt.showToast({message: message})
  });
  message = 'readDescriptorValue(callback):invoke succeeds!';
  prompt.showToast({message: message})
  return message
}

export function testReadDescriptorValuePromise():String{
  let serviceUUID = AppStorage.Get('serviceUUID');
  let characteristicUUID = AppStorage.Get('characteristicUUID');
  let descriptorUUID = AppStorage.Get('descriptorUUID');
  let message = "readDescriptorValuePromise test "
  let btState = bluetooth.getState();
  if (btState === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({message: message})
    return message;
  }
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({message: message})
    return message;
  }
  let bufferDesc = new ArrayBuffer(8);
  let descV = new Uint8Array(bufferDesc);
  descV[0] = 99;
  let descriptorIn = {
    serviceUuid: serviceUUID,
    characteristicUuid: characteristicUUID,
    descriptorUuid: descriptorUUID,
    descriptorValue: bufferDesc
  };
  gattClientInstance.readDescriptorValue(descriptorIn).then((descriptorOut) => {
    message = 'readDescriptorValue(promise):descriptor uuid=' + descriptorOut.descriptorUuid + "\n";
    let value = new Uint8Array(descriptorOut.descriptorValue);
    for (let i = 0; i < descriptorOut.descriptorValue.byteLength; i++) {
      message += 'descriptor value: ' + value[i] + "\n";
    }
    prompt.showToast({message: message})
  });
  message = 'readDescriptorValue(promise):invoke succeeds!';
  prompt.showToast({message: message})
  return message
}

export function testWriteCharacteristicValue():String{
  let serviceUUID = AppStorage.Get('serviceUUID');
  let characteristicUUID = AppStorage.Get('characteristicUUID');
  let characteristicValue = AppStorage.Get('characteristicValue');
  let message = "writeCharacteristicValue test"
  let btState = bluetooth.getState();
  if (btState === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({message: message})
    return message;
  }
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({message: message})
    return message;
  }
  let bufferCCC = string2ArrayBuffer(characteristicValue);
  let characteristic = {
    serviceUuid: serviceUUID,
    characteristicUuid: characteristicUUID,
    characteristicValue: bufferCCC
  };
  if (gattClientInstance.writeCharacteristicValue(characteristic)) {
    message = 'writeCharacteristicValue succeeds.';
  } else {
    message = 'writeCharacteristicValue failed!';
  }
  prompt.showToast({message: message})
  return message
}

export function string2ArrayBuffer (str) {
  let array = new Uint8Array(str.length);
  for(var i = 0; i < str.length; i++) {
    array[i] = str.charCodeAt(i);
  }
  return array.buffer
}

export function testWriteDescriptorValue():String{
  let serviceUUID = AppStorage.Get('serviceUUID');
  let characteristicUUID = AppStorage.Get('characteristicUUID');
  let descriptorUUID = AppStorage.Get('descriptorUUID');
  let descriptorValue = AppStorage.Get('descriptorValue');
  let message = "writeDescriptorValue test"
  let btState = bluetooth.getState();
  if (btState === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({message: message})
    return message;
  }
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({message: message})
    return message;
  }
  let bufferDesc = string2ArrayBuffer(descriptorValue);
  let descriptor = {
    serviceUuid: serviceUUID,
    characteristicUuid: characteristicUUID,
    descriptorUuid: descriptorUUID,
    descriptorValue: bufferDesc };
  if (gattClientInstance.writeDescriptorValue(descriptor)) {
    message = 'writeDescriptorValue succeeds.';
  }
  else {
    message = 'writeDescriptorValue,failed!';
  }
  prompt.showToast({message: message})
  return message
}

//回调函数
let BLECharacteristicChange = false
export function testOnBLECharacteristicChange():String{
  let message = "onBLECharacteristicChange test"
  let btStatus = bluetooth.getState();
  if (btStatus === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({message: message})
    return message;
  }
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({message: message})
    return message;
  }

  if (BLECharacteristicChange) {
    gattClientInstance.off('BLECharacteristicChange', () => {
      message = "off BLECharacteristicChange"
      prompt.showToast({message: message})
    })
  } else {
    gattClientInstance.on('BLECharacteristicChange', (data) => {
      let serviceUuid = data.serviceUuid;
      let characteristicUuid = data.characteristicUuid;
      bleCharacteristicChangeInfo += "serviceUuid:" + serviceUuid + "\n";
      bleCharacteristicChangeInfo += "characteristicUuid:" + characteristicUuid + "\n";
      let value = new Uint8Array(data.characteristicValue);
      for (let i = 0; i < data.characteristicValue.byteLength; i++) {
        bleCharacteristicChangeInfo += "\n" + 'value:' + value[i]
      }
      AppStorage.SetOrCreate("bleCharacteristicChangeInfo", bleCharacteristicChangeInfo)
      prompt.showToast({message: "on:BLECharacteristicChange" })
    })
    message = "On BLECharacteristicChange"
    }
  BLECharacteristicChange = !BLECharacteristicChange
  prompt.showToast({message: message})
  return message
}

export function testSetBLEMtuSize():String{
  let message = "setBLEMtuSize test"
  let btStatus = bluetooth.getState();
  if (btStatus === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({message: message})
    return message;
  }
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({message: message})
    return message;
  }

  message = 'setBLEMtuSize: \n';
  let mtuSize = 256;
  let ret = gattClientInstance.setBLEMtuSize(mtuSize);
  message += "result : " + ret;
  prompt.showToast({message: message})
  return message
}

export function testSetNotifyCharacteristicChanged():String{
  let serviceUUID = AppStorage.Get('serviceUUID');
  let characteristicUUID = AppStorage.Get('characteristicUUID');
  let descriptorUUID = AppStorage.Get('descriptorUUID');
  let descriptorValue = AppStorage.Get('descriptorValue');
  let message = "setNotifyCharacteristicChanged test"
  let btStatus = bluetooth.getState();
  if (btStatus === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({message: message})
    return message;
  }
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({message: message})
    return message;
  }

  let descriptors = [];
  let bufferCCC = new ArrayBuffer(8);
  let cccV = new Uint8Array(bufferCCC);
  cccV[0] = 66;
  let descriptor = {

    serviceUuid: serviceUUID,
    characteristicUuid: characteristicUUID,
    descriptorUuid: descriptorUUID,
    descriptorValue: string2ArrayBuffer(descriptorValue)
  }
  descriptors.push(descriptor);
  let characteristicIn = {
    serviceUuid: serviceUUID,
    characteristicUuid: characteristicUUID,
    characteristicValue: bufferCCC,
    descriptors: descriptors
  };
  if (gattClientInstance.setNotifyCharacteristicChanged(characteristicIn, true)) {
    message = "setNotifyCharacteristicChanged(true):succeeds."
  } else {
    message = "setNotifyCharacteristicChanged(true):failed!"
    if (gattClientInstance.setNotifyCharacteristicChanged(characteristicIn, false)) {
      message = "setNotifyCharacteristicChanged(false):true!"
    } else {
      message = "setNotifyCharacteristicChanged(false):false!"
    }
  }
  prompt.showToast({message: message})
  return message
}

export function testGetDeviceNameCallback():String{
  let deviceName =""
  let message = "getDeviceNameCallback test"
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({message: message})
    return message;
  }
  //currentClick = 15;
  gattClientInstance.getDeviceName((err, asyncRemoteDeviceName) => {
    message = "return code:"
    message += JSON.stringify(err);
    message += ",and DeviceName is: "
    message += JSON.stringify(asyncRemoteDeviceName);
    deviceName = JSON.stringify(asyncRemoteDeviceName)
    AppStorage.SetOrCreate("deviceName", deviceName)
    prompt.showToast({message: message})
  });
  prompt.showToast({message: message})
  return message
}

export function testGetDeviceNamePromise():String{
  let deviceName =""
  let message = "getDeviceNamePromise test"
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({message: message})
    return message;
  }

 gattClientInstance.getDeviceName().then((peripheralDeviceName) => {
   message = "getDeviceName()Promise:" + peripheralDeviceName;
   deviceName = peripheralDeviceName
   AppStorage.SetOrCreate("deviceName", deviceName)
   prompt.showToast({message: message})
  });
  prompt.showToast({message: message})
  return message
}

export function testGetRssiValueCallback():String{
  let Rssi =""
  let message = "getRssiValueCallback test"
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({message: message})
    return message;
  }

  message = "getRssiValue callback";
  gattClientInstance.getRssiValue((err, cbRssi) => {
    message = "return code:"
    message += JSON.stringify(err);
    message += ",and RSSI="
    message += JSON.stringify(cbRssi);
    Rssi = JSON.stringify(cbRssi);
    AppStorage.SetOrCreate("Rssi", Rssi)
    prompt.showToast({message: message })
  });
  prompt.showToast({message: message})
  return message
}

export function testGetRssiValuePromise():String{
  let Rssi =""
  let message = "getRssiValuePromise test"
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({message: message })
    return message;
  }
  message = "getRssiValue promise:";
  gattClientInstance.getRssiValue().then((rssiValue) => {
    message += 'rssi' + JSON.stringify(rssiValue)
    Rssi = JSON.stringify(rssiValue)
    AppStorage.SetOrCreate("Rssi", Rssi)
    prompt.showToast({message: message})
  })
  prompt.showToast({message: message})
  return message
}