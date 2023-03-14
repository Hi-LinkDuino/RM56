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


describe('formPropsJsTest', function () {

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
                })
                resolve()
            }, 500)
        })
        let clearPromise = new Promise((resolve, reject) => {
            setTimeout(() => {
                router.clear()
                resolve()
            }, 500);
        })
        await backToIndexPromise.then(() => {
            return clearPromise
        })
    }

    /**
    * run before testcase
    */
    beforeAll(async function (done) {
        console.info('[formPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/form/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push formProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push formProps page error " + JSON.stringify(result));
        }
        await sleep(20000)
        done()
    })

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[formPropsJsTest] after each called')
        await backToIndex();
        await sleep(1000)
    })

    // 通用属性

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformIdProp
     * @tc.desc      ACE
     */
    it('testformIdProp', 0, async function (done) {
        console.info('testformIdProp START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    })

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformClassProp
     * @tc.desc      ACE
     */
    it('testformClassProp', 0, async function (done) {
        console.info('testformClassProp START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    })

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformClassPropNone
     * @tc.desc      ACE
     */
    it('testformClassPropNone', 0, async function (done) {
        console.info('testformClassPropNone START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[formProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    })

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformRefProp
     * @tc.desc      ACE
     */
    it('testformRefProp', 0, async function (done) {
        console.info('testformRefProp START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformRefPropNone
     * @tc.desc      ACE
     */
    it('testformRefPropNone', 0, async function (done) {
        console.info('testformRefPropNone START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[formProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testformDisabledPropTrue', 0, async function (done) {
        console.info('testformDisabledPropTrue START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testformDisabledPropFalse', 0, async function (done) {
        console.info('testformDisabledPropFalse START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformDisabledPropNone
     * @tc.desc      ACE
     */
    it('testformDisabledPropNone', 0, async function (done) {
        console.info('testformDisabledPropNone START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testformFocusablePropTrue', 0, async function (done) {
        console.info('testformFocusablePropTrue START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testformFocusablePropFalse', 0, async function (done) {
        console.info('testformFocusablePropFalse START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformFocusablePropNone
     * @tc.desc      ACE
     */
    it('testformFocusablePropNone', 0, async function (done) {
        console.info('testformFocusablePropNone START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual(undefined)
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformDataProp
     * @tc.desc      ACE
     */
    it('testformDataProp', 0, async function (done) {
        console.info('testformDataProp START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataform).assertEqual(undefined);
        console.info("[formProps] get dataform value is: " + JSON.stringify(obj.$attrs.dataform));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformDataPropNone
     * @tc.desc      ACE
     */
    it('testformDataPropNone', 0, async function (done) {
        console.info('testformDataPropNone START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataform).assertEqual(undefined)
        console.info("[formProps] get dataform value is: " + JSON.stringify(obj.$attrs.dataform));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testformClickEffectPropSmall', 0, async function (done) {
        console.info('testformClickEffectPropSmall START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testformClickEffectPropMedium', 0, async function (done) {
        console.info('testformClickEffectPropMedium START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testformClickEffectPropLarge', 0, async function (done) {
        console.info('testformClickEffectPropLarge START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testformClickEffectPropNone', 0, async function (done) {
        console.info('testformClickEffectPropNone START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[formProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformDirPropRtl
     * @tc.desc      ACE
     */
    it('testformDirPropRtl', 0, async function (done) {
        console.info('testformDirPropRtl START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformDirPropLtr
     * @tc.desc      ACE
     */
    it('testformDirPropLtr', 0, async function (done) {
        console.info('testformDirPropLtr START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformDirPropAuto
     * @tc.desc      ACE
     */
    it('testformDirPropAuto', 0, async function (done) {
        console.info('testformDirPropAuto START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformDirPropNone
     * @tc.desc      ACE
     */
    it('testformDirPropNone', 0, async function (done) {
        console.info('testformDirPropNone START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformForPropNull
     * @tc.desc      ACE
     */
    it('testformForPropNull', 0, async function (done) {
        console.info('testformForPropNull START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[formProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformForPropOne
     * @tc.desc      ACE
     */
    it('testformForPropOne', 0, async function (done) {
        console.info('testformForPropOne START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[formProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformForPropThree
     * @tc.desc      ACE
     */
    it('testformForPropThree', 0, async function (done) {
        console.info('testformForPropThree START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[formProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformIfPropTrue
     * @tc.desc      ACE
     */
    it('testformIfPropTrue', 0, async function (done) {
        console.info('testformIfPropTrue START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[formProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformShowPropTrue
     * @tc.desc      ACE
     */
    it('testformShowPropTrue', 0, async function (done) {
        console.info('testformShowPropTrue START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[formProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformShowPropFalse
     * @tc.desc      ACE
     */
    it('testformShowPropFalse', 0, async function (done) {
        console.info('testformShowPropFalse START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[formProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testformShowPropNone
     * @tc.desc      ACE
     */
    it('testformShowPropNone', 0, async function (done) {
        console.info('testformShowPropNone START');
        console.info("[formProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[formProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[formProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('form')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[formProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    // 特有属性
});
