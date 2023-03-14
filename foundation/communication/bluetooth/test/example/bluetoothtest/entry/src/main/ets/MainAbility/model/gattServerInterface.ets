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
import BluetoothModel from './BluetoothModel'

let gattServerInstance = null
export function testCreateGattServer():String{
  let btState = bluetooth.getState();
  let message ="createGattServer test"
  if (btState === bluetooth.BluetoothState.STATE_ON) {
    if (gattServerInstance) {
      message = "GattServer exists!";
      prompt.showToast({message: message})
      return message
    }
    gattServerInstance = BluetoothModel.createGattServer();
    message = "CreateGattServer:succeeds!";
    prompt.showToast({message: message})
    return message
  } else if (btState === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
  } else {
    message = "switching,wait!";
  }
  prompt.showToast({message: message})
  return message
}

export function testAddService():String{
  let message = "addService test"
  let btStatus = bluetooth.getState();
  if (btStatus === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({message: message})
    return message
  }
  if(!gattServerInstance) {
    message = "NO_GATT_Server_OBJECT";
    prompt.showToast({message: message})
    return message
  }
// build service
// 调这个本地函数 buildGattService();
  let curServiceUUID = AppStorage.Get("curServiceUUID")
  let curCharacteristicUUID = AppStorage.Get("curCharacteristicUUID")
  let curDescriptorUUID = AppStorage.Get("curDescriptorUUID")
  let curDescriptorValue = AppStorage.Get("curDescriptorValue")
  let curCharacteristicValue = AppStorage.Get("curCharacteristicValue")
  let isPrimary = AppStorage.Get("isPrimary")
  let descriptors = [];
  let descriptor = {
    serviceUuid:        curServiceUUID,
    characteristicUuid: curCharacteristicUUID,
    descriptorUuid:     curDescriptorUUID,
    descriptorValue:    string2ArrayBuffer(curDescriptorValue)
  };
  descriptors.push(descriptor);

// build characteristics
  let characteristics = [];
  let characteristic = {
    serviceUuid: curServiceUUID,
    characteristicUuid: curCharacteristicUUID,
    characteristicValue:string2ArrayBuffer(curCharacteristicValue),
    descriptors:descriptors
  };
  characteristics.push(characteristic);
  let curGattService = {
    serviceUuid: curServiceUUID,
    characteristics: characteristics,
    isPrimary: isPrimary,
    includeServices: []
  }
  let ret = gattServerInstance.addService(curGattService);
  message = "result: addService,"+ret;
  prompt.showToast({message: message})
  return message
}

export function testRemoveService():String{
  let message = "removeService test"
  let curServiceUUID = AppStorage.Get("curServiceUUID")
  let btState = bluetooth.getState();
  if (btState == bluetooth.BluetoothState.STATE_ON) {
    if(!gattServerInstance) {
      message = "NO_GATT_Server_OBJECT";
      prompt.showToast({message: message})
      return message
    }
    message = "result: removeService ";
    if(gattServerInstance.removeService(curServiceUUID)) {
      message += 'true';
    } else {
      message += 'false';
    }
  } else if (btState == bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
  } else {
    message = "switching,please wait!";
  }
  prompt.showToast({message: message})
  return message
}

let curAdvData: bluetooth.AdvertiseData = {
  serviceUuids:["00003344-0000-1000-8000-00805f9b34fb","00008899-0000-1000-8000-00805f9b34fb"],
  manufactureData:[{
    manufactureId:7456,
    manufactureValue:string2ArrayBuffer("OHOS CHN")
  }],
  serviceData:[{
    serviceUuid:"000054F6-0000-1000-8000-00805f9b34fb",
    serviceValue:string2ArrayBuffer("OhosBle!")
  }, {
    serviceUuid:"00009999-0000-1000-8000-00805f9b34fb",
    serviceValue:string2ArrayBuffer("dudu_tiger")
  }]
};

let curAdvResponse:bluetooth.AdvertiseData ={
  serviceUuids:[//"000054F6-0000-1000-8000-00805f9b34fb",
    "00009999-0000-1000-8000-00805f9b34fb"],
  manufactureData:[{
    manufactureId:6547,
    manufactureValue:string2ArrayBuffer("OHOS CHN")
  }],
  serviceData:[{
    serviceUuid:"000054F6-0000-1000-8000-00805f9b34fb",
    serviceValue:string2ArrayBuffer("OhosBle!")
  },{
    serviceUuid:"00007456-0000-1000-8000-00805f9b34fb",
    serviceValue:string2ArrayBuffer("OhosBle")
  }]
};

