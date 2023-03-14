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


describe('textPropsJsTest', function () {

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
        console.info('[textPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/text/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push textProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push textProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[textPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextIdProp
     * @tc.desc      ACE
     */
    it('testTextIdProp', 0, async function (done) {
        console.info('testTextIdProp START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextClassProp
     * @tc.desc      ACE
     */
    it('testTextClassProp', 0, async function (done) {
        console.info('testTextClassProp START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextClassPropNone
     * @tc.desc      ACE
     */
    it('testTextClassPropNone', 0, async function (done) {
        console.info('testTextClassPropNone START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[textProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });


    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextRefProp
     * @tc.desc      ACE
     */
    it('testTextRefProp', 0, async function (done) {
        console.info('testTextRefProp START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextRefPropNone
     * @tc.desc      ACE
     */
    it('testTextRefPropNone', 0, async function (done) {
        console.info('testTextRefPropNone START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[textProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testTextDisabledPropTrue', 0, async function (done) {
        console.info('testTextDisabledPropTrue START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testTextDisabledPropFalse', 0, async function (done) {
        console.info('testTextDisabledPropFalse START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextDisabledPropNone
     * @tc.desc      ACE
     */
    it('testTextDisabledPropNone', 0, async function (done) {
        console.info('testTextDisabledPropNone START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testTextFocusablePropTrue', 0, async function (done) {
        console.info('testTextFocusablePropTrue START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testTextFocusablePropFalse', 0, async function (done) {
        console.info('testTextFocusablePropFalse START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextFocusablePropNone
     * @tc.desc      ACE
     */
    it('testTextFocusablePropNone', 0, async function (done) {
        console.info('testTextFocusablePropNone START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextDataProp
     * @tc.desc      ACE
     */
    it('testTextDataProp', 0, async function (done) {
        console.info('testTextDataProp START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataText).assertEqual(undefined);
        console.info("[textProps] get dataText value is: " + JSON.stringify(obj.$attrs.dataText));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextDataPropNone
     * @tc.desc      ACE
     */
    it('testTextDataPropNone', 0, async function (done) {
        console.info('testTextDataPropNone START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataText).assertEqual(undefined)
        console.info("[textProps] get dataText value is: " + JSON.stringify(obj.$attrs.dataText));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testTextClickEffectPropSmall', 0, async function (done) {
        console.info('testTextClickEffectPropSmall START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testTextClickEffectPropMedium', 0, async function (done) {
        console.info('testTextClickEffectPropMedium START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testTextClickEffectPropLarge', 0, async function (done) {
        console.info('testTextClickEffectPropLarge START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testTextClickEffectPropNone', 0, async function (done) {
        console.info('testTextClickEffectPropNone START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[textProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextDirPropRtl
     * @tc.desc      ACE
     */
    it('testTextDirPropRtl', 0, async function (done) {
        console.info('testTextDirPropRtl START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextDirPropLtr
     * @tc.desc      ACE
     */
    it('testTextDirPropLtr', 0, async function (done) {
        console.info('testTextDirPropLtr START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextDirPropAuto
     * @tc.desc      ACE
     */
    it('testTextDirPropAuto', 0, async function (done) {
        console.info('testTextDirPropAuto START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextDirPropNone
     * @tc.desc      ACE
     */
    it('testTextDirPropNone', 0, async function (done) {
        console.info('testTextDirPropNone START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextForPropNull
     * @tc.desc      ACE
     */
    it('testTextForPropNull', 0, async function (done) {
        console.info('testTextForPropNull START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[textProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextForPropOne
     * @tc.desc      ACE
     */
    it('testTextForPropOne', 0, async function (done) {
        console.info('testTextForPropOne START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[textProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextForPropThree
     * @tc.desc      ACE
     */
    it('testTextForPropThree', 0, async function (done) {
        console.info('testTextForPropThree START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[textProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextIfPropTrue
     * @tc.desc      ACE
     */
    it('testTextIfPropTrue', 0, async function (done) {
        console.info('testTextIfPropTrue START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[textProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testTextIfPropFalse', 0, async function (done) {
    //        console.info('testTextIfPropFalse START');
    //        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('text')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextIfPropNone
     * @tc.desc      ACE
     */
    //    it('testTextIfPropNone', 0, async function (done) {
    //        console.info('testTextIfPropNone START');
    //        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('text')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPropsShowPropTrue
     * @tc.desc      ACE
     */
    it('testTextPropsShowPropTrue', 0, async function (done) {
        console.info('testTextPropsShowPropTrue START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[textProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextShowPropFalse
     * @tc.desc      ACE
     */
    it('testTextShowPropFalse', 0, async function (done) {
        console.info('testTextShowPropFalse START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[textProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextShowPropNone
     * @tc.desc      ACE
     */
    it('testTextShowPropNone', 0, async function (done) {
        console.info('testTextShowPropNone START');
        console.info("[textProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[textProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('text')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[textProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
});
