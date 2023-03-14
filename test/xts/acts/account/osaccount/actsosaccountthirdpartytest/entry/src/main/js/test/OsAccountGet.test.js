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
import osaccount from '@ohos.account.osAccount'
import bundle from '@ohos.bundle'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

const ERR_OS_ACCOUNT_SERVICE_MANAGER_BAD_UID_ERR = 4653057;
describe('ActsOsAccountThirdPartyTest', function () {

    /*
     * @tc.number  : ActsOsAccountGetIdFormUid_0100
     * @tc.name    : getOsAccountLocalIdFromUid callback
     * @tc.desc    : Verify that the user localId is obtained by uid
     */
    it('ActsOsAccountGetIdFormUid_0100', 0, async function (done) {
        console.debug("====>ActsOsAccountGetIdFormUid_0100 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var bundleName = "com.example.actsosaccountthirdpartytest";
        var bundleInfo = await bundle.getBundleInfo(bundleName, bundle.BundleFlag.GET_BUNDLE_WITH_ABILITIES);
        var uid = bundleInfo.uid;
        console.debug("====>obtained uid:" + uid);
        osAccountManager.getOsAccountLocalIdFromUid(uid, (err, localId)=>{
            console.debug("====>get localId err: " + JSON.stringify(err));
            console.debug("====>localId obtained by uid:" + localId);
            expect(err.code).assertEqual(0);
            expect(localId).assertEqual(100);
            console.debug("====>ActsOsAccountGetIdFormUid_0100 end====");
            done();
        });
    });

    /*
     * @tc.number  : ActsOsAccountGetIdFormUid_0200
     * @tc.name    : getOsAccountLocalIdFromUid promise
     * @tc.desc    : Verify that the user localId is obtained by uid
     */
    it('ActsOsAccountGetIdFormUid_0200', 0, async function (done) {
        console.debug("====>ActsOsAccountGetIdFormUid_0200 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var bundleName = "com.example.actsosaccountthirdpartytest";
        var bundleInfo = await bundle.getBundleInfo(bundleName, bundle.BundleFlag.GET_BUNDLE_WITH_ABILITIES);
        var uid = bundleInfo.uid;
        console.debug("====>obtained uid:" + uid);
        var localId = await osAccountManager.getOsAccountLocalIdFromUid(uid);
        console.debug("====>localId obtained by uid:" + localId);
        expect(localId).assertEqual(100);
        console.debug("====>ActsOsAccountGetIdFormUid_0200 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountGetIdFormUid_0300
     * @tc.name    : getOsAccountLocalIdFromUid callback
     * @tc.desc    : Authentication failed to query user by uid -1
     */
    it('ActsOsAccountGetIdFormUid_0300', 0, async function (done) {
        console.debug("====>ActsOsAccountGetIdFormUid_0300 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var incorrectUid = -1;
        osAccountManager.getOsAccountLocalIdFromUid(incorrectUid, (err, localId)=>{
            console.debug("====>get localId err: " + JSON.stringify(err));
            console.debug("====>localId obtained by uid:" + localId);
            expect(err.code).assertEqual(ERR_OS_ACCOUNT_SERVICE_MANAGER_BAD_UID_ERR);
            expect(localId).assertEqual(0);
            console.debug("====>ActsOsAccountGetIdFormUid_0300 end====");
            done();
        });
    });

    /*
     * @tc.number  : ActsOsAccountGetIdFormUid_0400
     * @tc.name    : getOsAccountLocalIdFromUid promise
     * @tc.desc    : Authentication failed to query user by uid -1
     */
    it('ActsOsAccountGetIdFormUid_0400', 0, async function (done) {
        console.debug("====>ActsOsAccountGetIdFormUid_0400 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var incorrectUid = -1;
        try{
            await osAccountManager.getOsAccountLocalIdFromUid(incorrectUid);
        }catch(err){
            console.debug("====>get localId by uid err:"  +JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OS_ACCOUNT_SERVICE_MANAGER_BAD_UID_ERR);
            console.debug("====>ActsOsAccountGetIdFormUid_0400 end====");
            done();
        }
    });

    /*
     * @tc.number  : ActsOsAccountGetIdFormUid_0500
     * @tc.name    : getOsAccountLocalIdFromUid callback
     * @tc.desc    : Authentication failed to query user by uid 2147483648
     */
    it('ActsOsAccountGetIdFormUid_0500', 0, async function (done) {
        console.debug("====>ActsOsAccountGetIdFormUid_0500 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var incorrectUid = 2147483648;
        osAccountManager.getOsAccountLocalIdFromUid(incorrectUid, (err, localId)=>{
            console.debug("====>get localId err: " + JSON.stringify(err));
            console.debug("====>localId obtained by uid:" + localId);
            expect(err.code).assertEqual(ERR_OS_ACCOUNT_SERVICE_MANAGER_BAD_UID_ERR);
            expect(localId).assertEqual(0);
            console.debug("====>ActsOsAccountGetIdFormUid_0500 end====");
            done();
        });
    });

    /*
     * @tc.number  : ActsOsAccountGetIdFormUid_0600
     * @tc.name    : getOsAccountLocalIdFromUid promise
     * @tc.desc    : Authentication failed to query user by uid 2147483648
     */
    it('ActsOsAccountGetIdFormUid_0600', 0, async function (done) {
        console.debug("====>ActsOsAccountGetIdFormUid_0600 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var incorrectUid = 2147483648;
        try{
            await osAccountManager.getOsAccountLocalIdFromUid(incorrectUid);
        }catch(err){
            console.debug("====>get localId by uid err:"  +JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OS_ACCOUNT_SERVICE_MANAGER_BAD_UID_ERR);
            console.debug("====>ActsOsAccountGetIdFormUid_0600 end====");
            done();
        }
    });

    /*
     * @tc.number  : ActsOsAccountGetIdFormProcess_0100
     * @tc.name    : getOsAccountLocalIdFromProcess callback
     * @tc.desc    : Verify that the user localId obtained from the current process uid
     */
    it('ActsOsAccountGetIdFormProcess_0100', 0, async function (done) {
        console.debug("====>ActsOsAccountGetIdFormProcess_0100 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.getOsAccountLocalIdFromProcess((err, localId)=>{
            console.debug("====>get localId err: " + JSON.stringify(err));
            console.debug("====>localId obtained by process:" + localId);
            expect(err.code).assertEqual(0);
            expect(localId).assertEqual(100);
            console.debug("====>ActsOsAccountGetIdFormProcess_0100 end====");
            done();
        });
    });

    /*
     * @tc.number  : ActsOsAccountGetIdFormProcess_0200
     * @tc.name    : getOsAccountLocalIdFromProcess promise
     * @tc.desc    : Verify that the user localId obtained from the current process uid
     */
    it('ActsOsAccountGetIdFormProcess_0200', 0, async function (done) {
        console.debug("====>ActsOsAccountGetIdFormProcess_0200 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId = await osAccountManager.getOsAccountLocalIdFromProcess();
        console.debug("====>localId obtained by process:" + localId);
        expect(localId).assertEqual(100);
        console.debug("====>ActsOsAccountGetIdFormProcess_0200 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountGetTypeFormProcess_0100
     * @tc.name    : getOsAccountTypeFromProcess callback
     * @tc.desc    : Verify that the user type obtained from the current process uid
     */
    it('ActsOsAccountGetTypeFormProcess_0100', 0, async function (done) {
        console.debug("====>ActsOsAccountGetTypeFormProcess_0100 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.getOsAccountTypeFromProcess((err, accountType)=>{
            console.debug("====>get type err: " + JSON.stringify(err));
            console.debug("====>type obtained by process:" + JSON.stringify(accountType));
            expect(err.code).assertEqual(0);
            expect(accountType.ADMIN).assertEqual(0);
            console.debug("====>ActsOsAccountGetTypeFormProcess_0100 end====");
            done();
        });
    });

    /*
     * @tc.number  : ActsOsAccountGetTypeFormProcess_0200
     * @tc.name    : getOsAccountTypeFromProcess promise
     * @tc.desc    : Verify that the user type obtained from the current process uid
     */
    it('ActsOsAccountGetTypeFormProcess_0200', 0, async function (done) {
        console.debug("====>ActsOsAccountGetTypeFormProcess_0200 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var accountType = await osAccountManager.getOsAccountTypeFromProcess();
        console.debug("====>type obtained by process:" + JSON.stringify(accountType));
        expect(accountType.ADMIN).assertEqual(0);
        console.debug("====>ActsOsAccountGetTypeFormProcess_0200 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountLocalIdSerial_0100
     * @tc.name    : getSerialNumberByOsAccountLocalId callback
     * @tc.desc    : Verify query serialNumber by 100 user and query 100 user by serialNumber
     */
    it('ActsOsAccountLocalIdSerial_0100', 0, async function (done) {
        console.debug("====>ActsOsAccountLocalIdSerial_0100 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.getSerialNumberByOsAccountLocalId(100, (err, serialNumber)=>{
            console.debug("====>ger serialNumber err:" + JSON.stringify(err));
            console.debug("====>get serialNumber:" + serialNumber + " by localId: 100" );
            expect(err.code).assertEqual(0);
            var serialNumberStr = serialNumber.toString();
            var serialIntercept = serialNumberStr.substring(8);
            console.debug("====>truncate the last eight characters: " + serialIntercept);
            expect(serialIntercept).assertEqual("00000001");
            osAccountManager.getOsAccountLocalIdBySerialNumber(serialNumber, (err, localId)=>{
                console.debug("====>ger localId err:" + JSON.stringify(err));
                console.debug("====>get localId:" + localId + " by serialNumber: " + serialNumber);
                expect(err.code).assertEqual(0);
                expect(localId).assertEqual(100);
                console.debug("====>ActsOsAccountLocalIdSerial_0100 end====");
                done();
            })
        })
    });

    /*
     * @tc.number  : ActsOsAccountLocalIdSerial_0200
     * @tc.name    : getSerialNumberByOsAccountLocalId promise
     * @tc.desc    : Verify query serialNumber by 100 user and query 100 user by serialNumber
     */
    it('ActsOsAccountLocalIdSerial_0200', 0, async function (done) {
        console.debug("====>ActsOsAccountLocalIdSerial_0200 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var serialNumber = await osAccountManager.getSerialNumberByOsAccountLocalId(100);
        console.debug("====>get serialNumber:" + serialNumber + " by localId: 100" );
        var serialNumberStr = serialNumber.toString();
        var serialIntercept = serialNumberStr.substring(8);
        console.debug("====>truncate the last eight characters: " + serialIntercept);
        expect(serialIntercept).assertEqual("00000001");
        var localId = await osAccountManager.getOsAccountLocalIdBySerialNumber(serialNumber);
        console.debug("====>get localId:" + localId + " by serialNumber: " + serialNumber);
        expect(localId).assertEqual(100);
        console.debug("====>ActsOsAccountLocalIdSerial_0200 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountLocalIdSerial_0300
     * @tc.name    : getOsAccountLocalIdBySerialNumber callback
     * @tc.desc    : Verify query serialNumber by 0 user and query 0 user by serialNumber
     */
    it('ActsOsAccountLocalIdSerial_0300', 0, async function (done) {
        console.debug("====>ActsOsAccountLocalIdSerial_0300 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.getSerialNumberByOsAccountLocalId(0, (err, serialNumber)=>{
            console.debug("====>ger serialNumber err:" + JSON.stringify(err));
            console.debug("====>get serialNumber:" + serialNumber + " by localId: 0" );
            expect(err.code).assertEqual(0);
            var serialNumberStr = serialNumber.toString();
            var serialIntercept = serialNumberStr.substring(8);
            console.debug("====>truncate the last eight characters: " + serialIntercept);
            expect(serialIntercept).assertEqual("00000000");
            osAccountManager.getOsAccountLocalIdBySerialNumber(serialNumber, (err, localId)=>{
                console.debug("====>ger localId err:" + JSON.stringify(err));
                console.debug("====>get localId:" + localId + " by serialNumber: " + serialNumber);
                expect(err.code).assertEqual(0);
                expect(localId).assertEqual(0);
                console.debug("====>ActsOsAccountLocalIdSerial_0300 end====");
                done();
            })
        })
    });

    /*
     * @tc.number  : ActsOsAccountLocalIdSerial_0400
     * @tc.name    : getOsAccountLocalIdBySerialNumber promise
     * @tc.desc    : Verify query serialNumber by 0 user and query 0 user by serialNumber
     */
    it('ActsOsAccountLocalIdSerial_0400', 0, async function (done) {
        console.debug("====>ActsOsAccountLocalIdSerial_0400 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var serialNumber = await osAccountManager.getSerialNumberByOsAccountLocalId(0);
        console.debug("====>get serialNumber:" + serialNumber + " by localId: 0" );
        var serialNumberStr = serialNumber.toString();
        var serialIntercept = serialNumberStr.substring(8);
        console.debug("====>truncate the last eight characters: " + serialIntercept);
        expect(serialIntercept).assertEqual("00000000");
        var localId = await osAccountManager.getOsAccountLocalIdBySerialNumber(serialNumber);
        console.debug("====>get localId:" + localId + " by serialNumber: " + serialNumber);
        expect(localId).assertEqual(0);
        console.debug("====>ActsOsAccountLocalIdSerial_0400 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountLocalIdSerial_0500
     * @tc.name    : getSerialNumberByOsAccountLocalId callback
     * @tc.desc    : Verify the query for the newly created user serialNumber and query the owning user through the
     *               serialNumber
     */
    it('ActsOsAccountLocalIdSerial_0500', 0, async function (done) {
        console.debug("====>ActsOsAccountLocalIdSerial_0500 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        osAccountManager.createOsAccount("osAccountNameIdSerialA", osaccount.OsAccountType.NORMAL, (err, data)=>{
            console.debug("====>create os account err: " + JSON.stringify(err));
            console.debug("====>create os account OsAccountInfo: " + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.localName).assertEqual("osAccountNameIdSerialA");
            localId = data.localId;
            osAccountManager.getSerialNumberByOsAccountLocalId(localId, (err, serialNumber)=>{
                console.debug("====>queryOsAccountById err:" + JSON.stringify(err));
                console.debug("====>get serialNumber:" + serialNumber + " by localId: " + localId);
                expect(err.code).assertEqual(0);
                osAccountManager.getOsAccountLocalIdBySerialNumber(serialNumber, (err, getlocalId)=>{
                    console.debug("====>ger localId err:" + JSON.stringify(err));
                    console.debug("====>get localId:" + getlocalId + " by serialNumber: " + serialNumber);
                    expect(err.code).assertEqual(0);
                    expect(getlocalId).assertEqual(localId);
                    osAccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountLocalIdSerial_0500 end====");
                        done();
                    })
                })
            })
        })
    });

    /*
     * @tc.number  : ActsOsAccountLocalIdSerial_0600
     * @tc.name    : getOsAccountLocalIdBySerialNumber promise
     * @tc.desc    : Verify the query for the newly created user serialNumber and query the owning user through the
     *               serialNumber
     */
    it('ActsOsAccountLocalIdSerial_0600', 0, async function (done) {
        console.debug("====>ActsOsAccountLocalIdSerial_0600 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var OsAccountInfo = await osAccountManager.createOsAccount("accountIdSerialB", osaccount.OsAccountType.NORMAL);
        console.debug("====>create os account OsAccountInfo: " + JSON.stringify(OsAccountInfo));
        expect(OsAccountInfo.localName).assertEqual("accountIdSerialB");
        localId = OsAccountInfo.localId;
        var serialNumber = await osAccountManager.getSerialNumberByOsAccountLocalId(localId);
        console.debug("====>get serialNumber:" + serialNumber + " by localId: " + localId);
        var getlocalId = await osAccountManager.getOsAccountLocalIdBySerialNumber(serialNumber);
        console.debug("====>get localId:" + getlocalId + " by serialNumber: " + serialNumber);
        expect(getlocalId).assertEqual(localId);
        await osAccountManager.removeOsAccount(localId);
        console.debug("====>ActsOsAccountLocalIdSerial_0600 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountGetCount_0100
     * @tc.name    : getCreatedOsAccountsCount callback
     * @tc.desc    : Verify to obtain the number os all os accounts created
     */
    it('ActsOsAccountGetCount_0100', 0, async function (done) {
        console.debug("====>ActsOsAccountGetCount_0100 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var obtainCount = 0;
        var localIdFir;
        var localIdSec;
        osAccountManager.getCreatedOsAccountsCount((err, data)=>{
            console.debug("====>obtains the number of all os accounts created err:" + JSON.stringify(err));
            console.debug("====>obtains the number of all os accounts created data:" + data);
            expect(err.code).assertEqual(0);
            obtainCount = data;
            osAccountManager.createOsAccount("osAccountNameIdSerialE", osaccount.OsAccountType.NORMAL, (err, data)=>{
                console.debug("====>create first os account err: " + JSON.stringify(err));
                console.debug("====>create first os account OsAccountInfo: " + JSON.stringify(data));
                localIdFir = data.localId;
                expect(err.code).assertEqual(0);
                expect(data.localName).assertEqual("osAccountNameIdSerialE");
                osAccountManager.createOsAccount("osAccountIdSerialF", osaccount.OsAccountType.NORMAL, (err, data)=>{
                    console.debug("====>create second os account err: " + JSON.stringify(err));
                    console.debug("====>create second os account OsAccountInfo: " + JSON.stringify(data));
                    localIdSec = data.localId;
                    expect(err.code).assertEqual(0);
                    expect(data.localName).assertEqual("osAccountIdSerialF");
                    osAccountManager.getCreatedOsAccountsCount((err, count)=>{
                        console.debug("====>obtains the number of all os accounts created err:" + JSON.stringify(err));
                        console.debug("====>obtains the number of all os accounts created count:" + count);
                        expect(err.code).assertEqual(0);
                        count = count - 2;
                        expect(count).assertEqual(obtainCount);
                        osAccountManager.removeOsAccount(localIdFir, (err)=>{
                            console.debug("====>remove localId: " + localIdFir + " err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            osAccountManager.getCreatedOsAccountsCount((err, data)=>{
                                console.debug("====>obtains the number accounts created err:" + JSON.stringify(err));
                                console.debug("====>obtains the number accounts created data:" + data);
                                expect(err.code).assertEqual(0);
                                data = data - 1;
                                expect(data).assertEqual(obtainCount);
                                osAccountManager.removeOsAccount(localIdSec, (err)=>{
                                    console.debug("====>remove localId: " + localIdSec + " err:" + JSON.stringify(err));
                                    expect(err.code).assertEqual(0);
                                    console.debug("====>ActsOsAccountGetCount_0100 end====");
                                    done();
                                })
                            })
                        })
                    })
                })
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountGetCount_0200
     * @tc.name    : getCreatedOsAccountsCount promise
     * @tc.desc    : Verify to obtain the number os all os accounts created
     */
    it('ActsOsAccountGetCount_0200', 0, async function (done) {
        console.debug("====>ActsOsAccountGetCount_0200 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var obtainCount = await osAccountManager.getCreatedOsAccountsCount();
        console.debug("====>obtains the number of all os accounts created:" + obtainCount);
        var osAccountFir = await osAccountManager.createOsAccount("osAccountIdSerialG", osaccount.OsAccountType.NORMAL);
        console.debug("====>create first os account OsAccountInfo: " + JSON.stringify(osAccountFir));
        var localIdFir = osAccountFir.localId;
        expect(osAccountFir.localName).assertEqual("osAccountIdSerialG");
        var osAccountSec = await osAccountManager.createOsAccount("osAccountIdSerialH", osaccount.OsAccountType.NORMAL);
        console.debug("====>create second os account OsAccountInfo: " + JSON.stringify(osAccountSec));
        var localIdSec = osAccountSec.localId;
        expect(osAccountSec.localName).assertEqual("osAccountIdSerialH");
        var countFir = await osAccountManager.getCreatedOsAccountsCount();            
        console.debug("====>obtains the number of all os accounts created count:" + countFir);
        countFir = countFir - 2;
        expect(countFir).assertEqual(obtainCount);
        await osAccountManager.removeOsAccount(localIdFir);
        console.debug("====>remove localId: " + localIdFir);
        var countSec = await osAccountManager.getCreatedOsAccountsCount();
        console.debug("====>obtains the number accounts created count:" + countSec);
        countSec = countSec - 1;
        expect(countSec).assertEqual(obtainCount);
        await osAccountManager.removeOsAccount(localIdSec);
        console.debug("====>remove localId: " + localIdSec);
        console.debug("====>ActsOsAccountGetCount_0200 end====");
        done();
    })
})