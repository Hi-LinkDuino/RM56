/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import bundle from '@ohos.bundle'
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit'

const BUNDLE_PATH1 = '/data/test/bmsThirdBundleTest1.hap';
const BUNDLE_PATH2 = '/data/test/bmsThirdBundleTest2.hap';
const BUNDLE_PATH3 = '/data/test/bmsThirdBundleTest3.hap';
const BUNDLE_PATH4 = '/data/test/bmsThirdBundleTest4.hap';
const BUNDLE_PATH5 = '/data/test/bmsThirdBundleTest5.hap';
const BUNDLE_PATH6 = '/data/test/bmsThirdBundleTest6.hap';
const BUNDLE_PATHUPDATE = '/data/test/bmsThirdBundleTestA1.hap';
const SYSTEM_PATH = '/data/test/bmsSystemBundleTest2.hap';
const BUNDLE_NAME1 = 'com.example.third1';
const BUNDLE_NAME2 = 'com.example.third2';
const BUNDLE_NAME4 = 'com.example.third4';
const BUNDLE_NAME5 = 'com.example.third5';
const BUNDLE_NAME6 = 'com.example.third6';
const SYSTEM_NAME = 'com.example.system2';
const NUM_TWO = 2;
const NUM_THREE = 3;
const NUM_FOUR = 4;
const NUM_NINE = 9;
let dataTransfer = 1;
let audioPlayback = 2;
let audioRecording = 4;
let location = 8;
let bluetoothInteraction = 16;
let multiDeviceConnection = 32;
let wifiInteraction = 64;
let voip = 128;
let taskKeeping = 256;
const USERID = 100;

let installParam = {
    userId: USERID,
    installFlag: 1,
    isKeepData: false
}

function checkInstallOrUninstall(err, data) {
    expect(err.code).assertEqual(0);
    expect(data.status).assertEqual(0);
    expect(data.statusMessage).assertEqual('SUCCESS');
}

