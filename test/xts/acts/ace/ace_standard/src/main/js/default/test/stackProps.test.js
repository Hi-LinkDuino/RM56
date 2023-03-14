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


describe('stackPropsJsTest', function () {

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
        console.info('[stackPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/stack/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push stackProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push stackProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[stackPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      teststackIdProp
     * @tc.desc      ACE
     */
    it('testStackIdProp', 0, async function (done) {
        console.info('testStackIdProp START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackClassProp
     * @tc.desc      ACE
     */
    it('testStackClassProp', 0, async function (done) {
        console.info('testStackClassProp START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackClassPropNone
     * @tc.desc      ACE
     */
    it('testStackClassPropNone', 0, async function (done) {
        console.info('testStackClassPropNone START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[stackProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackRefProp
     * @tc.desc      ACE
     */
    it('testStackRefProp', 0, async function (done) {
        console.info('testStackRefProp START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackRefPropNone
     * @tc.desc      ACE
     */
    it('testStackRefPropNone', 0, async function (done) {
        console.info('testStackRefPropNone START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[stackProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testStackDisabledPropTrue', 0, async function (done) {
        console.info('testStackDisabledPropTrue START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testStackDisabledPropFalse', 0, async function (done) {
        console.info('testStackDisabledPropFalse START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackDisabledPropNone
     * @tc.desc      ACE
     */
    it('testStackDisabledPropNone', 0, async function (done) {
        console.info('testStackDisabledPropNone START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testStackFocusablePropTrue', 0, async function (done) {
        console.info('testStackFocusablePropTrue START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testStackFocusablePropFalse', 0, async function (done) {
        console.info('testStackFocusablePropFalse START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackFocusablePropNone
     * @tc.desc      ACE
     */
    it('testStackFocusablePropNone', 0, async function (done) {
        console.info('testStackFocusablePropNone START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackDataProp
     * @tc.desc      ACE
     */
    it('testStackDataProp', 0, async function (done) {
        console.info('testStackDataProp START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataStack).assertEqual(undefined);
        console.info("[stackProps] get dataStack value is: " + JSON.stringify(obj.$attrs.dataStack));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackDataPropNone
     * @tc.desc      ACE
     */
    it('testStackDataPropNone', 0, async function (done) {
        console.info('testStackDataPropNone START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataStack).assertEqual(undefined)
        console.info("[stackProps] get dataStack value is: " + JSON.stringify(obj.$attrs.dataStack));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testStackClickEffectPropSmall', 0, async function (done) {
        console.info('testStackClickEffectPropSmall START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testStackClickEffectPropMedium', 0, async function (done) {
        console.info('testStackClickEffectPropMedium START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testStackClickEffectPropLarge', 0, async function (done) {
        console.info('testStackClickEffectPropLarge START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testStackClickEffectPropNone', 0, async function (done) {
        console.info('testStackClickEffectPropNone START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[stackProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackDirPropRtl
     * @tc.desc      ACE
     */
    it('testStackDirPropRtl', 0, async function (done) {
        console.info('testStackDirPropRtl START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackDirPropLtr
     * @tc.desc      ACE
     */
    it('testStackDirPropLtr', 0, async function (done) {
        console.info('testStackDirPropLtr START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackDirPropAuto
     * @tc.desc      ACE
     */
    it('testStackDirPropAuto', 0, async function (done) {
        console.info('testStackDirPropAuto START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackDirPropNone
     * @tc.desc      ACE
     */
    it('testStackDirPropNone', 0, async function (done) {
        console.info('testStackDirPropNone START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackForPropNull
     * @tc.desc      ACE
     */
    it('testStackForPropNull', 0, async function (done) {
        console.info('testStackForPropNull START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[stackProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackForPropOne
     * @tc.desc      ACE
     */
    it('testStackForPropOne', 0, async function (done) {
        console.info('testStackForPropOne START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[stackProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackForPropThree
     * @tc.desc      ACE
     */
    it('testStackForPropThree', 0, async function (done) {
        console.info('testStackForPropThree START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[stackProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackIfPropTrue
     * @tc.desc      ACE
     */
    it('testStackIfPropTrue', 0, async function (done) {
        console.info('testStackIfPropTrue START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[stackProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testStackIfPropFalse', 0, async function (done) {
    //        console.info('testStackIfPropFalse START');
    //        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('stack')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackIfPropNone
     * @tc.desc      ACE
     */
    //    it('testStackIfPropNone', 0, async function (done) {
    //        console.info('testStackIfPropNone START');
    //        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('stack')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackShowPropTrue
     * @tc.desc      ACE
     */
    it('testStackShowPropTrue', 0, async function (done) {
        console.info('testStackShowPropTrue START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[stackProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackShowPropFalse
     * @tc.desc      ACE
     */
    it('testStackShowPropFalse', 0, async function (done) {
        console.info('testStackShowPropFalse START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[stackProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackShowPropNone
     * @tc.desc      ACE
     */
    it('testStackShowPropNone', 0, async function (done) {
        console.info('testStackShowPropNone START');
        console.info("[stackProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[stackProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stackProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stack')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[stackProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
});
