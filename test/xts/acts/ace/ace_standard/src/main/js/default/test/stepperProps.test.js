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


describe('stepperPropsJsTest', function () {

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
            return clearPromise;
        })
    }

    /**
    * run before testcase
    */
    beforeAll(async function (done) {
        console.info('[stepperPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/stepper/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push stepperProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push stepperProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    })

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[stepperPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    })

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperIdProp
     * @tc.desc      ACE
     */
    it('testStepperIdProp', 0, async function (done) {
        console.info('testStepperIdProp START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperClassProp
     * @tc.desc      ACE
     */
    it('testStepperClassProp', 0, async function (done) {
        console.info('testStepperClassProp START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperClassPropNone
     * @tc.desc      ACE
     */
    it('testStepperClassPropNone', 0, async function (done) {
        console.info('testStepperClassPropNone START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[stepperProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperStyleProp
     * @tc.desc      ACEs
     */
//    it('testDivStyleProp', 0, async function (done) {
//        console.info('testDivStyleProp START');
//        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
//
//        let obj = JSON.parse(globalThis.value.styleProp);
//        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
//        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
//
//        expect(obj.$type).assertEqual('div')
//        expect(obj.$attrs.id).assertEqual('styleProp')
//        expect(obj.$attrs.style).assertEqual(undefined)
//        console.info("[divProps] get style value is: " + JSON.stringify(obj.$attrs.style));
//
//        expect(obj.$styles.width).assertEqual(undefined);
//        console.info("[divProps] get style width value is: " + JSON.stringify(obj.$styles.width));
//        expect(obj.$styles.height).assertEqual(undefined);
//        console.info("[divProps] get style height value is: " + JSON.stringify(obj.$styles.height));
//        expect(obj.$styles.get('background-color')).assertEqual(undefined);
//        done();
//    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperRefProp
     * @tc.desc      ACE
     */
    it('testStepperRefProp', 0, async function (done) {
        console.info('testStepperRefProp START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperRefPropNone
     * @tc.desc      ACE
     */
    it('testStepperRefPropNone', 0, async function (done) {
        console.info('testStepperRefPropNone START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[stepperProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testStepperDisabledPropTrue', 0, async function (done) {
        console.info('testStepperDisabledPropTrue START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testStepperDisabledPropFalse', 0, async function (done) {
        console.info('testStepperDisabledPropFalse START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperDisabledPropNone
     * @tc.desc      ACE
     */
    it('testStepperDisabledPropNone', 0, async function (done) {
        console.info('testStepperDisabledPropNone START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testStepperFocusablePropTrue', 0, async function (done) {
        console.info('testStepperFocusablePropTrue START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testStepperFocusablePropFalse', 0, async function (done) {
        console.info('testStepperFocusablePropFalse START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperFocusablePropNone
     * @tc.desc      ACE
     */
    it('testStepperFocusablePropNone', 0, async function (done) {
        console.info('testStepperFocusablePropNone START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperDataProp
     * @tc.desc      ACE
     */
    it('testStepperDataProp', 0, async function (done) {
        console.info('testStepperDataProp START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataDiv).assertEqual(undefined);
        console.info("[stepperProps] get dataDiv value is: " + JSON.stringify(obj.$attrs.dataDiv));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperDataPropNone
     * @tc.desc      ACE
     */
    it('testStepperDataPropNone', 0, async function (done) {
        console.info('testStepperDataPropNone START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataDiv).assertEqual(undefined)
        console.info("[stepperProps] get dataDiv value is: " + JSON.stringify(obj.$attrs.dataDiv));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testStepperClickEffectPropSmall', 0, async function (done) {
        console.info('testStepperClickEffectPropSmall START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testStepperClickEffectPropMedium', 0, async function (done) {
        console.info('testStepperClickEffectPropMedium START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testStepperClickEffectPropLarge', 0, async function (done) {
        console.info('testStepperClickEffectPropLarge START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testStepperClickEffectPropNone', 0, async function (done) {
        console.info('testStepperClickEffectPropNone START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[stepperProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperDirPropRtl
     * @tc.desc      ACE
     */
    it('testStepperDirPropRtl', 0, async function (done) {
        console.info('testStepperDirPropRtl START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperDirPropLtr
     * @tc.desc      ACE
     */
    it('testStepperDirPropLtr', 0, async function (done) {
        console.info('testStepperDirPropLtr START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperDirPropAuto
     * @tc.desc      ACE
     */
    it('testStepperDirPropAuto', 0, async function (done) {
        console.info('testStepperDirPropAuto START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperDirPropNone
     * @tc.desc      ACE
     */
    it('testStepperDirPropNone', 0, async function (done) {
        console.info('testStepperDirPropNone START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperForPropNull
     * @tc.desc      ACE
     */
    it('testStepperForPropNull', 0, async function (done) {
        console.info('testStepperForPropNull START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[stepperProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperForPropOne
     * @tc.desc      ACE
     */
    it('testStepperForPropOne', 0, async function (done) {
        console.info('testStepperForPropOne START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[stepperProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperForPropThree
     * @tc.desc      ACE
     */
    it('testStepperForPropThree', 0, async function (done) {
        console.info('testStepperForPropThree START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[stepperProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperIfPropTrue
     * @tc.desc      ACE
     */
    it('testStepperIfPropTrue', 0, async function (done) {
        console.info('testStepperIfPropTrue START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[stepperProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testDivIfPropFalse', 0, async function (done) {
    //        console.info('testDivIfPropFalse START');
    //        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('div')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivIfPropNone
     * @tc.desc      ACE
     */
    //    it('testDivIfPropNone', 0, async function (done) {
    //        console.info('testDivIfPropNone START');
    //        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('div')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperShowPropTrue
     * @tc.desc      ACE
     */
    it('testStepperShowPropTrue', 0, async function (done) {
        console.info('testStepperShowPropTrue START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[stepperProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperShowPropFalse
     * @tc.desc      ACE
     */
    it('testStepperShowPropFalse', 0, async function (done) {
        console.info('testStepperShowPropFalse START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[stepperProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperShowPropNone
     * @tc.desc      ACE
     */
    it('testStepperShowPropNone', 0, async function (done) {
        console.info('testStepperShowPropNone START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[stepperProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperIndexProp
     * @tc.desc      ACE
     */
    it('testStepperIndexProp', 0, async function (done) {
        console.info('testStepperIndexProp START');
        console.info("[stepperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indexProp);
        console.info("[stepperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[stepperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('stepper')
        expect(obj.$attrs.id).assertEqual('indexProp')
        expect(obj.$attrs.index).assertEqual('1')
        console.info("[stepperProps] get index value is: " + JSON.stringify(obj.$attrs.index));
        done();
    });
});
