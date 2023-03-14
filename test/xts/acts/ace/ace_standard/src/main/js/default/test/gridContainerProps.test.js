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


describe('gridContainerPropsJsTest', function () {

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
        console.info('[gridContainerPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/gridContainer/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push gridContainerProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push gridContainerProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[gridContainerPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerIdProp
     * @tc.desc      ACE
     */
    it('testgridContainerIdProp', 0, async function (done) {
        console.info('testgridContainerIdProp START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerClassProp
     * @tc.desc      ACE
     */
    it('testgridContainerClassProp', 0, async function (done) {
        console.info('testgridContainerClassProp START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerClassPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerClassPropNone', 0, async function (done) {
        console.info('testgridContainerClassPropNone START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[gridContainerProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerRefProp
     * @tc.desc      ACE
     */
    it('testgridContainerRefProp', 0, async function (done) {
        console.info('testgridContainerRefProp START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerRefPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerRefPropNone', 0, async function (done) {
        console.info('testgridContainerRefPropNone START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[gridContainerProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testgridContainerDisabledPropTrue', 0, async function (done) {
        console.info('testgridContainerDisabledPropTrue START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testgridContainerDisabledPropFalse', 0, async function (done) {
        console.info('testgridContainerDisabledPropFalse START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerDisabledPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerDisabledPropNone', 0, async function (done) {
        console.info('testgridContainerDisabledPropNone START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testgridContainerFocusablePropTrue', 0, async function (done) {
        console.info('testgridContainerFocusablePropTrue START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testgridContainerFocusablePropFalse', 0, async function (done) {
        console.info('testgridContainerFocusablePropFalse START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerFocusablePropNone
     * @tc.desc      ACE
     */
    it('testgridContainerFocusablePropNone', 0, async function (done) {
        console.info('testgridContainerFocusablePropNone START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerDataProp
     * @tc.desc      ACE
     */
    it('testgridContainerDataProp', 0, async function (done) {
        console.info('testgridContainerDataProp START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.datagridContainer).assertEqual(undefined);
        console.info("[gridConProp] get data value is: " + JSON.stringify(obj.$attrs.datagridContainer));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerDataPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerDataPropNone', 0, async function (done) {
        console.info('testgridContainerDataPropNone START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.datagridContainer).assertEqual(undefined)
        console.info("[gridConProp] get data value is: " + JSON.stringify(obj.$attrs.datagridContainer));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testgridContainerClickEffectPropSmall', 0, async function (done) {
        console.info('testgridContainerClickEffectPropSmall START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testgridContainerClickEffectPropMedium', 0, async function (done) {
        console.info('testgridContainerClickEffectPropMedium START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testgridContainerClickEffectPropLarge', 0, async function (done) {
        console.info('testgridContainerClickEffectPropLarge START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerClickEffectPropNone', 0, async function (done) {
        console.info('testgridContainerClickEffectPropNone START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[gridContainerProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerDirPropRtl
     * @tc.desc      ACE
     */
    it('testgridContainerDirPropRtl', 0, async function (done) {
        console.info('testgridContainerDirPropRtl START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerDirPropLtr
     * @tc.desc      ACE
     */
    it('testgridContainerDirPropLtr', 0, async function (done) {
        console.info('testgridContainerDirPropLtr START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerDirPropAuto
     * @tc.desc      ACE
     */
    it('testgridContainerDirPropAuto', 0, async function (done) {
        console.info('testgridContainerDirPropAuto START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerDirPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerDirPropNone', 0, async function (done) {
        console.info('testgridContainerDirPropNone START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerForPropNull
     * @tc.desc      ACE
     */
    it('testgridContainerForPropNull', 0, async function (done) {
        console.info('testgridContainerForPropNull START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[gridContainerProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerForPropOne
     * @tc.desc      ACE
     */
    it('testgridContainerForPropOne', 0, async function (done) {
        console.info('testgridContainerForPropOne START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[gridContainerProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerForPropThree
     * @tc.desc      ACE
     */
    it('testgridContainerForPropThree', 0, async function (done) {
        console.info('testgridContainerForPropThree START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[gridContainerProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerIfPropTrue
     * @tc.desc      ACE
     */
    it('testgridContainerIfPropTrue', 0, async function (done) {
        console.info('testgridContainerIfPropTrue START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[gridContainerProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerShowPropTrue
     * @tc.desc      ACE
     */
    it('testgridContainerShowPropTrue', 0, async function (done) {
        console.info('testgridContainerShowPropTrue START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[gridContainerProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerShowPropFalse
     * @tc.desc      ACE
     */
    it('testgridContainerShowPropFalse', 0, async function (done) {
        console.info('testgridContainerShowPropFalse START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[gridContainerProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerShowPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerShowPropNone', 0, async function (done) {
        console.info('testgridContainerShowPropNone START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual("true")
        console.info("[gridContainerProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerShowPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerColumnsPropNone', 0, async function (done) {
        console.info('testgridContainerXsPropNone START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeColumnsNull);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('typeColumnsNull')
        expect(obj.$attrs.columns).assertEqual('auto')
        console.info("[gridContainerProps] get columns value is: " + JSON.stringify(obj.$attrs.columns));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerShowPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerColumnsPropXs', 0, async function (done) {
        console.info('testgridContainerXsProp1 START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeColumnsXs);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('typeColumnsXs')
        expect(obj.$attrs.columns).assertEqual('xs')
        console.info("[gridContainerProps] get columns value is: " + JSON.stringify(obj.$attrs.columns));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerShowPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerColumnsPropSm', 0, async function (done) {
        console.info('testgridContainerXsProp1 START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeColumnsSm);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('typeColumnsSm')
        expect(obj.$attrs.columns).assertEqual('sm')
        console.info("[gridContainerProps] get columns value is: " + JSON.stringify(obj.$attrs.columns));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerShowPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerColumnsPropMd', 0, async function (done) {
        console.info('testgridContainerXsPropObject START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeColumnsMd);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('typeColumnsMd')
        expect(obj.$attrs.columns).assertEqual("md")
        console.info("[gridContainerProps] get columns value is: " + JSON.stringify(obj.$attrs.columns));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerShowPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerColumnsPropLg', 0, async function (done) {
        console.info('testgridContainerXsPropObject START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeColumnsLg);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('typeColumnsLg')
        expect(obj.$attrs.columns).assertEqual("lg")
        console.info("[gridContainerProps] get columns value is: " + JSON.stringify(obj.$attrs.columns));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerShowPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerSizePropNone', 0, async function (done) {
        console.info('testgridContainerXsPropNone START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeSizetypeNull);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('typeSizetypeNull')
        expect(obj.$attrs.sizetype).assertEqual("auto")
        console.info("[gridContainerProps] get size value is: " + JSON.stringify(obj.$attrs.sizetype));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerShowPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerSizePropXs', 0, async function (done) {
        console.info('testgridContainerXsProp1 START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeSizetypeXs);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('typeSizetypeXs')
        expect(obj.$attrs.sizetype).assertEqual("xs" )
        console.info("[gridContainerProps] get size value is: " + JSON.stringify(obj.$attrs.sizetype));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerShowPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerSizePropSm', 0, async function (done) {
        console.info('testgridContainerXsProp1 START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeSizetypeSm);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('typeSizetypeSm')
        expect(obj.$attrs.sizetype).assertEqual('sm')
        console.info("[gridContainerProps] get size value is: " + JSON.stringify(obj.$attrs.sizetype));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerShowPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerSizePropMd', 0, async function (done) {
        console.info('testgridContainerXsPropObject START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeSizetypeMd);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('typeSizetypeMd')
        expect(obj.$attrs.sizetype).assertEqual("md")
        console.info("[gridContainerProps] get size value is: " + JSON.stringify(obj.$attrs.sizetype));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerShowPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerSizePropLg', 0, async function (done) {
        console.info('testgridContainerXsPropObject START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeSizetypeLg);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('typeSizetypeLg')
        expect(obj.$attrs.sizetype).assertEqual("lg")
        console.info("[gridContainerProps] get size value is: " + JSON.stringify(obj.$attrs.sizetype));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerShowPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerGutterProp16', 0, async function (done) {
        console.info('testgridContainerSmPropNone START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeGutter16);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('typeGutter16')
        expect(obj.$attrs.gutter).assertEqual("16px")
        console.info("[gridContainerProps] get gutter value is: " + JSON.stringify(obj.$attrs.gutter));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerShowPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerGutterProp30', 0, async function (done) {
        console.info('testgridContainerSmPropNone START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeGutter30);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('typeGutter30')
        expect(obj.$attrs.gutter).assertEqual("30px")
        console.info("[gridContainerProps] get gutter value is: " + JSON.stringify(obj.$attrs.gutter));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridContainerShowPropNone
     * @tc.desc      ACE
     */
    it('testgridContainerGridtemplatePropDefault', 0, async function (done) {
        console.info('testgridContainerSmProp1 START');
        console.info("[gridContainerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeGridtemplateDefault);
        console.info("[gridContainerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridContainerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-container')
        expect(obj.$attrs.id).assertEqual('typeGridtemplateDefault')
        expect(obj.$attrs.gridtemplate).assertEqual("default")
        console.info("[gridContainerProps] get gridtemplate value is: " + JSON.stringify(obj.$attrs.gridtemplate));
        done();
    });

});
