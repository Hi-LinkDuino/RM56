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


describe('searchPropsJsTest', function () {

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
        console.info('[searchPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/search/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push searchProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push searchProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[searchPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchIdProp
     * @tc.desc      ACE
     */
    it('testsearchIdProp', 0, async function (done) {
        console.info('testsearchIdProp START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchClassProp
     * @tc.desc      ACE
     */
    it('testsearchClassProp', 0, async function (done) {
        console.info('testsearchClassProp START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchClassPropNone
     * @tc.desc      ACE
     */
    it('testsearchClassPropNone', 0, async function (done) {
        console.info('testsearchClassPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[searchProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchRefProp
     * @tc.desc      ACE
     */
    it('testsearchRefProp', 0, async function (done) {
        console.info('testsearchRefProp START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchRefPropNone
     * @tc.desc      ACE
     */
    it('testsearchRefPropNone', 0, async function (done) {
        console.info('testsearchRefPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[searchProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testsearchDisabledPropTrue', 0, async function (done) {
        console.info('testsearchDisabledPropTrue START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testsearchDisabledPropFalse', 0, async function (done) {
        console.info('testsearchDisabledPropFalse START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchDisabledPropNone
     * @tc.desc      ACE
     */
    it('testsearchDisabledPropNone', 0, async function (done) {
        console.info('testsearchDisabledPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testsearchFocusablePropTrue', 0, async function (done) {
        console.info('testsearchFocusablePropTrue START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testsearchFocusablePropFalse', 0, async function (done) {
        console.info('testsearchFocusablePropFalse START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchFocusablePropNone
     * @tc.desc      ACE
     */
    it('testsearchFocusablePropNone', 0, async function (done) {
        console.info('testsearchFocusablePropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchDataProp
     * @tc.desc      ACE
     */
    it('testsearchDataProp', 0, async function (done) {
        console.info('testsearchDataProp START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.datasearch).assertEqual(undefined);
        console.info("[searchProps] get datasearch value is: " + JSON.stringify(obj.$attrs.datasearch));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchDataPropNone
     * @tc.desc      ACE
     */
    it('testsearchDataPropNone', 0, async function (done) {
        console.info('testsearchDataPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.datasearch).assertEqual(undefined)
        console.info("[searchProps] get datasearch value is: " + JSON.stringify(obj.$attrs.datasearch));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testsearchClickEffectPropSmall', 0, async function (done) {
        console.info('testsearchClickEffectPropSmall START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testsearchClickEffectPropMedium', 0, async function (done) {
        console.info('testsearchClickEffectPropMedium START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testsearchClickEffectPropLarge', 0, async function (done) {
        console.info('testsearchClickEffectPropLarge START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testsearchClickEffectPropNone', 0, async function (done) {
        console.info('testsearchClickEffectPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[searchProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchDirPropRtl
     * @tc.desc      ACE
     */
    it('testsearchDirPropRtl', 0, async function (done) {
        console.info('testsearchDirPropRtl START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchDirPropLtr
     * @tc.desc      ACE
     */
    it('testsearchDirPropLtr', 0, async function (done) {
        console.info('testsearchDirPropLtr START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchDirPropAuto
     * @tc.desc      ACE
     */
    it('testsearchDirPropAuto', 0, async function (done) {
        console.info('testsearchDirPropAuto START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchDirPropNone
     * @tc.desc      ACE
     */
    it('testsearchDirPropNone', 0, async function (done) {
        console.info('testsearchDirPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchForPropNull
     * @tc.desc      ACE
     */
    it('testsearchForPropNull', 0, async function (done) {
        console.info('testsearchForPropNull START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[searchProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchForPropOne
     * @tc.desc      ACE
     */
    it('testsearchForPropOne', 0, async function (done) {
        console.info('testsearchForPropOne START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[searchProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchForPropThree
     * @tc.desc      ACE
     */
    it('testsearchForPropThree', 0, async function (done) {
        console.info('testsearchForPropThree START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[searchProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchIfPropTrue
     * @tc.desc      ACE
     */
    it('testsearchIfPropTrue', 0, async function (done) {
        console.info('testsearchIfPropTrue START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[searchProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchShowPropTrue
     * @tc.desc      ACE
     */
    it('testsearchShowPropTrue', 0, async function (done) {
        console.info('testsearchShowPropTrue START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[searchProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchShowPropFalse
     * @tc.desc      ACE
     */
    it('testsearchShowPropFalse', 0, async function (done) {
        console.info('testsearchShowPropFalse START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[searchProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchShowPropNone
     * @tc.desc      ACE
     */
    it('testsearchShowPropNone', 0, async function (done) {
        console.info('testsearchShowPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual("true")
        console.info("[searchProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchShowPropNone
     * @tc.desc      ACE
     */
    it('testsearchValuePropNone', 0, async function (done) {
        console.info('testsearchXsPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.valueNone);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('valueNone')
        expect(obj.$attrs.value).assertEqual(undefined)
        console.info("[searchProps] get value value is: " + JSON.stringify(obj.$attrs.value));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchShowPropNone
     * @tc.desc      ACE
     */
    it('testsearchValueProp', 0, async function (done) {
        console.info('testsearchXsPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.value);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('value')
        expect(obj.$attrs.value).assertEqual('text')
        console.info("[searchProps] get value value is: " + JSON.stringify(obj.$attrs.value));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchShowPropNone
     * @tc.desc      ACE
     */
    it('testsearchIconPropNone', 0, async function (done) {
        console.info('testsearchXsPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.iconNone);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('iconNone')
        expect(obj.$attrs.icon).assertEqual(undefined)
        console.info("[searchProps] get checked value is: " + JSON.stringify(obj.$attrs.icon));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchShowPropNone
     * @tc.desc      ACE
     */
    it('testsearchIconProp', 0, async function (done) {
        console.info('testsearchXsPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.icon);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('icon')
        expect(obj.$attrs.icon).assertEqual("common/images/image.png")
        console.info("[searchProps] get checked value is: " + JSON.stringify(obj.$attrs.icon));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchShowPropNone
     * @tc.desc      ACE
     */
    it('testsearchHintPropNone', 0, async function (done) {
        console.info('testsearchXsPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.hintNone);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('hintNone')
        expect(obj.$attrs.hint).assertEqual(undefined)
        console.info("[searchProps] get hint value is: " + JSON.stringify(obj.$attrs.hint));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchShowPropNone
     * @tc.desc      ACE
     */
    it('testsearchHintProp', 0, async function (done) {
        console.info('testsearchXsPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.hint);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('hint')
        expect(obj.$attrs.hint).assertEqual("提示")
        console.info("[searchProps] get hint value is: " + JSON.stringify(obj.$attrs.hint));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchShowPropNone
     * @tc.desc      ACE
     */
    it('testsearchMenuoptionsPropNone', 0, async function (done) {
        console.info('testsearchXsPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.menuoptionsNone);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('menuoptionsNone')
        expect(obj.$attrs.menuoptions).assertEqual(undefined)
        console.info("[searchProps] get menuoptions value is: " + JSON.stringify(obj.$attrs.menuoptions));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchShowPropNone
     * @tc.desc      ACE
     */
    it('testsearchMenuoptionsProp', 0, async function (done) {
        console.info('testsearchXsPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.menuoptions);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('menuoptions')
        expect(obj.$attrs.menuoptions).assertEqual(undefined)
        console.info("[searchProps] get menuoptions value is: " + JSON.stringify(obj.$attrs.menuoptions));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchShowPropNone
     * @tc.desc      ACE
     */
    it('testsearchSearchbuttonPropNone', 0, async function (done) {
        console.info('testsearchXsPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.searchbuttonNone);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('searchbuttonNone')
        expect(obj.$attrs.searchbutton).assertEqual(undefined)
        console.info("[searchProps] get searchbutton value is: " + JSON.stringify(obj.$attrs.searchbutton));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testsearchShowPropNone
     * @tc.desc      ACE
     */
    it('testsearchSearchbuttonProp', 0, async function (done) {
        console.info('testsearchXsPropNone START');
        console.info("[searchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.searchbutton);
        console.info("[searchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[searchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('search')
        expect(obj.$attrs.id).assertEqual('searchbutton')
        expect(obj.$attrs.searchbutton).assertEqual("按钮")
        console.info("[searchProps] get searchbutton value is: " + JSON.stringify(obj.$attrs.searchbutton));
        done();
    });
});



