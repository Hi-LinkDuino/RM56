/**
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


describe('spanPropsJsTest', function () {

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
                })
                resolve()
            }, 500)
        })
        let clearPromise = new Promise((resolve, reject) => {
            setTimeout(() => {
                router.clear()
                resolve()
            }, 500)
        })
        await backToIndexPromise.then(() => {
            return clearPromise
        })
    }

    /**
    * run before testcase
    */
    beforeAll(async function (done) {
        console.info('[spanPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/span/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push spanProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push spanProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    })

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[spanPropsJsTest] after each called')
        await backToIndex();
        await sleep(1000)
    })

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanIdProp
     * @tc.desc      ACE
     */
    it('testSpanIdProp', 0, async function (done) {
        console.info('testSpanIdProp START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    it('testSpanClassProp', 0, async function (done) {
        console.info('testSpanClassProp START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanClassPropNone
     * @tc.desc      ACE
     */
    it('testSpanClassPropNone', 0, async function (done) {
        console.info('testSpanClassPropNone START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[spanProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanRefProp
     * @tc.desc      ACE
     */
    it('testSpanRefProp', 0, async function (done) {
        console.info('testSpanRefProp START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanRefPropNone
     * @tc.desc      ACE
     */
    it('testSpanRefPropNone', 0, async function (done) {
        console.info('testSpanRefPropNone START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[spanProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanDataProp
     * @tc.desc      ACE
     */
    it('testSpanDataProp', 0, async function (done) {
        console.info('testSpanDataProp START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataSpan).assertEqual(undefined);
        console.info("[spanProps] get dataSpan value is: " + JSON.stringify(obj.$attrs.dataSpan));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanDataPropNone
     * @tc.desc      ACE
     */
    it('testSpanDataPropNone', 0, async function (done) {
        console.info('testSpanDataPropNone START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataSpan).assertEqual(undefined)
        console.info("[spanProps] get dataSpan value is: " + JSON.stringify(obj.$attrs.dataSpan));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testSpanClickEffectPropSmall', 0, async function (done) {
        console.info('testSpanClickEffectPropSmall START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testSpanClickEffectPropMedium', 0, async function (done) {
        console.info('testSpanClickEffectPropMedium START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testSpanClickEffectPropLarge', 0, async function (done) {
        console.info('testSpanClickEffectPropLarge START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testSpanClickEffectPropNone', 0, async function (done) {
        console.info('testSpanClickEffectPropNone START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[spanProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanDirPropRtl
     * @tc.desc      ACE
     */
    it('testSpanDirPropRtl', 0, async function (done) {
        console.info('testSpanDirPropRtl START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanDirPropLtr
     * @tc.desc      ACE
     */
    it('testSpanDirPropLtr', 0, async function (done) {
        console.info('testSpanDirPropLtr START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanDirPropAuto
     * @tc.desc      ACE
     */
    it('testSpanDirPropAuto', 0, async function (done) {
        console.info('testSpanDirPropAuto START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanDirPropNone
     * @tc.desc      ACE
     */
    it('testSpanDirPropNone', 0, async function (done) {
        console.info('testSpanDirPropNone START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanForPropNull
     * @tc.desc      ACE
     */
    it('testSpanForPropNull', 0, async function (done) {
        console.info('testSpanForPropNull START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[spanProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanForPropOne
     * @tc.desc      ACE
     */
    it('testSpanForPropOne', 0, async function (done) {
        console.info('testSpanForPropOne START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[spanProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanForPropThree
     * @tc.desc      ACE
     */
    it('testSpanForPropThree', 0, async function (done) {
        console.info('testSpanForPropThree START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[spanProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanIfPropTrue
     * @tc.desc      ACE
     */
    it('testSpanIfPropTrue', 0, async function (done) {
        console.info('testSpanIfPropTrue START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[spanProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanShowPropTrue
     * @tc.desc      ACE
     */
    it('testSpanShowPropTrue', 0, async function (done) {
        console.info('testSpanShowPropTrue START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[spanProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanShowPropFalse
     * @tc.desc      ACE
     */
    it('testSpanShowPropFalse', 0, async function (done) {
        console.info('testSpanShowPropFalse START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[spanProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSpanShowPropNone
     * @tc.desc      ACE
     */
    it('testSpanShowPropNone', 0, async function (done) {
        console.info('testSpanShowPropNone START');
        console.info("[spanProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[spanProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[spanProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('span')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[spanProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
});
