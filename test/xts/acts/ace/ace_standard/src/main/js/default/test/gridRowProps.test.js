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


describe('gridRowPropsJsTest', function () {

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
        console.info('[gridRowPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/gridRow/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push gridRowProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push gridRowProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[gridRowPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowIdProp
     * @tc.desc      ACE
     */
    it('testgridRowIdProp', 0, async function (done) {
        console.info('testgridRowIdProp START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowClassProp
     * @tc.desc      ACE
     */
    it('testgridRowClassProp', 0, async function (done) {
        console.info('testgridRowClassProp START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowClassPropNone
     * @tc.desc      ACE
     */
    it('testgridRowClassPropNone', 0, async function (done) {
        console.info('testgridRowClassPropNone START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[gridRowProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowRefProp
     * @tc.desc      ACE
     */
    it('testgridRowRefProp', 0, async function (done) {
        console.info('testgridRowRefProp START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowRefPropNone
     * @tc.desc      ACE
     */
    it('testgridRowRefPropNone', 0, async function (done) {
        console.info('testgridRowRefPropNone START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[gridRowProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testgridRowDisabledPropTrue', 0, async function (done) {
        console.info('testgridRowDisabledPropTrue START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testgridRowDisabledPropFalse', 0, async function (done) {
        console.info('testgridRowDisabledPropFalse START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowDisabledPropNone
     * @tc.desc      ACE
     */
    it('testgridRowDisabledPropNone', 0, async function (done) {
        console.info('testgridRowDisabledPropNone START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testgridRowFocusablePropTrue', 0, async function (done) {
        console.info('testgridRowFocusablePropTrue START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testgridRowFocusablePropFalse', 0, async function (done) {
        console.info('testgridRowFocusablePropFalse START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowFocusablePropNone
     * @tc.desc      ACE
     */
    it('testgridRowFocusablePropNone', 0, async function (done) {
        console.info('testgridRowFocusablePropNone START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowDataProp
     * @tc.desc      ACE
     */
    it('testgridRowDataProp', 0, async function (done) {
        console.info('testgridRowDataProp START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.datagridRow).assertEqual(undefined);
        console.info("[gridRowProps] get datagridRow value is: " + JSON.stringify(obj.$attrs.datagridRow));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowDataPropNone
     * @tc.desc      ACE
     */
    it('testgridRowDataPropNone', 0, async function (done) {
        console.info('testgridRowDataPropNone START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.datagridRow).assertEqual(undefined)
        console.info("[gridRowProps] get datagridRow value is: " + JSON.stringify(obj.$attrs.datagridRow));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testgridRowClickEffectPropSmall', 0, async function (done) {
        console.info('testgridRowClickEffectPropSmall START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testgridRowClickEffectPropMedium', 0, async function (done) {
        console.info('testgridRowClickEffectPropMedium START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testgridRowClickEffectPropLarge', 0, async function (done) {
        console.info('testgridRowClickEffectPropLarge START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testgridRowClickEffectPropNone', 0, async function (done) {
        console.info('testgridRowClickEffectPropNone START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[gridRowProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowDirPropRtl
     * @tc.desc      ACE
     */
    it('testgridRowDirPropRtl', 0, async function (done) {
        console.info('testgridRowDirPropRtl START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowDirPropLtr
     * @tc.desc      ACE
     */
    it('testgridRowDirPropLtr', 0, async function (done) {
        console.info('testgridRowDirPropLtr START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowDirPropAuto
     * @tc.desc      ACE
     */
    it('testgridRowDirPropAuto', 0, async function (done) {
        console.info('testgridRowDirPropAuto START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowDirPropNone
     * @tc.desc      ACE
     */
    it('testgridRowDirPropNone', 0, async function (done) {
        console.info('testgridRowDirPropNone START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowForPropNull
     * @tc.desc      ACE
     */
    it('testgridRowForPropNull', 0, async function (done) {
        console.info('testgridRowForPropNull START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[gridRowProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowForPropOne
     * @tc.desc      ACE
     */
    it('testgridRowForPropOne', 0, async function (done) {
        console.info('testgridRowForPropOne START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[gridRowProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowForPropThree
     * @tc.desc      ACE
     */
    it('testgridRowForPropThree', 0, async function (done) {
        console.info('testgridRowForPropThree START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[gridRowProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowIfPropTrue
     * @tc.desc      ACE
     */
    it('testgridRowIfPropTrue', 0, async function (done) {
        console.info('testgridRowIfPropTrue START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[gridRowProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowShowPropTrue
     * @tc.desc      ACE
     */
    it('testgridRowShowPropTrue', 0, async function (done) {
        console.info('testgridRowShowPropTrue START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[gridRowProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowShowPropFalse
     * @tc.desc      ACE
     */
    it('testgridRowShowPropFalse', 0, async function (done) {
        console.info('testgridRowShowPropFalse START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[gridRowProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testgridRowShowPropNone
     * @tc.desc      ACE
     */
    it('testgridRowShowPropNone', 0, async function (done) {
        console.info('testgridRowShowPropNone START');
        console.info("[gridRowProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[gridRowProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[gridRowProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('grid-row')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual("true")
        console.info("[gridRowProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });


});