export function testStartAdvertising():String{
  let message = "startAdvertising test"
  let btState = bluetooth.getState();
  if (btState === bluetooth.BluetoothState.STATE_ON) {
    if (!gattServerInstance) {
      message = "NO_GATT_Server_OBJECT";
      prompt.showToast({message: message})
      return message
    }
    message = "startAdvertising:build input parameter In!";
// 调三个本地函数
// buildAdvSettings();
    let cbxAdvSettings=AppStorage.Get("cbxAdvSettings")
    let cbxInterval=AppStorage.Get("cbxInterval")
    let cbxTxPower=AppStorage.Get("cbxTxPower")
    let curAdvSettings ={
      interval:800,  //500ms
      txPower:-7 ,
      connectable:true
    }
    if(cbxAdvSettings) {
      if (cbxInterval) {
        let advSettings_interval = AppStorage.Get("advSettings_interval")
        curAdvSettings.interval = parseInt(advSettings_interval)
      }
      if (cbxTxPower) {
        let advSettings_txPower = AppStorage.Get("advSettings_txPower")
        curAdvSettings.txPower = parseInt(advSettings_txPower)
      }
        let advSettings_connectable = AppStorage.Get("advSettings_connectable")
        curAdvSettings.connectable = advSettings_connectable
    }

//    buildAdvData();
    let advDataServiceUuids_serviceUuid = AppStorage.Get("advDataServiceUuids_serviceUuid")
    let advDataManufactureData_manufactureId =AppStorage.Get("advDataManufactureData_manufactureId")
    let advDataManufactureData_manufactureValue =AppStorage.Get("advDataManufactureData_manufactureValue")
    let advDataServiceData_serviceUuid = AppStorage.Get("advDataServiceData_serviceUuid")
    let advDataServiceData_serviceValue = AppStorage.Get("advDataServiceData_serviceValue")
    let advDataServiceUuidList = []
    let advDataManufactureDataList = []
    let advDataServiceDataList =[]
    let advDataManufactureDataInstance = {
      advDataManufactureId : parseInt(advDataManufactureData_manufactureId),
      advDataManufactureValue : string2ArrayBuffer(advDataManufactureData_manufactureValue)
    }
    let advDataServiceDataInstance = {
      advDataServiceUuid : advDataServiceData_serviceUuid,
      advDataServiceValue : string2ArrayBuffer(advDataServiceData_serviceValue)
    }
    let advDataServiceUuidListLength = advDataServiceUuidList.push(advDataServiceUuids_serviceUuid)
    let advDataManufactureDataListLength = advDataManufactureDataList.push(advDataManufactureDataInstance)
    let advDataServiceDataListLength = advDataServiceDataList.push(advDataServiceDataInstance)

    let curAdvData = {
      serviceUuids : advDataServiceUuidList,
      manufactureData : advDataManufactureDataList,
      serviceData : advDataServiceDataList
    }

//    buildAdvResponse();
    let advResponseServiceUuids_serviceUuid = AppStorage.Get("advResponseServiceUuids_serviceUuid")
    let advResponseManufactureData_manufactureId = AppStorage.Get("advResponseManufactureData_manufactureId")
    let advResponseManufactureData_manufactureValue = AppStorage.Get("advResponseManufactureData_manufactureValue")
    let advResponseServiceData_serviceUuid = AppStorage.Get("advResponseServiceData_serviceUuid")
    let advResponseServiceData_serviceValue = AppStorage.Get("advResponseServiceData_serviceValue")
    let advResponseServiceUuidList = []
    let advResponseManufactureDataList = []
    let advResponseServiceDataList = []
    let advResponseManufactureDataInstance = {
      manufactureId : parseInt(advResponseManufactureData_manufactureId),
      manufactureValue : string2ArrayBuffer(advResponseManufactureData_manufactureValue)
    }
    let advResponseServiceDataInstance = {
      serviceUuid : advResponseServiceData_serviceUuid,
      serviceValue : string2ArrayBuffer(advResponseServiceData_serviceValue)
    }

    let advResponseServiceUuidListLength = advResponseServiceUuidList.push(advResponseServiceUuids_serviceUuid)
    let advResponseManufactureDataListLength = advResponseManufactureDataList.push(advResponseManufactureDataInstance)
    let advResponseServiceDataListLength = advResponseServiceDataList.push(advResponseServiceDataInstance)

    let curAdvResponse = {
      serviceUuids : advResponseServiceUuidList,
      manufactureData : advResponseManufactureDataList,
      serviceData : advResponseServiceDataList
    }

    let setting = curAdvSettings;
    let advData = curAdvData;
    let advResponse = curAdvResponse;
    let cbxAdvResponse = AppStorage.Get("cbxAdvResponse")
    message = "startAdvertising:build input parameter Out!";

    if(!cbxAdvResponse)
    {
      console.log("advData.serviceUuids:"+ advData.serviceUuids + "\n")
      console.log("advData.manufactureData:" +advData.manufactureData + "\n")
      console.log("advData.serviceData:" +advData.serviceData + "\n")
      gattServerInstance.startAdvertising(setting,advData);
    } else {
      console.log("advResponse.serviceUuids:"+ advResponse.serviceUuids+ "\n")
      console.log("advResponse.manufactureData:" +advResponse.manufactureData+ "\n")
      console.log("advResponse.serviceData:" +advResponse.serviceData+ "\n")
      gattServerInstance.startAdvertising(setting,advData,advResponse);
    }
    console.log("cbxAdvSettings:"+ cbxAdvSettings+ "\n")
    console.log("cbxAdvResponse:"+ cbxAdvResponse+ "\n")
    message = "startAdvertising succeeds!" + cbxAdvSettings +","+ cbxAdvResponse;
  } else if (btState === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
  } else {
    message = "switching,please wait!";
  }
  prompt.showToast({message: message})
  return message
}

