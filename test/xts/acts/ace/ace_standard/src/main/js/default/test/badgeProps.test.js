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


describe('badgePropsJsTest', function () {

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
        console.info('[badgePropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/badge/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push badgeProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push badgeProps page error " + JSON.stringify(result));
        }
        await sleep(20000)
        done()
    })

    /**
     * run after testcase
     */
    afterAll(async function () {
        console.info('[badgePropsJsTest] after each called')
        await backToIndex();
        await sleep(1000)
    })

    // 通用属性

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeIdProp
      * @tc.desc      ACE
      */
    it('testbadgeIdProp', 0, async function (done) {
        console.info('testbadgeIdProp START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    })

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeClassProp
      * @tc.desc      ACE
      */
    it('testbadgeClassProp', 0, async function (done) {
        console.info('testbadgeClassProp START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    })

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeClassPropNone
      * @tc.desc      ACE
      */
    it('testbadgeClassPropNone', 0, async function (done) {
        console.info('testbadgeClassPropNone START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[badgeProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    })

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeRefProp
      * @tc.desc      ACE
      */
    it('testbadgeRefProp', 0, async function (done) {
        console.info('testbadgeRefProp START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeRefPropNone
      * @tc.desc      ACE
      */
    it('testbadgeRefPropNone', 0, async function (done) {
        console.info('testbadgeRefPropNone START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[badgeProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeDisabledPropTrue
      * @tc.desc      ACE
      */
    it('testbadgeDisabledPropTrue', 0, async function (done) {
        console.info('testbadgeDisabledPropTrue START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeDisabledPropFalse
      * @tc.desc      ACE
      */
    it('testbadgeDisabledPropFalse', 0, async function (done) {
        console.info('testbadgeDisabledPropFalse START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeDisabledPropNone
      * @tc.desc      ACE
      */
    it('testbadgeDisabledPropNone', 0, async function (done) {
        console.info('testbadgeDisabledPropNone START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeFocusablePropTrue
      * @tc.desc      ACE
      */
    it('testbadgeFocusablePropTrue', 0, async function (done) {
        console.info('testbadgeFocusablePropTrue START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeFocusablePropFalse
      * @tc.desc      ACE
      */
    it('testbadgeFocusablePropFalse', 0, async function (done) {
        console.info('testbadgeFocusablePropFalse START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeFocusablePropNone
      * @tc.desc      ACE
      */
    it('testbadgeFocusablePropNone', 0, async function (done) {
        console.info('testbadgeFocusablePropNone START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeDataProp
      * @tc.desc      ACE
      */
    it('testbadgeDataProp', 0, async function (done) {
        console.info('testbadgeDataProp START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.databadge).assertEqual(undefined);
        console.info("[badgeProps] get databadge value is: " + JSON.stringify(obj.$attrs.databadge));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeDataPropNone
      * @tc.desc      ACE
      */
    it('testbadgeDataPropNone', 0, async function (done) {
        console.info('testbadgeDataPropNone START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.databadge).assertEqual(undefined)
        console.info("[badgeProps] get databadge value is: " + JSON.stringify(obj.$attrs.databadge));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeClickEffectPropSmall
      * @tc.desc      ACE
      */
    it('testbadgeClickEffectPropSmall', 0, async function (done) {
        console.info('testbadgeClickEffectPropSmall START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeClickEffectPropMedium
      * @tc.desc      ACE
      */
    it('testbadgeClickEffectPropMedium', 0, async function (done) {
        console.info('testbadgeClickEffectPropMedium START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeClickEffectPropLarge
      * @tc.desc      ACE
      */
    it('testbadgeClickEffectPropLarge', 0, async function (done) {
        console.info('testbadgeClickEffectPropLarge START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeClickEffectPropNone
      * @tc.desc      ACE
      */
    it('testbadgeClickEffectPropNone', 0, async function (done) {
        console.info('testbadgeClickEffectPropNone START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[badgeProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeDirPropRtl
      * @tc.desc      ACE
      */
    it('testbadgeDirPropRtl', 0, async function (done) {
        console.info('testbadgeDirPropRtl START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeDirPropLtr
      * @tc.desc      ACE
      */
    it('testbadgeDirPropLtr', 0, async function (done) {
        console.info('testbadgeDirPropLtr START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeDirPropAuto
      * @tc.desc      ACE
      */
    it('testbadgeDirPropAuto', 0, async function (done) {
        console.info('testbadgeDirPropAuto START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeDirPropNone
      * @tc.desc      ACE
      */
    it('testbadgeDirPropNone', 0, async function (done) {
        console.info('testbadgeDirPropNone START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeForPropNull
      * @tc.desc      ACE
      */
    it('testbadgeForPropNull', 0, async function (done) {
        console.info('testbadgeForPropNull START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[badgeProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeForPropOne
      * @tc.desc      ACE
      */
    it('testbadgeForPropOne', 0, async function (done) {
        console.info('testbadgeForPropOne START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[badgeProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeForPropThree
      * @tc.desc      ACE
      */
    it('testbadgeForPropThree', 0, async function (done) {
        console.info('testbadgeForPropThree START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[badgeProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeIfPropTrue
      * @tc.desc      ACE
      */
    it('testbadgeIfPropTrue', 0, async function (done) {
        console.info('testbadgeIfPropTrue START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[badgeProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeShowPropTrue
      * @tc.desc      ACE
      */
    it('testbadgeShowPropTrue', 0, async function (done) {
        console.info('testbadgeShowPropTrue START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[badgeProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeShowPropFalse
      * @tc.desc      ACE
      */
    it('testbadgeShowPropFalse', 0, async function (done) {
        console.info('testbadgeShowPropFalse START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[badgeProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeShowPropNone
      * @tc.desc      ACE
      */
    it('testbadgeShowPropNone', 0, async function (done) {
        console.info('testbadgeShowPropNone START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[badgeProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    // 特有属性

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeplacementRight
      * @tc.desc      ACE
      */
    it('testbadgeplacementRight', 0, async function (done) {
        console.info('testbadgeplacementRight START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.placementRight);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('placementRight')
        expect(obj.$attrs.placement).assertEqual('right')
        console.info("[badgeProps] get placement value is: " + JSON.stringify(obj.$attrs.placement));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeplacementRightTop
      * @tc.desc      ACE
      */
    it('testbadgeplacementRightTop', 0, async function (done) {
        console.info('testbadgeplacementRightTop START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.placementRightTop);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('placementRightTop')
        expect(obj.$attrs.placement).assertEqual('rightTop')
        console.info("[badgeProps] get placement value is: " + JSON.stringify(obj.$attrs.placement));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeplacementLeft
      * @tc.desc      ACE
      */
    it('testbadgeplacementLeft', 0, async function (done) {
        console.info('testbadgeplacementLeft START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.placementLeft);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('placementLeft')
        expect(obj.$attrs.placement).assertEqual('Left')
        console.info("[badgeProps] get placement value is: " + JSON.stringify(obj.$attrs.placement));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgeplacementNone
      * @tc.desc      ACE
      */
    it('testbadgeplacementNone', 0, async function (done) {
        console.info('testbadgeplacementNone START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.placementNone);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('placementNone')
        expect(obj.$attrs.placement).assertEqual(undefined)
        console.info("[badgeProps] get placement value is: " + JSON.stringify(obj.$attrs.placement));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testbadgecount
      * @tc.desc      ACE
      */
    it('testbadgecount', 0, async function (done) {
        console.info('testbadgecount START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.count);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('count')
        expect(obj.$attrs.count).assertEqual('10')
        console.info("[badgeProps] get count value is: " + JSON.stringify(obj.$attrs.count));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testvisibleTrue
      * @tc.desc      ACE
      */
    it('testvisibleTrue', 0, async function (done) {
        console.info('testvisibleTrue START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.visibleTrue);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('visibleTrue')
        expect(obj.$attrs.visible).assertEqual('true')
        console.info("[badgeProps] get visible value is: " + JSON.stringify(obj.$attrs.visible));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testvisibleFalse
      * @tc.desc      ACE
      */
    it('testvisibleFalse', 0, async function (done) {
        console.info('testvisibleFalse START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.visibleFalse);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('visibleFalse')
        expect(obj.$attrs.visible).assertEqual('false')
        console.info("[badgeProps] get visible value is: " + JSON.stringify(obj.$attrs.visible));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testvisibleNone
      * @tc.desc      ACE
      */
    it('testvisibleNone', 0, async function (done) {
        console.info('testvisibleNone START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.visibleNone);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('visibleNone')
        expect(obj.$attrs.visible).assertEqual(undefined)
        console.info("[badgeProps] get visible value is: " + JSON.stringify(obj.$attrs.visible));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testmaxcount
      * @tc.desc      ACE
      */
    it('testmaxcount', 0, async function (done) {
        console.info('testmaxcount START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.maxcount);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('maxcount')
        expect(obj.$attrs.maxcount).assertEqual('100')
        console.info("[badgeProps] get maxcount value is: " + JSON.stringify(obj.$attrs.maxcount));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testmaxcountNone
      * @tc.desc      ACE
      */
    it('testmaxcountNone', 0, async function (done) {
        console.info('testmaxcountNone START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.maxcountNone);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('maxcountNone')
        expect(obj.$attrs.maxcount).assertEqual(undefined)
        console.info("[badgeProps] get maxcount value is: " + JSON.stringify(obj.$attrs.maxcount));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testconfig
      * @tc.desc      ACE
      */
    it('testconfig', 0, async function (done) {
        console.info('testconfig START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.config);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('config')
        expect(obj.$attrs.config).assertEqual('{{badgeconfig}}')
        console.info("[badgeProps] get config value is: " + JSON.stringify(obj.$attrs.config));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testconfigNone
      * @tc.desc      ACE
      */
    it('testconfigNone', 0, async function (done) {
        console.info('testconfigNone START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.configNone);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('configNone')
        expect(obj.$attrs.config).assertEqual(undefined)
        console.info("[badgeProps] get config value is: " + JSON.stringify(obj.$attrs.config));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlabel
      * @tc.desc      ACE
      */
    it('testlabel', 0, async function (done) {
        console.info('testlabel START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.label);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('label')
        expect(obj.$attrs.label).assertEqual('提示')
        console.info("[badgeProps] get label value is: " + JSON.stringify(obj.$attrs.label));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlabelNone
      * @tc.desc      ACE
      */
    it('testlabelNone', 0, async function (done) {
        console.info('testlabelNone START');
        console.info("[badgeProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.labelNone);
        console.info("[badgeProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[badgeProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('badge')
        expect(obj.$attrs.id).assertEqual('labelNone')
        expect(obj.$attrs.label).assertEqual(undefined)
        console.info("[badgeProps] get label value is: " + JSON.stringify(obj.$attrs.label));
        done();
    });
});
