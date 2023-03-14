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


describe('divPropsJsTest', function () {

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
        console.info('[divPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/div/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push divProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push divProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[divPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivIdProp
     * @tc.desc      ACE
     */
    it('testDivIdProp', 0, async function (done) {
        console.info('testDivIdProp START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivClassProp
     * @tc.desc      ACE
     */
    it('testDivClassProp', 0, async function (done) {
        console.info('testDivClassProp START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivClassPropNone
     * @tc.desc      ACE
     */
    it('testDivClassPropNone', 0, async function (done) {
        console.info('testDivClassPropNone START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[divProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivStyleProp
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
     * @tc.name      testDivRefProp
     * @tc.desc      ACE
     */
    it('testDivRefProp', 0, async function (done) {
        console.info('testDivRefProp START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivRefPropNone
     * @tc.desc      ACE
     */
    it('testDivRefPropNone', 0, async function (done) {
        console.info('testDivRefPropNone START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[divProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testDivDisabledPropTrue', 0, async function (done) {
        console.info('testDivDisabledPropTrue START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testDivDisabledPropFalse', 0, async function (done) {
        console.info('testDivDisabledPropFalse START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivDisabledPropNone
     * @tc.desc      ACE
     */
    it('testDivDisabledPropNone', 0, async function (done) {
        console.info('testDivDisabledPropNone START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testDivFocusablePropTrue', 0, async function (done) {
        console.info('testDivFocusablePropTrue START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testDivFocusablePropFalse', 0, async function (done) {
        console.info('testDivFocusablePropFalse START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivFocusablePropNone
     * @tc.desc      ACE
     */
    it('testDivFocusablePropNone', 0, async function (done) {
        console.info('testDivFocusablePropNone START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivDataProp
     * @tc.desc      ACE
     */
    it('testDivDataProp', 0, async function (done) {
        console.info('testDivDataProp START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataDiv).assertEqual(undefined);
        console.info("[divProps] get dataDiv value is: " + JSON.stringify(obj.$attrs.dataDiv));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivDataPropNone
     * @tc.desc      ACE
     */
    it('testDivDataPropNone', 0, async function (done) {
        console.info('testDivDataPropNone START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataDiv).assertEqual(undefined)
        console.info("[divProps] get dataDiv value is: " + JSON.stringify(obj.$attrs.dataDiv));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testDivClickEffectPropSmall', 0, async function (done) {
        console.info('testDivClickEffectPropSmall START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testDivClickEffectPropMedium', 0, async function (done) {
        console.info('testDivClickEffectPropMedium START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testDivClickEffectPropLarge', 0, async function (done) {
        console.info('testDivClickEffectPropLarge START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testDivClickEffectPropNone', 0, async function (done) {
        console.info('testDivClickEffectPropNone START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[divProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivDirPropRtl
     * @tc.desc      ACE
     */
    it('testDivDirPropRtl', 0, async function (done) {
        console.info('testDivDirPropRtl START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivDirPropLtr
     * @tc.desc      ACE
     */
    it('testDivDirPropLtr', 0, async function (done) {
        console.info('testDivDirPropLtr START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivDirPropAuto
     * @tc.desc      ACE
     */
    it('testDivDirPropAuto', 0, async function (done) {
        console.info('testDivDirPropAuto START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivDirPropNone
     * @tc.desc      ACE
     */
    it('testDivDirPropNone', 0, async function (done) {
        console.info('testDivDirPropNone START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivForPropNull
     * @tc.desc      ACE
     */
    it('testDivForPropNull', 0, async function (done) {
        console.info('testDivForPropNull START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[divProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivForPropOne
     * @tc.desc      ACE
     */
    it('testDivForPropOne', 0, async function (done) {
        console.info('testDivForPropOne START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[divProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivForPropThree
     * @tc.desc      ACE
     */
    it('testDivForPropThree', 0, async function (done) {
        console.info('testDivForPropThree START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[divProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivIfPropTrue
     * @tc.desc      ACE
     */
    it('testDivIfPropTrue', 0, async function (done) {
        console.info('testDivIfPropTrue START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[divProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivIfPropFalse
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
     * @tc.name      testDivShowPropTrue
     * @tc.desc      ACE
     */
    it('testDivShowPropTrue', 0, async function (done) {
        console.info('testDivShowPropTrue START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[divProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivShowPropFalse
     * @tc.desc      ACE
     */
    it('testDivShowPropFalse', 0, async function (done) {
        console.info('testDivShowPropFalse START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[divProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivShowPropNone
     * @tc.desc      ACE
     */
    it('testDivShowPropNone', 0, async function (done) {
        console.info('testDivShowPropNone START');
        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('div')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[divProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
});
