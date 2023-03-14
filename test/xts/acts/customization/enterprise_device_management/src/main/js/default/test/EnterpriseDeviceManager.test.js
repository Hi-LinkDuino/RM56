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
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'
import bundle from '@ohos.bundle'
import enterpriseDeviceManager from '@ohos.enterpriseDeviceManager'

const PATH = "/data/"
const EDMJSTEST1 = "edmJstest1.hap"
const WANT1 = {
    bundleName: "com.example.myapplication1",
    abilityName: "com.example.myapplication1.MainAbility"
};

const SELFWANT = {
    bundleName: "ohos.edm.test",
    abilityName: "ohos.edm.test.MainAbility"
};
const HAPNAME1 = "com.example.myapplication1"
const SELFHAPNAME = "ohos.edm.test"
const COMPANYNAME2 = "company2"
const DESCRIPTION2 = "edm demo2"
const ENTINFO1 = {
    name: "company",
    description: "edm demo"
};

const ENTINFO2 = {
    name: "company2",
    description: "edm demo2"
};

describe('EnterpriseDeviceManagerTest', function () {
    console.log('*************start EnterpriseDeviceManagerTest*************');

    /**
     * @tc.number SUB_CUSTOMIZATION_ENTERPRISE_DEVICE_MANAGER_JS_0100
     * @tc.name test activateAdmin method in promise mode
     * @tc.desc activate admin in promise mode
     */
    it('activateAdmin_test_001', 0, async function (done) {
        let installData = await bundle.getBundleInstaller()
        installData.install([PATH + EDMJSTEST1], {
            userId: 100,
            installFlag: 0,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            await bundle.getBundleInfo(HAPNAME1, 
                bundle.BundleFlag.GET_BUNDLE_WITH_REQUESTED_PERMISSION).then(datainfo => {
                    console.info("getBundleInfo success:" + JSON.stringify(datainfo));
            });
            activateAdmin();
        });
        async function activateAdmin() {
            var retValue = await enterpriseDeviceManager.activateAdmin(WANT1, ENTINFO1,
                enterpriseDeviceManager.AdminType.ADMIN_TYPE_NORMAL);
            console.log('enterpriseDeviceManager.activateAdmin ADMIN_TYPE_NORMAL : ' + retValue);
            expect(retValue).assertTrue();

            var isActive = await enterpriseDeviceManager.isAdminAppActive(WANT1);
            console.log('enterpriseDeviceManager.isAdminAppActive : ' + isActive);
            expect(isActive).assertTrue();

            retValue = await enterpriseDeviceManager.deactivateAdmin(WANT1);
            console.log('enterpriseDeviceManager.deactivateAdmin : ' + retValue);
            expect(retValue).assertTrue();

            isActive = await enterpriseDeviceManager.isAdminAppActive(WANT1);
            console.log('enterpriseDeviceManager.isAdminAppActive : ' + isActive);
            expect(isActive).assertFalse();

            installData.uninstall(HAPNAME1, {
                userId: 100,
                installFlag: 0,
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
     * @tc.number SUB_CUSTOMIZATION_ENTERPRISE_DEVICE_MANAGER_JS_0200
     * @tc.name test activateAdmin method in callback mode
     * @tc.desc activate admin in callback mode
     */
    it('activateAdmin_test_002', 0, async function (done) {
        let installData = await bundle.getBundleInstaller()
        installData.install([PATH + EDMJSTEST1], {
            userId: 100,
            installFlag: 0,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            activateAdmin();
        });
        async function activateAdmin() {
            console.log('activateAdmin()');
            var retValue = await enterpriseDeviceManager.activateAdmin(WANT1, ENTINFO1,
                enterpriseDeviceManager.AdminType.ADMIN_TYPE_NORMAL, OnReceiveEvent);
            async function OnReceiveEvent(err, datainfo) {
                console.log('enterpriseDeviceManager.activateAdmin ADMIN_TYPE_NORMAL :' + datainfo);
                expect(datainfo).assertTrue();

                var isActive = await enterpriseDeviceManager.isAdminAppActive(WANT1);
                console.log('enterpriseDeviceManager.isAdminAppActive :' + isActive);
                expect(isActive).assertTrue();

                retValue = await enterpriseDeviceManager.deactivateAdmin(WANT1);
                console.log('enterpriseDeviceManager.deactivateAdmin : ' + retValue);
                expect(retValue).assertTrue();

                isActive = await enterpriseDeviceManager.isAdminAppActive(WANT1);
                console.log('enterpriseDeviceManager.isAdminAppActive : ' + isActive);
                expect(isActive).assertFalse();

                installData.uninstall(HAPNAME1, {
                    userId: 100,
                    installFlag: 0,
                    isKeepData: false
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            }
        }
    })


    /**
     * @tc.number SUB_CUSTOMIZATION_ENTERPRISE_DEVICE_MANAGER_JS_0300
     * @tc.name test activateAdmin method in promise mode with ADMIN_TYPE_SUPER param
     * @tc.desc activate super admin in promise mode
     */
    it('activateAdmin_test_003', 0, async function (done) {
        await bundle.getBundleInfo(SELFHAPNAME, 
            bundle.BundleFlag.GET_BUNDLE_WITH_REQUESTED_PERMISSION).then(datainfo => {
                console.info("getBundleInfo success:" + JSON.stringify(datainfo));
        });

        console.log('enterpriseDeviceManager.activateAdmin ADMIN_TYPE_SUPER');
        var retValue = await enterpriseDeviceManager.activateAdmin(SELFWANT, ENTINFO1,
            enterpriseDeviceManager.AdminType.ADMIN_TYPE_SUPER);
        console.log('enterpriseDeviceManager.activateAdmin ADMIN_TYPE_SUPER : ' + retValue);
        expect(retValue).assertTrue();

        var isActive = await enterpriseDeviceManager.isSuperAdmin(SELFHAPNAME);
        console.log('enterpriseDeviceManager.isSuperAdmin :' + isActive);
        expect(isActive).assertTrue();

        retValue = await enterpriseDeviceManager.deactivateSuperAdmin(SELFHAPNAME);
        console.log('enterpriseDeviceManager.deactivateSuperAdmin : ' + retValue);
        expect(retValue).assertTrue();

        isActive = await enterpriseDeviceManager.isSuperAdmin(SELFHAPNAME);
        console.log('enterpriseDeviceManager.isSuperAdmin : ' + isActive);
        expect(isActive).assertFalse();
        done();
    })

    /**
     * @tc.number SUB_CUSTOMIZATION_ENTERPRISE_DEVICE_MANAGER_JS_0400
     * @tc.name test activateAdmin method in callback mode with ADMIN_TYPE_SUPER param
     * @tc.desc activate super admin in callback mode
     */
    it('activateAdmin_test_004', 0, async function (done) {
        var retValue = await enterpriseDeviceManager.activateAdmin(SELFWANT, ENTINFO1,
            enterpriseDeviceManager.AdminType.ADMIN_TYPE_SUPER, OnReceiveEvent);
        async function OnReceiveEvent(err, datainfo) {
            console.log('enterpriseDeviceManager.activateAdmin : ' + retValue);
            expect(datainfo).assertTrue();

            var isActive = await enterpriseDeviceManager.isSuperAdmin(SELFHAPNAME);
            console.log('enterpriseDeviceManager.isSuperAdmin :' + isActive);
            expect(isActive).assertTrue();

            retValue = await enterpriseDeviceManager.deactivateSuperAdmin(SELFHAPNAME);
            console.log('enterpriseDeviceManager.deactivateSuperAdmin : ' + retValue);
            expect(retValue).assertTrue();

            isActive = await enterpriseDeviceManager.isSuperAdmin(SELFHAPNAME);
            console.log('enterpriseDeviceManager.isSuperAdmin : ' + isActive);
            expect(isActive).assertFalse();
            done();
        }
    })

    /**
     * @tc.number SUB_CUSTOMIZATION_ENTERPRISE_DEVICE_MANAGER_JS_0500
     * @tc.name test activateAdmin method in promise mode
     * @tc.desc activate admin in promise mode
     */
    it('activateAdmin_test_005', 0, async function (done) {

        var retValue = await enterpriseDeviceManager.activateAdmin(SELFWANT, ENTINFO1,
            enterpriseDeviceManager.AdminType.ADMIN_TYPE_NORMAL);
        console.log('enterpriseDeviceManager.activateAdmin ADMIN_TYPE_NORMAL : ' + retValue);
        expect(retValue).assertTrue();

        var isActive = await enterpriseDeviceManager.isAdminAppActive(SELFWANT);
        console.log('enterpriseDeviceManager.isAdminAppActive : ' + isActive);
        expect(isActive).assertTrue();

        retValue = await enterpriseDeviceManager.deactivateAdmin(SELFWANT);
        console.log('enterpriseDeviceManager.deactivateAdmin : ' + retValue);
        expect(retValue).assertTrue();

        isActive = await enterpriseDeviceManager.isAdminAppActive(SELFWANT);
        console.log('enterpriseDeviceManager.isAdminAppActive : ' + isActive);
        expect(isActive).assertFalse();

        done();
    })

    /**
     * @tc.number SUB_CUSTOMIZATION_ENTERPRISE_DEVICE_MANAGER_JS_0600
     * @tc.name test activateAdmin method in callback mode
     * @tc.desc activate admin in callback mode
     */
    it('activateAdmin_test_006', 0, async function (done) {

        console.log(' activateAdmin()');
        var retValue = await enterpriseDeviceManager.activateAdmin(SELFWANT, ENTINFO1,
            enterpriseDeviceManager.AdminType.ADMIN_TYPE_NORMAL, OnReceiveEvent);
        async function OnReceiveEvent(err, datainfo) {
            console.log('enterpriseDeviceManager.activateAdmin ADMIN_TYPE_NORMAL :' + datainfo);
            expect(datainfo).assertTrue();

            var isActive = await enterpriseDeviceManager.isAdminAppActive(SELFWANT);
            console.log('enterpriseDeviceManager.isAdminAppActive :' + isActive);
            expect(isActive).assertTrue();

            retValue = await enterpriseDeviceManager.deactivateAdmin(SELFWANT);
            console.log('enterpriseDeviceManager.deactivateAdmin : ' + retValue);
            expect(retValue).assertTrue();

            isActive = await enterpriseDeviceManager.isAdminAppActive(SELFWANT);
            console.log('enterpriseDeviceManager.isAdminAppActive : ' + isActive);
            expect(isActive).assertFalse();
            done();
        }
    })


    /**
     * @tc.number SUB_CUSTOMIZATION_ENTERPRISE_DEVICE_MANAGER_JS_0700
     * @tc.name test setEnterpriseInfo method in promise mode
     * @tc.desc set enterprise info in promise mode
     */
    it('setEnterpriseInfo_test_001', 0, async function (done) {
        var retValue = await enterpriseDeviceManager.activateAdmin(SELFWANT, ENTINFO1,
            enterpriseDeviceManager.AdminType.ADMIN_TYPE_NORMAL);
        console.log('enterpriseDeviceManager.activateAdmin ADMIN_TYPE_NORMAL : ' + retValue);
        expect(retValue).assertTrue();

        var isActive = await enterpriseDeviceManager.isAdminAppActive(SELFWANT);
        expect(isActive).assertTrue();
        console.log('enterpriseDeviceManager.isAdminAppActive : ' + isActive);

        retValue = await enterpriseDeviceManager.setEnterpriseInfo(SELFWANT, ENTINFO2);
        console.log('enterpriseDeviceManager.setEnterpriseInfo : ' + retValue);
        expect(retValue).assertTrue();

        var entInfo = await enterpriseDeviceManager.getEnterpriseInfo(SELFWANT);
        expect(entInfo.name).assertEqual(COMPANYNAME2);
        expect(entInfo.description).assertEqual(DESCRIPTION2);

        retValue = await enterpriseDeviceManager.deactivateAdmin(SELFWANT);
        console.log('enterpriseDeviceManager.deactivateAdmin : ' + retValue);
        expect(retValue).assertTrue();

        isActive = await enterpriseDeviceManager.isAdminAppActive(SELFWANT);
        console.log('enterpriseDeviceManager.isAdminAppActive : ' + isActive);
        expect(isActive).assertFalse();
        done();

    })

    /**
     * @tc.number SUB_CUSTOMIZATION_ENTERPRISE_DEVICE_MANAGER_JS_0800
     * @tc.name test setEnterpriseInfo method in callback mode
     * @tc.desc set enterprise info in callback mode
     */
    it('setEnterpriseInfo_test_002', 0, async function (done) {
        var retValue = await enterpriseDeviceManager.activateAdmin(SELFWANT, ENTINFO1,
            enterpriseDeviceManager.AdminType.ADMIN_TYPE_NORMAL);
        console.log('enterpriseDeviceManager.activateAdmin ADMIN_TYPE_NORMAL : ' + retValue);
        expect(retValue).assertTrue();
        var isActive = await enterpriseDeviceManager.isAdminAppActive(SELFWANT);
        expect(isActive).assertTrue();
        console.log('enterpriseDeviceManager.isAdminAppActive : ' + isActive);

        retValue = await enterpriseDeviceManager.setEnterpriseInfo(SELFWANT, ENTINFO2, OnReceiveEvent);
        async function OnReceiveEvent(err, datainfo) {
            console.log('enterpriseDeviceManager.setEnterpriseInfo : ' + datainfo);
            expect(datainfo).assertTrue();

            var entInfo = await enterpriseDeviceManager.getEnterpriseInfo(SELFWANT);
            expect(entInfo.name).assertEqual(COMPANYNAME2);
            expect(entInfo.description).assertEqual(DESCRIPTION2);

            retValue = await enterpriseDeviceManager.deactivateAdmin(SELFWANT);
            console.log('enterpriseDeviceManager.deactivateAdmin : ' + retValue);
            expect(retValue).assertTrue();

            isActive = await enterpriseDeviceManager.isAdminAppActive(SELFWANT);
            console.log('enterpriseDeviceManager.isAdminAppActive : ' + isActive);
            expect(isActive).assertFalse();
            done();
        }
    })

    /**
     * @tc.number SUB_CUSTOMIZATION_ENTERPRISE_DEVICE_MANAGER_JS_0900
     * @tc.name test getDeviceSettingsManager method in callback mode
     * @tc.desc get the device settings manager in callback mode
     */
    it('getDeviceSettingsManager_test_001', 0, async function (done) {
        enterpriseDeviceManager.getDeviceSettingsManager((error, mgr) => {
            expect(mgr !== null).assertTrue();
        })
        console.log('getDeviceSettingsManager_test_001 done');
        done();
    })

    /**
     * @tc.number SUB_CUSTOMIZATION_ENTERPRISE_DEVICE_MANAGER_JS_1000
     * @tc.name test getDeviceSettingsManager method in promise mode
     * @tc.desc get the device settings manager in promise mode
     */
    it('getDeviceSettingsManager_test_002', 0, async function (done) {
        enterpriseDeviceManager.getDeviceSettingsManager().then(mgr => {
            expect(mgr !== null).assertTrue();
        })
        console.log('getDeviceSettingsManager_test_002 done');
        done();
    })

    /**
     * @tc.number SUB_CUSTOMIZATION_ENTERPRISE_DEVICE_MANAGER_JS_1100
     * @tc.name test setDateTime method in promise mode
     * @tc.desc set system date time in promise mode
     */
    it('setDateTime_test_001', 0, async function (done) {
        await enterpriseDeviceManager.activateAdmin(SELFWANT, ENTINFO1,
            enterpriseDeviceManager.AdminType.ADMIN_TYPE_SUPER);
        let dsmgr = await enterpriseDeviceManager.getDeviceSettingsManager();
        expect(dsmgr !== null).assertTrue();
        console.log('before setDateTime');
        var setSuccess = await dsmgr.setDateTime(SELFWANT, 1526003846000);
        expect(setSuccess).assertTrue();  
        await enterpriseDeviceManager.deactivateSuperAdmin(SELFHAPNAME);
        done();
    })

    /**
     * @tc.number SUB_CUSTOMIZATION_ENTERPRISE_DEVICE_MANAGER_JS_1200
     * @tc.name test setDateTime method in callback mode
     * @tc.desc set system date time in callback mode
     */
     it('setDateTime_test_002', 0, async function (done) {
      await enterpriseDeviceManager.activateAdmin(SELFWANT, ENTINFO1,
          enterpriseDeviceManager.AdminType.ADMIN_TYPE_SUPER);
      let dsmgr = await enterpriseDeviceManager.getDeviceSettingsManager();
      expect(dsmgr !== null).assertTrue();
      console.log('before setDateTime');
      await dsmgr.setDateTime(SELFWANT, 1526003846000, (error, data) => {
        console.log("setDateTime ===data: " + data);
        console.log("setDateTime ===error: " + error);
      });
      await enterpriseDeviceManager.deactivateSuperAdmin(SELFHAPNAME);
      done();
  })

    console.log('*************end EnterpriseDeviceManagerTest*************');
}) 