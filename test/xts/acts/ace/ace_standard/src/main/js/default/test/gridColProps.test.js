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


describe('gridColPropsJsTest', function () {

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
        console.info('[gridColPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/gridCol/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push gridColProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push gridColProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[gridColPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColIdProp
     * @tc.desc      ACE
     */
    it('testgridColIdProp', 0, async function (done) {
        console.info('testgridColIdProp START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColClassProp
     * @tc.desc      ACE
     */
    it('testgridColClassProp', 0, async function (done) {
        console.info('testgridColClassProp START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColClassPropNone
     * @tc.desc      ACE
     */
    it('testgridColClassPropNone', 0, async function (done) {
        console.info('testgridColClassPropNone START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[gridColProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColRefProp
     * @tc.desc      ACE
     */
    it('testgridColRefProp', 0, async function (done) {
        console.info('testgridColRefProp START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColRefPropNone
     * @tc.desc      ACE
     */
    it('testgridColRefPropNone', 0, async function (done) {
        console.info('testgridColRefPropNone START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[gridColProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testgridColDisabledPropTrue', 0, async function (done) {
        console.info('testgridColDisabledPropTrue START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testgridColDisabledPropFalse', 0, async function (done) {
        console.info('testgridColDisabledPropFalse START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColDisabledPropNone
     * @tc.desc      ACE
     */
    it('testgridColDisabledPropNone', 0, async function (done) {
        console.info('testgridColDisabledPropNone START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testgridColFocusablePropTrue', 0, async function (done) {
        console.info('testgridColFocusablePropTrue START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testgridColFocusablePropFalse', 0, async function (done) {
        console.info('testgridColFocusablePropFalse START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColFocusablePropNone
     * @tc.desc      ACE
     */
    it('testgridColFocusablePropNone', 0, async function (done) {
        console.info('testgridColFocusablePropNone START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColDataProp
     * @tc.desc      ACE
     */
    it('testgridColDataProp', 0, async function (done) {
        console.info('testgridColDataProp START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.datagridCol).assertEqual(undefined);
        console.info("[gridColProps] get datagridCol value is: " + JSON.stringify(obj.$attrs.datagridCol));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColDataPropNone
     * @tc.desc      ACE
     */
    it('testgridColDataPropNone', 0, async function (done) {
        console.info('testgridColDataPropNone START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.datagridCol).assertEqual(undefined)
        console.info("[gridColProps] get datagridCol value is: " + JSON.stringify(obj.$attrs.datagridCol));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testgridColClickEffectPropSmall', 0, async function (done) {
        console.info('testgridColClickEffectPropSmall START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testgridColClickEffectPropMedium', 0, async function (done) {
        console.info('testgridColClickEffectPropMedium START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testgridColClickEffectPropLarge', 0, async function (done) {
        console.info('testgridColClickEffectPropLarge START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testgridColClickEffectPropNone', 0, async function (done) {
        console.info('testgridColClickEffectPropNone START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[gridColProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColDirPropRtl
     * @tc.desc      ACE
     */
    it('testgridColDirPropRtl', 0, async function (done) {
        console.info('testgridColDirPropRtl START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColDirPropLtr
     * @tc.desc      ACE
     */
    it('testgridColDirPropLtr', 0, async function (done) {
        console.info('testgridColDirPropLtr START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColDirPropAuto
     * @tc.desc      ACE
     */
    it('testgridColDirPropAuto', 0, async function (done) {
        console.info('testgridColDirPropAuto START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColDirPropNone
     * @tc.desc      ACE
     */
    it('testgridColDirPropNone', 0, async function (done) {
        console.info('testgridColDirPropNone START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColForPropNull
     * @tc.desc      ACE
     */
    it('testgridColForPropNull', 0, async function (done) {
        console.info('testgridColForPropNull START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[gridColProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColForPropOne
     * @tc.desc      ACE
     */
    it('testgridColForPropOne', 0, async function (done) {
        console.info('testgridColForPropOne START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[gridColProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColForPropThree
     * @tc.desc      ACE
     */
    it('testgridColForPropThree', 0, async function (done) {
        console.info('testgridColForPropThree START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[gridColProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColIfPropTrue
     * @tc.desc      ACE
     */
    it('testgridColIfPropTrue', 0, async function (done) {
        console.info('testgridColIfPropTrue START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[gridColProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropTrue
     * @tc.desc      ACE
     */
    it('testgridColShowPropTrue', 0, async function (done) {
        console.info('testgridColShowPropTrue START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[gridColProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropFalse
     * @tc.desc      ACE
     */
    it('testgridColShowPropFalse', 0, async function (done) {
        console.info('testgridColShowPropFalse START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[gridColProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColShowPropNone', 0, async function (done) {
        console.info('testgridColShowPropNone START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual("true")
        console.info("[gridColProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColXsPropNone', 0, async function (done) {
        console.info('testgridColXsPropNone START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeXsNull);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('typeXsNull')
        expect(obj.$attrs.xs).assertEqual(undefined)
        console.info("[gridColProps] get xs value is: " + JSON.stringify(obj.$attrs.xs));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColXsProp1', 0, async function (done) {
        console.info('testgridColXsProp1 START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeXs1);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('typeXs1')
        expect(obj.$attrs.xs).assertEqual('1')
        console.info("[gridColProps] get xs value is: " + JSON.stringify(obj.$attrs.xs));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColXsPropObject', 0, async function (done) {
        console.info('testgridColXsPropObject START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeLgObject);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('typeLgObject')
        expect(obj.$attrs.xs).assertEqual(undefined)
        console.info("[gridColProps] get xs value is: " + JSON.stringify(obj.$attrs.xs));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColSmPropNone', 0, async function (done) {
        console.info('testgridColSmPropNone START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeSmNull);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('typeSmNull')
        expect(obj.$attrs.sm).assertEqual(undefined)
        console.info("[gridColProps] get sm value is: " + JSON.stringify(obj.$attrs.sm));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColSmProp1', 0, async function (done) {
        console.info('testgridColSmProp1 START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeSm1);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('typeSm1')
        expect(obj.$attrs.sm).assertEqual("1")
        console.info("[gridColProps] get sm value is: " + JSON.stringify(obj.$attrs.sm));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColSmObject', 0, async function (done) {
        console.info('testgridColSmProp1 START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeSmObject);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('typeSmObject')
        expect(obj.$attrs.Sm).assertEqual(undefined)
        console.info("[gridColProps] get Sm value is: " + JSON.stringify(obj.$attrs.sm));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColMdPropNone', 0, async function (done) {
        console.info('testgridColSmPropNone START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeMdNull);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('typeMdNull')
        expect(obj.$attrs.md).assertEqual(undefined)
        console.info("[gridColProps] get md value is: " + JSON.stringify(obj.$attrs.md));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColMdProp1', 0, async function (done) {
        console.info('testgridColSmProp1 START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeMd1);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('typeMd1')
        expect(obj.$attrs.md).assertEqual("1")
        console.info("[gridColProps] get md value is: " + JSON.stringify(obj.$attrs.md));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColMdObject', 0, async function (done) {
        console.info('testgridColSmProp1 START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeMdObject);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('typeMdObject')
        expect(obj.$attrs.md).assertEqual(undefined)
        console.info("[gridColProps] get md value is: " + JSON.stringify(obj.$attrs.md));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColLgPropNone', 0, async function (done) {
        console.info('testgridColSmPropNone START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeLgNull);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('typeLgNull')
        expect(obj.$attrs.lg).assertEqual(undefined)
        console.info("[gridColProps] get lg value is: " + JSON.stringify(obj.$attrs.lg));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColLgProp1', 0, async function (done) {
        console.info('testgridColSmProp1 START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeLg1);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('typeLg1')
        expect(obj.$attrs.lg).assertEqual("1")
        console.info("[gridColProps] get lg value is: " + JSON.stringify(obj.$attrs.lg));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColLgObject', 0, async function (done) {
        console.info('testgridColSmProp1 START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeLgObject);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('typeLgObject')
        expect(obj.$attrs.lg).assertEqual(undefined)
        console.info("[gridColProps] get lg value is: " + JSON.stringify(obj.$attrs.lg));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColSpanProp1', 0, async function (done) {
        console.info('testgridColSmProp1 START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeSpan1);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('typeSpan1')
        expect(obj.$attrs.span).assertEqual("1")
        console.info("[gridColProps] get span value is: " + JSON.stringify(obj.$attrs.span));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColSpanProp3', 0, async function (done) {
        console.info('testgridColSmProp1 START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeSpan3);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('typeSpan3')
        expect(obj.$attrs.span).assertEqual("3")
        console.info("[gridColProps] get span value is: " + JSON.stringify(obj.$attrs.span));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColOffsetProp1', 0, async function (done) {
        console.info('testgridColSmProp1 START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeOffset1);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('typeOffset1')
        expect(obj.$attrs.offset).assertEqual("1")
        console.info("[gridColProps] get offset value is: " + JSON.stringify(obj.$attrs.offset));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridColShowPropNone
     * @tc.desc      ACE
     */
    it('testgridColOffsetProp3', 0, async function (done) {
        console.info('testgridColSmProp1 START');
        console.info("[gridColProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeOffset3);
        console.info("[gridColProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridColProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-col')
        expect(obj.$attrs.id).assertEqual('typeOffset3')
        expect(obj.$attrs.offset).assertEqual("3")
        console.info("[gridColProps] get offset value is: " + JSON.stringify(obj.$attrs.offset));
        done();
    });

});
