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

describe('inputPropsJsTest', function () {

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
        console.info('[inputPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/input/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push inputProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push inputProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[inputPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testinputIdProp
     * @tc.desc      ACE
     */
    it('testInputIdProp', 0, async function (done) {
        console.info('testInputIdProp START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputClassProp
     * @tc.desc      ACE
     */
    it('testinputClassProp', 0, async function (done) {
        console.info('testInputClassProp START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputClassPropNone
     * @tc.desc      ACE
     */
    it('testInputClassPropNone', 0, async function (done) {
        console.info('testInputClassPropNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[inputProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputStyleProp
     * @tc.desc      ACEs
     */
//    it('testInputStyleProp', 0, async function (done) {
//        console.info('testInputStyleProp START');
//        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
//
//        let obj = JSON.parse(globalThis.value.styleProp);
//        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
//        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
//
//        expect(obj.$type).assertEqual('input')
//        expect(obj.$attrs.id).assertEqual('styleProp')
//        expect(obj.$attrs.style).assertEqual(undefined)
//        console.info("[inputProps] get style value is: " + JSON.stringify(obj.$attrs.style));
//
//        expect(obj.$styles.width).assertEqual(undefined);
//        console.info("[inputProps] get style width value is: " + JSON.stringify(obj.$styles.width));
//        expect(obj.$styles.height).assertEqual(undefined);
//        console.info("[inputProps] get style height value is: " + JSON.stringify(obj.$styles.height));
//        expect(obj.$styles.get('background-color')).assertEqual(undefined);
//        done();
//    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputRefProp
     * @tc.desc      ACE
     */
    it('testInputRefProp', 0, async function (done) {
        console.info('testInputRefProp START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputRefPropNone
     * @tc.desc      ACE
     */
    it('testInputRefPropNone', 0, async function (done) {
        console.info('testInputRefPropNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[inputProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testInputDisabledPropTrue', 0, async function (done) {
        console.info('testInputDisabledPropTrue START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testInputDisabledPropFalse', 0, async function (done) {
        console.info('testInputDisabledPropFalse START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputDisabledPropNone
     * @tc.desc      ACE
     */
    it('testInputDisabledPropNone', 0, async function (done) {
        console.info('testInputDisabledPropNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testInputFocusablePropTrue', 0, async function (done) {
        console.info('testInputFocusablePropTrue START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testInputFocusablePropFalse', 0, async function (done) {
        console.info('testInputFocusablePropFalse START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputFocusablePropNone
     * @tc.desc      ACE
     */
    it('testInputFocusablePropNone', 0, async function (done) {
        console.info('testInputFocusablePropNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputDataProp
     * @tc.desc      ACE
     */
    it('testInputDataProp', 0, async function (done) {
        console.info('testInputDataProp START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataInput).assertEqual(undefined);
        console.info("[inputProps] get dataInput value is: " + JSON.stringify(obj.$attrs.dataInput));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputDataPropNone
     * @tc.desc      ACE
     */
    it('testInputDataPropNone', 0, async function (done) {
        console.info('testInputDataPropNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataInput).assertEqual(undefined)
        console.info("[inputProps] get dataInput value is: " + JSON.stringify(obj.$attrs.dataInput));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testInputClickEffectPropSmall', 0, async function (done) {
        console.info('testInputClickEffectPropSmall START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testInputClickEffectPropMedium', 0, async function (done) {
        console.info('testInputClickEffectPropMedium START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testInputClickEffectPropLarge', 0, async function (done) {
        console.info('testInputClickEffectPropLarge START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testInputClickEffectPropNone', 0, async function (done) {
        console.info('testInputClickEffectPropNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[inputProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputDirPropRtl
     * @tc.desc      ACE
     */
    it('testInputDirPropRtl', 0, async function (done) {
        console.info('testInputDirPropRtl START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputDirPropLtr
     * @tc.desc      ACE
     */
    it('testInputDirPropLtr', 0, async function (done) {
        console.info('testInputDirPropLtr START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputDirPropAuto
     * @tc.desc      ACE
     */
    it('testInputDirPropAuto', 0, async function (done) {
        console.info('testInputDirPropAuto START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputDirPropNone
     * @tc.desc      ACE
     */
    it('testInputDirPropNone', 0, async function (done) {
        console.info('testInputDirPropNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputForPropNull
     * @tc.desc      ACE
     */
    it('testInputForPropNull', 0, async function (done) {
        console.info('testInputForPropNull START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[inputProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputForPropOne
     * @tc.desc      ACE
     */
    it('testInputForPropOne', 0, async function (done) {
        console.info('testInputForPropOne START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[inputProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputForPropThree
     * @tc.desc      ACE
     */
    it('testInputForPropThree', 0, async function (done) {
        console.info('testInputForPropThree START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[inputProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputIfPropTrue
     * @tc.desc      ACE
     */
    it('testInputIfPropTrue', 0, async function (done) {
        console.info('testInputIfPropTrue START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[inputProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testInputIfPropFalse', 0, async function (done) {
    //        console.info('testInputIfPropFalse START');
    //        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('input')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputIfPropNone
     * @tc.desc      ACE
     */
    //    it('testInputIfPropNone', 0, async function (done) {
    //        console.info('testInputIfPropNone START');
    //        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('input')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputShowPropTrue
     * @tc.desc      ACE
     */
    it('testInputShowPropTrue', 0, async function (done) {
        console.info('testInputShowPropTrue START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputShowPropFalse
     * @tc.desc      ACE
     */
    it('testInputShowPropFalse', 0, async function (done) {
        console.info('testInputShowPropFalse START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputShowPropNone
     * @tc.desc      ACE
     */
    it('testInputShowPropNone', 0, async function (done) {
        console.info('testInputShowPropNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputTypeText
     * @tc.desc      ACE
     */
    it('testInputTypeText', 0, async function (done) {
        console.info('testInputTypeText START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeText);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('typeText')
        expect(obj.$attrs.type).assertEqual('text')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputTypeEmail
     * @tc.desc      ACE
     */
    it('testInputTypeEmail', 0, async function (done) {
        console.info('testInputTypeEmail START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeEmail);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('typeEmail')
        expect(obj.$attrs.type).assertEqual('email')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputTypeDate
     * @tc.desc      ACE
     */
    it('testInputTypeDate', 0, async function (done) {
        console.info('testInputTypeDate START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeDate);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('typeDate')
        expect(obj.$attrs.type).assertEqual('date')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputTypeTime
     * @tc.desc      ACE
     */
    it('testInputTypeTime', 0, async function (done) {
        console.info('testInputTypeTime START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeTime);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('typeTime')
        expect(obj.$attrs.type).assertEqual('time')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputTypeNumber
     * @tc.desc      ACE
     */
    it('testInputTypeNumber', 0, async function (done) {
        console.info('testInputTypeNumber START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeNumber);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('typeNumber')
        expect(obj.$attrs.type).assertEqual('number')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputTypePassword
     * @tc.desc      ACE
     */
    it('testInputTypePassword', 0, async function (done) {
        console.info('testInputTypePassword START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typePassword);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('typePassword')
        expect(obj.$attrs.type).assertEqual('password')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputTypeButton
     * @tc.desc      ACE
     */
    it('testInputTypeButton', 0, async function (done) {
        console.info('testInputTypeButton START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeButton);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('typeButton')
        expect(obj.$attrs.type).assertEqual('button')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputTypeCheckbox
     * @tc.desc      ACE
     */
    it('testInputTypeCheckbox', 0, async function (done) {
        console.info('testInputTypeCheckbox START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeCheckbox);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('typeCheckbox')
        expect(obj.$attrs.type).assertEqual('checkbox')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputTypeRadio
     * @tc.desc      ACE
     */
    it('testInputTypeRadio', 0, async function (done) {
        console.info('testInputTypeRadio START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeRadio);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('typeRadio')
        expect(obj.$attrs.type).assertEqual('radio')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputTypeNone
     * @tc.desc      ACE
     */
    it('testInputTypeNone', 0, async function (done) {
        console.info('testInputTypeNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('typeNone')
        expect(obj.$attrs.type).assertEqual('text')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputCheckboxFalse
     * @tc.desc      ACE
     */
    it('testInputCheckboxFalse', 0, async function (done) {
        console.info('testInputCheckboxFalse START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.checkboxFalse);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('checkboxFalse')
        expect(obj.$attrs.type).assertEqual('checkbox')
        expect(obj.$attrs.checked).assertEqual('false')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputCheckboxTrue
     * @tc.desc      ACE
     */
    it('testInputCheckboxTrue', 0, async function (done) {
        console.info('testInputCheckboxTrue START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.checkboxTrue);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('checkboxTrue')
        expect(obj.$attrs.type).assertEqual('checkbox')
        expect(obj.$attrs.checked).assertEqual('true')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputCheckboxNone
     * @tc.desc      ACE
     */
    it('testInputCheckboxNone', 0, async function (done) {
        console.info('testInputCheckboxNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.checkboxNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('checkboxNone')
        expect(obj.$attrs.type).assertEqual('checkbox')
        expect(obj.$attrs.checked).assertEqual('false')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputRadioFalse
     * @tc.desc      ACE
     */
    it('testInputRadioFalse', 0, async function (done) {
        console.info('testInputRadioFalse START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.radioFalse);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('radioFalse')
        expect(obj.$attrs.type).assertEqual('radio')
        expect(obj.$attrs.checked).assertEqual('false')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputRadioTrue
     * @tc.desc      ACE
     */
    it('testInputRadioTrue', 0, async function (done) {
        console.info('testInputRadioTrue START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.radioTrue);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('radioTrue')
        expect(obj.$attrs.type).assertEqual('radio')
        expect(obj.$attrs.checked).assertEqual('true')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputRadioNone
     * @tc.desc      ACE
     */
    it('testInputRadioNone', 0, async function (done) {
        console.info('testInputRadioNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.radioNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('radioNone')
        expect(obj.$attrs.type).assertEqual('radio')
        expect(obj.$attrs.checked).assertEqual('false')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputInputName
     * @tc.desc      ACE
     */
    it('testInputInputName', 0, async function (done) {
        console.info('testInputInputName START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.inputName);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('inputName')
        expect(obj.$attrs.name).assertEqual('inputName')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    //inputName
    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputInputNameNone
     * @tc.desc      ACE
     */
    it('testInputInputNameNone', 0, async function (done) {
        console.info('testInputInputNameNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.inputNameNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('inputNameNone')
        expect(obj.$attrs.name).assertEqual(undefined)
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputInputValue
     * @tc.desc      ACE
     */
    it('testInputInputValue', 0, async function (done) {
        console.info('testInputInputValue START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.inputValue);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('inputValue')
        expect(obj.$attrs.value).assertEqual('inputValue')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputInputValueNone
     * @tc.desc      ACE
     */
    it('testInputInputValueNone', 0, async function (done) {
        console.info('testInputInputValueNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.inputValueNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('inputValueNone')
        expect(obj.$attrs.value).assertEqual(undefined)
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputTextPlaceholder
     * @tc.desc      ACE
     */
    it('testInputTextPlaceholder', 0, async function (done) {
        console.info('testInputTextPlaceholder START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.textPlaceholder);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('textPlaceholder')
        expect(obj.$attrs.type).assertEqual('text')
        expect(obj.$attrs.placeholder).assertEqual('placeholder')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputTextPlaceholderNone
     * @tc.desc      ACE
     */
    it('testInputTextPlaceholderNone', 0, async function (done) {
        console.info('testInputTextPlaceholderNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.textPlaceholderNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('textPlaceholderNone')
        expect(obj.$attrs.type).assertEqual('text')
        expect(obj.$attrs.placeholder).assertEqual(undefined)
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputEmailPlaceholder
     * @tc.desc      ACE
     */
    it('testInputEmailPlaceholder', 0, async function (done) {
        console.info('testInputEmailPlaceholder START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.emailPlaceholder);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('emailPlaceholder')
        expect(obj.$attrs.type).assertEqual('email')
        expect(obj.$attrs.placeholder).assertEqual('placeholder')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputEmailPlaceholderNone
     * @tc.desc      ACE
     */
    it('testInputEmailPlaceholderNone', 0, async function (done) {
        console.info('testInputEmailPlaceholderNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.emailPlaceholderNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('emailPlaceholderNone')
        expect(obj.$attrs.type).assertEqual('email')
        expect(obj.$attrs.placeholder).assertEqual(undefined)
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputDatePlaceholder
     * @tc.desc      ACE
     */
    it('testInputDatePlaceholder', 0, async function (done) {
        console.info('testInputDatePlaceholder START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.datePlaceholder);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('datePlaceholder')
        expect(obj.$attrs.type).assertEqual('date')
        expect(obj.$attrs.placeholder).assertEqual('placeholder')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputDatePlaceholderNone
     * @tc.desc      ACE
     */
    it('testInputDatePlaceholderNone', 0, async function (done) {
        console.info('testInputDatePlaceholderNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.datePlaceholderNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('datePlaceholderNone')
        expect(obj.$attrs.type).assertEqual('date')
        expect(obj.$attrs.placeholder).assertEqual(undefined)
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
    
    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputTimePlaceholder
     * @tc.desc      ACE
     */
     it('testInputTimePlaceholder', 0, async function (done) {
        console.info('testInputTimePlaceholder START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.timePlaceholder);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('timePlaceholder')
        expect(obj.$attrs.type).assertEqual('time')
        expect(obj.$attrs.placeholder).assertEqual('placeholder')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputTimePlaceholderNone
     * @tc.desc      ACE
     */
    it('testInputTimePlaceholderNone', 0, async function (done) {
        console.info('testInputTimePlaceholderNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.timePlaceholderNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('timePlaceholderNone')
        expect(obj.$attrs.type).assertEqual('time')
        expect(obj.$attrs.placeholder).assertEqual(undefined)
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
    
    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputNumberPlaceholder
     * @tc.desc      ACE
     */
     it('testInputNumberPlaceholder', 0, async function (done) {
        console.info('testInputNumberPlaceholder START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.numberPlaceholder);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('numberPlaceholder')
        expect(obj.$attrs.type).assertEqual('number')
        expect(obj.$attrs.placeholder).assertEqual('placeholder')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputNumberPlaceholderNone
     * @tc.desc      ACE
     */
    it('testInputNumberPlaceholderNone', 0, async function (done) {
        console.info('testInputNumberPlaceholderNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.numberPlaceholderNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('numberPlaceholderNone')
        expect(obj.$attrs.type).assertEqual('number')
        expect(obj.$attrs.placeholder).assertEqual(undefined)
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
    
    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputPasswordPlaceholder
     * @tc.desc      ACE
     */
     it('testInputPasswordPlaceholder', 0, async function (done) {
        console.info('testInputPasswordPlaceholder START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.passwordPlaceholder);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('passwordPlaceholder')
        expect(obj.$attrs.type).assertEqual('password')
        expect(obj.$attrs.placeholder).assertEqual('placeholder')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputPasswordPlaceholderNone
     * @tc.desc      ACE
     */
    it('testInputPasswordPlaceholderNone', 0, async function (done) {
        console.info('testInputPasswordPlaceholderNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.passwordPlaceholderNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('passwordPlaceholderNone')
        expect(obj.$attrs.type).assertEqual('password')
        expect(obj.$attrs.placeholder).assertEqual(undefined)
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputMaxlength
     * @tc.desc      ACE
     */
    it('testInputMaxlength', 0, async function (done) {
        console.info('testInputMaxlength START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.maxlength);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('maxlength')
        expect(obj.$attrs.maxlength).assertEqual('10')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputEnterkeytypeDefault
     * @tc.desc      ACE
     */
    it('testInputEnterkeytypeDefault', 0, async function (done) {
        console.info('testInputEnterkeytypeDefault START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.enterkeytypeDefault);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('enterkeytypeDefault')
        expect(obj.$attrs.enterkeytype).assertEqual('default')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputEnterkeytypeNext
     * @tc.desc      ACE
     */
    it('testInputEnterkeytypeNext', 0, async function (done) {
        console.info('testInputEnterkeytypeNext START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.enterkeytypeNext);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('enterkeytypeNext')
        expect(obj.$attrs.enterkeytype).assertEqual('next')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputEnterkeytypeGo
     * @tc.desc      ACE
     */
    it('testInputEnterkeytypeGo', 0, async function (done) {
        console.info('testInputEnterkeytypeGo START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.enterkeytypeGo);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('enterkeytypeGo')
        expect(obj.$attrs.enterkeytype).assertEqual('go')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputEnterkeytypeDone
     * @tc.desc      ACE
     */
    it('testInputEnterkeytypeDone', 0, async function (done) {
        console.info('testInputEnterkeytypeDone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.enterkeytypeDone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('enterkeytypeDone')
        expect(obj.$attrs.enterkeytype).assertEqual('done')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputEnterkeytypeSend
     * @tc.desc      ACE
     */
    it('testInputEnterkeytypeSend', 0, async function (done) {
        console.info('testInputEnterkeytypeSend START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.enterkeytypeSend);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('enterkeytypeSend')
        expect(obj.$attrs.enterkeytype).assertEqual('send')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputEnterkeytypeSearch
     * @tc.desc      ACE
     */
    it('testInputEnterkeytypeSearch', 0, async function (done) {
        console.info('testInputEnterkeytypeSearch START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.enterkeytypeSearch);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('enterkeytypeSearch')
        expect(obj.$attrs.enterkeytype).assertEqual('search')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputHeadericon
     * @tc.desc      ACE
     */
    it('testInputHeadericon', 0, async function (done) {
        console.info('testInputHeadericon START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.headericon);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('headericon')
        expect(obj.$attrs.headericon).assertEqual('../../../common/images/icon.png')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputHeadericonNone
     * @tc.desc      ACE
     */
    it('testInputHeadericonNone', 0, async function (done) {
        console.info('testInputHeadericonNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.headericonNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('headericonNone')
        expect(obj.$attrs.headericon).assertEqual(undefined)
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputShowcounterTrue
     * @tc.desc      ACE
     */
    it('testInputShowcounterTrue', 0, async function (done) {
        console.info('testInputShowcounterTrue START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showcounterTrue);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('showcounterTrue')
        expect(obj.$attrs.maxlength).assertEqual('10')
        expect(obj.$attrs.showcounter).assertEqual('true')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputShowcounterFalse
     * @tc.desc      ACE
     */
    it('testInputShowcounterFalse', 0, async function (done) {
        console.info('testInputShowcounterFalse START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showcounterFalse);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('showcounterFalse')
        expect(obj.$attrs.maxlength).assertEqual('10')
        expect(obj.$attrs.showcounter).assertEqual('false')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputShowcounterNone
     * @tc.desc      ACE
     */
    it('testInputShowcounterNone', 0, async function (done) {
        console.info('testInputShowcounterNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showcounterNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('showcounterNone')
        expect(obj.$attrs.maxlength).assertEqual(undefined)
        expect(obj.$attrs.showcounter).assertEqual('false')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputSelectedstart
     * @tc.desc      ACE
     */
    it('testInputSelectedstart', 0, async function (done) {
        console.info('testInputSelectedstart START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.selectedstart);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('selectedstart')
        expect(obj.$attrs.selectedstart).assertEqual('0')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputSelectedstartNone
     * @tc.desc      ACE
     */
    it('testInputSelectedstartNone', 0, async function (done) {
        console.info('testInputSelectedstartNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.selectedstartNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('selectedstartNone')
        expect(obj.$attrs.selectedstart).assertEqual('-1')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
    
    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputSelectedendTrue
     * @tc.desc      ACE
     */
     it('testInputSelectedendTrue', 0, async function (done) {
        console.info('testInputSelectedendTrue START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.selectedendTrue);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('selectedendTrue')
        expect(obj.$attrs.selectedend).assertEqual('10')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputSelectedendNone
     * @tc.desc      ACE
     */
    it('testInputSelectedendNone', 0, async function (done) {
        console.info('testInputSelectedendNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.selectedendNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('selectedendNone')
        expect(obj.$attrs.selectedend).assertEqual('-1')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
    
    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputSoftkeyboardenabledTrue
     * @tc.desc      ACE
     */
     it('testInputSoftkeyboardenabledTrue', 0, async function (done) {
        console.info('testInputSoftkeyboardenabledTrue START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.softkeyboardenabledTrue);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('softkeyboardenabledTrue')
        expect(obj.$attrs.softkeyboardenabled).assertEqual('true')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputSoftkeyboardenabledFalse
     * @tc.desc      ACE
     */
    it('testInputSoftkeyboardenabledFalse', 0, async function (done) {
        console.info('testInputSoftkeyboardenabledFalse START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.softkeyboardenabledFalse);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('softkeyboardenabledFalse')
        expect(obj.$attrs.softkeyboardenabled).assertEqual('false')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputSoftkeyboardenabledNone
     * @tc.desc      ACE
     */
    it('testInputSoftkeyboardenabledNone', 0, async function (done) {
        console.info('testInputSoftkeyboardenabledNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.softkeyboardenabledNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('softkeyboardenabledNone')
        expect(obj.$attrs.softkeyboardenabled).assertEqual('true')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
    
    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputShowpasswordiconTrue
     * @tc.desc      ACE
     */
     it('testInputShowpasswordiconTrue', 0, async function (done) {
        console.info('testInputShowpasswordiconTrue START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showpasswordiconTrue);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('showpasswordiconTrue')
        expect(obj.$attrs.type).assertEqual('password')
        expect(obj.$attrs.showpasswordicon).assertEqual('true')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputShowpasswordiconFalse
     * @tc.desc      ACE
     */
    it('testInputShowpasswordiconFalse', 0, async function (done) {
        console.info('testInputShowpasswordiconFalse START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showpasswordiconFalse);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('showpasswordiconFalse')
        expect(obj.$attrs.type).assertEqual('password')
        expect(obj.$attrs.showpasswordicon).assertEqual('false')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testInputShowpasswordiconNone
     * @tc.desc      ACE
     */
    it('testInputShowpasswordiconNone', 0, async function (done) {
        console.info('testInputShowpasswordiconNone START');
        console.info("[inputProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showpasswordiconNone);
        console.info("[inputProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[inputProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('input')
        expect(obj.$attrs.id).assertEqual('showpasswordiconNone')
        expect(obj.$attrs.type).assertEqual('password')
        expect(obj.$attrs.showpasswordicon).assertEqual('true')
        console.info("[inputProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
});
