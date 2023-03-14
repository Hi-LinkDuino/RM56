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


describe('textAreaPropsJsTest', function () {

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
        console.info('[textAreaPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/textArea/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push textAreaProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push textAreaProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[textAreaPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaIdProp
     * @tc.desc      ACE
     */
    it('testtextAreaIdProp', 0, async function (done) {
        console.info('testtextAreaIdProp START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaClassProp
     * @tc.desc      ACE
     */
    it('testtextAreaClassProp', 0, async function (done) {
        console.info('testtextAreaClassProp START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaClassPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaClassPropNone', 0, async function (done) {
        console.info('testtextAreaClassPropNone START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[textAreaProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaRefProp
     * @tc.desc      ACE
     */
    it('testtextAreaRefProp', 0, async function (done) {
        console.info('testtextAreaRefProp START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaRefPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaRefPropNone', 0, async function (done) {
        console.info('testtextAreaRefPropNone START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[textAreaProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testtextAreaDisabledPropTrue', 0, async function (done) {
        console.info('testtextAreaDisabledPropTrue START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testtextAreaDisabledPropFalse', 0, async function (done) {
        console.info('testtextAreaDisabledPropFalse START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaDisabledPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaDisabledPropNone', 0, async function (done) {
        console.info('testtextAreaDisabledPropNone START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testtextAreaFocusablePropTrue', 0, async function (done) {
        console.info('testtextAreaFocusablePropTrue START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testtextAreaFocusablePropFalse', 0, async function (done) {
        console.info('testtextAreaFocusablePropFalse START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaFocusablePropNone
     * @tc.desc      ACE
     */
    it('testtextAreaFocusablePropNone', 0, async function (done) {
        console.info('testtextAreaFocusablePropNone START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaDataProp
     * @tc.desc      ACE
     */
    it('testtextAreaDataProp', 0, async function (done) {
        console.info('testtextAreaDataProp START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.datatextArea).assertEqual(undefined);
        console.info("[textAreaProps] get datatextArea value is: " + JSON.stringify(obj.$attrs.datatextArea));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaDataPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaDataPropNone', 0, async function (done) {
        console.info('testtextAreaDataPropNone START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.datatextArea).assertEqual(undefined)
        console.info("[textAreaProps] get datatextArea value is: " + JSON.stringify(obj.$attrs.datatextArea));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testtextAreaClickEffectPropSmall', 0, async function (done) {
        console.info('testtextAreaClickEffectPropSmall START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testtextAreaClickEffectPropMedium', 0, async function (done) {
        console.info('testtextAreaClickEffectPropMedium START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testtextAreaClickEffectPropLarge', 0, async function (done) {
        console.info('testtextAreaClickEffectPropLarge START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaClickEffectPropNone', 0, async function (done) {
        console.info('testtextAreaClickEffectPropNone START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[textAreaProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaDirPropRtl
     * @tc.desc      ACE
     */
    it('testtextAreaDirPropRtl', 0, async function (done) {
        console.info('testtextAreaDirPropRtl START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaDirPropLtr
     * @tc.desc      ACE
     */
    it('testtextAreaDirPropLtr', 0, async function (done) {
        console.info('testtextAreaDirPropLtr START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaDirPropAuto
     * @tc.desc      ACE
     */
    it('testtextAreaDirPropAuto', 0, async function (done) {
        console.info('testtextAreaDirPropAuto START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaDirPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaDirPropNone', 0, async function (done) {
        console.info('testtextAreaDirPropNone START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaForPropNull
     * @tc.desc      ACE
     */
    it('testtextAreaForPropNull', 0, async function (done) {
        console.info('testtextAreaForPropNull START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[textAreaProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaForPropOne
     * @tc.desc      ACE
     */
    it('testtextAreaForPropOne', 0, async function (done) {
        console.info('testtextAreaForPropOne START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[textAreaProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaForPropThree
     * @tc.desc      ACE
     */
    it('testtextAreaForPropThree', 0, async function (done) {
        console.info('testtextAreaForPropThree START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[textAreaProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaIfPropTrue
     * @tc.desc      ACE
     */
    it('testtextAreaIfPropTrue', 0, async function (done) {
        console.info('testtextAreaIfPropTrue START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[textAreaProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropTrue
     * @tc.desc      ACE
     */
    it('testtextAreaShowPropTrue', 0, async function (done) {
        console.info('testtextAreaShowPropTrue START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[textAreaProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropFalse
     * @tc.desc      ACE
     */
    it('testtextAreaShowPropFalse', 0, async function (done) {
        console.info('testtextAreaShowPropFalse START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[textAreaProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaShowPropNone', 0, async function (done) {
        console.info('testtextAreaShowPropNone START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual("true")
        console.info("[textAreaProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaPlaceholderPropNone', 0, async function (done) {
        console.info('testtextAreaXsPropNone START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.placeholderNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('placeholderNone')
        expect(obj.$attrs.placeholder).assertEqual(undefined)
        console.info("[textAreaProps] get placeholder value is: " + JSON.stringify(obj.$attrs.placeholder));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaPlaceholderProp', 0, async function (done) {
        console.info('testtextAreaXsPropNone START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.placeholder);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('placeholder')
        expect(obj.$attrs.placeholder).assertEqual('please--input')
        console.info("[textAreaProps] get placeholder value is: " + JSON.stringify(obj.$attrs.placeholder));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaMaxlengthProp20', 0, async function (done) {
        console.info('testtextAreaXsPropNone START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.maxlength20);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('maxlength20')
        expect(obj.$attrs.maxlength).assertEqual('20')
        console.info("[textAreaProps] get maxlength value is: " + JSON.stringify(obj.$attrs.maxlength));
        done();
    });

/**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
it('testtextAreaHeadericonPropNull', 0, async function (done) {
    console.info('testtextAreaXsPropObject START');
    console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

    let obj = JSON.parse(globalThis.value.headericonNone);
    console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
    console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

    expect(obj.$type).assertEqual('textarea')
    expect(obj.$attrs.id).assertEqual('headericonNone')
    expect(obj.$attrs.headericon).assertEqual(undefined)
    console.info("[textAreaProps] get headericon value is: " + JSON.stringify(obj.$attrs.headericon));
    done();
});

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaHeadericonProp', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.headericon);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('headericon')
        expect(obj.$attrs.headericon).assertEqual("common/images/icon.png")
        console.info("[textAreaProps] get headericon value is: " + JSON.stringify(obj.$attrs.headericon));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaExtendPropNone', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.extendNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('extendNone')
        expect(obj.$attrs.extend).assertEqual("false")
        console.info("[textAreaProps] get extend value is: " + JSON.stringify(obj.$attrs.extend));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaExtendPropFalse', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.extendFalse);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('extendFalse')
        expect(obj.$attrs.extend).assertEqual("false")
        console.info("[textAreaProps] get extend value is: " + JSON.stringify(obj.$attrs.extend));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaExtendProptTrue', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.extendTrue);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('extendTrue')
        expect(obj.$attrs.extend).assertEqual("true")
        console.info("[textAreaProps] get extend value is: " + JSON.stringify(obj.$attrs.extend));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaValuePropNull', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.valueNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('valueNone')
        expect(obj.$attrs.value).assertEqual(undefined)
        console.info("[textAreaProps] get value value is: " + JSON.stringify(obj.$attrs.value));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaValueProp', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.value);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('value')
        expect(obj.$attrs.value).assertEqual("text")
        console.info("[textAreaProps] get value value is: " + JSON.stringify(obj.$attrs.value));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaShowcounterPropNone', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showcounterNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('showcounterNone')
        expect(obj.$attrs.showcounter).assertEqual("false")
        console.info("[textAreaProps] get showcounter value is: " + JSON.stringify(obj.$attrs.showcounter));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaShowcounterPropFalse', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showcounterFalse);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('showcounterFalse')
        expect(obj.$attrs.showcounter).assertEqual("false")
        console.info("[textAreaProps] get showcounter value is: " + JSON.stringify(obj.$attrs.showcounter));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaShowcounterProptTrue', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showcounterTrue);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('showcounterTrue')
        expect(obj.$attrs.showcounter).assertEqual("true")
        console.info("[textAreaProps] get showcounter value is: " + JSON.stringify(obj.$attrs.showcounter));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaAutofocusPropNone', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.autofocusNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('autofocusNone')
        expect(obj.$attrs.autofocus).assertEqual("false")
        console.info("[textAreaProps] get autofocus value is: " + JSON.stringify(obj.$attrs.autofocus));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaAutofocusPropFalse', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.autofocusFalse);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('autofocusFalse')
        expect(obj.$attrs.autofocus).assertEqual("false")
        console.info("[textAreaProps] get autofocus value is: " + JSON.stringify(obj.$attrs.autofocus));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaAutofocusProptTrue', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.autofocusTrue);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('autofocusTrue')
        expect(obj.$attrs.autofocus).assertEqual("true")
        console.info("[textAreaProps] get autofocus value is: " + JSON.stringify(obj.$attrs.autofocus));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaSoftkeyboardenabledPropNone', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.softkeyboardenabledNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('softkeyboardenabledNone')
        expect(obj.$attrs.softkeyboardenabled).assertEqual("true")
        console.info("[textAreaProps] get softkey value is: " + JSON.stringify(obj.$attrs.softkey));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaSoftkeyboardenabledPropFalse', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.softkeyboardenabledFalse);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('softkeyboardenabledFalse')
        expect(obj.$attrs.softkeyboardenabled).assertEqual("false")
        console.info("[textAreaProps] get softkey value is: " + JSON.stringify(obj.$attrs.softkey));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaSoftkeyboardenabledProptTrue', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.softkeyboardenabledTrue);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('softkeyboardenabledTrue')
        expect(obj.$attrs.softkeyboardenabled).assertEqual("true")
        console.info("[textAreaProps] get softkey value is: " + JSON.stringify(obj.$attrs.softkey));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaMenuoptionsPropNone', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.menuoptionsNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('menuoptionsNone')
        expect(obj.$attrs.menuoptions).assertEqual(undefined)
        console.info("[textAreaProps] get menuoptions value is: " + JSON.stringify(obj.$attrs.menuoptions));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaMenuoptionsProp', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.menuoptions);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('menuoptions')
        expect(obj.$attrs.menuoptions).assertEqual(undefined)
        console.info("[textAreaProps] get menuoptions value is: " + JSON.stringify(obj.$attrs.menuoptions));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaSelectedstartPropNone', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.selectedstartNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('selectedstartNone')
        expect(obj.$attrs.selectedstart).assertEqual("-1")
        console.info("[textAreaProps] get selectedstart value is: " + JSON.stringify(obj.$attrs.selectedstart));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaSelectedstartProp', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.selectedstart5);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('selectedstart5')
        expect(obj.$attrs.selectedstart).assertEqual("5")
        console.info("[textAreaProps] get selectedstart value is: " + JSON.stringify(obj.$attrs.selectedstart));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaSelectedEndtPropNone', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.selectedendNone);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('selectedendNone')
        expect(obj.$attrs.selectedend).assertEqual("-1")
        console.info("[textAreaProps] get selectedend value is: " + JSON.stringify(obj.$attrs.selectedend));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtextAreaShowPropNone
     * @tc.desc      ACE
     */
    it('testtextAreaSelectedEndProp', 0, async function (done) {
        console.info('testtextAreaXsPropObject START');
        console.info("[textAreaProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.selectedend5);
        console.info("[textAreaProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[textAreaProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('textarea')
        expect(obj.$attrs.id).assertEqual('selectedend5')
        expect(obj.$attrs.selectedend).assertEqual("5")
        console.info("[textAreaProps] get selectedend value is: " + JSON.stringify(obj.$attrs.selectedend));
        done();
    });
});



