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


describe('optionPropsJsTest', function () {

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
        })
        let clearPromise = new Promise((resolve, reject) => {
            setTimeout(() => {
                router.clear();
                resolve();
            }, 500);
        })
        await backToIndexPromise.then(() => {
            return clearPromise;
        })
    }

    /**
    * run before testcase
    */
    beforeAll(async function (done) {
        console.info('[optionPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/option/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push optionProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push optionProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    })

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[optionPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    })

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionIdProp
     * @tc.desc      ACE
     */
     it('testOptionIdProp', 0, async function (done) {
        console.info('testOptionIdProp START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionClassProp
     * @tc.desc      ACE
     */
    it('testOptionClassProp', 0, async function (done) {
        console.info('testOptionClassProp START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionClassPropNone
     * @tc.desc      ACE
     */
    it('testOptionClassPropNone', 0, async function (done) {
        console.info('testOptionClassPropNone START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[optionProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionStyleProp
     * @tc.desc      ACEs
     */
//    it('testOptionStyleProp', 0, async function (done) {
//        console.info('testOptionStyleProp START');
//        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
//
//        let obj = JSON.parse(globalThis.value.styleProp);
//        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
//        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
//
//        expect(obj.$type).assertEqual('option')
//        expect(obj.$attrs.id).assertEqual('styleProp')
//        expect(obj.$attrs.style).assertEqual(undefined)
//        console.info("[optionProps] get style value is: " + JSON.stringify(obj.$attrs.style));
//
//        expect(obj.$styles.width).assertEqual(undefined);
//        console.info("[optionProps] get style width value is: " + JSON.stringify(obj.$styles.width));
//        expect(obj.$styles.height).assertEqual(undefined);
//        console.info("[optionProps] get style height value is: " + JSON.stringify(obj.$styles.height));
//        expect(obj.$styles.get('background-color')).assertEqual(undefined);
//        done();
//    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionRefProp
     * @tc.desc      ACE
     */
    it('testOptionRefProp', 0, async function (done) {
        console.info('testOptionRefProp START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionRefPropNone
     * @tc.desc      ACE
     */
    it('testOptionRefPropNone', 0, async function (done) {
        console.info('testOptionRefPropNone START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[optionProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testOptionDisabledPropTrue', 0, async function (done) {
        console.info('testOptionDisabledPropTrue START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testOptionDisabledPropFalse', 0, async function (done) {
        console.info('testOptionDisabledPropFalse START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionDisabledPropNone
     * @tc.desc      ACE
     */
    it('testOptionDisabledPropNone', 0, async function (done) {
        console.info('testOptionDisabledPropNone START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testOptionFocusablePropTrue', 0, async function (done) {
        console.info('testOptionFocusablePropTrue START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testOptionFocusablePropFalse', 0, async function (done) {
        console.info('testOptionFocusablePropFalse START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionFocusablePropNone
     * @tc.desc      ACE
     */
    it('testOptionFocusablePropNone', 0, async function (done) {
        console.info('testOptionFocusablePropNone START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionDataProp
     * @tc.desc      ACE
     */
    it('testOptionDataProp', 0, async function (done) {
        console.info('testOptionDataProp START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataoption).assertEqual(undefined);
        console.info("[optionProps] get dataoption value is: " + JSON.stringify(obj.$attrs.dataoption));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionDataPropNone
     * @tc.desc      ACE
     */
    it('testOptionDataPropNone', 0, async function (done) {
        console.info('testOptionDataPropNone START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataoption).assertEqual(undefined)
        console.info("[optionProps] get dataoption value is: " + JSON.stringify(obj.$attrs.dataoption));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testOptionClickEffectPropSmall', 0, async function (done) {
        console.info('testOptionClickEffectPropSmall START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testOptionClickEffectPropMedium', 0, async function (done) {
        console.info('testOptionClickEffectPropMedium START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testOptionClickEffectPropLarge', 0, async function (done) {
        console.info('testOptionClickEffectPropLarge START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testOptionClickEffectPropNone', 0, async function (done) {
        console.info('testOptionClickEffectPropNone START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[optionProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionDirPropRtl
     * @tc.desc      ACE
     */
    it('testOptionDirPropRtl', 0, async function (done) {
        console.info('testOptionDirPropRtl START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionDirPropLtr
     * @tc.desc      ACE
     */
    it('testOptionDirPropLtr', 0, async function (done) {
        console.info('testOptionDirPropLtr START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionDirPropAuto
     * @tc.desc      ACE
     */
    it('testOptionDirPropAuto', 0, async function (done) {
        console.info('testOptionDirPropAuto START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionDirPropNone
     * @tc.desc      ACE
     */
    it('testOptionDirPropNone', 0, async function (done) {
        console.info('testOptionDirPropNone START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionForPropNull
     * @tc.desc      ACE
     */
    it('testOptionForPropNull', 0, async function (done) {
        console.info('testOptionForPropNull START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[optionProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionForPropOne
     * @tc.desc      ACE
     */
    it('testOptionForPropOne', 0, async function (done) {
        console.info('testOptionForPropOne START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[optionProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionForPropThree
     * @tc.desc      ACE
     */
    it('testOptionForPropThree', 0, async function (done) {
        console.info('testOptionForPropThree START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[optionProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionIfPropTrue
     * @tc.desc      ACE
     */
    it('testOptionIfPropTrue', 0, async function (done) {
        console.info('testOptionIfPropTrue START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[optionProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testOptionIfPropFalse', 0, async function (done) {
    //        console.info('testOptionIfPropFalse START');
    //        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('option')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionIfPropNone
     * @tc.desc      ACE
     */
    //    it('testOptionIfPropNone', 0, async function (done) {
    //        console.info('testOptionIfPropNone START');
    //        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('option')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionShowPropTrue
     * @tc.desc      ACE
     */
    it('testOptionShowPropTrue', 0, async function (done) {
        console.info('testOptionShowPropTrue START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[optionProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionShowPropFalse
     * @tc.desc      ACE
     */
    it('testOptionShowPropFalse', 0, async function (done) {
        console.info('testOptionShowPropFalse START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[optionProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionShowPropNone
     * @tc.desc      ACE
     */
    it('testOptionShowPropNone', 0, async function (done) {
        console.info('testOptionShowPropNone START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[optionProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionSelectedTrue
     * @tc.desc      ACE
     */
    it('testOptionSelectedTrue', 0, async function (done) {
        console.info('testOptionSelectedTrue START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.selectedTrue);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('selectedTrue')
        expect(obj.$attrs.selected).assertEqual('true')
        console.info("[optionProps] get selected value is: " + JSON.stringify(obj.$attrs.selected));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionSelectedFalse
     * @tc.desc      ACE
     */
    it('testOptionSelectedFalse', 0, async function (done) {
        console.info('testOptionSelectedFalse START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.selectedFalse);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('selectedFalse')
        expect(obj.$attrs.selected).assertEqual('false')
        console.info("[optionProps] get selected value is: " + JSON.stringify(obj.$attrs.selected));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionOptionValue
     * @tc.desc      ACE
     */
    it('testOptionOptionValue', 0, async function (done) {
        console.info('testOptionOptionValue START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.optionValue);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('optionValue')
        expect(obj.$attrs.value).assertEqual('optionValue')
        console.info("[optionProps] get value value is: " + JSON.stringify(obj.$attrs.value));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOptionOptionIcon
     * @tc.desc      ACE
     */
    it('testOptionOptionIcon', 0, async function (done) {
        console.info('testOptionOptionIcon START');
        console.info("[optionProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.optionIcon);
        console.info("[optionProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[optionProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('option')
        expect(obj.$attrs.id).assertEqual('optionIcon')
        expect(obj.$attrs.icon).assertEqual('optionIcon')
        console.info("[optionProps] get icon value is: " + JSON.stringify(obj.$attrs.icon));
        console.info("[optionProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
});
