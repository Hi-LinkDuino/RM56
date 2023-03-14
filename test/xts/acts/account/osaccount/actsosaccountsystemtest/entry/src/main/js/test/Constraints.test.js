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
import osAccount from '@ohos.account.osAccount'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('ActsOsAccountSystemTest', function () {

    /*
     * @tc.number  : ActsOsAccountConstraints_0100
     * @tc.name    : Constraints callback
     * @tc.desc    : get constraints for users of the same type as 100 user
     */
    it('ActsOsAccountConstraints_0100', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_0100 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var adminConstraints = ["constraint.wifi","constraint.wifi.set","constraint.locale.set",
        "constraint.app.accounts","constraint.apps.install","constraint.apps.uninstall","constraint.location.shared",
        "constraint.unknown.sources.install","constraint.global.unknown.app.install","constraint.bluetooth.set",
        "constraint.bluetooth"];
        var localId;
        AccountManager.createOsAccount("osAccountNameCreate", osAccount.OsAccountType.ADMIN, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            AccountManager.getOsAccountAllConstraints(localId, (err, constraints)=>{
                console.debug("====>getOsAccountAllConstraints err:" + JSON.stringify(err));
                console.debug("====>getOsAccountAllConstraints:" + JSON.stringify(constraints));
                expect(err.code).assertEqual(0);
                expect(constraints.length).assertEqual(adminConstraints.length);
                var succeed = 0;
                var failed = 0;
                for(var j = 0; j<constraints.length; j++){
                    if(adminConstraints.indexOf(constraints[j]) == -1){
                        failed++;
                    }
                    else{
                        succeed++;
                    }
                }
                console.debug("====>the number of constraints obtained:" + succeed);
                console.debug("====>number of constraints not fetched:" + failed);
                expect(failed).assertEqual(0);
                AccountManager.removeOsAccount(localId, (err)=>{
                    console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                    console.debug("====>ActsOsAccountConstraints_0100 end====");
                    done();
                })
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_0200
     * @tc.name    : Constraints promise
     * @tc.desc    : get constraints for users of the same type as 100 user
     */
    it('ActsOsAccountConstraints_0200', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_0200 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var adminConstraints = ["constraint.wifi","constraint.wifi.set","constraint.locale.set",
        "constraint.app.accounts","constraint.apps.install","constraint.apps.uninstall","constraint.location.shared",
        "constraint.unknown.sources.install","constraint.global.unknown.app.install","constraint.bluetooth.set",
        "constraint.bluetooth"];
        var localId;
        var osAccountInfo = await AccountManager.createOsAccount("osAccountNameCreate", osAccount.OsAccountType.ADMIN);
        console.debug("====>createOsAccount:" + JSON.stringify(osAccountInfo));
        localId = osAccountInfo.localId;
        var constraints = await AccountManager.getOsAccountAllConstraints(localId);
        console.debug("====>getAccountManager constraints:" + JSON.stringify(constraints));
        expect(constraints.length).assertEqual(adminConstraints.length);
        var succeed = 0;
        var failed = 0;
        for(var j = 0; j<constraints.length; j++){
            if(adminConstraints.indexOf(constraints[j]) == -1){
                failed++;
            }
            else{
                succeed++;
            }
        }
        console.debug("====>the number of constraints obtained:" + succeed);
        console.debug("====>number of constraints not fetched:" + failed);
        expect(failed).assertEqual(0);
        await AccountManager.removeOsAccount(localId);
        console.debug("====>ActsOsAccountConstraints_0200 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_0300
     * @tc.name    : Constraints callback
     * @tc.desc    : get 0 local user all constraints
     */
    it('ActsOsAccountConstraints_0300', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_0300 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        AccountManager.getOsAccountAllConstraints(0, (err, constraints)=>{
            console.debug("====>getOsAccountAllConstraints err:" + JSON.stringify(err));
            console.debug("====>getOsAccountAllConstraints:" + JSON.stringify(constraints));
            expect(err.code).assertEqual(0);
            expect(constraints.length).assertEqual(0);
            console.debug("====>ActsOsAccountConstraints_0300 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_0400
     * @tc.name    : Constraints promise
     * @tc.desc    : get 0 local user all constraints
     */
    it('ActsOsAccountConstraints_0400', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_0400 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var constraints = await AccountManager.getOsAccountAllConstraints(0);
        console.debug("====>getAccountManager constraints:" +JSON.stringify(constraints));
        expect(constraints.length).assertEqual(0);
        console.debug("====>ActsOsAccountConstraints_0400 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_0500
     * @tc.name    : Constraints callback
     * @tc.desc    : create and add constraints
     */
    it('ActsOsAccountConstraints_0500', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_0500 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        AccountManager.createOsAccount('NameConstraintsC', osAccount.OsAccountType.NORMAL, (err, data)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.localName).assertEqual("NameConstraintsC");
            localId = data.localId;
            AccountManager.isOsAccountConstraintEnable(localId, "constraint.location.set", (err, constraint)=>{
                console.debug("====>isOsAccountConstraintEnable err:" + JSON.stringify(err));
                console.debug("====>isOsAccountConstraintEnable constraint:" + constraint);
                expect(err.code).assertEqual(0);
                expect(constraint).assertFalse();
                AccountManager.setOsAccountConstraints(localId, ["constraint.location.set"], true, (err)=>{
                    console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    AccountManager.isOsAccountConstraintEnable(localId, "constraint.location.set", (err, constraint)=>{
                        console.debug("====>twice isOsAccountConstraintEnable err:" + JSON.stringify(err));
                        console.debug("====>twice isOsAccountConstraintEnable constraint:" + constraint);
                        expect(err.code).assertEqual(0);
                        expect(constraint).assertTrue();
                        AccountManager.removeOsAccount(localId, (err)=>{
                            console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            console.debug("====>ActsOsAccountConstraints_0500 end====");
                            done();
                        })
                    })
                })
            })
        })
    })

     /*
     * @tc.number  : ActsOsAccountConstraints_0600
     * @tc.name    : Constraints promise
     * @tc.desc    : create and add constraints
     */
     it('ActsOsAccountConstraints_0600', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_0600 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var info = await AccountManager.createOsAccount("NameConstraintsP", osAccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("NameConstraintsP");
        localId = info.localId;
        var constraint = await AccountManager.isOsAccountConstraintEnable(localId, "constraint.location.set");
        console.debug("====>isOsAccountConstraintEnable constraints:" + constraint);
        expect(constraint).assertFalse();
        try{
            await AccountManager.setOsAccountConstraints(localId, ["constraint.location.set"], true);
        }
        catch(err){
            console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        var constraintTwice = await AccountManager.isOsAccountConstraintEnable(localId, "constraint.location.set");
        console.debug("====>isOsAccountConstraintEnable constraints:" + constraintTwice);
        expect(constraintTwice).assertTrue();
        await AccountManager.removeOsAccount(localId);
        console.debug("====>ActsOsAccountConstraints_0600 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_0700
     * @tc.name    : Constraints callback
     * @tc.desc    : create and delete constraints
     */
    it('ActsOsAccountConstraints_0700', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_0700 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        AccountManager.createOsAccount('NameConstraintsC', osAccount.OsAccountType.NORMAL, (err, data)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.localName).assertEqual("NameConstraintsC");
            localId = data.localId;
            AccountManager.isOsAccountConstraintEnable(localId, "constraint.bluetooth.set", (err, constraint)=>{
                console.debug("====>isOsAccountConstraintEnable err:" + JSON.stringify(err));
                console.debug("====>isOsAccountConstraintEnable constraint:" + constraint);
                expect(err.code).assertEqual(0);
                expect(constraint).assertTrue();
                AccountManager.setOsAccountConstraints(localId, ["constraint.bluetooth.set"], false, (err)=>{
                    console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    AccountManager.isOsAccountConstraintEnable(localId, "constraint.bluetooth.set", (err, constraint)=>{
                        console.debug("====>twice isOsAccountConstraintEnable err:" + JSON.stringify(err));
                        console.debug("====>twice isOsAccountConstraintEnable constraint:" + constraint);
                        expect(err.code).assertEqual(0);
                        expect(constraint).assertFalse();
                        AccountManager.removeOsAccount(localId, (err)=>{
                            console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            console.debug("====>ActsOsAccountConstraints_0700 end====");
                            done();
                        })
                    })
                })
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_0800
     * @tc.name    : Constraints promise
     * @tc.desc    : create and delete constraints
     */
    it('ActsOsAccountConstraints_0800', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_0800 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var info = await AccountManager.createOsAccount("NameConstraintsP", osAccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("NameConstraintsP");
        localId = info.localId;
        var constraint = await AccountManager.isOsAccountConstraintEnable(localId, "constraint.bluetooth.set");
        console.debug("====>isOsAccountConstraintEnable constraints:" + constraint);
        expect(constraint).assertTrue();
        await AccountManager.setOsAccountConstraints(localId, ["constraint.bluetooth.set"], false);
        var constraintTwice = await AccountManager.isOsAccountConstraintEnable(localId, "constraint.bluetooth.set");
        console.debug("====>isOsAccountConstraintEnable constraints:" + constraintTwice);
        expect(constraintTwice).assertFalse();
        await AccountManager.removeOsAccount(localId);
        console.debug("====>ActsOsAccountConstraints_0800 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_0900
     * @tc.name    : Constraints callback
     * @tc.desc    : add the same constraints consecutively
     */
    it('ActsOsAccountConstraints_0900', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_0900 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        AccountManager.createOsAccount('NameConstraintsC', osAccount.OsAccountType.NORMAL, (err, data)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.localName).assertEqual("NameConstraintsC");
            localId = data.localId;
            AccountManager.isOsAccountConstraintEnable(localId, "constraint.location.set", (err, constraint)=>{
                console.debug("====>isOsAccountConstraintEnable err:" + JSON.stringify(err));
                console.debug("====>isOsAccountConstraintEnable constraint:" + constraint);
                expect(err.code).assertEqual(0);
                expect(constraint).assertFalse();
                AccountManager.setOsAccountConstraints(localId, ["constraint.location.set"], true, (err)=>{
                    console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    AccountManager.getOsAccountAllConstraints(localId, (err, constraints)=>{
                        console.debug("====>getOsAccountAllConstraints err:" + JSON.stringify(err));
                        console.debug("====>getOsAccountAllConstraints :" + JSON.stringify(constraints));
                        expect(err.code).assertEqual(0);
                        let result = constraints.includes("constraint.location.set");
                        expect(result).assertTrue();
                        AccountManager.setOsAccountConstraints(localId, ["constraint.location.set"], true, (err)=>{
                            console.debug("====>twice setOsAccountConstraints err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            AccountManager.getOsAccountAllConstraints(localId, (err, constraints)=>{
                                console.debug("====>twice getOsAccountAllConstraints err:" + JSON.stringify(err));
                                console.debug("====>twice getOsAccountAllConstraints :" + JSON.stringify(constraints));
                                expect(err.code).assertEqual(0);
                                let result = constraints.includes("constraint.location.set");
                                expect(result).assertTrue();
                                AccountManager.removeOsAccount(localId, (err)=>{
                                    console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                                    expect(err.code).assertEqual(0);
                                    console.debug("====>ActsOsAccountConstraints_0900 end====");
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
     * @tc.number  : ActsOsAccountConstraints_1000
     * @tc.name    : Constraints promise
     * @tc.desc    : add the same constraints consecutively
     */
    it('ActsOsAccountConstraints_1000', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_1000 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var info = await AccountManager.createOsAccount("NameConstraintsP", osAccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("NameConstraintsP");
        localId = info.localId;
        var constraint = await AccountManager.isOsAccountConstraintEnable(localId, "constraint.location.set");
        console.debug("====>isOsAccountConstraintEnable constraints:" + constraint);
        expect(constraint).assertFalse();
        await AccountManager.setOsAccountConstraints(localId, ["constraint.location.set"], true);
        var constraints = await AccountManager.getOsAccountAllConstraints(localId);
        console.debug("====>getOsAccountAllConstraints:" + JSON.stringify(constraints));
        let result = constraints.includes("constraint.location.set");
        expect(result).assertTrue();
        await AccountManager.setOsAccountConstraints(localId, ["constraint.location.set"], true);
        var constraints = await AccountManager.getOsAccountAllConstraints(localId);
        console.debug("====>twice getOsAccountAllConstraints:" + JSON.stringify(constraints));
        let result1 = constraints.includes("constraint.location.set");
        expect(result1).assertTrue();
        await AccountManager.removeOsAccount(localId);
        console.debug("====>ActsOsAccountConstraints_1000 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_1100
     * @tc.name    : Constraints callback
     * @tc.desc    : delete the same constraints consecutively
     */
    it('ActsOsAccountConstraints_1100', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_1100 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        AccountManager.createOsAccount('NameConstraintsC', osAccount.OsAccountType.NORMAL, (err, data)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.localName).assertEqual("NameConstraintsC");
            localId = data.localId;
            AccountManager.isOsAccountConstraintEnable(localId, "constraint.network.reset", (err, constraint)=>{
                console.debug("====>isOsAccountConstraintEnable err:" + JSON.stringify(err));
                console.debug("====>isOsAccountConstraintEnable constraint:" + constraint);
                expect(err.code).assertEqual(0);
                expect(constraint).assertTrue();
                AccountManager.setOsAccountConstraints(localId, ["constraint.network.reset"], false, (err)=>{
                    console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    AccountManager.getOsAccountAllConstraints(localId, (err, constraints)=>{
                        console.debug("====>getOsAccountAllConstraints err:" + JSON.stringify(err));
                        console.debug("====>getOsAccountAllConstraints :" + JSON.stringify(constraints));
                        expect(err.code).assertEqual(0);
                        var number = 0;
                        for(var i=0; i < constraints.length; i++){
                            if(constraints[i] != "constraint.network.reset"){
                                number++;
                            }
                        }
                        if(number == constraints.length-1){
                            expect().assertTrue();
                        }
                        AccountManager.setOsAccountConstraints(localId, ["constraint.network.reset"], false, (err)=>{
                            console.debug("====>twice setOsAccountConstraints err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            AccountManager.getOsAccountAllConstraints(localId, (err, constraints)=>{
                                console.debug("====>twice getOsAccountAllConstraints err:" + JSON.stringify(err));
                                console.debug("====>twice getOsAccountAllConstraints :" + JSON.stringify(constraints));
                                expect(err.code).assertEqual(0);
                                var numberTwo = 0;
                                for(var i=0; i < constraints.length; i++){
                                    if(constraints[i] != "constraint.network.reset"){
                                        numberTwo++;
                                    }
                                }
                                if(numberTwo == constraints.length-1){
                                    expect().assertTrue();
                                }
                                AccountManager.removeOsAccount(localId, (err)=>{
                                    console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                                    expect(err.code).assertEqual(0);
                                    console.debug("====>ActsOsAccountConstraints_1100 end====");
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
     * @tc.number  : ActsOsAccountConstraints_1200
     * @tc.name    : Constraints promise
     * @tc.desc    : delete the same constraints consecutively
     */
    it('ActsOsAccountConstraints_1200', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_1200 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var info = await AccountManager.createOsAccount("NameConstraintsP", osAccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("NameConstraintsP");
        localId = info.localId;
        var constraint = await AccountManager.isOsAccountConstraintEnable(localId, "constraint.network.reset");
        console.debug("====>isOsAccountConstraintEnable constraints:" + constraint);
        expect(constraint).assertTrue();
        await AccountManager.setOsAccountConstraints(localId, ["constraint.network.reset"], false);
        var constraints = await AccountManager.getOsAccountAllConstraints(localId);
        console.debug("====>getOsAccountAllConstraints:" + JSON.stringify(constraints));
        var number = 0;
        for(var i=0; i < constraints.length; i++){
            if(constraints[i] != "constraint.network.reset"){
                number++;
            }
        }
        if(number == constraints.length-1){
            expect().assertTrue();
        }
        await AccountManager.setOsAccountConstraints(localId, ["constraint.network.reset"], false);
        var constraints = await AccountManager.getOsAccountAllConstraints(localId);
        console.debug("====>getOsAccountAllConstraints:" + JSON.stringify(constraints));
        var numberTwo = 0;
        for(var i=0; i < constraints.length; i++){
            if(constraints[i] != "constraint.network.reset"){
                numberTwo++;
            }
        }
        if(numberTwo == constraints.length-1){
            expect().assertTrue();
        }
        await AccountManager.removeOsAccount(localId);
        console.debug("====>ActsOsAccountConstraints_1200 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_1300
     * @tc.name    : Constraints callback
     * @tc.desc    : to add a constraint not in the constraint list
     */
    it('ActsOsAccountConstraints_1300', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_1300 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        AccountManager.isOsAccountConstraintEnable(100, "constraint.undefined", (err, constraint)=>{
            console.debug("====>isOsAccountConstraintEnable err:" + JSON.stringify(err));
            console.debug("====>isOsAccountConstraintEnable constraint:" + constraint);
            expect(err.code).assertEqual(0);
            expect(constraint).assertFalse();
            AccountManager.setOsAccountConstraints(100, ["constraint.undefined"], true, (err)=>{
                console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
                expect(err.code != 0 ).assertTrue();
                console.debug("====>ActsOsAccountConstraints_1300 end====");
                done();
            })
        })
    })

     /*
     * @tc.number  : ActsOsAccountConstraints_1400
     * @tc.name    : Constraints promise
     * @tc.desc    : to add a constraint not in the constraint list
     */
    it('ActsOsAccountConstraints_1400', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_1400 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var constraint = await AccountManager.isOsAccountConstraintEnable(100, "constraint.undefined");
        console.debug("====>isOsAccountConstraintEnable constraints:" + constraint);
        expect(constraint).assertFalse();
        try{
            await AccountManager.setOsAccountConstraints(100, ["constraint.undefined"], true);
        }
        catch(err){
            console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
            expect(err.code != 0).assertTrue();
            console.debug("====>ActsOsAccountConstraints_1400 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_1500
     * @tc.name    : Constraints callback
     * @tc.desc    : to add constraint is empty string and not in constraint list
     */
    it('ActsOsAccountConstraints_1500', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_1500 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        AccountManager.isOsAccountConstraintEnable(100, "", (err, constraint)=>{
            console.debug("====>isOsAccountConstraintEnable err:" + JSON.stringify(err));
            console.debug("====>isOsAccountConstraintEnable constraint:" + constraint);
            expect(err.code).assertEqual(0);
            expect(constraint).assertFalse();
            AccountManager.setOsAccountConstraints(100, [""], true, (err)=>{
                console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
                expect(err.code != 0 ).assertTrue();
                console.debug("====>ActsOsAccountConstraints_1500 end====");
                done();
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_1600
     * @tc.name    : Constraints promise
     * @tc.desc    : to add constraint is empty string and not in constraint list
     */
    it('ActsOsAccountConstraints_1600', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_1600 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var constraint = await AccountManager.isOsAccountConstraintEnable(100, "");
        console.debug("====>isOsAccountConstraintEnable constraints:" + constraint);
        expect(constraint).assertFalse();
        try{
            await AccountManager.setOsAccountConstraints(100, [""], true);
        }
        catch(err){
            console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
            expect(err.code != 0).assertTrue();
            console.debug("====>ActsOsAccountConstraints_1600 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_1700
     * @tc.name    : Constraints callback
     * @tc.desc    : to delete a constraint not in the constraint list
     */
    it('ActsOsAccountConstraints_1700', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_1700 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        AccountManager.isOsAccountConstraintEnable(100, "constraint.undefined", (err, constraint)=>{
            console.debug("====>isOsAccountConstraintEnable err:" + JSON.stringify(err));
            console.debug("====>isOsAccountConstraintEnable constraint:" + constraint);
            expect(err.code).assertEqual(0);
            expect(constraint).assertFalse();
            AccountManager.setOsAccountConstraints(100, ["constraint.undefined"], false, (err)=>{
                console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
                expect(err.code != 0 ).assertTrue();
                console.debug("====>ActsOsAccountConstraints_1700 end====");
                done();
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_1800
     * @tc.name    : Constraints promise
     * @tc.desc    : to delete a constraint not in the constraint list
     */
    it('ActsOsAccountConstraints_1800', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_1800 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var constraint = await AccountManager.isOsAccountConstraintEnable(100, "constraint.undefined");
        console.debug("====>isOsAccountConstraintEnable constraints:" + constraint);
        expect(constraint).assertFalse();
        try{
            await AccountManager.setOsAccountConstraints(100, ["constraint.undefined"], false);
        }
        catch(err){
            console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
            expect(err.code != 0).assertTrue();
            console.debug("====>ActsOsAccountConstraints_1800 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_1900
     * @tc.name    : Constraints callback
     * @tc.desc    : add constraints, whether in the constraint list or not in the constraint list
     */
    it('ActsOsAccountConstraints_1900', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_1900 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        AccountManager.createOsAccount('NameConstraintsC', osAccount.OsAccountType.NORMAL, (err, data)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.localName).assertEqual("NameConstraintsC");
            localId = data.localId;
            AccountManager.isOsAccountConstraintEnable(localId, "constraint.device.unmute", (err, constraint)=>{
                console.debug("====>isOsAccountConstraintEnable err:" + JSON.stringify(err));
                console.debug("====>isOsAccountConstraintEnable constraint:" + constraint);
                expect(err.code).assertEqual(0);
                expect(constraint).assertFalse();
                AccountManager.setOsAccountConstraints(localId, ["constraint.device.unmute",
                "constraint.undefined"], true, (err)=>{
                    console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
                    expect(err.code != 0).assertTrue();
                    AccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountConstraints_1900 end====");
                        done();
                    })
                })
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_2000
     * @tc.name    : Constraints promise
     * @tc.desc    : add constraints, whether in the constraint list or not in the constraint list
     */
    it('ActsOsAccountConstraints_2000', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_2000 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var info = await AccountManager.createOsAccount("NameConstraintsP", osAccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("NameConstraintsP");
        localId = info.localId;
        var constraint = await AccountManager.isOsAccountConstraintEnable(localId, "constraint.device.unmute");
        console.debug("====>isOsAccountConstraintEnable constraints:" + constraint);
        expect(constraint).assertFalse();
        try{
            await AccountManager.setOsAccountConstraints(localId, ["constraint.device.unmute",
            "constraint.undefined"], true);
        }
        catch(err){
            console.debug("====>setOsAccountConstraints:" + JSON.stringify(err));
            expect(err.code != 0).assertTrue();
            await AccountManager.removeOsAccount(localId);
            console.debug("====>ActsOsAccountConstraints_2000 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_2100
     * @tc.name    : Constraints callback
     * @tc.desc    : delete constraints, whether in the constraint list or not in the constraint list
     */
    it('ActsOsAccountConstraints_2100', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_2100 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        AccountManager.createOsAccount('NameConstraintsC', osAccount.OsAccountType.NORMAL, (err, data)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.localName).assertEqual("NameConstraintsC");
            localId = data.localId;
            AccountManager.isOsAccountConstraintEnable(localId, "constraint.cell.broadcasts.set", (err, constraint)=>{
                console.debug("====>isOsAccountConstraintEnable err:" + JSON.stringify(err));
                console.debug("====>isOsAccountConstraintEnable constraint:" + constraint);
                expect(err.code).assertEqual(0);
                expect(constraint).assertTrue();
                AccountManager.setOsAccountConstraints(localId, ["constraint.cell.broadcasts.set",
                "constraint.undefined"], false, (err)=>{
                    console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
                    expect(err.code != 0).assertTrue();
                    AccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountConstraints_2100 end====");
                        done();
                    })
                })
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_2200
     * @tc.name    : Constraints promise
     * @tc.desc    : delete constraints, whether in the constraint list or not in the constraint list
     */
    it('ActsOsAccountConstraints_2200', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_2200 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var info = await AccountManager.createOsAccount("NameConstraintsP", osAccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("NameConstraintsP");
        localId = info.localId;
        var constraint = await AccountManager.isOsAccountConstraintEnable(localId, "constraint.device.unmute");
        console.debug("====>isOsAccountConstraintEnable constraints:" + constraint);
        expect(constraint).assertFalse();
        try{
            await AccountManager.setOsAccountConstraints(localId, ["constraint.device.unmute",
            "constraint.undefined"], true);
        }
        catch(err){
            console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
            expect(err.code !=0 ).assertTrue();
            await AccountManager.removeOsAccount(localId);
            console.debug("====>ActsOsAccountConstraints_2200 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_2300
     * @tc.name    : Constraints callback
     * @tc.desc    : create a local user of type normal and get all constraints
     */
    it('ActsOsAccountConstraints_2300', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_2300 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var normalConstraint = ["constraint.global.unknown.app.install","constraint.bluetooth.set",
        "constraint.bluetooth","constraint.bluetooth.share","constraint.usb.file.transfer",
        "constraint.credentials.set","constraint.managed.profile.remove","constraint.debug.features.use",
        "constraint.vpn.set","constraint.date.time.set","constraint.tethering.config",
        "constraint.network.reset","constraint.factory.reset","constraint.add.managed.profile",
        "constraint.apps.verify.disable","constraint.cell.broadcasts.set"];
        AccountManager.createOsAccount('NameConstraintsC', osAccount.OsAccountType.NORMAL, (err, data)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.localName).assertEqual("NameConstraintsC");
            localId = data.localId;
            AccountManager.getOsAccountAllConstraints(localId, (err, allConstraints)=>{
                console.debug("====>getOsAccountAllConstraints err:" + JSON.stringify(err));
                console.debug("====>getOsAccountAllConstraints:" + JSON.stringify(allConstraints));
                expect(err.code).assertEqual(0);
                expect(allConstraints.length).assertEqual(normalConstraint.length);
                var succeed = 0;
                var failed = 0;
                for(var j = 0; j<allConstraints.length; j++){
                    if(normalConstraint.indexOf(allConstraints[j]) == -1){
                        failed++;
                    }
                    else{
                        succeed++;
                    }
                }
                console.debug("====>the number of constraints obtained:" + succeed);
                console.debug("====>number of constraints not fetched:" + failed);
                if(failed == 0){
                    AccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountConstraints_2300 end====");
                        done();
                    })
                }else{
                    AccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        expect().assertFail();
                        console.debug("====>ActsOsAccountConstraints_2300 end====");
                        done();
                    })
                }
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_2400
     * @tc.name    : Constraints promise
     * @tc.desc    : create a local user of type normal and get all constraints
     */
    it('ActsOsAccountConstraints_2400', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_2400 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var normalConstraint = ["constraint.global.unknown.app.install","constraint.bluetooth.set",
        "constraint.bluetooth","constraint.bluetooth.share","constraint.usb.file.transfer",
        "constraint.credentials.set","constraint.managed.profile.remove","constraint.debug.features.use",
        "constraint.vpn.set","constraint.date.time.set","constraint.tethering.config",
        "constraint.network.reset","constraint.factory.reset","constraint.add.managed.profile",
        "constraint.apps.verify.disable","constraint.cell.broadcasts.set"];
        var info = await AccountManager.createOsAccount("NameConstraintsP", osAccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("NameConstraintsP");
        localId = info.localId;
        var allConstraints = await AccountManager.getOsAccountAllConstraints(localId);
        console.debug("====>getOsAccountAllConstraints:" + JSON.stringify(allConstraints));
        expect(allConstraints.length).assertEqual(normalConstraint.length);
        var succeed = 0;
        var failed = 0;
        for(var j = 0; j<allConstraints.length; j++){
            if(normalConstraint.indexOf(allConstraints[j]) == -1){
                failed++;
            }
            else{
                succeed++;
            }
        }
        console.debug("====>the number of constraints obtained:" + succeed);
        console.debug("====>number of constraints not fetched:" + failed);
        if(failed == 0){
            await AccountManager.removeOsAccount(localId);
            console.debug("====>ActsOsAccountConstraints_2400 end====");
            done();
        }else{
            await AccountManager.removeOsAccount(localId);
            expect().assertFail();
            console.debug("====>ActsOsAccountConstraints_2400 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_2500
     * @tc.name    : Constraints callback
     * @tc.desc    : create a local user of type guest and get all constraints
     */
    it('ActsOsAccountConstraints_2500', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_2500 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var guestConstraints = ["constraint.tethering.config","constraint.network.reset","constraint.factory.reset",
        "constraint.add.managed.profile","constraint.apps.verify.disable","constraint.cell.broadcasts.set",
        "constraint.mobile.networks.set","constraint.control.apps","constraint.physical.media","constraint.microphone",
        "constraint.microphone.unmute","constraint.volume.adjust","constraint.calls.outgoing","constraint.sms.use"];
        AccountManager.createOsAccount('NameConstraintsC', osAccount.OsAccountType.GUEST, (err, data)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.localName).assertEqual("NameConstraintsC");
            localId = data.localId;
            AccountManager.getOsAccountAllConstraints(localId, (err, allConstraints)=>{
                console.debug("====>getOsAccountAllConstraints err:" + JSON.stringify(err));
                console.debug("====>getOsAccountAllConstraints:" + JSON.stringify(allConstraints));
                expect(err.code).assertEqual(0);
                expect(allConstraints.length).assertEqual(guestConstraints.length);
                var succeed = 0;
                var failed = 0;
                for(var j = 0; j<allConstraints.length; j++){
                    if(guestConstraints.indexOf(allConstraints[j]) == -1){
                        failed++;
                    }
                    else{
                        succeed++;
                    }
                }
                console.debug("====>the number of constraints obtained:" + succeed);
                console.debug("====>number of constraints not fetched:" + failed);
                if(failed == 0){
                    AccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountConstraints_2500 end====");
                        done();
                    })
                }else{
                    AccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        expect().assertFail();
                        console.debug("====>ActsOsAccountConstraints_2500 end====");
                        done();
                    })
                }
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_2600
     * @tc.name    : Constraints promise
     * @tc.desc    : create a local user of type guest and get all constraints
     */
    it('ActsOsAccountConstraints_2600', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_2600 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var guestConstraints = ["constraint.tethering.config","constraint.network.reset","constraint.factory.reset",
        "constraint.add.managed.profile","constraint.apps.verify.disable","constraint.cell.broadcasts.set",
        "constraint.mobile.networks.set","constraint.control.apps","constraint.physical.media","constraint.microphone",
        "constraint.microphone.unmute","constraint.volume.adjust","constraint.calls.outgoing","constraint.sms.use"];
        var info = await AccountManager.createOsAccount("NameConstraintsP", osAccount.OsAccountType.GUEST);
        console.debug("====>createOsAccount info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("NameConstraintsP");
        localId = info.localId;
        var allConstraints = await AccountManager.getOsAccountAllConstraints(localId);
        console.debug("====>getOsAccountAllConstraints:" + JSON.stringify(allConstraints));
        expect(allConstraints.length).assertEqual(guestConstraints.length);
        var succeed = 0;
        var failed = 0;
        for(var j = 0; j<allConstraints.length; j++){
            if(guestConstraints.indexOf(allConstraints[j]) == -1){
                failed++;
            }
            else{
                succeed++;
            }
        }
        console.debug("====>the number of constraints obtained:" + succeed);
        console.debug("====>number of constraints not fetched:" + failed);
        if(failed == 0){
            await AccountManager.removeOsAccount(localId);
            console.debug("====>ActsOsAccountConstraints_2600 end====");
            done();
        }else{
            await AccountManager.removeOsAccount(localId);
            expect().assertFail();
            console.debug("====>ActsOsAccountConstraints_2600 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_2700
     * @tc.name    : Constraints callback
     * @tc.desc    : get all constrain information of non-existing local users
     */
    it('ActsOsAccountConstraints_2700', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_2700 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var nonExistentLocalId = 1000;
        AccountManager.getOsAccountAllConstraints(nonExistentLocalId, (err)=>{
            console.debug("====>getOsAccountAllConstraints err:" + JSON.stringify(err));
            expect(err.code != 0).assertTrue();
            console.debug("====>ActsOsAccountConstraints_2700 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_2800
     * @tc.name    : Constraints promise
     * @tc.desc    : get all constrain information of non-existing local users
     */
    it('ActsOsAccountConstraints_2800', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_2800 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var nonExistentLocalId = 1000;
        try{
            await AccountManager.getOsAccountAllConstraints(nonExistentLocalId);
        }
        catch(err){
            console.debug("====>getOsAccountAllConstraints:" + JSON.stringify(err));
            expect(err.code != 0).assertTrue();
            console.debug("====>ActsOsAccountConstraints_2800 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_2900
     * @tc.name    : Constraints callback
     * @tc.desc    : create a normal type local user,add all constraints and delete two constraints
     */
    it('ActsOsAccountConstraints_2900', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_2900 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var addallConstraints = ["constraint.wifi","constraint.wifi.set","constraint.locale.set",
        "constraint.app.accounts","constraint.apps.install","constraint.apps.uninstall","constraint.location.shared",
        "constraint.unknown.sources.install","constraint.global.unknown.app.install","constraint.bluetooth.set",
        "constraint.bluetooth","constraint.bluetooth.share","constraint.usb.file.transfer","constraint.credentials.set",
        "constraint.os.account.remove","constraint.managed.profile.remove","constraint.debug.features.use",
        "constraint.vpn.set","constraint.date.time.set","constraint.tethering.config","constraint.network.reset",
        "constraint.factory.reset","constraint.os.account.create","constraint.add.managed.profile",
        "constraint.apps.verify.disable","constraint.cell.broadcasts.set","constraint.mobile.networks.set",
        "constraint.control.apps","constraint.physical.media","constraint.microphone","constraint.microphone.unmute",
        "constraint.volume.adjust","constraint.calls.outgoing","constraint.sms.use","constraint.fun",
        "constraint.windows.create","constraint.system.error.dialogs","constraint.cross.profile.copy.paste",
        "constraint.beam.outgoing","constraint.wallpaper","constraint.safe.boot",
        "constraint.parent.profile.app.linking","constraint.audio.record","constraint.camera.use",
        "constraint.os.account.background.run","constraint.data.roam","constraint.os.account.set.icon",
        "constraint.wallpaper.set","constraint.oem.unlock","constraint.device.unmute","constraint.password.unified",
        "constraint.autofill","constraint.content.capture","constraint.content.suggestions",
        "constraint.os.account.start","constraint.location.set","constraint.airplane.mode.set",
        "constraint.brightness.set","constraint.share.into.profile","constraint.ambient.display",
        "constraint.screen.timeout.set","constraint.print","constraint.private.dns.set"];
        AccountManager.createOsAccount('NameConstraintsC', osAccount.OsAccountType.NORMAL, (err, data)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.localName).assertEqual("NameConstraintsC");
            localId = data.localId;
            AccountManager.setOsAccountConstraints(localId, addallConstraints, true,(err)=>{
                console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                AccountManager.getOsAccountAllConstraints(localId, (err, constraints)=>{
                    console.debug("====>getOsAccountAllConstraints err:" + JSON.stringify(err));
                    console.debug("====>getOsAccountAllConstraints:" + JSON.stringify(constraints));
                    expect(err.code).assertEqual(0);
                    expect(constraints.length).assertEqual(63);
                    AccountManager.setOsAccountConstraints(localId, ["constraint.print",
                    "constraint.private.dns.set"], false, (err)=>{
                        console.debug("====>getOsAccountAllConstraints err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        AccountManager.getOsAccountAllConstraints(localId, (err, constraints)=>{
                            console.debug("====>getOsAccountAllConstraints err:" + JSON.stringify(err));
                            console.debug("====>getOsAccountAllConstraints:" + JSON.stringify(constraints));
                            expect(err.code).assertEqual(0);
                            expect(constraints.length).assertEqual(61);
                            AccountManager.removeOsAccount(localId, (err)=>{
                                console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                                expect(err.code).assertEqual(0);
                                console.debug("====>ActsOsAccountConstraints_2900 end====");
                                done();
                            })
                        })
                    })
                })
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountConstraints_3000
     * @tc.name    : Constraints promise
     * @tc.desc    : create a normal type local user,add all constraints and delete two constraints
     */
    it('ActsOsAccountConstraints_3000', 0, async function(done){
        console.debug("====>ActsOsAccountConstraints_3000 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var addallConstraints = ["constraint.wifi","constraint.wifi.set","constraint.locale.set",
        "constraint.app.accounts","constraint.apps.install","constraint.apps.uninstall","constraint.location.shared",
        "constraint.unknown.sources.install","constraint.global.unknown.app.install","constraint.bluetooth.set",
        "constraint.bluetooth","constraint.bluetooth.share","constraint.usb.file.transfer","constraint.credentials.set",
        "constraint.os.account.remove","constraint.managed.profile.remove","constraint.debug.features.use",
        "constraint.vpn.set","constraint.date.time.set","constraint.tethering.config","constraint.network.reset",
        "constraint.factory.reset","constraint.os.account.create","constraint.add.managed.profile",
        "constraint.apps.verify.disable","constraint.cell.broadcasts.set","constraint.mobile.networks.set",
        "constraint.control.apps","constraint.physical.media","constraint.microphone","constraint.microphone.unmute",
        "constraint.volume.adjust","constraint.calls.outgoing","constraint.sms.use","constraint.fun",
        "constraint.windows.create","constraint.system.error.dialogs","constraint.cross.profile.copy.paste",
        "constraint.beam.outgoing","constraint.wallpaper","constraint.safe.boot",
        "constraint.parent.profile.app.linking","constraint.audio.record","constraint.camera.use",
        "constraint.os.account.background.run","constraint.data.roam","constraint.os.account.set.icon",
        "constraint.wallpaper.set","constraint.oem.unlock","constraint.device.unmute","constraint.password.unified",
        "constraint.autofill","constraint.content.capture","constraint.content.suggestions",
        "constraint.os.account.start","constraint.location.set","constraint.airplane.mode.set",
        "constraint.brightness.set","constraint.share.into.profile","constraint.ambient.display",
        "constraint.screen.timeout.set","constraint.print","constraint.private.dns.set"];
        var info = await AccountManager.createOsAccount("NameConstraintsP", osAccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("NameConstraintsP");
        localId = info.localId;
        await AccountManager.setOsAccountConstraints(localId, addallConstraints, true);
        console.debug("====>add allConstraints finish====");
        var constraints = await AccountManager.getOsAccountAllConstraints(localId);
        console.debug("====>getAccountManager constraints:" +JSON.stringify(constraints));
        expect(constraints.length).assertEqual(63);
        await AccountManager.setOsAccountConstraints(localId, ["constraint.print",
        "constraint.private.dns.set"], false);
        var constraints = await AccountManager.getOsAccountAllConstraints(localId);
        console.debug("====>getAccountManager constraints:" +JSON.stringify(constraints));
        expect(constraints.length).assertEqual(61);
        await AccountManager.removeOsAccount(localId);
        console.debug("====>ActsOsAccountConstraints_3000 end====");
        done();
    })

})