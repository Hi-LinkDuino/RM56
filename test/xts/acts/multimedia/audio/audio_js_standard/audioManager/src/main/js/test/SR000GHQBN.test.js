/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import audio from '@ohos.multimedia.audio';
import fileio from '@ohos.fileio';

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index';

describe('audioManger', function () {
    console.info('AudioFrameworkTest: Create AudioManger Object JS Framework');
    const audioManager = audio.getAudioManager();
    var deviceRoleValue = null;
    var deviceTypeValue = null;
    var volErrorMesg = 'Error, Operation not supported or Failed';
    var audioMedia = 3;
    var audioRingtone = 2;
    var minVol = 0;
    var maxVol = 15;
    var lowVol = 5;
    var highVol = 14;
    var outOfRangeVol = 28;
    var longValue = '28374837458743875804735081439085918459801437584738967509184509813904850914375904790589104801843';

    function sleep (ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }

    function displayDeviceProp(value, index, array) {
        var devRoleName;
        var devTypeName;
        if (value.deviceRole==1) {
            devRoleName = 'INPUT_DEVICE';
        }
        else if (value.deviceRole==2) {
            devRoleName = 'OUTPUT_DEVICE ';
        }
        else {
            devRoleName = 'ERROR : UNKNOWN : '+value.deviceRole;
        }

        if (value.deviceType == 1) {
            devTypeName = 'EARPIECE';
        }
        else if (value.deviceType == 2){
            devTypeName = 'SPEAKER';
        }
        else if (value.deviceType == 3){
            devTypeName = 'WIRED_HEADSET';
        }
        else if (value.deviceType == 7){
            devTypeName = 'BLUETOOTH_SCO';
        }
        else if (value.deviceType == 8){
            devTypeName = 'BLUETOOTH_A2DP';
        }
        else if (value.deviceType == 15){
            devTypeName = 'MIC';
        }
        else {
            devTypeName = 'ERROR : UNKNOWN :'+value.deviceType;
        }

        console.info(`AudioFrameworkTest: device role: ${devRoleName}`);
        deviceRoleValue = value.deviceRole;
        console.info(`AudioFrameworkTest: device type: ${devTypeName}`);
        deviceTypeValue = value.deviceType;

    }

    beforeAll(function () {
        console.info('AudioFrameworkTest: beforeAll: Prerequisites at the test suite level');
    })

    beforeEach(function () {
        console.info('AudioFrameworkTest: beforeEach: Prerequisites at the test case level');
    })

    afterEach(async function () {
        console.info('AudioFrameworkTest: afterEach: Test case-level clearance conditions');
        await sleep(1000);
    })

    afterAll(async function () {
        console.info('AudioFrameworkTest: afterAll: Test suite-level cleanup condition');

    })


    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_getAudioManger_001
                * @tc.name      : getAudioManger is Not returned Empty
                * @tc.desc      : Check getAudioManger is not empty
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_getAudioManger_001', 0, function (done) {
        if(audioManager!=null){
            console.info('AudioFrameworkTest: getAudioManger : PASS');
            expect(true).assertTrue();
        }
        else{
            console.info('AudioFrameworkTest: getAudioManger : FAIL');
            expect(false).assertTrue();
        }
        done();
    })


    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_PR_getDevices_output_001
                * @tc.name      : getDevices - Output device - Promise - ENAME
                * @tc.desc      : getDevices - Output device
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_PR_getDevices_output_001', 0, async function (done) {
        deviceRoleValue = null;
        deviceTypeValue = null;
        const promise = audioManager.getDevices(audio.DeviceFlag.OUTPUT_DEVICES_FLAG)
        promise.then(function (value) {

            console.info('AudioFrameworkTest: Promise: getDevices OUTPUT_DEVICES_FLAG');
            value.forEach(displayDeviceProp);
            if (deviceTypeValue != null && deviceRoleValue != null){
                console.info('AudioFrameworkTest: Promise: getDevices : OUTPUT_DEVICES_FLAG :  PASS');
                expect(true).assertTrue();
            }
            else{
                console.info('AudioFrameworkTest: Promise: getDevices : OUTPUT_DEVICES_FLAG :  FAIL');
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_PR_getDevices_output_enum_002
                * @tc.name      : getDevices - Output device - Promise - ENAME -
                * @tc.desc      : getDevices - Output device
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_PR_getDevices_output_enum_002', 0, async function (done) {
        deviceRoleValue = null;
        deviceTypeValue = null;
        const promise = audioManager.getDevices(1)
        promise.then(function (value) {

            console.info('AudioFrameworkTest: Promise: getDevices OUTPUT_DEVICES_FLAG');
            value.forEach(displayDeviceProp);
            if (deviceTypeValue != null && deviceRoleValue != null){
                console.info('AudioFrameworkTest: Promise: getDevices : OUTPUT_DEVICES_FLAG :  PASS');
                expect(true).assertTrue();
            }
            else{
                console.info('AudioFrameworkTest: Promise: getDevices : OUTPUT_DEVICES_FLAG :  FAIL');
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_PR_getDevices_input_003
                * @tc.name      : getDevices - Input device - Promise - ENAME
                * @tc.desc      : getDevices - Input device
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_PR_getDevices_input_003', 0, async function (done) {
        deviceRoleValue = null;
        deviceTypeValue = null;
        const promise = audioManager.getDevices(audio.DeviceFlag.INPUT_DEVICES_FLAG);
        promise.then(function (value) {
            console.info('AudioFrameworkTest: Promise: getDevices INPUT_DEVICES_FLAG');
            value.forEach(displayDeviceProp);
            if (deviceTypeValue != null && deviceRoleValue != null){
                console.info('AudioFrameworkTest: Promise: getDevices : INPUT_DEVICES_FLAG :  PASS');
                expect(true).assertTrue();
            }
            else{
                console.info('AudioFrameworkTest: Promise: getDevices : INPUT_DEVICES_FLAG :  FAIL');
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_PR_getDevices_input_enum_004
                * @tc.name      : getDevices - Input device - Promise - ENAME
                * @tc.desc      : getDevices - Input device
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_PR_getDevices_input_enum_004', 0, async function (done) {
        deviceRoleValue = null;
        deviceTypeValue = null;
        const promise = audioManager.getDevices(2);
        promise.then(function (value) {
            console.info('AudioFrameworkTest: Promise: getDevices INPUT_DEVICES_FLAG');
            value.forEach(displayDeviceProp);

            if (deviceTypeValue != null && deviceRoleValue != null){
                console.info('AudioFrameworkTest: Promise: getDevices : INPUT_DEVICES_FLAG :  PASS');
                expect(true).assertTrue();
            }
            else{
                console.info('AudioFrameworkTest: Promise: getDevices : INPUT_DEVICES_FLAG :  FAIL');
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_PR_getDevices_all_005
                * @tc.name      : getDevices - ALL device - Promise - ENAME
                * @tc.desc      : getDevices - ALL device
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_PR_getDevices_all_005', 0, async function (done) {
        deviceRoleValue = null;
        deviceTypeValue = null;
        const promise = audioManager.getDevices(audio.DeviceFlag.ALL_DEVICES_FLAG);
        promise.then(function (value) {
            console.info('AudioFrameworkTest: Promise: getDevices ALL_DEVICES_FLAG');
            value.forEach(displayDeviceProp);

            if (deviceTypeValue != null && deviceRoleValue != null){
                console.info('AudioFrameworkTest: Promise: getDevices : ALL_DEVICES_FLAG :  PASS');
                expect(true).assertTrue();
            }
            else{
                console.info('AudioFrameworkTest: Promise: getDevices : ALL_DEVICES_FLAG :  FAIL');
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_PR_getDevices_all_enum_006
                * @tc.name      : getDevices - ALL device - Promise - ENAME
                * @tc.desc      : getDevices - ALL device
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_PR_getDevices_all_enum_006', 0, async function (done) {
        deviceRoleValue = null;
        deviceTypeValue = null;
        const promise = audioManager.getDevices(3);
        promise.then(function (value) {
            console.info('AudioFrameworkTest: Promise: getDevices ALL_DEVICES_FLAG');
            value.forEach(displayDeviceProp);

            if (deviceTypeValue != null && deviceRoleValue != null){
                console.info('AudioFrameworkTest: Promise: getDevices : ALL_DEVICES_FLAG :  PASS');
                expect(true).assertTrue();
            }
            else{
                console.info('AudioFrameworkTest: Promise: getDevices : ALL_DEVICES_FLAG :  FAIL');
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_CB_getDevices_OUT_007
                * @tc.name      : getDevices - Output device - Callback - ENAME
                * @tc.desc      : getDevices - Output device
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_CB_getDevices_OUT_007', 0, async function (done) {
        deviceRoleValue = null;
        deviceTypeValue = null;
        audioManager.getDevices(audio.DeviceFlag.OUTPUT_DEVICES_FLAG, (err, value) => {
            console.info('AudioFrameworkTest: Callback: getDevices OUTPUT_DEVICES_FLAG');
            if (err) {
                console.error(`AudioFrameworkTest: Callback: OUTPUT_DEVICES_FLAG: failed to get devices ${err.message}`);
                expect().assertFail();
            }
            else{
                console.info('AudioFrameworkTest: Callback: getDevices OUTPUT_DEVICES_FLAG');
                value.forEach(displayDeviceProp);
                if (deviceTypeValue != null && deviceRoleValue != null){
                    console.info('AudioFrameworkTest: Callback: getDevices : OUTPUT_DEVICES_FLAG :  PASS');
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTest: Callback: getDevices : OUTPUT_DEVICES_FLAG :  FAIL');
                    expect(false).assertTrue();
                }
            }
            done();
        });
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_CB_getDevices_OUT_ENUM_008
                * @tc.name      : getDevices - Output device - Callback - ENAME
                * @tc.desc      : getDevices - Output device
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_CB_getDevices_OUT_ENUM_008', 0, async function (done) {
        deviceRoleValue = null;
        deviceTypeValue = null;
        audioManager.getDevices(1, (err, value) => {
            console.info('AudioFrameworkTest: Callback: getDevices OUTPUT_DEVICES_FLAG');
            if (err) {
                console.error(`AudioFrameworkTest: Callback: OUTPUT_DEVICES_FLAG: failed to get devices ${err.message}`);
                expect().assertFail();
            }
            else{
                console.info('AudioFrameworkTest: Callback: getDevices OUTPUT_DEVICES_FLAG');
                value.forEach(displayDeviceProp);
                if (deviceTypeValue != null && deviceRoleValue != null){
                    console.info('AudioFrameworkTest: Callback: getDevices : OUTPUT_DEVICES_FLAG :  PASS');
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTest: Callback: getDevices : OUTPUT_DEVICES_FLAG :  FAIL');
                    expect(false).assertTrue();
                }
            }
            done();
        });
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_CB_getDevices_INPUT_009
                * @tc.name      : getDevices - Input device - Callback - ENAME
                * @tc.desc      : getDevices - Input device
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_CB_getDevices_INPUT_009', 0, async function (done) {
        deviceRoleValue = null;
        deviceTypeValue = null;
        audioManager.getDevices(audio.DeviceFlag.INPUT_DEVICES_FLAG, (err, value) => {
            console.info('AudioFrameworkTest: Callback: getDevices INPUT_DEVICES_FLAG');
            if (err) {
                console.error(`AudioFrameworkTest: Callback: INPUT_DEVICES_FLAG: failed to get devices ${err.message}`);
                expect().assertFail();
            }
            else{
                console.info('AudioFrameworkTest: Callback: getDevices INPUT_DEVICES_FLAG');
                value.forEach(displayDeviceProp);

                if (deviceTypeValue != null && deviceRoleValue != null){
                    console.info('AudioFrameworkTest: Callback: getDevices : INPUT_DEVICES_FLAG:  PASS');
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTest: Callback: getDevices : INPUT_DEVICES_FLAG:  FAIL');
                    expect(false).assertTrue();
                }
            }
            done();
        });
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_CB_getDevices_INPUT_ENUM_010
                * @tc.name      : getDevices - Input device - Callback - ENAME
                * @tc.desc      : getDevices - Input device
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_CB_getDevices_INPUT_ENUM_010', 0, async function (done) {
        deviceRoleValue = null;
        deviceTypeValue = null;
        audioManager.getDevices(2, (err, value) => {
            console.info('AudioFrameworkTest: Callback: getDevices INPUT_DEVICES_FLAG');
            if (err) {
                console.error(`AudioFrameworkTest: Callback: INPUT_DEVICES_FLAG: failed to get devices ${err.message}`);
                expect().assertFail();
            }
            else{
                console.info('AudioFrameworkTest: Callback: getDevices INPUT_DEVICES_FLAG');
                value.forEach(displayDeviceProp);

                if (deviceTypeValue != null && deviceRoleValue != null){
                    console.info('AudioFrameworkTest: Callback: getDevices : INPUT_DEVICES_FLAG:  PASS');
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTest: Callback: getDevices : INPUT_DEVICES_FLAG:  FAIL');
                    expect(false).assertTrue();
                }
            }
            done();
        });
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_CB_getDevices_All_011
                * @tc.name      : getDevices - ALL device - Callback - ENAME
                * @tc.desc      : getDevices - ALL device
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_getDevices_All_011', 0, async function (done) {
        deviceRoleValue = null;
        deviceTypeValue = null;
        audioManager.getDevices(audio.DeviceFlag.ALL_DEVICES_FLAG, (err, value) => {
            console.info('AudioFrameworkTest: Callback: getDevices ALL_DEVICES_FLAG');
            if (err) {
                console.error(`AudioFrameworkTest: Callback: ALL_DEVICES_FLAG: failed to get devices ${err.message}`);
                expect().assertFail();
            }
            else{
                console.info('AudioFrameworkTest: Callback: getDevices ALL_DEVICES_FLAG');
                value.forEach(displayDeviceProp);
                if (deviceTypeValue != null && deviceRoleValue != null){
                    console.info('AudioFrameworkTest: Callback: getDevices : ALL_DEVICES_FLAG:  PASS');
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTest: Callback: getDevices : ALL_DEVICES_FLAG:  FAIL');
                    expect(false).assertTrue();
                }
            }
            done();
        });
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_CB_getDevices_All_ENUM_012
                * @tc.name      : getDevices - ALL device - Callback - ENAME
                * @tc.desc      : getDevices - ALL device
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_CB_getDevices_All_ENUM_012', 0, async function (done) {
        deviceRoleValue = null;
        deviceTypeValue = null;
        audioManager.getDevices(3, (err, value) => {
            console.info('AudioFrameworkTest: Callback: getDevices ALL_DEVICES_FLAG');
            if (err) {
                console.error(`AudioFrameworkTest: Callback: ALL_DEVICES_FLAG: failed to get devices ${err.message}`);
                expect().assertFail();
            }
            else{
                console.info('AudioFrameworkTest: Callback: getDevices ALL_DEVICES_FLAG');
                value.forEach(displayDeviceProp);
                if (deviceTypeValue != null && deviceRoleValue != null){
                    console.info('AudioFrameworkTest: Callback: getDevices : ALL_DEVICES_FLAG:  PASS');
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTest: Callback: getDevices : ALL_DEVICES_FLAG:  FAIL');
                    expect(false).assertTrue();
                }
            }
            done();
        });
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_PR_Activate_013
                * @tc.name      : setDeviceActive - BLUETOOTH_SCO - Activate - Promise
                * @tc.desc      : Activate BLUETOOTH_SCO - Promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_setDeviceActive_PR_Activate_013', 0, async function (done) {
        await audioManager.setDeviceActive(audio.ActiveDeviceType.BLUETOOTH_SCO,true).then(function (){
            console.info('AudioFrameworkTest: Device Test: Promise : setDeviceActive : BLUETOOTH_SCO: Activate');
            audioManager.isDeviceActive(audio.ActiveDeviceType.BLUETOOTH_SCO).then(function (value){
                if(value==true){
                    console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : BLUETOOTH_SCO: Activate : PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : BLUETOOTH_SCO: Activate : FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        }).catch((err) => {
            console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive :  BLUETOOTH_SCO: Activate : FAIL : Error :' + err.message);
            expect(false).assertTrue();
        });
        done();
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_PR_Activate_ENUM_014
                * @tc.name      : setDeviceActive - BLUETOOTH_SCO - Activate - Promise
                * @tc.desc      : Activate BLUETOOTH_SCO - Promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_setDeviceActive_PR_Activate_ENUM_014', 0, async function (done) {
        await audioManager.setDeviceActive(7,true).then(function (){
            console.info('AudioFrameworkTest: Device Test: Promise : setDeviceActive : BLUETOOTH_SCO: Activate');
            audioManager.isDeviceActive(7).then(function (value){
                if(value==true){
                    console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : BLUETOOTH_SCO: Activate : PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : BLUETOOTH_SCO: Activate : FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        }).catch((err) => {
            console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive :  BLUETOOTH_SCO: Activate : FAIL : Error :' + err.message);
            expect(false).assertTrue();
        });
        done();
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_PR_Deactivate_015
                * @tc.name      : setDeviceActive - SPEAKER - deactivate - Promise
                * @tc.desc      : Deactivate speaker - Promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_setDeviceActive_PR_Deactivate_015', 0, async function (done) {
        await audioManager.setDeviceActive(audio.ActiveDeviceType.SPEAKER,false).then(function (){
            // Setting device active ENUM 2 = SPEAKER
            console.info('AudioFrameworkTest: Device Test: Promise : setDeviceActive : SPEAKER: Deactivate');
            audioManager.isDeviceActive(audio.ActiveDeviceType.SPEAKER).then(function (value){
                if(value==false){
                    console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : SPEAKER: Deactivate : PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : SPEAKER: Deactivate : FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        }).catch((err) => {
            console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : SPEAKER: Deactivate : FAIL : Error :' + err.message);
            expect(false).assertTrue();
        });
        done();
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_PR_Deactivate_ENUM_016
                * @tc.name      : setDeviceActive - SPEAKER - deactivate - Promise
                * @tc.desc      : Deactivate speaker - Promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_setDeviceActive_PR_Deactivate_ENUM_016', 0, async function (done) {
        await audioManager.setDeviceActive(2,true).then(function (){
            console.info('AudioFrameworkTest: Device Test: Promise : setDeviceActive : SPEAKER: Active');
        });
        await audioManager.setDeviceActive(2,false).then(function (){
            // Setting device active ENUM 2 = SPEAKER
            console.info('AudioFrameworkTest: Device Test: Promise : setDeviceActive : SPEAKER: Deactivate');
            audioManager.isDeviceActive(2).then(function (value){
                if(value==false){
                    console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : SPEAKER: Deactivate : PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : SPEAKER: Deactivate : FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        }).catch((err) => {
            console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : SPEAKER: Deactivate : FAIL : Error :' + err.message);
            expect(false).assertTrue();
        });
        done();
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_PR_Activate_017
                * @tc.name      : setDeviceActive - SPEAKER - Activate - Promise
                * @tc.desc      : Activate speaker - Promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_setDeviceActive_PR_Activate_017', 0, async function (done) {
        await audioManager.setDeviceActive(audio.ActiveDeviceType.SPEAKER,true).then(function (){
            console.info('AudioFrameworkTest: Device Test: Promise : setDeviceActive : SPEAKER: Activate');
            audioManager.isDeviceActive(audio.ActiveDeviceType.SPEAKER).then(function (value){
                if(value==true){
                    console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : SPEAKER: Activate : PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : SPEAKER: Activate : FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        }).catch((err) => {
            console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : SPEAKER: Activate : FAIL :Error :' + err.message);
            expect(false).assertTrue();
        });
        done();
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_PR_Activate_ENUM_018
                * @tc.name      : setDeviceActive - SPEAKER - Activate - Promise
                * @tc.desc      : Activate speaker - Promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_setDeviceActive_PR_Activate_ENUM_018', 0, async function (done) {
        await audioManager.setDeviceActive(2,true).then(function (){
            console.info('AudioFrameworkTest: Device Test: Promise : setDeviceActive : SPEAKER: Activate');
            audioManager.isDeviceActive(2).then(function (value){
                if(value==true){
                    console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : SPEAKER: Activate : PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : SPEAKER: Activate : FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        }).catch((err) => {
            console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : SPEAKER: Activate : FAIL :Error :' + err.message);
            expect(false).assertTrue();
        });
        done();
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_PR_Deactivate_019
                * @tc.name      : setDeviceActive - BLUETOOTH_SCO - deactivate - Promise
                * @tc.desc      : Deactivate BLUETOOTH_SCO - Promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_setDeviceActive_PR_Deactivate_019', 0, async function (done) {
        await audioManager.setDeviceActive(audio.ActiveDeviceType.BLUETOOTH_SCO,false).then(function (){
            // Setting device active ENUM 3 = BLUETOOTH_SCO
            console.info('AudioFrameworkTest: Device Test: Promise : setDeviceActive : BLUETOOTH_SCO: Deactivate');
            audioManager.isDeviceActive(audio.ActiveDeviceType.BLUETOOTH_SCO).then(function (value){
                if(value==false){
                    console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : BLUETOOTH_SCO: Deactivate : PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : BLUETOOTH_SCO: Deactivate : FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        }).catch((err) => {
            console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive :  BLUETOOTH_SCO: Deactivate : FAIL : Error :' + err.message);
            expect(false).assertTrue();
        });
        done();
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_PR_Deactivate_ENUM_020
                * @tc.name      : setDeviceActive - BLUETOOTH_SCO - deactivate - Promise
                * @tc.desc      : Deactivate BLUETOOTH_SCO - Promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_setDeviceActive_PR_Deactivate_ENUM_020', 0, async function (done) {
        await audioManager.setDeviceActive(7,true).then(function (){
            console.info('AudioFrameworkTest: Device Test: Promise : setDeviceActive : BLUETOOTH_SCO: Active');
        });
        await audioManager.setDeviceActive(7,false).then(function (){
            // Setting device active ENUM 3 = BLUETOOTH_SCO
            console.info('AudioFrameworkTest: Device Test: Promise : setDeviceActive : BLUETOOTH_SCO: Deactivate');
            audioManager.isDeviceActive(7).then(function (value){
                if(value==false){
                    console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : BLUETOOTH_SCO: Deactivate : PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive : BLUETOOTH_SCO: Deactivate : FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        }).catch((err) => {
            console.info('AudioFrameworkTest: Device Test: Promise : isDeviceActive :  BLUETOOTH_SCO: Deactivate : FAIL : Error :' + err.message);
            expect(false).assertTrue();
        });
        done();
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_CB_Activate_021
                * @tc.name      : setDeviceActive - BLUETOOTH_SCO - Activate - Callback
                * @tc.desc      : Activate BLUETOOTH_SCO - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_setDeviceActive_CB_Activate_021', 0, async function (done) {
        audioManager.setDeviceActive(audio.ActiveDeviceType.BLUETOOTH_SCO,true, (err) => {
            if (err) {
                console.error(`AudioFrameworkTest: Device Test: Callback : setDeviceActive : BLUETOOTH_SCO: Active: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                console.info('AudioFrameworkTest: Device Test: Callback : setDeviceActive : BLUETOOTH_SCO: Active');
                audioManager.isDeviceActive(audio.ActiveDeviceType.BLUETOOTH_SCO,(err, value) => {
                    if (err) {
                        console.error(`AudioFrameworkTest: Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Active: Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value==true){
                        console.info('AudioFrameworkTest: Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Active : PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('AudioFrameworkTest: Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Active : FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_CB_Activate_ENUM_022
                * @tc.name      : setDeviceActive - BLUETOOTH_SCO - Activate - Callback
                * @tc.desc      : Activate BLUETOOTH_SCO - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_setDeviceActive_CB_Activate_ENUM_022', 0, async function (done) {
        audioManager.setDeviceActive(7,true, (err) => {
            if (err) {
                console.error(`AudioFrameworkTest: Device Test: Callback : setDeviceActive : BLUETOOTH_SCO: Active: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                console.info('AudioFrameworkTest: Device Test: Callback : setDeviceActive : BLUETOOTH_SCO: Active');
                audioManager.isDeviceActive(7,(err, value) => {
                    if (err) {
                        console.error(`AudioFrameworkTest: Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Active: Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value==true){
                        console.info('AudioFrameworkTest: Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Active : PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('AudioFrameworkTest: Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Active : FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_CB_DEACTIVATE_023
                * @tc.name      : setDeviceActive - SPEAKER - deactivate - Callback
                * @tc.desc      : Deactivate speaker - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_setDeviceActive_CB_DEACTIVATE_023', 0, async function (done) {
        audioManager.setDeviceActive(audio.ActiveDeviceType.SPEAKER,false, (err) => {
            if (err) {
                console.error(`AudioFrameworkTest: Device Test: Callback : setDeviceActive : SPEAKER: Deactivate: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                console.info('AudioFrameworkTest: Device Test: Callback : setDeviceActive : SPEAKER: Active');
                audioManager.isDeviceActive(audio.ActiveDeviceType.SPEAKER,(err, value) => {
                    if (err) {
                        console.error(`AudioFrameworkTest: Device Test: Callback : isDeviceActive : SPEAKER: Deactivate: Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value==false){
                        console.info('AudioFrameworkTest: Device Test: Callback : isDeviceActive : SPEAKER: Deactivate : PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('AudioFrameworkTest: Device Test: Callback : isDeviceActive : SPEAKER: Deactivate : FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_CB_DEACTIVATE_ENUM_024
                * @tc.name      : setDeviceActive - SPEAKER - deactivate - Callback
                * @tc.desc      : Deactivate speaker - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_setDeviceActive_CB_DEACTIVATE_ENUM_024', 0, async function (done) {
        await audioManager.setDeviceActive(2,true).then(function (){
            console.info('AudioFrameworkTest: Device Test: Promise : setDeviceActive : SPEAKER : Active');
        });
        audioManager.setDeviceActive(2,false, (err) => {
            if (err) {
                console.error(`AudioFrameworkTest: Device Test: Callback : setDeviceActive : SPEAKER: Deactivate: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                console.info('AudioFrameworkTest: Device Test: Callback : setDeviceActive : SPEAKER: Active');
                audioManager.isDeviceActive(2,(err, value) => {
                    if (err) {
                        console.error(`AudioFrameworkTest: Device Test: Callback : isDeviceActive : SPEAKER: Deactivate: Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value==false){
                        console.info('AudioFrameworkTest: Device Test: Callback : isDeviceActive : SPEAKER: Deactivate : PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('AudioFrameworkTest: Device Test: Callback : isDeviceActive : SPEAKER: Deactivate : FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })


    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_CB_ACTIVATE_025
                * @tc.name      : setDeviceActive - SPEAKER - activate - Callback
                * @tc.desc      : Activate speaker - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_setDeviceActive_CB_ACTIVATE_025', 0, async function (done) {
        audioManager.setDeviceActive(audio.ActiveDeviceType.SPEAKER,true, (err) => {
            if (err) {
                console.error(`AudioFrameworkTest: Device Test: Callback : setDeviceActive : SPEAKER: Active: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                console.info('AudioFrameworkTest: Device Test: Callback : setDeviceActive : SPEAKER: Active');
                audioManager.isDeviceActive(audio.ActiveDeviceType.SPEAKER,(err, value) => {
                    if (err) {
                        console.error(`AudioFrameworkTest: Device Test: Callback : isDeviceActive : SPEAKER: Active: Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value==true){
                        console.info('AudioFrameworkTest: Device Test: Callback : isDeviceActive : SPEAKER: Active : PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('AudioFrameworkTest: Device Test: Callback : isDeviceActive : SPEAKER: Active : FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_CB_ACTIVATE_ENUM_026
                * @tc.name      : setDeviceActive - SPEAKER - activate - Callback
                * @tc.desc      : Activate speaker - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_setDeviceActive_CB_ACTIVATE_ENUM_026  ', 0, async function (done) {
        audioManager.setDeviceActive(2,true, (err) => {
            if (err) {
                console.error(`AudioFrameworkTest: Device Test: Callback : setDeviceActive : SPEAKER: Active: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                console.info('AudioFrameworkTest: Device Test: Callback : setDeviceActive : SPEAKER: Active');
                audioManager.isDeviceActive(2,(err, value) => {
                    if (err) {
                        console.error(`AudioFrameworkTest: Device Test: Callback : isDeviceActive : SPEAKER: Active: Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value==true){
                        console.info('AudioFrameworkTest: Device Test: Callback : isDeviceActive : SPEAKER: Active : PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('AudioFrameworkTest: Device Test: Callback : isDeviceActive : SPEAKER: Active : FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_DEACTIVATE_027
                * @tc.name      : setDeviceActive - BLUETOOTH_SCO - deactivate - Callback
                * @tc.desc      : Deactivate BLUETOOTH_SCO - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_setDeviceActive_DEACTIVATE_027', 0, async function (done) {
        audioManager.setDeviceActive(audio.ActiveDeviceType.BLUETOOTH_SCO,false, (err) => {
            if (err) {
                console.error(`AudioFrameworkTest: Device Test: Callback : setDeviceActive : BLUETOOTH_SCO: Deactivate: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                console.info('AudioFrameworkTest: Device Test: Callback : setDeviceActive : BLUETOOTH_SCO: Active');
                audioManager.isDeviceActive(audio.ActiveDeviceType.BLUETOOTH_SCO,(err, value) => {
                    if (err) {
                        console.error(`AudioFrameworkTest: Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Deactivate: Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value==false){
                        console.info('AudioFrameworkTest: Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Deactivate : PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('AudioFrameworkTest: Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Deactivate : FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_DEACTIVATE_ENUM_028
                * @tc.name      : setDeviceActive - BLUETOOTH_SCO - deactivate - Callback
                * @tc.desc      : Deactivate BLUETOOTH_SCO - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_setDeviceActive_DEACTIVATE_ENUM_028', 0, async function (done) {
        await audioManager.setDeviceActive(7,true).then(function (){
            console.info('AudioFrameworkTest: Device Test: Promise : setDeviceActive : BLUETOOTH_SCO : Active');
        });
        audioManager.setDeviceActive(7,false, (err) => {
            if (err) {
                console.error(`AudioFrameworkTest: Device Test: Callback : setDeviceActive : BLUETOOTH_SCO: Deactivate: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                console.info('AudioFrameworkTest: Device Test: Callback : setDeviceActive : BLUETOOTH_SCO: Active');
                audioManager.isDeviceActive(7,async(err, value) => {
                    if (err) {
                        console.error(`AudioFrameworkTest: Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Deactivate: Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value==false){
                        console.info('AudioFrameworkTest: Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Deactivate : PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('AudioFrameworkTest: Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Deactivate : FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                    await sleep(1000);
                });
            }
            done();
        });
        await sleep(1000);
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_DeviceType_001
                * @tc.name      : DeviceType - INVALID
                * @tc.desc      : DeviceType - INVALID
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_DeviceType_001', 0, async function (done) {

        expect(audio.DeviceType.INVALID).assertEqual(0);
        await sleep(50);
        done();
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_DeviceType_002
                * @tc.name      : DeviceType - WIRED_HEADSET
                * @tc.desc      : DeviceType - WIRED_HEADSET
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_DeviceType_001', 0, async function (done) {

        expect(audio.DeviceType.WIRED_HEADSET).assertEqual(3);
        await sleep(50);
        done();
    })
	
    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_DeviceType_003
                * @tc.name      : DeviceType - USB_HEADSET
                * @tc.desc      : DeviceType - USB_HEADSET
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_DeviceType_003', 0, async function (done) {
        var resultflag = false;
        if (audio.DeviceType.USB_HEADSET == 22 || audio.DeviceType.USB_HEADSET == undefined)
        resultflag = true;
        expect(resultflag).assertTrue();
        await sleep(50);
        done();
    })



})