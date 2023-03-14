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


describe('dividerPropsJsTest', function () {

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
        console.info('[dividerPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/divider/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push dividerProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push dividerProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    })

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[dividerPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    })

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerIdProp
     * @tc.desc      ACE
     */
     it('testDividerIdProp', 0, async function (done) {
        console.info('testDividerIdProp START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerClassProp
     * @tc.desc      ACE
     */
    it('testDividerClassProp', 0, async function (done) {
        console.info('testDividerClassProp START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerClassPropNone
     * @tc.desc      ACE
     */
    it('testDividerClassPropNone', 0, async function (done) {
        console.info('testDividerClassPropNone START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[dividerProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerStyleProp
     * @tc.desc      ACEs
     */
//    it('testDividerStyleProp', 0, async function (done) {
//        console.info('testDividerStyleProp START');
//        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
//
//        let obj = JSON.parse(globalThis.value.styleProp);
//        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
//        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
//
//        expect(obj.$type).assertEqual('divider')
//        expect(obj.$attrs.id).assertEqual('styleProp')
//        expect(obj.$attrs.style).assertEqual(undefined)
//        console.info("[dividerProps] get style value is: " + JSON.stringify(obj.$attrs.style));
//
//        expect(obj.$styles.width).assertEqual(undefined);
//        console.info("[dividerProps] get style width value is: " + JSON.stringify(obj.$styles.width));
//        expect(obj.$styles.height).assertEqual(undefined);
//        console.info("[dividerProps] get style height value is: " + JSON.stringify(obj.$styles.height));
//        expect(obj.$styles.get('background-color')).assertEqual(undefined);
//        done();
//    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerRefProp
     * @tc.desc      ACE
     */
    it('testDividerRefProp', 0, async function (done) {
        console.info('testDividerRefProp START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerRefPropNone
     * @tc.desc      ACE
     */
    it('testDividerRefPropNone', 0, async function (done) {
        console.info('testDividerRefPropNone START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[dividerProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerDataProp
     * @tc.desc      ACE
     */
    it('testDividerDataProp', 0, async function (done) {
        console.info('testDividerDataProp START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataDiv).assertEqual(undefined);
        console.info("[dividerProps] get dataDiv value is: " + JSON.stringify(obj.$attrs.dataDiv));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerDataPropNone
     * @tc.desc      ACE
     */
    it('testDividerDataPropNone', 0, async function (done) {
        console.info('testDividerDataPropNone START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataDiv).assertEqual(undefined)
        console.info("[dividerProps] get dataDiv value is: " + JSON.stringify(obj.$attrs.dataDiv));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testDividerClickEffectPropSmall', 0, async function (done) {
        console.info('testDividerClickEffectPropSmall START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testDividerClickEffectPropMedium', 0, async function (done) {
        console.info('testDividerClickEffectPropMedium START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testDividerClickEffectPropLarge', 0, async function (done) {
        console.info('testDividerClickEffectPropLarge START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testDividerClickEffectPropNone', 0, async function (done) {
        console.info('testDividerClickEffectPropNone START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[dividerProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerDirPropRtl
     * @tc.desc      ACE
     */
    it('testDividerDirPropRtl', 0, async function (done) {
        console.info('testDividerDirPropRtl START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerDirPropLtr
     * @tc.desc      ACE
     */
    it('testDividerDirPropLtr', 0, async function (done) {
        console.info('testDividerDirPropLtr START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerDirPropAuto
     * @tc.desc      ACE
     */
    it('testDividerDirPropAuto', 0, async function (done) {
        console.info('testDividerDirPropAuto START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerDirPropNone
     * @tc.desc      ACE
     */
    it('testDividerDirPropNone', 0, async function (done) {
        console.info('testDividerDirPropNone START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerForPropNull
     * @tc.desc      ACE
     */
    it('testDividerForPropNull', 0, async function (done) {
        console.info('testDividerForPropNull START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[dividerProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerForPropOne
     * @tc.desc      ACE
     */
    it('testDividerForPropOne', 0, async function (done) {
        console.info('testDividerForPropOne START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[dividerProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerForPropThree
     * @tc.desc      ACE
     */
    it('testDividerForPropThree', 0, async function (done) {
        console.info('testDividerForPropThree START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[dividerProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerIfPropTrue
     * @tc.desc      ACE
     */
    it('testDividerIfPropTrue', 0, async function (done) {
        console.info('testDividerIfPropTrue START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[dividerProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testDividerIfPropFalse', 0, async function (done) {
    //        console.info('testDividerIfPropFalse START');
    //        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('divider')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerIfPropNone
     * @tc.desc      ACE
     */
    //    it('testDividerIfPropNone', 0, async function (done) {
    //        console.info('testDividerIfPropNone START');
    //        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('divider')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerShowPropTrue
     * @tc.desc      ACE
     */
    it('testDividerShowPropTrue', 0, async function (done) {
        console.info('testDividerShowPropTrue START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[dividerProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerShowPropFalse
     * @tc.desc      ACE
     */
    it('testDividerShowPropFalse', 0, async function (done) {
        console.info('testDividerShowPropFalse START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[dividerProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerShowPropNone
     * @tc.desc      ACE
     */
    it('testDividerShowPropNone', 0, async function (done) {
        console.info('testDividerShowPropNone START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[dividerProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerVerticalTrue
     * @tc.desc      ACE
     */
    it('testDividerVerticalTrue', 0, async function (done) {
        console.info('testDividerVerticalTrue START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.verticalTrue);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('verticalTrue')
        expect(obj.$attrs.vertical).assertEqual('true')
        console.info("[dividerProps] get vertical value is: " + JSON.stringify(obj.$attrs.vertical));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDividerVerticalFalse
     * @tc.desc      ACE
     */
    it('testDividerVerticalFalse', 0, async function (done) {
        console.info('testDividerVerticalFalse START');
        console.info("[dividerProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.verticalFalse);
        console.info("[dividerProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dividerProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('divider')
        expect(obj.$attrs.id).assertEqual('verticalFalse')
        expect(obj.$attrs.vertical).assertEqual('false')
        console.info("[dividerProps] get vertical value is: " + JSON.stringify(obj.$attrs.vertical));
        done();
    });
});
