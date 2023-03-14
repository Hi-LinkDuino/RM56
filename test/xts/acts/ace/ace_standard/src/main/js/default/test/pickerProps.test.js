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


describe('pickerPropsJsTest', function () {

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
        console.info('[pickerPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/picker/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push pickerProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push pickerProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    })

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[pickerPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    })

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerIdProp
     * @tc.desc      ACE
     */
     it('testPickerIdProp', 0, async function (done) {
        console.info('testPickerIdProp START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerClassProp
     * @tc.desc      ACE
     */
    it('testPickerClassProp', 0, async function (done) {
        console.info('testPickerClassProp START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerClassPropNone
     * @tc.desc      ACE
     */
    it('testPickerClassPropNone', 0, async function (done) {
        console.info('testPickerClassPropNone START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[pickerProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerStyleProp
     * @tc.desc      ACEs
     */
//    it('testPickerStyleProp', 0, async function (done) {
//        console.info('testPickerStyleProp START');
//        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
//
//        let obj = JSON.parse(globalThis.value.styleProp);
//        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
//        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
//
//        expect(obj.$type).assertEqual('picker')
//        expect(obj.$attrs.id).assertEqual('styleProp')
//        expect(obj.$attrs.style).assertEqual(undefined)
//        console.info("[pickerProps] get style value is: " + JSON.stringify(obj.$attrs.style));
//
//        expect(obj.$styles.width).assertEqual(undefined);
//        console.info("[pickerProps] get style width value is: " + JSON.stringify(obj.$styles.width));
//        expect(obj.$styles.height).assertEqual(undefined);
//        console.info("[pickerProps] get style height value is: " + JSON.stringify(obj.$styles.height));
//        expect(obj.$styles.get('background-color')).assertEqual(undefined);
//        done();
//    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerRefProp
     * @tc.desc      ACE
     */
    it('testPickerRefProp', 0, async function (done) {
        console.info('testPickerRefProp START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerRefPropNone
     * @tc.desc      ACE
     */
    it('testPickerRefPropNone', 0, async function (done) {
        console.info('testPickerRefPropNone START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[pickerProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testPickerDisabledPropTrue', 0, async function (done) {
        console.info('testPickerDisabledPropTrue START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testPickerDisabledPropFalse', 0, async function (done) {
        console.info('testPickerDisabledPropFalse START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerDisabledPropNone
     * @tc.desc      ACE
     */
    it('testPickerDisabledPropNone', 0, async function (done) {
        console.info('testPickerDisabledPropNone START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testPickerFocusablePropTrue', 0, async function (done) {
        console.info('testPickerFocusablePropTrue START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testPickerFocusablePropFalse', 0, async function (done) {
        console.info('testPickerFocusablePropFalse START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerFocusablePropNone
     * @tc.desc      ACE
     */
    it('testPickerFocusablePropNone', 0, async function (done) {
        console.info('testPickerFocusablePropNone START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerDataProp
     * @tc.desc      ACE
     */
    it('testPickerDataProp', 0, async function (done) {
        console.info('testPickerDataProp START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.datapicker).assertEqual(undefined);
        console.info("[pickerProps] get datapicker value is: " + JSON.stringify(obj.$attrs.datapicker));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerDataPropNone
     * @tc.desc      ACE
     */
    it('testPickerDataPropNone', 0, async function (done) {
        console.info('testPickerDataPropNone START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.datapicker).assertEqual(undefined)
        console.info("[pickerProps] get datapicker value is: " + JSON.stringify(obj.$attrs.datapicker));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testPickerClickEffectPropSmall', 0, async function (done) {
        console.info('testPickerClickEffectPropSmall START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testPickerClickEffectPropMedium', 0, async function (done) {
        console.info('testPickerClickEffectPropMedium START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testPickerClickEffectPropLarge', 0, async function (done) {
        console.info('testPickerClickEffectPropLarge START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testPickerClickEffectPropNone', 0, async function (done) {
        console.info('testPickerClickEffectPropNone START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[pickerProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerDirPropRtl
     * @tc.desc      ACE
     */
    it('testPickerDirPropRtl', 0, async function (done) {
        console.info('testPickerDirPropRtl START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerDirPropLtr
     * @tc.desc      ACE
     */
    it('testPickerDirPropLtr', 0, async function (done) {
        console.info('testPickerDirPropLtr START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerDirPropAuto
     * @tc.desc      ACE
     */
    it('testPickerDirPropAuto', 0, async function (done) {
        console.info('testPickerDirPropAuto START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerDirPropNone
     * @tc.desc      ACE
     */
    it('testPickerDirPropNone', 0, async function (done) {
        console.info('testPickerDirPropNone START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerForPropNull
     * @tc.desc      ACE
     */
    it('testPickerForPropNull', 0, async function (done) {
        console.info('testPickerForPropNull START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[pickerProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerForPropOne
     * @tc.desc      ACE
     */
    it('testPickerForPropOne', 0, async function (done) {
        console.info('testPickerForPropOne START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[pickerProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerForPropThree
     * @tc.desc      ACE
     */
    it('testPickerForPropThree', 0, async function (done) {
        console.info('testPickerForPropThree START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[pickerProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerIfPropTrue
     * @tc.desc      ACE
     */
    it('testPickerIfPropTrue', 0, async function (done) {
        console.info('testPickerIfPropTrue START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[pickerProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testPickerIfPropFalse', 0, async function (done) {
    //        console.info('testPickerIfPropFalse START');
    //        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('picker')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerIfPropNone
     * @tc.desc      ACE
     */
    //    it('testPickerIfPropNone', 0, async function (done) {
    //        console.info('testPickerIfPropNone START');
    //        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('picker')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerShowPropTrue
     * @tc.desc      ACE
     */
    it('testPickerShowPropTrue', 0, async function (done) {
        console.info('testPickerShowPropTrue START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[pickerProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerShowPropFalse
     * @tc.desc      ACE
     */
    it('testPickerShowPropFalse', 0, async function (done) {
        console.info('testPickerShowPropFalse START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[pickerProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerShowPropNone
     * @tc.desc      ACE
     */
    it('testPickerShowPropNone', 0, async function (done) {
        console.info('testPickerShowPropNone START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[pickerProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerPickerText
     * @tc.desc      ACE
     */
    it('testPickerPickerText', 0, async function (done) {
        console.info('testPickerPickerText START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.pickerText);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('pickerText')
        expect(obj.$attrs.type).assertEqual('text')
        console.info("[pickerProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerPickerTime
     * @tc.desc      ACE
     */
    it('testPickerPickerTime', 0, async function (done) {
        console.info('testPickerPickerTime START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.pickerTime);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('pickerTime')
        expect(obj.$attrs.type).assertEqual('time')
        console.info("[pickerProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerPickerDate
     * @tc.desc      ACE
     */
    it('testPickerPickerDate', 0, async function (done) {
        console.info('testPickerPickerDate START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.pickerDate);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('pickerDate')
        expect(obj.$attrs.type).assertEqual('date')
        console.info("[pickerProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerPickerDatetime
     * @tc.desc      ACE
     */
    it('testPickerPickerDatetime', 0, async function (done) {
        console.info('testPickerPickerDatetime START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.pickerDatetime);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get type value is: " + JSON.stringify(obj.$attrs.type));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('pickerDatetime')
        expect(obj.$attrs.type).assertEqual('datetime')
        console.info("[pickerProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPickerPickerMultiText
     * @tc.desc      ACE
     */
    it('testPickerPickerMultiText', 0, async function (done) {
        console.info('testPickerPickerMultiText START');
        console.info("[pickerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.pickerMultiText);
        console.info("[pickerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pickerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('picker')
        expect(obj.$attrs.id).assertEqual('pickerMultiText')
        expect(obj.$attrs.type).assertEqual('multi-text')
        console.info("[pickerProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });
});
