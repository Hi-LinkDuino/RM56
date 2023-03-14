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

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import client from '@ohos.update';

let updater;
let param_value1 = 0;
let param_value2 = 0;
let expect_value = 0;

let updateState = {
    UPDATE_STATE_INIT : 0,
    UPDATE_STATE_CHECK_VERSION_ON : 10,
    UPDATE_STATE_CHECK_VERSION_FAIL : 11,
    UPDATE_STATE_CHECK_VERSION_SUCCESS : 12,
    UPDATE_STATE_DOWNLOAD_ON : 20,
    UPDATE_STATE_DOWNLOAD_PAUSE : 21,
    UPDATE_STATE_DOWNLOAD_CANCEL : 22,
    UPDATE_STATE_DOWNLOAD_FAIL : 23,
    UPDATE_STATE_DOWNLOAD_SUCCESS : 24,
    UPDATE_STATE_VERIFY_ON : 30,
    UPDATE_STATE_VERIFY_FAIL : 31,
    UPDATE_STATE_VERIFY_SUCCESS : 32,
    UPDATE_STATE_PACKAGE_TRANS_ON : 70,
    UPDATE_STATE_PACKAGE_TRANS_FAIL : 71,
    UPDATE_STATE_PACKAGE_TRANS_SUCCESS : 72,
    UPDATE_STATE_INSTALL_ON : 80,
    UPDATE_STATE_INSTALL_FAIL : 81,
    UPDATE_STATE_INSTALL_SUCCESS : 82,
    UPDATE_STATE_UPDATE_ON : 90,
    UPDATE_STATE_UPDATE_FAIL : 91,
    UPDATE_STATE_UPDATE_SUCCESS : 92
};

