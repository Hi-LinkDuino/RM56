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
import workScheduler from '@ohos.workScheduler'
import workSchedulerCallback from '@ohos.workSchedulerExtensionAbility'

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe("WorkSchedulerJsTest", function () {
    beforeAll(function() {
       
        /*
         * @tc.setup: setup invoked before all testcases
         */
         console.info('beforeAll caled')
    })

    afterAll(function() {
        
        /*
         * @tc.teardown: teardown invoked after all testcases
         */
         console.info('afterAll caled')
    })

    beforeEach(function() {
       
        /*
         * @tc.setup: setup invoked before each testcases
         */
         console.info('beforeEach caled')
    })

    afterEach(function() {
        
        /*
         * @tc.teardown: teardown invoked after each testcases
         */
         console.info('afterEach caled')
    })

    /*
     * @tc.name: WorkSchedulerJsTest001
     * @tc.desc: test work scheduler work id < 0
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest001", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest001---------------------------');
        let workInfo = {
            workId: -1,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName"
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })

    /*
     * @tc.name: WorkSchedulerJsTest002
     * @tc.desc: test work scheduler work id = 0
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest002", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest002---------------------------');
        let workInfo = {
            workId: 0,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
            storageRequest: workScheduler.StorageRequest.STORAGE_LEVEL_OKAY
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(true)
        done();
    })

    /*
     * @tc.name: WorkSchedulerJsTest003
     * @tc.desc: test work scheduler without bundle name.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest003", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest003---------------------------');
        let workInfo = {
            workId: 3,
            abilityName: "com.mytest.abilityName",
            storageRequest: workScheduler.StorageRequest.STORAGE_LEVEL_OKAY
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })

    /*
     * @tc.name: WorkSchedulerJsTest004
     * @tc.desc: test work scheduler without ability name.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest004", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest004---------------------------');
        let workInfo = {
            workId: 4,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            storageRequest: workScheduler.StorageRequest.STORAGE_LEVEL_OKAY
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })

    /*
     * @tc.name: WorkSchedulerJsTest005
     * @tc.desc: test work scheduler without conditions.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest005", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest005---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName"
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })

    /*
     * @tc.name: WorkSchedulerJsTest006
     * @tc.desc: test work scheduler success.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest006", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest006---------------------------');
        let workInfo = {
            workId: 6,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
            storageRequest: workScheduler.StorageRequest.STORAGE_LEVEL_OKAY
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(true)
        done();
    })

    /*
     * @tc.name: WorkSchedulerJsTest007
     * @tc.desc: test stopWork.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest007", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest007---------------------------');
        let workInfo = {
            workId: 7,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
            storageRequest: workScheduler.StorageRequest.STORAGE_LEVEL_OKAY
        }
        let res = workScheduler.startWork(workInfo);
        if (res == true) {
            let stopRes = workScheduler.stopWork(workInfo, false);
            expect(stopRes).assertEqual(true)
        } else {
            expect(false).assertEqual(true)
        }
        done();
    })

    /*
     * @tc.name: WorkSchedulerJsTest008
     * @tc.desc: test stopWork.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest008", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest008---------------------------');
        let workInfo = {
            workId: 8,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
            storageRequest: workScheduler.StorageRequest.STORAGE_LEVEL_OKAY
        }
        let res = workScheduler.startWork(workInfo);
        if (res == true) {
            let stopRes = workScheduler.stopWork(workInfo, false);
            expect(stopRes).assertEqual(true)
        } else {
            expect(false).assertEqual(true)
        }
        done();
    })

    /*
     * @tc.name: WorkSchedulerJsTest009
     * @tc.desc: test getWorkStatus callback.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest009", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest009---------------------------');
        let workInfo = {
            workId: 9,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
            storageRequest: workScheduler.StorageRequest.STORAGE_LEVEL_OKAY
        }
        let res = workScheduler.startWork(workInfo);
        if (res == false) {
            expect(false).assertEqual(true)
            done();
        }
        workScheduler.getWorkStatus(9, (err, res) => {
            if (err) {
                expect(false).assertEqual(true)
            } else {
                for (let item in res) {
                    console.info('WORK_SCHEDULER getWorkStatuscallback success,' + item + ' is:' + res[item]);
                }
                expect(true).assertEqual(true)
            }
        });
        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: WorkSchedulerJsTest010
     * @tc.desc: test getWorkStatus promise.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest010", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest010---------------------------');
        let workInfo = {
            workId: 10,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
            storageRequest: workScheduler.StorageRequest.STORAGE_LEVEL_OKAY,
            batteryLevel: 15
        }
        let res = workScheduler.startWork(workInfo);
        if (res == false) {
            expect(false).assertEqual(true)
            done();
        }

        workScheduler.getWorkStatus(10).then((res) => {
            for (let item in res) {
                console.info('WORK_SCHEDULER getWorkStatuscallback success,' + item + ' is:' + res[item]);
            }
            expect(true).assertEqual(true)
        }).catch((err) => {
            expect(false).assertEqual(true)
        })

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: WorkSchedulerJsTest011
     * @tc.desc: test obtainAllWorks callback.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest011", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest011---------------------------');
        let workInfo = {
            workId: 11,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
            storageRequest: workScheduler.StorageRequest.STORAGE_LEVEL_OKAY
        }
        let res = workScheduler.startWork(workInfo);
        if (res == false) {
            expect(false).assertEqual(true)
            done();
        }

        workScheduler.obtainAllWorks((err, res) =>{
            if (err) {
                expect(false).assertEqual(true)
            } else {
                console.info('WORK_SCHEDULER obtainAllWorks callback success, data is:' + JSON.stringify(res));
                expect(true).assertEqual(true)
            }
         });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: WorkSchedulerJsTest012
     * @tc.desc: test obtainAllWorks promise.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest012", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest012---------------------------');
        let workInfo = {
            workId: 12,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
            storageRequest: workScheduler.StorageRequest.STORAGE_LEVEL_OKAY,
            batteryLevel: 15
        }
        let res = workScheduler.startWork(workInfo);
        if (res == false) {
            expect(false).assertEqual(true)
            done();
        }

        workScheduler.obtainAllWorks().then((res) => {
            console.info('WORK_SCHEDULER obtainAllWorks promise success, data is:' + JSON.stringify(res));
            expect(true).assertEqual(true)
        }).catch((err) => {
            console.info('workschedulerLog obtainAllWorks promise failed, because:' + err.data);
            expect(false).assertEqual(true)
        })

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: WorkSchedulerJsTest013
     * @tc.desc: test stopAndClearWorks.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest013", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest013---------------------------');
        let workInfo = {
            workId: 13,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
            storageRequest: workScheduler.StorageRequest.STORAGE_LEVEL_OKAY
        }
        let res = workScheduler.startWork(workInfo);
        if (res == true) {
            let stopRes = workScheduler.stopAndClearWorks();
            expect(stopRes).assertEqual(true)
        } else {
            expect(false).assertEqual(true)
        }
        done();
    })

    /*
     * @tc.name: WorkSchedulerJsTest014
     * @tc.desc: test isLastWorkTimeOut callback.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest014", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest014---------------------------');
        workScheduler.isLastWorkTimeOut(14, (err, res) =>{
            if (err) {
                expect(false).assertEqual(true)
            } else {
                console.info('WORK_SCHEDULER isLastWorkTimeOut callback success, data is:' + res);
                expect(true).assertEqual(true)
            }
        });
        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: WorkSchedulerJsTest015
     * @tc.desc: test isLastWorkTimeOut promise.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest015", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest015---------------------------');
        workScheduler.isLastWorkTimeOut(15)
            .then(res => {
                console.info('WORK_SCHEDULER isLastWorkTimeOut promise success, data is:' + res);
                expect(true).assertEqual(true)
            })
            .catch(err =>  {
                expect(false).assertEqual(true)
        });
        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: WorkSchedulerJsTest016
     * @tc.desc: test work scheduler constant
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest016", 0, function (done) {
        console.info('----------------------WorkSchedulerJsTest016---------------------------');
        let value1 = workScheduler.NetworkType.NETWORK_TYPE_ANY;
        expect(value1).assertEqual(0)
        done();
    })

    /*
     * @tc.name: WorkSchedulerJsTest017
     * @tc.desc: test work scheduler constant
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest017", 0, function (done) {
        console.info('----------------------WorkSchedulerJsTest017---------------------------');
        let value1 = workScheduler.NetworkType.NETWORK_TYPE_MOBILE;
        expect(value1).assertEqual(1)
        done();
    })

    /*
     * @tc.name: WorkSchedulerJsTest018
     * @tc.desc: test work scheduler constant
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest018", 0, function (done) {
        console.info('----------------------WorkSchedulerJsTest018---------------------------');
        let value1 = workScheduler.NetworkType.NETWORK_TYPE_WIFI;
        expect(value1).assertEqual(2)
        done();
    })

    /*
     * @tc.name: WorkSchedulerJsTest019
     * @tc.desc: test work scheduler constant
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest019", 0, function (done) {
        console.info('----------------------WorkSchedulerJsTest019---------------------------');
        let value1 = workScheduler.NetworkType.NETWORK_TYPE_BLUETOOTH;
        expect(value1).assertEqual(3)
        done();
    })

    /*
     * @tc.name: WorkSchedulerJsTest020
     * @tc.desc: test work scheduler constant
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest020", 0, function (done) {
        console.info('----------------------WorkSchedulerJsTest020---------------------------');
        let value1 = workScheduler.NetworkType.NETWORK_TYPE_WIFI_P2P;
        expect(value1).assertEqual(4)
        done();
    })

    /*
     * @tc.name: WorkSchedulerJsTest021
     * @tc.desc: test work scheduler constant
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest021", 0, function (done) {
        console.info('----------------------WorkSchedulerJsTest021---------------------------');
        let value1 = workScheduler.NetworkType.NETWORK_TYPE_ETHERNET;
        expect(value1).assertEqual(5)
        done();
    })

   /*
     * @tc.name: WorkSchedulerJsTest022
     * @tc.desc: test work scheduler constant
     * @tc.type: FUNC
     * @tc.require: 
     */
   it("WorkSchedulerJsTest022", 0, function (done) {
    console.info('----------------------WorkSchedulerJsTest022---------------------------');
    let value1 = workScheduler.ChargingType.CHARGING_PLUGGED_ANY;
    expect(value1).assertEqual(0)
    done();
    })

    /*
    * @tc.name: WorkSchedulerJsTest023
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("WorkSchedulerJsTest023", 0, function (done) {
        console.info('----------------------WorkSchedulerJsTest023---------------------------');
        let value1 = workScheduler.ChargingType.CHARGING_PLUGGED_AC;
        expect(value1).assertEqual(1)
        done();
    })

    /*
    * @tc.name: WorkSchedulerJsTest024
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("WorkSchedulerJsTest024", 0, function (done) {
        console.info('----------------------WorkSchedulerJsTest024---------------------------');
        let value1 = workScheduler.ChargingType.CHARGING_PLUGGED_USB;
        expect(value1).assertEqual(2)
        done();
    })

    /*
    * @tc.name: WorkSchedulerJsTest025
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("WorkSchedulerJsTest025", 0, function (done) {
        console.info('----------------------WorkSchedulerJsTest025---------------------------');
        let value1 = workScheduler.ChargingType.CHARGING_PLUGGED_WIRELESS;
        expect(value1).assertEqual(3)
        done();
    })

    /*
    * @tc.name: WorkSchedulerJsTest026
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("WorkSchedulerJsTest026", 0, function (done) {
        console.info('----------------------WorkSchedulerJsTest026---------------------------');
        let value1 = workScheduler.BatteryStatus.BATTERY_STATUS_LOW;
        expect(value1).assertEqual(0)
        done();
    })

    /*
    * @tc.name: WorkSchedulerJsTest027
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("WorkSchedulerJsTest027", 0, function (done) {
        console.info('----------------------WorkSchedulerJsTest027---------------------------');
        let value1 = workScheduler.BatteryStatus.BATTERY_STATUS_OKAY;
        expect(value1).assertEqual(1)
        done();
    })

    /*
    * @tc.name: WorkSchedulerJsTest028
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("WorkSchedulerJsTest028", 0, function (done) {
        console.info('----------------------WorkSchedulerJsTest028---------------------------');
        let value1 = workScheduler.BatteryStatus.BATTERY_STATUS_LOW_OR_OKAY;
        expect(value1).assertEqual(2)
        done();
    })

    /*
    * @tc.name: WorkSchedulerJsTest029
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("WorkSchedulerJsTest029", 0, function (done) {
        console.info('----------------------WorkSchedulerJsTest029---------------------------');
        let value1 = workScheduler.StorageRequest.STORAGE_LEVEL_LOW;
        expect(value1).assertEqual(0)
        done();
    })

    /*
    * @tc.name: WorkSchedulerJsTest030
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("WorkSchedulerJsTest030", 0, function (done) {
        console.info('----------------------WorkSchedulerJsTest030---------------------------');
        let value1 = workScheduler.StorageRequest.STORAGE_LEVEL_OKAY;
        expect(value1).assertEqual(1)
        done();
    })

    /*
    * @tc.name: WorkSchedulerJsTest031
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("WorkSchedulerJsTest031", 0, function (done) {
        console.info('----------------------WorkSchedulerJsTest031---------------------------');
        let value1 = workScheduler.StorageRequest.STORAGE_LEVEL_LOW_OR_OKAY;
        expect(value1).assertEqual(2)
        done();
    })
	
    /*
     * @tc.name: WorkSchedulerJsTest032
     * @tc.desc: test work scheduler without conditions.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest032", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest032---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			isPersisted: true
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })
	
	 /*
     * @tc.name: WorkSchedulerJsTest033
     * @tc.desc: test work scheduler without conditions.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest033", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest033---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			isPersisted: false
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })
	
	/*
     * @tc.name: WorkSchedulerJsTest034
     * @tc.desc: test work scheduler without conditions.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest034", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest034---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			isCharging: true
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(true)
        done();
    })
	
	 /*
     * @tc.name: WorkSchedulerJsTest035
     * @tc.desc: test work scheduler without conditions.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest035", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest035---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			isCharging: false
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })
	
	/*
     * @tc.name: WorkSchedulerJsTest036
     * @tc.desc: test work scheduler without conditions.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest036", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest036---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			isRepeat: true
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })
	
	 /*
     * @tc.name: WorkSchedulerJsTest037
     * @tc.desc: test work scheduler without conditions.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest037", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest037---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			isRepeat: false
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })
	
	/*
     * @tc.name: WorkSchedulerJsTest038
     * @tc.desc: test work scheduler without conditions.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest038", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest038---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			isDeepIdle: true
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })
	
	 /*
     * @tc.name: WorkSchedulerJsTest039
     * @tc.desc: test work scheduler without conditions.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest039", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest039---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			isDeepIdle: false
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })
	
	 /*
     * @tc.name: WorkSchedulerJsTest040
     * @tc.desc: test work scheduler without conditions.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest040", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest040---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			batteryLevel: 20
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })
	
	 /*
     * @tc.name: WorkSchedulerJsTest041
     * @tc.desc: test work scheduler without conditions.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest041", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest041---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			batteryLevel: 50
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })
	
	 /*
     * @tc.name: WorkSchedulerJsTest042
     * @tc.desc: test work scheduler without conditions.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest042", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest042---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			batteryLevel: 80
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })
	
	 /*
     * @tc.name: WorkSchedulerJsTest043
     * @tc.desc: test work scheduler without conditions.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest043", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest043---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			isRepeat: true,
			repeatCycleTime: 1800000,
			repeatCount: 2
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })
	
	 /*
     * @tc.name: WorkSchedulerJsTest044
     * @tc.desc: test work scheduler without conditions.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest044", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest044---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			isRepeat: false,
			repeatCycleTime: 2400000,
			repeatCount: 3
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })
	
	 /*
     * @tc.name: WorkSchedulerJsTest045
     * @tc.desc: test work scheduler without conditions.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest045", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest045---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			idleWaitTime: 3000,
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false)
        done();
    })
	
	/*
     * @tc.name: WorkSchedulerJsTest046
     * @tc.desc: test workScheduler workInfo networkType
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest046", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest046---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			networkType: workScheduler.NetworkType.NETWORK_TYPE_ANY
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false);
		console.info("workSchedulerLog workInfo networkType :" + workInfo.networkType);
        done();
    })
	
	/*
     * @tc.name: WorkSchedulerJsTest047
     * @tc.desc: test workScheduler workInfo ChargingType
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest047", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest047---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			chargerType: workScheduler.ChargingType.CHARGING_PLUGGED_AC
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false);
		console.info("workSchedulerLog workInfo ChargingType :" + workScheduler.ChargingType.CHARGING_PLUGGED_AC);
        done();
    })
	
	/*
     * @tc.name: WorkSchedulerJsTest048
     * @tc.desc: test workScheduler workInfo BatteryStatus
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest048", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest048---------------------------');
        let workInfo = {
            workId: 5,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
			batteryStatus: workScheduler.BatteryStatus.BATTERY_STATUS_LOW
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false);
		console.info("workSchedulerLog workInfo BatteryStatus :" + workScheduler.BatteryStatus.BATTERY_STATUS_LOW);
        done();
    })
	
	/*
     * @tc.name: WorkSchedulerJsTest049
     * @tc.desc: test WorkSchedulerExtensoinAbility onWorkStart
     * @tc.type: FUNC
     * @tc.require: 
     */
    /*it("WorkSchedulerJsTest049", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest049---------------------------');
        let workInfo = {
            workId: 2,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
        }
        let res = workScheduler.onWorkStart(workInfo);
        expect(res).assertEqual(false);
        done();
    })*/
	
	/*
     * @tc.name: WorkSchedulerJsTest050
     * @tc.desc: test WorkSchedulerExtensoinAbility onWorkStop
     * @tc.type: FUNC
     * @tc.require: 
     */
    /*it("WorkSchedulerJsTest050", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest050---------------------------');
        let workInfo = {
            workId: 2,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
        }
        let stopRes = workSchedulerCallback.onWorkStop(workInfo, true);
			expect(stopRes).assertEqual(false);
        done();
    })*/
	
	/*
     * @tc.name: WorkSchedulerJsTest051
     * @tc.desc: test workScheduler bundleName
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest051", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest051---------------------------');
        let workInfo = {
            workId: 4,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false);
		console.log("workScheduler workInfo bundleName :" + workInfo.bundleName);
        done();
    })
	
	/*
     * @tc.name: WorkSchedulerJsTest052
     * @tc.desc: test workScheduler workInfo abilityName
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("WorkSchedulerJsTest052", 0, async function (done) {
        console.info('----------------------WorkSchedulerJsTest052---------------------------');
        let workInfo = {
            workId: 8,
            bundleName: "ohos.acts.resourceschedule.workscheduler.js.function",
            abilityName: "com.mytest.abilityName",
        }
        let res = workScheduler.startWork(workInfo);
        expect(res).assertEqual(false);
		console.log("test workScheduler workInfo abilityName :" + workInfo.abilityName);
        done();
    })
})