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
  { 'name': '注册状态变化', 'detail': '1', 'method': 'on.stateChange', 'api': testOnStateChange, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
  { 'name': '开蓝牙', 'detail': '2', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
  { 'name': '获取状态', 'detail': '3', 'method': 'getState', 'api': testGetState, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
  { 'name': '设置本地名称', 'detail': '4', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
  { 'name': '获取本地名称', 'detail': '5', 'method': 'getLocalName', 'api': testGetLocalName, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
  { 'name': '设置扫描模式', 'detail': '6', 'method': 'setBluetoothScanMode', 'api': testSetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
  { 'name': '获取扫描模式', 'detail': '7', 'method': 'getBluetoothScanMode', 'api': testGetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
//  { 'name': '开始蓝牙发现', 'detail': '8', 'method': 'startBluetoothDiscovery','api': testStartBluetoothDiscovery, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
//  { 'name': '停止蓝牙发现', 'detail': '9', 'method': 'stopBluetoothDiscovery', 'api': testStopBluetoothDiscovery, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark },
//  { 'name': '关蓝牙', 'detail': '10', 'method': 'disableBluetooth', 'api': testDisableBluetooth, 'result': 'None', 'category': BenchmarkCategory.BrBenchmark }
]

export const BleBenchmarkTestCase: [] =[
  { 'name': '注册状态变化', 'detail': '1', 'method': 'on.stateChange', 'api': testOnStateChange, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
  { 'name': '开蓝牙', 'detail': '2', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
  { 'name': '获取状态', 'detail': '3', 'method': 'getState', 'api': testGetState, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
//  { 'name': '注册Ble查找状态变化', 'detail': '4', 'method': 'on.BleDeviceFind', 'api': testOnBLEDeviceFind, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
  { 'name': '设置本地名称', 'detail': '5', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
  { 'name': '获取本地名称', 'detail': '6', 'method': 'getLocalName', 'api': testGetLocalName, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
  { 'name': '设置扫描模式', 'detail': '7', 'method': 'setBluetoothScanMode', 'api': testSetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
  { 'name': '获取扫描模式', 'detail': '8', 'method': 'getBluetoothScanMode', 'api': testGetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
//  { 'name': '开始Ble扫描', 'detail': '5', 'method': 'startBleScan',  'api': testStartBLEScan, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
//  { 'name': '获取连接的Ble设备', 'detail': '6', 'method': 'getConnectedBleDevices', 'api': testGetConnectedBLEDevices,'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
//  { 'name': '停止Ble扫描', 'detail': '7', 'method': 'stopBleScan', 'api': testStopBLEScan,  'result': 'None', 'category': BenchmarkCategory.BleBenchmark },
//  { 'name': '关蓝牙', 'detail': '8', 'method': 'disableBluetooth', 'api': testDisableBluetooth, 'result': 'None', 'category': BenchmarkCategory.BleBenchmark }
]

export const GattClientBenchmarkTestCase: [] =[
//  { 'name': '注册状态变化', 'detail': '1', 'method': 'on.stateChange', 'api': testOnStateChange, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '开蓝牙', 'detail': '2', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '获取状态', 'detail': '3', 'method': 'getState', 'api': testGetState, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '设置本地名称', 'detail': '4', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '获取本地名称', 'detail': '5', 'method': 'getLocalName', 'api': testGetLocalName, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '设置扫描模式', 'detail': '6', 'method': 'setBluetoothScanMode', 'api': testSetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '获取扫描模式', 'detail': '7', 'method': 'getBluetoothScanMode', 'api': testGetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },

  { 'name': '创建Gatt客户端', 'detail': '4', 'method': 'createGattClient',  'api': testCreateGattClient,  'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  { 'name': 'Ble连接状态', 'detail': '5', 'method': 'on.BleConnectionStateChange', 'api': testOnBLEConnectionStateChange, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  { 'name': '连接', 'detail': '6', 'method': 'connect',  'api': testConnect, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  { 'name': '设置通知特性更改', 'detail': '7', 'method': 'setNotifyCharacteristicChanged','api': testSetNotifyCharacteristicChanged, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  { 'name': 'Ble特征值变化', 'detail': '8', 'method': 'on.BleCharacteristicChange', 'api': testOnBLECharacteristicChange, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  { 'name': '获取服务callback', 'detail': '9', 'method': 'getServices(Callback)', 'api': testGetServicesCallback, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  //{ 'name': '获取服务promise', 'detail': '11', 'method': 'getServices(promise)',  'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  { 'name': '读取特征值callback', 'detail': '10', 'method': 'readCharacteristicValue(callback)', 'api': testReadCharacteristicValueCallback, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  //{ 'name': '读取特征值promise', 'detail': '13', 'method': 'readCharacteristicValue(promise)',  'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '读取描述符值callback', 'detail': '11', 'method': 'readDescriptorValue(callback)', 'api': testReadDescriptorValueCallback, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  //{ 'name': '读取描述符值promise', 'detail': '15', 'method': 'readDescriptorValue(promise)',  'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  { 'name': '写入特征值', 'detail': '11', 'method': 'writeCharacteristicValue', 'api': testWriteCharacteristicValue,  'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  //{ 'name': '写入描述符值', 'detail': '12', 'method': 'writeDescriptorValue', 'api': testWriteDescriptorValue, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },

  //{ 'name': '获取设备名称callback', 'detail': '12', 'method': 'getDeviceName()callback', 'api': testGetDeviceNameCallback, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  //{ 'name': '获取设备名称promise', 'detail': '21', 'method': 'getDeviceName()Promise', 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '获取Rssi值callback', 'detail': '13', 'method': 'getRssiValue()Callback', 'api': testGetRssiValueCallback, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  //{ 'name': '获取Rssi值promise', 'detail': '23', 'method': 'getRssiValue()Promise',  'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '取消连接', 'detail': '14', 'method': 'disconnect', 'api': testDisconnect, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '关闭', 'detail': '15', 'method': 'close',  'api': testGattClientClose,'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
  //  { 'name': '停止Ble扫描', 'detail': '18', 'method': 'stopBleScan',  'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark },
//  { 'name': '关蓝牙', 'detail': '16', 'method': 'disableBluetooth', 'api': testDisableBluetooth, 'result': 'None', 'category': BenchmarkCategory.GattClientBenchmark }
]

export const GattServerBenchmarkTestCase: []  = [
//  { 'name': '注册状态变化', 'detail': '1', 'method': 'on.stateChange', 'api': testOnStateChange, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '开蓝牙', 'detail': '2', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '获取状态', 'detail': '3', 'method': 'getState', 'api': testGetState, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '设置本地名称', 'detail': '4', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '获取本地名称', 'detail': '5', 'method': 'getLocalName', 'api': testGetLocalName, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '设置扫描模式', 'detail': '6', 'method': 'setBluetoothScanMode', 'api': testSetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '获取扫描模式', 'detail': '7', 'method': 'getBluetoothScanMode', 'api': testGetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },

  { 'name': '创建Gatt服务器', 'detail': '4', 'method': 'createGattServer', 'api': testCreateGattServer, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
  { 'name': '添加服务', 'detail': '5', 'method': 'addService', 'api': testAddService,'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
  { 'name': '连接状态', 'detail': '6', 'method': 'on.connectStateChange', 'api': testOnConnectStateChange,  'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
  { 'name': '开始广播', 'detail': '7', 'method': 'startAdvertising', 'api': testStartAdvertising, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
  { 'name': '读取特征值', 'detail': '8', 'method': 'on.characteristicRead', 'api': testOnCharacteristicRead, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
  { 'name': '写入特征值', 'detail': '9', 'method': 'on.characteristicWrite',  'api': testOnCharacteristicWrite, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
  //  { 'name': '读取描述符', 'detail': '9', 'method': 'on.descriptorRead', 'api': testOnDescriptorRead,  'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
  //  { 'name': '写入描述符', 'detail': '11', 'method': 'on.descriptorWrite',  'api': testOnDescriptorWrite, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },

  //  { 'name': '停止广播', 'detail': '10','method': 'stopAdvertising', 'api': testStopAdvertising,  'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '删除服务', 'detail': '11', 'method': 'removeService', 'api': testRemoveService, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '关闭', 'detail': '12', 'method': 'close',  'api': testGattServerClose,'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
  //  { 'name': '停止Ble扫描', 'detail': '15', 'method': 'stopBleScan',  'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark },
//  { 'name': '关蓝牙', 'detail': '13', 'method': 'disableBluetooth', 'api': testDisableBluetooth, 'result': 'None', 'category': BenchmarkCategory.GattServerBenchmark }
]

export const SppClientBenchmarkTestCase:[]=[
  { 'name': '注册状态变化', 'detail': '1', 'method': 'on.stateChange', 'api': testOnStateChange, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
  { 'name': '开蓝牙', 'detail': '2', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
  { 'name': '获取状态', 'detail': '3', 'method': 'getState', 'api': testGetState, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
  { 'name': '设置本地名称', 'detail': '5', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
  { 'name': '获取本地名称', 'detail': '6', 'method': 'getLocalName', 'api': testGetLocalName, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },

  { 'name': '设置SPP扫描模式', 'detail': '4', 'method': 'setBluetoothScanMode', 'api': testSetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
  { 'name': '获取扫描模式', 'detail': '5', 'method': 'getBluetoothScanMode', 'api': testGetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
  { 'name': 'Spp客户端连接(安全模式)', 'detail': '6', 'method': 'sppConnect', 'api': testSppClientConnectSafe, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },

//  { 'name': '获取当前连接状态', 'detail': '7', 'method': 'getBtConnectionState', 'api': testGetBtConnectionState, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
//  { 'name': 'Spp客户端读取打开', 'detail': '9', 'method': 'on.sppRead',  'api': testSppClientReadOn,'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },

  //  { 'name': 'Spp客户端连接(非安全模式)', 'detail': '8', 'method': 'sppConnect','result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
  //  { 'name': '配对设备', 'detail': '8', 'method': 'pairDevice', 'api': testPairDevice, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
//  { 'name': 'Spp客户端写入', 'detail': '8', 'method': 'sppWrite', 'api': testSppClientWrite, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
//  { 'name': 'Spp客户端读取关闭', 'detail': '10', 'method': 'off.sppRead', 'api': testSppClientReadOff, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
//  { 'name': 'Spp客户端关闭', 'detail': '11', 'method': 'sppCloseClientSocket', 'api': testSppClientClose,'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
//  { 'name': '关蓝牙', 'detail': '12', 'method': 'disableBluetooth', 'api': testDisableBluetooth, 'result': 'None', 'category': BenchmarkCategory.SppClientBenchmark },
]

export const SppServerBenchmarkTestCase:[]=[
  { 'name': '注册状态变化', 'detail': '1', 'method': 'on.stateChange', 'api': testOnStateChange, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
  { 'name': '开蓝牙', 'detail': '2', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
  { 'name': '获取状态', 'detail': '3', 'method': 'getState', 'api': testGetState, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
  { 'name': '设置本地名称', 'detail': '5', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
  { 'name': '获取本地名称', 'detail': '6', 'method': 'getLocalName', 'api': testGetLocalName, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },

  { 'name': '设置SPP扫描模式', 'detail': '4', 'method': 'setBluetoothScanMode',  'api': testSetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
  { 'name': '获取扫描模式', 'detail': '5', 'method': 'getBluetoothScanMode',  'api': testGetBluetoothScanMode, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': '注册pin码变化', 'detail': '6', 'method': 'on.pinRequired', 'api': testOnPinRequired, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp服务器创建(安全模式)', 'detail': '7', 'method': 'sppListen', 'api': testSppServerCreateSafe, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp服务器接受', 'detail': '10', 'method': 'sppAccept', 'api': testSppServerAccept, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },

//  { 'name': '获取当前连接状态', 'detail': '8', 'method': 'getBtConnectionState', 'api': testGetBtConnectionState,  'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp服务器读取打开', 'detail': '11', 'method': 'on.sppRead', 'api': testSppServerReadOn,  'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },


  //{ 'name': 'Spp服务器创建(非安全模式)', 'detail': '8', 'method': 'sppListen',  'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp服务器写入', 'detail': '9', 'method': 'sppWrite', 'api': testSppServerWrite, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp服务器接受', 'detail': '10', 'method': 'sppAccept', 'api': testSppServerAccept, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp服务器读取打开', 'detail': '11', 'method': 'on.sppRead', 'api': testSppServerReadOn,  'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp服务器读取关闭', 'detail': '12', 'method': 'off.sppRead', 'api': testSppServerReadOff,  'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp服务器关闭', 'detail': '13', 'method': 'sppCloseServerSocket', 'api': testSppCloseServerSocket, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': 'Spp客户端关闭', 'detail': '14', 'method': 'sppCloseClientSocket', 'api': testSppCloseClientSocket, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
//  { 'name': '关蓝牙', 'detail': '15', 'method': 'disableBluetooth', 'api': testDisableBluetooth, 'result': 'None', 'category': BenchmarkCategory.SppServerBenchmark },
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