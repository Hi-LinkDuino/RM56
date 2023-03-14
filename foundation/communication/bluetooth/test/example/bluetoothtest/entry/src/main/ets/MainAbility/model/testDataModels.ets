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
import { TestData, TestApi, Category, CaseCategory } from './testData'
import { testEnableBluetooth, testDisableBluetooth, testGetState, testGetBtConnectionState, testSetLocalName,
  testGetLocalName, testSetBluetoothScanMode, testGetBluetoothScanMode, testStartBluetoothDiscovery, testStopBluetoothDiscovery,
  testPairDevice, testGetRemoteDeviceName, testGetRemoteDeviceClass, testGetPairedDevices, testUnPairedDevice,
  testGetProfileConnState1,testGetProfileConnState2,testOnStateChange,testOnPinRequired,testOnBondStateChange} from './brInterface'
import { testCreateGattServerDevice, testCreateGattClientDevice, testGetConnectedBLEDevices, testStartBLEScan,testStopBLEScan,testOnBLEDeviceFind
} from './bleInterface'
import { testGattClient, testGattServer } from './gattInterface'
import { testSppClient, testSppServer } from './sppInterface'
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

import bluetooth from '@ohos.bluetooth';

const BRTestCase: []= [
  { 'name': '开蓝牙', 'detail': '使能蓝牙', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '关蓝牙', 'detail': '去使能蓝牙', 'method': 'disableBluetooth', 'api': testDisableBluetooth, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '获取状态', 'detail': '获取蓝牙状态', 'method': 'getState', 'api': testGetState, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '获取当前连接状态', 'detail': '获取当前蓝牙连接状态', 'method': 'getBtConnectionState', 'api': testGetBtConnectionState, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '设置本地名称', 'detail': '设置蓝牙本地名称', 'method': 'setLocalName', 'api': testSetLocalName,'result': 'None', 'category': CaseCategory.BR },
  { 'name': '获取本地名称', 'detail': '获取蓝牙本地名称', 'method': 'getLocalName', 'api': testGetLocalName,'result': 'None', 'category': CaseCategory.BR },
  { 'name': '设置扫描模式', 'detail': '设置蓝牙扫描模式', 'method': 'setBluetoothScanMode', 'api': testSetBluetoothScanMode, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '获取扫描模式', 'detail': '获取蓝牙扫描模式', 'method': 'getBluetoothScanMode', 'api': testGetBluetoothScanMode, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '开始蓝牙发现', 'detail': '发现可配对蓝牙设备', 'method': 'startBluetoothDiscovery', 'api': testStartBluetoothDiscovery, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '停止蓝牙发现', 'detail': '停止发现', 'method': 'stopBluetoothDiscovery', 'api': testStopBluetoothDiscovery, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '配对设备', 'detail': '进行蓝牙设备配对', 'method': 'pairDevice', 'api': testPairDevice, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '取消配对设备', 'detail': '取消蓝牙设备配对', 'method': 'unpairDevice', 'api': testUnPairedDevice, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '获取远程设备名称', 'detail': '获取配对蓝牙设备的名称', 'method': 'getDeviceName', 'api': testGetRemoteDeviceName, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '获取远程设备类型', 'detail': '获取配对蓝牙设备的类型', 'method': 'getDeviceType', 'api': testGetRemoteDeviceClass, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '获取配对设备', 'detail': '获取配对蓝牙设备的信息', 'method': 'getPairedDeviceIds', 'api': testGetPairedDevices, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '获取配置文件连接状态1', 'detail': '获取配置文件的连接状态', 'method': 'getProfileConnState', 'api': testGetProfileConnState1, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '获取配置文件连接状态2', 'detail': '获取设备的配置文件状态', 'method': 'getDeviceState', 'api': testGetProfileConnState2, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '注册状态变化', 'detail': '注册状态变化', 'method': 'on.stateChange', 'api': testOnStateChange, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '注册pin码变化', 'detail': '注册pin码变化', 'method': 'on.pinRequired', 'api': testOnPinRequired, 'result': 'None', 'category': CaseCategory.BR },
  { 'name': '注册绑定状态变化', 'detail': '注册绑定状态变化', 'method': 'on.bondStateChange', 'api': testOnBondStateChange, 'result': 'None', 'category': CaseCategory.BR }
]

