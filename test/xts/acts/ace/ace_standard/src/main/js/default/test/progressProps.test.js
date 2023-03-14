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


describe('progressPropsJsTest', function () {

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
                router.clear();
                resolve();
            }, 500);
        })
        await backToIndexPromise.then(() => {
            return clearPromise;
        })
    }

    /**
    * run before testcase
    */
    beforeAll(async function (done) {
        console.info('[progressPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/progress/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push progressProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push progressProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    })

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[progressPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    })

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressIdProp
     * @tc.desc      ACE
     */
    it('testProgressIdProp', 0, async function (done) {
        console.info('testProgressIdProp START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressClassProp
     * @tc.desc      ACE
     */
    it('testProgressClassProp', 0, async function (done) {
        console.info('testProgressClassProp START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressClassPropNone
     * @tc.desc      ACE
     */
    it('testProgressClassPropNone', 0, async function (done) {
        console.info('testProgressClassPropNone START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[progressProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressStyleProp
     * @tc.desc      ACEs
     */
    //    it('testProgressStyleProp', 0, async function (done) {
    //        console.info('testProgressStyleProp START');
    //        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.styleProp);
    //        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('progress')
    //        expect(obj.$attrs.id).assertEqual('styleProp')
    //        expect(obj.$attrs.style).assertEqual(undefined)
    //        console.info("[progressProps] get style value is: " + JSON.stringify(obj.$attrs.style));
    //
    //        expect(obj.$styles.width).assertEqual(undefined);
    //        console.info("[progressProps] get style width value is: " + JSON.stringify(obj.$styles.width));
    //        expect(obj.$styles.height).assertEqual(undefined);
    //        console.info("[progressProps] get style height value is: " + JSON.stringify(obj.$styles.height));
    //        expect(obj.$styles.get('background-color')).assertEqual(undefined);
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressRefProp
     * @tc.desc      ACE
     */
    it('testProgressRefProp', 0, async function (done) {
        console.info('testProgressRefProp START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressRefPropNone
     * @tc.desc      ACE
     */
    it('testProgressRefPropNone', 0, async function (done) {
        console.info('testProgressRefPropNone START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[progressProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testProgressDisabledPropTrue', 0, async function (done) {
        console.info('testProgressDisabledPropTrue START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testProgressDisabledPropFalse', 0, async function (done) {
        console.info('testProgressDisabledPropFalse START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressDisabledPropNone
     * @tc.desc      ACE
     */
    it('testProgressDisabledPropNone', 0, async function (done) {
        console.info('testProgressDisabledPropNone START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testProgressFocusablePropTrue', 0, async function (done) {
        console.info('testProgressFocusablePropTrue START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testProgressFocusablePropFalse', 0, async function (done) {
        console.info('testProgressFocusablePropFalse START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressFocusablePropNone
     * @tc.desc      ACE
     */
    it('testProgressFocusablePropNone', 0, async function (done) {
        console.info('testProgressFocusablePropNone START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressDataProp
     * @tc.desc      ACE
     */
    it('testProgressDataProp', 0, async function (done) {
        console.info('testProgressDataProp START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataprogress).assertEqual(undefined);
        console.info("[progressProps] get dataprogress value is: " + JSON.stringify(obj.$attrs.dataprogress));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressDataPropNone
     * @tc.desc      ACE
     */
    it('testProgressDataPropNone', 0, async function (done) {
        console.info('testProgressDataPropNone START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataprogress).assertEqual(undefined)
        console.info("[progressProps] get dataprogress value is: " + JSON.stringify(obj.$attrs.dataprogress));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testProgressClickEffectPropSmall', 0, async function (done) {
        console.info('testProgressClickEffectPropSmall START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testProgressClickEffectPropMedium', 0, async function (done) {
        console.info('testProgressClickEffectPropMedium START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testProgressClickEffectPropLarge', 0, async function (done) {
        console.info('testProgressClickEffectPropLarge START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testProgressClickEffectPropNone', 0, async function (done) {
        console.info('testProgressClickEffectPropNone START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[progressProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressDirPropRtl
     * @tc.desc      ACE
     */
    it('testProgressDirPropRtl', 0, async function (done) {
        console.info('testProgressDirPropRtl START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressDirPropLtr
     * @tc.desc      ACE
     */
    it('testProgressDirPropLtr', 0, async function (done) {
        console.info('testProgressDirPropLtr START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressDirPropAuto
     * @tc.desc      ACE
     */
    it('testProgressDirPropAuto', 0, async function (done) {
        console.info('testProgressDirPropAuto START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressDirPropNone
     * @tc.desc      ACE
     */
    it('testProgressDirPropNone', 0, async function (done) {
        console.info('testProgressDirPropNone START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressForPropNull
     * @tc.desc      ACE
     */
    it('testProgressForPropNull', 0, async function (done) {
        console.info('testProgressForPropNull START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[progressProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressForPropOne
     * @tc.desc      ACE
     */
    it('testProgressForPropOne', 0, async function (done) {
        console.info('testProgressForPropOne START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[progressProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressForPropThree
     * @tc.desc      ACE
     */
    it('testProgressForPropThree', 0, async function (done) {
        console.info('testProgressForPropThree START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[progressProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressIfPropTrue
     * @tc.desc      ACE
     */
    it('testProgressIfPropTrue', 0, async function (done) {
        console.info('testProgressIfPropTrue START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[progressProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testProgressIfPropFalse', 0, async function (done) {
    //        console.info('testProgressIfPropFalse START');
    //        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('progress')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressIfPropNone
     * @tc.desc      ACE
     */
    //    it('testProgressIfPropNone', 0, async function (done) {
    //        console.info('testProgressIfPropNone START');
    //        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('progress')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressShowPropTrue
     * @tc.desc      ACE
     */
    it('testProgressShowPropTrue', 0, async function (done) {
        console.info('testProgressShowPropTrue START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[progressProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressShowPropFalse
     * @tc.desc      ACE
     */
    it('testProgressShowPropFalse', 0, async function (done) {
        console.info('testProgressShowPropFalse START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[progressProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressShowPropNone
     * @tc.desc      ACE
     */
    it('testProgressShowPropNone', 0, async function (done) {
        console.info('testProgressShowPropNone START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[progressProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressTypeHorizontal
     * @tc.desc      ACE
     */
    it('testProgressTypeHorizontal', 0, async function (done) {
        console.info('testProgressTypeHorizontal START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeHorizontal);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('typeHorizontal')
        expect(obj.$attrs.type).assertEqual('horizontal')
        console.info("[progressProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressTypeCircular
     * @tc.desc      ACE
     */
    it('testProgressTypeCircular', 0, async function (done) {
        console.info('testProgressTypeCircular START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeCircular);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('typeCircular')
        expect(obj.$attrs.type).assertEqual('circular')
        console.info("[progressProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressTypeRing
     * @tc.desc      ACE
     */
    it('testProgressTypeRing', 0, async function (done) {
        console.info('testProgressTypeRing START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeRing);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('typeRing')
        expect(obj.$attrs.type).assertEqual('ring')
        console.info("[progressProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressTypeScaleRing
     * @tc.desc      ACE
     */
    it('testProgressTypeScaleRing', 0, async function (done) {
        console.info('testProgressTypeScaleRing START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeScaleRing);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('typeScaleRing')
        expect(obj.$attrs.type).assertEqual('scale-ring')
        console.info("[progressProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressTypeArc
     * @tc.desc      ACE
     */
    it('testProgressTypeArc', 0, async function (done) {
        console.info('testProgressTypeArc START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeArc);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('typeArc')
        expect(obj.$attrs.type).assertEqual('arc')
        console.info("[progressProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testProgressTypeEclipse
     * @tc.desc      ACE
     */
    it('testProgressTypeEclipse', 0, async function (done) {
        console.info('testProgressTypeEclipse START');
        console.info("[progressProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeEclipse);
        console.info("[progressProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[progressProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('progress')
        expect(obj.$attrs.id).assertEqual('typeEclipse')
        expect(obj.$attrs.type).assertEqual('eclipse')
        console.info("[progressProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });
});
