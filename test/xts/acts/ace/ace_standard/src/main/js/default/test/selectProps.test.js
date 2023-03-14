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

describe('selectPropsJsTest', function () {

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
        console.info('[selectPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/select/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push selectProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push selectProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[selectPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectIdProp
     * @tc.desc      ACE
     */
    it('testSelectIdProp', 0, async function (done) {
        console.info('testSelectIdProp START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectClassProp
     * @tc.desc      ACE
     */
    it('testSelectClassProp', 0, async function (done) {
        console.info('testSelectClassProp START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectClassPropNone
     * @tc.desc      ACE
     */
    it('testSelectClassPropNone', 0, async function (done) {
        console.info('testSelectClassPropNone START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[selectProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectStyleProp
     * @tc.desc      ACEs
     */
//    it('testSelectStyleProp', 0, async function (done) {
//        console.info('testSelectStyleProp START');
//        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
//
//        let obj = JSON.parse(globalThis.value.styleProp);
//        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
//        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
//
//        expect(obj.$type).assertEqual('select')
//        expect(obj.$attrs.id).assertEqual('styleProp')
//        expect(obj.$attrs.style).assertEqual(undefined)
//        console.info("[selectProps] get style value is: " + JSON.stringify(obj.$attrs.style));
//
//        expect(obj.$styles.width).assertEqual(undefined);
//        console.info("[selectProps] get style width value is: " + JSON.stringify(obj.$styles.width));
//        expect(obj.$styles.height).assertEqual(undefined);
//        console.info("[selectProps] get style height value is: " + JSON.stringify(obj.$styles.height));
//        expect(obj.$styles.get('background-color')).assertEqual(undefined);
//        done();
//    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectRefProp
     * @tc.desc      ACE
     */
    it('testSelectRefProp', 0, async function (done) {
        console.info('testSelectRefProp START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectRefPropNone
     * @tc.desc      ACE
     */
    it('testSelectRefPropNone', 0, async function (done) {
        console.info('testSelectRefPropNone START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[selectProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testSelectDisabledPropTrue', 0, async function (done) {
        console.info('testSelectDisabledPropTrue START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testSelectDisabledPropFalse', 0, async function (done) {
        console.info('testSelectDisabledPropFalse START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectDisabledPropNone
     * @tc.desc      ACE
     */
    it('testSelectDisabledPropNone', 0, async function (done) {
        console.info('testSelectDisabledPropNone START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testSelectFocusablePropTrue', 0, async function (done) {
        console.info('testSelectFocusablePropTrue START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testSelectFocusablePropFalse', 0, async function (done) {
        console.info('testSelectFocusablePropFalse START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectFocusablePropNone
     * @tc.desc      ACE
     */
    it('testSelectFocusablePropNone', 0, async function (done) {
        console.info('testSelectFocusablePropNone START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectDataProp
     * @tc.desc      ACE
     */
    it('testSelectDataProp', 0, async function (done) {
        console.info('testSelectDataProp START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataSelect).assertEqual(undefined);
        console.info("[selectProps] get dataSelect value is: " + JSON.stringify(obj.$attrs.dataSelect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectDataPropNone
     * @tc.desc      ACE
     */
    it('testSelectDataPropNone', 0, async function (done) {
        console.info('testSelectDataPropNone START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataSelect).assertEqual(undefined)
        console.info("[selectProps] get dataSelect value is: " + JSON.stringify(obj.$attrs.dataSelect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testSelectClickEffectPropSmall', 0, async function (done) {
        console.info('testSelectClickEffectPropSmall START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testSelectClickEffectPropMedium', 0, async function (done) {
        console.info('testSelectClickEffectPropMedium START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testSelectClickEffectPropLarge', 0, async function (done) {
        console.info('testSelectClickEffectPropLarge START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testSelectClickEffectPropNone', 0, async function (done) {
        console.info('testSelectClickEffectPropNone START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[selectProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectDirPropRtl
     * @tc.desc      ACE
     */
    it('testSelectDirPropRtl', 0, async function (done) {
        console.info('testSelectDirPropRtl START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectDirPropLtr
     * @tc.desc      ACE
     */
    it('testSelectDirPropLtr', 0, async function (done) {
        console.info('testSelectDirPropLtr START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectDirPropAuto
     * @tc.desc      ACE
     */
    it('testSelectDirPropAuto', 0, async function (done) {
        console.info('testSelectDirPropAuto START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectDirPropNone
     * @tc.desc      ACE
     */
    it('testSelectDirPropNone', 0, async function (done) {
        console.info('testSelectDirPropNone START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectForPropNull
     * @tc.desc      ACE
     */
    it('testSelectForPropNull', 0, async function (done) {
        console.info('testSelectForPropNull START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[selectProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectForPropOne
     * @tc.desc      ACE
     */
    it('testSelectForPropOne', 0, async function (done) {
        console.info('testSelectForPropOne START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[selectProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectForPropThree
     * @tc.desc      ACE
     */
    it('testSelectForPropThree', 0, async function (done) {
        console.info('testSelectForPropThree START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[selectProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectIfPropTrue
     * @tc.desc      ACE
     */
    it('testSelectIfPropTrue', 0, async function (done) {
        console.info('testSelectIfPropTrue START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[selectProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testSelectIfPropFalse', 0, async function (done) {
    //        console.info('testSelectIfPropFalse START');
    //        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('select')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectIfPropNone
     * @tc.desc      ACE
     */
    //    it('testSelectIfPropNone', 0, async function (done) {
    //        console.info('testSelectIfPropNone START');
    //        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('select')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectShowPropTrue
     * @tc.desc      ACE
     */
    it('testSelectShowPropTrue', 0, async function (done) {
        console.info('testSelectShowPropTrue START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[selectProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectShowPropFalse
     * @tc.desc      ACE
     */
    it('testSelectShowPropFalse', 0, async function (done) {
        console.info('testSelectShowPropFalse START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[selectProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSelectShowPropNone
     * @tc.desc      ACE
     */
    it('testSelectShowPropNone', 0, async function (done) {
        console.info('testSelectShowPropNone START');
        console.info("[selectProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[selectProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[selectProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('select')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[selectProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
});
