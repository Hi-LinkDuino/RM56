/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
import app from '@system.app';
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import deviceManager from '@ohos.distributedHardware.deviceManager'

describe('deviceManagerTest', function() {

    /*
     * @tc.number  deviceManagerTest_createDeviceManager_0100
     * @tc.name    correct Packagesname for createDeviceManager
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('deviceManagerTest_createDeviceManager_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_createDeviceManager_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            expect(data != null).assertTrue()
        })
        console.log("----------stop running deviceManagerTest_createDeviceManager_0100----------");
        done();
    })   

    /*
     * @tc.number  deviceManagerTest_createDeviceManager_0200
     * @tc.name    null Packagesname  for createDeviceManager
     * @tc.desc    Function test
     * @tc.level   3
     */
    it('deviceManagerTest_createDeviceManager_0200', 0, async function (done) {
        console.log("----------start running deviceManagerTest_createDeviceManager_0200----------");
        deviceManager.createDeviceManager('', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            expect(data == null).assertTrue()
        });
        console.log("----------stop running deviceManagerTest_createDeviceManager_0200----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_createDeviceManager_0300
     * @tc.name    incorrect Packagesname for createDeviceManager
     * @tc.desc    Function test
     * @tc.level   3
     */
    it('deviceManagerTest_createDeviceManager_0300', 0, async function (done) {
        console.log("----------start running deviceManagerTest_createDeviceManager_0300----------");
        deviceManager.createDeviceManager('comohosdevicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            expect(data == null).assertTrue()
        });
        console.log("----------stop running deviceManagerTest_createDeviceManager_0300----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_createDeviceManager_0400
     * @tc.name    incorrect Packagesname case2 for createDeviceManager
     * @tc.desc    Function test
     * @tc.level   3
     */
    it('deviceManagerTest_createDeviceManager_0400', 0, async function (done) {
        console.log("----------start running deviceManagerTest_createDeviceManager_0400----------");
        deviceManager.createDeviceManager('com.ohos.123', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            expect(data == null).assertTrue()
        });      
        console.log("----------stop running deviceManagerTest_createDeviceManager_0400----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_createDeviceManager_0500
     * @tc.name    special Packagesname for createDeviceManager
     * @tc.desc    Function test
     * @tc.level   3
     */
    it('deviceManagerTest_createDeviceManager_0500', 0, async function (done) {
        console.log("----------start running deviceManagerTest_createDeviceManager_0500----------");
        deviceManager.createDeviceManager('com#ohos%devicemangagerdemo&*', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            expect(data == null).assertTrue()
        });  
        console.log("----------stop running deviceManagerTest_createDeviceManager_0500----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_startDeviceDiscovery_0100
     * @tc.name    correct deviceinfo for startDeviceDiscovery with different para 1
     * @tc.desc    Function test
     * @tc.level   3
     */
    it('deviceManagerTest_startDeviceDiscovery_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_startDeviceDiscovery_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }

            let info = {
                "subscribeId": 2115,
                "mode": DISCOVER_MODE_ACTIVE,
                "medium": AUTO,
                "freq": LOW,
                "isSameAccount": false,
                "isWakeRemote": true,
                "capability": 0
            };
            let start = data.startDeviceDiscovery(info);
            expect(start != null).assertTrue();
        });
        console.log("----------stop running deviceManagerTest_startDeviceDiscovery_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_startDeviceDiscovery_0110
     * @tc.name    correct deviceinfo for startDeviceDiscovery with different para 2
     * @tc.desc    Function test
     * @tc.level   3
     */
    it('deviceManagerTest_startDeviceDiscovery_0110', 0, async function (done) {
        console.log("----------start running deviceManagerTest_startDeviceDiscovery_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }

            let info = {
                "subscribeId": 2115,
                "mode": DISCOVER_MODE_PASSIVE,
                "medium": BLE,
                "freq": MID,
                "isSameAccount": false,
                "isWakeRemote": true,
                "capability": SUBSCRIBE_CAPABILITY_OSD
            };
            let start = data.startDeviceDiscovery(info);
            expect(start != null).assertTrue();
        });
        console.log("----------stop running deviceManagerTest_startDeviceDiscovery_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_startDeviceDiscovery_0120
     * @tc.name    correct deviceinfo for startDeviceDiscovery with different para 3
     * @tc.desc    Function test
     * @tc.level   3
     */
    it('deviceManagerTest_startDeviceDiscovery_0120', 0, async function (done) {
        console.log("----------start running deviceManagerTest_startDeviceDiscovery_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }

            let info = {
                "subscribeId": 2115,
                "mode": DISCOVER_MODE_PASSIVE,
                "medium": COAP,
                "freq": SUPER_HIGH,
                "isSameAccount": false,
                "isWakeRemote": true,
                "capability": SUBSCRIBE_CAPABILITY_DDMP
            };
            let start = data.startDeviceDiscovery(info);
            expect(start != null).assertTrue();
        });
        console.log("----------stop running deviceManagerTest_startDeviceDiscovery_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_release_0100
     * @tc.name    testcase for release
     * @tc.desc    Function test
     * @tc.level   1
     */
    it('deviceManagerTest_release_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_release_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let re = data.release();
            expect(re != null).assertTrue();
        });
        console.log("----------stop running deviceManagerTest_release_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_getTrustedDeviceListSync_0100
     * @tc.name    testcase for getTrustedDeviceListSync
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_getTrustedDeviceListSync_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_getTrustedDeviceListSync_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let array = data.getTrustedDeviceListSync();
            expect(array == null).assertTrue();
        });
        console.log("----------stop running deviceManagerTest_getTrustedDeviceListSync_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_stopDeviceDiscovery_0100
     * @tc.name    testcase for stopDeviceDiscovery
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_stopDeviceDiscovery_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_stopDeviceDiscovery_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let subscribeId = 2115;
            let start = data.stopDeviceDiscovery(subscribeId);
            expect(start != null).assertTrue();
        });
        console.log("----------stop running deviceManagerTest_stopDeviceDiscovery_0100----------");
        done();
    })

/*
     * @tc.number  deviceManagerTest_getLocalDeviceInfoSync_0100
     * @tc.name    call getLocalDeviceInfoSync interface
     * @tc.desc    Function test
     * @tc.level   1
     */
    it('deviceManagerTest_getLocalDeviceInfoSync_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_getLocalDeviceInfoSync_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            setTimeout(function(){
                dmClass = data;
                dmGetLocalDeviceInfoSync();
            },1000);
        });
        async function dmGetLocalDeviceInfoSync(){
            let deviceInfo = dmClass.getLocalDeviceInfoSync();
            expect(deviceInfo.deviceId != null).assertTrue();
        }
        console.log("----------stop running deviceManagerTest_getLocalDeviceInfoSync_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_getLocalDeviceInfo1_0100
     * @tc.name    Call the getLocalDeviceInfo interface
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('deviceManagerTest_deviceManagerTest_getLocalDeviceInfo1_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_deviceManagerTest_getLocalDeviceInfo1_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            setTimeout(function(){
                dmClass = data;
                getLocalDeviceInfo1();
            },1000);
        });
        async function getLocalDeviceInfo1(){
            let deviceInfo = dmClass.getLocalDeviceInfo((err, data) => {
                if (err) {
                    expect().assertFail();
                    console.log("[deviceManagerTest] createDeviceManager error");
                }
            });
            expect(deviceInfo == undefined).assertTrue();
        }
        console.log("----------stop running deviceManagerTest_deviceManagerTest_getLocalDeviceInfo1_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_getLocalDeviceInfo(promise)_0100
     * @tc.name    Call the getLocalDeviceInfo interface
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('deviceManagerTest_getLocalDeviceInfo(promise)_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_getLocalDeviceInfo(promise)_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            setTimeout(function(){
                dmClass = data;
                getLocalDeviceInfo2();
            },1000);
        });
        async function getLocalDeviceInfo2(){
            let deviceInfo = dmClass.getLocalDeviceInfo().then(data => {
            })
            expect(deviceInfo != null).assertTrue();
        }
        console.log("----------stop running deviceManagerTest_getLocalDeviceInfo(promise)_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_authenticateDevice_0100
     * @tc.name    testcase for authenticateDevice in correct deviceType para with WEARABLE
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_authenticateDevice_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_authenticateDevice_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let deviceInfo = {
            deviceId: "",
            deviceName: "",
            deviceType: WEARABLE
            }
            let extraInfo = {
                "targetPkgName": 'com.ohos.devicemangagerdemo',
                "appName": "myapplication",
                "appDescription": "apply auth",
                "business": '0',
                "displayOwner": 0
            }
            let authParam = {
                "authType": 1,
                "appIcon": new Uint8Array([]),
                "appThumbnail": new Uint8Array([]),
                "extraInfo": extraInfo
            }
            data.authenticateDevice(deviceInfo, authParam, (err, data) => {
                if (err) {
                    expect().assertFail();
                    console.log("[deviceManagerTest] createDeviceManager error");
                }
                let token = data.pinToken;
                expect(token != null).assertTrue();
            });
        });
        console.log("----------stop running deviceManagerTest_authenticateDevice_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_authenticateDevice_0110
     * @tc.name    testcase for authenticateDevice in correct deviceType para with SPEAKER
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_authenticateDevice_0110', 0, async function (done) {
        console.log("----------start running deviceManagerTest_authenticateDevice_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let deviceInfo = {
                deviceId: "",
                deviceName: "",
                deviceType: SPEAKER
            }
            let extraInfo = {
                "targetPkgName": 'com.ohos.devicemangagerdemo',
                "appName": "myapplication",
                "appDescription": "apply auth",
                "business": '0',
                "displayOwner": 0
            }
            let authParam = {
                "authType": 1,
                "appIcon": new Uint8Array([]),
                "appThumbnail": new Uint8Array([]),
                "extraInfo": extraInfo
            }
            data.authenticateDevice(deviceInfo, authParam, (err, data) => {
                if (err) {
                    console.log("[deviceManagerTest] authenticateDevice error");
                    expect().assertFail();
                }
                let token = data.pinToken;
                expect(token != null).assertTrue();
            });
        });
        console.log("----------stop running deviceManagerTest_authenticateDevice_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_authenticateDevice_0120
     * @tc.name    testcase for authenticateDevice in correct deviceType para with PHONE
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_authenticateDevice_0120', 0, async function (done) {
        console.log("----------start running deviceManagerTest_authenticateDevice_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let deviceInfo = {
                deviceId: "",
                deviceName: "",
                deviceType: PHONE
            }
            let extraInfo = {
                "targetPkgName": 'com.ohos.devicemangagerdemo',
                "appName": "myapplication",
                "appDescription": "apply auth",
                "business": '0',
                "displayOwner": 0
            }
            let authParam = {
                "authType": 1,
                "appIcon": new Uint8Array([]),
                "appThumbnail": new Uint8Array([]),
                "extraInfo": extraInfo
            }
            data.authenticateDevice(deviceInfo, authParam, (err, data) => {
                if (err) {
                    console.log("[deviceManagerTest] authenticateDevice error");
                    expect().assertFail();
                }
                let token = data.pinToken;
                expect(token != null).assertTrue();
            });
        });
        console.log("----------stop running deviceManagerTest_authenticateDevice_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_authenticateDevice_0130
     * @tc.name    testcase for authenticateDevice in correct deviceType para with TABLET
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_authenticateDevice_0130', 0, async function (done) {
        console.log("----------start running deviceManagerTest_authenticateDevice_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let deviceInfo = {
                deviceId: "",
                deviceName: "",
                deviceType: TABLET
            }
            let extraInfo = {
                "targetPkgName": 'com.ohos.devicemangagerdemo',
                "appName": "myapplication",
                "appDescription": "apply auth",
                "business": '0',
                "displayOwner": 0
            }
            let authParam = {
                "authType": 1,
                "appIcon": new Uint8Array([]),
                "appThumbnail": new Uint8Array([]),
                "extraInfo": extraInfo
            }
            data.authenticateDevice(deviceInfo, authParam, (err, data) => {
                if (err) {
                    console.log("[deviceManagerTest] authenticateDevice error");
                    expect().assertFail();
                }
                let token = data.pinToken;
                expect(token != null).assertTrue();
            });
        });
        console.log("----------stop running deviceManagerTest_authenticateDevice_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_authenticateDevice_0140
     * @tc.name    testcase for authenticateDevice in correct deviceType para with CAR
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_authenticateDevice_0140', 0, async function (done) {
        console.log("----------start running deviceManagerTest_authenticateDevice_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let deviceInfo = {
                deviceId: "",
                deviceName: "",
                deviceType: CAR
            }
            let extraInfo = {
                "targetPkgName": 'com.ohos.devicemangagerdemo',
                "appName": "myapplication",
                "appDescription": "apply auth",
                "business": '0',
                "displayOwner": 0
            }
            let authParam = {
                "authType": 1,
                "appIcon": new Uint8Array([]),
                "appThumbnail": new Uint8Array([]),
                "extraInfo": extraInfo
            }
            data.authenticateDevice(deviceInfo, authParam, (err, data) => {
                if (err) {
                    console.log("[deviceManagerTest] authenticateDevice error");
                    expect().assertFail();
                }
                let token = data.pinToken;
                expect(token != null).assertTrue();
            });
        });
        console.log("----------stop running deviceManagerTest_authenticateDevice_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_authenticateDevice_0150
     * @tc.name    testcase for authenticateDevice in correct deviceType para with TV
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_authenticateDevice_0150', 0, async function (done) {
        console.log("----------start running deviceManagerTest_authenticateDevice_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let deviceInfo = {
                deviceId: "",
                deviceName: "",
                deviceType: TV
            }
            let extraInfo = {
                "targetPkgName": 'com.ohos.devicemangagerdemo',
                "appName": "myapplication",
                "appDescription": "apply auth",
                "business": '0',
                "displayOwner": 0
            }
            let authParam = {
                "authType": 1,
                "appIcon": new Uint8Array([]),
                "appThumbnail": new Uint8Array([]),
                "extraInfo": extraInfo
            }
            data.authenticateDevice(deviceInfo, authParam, (err, data) => {
                if (err) {
                    console.log("[deviceManagerTest] authenticateDevice error");
                    expect().assertFail();
                }
                let token = data.pinToken;
                expect(token != null).assertTrue();
            });
        });
        console.log("----------stop running deviceManagerTest_authenticateDevice_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_verifyAuthInfo_0100
     * @tc.name    testcase two for verifyAuthInfo in correct para 
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_authenticateDevice_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_authenticateDevice_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let authInfo = {
                "authType": 1,
                "token": 12,
                "extraInfo": {
                    "pinCode": 123456,
                }
            }
            data.verifyAuthInfo(authInfo, (err, data) => {
                if (err) {
                    expect().assertFail();
                }
                expect(data == undefined).assertTrue();
            });
        });
        console.log("----------stop running deviceManagerTest_authenticateDevice_0100----------");
        done();
    })

