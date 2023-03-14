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


describe('refreshPropsJsTest', function () {

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
        console.info('[refreshPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/refresh/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push refreshProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push refreshProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[refreshPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshIdProp
     * @tc.desc      ACE
     */
    it('testRefreshIdProp', 0, async function (done) {
        console.info('testRefreshIdProp START');
        console.info("[RefreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[RefreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[RefreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshClassProp
     * @tc.desc      ACE
     */
    it('testRefreshClassProp', 0, async function (done) {
        console.info('testRefreshClassProp START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshClassPropNone
     * @tc.desc      ACE
     */
    it('testRefreshClassPropNone', 0, async function (done) {
        console.info('testRefreshClassPropNone START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[refreshProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshRefProp
     * @tc.desc      ACE
     */
    it('testRefreshRefProp', 0, async function (done) {
        console.info('testRefreshRefProp START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshRefPropNone
     * @tc.desc      ACE
     */
    it('testRefreshRefPropNone', 0, async function (done) {
        console.info('testRefreshRefPropNone START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[refreshProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testRefreshDisabledPropTrue', 0, async function (done) {
        console.info('testRefreshDisabledPropTrue START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testRefreshDisabledPropFalse', 0, async function (done) {
        console.info('testRefreshDisabledPropFalse START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshDisabledPropNone
     * @tc.desc      ACE
     */
    it('testRefreshDisabledPropNone', 0, async function (done) {
        console.info('testRefreshDisabledPropNone START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testRefreshFocusablePropTrue', 0, async function (done) {
        console.info('testRefreshFocusablePropTrue START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testRefreshFocusablePropFalse', 0, async function (done) {
        console.info('testRefreshFocusablePropFalse START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshFocusablePropNone
     * @tc.desc      ACE
     */
    it('testRefreshFocusablePropNone', 0, async function (done) {
        console.info('testRefreshFocusablePropNone START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshDataProp
     * @tc.desc      ACE
     */
    it('testRefreshDataProp', 0, async function (done) {
        console.info('testRefreshDataProp START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataRefresh).assertEqual(undefined);
        console.info("[refreshProps] get dataRefresh value is: " + JSON.stringify(obj.$attrs.dataRefresh));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshDataPropNone
     * @tc.desc      ACE
     */
    it('testRefreshDataPropNone', 0, async function (done) {
        console.info('testRefreshDataPropNone START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataRefresh).assertEqual(undefined)
        console.info("[refreshProps] get dataRefresh value is: " + JSON.stringify(obj.$attrs.dataRefresh));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testRefreshClickEffectPropSmall', 0, async function (done) {
        console.info('testRefreshClickEffectPropSmall START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testRefreshClickEffectPropMedium', 0, async function (done) {
        console.info('testRefreshClickEffectPropMedium START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testRefreshClickEffectPropLarge', 0, async function (done) {
        console.info('testRefreshClickEffectPropLarge START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testRefreshClickEffectPropNone', 0, async function (done) {
        console.info('testRefreshClickEffectPropNone START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[refreshProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshDirPropRtl
     * @tc.desc      ACE
     */
    it('testRefreshDirPropRtl', 0, async function (done) {
        console.info('testRefreshDirPropRtl START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshDirPropLtr
     * @tc.desc      ACE
     */
    it('testRefreshDirPropLtr', 0, async function (done) {
        console.info('testRefreshDirPropLtr START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshDirPropAuto
     * @tc.desc      ACE
     */
    it('testRefreshDirPropAuto', 0, async function (done) {
        console.info('testRefreshDirPropAuto START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshDirPropNone
     * @tc.desc      ACE
     */
    it('testRefreshDirPropNone', 0, async function (done) {
        console.info('testRefreshDirPropNone START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshForPropNull
     * @tc.desc      ACE
     */
    it('testRefreshForPropNull', 0, async function (done) {
        console.info('testRefreshForPropNull START');
        console.info("[RefreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[refreshProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshForPropOne
     * @tc.desc      ACE
     */
    it('testRefreshForPropOne', 0, async function (done) {
        console.info('testRefreshForPropOne START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[refreshProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshForPropThree
     * @tc.desc      ACE
     */
    it('testRefreshForPropThree', 0, async function (done) {
        console.info('testRefreshForPropThree START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[refreshProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshIfPropTrue
     * @tc.desc      ACE
     */
    it('testRefreshIfPropTrue', 0, async function (done) {
        console.info('testRefreshIfPropTrue START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[refreshProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testRefreshIfPropFalse', 0, async function (done) {
    //        console.info('testRefreshIfPropFalse START');
    //        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('refresh')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshIfPropNone
     * @tc.desc      ACE
     */
    //    it('testRefreshIfPropNone', 0, async function (done) {
    //        console.info('testRefreshIfPropNone START');
    //        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('refresh')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshShowPropTrue
     * @tc.desc      ACE
     */
    it('testRefreshShowPropTrue', 0, async function (done) {
        console.info('testRefreshShowPropTrue START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[refreshProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshShowPropFalse
     * @tc.desc      ACE
     */
    it('testRefreshShowPropFalse', 0, async function (done) {
        console.info('testRefreshShowPropFalse START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[refreshProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshShowPropNone
     * @tc.desc      ACE
     */
    it('testRefreshShowPropNone', 0, async function (done) {
        console.info('testRefreshShowPropNone START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[refreshProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOffsetProp
     * @tc.desc      ACE
     */

    it('testOffsetProp', 0, async function (done) {
        console.info('testOffsetProp START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.offsetProp);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('offsetProp')
        expect(obj.$attrs.offset).assertEqual('10px')
        console.info("[refreshProps] get for value is: " + JSON.stringify(obj.$attrs.offset));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshingPropFalse
     * @tc.desc      ACE
     */

    it('testRefreshingPropFalse', 0, async function (done) {
        console.info('testRefreshingPropFalse START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refreshingPropFalse);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('refreshingPropFalse')
        expect(obj.$attrs.refreshing).assertEqual('false')
        console.info("[refreshProps] get for value is: " + JSON.stringify(obj.$attrs.refreshing));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshingPropTrue
     * @tc.desc      ACE
     */

    it('testRefreshingPropTrue', 0, async function (done) {
        console.info('testRefreshingPropTrue START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refreshingPropTrue);//id
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')//type
        expect(obj.$attrs.id).assertEqual('refreshingPropTrue')//id
        expect(obj.$attrs.refreshing).assertEqual('false')//propValue
        console.info("[refreshProps] get for value is: " + JSON.stringify(obj.$attrs.refreshing));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTypePropAuto
     * @tc.desc      ACE
     */

    it('testTypePropAuto', 0, async function (done) {
        console.info('testTypePropAuto START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typePropAuto);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('typePropAuto')
        expect(obj.$attrs.type).assertEqual('auto')
        console.info("[refreshProps] get for value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTypePropPullDown
     * @tc.desc      ACE
     */

    it('testTypePropPullDown', 0, async function (done) {
        console.info('testTypePropPullDown START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typePropPullDown);
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')
        expect(obj.$attrs.id).assertEqual('typePropPullDown')
        expect(obj.$attrs.typePropAuto).assertEqual('auto')
        console.info("[refreshProps] get for value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLastTimePropTrue
     * @tc.desc      ACE
     */

    it('testLastTimePropTrue', 0, async function (done) {
        console.info('testLastTimePropTrue START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.lastTimePropTrue);//id
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')//type
        expect(obj.$attrs.id).assertEqual('lastTimePropTrue')//id
        expect(obj.$attrs.refreshing).assertEqual('true')//propValue
        console.info("[refreshProps] get for value is: " + JSON.stringify(obj.$attrs.lasttime));//pro
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLastTimePropFalse
     * @tc.desc      ACE
     */

    it('testLastTimePropFalse', 0, async function (done) {
        console.info('testLastTimePropFalse START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.lastTimePropFalse);//id
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')//type
        expect(obj.$attrs.id).assertEqual('lastTimePropFalse')//id
        expect(obj.$attrs.refreshing).assertEqual('false')//propValue
        console.info("[refreshProps] get for value is: " + JSON.stringify(obj.$attrs.lasttime));//pro
        done();
    })

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testFrictionProp
     * @tc.desc      ACE
     */

    it('testFrictionProp', 0, async function (done) {
        console.info('testFrictionProp START');
        console.info("[refreshProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.frictionProp);//id
        console.info("[refreshProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[refreshProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('refresh')//type
        expect(obj.$attrs.id).assertEqual('frictionProp')//id
        expect(obj.$attrs.refreshing).assertEqual('42')//propValue
        console.info("[refreshProps] get for value is: " + JSON.stringify(obj.$attrs.friction));//pro
        done();
    })

});
