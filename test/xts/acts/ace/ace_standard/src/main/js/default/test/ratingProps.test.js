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

describe('ratingPropsJsTest', function () {

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
        console.info('[ratingPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/rating/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push ratingProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push ratingProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[ratingPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingIdProp
     * @tc.desc      ACE
     */
    it('testRatingIdProp', 0, async function (done) {
        console.info('testRatingIdProp START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingClassProp
     * @tc.desc      ACE
     */
    it('testRatingClassProp', 0, async function (done) {
        console.info('testRatingClassProp START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingClassPropNone
     * @tc.desc      ACE
     */
    it('testRatingClassPropNone', 0, async function (done) {
        console.info('testRatingClassPropNone START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[ratingProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingStyleProp
     * @tc.desc      ACEs
     */
//    it('testRatingStyleProp', 0, async function (done) {
//        console.info('testRatingStyleProp START');
//        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
//
//        let obj = JSON.parse(globalThis.value.styleProp);
//        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
//        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
//
//        expect(obj.$type).assertEqual('rating')
//        expect(obj.$attrs.id).assertEqual('styleProp')
//        expect(obj.$attrs.style).assertEqual(undefined)
//        console.info("[ratingProps] get style value is: " + JSON.stringify(obj.$attrs.style));
//
//        expect(obj.$styles.width).assertEqual(undefined);
//        console.info("[ratingProps] get style width value is: " + JSON.stringify(obj.$styles.width));
//        expect(obj.$styles.height).assertEqual(undefined);
//        console.info("[ratingProps] get style height value is: " + JSON.stringify(obj.$styles.height));
//        expect(obj.$styles.get('background-color')).assertEqual(undefined);
//        done();
//    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingRefProp
     * @tc.desc      ACE
     */
    it('testRatingRefProp', 0, async function (done) {
        console.info('testRatingRefProp START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingRefPropNone
     * @tc.desc      ACE
     */
    it('testRatingRefPropNone', 0, async function (done) {
        console.info('testRatingRefPropNone START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[ratingProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testRatingDisabledPropTrue', 0, async function (done) {
        console.info('testRatingDisabledPropTrue START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testRatingDisabledPropFalse', 0, async function (done) {
        console.info('testRatingDisabledPropFalse START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingDisabledPropNone
     * @tc.desc      ACE
     */
    it('testRatingDisabledPropNone', 0, async function (done) {
        console.info('testRatingDisabledPropNone START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testRatingFocusablePropTrue', 0, async function (done) {
        console.info('testRatingFocusablePropTrue START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testRatingFocusablePropFalse', 0, async function (done) {
        console.info('testRatingFocusablePropFalse START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingFocusablePropNone
     * @tc.desc      ACE
     */
    it('testRatingFocusablePropNone', 0, async function (done) {
        console.info('testRatingFocusablePropNone START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingDataProp
     * @tc.desc      ACE
     */
    it('testRatingDataProp', 0, async function (done) {
        console.info('testRatingDataProp START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataRating).assertEqual(undefined);
        console.info("[ratingProps] get dataRating value is: " + JSON.stringify(obj.$attrs.dataRating));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingDataPropNone
     * @tc.desc      ACE
     */
    it('testRatingDataPropNone', 0, async function (done) {
        console.info('testRatingDataPropNone START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataRating).assertEqual(undefined)
        console.info("[ratingProps] get dataRating value is: " + JSON.stringify(obj.$attrs.dataRating));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testRatingClickEffectPropSmall', 0, async function (done) {
        console.info('testRatingClickEffectPropSmall START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testRatingClickEffectPropMedium', 0, async function (done) {
        console.info('testRatingClickEffectPropMedium START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testRatingClickEffectPropLarge', 0, async function (done) {
        console.info('testRatingClickEffectPropLarge START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testRatingClickEffectPropNone', 0, async function (done) {
        console.info('testRatingClickEffectPropNone START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[ratingProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingDirPropRtl
     * @tc.desc      ACE
     */
    it('testRatingDirPropRtl', 0, async function (done) {
        console.info('testRatingDirPropRtl START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingDirPropLtr
     * @tc.desc      ACE
     */
    it('testRatingDirPropLtr', 0, async function (done) {
        console.info('testRatingDirPropLtr START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingDirPropAuto
     * @tc.desc      ACE
     */
    it('testRatingDirPropAuto', 0, async function (done) {
        console.info('testRatingDirPropAuto START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingDirPropNone
     * @tc.desc      ACE
     */
    it('testRatingDirPropNone', 0, async function (done) {
        console.info('testRatingDirPropNone START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingForPropNull
     * @tc.desc      ACE
     */
    it('testRatingForPropNull', 0, async function (done) {
        console.info('testRatingForPropNull START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[ratingProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingForPropOne
     * @tc.desc      ACE
     */
    it('testRatingForPropOne', 0, async function (done) {
        console.info('testRatingForPropOne START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[ratingProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingForPropThree
     * @tc.desc      ACE
     */
    it('testRatingForPropThree', 0, async function (done) {
        console.info('testRatingForPropThree START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[ratingProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingIfPropTrue
     * @tc.desc      ACE
     */
    it('testRatingIfPropTrue', 0, async function (done) {
        console.info('testRatingIfPropTrue START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[ratingProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testRatingIfPropFalse', 0, async function (done) {
    //        console.info('testRatingIfPropFalse START');
    //        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('rating')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingIfPropNone
     * @tc.desc      ACE
     */
    //    it('testRatingIfPropNone', 0, async function (done) {
    //        console.info('testRatingIfPropNone START');
    //        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('rating')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingShowPropTrue
     * @tc.desc      ACE
     */
    it('testRatingShowPropTrue', 0, async function (done) {
        console.info('testRatingShowPropTrue START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[ratingProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingShowPropFalse
     * @tc.desc      ACE
     */
    it('testRatingShowPropFalse', 0, async function (done) {
        console.info('testRatingShowPropFalse START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[ratingProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingShowPropNone
     * @tc.desc      ACE
     */
    it('testRatingShowPropNone', 0, async function (done) {
        console.info('testRatingShowPropNone START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[ratingProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingNumstars
     * @tc.desc      ACE
     */
    it('testRatingNumstars', 0, async function (done) {
        console.info('testRatingNumstars START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.numstars);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('numstars')
        expect(obj.$attrs.numstars).assertEqual('5')
        console.info("[ratingProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingRating
     * @tc.desc      ACE
     */
    it('testRatingRating', 0, async function (done) {
        console.info('testRatingRating START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.rating);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('rating')
        expect(obj.$attrs.rating).assertEqual('2')
        console.info("[ratingProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingStepsize
     * @tc.desc      ACE
     */
    it('testRatingStepsize', 0, async function (done) {
        console.info('testRatingStepsize START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.stepsize);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('stepsize')
        expect(obj.$attrs.stepsize).assertEqual('1')
        console.info("[ratingProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingIndicatorFalse
     * @tc.desc      ACE
     */
    it('testRatingIndicatorFalse', 0, async function (done) {
        console.info('testRatingIndicatorFalse START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indicatorFalse);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('indicatorFalse')
        expect(obj.$attrs.indicator).assertEqual('false')
        console.info("[ratingProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingIndicatorTrue
     * @tc.desc      ACE
     */
    it('testRatingIndicatorTrue', 0, async function (done) {
        console.info('testRatingIndicatorTrue START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indicatorTrue);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('indicatorTrue')
        expect(obj.$attrs.indicator).assertEqual('true')
        console.info("[ratingProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRatingIndicatorNone
     * @tc.desc      ACE
     */
    it('testRatingIndicatorNone', 0, async function (done) {
        console.info('testRatingIndicatorNone START');
        console.info("[ratingProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indicatorNone);
        console.info("[ratingProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[ratingProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('rating')
        expect(obj.$attrs.id).assertEqual('indicatorNone')
        expect(obj.$attrs.indicator).assertEqual('false')
        console.info("[ratingProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
});
