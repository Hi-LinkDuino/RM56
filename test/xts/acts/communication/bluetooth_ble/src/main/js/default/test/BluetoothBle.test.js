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

import bluetooth from '@ohos.bluetooth';
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'


let SppType = {
    SPP_RFCOMM : 0
}

let MatchMode = {
    MATCH_MODE_AGGRESSIVE : 1,
    MATCH_MODE_STICKY : 2
}

let MajorClass = {
    MAJOR_MISC : 0x0000,
    MAJOR_COMPUTER : 0x0100,
    MAJOR_PHONE : 0x0200,
    MAJOR_NETWORKING : 0x0300,
    MAJOR_AUDIO_VIDEO : 0x0400,
    MAJOR_PERIPHERAL : 0x0500,
    MAJOR_IMAGING : 0x0600,
    MAJOR_WEARABLE : 0x0700,
    MAJOR_TOY : 0x0800,
    MAJOR_HEALTH : 0x0900,
    MAJOR_UNCATEGORIZED : 0x1F00
}


describe('bluetoothhostTest', function() {

    let gattServer = null;
    let gattClient = null;
    beforeAll(function () {
        console.info('beforeAll called')
        gattServer = bluetooth.BLE.createGattServer();
        gattClient = bluetooth.BLE.createGattClientDevice("00:00:00:00:00:00");
    })
    beforeEach(function () {
        console.info('beforeEach called')

    })
    afterEach(function () {
        console.info('afterEach called')
    })
    afterAll(function () {
        console.info('afterAll called')
    })

    function sleep(delay) {
        return new Promise(resovle => setTimeout(resovle, delay))
    }

    async function tryToEnableBt() {
        let sta = bluetooth.getState();
        switch(sta){
            case 0:
                console.info('[bluetooth_js] bt turn off:'+ JSON.stringify(sta));
                bluetooth.enableBluetooth();
                await sleep(3000);
                break;
            case 1:
                console.info('[bluetooth_js] bt turning on:'+ JSON.stringify(sta));
                await sleep(3000);
                break;
            case 2:
                console.info('[bluetooth_js] bt turn on:'+ JSON.stringify(sta));
                break;
            case 3:
                console.info('[bluetooth_js] bt turning off:'+ JSON.stringify(sta));
                bluetooth.enableBluetooth();
                await sleep(3000);
                break;
            default:
                console.info('[bluetooth_js] enable success');
        }
    }

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_ENABLE_0001
     * @tc.name testEnableBluetooth
     * @tc.desc Test EnableBluetooth api by promise.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_ENABLE_0001', 0, async function (done) {
        console.info('[bluetooth_js] enable start');
        await tryToEnableBt();
        let state = bluetooth.getState();
        console.info('[bluetooth_js] getState On = '+ JSON.stringify(state));
        expect(state).assertEqual(2);
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetoothble_GET_CONNECTED_BLEDEVICES_0001
     * @tc.name testGetConnectedBLEDevices
     * @tc.desc Test getConnectedBLEDevices api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetoothble_GET_CONNECTED_BLEDEVICES_0001', 0, async function (done) {
        console.info('[bluetooth_js] getConnectedBLEDevices test start ...');
        await tryToEnableBt();
        let result = bluetooth.BLE.getConnectedBLEDevices();
        console.info("[bluetooth_js] getConnDev:" + JSON.stringify(result)+ "length:" +result.length);
        expect(result.length).assertEqual(0);
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetoothble_CREATE_GATT_SERVER_0001
     * @tc.name testCreateGattServer
     * @tc.desc Test CreateGattServer api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetoothble_CREATE_GATT_SERVER_0001', 0, async function (done) {
        console.info('[bluetooth_js] createGattServer test start ...');
        await tryToEnableBt();
        let result = bluetooth.BLE.createGattServer();
        let resultLength = Object.keys(result).length;
        console.info("[bluetooth_js] createGattServer:" + JSON.stringify(result) + "length:" + resultLength);
        expect(resultLength).assertEqual(1);
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_START_ADVERTISING_0001
     * @tc.name testStartAdvertising
     * @tc.desc Test StartAdvertising api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_START_ADVERTISING_0001', 0, async function (done) {
        console.info('[bluetooth_js] BLE advertising start');
        await tryToEnableBt();
        let manufactureValueBuffer = new Uint8Array(4);
        manufactureValueBuffer[0] = 1;
        manufactureValueBuffer[1] = 2;
        manufactureValueBuffer[2] = 3;
        manufactureValueBuffer[3] = 4;
        let serviceValueBuffer = new Uint8Array(4);
        serviceValueBuffer[0] = 4;
        serviceValueBuffer[1] = 6;
        serviceValueBuffer[2] = 7;
        serviceValueBuffer[3] = 8;
        let promise = new Promise((resolve) => {
            let gattServer = bluetooth.BLE.createGattServer()
            let start = gattServer.startAdvertising({
                interval:150,
                txPower:60,
                connectable:true,
            },{
                serviceUuids:["12"],
                manufactureData:[{
                                     manufactureId:4567,
                                     manufactureValue:manufactureValueBuffer.buffer
                                 }],
                serviceData:[{
                                 serviceUuid:"1234",
                                 serviceValue:serviceValueBuffer.buffer
                             }],
            },{
                serviceUuids:["12"],
                manufactureData:[{
                                     manufactureId:1789,
                                     manufactureValue:manufactureValueBuffer.buffer
                                 }],
                serviceData:[{
                                 serviceUuid:"1794",
                                 serviceValue:serviceValueBuffer.buffer
                             }],
            });
            console.info('[bluetooth_js] startAdv res: ' + JSON.stringify(start));
            expect(start).assertNull();
            resolve()
        })
        await promise.then(done)
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_STOP_ADVERTISING_0001
     * @tc.name testStopAdvertising
     * @tc.desc Test StopAdvertising api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_STOP_ADVERTISING_0001', 0, async function (done) {
        console.info('[bluetooth_js] BLE stop advertising start');
        await tryToEnableBt();
        let gattServer = bluetooth.BLE.createGattServer();
        let stop = gattServer.stopAdvertising();
        expect(stop).assertNull();
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GATT_CONNRCT_0001
     * @tc.name testConnect
     * @tc.desc Test Connect api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GATT_CONNRCT_0001', 0, async function (done) {
        console.info('[bluetooth_js] gatt connect start');
        await tryToEnableBt();
        gattClient = bluetooth.BLE.createGattClientDevice("00:00:00:00:00:00");
        let ret = gattClient.connect();
        expect(ret).assertTrue();
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GET_RSSI_VALUE_CALLBACK
     * @tc.name testGetRssiValue
     * @tc.desc Test GetRssiValue api by callback.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GET_RSSI_VALUE_CALLBACK', 0, async function (done) {
        console.info('[bluetooth_js] BLE get rssi1 start');
        await tryToEnableBt();
        let promise = new Promise((resolve) => {
            let gattClient = bluetooth.BLE.createGattClientDevice("00:00:00:00:00:00");
            gattClient.getRssiValue((err, data)=> {
                console.info('[bluetooth_js] rssi err:' + JSON.stringify(err)+'value:'+JSON.stringify(data));
                expect(data).assertNull();
            });
            resolve()
        })
        await promise.then(done)
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GET_RSSI_VALUE_PROMISE_0001
     * @tc.name testGetRssiValue
     * @tc.desc Test GetRssiValue api by promise.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GET_RSSI_VALUE_PROMISE_0001', 0, async function (done) {
        console.info('[bluetooth_js] BLE get rssi start');
        await tryToEnableBt();
        let promise = new Promise((resolve) => {
            let gattClient = bluetooth.BLE.createGattClientDevice("00:00:00:00:00:00");
            gattClient.getRssiValue().then((data) => {
                if (data != null) {
                    expect(true).assertEqual(true);
                } else {
                    console.info('[bluetooth_js] BLE read rssi ' + JSON.stringify(data));
                    let rssiLength = Object.keys(data).length;
                    expect(rssiLength).assertEqual(0);
                }
            }).catch(err => {
                console.error(`bluetooth getRssiValue has error: ${err}`);
                expect(true).assertEqual(true);
            });
            resolve()
        })
        await promise.then(done)
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GET_DEVICE_NAME_PROMISE_0001
     * @tc.name testGetDeviceName
     * @tc.desc Test GetDeviceName api by promise.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GET_DEVICE_NAME_PROMISE_0001', 0, async function (done) {
        console.info('[bluetooth_js] BLE get device name promise start');
        await tryToEnableBt();
        let promise = new Promise((resolve) => {
            let gattClient = bluetooth.BLE.createGattClientDevice("00:00:00:00:00:00");
            gattClient.getDeviceName().then((data) => {
                console.info('[bluetooth_js] device name' + JSON.stringify(data))
                expect(data).assertNull();
                done();
            })
            resolve()
        })
        await promise.then(done)
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GATT_GETSERVICES_CALLBACK_0001
     * @tc.name testGetServices
     * @tc.desc Test GetServices api by callback.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GATT_GETSERVICES_CALLBACK_0001', 0, async function (done) {
        console.info('[bluetooth_js] getServices callback start');
        await tryToEnableBt();
        let promise = new Promise((resolve) => {
            let gattClient = bluetooth.BLE.createGattClientDevice("00:00:00:00:00:00");
            gattClient.getServices((err, data)=> {
                console.info('[bluetooth_js] device name err1 ' + JSON.stringify(err) +'name1' + JSON.stringify(data));
                expect(true).assertEqual(err.code == -1);
                done();
            })
            resolve()
        })
        await promise.then(done)
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GATT_GETSERVICES_PROMISE_0001
     * @tc.name testGetServices
     * @tc.desc Test GetServices api by promise.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GATT_GETSERVICES_PROMISE_0001', 0, async function (done) {
        console.info('[bluetooth_js] getServices promise start');
        await tryToEnableBt();
        let promise = new Promise((resolve) => {
            let gattClient = bluetooth.BLE.createGattClientDevice("00:00:00:00:00:00");
            gattClient.getServices().then((object) => {
                if (object != null) {
                    expect(true).assertEqual(true);
                } else {
                    console.info('[bluetooth_js] getServices successfully:'+JSON.stringify(object));
                    expect(null).assertFail();
                }
                done();
            }).catch(err => {
                console.error(`bluetooth getServices has error: ${err}`);
                expect(true).assertEqual(true);
                done();
            });
            resolve()
        })
        await promise.then(done)
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GATT_READ_CHARA_VALUE_0001
     * @tc.name testReadCharacteristicValue
     * @tc.desc Test ReadCharacteristicValue api by promise.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GATT_READ_CHARA_VALUE_0001', 0, async function (done) {
        console.info('[bluetooth_js] readCharacteristicValue start');
        await tryToEnableBt();
        let promise = new Promise((resolve) => {
            let descriptors = [];
            let arrayBuffer = new ArrayBuffer(8);
            let desValue =  new Uint8Array(arrayBuffer);
            desValue[0] = 11;
            let descriptor = {serviceUuid: '00001810-0000-1000-8000-00805F9B34FB',
                descriptorUuid: '00001830-0000-1000-8000-00805F9B34FB', descriptorValue: arrayBuffer};
            descriptors[0] = descriptor;
            let arrayBufferCCC = new ArrayBuffer(8);
            let cccValue = new Uint8Array(arrayBufferCCC);
            cccValue[0] = 32;
            let characteristic = {serviceUuid: '00001810-0000-1000-8000-00805F9B34FB',
                characteristicUuid: '00001820-0000-1000-8000-00805F9B34FB',
                characteristicValue: arrayBufferCCC, descriptors:descriptors};
            let gattClient = bluetooth.BLE.createGattClientDevice("00:00:00:00:00:00");
            gattClient.readCharacteristicValue(characteristic).then((object) => {
                if (object != null) {
                    expect(true).assertEqual(true);
                } else {
                    console.info('[bluetooth_js] readCharacValue data:' + JSON.stringify(data));
                    expect(null).assertFail();
                }
                done();
            }).catch(err => {
                console.error(`bluetooth readCharacteristicValue has error: ${err}`);
                expect(true).assertEqual(true);
                done();
            });
            resolve()
        })
        await promise.then(done)
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GATT_READ_DESCRI_VALUE_0001
     * @tc.name testReadDescriptorValue
     * @tc.desc Test ReadDescriptorValue api by promise.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GATT_READ_DESCRI_VALUE_0001', 0, async function (done) {
        console.info('[bluetooth_js] readDescriptorValue start');
        await tryToEnableBt();
        let promise = new Promise((resolve) => {
            let arrayBuffer = new ArrayBuffer(8);
            let desValue =  new Uint8Array(arrayBuffer);
            desValue[0] = 11;
            let descriptor = {serviceUuid: '00001810-0000-1000-8000-00805F9B34FB',
                descriptorUuid: '00001830-0000-1000-8000-00805F9B34FB', descriptorValue: arrayBuffer};
            let gattClient = bluetooth.BLE.createGattClientDevice("00:00:00:00:00:00");
            gattClient.readDescriptorValue(descriptor).then((object) => {
                if (object != null) {
                    expect(true).assertEqual(true);
                } else {
                    console.info('[bluetooth_js] BLEDescriptor data:' + JSON.stringify(object));
                    expect(null).assertFail();
                }
                done();
            }).catch(err => {
                console.error(`bluetooth readDescriptorValue has error: ${err}`);
                expect(true).assertEqual(true);
                done();
            });
            resolve()
        })
        await promise.then(done)
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GATT_WRITE_CHARACT_VALUE_0001
     * @tc.name testWriteCharacteristicValue
     * @tc.desc Test WriteCharacteristicValue api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GATT_WRITE_CHARACT_VALUE_0001', 0, async function (done) {
        console.info('[bluetooth_js] writeCharacteristicValue start');
        await tryToEnableBt();
        let promise = new Promise((resolve) => {
            let descriptors = [];
            let arrayBuffer = new ArrayBuffer(8);
            let desValue =  new Uint8Array(arrayBuffer);
            desValue[0] = 11;
            let descriptor = {serviceUuid: '00001810-0000-1000-8000-00805F9B34FB',
                descriptorUuid: '00001830-0000-1000-8000-00805F9B34FB', descriptorValue: arrayBuffer};
            descriptors[0] = descriptor;
            let arrayBufferCCC = new ArrayBuffer(8);
            let cccValue = new Uint8Array(arrayBufferCCC);
            cccValue[0] = 32;
            let characteristic = {serviceUuid: '00001810-0000-1000-8000-00805F9B34FB',
                characteristicUuid: '00001820-0000-1000-8000-00805F9B34FB',
                characteristicValue: arrayBufferCCC, descriptors:descriptors};
            let gattClient = bluetooth.BLE.createGattClientDevice("00:00:00:00:00:00");
            let ret = gattClient.writeCharacteristicValue(characteristic);
            console.info('[bluetooth_js] writeCharacteristicValue ret : ' + ret);
            expect(ret).assertEqual(false);
            resolve()
        })
        await promise.then(done)
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GATT_WRITE_DESC_VALUE_0001
     * @tc.name testWriteDescriptorValue
     * @tc.desc Test WriteDescriptorValue api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GATT_WRITE_DESC_VALUE_0001', 0, async function (done) {
        console.info('[bluetooth_js] writeDescriptorValue start');
        await tryToEnableBt();
        let promise = new Promise((resolve) => {
            let arrayBuffer = new ArrayBuffer(8);
            let desValue =  new Uint8Array(arrayBuffer);
            desValue[0] = 11;
            let descriptor = {serviceUuid: '00001810-0000-1000-8000-00805F9B34FB',
                descriptorUuid: '00001830-0000-1000-8000-00805F9B34FB', descriptorValue: arrayBuffer};
            let gattClient = bluetooth.BLE.createGattClientDevice("00:00:00:00:00:00");
            let ret = gattClient.writeDescriptorValue(descriptor);
            console.info('[bluetooth_js] bluetooth writeDescriptorValue ret : ' + ret);
            expect(ret).assertEqual(false);
            resolve()
        })
        await promise.then(done)
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GATT_SET_BLE_MTUSIZE_0001
     * @tc.name testSetBLEMtuSize
     * @tc.desc Test SetBLEMtuSize api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GATT_SET_BLE_MTUSIZE_0001', 0, async function (done) {
        console.info('[bluetooth_js] setBLEMtuSize start');
        await tryToEnableBt();
        let gattClient = bluetooth.BLE.createGattClientDevice("00:00:00:00:00:00");
        let ret = gattClient.setBLEMtuSize(128);
        console.info('[bluetooth_js] bluetooth setBLEMtuSize ret:' + ret);
        expect(ret).assertEqual(false);
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GATT_SET_NOTIFY_CHARA_CHANGED_0001
     * @tc.name testSetNotifyCharacteristicChanged
     * @tc.desc Test SetNotifyCharacteristicChanged api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GATT_SET_NOTIFY_CHARA_CHANGED_0001', 0, async function (done) {
        console.info('[bluetooth_js] setNotifyCharacteristicChanged start');
        await tryToEnableBt();
        let gattClient = bluetooth.BLE.createGattClientDevice("00:00:00:00:00:00");
        let descriptors = [];
        let arrayBuffer = new ArrayBuffer(8);
        let desValue =  new Uint8Array(arrayBuffer);
        desValue[0] = 11;
        let arrayBufferNotify = new ArrayBuffer(8);
        let descNotifyValue = new Uint8Array(arrayBufferNotify);
        descNotifyValue[0] = 1
        let descriptor = {serviceUuid: '00001810-0000-1000-8000-00805F9B34FB',
            descriptorUuid: '00001830-0000-1000-8000-00805F9B34FB',
            descriptorValue: arrayBuffer};
        let descriptorNotify = {serviceUuid: '00001810-0000-1000-8000-00805F9B34FB',
            descriptorUuid: '00002902-0000-1000-8000-00805F9B34FB',
            descriptorValue: arrayBufferNotify};
        descriptors[0] = descriptor;
        descriptors[1] = descriptorNotify;
        let arrayBufferCCC = new ArrayBuffer(8);
        let cccValue = new Uint8Array(arrayBufferCCC);
        cccValue[0] = 1;
        let characteristic = {serviceUuid: '00001810-0000-1000-8000-00805F9B34FB',
            characteristicUuid: '00001820-0000-1000-8000-00805F9B34FB',
            characteristicValue: arrayBufferCCC, descriptors:descriptors};
        let ret = gattClient.setNotifyCharacteristicChanged(characteristic, false);
        console.info('[bluetooth_js] setNotifyCharacteristicChanged ret:' + ret);
        expect(ret).assertEqual(false);
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GATT_ADD_SERVICE_0001
     * @tc.name testAddService
     * @tc.desc Test AddService api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GATT_ADD_SERVICE_0001', 0, async function (done) {
        console.info('[bluetooth_js] addService start');
        await tryToEnableBt();
        let gattServer = bluetooth.BLE.createGattServer();
        let descriptors = [];
        let arrayBuffer = new ArrayBuffer(8);
        let desValue =  new Uint8Array(arrayBuffer);
        desValue[0] = 11;
        let descriptor = {serviceUuid: '00001810-0000-1000-8000-00805F9B34FB',
            characteristicUuid: '00001820-0000-1000-8000-00805F9B34FB',
            descriptorUuid: '00001830-0000-1000-8000-00805F9B34FB',
            descriptorValue: arrayBuffer};
        descriptors[0] = descriptor;
        let characteristics = [];
        let arrayBufferCCC = new ArrayBuffer(8);
        let cccValue = new Uint8Array(arrayBufferCCC);
        cccValue[0] = 1;
        let characteristic = {serviceUuid: '00001810-0000-1000-8000-00805F9B34FB',
            characteristicUuid: '00001820-0000-1000-8000-00805F9B34FB',
            characteristicValue: arrayBufferCCC, descriptors:descriptors};
        characteristics[0] = characteristic;
        let service = {serviceUuid: '00001810-0000-1000-8000-00805F9B34FB',
            isPrimary: true, characteristics: characteristics, includeServices: []};
        let ret = gattServer.addService(service);
        console.info('[bluetooth_js] bluetooth addService ret : ' + ret);
        expect(ret).assertEqual(false);
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GATT_REMOVE_SERVICE_0001
     * @tc.name testRemoveService
     * @tc.desc Test RemoveService api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GATT_REMOVE_SERVICE_0001', 0, async function (done) {
        console.info('[bluetooth_js] removeService start');
        await tryToEnableBt();
        let gattServer = bluetooth.BLE.createGattServer();
        let ret = gattServer.removeService('00001810-0000-1000-8000-008000000000');
        console.info('[bluetooth_js] removeService ret : ' + ret);
        expect(ret).assertEqual(false);
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GATT_NOTIFY_CHARA_CHANGED_0001
     * @tc.name testNotifyCharacteristicChanged
     * @tc.desc Test NotifyCharacteristicChanged api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GATT_NOTIFY_CHARA_CHANGED_0001', 0, async function (done) {
        console.info('[bluetooth_js] notifyCharacteristicChanged start');
        await tryToEnableBt();
        let gattServer = bluetooth.BLE.createGattServer()
        let descriptors = [];
        let arrayBufferDesc = new ArrayBuffer(8);
        let desValue =  new Uint8Array(arrayBufferDesc);
        desValue[0] = 11;
        let arrayBufferNotifyDesc = new ArrayBuffer(8);
        let descNotifyValue = new Uint8Array(arrayBufferNotifyDesc);
        descNotifyValue[0] = 1
        let
            descriptor = {serviceUuid: '00001810-0000-1000-8000-00805F9B34FB',
                descriptorUuid: '00001830-0000-1000-8000-00805F9B34FB', descriptorValue: desValue};
        let descriptorNotify = {serviceUuid: '00001810-0000-1000-8000-00805F9B34FB',
            descriptorUuid: '00002902-0000-1000-8000-00805F9B34FB',
            descriptorValue: descNotifyValue};
        descriptors[0] = descriptor;
        descriptors[1] = descriptorNotify;
        let arrayBufferCCC = new ArrayBuffer(8);
        let cccValue = new Uint8Array(arrayBufferCCC);
        cccValue[0] = 1;
        let characteristic = {serviceUuid: '00001810-0000-1000-8000-00805F9B34FB',
            characteristicUuid: '00001820-0000-1000-8000-00805F9B34FB',
            characteristicValue: cccValue,
            descriptors:descriptors,confirm:'false'};
        let ret = gattServer.notifyCharacteristicChanged('00:00:00:00:00:00', characteristic);
        console.info('[bluetooth_js] notifyCharacteristicChanged ret : ' + ret);
        expect(ret).assertEqual(false);
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GATT_SEND_RESPONSE_0001
     * @tc.name testSendResponse
     * @tc.desc Test SendResponse api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GATT_SEND_RESPONSE_0001', 0, async function (done) {
        console.info('[bluetooth_js] sendResponse start');
        await tryToEnableBt();
        let gattServer = bluetooth.BLE.createGattServer()
        let arrayBuffer = new ArrayBuffer(8);
        let value =  new Uint8Array(arrayBuffer);
        value[0] = 1;
        let serverResponse = {deviceId: '00:00:00:00:00', transId: 1,
            status: 0, offset: 0, value: arrayBuffer};
        let ret = gattServer.sendResponse(serverResponse);
        console.info('[bluetooth_js] sendResponse ret : ' + ret);

        expect(ret).assertEqual(false);
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetoothble_GATTSERVER_CLOSE_0001
     * @tc.name testBlePeripheralManagerClose
     * @tc.desc Test BlePeripheralManagerClose api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetoothble_GATTSERVER_CLOSE_0001', 0, async function (done) {
        console.info('[bluetooth_js] GattserverClose test start ...');
        await tryToEnableBt();
        try{
            let gattServer = bluetooth.BLE.createGattServer()
            let result = gattServer.close();
            console.info("[bluetooth_js] GattserverClose res:"+ JSON.stringify(result));
            expect(result).assertTrue();
        }catch(error){
            console.info("[bluetooth_js] GattserverClose err:" + JSON.stringify(error));
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GATT_DISCONNRCT_0001
     * @tc.name testDisConnect
     * @tc.desc Test DisConnect api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GATT_DISCONNRCT_0001', 0, async function (done) {
        console.info('[bluetooth_js] gatt disconnect start');
        await tryToEnableBt();
        let gattClient = bluetooth.BLE.createGattClientDevice("00:00:00:00:00:00");
        let ret = gattClient.disconnect();
        console.info('[bluetooth_js] gatt disconnect ret:' + ret);
        expect(ret).assertEqual(false);
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GATT_CLIENT_CLOSE_0001
     * @tc.name testGattClientClose
     * @tc.desc Test GattClientClose api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GATT_CLIENT_CLOSE_0001', 0, async function (done) {
        console.info('[bluetooth_js] GattclientClose test start ...');
        await tryToEnableBt();
        try{
            let gattClient = bluetooth.BLE.createGattClientDevice("00:00:00:00:00:00");
            let result = gattClient.close();
            console.info("[bluetooth_js] GattclientClose result:" + JSON.stringify(result));
            expect(result).assertTrue();
        }catch(error){
            console.info("[bluetooth_js] GattclientClose err:" + JSON.stringify(error));
            expect(null).assertFail();
        }
        done();
    })

})
