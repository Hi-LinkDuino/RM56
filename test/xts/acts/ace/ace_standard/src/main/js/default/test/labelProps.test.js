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


describe('labelPropsJsTest', function () {

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
        console.info('[labelPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/label/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push labelProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push labelProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    })

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[labelPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    })

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelIdProp
     * @tc.desc      ACE
     */
     it('testLabelIdProp', 0, async function (done) {
        console.info('testLabelIdProp START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelClassProp
     * @tc.desc      ACE
     */
    it('testLabelClassProp', 0, async function (done) {
        console.info('testLabelClassProp START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelClassPropNone
     * @tc.desc      ACE
     */
    it('testLabelClassPropNone', 0, async function (done) {
        console.info('testLabelClassPropNone START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[labelProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelStyleProp
     * @tc.desc      ACEs
     */
//    it('testLabelStyleProp', 0, async function (done) {
//        console.info('testLabelStyleProp START');
//        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
//
//        let obj = JSON.parse(globalThis.value.styleProp);
//        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
//        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
//
//        expect(obj.$type).assertEqual('label')
//        expect(obj.$attrs.id).assertEqual('styleProp')
//        expect(obj.$attrs.style).assertEqual(undefined)
//        console.info("[labelProps] get style value is: " + JSON.stringify(obj.$attrs.style));
//
//        expect(obj.$styles.width).assertEqual(undefined);
//        console.info("[labelProps] get style width value is: " + JSON.stringify(obj.$styles.width));
//        expect(obj.$styles.height).assertEqual(undefined);
//        console.info("[labelProps] get style height value is: " + JSON.stringify(obj.$styles.height));
//        expect(obj.$styles.get('background-color')).assertEqual(undefined);
//        done();
//    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelRefProp
     * @tc.desc      ACE
     */
    it('testLabelRefProp', 0, async function (done) {
        console.info('testLabelRefProp START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelRefPropNone
     * @tc.desc      ACE
     */
    it('testLabelRefPropNone', 0, async function (done) {
        console.info('testLabelRefPropNone START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[labelProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testLabelDisabledPropTrue', 0, async function (done) {
        console.info('testLabelDisabledPropTrue START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testLabelDisabledPropFalse', 0, async function (done) {
        console.info('testLabelDisabledPropFalse START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelDisabledPropNone
     * @tc.desc      ACE
     */
    it('testLabelDisabledPropNone', 0, async function (done) {
        console.info('testLabelDisabledPropNone START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testLabelFocusablePropTrue', 0, async function (done) {
        console.info('testLabelFocusablePropTrue START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testLabelFocusablePropFalse', 0, async function (done) {
        console.info('testLabelFocusablePropFalse START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelFocusablePropNone
     * @tc.desc      ACE
     */
    it('testLabelFocusablePropNone', 0, async function (done) {
        console.info('testLabelFocusablePropNone START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelDataProp
     * @tc.desc      ACE
     */
    it('testLabelDataProp', 0, async function (done) {
        console.info('testLabelDataProp START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.datalabel).assertEqual(undefined);
        console.info("[labelProps] get datalabel value is: " + JSON.stringify(obj.$attrs.datalabel));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelDataPropNone
     * @tc.desc      ACE
     */
    it('testLabelDataPropNone', 0, async function (done) {
        console.info('testLabelDataPropNone START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.datalabel).assertEqual(undefined)
        console.info("[labelProps] get datalabel value is: " + JSON.stringify(obj.$attrs.datalabel));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testLabelClickEffectPropSmall', 0, async function (done) {
        console.info('testLabelClickEffectPropSmall START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testLabelClickEffectPropMedium', 0, async function (done) {
        console.info('testLabelClickEffectPropMedium START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testLabelClickEffectPropLarge', 0, async function (done) {
        console.info('testLabelClickEffectPropLarge START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testLabelClickEffectPropNone', 0, async function (done) {
        console.info('testLabelClickEffectPropNone START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[labelProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelDirPropRtl
     * @tc.desc      ACE
     */
    it('testLabelDirPropRtl', 0, async function (done) {
        console.info('testLabelDirPropRtl START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelDirPropLtr
     * @tc.desc      ACE
     */
    it('testLabelDirPropLtr', 0, async function (done) {
        console.info('testLabelDirPropLtr START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelDirPropAuto
     * @tc.desc      ACE
     */
    it('testLabelDirPropAuto', 0, async function (done) {
        console.info('testLabelDirPropAuto START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelDirPropNone
     * @tc.desc      ACE
     */
    it('testLabelDirPropNone', 0, async function (done) {
        console.info('testLabelDirPropNone START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelForPropNull
     * @tc.desc      ACE
     */
    it('testLabelForPropNull', 0, async function (done) {
        console.info('testLabelForPropNull START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[labelProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelForPropOne
     * @tc.desc      ACE
     */
    it('testLabelForPropOne', 0, async function (done) {
        console.info('testLabelForPropOne START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[labelProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelForPropThree
     * @tc.desc      ACE
     */
    it('testLabelForPropThree', 0, async function (done) {
        console.info('testLabelForPropThree START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[labelProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelIfPropTrue
     * @tc.desc      ACE
     */
    it('testLabelIfPropTrue', 0, async function (done) {
        console.info('testLabelIfPropTrue START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[labelProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testLabelIfPropFalse', 0, async function (done) {
    //        console.info('testLabelIfPropFalse START');
    //        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('label')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelIfPropNone
     * @tc.desc      ACE
     */
    //    it('testLabelIfPropNone', 0, async function (done) {
    //        console.info('testLabelIfPropNone START');
    //        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('label')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelShowPropTrue
     * @tc.desc      ACE
     */
    it('testLabelShowPropTrue', 0, async function (done) {
        console.info('testLabelShowPropTrue START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[labelProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelShowPropFalse
     * @tc.desc      ACE
     */
    it('testLabelShowPropFalse', 0, async function (done) {
        console.info('testLabelShowPropFalse START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[labelProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelShowPropNone
     * @tc.desc      ACE
     */
    it('testLabelShowPropNone', 0, async function (done) {
        console.info('testLabelShowPropNone START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[labelProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLabelLabelTarget
     * @tc.desc      ACE
     */
    it('testLabelLabelTarget', 0, async function (done) {
        console.info('testLabelLabelTarget START');
        console.info("[labelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.labelTarget);
        console.info("[labelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[labelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('label')
        expect(obj.$attrs.id).assertEqual('labelTarget')
        expect(obj.$attrs.target).assertEqual('labelTarget')
        console.info("[labelProps] get target value is: " + JSON.stringify(obj.$attrs.target));
        done();
    });
});
