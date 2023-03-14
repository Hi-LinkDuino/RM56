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


describe('tabsPropsJsTest', function () {

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
        })
        let clearPromise = new Promise((resolve, reject) => {
            setTimeout(() => {
                router.clear()
                resolve()
            }, 500)
        })
        await backToIndexPromise.then(() => {
            return clearPromise;
        })
    }

    /**
    * run before testcase
    */
    beforeAll(async function (done) {
        console.info('[tabsPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/tabs/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push tabsProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push tabsProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    })

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[tabsPropsJsTest] after each called')
        await backToIndex();
        await sleep(1000)
    })

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsIdProp
     * @tc.desc      ACE
     */
    it('testTabsIdProp', 0, async function (done) {
        console.info('testTabsIdProp START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsClassProp
     * @tc.desc      ACE
     */
    it('testTabsClassProp', 0, async function (done) {
        console.info('testTabsClassProp START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsClassPropNone
     * @tc.desc      ACE
     */
    it('testTabsClassPropNone', 0, async function (done) {
        console.info('testTabsClassPropNone START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[tabsProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsRefProp
     * @tc.desc      ACE
     */
    it('testTabsRefProp', 0, async function (done) {
        console.info('testTabsRefProp START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsRefPropNone
     * @tc.desc      ACE
     */
    it('testTabsRefPropNone', 0, async function (done) {
        console.info('testTabsRefPropNone START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[tabsProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testTabsDisabledPropTrue', 0, async function (done) {
        console.info('testTabsDisabledPropTrue START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testTabsDisabledPropFalse', 0, async function (done) {
        console.info('testTabsDisabledPropFalse START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsDisabledPropNone
     * @tc.desc      ACE
     */
    it('testTabsDisabledPropNone', 0, async function (done) {
        console.info('testTabsDisabledPropNone START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testTabsFocusablePropTrue', 0, async function (done) {
        console.info('testTabsFocusablePropTrue START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testTabsFocusablePropFalse', 0, async function (done) {
        console.info('testTabsFocusablePropFalse START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsFocusablePropNone
     * @tc.desc      ACE
     */
    it('testTabsFocusablePropNone', 0, async function (done) {
        console.info('testTabsFocusablePropNone START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsDataProp
     * @tc.desc      ACE
     */
    it('testTabsDataProp', 0, async function (done) {
        console.info('testTabsDataProp START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.datatabs).assertEqual(undefined);
        console.info("[tabsProps] get datatabs value is: " + JSON.stringify(obj.$attrs.datatabs));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsDataPropNone
     * @tc.desc      ACE
     */
    it('testTabsDataPropNone', 0, async function (done) {
        console.info('testTabsDataPropNone START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.datatabs).assertEqual(undefined)
        console.info("[tabsProps] get datatabs value is: " + JSON.stringify(obj.$attrs.datatabs));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testTabsClickEffectPropSmall', 0, async function (done) {
        console.info('testTabsClickEffectPropSmall START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testTabsClickEffectPropMedium', 0, async function (done) {
        console.info('testTabsClickEffectPropMedium START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testTabsClickEffectPropLarge', 0, async function (done) {
        console.info('testTabsClickEffectPropLarge START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testTabsClickEffectPropNone', 0, async function (done) {
        console.info('testTabsClickEffectPropNone START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[tabsProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsDirPropRtl
     * @tc.desc      ACE
     */
    it('testTabsDirPropRtl', 0, async function (done) {
        console.info('testTabsDirPropRtl START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsDirPropLtr
     * @tc.desc      ACE
     */
    it('testTabsDirPropLtr', 0, async function (done) {
        console.info('testTabsDirPropLtr START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsDirPropAuto
     * @tc.desc      ACE
     */
    it('testTabsDirPropAuto', 0, async function (done) {
        console.info('testTabsDirPropAuto START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsDirPropNone
     * @tc.desc      ACE
     */
    it('testTabsDirPropNone', 0, async function (done) {
        console.info('testTabsDirPropNone START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsForPropNull
     * @tc.desc      ACE
     */
    it('testTabsForPropNull', 0, async function (done) {
        console.info('testTabsForPropNull START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[tabsProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsForPropOne
     * @tc.desc      ACE
     */
    it('testTabsForPropOne', 0, async function (done) {
        console.info('testTabsForPropOne START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[tabsProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsForPropThree
     * @tc.desc      ACE
     */
    it('testTabsForPropThree', 0, async function (done) {
        console.info('testTabsForPropThree START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[tabsProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsIfPropTrue
     * @tc.desc      ACE
     */
    it('testTabsIfPropTrue', 0, async function (done) {
        console.info('testTabsIfPropTrue START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[tabsProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsShowPropTrue
     * @tc.desc      ACE
     */
    it('testTabsShowPropTrue', 0, async function (done) {
        console.info('testTabsShowPropTrue START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[tabsProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsShowPropFalse
     * @tc.desc      ACE
     */
    it('testTabsShowPropFalse', 0, async function (done) {
        console.info('testTabsShowPropFalse START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[tabsProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsShowPropNone
     * @tc.desc      ACE
     */
    it('testTabsShowPropNone', 0, async function (done) {
        console.info('testTabsShowPropNone START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[tabsProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    //   特有属性

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsIndexNumber
     * @tc.desc      ACE
     */
    it('testTabsIndexNumber', 0, async function (done) {
        console.info('testTabsIndexNumber START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indexNumber);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('indexNumber')
        expect(obj.$attrs.index).assertEqual('0')
        console.info("[tabsProps] get index value is: " + JSON.stringify(obj.$attrs.index));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsVerticalTrue
     * @tc.desc      ACE
     */
    it('testTabsVerticalTrue', 0, async function (done) {
        console.info('testTabsVerticalTrue START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.verticalTrue);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('verticalTrue')
        expect(obj.$attrs.vertical).assertEqual('true')
        console.info("[tabsProps] get vertical value is: " + JSON.stringify(obj.$attrs.vertical));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsVerticalFalse
     * @tc.desc      ACE
     */
    it('testTabsVerticalFalse', 0, async function (done) {
        console.info('testTabsVerticalFalse START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.verticalFalse);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('verticalFalse')
        expect(obj.$attrs.vertical).assertEqual('false')
        console.info("[tabsProps] get vertical value is: " + JSON.stringify(obj.$attrs.vertical));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsVerticalNone
     * @tc.desc      ACE
     */
    it('testTabsVerticalNone', 0, async function (done) {
        console.info('testTabsVerticalNone START');
        console.info("[tabsProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.verticalNone);
        console.info("[tabsProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[tabsProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('tabs')
        expect(obj.$attrs.id).assertEqual('verticalNone')
        expect(obj.$attrs.vertical).assertEqual('false')
        console.info("[tabsProps] get vertical value is: " + JSON.stringify(obj.$attrs.vertical));
        done();
    });
});
