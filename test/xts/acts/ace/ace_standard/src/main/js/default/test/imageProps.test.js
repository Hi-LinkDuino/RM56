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


describe('imagePropsJsTest', function () {

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
            return clearPromise;
        })
    }

    /**
    * run before testcase
    */
    beforeAll(async function (done) {
        console.info('[imagePropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/image/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push imageProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push imageProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    })

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[imagePropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    })

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageIdProp
     * @tc.desc      ACE
     */
    it('testImageIdProp', 0, async function (done) {
        console.info('testImageIdProp START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    it('testImageClassProp', 0, async function (done) {
        console.info('testImageClassProp START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageClassPropNone
     * @tc.desc      ACE
     */
    it('testImageClassPropNone', 0, async function (done) {
        console.info('testImageClassPropNone START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[imageProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageRefProp
     * @tc.desc      ACE
     */
    it('testImageRefProp', 0, async function (done) {
        console.info('testImageRefProp START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageRefPropNone
     * @tc.desc      ACE
     */
    it('testImageRefPropNone', 0, async function (done) {
        console.info('testImageRefPropNone START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[imageProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testImageDisabledPropTrue', 0, async function (done) {
        console.info('testImageDisabledPropTrue START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testImageDisabledPropFalse', 0, async function (done) {
        console.info('testImageDisabledPropFalse START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageDisabledPropNone
     * @tc.desc      ACE
     */
    it('testImageDisabledPropNone', 0, async function (done) {
        console.info('testImageDisabledPropNone START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageFocusablePropTrue
     * @tc.desc      ACE
     */
    it('testImageFocusablePropTrue', 0, async function (done) {
        console.info('testImageFocusablePropTrue START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageFocusablePropFalse
     * @tc.desc      ACE
     */
    it('testImageFocusablePropFalse', 0, async function (done) {
        console.info('testImageFocusablePropFalse START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageFocusablePropNone
     * @tc.desc      ACE
     */
    it('testImageFocusablePropNone', 0, async function (done) {
        console.info('testImageFocusablePropNone START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageDataProp
     * @tc.desc      ACE
     */
    it('testImageDataProp', 0, async function (done) {
        console.info('testImageDataProp START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataImage).assertEqual(undefined);
        console.info("[imageProps] get dataImage value is: " + JSON.stringify(obj.$attrs.dataImage));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageDataPropNone
     * @tc.desc      ACE
     */
    it('testImageDataPropNone', 0, async function (done) {
        console.info('testImageDataPropNone START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataImage).assertEqual(undefined)
        console.info("[imageProps] get dataImage value is: " + JSON.stringify(obj.$attrs.dataImage));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testImageClickEffectPropSmall', 0, async function (done) {
        console.info('testImageClickEffectPropSmall START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testImageClickEffectPropMedium', 0, async function (done) {
        console.info('testImageClickEffectPropMedium START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testImageClickEffectPropLarge', 0, async function (done) {
        console.info('testImageClickEffectPropLarge START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testImageClickEffectPropNone', 0, async function (done) {
        console.info('testImageClickEffectPropNone START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[imageProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageDirPropRtl
     * @tc.desc      ACE
     */
    it('testImageDirPropRtl', 0, async function (done) {
        console.info('testImageDirPropRtl START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageDirPropLtr
     * @tc.desc      ACE
     */
    it('testImageDirPropLtr', 0, async function (done) {
        console.info('testImageDirPropLtr START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageDirPropAuto
     * @tc.desc      ACE
     */
    it('testImageDirPropAuto', 0, async function (done) {
        console.info('testImageDirPropAuto START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageDirPropNone
     * @tc.desc      ACE
     */
    it('testImageDirPropNone', 0, async function (done) {
        console.info('testImageDirPropNone START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageForPropNull
     * @tc.desc      ACE
     */
    it('testImageForPropNull', 0, async function (done) {
        console.info('testImageForPropNull START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[imageProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageForPropOne
     * @tc.desc      ACE
     */
    it('testImageForPropOne', 0, async function (done) {
        console.info('testImageForPropOne START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[imageProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageForPropThree
     * @tc.desc      ACE
     */
    it('testImageForPropThree', 0, async function (done) {
        console.info('testImageForPropThree START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[imageProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageIfPropTrue
     * @tc.desc      ACE
     */
    it('testImageIfPropTrue', 0, async function (done) {
        console.info('testImageIfPropTrue START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[imageProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageShowPropTrue
     * @tc.desc      ACE
     */
    it('testImageShowPropTrue', 0, async function (done) {
        console.info('testImageShowPropTrue START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[imageProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageShowPropFalse
     * @tc.desc      ACE
     */
    it('testImageShowPropFalse', 0, async function (done) {
        console.info('testImageShowPropFalse START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[imageProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageShowPropNone
     * @tc.desc      ACE
     */
    it('testImageShowPropNone', 0, async function (done) {
        console.info('testImageShowPropNone START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[imageProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    // 特有属性
    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageImageSrc
     * @tc.desc      ACE
     */
    it('testImageImageSrc', 0, async function (done) {
        console.info('testImageImageSrc START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.imageSrc);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('imageSrc')
        expect(obj.$attrs.src).assertEqual('common/images/image.png')
        console.info("[imageProps] get src value is: " + JSON.stringify(obj.$attrs.src));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageImageSrcNone
     * @tc.desc      ACE
     */
    it('testImageImageSrcNone', 0, async function (done) {
        console.info('testImageImageSrcNone START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.imageSrcNone);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('imageSrcNone')
        expect(obj.$attrs.src).assertEqual(undefined)
        console.info("[imageProps] get src value is: " + JSON.stringify(obj.$attrs.src));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageImageAlt
     * @tc.desc      ACE
     */
    it('testImageImageAlt', 0, async function (done) {
        console.info('testImageImageAlt START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.imageAlt);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('imageAlt')
        expect(obj.$attrs.alt).assertEqual('common/images/image.png')
        console.info("[imageProps] get alt value is: " + JSON.stringify(obj.$attrs.alt));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageImageAltNone
     * @tc.desc      ACE
     */
    it('testImageImageAltNone', 0, async function (done) {
        console.info('testImageImageAltNone START');
        console.info("[imageProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.imageAltNone);
        console.info("[imageProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[imageProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('image')
        expect(obj.$attrs.id).assertEqual('imageAltNone')
        expect(obj.$attrs.alt).assertEqual(undefined)
        console.info("[imageProps] get alt value is: " + JSON.stringify(obj.$attrs.alt));
        done();
    });
});
