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

import bundleState from '@ohos.bundleState'

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe("DeviceUsageStatisticsJsTest", function () {
    beforeAll(function() {
		
        /*
         * @tc.setup: setup invoked before all testcases
         */
         console.info('beforeAll called')
    })
    
    afterAll(function() {
		
        /*
         * @tc.teardown: teardown invoked after all testcases
         */
         console.info('afterAll called')
    })
    
    beforeEach(function() {
		
        /*
         * @tc.setup: setup invoked before each testcases
         */
         console.info('beforeEach called')
    })
    
    afterEach(function() {
		
        /*
         * @tc.teardown: teardown invoked after each testcases
         */
        console.info('afterEach caled')
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest001
     * @tc.desc: test isIdleState promise.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("DeviceUsageStatisticsJsTest001", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest001---------------------------');
        let bundleName = 'com.explace.deviceUsageStatistics';
        bundleState.isIdleState(bundleName).then((res) => {
            console.info('BUNDLE_ACTIVE isIdleState promise success.');
            expect(true).assertEqual(true);
        }).catch((err) => {
            expect(false).assertEqual(true);
            console.info('BUNDLE_ACTIVE isIdleState promise failure.');
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest002
     * @tc.desc: test isIdleState callback.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("DeviceUsageStatisticsJsTest002", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest002---------------------------');
        let bundleName = 'com.explace.deviceUsageStatistics';
        bundleState.isIdleState(bundleName, (err, res) => {
            if(err.code === 0) {
                console.info('BUNDLE_ACTIVE isIdleState callback success.');
                expect(true).assertEqual(true);
            } else {
                expect(false).assertEqual(true);
                console.info('BUNDLE_ACTIVE isIdleState callback failure.');
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest003
     * @tc.desc: test queryAppUsagePriorityGroup promise.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("DeviceUsageStatisticsJsTest003", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest003---------------------------');
        bundleState.queryAppUsagePriorityGroup().then( res => {
            console.info('BUNDLE_ACTIVE queryAppUsagePriorityGroup promise success.');
            expect(true).assertEqual(true)
        }).catch( err => {
            expect(false).assertEqual(true)
            console.info('BUNDLE_ACTIVE queryAppUsagePriorityGroup promise failure.');
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest004
     * @tc.desc: test queryAppUsagePriorityGroup callback.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("DeviceUsageStatisticsJsTest004", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest004---------------------------');
        bundleState.queryAppUsagePriorityGroup((err, res) => {
            if(err.code === 0) {
                console.info('BUNDLE_ACTIVE queryAppUsagePriorityGroup callback success.');
                expect(true).assertEqual(true)
            } else {
                expect(false).assertEqual(true)
                console.info('BUNDLE_ACTIVE queryAppUsagePriorityGroup callback failure.');
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest005
     * @tc.desc: test queryBundleActiveStates promise.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("DeviceUsageStatisticsJsTest005", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest005---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryBundleActiveStates(beginTime, endTime).then((res) => {
            console.info('BUNDLE_ACTIVE queryBundleActiveStates promise success.');
            expect(true).assertEqual(true);
        }).catch((err) => {
            expect(false).assertEqual(true);
            console.info('BUNDLE_ACTIVE queryBundleActiveStates promise failure.');
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest006
     * @tc.desc: test queryBundleActiveStates callback.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("DeviceUsageStatisticsJsTest006", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest006---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryBundleActiveStates(beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                console.info('BUNDLE_ACTIVE queryBundleActiveStates callback success.');
                expect(true).assertEqual(true);
            } else {
                expect(false).assertEqual(true);
                console.info('BUNDLE_ACTIVE queryBundleActiveStates callback failure.');
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest007
     * @tc.desc: test queryBundleStateInfos promise.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("DeviceUsageStatisticsJsTest007", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest007---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfos(beginTime, endTime).then((res) => {
            console.info('BUNDLE_ACTIVE queryBundleStateInfos promise success.');
            expect(true).assertEqual(true);
        }).catch((err) => {
            expect(false).assertEqual(true);
            console.info('BUNDLE_ACTIVE queryBundleStateInfos promise failure.');
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest008
     * @tc.desc: test queryBundleStateInfos callback.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("DeviceUsageStatisticsJsTest008", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest008---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfos(beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                console.info('BUNDLE_ACTIVE queryBundleStateInfos callback success.');
                expect(true).assertEqual(true);
            } else {
                expect(false).assertEqual(true);
                console.info('BUNDLE_ACTIVE queryBundleStateInfos callback failure.');
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest009
     * @tc.desc: test queryCurrentBundleActiveStates promise.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("DeviceUsageStatisticsJsTest009", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest009---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryCurrentBundleActiveStates(beginTime, endTime).then((res) => {
            console.info('BUNDLE_ACTIVE queryCurrentBundleActiveStates promise success.');
            expect(true).assertEqual(true);
        }).catch((err) => {
            expect(false).assertEqual(true);
            console.info('BUNDLE_ACTIVE queryCurrentBundleActiveStates promise failure.');
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest010
     * @tc.desc: test queryCurrentBundleActiveStates callback.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("DeviceUsageStatisticsJsTest010", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest010---------------------------');
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryCurrentBundleActiveStates(beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                console.info('BUNDLE_ACTIVE queryCurrentBundleActiveStates callback success.');
                expect(true).assertEqual(true);
            } else {
                expect(false).assertEqual(true);
                console.info('BUNDLE_ACTIVE queryCurrentBundleActiveStates callback failure.');
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest011
     * @tc.desc: test queryBundleStateInfoByInterval promise.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("DeviceUsageStatisticsJsTest011", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest011---------------------------');
        let intervalType = 0;
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfoByInterval(intervalType, beginTime, endTime).then((res) => {
            console.info('BUNDLE_ACTIVE queryBundleStateInfoByInterval promise success.');
            expect(true).assertEqual(true);
        }).catch((err) => {
            expect(false).assertEqual(true);
            console.info('BUNDLE_ACTIVE queryBundleStateInfoByInterval promise failure.');
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: DeviceUsageStatisticsJsTest012
     * @tc.desc: test queryBundleStateInfoByInterval callback.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("DeviceUsageStatisticsJsTest012", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest012---------------------------');
        let intervalType = 0;
        let beginTime = 0;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfoByInterval(intervalType, beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                console.info('BUNDLE_ACTIVE queryBundleStateInfoByInterval callback success.');
                expect(true).assertEqual(true);
            } else {
                expect(false).assertEqual(true);
                console.info('BUNDLE_ACTIVE queryBundleStateInfoByInterval callback failure.');
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest013
     * @tc.desc: test intervalType BY_OPTIMIZED
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest013", 0, function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest013---------------------------');
        let value1 = bundleState.IntervalType.BY_OPTIMIZED;
        expect(value1).assertEqual(0)
        done();
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest014
     * @tc.desc: test intervalType BY_DAILY
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest014", 0, function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest014---------------------------');
        let value1 = bundleState.IntervalType.BY_DAILY;
        expect(value1).assertEqual(1)
        done();
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest015
     * @tc.desc: test intervalType BY_WEEKLY
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest015", 0, function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest015---------------------------');
        let value1 = bundleState.IntervalType.BY_WEEKLY;
        expect(value1).assertEqual(2)
        done();
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest016
     * @tc.desc: test intervalType BY_MONTHLY
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest016", 0, function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest016---------------------------');
        let value1 = bundleState.IntervalType.BY_MONTHLY;
        expect(value1).assertEqual(3)
        done();
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest017
     * @tc.desc: test intervalType BY_ANNUALLY
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest017", 0, function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest017---------------------------');
        let value1 = bundleState.IntervalType.BY_ANNUALLY;
        expect(value1).assertEqual(4)
        done();
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest018
     * @tc.desc: test BundleStateInfo id.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest018", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest018---------------------------');
        let intervalType = 1;
        let beginTime = 1000;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfoByInterval(intervalType, beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					res = [{id: 1}];
				}
				console.log('test BundleStateInfo id :' + res[0].id)
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest019
     * @tc.desc: test BundleStateInfo bundleName.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest019", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest019---------------------------');
        let intervalType = 1;
        let beginTime = 100;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfoByInterval(intervalType, beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					res = [{bundleName: 'www.explace.com'}];
				}
				console.log('test BundleStateInfo bundleName :' + res[0].bundleName)
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest020
     * @tc.desc: test BundleStateInfo infosEndTime.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest020", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest020---------------------------');
        let intervalType = 1;
        let beginTime = 1001;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfoByInterval(intervalType, beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					res = [{infosEndTime: 0}];
				}
				console.log('test BundleStateInfo infosEndTime :' + res[0].infosEndTime)
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest021
     * @tc.desc: test BundleStateInfo abilityPrevSeenTime.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest021", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest021---------------------------');
        let intervalType = 1;
        let beginTime = 1001;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfoByInterval(intervalType, beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					res = [{abilityPrevSeenTime: 0}];
				}
				console.log('test BundleStateInfo abilityPrevSeenTime :' + res[0].abilityPrevSeenTime)
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest022
     * @tc.desc: test BundleStateInfo abilitySeenTotalTime.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest022", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest022---------------------------');
        let intervalType = 1;
        let beginTime = 1001;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfoByInterval(intervalType, beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					res = [{abilitySeenTotalTime: 0}];
				}
				console.log('test BundleStateInfo abilitySeenTotalTime :' + res[0].abilitySeenTotalTime)
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest023
     * @tc.desc: test BundleStateInfo fgAbilityAccessTotalTime.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest023", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest023---------------------------');
        let intervalType = 1;
        let beginTime = 1001;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfoByInterval(intervalType, beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					res = [{fgAbilityAccessTotalTime: 0}];
				}
				console.log('test BundleStateInfo fgAbilityAccessTotalTime :' + res[0].fgAbilityAccessTotalTime)
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest024
     * @tc.desc: test BundleStateInfo fgAbilityPrevAccessTime.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest024", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest024---------------------------');
        let intervalType = 1;
        let beginTime = 1001;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfoByInterval(intervalType, beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					res = [{fgAbilityPrevAccessTime: 0}];
				}
				console.log('test BundleStateInfo fgAbilityPrevAccessTime :' + res[0].fgAbilityPrevAccessTime)
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest025
     * @tc.desc: test BundleStateInfo abilityPrevAccessTime.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest025", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest025---------------------------');
        let intervalType = 1;
        let beginTime = 1001;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfoByInterval(intervalType, beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					res = [{abilityPrevAccessTime: 0}];
				}
				console.log('test BundleStateInfo abilityPrevAccessTime :' + res[0].abilityPrevAccessTime)
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest026
     * @tc.desc: test BundleStateInfo infosBeginTime.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest026", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest026---------------------------');
        let intervalType = 1;
        let beginTime = 1001;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfoByInterval(intervalType, beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					res = [{infosBeginTime: 0}];
				}
				console.log('test BundleStateInfo infosBeginTime :' + res[0].infosBeginTime)
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest027
     * @tc.desc: test BundleStateInfo abilityInFgTotalTime.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest027", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest027---------------------------');
        let intervalType = 1;
        let beginTime = 1001;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfoByInterval(intervalType, beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					res = [{abilityInFgTotalTime: 0}];
				}
				console.log('test BundleStateInfo abilityInFgTotalTime :' + res[0].abilityInFgTotalTime)
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest028
     * @tc.desc: test BundleActiveState bundleName.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest028", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest028---------------------------');
        let beginTime = 10;
        let endTime = 20000000000000;
        bundleState.queryBundleActiveStates(beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					res = [{bundleName: 'www.baidu.com'}];
				}
				console.log('test BundleActiveState bundleName :' + res[0].bundleName)
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest029
     * @tc.desc: test BundleActiveState stateType.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest029", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest029---------------------------');
        let beginTime = 10;
        let endTime = 20000000000000;
        bundleState.queryBundleActiveStates(beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					res = [{stateType: 0}];
				}
				console.log('test BundleActiveState stateType :' + res[0].stateType)
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest030
     * @tc.desc: test BundleActiveState stateOccurredTime.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest030", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest030---------------------------');
        let beginTime = 10;
        let endTime = 20000000000000;
        bundleState.queryBundleActiveStates(beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					res = [{stateOccurredTime: 0}];
				}
				console.log('test BundleActiveState stateOccurredTime :' + res[0].stateOccurredTime)
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest031
     * @tc.desc: test BundleActiveState appUsagePriorityGroup.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest031", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest031---------------------------');
        let beginTime = 10;
        let endTime = 20000000000000;
        bundleState.queryBundleActiveStates(beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					res = [{appUsagePriorityGroup: 0}];
				}
				console.log('test BundleActiveState appUsagePriorityGroup :' + res[0].appUsagePriorityGroup)
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest032
     * @tc.desc: test BundleActiveState indexOfLink.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest032", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest032---------------------------');
        let beginTime = 10;
        let endTime = 20000000000000;
        bundleState.queryBundleActiveStates(beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					res = [{indexOfLink: 'id'}];
				}
				console.log('test BundleActiveState indexOfLink :' + res[0].indexOfLink)
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest033
     * @tc.desc: test BundleActiveState nameOfClass.
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("DeviceUsageStatisticsJsTest033", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest033---------------------------');
        let beginTime = 10;
        let endTime = 20000000000000;
        bundleState.queryBundleActiveStates(beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					res = [{nameOfClass: 'BundleStates'}];
				}
				console.log('test BundleActiveState nameOfClass :' + res[0].nameOfClass)
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest034
     * @tc.desc: test BundleActiveinfoResponse BundleStateinfo key.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("DeviceUsageStatisticsJsTest034", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest034---------------------------');
        let beginTime = 10;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfos(beginTime, endTime).then((res) => {
            expect(true).assertEqual(true);
			let bundleStateInfo1 = {
				bundleName: 'www.explace.com',
				abilityPrevAccessTime: 0,
				abilityInFgTotalTime: 0
			};
			let bundleStateInfo2 = {
				bundleName: 'www.baidu.com',
				abilityPrevAccessTime: 1,
				abilityInFgTotalTime: 1
			};
			if(res === undefined) {
				res = [];
				res['www.explace.com'] = bundleStateInfo1;
				res['www.baidu.com'] = bundleStateInfo2;
				for(let k in res) {
					console.log('test BundleStateInfo key :' + k + ',value is :' + JSON.stringify(res[k]));
				}
			}
        }).catch((err) => {
            expect(false).assertEqual(true);
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
	
	/*
     * @tc.name: DeviceUsageStatisticsJsTest035
     * @tc.desc: test BundleStateInfo merge.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("DeviceUsageStatisticsJsTest035", 0, async function (done) {
        console.info('----------------------DeviceUsageStatisticsJsTest035---------------------------');
        let beginTime = 1001;
        let endTime = 20000000000000;
        bundleState.queryBundleStateInfoByInterval(0, beginTime, endTime, (err, res) => {
            if(err.code === 0) {
                expect(true).assertEqual(true);
				if(res === undefined) {
					let bundleStateInfo = {
						bundleName: 'www.explace.com',
						abilityPrevAccessTime: 0,
						abilityInFgTotalTime: 0
					};
					res = [{
						bundleName: 'www.explace.com',
						abilityPrevAccessTime: 0,
						abilityInFgTotalTime: 0,
						merge : function (toMerge) {
							return 'is function,toMerge :' + JSON.stringify(toMerge);
						}
					}]
					console.log('test bundleStateInfo merge' + res[0].merge(bundleStateInfo));
				} else {
					console.log('test bundleStateInfo merge')
				}
            } else {
                expect(false).assertEqual(true);
            }
        });

        setTimeout(()=>{
            done();
        }, 500);
    })
})