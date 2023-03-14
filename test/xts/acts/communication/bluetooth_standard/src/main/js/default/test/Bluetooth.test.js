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


let Btname = {
    NUM_TEST :'012345678901234567890123456789012345678901234567890123'+
    '45678901234567890123456789012345678901234567890123456789012345678901234567',
    NUM_TEST1 :'01234567890123456789012345678901234567890123456'
    +'7890123456789012345678901234567890123456789012345678901234567890123456789012345678',
    LETTERS_TEST :'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa',
    CHINESES_TEST :'测试蓝牙名称是否正常测试蓝牙名称是否试蓝牙',
    SYMBOL_TEST:'*^_^* 、。·ˉˇ¨〃々—～‖·‘’“”「『』〖❂【±×'
    +'÷∶∧∨∑∏∪∩∈∷√⊥‖∠⌒⊙∫∮≡≌≈∽∝≠♂♀°℃＄¤￠￡‰§№☆★○●◎◇□■△※→←↑↓〓',
    MIXES:'测试蓝牙名称是否正试蓝牙\'名称是否[666]aaw',
    MIXES2:'——◎◇◆□■△▲测试蓝牙',
    MIXES3:'78453-、、。。◎◇◆□■△▲',
    MIXES4:'hhhfdf-、、。。◎◇◆□■△▲',
    MIXES5:'#01-5@<?;:!@$%^&*(1[]{【+-；：‘’“”测试=》《\'[6]',
    MIXES6:'#01-567890@<>?/;:5675ASDF012345678!@'
    +'$%^&*()9012378901[]{}【】566~·67890blue',
    MIXES7:'0123456789012345678901234567890123456789012345678901'
    +'23456789012345678901234567890123456789012345678901234567890123456789012345678012'
    +'345678901234567890123456789012345678901234567890123456789012367890123456789012345568'
}

