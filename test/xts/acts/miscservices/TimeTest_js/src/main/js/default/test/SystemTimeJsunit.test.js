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

// @ts-nocheck
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import systemTime from '@ohos.systemtime'

describe('TimeTest', function() {
    console.log('start################################start');
    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_0100
     * @tc.name      Test systemTime.setTimezone timezone = 'Anadyr, Russia'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test1',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_0100 start')
		var timezone = 'Antarctica/McMurdo'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_0100 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_0200
     * @tc.name      Test systemTime.setTimezone timezone = 'Honiara, SolomonIslands'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test2',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_0200 start')
		var timezone = 'Africa/Kinshasa'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_0200 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_0300
     * @tc.name      Test systemTime.setTimezone timezone = 'Melbourne, Australia'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test3',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_0300 start')
		var timezone = 'Asia/Nicosia'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_0300 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_0400
     * @tc.name      Test systemTime.setTimezone timezone = 'Tokyo, Japan'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test4',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_0400 start')
		var timezone = 'America/Guayaquil'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_0400 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_0500
     * @tc.name      Test systemTime.setTimezone timezone = 'Beijing, China'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test5',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_0500 start')
		var timezone = 'Pacific/Pohnpei'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_0500 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_0600
     * @tc.name      Test systemTime.setTimezone timezone = 'Jakarta, Indonesia'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test6',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_0600 start')
		var timezone = 'Asia/Almaty'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_0600 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_0700
     * @tc.name      Test systemTime.setTimezone timezone = 'Dhaka, Bangladesh'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test7',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_0700 start')
		var timezone = 'America/Godthab'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_0700 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_0800
     * @tc.name      Test systemTime.setTimezone timezone = 'Tashkent, Uzbekistan'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test8',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_0800 start')
		var timezone = 'Pacific/Majuro'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_0800 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_0900
     * @tc.name      Test systemTime.setTimezone timezone = 'Dubai, U.A.E.'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test9',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_0900 start')
		var timezone = 'Asia/Kuala_Lumpur'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_0900 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_1000
     * @tc.name      Test systemTime.setTimezone timezone = 'Moscow, Russia'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test10',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_1000 start')
		var timezone = 'Asia/Gaza'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_1000 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_1100
     * @tc.name      Test systemTime.setTimezone timezone = 'Brussels, Belgium'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test11',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_1100 start')
		var timezone = 'Europe/Kiev'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_1100 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_1200
     * @tc.name      Test systemTime.setTimezone timezone = 'London, England'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test12',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_1200 start')
		var timezone = 'America/New_York'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_1200 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_1300
     * @tc.name      Test systemTime.setTimezone timezone = 'Accra, Ghana'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test13',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_1300 start')
		var timezone = 'Pacific/Pohnpei'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_1300 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_1400
     * @tc.name      Test systemTime.setTimezone timezone = 'Praia, CaboVerde'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test14',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_1400 start')
		var timezone = 'America/Guayaquil'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_1400 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_1500
     * @tc.name      Test systemTime.setTimezone timezone = 'Nuuk, Greenland'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test15',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_1500 start')
		var timezone = 'Pacific/Auckland'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_1500 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_1600
     * @tc.name      Test systemTime.setTimezone timezone = 'Buenos Aires, Argentina'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test16',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_1600 start')
		var timezone = 'Pacific/Auckland'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_1600 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_1700
     * @tc.name      Test systemTime.setTimezone timezone = 'New York, U.S.A.'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test17',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_1700 start')
		var timezone = 'America/Godthab'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_1700 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_1800
     * @tc.name      Test systemTime.setTimezone timezone = 'Mexico City, Mexico'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test18',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_1800 start')
		var timezone = 'Europe/Berlin'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_1800 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_1900
     * @tc.name      Test systemTime.setTimezone timezone = 'Guatemala City, Guatemala'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test19',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_1900 start')
		var timezone = 'Asia/Jakarta'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_1900 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_2000
     * @tc.name      Test systemTime.setTimezone timezone = 'Los Angeles, U.S.A.'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test20',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_2000 start')
		var timezone = 'Asia/Tashkent'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_2000 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_2100
     * @tc.name      Test systemTime.setTimezone timezone = 'Anchorage, U.S.A.'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test20',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_2000 start')
		var timezone = 'Asia/Tashkent'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_2000 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_2200
     * @tc.name      Test systemTime.setTimezone timezone = 'Adak, U.S.A.'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test22',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_2200 start')
		var timezone = 'Asia/Tashkent'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_2200 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_2300
     * @tc.name      Test systemTime.setTimezone timezone = 'Honolulu, U.S.A.'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test23',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_2300 start')
		var timezone = 'Pacific/Pohnpei'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_2300 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_2400
     * @tc.name      Test systemTime.setTimezone timezone = 'Alofi, Niue'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test24',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_2400 start')
		var timezone = 'Pacific/Pohnpei'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_2400 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_2500
     * @tc.name      Test systemTime.setTimezone timezone = 'Baker Island, U.S.A.'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test25',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_2500 start')
		var timezone = 'Asia/Nicosia'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_2500 end');
    })

    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_2600
     * @tc.name      Test systemTime.setTimezone timezone = 'XXXX, Asia'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test26',0, async () => {
        console.log('SUB_systemTime_setTimezone_JS_API_2600 start')
		var timezone = 'Asia/Nicosia'
		
		console.log('setTimezone = ' + timezone)
        systemTime.setTimezone(timezone)
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
		console.log('setTimezone = XXXX, Asia')
        systemTime.setTimezone('XXXX, Asia')
		
		var resGetTimeZone = systemTime.getTimeZone()
        console.log('GetTimezone = ' + resGetTimeZone)
		assert.equal(resGetTimeZone,timezone)
		
        console.log('SUB_systemTime_setTimezone_JS_API_2600 end');
    })
	
	/**
     * @tc.number    SUB_systemTime_setTime_JS_API_0100
     * @tc.name      Test systemTime.setTime time = 0
     * @tc.desc      Test systemTime_setTime API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTime_test1',0, async () => {
        console.log('SUB_systemTime_setTime_JS_API_0100 start')
        var time = 0
		
		console.log('setTime = ' + time)
		systemTime.setTime(time)
		
		var resGetTime = systemTime.getCurrentTime(true)
		console.log('GetTime = ' + resGetTime)
		assert.equal(resGetTime,time)
		
		sleep(5000).then(() => {
			resGetTime = systemTime.getCurrentTime(true);
			console.log('GetTime = ' + resGetTime);
			assert.equal(resGetTime <= (time + 6000) && resGetTime >= (time + 5000));
			
			console.log('SUB_systemTime_setTime_JS_API_0100 end');
		})	 	
    })

    /**
     * @tc.number    SUB_systemTime_setTime_JS_API_0200
     * @tc.name      Test systemTime.setTime time = 1
     * @tc.desc      Test systemTime_setTime API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTime_test2',0, async () => {
        console.log('SUB_systemTime_setTime_JS_API_0200 start')
        var time = 1
		
		console.log('setTime = ' + time)
		systemTime.setTime(time)
		
		var resGetTime = systemTime.getCurrentTime(true)
		console.log('GetTime = ' + resGetTime)
		assert.equal(resGetTime,time)
		
		sleep(5000).then(() => {
			resGetTime = systemTime.getCurrentTime(true);
			console.log('GetTime = ' + resGetTime);
			assert.equal(resGetTime <= (time + 6000) && resGetTime >= (time + 5000));
			
			console.log('SUB_systemTime_setTime_JS_API_0200 end');
		})	 	
    })

    /**
     * @tc.number    SUB_systemTime_setTime_JS_API_0300
     * @tc.name      Test systemTime.setTime time = Number.MAX_VALUE/2
     * @tc.desc      Test systemTime_setTime API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTime_test3',0, async () => {
        console.log('SUB_systemTime_setTime_JS_API_0300 start')
        var time = Number.MAX_VALUE/2
		
		console.log('setTime = ' + time)
		systemTime.setTime(time)
		
		var resGetTime = systemTime.getCurrentTime(true)
		console.log('GetTime = ' + resGetTime)
		assert.equal(resGetTime,time)
		
		sleep(5000).then(() => {
			resGetTime = systemTime.getCurrentTime(true);
			console.log('GetTime = ' + resGetTime);
			assert.equal(resGetTime <= (time + 6000) && resGetTime >= (time + 5000));
			
			console.log('SUB_systemTime_setTime_JS_API_0300 end');
		})	 	
    })
	
    /**
     * @tc.number    SUB_systemTime_setTime_JS_API_0400
     * @tc.name      Test systemTime.setTime time = -1
     * @tc.desc      Test systemTime_setTime API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTime_test4',0, async () => {
        console.log('SUB_systemTime_setTime_JS_API_0400 start')
        var time = -1
		
		console.log('setTime = ' + time)
		systemTime.setTime(time)
		
		console.log('SUB_systemTime_setTime_JS_API_0400 end');
    })
	
	/**
     * @tc.number    SUB_systemTime_setDate_JS_API_0100
     * @tc.name      Test systemTime.setDate date = new Date()
     * @tc.desc      Test systemTime_setDate API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setDate_test1',0, async () => {
        console.log('SUB_systemTime_setDate_JS_API_0100 start')
		var date = new Date()
		
		console.log('setDate = ' + date)
		systemTime.setDate(date)
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
        console.log('SUB_systemTime_setDate_JS_API_0100 end');
    })

    /**
     * @tc.number    SUB_systemTime_setDate_JS_API_0200
     * @tc.name      Test systemTime.setDate date = new Date(2022,1,1)
     * @tc.desc      Test systemTime_setDate API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setDate_test2',0, async () => {
        console.log('SUB_systemTime_setDate_JS_API_0200 start')
		var date = new Date(2022,1,1)
		
		console.log('setDate = ' + date)
		systemTime.setDate(date)
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
        console.log('SUB_systemTime_setDate_JS_API_0200 end');
    })

    /**
     * @tc.number    SUB_systemTime_setDate_JS_API_0300
     * @tc.name      Test systemTime.setDate date = new Date(2022,1,15)
     * @tc.desc      Test systemTime_setDate API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setDate_test3',0, async () => {
        console.log('SUB_systemTime_setDate_JS_API_0300 start')
		var date = new Date(2022,1,15)
		
		console.log('setDate = ' + date)
		systemTime.setDate(date)
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
        console.log('SUB_systemTime_setDate_JS_API_0300 end');
    })

    /**
     * @tc.number    SUB_systemTime_setDate_JS_API_0400
     * @tc.name      Test systemTime.setDate date = new Date(2022,1,31)
     * @tc.desc      Test systemTime_setDate API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setDate_test4',0, async () => {
        console.log('SUB_systemTime_setDate_JS_API_0400 start')
		var date = new Date(2022,1,31)
		
		console.log('setDate = ' + date)
		systemTime.setDate(date)
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
        console.log('SUB_systemTime_setDate_JS_API_0400 end');
    })

    /**
     * @tc.number    SUB_systemTime_setDate_JS_API_0500
     * @tc.name      Test systemTime.setDate date = new Date(2022,1,32)
     * @tc.desc      Test systemTime_setDate API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setDate_test5',0, async () => {
        console.log('SUB_systemTime_setDate_JS_API_0500 start')
		var date = new Date(2022,1,31)
		
		console.log('setDate = ' + date)
		systemTime.setDate(date)
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
		console.log('setDate = new Date(2022,1,32)')
		systemTime.setDate(new Date(2022,1,32))
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
        console.log('SUB_systemTime_setDate_JS_API_0500 end');
    })

    /**
     * @tc.number    SUB_systemTime_setDate_JS_API_0600
     * @tc.name      Test systemTime.setDate date = new Date(2022,2,29)
     * @tc.desc      Test systemTime_setDate API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setDate_test6',0, async () => {
        console.log('SUB_systemTime_setDate_JS_API_0600 start')
		var date = new Date(2022,1,31)
		
		console.log('setDate = ' + date)
		systemTime.setDate(date)
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
		console.log('setDate = new Date(2022,2,29)')
		systemTime.setDate(new Date(2022,2,29))
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
        console.log('SUB_systemTime_setDate_JS_API_0600 end');
    })

    /**
     * @tc.number    SUB_systemTime_setDate_JS_API_0700
     * @tc.name      Test systemTime.setDate date = new Date(2040,2,29)
     * @tc.desc      Test systemTime_setDate API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setDate_test7',0, async () => {
        console.log('SUB_systemTime_setDate_JS_API_0700 start')
		var date = new Date(2040,2,29)
		
		console.log('setDate = ' + date)
		systemTime.setDate(date)
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
        console.log('SUB_systemTime_setDate_JS_API_0700 end');
    })

    /**
     * @tc.number    SUB_systemTime_setDate_JS_API_0800
     * @tc.name      Test systemTime.setDate date = new Date(2040,2,30)
     * @tc.desc      Test systemTime_setDate API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setDate_test8',0, async () => {
        console.log('SUB_systemTime_setDate_JS_API_0800 start')
		var date = new Date(2022,1,31)
		
		console.log('setDate = ' + date)
		systemTime.setDate(date)
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
		console.log('setDate = new Date(2040,2,30)')
		systemTime.setDate(new Date(2040,2,30))
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
        console.log('SUB_systemTime_setDate_JS_API_0800 end');
    })

    /**
     * @tc.number    SUB_systemTime_setDate_JS_API_0900
     * @tc.name      Test systemTime.setDate date = new Date(2022,6,30)
     * @tc.desc      Test systemTime_setDate API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setDate_test9',0, async () => {
        console.log('SUB_systemTime_setDate_JS_API_0900 start')
		var date = new Date(2022,6,30)
		
		console.log('setDate = ' + date)
		systemTime.setDate(date)
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
        console.log('SUB_systemTime_setDate_JS_API_0900 end');
    })
	
	/**
     * @tc.number    SUB_systemTime_setDate_JS_API_1000
     * @tc.name      Test systemTime.setDate date = new Date(2022,6,31)
     * @tc.desc      Test systemTime_setDate API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setDate_test10',0, async () => {
        console.log('SUB_systemTime_setDate_JS_API_1000 start')
		var date = new Date(2022,1,31)
		
		console.log('setDate = ' + date)
		systemTime.setDate(date)
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
		console.log('setDate = new Date(2022,6,31)')
		systemTime.setDate(new Date(2022,6,31))
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
        console.log('SUB_systemTime_setDate_JS_API_1000 end');
    })

    /**
     * @tc.number    SUB_systemTime_setDate_JS_API_1100
     * @tc.name      Test systemTime.setDate date = new Date(2022,12,31)
     * @tc.desc      Test systemTime_setDate API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setDate_test11',0, async () => {
        console.log('SUB_systemTime_setDate_JS_API_1100 start')
		var date = new Date(2022,12,31)
		
		console.log('setDate = ' + date)
		systemTime.setDate(date)
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
        console.log('SUB_systemTime_setDate_JS_API_1100 end');
    })
	
    /**
     * @tc.number    SUB_systemTime_setDate_JS_API_1200
     * @tc.name      Test systemTime.setDate date = new Date(2022,13,1)
     * @tc.desc      Test systemTime_setDate API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setDate_test12',0, async () => {
        console.log('SUB_systemTime_setDate_JS_API_1200 start')
		var date = new Date(2022,1,31)
		
		console.log('setDate = ' + date)
		systemTime.setDate(date)
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
		console.log('setDate = new Date(2022,13,1)')
		systemTime.setDate(new Date(2022,13,1))
		
		var resGetDate = systemTime.getDate()   
		console.log('GetDate = ' + resGetDate)
		assert.deepEqual(resGetDate,date)
		
        console.log('SUB_systemTime_setDate_JS_API_1200 end');
    })
	
	/**
     * @tc.number    SUB_systemTime_getRealActiveTime_JS_API_0100
     * @tc.name      Test systemTime.getRealActiveTime
     * @tc.desc      Test systemTime_getRealActiveTime API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_getRealActiveTime_test1',0, async () => {
        console.log('SUB_systemTime_getRealActiveTime_JS_API_0100 start')
		
		var resGetTime0 = systemTime.getRealActiveTime()
		console.log('GetTime0 = ' + resGetTime0)
		
		sleep(5000).then(() => {
			var resGetTime1 = systemTime.getRealActiveTime();
			console.log('GetTime1 = ' + resGetTime1);
			assert.equal(resGetTime1 <= (resGetTime0 + 6000) && resGetTime1 >= (resGetTime0 + 5000));
			
			console.log('SUB_systemTime_getRealActiveTime_JS_API_0100 end');
		})	 	
    })
	
	/**
     * @tc.number    SUB_systemTime_getRealTime_JS_API_0100
     * @tc.name      Test systemTime.getRealTime
     * @tc.desc      Test systemTime_getRealTime API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_getRealTime_test1',0, async () => {
        console.log('SUB_systemTime_getRealTime_JS_API_0100 start')
		
		var resGetTime0 = systemTime.getRealTime()
		console.log('GetTime0 = ' + resGetTime0)
		
		sleep(5000).then(() => {
			var resGetTime1 = systemTime.getRealTime();
			console.log('GetTime1 = ' + resGetTime1);
			assert.equal(resGetTime1 <= (resGetTime0 + 6000) && resGetTime1 >= (resGetTime0 + 5000));
			
			console.log('SUB_systemTime_getRealTime_JS_API_0100 end');
		})	 	
    })
	
	function sleep (time) {
	return new Promise((resolve) => setTimeout(resolve, time));
	}
})