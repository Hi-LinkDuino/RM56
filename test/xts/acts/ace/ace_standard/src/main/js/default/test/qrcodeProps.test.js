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

describe('qrcodePropsJsTest', function () {

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
        console.info('[qrcodePropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/qrcode/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push qrcodeProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push qrcodeProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[qrcodePropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testqrcodeIdProp
     * @tc.desc      ACE
     */
    it('testQrcodeIdProp', 0, async function (done) {
        console.info('testQrcodeIdProp START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('idProp')
        expect(obj.$attrs.value).assertEqual('value')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeClassProp
     * @tc.desc      ACE
     */
    it('testQrcodeClassProp', 0, async function (done) {
        console.info('testQrcodeClassProp START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        expect(obj.$attrs.value).assertEqual('value')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeClassPropNone
     * @tc.desc      ACE
     */
    it('testQrcodeClassPropNone', 0, async function (done) {
        console.info('testQrcodeClassPropNone START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        expect(obj.$attrs.value).assertEqual('value')
        console.info("[qrcodeProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeStyleProp
     * @tc.desc      ACEs
     */
//    it('testQrcodeStyleProp', 0, async function (done) {
//        console.info('testQrcodeStyleProp START');
//        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
//
//        let obj = JSON.parse(globalThis.value.styleProp);
//        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
//        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
//
//        expect(obj.$type).assertEqual('qrcode')
//        expect(obj.$attrs.id).assertEqual('styleProp')
//        expect(obj.$attrs.style).assertEqual(undefined)
//        console.info("[qrcodeProps] get style value is: " + JSON.stringify(obj.$attrs.style));
//
//        expect(obj.$styles.width).assertEqual(undefined);
//        console.info("[qrcodeProps] get style width value is: " + JSON.stringify(obj.$styles.width));
//        expect(obj.$styles.height).assertEqual(undefined);
//        console.info("[qrcodeProps] get style height value is: " + JSON.stringify(obj.$styles.height));
//        expect(obj.$styles.get('background-color')).assertEqual(undefined);
        // expect(obj.$attrs.value).assertEqual('value')
//        done();
//    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeRefProp
     * @tc.desc      ACE
     */
    it('testQrcodeRefProp', 0, async function (done) {
        console.info('testQrcodeRefProp START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        expect(obj.$attrs.value).assertEqual('value')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeRefPropNone
     * @tc.desc      ACE
     */
    it('testQrcodeRefPropNone', 0, async function (done) {
        console.info('testQrcodeRefPropNone START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        expect(obj.$attrs.value).assertEqual('value')
        console.info("[qrcodeProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testQrcodeDisabledPropTrue', 0, async function (done) {
        console.info('testQrcodeDisabledPropTrue START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        expect(obj.$attrs.value).assertEqual('value')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testQrcodeDisabledPropFalse', 0, async function (done) {
        console.info('testQrcodeDisabledPropFalse START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        expect(obj.$attrs.value).assertEqual('value')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeDisabledPropNone
     * @tc.desc      ACE
     */
    it('testQrcodeDisabledPropNone', 0, async function (done) {
        console.info('testQrcodeDisabledPropNone START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        expect(obj.$attrs.value).assertEqual('value')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testQrcodeFocusablePropTrue', 0, async function (done) {
        console.info('testQrcodeFocusablePropTrue START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        expect(obj.$attrs.value).assertEqual('value')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testQrcodeFocusablePropFalse', 0, async function (done) {
        console.info('testQrcodeFocusablePropFalse START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        expect(obj.$attrs.value).assertEqual('value')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeFocusablePropNone
     * @tc.desc      ACE
     */
    it('testQrcodeFocusablePropNone', 0, async function (done) {
        console.info('testQrcodeFocusablePropNone START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        expect(obj.$attrs.value).assertEqual('value')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeDataProp
     * @tc.desc      ACE
     */
    it('testQrcodeDataProp', 0, async function (done) {
        console.info('testQrcodeDataProp START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataQrcode).assertEqual(undefined);
        expect(obj.$attrs.value).assertEqual('value')
        console.info("[qrcodeProps] get dataQrcode value is: " + JSON.stringify(obj.$attrs.dataQrcode));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeDataPropNone
     * @tc.desc      ACE
     */
    it('testQrcodeDataPropNone', 0, async function (done) {
        console.info('testQrcodeDataPropNone START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataQrcode).assertEqual(undefined)
        expect(obj.$attrs.value).assertEqual('value')
        console.info("[qrcodeProps] get dataQrcode value is: " + JSON.stringify(obj.$attrs.dataQrcode));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testQrcodeClickEffectPropSmall', 0, async function (done) {
        console.info('testQrcodeClickEffectPropSmall START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        expect(obj.$attrs.value).assertEqual('value')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testQrcodeClickEffectPropMedium', 0, async function (done) {
        console.info('testQrcodeClickEffectPropMedium START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        expect(obj.$attrs.value).assertEqual('value')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testQrcodeClickEffectPropLarge', 0, async function (done) {
        console.info('testQrcodeClickEffectPropLarge START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        expect(obj.$attrs.value).assertEqual('value')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testQrcodeClickEffectPropNone', 0, async function (done) {
        console.info('testQrcodeClickEffectPropNone START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        expect(obj.$attrs.value).assertEqual('value')
        console.info("[qrcodeProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeDirPropRtl
     * @tc.desc      ACE
     */
    it('testQrcodeDirPropRtl', 0, async function (done) {
        console.info('testQrcodeDirPropRtl START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        expect(obj.$attrs.value).assertEqual('value')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeDirPropLtr
     * @tc.desc      ACE
     */
    it('testQrcodeDirPropLtr', 0, async function (done) {
        console.info('testQrcodeDirPropLtr START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        expect(obj.$attrs.value).assertEqual('value')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeDirPropAuto
     * @tc.desc      ACE
     */
    it('testQrcodeDirPropAuto', 0, async function (done) {
        console.info('testQrcodeDirPropAuto START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        expect(obj.$attrs.value).assertEqual('value')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeDirPropNone
     * @tc.desc      ACE
     */
    it('testQrcodeDirPropNone', 0, async function (done) {
        console.info('testQrcodeDirPropNone START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        expect(obj.$attrs.value).assertEqual('value')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeForPropNull
     * @tc.desc      ACE
     */
    it('testQrcodeForPropNull', 0, async function (done) {
        console.info('testQrcodeForPropNull START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        expect(obj.$attrs.value).assertEqual('value')
        console.info("[qrcodeProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeForPropOne
     * @tc.desc      ACE
     */
    it('testQrcodeForPropOne', 0, async function (done) {
        console.info('testQrcodeForPropOne START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        expect(obj.$attrs.value).assertEqual('value')
        console.info("[qrcodeProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeForPropThree
     * @tc.desc      ACE
     */
    it('testQrcodeForPropThree', 0, async function (done) {
        console.info('testQrcodeForPropThree START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        expect(obj.$attrs.value).assertEqual('value')
        console.info("[qrcodeProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeIfPropTrue
     * @tc.desc      ACE
     */
    it('testQrcodeIfPropTrue', 0, async function (done) {
        console.info('testQrcodeIfPropTrue START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        expect(obj.$attrs.value).assertEqual('value')
        console.info("[qrcodeProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testQrcodeIfPropFalse', 0, async function (done) {
    //        console.info('testQrcodeIfPropFalse START');
    //        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('qrcode')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
        // expect(obj.$attrs.value).assertEqual('value')
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeIfPropNone
     * @tc.desc      ACE
     */
    //    it('testQrcodeIfPropNone', 0, async function (done) {
    //        console.info('testQrcodeIfPropNone START');
    //        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('qrcode')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
        // expect(obj.$attrs.value).assertEqual('value')
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeShowPropTrue
     * @tc.desc      ACE
     */
    it('testQrcodeShowPropTrue', 0, async function (done) {
        console.info('testQrcodeShowPropTrue START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        expect(obj.$attrs.value).assertEqual('value')
        console.info("[qrcodeProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeShowPropFalse
     * @tc.desc      ACE
     */
    it('testQrcodeShowPropFalse', 0, async function (done) {
        console.info('testQrcodeShowPropFalse START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        expect(obj.$attrs.value).assertEqual('value')
        console.info("[qrcodeProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeShowPropNone
     * @tc.desc      ACE
     */
    it('testQrcodeShowPropNone', 0, async function (done) {
        console.info('testQrcodeShowPropNone START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        expect(obj.$attrs.value).assertEqual('value')
        console.info("[qrcodeProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeQrcodeTypeValue
     * @tc.desc      ACE
     */
    it('testQrcodeQrcodeTypeValue', 0, async function (done) {
        console.info('testQrcodeQrcodeTypeValue START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.qrcodeTypeValue);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('qrcodeTypeValue')
        expect(obj.$attrs.value).assertEqual('value')
        console.info("[qrcodeProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeQrcodeTypeValueNone
     * @tc.desc      ACE
     */
    it('testQrcodeQrcodeTypeValueNone', 0, async function (done) {
        console.info('testQrcodeQrcodeTypeValueNone START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.qrcodeTypeValueNone);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('qrcodeTypeValueNone')
        expect(obj.$attrs.value).assertEqual(undefined)
        console.info("[qrcodeProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeQrcodeTypeRect
     * @tc.desc      ACE
     */
    it('testQrcodeQrcodeTypeRect', 0, async function (done) {
        console.info('testQrcodeQrcodeTypeRect START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.qrcodeTypeRect);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('qrcodeTypeRect')
        expect(obj.$attrs.value).assertEqual('value')
        expect(obj.$attrs.type).assertEqual('rect')
        console.info("[qrcodeProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeQrcodeTypeCircle
     * @tc.desc      ACE
     */
    it('testQrcodeQrcodeTypeCircle', 0, async function (done) {
        console.info('testQrcodeQrcodeTypeCircle START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.qrcodeTypeCircle);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('qrcodeTypeCircle')
        expect(obj.$attrs.value).assertEqual('value')
        expect(obj.$attrs.type).assertEqual('circle')
        console.info("[qrcodeProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testQrcodeQrcodeTypeNone
     * @tc.desc      ACE
     */
    it('testQrcodeQrcodeTypeNone', 0, async function (done) {
        console.info('testQrcodeQrcodeTypeNone START');
        console.info("[qrcodeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.qrcodeTypeNone);
        console.info("[qrcodeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[qrcodeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('qrcode')
        expect(obj.$attrs.id).assertEqual('qrcodeTypeNone')
        expect(obj.$attrs.value).assertEqual('value')
        expect(obj.$attrs.type).assertEqual(undefined)
        console.info("[qrcodeProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
});
