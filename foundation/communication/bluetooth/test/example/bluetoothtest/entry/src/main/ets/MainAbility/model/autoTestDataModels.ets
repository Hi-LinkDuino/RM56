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
import { TestAuto, AutoCategory } from './testData'
import { testEnableBluetooth, testDisableBluetooth, testGetState, testGetBtConnectionState, testSetLocalName,
  testGetLocalName, testSetBluetoothScanMode, testGetBluetoothScanMode, subscribeBluetoothDeviceFind, testStopBluetoothDiscovery,
  testPairDevice, testGetRemoteDeviceName, testGetRemoteDeviceClass, testGetPairedDevices, testUnPairedDevice,
  testGetProfileConnState1,testGetProfileConnState2,testOnStateChange,testOnPinRequired,testOnBondStateChange} from './brInterface'
import { testCreateGattServerDevice, testCreateGattClientDevice, testGetConnectedBLEDevices, testStartBLEScan,testStopBLEScan,testOnBLEDeviceFind
} from './bleInterface'

import { testA2dpSourceProfile, testHandsFreeAudioGatewayProfile,testHidHostProfile,testPanNetworkProfile,} from './profileInterface'
import {testCreateGattClient, testConnect, testDisconnect, testOnBLEConnectionStateChange, testGattClientClose, testGetServicesCallback,
  testGetServicesPromise, testReadCharacteristicValueCallback, testReadCharacteristicValuePromise, testReadDescriptorValueCallback,
  testReadDescriptorValuePromise, testWriteCharacteristicValue, testWriteDescriptorValue, testOnBLECharacteristicChange, testSetBLEMtuSize,
  testSetNotifyCharacteristicChanged, testGetDeviceNameCallback,testGetDeviceNamePromise, testGetRssiValueCallback,testGetRssiValuePromise} from './gattClientInterface'
import {testCreateGattServer,testStartAdvertising,testStopAdvertising,testAddService,testRemoveService,testGattServerClose,testOnConnectStateChange,
  testOnCharacteristicRead,testOnCharacteristicWrite,testOnDescriptorRead,testOnDescriptorWrite} from './gattServerInterface'
import {testSppClientConnectSafe,testSppClientConnectUnSafe,testSppClientWrite,
  testSppClientReadOn,testSppClientReadOff,testSppClientClose} from './sppClientInterface'
import {testSppServerCreateSafe,testSppServerCreateUnSafe,testSppServerAccept,testSppServerWrite,testSppServerReadOn,
  testSppServerReadOff,testSppCloseServerSocket,testSppCloseClientSocket} from './sppServerInterface'
