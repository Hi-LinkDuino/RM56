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


describe('imageAnimatorPropsJsTest', function () {

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
        console.info('[imageAnimatorPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/imageAnimator/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push imageAnimatorProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push imageAnimatorProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[imageAnimatorPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorIdProp
     * @tc.desc      ACE
     */
    it('testimageAnimatorIdProp', 0, async function (done) {
        console.info('testimageAnimatorIdProp START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorClassProp
     * @tc.desc      ACE
     */
    it('testimageAnimatorClassProp', 0, async function (done) {
        console.info('testimageAnimatorClassProp START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorClassPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorClassPropNone', 0, async function (done) {
        console.info('testimageAnimatorClassPropNone START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[imageAnimatorProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorRefProp
     * @tc.desc      ACE
     */
    it('testimageAnimatorRefProp', 0, async function (done) {
        console.info('testimageAnimatorRefProp START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorRefPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorRefPropNone', 0, async function (done) {
        console.info('testimageAnimatorRefPropNone START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[imageAnimatorProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testimageAnimatorDisabledPropTrue', 0, async function (done) {
        console.info('testimageAnimatorDisabledPropTrue START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testimageAnimatorDisabledPropFalse', 0, async function (done) {
        console.info('testimageAnimatorDisabledPropFalse START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorDisabledPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorDisabledPropNone', 0, async function (done) {
        console.info('testimageAnimatorDisabledPropNone START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testimageAnimatorFocusablePropTrue', 0, async function (done) {
        console.info('testimageAnimatorFocusablePropTrue START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testimageAnimatorFocusablePropFalse', 0, async function (done) {
        console.info('testimageAnimatorFocusablePropFalse START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorFocusablePropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorFocusablePropNone', 0, async function (done) {
        console.info('testimageAnimatorFocusablePropNone START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorDataProp
     * @tc.desc      ACE
     */
    it('testimageAnimatorDataProp', 0, async function (done) {
        console.info('testimageAnimatorDataProp START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.data).assertEqual(undefined);
        console.info("[imageAnimatorProps] get data value is: " + JSON.stringify(obj.$attrs.data));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorDataPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorDataPropNone', 0, async function (done) {
        console.info('testimageAnimatorDataPropNone START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.data).assertEqual(undefined)
        console.info("[imageAnimatorProps] get data value is: " + JSON.stringify(obj.$attrs.data));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testimageAnimatorClickEffectPropSmall', 0, async function (done) {
        console.info('testimageAnimatorClickEffectPropSmall START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testimageAnimatorClickEffectPropMedium', 0, async function (done) {
        console.info('testimageAnimatorClickEffectPropMedium START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testimageAnimatorClickEffectPropLarge', 0, async function (done) {
        console.info('testimageAnimatorClickEffectPropLarge START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorClickEffectPropNone', 0, async function (done) {
        console.info('testimageAnimatorClickEffectPropNone START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[imageAnimatorProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorDirPropRtl
     * @tc.desc      ACE
     */
    it('testimageAnimatorDirPropRtl', 0, async function (done) {
        console.info('testimageAnimatorDirPropRtl START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorDirPropLtr
     * @tc.desc      ACE
     */
    it('testimageAnimatorDirPropLtr', 0, async function (done) {
        console.info('testimageAnimatorDirPropLtr START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorDirPropAuto
     * @tc.desc      ACE
     */
    it('testimageAnimatorDirPropAuto', 0, async function (done) {
        console.info('testimageAnimatorDirPropAuto START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorDirPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorDirPropNone', 0, async function (done) {
        console.info('testimageAnimatorDirPropNone START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorForPropNull
     * @tc.desc      ACE
     */
    it('testimageAnimatorForPropNull', 0, async function (done) {
        console.info('testimageAnimatorForPropNull START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[imageAnimatorProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorForPropOne
     * @tc.desc      ACE
     */
    it('testimageAnimatorForPropOne', 0, async function (done) {
        console.info('testimageAnimatorForPropOne START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[imageAnimatorProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorForPropThree
     * @tc.desc      ACE
     */
    it('testimageAnimatorForPropThree', 0, async function (done) {
        console.info('testimageAnimatorForPropThree START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[imageAnimatorProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorIfPropTrue
     * @tc.desc      ACE
     */
    it('testimageAnimatorIfPropTrue', 0, async function (done) {
        console.info('testimageAnimatorIfPropTrue START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[imageAnimatorProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropTrue
     * @tc.desc      ACE
     */
    it('testimageAnimatorShowPropTrue', 0, async function (done) {
        console.info('testimageAnimatorShowPropTrue START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[imageAnimatorProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropFalse
     * @tc.desc      ACE
     */
    it('testimageAnimatorShowPropFalse', 0, async function (done) {
        console.info('testimageAnimatorShowPropFalse START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[imageAnimatorProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorShowPropNone', 0, async function (done) {
        console.info('testimageAnimatorShowPropNone START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual("false")
        console.info("[imageAnimatorProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorImagePropNone', 0, async function (done) {
        console.info('testimageAnimatorXsPropNone START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeImageNull);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('typeImageNull')
        expect(obj.$attrs.images).assertEqual(undefined)
        console.info("[imageAnimatorProps] get image value is: " + JSON.stringify(obj.$attrs.image));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorImageProp', 0, async function (done) {
        console.info('testimageAnimatorXsPropNone START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeImage);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('typeImage')
        expect(obj.$attrs.images).assertEqual(undefined)
        console.info("[imageAnimatorProps] get image value is: " + JSON.stringify(obj.$attrs.image));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorPredecodeProp2', 0, async function (done) {
        console.info('testimageAnimatorXsProp1 START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typePredecode2);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('typePredecode2')
        expect(obj.$attrs.predecode).assertEqual('2')
        console.info("[imageAnimatorProps] get predecode value is: " + JSON.stringify(obj.$attrs.predecode));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorIterationPropInfi', 0, async function (done) {
        console.info('testimageAnimatorXsPropObject START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeIterationInfi);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('typeIterationInfi')
        expect(obj.$attrs.iteration).assertEqual("infinite")
        console.info("[imageAnimatorProps] get iteration value is: " + JSON.stringify(obj.$attrs.iteration));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorIterationProp3', 0, async function (done) {
        console.info('testimageAnimatorXsPropObject START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeIteration3);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('typeIteration3')
        expect(obj.$attrs.iteration).assertEqual("3")
        console.info("[imageAnimatorProps] get iteration value is: " + JSON.stringify(obj.$attrs.iteration));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorReversePropNull', 0, async function (done) {
        console.info('testimageAnimatorXsProp1 START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeReverseNull);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('typeReverseNull')
        expect(obj.$attrs.reverse).assertEqual("false")
        console.info("[imageAnimatorProps] get reverse value is: " + JSON.stringify(obj.$attrs.reverse));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorReversePropFalse', 0, async function (done) {
        console.info('testimageAnimatorXsProp1 START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeReverseFalse);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('typeReverseFalse')
        expect(obj.$attrs.reverse).assertEqual('false')
        console.info("[imageAnimatorProps] get reverse value is: " + JSON.stringify(obj.$attrs.reverse));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorReversePropTrue', 0, async function (done) {
        console.info('testimageAnimatorXsProp1 START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeReverseTrue);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('typeReverseTrue')
        expect(obj.$attrs.reverse).assertEqual('true')
        console.info("[imageAnimatorProps] get reverse value is: " + JSON.stringify(obj.$attrs.reverse));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorFixedsizePropNull', 0, async function (done) {
        console.info('testimageAnimatorXsProp1 START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeFixedsizeNull);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('typeFixedsizeNull')
        expect(obj.$attrs.fixedsize).assertEqual("true")
        console.info("[imageAnimatorProps] get fixedsize value is: " + JSON.stringify(obj.$attrs.fixedsize));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorFixedsizePropFalse', 0, async function (done) {
        console.info('testimageAnimatorXsProp1 START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeFixedsizeFalse);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('typeFixedsizeFalse')
        expect(obj.$attrs.fixedsize).assertEqual('false')
        console.info("[imageAnimatorProps] get fixedsize value is: " + JSON.stringify(obj.$attrs.fixedsize));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorFixedsizePropTrue', 0, async function (done) {
        console.info('testimageAnimatorXsProp1 START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeFixedsizeTrue);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('typeFixedsizeTrue')
        expect(obj.$attrs.fixedsize).assertEqual('true')
        console.info("[imageAnimatorProps] get fixedsize value is: " + JSON.stringify(obj.$attrs.fixedsize));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
    it('testimageAnimatorFillmodePropNull', 0, async function (done) {
        console.info('testimageAnimatorXsPropObject START');
        console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeFillmodeNull);
        console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image-animator')
        expect(obj.$attrs.id).assertEqual('typeFillmodeNull')
        expect(obj.$attrs.fillmode).assertEqual("forwards")
        console.info("[imageAnimatorProps] get fillmode value is: " + JSON.stringify(obj.$attrs.fillmode));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
it('testimageAnimatorFillmodePropForwards', 0, async function (done) {
    console.info('testimageAnimatorXsPropObject START');
    console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

    let obj = JSON.parse(globalThis.value.typeFillmodeForwards);
    console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
    console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

    expect(obj.$type).assertEqual('image-animator')
    expect(obj.$attrs.id).assertEqual('typeFillmodeForwards')
    expect(obj.$attrs.fillmode).assertEqual("forwards")
    console.info("[imageAnimatorProps] get fillmode value is: " + JSON.stringify(obj.$attrs.fillmode));
    done();
});

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
it('testimageAnimatorFillmodePropNone', 0, async function (done) {
    console.info('testimageAnimatorXsPropObject START');
    console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

    let obj = JSON.parse(globalThis.value.typeFillmodeNone);
    console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
    console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

    expect(obj.$type).assertEqual('image-animator')
    expect(obj.$attrs.id).assertEqual('typeFillmodeNone')
    expect(obj.$attrs.fillmode).assertEqual("none")
    console.info("[imageAnimatorProps] get fillmode value is: " + JSON.stringify(obj.$attrs.fillmode));
    done();
});

     /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
it('testimageAnimatorDurationPropNull', 0, async function (done) {
    console.info('testimageAnimatorXsPropObject START');
    console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

    let obj = JSON.parse(globalThis.value.typeDurationNull);
    console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
    console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

    expect(obj.$type).assertEqual('image-animator')
    expect(obj.$attrs.id).assertEqual('typeDurationNull')
    expect(obj.$attrs.duration).assertEqual(undefined)
    console.info("[imageAnimatorProps] get duration value is: " + JSON.stringify(obj.$attrs.duration));
    done();
});

     /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
it('testimageAnimatorDurationProp3', 0, async function (done) {
    console.info('testimageAnimatorXsPropObject START');
    console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

    let obj = JSON.parse(globalThis.value.typeDuration3);
    console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
    console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

    expect(obj.$type).assertEqual('image-animator')
    expect(obj.$attrs.id).assertEqual('typeDuration3')
    expect(obj.$attrs.duration).assertEqual("3s")
    console.info("[imageAnimatorProps] get duration value is: " + JSON.stringify(obj.$attrs.duration));
    done();
});

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testimageAnimatorShowPropNone
     * @tc.desc      ACE
     */
it('testimageAnimatorDurationProp6', 0, async function (done) {
    console.info('testimageAnimatorXsPropObject START');
    console.info("[imageAnimatorProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

    let obj = JSON.parse(globalThis.value.typeDuration6);
    console.info("[imageAnimatorProps] get inspector value is: " + JSON.stringify(obj));
    console.info("[imageAnimatorProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

    expect(obj.$type).assertEqual('image-animator')
    expect(obj.$attrs.id).assertEqual('typeDuration6')
    expect(obj.$attrs.duration).assertEqual("6s")
    console.info("[imageAnimatorProps] get duration value is: " + JSON.stringify(obj.$attrs.duration));
    done();
});
});
