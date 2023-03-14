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

let MajorMinorClass = {
    COMPUTER_UNCATEGORIZED : 0x0100,
    COMPUTER_DESKTOP : 0x0104,
    COMPUTER_SERVER : 0x0108,
    COMPUTER_LAPTOP : 0x010C,
    COMPUTER_HANDHELD_PC_PDA : 0x0110,
    COMPUTER_PALM_SIZE_PC_PDA : 0x0114,
    COMPUTER_WEARABLE : 0x0118,
    COMPUTER_TABLET : 0x011C,
    PHONE_UNCATEGORIZED : 0x0200,
    PHONE_CELLULAR : 0x0204,
    PHONE_CORDLESS : 0x0208,
    PHONE_SMART : 0x020C,
    PHONE_MODEM_OR_GATEWAY : 0x0210,
    PHONE_ISDN : 0x0214,
    NETWORK_FULLY_AVAILABLE : 0x0300,
    NETWORK_1_TO_17_UTILIZED : 0x0320,
    NETWORK_17_TO_33_UTILIZED : 0x0340,
    NETWORK_33_TO_50_UTILIZED : 0x0360,
    NETWORK_60_TO_67_UTILIZED : 0x0380,
    NETWORK_67_TO_83_UTILIZED : 0x03A0,
    NETWORK_83_TO_99_UTILIZED : 0x03C0,
    NETWORK_NO_SERVICE : 0x03E0,
    AUDIO_VIDEO_UNCATEGORIZED : 0x0400,
    AUDIO_VIDEO_WEARABLE_HEADSET : 0x0404,
    AUDIO_VIDEO_HANDSFREE : 0x0408,
    AUDIO_VIDEO_MICROPHONE : 0x0410,
    AUDIO_VIDEO_LOUDSPEAKER : 0x0414,
    AUDIO_VIDEO_HEADPHONES : 0x0418,
    AUDIO_VIDEO_PORTABLE_AUDIO : 0x041C,
    AUDIO_VIDEO_CAR_AUDIO : 0x0420,
    AUDIO_VIDEO_SET_TOP_BOX : 0x0424,
    AUDIO_VIDEO_HIFI_AUDIO : 0x0428,
    AUDIO_VIDEO_VCR : 0x042C,
    AUDIO_VIDEO_VIDEO_CAMERA : 0x0430,
    AUDIO_VIDEO_CAMCORDER : 0x0434,
    AUDIO_VIDEO_VIDEO_MONITOR : 0x0438,
    AUDIO_VIDEO_VIDEO_DISPLAY_AND_LOUDSPEAKER : 0x043C,
    AUDIO_VIDEO_VIDEO_CONFERENCING : 0x0440,
    AUDIO_VIDEO_VIDEO_GAMING_TOY : 0x0448,
    PERIPHERAL_NON_KEYBOARD_NON_POINTING : 0x0500,
    PERIPHERAL_KEYBOARD : 0x0540,
    PERIPHERAL_POINTING_DEVICE : 0x0580,
    PERIPHERAL_KEYBOARD_POINTING : 0x05C0,
    PERIPHERAL_UNCATEGORIZED : 0x0500,
    PERIPHERAL_JOYSTICK : 0x0504,
    PERIPHERAL_GAMEPAD : 0x0508,
    PERIPHERAL_REMOTE_CONTROL : 0x05C0,
    PERIPHERAL_SENSING_DEVICE : 0x0510,
    PERIPHERAL_DIGITIZER_TABLET : 0x0514,
    PERIPHERAL_CARD_READER : 0x0518,
    PERIPHERAL_DIGITAL_PEN : 0x051C,
    PERIPHERAL_SCANNER_RFID : 0x0520,
    PERIPHERAL_GESTURAL_INPUT : 0x0522,
    IMAGING_UNCATEGORIZED : 0x0600,
    IMAGING_DISPLAY : 0x0610,
    IMAGING_CAMERA : 0x0620,
    IMAGING_SCANNER : 0x0640,
    IMAGING_PRINTER : 0x0680,
    WEARABLE_UNCATEGORIZED : 0x0700,
    WEARABLE_WRIST_WATCH : 0x0704,
    WEARABLE_PAGER : 0x0708,
    WEARABLE_JACKET : 0x070C,
    WEARABLE_HELMET : 0x0710,
    WEARABLE_GLASSES : 0x0714,
    TOY_UNCATEGORIZED : 0x0800,
    TOY_ROBOT : 0x0804,
    TOY_VEHICLE : 0x0808,
    TOY_DOLL_ACTION_FIGURE : 0x080C,
    TOY_CONTROLLER : 0x0810,
    TOY_GAME : 0x0814,
    HEALTH_UNCATEGORIZED : 0x0900,
    HEALTH_BLOOD_PRESSURE : 0x0904,
    HEALTH_THERMOMETER : 0x0908,
    HEALTH_WEIGHING : 0x090C,
    HEALTH_GLUCOSE : 0x0910,
    HEALTH_PULSE_OXIMETER : 0x0914,
    HEALTH_PULSE_RATE : 0x0918,
    HEALTH_DATA_DISPLAY : 0x091C,
    HEALTH_STEP_COUNTER : 0x0920,
    HEALTH_BODY_COMPOSITION_ANALYZER : 0x0924,
    HEALTH_PEAK_FLOW_MOITOR : 0x0928,
    HEALTH_MEDICATION_MONITOR : 0x092C,
    HEALTH_KNEE_PROSTHESIS : 0x0930,
    HEALTH_ANKLE_PROSTHESIS : 0x0934,
    HEALTH_GENERIC_HEALTH_MANAGER : 0x0938,
    HEALTH_PERSONAL_MOBILITY_DEVICE : 0x093C,
    HEALTH_PERSONAL_MOBILITY_DEVICE : 0x093C
};


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
        await bluetooth.on("stateChange", onReceiveEvent);
        function onReceiveEvent(data) {
            if (data == bluetooth.BluetoothState.STATE_ON) {
                console.info('enable bluetooth');
                done();
            }
        }
        await tryToEnableBt();
        let state = bluetooth.getState();
        expect(state).assertEqual(2);
        await bluetooth.off('stateChange', result => {
            expect(true).assertEqual(result ==null);
            done();
        });
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
        await bluetooth.on("bluetoothDeviceFind", onReceiveEvent)
        function onReceiveEvent(data) {
            console.info('[bluetooth_js] Device' + JSON.stringify(data)+ 'length' + data.length)
            expect(data.length).assertLarger(0);
            done();
        }
        await bluetooth.startBluetoothDiscovery();
        await bluetooth.off('bluetoothDeviceFind', result => {
            console.info("[bluetooth_js] bluetoothDeviceFind off:" + JSON.stringify(result));
            expect(true).assertEqual(result ==null);
            done();
        });
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_START_BLESCAN_WITHOUT_PARAM_0001
     * @tc.name testClassicStartBLEScan
     * @tc.desc Test ClassicStartBLEScan api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_START_BLESCAN_WITHOUT_PARAM_0001', 0, async function (done) {
        console.info('[bluetooth_js] BLE scan start without scan options start');
        await tryToEnableBt();
        let promise = new Promise((resolve) => {
            bluetooth.BLE.on("BLEDeviceFind", onReceiveEvent)
            function onReceiveEvent(data) {
                console.info('[bluetooth_js] BLE scan device find result3 = '+ JSON.stringify(data));
                expect(data.length).assertLarger(0);
                done();
            }
            bluetooth.BLE.startBLEScan([{}]);
            bluetooth.BLE.off('BLEDeviceFind', result => {
                console.info("[bluetooth_js] BLE scan device find off2:" + JSON.stringify(result));
                expect(true).assertEqual(result ==null);
                done();
            });
            let result = bluetooth.BLE.stopBLEScan();
            console.info("[bluetooth_js] onStopBLEScan -> " + JSON.stringify(result));
            console.info('[bluetooth_js] BLE scan start end');
            resolve()
        })
        await promise.then(done)
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_PAIR_DEVICE_0001
     * @tc.name testClassicPairDevice
     * @tc.desc Test ClassicPairDevice api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_PAIR_DEVICE_0001', 0, async function (done) {
        console.info('[bluetooth_js] pair device start');
        await tryToEnableBt();
        await bluetooth.BLE.on('pinRequired', result => {
            console.info("[bluetooth_js] pinRequired on:" + JSON.stringify(result));
            bluetooth.setDevicePairingConfirmation(result,false);
            expect(true).assertEqual(result !=null);
            done();
        });
        let enable3 = bluetooth.pairDevice("00:00:00:00:00:00")
        bluetooth.BLE.off('pinRequired', result => {
            console.info("[bluetooth_js] pinRequired off:" + JSON.stringify(result));
            expect(true).assertEqual(result ==null);
            done();
        });
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_PAIR_DEVICE_0002
     * @tc.name testClassicPairDevice
     * @tc.desc Test ClassicPairDevice api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_PAIR_DEVICE_0002', 0, async function (done) {
        console.info('[bluetooth_js] pair device start');
        await tryToEnableBt();
        await bluetooth.BLE.on('bondStateChange', result => {
            console.info("[bluetooth_js] bondStateChange on:" + JSON.stringify(result)
            +'bondStateChange deviceId:' + data.deviceId + 'bondStateChange state:' +  data.state);
            expect(true).assertEqual(result !=null);
            done();
        });
        let enable4 = bluetooth.pairDevice("00:00:00:00:00:00")
        expect(bluetooth.BondState.BOND_STATE_INVALID == 0).assertTrue();
        expect(bluetooth.BondState.BOND_STATE_BONDING == 1).assertTrue();
        expect(bluetooth.BondState.BOND_STATE_BONDED == 2).assertTrue();
        bluetooth.BLE.off('bondStateChange', result => {
            expect(true).assertEqual(result ==null);
            done();
        });
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
     * @tc.number SUB_COMMUNACATION_bluetoothble_CHARAC_READ_ON_0001
     * @tc.name testonCharacteristicReadOn
     * @tc.desc Test CharacteristicReadOn api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetoothble_CHARAC_READ_ON_0001', 0, async function (done) {
        try {
            await tryToEnableBt();
            console.info('[bluetooth_js] CharacteristicReadOn test start');
            await gattServer.on('characteristicRead', function (data) {
                console.info('[bluetooth_js] CharRedReq deviceId: ' + data.deviceId +
                'transId:' + data.transId + 'offset:' + data.offset + 'charUuid:' +
                data.characteristicUuid + 'serviceUuid:' + data.serviceUuid);
                let serverResponse = {
                    "deviceId": data.deviceId,
                    "transId": data.transId,
                    "status": 0,
                    "offset": data.offset,
                    "value": str2ab("characteristic read response", data.offset),
                };
                let result = gattServer.sendResponse(serverResponse);
                expect(JSON.stringify(result)).assertContain("true");
            });
        }catch(e) {
            expect(null).assertFail();
        }

        try {
            console.info('[bluetooth_js] characteristicRead test1 start');
            await gattServer.off('characteristicRead', function (data) {
                console.info("[bluetooth_js] charaRead off data:" + JSON.stringify(data));
                expect(true).assertEqual(data ==null);
            });
        }catch(e) {
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetoothble_CHARAC_WRITE_ON_0001
     * @tc.name testonCharacteristicwriteOn
     * @tc.desc Test CharacteristicwriteOn api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetoothble_CHARAC_WRITE_ON_0001', 0, async function (done) {
        try {
            await tryToEnableBt();
            console.info('[bluetooth_js] CharacteristicwriteOn test start');
            await gattServer.on('characteristicWrite', function (data) {

                console.info('[bluetooth_js] CharWriReq deviceId: ' + data.deviceId +
                'transId:' + data.transId + 'offset:' + data.offset + 'isPrep:' + data.isPrep +
                'charUuid:' + data.characteristicUuid + 'serviceUuid:' + data.serviceUuid +
                'value:' + data.value + 'needRsp' + data.needRsp);
                if (data.value instanceof ArrayBuffer) {
                    console.log(`[bluetooth_js] value: ${ab2hex(data.value)}`)
                }
                if (data.needRsp == false) {
                    return;
                }
                console.log(`data.value is ArraryBuffer: ${ab2hex(data.value)}`)
                let serverResponse = {
                    "deviceId": data.deviceId,
                    "transId": data.transId,
                    "status": 0,
                    "offset": data.offset,
                    "value": data.value,
                };
                let result = gattServer.sendResponse(serverResponse);
                expect(JSON.stringify(result)).assertContain("true");
            });
        }catch(e) {
            expect(null).assertFail();
        }

        try {
            console.info('[bluetooth_js] characteristicWrite test1 start');
            await gattServer.off('characteristicWrite', function (data) {
                console.info("[bluetooth_js] charaWrite off data2:" + JSON.stringify(data));
                expect(true).assertEqual(data ==null);
            });
        }catch(e) {
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_DESC_READ_ON_0001
     * @tc.name testDescriptorReadOn
     * @tc.desc Test DescriptorReadOn api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_DESC_READ_ON_0001', 0, async function (done) {
        try {
            await tryToEnableBt();
            console.info('[bluetooth_js] descriptorReadOn test start ...');
            await gattServer.on('descriptorRead', function (data) {
                console.info("[bluetooth_js] DesRedon jsondata:" + JSON.stringify(data) +
                'deviceId:' + data.deviceId + 'transId:' + data.transId + 'offset:' + 
                data.offset +'descriptorUuid:' + data.descriptorUuid + 'characteristicUuid:' + 
                data.characteristicUuid + 'serviceUuid:' + data.serviceUuid);
                expect(true).assertEqual(data !=null);

            });
        }catch(e) {
            expect(null).assertFail();
        }

        try {
            console.info('[bluetooth_js] descriptorReadOff test start ...');
            await gattServer.off('descriptorRead', function (data) {
                expect(true).assertEqual(data ==null);
            });
        }catch(e) {
            expect(null).assertFail();
        }
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_DESC_WRITE_ON_0001
     * @tc.name testDescriptorWriteOn
     * @tc.desc Test DescriptorWriteOn api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_DESC_WRITE_ON_0001', 0, async function (done) {
        try {
            await tryToEnableBt();
            console.info('[bluetooth_js] descriptorWriteOn test start ...');
            await gattServer.on('descriptorWrite', function (data) {
                console.info("[bluetooth_js] desWriOn jsondata: " + JSON.stringify(data) +
                'deviceId: ' + data.deviceId + 'transId:' + data.transId + 'offset:' + 
                data.offset +'descriptorUuid:' + data.descriptorUuid + 
                'charUuid:' + data.characteristicUuid +'serviceUuid:' + data.serviceUuid +
                'value:' + data.value + 'needRsp' + data.needRsp + 'isPrep:' + data.isPrep );
                expect(true).assertEqual(data !=null);
            });
        }catch(e) {
            expect(null).assertFail();
        }

        try {
            console.info('[bluetooth_js] descriptorWriteOff test start ...');
            await gattServer.off('descriptorWrite', function (data) {
                expect(true).assertEqual(data ==null);
            });
        }catch(e) {
            expect(null).assertFail();
        }
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_CONNE_STATE_CHANGE_ON_0001
     * @tc.name testConnectStateChangeOn
     * @tc.desc Test ConnectStateChangeOn api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_CONNE_STATE_CHANGE_ON_0001', 0, async function (done) {
        try {
            await tryToEnableBt();
            console.info('[bluetooth_js] ConnectStateChangeOn test start ...');
            await gattServer.on('connectStateChange', function (data) {
                console.info("[bluetooth_js] connectStaOn jsonData -> " + JSON.stringify(data) +
                'deviceId: ' + data.deviceId + 'state:'+ data.state);
                expect(true).assertEqual(data !=null);
            });
        }catch(e) {
            expect(null).assertFail();
        }

        try {
            console.info('[bluetooth_js] ConnectStateChangeOff test start ...');
            await gattServer.off('connectStateChange', function (data) {
                console.info("[bluetooth_js] connectStateChange_off Data:" + JSON.stringify(data));
                expect(true).assertEqual(data ==null);
            });
        }catch(e) {
            expect(null).assertFail();
        }
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_BLE_CHAR_CHANGE_ON_0001
     * @tc.name testBLECharacteristicChangeOn
     * @tc.desc Test BLECharacteristicChangeOn api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_BLE_CHAR_CHANGE_ON_0001', 0, async function (done) {
        try {
            await tryToEnableBt();
            console.info('[bluetooth_js] BLECharacteristicChangeOn test start ...');
            await gattClient.on('BLECharacteristicChange', function (data) {
                console.info("[bluetooth_js] BLECharacteristicChange data " + JSON.stringify(data));
                expect(true).assertEqual(data !=null);
            });
        }catch(e) {
            expect(null).assertFail();
        }

        try {
            console.info('[bluetooth_js] BLECharacteristicChangeOff test start');
            await gattClient.off('BLECharacteristicChange', function (data) {
                console.info("[bluetooth_js] BLECharcChange_off data-> " + JSON.stringify(data));
                expect(true).assertEqual(data ==null);
            });
        }catch(e) {
            expect(null).assertFail();
        }
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetooth_BLE_CONNE_STATE_CHANGE_ON_0001
     * @tc.name testBLEConnectionStateChangeOn
     * @tc.desc Test BLEConnectionStateChangeOn api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetooth_BLE_CONNE_STATE_CHANGE_ON_0001', 0, async function (done) {
        try {
            await tryToEnableBt();
            console.info('[bluetooth_js] BLEConnectionStateChangeOn test start');
            await gattClient.on('BLEConnectionStateChange', function (data) {
                console.info("[bluetooth_js] BLEConnecStateChange_on data " + JSON.stringify(data)
                +'deviceId: ' + data.deviceId + 'state:'+ data.state);
                expect(true).assertEqual(data !=null);
            });
        }catch(e) {
            expect(null).assertFail();
        }

        try {
            console.info('[bluetooth_js] BLEConnectionStateChangeOff test start');
            await gattClient.off('BLEConnectionStateChange', function (data) {
                console.info("[bluetooth_js] BLEConneStateChange_off data-> " + JSON.stringify(data));
                expect(true).assertEqual(data ==null);
            });
        }catch(e) {
            expect(null).assertFail();
        }
        done();
    })


    /**
     * @tc.number SUB_COMMUNACATION_bluetoothble_SPP_READ_ON_0001
     * @tc.name testonsppReadOn
     * @tc.desc Test sppReadOn api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('SUB_COMMUNACATION_bluetoothble_SPP_READ_ON_0001', 0, async function (done) {
        try {
            await tryToEnableBt();
            console.info('[bluetooth_js] sppReadOn test start');
            await bluetooth.on("sppRead",-1, (result) => {
                console.info("[bluetooth_js] sppReadOn json_result -> " + JSON.stringify(result));
                expect(true).assertEqual(result !=null);
            });
        }catch(e) {
            expect(null).assertFail();
        }

        try {
            console.info('[bluetooth_js] sppReadOff test start ...');
            await bluetooth.off("sppRead",-1, (result) => {
                console.info("[bluetooth_js] sppReadOff json_result -> " + JSON.stringify(result));
                expect(true).assertEqual(result ==null);
            });
        }catch(e) {
            expect(null).assertFail();
        }
        done();
    })

})