const BleTestCase: any[]  = [
  { 'name': '创建Gatt服务器', 'detail': '创建Gatt服务器', 'method': 'createGattServer', 'api': testCreateGattServerDevice, 'result': 'None', 'category': CaseCategory.BLE },
  { 'name': '创建Gatt客户端', 'detail': '创建Gatt客户端', 'method': 'createGattClient', 'api': testCreateGattClientDevice, 'result': 'None', 'category': CaseCategory.BLE },
  { 'name': '获取连接的BLE设备', 'detail': '获取连接的BLE设备信息', 'method': 'getConnectedBLEDevices', 'api': testGetConnectedBLEDevices, 'result': 'None', 'category': CaseCategory.BLE },
  { 'name': '开始BLE扫描', 'detail': '开始扫描BLE设备', 'method': 'startBLEScan', 'api': testStartBLEScan, 'result': 'None', 'category': CaseCategory.BLE },
  { 'name': '停止BLE扫描', 'detail': '停止扫描', 'method': 'stopBLEScan', 'api': testStopBLEScan, 'result': 'None', 'category': CaseCategory.BLE },
  { 'name': '注册BLE查找状态变化', 'detail': '注册BLE设备查找状态变化', 'method': 'on.BLEDeviceFind', 'api': testOnBLEDeviceFind, 'result': 'None', 'category': CaseCategory.BLE }
]

const GattTestComposition: any[]  = [
  { 'name': 'GattClient','detail': 'Gatt客户端测试','url':'pages/subManualApiTest/subGattTest/gattClient','image': $r('app.media.Gatt') , 'category': CaseCategory.Gatt },
  { 'name': 'GattServer', 'detail': 'Gatt服务器测试', 'url':'pages/subManualApiTest/subGattTest/gattServer','image': $r('app.media.Gatt') , 'category': CaseCategory.Gatt },
]

const SppTestComposition: any[]  = [
  { 'name': 'SppClient', 'detail': 'Spp客户端测试', 'url':'pages/subManualApiTest/subSppTest/sppClientTest','image': $r('app.media.spp') , 'category': CaseCategory.SPP },
  { 'name': 'SppServer', 'detail': 'Spp服务器测试', 'url':'pages/subManualApiTest/subSppTest/sppServerTest','image': $r('app.media.spp') , 'category': CaseCategory.SPP },
]

//const ProfileTestCase: any[]  = [
//  { 'name': 'A2dpSource配置', 'detail': 'A2dpSource配置测试', 'method': 'A2dpSourceProfile', 'api': testA2dpSourceProfile, 'result': 'None', 'category': CaseCategory.Profile },
//  { 'name': 'HandsFreeAudioGateway配置', 'detail': 'HandsFreeAudioGateway配置测试', 'method': 'HandsFreeAudioGatewayProfile', 'api': testHandsFreeAudioGatewayProfile, 'result': 'None', 'category': CaseCategory.Profile },
//  { 'name': 'HidHost配置', 'detail': 'HidHost配置测试', 'method': 'HidHostProfile', 'api': testHidHostProfile, 'result': 'None', 'category': CaseCategory.Profile },
//  { 'name': 'PanNetwork配置', 'detail': 'PanNetwork配置测试', 'method': 'PanNetworkProfile', 'api': testPanNetworkProfile, 'result': 'None', 'category': CaseCategory.Profile },
//]

const GattClientTestCase: any[] =[
  { 'name': '创造Gatt客户端', 'detail': '创造Gatt客户端', 'method': 'createGattClient', 'api': testCreateGattClient, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '连接', 'detail': 'Gatt连接', 'method': 'connect', 'api': testConnect, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '取消连接', 'detail': '取消Gatt连接', 'method': 'disconnect', 'api': testDisconnect, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': 'BLE连接状态', 'detail': '查看BLE连接状态', 'method': 'on.BLEConnectionStateChange', 'api': testOnBLEConnectionStateChange, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '关闭', 'detail': '关闭Gatt客户端', 'method': 'close', 'api': testGattClientClose, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '获取服务callback', 'detail': '获取服务callback', 'method': 'getServices(Callback)', 'api': testGetServicesCallback, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '获取服务promise', 'detail': '获取服务promise', 'method': 'getServices(promise)', 'api': testGetServicesPromise, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '读取特征值callback', 'detail': '读取特征值callback', 'method': 'readCharacteristicValue(callback)', 'api': testReadCharacteristicValueCallback, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '读取特征值promise', 'detail': '读取特征值promise', 'method': 'readCharacteristicValue(promise)', 'api': testReadCharacteristicValuePromise, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '读取描述符值callback', 'detail': '读取描述符值callback', 'method': 'readDescriptorValue(callback)', 'api': testReadDescriptorValueCallback, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '读取描述符值promise', 'detail': '读取描述符值promise', 'method': 'readDescriptorValue(promise)', 'api': testReadDescriptorValuePromise, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '写入特征值', 'detail': '写入特征值', 'method': 'writeCharacteristicValue', 'api': testWriteCharacteristicValue, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '写入描述符值', 'detail': '写入描述符值', 'method': 'writeDescriptorValue', 'api': testWriteDescriptorValue, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': 'BLE特征值变化', 'detail': '查看BLE特征值变化', 'method': 'on.BLECharacteristicChange', 'api': testOnBLECharacteristicChange, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '设定BLEMtu尺寸', 'detail': '设定BLEMtu尺寸', 'method': 'setBLEMtuSize', 'api': testSetBLEMtuSize, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '设置通知特性更改', 'detail': '设置通知特性更改', 'method': 'setNotifyCharacteristicChanged', 'api': testSetNotifyCharacteristicChanged, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '获取设备名称callback', 'detail': '获取设备名称callback', 'method': 'getDeviceName()callback', 'api': testGetDeviceNameCallback, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '获取设备名称promise', 'detail': '获取设备名称promise', 'method': 'getDeviceName()Promise', 'api': testGetDeviceNamePromise, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '获取Rssi值callback', 'detail': '获取Rssi值callback', 'method': 'getRssiValue()Callback', 'api': testGetRssiValueCallback, 'result': 'None', 'category': CaseCategory.GattClient },
  { 'name': '获取Rssi值promise', 'detail': '获取Rssi值promise', 'method': 'getRssiValue()Promise', 'api': testGetRssiValuePromise, 'result': 'None', 'category': CaseCategory.GattClient },
]

