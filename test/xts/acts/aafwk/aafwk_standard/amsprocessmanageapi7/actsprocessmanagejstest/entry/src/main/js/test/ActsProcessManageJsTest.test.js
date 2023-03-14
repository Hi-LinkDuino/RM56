/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
import fileio from '@ohos.fileio';
import commonEvent from "@ohos.commonevent"
import abilitymanager from '@ohos.app.abilitymanager'
import abilityManager from '@ohos.application.appManager'
import missionmanager from '@ohos.application.missionManager'
import featureAbility from '@ohos.ability.featureAbility'
import file from '@system.file'
import bundle from '@ohos.bundle'
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

const BUNDLE_PATHS = [['/data/ActsAmsProcessManageSystemAppAApi7.hap',
    '/data/ActsAmsProcessManageSystemAppAMultiApi7.hap'],
['/data/ActsAmsProcessManageSystemAppBApi7.hap'],
['/data/ActsAmsProcessManageVendorAppAApi7.hap']];
const BUNDLE_NAMES = ['com.acts.pmsystemappaapi7',
    'com.acts.pmsystemappbapi7',
    'com.acts.pmvendorappaapi7'];
const PATHS_COUNT = 3;
const BUNDLE_COUNT = 3;

var subscribeInfo = {
    events: [
        "event_from_ProcessManage_SystemAppA",
        "event_from_ProcessManage_VendorAppA"
    ],
};
var eventFromSystemA = "event_from_ProcessManage_SystemAppA";
var eventFromVendorA = "event_from_ProcessManage_VendorAppA";

var systemABundleName = "com.acts.pmsystemappaapi7";
var systemAMultiBundleName = "com.acts.pmsystemappamultiapi7";
var systemBBundleName = "com.acts.pmsystemappbapi7";
var vendorABundleName = "com.acts.pmvendorappaapi7";
var vendorServiceBundleName = "com.acts.pmvendorappserviceapi7";
var systemAAbilityName = "com.acts.pmsystemappaapi7.MainAbility";
var systemBAbilityName = "com.acts.pmsystemappbapi7.MainAbility";
var vendorAAbilityName = "com.acts.pmvendorappaapi7.MainAbility";
var systemAMultiAbilityName = "com.acts.pmsystemappamultiapi7.MainAbility";

var systemAProcessName = systemABundleName;
var systemAMultiProcessName = systemAMultiBundleName;
var systemBProcessName = systemBBundleName;
var vendorAProcessName = vendorABundleName;
var uriSystemA = '/data/accounts/account_0/appdata/com.acts.pmsystemappa/com.acts.pmsystemappa';
var uriSystemAMulti = '/data/accounts/account_0/appdata/com.acts.pmsystemappa/com.acts.pmsystemappamulti';
var uriSystemB = '/data/accounts/account_0/appdata/com.acts.pmsystemappb/com.acts.pmsystemappb';
var uriVendorA = '/data/accounts/account_0/appdata/com.acts.pmvendorappa/com.acts.pmvendorappa';

