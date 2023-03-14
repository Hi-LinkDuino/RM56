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
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

describe('ActsBmsHapModuleTest', function () {

    /*
     * @tc.number: bms_getHapModuleInfo_0100
     * @tc.name: get hapModuleInfo from one app by getBundleInfo
     * @tc.desc: get the module information of the hap with type of entry
     */
    it('bms_getHapModuleInfo_0100', 0, async function (done) {
        console.debug('===========begin bms_getHapModuleInfo_0100===========')
        let bundlePath = ['/data/test/bmsMainAbilityFirstScene.hap'];
        let bundleName = 'com.example.bmsmainabilityfirstscene';
        let installer = await bundle.getBundleInstaller();
        installer.install(bundlePath, {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveInstallEvent);
        function onReceiveInstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            bundle.getBundleInfo(bundleName, 1, callback);
            async function callback(err, result) {
                expect(result.hapModuleInfos.length).assertEqual(1);
                if (result.hapModuleInfos.length > 0) {
                    let hapModuleInfo = result.hapModuleInfos[0];
                    expect(hapModuleInfo.moduleName).assertEqual('entry');
                    expect(hapModuleInfo.mainAbilityName).assertEqual('com.example.bmsmainabilityfirstscene.MainAbility');
                    expect(hapModuleInfo.mainElementName).assertEqual('com.example.bmsmainabilityfirstscene.MainAbility');
                    expect(hapModuleInfo.name).assertEqual('com.example.bmsmainabilityfirstscene');
                    expect(hapModuleInfo.description).assertEqual('');
                    expect(hapModuleInfo.descriptionId).assertEqual(0);
                    expect(hapModuleInfo.icon).assertEqual('');
                    expect(hapModuleInfo.label).assertEqual('$string:app_name');
                    expect(hapModuleInfo.labelId).assertEqual(0);
                    expect(hapModuleInfo.iconId).assertEqual(0);
                    expect(hapModuleInfo.backgroundImg).assertEqual('');
                    expect(hapModuleInfo.supportedModes).assertEqual(0);
                    console.info('hapModuleInfo.reqCapabilities' + JSON.stringify(hapModuleInfo.reqCapabilities));
                    expect(typeof hapModuleInfo.reqCapabilities).assertEqual('object');
                    console.info('===hapModuleInfo devicetypes===' + JSON.stringify(hapModuleInfo.deviceTypes));
                    expect(hapModuleInfo.deviceTypes[0]).assertEqual('phone');
                    console.info('===hapModuleInfo.abilityInfo===' + JSON.stringify(hapModuleInfo.abilityInfo));
                    expect(typeof hapModuleInfo.abilityInfo).assertEqual('object');
                    expect(hapModuleInfo.moduleName).assertEqual('entry');
                    expect(hapModuleInfo.mainAbilityName).assertEqual('com.example.bmsmainabilityfirstscene.MainAbility');
                    expect(hapModuleInfo.installationFree).assertEqual(false);
                    for (let i = 0, len = hapModuleInfo.reqCapabilities.length; i < len; i++) {
                        expect(hapModuleInfo.reqCapabilities[i]).assertEqual('');
                    }
                    for (let j = 0, len = hapModuleInfo.abilityInfo.length; j < len; j++) {
                        expect(hapModuleInfo.abilityInfo[j].name).assertEqual('com.example.bmsmainabilityfirstscene.MainAbility');
                    }
                }
                installer.uninstall(bundleName, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            }
        }
    });

    /*
     * @tc.number: bms_getHapModuleInfo_0200
     * @tc.name: get hapModuleInfo from two modules by getBundleInfo
     * @tc.desc: get the module information of the hap with type of feature
     */
    it('bms_getHapModuleInfo_0200', 0, async function (done) {
        console.debug('===========begin bms_getHapModuleInfo_0200===========')
        let bundlePath = ['/data/test/bmsMainAbilityFirstScene.hap', '/data/test/bmsMainAbilitySecondScene.hap'];
        let bundleName = 'com.example.bmsmainabilityfirstscene';
        let firstMainAbility = 'com.example.bmsmainabilityfirstscene.MainAbility';
        let secondMainAbility = 'com.example.bmsmainabilitysecondscene.MainAbility';
        let installer = await bundle.getBundleInstaller();
        installer.install(bundlePath, {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            bundle.getBundleInfo(bundleName, 1, async (err, result) => {
                expect(result.hapModuleInfos.length).assertEqual(2);
                if (result.hapModuleInfos.length == 2) {
                    expect(result.hapModuleInfos[0].mainAbilityName).assertEqual(firstMainAbility);
                    expect(result.hapModuleInfos[0].moduleName).assertEqual('entry');
                    checkHapModuleInfo(result.hapModuleInfos[0]);
                    expect(result.hapModuleInfos[1].mainAbilityName).assertEqual(secondMainAbility);
                    expect(result.hapModuleInfos[1].moduleName).assertEqual('bmsmainabilitysecondscene');
                    checkHapModuleInfo(result.hapModuleInfos[1]);
                }
                installer.uninstall(bundleName, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            });
        });
    })

    /*
     * @tc.number: bms_getHapModuleInfo_0300
     * @tc.name: get hapModuleInfo one app without mainAbility by getBundleInfo
     * @tc.desc: get the module information of the hap without mainAbility
     */
    it('bms_getHapModuleInfo_0300', 0, async function (done) {
        console.debug('===========begin bms_getHapModuleInfo_0300===========')
        let bundlePath = ['/data/test/bmsThirdBundleTest2.hap'];
        let bundleName = 'com.example.third2';
        let installer = await bundle.getBundleInstaller();
        console.log('========install========' + typeof installer);
        installer.install(bundlePath, {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveInstallEvent);
        function onReceiveInstallEvent(err, data) {
            console.info('========install Finish========');
            expect(typeof err).assertEqual('object');
            expect(err.code).assertEqual(0);
            expect(typeof data).assertEqual('object');
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            bundle.getBundleInfo(bundleName, 1,).then(async (result) => {
                console.debug('=======get hapModule========' + JSON.stringify(result))
                expect(result.hapModuleInfos.length).assertEqual(1);
                if (result.hapModuleInfos.length > 0) {
                    console.debug('=======get hapModule mainAbilityName========' + result.hapModuleInfos[0].mainAbilityName)
                    expect(result.hapModuleInfos[0].mainAbilityName).assertEqual('');
                    expect(result.hapModuleInfos[0].moduleName).assertEqual('entry');
                    checkHapModuleInfo(result.hapModuleInfos[0]);
                }
                installer.uninstall(bundleName, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, onReceiveUninstallEvent);

                function onReceiveUninstallEvent(err, data) {
                    console.info('========uninstall Finish========');
                    expect(typeof err).assertEqual('object');
                    expect(err.code).assertEqual(0);
                    expect(typeof data).assertEqual('object');
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                }
            });
        }
    })

    /*
     * @tc.number: bms_getHapModuleInfo_0400
     * @tc.name: get hapModuleInfo through getBundleInfo, an application that adds mainAbility
     * @tc.desc: get the module information of the hap with the added field mainAbility
     */
    it('bms_getHapModuleInfo_0400', 0, async function (done) {
        console.debug('===========begin bms_getHapModuleInfo_0400===========')
        let bundlePath = ['/data/test/bmsThirdBundleTest5.hap'];
        let bundleName = 'com.example.third5';
        let installer = await bundle.getBundleInstaller();
        console.log('========install========' + typeof installer);
        installer.install(bundlePath, {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, (err, data) => {
            console.info('========install Finish========');
            expect(typeof err).assertEqual('object');
            expect(err.code).assertEqual(0);
            expect(typeof data).assertEqual('object');
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            bundle.getBundleInfo(bundleName, 1, async (err, result) => {
                console.debug('=======get hapModule========' + JSON.stringify(result))
                expect(result.hapModuleInfos.length).assertEqual(1);
                if (result.hapModuleInfos.length == 1) {
                    expect(result.hapModuleInfos[0].mainAbilityName).assertEqual('com.example.third5.AMainAbility');
                    expect(result.hapModuleInfos[0].moduleName).assertEqual('entry');
                    checkHapModuleInfo(result.hapModuleInfos[0]);
                }
                installer.uninstall(bundleName, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, onReceiveUninstallEvent);

                function onReceiveUninstallEvent(err, data) {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                }
            });
        })
    })

    /*
     * @tc.number: bms_getHapModuleInfo_0500
     * @tc.name: get the hapModuleInfo of the upgraded hap package from an application through getBundleInfo
     * @tc.desc: get the module information of the mainAbility upgrade hap
     */
    it('bms_getHapModuleInfo_0500', 0, async function (done) {
        console.debug('===========begin bms_getHapModuleInfo_0500===========')
        let bundlePath1 = ['/data/test/bmsThirdBundleTest1.hap'];
        let bundlePath2 = ['/data/test/bmsThirdBundleTestA1.hap'];
        let bundleName = 'com.example.third1';
        let installer = await bundle.getBundleInstaller();
        installer.install(bundlePath1, {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            installer.install(bundlePath2, {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                bundle.getBundleInfo(bundleName, 1, callback);
            })
        })
        function callback(err, result) {
            expect(result.hapModuleInfos.length).assertEqual(1);
            if (result.hapModuleInfos.length == 1) {
                let hapModuleInfo = result.hapModuleInfos[0];
                expect(hapModuleInfo.moduleName).assertEqual('entry');
                expect(hapModuleInfo.mainAbilityName).assertEqual('com.example.third1.AMainAbility');
                checkHapModuleInfo(hapModuleInfo);
            }
            installer.uninstall(bundleName, {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                done();
            })
        }
    });

    /*
     * @tc.number: bms_getHapModuleInfo_0600
     * @tc.name: get hapModuleInfo from the system through getBundleInfo 
     * @tc.desc: get module information of mainAbility system application 
     */
    it('bms_getHapModuleInfo_0600', 0, async function (done) {
        console.debug('===========begin bms_getHapModuleInfo_0600===========')
        let bundleName = 'com.example.system1';
        bundle.getBundleInfo(bundleName, 1, callback);
        function callback(err, data) {
            expect(data.hapModuleInfos.length).assertEqual(1);
            if (data.hapModuleInfos.length == 1) {
                let hapModuleInfo = data.hapModuleInfos[0];
                console.debug('=======get hapModule========' + JSON.stringify(hapModuleInfo))
                console.debug('=======get hapModule mainAbilityName========' + hapModuleInfo.mainAbilityName)
                expect(hapModuleInfo.moduleName).assertEqual('entry');
                expect(hapModuleInfo.mainAbilityName).assertEqual('com.example.system1.MainAbility');
                checkHapModuleInfo(hapModuleInfo);
            }
            done();
        }
    });

    function checkHapModuleInfo(dataInfo) {
        expect(typeof dataInfo.name).assertEqual('string');
        expect(typeof dataInfo.description).assertEqual('string');
        expect(typeof dataInfo.descriptionId).assertEqual('number');
        expect(typeof dataInfo.icon).assertEqual('string');
        expect(typeof dataInfo.label).assertEqual('string');
        expect(typeof dataInfo.labelId).assertEqual('number');
        expect(typeof dataInfo.iconId).assertEqual('number');
        expect(typeof dataInfo.backgroundImg).assertEqual('string');
        expect(typeof dataInfo.supportedModes).assertEqual('number');
        expect(typeof dataInfo.reqCapabilities).assertEqual('object');
        expect(typeof dataInfo.deviceTypes).assertEqual('object');
        expect(typeof dataInfo.abilityInfo).assertEqual('object');
        expect(typeof dataInfo.moduleName).assertEqual('string');
        expect(typeof dataInfo.mainAbilityName).assertEqual('string');
        expect(typeof dataInfo.installationFree).assertEqual('boolean');
    }
})