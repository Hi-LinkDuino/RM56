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
import hichecker from '@ohos.hichecker'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('HiCheckerTest', function () {

    beforeAll(function() {
        console.info('HiCheckerTest beforeAll called')
    })

    afterAll(function() {
        console.info('HiCheckerTest afterAll called')
    })

    beforeEach(function() {
        console.info('HiCheckerTest beforeEach called');
        hichecker.removeRule(hichecker.RULE_THREAD_CHECK_SLOW_PROCESS);
        hichecker.removeRule(hichecker.RULE_CHECK_SLOW_EVENT);
        hichecker.removeRule(hichecker.RULE_CHECK_ABILITY_CONNECTION_LEAK);
        hichecker.removeRule(hichecker.RULE_CAUTION_PRINT_LOG);
        hichecker.removeRule(hichecker.RULE_CAUTION_TRIGGER_CRASH);
    })

    afterEach(function() {
        console.info('HiCheckerTest afterEach called')
    })

    async function msleep(time) {
        let promise = new Promise((resolve, reject) => {
            setTimeout(() => resolve("done!"), time)
        });
        let result = await promise;
    }

    /**
     * @tc.number 
     * @tc.name HiCheckerTest001
     * @tc.desc 
     */
    it('HiCheckerTest001', 0, async function (done) {
        console.info('HiCheckerTest001 start');
        hichecker.addRule(hichecker.RULE_THREAD_CHECK_SLOW_PROCESS);
        let tmp = hichecker.contains(hichecker.RULE_THREAD_CHECK_SLOW_PROCESS);
        console.log('add rule success!');
        expect(tmp).assertTrue();
        done();
    })

    /**
     * @tc.number 
     * @tc.name HiCheckerTest002
     * @tc.desc 
     */
     it('HiCheckerTest002', 0, async function (done) {
        console.info('HiCheckerTest002 start');
        hichecker.addRule(hichecker.RULE_THREAD_CHECK_SLOW_PROCESS | hichecker.RULE_CHECK_SLOW_EVENT);
        hichecker.addRule(hichecker.RULE_CHECK_ABILITY_CONNECTION_LEAK | hichecker.RULE_CAUTION_PRINT_LOG | hichecker.RULE_CAUTION_TRIGGER_CRASH);
        console.log('add rule success!');
        expect(hichecker.contains(hichecker.RULE_THREAD_CHECK_SLOW_PROCESS)).assertTrue();
        expect(hichecker.contains(hichecker.RULE_CHECK_SLOW_EVENT)).assertTrue();
        expect(hichecker.contains(hichecker.RULE_CHECK_ABILITY_CONNECTION_LEAK)).assertTrue();
        expect(hichecker.contains(hichecker.RULE_CAUTION_PRINT_LOG)).assertTrue();
        expect(hichecker.contains(hichecker.RULE_CAUTION_TRIGGER_CRASH)).assertTrue();
        let tmp = (hichecker.RULE_THREAD_CHECK_SLOW_PROCESS | hichecker.RULE_CHECK_SLOW_EVENT | hichecker.RULE_CHECK_ABILITY_CONNECTION_LEAK | hichecker.RULE_CAUTION_PRINT_LOG | hichecker.RULE_CAUTION_TRIGGER_CRASH);
        expect(hichecker.getRule() == tmp).assertTrue();
        done();
    })

    /**
     * @tc.number 
     * @tc.name HiCheckerTest003
     * @tc.desc 
     */
     it('HiCheckerTest003', 0, async function (done) {
        console.info('HiCheckerTest003 start');
        hichecker.addRule(-1);
        console.log('add wrong rule!');
        expect(hichecker.contains(-1)).assertEqual(false);
        hichecker.addRule(0);
        console.log('add wrong rule!');
        expect(hichecker.contains(0)).assertEqual(false);
        hichecker.addRule(999999);
        console.log('add wrong rule!');
        expect(hichecker.contains(-1)).assertEqual(false);
        done();
    })

    /**
     * @tc.number 
     * @tc.name HiCheckerTest004
     * @tc.desc 
     */
     it('HiCheckerTest004', 0, async function (done) {
        console.info('HiCheckerTest004 start');
        hichecker.addRule(hichecker.RULE_THREAD_CHECK_SLOW_PROCESS);
        expect(hichecker.contains(hichecker.RULE_THREAD_CHECK_SLOW_PROCESS)).assertTrue();
        console.log('add rule success!');
        hichecker.removeRule(hichecker.RULE_THREAD_CHECK_SLOW_PROCESS);
        expect(hichecker.contains(hichecker.RULE_THREAD_CHECK_SLOW_PROCESS)).assertEqual(false);
        done();
    })

    /**
     * @tc.number 
     * @tc.name HiCheckerTest005
     * @tc.desc 
     */
     it('HiCheckerTest005', 0, async function (done) {
        console.info('HiCheckerTest005 start');
        hichecker.addRule(hichecker.RULE_THREAD_CHECK_SLOW_PROCESS | hichecker.RULE_CHECK_SLOW_EVENT);
        hichecker.addRule(hichecker.RULE_CHECK_ABILITY_CONNECTION_LEAK | hichecker.RULE_CAUTION_PRINT_LOG | hichecker.RULE_CAUTION_TRIGGER_CRASH);
        console.log('add rule success!');
        let tmp = (hichecker.RULE_CHECK_ABILITY_CONNECTION_LEAK | hichecker.RULE_CAUTION_PRINT_LOG | hichecker.RULE_CAUTION_TRIGGER_CRASH);
        hichecker.removeRule(hichecker.RULE_THREAD_CHECK_SLOW_PROCESS | hichecker.RULE_CHECK_SLOW_EVENT);
        expect(hichecker.getRule() == tmp).assertTrue();
        hichecker.removeRule(hichecker.RULE_CHECK_ABILITY_CONNECTION_LEAK | hichecker.RULE_CAUTION_PRINT_LOG | hichecker.RULE_CAUTION_TRIGGER_CRASH);
        expect(hichecker.getRule() == 0).assertTrue();
        done();
    })

    /**
     * @tc.number 
     * @tc.name HiCheckerTest006
     * @tc.desc 
     */
     it('HiCheckerTest006', 0, async function (done) {
        console.info('HiCheckerTest006 start');
        hichecker.addRule(hichecker.RULE_THREAD_CHECK_SLOW_PROCESS | hichecker.RULE_CHECK_SLOW_EVENT);
        hichecker.addRule(hichecker.RULE_CHECK_ABILITY_CONNECTION_LEAK | hichecker.RULE_CAUTION_PRINT_LOG | hichecker.RULE_CAUTION_TRIGGER_CRASH);
        console.log('add rule success!');
        let tmp = (hichecker.RULE_THREAD_CHECK_SLOW_PROCESS | hichecker.RULE_CHECK_SLOW_EVENT | hichecker.RULE_CHECK_ABILITY_CONNECTION_LEAK | hichecker.RULE_CAUTION_PRINT_LOG | hichecker.RULE_CAUTION_TRIGGER_CRASH);
        hichecker.removeRule(-1);
        expect(hichecker.getRule() == tmp).assertTrue();
        hichecker.removeRule(0);
        expect(hichecker.getRule() == tmp).assertTrue();
        hichecker.removeRule(999999);
        expect(hichecker.getRule() == tmp).assertTrue();
        done();
    })
    
})