describe('ActsBmsGetBackGroundModes', function () {

    /*
    * @tc.number: bms_backGroundModes_0100
    * @tc.name: Get the backgroundModes information of the application through queryAbilityByWant
    * @tc.desc: Get the information of the background modes from multi-ability application
    */
    it('bms_backGroundModes_0100', 0, async function (done) {
        console.info('=====================bms_backGroundModes_0100==================');
        let bundlePath = [BUNDLE_PATH5]
        let installer = await bundle.getBundleInstaller();
        installer.install(bundlePath, installParam, async (err, data) => {
            checkInstallOrUninstall(err, data);
            let dataInfos = await bundle.queryAbilityByWant({
                action: 'action.system.home',
                entities: ['entity.system.home'],
                elementName: {
                    deviceId: '0',
                    bundleName: BUNDLE_NAME5,
                    abilityName: '',
                },
            }, 0, USERID)
            expect(dataInfos.length).assertEqual(NUM_FOUR);
            if (dataInfos.length == NUM_FOUR) {
                expect(dataInfos[NUM_TWO].name).assertEqual("com.example.third5.MainAbilityA");
                expect(dataInfos[NUM_TWO].backgroundModes).assertEqual(dataTransfer | audioPlayback | audioRecording |
                    location | bluetoothInteraction | multiDeviceConnection | wifiInteraction | voip | taskKeeping);
                expect(dataInfos[NUM_THREE].name).assertEqual("com.example.third5.MainAbilityB");
                expect(dataInfos[NUM_THREE].backgroundModes).assertEqual(dataTransfer | voip);
            }
            let bundleInfos = await bundle.getAllBundleInfo(1, USERID)
            for (let i = 0; i < bundleInfos.length; i++) {
                console.info("========>bundleInfos[i].name======" + bundleInfos[i].name)
                console.info("========>bundleInfos[i]======" + JSON.stringify(bundleInfos[i]));
                if (bundleInfos[i].name == BUNDLE_NAME5) {
                    console.info("====bundleInfos[i].abilityinfos===" + JSON.stringify(bundleInfos[i].abilityInfos))
                    for (let j = 0; j < bundleInfos[i].abilityInfos.length; j++) {
                        console.info("========>bundleInfos[i].abilityinfos[j].name======"
                            + bundleInfos[i].abilityInfos[j].name)
                        if (bundleInfos[i].abilityInfos[j].name == 'com.example.third5.MainAbilityA') {
                            expect(bundleInfos[i].abilityInfos[j].backgroundModes).assertEqual(dataTransfer | 
                                audioPlayback | audioRecording | location | bluetoothInteraction | 
                                multiDeviceConnection | wifiInteraction | voip | taskKeeping);
                        } else if (bundleInfos[i].abilityInfos[j].name == 'com.example.third5.MainAbilityB') {
                            expect(bundleInfos[i].abilityInfos[j].backgroundModes).assertEqual(dataTransfer | voip);
                        }
                    }
                }

            }
            let data3 = await bundle.getBundleInfo(BUNDLE_NAME5, 1)
            console.info("========>data3======" + data3.abilityInfos.length)
            for (let q = 0; q < data3.abilityInfos.length; q++) {
                console.info("=====>data3.abilityInfos======" + JSON.stringify(data3.abilityInfos[2].backgroundModes))
                console.info("=====>data3.abilityInfos======" + JSON.stringify(data3.abilityInfos[3].backgroundModes))
            }
            expect(data3.abilityInfos[2].backgroundModes).assertEqual(dataTransfer | audioPlayback | audioRecording |
                location | bluetoothInteraction | multiDeviceConnection | wifiInteraction | voip | taskKeeping);
            expect(data3.abilityInfos[3].backgroundModes).assertEqual(dataTransfer | voip);

            let data5 = await bundle.getBundleArchiveInfo(BUNDLE_PATH5, 1)
            console.info("========>data5======" + data5.abilityInfos.length)
            for (let i = 0; i < data5.abilityInfos.length; i++) {
                console.info("=====>data5.abilityInfos======" + JSON.stringify(data5.abilityInfos[2].backgroundModes))
                console.info("=====>data5.abilityInfos======" + JSON.stringify(data5.abilityInfos[3].backgroundModes))
            }
            expect(data5.abilityInfos[2].backgroundModes).assertEqual(dataTransfer | audioPlayback | audioRecording |
                location | bluetoothInteraction | multiDeviceConnection | wifiInteraction | voip | taskKeeping);
            expect(data5.abilityInfos[3].backgroundModes).assertEqual(dataTransfer | voip);
            installer.uninstall(BUNDLE_NAME5, installParam, (err, data) => {
                checkInstallOrUninstall(err, data);
                done();
            });
        })
    })

    /*
    * @tc.number: bms_backGroundModes_0200
    * @tc.name: Get the backgroundModes information of the application through queryAbilityByWant 
    * @tc.desc: Get all background modes information, and each ability of the application
    *               contains one of the background mode
    */
    it('bms_backGroundModes_0200', 0, async function (done) {
        console.info('=====================bms_backGroundModes_0200==================');
        let bundlePath = [BUNDLE_PATH6]
        let installer = await bundle.getBundleInstaller();
        installer.install(bundlePath, installParam, async (err, data) => {
            checkInstallOrUninstall(err, data);
            let dataInfos = await bundle.queryAbilityByWant({
                action: 'action.system.home',
                entities: ['entity.system.home'],
                elementName: {
                    deviceId: '0',
                    bundleName: BUNDLE_NAME6,
                    abilityName: '',
                },
            }, 0, USERID)
            expect(dataInfos.length).assertEqual(NUM_NINE);
            for (let i = 0, len = dataInfos.length; i < len; i++) {
                expect(dataInfos[i].backgroundModes).assertEqual(1 << i);
                console.info("==========dataInfos[i].backgroundModes=========" + dataInfos[i].backgroundModes);
                console.info("==========dataInfos[1].name=========" + dataInfos[i].name);
            }
            installer.uninstall(BUNDLE_NAME6, installParam, (err, data) => {
                checkInstallOrUninstall(err, data);
                done();
            });
        })
    })

    /*
    * @tc.number: bms_backGroundModes_0300
    * @tc.name: Get the backgroundModes information of the application through queryAbilityByWant 
    * @tc.desc: Read the backgroundModes information of the app's ability and replace invalid attributes 
    */
    it('bms_backGroundModes_0300', 0, async function (done) {
        console.info('=====================bms_backGroundModes_0300==================');
        let bundlePath = [BUNDLE_PATH2]
        let installer = await bundle.getBundleInstaller();
        installer.install(bundlePath, installParam, async (err, data) => {
            checkInstallOrUninstall(err, data);
            let dataInfos = await bundle.queryAbilityByWant({
                action: 'action.system.home',
                entities: ['entity.system.home'],
                elementName: {
                    deviceId: '0',
                    bundleName: BUNDLE_NAME2,
                    abilityName: '',
                },
            }, 0, USERID)
            expect(dataInfos.length).assertEqual(NUM_TWO);
            if (dataInfos.length == NUM_TWO) {
                expect(dataInfos[1].name).assertEqual("com.example.third2.MainAbilityA")
                expect(dataInfos[1].backgroundModes).assertEqual(audioPlayback | audioRecording | location
                    | bluetoothInteraction | multiDeviceConnection | wifiInteraction | voip | taskKeeping)
            }
            bundle.getAllBundleInfo(1, USERID, (err, bundleInfos) => {
                for (let i = 0; i < bundleInfos.length; i++) {
                    console.info("========>bundleInfos[i].name======" + bundleInfos[i].name)
                    console.info("========>bundleInfos[i]======" + JSON.stringify(bundleInfos[i]));
                    if (bundleInfos[i].name == BUNDLE_NAME2) {
                        console.info("==bundleInfos[i].abilityinfos===" + JSON.stringify(bundleInfos[i].abilityInfos))
                        for (let j = 0; j < bundleInfos[i].abilityInfos.length; j++) {
                            console.info("========>bundleInfos[i].abilityinfos[j].name======"
                                + bundleInfos[i].abilityInfos[j].name)
                            if (bundleInfos[i].abilityInfos[j].name == 'com.example.third2.MainAbilityA') {
                                expect(bundleInfos[i].abilityInfos[j].backgroundModes).assertEqual(
                                    audioPlayback | audioRecording| location | bluetoothInteraction 
                                    | multiDeviceConnection | wifiInteraction | voip | taskKeeping);
                            }
                        }
                    }

                }
            })
            bundle.getBundleInfo(BUNDLE_NAME2, 1, (err, data3) => {
                expect(data3.abilityInfos[1].backgroundModes).assertEqual(audioPlayback | audioRecording | location
                    | bluetoothInteraction | multiDeviceConnection | wifiInteraction | voip | taskKeeping)
            })
            bundle.getBundleArchiveInfo(BUNDLE_PATH2, 1, (err, data5) => {
                expect(data5.abilityInfos[1].backgroundModes).assertEqual(audioPlayback | audioRecording | location
                    | bluetoothInteraction | multiDeviceConnection | wifiInteraction | voip | taskKeeping)
            })
            installer.uninstall(BUNDLE_NAME2, installParam, (err, data) => {
                checkInstallOrUninstall(err, data);
                done();
            });
        })
    })

    /*
    * @tc.number: bms_backGroundModes_0400
    * @tc.name: Get the backgroundModes information of the application through queryAbilityByWant 
    * @tc.desc: Read the backgroundModes information of the app's ability and replace invalid attributes 
    */
    it('bms_backGroundModes_0400', 0, async function (done) {
        console.info('=====================bms_backGroundModes_0400==================');
        let bundlePath1 = [BUNDLE_PATH4]
        let installer = await bundle.getBundleInstaller();
        installer.install(bundlePath1, installParam, async (err, data) => {
            checkInstallOrUninstall(err, data);
            let dataInfos = await bundle.queryAbilityByWant({
                action: 'action.system.home',
                entities: ['entity.system.home'],
                elementName: {
                    deviceId: '0',
                    bundleName: BUNDLE_NAME4,
                    abilityName: '',
                },
            }, 0, USERID)
            expect(dataInfos.length).assertEqual(1);
            if (dataInfos.length == 1) {
                expect(dataInfos[0].name).assertEqual("com.example.third4.MainAbility")
                expect(dataInfos[0].backgroundModes).assertEqual(0)
            }
            installer.uninstall(BUNDLE_NAME4, installParam, (err, data) => {
                checkInstallOrUninstall(err, data);
                done();
            });
        })
    })

    /*
    * @tc.number: bms_backGroundModes_0500
    * @tc.name: Get the backgroundModes information of the application through queryAbilityByWant 
    * @tc.desc: Get the backgroundModes information of the multi-hap package of the application 
    */
    it('bms_backGroundModes_0500', 0, async function (done) {
        console.info('=====================bms_backGroundModes_0500==================');
        let bundlePath = [BUNDLE_PATH1, BUNDLE_PATH3];
        let installer = await bundle.getBundleInstaller();
        installer.install(bundlePath, installParam, async (err, data) => {
            checkInstallOrUninstall(err, data);
            let dataInfos = await bundle.queryAbilityByWant({
                action: 'action.system.home',
                entities: ['entity.system.home'],
                elementName: {
                    deviceId: '0',
                    bundleName: BUNDLE_NAME1,
                    abilityName: '',
                },
            }, 0, USERID)
            expect(dataInfos.length).assertEqual(NUM_FOUR);
            if (dataInfos.length == NUM_FOUR) {
                expect(dataInfos[1].name).assertEqual("com.example.third1.MainAbilityA")
                expect(dataInfos[1].backgroundModes).assertEqual(dataTransfer | audioPlayback | audioRecording |
                    location | bluetoothInteraction | multiDeviceConnection | wifiInteraction | voip | taskKeeping)
                expect(dataInfos[3].name).assertEqual("com.example.third3.MainAbilityA")
                expect(dataInfos[3].backgroundModes).assertEqual(dataTransfer | audioPlayback | audioRecording |
                    location | bluetoothInteraction | multiDeviceConnection | wifiInteraction | voip | taskKeeping)
            }
            console.info("========dataInfos[0].backgroundModes=======>" + dataInfos[0].backgroundModes)
            installer.uninstall(BUNDLE_NAME1, installParam, (err, data) => {
                checkInstallOrUninstall(err, data);
                done();
            });
        })
    })

    /*
    * @tc.number: bms_backGroundModes_0600
    * @tc.name: Get the backgroundModes information of the application through queryAbilityByWant
    * @tc.desc: Get the backgroundModes information of the upgraded application's ability 
    */
    it('bms_backGroundModes_0600', 0, async function (done) {
        console.info('=====================bms_backGroundModes_0600==================');
        let bundlePath1 = [BUNDLE_PATH1]
        let bundlePath2 = [BUNDLE_PATHUPDATE]
        let installer = await bundle.getBundleInstaller();
        installer.install(bundlePath1, installParam, async (err, data) => {
            checkInstallOrUninstall(err, data);
            let dataInfos = await bundle.queryAbilityByWant({
                action: 'action.system.home',
                entities: ['entity.system.home'],
                elementName: {
                    deviceId: '0',
                    bundleName: BUNDLE_NAME1,
                    abilityName: '',
                },
            }, 0, USERID)
            expect(dataInfos.length).assertEqual(NUM_TWO);
            if (dataInfos.length == NUM_TWO) {
                expect(dataInfos[1].name).assertEqual("com.example.third1.MainAbilityA")
                expect(dataInfos[1].backgroundModes).assertEqual(dataTransfer | audioPlayback | audioRecording |
                    location | bluetoothInteraction | multiDeviceConnection | wifiInteraction | voip | taskKeeping)
            }
            installer.install(bundlePath2, installParam, async (err, data) => {
                checkInstallOrUninstall(err, data);
                let dataInfos = await bundle.queryAbilityByWant({
                    action: 'action.system.home',
                    entities: ['entity.system.home'],
                    elementName: {
                        deviceId: '0',
                        bundleName: BUNDLE_NAME1,
                        abilityName: '',
                    },
                }, 0, USERID)
                expect(dataInfos.length).assertEqual(NUM_TWO);
                if (dataInfos.length == NUM_TWO) {
                    expect(dataInfos[1].name).assertEqual("com.example.third1.AMainAbilityA");
                    expect(dataInfos[1].backgroundModes).assertEqual(audioRecording | location | bluetoothInteraction |
                        multiDeviceConnection | wifiInteraction | voip | taskKeeping);
                }
                installer.uninstall(BUNDLE_NAME1, installParam, (err, data) => {
                    checkInstallOrUninstall(err, data);
                    done();
                });
            })
        })
    })

    /*
    * @tc.number: bms_queryAbilityByEntities_0100
    * @tc.name: Use the implicit query method in queryAbilityByWant to get abilityInfos 
    * @tc.desc: The entities in the parameter want pass in the new field, and use the implicit query to get abilitInfos
    */
    it('bms_queryAbilityByEntities_0100', 0, async function (done) {
        console.info('===========begin bms_queryAbilityByEntities_0100===========');
        let installer = await bundle.getBundleInstaller()
        installer.install([SYSTEM_PATH], installParam, async (err, data) => {
            checkInstallOrUninstall(err, data);
            let dataInfos = await bundle.queryAbilityByWant({
                action: 'action.system.home',
                entities: ["entity.app.music",
                    "entity.app.email",
                    "entity.app.contacts",
                    "entity.app.maps",
                    "entity.app.browser",
                    "entity.app.calendar",
                    "entity.app.messaging",
                    "entity.app.files",
                    "entity.app.gallery"],
                elementName: {
                    deviceId: '0',
                    bundleName: '',
                    abilityName: '',
                },
            }, 0, USERID)
            console.log('=========ability info is=========' + JSON.stringify(dataInfos));
            expect(dataInfos.length).assertEqual(1);
            expect(dataInfos[0].name).assertEqual('com.example.system2.MainAbility');
            installer.uninstall(SYSTEM_NAME, installParam, (err, data) => {
                checkInstallOrUninstall(err, data);
                done();
            });
        });
    })

})