export function testStopAdvertising():String{
  let message = "stopAdvertising test"
  let btState = bluetooth.getState();
  if (btState == bluetooth.BluetoothState.STATE_ON) {
    if(!gattServerInstance) {
      message ="NO_GATT_Server_OBJECT";
      prompt.showToast({message: message})
      return message
    }
    gattServerInstance.stopAdvertising();
    message = "stopAdvertising succeeds.";
  } else if (btState == bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
  } else {
    message = "switching,please wait!";
  }
  prompt.showToast({message: message})
  return message
}

export function testGattServerClose():String{
  let message = "gattServerClose test"
  let btState = bluetooth.getState();
  if (btState == bluetooth.BluetoothState.STATE_ON) {
    if(!gattServerInstance) {
      message ="NO_GATT_Server_OBJECT";
      prompt.showToast({message: message})
      return message
    }
    gattServerInstance.close();
    message = "GattServer closed: succeeds";
  } else if (btState == bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
  } else {
    message = "switching,please wait!";
  }
  prompt.showToast({message: message})
  return message
}

//回调函数
let connectStateChange = false
export function testOnConnectStateChange():String{
  let message = "onConnectStateChange test"
  let btStatus = bluetooth.getState();
  if (btStatus === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({ message: message })
    return message
  }
  if (connectStateChange) {
    gattServerInstance.off("connectStateChange", () => {
    })
    message = "关闭连接状态变化 "
    prompt.showToast({ message: message })
    return message
  } else {
    gattServerInstance.on('connectStateChange', (data) => {
      connectStateMsg = "deviceId:" + data.deviceId + ",connectState:" + data.state;
      AppStorage.SetOrCreate("connect_StateMsg",data.state)
      prompt.showToast({message: connectStateMsg})
    })
    message = '打开连接状态变化'
  }
  connectStateChange = !connectStateChange
  prompt.showToast({message: message})
  return message
}