describe('updateclient', function() {
    beforeAll(function() {
        param_value1 = 444;
        param_value2 = 555;
        expect_value = 100;
    });
    beforeEach(function() {
        if (updater === undefined) {
            updater = client.getUpdater('/data/updater/updater.zip', 'OTA');
            console.info("beforeEach updater" + updater);
        }
    });
    afterEach(function() {
        updater = None;
    });
    afterAll(function() {
        param_value1 = 0;
        param_value2 = 0;
        expect_value = 0;
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0001
     * @tc.name      testGetUpdate
     * @tc.desc      Test get update obj.
     */
    it('testGetUpdate', 0, function() {
        console.info('testGetUpdate START');
        try {
            if (client === undefined) {
                console.error("client not defined");
                return;
            }
            let tmpUpdater = client.getUpdater('/data/updater/updater.zip', 'OTA');
            expect(tmpUpdater != undefined).assertTrue();
        } catch(e) {
            console.error('[testGetUpdate] fail.' + e);
        }

        console.info('[client.getUpdate] success.');
        console.info('testGetUpdate END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0002
     * @tc.name      testGetUpdate2
     * @tc.desc      Test incorrect parameters.
     */
    it('testGetUpdate2', 0, function() {
        console.info('testGetUpdate2 START');
        try {
            if (client === undefined) {
                console.error("client not defined");
                return;
            }
            let tmpUpdater = client.getUpdater('/data/updater/updater.zip', '44444');
            console.info("testGetUpdate3 updater" + tmpUpdater);
            expect(tmpUpdater === undefined).assertTrue();
        } catch(e) {
            console.error('[testGetUpdate] fail.' + e);
        }

        console.info('[client.getUpdate] success.');
        console.info('testGetUpdate2 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0003
     * @tc.name      testGetUpdate3
     * @tc.desc      Test incorrect parameters.
     */
    it('testGetUpdate3', 0, function() {
        console.info('testGetUpdate3 START');
        try {
            if (client === undefined) {
                console.error("client not defined");
                return;
            }
            let tmpUpdater = client.getUpdater('/data/updater/updater.zip', 'patch');
            console.info("testGetUpdate3 updater" + tmpUpdater);
            expect(tmpUpdater != undefined).assertTrue();
            if (tmpUpdater === undefined) {
                console.error("Fail to get updater");
                return;
            }

        } catch(e) {
            console.error('[testGetUpdate] fail.' + e);
        }

        console.info('[client.getUpdate] success.');
        console.info('testGetUpdate3 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0004
     * @tc.name      testGetUpdate4
     * @tc.desc      Test incorrect parameters.
     */
    it('testGetUpdate4', 0, function() {
        console.info('testGetUpdate4 START');
        try {
            if (client === undefined) {
                console.error("client not defined");
                return;
            }
            let tmpUpdater = client.getUpdater(function(){
            });
            expect(tmpUpdater === undefined).assertTrue();
        } catch(e) {
            console.error('[testGetUpdate4] fail.' + e);
        }

        console.info('[client.getUpdate] success.');
        console.info('testGetUpdate4 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0005
     * @tc.name      testGetUpdate5
     * @tc.desc      Test incorrect parameters.
     */
    it('testGetUpdate5', 0, function() {
        console.info('testGetUpdate5 START');
        try {
            if (client === undefined) {
                console.error("client not defined");
                return;
            }
            let tmpUpdater = client.getUpdater(param_value1, param_value2);
            expect(tmpUpdater === undefined).assertTrue();
        } catch(e) {
            console.error('[testGetUpdate] fail.' + e);
        }

        console.info('[client.getUpdate] success.');
        console.info('testGetUpdate5 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0006
     * @tc.name      testGetNewVersionInfo1
     * @tc.desc      Get the current system version.
     */
    it('testGetNewVersionInfo1', 0, function() {
        console.info('testGetNewVersionInfo1 START');
        try {
            let ret = updater.getNewVersionInfo(function(err, data){
                console.log("getNewVersionInfo success" + data);
                expect(data.status).assertEqual(0); // Has new version.
            });
            expect(ret).assertInstanceOf('Number');
        } catch(e) {
            console.error('[testGetUpdate] fail.' + e);
        }
        console.info('[getNewVersionInfo] success.');
        console.info('testGetNewVersionInfo1 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0007
     * @tc.name      testGetNewVersionInfo2
     * @tc.desc      Test incorrect parameters.
     */
     it('testGetNewVersionInfo2', 0, function() {
        console.info('testGetNewVersionInfo2 START');
        try {
            let ret = updater.getNewVersionInfo();
            expect(ret != 0).assertTrue();
            console.info('[getNewVersionInfo] success.');
            console.info('testGetNewVersionInfo2 END');
        } catch(e) {
            console.error('[testGetUpdate] fail.' + e);
        }
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0008
     * @tc.name      testGetNewVersionInfo3
     * @tc.desc      Test incorrect parameters.
     */
    it('testGetNewVersionInfo3', 0, function() {
        console.info('testGetNewVersionInfo3 START');
        try {
            let ret = updater.getNewVersionInfo("dddddd");
            expect(ret != 0).assertTrue();
            console.info('[getNewVersionInfo] success.');
            console.info('testGetNewVersionInfo2 END');
        } catch(e) {
            console.error('[testGetUpdate] fail.' + e);
        }
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0009
     * @tc.name      testGetNewVersionInfo4
     * @tc.desc      Test incorrect parameters.
     */
    it('testGetNewVersionInfo4', 0, function() {
        console.info('testGetNewVersionInfo4 START');
        try {
            let ret = updater.getNewVersionInfo(param_value1);
            expect(ret != 0).assertTrue();
            console.info('[getNewVersionInfo] success.');
            console.info('testGetNewVersionInfo5 END');
        } catch(e) {
            console.error('[testGetUpdate] fail.' + e);
        }
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0010
     * @tc.name      testGetNewVersionInfo5
     * @tc.desc      Test incorrect parameters.
     */
     it('testGetNewVersionInfo5', 0, function() {
        console.info('testGetNewVersionInfo5 START');
        try {
            let ret = updater.getNewVersionInfo(param_value1, param_value2);
            expect(ret != 0).assertTrue();
            console.info('[getNewVersionInfo] success.');
            console.info('testGetNewVersionInfo5 END');
        } catch(e) {
            console.error('[testGetUpdate] fail.' + e);
        }
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0011
     * @tc.name      testGetNewVersionInfoPromise
     * @tc.desc      Called by Promise to get the new version.
     */
     it('testGetNewVersionInfoPromise', 0, function() {
        console.info('testGetNewVersionInfoPromise START');
        try {
            let ret = updater.getNewVersionInfo().then(data => {
                console.log("getNewVersionInfo success  " + data);
                expect(data.status).assertEqual(0); // Has new version.
                expect(data.checkResults.size > 0).assertTrue();
                console.log(`info versionName = ` + data.checkResults[0].versionName);
                console.log(`info versionCode = ` + data.checkResults[0].versionCode);
                console.log(`info verifyInfo = ` + data.checkResults[0].verifyInfo);
                console.log(`info descriptionId = ` + data.checkResults[0].descriptionId);
                console.log(`info content = ` + data.descriptionInfo[0].content);
                console.log(`info size = ` + data.checkResults[0].size);
            }).catch(error =>
                console.error("getNewVersionInfo catch " + error)
            );
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error("getNewVersionInfo catch " + e);
        }
        console.info('[getNewVersionInfo] success.');
        console.info('testGetNewVersionInfoPromise END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0012
     * @tc.name      testGetNewVersionInfoPromise2
     * @tc.desc      Test incorrect parameters.
     */
     it('testGetNewVersionInfoPromise2', 0, function() {
        console.info('testGetNewVersionInfoPromise2 START');
        try {
            let ret = updater.getNewVersionInfo(param_value1).then(data => {
                console.log("getNewVersionInfo success  " + data);
            }).catch(error =>
                console.error("getNewVersionInfo catch " + error)
            );
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error("getNewVersionInfo catch " + e);
        }
        console.info('[getNewVersionInfo] success.');
        console.info('testGetNewVersionInfoPromise2 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0013
     * @tc.name      testGetNewVersionInfoPromise3
     * @tc.desc      Test incorrect parameters.
     */
     it('testGetNewVersionInfoPromise3', 0, function() {
        console.info('testGetNewVersionInfoPromise3 START');
        try {
            let ret = updater.getNewVersionInfo(param_value1, param_value2).then(data => {
            }).catch(error =>
                console.error("getNewVersionInfo catch " + error)
            );
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error("getNewVersionInfo catch " + e);
        }
        console.info('[getNewVersionInfo] success.');
        console.info('testGetNewVersionInfoPromise3 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0014
     * @tc.name      testGetNewVersionInfoPromise4
     * @tc.desc      Test incorrect parameters.
     */
     it('testGetNewVersionInfoPromise4', 0, function() {
        console.info('testGetNewVersionInfoPromise4 START');
        try {
            let ret = updater.getNewVersionInfo("44444444").then(data => {
            }).catch(error =>
                console.error("getNewVersionInfo catch " + error)
            );
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error("getNewVersionInfo catch " + e);
        }
        console.info('[getNewVersionInfo] success.');
        console.info('testGetNewVersionInfoPromise4 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0015
     * @tc.name      testGetNewVersionInfoPromise5
     * @tc.desc      Test incorrect parameters.
     */
     it('testGetNewVersionInfoPromise5', 0, function() {
        console.info('testGetNewVersionInfoPromise5 START');
        try {
            let ret = updater.getNewVersionInfo("44444444", "5555555").then(data => {
            }).catch(error =>
                console.error("getNewVersionInfo error " + error)
            );
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error("getNewVersionInfo catch " + e);
        }
        console.info('[getNewVersionInfo] success.');
        console.info('testGetNewVersionInfoPromise5 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0016
     * @tc.name      testCheckNewVersion1
     * @tc.desc      Check the new version.
     */
    it('testCheckNewVersion1', 0, function() {
        console.info('testCheckNewVersion1 START');
        try {
            let ret = updater.checkNewVersion(function(err, data){
                console.log("checkNewVersion success" + data);
                expect(data.status).assertEqual(0); // Has new version.
                expect(data.checkResults.size > 0).assertTrue();
                console.log(`info versionName = ` + data.checkResults[0].versionName);
                console.log(`info versionCode = ` + data.checkResults[0].versionCode);
                console.log(`info verifyInfo = ` + data.checkResults[0].verifyInfo);
                console.log(`info descriptionId = ` + data.checkResults[0].descriptionId);
                console.log(`info content = ` + data.descriptionInfo[0].content);
                console.log(`info size = ` + data.checkResults[0].size);
            },
            error => {
                console.log("checkNewVersion error" + error)
            });
            expect(ret).assertInstanceOf('Number');
        } catch(e) {
            console.error("checkNewVersion catch " + e);
        }
        console.info('[checkNewVersion] success.');
        console.info('testCheckNewVersion1 END');
    });

   /**
     * @tc.number    SUB_UPDATE_JS_API_0017
     * @tc.name      testCheckNewVersion2
     * @tc.desc      Test incorrect parameters.
     */
    it('testCheckNewVersion2', 0, function() {
        console.info('testCheckNewVersion2 START');
        try {
            let ret = updater.checkNewVersion();
            expect(ret != 0).assertTrue();
            console.info('[checkNewVersion] success.' + ret);
            console.info('testCheckNewVersion2 END');
        } catch(e) {
            console.error("checkNewVersion catch " + e);
        }
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0018
     * @tc.name      testCheckNewVersion3
     * @tc.desc      Test incorrect parameters.
     */
    it('testCheckNewVersion3', 0, function() {
        console.info('testCheckNewVersion3 START');
        try {
            let ret = updater.checkNewVersion("33333");
            expect(ret != 0).assertTrue();
            console.info('[checkNewVersion] success.' + ret);
            console.info('testCheckNewVersion3 END');
        } catch(e) {
            console.error("checkNewVersion catch " + e);
        }
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0019
     * @tc.name      testCheckNewVersion4
     * @tc.desc      Test incorrect parameters.
     */
    it('testCheckNewVersion4', 0, function() {
        console.info('testCheckNewVersion4 START');
        try {
            let ret = updater.checkNewVersion(param_value1);
            expect(ret != 0).assertTrue();
            console.info('[checkNewVersion] success.' + ret);
            console.info('testCheckNewVersion4 END');
        } catch(e) {
            console.error("checkNewVersion catch " + e);
        }
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0020
     * @tc.name      testCheckNewVersion5
     * @tc.desc      Test incorrect parameters.
     */
     it('testCheckNewVersion5', 0, function() {
        console.info('testCheckNewVersion4 START');
        try {
            let ret = updater.checkNewVersion(param_value1, param_value2);
            expect(ret != 0).assertTrue();
            console.info('[checkNewVersion] success.' + ret);
            console.info('testCheckNewVersion5 END');
        } catch(e) {
            console.error("checkNewVersion catch " + e);
        }
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0021
     * @tc.name      testCheckNewVersionPromise
     * @tc.desc      Called by promise, the result is the new Promise version.
     */
    it('testCheckNewVersionPromise', 0, function() {
        console.info('testCheckNewVersionPromise START');
        try {
            let ret = updater.checkNewVersion().then(data => {
                console.log("checkNewVersion success" + data);
                expect(data.status).assertEqual(0);
                expect(data.checkResults.size > 0).assertTrue();
                console.log(`info versionName = ` + data.checkResults[0].versionName);
                console.log(`info versionCode = ` + data.checkResults[0].versionCode);
                console.log(`info verifyInfo = ` + data.checkResults[0].verifyInfo);
                console.log(`info descriptionId = ` + data.checkResults[0].descriptionId);
                console.log(`info content = ` + data.descriptionInfo[0].content);
                console.log(`info size = ` + data.checkResults[0].size);
            }).catch(error =>
                console.error("checkNewVersion error " + error)
            );
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error("checkNewVersion catch " + e)
        }
        console.info('[checkNewVersion] success.');
        console.info('testCheckNewVersionPromise END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0022
     * @tc.name      testCheckNewVersionPromise2
     * @tc.desc      Test incorrect parameters.
     */
     it('testCheckNewVersionPromise2', 0, function() {
        console.info('testCheckNewVersionPromise2 START');
        try{
            let ret = updater.checkNewVersion(2).then(data => {
            }).catch(error =>
                console.error("checkNewVersion error " + error)
            );
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error("checkNewVersion catch " + e);
        }
        console.info('[checkNewVersion] success.');
        console.info('testCheckNewVersionPromise2 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0023
     * @tc.name      testCheckNewVersionPromise3
     * @tc.desc      Test incorrect parameters.
     */
     it('testCheckNewVersionPromise3', 0, function() {
        console.info('testCheckNewVersionPromise3 START');
        try{
            let ret = updater.checkNewVersion(2, 4).then(data => {
            }).catch(error =>
            console.error("checkNewVersion error " + error)
            );
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error("checkNewVersion catch " + e);
        }
        console.info('[checkNewVersion] success.');
        console.info('testCheckNewVersionPromise3 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0024
     * @tc.name      testCheckNewVersionPromise4
     * @tc.desc      Test incorrect parameters.
     */
     it('testCheckNewVersionPromise4', 0, function() {
        console.info('testCheckNewVersionPromise4 START');
        try{
            let ret = updater.checkNewVersion("3333").then(data => {
            }).catch(error =>
            console.error("checkNewVersion error " + error)
            );
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error("checkNewVersion catch " + e);
        }
        console.info('[checkNewVersion] success.');
        console.info('testCheckNewVersionPromise4 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0025
     * @tc.name      testCheckNewVersionPromise5
     * @tc.desc      Test incorrect parameters.
     */
     it('testCheckNewVersionPromise5', 0, function() {
        console.info('testCheckNewVersionPromise5 START');
        try{
            let ret = updater.checkNewVersion("3333", "44444").then(data => {
            }).catch(error =>
            console.error("checkNewVersion error " + error)
            );
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error("checkNewVersion catch " + e);
        }
        console.info('[checkNewVersion] success.');
        console.info('testCheckNewVersionPromise5 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0026
     * @tc.name      testSetUpdatePolicy001
     * @tc.desc      Test and set the upgrade strategy, check whether the result is correct.
     */
    it('testSetUpdatePolicy001', 0, function() {
        console.info('testSetUpdatePolicy001 START' + updater);
        // Set strategy.
        let policy = {
            autoDownload: false,
            autoDownloadNet: true,
            mode: 2,
            autoUpgradeInterval: [ 2, 3 ],
            autoUpgradeCondition: 2
        }
        try {
            let ret = updater.setUpdatePolicy(policy, function(err, data){
                console.log("setUpdatePolicy success" + data);
                expect(data).assertEqual(0);
            });
            expect(ret).assertInstanceOf('Number');
        } catch(e) {
            console.error('[testSetUpdatePolicy001] fail.' + e);
        }
        console.info('[setUpdatePolicy] success.');
        console.info('testSetUpdatePolicy001 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0027
     * @tc.name      testGetUpdatePolicy001
     * @tc.desc      Test get update, policy func 001.
     */
    it('testGetUpdatePolicy001', 0, function() {
        console.info('testGetUpdatePolicy001 START');
        try {
            let ret = updater.getUpdatePolicy(function(err, data){
                console.log("getUpdatePolicy success" + data);
                expect(data.autoDownload).assertEqual(false);
                expect(data.autoDownloadNet).assertEqual(true);
                expect(data.mode).assertEqual(2);
                expect(data.autoUpgradeInterval[0]).assertEqual(2);
                expect(data.autoUpgradeInterval[1]).assertEqual(3);

            });
            expect(ret).assertInstanceOf('Number');
        } catch(e) {
            console.error('[testGetUpdatePolicy001] fail.' + e);
        }
        console.info('[getUpdatePolicy] success.');
        console.info('testGetUpdatePolicy001 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0028
     * @tc.name      testSetUpdatePolicy002
     * @tc.desc      Modify the upgrade strategy and check whether the result is correct.
     */
    it('testSetUpdatePolicy002', 0, function() {
        console.info('testSetUpdatePolicy002 START' + updater);
        let policy = {
            autoDownload: true,
            autoDownloadNet: true,
            mode: 2,
            autoUpgradeInterval: [ 2, 3 ],
            autoUpgradeCondition: 2
        }
        try {
            let ret = updater.setUpdatePolicy(policy, function(err, data){
                console.log("setUpdatePolicy success" + data);
                expect(data).assertEqual(0);
            });
            expect(ret).assertInstanceOf('Number');
        } catch(e) {
            console.error('[testSetUpdatePolicy002] fail.' + e);
        }

        console.info('[setUpdatePolicy] success.');
        console.info('testSetUpdatePolicy002 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0029
     * @tc.name      testGetUpdatePolicy002
     * @tc.desc      Get the modified strategy.
     */
    it('testGetUpdatePolicy002', 0, function() {
        console.info('testGetUpdatePolicy002 START');
        try {
            let ret = updater.getUpdatePolicy(function(err, data){
                console.log("getUpdatePolicy success" + data);
                expect(data.autoDownload).assertEqual(true);
                expect(data.autoDownloadNet).assertEqual(true);
                expect(data.mode).assertEqual(2);
                expect(data.autoUpgradeInterval[0]).assertEqual(2);
                expect(data.autoUpgradeInterval[1]).assertEqual(3);
            });
            expect(ret).assertInstanceOf('Number');
        } catch(e) {
            console.error('[testGetUpdatePolicy002] fail.' + e);
        }

        console.info('[getUpdatePolicy] success.');
        console.info('testGetUpdatePolicy002 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0030
     * @tc.name      testSetUpdatePolicy003
     * @tc.desc      Modify strategy mode.
     */
    it('testSetUpdatePolicy003', 0, function() {
        console.info('testSetUpdatePolicy003 START' + updater);
        // Set strategy.
        let policy = {
            autoDownload: false,
            autoDownloadNet: false,
            mode: 2,
            autoUpgradeInterval: [ 2, 3 ],
            autoUpgradeCondition: 2
        }
        try {
            let ret = updater.setUpdatePolicy(policy, function(err, data){
                console.log("setUpdatePolicy success" + data);
                expect(data).assertEqual(0);
            })
            expect(ret).assertInstanceOf('Number');
        } catch(e) {
            console.error('[testSetUpdatePolicy003] fail.' + e);
        }
        console.info('[setUpdatePolicy] success.');
        console.info('testSetUpdatePolicy003 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0031
     * @tc.name      testGetUpdatePolicy003
     * @tc.desc      Obtain strategy mode.
     */
    it('testGetUpdatePolicy003', 0, function() {
        console.info('testGetUpdatePolicy003 START ' + updater);
        try {
            let ret = updater.getUpdatePolicy(function(err, data){
                console.log("getUpdatePolicy success" + data);
                expect(data.autoDownload).assertEqual(false);
                expect(data.autoDownloadNet).assertEqual(false);
                expect(data.mode).assertEqual(2);
                expect(data.autoUpgradeInterval[0]).assertEqual(2);
                expect(data.autoUpgradeInterval[1]).assertEqual(3);
            })
            expect(ret).assertInstanceOf('Number');
        } catch(e) {
            console.error('[testGetUpdatePolicy003] fail.' + e);
        }
        console.info('[getUpdatePolicy] success.');
        console.info('testGetUpdatePolicy003 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0032
     * @tc.name      testSetUpdatePolicy004
     * @tc.desc      Set policy upgrade interval.
     */
    it('testSetUpdatePolicy004', 0, function() {
        console.info('testSetUpdatePolicy004 START' + updater);
        // Set strategy.
        let policy = {
            autoDownload: false,
            autoDownloadNet: false,
            mode: 1,
            autoUpgradeInterval: [ 100, 100],
            autoUpgradeCondition: 2
        }
        try {
            let ret = updater.setUpdatePolicy(policy, function(err, data){
            console.log("setUpdatePolicy success" + data);
            expect(data).assertEqual(0);
            })
            expect(ret).assertInstanceOf('Number');
        } catch(e) {
            console.error('[testGetUpdatePolicy003] fail.' + e);
        }
        console.info('[setUpdatePolicy] success.');
        console.info('testSetUpdatePolicy004 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0033
     * @tc.name      testGetUpdatePolicy004
     * @tc.desc      Obtain policy upgrade interval.
     */
    it('testGetUpdatePolicy004', 0, function() {
        console.info('testGetUpdatePolicy004 START' + updater);
        try {
            let ret = updater.getUpdatePolicy(function(err, data){
                console.log("getUpdatePolicy success" + data);
                expect(data.autoDownload).assertEqual(false);
                expect(data.autoDownloadNet).assertEqual(false);
                expect(data.mode).assertEqual(1);
                expect(data.autoUpgradeInterval[0]).assertEqual(expect_value);
                expect(data.autoUpgradeInterval[1]).assertEqual(expect_value);
            })
            expect(ret).assertInstanceOf('Number');
        } catch(e) {
            console.error('[testGetUpdatePolicy004] fail.' + e);
        }
        console.info('[getUpdatePolicy] success.');
        console.info('testGetUpdatePolicy004 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0034
     * @tc.name      testSetUpdatePolicy005
     * @tc.desc      Test incorrect parameters.
     */
     it('testSetUpdatePolicy005', 0, function() {
        console.info('testSetUpdatePolicy005 START' + updater);
        try {
            let ret = updater.setUpdatePolicy("", function(err, data){
                console.log("setUpdatePolicy success" + data);
                expect(data).assertEqual(0);
            });
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error('[testSetUpdatePolicy005] fail.' + e);
        }
        console.info('[setUpdatePolicy] success.');
        console.info('testSetUpdatePolicy005 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0035
     * @tc.name      testGetUpdatePolicy005
     * @tc.desc      Test incorrect parameters.
     */
    it('testGetUpdatePolicy005', 0, function() {
        console.info('testGetUpdatePolicy005 START' + updater);
        try {
            let ret = updater.getUpdatePolicy("", function(err, data){
            })
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error('[testSetUpdatePolicy005] fail.' + e);
        }
        console.info('[getUpdatePolicy] success.');
        console.info('testGetUpdatePolicy005 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0036
     * @tc.name      testSetUpdatePolicyPromise
     * @tc.desc      Promise method call.
     */
    it('testSetUpdatePolicyPromise', 0, function() {
        console.info('testSetUpdatePolicyPromise START' + updater);
        // Set strategy.
        let policy = {
            autoDownload: false,
            autoDownloadNet: true,
            mode: 2,
            autoUpgradeInterval: [ 2, 3 ],
            autoUpgradeCondition: 2
        }
        try {
            let ret = updater.setUpdatePolicy(policy).then(data => {
                console.log("setUpdatePolicy success" + data);
                expect(data).assertEqual(0);
            }).catch(error =>
                console.error("setUpdatePolicy error " + error)
            );
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error('[testSetUpdatePolicyPromise] fail.' + e);
        }
        console.info('[setUpdatePolicy] success.');
        console.info('testSetUpdatePolicyPromise END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0037
     * @tc.name      testGetUpdatePolicyPromise
     * @tc.desc      Promise method call.
     */
    it('testGetUpdatePolicyPromise', 0, function() {
        console.info('testGetUpdatePolicyPromise START');
        try {
            let ret = updater.getUpdatePolicy().then(data => {
                console.log("getUpdatePolicy success" + data);
                expect(data.autoDownload).assertEqual(false);
                expect(data.autoDownload).assertEqual(true);
                expect(data.mode).assertEqual(2);
                expect(data.autoUpgradeInterval[0]).assertEqual(2);
                expect(data.autoUpgradeInterval[1]).assertEqual(3);
                expect(data.autoUpgradeCondition).assertEqual(2);
            }).catch(error =>
                console.error("getUpdatePolicy catch " + error)
            );
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error('[testGetUpdatePolicyPromise] fail.' + e);
        }
        console.info('[getUpdatePolicy] success.');
        console.info('testGetUpdatePolicyPromise END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0038
     * @tc.name      testSetUpdatePolicyPromise002
     * @tc.desc      Promise method call.
     */
    it('testSetUpdatePolicyPromise002', 0, function() {
        console.info('testSetUpdatePolicyPromise002 START' + updater);
        let policy = {
            autoDownload: true,
            autoDownloadNet: true,
            mode: 2,
            autoUpgradeInterval: [ 2, 3 ],
            autoUpgradeCondition: 2
        }
        try {
            let ret = updater.setUpdatePolicy(policy).then(data => {
                console.log("setUpdatePolicy success" + data);
                expect(data).assertEqual(0);
            }).catch(error =>
                console.error("setUpdatePolicy error " + error)
            );
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error('[setUpdatePolicy] fail.' + e);
        }
        console.info('[setUpdatePolicy] success.');
        console.info('testSetUpdatePolicyPromise002 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0039
     * @tc.name      testGetUpdatePolicy002
     * @tc.desc      Promise method call.
     */
    it('testGetUpdatePolicyPromise002', 0, function() {
        console.info('testGetUpdatePolicyPromise002 START');
        try {
            let ret = updater.getUpdatePolicy().then(data => {
                console.log("getUpdatePolicy success" + data);
                expect(data.autoDownload).assertEqual(true);
                expect(data.autoDownloadNet).assertEqual(true);
                expect(data.mode).assertEqual(2);
                expect(data.autoUpgradeInterval[0]).assertEqual(2);
                expect(data.autoUpgradeInterval[1]).assertEqual(3);
            });
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error('[testGetUpdatePolicyPromise002] fail.' + e);
        }
        console.info('[getUpdatePolicy] success.');
        console.info('testGetUpdatePolicyPromise002 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0040
     * @tc.name      testSetUpdatePolicyPromise003
     * @tc.desc      Promise method call.
     */
    it('testSetUpdatePolicyPromise003', 0, function() {
        console.info('testSetUpdatePolicyPromise003 START');
        // Set strategy.
        let policy = {
            autoDownload: false,
            autoDownloadNet: false,
            mode: 2,
            autoUpgradeInterval: [ 2, 3 ],
            autoUpgradeCondition: 2
        }
        try {
            let ret = updater.setUpdatePolicy(policy).then(data => {
                console.log("setUpdatePolicy success" + data);
                expect(data).assertEqual(0);
            }).catch(error => {
                console.info('[setUpdatePolicy] error ' + error)
            })
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.error('[testSetUpdatePolicyPromise003] fail.' + e);
        }
        console.info('[setUpdatePolicy] success.');
        console.info('testSetUpdatePolicyPromise003 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0041
     * @tc.name      testGetUpdatePolicyPromise003
     * @tc.desc      Promise method call.
     */
    it('testGetUpdatePolicyPromise003', 0, function() {
        console.info('testGetUpdatePolicyPromise003 START ');
        try {
            let ret = updater.getUpdatePolicy().then(data => {
                console.log("getUpdatePolicy success" + data);
                expect(data.autoDownload).assertEqual(false);
                expect(data.autoDownloadNet).assertEqual(false);
                expect(data.mode).assertEqual(2);
                expect(data.autoUpgradeInterval[0]).assertEqual(2);
                expect(data.autoUpgradeInterval[1]).assertEqual(3);
            }).catch(error => {
                console.info('[getUpdatePolicy] error ' + error)
            })
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.info('[getUpdatePolicy] catch ' + e)
        }
        console.info('[getUpdatePolicy] success.');
        console.info('testGetUpdatePolicyPromise003 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0042
     * @tc.name      testSetUpdatePolicyPromise004
     * @tc.desc      Promise method call.
     */
    it('testSetUpdatePolicyPromise004', 0, function() {
        console.info('testSetUpdatePolicyPromise004 START' + updater);
        // Set strategy.
        let policy = {
            autoDownload: false,
            autoDownloadNet: false,
            mode: 1,
            autoUpgradeInterval: [ 100, 100],
            autoUpgradeCondition: 2
        }
        try {
            let ret = updater.setUpdatePolicy(policy).then(data => {
                console.log("setUpdatePolicy success" + data);
                expect(data).assertEqual(0);
            }).catch(error => {
                console.info('[setUpdatePolicy] error ' + error)
            })
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.info('[testSetUpdatePolicyPromise004] catch ' + e)
        }
        console.info('[setUpdatePolicy] success.');
        console.info('testSetUpdatePolicyPromise004 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0043
     * @tc.name      testGetUpdatePolicyPromise004
     * @tc.desc      Promise method call.
     */
    it('testGetUpdatePolicyPromise004', 0, function() {
        console.info('testGetUpdatePolicyPromise004 START' + updater);
        try {
            let ret = updater.getUpdatePolicy().then(data => {
                console.log("getUpdatePolicy success" + data);
                expect(data.autoDownload).assertEqual(false);
                expect(data.autoDownloadNet).assertEqual(false);
                expect(data.mode).assertEqual(1);
                expect(data.autoUpgradeInterval[0]).assertEqual(expect_value);
                expect(data.autoUpgradeInterval[1]).assertEqual(expect_value);
            }).catch(error => {
                console.info('[getUpdatePolicy] error ' + error)
            })
            expect(ret != 0).assertTrue();
        } catch(e) {
            console.info('[testSetUpdatePolicyPromise004] catch ' + e)
        }
        console.info('[getUpdatePolicy] success.');
        console.info('testGetUpdatePolicyPromise004 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0044
     * @tc.name      testSetUpdatePolicyPromise005
     * @tc.desc      Test incorrect parameters.
     */
    it('testSetUpdatePolicyPromise005', 0, function() {
        console.info('testSetUpdatePolicyPromise005 START' + updater);
        try {
            let ret = updater.setUpdatePolicy("").then(data => {
                console.log("setUpdatePolicy success" + data);
                expect(data).assertEqual(0);
            });
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[setUpdatePolicy] catch ' + e);
        }
        console.info('[setUpdatePolicy] success.');
        console.info('testSetUpdatePolicyPromise005 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0045
     * @tc.name      testGetUpdatePolicyPromise005
     * @tc.desc      Test incorrect parameters.
     */
    it('testGetUpdatePolicyPromise005', 0, function() {
        console.info('testGetUpdatePolicy005 START' + updater);
        try{
            let ret = updater.getUpdatePolicy(param_value1).then(data => {
            })
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[setUpdatePolicy] catch ' + e);
        }
        console.info('[getUpdatePolicy] success.');
        console.info('testGetUpdatePolicyPromise005 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0046
     * @tc.name      testDownloadVersion
     * @tc.desc      Unsubscribe from monitoring.
     */
    it('testDownloadVersion', 0, function() {
        console.info('testDownloadVersion START');
        // Open download monitor.
        try{
            updater.on('downloadProgress', progress => {
                console.log("downloadProgress on" + progress);
                console.log(`downloadProgress status: ` + progress.status);
                console.log(`downloadProgress percent: ` + progress.percent);
                console.log(`downloadProgress endReason: ` + progress.endReason);
                if (progress.status === updateState.UPDATE_STATE_DOWNLOAD_SUCCESS ||
                    progress.status === updateState.UPDATE_STATE_VERIFY_SUCCESS) {
                    expect(progress.percent).assertEqual(expect_value);
                    updater.off("downloadProgress");
                }
            })
            let ret = updater.download();
            expect(ret).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testDownloadVersion] catch ' + e);
        }

        console.info('[downloadVersion] success.');
        console.info('testDownloadVersion END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0047
     * @tc.name      testDownloadVersion02
     * @tc.desc      Unsubscribe out of monitoring.
     */
    it('testDownloadVersion02', 0, function() {
        console.info('testDownloadVersion02 START');
        // Open download monitor
        try{
            updater.on('downloadProgress', progress => {
                console.log("downloadProgress on" + progress);
                console.log(`downloadProgress status: ` + progress.status);
                console.log(`downloadProgress percent: ` + progress.percent);
                console.log(`downloadProgress endReason: ` + progress.endReason);
                if (progress.status === updateState.UPDATE_STATE_DOWNLOAD_SUCCESS ||
                    progress.status === updateState.UPDATE_STATE_VERIFY_SUCCESS) {
                    expect(progress.percent).assertEqual(expect_value);
                }
            })
            let ret = updater.download();
            // Cancel subscription immediately, no callback will be received.
            updater.off("downloadProgress");
            expect(ret).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testDownloadVersion02] catch ' + e);
        }
        console.info('[downloadVersion] success.');
        console.info('testDownloadVersion END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0048
     * @tc.name      testDownloadVersion03
     * @tc.desc      Test download parameter, callback is not supported.
     */
    it('testDownloadVersion03', 0, function() {
        console.info('testDownloadVersion03 START');
        try{
            let ret = updater.download(progress => {
                console.log("downloadProgress on" + progress);
                console.log(`downloadProgress status: ` + progress.status);
                console.log(`downloadProgress percent: ` + progress.percent);
                console.log(`downloadProgress endReason: ` + progress.endReason);
                if (progress.status === updateState.UPDATE_STATE_DOWNLOAD_SUCCESS ||
                    progress.status === updateState.UPDATE_STATE_VERIFY_SUCCESS) {
                    expect(progress.percent).assertEqual(expect_value);
                }
            })
            expect(ret).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testDownloadVersion03] catch ' + e);
        }
        console.info('[downloadVersion] success.');
        console.info('testDownloadVersion END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0049
     * @tc.name      testDownloadVersion04
     * @tc.desc      Test subscription without parameters, return error.
     */
    it('testDownloadVersion04', 0, function() {
        console.info('testDownloadVersion04 START');
        try{
            // Open download monitor.
            let ret = updater.on('downloadProgress');
            expect(ret).assertInstanceOf('Number');
            let ret_download = updater.download();
            expect(ret_download).assertInstanceOf('Number');
            // Cancel subscription immediately, no callback will be received.
            let ret_off = updater.off("downloadProgress");
            expect(ret_off).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testDownloadVersion04] catch ' + e);
        }
        console.info('[downloadVersion] success.');
        console.info('testDownloadVersion04 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0050
     * @tc.name      testDownloadVersion05
     * @tc.desc      Take function to subscribe.
     */
    it('testDownloadVersion05', 0, function() {
        function testFunction(progress) {
            console.log("downloadProgress on" + progress);
            console.log(`downloadProgress status: ` + progress.status);
            console.log(`downloadProgress percent: ` + progress.percent);
            console.log(`downloadProgress endReason: ` + progress.endReason);
        }
        console.info('testDownloadVersion05 START');
        try{
            // Open download monitor.
            let ret = updater.on('downloadProgress', testFunction);
            expect(ret).assertInstanceOf('Number');

            let ret_download = updater.download();
            expect(ret_download).assertInstanceOf('Number');
            // Cancel subscription immediately, no callback will be received.
            let ret_off = updater.off("downloadProgress", testFunction);
            expect(ret_off).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testDownloadVersion05] catch ' + e);
        }
        console.info('[downloadVersion] success.');
        console.info('testDownloadVersion05 END');
    });


    /**
     * @tc.number    SUB_UPDATE_JS_API_0051
     * @tc.name      testUpgradeVersion01
     * @tc.desc      Unsubscribe from monitoring.
     */
    it('testUpgradeVersion01', 0, function() {
        console.info('testUpgradeVersion01 START');
        try{
            // Open download monitor.
            updater.on('upgradeProgress', progress => {
                console.log("upgradeProgress on" + progress);
                console.log(`upgradeProgress status: ` + progress.status);
                console.log(`upgradeProgress percent: ` + progress.percent);
                console.log(`upgradeProgress endReason: ` + progress.endReason);
                if (progress.status === updateState.UPDATE_STATE_DOWNLOAD_SUCCESS ||
                    progress.status === updateState.UPDATE_STATE_VERIFY_SUCCESS) {
                    expect(progress.percent).assertEqual(expect_value);
                    updater.off("upgradeProgress");
                }
            })
            let ret = updater.upgrade();
            expect(ret).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testUpgradeVersion01] catch ' + e);
        }
        console.info('[upgrade] success.');
        console.info('testUpgradeVersion01 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0052
     * @tc.name      testUpgradeVersion02
     * @tc.desc      Unsubscribe out of monitoring.
     */
    it('testUpgradeVersion02', 0, function() {
        console.info('testUpgradeVersion02 START');
        try{
            // Open download monitor.
            let ret_on = updater.on('upgradeProgress', progress => {
                console.log("upgradeProgress on" + progress);
                console.log(`upgradeProgress status: ` + progress.status);
                console.log(`upgradeProgress percent: ` + progress.percent);
                console.log(`upgradeProgress endReason: ` + progress.endReason);
                if (progress.status === updateState.UPDATE_STATE_DOWNLOAD_SUCCESS ||
                    progress.status === updateState.UPDATE_STATE_VERIFY_SUCCESS) {
                    expect(progress.percent).assertEqual(expect_value);
                }
            })
            expect(ret_on).assertInstanceOf('Number');
            let ret = updater.upgrade();
            expect(ret).assertInstanceOf('Number');
            // Cancel subscription immediately, no callback will be received.
            let ret_off = updater.off("upgradeProgress");
            expect(ret_off).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testUpgradeVersion02] catch ' + e);
        }
        console.info('[upgrade] success.');
        console.info('testUpgradeVersion02 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0053
     * @tc.name      testUpgradeVersion03
     * @tc.desc      Test download parameter, callback is not supported.
     */
    it('testUpgradeVersion03', 0, function() {
        console.info('testUpgradeVersion03 START');
        try{
            let ret = updater.upgrade(progress => {
                console.log("upgradeProgress on" + progress);
                console.log(`upgradeProgress status: ` + progress.status);
                console.log(`upgradeProgress percent: ` + progress.percent);
                console.log(`upgradeProgress endReason: ` + progress.endReason);
                if (progress.status === updateState.UPDATE_STATE_DOWNLOAD_SUCCESS ||
                    progress.status === updateState.UPDATE_STATE_VERIFY_SUCCESS) {
                    expect(progress.percent).assertEqual(expect_value);
                }
            })
            expect(ret).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testUpgradeVersion03] catch ' + e);
        }
        console.info('[upgrade] success.');
        console.info('testUpgradeVersion03 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0054
     * @tc.name      testUpgradeVersion04
     * @tc.desc      Test subscription without parameters, return error.
     */
    it('testUpgradeVersion04', 0, function() {
        console.info('testUpgradeVersion04 START');
        try{
            // Open download monitor.
            let ret = updater.on('upgradeProgress');
            expect(ret).assertInstanceOf('Number');
            let ret_upgrade = updater.upgrade();
            expect(ret_upgrade).assertInstanceOf('Number');
            // Cancel subscription immediately, no callback will be received.
            let ret_off = updater.off("upgradeProgress");
            expect(ret_off).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testUpgradeVersion04] catch ' + e);
        }
        console.info('[upgrade] success.');
        console.info('testUpgradeVersion04 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0055
     * @tc.name      testUpgradeVersion05
     * @tc.desc      Take function to subscribe.
     */
    it('testUpgradeVersion05', 0, function() {
        function testFunction(progress) {
            console.log("upgradeProgress on" + progress);
            console.log(`upgradeProgress status: ` + progress.status);
            console.log(`upgradeProgress percent: ` + progress.percent);
            console.log(`upgradeProgress endReason: ` + progress.endReason);
        }
        console.info('testUpgradeVersion05 START');
        try{
            let ret = updater.on('upgradeProgress', testFunction);
            expect(ret).assertInstanceOf('Number');

            let ret_upgrade = updater.upgrade();
            expect(ret_upgrade).assertInstanceOf('Number');

            let ret_off = updater.off("upgradeProgress", testFunction);
            expect(ret_off).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testUpgradeVersion05] catch ' + e);
        }
        console.info('[upgrade] success.');
        console.info('testUpgradeVersion05 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0056
     * @tc.name      testOnDownload2
     * @tc.desc      Test incorrect parameters.
     */
     it('testOnDownload', 0, function() {
        console.info('testOnDownload START');
        try{
            let ret = updater.on(function(){});
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOnDownload] catch ' + e);
        }
        console.info('[testOnDownload] success.');
        console.info('testOnDownload END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0057
     * @tc.name      testOnDownload2
     * @tc.desc      Test incorrect parameters.
     */
    it('testOnDownload2', 0, function() {
        console.info('testOnDownload2 START');
        try{
            let ret = updater.on();
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOnDownload2] catch ' + e);
        }
        console.info('[testOnDownload2] success.');
        console.info('testOnDownload2 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0058
     * @tc.name      testOnDownload3
     * @tc.desc      Test incorrect parameters.
     */
    it('testOnDownload3', 0, function() {
        console.info('testOnDownload3 START');
        try{
            let ret = updater.on("333333333");
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOnDownload3] catch ' + e);
        }
        console.info('[testOnDownload3] success.');
        console.info('testOnDownload3 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0059
     * @tc.name      testOnDownload3
     * @tc.desc      Test incorrect parameters.
     */
    it('testOnDownload4', 0, function() {
        console.info('testOnDownload3 START');
        try{
            let ret = updater.on(param_value2);
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOnDownload4] catch ' + e);
        }
        console.info('[testOnDownload4] success.');
        console.info('testOnDownload4 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0060
     * @tc.name      testOnDownload4
     * @tc.desc      Test incorrect parameters.
     */
    it('testOnDownload5', 0, function() {
        console.info('testOnDownload5 START');
        try{
            let ret = updater.on(param_value2, "4444444444444");
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOnDownload5] catch ' + e);
        }
        console.info('[testOnDownload5] success.');
        console.info('testOnDownload5 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0061
     * @tc.name      testOffDownload
     * @tc.desc      Test incorrect parameters.
     */
    it('testOffDownload', 0, function() {
        console.info('testOffDownload START');
        try{
            let ret = updater.off();
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOffDownload] catch ' + e);
        }
        console.info('[testOffDownload] success.');
        console.info('testOffDownload END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0062
     * @tc.name      testOffDownload2
     * @tc.desc      Test incorrect parameters.
     */
    it('testOffDownload2', 0, function() {
        console.info('testOffDownload2 START');
        try{
            let ret = updater.off("333333333");
            expect(ret).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testOffDownload2] catch ' + e);
        }
        console.info('[testOffDownload2] success.');
        console.info('testOffDownload2 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0063
     * @tc.name      testOffDownload3
     * @tc.desc      Test incorrect parameters.
     */
    it('testOffDownload3', 0, function() {
        console.info('testOffDownload3 START');
        try{
            let ret = updater.on(param_value2);
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOffDownload3] catch ' + e);
        }
        console.info('[testOffDownload3] success.');
        console.info('testOffDownload3 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0064
     * @tc.name      testOffDownload4
     * @tc.desc      Test incorrect parameters.
     */
    it('testOffDownload4', 0, function() {
        console.info('testOffDownload4 START');
        try{
            let ret = updater.off(param_value2, "4444444444444");
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOffDownload4] catch ' + e);
        }
        console.info('[testOffDownload4] success.');
        console.info('testOffDownload4 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0065
     * @tc.name      testOffDownload5
     * @tc.desc      Test incorrect parameters.
     */
     it('testOffDownload5', 0, function() {
        console.info('testOffDownload5 START');
        try{
            let ret = updater.off(function(){});
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOffDownload5] catch ' + e);
        }
        console.info('[testOffDownload5] success.');
        console.info('testOffDownload5 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0066
     * @tc.name      testOnUpgrade
     * @tc.desc      Test incorrect parameters.
     */
    it('testOnUpgrade', 0, function() {
        console.info('testOnUpgrade START');
        try{
            let ret = updater.on("upgradeProgress", function(){});
            expect(ret).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testOnUpgrade] catch ' + e);
        }
        console.info('[testOnUpgrade] success.');
        console.info('testOnUpgrade END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0067
     * @tc.name      testOnUpgrade2
     * @tc.desc      Test incorrect parameters.
     */
    it('testOnUpgrade2', 0, function() {
        console.info('testOnUpgrade2 START');
        try{
            let ret = updater.on("upgradeProgress");
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOnUpgrade2] catch ' + e);
        }
        console.info('[testOnUpgrade2] success.');
        console.info('testOnUpgrade2 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0068
     * @tc.name      testOnUpgrade3
     * @tc.desc      Test incorrect parameters.
     */
    it('testOnUpgrade3', 0, function() {
        console.info('testOnUpgrade3 START');
        try{
            let ret = updater.on("upgradeProgress", "333333333");
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOnUpgrade3] catch ' + e);
        }
        console.info('[testOnUpgrade3] success.');
        console.info('testOnUpgrade3 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0069
     * @tc.name      testOnDownload3
     * @tc.desc      Test incorrect parameters.
     */
    it('testOnUpgrade4', 0, function() {
        console.info('testOnUpgrade4 START');
        try{
            let ret = updater.on("upgradeProgress", param_value2);
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOnUpgrade3] catch ' + e);
        }
        console.info('[testOnUpgrade4] success.');
        console.info('testOnUpgrade4 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0070
     * @tc.name      testOnUpgrade5
     * @tc.desc      Test incorrect parameters.
     */
    it('testOnUpgrade5', 0, function() {
        console.info('testOnUpgrade5 START');
        try{
            let ret = updater.on(param_value2, "4444444444444");
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOnUpgrade5] catch ' + e);
        }
        console.info('[testOnUpgrade5] success.');
        console.info('testOnUpgrade5 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0071
     * @tc.name      testOffUpgrade
     * @tc.desc      Test incorrect parameters.
     */
    it('testOffUpgrade', 0, function() {
        console.info('testOffUpgrade START');
        try{
            let ret = updater.off("upgradeProgress");
            expect(ret).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testOffUpgrade] catch ' + e);
        }
        console.info('[testOffUpgrade] success.');
        console.info('testOffUpgrade END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0072
     * @tc.name      testOffUpgrade2
     * @tc.desc      Test incorrect parameters.
     */
    it('testOffUpgrade2', 0, function() {
        console.info('testOffUpgrade2 START');
        try{
            let ret = updater.off("upgradeProgress", "333333333");
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOffUpgrade] catch ' + e);
        }
        console.info('[testOffUpgrade2] success.');
        console.info('testOffUpgrade2 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0073
     * @tc.name      testOffUpgrade3
     * @tc.desc      Test incorrect parameters.
     */
    it('testOffUpgrade3', 0, function() {
        console.info('testOffDownload3 START');
        try{
            let ret = updater.on("upgradeProgress", param_value2);
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOffUpgrade3] catch ' + e);
        }
        console.info('[testOffUpgrade3] success.');
        console.info('testOffUpgrade3 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0074
     * @tc.name      testOffUpgrade4
     * @tc.desc      Test incorrect parameters.
     */
    it('testOffUpgrade4', 0, function() {
        console.info('testOffUpgrade4 START');
        try{
            let ret = updater.off("upgradeProgress", param_value2, "4444444444444");
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOffUpgrade4] catch ' + e);
        }
        console.info('[testOffUpgrade4] success.');
        console.info('testOffUpgrade4 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0075
     * @tc.name      testOffUpgrade5
     * @tc.desc      Test incorrect parameters.
     */
    it('testOffUpgrade5', 0, function() {
        console.info('testOffUpgrade5 START');
        try{
            let ret = updater.off("upgradeProgress", "6");
            expect(ret != 0).assertTrue();
        } catch (e) {
            console.info('[testOffUpgrade5] catch ' + e);
        }
        console.info('[testOffUpgrade5] success.');
        console.info('testOffUpgrade5 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0076
     * @tc.name      testCancelDownload
     * @tc.desc      Test Cancel download.
     */
    it('testCancelDownload', 0, function() {
        console.info('testCancelDownload START');
        try{
            let ret = updater.cancel();
            console.info('updater.cancelUpgrade' + ret);
            expect(ret === 0).assertTrue();
        } catch (e) {
            console.info('[cancel] catch ' + e);
        }
        console.info('[cancel] success.');
        console.info('testCancelDownload END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0077
     * @tc.name      testCancelDownload2
     * @tc.desc      Test Cancel download.
     */
    it('testCancelDownload2', 0, function() {
        console.info('testCancelDownload2 START');
        try{
            // Open download monitor.
            updater.on('downloadProgress', progress => {
                console.log("downloadProgress on" + progress);
                console.log(`downloadProgress status: ` + progress.status);
                console.log(`downloadProgress percent: ` + progress.percent);
                console.log(`downloadProgress endReason: ` + progress.endReason);
                if (progress.status === updateState.UPDATE_STATE_DOWNLOAD_SUCCESS ||
                progress.status === updateState.UPDATE_STATE_VERIFY_SUCCESS) {
                    expect(progress.percent).assertEqual(expect_value);
                    updater.off("downloadProgress");
                }
            })
            let ret = updater.download();
            expect(ret === 0).assertTrue();

            ret = updater.cancel();
            console.info('updater.cancel' + ret);
            expect(ret === 0).assertTrue();
        } catch (e) {
            console.info('[cancel] catch ' + e);
        }
        console.info('[cancel] success.');
        console.info('testCancelDownload2 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0078
     * @tc.name      testCancelDownload3
     * @tc.desc      Test Cancel download.
     */
    it('testCancelDownload3', 0, function() {
        console.info('testCancelDownload3 START');
        try{
            let ret = updater.cancel();
            console.info('updater.cancel' + ret);
            expect(ret === 0).assertTrue();
        } catch (e) {
            console.info('[cancel] catch ' + e);
        }
        console.info('[cancel] success.');
        console.info('testCancelDownload3 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0079
     * @tc.name      testCancelDownload4
     * @tc.desc      Test Cancel download.
     */
    it('testCancelDownload4', 0, function() {
        console.info('testCancelDownload4 START');
        try{
            let ret = updater.cancel("");
            console.info('updater.cancel' + ret);
            expect(ret === undefined).assertTrue();
        } catch (e) {
            console.info('[cancel] catch ' + e);
        }
        console.info('[cancel] success.');
        console.info('testCancelDownload4 END');
    });
    /**
     * @tc.number    SUB_UPDATE_JS_API_0080
     * @tc.name      testCancelDownload5
     * @tc.desc      Test Cancel download.
     */
    it('testCancelDownload5', 0, function() {
        console.info('testCancelDownload5 START');
        try{
            let ret = updater.cancel(function(){});
            console.info('updater.cancel' + ret);
            expect(ret === undefined).assertTrue();
        } catch (e) {
            console.info('[cancel] catch ' + e);
        }
        console.info('[cancelUpgrade] success.');
        console.info('testCancelDownload5 END');
    });
	    /**
     * @tc.number    SUB_UPDATE_JS_API_0081
     * @tc.name      testVerifyVersion
     * @tc.desc      Unsubscribe from monitoring.
     */
    it('testVerifyVersion', 0, function() {
        console.info('testVerifyVersion START');
        try{
            updater.on('verifyProgress', progress => {
                console.log("verifyProgress on" + progress);
                console.log(`verifyProgress status: ` + progress.status);
                console.log(`verifyProgress percent: ` + progress.percent);
                console.log(`verifyProgress endReason: ` + progress.endReason);
                if (progress.status === updateState.UPDATE_STATE_DOWNLOAD_SUCCESS ||
                progress.status === updateState.UPDATE_STATE_VERIFY_SUCCESS) {
                    expect(progress.percent).assertEqual(expect_value);
                    updater.off("verifyProgress");
                }
            })
            let ret = updater.verifyUpdatePackage('/data/updater/updater.zip','/data/updater/HwOUC/update_auth.sa');
            expect(ret).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testVerifyVersion] catch ' + e);
        }
        console.info('[verifyVersion] success.');
        console.info('testVerifyVersion END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0082
     * @tc.name      testVerifyVersion02
     * @tc.desc      Unsubscribe out of monitoring.
     */
    it('testVerifyVersion02', 0, function() {
        console.info('testVerifyVersion02 START');
        try{
            updater.on('verifyProgress', progress => {
                console.log("verifyProgress on" + progress);
                console.log(`verifyProgress status: ` + progress.status);
                console.log(`verifyProgress percent: ` + progress.percent);
                console.log(`verifyProgress endReason: ` + progress.endReason);
                if (progress.status === updateState.UPDATE_STATE_DOWNLOAD_SUCCESS ||
                progress.status === updateState.UPDATE_STATE_VERIFY_SUCCESS) {
                    expect(progress.percent).assertEqual(expect_value);
                }
            })
            let ret = updater.verifyUpdatePackage('/data/updater/updater.zip','/data/updater/HwOUC/update_auth.sa');
            updater.off("verifyProgress");
            expect(ret).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testVerifyVersion02] catch ' + e);
        }
        console.info('[verifyVersion] success.');
        console.info('testVerifyVersion END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0083
     * @tc.name      testVerifyVersion03
     * @tc.desc      Test verify parameter, callback is not supported.
     */
    it('testVerifyVersion03', 0, function() {
        console.info('testVerifyVersion03 START');
        try{
            let ret = updater.verifyUpdatePackage('/data/updater/updater.zip','/data/updater/HwOUC/update_auth.sa',progress => {
                console.log("verifyProgress on" + progress);
                console.log(`verifyProgress status: ` + progress.status);
                console.log(`verifyProgress percent: ` + progress.percent);
                console.log(`verifyProgress endReason: ` + progress.endReason);
                if (progress.status === updateState.UPDATE_STATE_DOWNLOAD_SUCCESS ||
                progress.status === updateState.UPDATE_STATE_VERIFY_SUCCESS) {
                    expect(progress.percent).assertEqual(expect_value);
                }
            })
            expect(ret).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testVerifyVersion03] catch ' + e);
        }
        console.info('[verifyVersion] success.');
        console.info('testVerifyVersion END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0084
     * @tc.name      testVerifyVersion04
     * @tc.desc      Test subscription without parameters, return error.
     */
    it('testVerifyVersion04', 0, function() {
        console.info('testVerifyVersion04 START');
        try{
            let ret = updater.on('verifyProgress');
            expect(ret).assertInstanceOf('Number');
            let ret_verify = updater.verifyUpdatePackage();
            expect(ret_verify).assertInstanceOf('Number');
            let ret_off = updater.off("verifyProgress");
            expect(ret_off).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testVerifyVersion04] catch ' + e);
        }
        console.info('[verifyVersion] success.');
        console.info('testVerifyVersion04 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0085
     * @tc.name      testVerifyVersion05
     * @tc.desc      Take function to subscribe.
     */
    it('testVerifyVersion05', 0, function() {
        function testFunction(progress) {
            console.log("verifyProgress on" + progress);
            console.log(`verifyProgress status: ` + progress.status);
            console.log(`verifyProgress percent: ` + progress.percent);
            console.log(`verifyProgress endReason: ` + progress.endReason);
        }
        console.info('testVerifyVersion05 START');
        try{
            let ret = updater.on('verifyProgress', testFunction);
            expect(ret).assertInstanceOf('Number');
            let ret_verify = updater.verifyUpdatePackage('/data/updater/updater.zip','/data/updater/HwOUC/update_auth.sa');
            expect(ret_verify).assertInstanceOf('Number');
            let ret_off = updater.off("verifyProgress", testFunction);
            expect(ret_off).assertInstanceOf('Number');
        } catch (e) {
            console.info('[testVerifyVersion05] catch ' + e);
        }
        console.info('[verifyVersion] success.');
        console.info('testVerifyVersion05 END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0086
     * @tc.name      testPackageTypes
     * @tc.desc      check packageTypes is defined correct.
     */
     it('testPackageTypes', 0, function() {
        console.info('testPackageTypes START');
        try {
            expect(updater.PackageTypes.PACKAGE_TYPE_NORMAL).assertEqual(1); 
            expect(updater.PackageTypes.PACKAGE_TYPE_BASE).assertEqual(2); 
            expect(updater.PackageTypes.PACKAGE_TYPE_CUST).assertEqual(3); 
            expect(updater.PackageTypes.PACKAGE_TYPE_PRELOAD).assertEqual(4); 
            expect(updater.PackageTypes.PACKAGE_TYPE_COTA).assertEqual(5); 
            expect(updater.PackageTypes.PACKAGE_TYPE_VERSION).assertEqual(6); 
            expect(updater.PackageTypes.PACKAGE_TYPE_PATCH).assertEqual(7); 
        } catch(e) {
            console.error('[testPackageTypes] fail.' + e);
        }
        console.info('[PackageTypes] success.');
        console.info('testPackageTypes END');
    });
	
	/**
     * @tc.number    SUB_UPDATE_JS_API_0087
     * @tc.name      testNewVersionStatus
     * @tc.desc      check NewVersionStatus is defined correct.
     */
    it('testNewVersionStatus', 0, function() {
        console.info('testNewVersionStatus START');
        try {
            expect(updater.NewVersionStatus.VERSION_STATUS_ERR).assertEqual(-1); 
            expect(updater.NewVersionStatus.VERSION_STATUS_NEW).assertEqual(0); 
            expect(updater.NewVersionStatus.VERSION_STATUS_NONE).assertEqual(1); 
            expect(updater.NewVersionStatus.VERSION_STATUS_BUSY).assertEqual(2); 
        } catch(e) {
            console.error('[testNewVersionStatus] fail.' + e);
        }
        console.info('[NewVersionStatus] success.');
        console.info('testNewVersionStatus END');
    });

	/**
     * @tc.number    SUB_UPDATE_JS_API_0088
     * @tc.name      testInstallMode
     * @tc.desc      check InstallMode is defined correct.
     */
    it('testInstallMode', 0, function() {
        console.info('testInstallMode START');
        try {
            expect(updater.InstallMode.INSTALL_MODE_NORMAL).assertEqual(0); 
            expect(updater.InstallMode.INSTALL_MODE_NIGHT).assertEqual(1);
            expect(updater.InstallMode.INSTALL_MODE_AUTO).assertEqual(2);			
        } catch(e) {
            console.error('[testInstallMode] fail.' + e);
        }
        console.info('[InstallMode] success.');
        console.info('testInstallMode END');
    });
	
	/**
     * @tc.number    SUB_UPDATE_JS_API_0089
     * @tc.name      testCheckResultAndDescriptionInfo
     * @tc.desc      check checkResult and descriptionInfo is defined correct.
     */
    it('testInstallMode', 0, function() {
        console.info('testCheckResultAndDescriptionInfo START');
        try {
            let descriptionInfo = updater.DescriptionInfo.content;
			console.info('test DescriptionInfo.content is ' + descriptionInfo);
            expect(updater.CheckResult.packageType.PACKAGE_TYPE_NORMAL).assertEqual(1);			
        } catch(e) {
            console.error('[testCheckResultAndDescriptionInfo] fail.' + e);
        }
        console.info('[CheckResultAndDescriptionInfo] success.');
        console.info('testCheckResultAndDescriptionInfo END');
    });

    /**
     * @tc.number    SUB_UPDATE_JS_API_0090
     * @tc.name      testGetUpdaterForOther
     * @tc.desc      Test getUpdaterForOther obj.
     */
    it('testGetUpdaterForOther', 0, function() {
        console.info('testGetUpdaterForOther START');
        try {
            if (client === undefined) {
                console.error("client not defined");
                return;
            }
            let tmpGetUpdaterForOther = client.getUpdaterForOther('/data/updater/updater.zip', '1234567890', 'OTA');
            expect(tmpGetUpdaterForOther != undefined).assertTrue();
        } catch(e) {
            console.error('[testGetUpdaterForOther] fail.' + e);
        }

        console.info('[client.getUpdaterForOther] success.');
        console.info('testGetUpdaterForOther END');
    });
	
    /**
     * @tc.number    SUB_UPDATE_JS_API_0091
     * @tc.name      testGetUpdaterFromOther
     * @tc.desc      Test getUpdaterFromOther obj.
     */
    it('testGetUpdaterFromOther', 0, function() {
        console.info('testGetUpdaterFromOther START');
        try {
            if (client === undefined) {
                console.error("client not defined");
                return;
            }
            let tmpGetUpdaterFromOther = client.getUpdaterFromOther('/data/updater/updater.zip', '1234567890', 'OTA');
            expect(tmpGetUpdaterFromOther != undefined).assertTrue();
        } catch(e) {
            console.error('[testGetUpdaterFromOther] fail.' + e);
        }

        console.info('[client.getUpdaterFromOther] success.');
        console.info('testGetUpdaterFromOther END');
    });
});
