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
 
 
 describe('dialogPropsJsTest', function () {
 
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
         console.info('[dialogPropsJsTest] before each called')
 
         let result;
         let options = {
             uri: 'pages/dialog/prop/index'
         }
         try {
             result = router.push(options)
             console.info("push dialogProps page success " + JSON.stringify(result));
         } catch (err) {
             console.error("push dialogProps page error " + JSON.stringify(result));
         }
         await sleep(30000)
         done()
     })
 
     /**
     * run after testcase
     */
     afterAll(async function () {
         console.info('[dialogPropsJsTest] after each called')
         await backToIndex();
         await sleep(1000)
     })
 
     // 通用属性
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogIdProp
      * @tc.desc      ACE
      */
     it('testdialogIdProp', 0, async function (done) {
         console.info('testdialogIdProp START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.idProp);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('idProp')
         done();
     })
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogClassProp
      * @tc.desc      ACE
      */
     it('testdialogClassProp', 0, async function (done) {
         console.info('testdialogClassProp START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.classProp);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('classProp')
         expect(obj.$attrs.className).assertEqual('classProp')
         done();
     })
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogClassPropNone
      * @tc.desc      ACE
      */
     it('testdialogClassPropNone', 0, async function (done) {
         console.info('testdialogClassPropNone START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.classPropNone);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('classPropNone')
         expect(obj.$attrs.className).assertEqual(undefined)
         console.info("[dialogProps] get className value is: " + JSON.stringify(obj.$attrs.className));
         done();
     })
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogRefProp
      * @tc.desc      ACE
      */
     it('testdialogRefProp', 0, async function (done) {
         console.info('testdialogRefProp START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.refProp);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('refProp')
         expect(obj.$attrs.ref).assertEqual('refProp')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogRefPropNone
      * @tc.desc      ACE
      */
     it('testdialogRefPropNone', 0, async function (done) {
         console.info('testdialogRefPropNone START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.refPropNone);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('refPropNone')
         expect(obj.$attrs.ref).assertEqual(undefined)
         console.info("[dialogProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogDisabledPropTrue
      * @tc.desc      ACE
      */
     it('testdialogDisabledPropTrue', 0, async function (done) {
         console.info('testdialogDisabledPropTrue START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.disabledPropTrue);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('disabledPropTrue')
         expect(obj.$attrs.disabled).assertEqual('true')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogDisabledPropFalse
      * @tc.desc      ACE
      */
     it('testdialogDisabledPropFalse', 0, async function (done) {
         console.info('testdialogDisabledPropFalse START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.disabledPropFalse);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('disabledPropFalse')
         expect(obj.$attrs.disabled).assertEqual('false')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogDisabledPropNone
      * @tc.desc      ACE
      */
     it('testdialogDisabledPropNone', 0, async function (done) {
         console.info('testdialogDisabledPropNone START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.disabledPropNone);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('disabledPropNone')
         expect(obj.$attrs.disabled).assertEqual('false')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogFocusablePropTrue
      * @tc.desc      ACE
      */
     it('testdialogFocusablePropTrue', 0, async function (done) {
         console.info('testdialogFocusablePropTrue START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.focusablePropTrue);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('focusablePropTrue')
         expect(obj.$attrs.focusable).assertEqual('true')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogFocusablePropFalse
      * @tc.desc      ACE
      */
     it('testdialogFocusablePropFalse', 0, async function (done) {
         console.info('testdialogFocusablePropFalse START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.focusablePropFalse);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('focusablePropFalse')
         expect(obj.$attrs.focusable).assertEqual('false')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogFocusablePropNone
      * @tc.desc      ACE
      */
     it('testdialogFocusablePropNone', 0, async function (done) {
         console.info('testdialogFocusablePropNone START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.focusablePropNone);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('focusablePropNone')
         expect(obj.$attrs.focusable).assertEqual(undefined)
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogDataProp
      * @tc.desc      ACE
      */
     it('testdialogDataProp', 0, async function (done) {
         console.info('testdialogDataProp START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.dataProp);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('dataProp')
         expect(obj.$attrs.datadialog).assertEqual(undefined);
         console.info("[dialogProps] get datadialog value is: " + JSON.stringify(obj.$attrs.datadialog));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogDataPropNone
      * @tc.desc      ACE
      */
     it('testdialogDataPropNone', 0, async function (done) {
         console.info('testdialogDataPropNone START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.dataPropNone);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('dataPropNone')
         expect(obj.$attrs.datadialog).assertEqual(undefined)
         console.info("[dialogProps] get datadialog value is: " + JSON.stringify(obj.$attrs.datadialog));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogClickEffectPropSmall
      * @tc.desc      ACE
      */
     it('testdialogClickEffectPropSmall', 0, async function (done) {
         console.info('testdialogClickEffectPropSmall START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
         expect(obj.$attrs.clickEffect).assertEqual('spring-small')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogClickEffectPropMedium
      * @tc.desc      ACE
      */
     it('testdialogClickEffectPropMedium', 0, async function (done) {
         console.info('testdialogClickEffectPropMedium START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
         expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogClickEffectPropLarge
      * @tc.desc      ACE
      */
     it('testdialogClickEffectPropLarge', 0, async function (done) {
         console.info('testdialogClickEffectPropLarge START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
         expect(obj.$attrs.clickEffect).assertEqual('spring-large')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogClickEffectPropNone
      * @tc.desc      ACE
      */
     it('testdialogClickEffectPropNone', 0, async function (done) {
         console.info('testdialogClickEffectPropNone START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.clickEffectPropNone);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
         expect(obj.$attrs.clickEffect).assertEqual(undefined)
         console.info("[dialogProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogDirPropRtl
      * @tc.desc      ACE
      */
     it('testdialogDirPropRtl', 0, async function (done) {
         console.info('testdialogDirPropRtl START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.dirPropRtl);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('dirPropRtl')
         expect(obj.$attrs.dir).assertEqual('rtl')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogDirPropLtr
      * @tc.desc      ACE
      */
     it('testdialogDirPropLtr', 0, async function (done) {
         console.info('testdialogDirPropLtr START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.dirPropLtr);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('dirPropLtr')
         expect(obj.$attrs.dir).assertEqual('ltr')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogDirPropAuto
      * @tc.desc      ACE
      */
     it('testdialogDirPropAuto', 0, async function (done) {
         console.info('testdialogDirPropAuto START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.dirPropAuto);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('dirPropAuto')
         expect(obj.$attrs.dir).assertEqual('auto')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogDirPropNone
      * @tc.desc      ACE
      */
     it('testdialogDirPropNone', 0, async function (done) {
         console.info('testdialogDirPropNone START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.dirPropNone);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('dirPropNone')
         expect(obj.$attrs.dir).assertEqual('auto')
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogForPropNull
      * @tc.desc      ACE
      */
     it('testdialogForPropNull', 0, async function (done) {
         console.info('testdialogForPropNull START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.forPropNull);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('forPropNull')
         expect(obj.$attrs.for).assertEqual(undefined)
         console.info("[dialogProps] get for value is: " + JSON.stringify(obj.$attrs.for));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogForPropOne
      * @tc.desc      ACE
      */
     it('testdialogForPropOne', 0, async function (done) {
         console.info('testdialogForPropOne START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.forPropOne);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('forPropOne')
         expect(obj.$attrs.for).assertEqual(undefined)
         console.info("[dialogProps] get for value is: " + JSON.stringify(obj.$attrs.for));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogForPropThree
      * @tc.desc      ACE
      */
     it('testdialogForPropThree', 0, async function (done) {
         console.info('testdialogForPropThree START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.forPropThree);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('forPropThree')
         expect(obj.$attrs.for).assertEqual(undefined)
         console.info("[dialogProps] get for value is: " + JSON.stringify(obj.$attrs.for));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogIfPropTrue
      * @tc.desc      ACE
      */
     it('testdialogIfPropTrue', 0, async function (done) {
         console.info('testdialogIfPropTrue START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.ifPropTrue);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('ifPropTrue')
         expect(obj.$attrs.if).assertEqual(undefined)
         console.info("[dialogProps] get for value is: " + JSON.stringify(obj.$attrs.if));
         done();
     });
     
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogShowPropTrue
      * @tc.desc      ACE
      */
     it('testdialogShowPropTrue', 0, async function (done) {
         console.info('testdialogShowPropTrue START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.showPropTrue);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('showPropTrue')
         expect(obj.$attrs.show).assertEqual('true')
         console.info("[dialogProps] get show value is: " + JSON.stringify(obj.$attrs.show));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogShowPropFalse
      * @tc.desc      ACE
      */
     it('testdialogShowPropFalse', 0, async function (done) {
         console.info('testdialogShowPropFalse START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.showPropFalse);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('showPropFalse')
         expect(obj.$attrs.show).assertEqual('false')
         console.info("[dialogProps] get show value is: " + JSON.stringify(obj.$attrs.show));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogShowPropNone
      * @tc.desc      ACE
      */
     it('testdialogShowPropNone', 0, async function (done) {
         console.info('testdialogShowPropNone START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.showPropNone);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('showPropNone')
         expect(obj.$attrs.show).assertEqual('true')
         console.info("[dialogProps] get show value is: " + JSON.stringify(obj.$attrs.show));
         done();
     });
 
     // 特有属性
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogdragableNone
      * @tc.desc      ACE
      */
     it('testdialogdragableNone', 0, async function (done) {
         console.info('testdialogdragableNone START');
         console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.dragableNone);
         console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('dialog')
         expect(obj.$attrs.id).assertEqual('dragableNone')
         expect(obj.$attrs.dragable).assertEqual(undefined)
         console.info("[dialogProps] get dragable value is: " + JSON.stringify(obj.$attrs.dragable));
         done();
     });

     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogdragableTrue
      * @tc.desc      ACE
      */
      it('testdialogdragableTrue', 0, async function (done) {
        console.info('testdialogdragableTrue START');
        console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dragableTrue);
        console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('dialog')
        expect(obj.$attrs.id).assertEqual('dragableTrue')
        expect(obj.$attrs.dragable).assertEqual('true')
        console.info("[dialogProps] get dragable value is: " + JSON.stringify(obj.$attrs.dragable));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testdialogdragableFalse
      * @tc.desc      ACE
      */
     it('testdialogdragableFalse', 0, async function (done) {
        console.info('testdialogdragableFalse START');
        console.info("[dialogProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dragableFalse);
        console.info("[dialogProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[dialogProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('dialog')
        expect(obj.$attrs.id).assertEqual('dragableFalse')
        expect(obj.$attrs.dragable).assertEqual('false')
        console.info("[dialogProps] get dragable value is: " + JSON.stringify(obj.$attrs.dragable));
        done();
    });
 });
 