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

describe('ActsBmsMetaDataTest', function () {

    /*
    * @tc.number: bms_getMetaData_0100
    * @tc.name: test to get meta data for an application.
    * @tc.desc: get an application's meta data which contains two HAP packages.
    */
    it('bms_getMetaData_0100', 0, async function (done) {
        console.info('=====================bms_getMetaData_0100==================');
        let bundlePath = ['/data/test/bmsThirdBundleTest1.hap']
        let installer = await bundle.getBundleInstaller();
        let abilityName1 = 'com.example.third1.MainAbility';
        let dataMap = new Map();
        installer.install(bundlePath, {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            bundle.queryAbilityByWant(
                {
                    "bundleName": 'com.example.third1',
                    "abilityName": 'com.example.third1.MainAbility',
                }, bundle.BundleFlag.GET_ABILITY_INFO_WITH_METADATA, 100).then(dataInfos => {
                    for (let i = 0, len = dataInfos.length; i < len; i++) {
                        dataMap.set(dataInfos[i].name, dataInfos[i].metaData)
                    }
                })
            if (dataMap.has(abilityName1)) {
                checkMetaData(dataMap.get(abilityName1), 'Data1');
            }
            installer.uninstall('com.example.third1', {
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

    })

    /*
    * @tc.number: bms_getMetaData_0200
    * @tc.name: test to get meta data for an update application.
    * @tc.desc: get an application's meta data that is updated.
    */
    it('bms_getMetaData_0200', 0, async function (done) {
        console.info('=====================bms_getMetaData_0200==================');
        let bundlePath1 = ['/data/test/bmsThirdBundleTest1.hap']
        let bundlePath2 = ['/data/test/bmsThirdBundleTestA1.hap']
        let dataMap = new Map();
        let abilityName1 = 'com.example.third1.AMainAbility';
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
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                bundle.queryAbilityByWant(
                    {
                        "bundleName": 'com.example.third1',
                        "abilityName": 'com.example.third1.AMainAbility',
                    }, bundle.BundleFlag.GET_ABILITY_INFO_WITH_METADATA, 100).then(dataInfos => {
                        for (let i = 0, len = dataInfos.length; i < len; i++) {
                            dataMap.set(dataInfos[i].name, dataInfos[i].metaData)
                        }
                    })
                if (dataMap.has(abilityName1)) {
                    checkMetaData(dataMap.get(abilityName1), 'DataA1');
                }
                installer.uninstall('com.example.third1', {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            })
        })
    })

    /*
    * @tc.number: bms_getMetaData_0300
    * @tc.name: test to get meta data for an uninstalled application.
    * @tc.desc: get an application's meta data that is uninstalled.
    */
    it('bms_getMetaData_0300', 0, async function (done) {
        console.info('=====================bms_getMetaData_0300==================');
        let bundlePath = ['/data/test/bmsThirdBundleTest1.hap']
        let bundleName = 'com.example.third1';
        let installer = await bundle.getBundleInstaller();
        installer.install(bundlePath, {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            installer.uninstall(bundleName, {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                bundle.queryAbilityByWant(
                    {
                        bundleName: 'com.example.third1',
                        abilityName: 'com.example.third1.MainAbility',
                    }, bundle.BundleFlag.GET_ABILITY_INFO_WITH_METADATA, 100).then(dataInfos => {
                        expect(dataInfos.length).assertEqual(0);
                    });
                done();
            });
        });
    })

    /*
    * @tc.number: bms_getMetaData_0400
    * @tc.name: test to get meta data for an application.
    * @tc.desc: get an application's meta data which has one ability.
    */
    it('bms_getMetaData_0400', 0, async function (done) {
        console.info('=====================bms_getMetaData_0400==================');
        let bundlePath = ['/data/test/bmsThirdBundleTest5.hap']
        let dataMap = new Map();
        let abilityName1 = 'com.example.third5.AMainAbility';
        let installer = await bundle.getBundleInstaller();
        installer.install(bundlePath, {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);
        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            bundle.queryAbilityByWant(
                {
                    bundleName: 'com.example.third5',
                    abilityName: 'com.example.third5.AMainAbility',
                }, bundle.BundleFlag.GET_ABILITY_INFO_WITH_METADATA, 100).then(dataInfos => {
                    for (let i = 0, len = dataInfos.length; i < len; i++) {
                        dataMap.set(dataInfos[i].name, dataInfos[i].metaData)
                    }
                })
            if (dataMap.has(abilityName1)) {
                checkMetaData(dataMap.get(abilityName1), 'Data5A');
            }
            installer.uninstall('com.example.third5', {
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
    })

    /*
    * @tc.number: bms_getMetaData_0500
    * @tc.name: test to get meta data for an application that does not exist.
    * @tc.desc: get an application's meta data which does not exist.
    */
    it('bms_getMetaData_0500', 0, async function (done) {
        console.info('=====================bms_getMetaData_0500==================');
        bundle.queryAbilityByWant(
            {
                bundleName: 'com.example.noexist',
                abilityName: 'com.example.noexist.MainAbility',
            }, bundle.BundleFlag.GET_ABILITY_INFO_WITH_METADATA, 100).then(dataInfos => {
                expect(dataInfos.length).assertEqual(0);
            })
        done();
    })

    /*
    * @tc.number: bms_getMetaData_0600
    * @tc.name: test to get meta data for a system application.
    * @tc.desc: get a system application's meta data.
    */
    it('bms_getMetaData_0600', 0, async function (done) {
        console.info('=====================bms_getMetaData_0600==================');
        let dataMap = new Map();
        let abilityName1 = 'com.example.system1.MainAbility';
        bundle.queryAbilityByWant(
            {
                bundleName: 'com.example.system1',
                abilityName: 'com.example.system1.MainAbility',
            }, bundle.BundleFlag.GET_ABILITY_INFO_WITH_METADATA, 100).then(dataInfos => {
                for (let i = 0, len = dataInfos.length; i < len; i++) {
                    dataMap.set(dataInfos[i].name, dataInfos[i].metaData)
                }
            })
        if (dataMap.has(abilityName1)) {
            checkMetaData(dataMap.get(abilityName1), 'Data1S');
        }
        done();
    })

    /*
    * @tc.number: bms_getMetaData_0700
    * @tc.name: test to get meta data for a vendor application.
    * @tc.desc: get a vendor application's meta data.
    */
    it('bms_getMetaData_0700', 0, async function (done) {
        console.info('=====================bms_getMetaData_0700==================');
        let dataMap = new Map();
        let abilityName1 = 'com.example.vendor1.MainAbility';
        bundle.queryAbilityByWant(
            {
                bundleName: 'com.example.vendor1',
                abilityName: 'com.example.vendor1.MainAbility',
            }, bundle.BundleFlag.GET_ABILITY_INFO_WITH_METADATA, 100).then(dataInfos => {
                for (let i = 0, len = dataInfos.length; i < len; i++) {
                    dataMap.set(dataInfos[i].name, dataInfos[i].metaData)
                }
            });
        if (dataMap.has(abilityName1)) {
            let data = dataMap.get(abilityName1);
            let parameters = data.parameters;
            let results = data.results;
            let customizeDatas = data.customizeDatas;
            expect(parameters.length).assertEqual(2);
            expect(results.length).assertEqual(1);
            expect(customizeDatas.length).assertEqual(1);
            console.debug('=====customizeDatas length=====' + customizeDatas.length);
            for (let i = 0; i < parameters.length; i++) {
                expect(parameters[i].description).assertEqual('$string:mainability_description');
                expect(parameters[i].name).assertEqual("Data1V" + i);
                expect(parameters[i].type).assertEqual('float');
            }
            for (let i = 0; i < results.length; i++) {
                expect(results[i].description).assertEqual('$string:mainability_description');
                expect(results[i].name).assertEqual('Data1V');
                expect(results[i].type).assertEqual('float');
            }
            for (let i = 0; i < customizeDatas.length; i++) {
                expect(customizeDatas[i].name).assertEqual('');
                expect(customizeDatas[i].value).assertEqual('');
                expect(customizeDatas[i].extra).assertEqual('');
            }
        }
        done();
    })

    function checkMetaData(data, name) {
        let parameters = data.parameters;
        let results = data.results;
        let customizeDatas = data.customizeDatas;
        expect(typeof parameters).assertEqual('object');
        expect(typeof results).assertEqual('object');
        expect(typeof customizeDatas).assertEqual('object');
        expect(parameters.length).assertLarger(0);
        expect(results.length).assertLarger(0);
        expect(customizeDatas.length).assertLarger(0);
        for (let i = 0; i < parameters.length; i++) {
            expect(typeof parameters[i].description).assertEqual('string');
            expect(typeof parameters[i].name).assertEqual('string');
            expect(parameters[i].name).assertEqual(name);
            expect(typeof parameters[i].type).assertEqual('string');
        }
        for (let i = 0; i < results.length; i++) {
            expect(typeof results[i].description).assertEqual('string');
            expect(typeof results[i].name).assertEqual('string');
            expect(results[i].name).assertEqual(name);
            expect(typeof results[i].type).assertEqual('string');
        }
        for (let i = 0; i < customizeDatas.length; i++) {
            expect(typeof customizeDatas[i].name).assertEqual('string');
            expect(typeof customizeDatas[i].value).assertEqual('string');
            expect(customizeDatas[i].name).assertEqual(name);
            expect(typeof customizeDatas[i].extra).assertEqual('string');
        }
    }
})