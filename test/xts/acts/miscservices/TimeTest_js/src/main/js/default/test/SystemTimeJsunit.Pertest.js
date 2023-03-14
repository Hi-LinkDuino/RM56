/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
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
     * @tc.number    SUB_systemTime_setTime_JS_API_0100
     * @tc.name      Test systemTime.setTime
     * @tc.desc      Test systemTime_setTime API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTime_test1', 0, async function (done) {
        console.log("SUB_systemTime_setTime_JS_API_0100 start");
        systemTime.setTime(152600, (err, data) => { // callback形式调用异步接口
        if ((typeof err !== 'undefined') && (err.code !== 0)) {
            console.error('SetTime failed because ' + JSON.stringify(err));
            expect().assertFail();
            done();
            return;
            }
            console.info('SetTime success data : ' + JSON.stringify(data));
            expect(true).assertTrue();
            done();
            return;
        });
        console.log('SUB_systemTime_setTime_JS_API_0100 end');
    })
    
    /**
     * @tc.number    SUB_systemTime_setTimezone_JS_API_0100
     * @tc.name      Test systemTime.setTimezone timezone = 'Anadyr, Russia'
     * @tc.desc      Test systemTime_setTimezone API functionality.
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('systemTime_setTimezone_test1', 0, async function (done) {
        console.log("SUB_systemTime_setTimezone_JS_API_0100 start");
        systemTime.setTimezone('Asia/Shanghai', (err, data) => { // callback形式调用异步接口
        if ((typeof err !== 'undefined') && (err.code !== 0)) {
            console.error('setTimezone failed because ' + JSON.stringify(err));
            expect().assertFail();
            done();
            return;
            }
            console.info('setTimezone success data : ' + JSON.stringify(data));
            expect(true).assertTrue();
            done();
            return;
        });
        console.log('SUB_systemTime_setTimezone_JS_API_0100 end');
    })
})
