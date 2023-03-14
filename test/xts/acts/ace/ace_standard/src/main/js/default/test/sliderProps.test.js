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

describe('sliderPropsJsTest', function () {

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
        console.info('[sliderPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/slider/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push sliderProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push sliderProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[sliderPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderIdProp
     * @tc.desc      ACE
     */
    it('testSliderIdProp', 0, async function (done) {
        console.info('testSliderIdProp START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderClassProp
     * @tc.desc      ACE
     */
    it('testSliderClassProp', 0, async function (done) {
        console.info('testSliderClassProp START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderClassPropNone
     * @tc.desc      ACE
     */
    it('testSliderClassPropNone', 0, async function (done) {
        console.info('testSliderClassPropNone START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[sliderProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderStyleProp
     * @tc.desc      ACEs
     */
//    it('testSliderStyleProp', 0, async function (done) {
//        console.info('testSliderStyleProp START');
//        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
//
//        let obj = JSON.parse(globalThis.value.styleProp);
//        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
//        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
//
//        expect(obj.$type).assertEqual('slider')
//        expect(obj.$attrs.id).assertEqual('styleProp')
//        expect(obj.$attrs.style).assertEqual(undefined)
//        console.info("[sliderProps] get style value is: " + JSON.stringify(obj.$attrs.style));
//
//        expect(obj.$styles.width).assertEqual(undefined);
//        console.info("[sliderProps] get style width value is: " + JSON.stringify(obj.$styles.width));
//        expect(obj.$styles.height).assertEqual(undefined);
//        console.info("[sliderProps] get style height value is: " + JSON.stringify(obj.$styles.height));
//        expect(obj.$styles.get('background-color')).assertEqual(undefined);
//        done();
//    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderRefProp
     * @tc.desc      ACE
     */
    it('testSliderRefProp', 0, async function (done) {
        console.info('testSliderRefProp START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderRefPropNone
     * @tc.desc      ACE
     */
    it('testSliderRefPropNone', 0, async function (done) {
        console.info('testSliderRefPropNone START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[sliderProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testSliderDisabledPropTrue', 0, async function (done) {
        console.info('testSliderDisabledPropTrue START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testSliderDisabledPropFalse', 0, async function (done) {
        console.info('testSliderDisabledPropFalse START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderDisabledPropNone
     * @tc.desc      ACE
     */
    it('testSliderDisabledPropNone', 0, async function (done) {
        console.info('testSliderDisabledPropNone START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testSliderFocusablePropTrue', 0, async function (done) {
        console.info('testSliderFocusablePropTrue START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testSliderFocusablePropFalse', 0, async function (done) {
        console.info('testSliderFocusablePropFalse START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderFocusablePropNone
     * @tc.desc      ACE
     */
    it('testSliderFocusablePropNone', 0, async function (done) {
        console.info('testSliderFocusablePropNone START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderDataProp
     * @tc.desc      ACE
     */
    it('testSliderDataProp', 0, async function (done) {
        console.info('testSliderDataProp START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataSlider).assertEqual(undefined);
        console.info("[sliderProps] get dataSlider value is: " + JSON.stringify(obj.$attrs.dataSlider));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderDataPropNone
     * @tc.desc      ACE
     */
    it('testSliderDataPropNone', 0, async function (done) {
        console.info('testSliderDataPropNone START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataSlider).assertEqual(undefined)
        console.info("[sliderProps] get dataSlider value is: " + JSON.stringify(obj.$attrs.dataSlider));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testSliderClickEffectPropSmall', 0, async function (done) {
        console.info('testSliderClickEffectPropSmall START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testSliderClickEffectPropMedium', 0, async function (done) {
        console.info('testSliderClickEffectPropMedium START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testSliderClickEffectPropLarge', 0, async function (done) {
        console.info('testSliderClickEffectPropLarge START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testSliderClickEffectPropNone', 0, async function (done) {
        console.info('testSliderClickEffectPropNone START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[sliderProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderDirPropRtl
     * @tc.desc      ACE
     */
    it('testSliderDirPropRtl', 0, async function (done) {
        console.info('testSliderDirPropRtl START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderDirPropLtr
     * @tc.desc      ACE
     */
    it('testSliderDirPropLtr', 0, async function (done) {
        console.info('testSliderDirPropLtr START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderDirPropAuto
     * @tc.desc      ACE
     */
    it('testSliderDirPropAuto', 0, async function (done) {
        console.info('testSliderDirPropAuto START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderDirPropNone
     * @tc.desc      ACE
     */
    it('testSliderDirPropNone', 0, async function (done) {
        console.info('testSliderDirPropNone START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderForPropNull
     * @tc.desc      ACE
     */
    it('testSliderForPropNull', 0, async function (done) {
        console.info('testSliderForPropNull START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[sliderProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderForPropOne
     * @tc.desc      ACE
     */
    it('testSliderForPropOne', 0, async function (done) {
        console.info('testSliderForPropOne START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[sliderProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderForPropThree
     * @tc.desc      ACE
     */
    it('testSliderForPropThree', 0, async function (done) {
        console.info('testSliderForPropThree START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[sliderProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderIfPropTrue
     * @tc.desc      ACE
     */
    it('testSliderIfPropTrue', 0, async function (done) {
        console.info('testSliderIfPropTrue START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[sliderProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testSliderIfPropFalse', 0, async function (done) {
    //        console.info('testSliderIfPropFalse START');
    //        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('slider')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderIfPropNone
     * @tc.desc      ACE
     */
    //    it('testSliderIfPropNone', 0, async function (done) {
    //        console.info('testSliderIfPropNone START');
    //        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('slider')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderShowPropTrue
     * @tc.desc      ACE
     */
    it('testSliderShowPropTrue', 0, async function (done) {
        console.info('testSliderShowPropTrue START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderShowPropFalse
     * @tc.desc      ACE
     */
    it('testSliderShowPropFalse', 0, async function (done) {
        console.info('testSliderShowPropFalse START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderShowPropNone
     * @tc.desc      ACE
     */
    it('testSliderShowPropNone', 0, async function (done) {
        console.info('testSliderShowPropNone START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderMin
     * @tc.desc      ACE
     */
    it('testSliderMin', 0, async function (done) {
        console.info('testSliderMin START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.min);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('min')
        expect(obj.$attrs.min).assertEqual('2')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderMax
     * @tc.desc      ACE
     */
    it('testSliderMax', 0, async function (done) {
        console.info('testSliderMax START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.max);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('max')
        expect(obj.$attrs.max).assertEqual('100')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderMaxNone
     * @tc.desc      ACE
     */
    it('testSliderMaxNone', 0, async function (done) {
        console.info('testSliderMaxNone START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.maxNone);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('maxNone')
        expect(obj.$attrs.max).assertEqual('100')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
    
    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderStep
     * @tc.desc      ACE
     */
     it('testSliderStep', 0, async function (done) {
        console.info('testSliderStep START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.step);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('step')
        expect(obj.$attrs.step).assertEqual('2')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderStepNone
     * @tc.desc      ACE
     */
    it('testSliderStepNone', 0, async function (done) {
        console.info('testSliderStepNone START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.stepNone);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('stepNone')
        expect(obj.$attrs.step).assertEqual('1')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderValue
     * @tc.desc      ACE
     */
    it('testSliderValue', 0, async function (done) {
        console.info('testSliderValue START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.value);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('value')
        expect(obj.$attrs.value).assertEqual('10')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderTypeContinuous
     * @tc.desc      ACE
     */
    it('testSliderTypeContinuous', 0, async function (done) {
        console.info('testSliderTypeContinuous START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeContinuous);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('typeContinuous')
        expect(obj.$attrs.type).assertEqual('continuous')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderTypeIntermittent
     * @tc.desc      ACE
     */
    it('testSliderTypeIntermittent', 0, async function (done) {
        console.info('testSliderTypeIntermittent START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeIntermittent);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('typeIntermittent')
        expect(obj.$attrs.type).assertEqual('intermittent')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderTypeNone
     * @tc.desc      ACE
     */
    it('testSliderTypeNone', 0, async function (done) {
        console.info('testSliderTypeNone START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeNone);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('typeNone')
        expect(obj.$attrs.type).assertEqual('continuous')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
    
    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderMinicon
     * @tc.desc      ACE
     */
     it('testSliderMinicon', 0, async function (done) {
        console.info('testSliderMinicon START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.minicon);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('minicon')
        expect(obj.$attrs.minicon).assertEqual('../../../common/images/icon.png')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderMiniconNone
     * @tc.desc      ACE
     */
    it('testSliderMiniconNone', 0, async function (done) {
        console.info('testSliderMiniconNone START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.miniconNone);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('miniconNone')
        expect(obj.$attrs.minicon).assertEqual(undefined)
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
    
    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderMaxicon
     * @tc.desc      ACE
     */
     it('testSliderMaxicon', 0, async function (done) {
        console.info('testSliderMaxicon START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.maxicon);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('maxicon')
        expect(obj.$attrs.maxicon).assertEqual('../../../common/images/icon.png')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderMaxiconNone
     * @tc.desc      ACE
     */
    it('testSliderMaxiconNone', 0, async function (done) {
        console.info('testSliderMaxiconNone START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.maxiconNone);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('maxiconNone')
        expect(obj.$attrs.maxicon).assertEqual(undefined)
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
    
    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderModeInset
     * @tc.desc      ACE
     */
     it('testSliderModeOutset', 0, async function (done) {
        console.info('testSliderModeOutset START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.modeOutset);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('modeOutset')
        expect(obj.$attrs.mode).assertEqual('outset')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderModeInset
     * @tc.desc      ACE
     */
     it('testSliderModeInset', 0, async function (done) {
        console.info('testSliderModeInset START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.modeInset);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('modeInset')
        expect(obj.$attrs.mode).assertEqual('inset')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderModeNone
     * @tc.desc      ACE
     */
    it('testSliderModeNone', 0, async function (done) {
        console.info('testSliderModeNone START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.modeNone);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('modeNone')
        expect(obj.$attrs.mode).assertEqual('outset')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderShowstepsTrue
     * @tc.desc      ACE
     */
    it('testSliderShowstepsTrue', 0, async function (done) {
        console.info('testSliderShowstepsTrue START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showstepsTrue);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('showstepsTrue')
        expect(obj.$attrs.showsteps).assertEqual('true')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderShowstepsFalse
     * @tc.desc      ACE
     */
    it('testSliderShowstepsFalse', 0, async function (done) {
        console.info('testSliderShowstepsFalse START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showstepsFalse);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('showstepsFalse')
        expect(obj.$attrs.showsteps).assertEqual('false')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderShowstepsNone
     * @tc.desc      ACE
     */
    it('testSliderShowstepsNone', 0, async function (done) {
        console.info('testSliderShowstepsNone START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showstepsNone);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('showstepsNone')
        expect(obj.$attrs.showsteps).assertEqual('false')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderShowtipsTrue
     * @tc.desc      ACE
     */
    it('testSliderShowtipsTrue', 0, async function (done) {
        console.info('testSliderShowtipsTrue START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showtipsTrue);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('showtipsTrue')
        expect(obj.$attrs.showtips).assertEqual('true')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderShowtipsFalse
     * @tc.desc      ACE
     */
    it('testSliderShowtipsFalse', 0, async function (done) {
        console.info('testSliderShowtipsFalse START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showtipsFalse);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('showtipsFalse')
        expect(obj.$attrs.showtips).assertEqual('false')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSliderShowtipsNone
     * @tc.desc      ACE
     */
    it('testSliderShowtipsNone', 0, async function (done) {
        console.info('testSliderShowtipsNone START');
        console.info("[sliderProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showtipsNone);
        console.info("[sliderProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[sliderProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('slider')
        expect(obj.$attrs.id).assertEqual('showtipsNone')
        expect(obj.$attrs.showtips).assertEqual('false')
        console.info("[sliderProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
});
