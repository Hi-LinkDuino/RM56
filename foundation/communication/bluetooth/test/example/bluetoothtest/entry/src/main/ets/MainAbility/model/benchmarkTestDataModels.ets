/**
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
import { TestBenchmark, BenchmarkCategory } from './testData'
import { testOnStateChange, testEnableBluetooth, testGetState, testSetLocalName, testGetLocalName, testSetBluetoothScanMode, testGetBluetoothScanMode,
  testStartBluetoothDiscovery, testStopBluetoothDiscovery, testDisableBluetooth,testGetBtConnectionState,testOnPinRequired,testPairDevice}from './brInterface'
import { testGetConnectedBLEDevices, testStartBLEScan, testStopBLEScan, testOnBLEDeviceFind } from './bleInterface'
import {testCreateGattClient, testConnect, testDisconnect, testOnBLEConnectionStateChange, testGattClientClose, testGetServicesCallback,
  testReadCharacteristicValueCallback, testReadDescriptorValueCallback,testWriteCharacteristicValue, testWriteDescriptorValue, testOnBLECharacteristicChange,
  testSetNotifyCharacteristicChanged, testGetDeviceNameCallback, testGetRssiValueCallback} from './gattClientInterface'
import {testCreateGattServer,testStartAdvertising,testStopAdvertising,testAddService,testRemoveService,testGattServerClose,testOnConnectStateChange,
  testOnCharacteristicRead,testOnCharacteristicWrite,testOnDescriptorRead,testOnDescriptorWrite} from './gattServerInterface'
import {testSppClientConnectSafe,testSppClientWrite,testSppClientReadOn,testSppClientReadOff,testSppClientClose} from './sppClientInterface'
import {testSppServerCreateSafe,testSppServerAccept,testSppServerWrite,testSppServerReadOn,
  testSppServerReadOff,testSppCloseServerSocket,testSppCloseClientSocket} from './sppServerInterface'

export const BrBenchmarkTestCase: [] =[
  { 'name': '??????????????????', 'detail': '1', 'method': 'on.stateChange', 'api': testOnStateChange, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
  { 'name': '?????????', 'detail': '2', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
  { 'name': '????????????', 'detail': '3', 'method': 'getState', 'api': testGetState, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
  { 'name': '??????????????????', 'detail': '4', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
  { 'name': '??????????????????', 'detail': '5', 'method': 'getLocalName', 'api': testGetLocalName, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
  { 'name': '??????????????????', 'detail': '6', 'method': 'setBluetoothScanMode', 'api': testSetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
  { 'name': '??????????????????', 'detail': '7', 'method': 'getBluetoothScanMode', 'api': testGetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
//  { 'name': '??????????????????', 'detail': '8', 'method': 'startBluetoothDiscovery','api': testStartBluetoothDiscovery, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
//  { 'name': '??????????????????', 'detail': '9', 'method': 'stopBluetoothDiscovery', 'api': testStopBluetoothDiscovery, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
//  { 'name': '?????????', 'detail': '10', 'method': 'disableBluetooth', 'api': testDisableBluetooth, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark }
]

export const BleBenchmarkTestCase: [] =[
  { 'name': '??????????????????', 'detail': '1', 'method': 'on.stateChange', 'api': testOnStateChange, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
  { 'name': '?????????', 'detail': '2', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
  { 'name': '????????????', 'detail': '3', 'method': 'getState', 'api': testGetState, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
//  { 'name': '??????Ble??????????????????', 'detail': '4', 'method': 'on.BleDeviceFind', 'api': testOnBLEDeviceFind, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
  { 'name': '??????????????????', 'detail': '5', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
  { 'name': '??????????????????', 'detail': '6', 'method': 'getLocalName', 'api': testGetLocalName, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
  { 'name': '??????????????????', 'detail': '7', 'method': 'setBluetoothScanMode', 'api': testSetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
  { 'name': '??????????????????', 'detail': '8', 'method': 'getBluetoothScanMode', 'api': testGetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
//  { 'name': '??????Ble??????', 'detail': '5', 'method': 'startBleScan',  'api': testStartBLEScan, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
//  { 'name': '???????????????Ble??????', 'detail': '6', 'method': 'getConnectedBleDevices', 'api': testGetConnectedBLEDevices,'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
//  { 'name': '??????Ble??????', 'detail': '7', 'method': 'stopBleScan', 'api': testStopBLEScan,  'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
//  { 'name': '?????????', 'detail': '8', 'method': 'disableBluetooth', 'api': testDisableBluetooth, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark }
]

export const GattClientBenchmarkTestCase: [] =[
//  { 'name': '??????????????????', 'detail': '1', 'method': 'on.stateChange', 'api': testOnStateChange, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '?????????', 'detail': '2', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '????????????', 'detail': '3', 'method': 'getState', 'api': testGetState, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '??????????????????', 'detail': '4', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '??????????????????', 'detail': '5', 'method': 'getLocalName', 'api': testGetLocalName, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '??????????????????', 'detail': '6', 'method': 'setBluetoothScanMode', 'api': testSetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '??????????????????', 'detail': '7', 'method': 'getBluetoothScanMode', 'api': testGetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },

  { 'name': '??????Gatt?????????', 'detail': '4', 'method': 'createGattClient',  'api': testCreateGattClient,  'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  { 'name': 'Ble????????????', 'detail': '5', 'method': 'on.BleConnectionStateChange', 'api': testOnBLEConnectionStateChange, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  { 'name': '??????', 'detail': '6', 'method': 'connect',  'api': testConnect, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  { 'name': '????????????????????????', 'detail': '7', 'method': 'setNotifyCharacteristicChanged','api': testSetNotifyCharacteristicChanged, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  { 'name': 'Ble???????????????', 'detail': '8', 'method': 'on.BleCharacteristicChange', 'api': testOnBLECharacteristicChange, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  { 'name': '????????????callback', 'detail': '9', 'method': 'getServices(Callback)', 'api': testGetServicesCallback, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  //{ 'name': '????????????promise', 'detail': '11', 'method': 'getServices(promise)',  'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  { 'name': '???????????????callback', 'detail': '10', 'method': 'readCharacteristicValue(callback)', 'api': testReadCharacteristicValueCallback, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  //{ 'name': '???????????????promise', 'detail': '13', 'method': 'readCharacteristicValue(promise)',  'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '??????????????????callback', 'detail': '11', 'method': 'readDescriptorValue(callback)', 'api': testReadDescriptorValueCallback, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  //{ 'name': '??????????????????promise', 'detail': '15', 'method': 'readDescriptorValue(promise)',  'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  { 'name': '???????????????', 'detail': '11', 'method': 'writeCharacteristicValue', 'api': testWriteCharacteristicValue,  'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  //{ 'name': '??????????????????', 'detail': '12', 'method': 'writeDescriptorValue', 'api': testWriteDescriptorValue, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },

  //{ 'name': '??????????????????callback', 'detail': '12', 'method': 'getDeviceName()callback', 'api': testGetDeviceNameCallback, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  //{ 'name': '??????????????????promise', 'detail': '21', 'method': 'getDeviceName()Promise', 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '??????Rssi???callback', 'detail': '13', 'method': 'getRssiValue()Callback', 'api': testGetRssiValueCallback, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  //{ 'name': '??????Rssi???promise', 'detail': '23', 'method': 'getRssiValue()Promise',  'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '????????????', 'detail': '14', 'method': 'disconnect', 'api': testDisconnect, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '??????', 'detail': '15', 'method': 'close',  'api': testGattClientClose,'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  //  { 'name': '??????Ble??????', 'detail': '18', 'method': 'stopBleScan',  'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '?????????', 'detail': '16', 'method': 'disableBluetooth', 'api': testDisableBluetooth, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark }
]

export const GattServerBenchmarkTestCase: []  = [
//  { 'name': '??????????????????', 'detail': '1', 'method': 'on.stateChange', 'api': testOnStateChange, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '?????????', 'detail': '2', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '????????????', 'detail': '3', 'method': 'getState', 'api': testGetState, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '??????????????????', 'detail': '4', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '??????????????????', 'detail': '5', 'method': 'getLocalName', 'api': testGetLocalName, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '??????????????????', 'detail': '6', 'method': 'setBluetoothScanMode', 'api': testSetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '??????????????????', 'detail': '7', 'method': 'getBluetoothScanMode', 'api': testGetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },

  { 'name': '??????Gatt?????????', 'detail': '4', 'method': 'createGattServer', 'api': testCreateGattServer, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
  { 'name': '????????????', 'detail': '5', 'method': 'addService', 'api': testAddService,'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
  { 'name': '????????????', 'detail': '6', 'method': 'on.connectStateChange', 'api': testOnConnectStateChange,  'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
  { 'name': '????????????', 'detail': '7', 'method': 'startAdvertising', 'api': testStartAdvertising, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
  { 'name': '???????????????', 'detail': '8', 'method': 'on.characteristicRead', 'api': testOnCharacteristicRead, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
  { 'name': '???????????????', 'detail': '9', 'method': 'on.characteristicWrite',  'api': testOnCharacteristicWrite, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
  //  { 'name': '???????????????', 'detail': '9', 'method': 'on.descriptorRead', 'api': testOnDescriptorRead,  'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
  //  { 'name': '???????????????', 'detail': '11', 'method': 'on.descriptorWrite',  'api': testOnDescriptorWrite, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },

  //  { 'name': '????????????', 'detail': '10','method': 'stopAdvertising', 'api': testStopAdvertising,  'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '????????????', 'detail': '11', 'method': 'removeService', 'api': testRemoveService, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '??????', 'detail': '12', 'method': 'close',  'api': testGattServerClose,'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
  //  { 'name': '??????Ble??????', 'detail': '15', 'method': 'stopBleScan',  'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '?????????', 'detail': '13', 'method': 'disableBluetooth', 'api': testDisableBluetooth, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark }
]

export const SppClientBenchmarkTestCase:[]=[
  { 'name': '??????????????????', 'detail': '1', 'method': 'on.stateChange', 'api': testOnStateChange, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
  { 'name': '?????????', 'detail': '2', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
  { 'name': '????????????', 'detail': '3', 'method': 'getState', 'api': testGetState, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
  { 'name': '??????????????????', 'detail': '5', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
  { 'name': '??????????????????', 'detail': '6', 'method': 'getLocalName', 'api': testGetLocalName, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },

  { 'name': '??????SPP????????????', 'detail': '4', 'method': 'setBluetoothScanMode', 'api': testSetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
  { 'name': '??????????????????', 'detail': '5', 'method': 'getBluetoothScanMode', 'api': testGetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
  { 'name': 'Spp???????????????(????????????)', 'detail': '6', 'method': 'sppConnect', 'api': testSppClientConnectSafe, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },

//  { 'name': '????????????????????????', 'detail': '7', 'method': 'getBtConnectionState', 'api': testGetBtConnectionState, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
//  { 'name': 'Spp?????????????????????', 'detail': '9', 'method': 'on.sppRead',  'api': testSppClientReadOn,'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },

  //  { 'name': 'Spp???????????????(???????????????)', 'detail': '8', 'method': 'sppConnect','result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
  //  { 'name': '????????????', 'detail': '8', 'method': 'pairDevice', 'api': testPairDevice, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
//  { 'name': 'Spp???????????????', 'detail': '8', 'method': 'sppWrite', 'api': testSppClientWrite, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
//  { 'name': 'Spp?????????????????????', 'detail': '10', 'method': 'off.sppRead', 'api': testSppClientReadOff, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
//  { 'name': 'Spp???????????????', 'detail': '11', 'method': 'sppCloseClientSocket', 'api': testSppClientClose,'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
//  { 'name': '?????????', 'detail': '12', 'method': 'disableBluetooth', 'api': testDisableBluetooth, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
]

export const SppServerBenchmarkTestCase:[]=[
  { 'name': '??????????????????', 'detail': '1', 'method': 'on.stateChange', 'api': testOnStateChange, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
  { 'name': '?????????', 'detail': '2', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
  { 'name': '????????????', 'detail': '3', 'method': 'getState', 'api': testGetState, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
  { 'name': '??????????????????', 'detail': '5', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
  { 'name': '??????????????????', 'detail': '6', 'method': 'getLocalName', 'api': testGetLocalName, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },

  { 'name': '??????SPP????????????', 'detail': '4', 'method': 'setBluetoothScanMode',  'api': testSetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
  { 'name': '??????????????????', 'detail': '5', 'method': 'getBluetoothScanMode',  'api': testGetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': '??????pin?????????', 'detail': '6', 'method': 'on.pinRequired', 'api': testOnPinRequired, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp???????????????(????????????)', 'detail': '7', 'method': 'sppListen', 'api': testSppServerCreateSafe, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp???????????????', 'detail': '10', 'method': 'sppAccept', 'api': testSppServerAccept, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },

//  { 'name': '????????????????????????', 'detail': '8', 'method': 'getBtConnectionState', 'api': testGetBtConnectionState,  'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp?????????????????????', 'detail': '11', 'method': 'on.sppRead', 'api': testSppServerReadOn,  'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },


  //{ 'name': 'Spp???????????????(???????????????)', 'detail': '8', 'method': 'sppListen',  'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp???????????????', 'detail': '9', 'method': 'sppWrite', 'api': testSppServerWrite, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp???????????????', 'detail': '10', 'method': 'sppAccept', 'api': testSppServerAccept, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp?????????????????????', 'detail': '11', 'method': 'on.sppRead', 'api': testSppServerReadOn,  'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp?????????????????????', 'detail': '12', 'method': 'off.sppRead', 'api': testSppServerReadOff,  'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp???????????????', 'detail': '13', 'method': 'sppCloseServerSocket', 'api': testSppCloseServerSocket, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp???????????????', 'detail': '14', 'method': 'sppCloseClientSocket', 'api': testSppCloseClientSocket, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': '?????????', 'detail': '15', 'method': 'disableBluetooth', 'api': testDisableBluetooth, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
]


export function initBrBenchmarkData(): Array<TestBenchmark> {
  let TestBenchmarkArray: Array<TestBenchmark> = []
  BrBenchmarkTestCase.forEach(item => {
    TestBenchmarkArray.push(new TestBenchmark(item.name, item.detail,  item.method, item.api, item.result, item.Category));
  })
  return TestBenchmarkArray;
}

export function initBleBenchmarkData(): Array<TestBenchmark> {
  let TestBenchmarkArray: Array<TestBenchmark> = []
  BleBenchmarkTestCase.forEach(item => {
    TestBenchmarkArray.push(new TestBenchmark(item.name, item.detail,  item.method, item.api, item.result, item.Category));
  })
  return TestBenchmarkArray;
}

export function initGattClientBenchmarkData(): Array<TestBenchmark> {
  let TestBenchmarkArray: Array<TestBenchmark> = []
  GattClientBenchmarkTestCase.forEach(item => {
    TestBenchmarkArray.push(new TestBenchmark(item.name, item.detail,  item.method, item.api, item.result, item.Category));
  })
  return TestBenchmarkArray;
}

export function initGattServerBenchmarkData(): Array<TestBenchmark> {
  let TestBenchmarkArray: Array<TestBenchmark> = []
  GattServerBenchmarkTestCase.forEach(item => {
    TestBenchmarkArray.push(new TestBenchmark(item.name, item.detail,  item.method, item.api, item.result, item.Category));
  })
  return TestBenchmarkArray;
}

export function initSppClientBenchmarkData(): Array<TestBenchmark> {
  let TestBenchmarkArray: Array<TestBenchmark> = []
  SppClientBenchmarkTestCase.forEach(item => {
    TestBenchmarkArray.push(new TestBenchmark(item.name, item.detail,  item.method, item.api, item.result, item.Category));
  })
  return TestBenchmarkArray;
}

export function initSppServerBenchmarkData(): Array<TestBenchmark> {
  let TestBenchmarkArray: Array<TestBenchmark> = []
  SppServerBenchmarkTestCase.forEach(item => {
    TestBenchmarkArray.push(new TestBenchmark(item.name, item.detail,  item.method, item.api, item.result, item.Category));
  })
  return TestBenchmarkArray;
}