import { testProfileConnect,testProfileDisConnect,testProfileGetPlayingState,testProfileGetConnectionDevices,testProfileGetDeviceState} from './a2dpSinkProfileInterface'
import BluetoothModel from './BluetoothModel'
export const BrAutoTestCase: []  = [
  { 'name': '注册状态变化', 'detail': '注册状态变化', 'method': 'on.stateChange', 'api':testOnStateChange, 'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '注册pin码变化', 'detail': '注册pin码变化', 'method': 'on.pinRequired','api':testOnPinRequired,'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '注册绑定状态变化', 'detail': '注册绑定状态变化', 'method': 'on.bondStateChange', 'api':testOnBondStateChange, 'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '开蓝牙', 'detail': '使能蓝牙', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '获取状态', 'detail': '获取蓝牙状态', 'method': 'getState','api': testGetState, 'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '获取当前连接状态', 'detail': '获取当前蓝牙连接状态', 'method': 'getBtConnectionState', 'api': testGetBtConnectionState, 'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '设置本地名称', 'detail': '设置蓝牙本地名称', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '获取本地名称', 'detail': '获取蓝牙本地名称', 'method': 'getLocalName',  'api': testGetLocalName,'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '设置扫描模式', 'detail': '设置蓝牙扫描模式', 'method': 'setBluetoothScanMode','api': testSetBluetoothScanMode,  'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '获取扫描模式', 'detail': '获取蓝牙扫描模式', 'method': 'getBluetoothScanMode','api': testGetBluetoothScanMode, 'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '订阅蓝牙发现', 'detail': '订阅发现蓝牙设备', 'method': 'subscribeBluetoothDeviceFind', 'api': subscribeBluetoothDeviceFind, 'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '开始蓝牙发现', 'detail': '开始发现', 'method': 'startBluetoothDiscovery', 'api': BluetoothModel.startBluetoothDiscovery, 'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '停止蓝牙发现', 'detail': '停止发现', 'method': 'stopBluetoothDiscovery', 'api': testStopBluetoothDiscovery, 'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '配对设备', 'detail': '进行蓝牙设备配对', 'method': 'pairDevice','api': testPairDevice, 'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '获取远程设备名称', 'detail': '获取配对蓝牙设备的名称', 'method': 'getDeviceName','api':testGetRemoteDeviceName, 'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '获取远程设备类型', 'detail': '获取配对蓝牙设备的类型', 'method': 'getDeviceType', 'api': testGetRemoteDeviceClass,'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '获取配对设备', 'detail': '获取配对蓝牙设备的信息', 'method': 'getPairedDeviceIds', 'api':testGetPairedDevices,'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '取消配对设备', 'detail': '取消蓝牙设备配对', 'method': 'unpairDevice', 'api':testUnPairedDevice,'result': 'None', 'category': AutoCategory.BrAuto },
//  { 'name': '获取配置文件连接状态1', 'detail': '获取配置文件的连接状态', 'method': 'getProfileConnState', 'api':testGetProfileConnState1,'result': 'None', 'category': AutoCategory.BrAuto },
//  { 'name': '获取配置文件连接状态2', 'detail': '获取设备的配置文件状态', 'method': 'getDeviceState','api':testGetProfileConnState2, 'result': 'None', 'category': AutoCategory.BrAuto },
  { 'name': '关蓝牙', 'detail': '去使能蓝牙', 'method': 'disableBluetooth', 'api':testDisableBluetooth, 'result': 'None', 'category': AutoCategory.BrAuto },
]

export const BleAutoTestCase: []  = [
  { 'name': '开蓝牙', 'detail': '使能蓝牙', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': AutoCategory.BleAuto },
  { 'name': '获取状态', 'detail': '获取蓝牙状态', 'method': 'getState','api': testGetState, 'result': 'None', 'category': AutoCategory.BleAuto },
  { 'name': '设置本地名称', 'detail': '设置蓝牙本地名称', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': AutoCategory.BleAuto },
  { 'name': '获取本地名称', 'detail': '获取蓝牙本地名称', 'method': 'getLocalName',  'api': testGetLocalName,'result': 'None', 'category': AutoCategory.BleAuto },
  { 'name': '设置扫描模式', 'detail': '设置蓝牙扫描模式', 'method': 'setBluetoothScanMode','api': testSetBluetoothScanMode,  'result': 'None', 'category': AutoCategory.BleAuto },
  { 'name': '获取扫描模式', 'detail': '获取蓝牙扫描模式', 'method': 'getBluetoothScanMode','api': testGetBluetoothScanMode, 'result': 'None', 'category': AutoCategory.BleAuto },
//  { 'name': '创建Gatt服务器', 'detail': '创建Gatt服务器', 'method': 'createGattServer', 'api':testCreateGattServerDevice, 'result': 'None', 'category': AutoCategory.BleAuto },
//  { 'name': '创建Gatt客户端', 'detail': '创建Gatt客户端', 'method': 'createGattClient', 'api':testCreateGattClientDevice, 'result': 'None', 'category': AutoCategory.BleAuto },
  { 'name': '获取连接的Ble设备', 'detail': '获取连接的Ble设备信息', 'method': 'getConnectedBleDevices','api':testGetConnectedBLEDevices,'result': 'None', 'category': AutoCategory.BleAuto },
  { 'name': '注册Ble查找状态变化', 'detail': '注册Ble设备查找状态变化', 'method': 'on.BleDeviceFind', 'api':testOnBLEDeviceFind, 'result': 'None', 'category': AutoCategory.BleAuto },
  { 'name': '开始Ble扫描', 'detail': '开始扫描Ble设备', 'method': 'startBleScan', 'api':testStartBLEScan, 'result': 'None', 'category': AutoCategory.BleAuto },
  { 'name': '停止Ble扫描', 'detail': '停止扫描', 'method': 'stopBleScan', 'api':testStopBLEScan, 'result': 'None', 'category': AutoCategory.BleAuto },
  { 'name': '关蓝牙', 'detail': '去使能蓝牙', 'method': 'disableBluetooth', 'api':testDisableBluetooth, 'result': 'None', 'category': AutoCategory.BleAuto },
]

export const GattClientAutoTestCase: [] =[
  { 'name': '开蓝牙', 'detail': '使能蓝牙', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '获取状态', 'detail': '获取蓝牙状态', 'method': 'getState','api': testGetState, 'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '设置本地名称', 'detail': '设置蓝牙本地名称', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '获取本地名称', 'detail': '获取蓝牙本地名称', 'method': 'getLocalName',  'api': testGetLocalName,'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '设置扫描模式', 'detail': '设置蓝牙扫描模式', 'method': 'setBluetoothScanMode','api': testSetBluetoothScanMode,  'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '获取扫描模式', 'detail': '获取蓝牙扫描模式', 'method': 'getBluetoothScanMode','api': testGetBluetoothScanMode, 'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '创造Gatt客户端', 'detail': '创造Gatt客户端', 'method': 'createGattClient','api':testCreateGattClient, 'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': 'Ble连接状态', 'detail': '查看Ble连接状态', 'method': 'on.BleConnectionStateChange', 'api':testOnBLEConnectionStateChange,  'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '连接', 'detail': 'Gatt连接', 'method': 'connect', 'api':testConnect,  'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '获取服务callback', 'detail': '获取服务callback', 'method': 'getServices(Callback)', 'api':testGetServicesCallback,  'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '获取服务promise', 'detail': '获取服务promise', 'method': 'getServices(promise)', 'api':testGetServicesPromise,  'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '读取特征值callback', 'detail': '读取特征值callback', 'method': 'readCharacteristicValue(callback)','api':testReadCharacteristicValueCallback,  'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '读取特征值promise', 'detail': '读取特征值promise', 'method': 'readCharacteristicValue(promise)', 'api':testReadCharacteristicValuePromise,  'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '读取描述符值callback', 'detail': '读取描述符值callback', 'method': 'readDescriptorValue(callback)','api':testReadDescriptorValueCallback,  'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '读取描述符值promise', 'detail': '读取描述符值promise', 'method': 'readDescriptorValue(promise)','api':testReadDescriptorValuePromise,   'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '写入特征值', 'detail': '写入特征值', 'method': 'writeCharacteristicValue', 'api':testWriteCharacteristicValue,  'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '写入描述符值', 'detail': '写入描述符值', 'method': 'writeDescriptorValue', 'api':testWriteDescriptorValue, 'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': 'Ble特征值变化', 'detail': '查看Ble特征值变化', 'method': 'on.BleCharacteristicChange', 'api':testOnBLECharacteristicChange, 'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '设定BleMtu尺寸', 'detail': '设定BleMtu尺寸', 'method': 'setBleMtuSize', 'api':testSetBLEMtuSize, 'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '设置通知特性更改', 'detail': '设置通知特性更改', 'method': 'setNotifyCharacteristicChanged','api':testSetNotifyCharacteristicChanged, 'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '获取设备名称callback', 'detail': '获取设备名称callback', 'method': 'getDeviceName()callback', 'api':testGetDeviceNameCallback,  'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '获取设备名称promise', 'detail': '获取设备名称promise', 'method': 'getDeviceName()Promise', 'api':testGetDeviceNamePromise, 'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '获取Rssi值callback', 'detail': '获取Rssi值callback', 'method': 'getRssiValue()Callback', 'api':testGetRssiValueCallback,  'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '获取Rssi值promise', 'detail': '获取Rssi值promise', 'method': 'getRssiValue()Promise', 'api':testGetRssiValuePromise,  'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '取消连接', 'detail': '取消Gatt连接', 'method': 'disconnect', 'api':testDisconnect,  'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '关闭', 'detail': '关闭Gatt客户端', 'method': 'close', 'api':testGattClientClose,  'result': 'None', 'category': AutoCategory.GattClientAuto },
  { 'name': '关蓝牙', 'detail': '去使能蓝牙', 'method': 'disableBluetooth', 'api':testDisableBluetooth, 'result': 'None', 'category': AutoCategory.GattClientAuto },
]

export const GattServerAutoTestCase: []  = [
  { 'name': '开蓝牙', 'detail': '使能蓝牙', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '获取状态', 'detail': '获取蓝牙状态', 'method': 'getState','api': testGetState, 'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '设置本地名称', 'detail': '设置蓝牙本地名称', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '获取本地名称', 'detail': '获取蓝牙本地名称', 'method': 'getLocalName',  'api': testGetLocalName,'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '设置扫描模式', 'detail': '设置蓝牙扫描模式', 'method': 'setBluetoothScanMode','api': testSetBluetoothScanMode,  'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '获取扫描模式', 'detail': '获取蓝牙扫描模式', 'method': 'getBluetoothScanMode','api': testGetBluetoothScanMode, 'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '创建Gatt服务器', 'detail': '创建Gatt服务器', 'method': 'createGattServer',  'api':testCreateGattServer,  'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '添加服务', 'detail': '添加Gatt服务', 'method': 'addService', 'api':testAddService,  'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '开始广播', 'detail': '开始广播播报', 'method': 'startAdvertising',  'api':testStartAdvertising,  'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '连接状态', 'detail': '查看Gatt连接状态', 'method': 'on.connectStateChange',  'api':testOnConnectStateChange,  'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '读取特征值', 'detail': '读取特征值', 'method': 'on.characteristicRead', 'api':testOnCharacteristicRead, 'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '写入特征值', 'detail': '写入特征值', 'method': 'on.characteristicWrite',  'api':testOnCharacteristicWrite,  'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '读取描述符', 'detail': '读取描述符', 'method': 'on.descriptorRead',  'api':testOnDescriptorRead,  'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '写入描述符', 'detail': '写入描述符', 'method': 'on.descriptorWrite', 'api':testOnDescriptorWrite,  'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '停止广播', 'detail': '停止广播播报', 'method': 'stopAdvertising',  'api':testStopAdvertising,  'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '删除服务', 'detail': '删除Gatt服务', 'method': 'removeService',  'api':testRemoveService,  'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '关闭', 'detail': '关闭Gatt服务器', 'method': 'close', 'api':testGattServerClose,  'result': 'None', 'category': AutoCategory.GattServerAuto },
  { 'name': '关蓝牙', 'detail': '去使能蓝牙', 'method': 'disableBluetooth', 'api':testDisableBluetooth, 'result': 'None', 'category': AutoCategory.GattServerAuto },
]

export const SppClientAutoTestCase: []  = [
  { 'name': '开蓝牙', 'detail': '使能蓝牙', 'method': 'enableBluetooth',  'api':testEnableBluetooth, 'result': 'None', 'category': AutoCategory.SppClientAuto },
  { 'name': '设置SPP扫描模式', 'detail': '设置SPP扫描模式', 'method': 'setBluetoothScanMode',  'api':testSetBluetoothScanMode, 'result': 'None', 'category': AutoCategory.SppClientAuto },
  { 'name': '设置本地名称', 'detail': '设置蓝牙本地名称', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': AutoCategory.SppClientAuto },
  { 'name': '获取本地名称', 'detail': '获取蓝牙本地名称', 'method': 'getLocalName',  'api': testGetLocalName,'result': 'None', 'category': AutoCategory.SppClientAuto },
  { 'name': '获取状态', 'detail': '获取蓝牙状态', 'method': 'getState',  'api':testGetState, 'result': 'None', 'category': AutoCategory.SppClientAuto },
  { 'name': '获取扫描模式', 'detail': '获取蓝牙扫描模式', 'method': 'getBluetoothScanMode', 'api':testGetBluetoothScanMode,'result': 'None', 'category': AutoCategory.SppClientAuto },
  { 'name': 'Spp客户端连接(安全模式)', 'detail': 'Spp客户端(安全模式)连接', 'method': 'sppConnect', 'api':testSppClientConnectSafe, 'result': 'None', 'category': AutoCategory.SppClientAuto },
  { 'name': 'Spp客户端连接(非安全模式)', 'detail': 'Spp客户端(非安全模式)连接', 'method': 'sppConnect', 'api':testSppClientConnectUnSafe,'result': 'None', 'category': AutoCategory.SppClientAuto },
  { 'name': '配对设备', 'detail': '进行蓝牙设备配对', 'method': 'pairDevice', 'api':testPairDevice, 'result': 'None', 'category': AutoCategory.SppClientAuto },
  { 'name': 'Spp客户端写入', 'detail': 'Spp客户端写入', 'method': 'sppWrite',  'api':testSppClientWrite, 'result': 'None', 'category': AutoCategory.SppClientAuto },
  { 'name': 'Spp客户端读取打开', 'detail': 'Spp客户端读取打开', 'method': 'on.sppRead', 'api':testSppClientReadOn, 'result': 'None', 'category': AutoCategory.SppClientAuto },
  { 'name': 'Spp客户端读取关闭', 'detail': 'Spp客户端读取关闭', 'method': 'off.sppRead',  'api':testSppClientReadOff,'result': 'None', 'category': AutoCategory.SppClientAuto },
  { 'name': 'Spp客户端关闭', 'detail': 'Spp客户端关闭', 'method': 'sppCloseClientSocket', 'api':testSppClientClose, 'result': 'None', 'category': AutoCategory.SppClientAuto },
  { 'name': '关蓝牙', 'detail': '去使能蓝牙', 'method': 'disableBluetooth',  'api':testDisableBluetooth, 'result': 'None', 'category': AutoCategory.SppClientAuto },
]

export const SppServerAutoTestCase: []  = [
  { 'name': '开蓝牙', 'detail': '使能蓝牙', 'method': 'enableBluetooth', 'api':testEnableBluetooth, 'result': 'None', 'category': AutoCategory.SppServerAuto },
  { 'name': '设置SPP扫描模式', 'detail': '设置SPP扫描模式', 'method': 'setBluetoothScanMode', 'api':testSetBluetoothScanMode, 'result': 'None', 'category': AutoCategory.SppServerAuto },
  { 'name': '注册pin码变化', 'detail': '注册pin码变化', 'method': 'on.pinRequired', 'api':testOnPinRequired, 'result': 'None', 'category': AutoCategory.SppServerAuto },
  { 'name': '设置本地名称', 'detail': '设置蓝牙本地名称', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': AutoCategory.SppServerAuto },
  { 'name': '获取本地名称', 'detail': '获取蓝牙本地名称', 'method': 'getLocalName',  'api': testGetLocalName,'result': 'None', 'category': AutoCategory.SppServerAuto },
  { 'name': '获取状态', 'detail': '获取蓝牙状态', 'method': 'getState', 'api':testGetState, 'result': 'None', 'category': AutoCategory.SppServerAuto },
  { 'name': '获取扫描模式', 'detail': '获取蓝牙扫描模式', 'method': 'getBluetoothScanMode', 'api':testGetBluetoothScanMode, 'result': 'None', 'category': AutoCategory.SppServerAuto },
  { 'name': 'Spp服务器创建(安全模式)', 'detail': 'Spp服务器(安全模式)创建', 'method': 'sppListen', 'api':testSppServerCreateSafe, 'result': 'None', 'category': AutoCategory.SppServerAuto },
  { 'name': 'Spp服务器创建(非安全模式)', 'detail': 'Spp服务器(非安全模式)创建', 'method': 'sppListen', 'api':testSppServerCreateUnSafe, 'result': 'None', 'category': AutoCategory.SppServerAuto },
  { 'name': 'Spp服务器写入', 'detail': 'Spp服务器写入', 'method': 'sppWrite','api':testSppServerWrite, 'result': 'None', 'category': AutoCategory.SppServerAuto },
  { 'name': 'Spp服务器接受', 'detail': 'Spp服务器接受', 'method': 'sppAccept', 'api':testSppServerAccept, 'result': 'None', 'category': AutoCategory.SppServerAuto },
  { 'name': 'Spp服务器读取打开', 'detail': 'Spp服务器读取打开', 'method': 'on.sppRead', 'api':testSppServerReadOn, 'result': 'None', 'category': AutoCategory.SppServerAuto },
  { 'name': 'Spp服务器读取关闭', 'detail': 'Spp服务器读取关闭', 'method': 'off.sppRead', 'api':testSppServerReadOff, 'result': 'None', 'category': AutoCategory.SppServerAuto },
  { 'name': 'Spp服务器关闭', 'detail': 'Spp服务器关闭', 'method': 'sppCloseServerSocket', 'api':testSppCloseServerSocket, 'result': 'None', 'category': AutoCategory.SppServerAuto },
  { 'name': 'Spp客户端关闭', 'detail': 'Spp客户端关闭', 'method': 'sppCloseClientSocket', 'api':testSppCloseClientSocket,'result': 'None', 'category': AutoCategory.SppServerAuto },
  { 'name': '关蓝牙', 'detail': '去使能蓝牙', 'method': 'disableBluetooth', 'api':testDisableBluetooth,'result': 'None', 'category': AutoCategory.SppServerAuto },
]

export const A2dpSourceProfileAutoTestCase: []=[
  { 'name': '开蓝牙', 'detail': '使能蓝牙', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': AutoCategory.A2dpSourceAuto },
  { 'name': '获取状态', 'detail': '获取蓝牙状态', 'method': 'getState','api': testGetState, 'result': 'None', 'category': AutoCategory.A2dpSourceAuto },
  { 'name': '设置本地名称', 'detail': '设置蓝牙本地名称', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': AutoCategory.A2dpSourceAuto },
  { 'name': '获取本地名称', 'detail': '获取蓝牙本地名称', 'method': 'getLocalName',  'api': testGetLocalName,'result': 'None', 'category': AutoCategory.A2dpSourceAuto },
  { 'name': '设置扫描模式', 'detail': '设置蓝牙扫描模式', 'method': 'setBluetoothScanMode','api': testSetBluetoothScanMode,  'result': 'None', 'category': AutoCategory.A2dpSourceAuto },
  { 'name': '获取扫描模式', 'detail': '获取蓝牙扫描模式', 'method': 'getBluetoothScanMode','api': testGetBluetoothScanMode, 'result': 'None', 'category': AutoCategory.A2dpSourceAuto },
  { 'name': '连接', 'detail': 'Profile连接', 'method': 'connect','api':testProfileConnect, 'result': 'None', 'category': AutoCategory.A2dpSourceAuto },
  { 'name': '获取播放状态', 'detail': '获取Profile播放状态', 'method': 'getPlayingState',  'api':testProfileGetPlayingState,'result': 'None', 'category': AutoCategory.A2dpSourceAuto},
  { 'name': '获取连接设备', 'detail': '获取Profile连接设备', 'method': 'getConnectionDevices', 'api':testProfileGetConnectionDevices,'result': 'None', 'category': AutoCategory.A2dpSourceAuto },
  { 'name': '获取设备状态', 'detail': '获取Profile设备状态', 'method': 'getDeviceState', 'api':testProfileGetDeviceState,'result': 'None', 'category': AutoCategory.A2dpSourceAuto},
  { 'name': '取消连接', 'detail': '取消Profile连接', 'method': 'disconnect', 'api':testProfileDisConnect,'result': 'None', 'category': AutoCategory.A2dpSourceAuto },
]

export const HandsFreeAudioGatewayProfileAutoTestCase: []  = [
  { 'name': '开蓝牙', 'detail': '使能蓝牙', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': AutoCategory.HandsFreeAudioGatewayAuto },
  { 'name': '获取状态', 'detail': '获取蓝牙状态', 'method': 'getState','api': testGetState, 'result': 'None', 'category': AutoCategory.HandsFreeAudioGatewayAuto },
  { 'name': '设置本地名称', 'detail': '设置蓝牙本地名称', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': AutoCategory.HandsFreeAudioGatewayAuto },
  { 'name': '获取本地名称', 'detail': '获取蓝牙本地名称', 'method': 'getLocalName',  'api': testGetLocalName,'result': 'None', 'category': AutoCategory.HandsFreeAudioGatewayAuto },
  { 'name': '设置扫描模式', 'detail': '设置蓝牙扫描模式', 'method': 'setBluetoothScanMode','api': testSetBluetoothScanMode,  'result': 'None', 'category': AutoCategory.HandsFreeAudioGatewayAuto },
  { 'name': '获取扫描模式', 'detail': '获取蓝牙扫描模式', 'method': 'getBluetoothScanMode','api': testGetBluetoothScanMode, 'result': 'None', 'category': AutoCategory.HandsFreeAudioGatewayAuto },
  { 'name': '连接', 'detail': 'Profile连接', 'method': 'connect', 'api':testProfileConnect, 'result': 'None', 'category': AutoCategory.HandsFreeAudioGatewayAuto },
  { 'name': '获取连接设备', 'detail': '获取Profile连接设备', 'method': 'getConnectionDevices', 'api':testProfileGetConnectionDevices,'result': 'None', 'category': AutoCategory.HandsFreeAudioGatewayAuto, },
  { 'name': '获取设备状态', 'detail': '获取Profile设备状态', 'method': 'getDeviceState', 'api':testProfileGetDeviceState, 'result': 'None', 'category': AutoCategory.HandsFreeAudioGatewayAuto, },
  { 'name': '取消连接', 'detail': '取消Profile连接', 'method': 'disconnect', 'api':testProfileDisConnect, 'result': 'None', 'category': AutoCategory.HandsFreeAudioGatewayAuto },
]

export const HitHostProfileAutoTestCase:[] =[
  { 'name': '开蓝牙', 'detail': '使能蓝牙', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': AutoCategory.HitHostAuto },
  { 'name': '获取状态', 'detail': '获取蓝牙状态', 'method': 'getState','api': testGetState, 'result': 'None', 'category': AutoCategory.HitHostAuto },
  { 'name': '设置本地名称', 'detail': '设置蓝牙本地名称', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': AutoCategory.HitHostAuto },
  { 'name': '获取本地名称', 'detail': '获取蓝牙本地名称', 'method': 'getLocalName',  'api': testGetLocalName,'result': 'None', 'category': AutoCategory.HitHostAuto },
  { 'name': '设置扫描模式', 'detail': '设置蓝牙扫描模式', 'method': 'setBluetoothScanMode','api': testSetBluetoothScanMode,  'result': 'None', 'category': AutoCategory.HitHostAuto },
  { 'name': '获取扫描模式', 'detail': '获取蓝牙扫描模式', 'method': 'getBluetoothScanMode','api': testGetBluetoothScanMode, 'result': 'None', 'category': AutoCategory.HitHostAuto },
  { 'name': '连接', 'detail': 'Profile连接', 'method': 'connect','api':testProfileConnect, 'result': 'None', 'category': AutoCategory.HitHostAuto },
  { 'name': '获取连接设备', 'detail': '获取Profile连接设备', 'method': 'getConnectionDevices', 'api':testProfileGetConnectionDevices, 'result': 'None', 'category': AutoCategory.HitHostAuto },
  { 'name': '获取设备状态', 'detail': '获取Profile设备状态', 'method': 'getDeviceState','api':testProfileGetDeviceState, 'result': 'None', 'category': AutoCategory.HitHostAuto },
  { 'name': '取消连接', 'detail': '取消Profile连接', 'method': 'disconnect','api':testProfileDisConnect,'result': 'None', 'category': AutoCategory.HitHostAuto },
]

export const PanNetworkProfileAutoTestCase:[] =[
  { 'name': '开蓝牙', 'detail': '使能蓝牙', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': AutoCategory.PanNetworkAuto },
  { 'name': '获取状态', 'detail': '获取蓝牙状态', 'method': 'getState','api': testGetState, 'result': 'None', 'category': AutoCategory.PanNetworkAuto },
  { 'name': '设置本地名称', 'detail': '设置蓝牙本地名称', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': AutoCategory.PanNetworkAuto },
  { 'name': '获取本地名称', 'detail': '获取蓝牙本地名称', 'method': 'getLocalName',  'api': testGetLocalName,'result': 'None', 'category': AutoCategory.PanNetworkAuto },
  { 'name': '设置扫描模式', 'detail': '设置蓝牙扫描模式', 'method': 'setBluetoothScanMode','api': testSetBluetoothScanMode,  'result': 'None', 'category': AutoCategory.PanNetworkAuto },
  { 'name': '获取扫描模式', 'detail': '获取蓝牙扫描模式', 'method': 'getBluetoothScanMode','api': testGetBluetoothScanMode, 'result': 'None', 'category': AutoCategory.PanNetworkAuto },
  { 'name': '获取连接设备', 'detail': '获取Profile连接设备', 'method': 'getConnectionDevices', 'api':testProfileGetConnectionDevices, 'result': 'None', 'category': AutoCategory.PanNetworkAuto },
  { 'name': '获取设备状态', 'detail': '获取Profile设备状态', 'method': 'getDeviceState', 'api':testProfileGetDeviceState, 'result': 'None', 'category': AutoCategory.PanNetworkAuto },
  { 'name': '取消连接', 'detail': '取消Profile连接', 'method': 'disconnect','api':testProfileDisConnect,'result': 'None', 'category': AutoCategory.PanNetworkAuto },
]

export function initBrAutoData(): Array<TestAuto> {
  let TestAutoArray: Array<TestAuto> = []
  BrAutoTestCase.forEach(item => {
    TestAutoArray.push(new TestAuto(item.name, item.detail,  item.method, item.api, item.result, item.Category));
  })
  return TestAutoArray;
}

export function initBleAutoData(): Array<TestAuto> {
  let TestAutoArray: Array<TestAuto> = []
  BleAutoTestCase.forEach(item => {
    TestAutoArray.push(new TestAuto(item.name, item.detail, item.method, item.api, item.result, item.Category));
  })
  return TestAutoArray;
}

export function initGattClientAutoData(): Array<TestAuto> {
  let TestAutoArray: Array<TestAuto> = []
  GattClientAutoTestCase.forEach(item => {
    TestAutoArray.push(new TestAuto(item.name, item.detail,  item.method, item.api, item.result, item.Category));
  })
  return TestAutoArray;
}

export function initGattServerAutoData(): Array<TestAuto> {
  let TestAutoArray: Array<TestAuto> = []
  GattServerAutoTestCase.forEach(item => {
    TestAutoArray.push(new TestAuto(item.name, item.detail, item.method, item.api, item.result, item.Category));
  })
  return TestAutoArray;
}

export function initSppClientAutoData(): Array<TestAuto> {
  let TestAutoArray: Array<TestAuto> = []
  SppClientAutoTestCase.forEach(item => {
    TestAutoArray.push(new TestAuto(item.name, item.detail,  item.method, item.api, item.result, item.Category));
  })
  return TestAutoArray;
}

export function initSppServerAutoData(): Array<TestAuto> {
  let TestAutoArray: Array<TestAuto> = []
  SppServerAutoTestCase.forEach(item => {
    TestAutoArray.push(new TestAuto(item.name, item.detail,  item.method, item.api, item.result, item.Category));
  })
  return TestAutoArray;
}


export function initA2dpSourceProfileAutoData(): Array<TestAuto> {
  let TestAutoArray: Array<TestAuto> = []
  A2dpSourceProfileAutoTestCase.forEach(item => {
    TestAutoArray.push(new TestAuto(item.name, item.detail,  item.method, item.api, item.result, item.Category));
  })
  return TestAutoArray;
}


export function initHandsFreeAudioGatewayProfileAutoData(): Array<TestAuto> {
  let TestAutoArray: Array<TestAuto> = []
  HandsFreeAudioGatewayProfileAutoTestCase.forEach(item => {
    TestAutoArray.push(new TestAuto(item.name, item.detail,  item.method, item.api, item.result, item.Category));
  })
  return TestAutoArray;
}


export function initHitHostProfileAutoData(): Array<TestAuto> {
  let TestAutoArray: Array<TestAuto> = []
  HitHostProfileAutoTestCase.forEach(item => {
    TestAutoArray.push(new TestAuto(item.name, item.detail,  item.method, item.api, item.result, item.Category));
  })
  return TestAutoArray;
}

export function initPanNetworkProfileAutoData(): Array<TestAuto> {
  let TestAutoArray: Array<TestAuto> = []
  PanNetworkProfileAutoTestCase.forEach(item => {
    TestAutoArray.push(new TestAuto(item.name, item.detail,  item.method, item.api, item.result, item.Category));
  })
  return TestAutoArray;
}