/*
     * @tc.number  deviceManagerTest_unAuthenticateDevice_0100
     * @tc.name    Pass in the normal deviceInfo、authParam,deviceType is PHONE
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('deviceManagerTest_unAuthenticateDevice_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_unAuthenticateDevice_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            setTimeout(function(){
                dmClass = data;
                dmUnAuthenticateDevice();
            })
        });
        async function dmUnAuthenticateDevice(){
            let deviceInfo = {
                deviceId: "",
                deviceName: "",
                deviceType: PHONE
            }
            let extraInfo = {
                "targetPkgName": 'com.ohos.devicemangagerdemo',
                "appName": "myapplication",
                "appDescription": "apply auth",
                "business": '0',
                "displayOwner": 0
            }
            let authParam = {
                "authType": 1,
                "appIcon": new Uint8Array([]),
                "appThumbnail": new Uint8Array([]),
                "extraInfo": extraInfo
            }
            let result = dmClass.unAuthenticateDevice(deviceInfo, authParam, (err, data) => {
                if (err) {
                    console.log("[deviceManagerTest] unAuthenticateDevice error");
                    expect().assertFail();
                }
            });
            expect(result == undefined).assertTrue()
        }
        console.log("----------stop running deviceManagerTest_unauthenticateDevice_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_getAuthenticationParam_0100
     * @tc.name    testcase for getAuthenticationParam
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_getAuthenticationParam_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_getAuthenticationParam_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let authparam = data.getAuthenticationParam();
            expect(authparam != null).assertTrue();
        });
        console.log("----------stop running deviceManagerTest_getAuthenticationParam_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_on-deviceStateChange_0100
     * @tc.name    testcase for deviceStateChange on with ONLINE
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_on-deviceStateChange_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_on-deviceStateChange_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let fa = data.on('deviceStateChange', (deviceStateChangeAction, deviceInfo) => {
                expect(deviceStateChangeAction.ONLINE == 0).assertTrue();
            });
            expect(fa != null).assertTrue();
        });
        console.log("----------stop running deviceManagerTest_on-deviceStateChange_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_on-deviceStateChange_0110
     * @tc.name    testcase for deviceStateChange on with OFFLINE
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_on-deviceStateChange_0110', 0, async function (done) {
        console.log("----------start running deviceManagerTest_on-deviceStateChange_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let fa = data.on('deviceStateChange', (deviceStateChangeAction, deviceInfo) => {
                expect(deviceStateChangeAction.OFFLINE == 0).assertTrue();
            });
            expect(fa != null).assertNotNull();
        });
        console.log("----------stop running deviceManagerTest_on-deviceStateChange_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_on-deviceStateChange_0120
     * @tc.name    testcase for deviceStateChange on with READY
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_on-deviceStateChange_0120', 0, async function (done) {
        console.log("----------start running deviceManagerTest_on-deviceStateChange_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let fa = data.on('deviceStateChange', (deviceStateChangeAction, deviceInfo) => {
                expect(deviceStateChangeAction.READY == 0).assertTrue();
            });
            expect(fa != null).assertTrue();
        });
        console.log("----------stop running deviceManagerTest_on-deviceStateChange_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_on-deviceStateChange_0130
     * @tc.name    testcase for deviceStateChange on with CHANGE
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_on-deviceStateChange_0130', 0, async function (done) {
        console.log("----------start running deviceManagerTest_on-deviceStateChange_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let fa = data.on('deviceStateChange', (deviceStateChangeAction, deviceInfo) => {
                expect(deviceStateChangeAction.CHANGE == 0).assertTrue();
            });
            expect(fa != null).assertTrue();
        });
        console.log("----------stop running deviceManagerTest_on-deviceStateChange_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_off-deviceStateChange_0100
     * @tc.name    testcase for deviceStateChange off
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_off-deviceStateChange_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_off-deviceStateChange_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let fa = data.off('deviceStateChange', () => {});
            expect(fa != null).assertTrue();
        });
        console.log("----------stop running deviceManagerTest_off-deviceStateChange_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_on-deviceFound_0100
     * @tc.name    testcase for deviceFound on
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_on-deviceFound_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_on-deviceFound_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let fa = data.on('deviceFound', () => {});
            expect(fa != null).assertTrue();
        });
        console.log("----------stop running deviceManagerTest_on-deviceFound_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_off-deviceFound_0100
     * @tc.name    testcase for deviceFound off
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_off-deviceFound_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_off-deviceFound_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let fa = data.off('deviceFound', () => {});
            expect(fa != null).assertTrue();
        });
        console.log("----------stop running deviceManagerTest_off-deviceFound_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_on-discoverFail_0100
     * @tc.name    testcase for discoverFail on
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_on-discoverFail_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_on-discoverFail_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let fa = data.on('discoverFail', () => {});
            expect(fa != null).assertTrue();
        });
        console.log("----------stop running deviceManagerTest_on-discoverFail_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_off-discoverFail_0100
     * @tc.name    testcase for discoverFail off
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_off-discoverFail_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_off-discoverFail_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let fa = data.off('discoverFail', () => {});
            expect(fa != null).assertTrue();
        });
        console.log("----------stop running deviceManagerTest_off-discoverFail_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_on-serviceDie_0100
     * @tc.name    testcase for serviceDie on
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_on-serviceDie_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_on-serviceDie_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let fa = data.on('serviceDie', () => {});
            expect(fa != null).assertTrue();
        });
        console.log("----------start running deviceManagerTest_on-serviceDie_0100----------");
        done();
    })

    /*
     * @tc.number  deviceManagerTest_off-serviceDie_0100
     * @tc.name    testcase for serviceDie off
     * @tc.desc    Function test
     * @tc.level   2
     */
    it('deviceManagerTest_off-serviceDie_0100', 0, async function (done) {
        console.log("----------start running deviceManagerTest_off-serviceDie_0100----------");
        deviceManager.createDeviceManager('com.ohos.devicemangagerdemo', (err, data) => {
            if (err) {
                console.log("[deviceManagerTest] createDeviceManager error");
                expect().assertFail();
            }
            let fa = data.off('serviceDie', () => {});
            expect(fa != null).assertTrue();
        });
        console.log("----------stop running deviceManagerTest_off-serviceDie_0100----------");
        done();
    })
})