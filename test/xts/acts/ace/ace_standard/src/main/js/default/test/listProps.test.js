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
 
 
 describe('listPropsJsTest', function () {
 
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
         console.info('[listPropsJsTest] before each called')
 
         let result;
         let options = {
             uri: 'pages/list/prop/index'
         }
         try {
             result = router.push(options)
             console.info("push listProps page success " + JSON.stringify(result));
         } catch (err) {
             console.error("push listProps page error " + JSON.stringify(result));
         }
         await sleep(20000)
         done()
     })
 
     /**
     * run after testcase
     */
     afterAll(async function () {
         console.info('[listPropsJsTest] after each called')
         await backToIndex();
         await sleep(1000)
     })
 
     // 通用属性
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistIdProp
      * @tc.desc      ACE
      */
     it('testlistIdProp', 0, async function (done) {
         console.info('testlistIdProp START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.idProp);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('idProp')
         done();
     })
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistClassProp
      * @tc.desc      ACE
      */
     it('testlistClassProp', 0, async function (done) {
         console.info('testlistClassProp START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.classProp);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('classProp')
         expect(obj.$attrs.className).assertEqual('classProp')
         done();
     })
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistClassPropNone
      * @tc.desc      ACE
      */
     it('testlistClassPropNone', 0, async function (done) {
         console.info('testlistClassPropNone START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.classPropNone);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('classPropNone')
         expect(obj.$attrs.className).assertEqual(undefined)
         console.info("[listProps] get className value is: " + JSON.stringify(obj.$attrs.className));
         done();
     })
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistRefProp
      * @tc.desc      ACE
      */
     it('testlistRefProp', 0, async function (done) {
         console.info('testlistRefProp START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.refProp);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('refProp')
         expect(obj.$attrs.ref).assertEqual('refProp')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistRefPropNone
      * @tc.desc      ACE
      */
     it('testlistRefPropNone', 0, async function (done) {
         console.info('testlistRefPropNone START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.refPropNone);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('refPropNone')
         expect(obj.$attrs.ref).assertEqual(undefined)
         console.info("[listProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistDisabledPropTrue
      * @tc.desc      ACE
      */
     it('testlistDisabledPropTrue', 0, async function (done) {
         console.info('testlistDisabledPropTrue START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.disabledPropTrue);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('disabledPropTrue')
         expect(obj.$attrs.disabled).assertEqual('true')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistDisabledPropFalse
      * @tc.desc      ACE
      */
     it('testlistDisabledPropFalse', 0, async function (done) {
         console.info('testlistDisabledPropFalse START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.disabledPropFalse);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('disabledPropFalse')
         expect(obj.$attrs.disabled).assertEqual('false')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistDisabledPropNone
      * @tc.desc      ACE
      */
     it('testlistDisabledPropNone', 0, async function (done) {
         console.info('testlistDisabledPropNone START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.disabledPropNone);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('disabledPropNone')
         expect(obj.$attrs.disabled).assertEqual('false')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistFocusablePropTrue
      * @tc.desc      ACE
      */
     it('testlistFocusablePropTrue', 0, async function (done) {
         console.info('testlistFocusablePropTrue START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.focusablePropTrue);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('focusablePropTrue')
         expect(obj.$attrs.focusable).assertEqual('true')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistFocusablePropFalse
      * @tc.desc      ACE
      */
     it('testlistFocusablePropFalse', 0, async function (done) {
         console.info('testlistFocusablePropFalse START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.focusablePropFalse);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('focusablePropFalse')
         expect(obj.$attrs.focusable).assertEqual('false')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistFocusablePropNone
      * @tc.desc      ACE
      */
     it('testlistFocusablePropNone', 0, async function (done) {
         console.info('testlistFocusablePropNone START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.focusablePropNone);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('focusablePropNone')
         expect(obj.$attrs.focusable).assertEqual('false')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistDataProp
      * @tc.desc      ACE
      */
     it('testlistDataProp', 0, async function (done) {
         console.info('testlistDataProp START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.dataProp);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('dataProp')
         expect(obj.$attrs.datalist).assertEqual(undefined);
         console.info("[listProps] get datalist value is: " + JSON.stringify(obj.$attrs.datalist));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistDataPropNone
      * @tc.desc      ACE
      */
     it('testlistDataPropNone', 0, async function (done) {
         console.info('testlistDataPropNone START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.dataPropNone);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('dataPropNone')
         expect(obj.$attrs.datalist).assertEqual(undefined)
         console.info("[listProps] get datalist value is: " + JSON.stringify(obj.$attrs.datalist));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistClickEffectPropSmall
      * @tc.desc      ACE
      */
     it('testlistClickEffectPropSmall', 0, async function (done) {
         console.info('testlistClickEffectPropSmall START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
         expect(obj.$attrs.clickEffect).assertEqual('spring-small')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistClickEffectPropMedium
      * @tc.desc      ACE
      */
     it('testlistClickEffectPropMedium', 0, async function (done) {
         console.info('testlistClickEffectPropMedium START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
         expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistClickEffectPropLarge
      * @tc.desc      ACE
      */
     it('testlistClickEffectPropLarge', 0, async function (done) {
         console.info('testlistClickEffectPropLarge START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
         expect(obj.$attrs.clickEffect).assertEqual('spring-large')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistClickEffectPropNone
      * @tc.desc      ACE
      */
     it('testlistClickEffectPropNone', 0, async function (done) {
         console.info('testlistClickEffectPropNone START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.clickEffectPropNone);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
         expect(obj.$attrs.clickEffect).assertEqual(undefined)
         console.info("[listProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistDirPropRtl
      * @tc.desc      ACE
      */
     it('testlistDirPropRtl', 0, async function (done) {
         console.info('testlistDirPropRtl START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.dirPropRtl);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('dirPropRtl')
         expect(obj.$attrs.dir).assertEqual('rtl')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistDirPropLtr
      * @tc.desc      ACE
      */
     it('testlistDirPropLtr', 0, async function (done) {
         console.info('testlistDirPropLtr START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.dirPropLtr);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('dirPropLtr')
         expect(obj.$attrs.dir).assertEqual('ltr')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistDirPropAuto
      * @tc.desc      ACE
      */
     it('testlistDirPropAuto', 0, async function (done) {
         console.info('testlistDirPropAuto START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.dirPropAuto);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('dirPropAuto')
         expect(obj.$attrs.dir).assertEqual('auto')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistDirPropNone
      * @tc.desc      ACE
      */
     it('testlistDirPropNone', 0, async function (done) {
         console.info('testlistDirPropNone START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.dirPropNone);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('dirPropNone')
         expect(obj.$attrs.dir).assertEqual('auto')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistForPropNull
      * @tc.desc      ACE
      */
     it('testlistForPropNull', 0, async function (done) {
         console.info('testlistForPropNull START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.forPropNull);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('forPropNull')
         expect(obj.$attrs.for).assertEqual(undefined)
         console.info("[listProps] get for value is: " + JSON.stringify(obj.$attrs.for));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistForPropOne
      * @tc.desc      ACE
      */
     it('testlistForPropOne', 0, async function (done) {
         console.info('testlistForPropOne START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.forPropOne);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('forPropOne')
         expect(obj.$attrs.for).assertEqual(undefined)
         console.info("[listProps] get for value is: " + JSON.stringify(obj.$attrs.for));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistForPropThree
      * @tc.desc      ACE
      */
     it('testlistForPropThree', 0, async function (done) {
         console.info('testlistForPropThree START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.forPropThree);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('forPropThree')
         expect(obj.$attrs.for).assertEqual(undefined)
         console.info("[listProps] get for value is: " + JSON.stringify(obj.$attrs.for));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistIfPropTrue
      * @tc.desc      ACE
      */
     it('testlistIfPropTrue', 0, async function (done) {
         console.info('testlistIfPropTrue START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.ifPropTrue);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('ifPropTrue')
         expect(obj.$attrs.if).assertEqual(undefined)
         console.info("[listProps] get for value is: " + JSON.stringify(obj.$attrs.if));
         done();
     });
     
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistShowPropTrue
      * @tc.desc      ACE
      */
     it('testlistShowPropTrue', 0, async function (done) {
         console.info('testlistShowPropTrue START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.showPropTrue);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('showPropTrue')
         expect(obj.$attrs.show).assertEqual('true')
         console.info("[listProps] get show value is: " + JSON.stringify(obj.$attrs.show));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistShowPropFalse
      * @tc.desc      ACE
      */
     it('testlistShowPropFalse', 0, async function (done) {
         console.info('testlistShowPropFalse START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.showPropFalse);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('showPropFalse')
         expect(obj.$attrs.show).assertEqual('false')
         console.info("[listProps] get show value is: " + JSON.stringify(obj.$attrs.show));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistShowPropNone
      * @tc.desc      ACE
      */
     it('testlistShowPropNone', 0, async function (done) {
         console.info('testlistShowPropNone START');
         console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.showPropNone);
         console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('list')
         expect(obj.$attrs.id).assertEqual('showPropNone')
         expect(obj.$attrs.show).assertEqual('true')
         console.info("[listProps] get show value is: " + JSON.stringify(obj.$attrs.show));
         done();
     });
 
     // 特有属性
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistscrollPageTrue
      * @tc.desc      ACE
      */
    it('testlistscrollPageTrue', 0, async function (done) {
        console.info('testlistscrollPageTrue START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.scrollPageTrue);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('scrollPageTrue')
        expect(obj.$attrs.scrollpage).assertEqual('true')
        console.info("[listProps] get scrollpage value is: " + JSON.stringify(obj.$attrs.scrollpage));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistscrollPageFalse
      * @tc.desc      ACE
      */
     it('testlistscrollPageFalse', 0, async function (done) {
        console.info('testlistscrollPageFalse START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.scrollPageFalse);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('scrollPageFalse')
        expect(obj.$attrs.scrollpage).assertEqual('false')
        console.info("[listProps] get scrollpage value is: " + JSON.stringify(obj.$attrs.scrollpage));
        done();
    });

     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistscrollPageNone
      * @tc.desc      ACE
      */
      it('testlistscrollPageNone', 0, async function (done) {
        console.info('testlistscrollPageNone START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.scrollPageNone);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('scrollPageNone')
        expect(obj.$attrs.scrollpage).assertEqual('false')
        console.info("[listProps] get scrollpage value is: " + JSON.stringify(obj.$attrs.scrollpage));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistcachedcount
      * @tc.desc      ACE
      */
     it('testlistcachedcount', 0, async function (done) {
        console.info('testlistcachedcount START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.cachedcount);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('cachedcount')
        expect(obj.$attrs.cachedcount).assertEqual('1')
        console.info("[listProps] get cachedcount value is: " + JSON.stringify(obj.$attrs.cachedcount));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistscrollbarOff
      * @tc.desc      ACE
      */
     it('testlistscrollbarOff', 0, async function (done) {
        console.info('testlistscrollbarOff START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.scrollbarOff);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('scrollbarOff')
        expect(obj.$attrs.scrollbar).assertEqual('off')
        console.info("[listProps] get scrollbar value is: " + JSON.stringify(obj.$attrs.scrollbar));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistscrollbarAuto
      * @tc.desc      ACE
      */
     it('testlistscrollbarAuto', 0, async function (done) {
        console.info('testlistscrollbarAuto START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.scrollbarAuto);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('scrollbarAuto')
        expect(obj.$attrs.scrollbar).assertEqual('auto')
        console.info("[listProps] get scrollbar value is: " + JSON.stringify(obj.$attrs.scrollbar));
        done();
    });

     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistscrollbarOn
      * @tc.desc      ACE
      */
      it('testlistscrollbarOn', 0, async function (done) {
        console.info('testlistscrollbarOn START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.scrollbarOn);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('scrollbarOn')
        expect(obj.$attrs.scrollbar).assertEqual('on')
        console.info("[listProps] get scrollbar value is: " + JSON.stringify(obj.$attrs.scrollbar));
        done();
    });

    
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistscrollbarNone
      * @tc.desc      ACE
      */
      it('testlistscrollbarNone', 0, async function (done) {
        console.info('testlistscrollbarNone START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.scrollbarNone);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('scrollbarNone')
        expect(obj.$attrs.scrollbar).assertEqual('off')
        console.info("[listProps] get scrollbar value is: " + JSON.stringify(obj.$attrs.scrollbar));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistscrolleffectSpring
      * @tc.desc      ACE
      */
     it('testlistscrolleffectSpring', 0, async function (done) {
        console.info('testlistscrolleffectSpring START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.scrolleffectSpring);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('scrolleffectSpring')
        expect(obj.$attrs.scrolleffect).assertEqual('spring')
        console.info("[listProps] get scrolleffect value is: " + JSON.stringify(obj.$attrs.scrolleffect));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistscrolleffectFade
      * @tc.desc      ACE
      */
     it('testlistscrolleffectFade', 0, async function (done) {
        console.info('testlistscrolleffectFade START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.scrolleffectFade);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('scrolleffectFade')
        expect(obj.$attrs.scrolleffect).assertEqual('fade')
        console.info("[listProps] get scrolleffect value is: " + JSON.stringify(obj.$attrs.scrolleffect));
        done();
    });

     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistscrolleffectNo
      * @tc.desc      ACE
      */
      it('testlistscrolleffectNo', 0, async function (done) {
        console.info('testlistscrolleffectNo START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.scrolleffectNo);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('scrolleffectNo')
        expect(obj.$attrs.scrolleffect).assertEqual('no')
        console.info("[listProps] get scrolleffect value is: " + JSON.stringify(obj.$attrs.scrolleffect));
        done();
    });

     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistscrolleffectNone
      * @tc.desc      ACE
      */
      it('testlistscrolleffectNone', 0, async function (done) {
        console.info('testlistscrolleffectNone START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.scrolleffectNone);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('scrolleffectNone')
        expect(obj.$attrs.scrolleffect).assertEqual('spring')
        console.info("[listProps] get scrolleffect value is: " + JSON.stringify(obj.$attrs.scrolleffect));
        done();
    });

     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistindexerTrue
      * @tc.desc      ACE
      */
      it('testlistindexerTrue', 0, async function (done) {
        console.info('testlistindexerTrue START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indexerTrue);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('indexerTrue')
        expect(obj.$attrs.indexer).assertEqual('true')
        console.info("[listProps] get indexer value is: " + JSON.stringify(obj.$attrs.indexer));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistindexerFalse
      * @tc.desc      ACE
      */
     it('testlistindexerFalse', 0, async function (done) {
        console.info('testlistindexerFalse START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indexerFalse);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('indexerFalse')
        expect(obj.$attrs.indexer).assertEqual('false')
        console.info("[listProps] get indexer value is: " + JSON.stringify(obj.$attrs.indexer));
        done();
    });

     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistindexerArray
      * @tc.desc      ACE
      */
      it('testlistindexerArray', 0, async function (done) {
        console.info('testlistindexerArray START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indexerArray);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('indexerArray')
        expect(obj.$attrs.indexer).assertEqual('#\n1\n2\n3\n4\n5\n6\n7\n8')
        console.info("[listProps] get indexer value is: " + JSON.stringify(obj.$attrs.indexer));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistindexerNone
      * @tc.desc      ACE
      */
     it('testlistindexerNone', 0, async function (done) {
        console.info('testlistindexerNone START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indexerNone);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('indexerNone')
        expect(obj.$attrs.indexer).assertEqual('false')
        console.info("[listProps] get indexer value is: " + JSON.stringify(obj.$attrs.indexer));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistindexercircleTrue
      * @tc.desc      ACE
      */
     it('testlistindexercircleTrue', 0, async function (done) {
        console.info('testlistindexercircleTrue START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indexercircleTrue);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('indexercircleTrue')
        expect(obj.$attrs.indexercircle).assertEqual('true')
        console.info("[listProps] get indexercircle value is: " + JSON.stringify(obj.$attrs.indexercircle));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistindexercircleFalse
      * @tc.desc      ACE
      */
     it('testlistindexercircleFalse', 0, async function (done) {
        console.info('testlistindexercircleFalse START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indexercircleFalse);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('indexercircleFalse')
        expect(obj.$attrs.indexercircle).assertEqual('false')
        console.info("[listProps] get indexercircle value is: " + JSON.stringify(obj.$attrs.indexercircle));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistindexercircleNone
      * @tc.desc      ACE
      */
     it('testlistindexercircleNone', 0, async function (done) {
        console.info('testlistindexercircleNone START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indexercircleNone);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('indexercircleNone')
        expect(obj.$attrs.indexercircle).assertEqual('false')
        console.info("[listProps] get indexercircle value is: " + JSON.stringify(obj.$attrs.indexercircle));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistindexermultiTrue
      * @tc.desc      ACE
      */
     it('testlistindexermultiTrue', 0, async function (done) {
        console.info('testlistindexermultiTrue START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indexermultiTrue);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('indexermultiTrue')
        expect(obj.$attrs.indexermulti).assertEqual('true')
        console.info("[listProps] get indexermulti value is: " + JSON.stringify(obj.$attrs.indexermulti));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistindexermultiFalse
      * @tc.desc      ACE
      */
     it('testlistindexermultiFalse', 0, async function (done) {
        console.info('testlistindexermultiFalse START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indexermultiFalse);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('indexermultiFalse')
        expect(obj.$attrs.indexermulti).assertEqual('false')
        console.info("[listProps] get indexermulti value is: " + JSON.stringify(obj.$attrs.indexermulti));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistindexermultiNone
      * @tc.desc      ACE
      */
     it('testlistindexermultiNone', 0, async function (done) {
        console.info('testlistindexermultiNone START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indexermultiNone);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('indexermultiNone')
        expect(obj.$attrs.indexermulti).assertEqual('false')
        console.info("[listProps] get indexermulti value is: " + JSON.stringify(obj.$attrs.indexermulti));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistindexerbubbleTrue
      * @tc.desc      ACE
      */
     it('testlistindexerbubbleTrue', 0, async function (done) {
        console.info('testlistindexerbubbleTrue START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indexerbubbleTrue);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('indexerbubbleTrue')
        expect(obj.$attrs.indexerbubble).assertEqual('true')
        console.info("[listProps] get indexerbubble value is: " + JSON.stringify(obj.$attrs.indexerbubble));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistindexerbubbleFalse
      * @tc.desc      ACE
      */
     it('testlistindexerbubbleFalse', 0, async function (done) {
        console.info('testlistindexerbubbleFalse START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indexerbubbleFalse);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('indexerbubbleFalse')
        expect(obj.$attrs.indexerbubble).assertEqual('false')
        console.info("[listProps] get indexerbubble value is: " + JSON.stringify(obj.$attrs.indexerbubble));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistindexerbubbleNone
      * @tc.desc      ACE
      */
     it('testlistindexerbubbleNone', 0, async function (done) {
        console.info('testlistindexerbubbleNone START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.indexerbubbleNone);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('indexerbubbleNone')
        expect(obj.$attrs.indexerbubble).assertEqual('true')
        console.info("[listProps] get indexerbubble value is: " + JSON.stringify(obj.$attrs.indexerbubble));
        done();
    });

     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistdividerTrue
      * @tc.desc      ACE
      */
      it('testlistdividerTrue', 0, async function (done) {
        console.info('testlistdividerTrue START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dividerTrue);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('dividerTrue')
        expect(obj.$attrs.divider).assertEqual('true')
        console.info("[listProps] get divider value is: " + JSON.stringify(obj.$attrs.divider));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistdividerFalse
      * @tc.desc      ACE
      */
     it('testlistdividerFalse', 0, async function (done) {
        console.info('testlistdividerFalse START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dividerFalse);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('dividerFalse')
        expect(obj.$attrs.divider).assertEqual('false')
        console.info("[listProps] get divider value is: " + JSON.stringify(obj.$attrs.divider));
        done();
    });

     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistdividerNone
      * @tc.desc      ACE
      */
      it('testlistdividerNone', 0, async function (done) {
        console.info('testlistdividerNone START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dividerNone);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('dividerNone')
        expect(obj.$attrs.divider).assertEqual('false')
        console.info("[listProps] get divider value is: " + JSON.stringify(obj.$attrs.divider));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistshapemodeDefault
      * @tc.desc      ACE
      */
     it('testlistshapemodeDefault', 0, async function (done) {
        console.info('testlistshapemodeDefault START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.shapemodeDefault);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('shapemodeDefault')
        expect(obj.$attrs.shapemode).assertEqual('default')
        console.info("[listProps] get shapemode value is: " + JSON.stringify(obj.$attrs.shapemode));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistshapemodeRect
      * @tc.desc      ACE
      */
     it('testlistshapemodeRect', 0, async function (done) {
        console.info('testlistshapemodeRect START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.shapemodeRect);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('shapemodeRect')
        expect(obj.$attrs.shapemode).assertEqual('rect')
        console.info("[listProps] get shapemode value is: " + JSON.stringify(obj.$attrs.shapemode));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistshapemodeRound
      * @tc.desc      ACE
      */
     it('testlistshapemodeRound', 0, async function (done) {
        console.info('testlistshapemodeRound START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.shapemodeRound);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('shapemodeRound')
        expect(obj.$attrs.shapemode).assertEqual('round')
        console.info("[listProps] get shapemode value is: " + JSON.stringify(obj.$attrs.shapemode));
        done();
    });

     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistshapemodeNone
      * @tc.desc      ACE
      */
      it('testlistshapemodeNone', 0, async function (done) {
        console.info('testlistshapemodeNone START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.shapemodeNone);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('shapemodeNone')
        expect(obj.$attrs.shapemode).assertEqual('default')
        console.info("[listProps] get shapemode value is: " + JSON.stringify(obj.$attrs.shapemode));
        done();
    });

     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistchainanimationTrue
      * @tc.desc      ACE
      */
      it('testlistchainanimationTrue', 0, async function (done) {
        console.info('testlistchainanimationTrue START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.chainanimationTrue);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('chainanimationTrue')
        expect(obj.$attrs.chainanimation).assertEqual('true')
        console.info("[listProps] get chainanimation value is: " + JSON.stringify(obj.$attrs.chainanimation));
        done();
    });

     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistchainanimationFalse
      * @tc.desc      ACE
      */
      it('testlistchainanimationFalse', 0, async function (done) {
        console.info('testlistchainanimationFalse START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.chainanimationFalse);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('chainanimationFalse')
        expect(obj.$attrs.chainanimation).assertEqual('false')
        console.info("[listProps] get chainanimation value is: " + JSON.stringify(obj.$attrs.chainanimation));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistchainanimationNone
      * @tc.desc      ACE
      */
     it('testlistchainanimationNone', 0, async function (done) {
        console.info('testlistchainanimationNone START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.chainanimationNone);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('chainanimationNone')
        expect(obj.$attrs.chainanimation).assertEqual('false')
        console.info("[listProps] get chainanimation value is: " + JSON.stringify(obj.$attrs.chainanimation));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistupdateeffectTrue
      * @tc.desc      ACE
      */
     it('testlistupdateeffectTrue', 0, async function (done) {
        console.info('testlistupdateeffectTrue START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.updateeffectTrue);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('updateeffectTrue')
        expect(obj.$attrs.updateeffect).assertEqual('true')
        console.info("[listProps] get updateeffect value is: " + JSON.stringify(obj.$attrs.updateeffect));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistupdateeffectFalse
      * @tc.desc      ACE
      */
     it('testlistupdateeffectFalse', 0, async function (done) {
        console.info('testlistupdateeffectFalse START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.updateeffectFalse);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('updateeffectFalse')
        expect(obj.$attrs.updateeffect).assertEqual('false')
        console.info("[listProps] get updateeffect value is: " + JSON.stringify(obj.$attrs.updateeffect));
        done();
    });

     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistupdateeffectNone
      * @tc.desc      ACE
      */
      it('testlistupdateeffectNone', 0, async function (done) {
        console.info('testlistupdateeffectNone START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.updateeffectNone);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('updateeffectNone')
        expect(obj.$attrs.updateeffect).assertEqual('false')
        console.info("[listProps] get updateeffect value is: " + JSON.stringify(obj.$attrs.updateeffect));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistinitialindex
      * @tc.desc      ACE
      */
     it('testlistinitialindex', 0, async function (done) {
        console.info('testlistinitialindex START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.initialindex);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('initialindex')
        expect(obj.$attrs.initialindex).assertEqual('0')
        console.info("[listProps] get initialindex value is: " + JSON.stringify(obj.$attrs.initialindex));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistinitialoffset
      * @tc.desc      ACE
      */
     it('testlistinitialoffset', 0, async function (done) {
        console.info('testlistinitialoffset START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.initialoffset);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('initialoffset')
        expect(obj.$attrs.initialoffset).assertEqual('0')
        console.info("[listProps] get initialoffset value is: " + JSON.stringify(obj.$attrs.initialoffset));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistselected
      * @tc.desc      ACE
      */
     it('testlistselected', 0, async function (done) {
        console.info('testlistselected START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.selected);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('selected')
        expect(obj.$attrs.selected).assertEqual('0')
        console.info("[listProps] get selected value is: " + JSON.stringify(obj.$attrs.selected));
        done();
    });

     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testlistselected
      * @tc.desc      ACE
      */
      it('testlistselectedNone', 0, async function (done) {
        console.info('testlistselectedNone START');
        console.info("[listProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.selectedNone);
        console.info("[listProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[listProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('list')
        expect(obj.$attrs.id).assertEqual('selectedNone')
        expect(obj.$attrs.selected).assertEqual(undefined)
        console.info("[listProps] get selected value is: " + JSON.stringify(obj.$attrs.selected));
        done();
    });
 });
 