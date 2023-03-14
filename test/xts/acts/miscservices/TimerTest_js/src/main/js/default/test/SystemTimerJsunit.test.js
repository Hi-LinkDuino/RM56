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
import systemTimer from '@ohos.systemtimer'

describe('TimerTest', function() {
    console.log('start################################start');

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_0100
     * @tc.name      Test systemTimer.createTimer type = TIMER_TYPE_REALTIME
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test1',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_0100 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_0100 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_0200
     * @tc.name      Test systemTimer.createTimer type = TIMER_TYPE_REALTIME_WAKEUP
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test2',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_0200 start")
        var options = {
			type:TIMER_TYPE_WAKEUP,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_0200 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_0300
     * @tc.name      Test systemTimer.createTimer type = TIMER_TYPE_EXACT
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test3',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_0300 start")
        var options = {
			type:TIMER_TYPE_EXACT,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_0300 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_0400
     * @tc.name      Test systemTimer.createTimer type = TIMER_TYPE_REALTIME
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test4',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_0400 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_0400 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_0500
     * @tc.name      Test systemTimer.createTimer triggerTime = 0
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test5',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_0500 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 0)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_0500 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_0600
     * @tc.name      Test systemTimer.createTimer triggerTime = 5000
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test6',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_0600 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 5000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_0600 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_0700
     * @tc.name      Test systemTimer.createTimer triggerTime = Number.MAX_VALUE/2
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test7',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_0700 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, Number.MAX_VALUE/2)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_0700 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_0800
     * @tc.name      Test systemTimer.createTimer triggerTime = Number.MAX_VALUE-1
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test8',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_0800 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, Number.MAX_VALUE-1)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_0800 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_0900
     * @tc.name      Test systemTimer.createTimer triggerTime = Number.MAX_VALUE
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test9',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_0900 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, Number.MAX_VALUE)
		
		console.log("stop timer")
		ystemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_0900 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_1000
     * @tc.name      Test systemTimer.createTimer repeat = true
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test10',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_1000 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:true,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_1000 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_1100
     * @tc.name      Test systemTimer.createTimer persistent = true
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test11',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_1100 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:true
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_1100 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_1200
     * @tc.name      Test systemTimer.createTimer repeat,persistent = true
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test12',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_1200 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:true,
			persistent:true
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_1200 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_1300
     * @tc.name      Test systemTimer.createTimer create,start,stop,destroy 1000 timers
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test13',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_1300 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:false
		}
		for (var index = 0; index < 1000; index++)
		{
			console.log("create timer")
			let timer = systemTimer.createTimer(options)
			expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)

			console.log("start timer")
			systemTimer.startTimer(timer, 100000)

			console.log("stop timer")
			systemTimer.stopTimer(timer)

			console.log("destroy timer")
			systemTimer.destroyTimer(timer)
			console.log('SUB_systemTimer_Timer_JS_API_1300 end');
		}
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_1400
     * @tc.name      Test systemTimer.createTimer interval = 0
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test14',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_1400 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			interval:0,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_1400 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_1500
     * @tc.name      Test systemTimer.createTimer interval = 5000
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test15',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_1500 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			interval:5000,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_1500 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_1600
     * @tc.name      Test systemTimer.createTimer interval = Number.MAX_VALUE/2
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test16',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_1600 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			interval:Number.MAX_VALUE/2,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_1600 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_1700
     * @tc.name      Test systemTimer.createTimer interval = Number.MAX_VALUE-1
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test17',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_1700 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			interval:Number.MAX_VALUE-1,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_1700 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_1800
     * @tc.name      Test systemTimer.createTimer interval = Number.MAX_VALUE
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test18',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_1800 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			interval:Number.MAX_VALUE,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_1800 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_1900
     * @tc.name      Test systemTimer.createTimer WantAgent
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test19',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_1900 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			interval:100000,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_1900 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_2000
     * @tc.name      Test systemTimer.createTimer Called back when the timer goes off.
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test20',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_2000 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			interval:100000,
			persistent:false,
			callback:callbackFunction
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_2000 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_2100
     * @tc.name      Test systemTimer.createTimer start a not exist timer
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test21',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_2100 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start a not exist timer")
		systemTimer.startTimer(timer + 1, 100000)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_2100 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_2200
     * @tc.name      Test systemTimer.createTimer stop a not exist timer
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test22',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_2200 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop a not exist timer")
		systemTimer.stopTimer(timer + 1)
		
		console.log("stop the current timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_2200 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_2300
     * @tc.name      Test systemTimer.createTimer destroy a not exist timer
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test23',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_2300 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy a not exist timer")
		systemTimer.destroyTimer(timer + 1)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_2300 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_2400
     * @tc.name      Test systemTimer.createTimer stop a not started timer
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test24',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_2400 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("stop a not started timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_2400 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_2500
     * @tc.name      Test systemTimer.createTimer destroy a started timer
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test25',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_2500 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("destroy a started timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_2500 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_2600
     * @tc.name      Test systemTimer.createTimer repeat to start a timer
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test26',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_2600 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("start timer again")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_2600 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_2700
     * @tc.name      Test systemTimer.createTimer repeat to stop a timer
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test27',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_2700 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("stop timer again")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_2700 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_2800
     * @tc.name      Test systemTimer.createTimer repeat to destroy a timer
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test28',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_2800 start")
        var options = {
			type:TIMER_TYPE_REALTIME,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		
		console.log("destroy timer again")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_2800 end');
	});

	/**
     * @tc.number    SUB_systemTimer_Timer_JS_API_2900
     * @tc.name      Test systemTimer.createTTimer type = TIMER_TYPE_IDLE
     * @tc.desc      Test systemTimer_Timer API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
	it('systemTimer_Timer_test29',0, async () => {
		console.log("SUB_systemTimer_Timer_JS_API_2900 start")
        var options = {
			type:TIMER_TYPE_IDLE,
			repeat:false,
			persistent:false
		}
		console.log("create timer")
        let timer = systemTimer.createTimer(options)
		expect(parseInt(timer) == parseFloat(timer)).assertEqual(true)
		
		console.log("start timer")
		systemTimer.startTimer(timer, 100000)
		
		console.log("stop timer")
		systemTimer.stopTimer(timer)
		
		console.log("destroy timer")
		systemTimer.destroyTimer(timer)
		console.log('SUB_systemTimer_Timer_JS_API_2900 end');
	});

	/**
     * @function     Used for callback functions
     * @tc.name      callbackFunction
     */
	function callbackFunction()
	{
		console.log("Start to call the callback function")
	}
})