//回调函数
let characteristicRead =false
export function testOnCharacteristicRead():String{
  let message = "onCharacteristicRead test"
  let btStatus = bluetooth.getState();
  if (btStatus === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({message: message})
    return message
  }
  if (characteristicRead) {
    gattServerInstance.off("characteristicRead", () => {
    })
    message = "特征值读取 关闭"
    prompt.showToast({ message: message })
    return message
  } else {
    gattServerInstance.on('characteristicRead', (characteristicReadReq) => {

      let characteristicReadMsg = 'characteristicRead received!';
      // client address
      let clientDeviceId = characteristicReadReq.deviceId;
      // transId
      let transId = characteristicReadReq.transId;
      // the byte offset of the start position for reading characteristic value
      let offset = characteristicReadReq.offset;
      // service/characteristic Uuid
      let characteristicUuid = characteristicReadReq.characteristicUuid;
      let serviceUuid = characteristicReadReq.serviceUuid;

      characteristicReadMsg += 'characteristicRead input parameters: deviceId = ' + clientDeviceId+'\n';
      characteristicReadMsg += 'transId = '+ transId+'\n';
      characteristicReadMsg +="characteristicUuid = "+ characteristicUuid+'\n';
      characteristicReadMsg +="serviceUuid = "+ serviceUuid+'\n';
      AppStorage.SetOrCreate("characteristicReadMsg",characteristicReadMsg)      // build data for response
      let arrayBufferCCC = string2ArrayBuffer('DaYuBlue');

      let serverResponse = {
        deviceId: clientDeviceId,
        transId: transId,
        status: 0,
        offset: offset,
        value:arrayBufferCCC
      }

      let ret = gattServerInstance.sendResponse(serverResponse)
      message = 'characteristicRead sendResponse:'+ ret
      prompt.showToast({message: message})
    })
    message = '特征值读取 打开'
  }
  characteristicRead =!characteristicRead
  prompt.showToast({message: message})
  return message
}

//回调函数
let characteristicWrite =false
export function testOnCharacteristicWrite():String{
  let message = "onCharacteristicWrite test"
  let btStatus = bluetooth.getState();
  if (btStatus === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({message: message})
    return message
  }
  if (characteristicWrite) {
    gattServerInstance.off("characteristicWrite", () => {});
    message = "特征值写入 关闭"
    prompt.showToast({ message: message })
    return message
  } else{
    gattServerInstance.on('characteristicWrite', (characteristicWriteReq) => {
      btOnCharacteristicWrite = 'on characteristicWrite'
      characteristicWriteMsg = '';

      let arrayBufferCCC = string2ArrayBuffer("characteristicWriteForResponse");

      // client address
      let deviceId = characteristicWriteReq.deviceId;
      let transId = characteristicWriteReq.transId;
      let offset = characteristicWriteReq.offset;
      /** Whether this request should be pending for later operation */
      let isPrep = characteristicWriteReq.isPrep;
      /** Whether the remote client need a response */
      let needRsp = characteristicWriteReq.needRsp;
      /** Indicates the value to be written */
      let value =  new Uint8Array(characteristicWriteReq.value);
      let characteristicUuid = characteristicWriteReq.characteristicUuid;
      let serviceUuid = characteristicWriteReq.serviceUuid;


      characteristicWriteMsg += "The characteristicWriteReq input parameters: deviceId ="+ deviceId;
      characteristicWriteMsg += "transId =" + transId;
      characteristicWriteMsg += "offset ="+ offset;
      characteristicWriteMsg += "isPrep ="+ isPrep;
      characteristicWriteMsg += "needRsp ="+ needRsp;
      characteristicWriteMsg += "value ="+ JSON.stringify(value);
      characteristicWriteMsg += "characteristicUuid =" + characteristicUuid;
      characteristicWriteMsg += "serviceUuid ="+ serviceUuid;
      AppStorage.SetOrCreate("characteristicWriteMsg",characteristicWriteMsg)
      let serverResponse = {
        deviceId: deviceId,
        transId: transId,
        status: 0,
        offset: offset,
        value:arrayBufferCCC
      };
      // sendResponse
      if(needRsp)
      {
        let ret = gattServerInstance.sendResponse(serverResponse);
        message = 'characteristicWrite sendResponse:'+ ret;
        prompt.showToast({message: message})
      }
      notifyCharacteristic_notifyCharacteristic = {
        serviceUuid: serviceUuid,
        characteristicUuid:  characteristicUuid,
        characteristicValue: string2ArrayBuffer("Value4notifyCharacteristic"),
        confirm: true //notification:true; indication:false;
      }
      let notifyCharacteristic = notifyCharacteristic_notifyCharacteristic;
      if (gattServerInstance.notifyCharacteristicChanged(deviceId, notifyCharacteristic)) {
        message= "notifyCharacteristicChanged successfully!";
      } else {
        message= "notifyCharacteristicChanged failed!";
      }
    })
    message = '特征值写入 打开';
  }
  characteristicWrite =!characteristicWrite
  prompt.showToast({message: message})
  return message
}

