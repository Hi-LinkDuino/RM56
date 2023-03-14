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
const BUNDLE_NAME1 = 'com.example.third1';
const BUNDLE_NAME2 = 'com.example.third2';
const BUNDLE_NAME4 = 'com.example.third4';
const BUNDLE_NAME5 = 'com.example.third5';
const BUNDLE_NAME6 = "com.ohos.callui"
const ABILITIY_NAME = "com.ohos.callui.MainAbility"
const SYSTEM_BUNDLE = 'com.example.system1';
const VENDOR_BUNDLE = 'com.example.vendor1';
const DESIGHN_WIDTH = 770;
const DEFAULT_DESIGHN_WIDTH = 720;
describe('ActsBmsFormsInfoTest', function () {

    function checkSystemHapForm(dataInfo) {
        console.debug('===system formInfo is ' + JSON.stringify(dataInfo))
        expect(dataInfo.name).assertEqual('Form_JS1S');
        expect(dataInfo.bundleName).assertEqual(SYSTEM_BUNDLE);
        expect(dataInfo.moduleName).assertEqual('entry');
        expect(dataInfo.abilityName).assertEqual(SYSTEM_BUNDLE + '.MainAbility');
        expect(dataInfo.description).assertEqual('JS1S Form');
        expect(dataInfo.descriptionId >= 0).assertTrue();
        expect(dataInfo.type).assertEqual(bundle.FormType.JS);
        expect(dataInfo.colorMode).assertEqual(bundle.ColorMode.AUTO_MODE);
        expect(dataInfo.defaultFlag).assertTrue();
        expect(dataInfo.jsComponentName).assertEqual('JS_name1S');
        expect(dataInfo.formVisibleNotify).assertFalse();
        expect(dataInfo.formConfigAbility).assertEqual('ability://com.example.system1.MainAbility');
        expect(dataInfo.updateDuration).assertEqual(1);
        expect(dataInfo.defaultDimension).assertEqual(4);
        expect(JSON.stringify(dataInfo.supportDimensions)).assertEqual('[1,2,3,4]');
        expect(dataInfo.relatedBundleName).assertEqual('');
        expect(JSON.stringify(dataInfo.customizeDatas))
            .assertEqual('[{"name":"originWidgetName1S","value":"com.openharmony.weather.testWidget1S"}]');
        expect(dataInfo.customizeDatas[0].name).assertEqual('originWidgetName1S');
        expect(dataInfo.customizeDatas[0].value).assertEqual('com.openharmony.weather.testWidget1S');
        expect(dataInfo.src).assertEqual('/pages/card/index');
        expect(dataInfo.window.designWidth).assertEqual(DESIGHN_WIDTH);
        expect(dataInfo.window.autoDesignWidth).assertEqual(true);
    }

    function checkVendorHapForm(dataInfo) {
        console.debug('===vendor formInfo is ' + JSON.stringify(dataInfo))
        expect(dataInfo.name).assertEqual('Form_JS1V');
        expect(dataInfo.bundleName).assertEqual(VENDOR_BUNDLE);
        expect(dataInfo.moduleName).assertEqual('entry');
        expect(dataInfo.abilityName).assertEqual(VENDOR_BUNDLE + '.MainAbility');
        expect(dataInfo.description).assertEqual('JS1V Form');
        expect(dataInfo.descriptionId >= 0).assertTrue();
        expect(dataInfo.type).assertEqual(bundle.FormType.JAVA);
        expect(dataInfo.colorMode).assertEqual(bundle.ColorMode.DARK_MODE);
        expect(dataInfo.defaultFlag).assertFalse();
        expect(dataInfo.jsComponentName).assertEqual('JS_name1V');
        expect(dataInfo.formVisibleNotify).assertFalse();
        expect(dataInfo.formConfigAbility).assertEqual('');
        expect(dataInfo.updateDuration).assertEqual(1);
        expect(dataInfo.defaultDimension).assertEqual(0);
        expect(JSON.stringify(dataInfo.supportDimensions)).assertEqual('[1,2,3]');
        expect(dataInfo.relatedBundleName).assertEqual('');
        expect(JSON.stringify(dataInfo.customizeDatas))
            .assertEqual('[{"name":"originWidgetName1V","value":"com.openharmony.weather.testWidget1V"}]');
        expect(dataInfo.src).assertEqual('');
        expect(dataInfo.window.designWidth).assertEqual(DEFAULT_DESIGHN_WIDTH);
        expect(dataInfo.window.autoDesignWidth).assertEqual(true);
    }

    /**
    * @tc.number: bms_getFormsInfo_0100
    * @tc.name: getFormsInfo : get forms information for one app
    * @tc.desc: check forms information for a third app which doesn't exist (by promise)
    */
    it('bms_getFormsInfo_0100', 0, async function (done) {
        console.info('==============bms_getFormsInfo_0100============');
        let data = await bundle.getFormsInfo('');
        expect(data.length).assertEqual(0);
        done();
    })

    /**
    * @tc.number: bms_getFormsInfo_0200
    * @tc.name: getFormsInfo : get forms information for one app
    * @tc.desc: check forms information for a third app which doesn't exist (by callback)
    */
    it('bms_getFormsInfo_0200', 0, async function (done) {
        console.info('==============bms_getFormsInfo_0200============');
        bundle.getFormsInfo('', (err, data) => {
            expect(err.code).assertEqual(-1);
            expect(data.length).assertEqual(0);
            done();
        });
    })

    /**
    * @tc.number: bms_getFormsInfo_0300
    * @tc.name: getFormsInfo : get forms information for one app
    * @tc.desc: check forms information for a third app which doesn't have forms (by promise)
    */
    it('bms_getFormsInfo_0300', 0, async function (done) {
        console.info('==============bms_getFormsInfo_0300============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest2.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            let formsInfo = await bundle.getFormsInfo(BUNDLE_NAME2);
            expect(formsInfo.length).assertEqual(0);
            installer.uninstall(BUNDLE_NAME2, {
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
    })

    /**
    * @tc.number: bms_getFormsInfo_0400
    * @tc.name: getFormsInfo : get forms information for one app
    * @tc.desc: check forms information for a third app which doesn't have forms (by callback)
    */
    it('bms_getFormsInfo_0400', 0, async function (done) {
        console.info('==============bms_getFormsInfo_0400============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest2.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            bundle.getFormsInfo(BUNDLE_NAME2, (err, formsInfo) => {
                expect(err.code).assertEqual(0);
                expect(formsInfo.length).assertEqual(0);
                installer.uninstall(BUNDLE_NAME2, {
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
        }
    })

    /**
    * @tc.number: bms_getFormsInfo_0500
    * @tc.name: getFormsInfo : get forms information for one app
    * @tc.desc: check forms information for a third app which has two haps (by promise)
    */
    it('bms_getFormsInfo_0500', 0, async function (done) {
        console.info('==============bms_getFormsInfo_0500============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest1.hap', '/data/test/bmsThirdBundleTest3.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            let formsInfo = await bundle.getFormsInfo(BUNDLE_NAME1);
            expect(formsInfo.length).assertEqual(2);
            checkFormIsExist('Form_JS1', formsInfo, '1');
            checkFormIsExist('Form_JS3', formsInfo, '3')
            installer.uninstall(BUNDLE_NAME1, {
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
    })

    /**
    * @tc.number: bms_getFormsInfo_0600
    * @tc.name: getFormsInfo : get forms information for one app
    * @tc.desc: check forms information for a third app which has two haps (by callback)
    */
    it('bms_getFormsInfo_0600', 0, async function (done) {
        console.info('==============bms_getFormsInfo_0600============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest1.hap', '/data/test/bmsThirdBundleTest3.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            bundle.getFormsInfo(BUNDLE_NAME1, (err, formsInfo) => {
                expect(err.code).assertEqual(0);
                expect(formsInfo.length).assertEqual(2);
                checkFormIsExist('Form_JS1', formsInfo, '1')
                checkFormIsExist('Form_JS3', formsInfo, '3')
                installer.uninstall(BUNDLE_NAME1, {
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
        }
    })

    /**
    * @tc.number: bms_getFormsInfo_0700
    * @tc.name: getFormsInfo : get forms information for one app
    * @tc.desc: check whether the form information of the update app is updated (by promise)
    */
    it('bms_getFormsInfo_0700', 0, async function (done) {
        console.info('==============bms_getFormsInfo_0700============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest1.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            installer.install(['/data/test/bmsThirdBundleTestA1.hap'], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                let formsInfo = await bundle.getFormsInfo(BUNDLE_NAME1)
                expect(formsInfo.length).assertEqual(1);
                checkFormIsExist('Form_JSA1', formsInfo, 'A1')
                installer.uninstall(BUNDLE_NAME1, {
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
        }
    })

    /**
    * @tc.number: bms_getFormsInfo_0800
    * @tc.name: getFormsInfo : get forms information for one app
    * @tc.desc: check whether the form information of the update app is updated (by callback)
    */
    it('bms_getFormsInfo_0800', 0, async function (done) {
        console.info('==============bms_getFormsInfo_0800============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest1.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            installer.install(['/data/test/bmsThirdBundleTestA1.hap'], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                bundle.getFormsInfo(BUNDLE_NAME1, (err, formsInfo) => {
                    expect(err.code).assertEqual(0);
                    expect(formsInfo.length).assertEqual(1);
                    checkFormIsExist('Form_JSA1', formsInfo, 'A1');
                    installer.uninstall(BUNDLE_NAME1, {
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
        }
    })

    /**
    * @tc.number: bms_getFormsInfo_0900
    * @tc.name: getFormsInfo : get forms information for one app
    * @tc.desc: check forms information for a third app which have two forms(by promise)
    */
    it('bms_getFormsInfo_0900', 0, async function (done) {
        console.info('==============bms_getFormsInfo_0900============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest4.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            let formsInfo = await bundle.getFormsInfo(BUNDLE_NAME4);
            expect(formsInfo.length).assertEqual(2);
            checkFormByName(formsInfo[0], '4A');
            checkFormByName(formsInfo[1], '4B');
            installer.uninstall(BUNDLE_NAME4, {
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
    })

    /**
    * @tc.number: bms_getFormsInfo_1000
    * @tc.name: getFormsInfo : get forms information for one app
    * @tc.desc: check forms information for a third app which have two forms(by callback)
    */
    it('bms_getFormsInfo_1000', 0, async function (done) {
        console.info('==============bms_getFormsInfo_1000============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest4.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            bundle.getFormsInfo(BUNDLE_NAME4, (err, formsInfo) => {
                expect(err.code).assertEqual(0);
                expect(formsInfo.length).assertEqual(2);
                checkFormByName(formsInfo[0], '4A');
                checkFormByName(formsInfo[1], '4B');
                installer.uninstall(BUNDLE_NAME4, {
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
        }
    })

    /**
    * @tc.number: bms_getFormsInfo_1100
    * @tc.name: getFormsInfo : get forms information for one app
    * @tc.desc: check forms information for a third app which have two abilities,
    *           and each ability has one forms (by promise)
    */
    it('bms_getFormsInfo_1100', 0, async function (done) {
        console.info('==============bms_getFormsInfo_1100============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest5.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            let formsInfo = await bundle.getFormsInfo(BUNDLE_NAME5);
            expect(formsInfo.length).assertEqual(2);
            checkFormIsExist('Form_JS5A', formsInfo, '5A');
            checkFormIsExist('Form_JS5B', formsInfo, '5B');
            installer.uninstall(BUNDLE_NAME5, {
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
    })

    /**
    * @tc.number: bms_getFormsInfo_1200
    * @tc.name: getFormsInfo : get forms information for one app
    * @tc.desc: check forms information for a third app which have two abilities,
    *           and each ability has one forms (by callback)
    */
    it('bms_getFormsInfo_1200', 0, async function (done) {
        console.info('==============bms_getFormsInfo_1200============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest5.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            bundle.getFormsInfo(BUNDLE_NAME5, (err, formsInfo) => {
                expect(err.code).assertEqual(0);
                expect(formsInfo.length).assertEqual(2);
                checkFormIsExist('Form_JS5A', formsInfo, '5A');
                checkFormIsExist('Form_JS5B', formsInfo, '5B');
                installer.uninstall(BUNDLE_NAME5, {
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
        }
    })

    /**
    * @tc.number: bms_getFormsInfo_1300
    * @tc.name: getFormsInfo : get forms information for one app
    * @tc.desc: check forms information for vendor and system app (by promise)
    */
    it('bms_getFormsInfo_1300', 0, async function (done) {
        console.info('==============bms_getFormsInfo_1300============');
        let data = await bundle.getFormsInfo('com.example.vendor1');
        expect(data.length).assertEqual(2);
        checkFormIsExist('Form_JS1V', data, '1V', false, true);
        checkFormIsExist('Form_JS1V2', data, '1V2');
        data = await bundle.getFormsInfo('com.example.system1');
        expect(data.length).assertEqual(1);
        checkFormIsExist('Form_JS1S', data, '1S', true);
        done();
    })

    /**
    * @tc.number: bms_getFormsInfo_1400
    * @tc.name: getFormsInfo : get forms information for one app
    * @tc.desc: check forms information for vendor and system app(by callback)
    */
    it('bms_getFormsInfo_1400', 0, async function (done) {
        console.info('==============bms_getFormsInfo_1400============');
        bundle.getFormsInfo('com.example.vendor1', (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.length).assertEqual(2);
            checkFormIsExist('Form_JS1V', data, '1V', false, true);
            checkFormIsExist('Form_JS1V2', data, '1V2');
        });
        bundle.getFormsInfo('com.example.system1', (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.length).assertEqual(1);
            checkFormIsExist('Form_JS1S', data, '1S', true);
            done();
        });
    })

    /**
    * @tc.number: bms_getFormsInfo_1500
    * @tc.name: getFormsInfo : get forms information for one app
    * @tc.desc: check whether the form information of the uninstall app is removed (by promise)
    */
    it('bms_getFormsInfo_1500', 0, async function (done) {
        console.info('==============bms_getFormsInfo_1500============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest4.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            installer.uninstall(BUNDLE_NAME4, {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                let formsInfo = await bundle.getFormsInfo(BUNDLE_NAME4);
                expect(formsInfo.length).assertEqual(0);
                done();
            });
        }
    })

    /**
    * @tc.number: bms_getFormsInfo_1600
    * @tc.name: getFormsInfo : get forms information for one app
    * @tc.desc: check whether the form information of the uninstall app is removed (by callback)
    */
    it('bms_getFormsInfo_1600', 0, async function (done) {
        console.info('==============bms_getFormsInfo_1600============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest4.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            installer.uninstall(BUNDLE_NAME4, {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                bundle.getFormsInfo(BUNDLE_NAME4, (err, formsInfo) => {
                    expect(err.code).assertEqual(-1);
                    expect(formsInfo.length).assertEqual(0);
                    done();
                });
            });
        }
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_0100
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check forms information for a third app by bundle name and module name (by promise)
    */
    it('bms_getFormsInfoByModule_0100', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_0100============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest1.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            let formsInfo = await bundle.getFormsInfoByModule(BUNDLE_NAME1, 'entry');
            expect(formsInfo.length).assertEqual(1);
            checkFormIsExist('Form_JS1', formsInfo, '1');
            installer.uninstall(BUNDLE_NAME1, {
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
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_0200
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check forms information for a third app by bundle name and module name (by callback)
    */
    it('bms_getFormsInfoByModule_0200', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_0200============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest1.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            bundle.getFormsInfoByModule(BUNDLE_NAME1, 'entry', (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.length).assertEqual(1);
                checkFormIsExist('Form_JS1', data, '1')
                installer.uninstall(BUNDLE_NAME1, {
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
        }
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_0300
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check forms information for a third app by wrong module name (by promise)
    */
    it('bms_getFormsInfoByModule_0300', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_0300============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest1.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            let formsInfo = await bundle.getFormsInfoByModule(BUNDLE_NAME1, '');
            expect(formsInfo.length).assertEqual(0);
            installer.uninstall(BUNDLE_NAME1, {
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
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_0400
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check forms information for a third app by wrong module name (by callback)
    */
    it('bms_getFormsInfoByModule_0400', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_0400============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest1.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            bundle.getFormsInfoByModule(BUNDLE_NAME1, '', (err, data) => {
                expect(err.code).assertEqual(-1);
                expect(data.length).assertEqual(0);
                installer.uninstall(BUNDLE_NAME1, {
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
        }
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_0500
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check forms information for a third app by wrong bundle name (by promise)
    */
    it('bms_getFormsInfoByModule_0500', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_0500============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest1.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            let formsInfo = await bundle.getFormsInfoByModule('', 'bmsThirdBundle1');
            expect(formsInfo.length).assertEqual(0);
            installer.uninstall(BUNDLE_NAME1, {
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
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_0600
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check forms information for a third app by wrong bundle name (by callback)
    */
    it('bms_getFormsInfoByModule_0600', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_0600============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest1.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            bundle.getFormsInfoByModule('', 'bmsThirdBundle1', (err, data) => {
                expect(err.code).assertEqual(-1);
                expect(data.length).assertEqual(0);
                installer.uninstall(BUNDLE_NAME1, {
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
        }
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_0700
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check forms information for a third app which has two haps (by promise)
    */
    it('bms_getFormsInfoByModule_0700', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_0700============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest1.hap', '/data/test/bmsThirdBundleTest3.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            let formsInfo = await bundle.getFormsInfoByModule(BUNDLE_NAME1, 'bmsThirdBundle3');
            expect(formsInfo.length).assertEqual(1);
            checkFormIsExist('Form_JS3', formsInfo, '3')
            installer.uninstall(BUNDLE_NAME1, {
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
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_0800
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check forms information for a third app which has two haps (by callback)
    */
    it('bms_getFormsInfoByModule_0800', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_0800============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest1.hap', '/data/test/bmsThirdBundleTest3.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            bundle.getFormsInfoByModule(BUNDLE_NAME1, 'bmsThirdBundle3', (err, formsInfo) => {
                expect(err.code).assertEqual(0);
                expect(formsInfo.length).assertEqual(1);
                checkFormIsExist('Form_JS3', formsInfo, '3')
                installer.uninstall(BUNDLE_NAME1, {
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
        }
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_0900
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check forms information for a third app which have two forms (by promise)
    */
    it('bms_getFormsInfoByModule_0900', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_0900============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest4.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            let formsInfo = await bundle.getFormsInfoByModule(BUNDLE_NAME4, 'entry');
            expect(formsInfo.length).assertEqual(2);
            checkFormIsExist('Form_JS4A', formsInfo, '4A');
            checkFormIsExist('Form_JS4B', formsInfo, '4B');
            installer.uninstall(BUNDLE_NAME4, {
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
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_1000
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check forms information for a third app which have two forms (by callback)
    */
    it('bms_getFormsInfoByModule_1000', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_1000============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest4.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            bundle.getFormsInfoByModule(BUNDLE_NAME4, 'entry', (err, formsInfo) => {
                expect(err.code).assertEqual(0);
                expect(formsInfo.length).assertEqual(2);
                checkFormIsExist('Form_JS4A', formsInfo, '4A');
                checkFormIsExist('Form_JS4B', formsInfo, '4B');
                installer.uninstall(BUNDLE_NAME4, {
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
        }
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_1100
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check forms information for a third app which have two abilities,
    *          and each ability has one forms (by promise)
    */
    it('bms_getFormsInfoByModule_1100', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_1100============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest5.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            let formsInfo = await bundle.getFormsInfoByModule(BUNDLE_NAME5, 'entry');
            expect(formsInfo.length).assertEqual(2);
            checkFormIsExist('Form_JS5A', formsInfo, '5A');
            checkFormIsExist('Form_JS5B', formsInfo, '5B');
            installer.uninstall(BUNDLE_NAME5, {
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
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_1200
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check forms information for a third app which have two abilities,
    *          and each ability has one forms (by callback)
    */
    it('bms_getFormsInfoByModule_1200', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_1200============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest5.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            bundle.getFormsInfoByModule(BUNDLE_NAME5, 'entry', (err, formsInfo) => {
                expect(err.code).assertEqual(0);
                expect(formsInfo.length).assertEqual(2);
                checkFormIsExist('Form_JS5A', formsInfo, '5A');
                checkFormIsExist('Form_JS5B', formsInfo, '5B');
                installer.uninstall(BUNDLE_NAME5, {
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
        }
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_1300
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check forms information for system and vendor app (by promise)
    */
    it('bms_getFormsInfoByModule_1300', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_1300============');
        let data = await bundle.getFormsInfoByModule('com.example.vendor1', 'entry');
        expect(data.length).assertEqual(2);
        checkFormIsExist('Form_JS1V', data, '1V', false, true);
        checkFormIsExist('Form_JS1V2', data, '1V2');
        data = await bundle.getFormsInfoByModule('com.example.system1', 'entry');
        expect(data.length).assertEqual(1);
        checkFormIsExist('Form_JS1S', data, '1S', true);
        done();
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_1400
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check forms information for system and vendor app (by callback)
    */
    it('bms_getFormsInfoByModule_1400', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_1400============');
        bundle.getFormsInfoByModule('com.example.vendor1', 'entry', (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.length).assertEqual(2);
            checkFormIsExist('Form_JS1V', data, '1V', false, true);
            checkFormIsExist('Form_JS1V2', data, '1V2');
        });
        bundle.getFormsInfoByModule('com.example.system1', 'entry', (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.length).assertEqual(1);
            checkFormIsExist('Form_JS1S', data, '1S', true);
            done();
        });
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_1500
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check whether the form information of the uninstall app is removed (by promise)
    */
    it('bms_getFormsInfoByModule_1500', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_1500============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest5.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            installer.uninstall(BUNDLE_NAME5, {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                let formsInfo = await bundle.getFormsInfoByModule(BUNDLE_NAME5, 'entry');
                expect(formsInfo.length).assertEqual(0);
                done();
            });
        }
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_1600
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check whether the form information of the uninstall app is removed (by callback)
    */
    it('bms_getFormsInfoByModule_1600', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_1600============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest5.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            installer.uninstall(BUNDLE_NAME5, {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                bundle.getFormsInfoByModule(BUNDLE_NAME5, 'entry', (err, formsInfo) => {
                    expect(err.code).assertEqual(-1);
                    expect(formsInfo.length).assertEqual(0);
                    done();
                });
            });
        }
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_1700
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check whether the form information of the update app is updated (by promise)
    */
    it('bms_getFormsInfoByModule_1700', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_1700============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest1.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            installer.install(['/data/test/bmsThirdBundleTestA1.hap'], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                let formsInfo = await bundle.getFormsInfoByModule(BUNDLE_NAME1, 'entry')
                expect(formsInfo.length).assertEqual(1);
                checkFormIsExist('Form_JSA1', formsInfo, 'A1')
                installer.uninstall(BUNDLE_NAME1, {
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
        }
    })

    /**
    * @tc.number: bms_getFormsInfoByModule_1800
    * @tc.name: getFormsInfoByModule : get forms information for one app
    * @tc.desc: check whether the form information of the update app is updated (by callback)
    */
    it('bms_getFormsInfoByModule_1800', 0, async function (done) {
        console.info('==============bms_getFormsInfoByModule_1800============');
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest1.hap'], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, onReceiveinstallEvent);

        function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            installer.install(['/data/test/bmsThirdBundleTestA1.hap'], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                bundle.getFormsInfoByModule(BUNDLE_NAME1, 'entry', (err, formsInfo) => {
                    expect(err.code).assertEqual(0);
                    expect(formsInfo.length).assertEqual(1);
                    checkFormIsExist('Form_JSA1', formsInfo, 'A1');
                    installer.uninstall(BUNDLE_NAME1, {
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
        }
    })

    function checkFormIsExist(formName, dataInfo, name, system = false, vendor = false) {
        let info = new Map();
        for (let i = 0, len = dataInfo.length; i < len; i++) {
            info.set(dataInfo[i].name, dataInfo[i]);
        }
        expect(info.has(formName)).assertTrue();
        if (info.has(formName)) {
            if (system) {
                checkSystemHapForm(info.get(formName));
                return;
            }
            if (vendor) {
                checkVendorHapForm(info.get(formName));
                return;
            }
            checkFormByName(info.get(formName), name);
        }
    }

    function checkFormNoExist(dataInfo, formName) {
        let info = new Map();
        for (let i = 0, len = dataInfo.length; i < len; i++) {
            info.set(dataInfo[i].name, 0)
        }
        expect(info.has(formName)).assertFalse();
    }

    function checkFormByName(dataInfo, name) {
        console.info('=======Form Info========' + JSON.stringify(dataInfo))
        expect(dataInfo.name).assertEqual('Form_JS' + name);
        console.info('=============ModuleName is=========' + dataInfo.moduleName);
        console.info('=============supportDimensions==============' + JSON.stringify(dataInfo.supportDimensions));
        console.info('=============abilityName==============' + JSON.stringify(dataInfo.abilityName));
        expect(dataInfo.jsComponentName).assertEqual('JS_name' + name);
        expect(dataInfo.formVisibleNotify).assertFalse();
        expect(typeof dataInfo.name).assertEqual('string');
        expect(typeof dataInfo.bundleName).assertEqual('string');
        expect(typeof dataInfo.moduleName).assertEqual('string');
        expect(typeof dataInfo.abilityName).assertEqual('string');
        expect(typeof dataInfo.description).assertEqual('string');
        expect(typeof dataInfo.descriptionId).assertEqual('number');
        expect(typeof dataInfo.type).assertEqual('number');
        expect(dataInfo.colorMode).assertEqual(bundle.ColorMode.LIGHT_MODE);
        expect(typeof dataInfo.defaultFlag).assertEqual('boolean');
        expect(typeof dataInfo.jsComponentName).assertEqual('string');
        expect(typeof dataInfo.formVisibleNotify).assertEqual('boolean');
        expect(typeof dataInfo.formConfigAbility).assertEqual('string');
        expect(typeof dataInfo.updateDuration).assertEqual('number');
        expect(typeof dataInfo.defaultDimension).assertEqual('number');
        expect(typeof dataInfo.supportDimensions).assertEqual('object');
        expect(typeof dataInfo.relatedBundleName).assertEqual('string');
        console.debug('====dataInfo.scheduledUpdateTime====' + dataInfo.scheduledUpdateTime)
        expect(typeof dataInfo.customizeDatas).assertEqual('object');
        for (let j = 0, len = dataInfo.customizeDatas.length; j < len; j++) {
            console.info('======customizeDatasName======' + JSON.stringify(dataInfo.customizeDatas[j].name));
            expect(dataInfo.customizeDatas[j].name).assertEqual('originWidgetName' + name);
            expect(typeof dataInfo.customizeDatas[j].name).assertEqual('string');
            console.info('======customizeDatasValue=====' + JSON.stringify(dataInfo.customizeDatas[j].value));
            expect(dataInfo.customizeDatas[j].value).assertEqual('com.openharmony.weather.testWidget' + name);
            expect(typeof dataInfo.customizeDatas[j].value).assertEqual('string');
        }
        expect(typeof dataInfo.src).assertEqual('string');
        expect(typeof dataInfo.window).assertEqual('object');
        expect(typeof dataInfo.window.designWidth).assertEqual('number');
        expect(typeof dataInfo.window.autoDesignWidth).assertEqual('boolean');
        expect(dataInfo.window.designWidth).assertEqual(DEFAULT_DESIGHN_WIDTH);
        expect(dataInfo.window.autoDesignWidth).assertEqual(false);
    }

    /*
     * @tc.number: bms_getAbilityIcon_0100
     * @tc.name: test getAbilityIcon`
     * @tc.desc: get the abilityIcon
     */
    it('bms_getAbilityIcon_0100', 0, async function (done) {
        bundle.getAbilityIcon(BUNDLE_NAME6, ABILITIY_NAME).then(pixelmap => {
            console.log('bms_getAbilityIcon_0100 success: ' + pixelmap);
            expect(pixelmap !== null).assertTrue()
            done()
        })
        .catch(err => {
            console.info("getAbilityIcon fail:" + JSON.stringify(err))
            expect(err).assertFail()
            done()
        })
    })

    /*
     * @tc.number: bms_getAbilityIcon_0200
     * @tc.name: test getAbilityIcon
     * @tc.desc: get the abilityIcon
     */
    it('bms_getAbilityIcon_0200', 0, async function (done) {
        bundle.getAbilityIcon(BUNDLE_NAME6, ABILITIY_NAME, (err, pixelmap) => {
            if (err) {
                console.info("getAbilityIcon fail:" + JSON.stringify(err))
                expect(err).assertFail()
            }
            console.info('bms_getAbilityIcon_0200 success: ' + JSON.stringify(pixelmap))
            expect(pixelmap !== null).assertTrue()
            done()
        })
    })
})