const GattServerTestCase: any[]  = [
  { 'name': '创建Gatt服务器', 'detail': '创建Gatt服务器', 'method': 'createGattServer', 'api': testCreateGattServer, 'result': 'None', 'category': CaseCategory.GattServer },
  { 'name': '添加服务', 'detail': '添加Gatt服务', 'method': 'addService', 'api': testAddService, 'result': 'None', 'category': CaseCategory.GattServer },
  { 'name': '删除服务', 'detail': '删除Gatt服务', 'method': 'removeService', 'api': testRemoveService, 'result': 'None', 'category': CaseCategory.GattServer },
  { 'name': '开始广播', 'detail': '开始广播播报', 'method': 'startAdvertising', 'api': testStartAdvertising, 'result': 'None', 'category': CaseCategory.GattServer },
  { 'name': '停止广播', 'detail': '停止广播播报', 'method': 'stopAdvertising', 'api': testStopAdvertising, 'result': 'None', 'category': CaseCategory.GattServer },
  { 'name': '关闭', 'detail': '关闭Gatt服务器', 'method': 'close', 'api': testGattServerClose, 'result': 'None', 'category': CaseCategory.GattServer },
  { 'name': '连接状态', 'detail': '查看Gatt连接状态', 'method': 'on.connectStateChange', 'api': testOnConnectStateChange, 'result': 'None', 'category': CaseCategory.GattServer },
  { 'name': '读取特征值', 'detail': '读取特征值', 'method': 'on.characteristicRead', 'api': testOnCharacteristicRead, 'result': 'None', 'category': CaseCategory.GattServer },
  { 'name': '写入特征值', 'detail': '写入特征值', 'method': 'on.characteristicWrite', 'api': testOnCharacteristicWrite, 'result': 'None', 'category': CaseCategory.GattServer },
  { 'name': '读取描述符', 'detail': '读取描述符', 'method': 'on.descriptorRead', 'api': testOnDescriptorRead, 'result': 'None', 'category': CaseCategory.GattServer },
  { 'name': '写入描述符', 'detail': '写入描述符', 'method': 'on.descriptorWrite', 'api': testOnDescriptorWrite, 'result': 'None', 'category': CaseCategory.GattServer },
]

