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


describe('piecePropsJsTest', function () {

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
        console.info('[piecePropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/piece/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push pieceProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push pieceProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[piecePropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceIdProp
     * @tc.desc      ACE
     */
    it('testPieceIdProp', 0, async function (done) {
        console.info('testPieceIdProp START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceClassProp
     * @tc.desc      ACE
     */
    it('testPieceClassProp', 0, async function (done) {
        console.info('testPieceClassProp START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceClassPropNone
     * @tc.desc      ACE
     */
    it('testPieceClassPropNone', 0, async function (done) {
        console.info('testPieceClassPropNone START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[pieceProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceRefProp
     * @tc.desc      ACE
     */
    it('testPieceRefProp', 0, async function (done) {
        console.info('testPieceRefProp START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceRefPropNone
     * @tc.desc      ACE
     */
    it('testPieceRefPropNone', 0, async function (done) {
        console.info('testPieceRefPropNone START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[pieceProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testPieceDisabledPropTrue', 0, async function (done) {
        console.info('testPieceDisabledPropTrue START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testPieceDisabledPropFalse', 0, async function (done) {
        console.info('testPieceDisabledPropFalse START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceDisabledPropNone
     * @tc.desc      ACE
     */
    it('testPieceDisabledPropNone', 0, async function (done) {
        console.info('testPieceDisabledPropNone START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testPieceFocusablePropTrue', 0, async function (done) {
        console.info('testPieceFocusablePropTrue START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testPieceFocusablePropFalse', 0, async function (done) {
        console.info('testPieceFocusablePropFalse START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceFocusablePropNone
     * @tc.desc      ACE
     */
    it('testPieceFocusablePropNone', 0, async function (done) {
        console.info('testPieceFocusablePropNone START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceDataProp
     * @tc.desc      ACE
     */
    it('testPieceDataProp', 0, async function (done) {
        console.info('testPieceDataProp START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataPiece).assertEqual(undefined);
        console.info("[pieceProps] get dataPiece value is: " + JSON.stringify(obj.$attrs.dataPiece));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceDataPropNone
     * @tc.desc      ACE
     */
    it('testPieceDataPropNone', 0, async function (done) {
        console.info('testPieceDataPropNone START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataPiece).assertEqual(undefined)
        console.info("[pieceProps] get dataPiece value is: " + JSON.stringify(obj.$attrs.dataPiece));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testPieceClickEffectPropSmall', 0, async function (done) {
        console.info('testPieceClickEffectPropSmall START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testPieceClickEffectPropMedium', 0, async function (done) {
        console.info('testPieceClickEffectPropMedium START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testPieceClickEffectPropLarge', 0, async function (done) {
        console.info('testPieceClickEffectPropLarge START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testPieceClickEffectPropNone', 0, async function (done) {
        console.info('testPieceClickEffectPropNone START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[pieceProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceDirPropRtl
     * @tc.desc      ACE
     */
    it('testPieceDirPropRtl', 0, async function (done) {
        console.info('testPieceDirPropRtl START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceDirPropLtr
     * @tc.desc      ACE
     */
    it('testPieceDirPropLtr', 0, async function (done) {
        console.info('testPieceDirPropLtr START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceDirPropAuto
     * @tc.desc      ACE
     */
    it('testPieceDirPropAuto', 0, async function (done) {
        console.info('testPieceDirPropAuto START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceDirPropNone
     * @tc.desc      ACE
     */
    it('testPieceDirPropNone', 0, async function (done) {
        console.info('testPieceDirPropNone START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceForPropNull
     * @tc.desc      ACE
     */
    it('testPieceForPropNull', 0, async function (done) {
        console.info('testPieceForPropNull START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[pieceProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceForPropOne
     * @tc.desc      ACE
     */
    it('testPieceForPropOne', 0, async function (done) {
        console.info('testPieceForPropOne START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[pieceProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceForPropThree
     * @tc.desc      ACE
     */
    it('testPieceForPropThree', 0, async function (done) {
        console.info('testPieceForPropThree START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[pieceProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceIfPropTrue
     * @tc.desc      ACE
     */
    it('testPieceIfPropTrue', 0, async function (done) {
        console.info('testPieceIfPropTrue START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[pieceProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testPieceIfPropFalse', 0, async function (done) {
    //        console.info('testPieceIfPropFalse START');
    //        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('piece')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceIfPropNone
     * @tc.desc      ACE
     */
    //    it('testPieceIfPropNone', 0, async function (done) {
    //        console.info('testPieceIfPropNone START');
    //        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('piece')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceShowPropTrue
     * @tc.desc      ACE
     */
    it('testPieceShowPropTrue', 0, async function (done) {
        console.info('testPieceShowPropTrue START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[pieceProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceShowPropFalse
     * @tc.desc      ACE
     */
    it('testPieceShowPropFalse', 0, async function (done) {
        console.info('testPieceShowPropFalse START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[pieceProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceShowPropNone
     * @tc.desc      ACE
     */
    it('testPieceShowPropNone', 0, async function (done) {
        console.info('testPieceShowPropNone START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[pieceProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceContentProp
     * @tc.desc      ACE
     */
    it('testPieceContentProp', 0, async function (done) {
        console.info('testPieceContentProp START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.contentProp); //id
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('contentProp') //id
        expect(obj.$attrs.show).assertEqual(undefined) //prop and value
        console.info("[pieceProps] get content value is: " + JSON.stringify(obj.$attrs.content));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceClosablePropTrue
     * @tc.desc      ACE
     */
    it('testPieceClosablePropTrue', 0, async function (done) {
        console.info('testPieceClosablePropTrue START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.closablePropTrue); //id
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('closablePropTrue') //id
        expect(obj.$attrs.closable).assertEqual('true') //prop and value
        console.info("[pieceProps] get closable value is: " + JSON.stringify(obj.$attrs.closable));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPieceClosablePropFalse
     * @tc.desc      ACE
     */
    it('testPieceClosablePropFalse', 0, async function (done) {
        console.info('testPieceClosablePropFalse START');
        console.info("[pieceProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.closablePropFalse); //id
        console.info("[pieceProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[pieceProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('piece')
        expect(obj.$attrs.id).assertEqual('closablePropFalse') //id
        expect(obj.$attrs.closable).assertEqual('false') //prop and value
        console.info("[pieceProps] get closable value is: " + JSON.stringify(obj.$attrs.closable));
        done();
    });
});
