/*
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import router from '@system.router';
import {describe, beforeAll,afterAll, it, expect} from 'deccjsunit/index';

describe('richtextPropsJsTest', function () {

    async function sleep(time) {
        return new Promise((resolve, reject) => {
            setTimeout(() => {
                resolve()
            }, time)
        }).then(() => {
            console.info(`sleep ${time} over...`)
        })
    }

    async function backToIndex() {
        let backToIndexPromise = new Promise((resolve, reject) => {
            setTimeout(() => {
                router.back({
                    uri: 'pages/index/index'
                });
                resolve();
            }, 500);
        });
        let clearPromise = new Promise((resolve, reject) => {
            setTimeout(() => {
                router.clear();
                resolve();
            }, 500);
        });
        await backToIndexPromise.then(() => {
            return clearPromise;
        });
    }

    /**
    * run before testcase
    */
    beforeAll(async function (done) {
        console.info('[richtextPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/richtext/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push richtextProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push richtextProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[richtextPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRichtextIdProp
     * @tc.desc      ACE
     */
    it('testRichtextIdProp', 0, async function (done) {
        console.info('testRichtextIdProp START');
        console.info("[richtextProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[richtextProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[richtextProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('richtext')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRichtextClassProp
     * @tc.desc      ACE
     */
    it('testRichtextClassProp', 0, async function (done) {
        console.info('testRichtextClassProp START');
        console.info("[richtextProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[richtextProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[richtextProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('richtext')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRichtextClassPropNone
     * @tc.desc      ACE
     */
    it('testRichtextClassPropNone', 0, async function (done) {
        console.info('testRichtextClassPropNone START');
        console.info("[richtextProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[richtextProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[richtextProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('richtext')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[richtextProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRichtextStyleProp
     * @tc.desc      ACE
     */
    it('testRichtextStyleProp', 0, async function (done) {
        console.info('testRichtextStyleProp START');
        console.info("[richtextProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.styleProp);
        console.info("[richtextProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[richtextProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('richtext')
        expect(obj.$attrs.id).assertEqual('styleProp')
        expect(obj.$attrs.style).assertEqual('visibility: visible;')
        console.info("[richtextProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });
});