const SppClientTestCase: any[]  = [
  { 'name': '开蓝牙', 'detail': '使能蓝牙', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': CaseCategory.SPPClient },
  { 'name': '关蓝牙', 'detail': '去使能蓝牙', 'method': 'disableBluetooth', 'api': testDisableBluetooth, 'result': 'None', 'category': CaseCategory.SPPClient },
  { 'name': '设置SPP扫描模式', 'detail': '设置SPP扫描模式', 'method': 'setBluetoothScanMode', 'api': testSetBluetoothScanMode, 'result': 'None', 'category': CaseCategory.SPPClient },
  { 'name': '获取状态', 'detail': '获取蓝牙状态', 'method': 'getState', 'api': testGetState, 'result': 'None', 'category': CaseCategory.SPPClient },
  { 'name': '获取扫描模式', 'detail': '获取蓝牙扫描模式', 'method': 'getBluetoothScanMode', 'api': testGetBluetoothScanMode, 'result': 'None', 'category': CaseCategory.SPPClient },
  { 'name': '获取当前连接状态', 'detail': '获取当前蓝牙连接状态', 'method': 'getBtConnectionState', 'api': testGetBtConnectionState, 'result': 'None', 'category': CaseCategory.SPPClient },
  { 'name': 'Spp客户端连接(安全模式)', 'detail': 'Spp客户端(安全模式)连接', 'method': 'sppConnect', 'api': testSppClientConnectSafe, 'result': 'None', 'category': CaseCategory.SPPClient },
  { 'name': 'Spp客户端连接(非安全模式)', 'detail': 'Spp客户端(非安全模式)连接', 'method': 'sppConnect', 'api': testSppClientConnectUnSafe, 'result': 'None', 'category': CaseCategory.SPPClient },
  { 'name': '配对设备', 'detail': '进行蓝牙设备配对', 'method': 'pairDevice', 'api': testPairDevice, 'result': 'None', 'category': CaseCategory.SPPClient },
  { 'name': 'Spp客户端写入', 'detail': 'Spp客户端写入', 'method': 'sppWrite', 'api': testSppClientWrite, 'result': 'None', 'category': CaseCategory.SPPClient },
  { 'name': 'Spp客户端读取打开', 'detail': 'Spp客户端读取打开', 'method': 'on.sppRead', 'api': testSppClientReadOn, 'result': 'None', 'category': CaseCategory.SPPClient },
  { 'name': 'Spp客户端读取关闭', 'detail': 'Spp客户端读取关闭', 'method': 'off.sppRead', 'api': testSppClientReadOff, 'result': 'None', 'category': CaseCategory.SPPClient },
  { 'name': 'Spp客户端关闭', 'detail': 'Spp客户端关闭', 'method': 'sppCloseClientSocket', 'api': testSppClientClose, 'result': 'None', 'category': CaseCategory.SPPClient },
]

const SppServerTestCase: any[]  = [
  { 'name': '开蓝牙', 'detail': '使能蓝牙', 'method': 'enableBluetooth', 'api': testEnableBluetooth, 'result': 'None', 'category': CaseCategory.SPPServer },
  { 'name': '关蓝牙', 'detail': '去使能蓝牙', 'method': 'disableBluetooth', 'api': testDisableBluetooth, 'result': 'None', 'category': CaseCategory.SPPServer },
  { 'name': '设置SPP扫描模式', 'detail': '设置SPP扫描模式', 'method': 'setBluetoothScanMode', 'api': testSetBluetoothScanMode, 'result': 'None', 'category': CaseCategory.SPPServer },
  { 'name': '获取状态', 'detail': '获取蓝牙状态', 'method': 'getState', 'api': testGetState, 'result': 'None', 'category': CaseCategory.SPPServer },
  { 'name': '获取扫描模式', 'detail': '获取蓝牙扫描模式', 'method': 'getBluetoothScanMode', 'api': testGetBluetoothScanMode, 'result': 'None', 'category': CaseCategory.SPPServer },
  { 'name': '获取当前连接状态', 'detail': '获取当前蓝牙连接状态', 'method': 'getBtConnectionState', 'api': testGetBtConnectionState, 'result': 'None', 'category': CaseCategory.SPPServer },
  { 'name': 'Spp服务器创建(安全模式)', 'detail': 'Spp服务器(安全模式)创建', 'method': 'sppListen', 'api': testSppServerCreateSafe, 'result': 'None', 'category': CaseCategory.SPPServer },
  { 'name': 'Spp服务器创建(非安全模式)', 'detail': 'Spp服务器(非安全模式)创建', 'method': 'sppListen', 'api': testSppServerCreateUnSafe, 'result': 'None', 'category': CaseCategory.SPPServer },
  { 'name': '注册pin码变化', 'detail': '注册pin码变化', 'method': 'on.pinRequired', 'api': testOnPinRequired, 'result': 'None', 'category': CaseCategory.SPPServer },
//  { 'name': 'SPP客户端写入', 'detail': '使能蓝牙', 'method': 'spp_Client_Write', 'api': testSppClientWrite, 'result': 'None', 'category': CaseCategory.SPPServer },
  { 'name': 'Spp服务器写入', 'detail': 'Spp服务器写入', 'method': 'sppWrite', 'api': testSppServerWrite, 'result': 'None', 'category': CaseCategory.SPPServer },
  { 'name': 'Spp服务器接受', 'detail': 'Spp服务器接受', 'method': 'sppAccept', 'api': testSppServerAccept,'result': 'None', 'category': CaseCategory.SPPServer },
  { 'name': 'Spp服务器读取打开', 'detail': 'Spp服务器读取打开', 'method': 'on.sppRead', 'api': testSppServerReadOn, 'result': 'None', 'category': CaseCategory.SPPServer },
  { 'name': 'Spp服务器读取关闭', 'detail': 'Spp服务器读取关闭', 'method': 'off.sppRead', 'api': testSppServerReadOff, 'result': 'None', 'category': CaseCategory.SPPServer },
  { 'name': 'Spp服务器关闭', 'detail': 'Spp服务器关闭', 'method': 'sppCloseServerSocket', 'api': testSppCloseServerSocket, 'result': 'None', 'category': CaseCategory.SPPServer },
  { 'name': 'Spp客户端关闭', 'detail': 'Spp客户端关闭', 'method': 'sppCloseClientSocket', 'api': testSppCloseClientSocket, 'result': 'None', 'category': CaseCategory.SPPServer },
]


const A2dpSourceProfileTestCase:any []=[
  { 'name': '连接', 'detail': 'Profile连接', 'method': 'connect', 'api': testProfileConnect, 'result': 'None', 'category': CaseCategory.ProfileA2dpSource },
  { 'name': '取消连接', 'detail': '取消Profile连接', 'method': 'disconnect', 'api': testProfileDisConnect, 'result': 'None', 'category': CaseCategory.ProfileA2dpSource },
  { 'name': '获取播放状态', 'detail': '获取Profile播放状态', 'method': 'getPlayingState', 'api': testProfileGetPlayingState, 'result': 'None', 'category': CaseCategory.ProfileA2dpSource},
  { 'name': '获取连接设备', 'detail': '获取Profile连接设备', 'method': 'getConnectionDevices', 'api': testProfileGetConnectionDevices, 'result': 'None', 'category': CaseCategory.ProfileA2dpSource },
  { 'name': '获取设备状态', 'detail': '获取Profile设备状态', 'method': 'getDeviceState', 'api': testProfileGetDeviceState, 'result': 'None', 'category': CaseCategory.ProfileA2dpSource},
]



const HandsFreeAudioGatewayProfileTestCase: any[]  = [
  { 'name': '连接', 'detail': 'Profile连接', 'method': 'connect', 'api': testProfileConnect, 'result': 'None', 'category': CaseCategory.ProfileHandsFreeAudioGateway },
  { 'name': '取消连接', 'detail': '取消Profile连接', 'method': 'disconnect', 'api': testProfileDisConnect, 'result': 'None', 'category': CaseCategory.ProfileHandsFreeAudioGateway },
  { 'name': '获取连接设备', 'detail': '获取Profile连接设备', 'method': 'getConnectionDevices', 'api': testProfileGetConnectionDevices, 'result': 'None', 'category': CaseCategory.ProfileHandsFreeAudioGateway, },
  { 'name': '获取设备状态', 'detail': '获取Profile设备状态', 'method': 'getDeviceState', 'api': testProfileGetDeviceState, 'result': 'None', 'category': CaseCategory.ProfileHandsFreeAudioGateway, },
]


const HitHostProfileTestCase:any[] =[
  { 'name': '连接', 'detail': 'Profile连接', 'method': 'connect', 'api': testProfileConnect, 'result': 'None', 'category': CaseCategory.ProfileHitHost },
  { 'name': '取消连接', 'detail': '取消Profile连接', 'method': 'disconnect', 'api': testProfileDisConnect, 'result': 'None', 'category': CaseCategory.ProfileHitHost },
  { 'name': '获取连接设备', 'detail': '获取Profile连接设备', 'method': 'getConnectionDevices', 'api': testProfileGetConnectionDevices, 'result': 'None', 'category': CaseCategory.ProfileHitHost },
  { 'name': '获取设备状态', 'detail': '获取Profile设备状态', 'method': 'getDeviceState', 'api': testProfileGetDeviceState, 'result': 'None', 'category': CaseCategory.ProfileHitHost },
]

const PanNetworkProfileTestCase:any[] =[
  { 'name': '取消连接', 'detail': '取消Profile连接', 'method': 'disconnect', 'api': testProfileDisConnect, 'result': 'None', 'category': CaseCategory.ProfilePanNetwork },
  { 'name': '获取连接设备', 'detail': '获取Profile连接设备', 'method': 'getConnectionDevices', 'api': testProfileGetConnectionDevices, 'result': 'None', 'category': CaseCategory.ProfilePanNetwork },
  { 'name': '获取设备状态', 'detail': '获取Profile设备状态', 'method': 'getDeviceState', 'api': testProfileGetDeviceState, 'result': 'None', 'category': CaseCategory.ProfilePanNetwork },

]


const TestComposition: any[] = [
  { 'name': 'Classic', 'detail': '经典蓝牙测试', 'url': 'pages/subManualApiTest/BrInterfaceTest', 'image': $r('app.media.Bticon'), 'category': Category.API},
  { 'name': 'BLE', 'detail': '低功耗蓝牙测试', 'url': 'pages/subManualApiTest/bleTest', 'image': $r('app.media.ble40'), 'category': Category.API},
  { 'name': 'Gatt', 'detail': 'GATT测试', 'url': 'pages/subManualApiTest/gattTest', 'image': $r('app.media.Gatt'), 'category': Category.API},
  { 'name': 'SPP', 'detail': 'SPP测试', 'url': 'pages/subManualApiTest/sppTest', 'image': $r('app.media.spp'), 'category': Category.API},
//  { 'name': 'Scenario', 'detail': '场景测试', 'image': $r('app.media.ScenarioTest'), 'category': Category.Scenario},
  { 'name': 'BrAutoTest', 'detail': 'Br自动测试', 'url': 'pages/subAutoTest/brAutoTest','image': $r("app.media.AutoTestIcon"), 'category': Category.AutoTest},
  { 'name': 'BleAutoTest', 'detail': 'Ble自动测试', 'url': 'pages/subAutoTest/bleAutoTest','image': $r("app.media.AutoTestIcon"), 'category': Category.AutoTest},
  { 'name': 'GattAutoTest', 'detail': 'Gatt自动测试', 'url': 'pages/subAutoTest/gattAutoTest','image': $r("app.media.AutoTestIcon"), 'category': Category.AutoTest},
  { 'name': 'SppAutoTest', 'detail': 'Spp自动测试', 'url': 'pages/subAutoTest/sppAutoTest','image': $r("app.media.AutoTestIcon"), 'category': Category.AutoTest},
  { 'name': 'BrFoundScenarioTest', 'detail':'经典蓝牙发现场景测试', 'url':'pages/subScenarioTest/brFoundScenarioTest', 'image': $r('app.media.ScenarioTest'), 'category': Category.Scenario},
  { 'name': 'BleFoundScenarioTest', 'detail':'Ble蓝牙发现场景测试', 'url':'pages/subScenarioTest/bleFoundScenarioTest', 'image': $r('app.media.ScenarioTest'), 'category': Category.Scenario},
  { 'name': 'GattClientScenarioTest', 'detail':'GattClient场景测试' , 'url':'pages/subScenarioTest/gattClientScenarioTest', 'image': $r('app.media.ScenarioTest'), 'category': Category.Scenario},
  { 'name': 'GattServerScenarioTest', 'detail':'GattServer场景测试' , 'url':'pages/subScenarioTest/gattServerScenarioTest', 'image': $r('app.media.ScenarioTest'), 'category': Category.Scenario},
  { 'name': 'SppClientScenarioTest', 'detail':'SppClient场景测试' , 'url':'pages/subScenarioTest/sppClientScenarioTest', 'image': $r('app.media.ScenarioTest'), 'category': Category.Scenario},
  { 'name': 'SppServerScenarioTest', 'detail':'SppServer场景测试' , 'url':'pages/subScenarioTest/sppServerScenarioTest', 'image': $r('app.media.ScenarioTest'), 'category': Category.Scenario},
  { 'name': 'ProfileAutoTest', 'detail': 'Profile自动测试', 'url': 'pages/subAutoTest/profileAutoTest','image': $r("app.media.AutoTestIcon"), 'category': Category.AutoTest},
//  { 'name': 'Profile', 'detail': 'Profile测试', 'url': 'pages/subManualApiTest/profileTest', 'image': $r('app.media.profile'), 'category': Category.Profile},
  { 'name': 'A2dpSource', 'detail': 'A2dpSource配置测试', 'url': 'pages/subProfileTest/a2dpSourceProfileTest', 'image': $r('app.media.profile'), 'category': Category.Profile },
  { 'name': 'HFAudioGateway', 'detail': 'HFAudioGateway配置测试', 'url': 'pages/subProfileTest/handsFreeAudioGatewayProfileTest', 'image': $r('app.media.profile'), 'category': Category.Profile},
  { 'name': 'HidHost', 'detail': 'HidHost配置测试', 'url': 'pages/subProfileTest/hidHostProfileTest', 'image': $r('app.media.profile'), 'category': Category.Profile},
  { 'name': 'PanNetwork', 'detail': 'PanNetwork配置测试', 'url': 'pages/subProfileTest/panNetworkProfileTest', 'image': $r('app.media.profile'), 'category': Category.Profile},
  { 'name': 'BrBenchmarkTest', 'detail': '经典蓝牙性能测试', 'url': 'pages/subBenchmarkTest/brBenchmarkTest', 'image': $r('app.media.Benchmark'), 'category': Category.BenchMark},
  { 'name': 'BleBenchmarkTest', 'detail': 'ble蓝牙性能测试', 'url': 'pages/subBenchmarkTest/bleBenchmarkTest', 'image': $r('app.media.Benchmark'), 'category': Category.BenchMark},
  { 'name': 'GattClientVelocityBenchmarkTest', 'detail': 'GattClient速率性能测试', 'url': 'pages/subBenchmarkTest/gattClientVelocityBenchmarkTest', 'image': $r('app.media.Benchmark'), 'category': Category.BenchMark},
  { 'name': 'GattServerVelocityBenchmarkTest', 'detail': 'GattServer速率性能测试', 'url': 'pages/subBenchmarkTest/gattServerVelocityBenchmarkTest', 'image': $r('app.media.Benchmark'), 'category': Category.BenchMark},
  { 'name': 'SppClientVelocityBenchmarkTest', 'detail': 'SppClient速率性能测试', 'url': 'pages/subBenchmarkTest/sppClientVelocityBenchmarkTest', 'image': $r('app.media.Benchmark'), 'category': Category.BenchMark},
  { 'name': 'SppServerVelocityBenchmarkTest', 'detail': 'SppServer速率性能测试', 'url': 'pages/subBenchmarkTest/sppServerVelocityBenchmarkTest', 'image': $r('app.media.Benchmark'), 'category': Category.BenchMark},
  { 'name': 'SppClientBandWidthBenchmarkTest', 'detail': 'SppClient带宽性能测试', 'url': 'pages/subBenchmarkTest/sppClientBandWidthBenchmarkTest', 'image': $r('app.media.Benchmark'), 'category': Category.BenchMark},
  { 'name': 'SppServerBandWidthBenchmarkTest', 'detail': 'SppServer带宽性能测试', 'url': 'pages/subBenchmarkTest/sppServerBandWidthBenchmarkTest', 'image': $r('app.media.Benchmark'), 'category': Category.BenchMark}
]

const GattAutoTestComposition :any[] =[
  { 'name': 'GattClientAutoTest', 'detail': 'GattClient自动测试', 'url': 'pages/subAutoTest/subGattAutoTest/gattClientAutoTest','image': $r("app.media.AutoTestIcon"), 'category': Category.GattAutoTest},
  { 'name': 'GattServerAutoTest', 'detail': 'GattServer自动测试', 'url': 'pages/subAutoTest/subGattAutoTest/gattServerAutoTest','image': $r("app.media.AutoTestIcon"), 'category': Category.GattAutoTest},
]

const SppAutoTestComposition :any[] =[
  { 'name': 'SppClientAutoTest', 'detail': 'SppClient自动测试', 'url': 'pages/subAutoTest/subSppAutoTest/sppClientAutoTest','image': $r("app.media.AutoTestIcon"), 'category': Category.SppAutoTest},
  { 'name': 'SppServerAutoTest', 'detail': 'SppServer自动测试', 'url': 'pages/subAutoTest/subSppAutoTest/sppServerAutoTest','image': $r("app.media.AutoTestIcon"), 'category': Category.SppAutoTest},
]

const ProfileAutoTestComposition :any[] =[
  { 'name': 'A2dpSourceAutoTest', 'detail': 'A2dpSource自动测试', 'url': 'pages/subAutoTest/subProfileAutoTest/a2dpSourceAutoTest','image': $r("app.media.AutoTestIcon"), 'category': Category.ProfileAutoTest},
  { 'name': 'HandsFreeAudioGatewayAutoTest', 'detail': 'HandsFreeAudioGateway自动测试', 'url': 'pages/subAutoTest/subProfileAutoTest/handsFreeAudioGatewayAutoTest','image': $r("app.media.AutoTestIcon"), 'category': Category.ProfileAutoTest},
  { 'name': 'HitHostAutoTest', 'detail': 'HitHost自动测试', 'url': 'pages/subAutoTest/subProfileAutoTest/hitHostAutoTest','image': $r("app.media.AutoTestIcon"), 'category': Category.ProfileAutoTest},
  { 'name': 'PanNetworkAutoTest', 'detail': 'PanNetwork自动测试', 'url': 'pages/subAutoTest/subProfileAutoTest/panNetworkAutoTest','image': $r("app.media.AutoTestIcon"), 'category': Category.ProfileAutoTest},

]

export function initTestData(): Array<TestData> {
  let TestDataArray: Array<TestData> = []
  TestComposition.forEach(item => {
    TestDataArray.push(new TestData(item.name, item.detail, item.url, item.image, item.category));
  })
  return TestDataArray;
}

export function initBRApiData(): Array<TestApi> {
  let TestApiArray: Array<TestApi> = []
  BRTestCase.forEach(item => {
    TestApiArray.push(new TestApi(item.name, item.detail, item.method, item.api, item.result, item.category));
  })
  return TestApiArray;
}

export function initBleApiData(): Array<TestApi> {
  let TestApiArray: Array<TestApi> = []
  BleTestCase.forEach(item => {
    TestApiArray.push(new TestApi(item.name, item.detail, item.method, item.api, item.result, item.category));
  })
  return TestApiArray;
}

//export function initGattApiData(): Array<TestApi> {
//  let TestApiArray: Array<TestApi> = []
//  GattTestCase.forEach(item => {
//    TestApiArray.push(new TestApi(item.name, item.detail, item.method, item.api, item.result, item.category));
//  })
//  return TestApiArray;
//}

export function initGattApiTestData(): Array<TestData> {
  let TestDataArray: Array<TestData> = []
  GattTestComposition.forEach(item => {
    TestDataArray.push(new TestData(item.name, item.detail, item.url, item.image, item.category));
  })
  return TestDataArray;
}

export function initSppApiTestData(): Array<TestData> {
  let TestDataArray: Array<TestData> = []
  SppTestComposition.forEach(item => {
    TestDataArray.push(new TestData(item.name, item.detail, item.url, item.image, item.category));
  })
  return TestDataArray;
}

//export function initProfileApiTestData(): Array<TestApi> {
//  let TestApiArray: Array<TestApi> = []
//  ProfileTestCase.forEach(item => {
//    TestApiArray.push(new TestApi(item.name, item.detail, item.method, item.api, item.result, item.category));
//  })
//  return TestApiArray;
//}

export function initGattClientApiData(): Array<TestApi> {
  let TestApiArray: Array<TestApi> = []
  GattClientTestCase.forEach(item => {
    TestApiArray.push(new TestApi(item.name, item.detail, item.method, item.api, item.result, item.category));
  })
  return TestApiArray;
}

export function initGattServerApiData(): Array<TestApi> {
  let TestApiArray: Array<TestApi> = []
  GattServerTestCase.forEach(item => {
    TestApiArray.push(new TestApi(item.name, item.detail, item.method, item.api, item.result, item.category));
  })
  return TestApiArray;
}

export function initSppClientApiData(): Array<TestApi> {
  let TestApiArray: Array<TestApi> = []
  SppClientTestCase.forEach(item => {
    TestApiArray.push(new TestApi(item.name, item.detail, item.method, item.api, item.result, item.category));
  })
  return TestApiArray;
}

export function initSppServerApiData(): Array<TestApi> {
  let TestApiArray: Array<TestApi> = []
  SppServerTestCase.forEach(item => {
    TestApiArray.push(new TestApi(item.name, item.detail, item.method, item.api, item.result, item.category));
  })
  return TestApiArray;
}

export function initA2dpSourceProfileApiData(): Array<TestApi> {
  let TestApiArray: Array<TestApi> = []
  A2dpSourceProfileTestCase.forEach(item => {
    TestApiArray.push(new TestApi(item.name, item.detail, item.method, item.api, item.result, item.category));
  })
  return TestApiArray;
}

export function initHandsFreeAudioGatewayProfileApiData(): Array<TestApi> {
  let TestApiArray: Array<TestApi> = []
  HandsFreeAudioGatewayProfileTestCase.forEach(item => {
    TestApiArray.push(new TestApi(item.name, item.detail, item.method, item.api, item.result, item.category));
  })
  return TestApiArray;
}


export function initHitHostProfileApiData(): Array<TestApi> {
  let TestApiArray: Array<TestApi> = []
  HitHostProfileTestCase.forEach(item => {
    TestApiArray.push(new TestApi(item.name, item.detail, item.method, item.api, item.result, item.category));
  })
  return TestApiArray;
}

export function initPanNetworkProfileApiData(): Array<TestApi> {
  let TestApiArray: Array<TestApi> = []
  PanNetworkProfileTestCase.forEach(item => {
    TestApiArray.push(new TestApi(item.name, item.detail, item.method, item.api, item.result, item.category));
  })
  return TestApiArray;
}

export function initGattAutoTestData(): Array<TestData> {
  let TestDataArray: Array<TestData> = []
  GattAutoTestComposition.forEach(item => {
    TestDataArray.push(new TestData(item.name, item.detail, item.url, item.image, item.category));
  })
  return TestDataArray;
}

export function initSppAutoTestData(): Array<TestData> {
  let TestDataArray: Array<TestData> = []
  SppAutoTestComposition.forEach(item => {
    TestDataArray.push(new TestData(item.name, item.detail, item.url, item.image, item.category));
  })
  return TestDataArray;
}

export function initProfileAutoTestData(): Array<TestData> {
  let TestDataArray: Array<TestData> = []
  ProfileAutoTestComposition.forEach(item => {
    TestDataArray.push(new TestData(item.name, item.detail, item.url, item.image, item.category));
  })
  return TestDataArray;
}