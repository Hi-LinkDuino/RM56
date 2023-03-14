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
import backgroundTaskManager from '@ohos.backgroundTaskManager'

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe("TransientTaskJsTest", function () {
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
     * @tc.name: TransientTaskJsTest001
     * @tc.desc: test request a suspend delay
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("TransientTaskJsTest001", 0, async function (done) {
        console.info('----------------------TransientTaskJsTest001---------------------------');
        function callback() {}
        let info = backgroundTaskManager.requestSuspendDelay("test", callback);
        if (info.requestId != -1) {
            console.info('TransientTaskJsTest001  backgroundTaskManager success, requestId:' + info.requestId);
            expect(true).assertTrue();
            backgroundTaskManager.cancelSuspendDelay(info.requestId)
        } else {
            expect(false).assertTrue();
        }
        done();
    })

    /*
     * @tc.name: TransientTaskJsTest002
     * @tc.desc: test transient task more than three
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("TransientTaskJsTest002", 0, async function (done) {
        console.info('----------------------TransientTaskJsTest002---------------------------');
        function callback() {}
        let info1 =  backgroundTaskManager.requestSuspendDelay("test", callback);
        let info2 =  backgroundTaskManager.requestSuspendDelay("test", callback);
        let info3 =  backgroundTaskManager.requestSuspendDelay("test", callback);
        let info4 =  backgroundTaskManager.requestSuspendDelay("test", callback);
        if (info4.requestId == -1) {
            console.info('TransientTaskJsTest002 backgroundTaskManager more than three');
            expect(true).assertTrue();
            backgroundTaskManager.cancelSuspendDelay(info1.requestId);
            backgroundTaskManager.cancelSuspendDelay(info2.requestId);
            backgroundTaskManager.cancelSuspendDelay(info3.requestId);
        } else {
            expect(false).assertTrue();
        }
        done();
    })

    /*
     * @tc.name: TransientTaskJsTest003
     * @tc.desc: test getRemainingDelayTime Promise
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("TransientTaskJsTest003", 0, async function (done) {
        function callback() {}
        let info = backgroundTaskManager.requestSuspendDelay("test", callback);
        if (info.requestId != -1) {
            console.info('TransientTaskJsTest003  backgroundTaskManager success, requestId:' + info.requestId);
            expect(true).assertTrue();
        } else {
            expect(false).assertTrue();
            done();
        }
        backgroundTaskManager.getRemainingDelayTime(info.requestId)
            .then(data => {
                console.info('TransientTaskJsTest003  backgroundTaskManager success, delaytime:' + data);
                expect(true).assertTrue();
                backgroundTaskManager.cancelSuspendDelay(info.requestId);
            })
            .catch(error => {
                console.info('TransientTaskJsTest003  backgroundTaskManager error');
                expect(false).assertTrue();
            });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: TransientTaskJsTest004
     * @tc.desc: test getRemainingDelayTime Callback
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("TransientTaskJsTest004", 0, async function (done) {
        function callback() {}
        let info = backgroundTaskManager.requestSuspendDelay("test", callback);
        if (info.requestId != -1) {
            console.info('TransientTaskJsTest004  backgroundTaskManager success, requestId:' + info.requestId);
            expect(true).assertTrue();
        } else {
            expect(false).assertTrue();
            done();
        }

        backgroundTaskManager.getRemainingDelayTime(info.requestId, (err, res) => {
            if (err.data === 0) {
                console.info('TransientTaskJsTest004  backgroundTaskManager success, delaytime:' + res);
                expect(true).assertTrue();

            } else {
                console.info('TransientTaskJsTest004  backgroundTaskManager error');
                expect(false).assertTrue();
            }
            backgroundTaskManager.cancelSuspendDelay(info.requestId)
        });

        setTimeout(()=>{
            done();
        }, 500);
    })

    /*
     * @tc.name: TransientTaskJsTest005
     * @tc.desc: test request a suspend delay
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("TransientTaskJsTest005", 0, async function (done) {
        console.info('----------------------TransientTaskJsTest005---------------------------');
        function callback() {}
        let info = backgroundTaskManager.requestSuspendDelay("test", callback);
        if (info.actualDealyTime != -1) {
            console.info('TransientTaskJsTest001  backgroundTaskManager success, actualDealyTime:' + 
            info.actualDealyTime);
            expect(true).assertTrue();
            backgroundTaskManager.cancelSuspendDelay(info.requestId)
        } else {
            expect(false).assertTrue();
        }
        done();
    })
	
	/*
     * @tc.name: TransientTaskJsTest006
     * @tc.desc: test DelaySuspendInfo actualDealyTime
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("TransientTaskJsTest006", 0, async function (done) {
        console.info('----------------------TransientTaskJsTest006---------------------------');
        function callback() {}
        let info = backgroundTaskManager.requestSuspendDelay("test", callback);
        if (info.requestId != -1) {
            console.info('TransientTaskJsTest006  DelaySuspendInfo actualDealyTime:' + 
            info.actualDealyTime);
            expect(true).assertTrue();
        } else {
            expect(false).assertTrue();
			done();
        }
		
		setTimeout(() => {
			done();
		},500);
    })
})
