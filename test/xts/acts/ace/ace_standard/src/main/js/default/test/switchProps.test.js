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


describe('switchPropsJsTest', function () {

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
                router.clear()
                resolve()
            }, 500)
        })
        await backToIndexPromise.then(() => {
            return clearPromise
        })
    }

    /**
    * run before testcase
    */
    beforeAll(async function (done) {
        console.info('[switchPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/switch/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push switchProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push switchProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    })

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[switchPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    })

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchIdProp
     * @tc.desc      ACE
     */
    it('testSwitchIdProp', 0, async function (done) {
        console.info('testSwitchIdProp START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    it('testSwitchClassProp', 0, async function (done) {
        console.info('testSwitchClassProp START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchClassPropNone
     * @tc.desc      ACE
     */
    it('testSwitchClassPropNone', 0, async function (done) {
        console.info('testSwitchClassPropNone START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[switchProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchRefProp
     * @tc.desc      ACE
     */
    it('testSwitchRefProp', 0, async function (done) {
        console.info('testSwitchRefProp START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchRefPropNone
     * @tc.desc      ACE
     */
    it('testSwitchRefPropNone', 0, async function (done) {
        console.info('testSwitchRefPropNone START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[switchProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testSwitchDisabledPropTrue', 0, async function (done) {
        console.info('testSwitchDisabledPropTrue START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testSwitchDisabledPropFalse', 0, async function (done) {
        console.info('testSwitchDisabledPropFalse START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchDisabledPropNone
     * @tc.desc      ACE
     */
    it('testSwitchDisabledPropNone', 0, async function (done) {
        console.info('testSwitchDisabledPropNone START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testSwitchFocusablePropTrue', 0, async function (done) {
        console.info('testSwitchFocusablePropTrue START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testSwitchFocusablePropFalse', 0, async function (done) {
        console.info('testSwitchFocusablePropFalse START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchFocusablePropNone
     * @tc.desc      ACE
     */
    it('testSwitchFocusablePropNone', 0, async function (done) {
        console.info('testSwitchFocusablePropNone START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchDataProp
     * @tc.desc      ACE
     */
    it('testSwitchDataProp', 0, async function (done) {
        console.info('testSwitchDataProp START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataSwitch).assertEqual(undefined);
        console.info("[switchProps] get dataSwitch value is: " + JSON.stringify(obj.$attrs.dataSwitch));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchDataPropNone
     * @tc.desc      ACE
     */
    it('testSwitchDataPropNone', 0, async function (done) {
        console.info('testSwitchDataPropNone START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataSwitch).assertEqual(undefined)
        console.info("[switchProps] get dataSwitch value is: " + JSON.stringify(obj.$attrs.dataSwitch));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testSwitchClickEffectPropSmall', 0, async function (done) {
        console.info('testSwitchClickEffectPropSmall START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testSwitchClickEffectPropMedium', 0, async function (done) {
        console.info('testSwitchClickEffectPropMedium START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testSwitchClickEffectPropLarge', 0, async function (done) {
        console.info('testSwitchClickEffectPropLarge START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testSwitchClickEffectPropNone', 0, async function (done) {
        console.info('testSwitchClickEffectPropNone START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[switchProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchDirPropRtl
     * @tc.desc      ACE
     */
    it('testSwitchDirPropRtl', 0, async function (done) {
        console.info('testSwitchDirPropRtl START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchDirPropLtr
     * @tc.desc      ACE
     */
    it('testSwitchDirPropLtr', 0, async function (done) {
        console.info('testSwitchDirPropLtr START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchDirPropAuto
     * @tc.desc      ACE
     */
    it('testSwitchDirPropAuto', 0, async function (done) {
        console.info('testSwitchDirPropAuto START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchDirPropNone
     * @tc.desc      ACE
     */
    it('testSwitchDirPropNone', 0, async function (done) {
        console.info('testSwitchDirPropNone START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchForPropNull
     * @tc.desc      ACE
     */
    it('testSwitchForPropNull', 0, async function (done) {
        console.info('testSwitchForPropNull START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[switchProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchForPropOne
     * @tc.desc      ACE
     */
    it('testSwitchForPropOne', 0, async function (done) {
        console.info('testSwitchForPropOne START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[switchProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchForPropThree
     * @tc.desc      ACE
     */
    it('testSwitchForPropThree', 0, async function (done) {
        console.info('testSwitchForPropThree START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[switchProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchIfPropTrue
     * @tc.desc      ACE
     */
    it('testSwitchIfPropTrue', 0, async function (done) {
        console.info('testSwitchIfPropTrue START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[switchProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchShowPropTrue
     * @tc.desc      ACE
     */
    it('testSwitchShowPropTrue', 0, async function (done) {
        console.info('testSwitchShowPropTrue START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[switchProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchShowPropFalse
     * @tc.desc      ACE
     */
    it('testSwitchShowPropFalse', 0, async function (done) {
        console.info('testSwitchShowPropFalse START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[switchProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchShowPropNone
     * @tc.desc      ACE
     */
    it('testSwitchShowPropNone', 0, async function (done) {
        console.info('testSwitchShowPropNone START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[switchProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    //   特有属性

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchCheckedTrue
     * @tc.desc      ACE
     */
    it('testSwitchCheckedTrue', 0, async function (done) {
        console.info('testSwitchCheckedTrue START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.checkedTrue);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('checkedTrue')
        expect(obj.$attrs.checked).assertEqual('true')
        console.info("[switchProps] get checked value is: " + JSON.stringify(obj.$attrs.checked));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchCheckedFalse
     * @tc.desc      ACE
     */
    it('testSwitchCheckedFalse', 0, async function (done) {
        console.info('testSwitchCheckedFalse START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.checkedFalse);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('checkedFalse')
        expect(obj.$attrs.checked).assertEqual('false')
        console.info("[switchProps] get checked value is: " + JSON.stringify(obj.$attrs.checked));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchCheckedNone
     * @tc.desc      ACE
     */
    it('testSwitchCheckedNone', 0, async function (done) {
        console.info('testSwitchCheckedNone START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.checkedNone);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('checkedNone')
        expect(obj.$attrs.checked).assertEqual('false')
        console.info("[switchProps] get checked value is: " + JSON.stringify(obj.$attrs.checked));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchShowTextTrue
     * @tc.desc      ACE
     */
    it('testSwitchShowTextTrue', 0, async function (done) {
        console.info('testSwitchShowTextTrue START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showTextTrue);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('showTextTrue')
        expect(obj.$attrs.showtext).assertEqual("true")
        console.info("[switchProps] get showtext value is: " + JSON.stringify(obj.$attrs.showtext));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchShowTextFalse
     * @tc.desc      ACE
     */
    it('testSwitchShowTextFalse', 0, async function (done) {
        console.info('testSwitchShowTextFalse START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showTextFalse);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('showTextFalse')
        expect(obj.$attrs.showtext).assertEqual("false")
        console.info("[switchProps] get showtext value is: " + JSON.stringify(obj.$attrs.showtext));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchShowTextNone
     * @tc.desc      ACE
     */
    it('testSwitchShowTextNone', 0, async function (done) {
        console.info('testSwitchShowTextNone START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showTextNone);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('showTextNone')
        expect(obj.$attrs.showtext).assertEqual('false')
        console.info("[switchProps] get showtext value is: " + JSON.stringify(obj.$attrs.showtext));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchTextOn
     * @tc.desc      ACE
     */
    it('testSwitchTextOn', 0, async function (done) {
        console.info('testSwitchTextOn START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.textOn);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('textOn')
        expect(obj.$attrs.texton).assertEqual("开")
        console.info("[switchProps] get texton value is: " + JSON.stringify(obj.$attrs.texton));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchTextOnNone
     * @tc.desc      ACE
     */
    it('testSwitchTextOnNone', 0, async function (done) {
        console.info('testSwitchTextOnNone START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.textOnNone);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('textOnNone')
        expect(obj.$attrs.texton).assertEqual('On')
        console.info("[switchProps] get texton value is: " + JSON.stringify(obj.$attrs.texton));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchTextOff
     * @tc.desc      ACE
     */
    it('testSwitchTextOff', 0, async function (done) {
        console.info('testSwitchTextOff START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.textOff);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('textOff')
        expect(obj.$attrs.textoff).assertEqual("关")
        console.info("[switchProps] get textoff value is: " + JSON.stringify(obj.$attrs.textoff));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwitchTextOffNone
     * @tc.desc      ACE
     */
    it('testSwitchTextOffNone', 0, async function (done) {
        console.info('testSwitchTextOffNone START');
        console.info("[switchProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.textOffNone);
        console.info("[switchProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[switchProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('switch')
        expect(obj.$attrs.id).assertEqual('textOffNone')
        expect(obj.$attrs.textoff).assertEqual('Off')
        console.info("[switchProps] get textoff value is: " + JSON.stringify(obj.$attrs.textoff));
        done();
    });
});