describe('ActsAmsProcessManageJsTestApi7', function () {

    beforeEach(async (done) => {
        await clearMissionId();
        setTimeout(async function () {
            try {
                let installer = await bundle.getBundleInstaller();
                console.log('======>ACTS_ProcessManage beforeEach installer<======='
                    + JSON.stringify(installer));
                var count = 0;
                for (let i = 0; i < PATHS_COUNT; i++) {
                    installer.install(BUNDLE_PATHS[i], {
                        userId: 100,
                        installFlag: 1,
                        isKeepData: false
                    }, (err, data) => {
                        count++;
                        console.log('======>ACTS_ProcessManage beforeEach install finish<======='
                            + err.code);
                        console.log('======>ACTS_ProcessManage beforeEach install finish<======='
                            + data.status);
                        console.log('======>ACTS_ProcessManage beforeEach install finish<======='
                            + data.statusMessage);
                        if (count == PATHS_COUNT) {
                            setTimeout(function () {
                                done();
                            }, 2000);
                        }
                    })
                }
            } catch (err) {
                console.log('======>ACTS_ProcessManage beforeEach err<=======' + err);
            }
        }, 500);
    })

    afterEach(async (done) => {
        setTimeout(async function () {
            let installer = await bundle.getBundleInstaller();
            var count = 0;
            var testMissionId;

            testMissionId = await getMissionId();
            console.log('======>ACTS_ProcessManage afterEach testMissionId<=======' + testMissionId);
            await missionmanager.moveMissionToFront(testMissionId);

            for (let i = 0; i < BUNDLE_COUNT; i++) {
                installer.uninstall(BUNDLE_NAMES[i], {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, (err, data) => {
                    count++;
                    console.log('======>ACTS_ProcessManage afterEach uninstall finish<======='
                        + err.code);
                    console.log('======>ACTS_ProcessManage afterEach uninstall finish<======='
                        + data.status);
                    console.log('======>ACTS_ProcessManage afterEach uninstall finish<======='
                        + data.statusMessage);
                    if (count == BUNDLE_COUNT) {
                        setTimeout(function () {
                            done();
                        }, 2000);
                    }
                })
            }
        }, 1000);
    })

    function getMissionId() {
        return new Promise(async (resolve, reject) => {
            var numMax = 1024;
            var missionId = -1;
            var missionInfos = await missionmanager.getMissionInfos('', numMax);
            for (let i = 0; i < missionInfos.length; i++) {
                console.log('getMissionId result: ' + i + '= ' + JSON.stringify(missionInfos[i]))
                if ((missionInfos[i].want.abilityName == "com.acts.actsprocessmanagejstestapi7.MainAbility") &&
                    (missionInfos[i].runningState == 0)) {
                    missionId = missionInfos[i].missionId;
                    break;
                }
            }
            console.log('======>getMissionId resolve missionId<=======' + missionId);
            resolve(missionId);
        })
    }

    function clearMissionId() {
        return new Promise(async (resolve, reject) => {
            var numMax = 1024;
            var missionInfos = await missionmanager.getMissionInfos('', numMax);
            for (let i = 0; i < missionInfos.length; i++) {
                if ((missionInfos[i].want.bundleName == systemABundleName) ||
                    (missionInfos[i].want.bundleName == systemBBundleName) ||
                    (missionInfos[i].want.bundleName == vendorABundleName) ||
                    (missionInfos[i].want.bundleName == vendorServiceBundleName)) {
                    missionmanager.clearMission(missionInfos[i].missionId, (err, data) => {
                        console.debug("====>ACTS_ProcessManage clearMission id:" + missionInfos[i].missionId);
                    });
                }
            }
            console.log('======>ACTS_ProcessManage clearMission finish<=======');
            resolve();
        })
    }


    function checkPromiseExistDelay(processName) {
        return new Promise(async (resolve, reject) => {
            setTimeout(async function () {
                try {
                    var processInfos = await abilitymanager.getActiveProcessInfos();
                    console.debug("====>ACTS_ProcessManage checkPromiseExistDelay processInfos====>"
                        + JSON.stringify(processInfos));
                    resolve(checkProcessExistInInfos(processInfos, processName));
                } catch (err) {
                    onsole.debug("====>ACTS_ProcessManage checkPromiseExistDelay err====>"
                        + JSON.stringify(err));
                }
            }, 2000);
        });
    }

    function checkProcessExistInInfos(processInfos, processName) {
        for (var i = 0; i < processInfos.length; i++) {
            if (processInfos[i].processName == processName) {
                return true;
            }
        }
        return false;
    }

    function checkPromiseMissionExistDelay(abilityName) {
        var numMax = 1024;

        return new Promise(async (resolve, reject) => {
            try {
                var missionInfos = await missionmanager.getMissionInfos('', numMax);
                console.debug("====>ACTS_ProcessManage checkPromiseMissionExistDelay missionInfos====>"
                    + JSON.stringify(missionInfos));
                resolve(checkMissionExistInInfos(missionInfos, abilityName));
            } catch (err) {
                onsole.debug("====>ACTS_ProcessManage checkPromiseMissionExistDelay err====>"
                    + JSON.stringify(err));
            }
        });
    }

    function checkMissionExistInInfos(missionInfos, abilityName) {
        for (var i = 0; i < missionInfos.length; i++) {
            if ((missionInfos[i].want.abilityName == abilityName) && (missionInfos[i].runningState == 0)) {
                return true;
            }
        }
        return false;
    }

    function getAppDataPathFileNums(uri, callBack) {
        setTimeout(async function () {

            file.list({
                uri: uri,
                success: function (data) {
                    console.log('====>ActsProcessManageJsTest call list success data ====>'
                        + JSON.stringify(data));
                    callBack(null, data.fileList.length);
                },
                fail: function (data, code) {
                    callBack(data, null);
                    console.log('call list failed, code: ' + code + ', data: ' + data);
                },
            });
        }, 2000);
    }

    /*
    * @tc.number: ACTS_ProcessManage_killProcessesByBundleNameFA_0100
    * @tc.name: The system application stops itself
    * @tc.desc: Check the process was killed (by promise)
    */
    it('ACTS_ProcessManage_killProcessesByBundleNameFA_0100', 0, async function (done) {
        console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0100 begin');

        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "promiseKill",
                    parameters: {
                        testTarget: systemABundleName
                    }
                },
            }
        );

        setTimeout(async function () {
            var processexist = await checkPromiseExistDelay(systemAProcessName);
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0100 processexist:====>"
                + JSON.stringify(processexist));
            expect(processexist).assertEqual(false);

            var missionexist = await checkPromiseMissionExistDelay(systemAAbilityName)
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0100 missionexist:====>"
                + JSON.stringify(missionexist));
            expect(missionexist).assertEqual(false);

            console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0100 end');
            done();
        }, 4000);
    })

    /*
    * @tc.number: ACTS_ProcessManage_killProcessesByBundleNameFA_0200
    * @tc.name: The system application stops other system application
    * @tc.desc: Check the process was killed (by promise)
    */
    it('ACTS_ProcessManage_killProcessesByBundleNameFA_0200', 0, async function (done) {
        console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0200 begin');
        var subscriber;

        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "promiseKill",
                    parameters: {
                        testTarget: systemBBundleName
                    }
                },
            }, (error, data) => {
                console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0200 - startAbility: '
                    + JSON.stringify(error) + ", " + JSON.stringify(data))
            }
        );
        await commonEvent.createSubscriber(subscribeInfo).then(async (data) => {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0200 Create Subscriber data====>"
                + data);
            subscriber = data;
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0200 subscriber====>"
                + JSON.stringify(subscriber));
            await commonEvent.subscribe(subscriber, subscribeCallBack);
        });
        async function subscribeCallBack(err, data) {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0200 Subscribe CallBack data:====>"
                + JSON.stringify(data));
            expect(data.event).assertEqual(eventFromSystemA);
            expect(data.data).assertEqual('0');
            commonEvent.unsubscribe(subscriber, unSubscribeCallback);

        }
        async function unSubscribeCallback() {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0200 UnSubscribe CallBack====>");
            var processexist = await checkPromiseExistDelay(systemBProcessName);
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0200 processexist:====>"
                + JSON.stringify(processexist));
            expect(processexist).assertEqual(false);

            var missionexist = await checkPromiseMissionExistDelay(systemBAbilityName)
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0200 missionexist:====>"
                + JSON.stringify(missionexist));
            expect(missionexist).assertEqual(false);

            console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0200 end');
            done();
        }
    })

    /*
    * @tc.number: ACTS_ProcessManage_killProcessesByBundleNameFA_0300
    * @tc.name: The system application stops third-party application
    * @tc.desc: Check the process was killed (by promise)
    */
    it('ACTS_ProcessManage_killProcessesByBundleNameFA_0300', 0, async function (done) {
        console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0300 begin');
        var subscriber;

        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "promiseKill",
                    parameters: {
                        testTarget: vendorABundleName
                    }
                },
            }
        );
        await commonEvent.createSubscriber(subscribeInfo).then(async (data) => {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0300 Create Subscriber data====>"
                + data);
            subscriber = data;
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0300 subscriber====>"
                + JSON.stringify(subscriber));
            await commonEvent.subscribe(subscriber, subscribeCallBack);
        });
        async function subscribeCallBack(err, data) {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0300 Subscribe CallBack data:====>"
                + JSON.stringify(data));
            expect(data.event).assertEqual(eventFromSystemA);
            expect(data.data).assertEqual('0');
            commonEvent.unsubscribe(subscriber, unSubscribeCallback);
        }

        async function unSubscribeCallback() {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0300 UnSubscribe CallBack====>");
            var processexist = await checkPromiseExistDelay(vendorAProcessName);
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0300 processexist:====>"
                + JSON.stringify(processexist));
            expect(processexist).assertEqual(false);

            var missionexist = await checkPromiseMissionExistDelay(vendorAAbilityName)
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0300 missionexist:====>"
                + JSON.stringify(missionexist));
            expect(missionexist).assertEqual(false);

            console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0300 end');
            done();
        }
    })

    /*
    * @tc.number: ACTS_ProcessManage_killProcessesByBundleNameFA_0400
    * @tc.name: The system application stops multiprocess application
    * @tc.desc: Check the process was killed (by promise)
    */
    it('ACTS_ProcessManage_killProcessesByBundleNameFA_0400', 0, async function (done) {
        console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0400 begin');
        var flagProcessA = 0;
        var flagProcessAMulti = 0;
        var flagMissionA = 0;
        var flagMissionAMulti = 0;
        var numMax = 1024;

        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "promiseKill",
                    parameters: {
                        testTarget: systemAMultiAbilityName
                    }
                },
            }
        );
        setTimeout(async function () {

            var processInfos = await abilitymanager.getActiveProcessInfos();
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0400 processInfos====>"
                + JSON.stringify(processInfos));
            for (var i = 0; i < processInfos.length; i++) {
                if (processInfos[i].processName == systemAProcessName) {
                    flagProcessA++;
                }
                if (processInfos[i].processName == systemAMultiProcessName) {
                    flagProcessAMulti++;
                }
            }
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0400 flagProcessA====>"
                + JSON.stringify(flagProcessA));
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0400 flagProcessAMulti====>"
                + JSON.stringify(flagProcessAMulti));
            expect(flagProcessA).assertEqual(0);
            expect(flagProcessAMulti).assertEqual(0);

            var missionInfos = await missionmanager.getMissionInfos('', numMax);
            console.debug("====>ACTS_ProcessManage checkPromiseMissionExistDelay missionInfos====>"
                + JSON.stringify(missionInfos));
            for (var i = 0; i < missionInfos.length; i++) {

                if ((missionInfos[i].want.abilityName == systemAAbilityName)
                    && (missionInfos[i].runningState == 0)) {
                    flagMissionA++;
                }
                if ((missionInfos[i].want.abilityName == systemAMultiAbilityName)
                    && (missionInfos[i].runningState == 0)) {
                    flagMissionAMulti++;
                }
            }
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0400 flagMissionA====>"
                + JSON.stringify(flagMissionA));
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0400 flagMissionAMulti====>"
                + JSON.stringify(flagMissionAMulti));
            expect(flagMissionA).assertEqual(0);
            expect(flagMissionAMulti).assertEqual(0);

            console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0400 end');
            done();
        }, 6000);
    })

    /*
    * @tc.number: ACTS_ProcessManage_killProcessesByBundleNameFA_0500
    * @tc.name: The system application stops itself
    * @tc.desc: Check the process was killed (by AsyncCallback)
    */
    it('ACTS_ProcessManage_killProcessesByBundleNameFA_0500', 0, async function (done) {
        console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0500 begin');
        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "asyncKill",
                    parameters: {
                        testTarget: systemABundleName
                    }
                },
            }
        );
        setTimeout(async function () {
            var processexist = await checkPromiseExistDelay(systemAProcessName);
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0500 processexist:====>"
                + JSON.stringify(processexist));
            expect(processexist).assertEqual(false);

            var missionexist = await checkPromiseMissionExistDelay(systemAAbilityName)
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0500 missionexist:====>"
                + JSON.stringify(missionexist));
            expect(missionexist).assertEqual(false);

            console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0500 end');
            done();
        }, 4000);
    })

    /*
    * @tc.number: ACTS_ProcessManage_killProcessesByBundleNameFA_0600
    * @tc.name: The system application stops other system application
    * @tc.desc: Check the process was killed (by AsyncCallback)
    */
    it('ACTS_ProcessManage_killProcessesByBundleNameFA_0600', 0, async function (done) {
        console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0600 begin');
        var subscriber;

        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "asyncKill",
                    parameters: {
                        testTarget: systemBBundleName
                    }
                },
            }
        );
        await commonEvent.createSubscriber(subscribeInfo).then(async (data) => {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0600 Create Subscriber data====>"
                + data);
            subscriber = data;
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0600 subscriber====>"
                + JSON.stringify(subscriber));
            await commonEvent.subscribe(subscriber, subscribeCallBack);
        });
        async function subscribeCallBack(err, data) {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0600 Subscribe CallBack data:====>"
                + JSON.stringify(data));
            expect(data.event).assertEqual(eventFromSystemA);
            expect(data.data).assertEqual('0');
            commonEvent.unsubscribe(subscriber, unSubscribeCallback);

        }

        async function unSubscribeCallback() {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0600 UnSubscribe CallBack====>");
            var processexist = await checkPromiseExistDelay(systemBProcessName);
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0600 processexist:====>"
                + JSON.stringify(processexist));
            expect(processexist).assertEqual(false);

            var missionexist = await checkPromiseMissionExistDelay(systemBAbilityName)
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0600 missionexist:====>"
                + JSON.stringify(missionexist));
            expect(missionexist).assertEqual(false);

            console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0600 end');
            done();
        }
    })

    /*
    * @tc.number: ACTS_ProcessManage_killProcessesByBundleNameFA_0700
    * @tc.name: The system application stops third-party application
    * @tc.desc: Check the process was killed (by AsyncCallback)
    */
    it('ACTS_ProcessManage_killProcessesByBundleNameFA_0700', 0, async function (done) {
        console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0700 begin');
        var subscriber;

        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "asyncKill",
                    parameters: {
                        testTarget: vendorABundleName
                    }
                },
            }
        );
        await commonEvent.createSubscriber(subscribeInfo).then(async (data) => {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0700 Create Subscriber data====>"
                + data);
            subscriber = data;
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0700 subscriber====>"
                + JSON.stringify(subscriber));
            await commonEvent.subscribe(subscriber, subscribeCallBack);
        });
        async function subscribeCallBack(err, data) {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0700 Subscribe CallBack data:====>"
                + JSON.stringify(data));
            expect(data.event).assertEqual(eventFromSystemA);
            expect(data.data).assertEqual('0');
            commonEvent.unsubscribe(subscriber, unSubscribeCallback);

        }

        async function unSubscribeCallback() {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0700 UnSubscribe CallBack====>");
            var processexist = await checkPromiseExistDelay(vendorAProcessName);
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0700 processexist:====>"
                + JSON.stringify(processexist));
            expect(processexist).assertEqual(false);

            var missionexist = await checkPromiseMissionExistDelay(vendorAAbilityName)
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0700 missionexist:====>"
                + JSON.stringify(missionexist));
            expect(missionexist).assertEqual(false);

            console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0700 end');
            done();

        }
    })

    /*
        * @tc.number: ACTS_ProcessManage_killProcessesByBundleNameFA_0800
        * @tc.name: The system application stops multiprocess application
        * @tc.desc: Check the process was killed (by AsyncCallback)
        */
    it('ACTS_ProcessManage_killProcessesByBundleNameFA_0800', 0, async function (done) {
        console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0800 begin');
        var flagProcessA = 0;
        var flagProcessAMulti = 0;
        var flagMissionA = 0;
        var flagMissionAMulti = 0;
        var numMax = 1024;

        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "asyncKill",
                    parameters: {
                        testTarget: systemAMultiAbilityName
                    }
                },
            }
        );
        setTimeout(async function () {

            var processInfos = await abilitymanager.getActiveProcessInfos();
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0800 processInfos====>"
                + JSON.stringify(processInfos));
            for (var i = 0; i < processInfos.length; i++) {
                if (processInfos[i].processName == systemAProcessName) {
                    flagProcessA++;
                }
                if (processInfos[i].processName == systemAMultiProcessName) {
                    flagProcessAMulti++;
                }
            }
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0800 flagProcessA====>"
                + JSON.stringify(flagProcessA));
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0800 flagProcessAMulti====>"
                + JSON.stringify(flagProcessAMulti));
            expect(flagProcessA).assertEqual(0);
            expect(flagProcessAMulti).assertEqual(0);

            var missionInfos = await missionmanager.getMissionInfos('', numMax);
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0800 missionInfos====>"
                + JSON.stringify(missionInfos));
            for (var i = 0; i < missionInfos.length; i++) {

                if ((missionInfos[i].want.abilityName == systemAAbilityName)
                    && (missionInfos[i].runningState == 0)) {
                    flagMissionA++;
                }
                if ((missionInfos[i].want.abilityName == systemAMultiAbilityName)
                    && (missionInfos[i].runningState == 0)) {
                    flagMissionAMulti++;
                }
            }
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0800 flagMissionA====>"
                + JSON.stringify(flagMissionA));
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0800 flagMissionAMulti====>"
                + JSON.stringify(flagMissionAMulti));
            expect(flagMissionA).assertEqual(0);
            expect(flagMissionAMulti).assertEqual(0);

            console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0800 end');
            done();
        }, 6000);

    })


    /*
    * @tc.number: ACTS_ProcessManage_killProcessesByBundleNameFA_0900
    * @tc.name: Parameter is null
    * @tc.desc: Check the result is OK (by promise)
    */
    it('ACTS_ProcessManage_killProcessesByBundleNameFA_0900', 0, async function (done) {

        console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0900 begin');
        try {
            var result = await abilityManager.killProcessesByBundleName(null);
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0900 null result:====>"
                + JSON.stringify(result));
            expect(result == 0).assertEqual(false);
            console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_0900 end');
            done();
        }
        catch (err) {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_0900 err:====>"
                + JSON.stringify(err));
            done();
        }
    })


    /*
    * @tc.number: ACTS_ProcessManage_killProcessesByBundleNameFA_1000
    * @tc.name: Parameter is ''
    * @tc.desc: Check the result is OK (by promise)
    */
    it('ACTS_ProcessManage_killProcessesByBundleNameFA_1000', 0, async function (done) {

        console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_1000 begin');
        try {
            var result = await abilityManager.killProcessesByBundleName('');
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_1000 '' result:====>"
                + JSON.stringify(result));
            expect(result).assertEqual(2097215);
            console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_1000 end');
            done();
        }
        catch (err) {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_1000 err:====>"
                + JSON.stringify(err));
            done();
        }
    })

    /*
    * @tc.number: ACTS_ProcessManage_killProcessesByBundleNameFA_1100
    * @tc.name: Parameter is undefined
    * @tc.desc: Check the result is OK (by promise)
    */
    it('ACTS_ProcessManage_killProcessesByBundleNameFA_1100', 0, async function (done) {
        console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_1100 begin');
        try {
            var result = await abilityManager.killProcessesByBundleName(undefined);
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_1100 undefined result:====>"
                + JSON.stringify(result));
            expect(result == 0).assertEqual(false);
            console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_1100 end');
            done();
        }
        catch (err) {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_1100 err:====>"
                + JSON.stringify(err));
            done();
        }
    })

    /*
    * @tc.number: ACTS_ProcessManage_killProcessesByBundleNameFA_1200
    * @tc.name: Stop an application that doesn't exist
    * @tc.desc: Check the result is OK (by promise)
    */
    it('ACTS_ProcessManage_killProcessesByBundleNameFA_1200', 0, async function (done) {
        console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_1200 begin');
        try {
            var result = await abilityManager.killProcessesByBundleName("aa.bb.notExist");
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_1200 aa.bb.notExist result:====>"
                + JSON.stringify(result));
            expect(result).assertEqual(2097215);
            console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_1200 end');
            done();
        }
        catch (err) {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_1200 err:====>"
                + JSON.stringify(err));
            done();
        }
    })

    /*
    * @tc.number: ACTS_ProcessManage_killProcessesByBundleNameFA_1300
    * @tc.name: Stop an application that exists but is not started
    * @tc.desc: Check the result is OK (by promise)
    */
    it('ACTS_ProcessManage_killProcessesByBundleNameFA_1300', 0, async function (done) {
        console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_1300 begin');
        try {
            var result = await abilityManager.killProcessesByBundleName(systemABundleName);
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_1300 without runnning result:====>"
                + JSON.stringify(result));
            expect(result).assertEqual(0);
            console.log('ACTS_ProcessManage_killProcessesByBundleNameFA_1300 end');
            done();
        }
        catch (err) {
            console.debug("====>ACTS_ProcessManage_killProcessesByBundleNameFA_1300 err:====>"
                + JSON.stringify(err));
            done();
        }
    })


    /*
  * @tc.number: ACTS_ProcessManage_clearUpApplicationDataFA_0100
  * @tc.name: The system application cleans itself
  * @tc.desc: Check the process was cleaned up (by promise)
  */
    it('ACTS_ProcessManage_clearUpApplicationDataFA_0100', 0, async function (done) {
        console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0100 begin');
        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "promiseCleanup",
                    parameters: {
                        testTarget: systemABundleName
                    }
                },
            }
        );
        setTimeout(async function () {
            var processexist = await checkPromiseExistDelay(systemAProcessName);
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0100 processexist:====>"
                + JSON.stringify(processexist));
            expect(processexist).assertEqual(false);

            var missionexist = await checkPromiseMissionExistDelay(systemAAbilityName)
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0100 missionexist:====>"
                + JSON.stringify(missionexist));
            expect(missionexist).assertEqual(false);

            fileio.opendir(uriSystemA).then(function (dir) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0100 opendir successfully:"
                    + JSON.stringify(dir));
            }).catch(function (err) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0100 opendir failed error:" + err);
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0100 opendir failed JSON error:"
                    + JSON.stringify(err));
                expect(err.code).assertEqual('2');
            });
            fileio.opendir(uriSystemAMulti).then(function (dir) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0100 opendir Multi successfully:"
                    + JSON.stringify(dir));
            }).catch(function (err) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0100 opendir Multi failed error:" + err);
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0100 opendir Multi failed JSON error:"
                    + JSON.stringify(err));
                expect(err.code).assertEqual('2');
                console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0100 end');
                done();
            });

        }, 2000);
    })

    /*
    * @tc.number: ACTS_ProcessManage_clearUpApplicationDataFA_0200
    * @tc.name: The system application cleans other system application
    * @tc.desc: Check the process was cleaned up (by promise)
    */
    it('ACTS_ProcessManage_clearUpApplicationDataFA_0200', 0, async function (done) {
        console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0200 begin');
        var subscriber;

        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "promiseCleanup",
                    parameters: {
                        testTarget: systemBBundleName
                    }
                },
            }
        );
        var subscriber = await commonEvent.createSubscriber(subscribeInfo)
        console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0200 subscriber====>"
            + JSON.stringify(subscriber));
        await commonEvent.subscribe(subscriber, subscribeCallBack);

        async function subscribeCallBack(err, data) {

            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0200 SubscribeCallBack data.event:====>"
                + JSON.stringify(data.event));
            expect(data.event).assertEqual(eventFromSystemA);
            expect(data.data).assertEqual('0');
            commonEvent.unsubscribe(subscriber, unSubscribeCallback);

            var processexist = await checkPromiseExistDelay(systemBProcessName);
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0200 subscribeCallBack processexist:"
                + JSON.stringify(processexist));
            expect(processexist).assertEqual(false);
            var missionexist = await checkPromiseMissionExistDelay(systemBAbilityName)
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0200 subscribeCallBack missionexist:"
                + JSON.stringify(missionexist));
            expect(missionexist).assertEqual(false);

            fileio.opendir(uriSystemB).then(function (dir) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0200 opendir successfully:"
                    + JSON.stringify(dir));
            }).catch(function (err) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0200 opendir failed error:" + err);
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0200 opendir failed JSON error:"
                    + JSON.stringify(err));
                expect(err.code).assertEqual('2');
                console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0200 end');
                done();
            });
        }
        function unSubscribeCallback() {
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0200 UnSubscribe CallBack====>");
        }
    })

    /*
    * @tc.number: ACTS_ProcessManage_clearUpApplicationDataFA_0300
    * @tc.name: The system application cleans third-party application
    * @tc.desc: Check the process was cleaned up (by promise)
    */
    it('ACTS_ProcessManage_clearUpApplicationDataFA_0300', 0, async function (done) {
        console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0300 begin');
        var subscriber;

        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "promiseCleanup",
                    parameters: {
                        testTarget: vendorABundleName
                    }
                },
            }
        );
        var subscriber = await commonEvent.createSubscriber(subscribeInfo)
        console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0300 subscriber====>"
            + JSON.stringify(subscriber));
        await commonEvent.subscribe(subscriber, subscribeCallBack);

        async function subscribeCallBack(err, data) {

            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0300 SubscribeCallBack data.event:====>"
                + JSON.stringify(data));
            expect(data.event).assertEqual(eventFromSystemA);
            expect(data.data).assertEqual('0');
            commonEvent.unsubscribe(subscriber, unSubscribeCallback);
            setTimeout(async function () {
                var processexist = await checkPromiseExistDelay(vendorAProcessName);
                console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0300 subscribeCallBack processexist:"
                    + JSON.stringify(processexist));
                expect(processexist).assertEqual(false);
                var missionexist = await checkPromiseMissionExistDelay(vendorAAbilityName)
                console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0300 subscribeCallBack missionexist:"
                    + JSON.stringify(missionexist));
                expect(missionexist).assertEqual(false);

                fileio.opendir(uriVendorA).then(function (dir) {
                    console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0300 opendir successfully:"
                        + JSON.stringify(dir));
                }).catch(function (err) {
                    console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0300 opendir failed error:" + err);
                    console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0300 opendir failed JSON error:"
                        + JSON.stringify(err));
                    expect(err.code).assertEqual('2');
                    console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0300 end');
                    done();
                });
            }, 6000);
        }
        function unSubscribeCallback() {
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0300 UnSubscribe CallBack====>");
        }
    })

    /*
    * @tc.number: ACTS_ProcessManage_clearUpApplicationDataFA_0400
    * @tc.name: The system application cleans multiprocess application
    * @tc.desc: Check the process was cleaned up (by promise)
    */
    it('ACTS_ProcessManage_clearUpApplicationDataFA_0400', 0, async function (done) {
        console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0400 begin');
        var flagProcessA = 0;
        var flagProcessAMulti = 0;
        var flagMissionA = 0;
        var flagMissionAMulti = 0;
        var numMax = 1024;

        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "promiseCleanup",
                    parameters: {
                        testTarget: systemAMultiAbilityName
                    }
                },
            }
        );
        setTimeout(async function () {

            var processInfos = await abilitymanager.getActiveProcessInfos();
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0400 processInfos====>"
                + JSON.stringify(processInfos));
            for (var i = 0; i < processInfos.length; i++) {
                if (processInfos[i].processName == systemAProcessName) {
                    flagProcessA++;
                }
                if (processInfos[i].processName == systemAMultiProcessName) {
                    flagProcessAMulti++;
                }
            }
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0400 flagProcessA====>"
                + JSON.stringify(flagProcessA));
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0400 flagProcessAMulti====>"
                + JSON.stringify(flagProcessAMulti));
            expect(flagProcessA).assertEqual(0);
            expect(flagProcessAMulti).assertEqual(0);

            var missionInfos = await missionmanager.getMissionInfos('', numMax);
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0400 missionInfos====>"
                + JSON.stringify(missionInfos));
            for (var i = 0; i < missionInfos.length; i++) {
                if ((missionInfos[i].want.abilityName == systemAAbilityName)
                    && (missionInfos[i].runningState == 0)) {
                    flagMissionA++;
                }
                if ((missionInfos[i].want.abilityName == systemAMultiAbilityName)
                    && (missionInfos[i].runningState == 0)) {
                    flagMissionAMulti++;
                }
            }
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0400 flagMissionA====>"
                + JSON.stringify(flagMissionA));
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0400 flagMissionAMulti====>"
                + JSON.stringify(flagMissionAMulti));
            expect(flagMissionA).assertEqual(0);
            expect(flagMissionAMulti).assertEqual(0);

            fileio.opendir(uriSystemA).then(function (dir) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0400 opendir successfully:"
                    + JSON.stringify(dir));
            }).catch(function (err) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0400 opendir failed error:" + err);
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0400 opendir failed JSON error:"
                    + JSON.stringify(err));
                expect(err.code).assertEqual('2');
            });
            fileio.opendir(uriSystemAMulti).then(function (dir) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0400 opendir Multi successfully:"
                    + JSON.stringify(dir));
            }).catch(function (err) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0400 opendir Multi failed error:" + err);
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0400 opendir Multi failed JSON error:"
                    + JSON.stringify(err));
                expect(err.code).assertEqual('2');
                console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0400 end');
                done();
            });
        }, 6000);
    })

    /*
        * @tc.number: ACTS_ProcessManage_clearUpApplicationDataFA_0500
        * @tc.name: The system application cleans itself
        * @tc.desc: Check the process was cleaned up (by AsyncCallback)
        */
    it('ACTS_ProcessManage_clearUpApplicationDataFA_0500', 0, async function (done) {
        console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0500 begin');
        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "asyncCleanup",
                    parameters: {
                        testTarget: systemABundleName
                    }
                },
            }
        );
        setTimeout(async function () {
            var processexist = await checkPromiseExistDelay(systemAProcessName);
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0500 processexist:====>"
                + JSON.stringify(processexist));
            expect(processexist).assertEqual(false);

            var missionexist = await checkPromiseMissionExistDelay(systemAAbilityName)
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0500 missionexist:====>"
                + JSON.stringify(missionexist));
            expect(missionexist).assertEqual(false);

            fileio.opendir(uriSystemA).then(function (dir) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0500 opendir successfully:"
                    + JSON.stringify(dir));
            }).catch(function (err) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0500 opendir failed error:" + err);
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0500 opendir failed JSON error:"
                    + JSON.stringify(err));
                expect(err.code).assertEqual('2');
            });
            fileio.opendir(uriSystemAMulti).then(function (dir) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0500 opendir Multi successfully:"
                    + JSON.stringify(dir));
            }).catch(function (err) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0500 opendir Multi failed error:" + err);
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0500 opendir Multi failed JSON error:"
                    + JSON.stringify(err));
                expect(err.code).assertEqual('2');
                console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0500 end');
                done();
            });
        }, 6000);
    })

    /*
    * @tc.number: ACTS_ProcessManage_clearUpApplicationDataFA_0600
    * @tc.name: The system application cleans other system application
    * @tc.desc: Check the process was cleaned up (by AsyncCallback)
    */
    it('ACTS_ProcessManage_clearUpApplicationDataFA_0600', 0, async function (done) {
        console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0600 begin');
        var subscriber;

        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "asyncCleanup",
                    parameters: {
                        testTarget: systemBBundleName
                    }
                },
            }
        );
        await commonEvent.createSubscriber(subscribeInfo).then(async (data) => {
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0600 Create Subscriber data====>"
                + data);
            subscriber = data;
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0600 subscriber====>"
                + JSON.stringify(subscriber));
            await commonEvent.subscribe(subscriber, subscribeCallBack);
        });
        async function subscribeCallBack(err, data) {
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0600 SubscribeCallBack data:====>"
                + JSON.stringify(data));
            expect(data.event).assertEqual(eventFromSystemA);
            expect(data.data).assertEqual('0');
            commonEvent.unsubscribe(subscriber, unSubscribeCallback);
            setTimeout(async function () {
                var processexist = await checkPromiseExistDelay(systemBProcessName);
                console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0600 processexist:====>"
                    + JSON.stringify(processexist));
                expect(processexist).assertEqual(false);

                var missionexist = await checkPromiseMissionExistDelay(systemBAbilityName)
                console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0600 missionexist:====>"
                    + JSON.stringify(missionexist));
                expect(missionexist).assertEqual(false);

                fileio.opendir(uriSystemB).then(function (dir) {
                    console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0600 opendir successfully:"
                        + JSON.stringify(dir));
                }).catch(function (err) {
                    console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0600 opendir failed error:" + err);
                    console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0600 opendir failed JSON error:"
                        + JSON.stringify(err));
                    expect(err.code).assertEqual('2');
                    console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0600 end');
                    done();
                });
            }, 6000);
        }
        function unSubscribeCallback() {
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0600 UnSubscribe CallBack====>");
        }
    })

    /*
    * @tc.number: ACTS_ProcessManage_clearUpApplicationDataFA_0700
    * @tc.name: The system application cleans third-party application
    * @tc.desc: Check the process was cleaned up (by AsyncCallback)
    */
    it('ACTS_ProcessManage_clearUpApplicationDataFA_0700', 0, async function (done) {
        console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0700 begin');
        var subscriber;
        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "asyncCleanup",
                    parameters: {
                        testTarget: vendorABundleName
                    }
                },
            }
        );
        await commonEvent.createSubscriber(subscribeInfo).then(async (data) => {
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0700 Create Subscriber data====>"
                + data);
            subscriber = data;
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0700 subscriber====>"
                + JSON.stringify(subscriber));
            await commonEvent.subscribe(subscriber, subscribeCallBack);
        });
        async function subscribeCallBack(err, data) {
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0700 SubscribeCallBack data:====>"
                + JSON.stringify(data));
            expect(data.event).assertEqual(eventFromSystemA);
            commonEvent.unsubscribe(subscriber, unSubscribeCallback);
            setTimeout(async function () {
                var processexist = await checkPromiseExistDelay(vendorAProcessName);
                console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0700 processexist:====>"
                    + JSON.stringify(processexist));
                expect(processexist).assertEqual(false);

                var missionexist = await checkPromiseMissionExistDelay(vendorAAbilityName)
                console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0700 missionexist:====>"
                    + JSON.stringify(missionexist));
                expect(missionexist).assertEqual(false);

                fileio.opendir(uriVendorA).then(function (dir) {
                    console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0700 opendir successfully:"
                        + JSON.stringify(dir));
                }).catch(function (err) {
                    console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0700 opendir failed error:" + err);
                    console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0700 opendir failed JSON error:"
                        + JSON.stringify(err));
                    expect(err.code).assertEqual('2');
                    console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0700 end');
                    done();
                });
            }, 6000);
        }
        function unSubscribeCallback() {
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0700 UnSubscribe CallBack====>");
        }
    })

    /*
    * @tc.number: ACTS_ProcessManage_clearUpApplicationDataFA_0800
    * @tc.name: The system application cleans multiprocess application
    * @tc.desc: Check the process was cleaned up (by AsyncCallback)
    */
    it('ACTS_ProcessManage_clearUpApplicationDataFA_0800', 0, async function (done) {
        console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0800 begin');
        var flagProcessA = 0;
        var flagProcessAMulti = 0;
        var flagMissionA = 0;
        var flagMissionAMulti = 0;
        var numMax = 1024;

        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "asyncCleanup",
                    parameters: {
                        testTarget: systemAMultiAbilityName
                    }
                },
            }
        );
        setTimeout(async function () {

            var processInfos = await abilitymanager.getActiveProcessInfos();
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0800 processInfos====>"
                + JSON.stringify(processInfos));
            for (var i = 0; i < processInfos.length; i++) {
                if (processInfos[i].processName == systemAProcessName) {
                    flagProcessA++;
                }
                if (processInfos[i].processName == systemAMultiProcessName) {
                    flagProcessAMulti++;
                }
            }
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0800 flagProcessA====>"
                + JSON.stringify(flagProcessA));
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0800 flagProcessAMulti====>"
                + JSON.stringify(flagProcessAMulti));
            expect(flagProcessA).assertEqual(0);
            expect(flagProcessAMulti).assertEqual(0);

            var missionInfos = await missionmanager.getMissionInfos('', numMax);
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0800 missionInfos====>"
                + JSON.stringify(missionInfos));
            for (var i = 0; i < missionInfos.length; i++) {
                if ((missionInfos[i].want.abilityName == systemAAbilityName)
                    && (missionInfos[i].runningState == 0)) {
                    flagMissionA++;
                }
                if ((missionInfos[i].want.abilityName == systemAMultiAbilityName)
                    && (missionInfos[i].runningState == 0)) {
                    flagMissionAMulti++;
                }
            }
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0800 flagMissionA====>"
                + JSON.stringify(flagMissionA));
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0800 flagMissionAMulti====>"
                + JSON.stringify(flagMissionAMulti));
            expect(flagMissionA).assertEqual(0);
            expect(flagMissionAMulti).assertEqual(0);

            fileio.opendir(uriSystemA).then(function (dir) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0800 opendir successfully:"
                    + JSON.stringify(dir));
            }).catch(function (err) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0800 opendir failed error:" + err);
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0800 opendir failed JSON error:"
                    + JSON.stringify(err));
                expect(err.code).assertEqual('2');
            });
            fileio.opendir(uriSystemAMulti).then(function (dir) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0800 opendir Multi successfully:"
                    + JSON.stringify(dir));
            }).catch(function (err) {
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0800 opendir Multi failed error:" + err);
                console.info("ACTS_ProcessManage_clearUpApplicationDataFA_0800 opendir Multi failed JSON error:"
                    + JSON.stringify(err));
                expect(err.code).assertEqual('2');
                console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0800 end');
                done();
            });
        }, 6000);
    })

    /*
   * @tc.number: ACTS_ProcessManage_clearUpApplicationDataFA_0900
   * @tc.name: Parameter is null
   * @tc.desc: Check the result is OK (by AsyncCallback)
   */
    it('ACTS_ProcessManage_clearUpApplicationDataFA_0900', 0, async function (done) {
        console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0900 begin');

        try {
            var result = await abilityManager.clearUpApplicationData(null);
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0900 null result:====>"
                + JSON.stringify(result));
            expect(result == 0).assertEqual(false);
            console.log('ACTS_ProcessManage_clearUpApplicationDataFA_0900 end');
            done();
        }
        catch (err) {
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_0900 err:====>"
                + JSON.stringify(err));
            done();
        }
    })

    /*
   * @tc.number: ACTS_ProcessManage_clearUpApplicationDataFA_1000
   * @tc.name: Parameter is ''
   * @tc.desc: Check the result is OK (by AsyncCallback)
   */
    it('ACTS_ProcessManage_clearUpApplicationDataFA_1000', 0, async function (done) {
        console.log('ACTS_ProcessManage_clearUpApplicationDataFA_1000 begin');
        abilityManager.clearUpApplicationData('', (err, data) => {
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_1000 '' result:====>"
                + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            console.log('ACTS_ProcessManage_clearUpApplicationDataFA_1000 end');
            done();
        });
    })

    /*
    * @tc.number: ACTS_ProcessManage_clearUpApplicationDataFA_1100
    * @tc.name: Parameter is undefined
    * @tc.desc: Check the result is OK (by AsyncCallback)
    */
    it('ACTS_ProcessManage_clearUpApplicationDataFA_1100', 0, async function (done) {
        console.log('ACTS_ProcessManage_clearUpApplicationDataFA_1100 begin');
        try {
            var result = await abilityManager.clearUpApplicationData(undefined);
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_1100 undefined result:====>"
                + JSON.stringify(result));
            expect(result == 0).assertEqual(false);
            console.log('ACTS_ProcessManage_clearUpApplicationDataFA_1100 end');
            done();
        }
        catch (err) {
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_1100 err:====>"
                + JSON.stringify(err));
            done();
        }
    })

    /*
    * @tc.number: ACTS_ProcessManage_clearUpApplicationDataFA_1200
    * @tc.name: Stop an application that doesn't exist
    * @tc.desc: Check the result is OK (by AsyncCallback)
    */
    it('ACTS_ProcessManage_clearUpApplicationDataFA_1200', 0, async function (done) {
        console.log('ACTS_ProcessManage_clearUpApplicationDataFA_1200 begin');
        abilityManager.clearUpApplicationData("aa.bb.notExist", (err, data) => {
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_1200 aa.bb.notExist result:====>"
                + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            console.log('ACTS_ProcessManage_clearUpApplicationDataFA_1200 end');
            done();
        });
    })

    /*
    * @tc.number: ACTS_ProcessManage_clearUpApplicationDataFA_1300
    * @tc.name: Stop an application that exists but is not started
    * @tc.desc: Check the result is OK (by AsyncCallback)
    */
    it('ACTS_ProcessManage_clearUpApplicationDataFA_1300', 0, async function (done) {
        console.log('ACTS_ProcessManage_clearUpApplicationDataFA_1300 begin');
        abilityManager.clearUpApplicationData(systemABundleName, (err, data) => {
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_1300 without running result:====>"
                + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            setTimeout(async function () {
                fileio.opendir(uriSystemA).then(function (dir) {
                    console.info("ACTS_ProcessManage_clearUpApplicationDataFA_1300 opendir successfully:"
                        + JSON.stringify(dir));
                    console.log('ACTS_ProcessManage_clearUpApplicationDataFA_1300 end');
                    done();
                }).catch(function (err) {
                    console.info("ACTS_ProcessManage_clearUpApplicationDataFA_1300 opendir failed error:" + err);
                    console.info("ACTS_ProcessManage_clearUpApplicationDataFA_1300 opendir failed JSON error:"
                        + JSON.stringify(err));
                    expect(err.code).assertEqual('2');
                    console.log('ACTS_ProcessManage_clearUpApplicationDataFA_1300 end');
                    done();
                });
            }, 4000);
        });
    })

    /*
 * @tc.number: ACTS_ProcessManage_clearUpApplicationDataFA_1400
 * @tc.name: The system application cleans third-party application for ten times
 * @tc.desc: Check the process was cleaned up (by AsyncCallback)
 */
    it('ACTS_ProcessManage_clearUpApplicationDataFA_1400', 0, async function (done) {
        console.log('ACTS_ProcessManage_clearUpApplicationDataFA_1400 begin');
        var subscriber;
        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: systemABundleName,
                    abilityName: systemAAbilityName,
                    action: "promiseCleanupTentimes",
                    parameters: {
                        testTarget: vendorABundleName
                    }
                },
            }
        );

        await commonEvent.createSubscriber(subscribeInfo).then(async (data) => {
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_1400 Create Subscriber data====>"
                + data);
            subscriber = data;
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_1400 subscriber====>"
                + JSON.stringify(subscriber));
            await commonEvent.subscribe(subscriber, subscribeCallBack);
        });

        async function subscribeCallBack(err, data) {
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_1400 Subscribe CallBack data:====>"
                + JSON.stringify(data));
            expect(data.event).assertEqual(eventFromSystemA);
            commonEvent.unsubscribe(subscriber, unSubscribeCallback);
            setTimeout(async function () {
                var processexist = await checkPromiseExistDelay(vendorAProcessName);
                console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_1400 processexist:====>"
                    + JSON.stringify(processexist));
                expect(processexist).assertEqual(false);

                var missionexist = await checkPromiseMissionExistDelay(vendorAAbilityName)
                console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_1400 missionexist:====>"
                    + JSON.stringify(missionexist));
                expect(missionexist).assertEqual(false);

                fileio.opendir(uriVendorA).then(function (dir) {
                    console.info("ACTS_ProcessManage_clearUpApplicationDataFA_1400 opendir successfully:"
                        + JSON.stringify(dir));
                }).catch(function (err) {
                    console.info("ACTS_ProcessManage_clearUpApplicationDataFA_1400 opendir failed error:" + err);
                    console.info("ACTS_ProcessManage_clearUpApplicationDataFA_1400 opendir failed JSON error:"
                        + JSON.stringify(err));
                    expect(err.code).assertEqual('2');
                    console.log('ACTS_ProcessManage_clearUpApplicationDataFA_1400 end');
                    done();
                });
            }, 15000);
        }
        function unSubscribeCallback() {
            console.debug("====>ACTS_ProcessManage_clearUpApplicationDataFA_1400 UnSubscribe CallBack====>");
        }
    })
})