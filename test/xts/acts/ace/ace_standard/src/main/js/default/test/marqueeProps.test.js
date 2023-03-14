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


describe('marqueePropsJsTest', function () {

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
        console.info('[marqueePropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/marquee/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push marqueeProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push marqueeProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[marqueePropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeIdProp
     * @tc.desc      ACE
     */
    it('testmarqueeIdProp', 0, async function (done) {
        console.info('testmarqueeIdProp START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeClassProp
     * @tc.desc      ACE
     */
    it('testmarqueeClassProp', 0, async function (done) {
        console.info('testmarqueeClassProp START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeClassPropNone
     * @tc.desc      ACE
     */
    it('testmarqueeClassPropNone', 0, async function (done) {
        console.info('testmarqueeClassPropNone START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[marqueeProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeRefProp
     * @tc.desc      ACE
     */
    it('testmarqueeRefProp', 0, async function (done) {
        console.info('testmarqueeRefProp START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeRefPropNone
     * @tc.desc      ACE
     */
    it('testmarqueeRefPropNone', 0, async function (done) {
        console.info('testmarqueeRefPropNone START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[marqueeProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testmarqueeDisabledPropTrue', 0, async function (done) {
        console.info('testmarqueeDisabledPropTrue START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testmarqueeDisabledPropFalse', 0, async function (done) {
        console.info('testmarqueeDisabledPropFalse START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeDisabledPropNone
     * @tc.desc      ACE
     */
    it('testmarqueeDisabledPropNone', 0, async function (done) {
        console.info('testmarqueeDisabledPropNone START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testmarqueeFocusablePropTrue', 0, async function (done) {
        console.info('testmarqueeFocusablePropTrue START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testmarqueeFocusablePropFalse', 0, async function (done) {
        console.info('testmarqueeFocusablePropFalse START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeFocusablePropNone
     * @tc.desc      ACE
     */
    it('testmarqueeFocusablePropNone', 0, async function (done) {
        console.info('testmarqueeFocusablePropNone START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeDataProp
     * @tc.desc      ACE
     */
    it('testmarqueeDataProp', 0, async function (done) {
        console.info('testmarqueeDataProp START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.datamarquee).assertEqual(undefined);
        console.info("[marqueeProps] get datamarquee value is: " + JSON.stringify(obj.$attrs.datamarquee));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeDataPropNone
     * @tc.desc      ACE
     */
    it('testmarqueeDataPropNone', 0, async function (done) {
        console.info('testmarqueeDataPropNone START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.datamarquee).assertEqual(undefined)
        console.info("[marqueeProps] get datamarquee value is: " + JSON.stringify(obj.$attrs.datamarquee));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testmarqueeClickEffectPropSmall', 0, async function (done) {
        console.info('testmarqueeClickEffectPropSmall START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testmarqueeClickEffectPropMedium', 0, async function (done) {
        console.info('testmarqueeClickEffectPropMedium START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testmarqueeClickEffectPropLarge', 0, async function (done) {
        console.info('testmarqueeClickEffectPropLarge START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testmarqueeClickEffectPropNone', 0, async function (done) {
        console.info('testmarqueeClickEffectPropNone START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[marqueeProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeDirPropRtl
     * @tc.desc      ACE
     */
    it('testmarqueeDirPropRtl', 0, async function (done) {
        console.info('testmarqueeDirPropRtl START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeDirPropLtr
     * @tc.desc      ACE
     */
    it('testmarqueeDirPropLtr', 0, async function (done) {
        console.info('testmarqueeDirPropLtr START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeDirPropAuto
     * @tc.desc      ACE
     */
    it('testmarqueeDirPropAuto', 0, async function (done) {
        console.info('testmarqueeDirPropAuto START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeDirPropNone
     * @tc.desc      ACE
     */
    it('testmarqueeDirPropNone', 0, async function (done) {
        console.info('testmarqueeDirPropNone START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeForPropNull
     * @tc.desc      ACE
     */
    it('testmarqueeForPropNull', 0, async function (done) {
        console.info('testmarqueeForPropNull START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[marqueeProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeForPropOne
     * @tc.desc      ACE
     */
    it('testmarqueeForPropOne', 0, async function (done) {
        console.info('testmarqueeForPropOne START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[marqueeProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeForPropThree
     * @tc.desc      ACE
     */
    it('testmarqueeForPropThree', 0, async function (done) {
        console.info('testmarqueeForPropThree START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[marqueeProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeIfPropTrue
     * @tc.desc      ACE
     */
    it('testmarqueeIfPropTrue', 0, async function (done) {
        console.info('testmarqueeIfPropTrue START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[marqueeProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeShowPropTrue
     * @tc.desc      ACE
     */
    it('testmarqueeShowPropTrue', 0, async function (done) {
        console.info('testmarqueeShowPropTrue START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[marqueeProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeShowPropFalse
     * @tc.desc      ACE
     */
    it('testmarqueeShowPropFalse', 0, async function (done) {
        console.info('testmarqueeShowPropFalse START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[marqueeProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeShowPropNone
     * @tc.desc      ACE
     */
    it('testmarqueeShowPropNone', 0, async function (done) {
        console.info('testmarqueeShowPropNone START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual("false")
        console.info("[marqueeProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeShowPropNone
     * @tc.desc      ACE
     */
    it('testmarqueeScrollamountPropNone', 0, async function (done) {
        console.info('testmarqueeXsPropNone START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeScrollamountNull);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('typeScrollamountNull')
        expect(obj.$attrs.scrollamount).assertEqual("6")
        console.info("[marqueeProps] get scrollamount value is: " + JSON.stringify(obj.$attrs.scrollamount));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeShowPropNone
     * @tc.desc      ACE
     */
    it('testmarqueeScrollamountProp10', 0, async function (done) {
        console.info('testmarqueeXsPropNone START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeScrollamount10);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('typeScrollamount10')
        expect(obj.$attrs.scrollamount).assertEqual('10')
        console.info("[marqueeProps] get scrollamount value is: " + JSON.stringify(obj.$attrs.scrollamount));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeShowPropNone
     * @tc.desc      ACE
     */
    it('testmarqueeScrollamountProp20', 0, async function (done) {
        console.info('testmarqueeXsPropNone START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeScrollamount20);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('typeScrollamount20')
        expect(obj.$attrs.scrollamount).assertEqual('20')
        console.info("[marqueeProps] get scrollamount value is: " + JSON.stringify(obj.$attrs.scrollamount));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeShowPropNone
     * @tc.desc      ACE
     */
it('testmarqueeLoopPropNull', 0, async function (done) {
    console.info('testmarqueeXsPropObject START');
    console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

    let obj = JSON.parse(globalThis.value.typeLoopNull);
    console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
    console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

    expect(obj.$type).assertEqual('marquee')
    expect(obj.$attrs.id).assertEqual('typeLoopNull')
    expect(obj.$attrs.loop).assertEqual('-1')
    console.info("[marqueeProps] get loop value is: " + JSON.stringify(obj.$attrs.loop));
    done();
});

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeShowPropNone
     * @tc.desc      ACE
     */
    it('testmarqueeLoopProp1', 0, async function (done) {
        console.info('testmarqueeXsPropObject START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeLoop1);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('typeLoop1')
        expect(obj.$attrs.loop).assertEqual("-1")
        console.info("[marqueeProps] get loop value is: " + JSON.stringify(obj.$attrs.loop));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeShowPropNone
     * @tc.desc      ACE
     */
    it('testmarqueeLoopProp3', 0, async function (done) {
        console.info('testmarqueeXsPropObject START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeLoop3);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('typeLoop3')
        expect(obj.$attrs.loop).assertEqual("3")
        console.info("[marqueeProps] get loop value is: " + JSON.stringify(obj.$attrs.loop));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeShowPropNone
     * @tc.desc      ACE
     */
    it('testmarqueeDirectionPropNull', 0, async function (done) {
        console.info('testmarqueeXsPropObject START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeDirectionNull);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('typeDirectionNull')
        expect(obj.$attrs.direction).assertEqual("left")
        console.info("[marqueeProps] get direction value is: " + JSON.stringify(obj.$attrs.direction));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeShowPropNone
     * @tc.desc      ACE
     */
    it('testmarqueeDirectionPropRight', 0, async function (done) {
        console.info('testmarqueeXsPropObject START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeDirectionRight);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('typeDirectionRight')
        expect(obj.$attrs.direction).assertEqual("right")
        console.info("[marqueeProps] get direction value is: " + JSON.stringify(obj.$attrs.direction));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testmarqueeShowPropNone
     * @tc.desc      ACE
     */
    it('testmarqueeDirectionPropLeft', 0, async function (done) {
        console.info('testmarqueeXsPropObject START');
        console.info("[marqueeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeDirectionLeft);
        console.info("[marqueeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[marqueeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('marquee')
        expect(obj.$attrs.id).assertEqual('typeDirectionLeft')
        expect(obj.$attrs.direction).assertEqual("left")
        console.info("[marqueeProps] get direction value is: " + JSON.stringify(obj.$attrs.direction));
        done();
    });

});



