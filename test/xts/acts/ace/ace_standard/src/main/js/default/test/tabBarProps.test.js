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


describe('tab-barPropsJsTest', function () {

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
                resolve()
            }, 500)
        })
        let clearPromise = new Promise((resolve, reject) => {
            setTimeout(() => {
                router.clear();
                resolve();
            }, 500);
        });
        await backToIndexPromise.then(() => {
            return clearPromise;
        })
    }

    /**
    * run before testcase
    */
    beforeAll(async function (done) {
        console.info('[tab-barPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/tab-bar/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push tab-barProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push tab-barProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    })

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[tab-barPropsJsTest] after each called')
        await backToIndex();
        await sleep(1000)
    })

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarIdProp
     * @tc.desc      ACE
     */
    it('testTabBarIdProp', 0, async function (done) {
        console.info('testTabBarIdProp START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarClassProp
     * @tc.desc      ACE
     */
    it('testTabBarClassProp', 0, async function (done) {
        console.info('testTabBarClassProp START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarClassPropNone
     * @tc.desc      ACE
     */
    it('testTabBarClassPropNone', 0, async function (done) {
        console.info('testTabBarClassPropNone START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[tab-barProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarRefProp
     * @tc.desc      ACE
     */
    it('testTabBarRefProp', 0, async function (done) {
        console.info('testTabBarRefProp START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarRefPropNone
     * @tc.desc      ACE
     */
    it('testTabBarRefPropNone', 0, async function (done) {
        console.info('testTabBarRefPropNone START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[tab-barProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testTabBarDisabledPropTrue', 0, async function (done) {
        console.info('testTabBarDisabledPropTrue START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testTabBarDisabledPropFalse', 0, async function (done) {
        console.info('testTabBarDisabledPropFalse START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarDisabledPropNone
     * @tc.desc      ACE
     */
    it('testTabBarDisabledPropNone', 0, async function (done) {
        console.info('testTabBarDisabledPropNone START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testTabBarFocusablePropTrue', 0, async function (done) {
        console.info('testTabBarFocusablePropTrue START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testTabBarFocusablePropFalse', 0, async function (done) {
        console.info('testTabBarFocusablePropFalse START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarFocusablePropNone
     * @tc.desc      ACE
     */
    it('testTabBarFocusablePropNone', 0, async function (done) {
        console.info('testTabBarFocusablePropNone START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarDataProp
     * @tc.desc      ACE
     */
    it('testTabBarDataProp', 0, async function (done) {
        console.info('testTabBarDataProp START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataTabBar).assertEqual(undefined);
        console.info("[tab-barProps] get dataTabBar value is: " + JSON.stringify(obj.$attrs.dataTabBar));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarDataPropNone
     * @tc.desc      ACE
     */
    it('testTabBarDataPropNone', 0, async function (done) {
        console.info('testTabBarDataPropNone START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataTabBar).assertEqual(undefined)
        console.info("[tab-barProps] get dataTabBar value is: " + JSON.stringify(obj.$attrs.dataTabBar));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testTabBarClickEffectPropSmall', 0, async function (done) {
        console.info('testTabBarClickEffectPropSmall START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testTabBarClickEffectPropMedium', 0, async function (done) {
        console.info('testTabBarClickEffectPropMedium START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testTabBarClickEffectPropLarge', 0, async function (done) {
        console.info('testTabBarClickEffectPropLarge START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testTabBarClickEffectPropNone', 0, async function (done) {
        console.info('testTabBarClickEffectPropNone START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[tab-barProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarDirPropRtl
     * @tc.desc      ACE
     */
    it('testTabBarDirPropRtl', 0, async function (done) {
        console.info('testTabBarDirPropRtl START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarDirPropLtr
     * @tc.desc      ACE
     */
    it('testTabBarDirPropLtr', 0, async function (done) {
        console.info('testTabBarDirPropLtr START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarDirPropAuto
     * @tc.desc      ACE
     */
    it('testTabBarDirPropAuto', 0, async function (done) {
        console.info('testTabBarDirPropAuto START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarDirPropNone
     * @tc.desc      ACE
     */
    it('testTabBarDirPropNone', 0, async function (done) {
        console.info('testTabBarDirPropNone START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarForPropNull
     * @tc.desc      ACE
     */
    it('testTabBarForPropNull', 0, async function (done) {
        console.info('testTabBarForPropNull START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[tab-barProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarForPropOne
     * @tc.desc      ACE
     */
    it('testTabBarForPropOne', 0, async function (done) {
        console.info('testTabBarForPropOne START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[tab-barProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarForPropThree
     * @tc.desc      ACE
     */
    it('testTabBarForPropThree', 0, async function (done) {
        console.info('testTabBarForPropThree START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[tab-barProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarIfPropTrue
     * @tc.desc      ACE
     */
    it('testTabBarIfPropTrue', 0, async function (done) {
        console.info('testTabBarIfPropTrue START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[tab-barProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarShowPropTrue
     * @tc.desc      ACE
     */
    it('testTabBarShowPropTrue', 0, async function (done) {
        console.info('testTabBarShowPropTrue START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[tab-barProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarShowPropFalse
     * @tc.desc      ACE
     */
    it('testTabBarShowPropFalse', 0, async function (done) {
        console.info('testTabBarShowPropFalse START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[tab-barProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarShowPropNone
     * @tc.desc      ACE
     */
    it('testTabBarShowPropNone', 0, async function (done) {
        console.info('testTabBarShowPropNone START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[tab-barProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    //   特有属性

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarIndexNumber
     * @tc.desc      ACE
     */
    it('testTabBarModeScroll', 0, async function (done) {
        console.info('testTabBarModeScroll START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.modeScroll);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('modeScroll')
        expect(obj.$attrs.mode).assertEqual('scrollable')
        console.info("[tab-barProps] get mode value is: " + JSON.stringify(obj.$attrs.mode));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarModeFixed
     * @tc.desc      ACE
     */
    it('testTabBarModeFixed', 0, async function (done) {
        console.info('testTabBarModeFixed START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.modeFixed);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('modeFixed')
        expect(obj.$attrs.mode).assertEqual('fixed')
        console.info("[tab-barProps] get mode value is: " + JSON.stringify(obj.$attrs.mode));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarModeNone
     * @tc.desc      ACE
     */
    it('testTabBarModeNone', 0, async function (done) {
        console.info('testTabBarModeNone START');
        console.info("[tab-barProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.modeNone);
        console.info("[tab-barProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tab-barProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tab-bar')
        expect(obj.$attrs.id).assertEqual('modeNone')
        expect(obj.$attrs.mode).assertEqual('scrollable')
        console.info("[tab-barProps] get modeNone value is: " + JSON.stringify(obj.$attrs.mode));
        done();
    });
});
