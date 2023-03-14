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


describe('swiperPropsJsTest', function () {

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
        console.info('[swiperPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/swiper/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push swiperProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push swiperProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[swiperPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperIdProp
     * @tc.desc      ACE
     */
    it('testSwiperIdProp', 0, async function (done) {
        console.info('testSwiperIdProp START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperClassProp
     * @tc.desc      ACE
     */
    it('testSwiperClassProp', 0, async function (done) {
        console.info('testSwiperClassProp START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperClassPropNone
     * @tc.desc      ACE
     */
    it('testSwiperClassPropNone', 0, async function (done) {
        console.info('testSwiperClassPropNone START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[swiperProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperRefProp
     * @tc.desc      ACE
     */
    it('testSwiperRefProp', 0, async function (done) {
        console.info('testSwiperRefProp START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperRefPropNone
     * @tc.desc      ACE
     */
    it('testSwiperRefPropNone', 0, async function (done) {
        console.info('testSwiperRefPropNone START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[swiperProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testSwiperDisabledPropTrue', 0, async function (done) {
        console.info('testSwiperDisabledPropTrue START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testSwiperDisabledPropFalse', 0, async function (done) {
        console.info('testSwiperDisabledPropFalse START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperDisabledPropNone
     * @tc.desc      ACE
     */
    it('testSwiperDisabledPropNone', 0, async function (done) {
        console.info('testSwiperDisabledPropNone START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testSwiperFocusablePropTrue', 0, async function (done) {
        console.info('testSwiperFocusablePropTrue START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testSwiperFocusablePropFalse', 0, async function (done) {
        console.info('testSwiperFocusablePropFalse START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperFocusablePropNone
     * @tc.desc      ACE
     */
    it('testSwiperFocusablePropNone', 0, async function (done) {
        console.info('testSwiperFocusablePropNone START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperDataProp
     * @tc.desc      ACE
     */
    it('testSwiperDataProp', 0, async function (done) {
        console.info('testSwiperDataProp START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataSwiper).assertEqual(undefined);
        console.info("[swiperProps] get dataSwiper value is: " + JSON.stringify(obj.$attrs.dataSwiper));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperDataPropNone
     * @tc.desc      ACE
     */
    it('testSwiperDataPropNone', 0, async function (done) {
        console.info('testSwiperDataPropNone START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataSwiper).assertEqual(undefined)
        console.info("[swiperProps] get dataSwiper value is: " + JSON.stringify(obj.$attrs.dataSwiper));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testSwiperClickEffectPropSmall', 0, async function (done) {
        console.info('testSwiperClickEffectPropSmall START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testSwiperClickEffectPropMedium', 0, async function (done) {
        console.info('testSwiperClickEffectPropMedium START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testSwiperClickEffectPropLarge', 0, async function (done) {
        console.info('testSwiperClickEffectPropLarge START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testSwiperClickEffectPropNone', 0, async function (done) {
        console.info('testSwiperClickEffectPropNone START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[swiperProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperDirPropRtl
     * @tc.desc      ACE
     */
    it('testSwiperDirPropRtl', 0, async function (done) {
        console.info('testSwiperDirPropRtl START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperDirPropLtr
     * @tc.desc      ACE
     */
    it('testSwiperDirPropLtr', 0, async function (done) {
        console.info('testSwiperDirPropLtr START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperDirPropAuto
     * @tc.desc      ACE
     */
    it('testSwiperDirPropAuto', 0, async function (done) {
        console.info('testSwiperDirPropAuto START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperDirPropNone
     * @tc.desc      ACE
     */
    it('testSwiperDirPropNone', 0, async function (done) {
        console.info('testSwiperDirPropNone START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperForPropNull
     * @tc.desc      ACE
     */
    it('testSwiperForPropNull', 0, async function (done) {
        console.info('testSwiperForPropNull START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[swiperProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperForPropOne
     * @tc.desc      ACE
     */
    it('testSwiperForPropOne', 0, async function (done) {
        console.info('testSwiperForPropOne START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[swiperProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperForPropThree
     * @tc.desc      ACE
     */
    it('testSwiperForPropThree', 0, async function (done) {
        console.info('testSwiperForPropThree START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[swiperProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperIfPropTrue
     * @tc.desc      ACE
     */
    it('testSwiperIfPropTrue', 0, async function (done) {
        console.info('testSwiperIfPropTrue START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[swiperProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testSwiperIfPropFalse', 0, async function (done) {
    //        console.info('testSwiperIfPropFalse START');
    //        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('swiper')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperIfPropNone
     * @tc.desc      ACE
     */
    //    it('testSwiperIfPropNone', 0, async function (done) {
    //        console.info('testSwiperIfPropNone START');
    //        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('swiper')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperShowPropTrue
     * @tc.desc      ACE
     */
    it('testSwiperShowPropTrue', 0, async function (done) {
        console.info('testSwiperShowPropTrue START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[swiperProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperShowPropFalse
     * @tc.desc      ACE
     */
    it('testSwiperShowPropFalse', 0, async function (done) {
        console.info('testSwiperShowPropFalse START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[swiperProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperShowPropNone
     * @tc.desc      ACE
     */
    it('testSwiperShowPropNone', 0, async function (done) {
        console.info('testSwiperShowPropNone START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[swiperProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperIndexProp
     * @tc.desc      ACE
     */
    it('testSwiperIndexProp', 0, async function (done) {
        console.info('testSwiperIndexProp START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indexProp); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('indexProp') //ID
        expect(obj.$attrs.index).assertEqual('0')  // prop and propValue
        console.info("[swiperProps] get index value is: " + JSON.stringify(obj.$attrs.index));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperAutoplayPropTrue
     * @tc.desc      ACE
     */
    it('testSwiperAutoplayPropTrue', 0, async function (done) {
        console.info('testSwiperAutoplayPropTrue START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.autoplayPropTrue); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('autoplayPropTrue') //ID
        expect(obj.$attrs.autoplay).assertEqual('true')  // prop and propValue
        console.info("[swiperProps] get autoplay value is: " + JSON.stringify(obj.$attrs.autoplay));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperAutoplayPropFalse
     * @tc.desc      ACE
     */
    it('testSwiperAutoplayPropFalse', 0, async function (done) {
        console.info('testSwiperAutoplayPropFalse START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.autoplayPropFalse); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('autoplayPropFalse') //ID
        expect(obj.$attrs.autoplay).assertEqual('false')  // prop and propValue
        console.info("[swiperProps] get autoplay value is: " + JSON.stringify(obj.$attrs.autoplay));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperIntervalProp
     * @tc.desc      ACE
     */
    it('testSwiperIntervalProp', 0, async function (done) {
        console.info('testSwiperIntervalProp START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.intervalProp); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('intervalProp') //ID
        expect(obj.$attrs.interval).assertEqual('2000')  // prop and propValue
        console.info("[swiperProps] get interval value is: " + JSON.stringify(obj.$attrs.interval));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperIndicatorPropTrue
     * @tc.desc      ACE
     */
    it('testSwiperIndicatorPropTrue', 0, async function (done) {
        console.info('testSwiperIndicatorPropTrue START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indicatorPropTrue); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('indicatorPropTrue') //ID
        expect(obj.$attrs.indicator).assertEqual('true')  // prop and propValue
        console.info("[swiperProps] get indicator value is: " + JSON.stringify(obj.$attrs.indicator));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperIndicatorPropFalse
     * @tc.desc      ACE
     */
    it('testSwiperIndicatorPropFalse', 0, async function (done) {
        console.info('testSwiperIndicatorPropFalse START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indicatorPropFalse); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('indicatorPropFalse') //ID
        expect(obj.$attrs.indicator).assertEqual('false')  // prop and propValue
        console.info("[swiperProps] get indicator value is: " + JSON.stringify(obj.$attrs.indicator));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperDigitalPropTrue
     * @tc.desc      ACE
     */
    it('testSwiperDigitalPropTrue', 0, async function (done) {
        console.info('testSwiperDigitalPropTrue START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.digitalPropTrue); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('digitalPropTrue') //ID
        expect(obj.$attrs.digital).assertEqual('true')  // prop and propValue
        console.info("[swiperProps] get digital value is: " + JSON.stringify(obj.$attrs.digital));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperDigitalPropFalse
     * @tc.desc      ACE
     */
    it('testSwiperDigitalPropFalse', 0, async function (done) {
        console.info('testSwiperDigitalPropFalse START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.digitalPropFalse); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('digitalPropFalse') //ID
        expect(obj.$attrs.digital).assertEqual('false')  // prop and propValue
        console.info("[swiperProps] get digital value is: " + JSON.stringify(obj.$attrs.digital));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperIndicatorMaskPropTrue
     * @tc.desc      ACE
     */
    it('testSwiperIndicatorMaskPropTrue', 0, async function (done) {
        console.info('testSwiperIndicatorMaskPropTrue START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indicatorMaskPropTrue); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('indicatorMaskPropTrue') //ID
        expect(obj.$attrs.indicatormask).assertEqual('true')  // prop and propValue
        console.info("[swiperProps] get indicatormask value is: " + JSON.stringify(obj.$attrs.indicatormask));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperIndicatorMaskPropFalse
     * @tc.desc      ACE
     */
    it('testSwiperIndicatorMaskPropFalse', 0, async function (done) {
        console.info('testSwiperIndicatorMaskPropFalse START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indicatorMaskPropFalse); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('indicatorMaskPropFalse') //ID
        expect(obj.$attrs.indicatormask).assertEqual('false')  // prop and propValue
        console.info("[swiperProps] get indicatormask value is: " + JSON.stringify(obj.$attrs.indicatormask));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperIndicatorDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testSwiperIndicatorDisabledPropTrue', 0, async function (done) {
        console.info('testSwiperIndicatorDisabledPropTrue START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indicatorDisabledPropTrue); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('indicatorDisabledPropTrue') //ID
        expect(obj.$attrs.indicatordisabled).assertEqual('true')  // prop and propValue
        console.info("[swiperProps] get indicatordisabled value is: "
            + JSON.stringify(obj.$attrs.indicatordisabled));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperIndicatorDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testSwiperIndicatorDisabledPropFalse', 0, async function (done) {
        console.info('testSwiperIndicatorDisabledPropFalse START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indicatorDisabledPropTrue); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('indicatorDisabledPropFalse') //ID
        expect(obj.$attrs.indicatordisabled).assertEqual('false')  // prop and propValue
        console.info("[swiperProps] get indicatordisabled value is: " 
            + JSON.stringify(obj.$attrs.indicatordisabled));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperLoopPropTrue
     * @tc.desc      ACE
     */
    it('testSwiperLoopPropTrue', 0, async function (done) {
        console.info('testSwiperLoopPropTrue START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.loopPropTrue); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('loopPropTrue') //ID
        expect(obj.$attrs.loop).assertEqual('true')  // prop and propValue
        console.info("[swiperProps] get loop value is: " + JSON.stringify(obj.$attrs.loop));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperLoopPropFalse
     * @tc.desc      ACE
     */
    it('testSwiperLoopPropFalse', 0, async function (done) {
        console.info('testSwiperLoopPropFalse START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.loopPropFalse); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('loopPropFalse') //ID
        expect(obj.$attrs.loop).assertEqual('false')  // prop and propValue
        console.info("[swiperProps] get loop value is: " + JSON.stringify(obj.$attrs.loop));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperDurationProp
     * @tc.desc      ACE
     */
    it('testSwiperDurationProp', 0, async function (done) {
        console.info('testSwiperDurationProp START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.durationProp); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('durationProp') //ID
        expect(obj.$attrs.duration).assertEqual('3000')  // prop and propValue
        console.info("[swiperProps] get duration value is: " + JSON.stringify(obj.$attrs.duration));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperVerticalPropTrue
     * @tc.desc      ACE
     */
    it('testSwiperVerticalPropTrue', 0, async function (done) {
        console.info('testSwiperVerticalPropTrue START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.verticalPropTrue); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('verticalPropTrue') //ID
        expect(obj.$attrs.vertical).assertEqual('true')  // prop and propValue
        console.info("[swiperProps] get vertical value is: " + JSON.stringify(obj.$attrs.vertical));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperVerticalPropFalse
     * @tc.desc      ACE
     */
    it('testSwiperVerticalPropFalse', 0, async function (done) {
        console.info('testSwiperVerticalPropFalse START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.verticalPropFalse); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('verticalPropFalse') //ID
        expect(obj.$attrs.vertical).assertEqual('false')  // prop and propValue
        console.info("[swiperProps] get vertical value is: " + JSON.stringify(obj.$attrs.vertical));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperCachedSizeProp
     * @tc.desc      ACE
     */
    it('testSwiperCachedSizeProp', 0, async function (done) {
        console.info('testSwiperCachedSizeProp START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.cachedSizeProp); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('cachedSizeProp') //ID
        expect(obj.$attrs.cachedsize).assertEqual('-1')  // prop and propValue
        console.info("[swiperProps] get cachedsize value is: " + JSON.stringify(obj.$attrs.cachedsize));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperScrollEffectPropFade
     * @tc.desc      ACE
     */
    it('testSwiperScrollEffectPropFade', 0, async function (done) {
        console.info('testSwiperScrollEffectPropFade START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.scrollEffectPropFade); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('scrollEffectPropFade') //ID
        expect(obj.$attrs.scrolleffect).assertEqual('fade')  // prop and propValue
        console.info("[swiperProps] get scrolleffect value is: " + JSON.stringify(obj.$attrs.scrolleffect));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperScrollEffectPropSpring
     * @tc.desc      ACE
     */
    it('testSwiperScrollEffectPropSpring', 0, async function (done) {
        console.info('testSwiperScrollEffectPropSpring START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.scrollEffectPropSpring); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('scrollEffectPropSpring') //ID
        expect(obj.$attrs.scrolleffect).assertEqual('spring')  // prop and propValue
        console.info("[swiperProps] get scrolleffect value is: " + JSON.stringify(obj.$attrs.scrolleffect));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperDisplayModePropStretch
     * @tc.desc      ACE
     */
    it('testSwiperDisplayModePropStretch', 0, async function (done) {
        console.info('testSwiperDisplayModePropStretch START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.displayModePropStretch); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('displayModePropStretch') //ID
        expect(obj.$attrs.displaymode).assertEqual('stretch')  // prop and propValue
        console.info("[swiperProps] get displaymode value is: " + JSON.stringify(obj.$attrs.displaymode));//prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperDisplayModePropAutoLinear
     * @tc.desc      ACE
     */
    it('testSwiperDisplayModePropAutoLinear', 0, async function (done) {
        console.info('testSwiperDisplayModePropAutoLinear START');
        console.info("[swiperProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.displayModePropAutoLinear); //id
        console.info("[swiperProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[swiperProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('swiper')
        expect(obj.$attrs.id).assertEqual('displayModePropAutoLinear') //ID
        expect(obj.$attrs.displaymode).assertEqual('autoLinear')  // prop and propValue
        console.info("[swiperProps] get displaymode value is: " + JSON.stringify(obj.$attrs.displaymode));//prop
        done();
    });
});