describe('bluetoothhostTest', function() {

    beforeAll(function () {
        console.info('beforeAll called')
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
     * @tc.number SUB_COMMUNACATION_bluetooth_GET_STATE_0001
     * @tc.name testEnableBluetooth
     * @tc.desc Test EnableBluetooth api by promise.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GET_STATE_0001', 0, async function (done) {
        console.info('[bluetooth_js] getState start');
        await tryToEnableBt();
        let state = bluetooth.getState();
        console.info('[bluetooth_js] get bluetooth state result = '+ JSON.stringify(state));
        expect(state).assertEqual(bluetooth.BluetoothState.STATE_ON);
        expect(bluetooth.BluetoothState.STATE_OFF != state).assertTrue();
        expect(bluetooth.BluetoothState.STATE_TURNING_ON != state).assertTrue();
        expect(bluetooth.BluetoothState.STATE_TURNING_OFF != state).assertTrue();
        expect(bluetooth.BluetoothState.STATE_BLE_TURNING_ON != state).assertTrue();
        expect(bluetooth.BluetoothState.STATE_BLE_ON != state).assertTrue();
        expect(bluetooth.BluetoothState.STATE_BLE_TURNING_OFF != state).assertTrue();
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SET_LOCALNAME_0001
     * @tc.name testClassicSetLocalName
     * @tc.desc Test ClassicSetLocalName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SET_LOCALNAME_0001', 0, async function (done) {
        console.info('[bluetooth_js] set localname start');
        await tryToEnableBt();
        let name = bluetooth.setLocalName('bluetooth_test');
        console.info('[bluetooth_js] appInfoTest enable bluetooth result = '+ JSON.stringify(name));
        expect(name).assertEqual(true);
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_START_PAIR_0001
     * @tc.name testStartpair
     * @tc.desc Test Startpair api by promise.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_START_PAIR_0001', 0, async function (done) {
        console.info('[bluetooth_js] Startpair test start ...');
        await tryToEnableBt();
        let result = bluetooth.pairDevice("00:00:00:00:00:00");
        console.info("[bluetooth_js] onStartpair -> " + JSON.stringify(result));
        expect(result).assertFalse();
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GET_LOCALNAME_0001
     * @tc.name testClassicGetLocalName
     * @tc.desc Test ClassicGetLocalName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GET_LOCALNAME_0001', 0, async function (done) {
        console.info('[bluetooth_js] get localName start');
        await tryToEnableBt();
        let localName = bluetooth.getLocalName();
        console.info('[bluetooth_js] get local Name result = ' + JSON.stringify(localName));
        expect(localName).assertEqual('bluetooth_test');
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SET_BLUETOOTH_SCAN_MODE_0001
     * @tc.name testClassicSetBluetoothScanMode
     * @tc.desc Test ClassicSetBluetoothScanMode api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SET_BLUETOOTH_SCAN_MODE_0001', 0, async function (done) {
        console.info('[bluetooth_js] set scan mode start');
        await tryToEnableBt();
        let result = bluetooth.setBluetoothScanMode(1,10000);
        expect(true).assertTrue(JSON.stringify(bluetooth.ScanDuty.SCAN_MODE_LOW_POWER) !=result );
        expect(true).assertTrue(JSON.stringify(bluetooth.ScanDuty.SCAN_MODE_BALANCED) != result);
        expect(true).assertTrue(JSON.stringify(bluetooth.ScanDuty.SCAN_MODE_BALANCED) != result );
        expect(result).assertEqual(true);
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GET_BLUETOOTH_SCAN_MODE_0001
     * @tc.name testClassicGetBluetoothScanMode
     * @tc.desc Test ClassicGetBluetoothScanMode api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GET_BLUETOOTH_SCAN_MODE_0001', 0, async function (done) {
        console.info('[bluetooth_js] get scan mode start');
        await tryToEnableBt();
        let result = bluetooth.setBluetoothScanMode(1,10000);
        expect(result).assertEqual(true);
        let scanMode = bluetooth.getBluetoothScanMode();
        console.info('[bluetooth_js] get scan mode result2 = ' + JSON.stringify(scanMode));
        expect(scanMode).assertEqual(1);
        expect(true).assertTrue(bluetooth.ScanMode.SCAN_MODE_NONE != scanMode);
        expect(true).assertTrue(bluetooth.ScanMode.SCAN_MODE_CONNECTABLE != scanMode);
        expect(true).assertTrue(bluetooth.ScanMode.SCAN_MODE_GENERAL_DISCOVERABLE != scanMode);
        expect(true).assertTrue(bluetooth.ScanMode.SCAN_MODE_LIMITED_DISCOVERABLE != scanMode);
        expect(true).assertTrue(bluetooth.ScanMode.SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE != scanMode);
        expect(true).assertTrue(bluetooth.ScanMode.SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE != scanMode);
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GET_BT_CONNECT_STATE_0001
     * @tc.name testClassicGetBtConnectionState
     * @tc.desc Test ClassicGetBtConnectionState api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GET_BT_CONNECT_STATE_0001', 0, async function (done) {
        console.info('[bluetooth_js] get connection state start');
        await tryToEnableBt();
        let connState = bluetooth.getBtConnectionState();
        console.info('[bluetooth_js] get bt connection state result' + JSON.stringify(connState));
        expect(connState).assertEqual(bluetooth.ProfileConnectionState.STATE_DISCONNECTED);
        expect(true).assertTrue(bluetooth.ProfileConnectionState.STATE_CONNECTING!= connState );
        expect(true).assertTrue(bluetooth.ProfileConnectionState.STATE_CONNECTED!= connState );
        expect(true).assertTrue(bluetooth.ProfileConnectionState.STATE_DISCONNECTING!= connState );
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_START_BLUETOOTH_DISCOVERY_0001
     * @tc.name testClassicStartBluetoothDiscovery
     * @tc.desc Test ClassicStartBluetoothDiscovery api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_START_BLUETOOTH_DISCOVERY_0001', 0, async function (done) {
        console.info('[bluetooth_js] discovery start');
        await tryToEnableBt();
        let result = bluetooth.startBluetoothDiscovery();
        console.info('[bluetooth_js] get bt connection state result' + JSON.stringify(result));
        expect(result).assertEqual(true);
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_STOP_BLUETOOTH_DISCOVERY_0001
     * @tc.name testClassicStopBluetoothDiscovery
     * @tc.desc Test ClassicStopBluetoothDiscovery api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_STOP_BLUETOOTH_DISCOVERY_0001', 0, async function (done) {
        console.info('[bluetooth_js] stop discovery start');
        await tryToEnableBt();
        let result = bluetooth.stopBluetoothDiscovery();
        console.info("stopBluetoothDiscovery :" + JSON.stringify(result));
        expect(result).assertTrue();
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GET_PAIRED_DEVICES_0001
     * @tc.name testClassicGetPairedDevices
     * @tc.desc Test ClassicGetPairedDevices api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GET_PAIRED_DEVICES_0001', 0, async function (done) {
        console.info('[bluetooth_js] get paired devices start');
        await tryToEnableBt();
        let devices = bluetooth.getPairedDevices();
        console.info('[bluetooth_js] get paired devices:' + JSON.stringify(devices) +'length:' + devices.length);
        expect(devices.length).assertEqual(0);
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SPP_LISTEN_0001
     * @tc.name testSppListen
     * @tc.desc Test SppListen api by callback.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SPP_LISTEN_0001', 0, async function (done) {
        console.log("[bluetooth_js]: spp listen start");
        await tryToEnableBt();
        let sppOption = {uuid: '00001810-0000-1000-8000-00805F9B34FB',
            secure: false, type: 0};
        bluetooth.sppListen('server1', sppOption, function(code, serverSocketNumber) {
            console.info('[bluetooth_js] code is: ' + code.code);
            if (code.code == 0) {
                expect(true).assertEqual(true);
                done();
            } else {
                expect(true).assertEqual(false);
                done();
            }
        });
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SPP_ACCEPT_0001
     * @tc.name testSppAccept
     * @tc.desc Test SppAccept api by callbck.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SPP_ACCEPT_0001', 0, async function (done) {
        console.log("[bluetooth_js] spp accept start");
        await tryToEnableBt();
        bluetooth.sppAccept(-1, function(code, clientSocketNumber) {
            console.info('[bluetooth_js] code is: ' + code.code);
            if (code.code == 0) {
                expect(true).assertEqual(true);
            } else {
                expect(true).assertEqual(false);
            }
        });
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SPP_CLOSE_SERVER_SOCKET_0001
     * @tc.name testSppCloseServerSocket
     * @tc.desc Test SppCloseServerSocket api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SPP_CLOSE_SERVER_SOCKET_0001', 0, async function (done) {
        console.log("[bluetooth_js] spp closeServerSocket start");
        await tryToEnableBt();
        let ret = bluetooth.sppCloseServerSocket(-1);
        console.info('[bluetooth_js] bluetooth sppCloseServerSocket ret : ' + ret);
        expect(ret).assertEqual(false);
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SPP_CONNECT_0001
     * @tc.name testSppConnect
     * @tc.desc Test SppConnect api by callback.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SPP_CONNECT_0001', 0, async function (done) {
        console.info('[bluetooth_js] spp_connect start');
        await tryToEnableBt();
        let sppOption = {uuid: '00001810-0000-1000-8000-00805F9B34FB',
            secure: true, type: 0};
        bluetooth.sppConnect(55, sppOption, function(code, clientSocketNumber) {
            console.info('[bluetooth_js] code is: ' + code.code);
            if (code.code == 0) {
                expect(true).assertEqual(true);
            } else {
                expect(true).assertEqual(false);
            }
        });
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SPP_WRITE_0001
     * @tc.name testSppWrite
     * @tc.desc Test SppWrite api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SPP_WRITE_0001', 0, async function (done) {
        console.info('[bluetooth_js] spp write start');
        await tryToEnableBt();
        let arrayBuffer = new ArrayBuffer(8);
        let data =  new Uint8Array(arrayBuffer);
        data[0] = 123;
        let ret = bluetooth.sppWrite(-1, arrayBuffer);
        console.info('[bluetooth_js] bluetooth sppWrite ret : ' + ret);
        expect(ret).assertEqual(false);
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GET_REMO_DEV_NAME_0001
     * @tc.name testGetRemoteDeviceName
     * @tc.desc Test GetRemoteDeviceName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GET_REMO_DEV_NAME_0001', 0, async function (done) {
        console.info('[bluetooth_js] getRemoteDeviceName start');
        await tryToEnableBt();
        let ret = bluetooth.getRemoteDeviceName("00:00:00:00:00:00");
        console.info('[bluetooth_js] getRemoteDeviceName ret2 : ' + JSON.stringify(ret));
        expect(ret.length).assertEqual(0);
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_GET_REMO_DEV_CLASS_0001
     * @tc.name testGetRemoteDeviceClass
     * @tc.desc Test GetRemoteDeviceClass api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_GET_REMO_DEV_CLASS_0001', 0, async function (done) {
        console.info('[bluetooth_js] getRemoteDeviceClass start');
        await tryToEnableBt();
        let ret = bluetooth.getRemoteDeviceClass("00:00:00:00:00:00");
        console.info('[bluetooth_js] getRemoteDeviceClass ret2 :' + JSON.stringify(ret) + 'majorClass:' +
        ret.majorClass + 'majorMinorClass:'+ ret.majorMinorClass + 'classOfDevice:' + ret.classOfDevice);
        expect(ret.majorClass).assertEqual(0);
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_CANCEL_PAIRED_DEV_0001
     * @tc.name testCancelPairedDevice
     * @tc.desc Test CancelPairedDevice api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_CANCEL_PAIRED_DEV_0001', 0, async function (done) {
        console.info('[bluetooth_js] cancelPairedDevice start');
        await tryToEnableBt();
        let ret = bluetooth.cancelPairedDevice("00:00:00:00:00:00");
        console.info('[bluetooth_js] cancelPairedDevice ret2 : ' + JSON.stringify(ret));
        expect(ret).assertFalse();
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0001
     * @tc.name testsetLocalName
     * @tc.desc Test setLocalName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0001', 0, async function (done) {
        await tryToEnableBt();
        let state = bluetooth.getState();
        expect(true).assertTrue(JSON.stringify(bluetooth.BluetoothState.STATE_ON)==state);
        let name = bluetooth.setLocalName('');
        expect(name).assertFalse();
        let localName = bluetooth.getLocalName();
        expect(true).assertTrue(localName==name);
        console.info('[bluetooth_js] getLocalName result1 = ' + JSON.stringify(localName));
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0002
     * @tc.name testsetLocalName
     * @tc.desc Test setLocalName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0002', 0, async function (done) {
        await tryToEnableBt();
        let localName = bluetooth.getLocalName();
        let name = bluetooth.setLocalName();
        console.info('[bluetooth_js] getName result2 = '+ JSON.stringify(localName) +
        'setName result2 =' + JSON.stringify(name));
        expect(name).assertNull();
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0003
     * @tc.name testsetLocalName
     * @tc.desc Test setLocalName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0003', 0, async function (done) {
        await tryToEnableBt();
        let name = bluetooth.setLocalName(Btname.NUM_TEST);
        expect(name).assertTrue();
        let localName = bluetooth.getLocalName();
        expect(true).assertTrue(Btname.NUM_TEST == JSON.stringify(localName));
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0004
     * @tc.name testsetLocalName
     * @tc.desc Test setLocalName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0004', 0, async function (done) {
        await tryToEnableBt();
        let name = bluetooth.setLocalName(Btname.NUM_TEST1);
        expect(name).assertTrue();
        let localName = bluetooth.getLocalName();
        expect(true).assertTrue(Btname.NUM_TEST1 == JSON.stringify(localName));
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0005
     * @tc.name testsetLocalName
     * @tc.desc Test setLocalName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0005', 0, async function (done) {
        await tryToEnableBt();
        let name = bluetooth.setLocalName(Btname.LETTERS_TEST);
        expect(name).assertTrue();
        let localName = bluetooth.getLocalName();
        expect(true).assertTrue(Btname.LETTERS_TEST == JSON.stringify(localName));
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0006
     * @tc.name testsetLocalName
     * @tc.desc Test setLocalName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0006', 0, async function (done) {
        await tryToEnableBt();
        let name = bluetooth.setLocalName(Btname.CHINESES_TEST);
        expect(name).assertTrue();
        let localName = bluetooth.getLocalName();
        expect(true).assertTrue(Btname.CHINESES_TEST == JSON.stringify(localName));
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0007
     * @tc.name testsetLocalName
     * @tc.desc Test setLocalName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0007', 0, async function (done) {
        await tryToEnableBt();
        let name = bluetooth.setLocalName(Btname.SYMBOL_TEST);
        console.info('[bluetooth_js] setLocalName result7 = '+ JSON.stringify(name));
        expect(name).assertTrue();
        let localName = bluetooth.getLocalName();
        expect(true).assertTrue(Btname.SYMBOL_TEST == JSON.stringify(localName));
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0008
     * @tc.name testsetLocalName
     * @tc.desc Test setLocalName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0008', 0, async function (done) {
        await tryToEnableBt();
        let name = bluetooth.setLocalName(Btname.MIXES);
        console.info('[bluetooth_js] setLocalName result8 = '+ JSON.stringify(name));
        expect(name).assertTrue();
        let localName = bluetooth.getLocalName();
        expect(true).assertTrue(Btname.MIXES == JSON.stringify(localName));
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0009
     * @tc.name testsetLocalName
     * @tc.desc Test setLocalName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_0009', 0, async function (done) {
        await tryToEnableBt();
        let name = bluetooth.setLocalName(Btname.MIXES2);
        console.info('[bluetooth_js] setLocalName result9 = '+ JSON.stringify(name));
        expect(name).assertTrue();
        let localName = bluetooth.getLocalName();
        expect(true).assertTrue(Btname.MIXES2 == JSON.stringify(localName));
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_00010
     * @tc.name testsetLocalName
     * @tc.desc Test setLocalName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_00010', 0, async function (done) {
        await tryToEnableBt();
        let name = bluetooth.setLocalName(Btname.MIXES3);
        console.info('[bluetooth_js] setLocalName result10 = '+ JSON.stringify(name));
        expect(name).assertTrue();
        let localName = bluetooth.getLocalName();
        expect(true).assertTrue(Btname.MIXES3 == JSON.stringify(localName));
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_00011
     * @tc.name testsetLocalName
     * @tc.desc Test setLocalName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_00011', 0, async function (done) {
        await tryToEnableBt();
        let name = bluetooth.setLocalName(Btname.MIXES4);
        console.info('[bluetooth_js] setLocalName result11 = '+ JSON.stringify(name));
        expect(name).assertTrue();
        let localName = bluetooth.getLocalName();
        expect(true).assertTrue(Btname.MIXES4 == JSON.stringify(localName));
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_00012
     * @tc.name testsetLocalName
     * @tc.desc Test setLocalName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_00012', 0, async function (done) {
        await tryToEnableBt();
        let name = bluetooth.setLocalName(Btname.MIXES5);
        console.info('[bluetooth_js] setLocalName result12 = '+ JSON.stringify(name));
        expect(name).assertTrue();
        let localName = bluetooth.getLocalName();
        expect(true).assertTrue(Btname.MIXES5 == JSON.stringify(localName));
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_00013
     * @tc.name testsetLocalName
     * @tc.desc Test setLocalName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_00013', 0, async function (done) {
        await tryToEnableBt();
        let name = bluetooth.setLocalName(Btname.MIXES6);
        console.info('[bluetooth_js] setLocalName result13 = '+ JSON.stringify(name));
        expect(name).assertTrue();
        let localName = bluetooth.getLocalName();
        expect(true).assertTrue(Btname.MIXES6 == JSON.stringify(localName));
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_00014
     * @tc.name testsetLocalName
     * @tc.desc Test setLocalName api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_SET_LOCAL_NAME_00014', 0, async function (done) {
        await tryToEnableBt();
        let name = bluetooth.setLocalName(Btname.MIXES7);
        console.info('[bluetooth_js] setLocalName result14 = '+ JSON.stringify(name));
        expect(name).assertTrue();
        let localName = bluetooth.getLocalName();
        expect(true).assertTrue(Btname.MIXES7 == JSON.stringify(localName));
        done();
    })



    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_DISABLE_0001
     * @tc.name testdisablebluetooth
     * @tc.desc Test disablebluetooth api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_DISABLE_0001', 0, async function (done) {
        await tryToEnableBt();
        console.info('[bluetooth_js] disable test start');
        let disable = bluetooth.disableBluetooth();
        console.info('[bluetooth_js] disable:' + JSON.stringify(disable));
        expect(disable).assertTrue();
        let state = bluetooth.getState();
        console.info('[bluetooth_js] bt state:' + JSON.stringify(state));
        done();
    })

})
