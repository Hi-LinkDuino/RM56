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


describe('togglePropsJsTest', function () {

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
        console.info('[togglePropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/toggle/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push toggleProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push toggleProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[togglePropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleIdProp
     * @tc.desc      ACE
     */
    it('testtoggleIdProp', 0, async function (done) {
        console.info('testtoggleIdProp START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleClassProp
     * @tc.desc      ACE
     */
    it('testtoggleClassProp', 0, async function (done) {
        console.info('testtoggleClassProp START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleClassPropNone
     * @tc.desc      ACE
     */
    it('testtoggleClassPropNone', 0, async function (done) {
        console.info('testtoggleClassPropNone START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[toggleProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleRefProp
     * @tc.desc      ACE
     */
    it('testtoggleRefProp', 0, async function (done) {
        console.info('testtoggleRefProp START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleRefPropNone
     * @tc.desc      ACE
     */
    it('testtoggleRefPropNone', 0, async function (done) {
        console.info('testtoggleRefPropNone START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[toggleProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testtoggleDisabledPropTrue', 0, async function (done) {
        console.info('testtoggleDisabledPropTrue START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testtoggleDisabledPropFalse', 0, async function (done) {
        console.info('testtoggleDisabledPropFalse START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleDisabledPropNone
     * @tc.desc      ACE
     */
    it('testtoggleDisabledPropNone', 0, async function (done) {
        console.info('testtoggleDisabledPropNone START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testtoggleFocusablePropTrue', 0, async function (done) {
        console.info('testtoggleFocusablePropTrue START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testtoggleFocusablePropFalse', 0, async function (done) {
        console.info('testtoggleFocusablePropFalse START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleFocusablePropNone
     * @tc.desc      ACE
     */
    it('testtoggleFocusablePropNone', 0, async function (done) {
        console.info('testtoggleFocusablePropNone START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleDataProp
     * @tc.desc      ACE
     */
    it('testtoggleDataProp', 0, async function (done) {
        console.info('testtoggleDataProp START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.datatoggle).assertEqual(undefined);
        console.info("[toggleProps] get datatoggle value is: " + JSON.stringify(obj.$attrs.datatoggle));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleDataPropNone
     * @tc.desc      ACE
     */
    it('testtoggleDataPropNone', 0, async function (done) {
        console.info('testtoggleDataPropNone START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.datatoggle).assertEqual(undefined)
        console.info("[toggleProps] get datatoggle value is: " + JSON.stringify(obj.$attrs.datatoggle));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testtoggleClickEffectPropSmall', 0, async function (done) {
        console.info('testtoggleClickEffectPropSmall START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testtoggleClickEffectPropMedium', 0, async function (done) {
        console.info('testtoggleClickEffectPropMedium START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testtoggleClickEffectPropLarge', 0, async function (done) {
        console.info('testtoggleClickEffectPropLarge START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testtoggleClickEffectPropNone', 0, async function (done) {
        console.info('testtoggleClickEffectPropNone START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[toggleProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleDirPropRtl
     * @tc.desc      ACE
     */
    it('testtoggleDirPropRtl', 0, async function (done) {
        console.info('testtoggleDirPropRtl START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleDirPropLtr
     * @tc.desc      ACE
     */
    it('testtoggleDirPropLtr', 0, async function (done) {
        console.info('testtoggleDirPropLtr START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleDirPropAuto
     * @tc.desc      ACE
     */
    it('testtoggleDirPropAuto', 0, async function (done) {
        console.info('testtoggleDirPropAuto START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleDirPropNone
     * @tc.desc      ACE
     */
    it('testtoggleDirPropNone', 0, async function (done) {
        console.info('testtoggleDirPropNone START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleForPropNull
     * @tc.desc      ACE
     */
    it('testtoggleForPropNull', 0, async function (done) {
        console.info('testtoggleForPropNull START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[toggleProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleForPropOne
     * @tc.desc      ACE
     */
    it('testtoggleForPropOne', 0, async function (done) {
        console.info('testtoggleForPropOne START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[toggleProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleForPropThree
     * @tc.desc      ACE
     */
    it('testtoggleForPropThree', 0, async function (done) {
        console.info('testtoggleForPropThree START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[toggleProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleIfPropTrue
     * @tc.desc      ACE
     */
    it('testtoggleIfPropTrue', 0, async function (done) {
        console.info('testtoggleIfPropTrue START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[toggleProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleShowPropTrue
     * @tc.desc      ACE
     */
    it('testtoggleShowPropTrue', 0, async function (done) {
        console.info('testtoggleShowPropTrue START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[toggleProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleShowPropFalse
     * @tc.desc      ACE
     */
    it('testtoggleShowPropFalse', 0, async function (done) {
        console.info('testtoggleShowPropFalse START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[toggleProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleShowPropNone
     * @tc.desc      ACE
     */
    it('testtoggleShowPropNone', 0, async function (done) {
        console.info('testtoggleShowPropNone START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual("true")
        console.info("[toggleProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleShowPropNone
     * @tc.desc      ACE
     */
    it('testtoggleValuePropNone', 0, async function (done) {
        console.info('testtoggleXsPropNone START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.valueNone);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('valueNone')
        expect(obj.$attrs.value).assertEqual(undefined)
        console.info("[toggleProps] get value value is: " + JSON.stringify(obj.$attrs.value));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleShowPropNone
     * @tc.desc      ACE
     */
    it('testtoggleValueProp', 0, async function (done) {
        console.info('testtoggleXsPropNone START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.value);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('value')
        expect(obj.$attrs.value).assertEqual('please--input')
        console.info("[toggleProps] get value value is: " + JSON.stringify(obj.$attrs.value));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleShowPropNone
     * @tc.desc      ACE
     */
    it('testtoggleCheckedPropNone', 0, async function (done) {
        console.info('testtoggleXsPropNone START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.checkedNone);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('checkedNone')
        expect(obj.$attrs.checked).assertEqual(undefined)
        console.info("[toggleProps] get checked value is: " + JSON.stringify(obj.$attrs.checked));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleShowPropNone
     * @tc.desc      ACE
     */
    it('testtoggleCheckedPropTrue', 0, async function (done) {
        console.info('testtoggleXsPropNone START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.checkedTrue);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('checkedTrue')
        expect(obj.$attrs.checked).assertEqual('true')
        console.info("[toggleProps] get checked value is: " + JSON.stringify(obj.$attrs.checked));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testtoggleShowPropNone
     * @tc.desc      ACE
     */
    it('testtoggleCheckedPropFalse', 0, async function (done) {
        console.info('testtoggleXsPropNone START');
        console.info("[toggleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.checkedFalse);
        console.info("[toggleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[toggleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('toggle')
        expect(obj.$attrs.id).assertEqual('checkedFalse')
        expect(obj.$attrs.checked).assertEqual('false')
        console.info("[toggleProps] get checked value is: " + JSON.stringify(obj.$attrs.checked));
        done();
    });
});