//回调函数
let descriptorRead =false
export function testOnDescriptorRead():String{
  let message = "onDescriptorRead test"
  let btStatus = bluetooth.getState();
  if (btStatus === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    return message;
  }
  if (descriptorRead) {
    gattServerInstance.off("descriptorRead", () => {});
    message = '描述符读取 关闭';
    prompt.showToast({message: message})
    return message
  } else {
    gattServerInstance.on('descriptorRead', (descriptorReadReq) => {
      btOnDescriptorRead = 'on:descriptorRead'
      descriptorReadMsg = "descriptorReadIn!!";
      let deviceId = descriptorReadReq.deviceId;
      let transId = descriptorReadReq.transId;
      /** Indicates the byte offset of the start position for reading characteristic value */
      let offset = descriptorReadReq.offset;
      let descriptorUuid = descriptorReadReq.descriptorUuid;
      let characteristicUuid = descriptorReadReq.characteristicUuid;
      let serviceUuid = descriptorReadReq.serviceUuid;
      let arrayBufferDesc = string2ArrayBuffer("Response4descriptorRead");
      let serverResponse = {
        deviceId: deviceId,
        transId: transId,
        status: 0,
        offset: offset,
        value:arrayBufferDesc}
      let ret = gattServerInstance.sendResponse(serverResponse);
      message = 'descriptorRead sendResponse:'+ ret;
      descriptorReadMsg += message
      AppStorage.SetOrCreate("descriptorReadMsg",descriptorReadMsg)
      prompt.showToast({message: message})
    })
    message = '描述符读取 打开';
  }
  descriptorRead=!descriptorRead
  prompt.showToast({message: message})
  return message
}

//回调函数
let descriptorWrite = false
export function testOnDescriptorWrite():String{
  let message = "onDescriptorWrite test"
  let btStatus = bluetooth.getState();
  if (btStatus === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({message: message})
    return message
  }
  if (descriptorWrite) {
    gattServerInstance.off("descriptorWrite", () => {});
    message = '描述符写入 关闭';
    prompt.showToast({ message: message })
    return message
  } else{
    gattServerInstance.on('descriptorWrite', (descriptorWriteReq) => {
      btOnDescriptorWrite = 'on:descriptorWrite'
      descriptorWriteMsg = "descriptorWriteIn";
      descriptorWriteMsg+="The descriptorWriteReq client address: deviceId is "+ descriptorWriteReq.deviceId;
      let deviceId = descriptorWriteReq.deviceId;
      let transId = descriptorWriteReq.transId;
      /** Indicates the byte offset of the start position for writing descriptor value */
      let offset = descriptorWriteReq.offset;
      /** Whether this request should be pending for later operation */
      let isPrep = descriptorWriteReq.isPrep;
      /** Whether the remote client need a response */
      let needRsp = descriptorWriteReq.needRsp;
      /** Indicates the value to be written */
      let value = new Uint8Array(descriptorWriteReq.value);
      let descriptorUuid = descriptorWriteReq.descriptorUuid;
      let characteristicUuid = descriptorWriteReq.characteristicUuid;
      let serviceUuid = descriptorWriteReq.serviceUuid;

      let arrayBufferDesc = new ArrayBuffer(8);
      let descValue = new Uint8Array(arrayBufferDesc);
      descValue[0] = value[0];
      let serverResponse = {
      deviceId: deviceId,
      transId: transId,
      status: 0,
      offset: offset,
      value:arrayBufferDesc
      };

      if(needRsp) {
        let ret = gattServerInstance.sendResponse(serverResponse);
        message = 'descriptorWrite sendResponse:'+ ret;
        prompt.showToast({message: message})
      }
      descriptorWriteMsg+= 'descriptorWrite Callback finished!';
    })
    message = '描述符写入 打开';
  }
  descriptorWrite =!descriptorWrite
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