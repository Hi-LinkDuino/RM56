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


describe('chartPropsJsTest', function () {

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
        console.info('[chartPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/chart/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push chartProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push chartProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[chartPropsJsTest] after each called')
        await backToIndex();
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartIdProp
     * @tc.desc      ACE
     */
    it('testChartIdProp', 0, async function (done) {
        console.info('testChartIdProp START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });
    
    it('testChartClassProp', 0, async function (done) {
        console.info('testChartClassProp START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartClassPropNone
     * @tc.desc      ACE
     */
    it('testChartClassPropNone', 0, async function (done) {
        console.info('testChartClassPropNone START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[chartProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartRefProp
     * @tc.desc      ACE
     */
    it('testChartRefProp', 0, async function (done) {
        console.info('testChartRefProp START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartRefPropNone
     * @tc.desc      ACE
     */
    it('testChartRefPropNone', 0, async function (done) {
        console.info('testChartRefPropNone START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[chartProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testChartDisabledPropTrue', 0, async function (done) {
        console.info('testChartDisabledPropTrue START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testChartDisabledPropFalse', 0, async function (done) {
        console.info('testChartDisabledPropFalse START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartDisabledPropNone
     * @tc.desc      ACE
     */
    it('testChartDisabledPropNone', 0, async function (done) {
        console.info('testChartDisabledPropNone START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testChartFocusablePropTrue', 0, async function (done) {
        console.info('testChartFocusablePropTrue START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testChartFocusablePropFalse', 0, async function (done) {
        console.info('testChartFocusablePropFalse START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartFocusablePropNone
     * @tc.desc      ACE
     */
    it('testChartFocusablePropNone', 0, async function (done) {
        console.info('testChartFocusablePropNone START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartDataProp
     * @tc.desc      ACE
     */
    it('testChartDataProp', 0, async function (done) {
        console.info('testChartDataProp START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataChart).assertEqual(undefined);
        console.info("[chartProps] get dataChart value is: " + JSON.stringify(obj.$attrs.dataChart));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartDataPropNone
     * @tc.desc      ACE
     */
    it('testChartDataPropNone', 0, async function (done) {
        console.info('testChartDataPropNone START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataChart).assertEqual(undefined)
        console.info("[chartProps] get dataChart value is: " + JSON.stringify(obj.$attrs.dataChart));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testChartClickEffectPropSmall', 0, async function (done) {
        console.info('testChartClickEffectPropSmall START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testChartClickEffectPropMedium', 0, async function (done) {
        console.info('testChartClickEffectPropMedium START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testChartClickEffectPropLarge', 0, async function (done) {
        console.info('testChartClickEffectPropLarge START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testChartClickEffectPropNone', 0, async function (done) {
        console.info('testChartClickEffectPropNone START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[chartProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartDirPropRtl
     * @tc.desc      ACE
     */
    it('testChartDirPropRtl', 0, async function (done) {
        console.info('testChartDirPropRtl START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartDirPropLtr
     * @tc.desc      ACE
     */
    it('testChartDirPropLtr', 0, async function (done) {
        console.info('testChartDirPropLtr START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartDirPropAuto
     * @tc.desc      ACE
     */
    it('testChartDirPropAuto', 0, async function (done) {
        console.info('testChartDirPropAuto START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartDirPropNone
     * @tc.desc      ACE
     */
    it('testChartDirPropNone', 0, async function (done) {
        console.info('testChartDirPropNone START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartForPropNull
     * @tc.desc      ACE
     */
    it('testChartForPropNull', 0, async function (done) {
        console.info('testChartForPropNull START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[chartProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartForPropOne
     * @tc.desc      ACE
     */
    it('testChartForPropOne', 0, async function (done) {
        console.info('testChartForPropOne START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[chartProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartForPropThree
     * @tc.desc      ACE
     */
    it('testChartForPropThree', 0, async function (done) {
        console.info('testChartForPropThree START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[chartProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartIfPropTrue
     * @tc.desc      ACE
     */
    it('testChartIfPropTrue', 0, async function (done) {
        console.info('testChartIfPropTrue START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[chartProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartShowPropTrue
     * @tc.desc      ACE
     */
    it('testChartShowPropTrue', 0, async function (done) {
        console.info('testChartShowPropTrue START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[chartProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartShowPropFalse
     * @tc.desc      ACE
     */
    it('testChartShowPropFalse', 0, async function (done) {
        console.info('testChartShowPropFalse START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[chartProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartShowPropNone
     * @tc.desc      ACE
     */
    it('testChartShowPropNone', 0, async function (done) {
        console.info('testChartShowPropNone START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[chartProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    //  特有属性

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartTypeBar
     * @tc.desc      ACE
     */
    it('testChartTypeBar', 0, async function (done) {
        console.info('testChartTypeBar START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeBar);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('typeBar')
        expect(obj.$attrs.type).assertEqual('bar')
        console.info("[chartProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartTypeLine
     * @tc.desc      ACE
     */
    it('testChartTypeLine', 0, async function (done) {
        console.info('testChartTypeLine START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeLine);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('typeLine')
        expect(obj.$attrs.type).assertEqual('line')
        console.info("[chartProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartTypeGauge
     * @tc.desc      ACE
     */
    it('testChartTypeGauge', 0, async function (done) {
        console.info('testChartTypeGauge START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeGauge);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('typeGauge')
        expect(obj.$attrs.type).assertEqual('gauge')
        console.info("[chartProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartTypeProgress
     * @tc.desc      ACE
     */
    it('testChartTypeProgress', 0, async function (done) {
        console.info('testChartTypeProgress START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeProgress);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('typeProgress')
        expect(obj.$attrs.type).assertEqual('progress')
        console.info("[chartProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartTypeLoading
     * @tc.desc      ACE
     */
    it('testChartTypeLoading', 0, async function (done) {
        console.info('testChartTypeLoading START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeLoading);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('typeLoading')
        expect(obj.$attrs.type).assertEqual('loading')
        console.info("[chartProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartTypeRainbow
     * @tc.desc      ACE
     */
    it('testChartTypeRainbow', 0, async function (done) {
        console.info('testChartTypeRainbow START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeRainbow);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('typeRainbow')
        expect(obj.$attrs.type).assertEqual('rainbow')
        console.info("[chartProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartTypeNone
     * @tc.desc      ACE
     */
    it('testChartTypeNone', 0, async function (done) {
        console.info('testChartTypeNone START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeNone);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('typeNone')
        expect(obj.$attrs.type).assertEqual('line')
        console.info("[chartProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartOptions
     * @tc.desc      ACE
     */
    it('testChartOptions', 0, async function (done) {
        console.info('testChartOptions START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.options);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('options')
        expect(obj.$attrs.options).assertEqual(undefined)
        console.info("[chartProps] get options value is: " + JSON.stringify(obj.$attrs.options));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartOptionsNone
     * @tc.desc      ACE
     */
    it('testChartOptionsNone', 0, async function (done) {
        console.info('testChartOptionsNone START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.optionsNone);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('optionsNone')
        expect(obj.$attrs.options).assertEqual(undefined)
        console.info("[chartProps] get options value is: " + JSON.stringify(obj.$attrs.options));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartDatasets
     * @tc.desc      ACE
     */
    it('testChartDatasets', 0, async function (done) {
        console.info('testChartDatasets START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.datasets);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('datasets')
        expect(obj.$attrs.datasets).assertEqual(undefined)
        console.info("[chartProps] get datasets value is: " + JSON.stringify(obj.$attrs.datasets));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartDatasetsNone
     * @tc.desc      ACE
     */
    it('testChartDatasetsNone', 0, async function (done) {
        console.info('testChartDatasetsNone START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.datasetsNone);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('datasetsNone')
        expect(obj.$attrs.datasets).assertEqual(undefined)
        console.info("[chartProps] get datasets value is: " + JSON.stringify(obj.$attrs.datasets));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartSegmentsSeg
     * @tc.desc      ACE
     */
    it('testChartSegmentsSeg', 0, async function (done) {
        console.info('testChartSegmentsSeg START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.segmentsSeg);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('segmentsSeg')
        expect(obj.$attrs.segments).assertEqual(undefined)
        console.info("[chartProps] get segments value is: " + JSON.stringify(obj.$attrs.segments));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartSegmentsRain
     * @tc.desc      ACE
     */
    it('testChartSegmentsRain', 0, async function (done) {
        console.info('testChartSegmentsRain START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.segmentsRain);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('segmentsRain')
        expect(obj.$attrs.segments).assertEqual(undefined)
        console.info("[chartProps] get segments value is: " + JSON.stringify(obj.$attrs.segments));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartSegmentsNone
     * @tc.desc      ACE
     */
    it('testChartSegmentsNone', 0, async function (done) {
        console.info('testChartSegmentsNone START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.segmentsNone);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('segmentsNone')
        expect(obj.$attrs.segments).assertEqual(undefined)
        console.info("[chartProps] get segments value is: " + JSON.stringify(obj.$attrs.segments));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartEffectsTrue
     * @tc.desc      ACE
     */
    it('testChartEffectsTrue', 0, async function (done) {
        console.info('testChartEffectsTrue START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.effectsTrue);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('effectsTrue')
        expect(obj.$attrs.effects).assertEqual('true')
        console.info("[chartProps] get effects value is: " + JSON.stringify(obj.$attrs.effects));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartEffectsFalse
     * @tc.desc      ACE
     */
    it('testChartEffectsFalse', 0, async function (done) {
        console.info('testChartEffectsFalse START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.effectsFalse);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('effectsFalse')
        expect(obj.$attrs.effects).assertEqual('false')
        console.info("[chartProps] get effects value is: " + JSON.stringify(obj.$attrs.effects));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartEffectsNone
     * @tc.desc      ACE
     */
    it('testChartEffectsNone', 0, async function (done) {
        console.info('testChartEffectsNone START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.effectsNone);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('effectsNone')
        expect(obj.$attrs.effects).assertEqual('true')
        console.info("[chartProps] get effects value is: " + JSON.stringify(obj.$attrs.effects));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartAnimationduration
     * @tc.desc      ACE
     */
    it('testChartAnimationduration', 0, async function (done) {
        console.info('testChartAnimationduration START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.animationduration);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('animationduration')
        expect(obj.$attrs.animationduration).assertEqual('3000')
        console.info("[chartProps] get animationduration value is: " + JSON.stringify(obj.$attrs.animationduration));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testChartAnimationdurationNone
     * @tc.desc      ACE
     */
    it('testChartAnimationdurationNone', 0, async function (done) {
        console.info('testChartAnimationdurationNone START');
        console.info("[chartProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.animationdurationNone);
        console.info("[chartProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[chartProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('chart')
        expect(obj.$attrs.id).assertEqual('animationdurationNone')
        expect(obj.$attrs.animationduration).assertEqual('3000')
        console.info("[chartProps] get animationduration value is: " + JSON.stringify(obj.$attrs.animationduration));
        done();
    });
});
