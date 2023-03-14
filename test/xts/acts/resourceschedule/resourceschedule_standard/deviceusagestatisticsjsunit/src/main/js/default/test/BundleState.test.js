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

import bundlestate from '@ohos.bundlestate';
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

function onInit() {
	let obj = {startTime: 0,endTime: 0};
	const myTime = new Date().getTime();
	obj.startTime = myTime - 86400000;
	obj.endTime = myTime
	return obj;
}
let bundleName = 'com.example.backgroundTaskManager';
const byInterval = bundlestate.IntervalType.BY_DAILY;
const toMerge = {
	"abilityInFgTotalTime": 0,
	"abilityPrevAccessTime": 1629907200000,
	"abilityPrevSeenTime": 0,
	"abilitySeenTotalTime": 0,
	"bundleName": "com.example.backgroundTaskManager",
	"fgAbilityAccessTotalTime": 0,
	"fgAbilityPrevAccessTime": 0,
	"id": 0,
	"infosBeginTime": 1629907200000,
	"infosEndTime": 1629993599999
}
describe('bundlestate', function() {
	console.info('Start test tvmw bundlestate######################');
	beforeAll(function() {
		console.info('beforeAll')
	})
	
	afterAll(function() {
		console.info('afterAll begin')
	})
	
	beforeEach(function() {
		console.info('beforeEach caled')
	})
	
	afterEach(function() {
		console.info('afterEach caled')
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0001
     * @tc.name: queryAppUsagePriorityGroup_001.
     * @tc.desc: Execute queryAppUsagePriorityGroup with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryAppUsagePriorityGroup_001', 0, async function(done) {
		console.info('queryAppUsagePriorityGroup_001 start');
		bundlestate.queryAppUsagePriorityGroup((err, data) => {
			if(err) {
				console.info('queryAppUsagePriorityGroup_001 callback fail result = ' + JSON.stringify(err.code));
				expect().assertFail();
			} else {
				console.info('queryAppUsagePriorityGroup_001 callback success result = ' + JSON.stringify(data));
				expect(true).assertEqual(true)
			}
			done();
		});
		console.info('queryAppUsagePriorityGroup_001 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0002
     * @tc.name: queryAppUsagePriorityGroup_002.
     * @tc.desc: Execute queryAppUsagePriorityGroup with Promise.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryAppUsagePriorityGroup_002', 0, async function(done) {
		console.info('queryAppUsagePriorityGroup_002 start');
		bundlestate.queryAppUsagePriorityGroup().then((data) => {
			console.info('queryAppUsagePriorityGroup_002 callback success result = ' + JSON.stringify(data));
			expect(true).assertEqual(true);
			done();
		}).catch((err) => {
			console.info('queryAppUsagePriorityGroup_002 callback fail result = ' + JSON.stringify(err.code));
			expect().assertFail();
		})
		console.info('queryAppUsagePriorityGroup_002 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0003
     * @tc.name: queryBundleStateInfos_003.
     * @tc.desc: Execute queryBundleStateInfos with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfos_003', 0, function(done) {
		console.info('queryBundleStateInfos_003 start');
		let timer = onInit();
		bundlestate.queryBundleStateInfos(timer.startTime,timer.endTime, (err, data) => {
			if(err) {
				console.info('queryBundleStateInfos_003 callback fail result = ' + JSON.stringify(err));
				expect().assertFail();
			} else {
				console.info('queryBundleStateInfos_003 callback success result = ' + JSON.stringify(data));
				expect(data).assertInstanceOf('Object');
			}
			done();
		});
		console.info('queryBundleStateInfos_003 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0004
     * @tc.name: queryBundleStateInfos_004.
     * @tc.desc: Execute queryBundleStateInfos with promise.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfos_004', 0, async function(done) {
		console.info('queryBundleStateInfos_004 start');
		let timer = onInit();
		bundlestate.queryBundleStateInfos(timer.startTime,timer.endTime).then((res) => {
			console.info('queryBundleStateInfos_004 promise success result = ' + JSON.stringify(res));
			expect(res).assertInstanceOf('Object');
			done();
		}).catch((err) => {
			console.info('queryBundleStateInfos_004 promise fail result = ' + JSON.stringify(err));
			expect().assertFail();
			done();
		})
		console.info('queryBundleStateInfos_004 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0005
     * @tc.name: queryBundleStateInfos_005.
     * @tc.desc: Execute queryBundleStateInfos with promise.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfos_005', 0, async function(done) {
		console.info('queryBundleStateInfos_005 start');
		let timer = onInit();
		bundlestate.queryBundleStateInfos(timer.startTime, 'endTime', (err, data) => {
			if(err) {
				console.info('queryBundleStateInfos_005 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleStateInfos_005 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleStateInfos_005 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0006
     * @tc.name: queryBundleStateInfos_006.
     * @tc.desc: Execute queryBundleStateInfos with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfos_006', 0, async function(done) {
		console.info('queryBundleStateInfos_006 start');
		bundlestate.queryBundleStateInfos('startTime', 1923746, (err, data) => {
			if(err) {
				console.info('queryBundleStateInfos_006 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleStateInfos_006 callback success result = ' + JSON.stringify(data));
				expect(data).assertFail();
			}
			done();
		});
		console.info('queryBundleStateInfos_006 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0007
     * @tc.name: queryBundleStateInfos_007.
     * @tc.desc: Execute queryBundleStateInfos with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfos_007', 0, async function(done) {
		console.info('queryBundleStateInfos_007 start');
		bundlestate.queryBundleStateInfos('', 1923746, (err, data) => {
			if(err) {
				console.info('queryBundleStateInfos_007 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleStateInfos_007 callback success result = ' + JSON.stringify(data));
				expect(data).assertFail();
			}
			done();
		});
		console.info('queryBundleStateInfos_007 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0008
     * @tc.name: queryBundleStateInfos_008.
     * @tc.desc: Execute queryBundleStateInfos with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfos_008', 0, async function(done) {
		console.info('queryBundleStateInfos_008 start');
		bundlestate.queryBundleStateInfos(true, true, (err, data) => {
			if(err) {
				console.info('queryBundleStateInfos_008 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleStateInfos_008 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleStateInfos_008 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0009
     * @tc.name: queryBundleStateInfos_009.
     * @tc.desc: Execute queryBundleStateInfos with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfos_009', 0, async function(done) {
		console.info('queryBundleStateInfos_009 start');
		let timer = onInit();
		bundlestate.queryBundleStateInfos('', '', (err, data) => {
			if(err) {
				console.info('queryBundleStateInfos_009 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleStateInfos_009 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleStateInfos_009 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0010
     * @tc.name: queryBundleStateInfos_010.
     * @tc.desc: Execute queryBundleStateInfos with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfos_010', 0, async function(done) {
		console.info('queryBundleStateInfos_010 start');
		let timer = onInit();
		bundlestate.queryBundleStateInfos(timer.startTime, timer.startTime, (err, data) => {
			if(err) {
				console.info('queryBundleStateInfos_010 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleStateInfos_010 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleStateInfos_010 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0011
     * @tc.name: queryBundleStateInfos_011.
     * @tc.desc: Execute queryBundleStateInfos with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfos_011', 0, async function(done) {
		console.info('queryBundleStateInfos_011 start');
		let timer = onInit();
		bundlestate.queryBundleStateInfos(timer.endTime, timer.startTime, (err, data) => {
			if(err) {
				console.info('queryBundleStateInfos_011 callback fail result = ' + JSON.stringify(err));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleStateInfos_011 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleStateInfos_011 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0012
     * @tc.name: queryBundleStateInfos_012.
     * @tc.desc: Execute queryBundleStateInfos with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfos_012', 0, async function(done) {
		console.info('queryBundleStateInfos_012 start');
		let timer = onInit();
		bundlestate.queryBundleStateInfos(null, -1, (err, data) => {
			if(err) {
				console.info('queryBundleStateInfos_012 callback fail result = ' + JSON.stringify(err));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleStateInfos_012 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleStateInfos_012 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0013
     * @tc.name: queryBundleStateInfoByInterval_013.
     * @tc.desc: Execute queryBundleStateInfoByInterval with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfoByInterval_013', 0, async function(done) {
		console.info('queryBundleStateInfoByInterval_013 start');
		let timer = onInit();
		bundlestate.queryBundleStateInfoByInterval(byInterval, timer.startTime, timer.endTime, (err, data) => {
			console.info("***" + byInterval + "***" + typeof byInterval)
			if(err) {
				console.info('queryBundleStateInfoByInterval_013 callback fail result = ' + JSON.stringify(err));
				expect().assertFail();
			} else {
				console.info('queryBundleStateInfoByInterval_013 callback success result = ' + JSON.stringify(data));
				expect(data).assertInstanceOf('Array');
			}
			done();
		});
		console.info('queryBundleStateInfoByInterval_013 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0014
     * @tc.name: queryBundleStateInfoByInterval_014.
     * @tc.desc: Execute queryBundleStateInfoByInterval with promise.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfoByInterval_014', 0, async function(done) {
		console.info('queryBundleStateInfoByInterval_014 start');
		let timer = onInit();
		bundlestate.queryBundleStateInfoByInterval(byInterval, timer.startTime, timer.endTime).then((data) => {
			console.info('queryBundleStateInfoByInterval_014 promise success result = ' + JSON.stringify(data));
			expect(data).assertInstanceOf('Array');
			done();
		}).catch((err) => {
			console.info('queryBundleStateInfoByInterval_014 promise fail result = ' + JSON.stringify(err));
			expect(err.code).assertFail();
			done();
		})
		console.info('queryBundleStateInfoByInterval_014 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0015
     * @tc.name: queryBundleStateInfoByInterval_015.
     * @tc.desc: Execute queryBundleStateInfoByInterval with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfoByInterval_015', 0, async function(done) {
		console.info('queryBundleStateInfoByInterval_015 start');
		bundlestate.queryBundleStateInfoByInterval(null, 0, 0, (err, data) => {
			if(err) {
				console.info('queryBundleStateInfoByInterval_015 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleStateInfoByInterval_015 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleStateInfoByInterval_015 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0016
     * @tc.name: queryBundleStateInfoByInterval_016.
     * @tc.desc: Execute queryBundleStateInfoByInterval with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfoByInterval_016', 0, async function(done) {
		console.info('queryBundleStateInfoByInterval_016 start');
		let timer = onInit();
		bundlestate.queryBundleStateInfoByInterval(byInterval, 'startTime', timer.endTime, (err, data) => {
			if(err) {
				console.info('queryBundleStateInfoByInterval_016 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleStateInfoByInterval_016 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleStateInfoByInterval_016 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0017
     * @tc.name: queryBundleStateInfoByInterval_017.
     * @tc.desc: Execute queryBundleStateInfoByInterval with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfoByInterval_017', 0, async function(done) {
		console.info('queryBundleStateInfoByInterval_017 start');
		let timer = onInit();
		bundlestate.queryBundleStateInfoByInterval(-1, timer.startTime, timer.endTime, (err, data) => {
			if(err) {
				console.info('queryBundleStateInfoByInterval_017 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleStateInfoByInterval_017 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleStateInfoByInterval_017 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0018
     * @tc.name: queryBundleStateInfoByInterval_018.
     * @tc.desc: Execute queryBundleStateInfoByInterval with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfoByInterval_018', 0, async function(done) {
		console.info('queryBundleStateInfoByInterval_018 start');
		let timer = onInit();
		bundlestate.queryBundleStateInfoByInterval(byInterval, timer.startTime, true, (err, data) => {
			if(err) {
				console.info('queryBundleStateInfoByInterval_018 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleStateInfoByInterval_018 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleStateInfoByInterval_018 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0019
     * @tc.name: queryBundleStateInfoByInterval_019.
     * @tc.desc: Execute queryBundleStateInfoByInterval with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfoByInterval_019', 0, async function(done) {
		console.info('queryBundleStateInfoByInterval_019 start');
		bundlestate.queryBundleStateInfoByInterval(byInterval, 'timer.startTime', 123, (err, data) => {
			if(err) {
				console.info('queryBundleStateInfoByInterval_019 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleStateInfoByInterval_019 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleStateInfoByInterval_019 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0020
     * @tc.name: queryBundleStateInfoByInterval_020.
     * @tc.desc: Execute queryBundleStateInfoByInterval with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfoByInterval_020', 0, async function(done) {
		console.info('queryBundleStateInfoByInterval_020 start');
		bundlestate.queryBundleStateInfoByInterval(null, null, null, (err, data) => {
			if(err) {
				console.info('queryBundleStateInfoByInterval_020 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleStateInfoByInterval_020 callback success result = ' + JSON.stringify(data));
				expect(data).assertFail();
			}
			done();
		});
		console.info('queryBundleStateInfoByInterval_020 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0021
     * @tc.name: queryBundleStateInfoByInterval_021.
     * @tc.desc: Execute queryBundleStateInfoByInterval with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfoByInterval_021', 0, async function(done) {
		console.info('queryBundleStateInfoByInterval_021 start');
		bundlestate.queryBundleStateInfoByInterval(null, null, null, (err, data) => {
			if(err) {
				console.info('queryBundleStateInfoByInterval_021 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleStateInfoByInterval_021 callback success result = ' + JSON.stringify(data));
				expect(data).assertFail();
			}
			done();
		});
		console.info('queryBundleStateInfoByInterval_021 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0022
     * @tc.name: queryBundleStateInfoByInterval_022.
     * @tc.desc: Execute queryBundleStateInfoByInterval with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleStateInfoByInterval_022', 0, async function(done) {
		console.info('queryBundleStateInfoByInterval_022 start');
		let timer = onInit();
		bundlestate.queryBundleStateInfoByInterval(byInterval, timer.endTime, timer.startTime, (err, res) => {
			if(err) {
				console.info('queryBundleStateInfoByInterval_022 callback fail result = ' + JSON.stringify(err));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleStateInfoByInterval_022 callback success result = ' + JSON.stringify(res));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleStateInfoByInterval_022 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_0023
     * @tc.name: merge_023.
     * @tc.desc: Execute merge with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('merge_023', 0, function(done) {
		console.info('bundle merge_023 start');
		let timer = onInit();
		bundlestate.queryBundleStateInfoByInterval(byInterval, timer.startTime, timer.endTime).then(res => {
			console.info("&&&&&&&" + JSON.stringify(res));
			try {
				let ret = bundlestate.BundleStateInfo.merge(toMerge);
				expect(ret).assertUndefined()
			} catch(e) {
				expect().assertUndefined();
			}
			done()
		})
		console.info('merge_023 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0024
     * @tc.name: merge_024.
     * @tc.desc: Execute merge with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('merge_024', 0, function(done) {
		console.info('bundle merge_024 start');
		let toMerge02 = {
			"abilityInFgTotalTime": 0,
			"abilityPrevAccessTime": 1629907200000,
			"abilityPrevSeenTime": 0,
			"abilitySeenTotalTime": 0,
			"bundleName": "com.example.backgroundTaskManager",
			"fgAbilityAccessTotalTime": 0,
			"fgAbilityPrevAccessTime": 0,
			"id": 0,
			"infosBeginTime": 1629907200000,
			"infosEndTime": 1629993599999,
		};
		try {
			let ret = bundlestate.BundleStateInfo.merge(toMerge02);
			expect(ret).assertUndefined()
		} catch(e) {
			expect().assertUndefined();
		}
		done();
		console.info('merge_024 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0025
     * @tc.name: merge_025.
     * @tc.desc: Execute merge with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('merge_025', 0, function(done) {
		console.info('bundle merge_025 start');
		let toMerge03 = {
			"abilityInFgTotalTime": 'text',
			"abilityPrevAccessTime": 'text',
			"abilityPrevSeenTime": 'text',
			"abilitySeenTotalTime": 'text',
			"bundleName": "com.example.backgroundTaskManager",
			"fgAbilityAccessTotalTime": 'text',
			"fgAbilityPrevAccessTime": 'text',
			"id": 'text',
			"infosBeginTime": 'text',
			"infosEndTime": 'text',
		};
		try {
			let ret = bundlestate.BundleStateInfo.merge(toMerge03);
			expect(ret).assertUndefined()
		} catch(e) {
			expect().assertUndefined();
		}
		done();
		console.info('merge_025 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_0026
     * @tc.name: merge_026.
     * @tc.desc: Execute merge with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('merge_026', 0, function(done) {
		console.info('bundle merge_026 start');
		try {
			let ret = bundlestate.BundleStateInfo.merge(null);
			expect(ret).assertUndefined()
		} catch(e) {
			expect().assertUndefined();
		}
		done();
		console.info('merge_026 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_0027
     * @tc.name: merge_027.
     * @tc.desc: Execute merge with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('merge_027', 0, function(done) {
		console.info('bundle merge_027 start');
		try {
			let ret = bundlestate.BundleStateInfo.merge(true);
			expect(ret).assertUndefined()
		} catch(e) {
			expect().assertUndefined();
		}
		done();
		console.info('merge_027 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_0028
     * @tc.name: merge_028.
     * @tc.desc: Execute merge with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('merge_028', 0, function(done) {
		console.info('bundle merge_028 start');
		try {
			let ret = bundlestate.BundleStateInfo.merge(123);
			expect(ret).assertUndefined()
		} catch(e) {
			expect().assertUndefined();
		}
		done();
		console.info('merge_028 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0029
     * @tc.name: queryBundleActiveStates_029.
     * @tc.desc: Execute queryBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleActiveStates_029', 0, async function(done) {
		console.info('queryBundleActiveStates_029 start');
		let timer = onInit();
		bundlestate.queryBundleActiveStates(timer.startTime, timer.endTime, (err, data) => {
			if(err) {
				console.info('queryBundleActiveStates_029 callback fail result = ' + JSON.stringify(err));
				expect(err.code).assertFail();
				done();
			} else {
				console.info('queryBundleActiveStates_029 callback success result = ' + JSON.stringify(data));
				expect(data).assertInstanceOf('Array');
			}
			done();
		});
		console.info('queryBundleActiveStahdctes_029 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0030
     * @tc.name: queryBundleActiveStates_030.
     * @tc.desc: Execute queryBundleActiveStates with promise.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleActiveStates_030', 0, async function(done) {
		console.info('queryBundleActiveStates_030 start');
		let timer = onInit();
		bundlestate.queryBundleActiveStates(timer.startTime, timer.endTime).then((data) => {
			console.info('queryBundleActiveStates_030 promise success result = ' + JSON.stringify(data));
			expect(data).assertInstanceOf('Array');
		}).catch((err) => {
			console.info('queryBundleActiveStates_030 promise fail result = ' + JSON.stringify(err.code));
			expect().assertFail();
		})
		done();
		console.info('queryBundleActiveStates_030 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0031
     * @tc.name: queryBundleActiveStates_031.
     * @tc.desc: Execute queryBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleActiveStates_031', 0, async function(done) {
		console.info('queryBundleActiveStates_031 start');
		let timer = onInit();
		bundlestate.queryBundleActiveStates(3.1415, timer.endTime, (err, data) => {
			if(err) {
				console.info('queryBundleActiveStates_031 callback fail result = ' + JSON.stringify(err));
				expect().assertFail();
			} else {
				console.info('queryBundleActiveStates_031 callback success result = ' + JSON.stringify(data));
				expect(data).assertInstanceOf('Array');
			}
			done();
		});
		console.info('queryBundleActiveStahdctes_031 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0032
     * @tc.name: queryBundleActiveStates_032.
     * @tc.desc: Execute queryBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleActiveStates_032', 0, async function(done) {
		console.info('queryBundleActiveStates_032 start');
		let timer = onInit();
		bundlestate.queryBundleActiveStates(timer.startTime, "endTime", (err, data) => {
			if(err) {
				console.info('queryBundleActiveStates_032 callback fail result = ' + JSON.stringify(err));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleActiveStates_032 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleActiveStates_032 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0033
     * @tc.name: queryBundleActiveStates_033.
     * @tc.desc: Execute queryBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleActiveStates_033', 0, async function(done) {
		console.info('queryBundleActiveStates_033 start');
		bundlestate.queryBundleActiveStates('startTime', -1, (err, data) => {
			if(err) {
				console.info('queryBundleActiveStates_033 callback fail result = ' + JSON.stringify(err));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleActiveStates_033 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleActiveStates_033 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0034
     * @tc.name: queryBundleActiveStates_034.
     * @tc.desc: Execute queryBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleActiveStates_034', 0, async function(done) {
		console.info('queryBundleActiveStates_034 start');
		let timer = onInit();
		bundlestate.queryBundleActiveStates('timer.startTime', timer.endTime, (err, data) => {
			if(err) {
				console.info('queryBundleActiveStates_034 callback fail result = ' + JSON.stringify(err));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleActiveStates_034 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleActiveStates_034 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0035
     * @tc.name: queryBundleActiveStates_035.
     * @tc.desc: Execute queryBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleActiveStates_035', 0, async function(done) {
		console.info('queryBundleActiveStates_035 start');
		bundlestate.queryBundleActiveStates('timer.startTime', 'timer.endTime', (err, data) => {
			if(err) {
				console.info('queryBundleActiveStates_035 callback fail result = ' + JSON.stringify(err));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleActiveStates_035 callback success result = ' + JSON.stringify(data));
				expect(data).assertFail();
			}
			done();
		});
		console.info('queryBundleActiveStates_035 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0036
     * @tc.name: queryBundleActiveStates_036.
     * @tc.desc: Execute queryBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleActiveStates_036', 0, async function(done) {
		console.info('queryBundleActiveStates_036 start');
		bundlestate.queryBundleActiveStates('timer.startTime', true, (err, data) => {
			if(err) {
				console.info('queryBundleActiveStates_036 callback fail result = ' + JSON.stringify(err));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleActiveStates_036 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleActiveStates_036 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0037
     * @tc.name: queryBundleActiveStates_037.
     * @tc.desc: Execute queryBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleActiveStates_037', 0, async function(done) {
		console.info('queryBundleActiveStates_037 start');
		bundlestate.queryBundleActiveStates('', '', (err, data) => {
			if(err) {
				console.info('queryBundleActiveStates_037 callback fail result = ' + JSON.stringify(err));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleActiveStates_037 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleActiveStates_037 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0038
     * @tc.name: queryBundleActiveStates_038.
     * @tc.desc: Execute queryBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleActiveStates_038', 0, async function(done) {
		console.info('queryBundleActiveStates_038 start');
		let timer = onInit();
		bundlestate.queryBundleActiveStates('', timer.endTime, (err, data) => {
			if(err) {
				console.info('queryBundleActiveStates_038 callback fail result = ' + JSON.stringify(err));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleActiveStates_038 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleActiveStates_038 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0039
     * @tc.name: queryBundleActiveStates_039.
     * @tc.desc: Execute queryBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleActiveStates_039', 0, async function(done) {
		console.info('queryBundleActiveStates_039 start');
		let timer = onInit();
		bundlestate.queryBundleActiveStates(timer.endTime, timer.startTime, (err, data) => {
			if(err) {
				console.info('queryBundleActiveStates_039 callback fail result = ' + JSON.stringify(err));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleActiveStates_039 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleActiveStates_039 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0040
     * @tc.name: queryBundleActiveStates_040.
     * @tc.desc: Execute queryBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryBundleActiveStates_040', 0, async function(done) {
		console.info('queryBundleActiveStates_040 start');
		let timer = onInit();
		bundlestate.queryBundleActiveStates(timer.startTime, timer.startTime, (err, data) => {
			if(err) {
				console.info('queryBundleActiveStates_040 callback fail result = ' + JSON.stringify(err));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryBundleActiveStates_040 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryBundleActiveStates_040 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0041
     * @tc.name: queryCurrentBundleActiveStates_041.
     * @tc.desc: Execute queryCurrentBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryCurrentBundleActiveStates_041', 0, async function(done) {
		console.info('queryCurrentBundleActiveStates_041 start');
		let timer = onInit();
		bundlestate.queryCurrentBundleActiveStates(timer.startTime, timer.endTime, (err, data) => {
			if(err) {
				console.info('queryCurrentBundleActiveStates_041 callback fail result = ' + err);
				expect().assertFail();
			} else {
				console.info('queryCurrentBundleActiveStates_041 callback success result = ' + JSON.stringify(data));
				expect(data).assertInstanceOf('Array');
			}
			done();
		});
		console.info('queryCurrentBundleActiveStates_041 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0042
     * @tc.name: queryCurrentBundleActiveStates_042.
     * @tc.desc: Execute queryCurrentBundleActiveStates with promise.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryCurrentBundleActiveStates_042', 0, async function(done) {
		console.info('queryCurrentBundleActiveStates_042 start');
		let timer = onInit();
		bundlestate.queryCurrentBundleActiveStates(timer.startTime, timer.endTime).then((data) => {
			console.info('queryCurrentBundleActiveStates_042 promise success result = ' + JSON.stringify(data));
			expect(data).assertInstanceOf('Array');
		}).catch((err) => {
			console.info('queryCurrentBundleActiveStates_042 promise fail result = ' + err.code);
			expect().assertFail();
		})
		done();
		console.info('queryCurrentBundleActiveStates_042 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0043
     * @tc.name: queryCurrentBundleActiveStates_043.
     * @tc.desc: Execute queryCurrentBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryCurrentBundleActiveStates_043', 0, async function(done) {
		console.info('queryCurrentBundleActiveStates_043 start');
		bundlestate.queryCurrentBundleActiveStates(true, false, (err, data) => {
			if(err) {
				console.info('queryCurrentBundleActiveStates_043 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryCurrentBundleActiveStates_043 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryCurrentBundleActiveStates_043 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0044
     * @tc.name: queryCurrentBundleActiveStates_044.
     * @tc.desc: Execute queryCurrentBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryCurrentBundleActiveStates_044', 0, async function(done) {
		console.info('queryCurrentBundleActiveStates_044 start');
		let timer = onInit();
		bundlestate.queryCurrentBundleActiveStates(timer.startTime, '', (err, data) => {
			if(err) {
				console.info('queryCurrentBundleActiveStates_044 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryCurrentBundleActiveStates_044 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryCurrentBundleActiveStates_044 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0045
     * @tc.name: queryCurrentBundleActiveStates_045.
     * @tc.desc: Execute queryCurrentBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryCurrentBundleActiveStates_045', 0, async function(done) {
		console.info('queryCurrentBundleActiveStates_045 start');
		bundlestate.queryCurrentBundleActiveStates(true, -1, (err, data) => {
			if(err) {
				console.info('queryCurrentBundleActiveStates_045 callback fail result = ' + err.code);
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryCurrentBundleActiveStates_045 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryCurrentBundleActiveStates_045 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0046
     * @tc.name: queryCurrentBundleActiveStates_046.
     * @tc.desc: Execute queryCurrentBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryCurrentBundleActiveStates_046', 0, async function(done) {
		console.info('queryCurrentBundleActiveStates_046 start');
		bundlestate.queryCurrentBundleActiveStates(10001, 'timer.endTime', (err, data) => {
			if(err) {
				console.info('queryCurrentBundleActiveStates_046 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryCurrentBundleActiveStates_046 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryCurrentBundleActiveStates_046 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0047
     * @tc.name: queryCurrentBundleActiveStates_047.
     * @tc.desc: Execute queryCurrentBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryCurrentBundleActiveStates_047', 0, async function(done) {
		console.info('queryCurrentBundleActiveStates_047 start');
		let timer = onInit();
		bundlestate.queryCurrentBundleActiveStates(timer.startTime, timer.startTime, (err, data) => {
			if(err) {
				console.info('queryCurrentBundleActiveStates_047 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryCurrentBundleActiveStates_047 callback success result = ' + JSON.stringify(data));
				expect(data).assertFail();
			}
			done();
		});
		console.info('queryCurrentBundleActiveStates_047 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0048
     * @tc.name: queryCurrentBundleActiveStates_048.
     * @tc.desc: Execute queryCurrentBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryCurrentBundleActiveStates_048', 0, async function(done) {
		console.info('queryCurrentBundleActiveStates_048 start');
		let timer = onInit();
		bundlestate.queryCurrentBundleActiveStates(timer.endTime, timer.startTime, (err, data) => {
			if(err) {
				console.info('queryCurrentBundleActiveStates_048 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryCurrentBundleActiveStates_048 callback success result = ' + JSON.stringify(data));
				expect(data).assertFail();
			}
			done();
		});
		console.info('queryCurrentBundleActiveStates_048 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0049
     * @tc.name: queryCurrentBundleActiveStates_049.
     * @tc.desc: Execute queryCurrentBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryCurrentBundleActiveStates_049', 0, async function(done) {
		console.info('queryCurrentBundleActiveStates_049 start');
		let timer = onInit();
		bundlestate.queryCurrentBundleActiveStates(3.145, timer.startTime, (err, data) => {
			if(err) {
				console.info('queryCurrentBundleActiveStates_049 callback fail result = ' + JSON.stringify(err.code));
				expect().assertFail();
			} else {
				console.info('queryCurrentBundleActiveStates_049 callback success result = ' + JSON.stringify(data));
				expect(data).assertInstanceOf('Array');
			}
			done();
		});
		console.info('queryCurrentBundleActiveStates_049 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0050
     * @tc.name: queryCurrentBundleActiveStates_050.
     * @tc.desc: Execute queryCurrentBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryCurrentBundleActiveStates_050', 0, async function(done) {
		console.info('queryCurrentBundleActiveStates_050 start');
		let timer = onInit();
		bundlestate.queryCurrentBundleActiveStates('', '', (err, data) => {
			if(err) {
				console.info('queryCurrentBundleActiveStates_050 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryCurrentBundleActiveStates_050 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryCurrentBundleActiveStates_050 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0051
     * @tc.name: queryCurrentBundleActiveStates_051.
     * @tc.desc: Execute queryCurrentBundleActiveStates with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('queryCurrentBundleActiveStates_051', 0, async function(done) {
		console.info('queryCurrentBundleActiveStates_051 start');
		let timer = onInit();
		bundlestate.queryCurrentBundleActiveStates(null, null, (err, data) => {
			if(err) {
				console.info('queryCurrentBundleActiveStates_051 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('queryCurrentBundleActiveStates_051 callback success result = ' + JSON.stringify(data));
				expect().assertFail();
			}
			done();
		});
		console.info('queryCurrentBundleActiveStates_051 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0052
     * @tc.name: isIdleState_052.
     * @tc.desc: Execute isIdleState with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('isIdleState_052', 0, async function(done) {
		console.info('isIdleState_052 start');
		bundlestate.isIdleState(bundleName, (err, res) => {
			if(err) {
				console.info('isIdleState_052 callback fail result = ' + JSON.stringify(err.code));
				expect().assertFail();
			} else {
				console.info('isIdleState_052 callback success result = ' + JSON.stringify(res));
				expect(res).assertTrue();
			}
			done();
		});
		console.info('isIdleState_052 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0053
     * @tc.name: isIdleState_053.
     * @tc.desc: Execute isIdleState with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('isIdleState_053', 0, async function(done) {
		console.info('isIdleState_053 start');
		let bundleName02 = 'ohos.plrdtest.resourceschedule'
		bundlestate.isIdleState(bundleName02, (err, res) => {
			if(err) {
				console.info('isIdleState_053 callback fail result = ' + JSON.stringify(err.code));
				expect().assertFail();
			} else {
				console.info('isIdleState_053 callback success result = ' + JSON.stringify(res));
				expect(res).assertInstanceOf('Boolean');
			}
			done();
		});
		console.info('isIdleState_053 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0054
     * @tc.name: isIdleState_054.
     * @tc.desc: Execute isIdleState with promise.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('isIdleState_054', 0, async function(done) {
		console.info('isIdleState_054 start');
		bundlestate.isIdleState(bundleName).then((data) => {
			console.info('isIdleState_054 promise success result = ' + JSON.stringify(data));
			expect(data).assertTrue();
			done();
		}).catch((err) => {
			console.info('isIdleState_054 promise fail result = ' + JSON.stringify(err.code));
			expect().assertFail();
			done();
		})
		console.info('isIdleState_054 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0055
     * @tc.name: isIdleState_055.
     * @tc.desc: Execute isIdleState with promise.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('isIdleState_055', 0, async function(done) {
		console.info('isIdleState_055 start');
		let bundleName31 = 'ohos.plrdtest.resourceschedule'
		bundlestate.isIdleState(bundleName31).then((res) => {
			console.info('isIdleState_055 promise success result = ' + res);
			expect(res).assertFalse();
		}).catch((err) => {
			console.info('isIdleState_055 promise fail result = ' + JSON.stringify(err));
			expect().assertFail();
		})
		done();
		console.info('isIdleState_055 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0056
     * @tc.name: isIdleState_056.
     * @tc.desc: Execute isIdleState with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('isIdleState_056', 0, async function(done) {
		console.info('isIdleState_056 start');
		bundlestate.isIdleState(123, (err, data) => {
			if(err) {
				console.info('isIdleState_056 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('isIdleState_056 callback success result = ' + JSON.stringify(data));
				expect(data).assertFail();
			}
			done();
		});
		console.info('isIdleState_056 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0057
     * @tc.name: isIdleState_057.
     * @tc.desc: Execute isIdleState with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('isIdleState_057', 0, async function(done) {
		console.info('isIdleState_057 start');
		bundlestate.isIdleState(null, (err, data) => {
			if(err) {
				console.info('isIdleState_057 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('isIdleState_057 callback success result = ' + JSON.stringify(data));
				expect(data).assertFail();
			}
			done();
		});
		console.info('isIdleState_057 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0058
     * @tc.name: isIdleState_058.
     * @tc.desc: Execute isIdleState with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('isIdleState_058', 0, async function(done) {
		console.info('isIdleState_058 start');
		bundlestate.isIdleState(true, (err, data) => {
			if(err) {
				console.info('isIdleState_058 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('isIdleState_058 callback success result = ' + JSON.stringify(data));
				expect(data).assertFail();
			}
			done();
		});
		console.info('isIdleState_058 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0059
     * @tc.name: isIdleState_059.
     * @tc.desc: Execute isIdleState with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('isIdleState_059', 0, async function(done) {
		console.info('isIdleState_059 start');
		bundlestate.isIdleState(false, (err, data) => {
			if(err) {
				console.info('isIdleState_059 callback fail result = ' + JSON.stringify(err));
				expect(err.code).assertLarger(0);
			} else {
				console.info('isIdleState_059 callback success result = ' + JSON.stringify(data));
				expect(data).assertFail();
			}
			done();
		});
		console.info('isIdleState_059 end');
	})
	
	/*
	 * @tc.number: SUB_INTELLITV_JS_BUNDLE_0060
     * @tc.name: isIdleState_060.
     * @tc.desc: Execute isIdleState with callback.
	 * @tc.size: MEDIUM
     * @tc.type: Function
     * @tc.level: level 0
     */
	it('isIdleState_060', 0, async function(done) {
		console.info('isIdleState_060 start');
		bundlestate.isIdleState('', (err, data) => {
			if(err) {
				console.info('isIdleState_060 callback fail result = ' + JSON.stringify(err.code));
				expect(err.code).assertLarger(0);
			} else {
				console.info('isIdleState_060 callback success result = ' + JSON.stringify(data));
				expect(data).assertFail();
			}
			done();
		});
		console.info('isIdleState_060 end');
	})
})