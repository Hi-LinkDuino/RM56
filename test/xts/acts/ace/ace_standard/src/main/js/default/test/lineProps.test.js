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
 import { describe, beforeAll, afterAll, it, expect } from 'deccjsunit/index';
 
 
 describe('linePropsJsTest', function () {
 
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
         console.info('[linePropsJsTest] before each called')
 
         let result;
         let options = {
             uri: 'pages/line/prop/index'
         }
         try {
             result = router.push(options)
             console.info("push lineProps page success " + JSON.stringify(result));
         } catch (err) {
             console.error("push lineProps page error " + JSON.stringify(result));
         }
         await sleep(4000)
         done()
     });
 
     /**
     * run after testcase
     */
     afterAll(async function () {
         console.info('[linePropsJsTest] after each called')
         await backToIndex()
         await sleep(1000)
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineOpacityPropOne
      * @tc.desc      ACE
      */
     it('testLineOpacityPropOne', 0, async function (done) {
         console.info('testLineOpacityPropOne START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.opacityPropOne);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('opacityPropOne')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.opacity).assertEqual('0.5')
         console.info("[lineProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.opacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineOpacityPropTwo
      * @tc.desc      ACE
      */
     it('testLineOpacityPropTwo', 0, async function (done) {
         console.info('testLineOpacityPropTwo START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.opacityPropTwo);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('opacityPropTwo')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.opacity).assertEqual('1')
         console.info("[lineProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.opacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineOpacityPropThree
      * @tc.desc      ACE
      */
     it('testLineOpacityPropThree', 0, async function (done) {
         console.info('testLineOpacityPropThree START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.opacityPropThree);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('opacityPropThree')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.opacity).assertEqual('1.5')
         console.info("[lineProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.opacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineStrokeProp
      * @tc.desc      ACE
      */
     it('testLineStrokeProp', 0, async function (done) {
         console.info('testLineStrokeProp START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeProp);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeProp')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineStrokeDashArrayProp
      * @tc.desc      ACE
      */
     it('testLineStrokeDashArrayProp', 0, async function (done) {
         console.info('testLineStrokeDashArrayProp START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeDashArrayProp);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeDashArrayProp')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeDasharray).assertEqual('2 3')
         console.info("[lineProps] get stroke-dasharray value is: " + JSON.stringify(obj.$attrs.strokeDasharray));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineStrokeDashOffsetProp
      * @tc.desc      ACE
      */
     it('testLineStrokeDashOffsetProp', 0, async function (done) {
         console.info('testLineStrokeDashOffsetProp START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeDashOffsetProp);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeDashOffsetProp')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeDashoffset).assertEqual('23')
         console.info("[lineProps] get stroke-dashoffset value is: " + JSON.stringify(obj.$attrs.strokeDashoffset));
         done();
     });
 
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineStrokeLineJoinPropOne
      * @tc.desc      ACE
      */
     it('testLineStrokeLineJoinPropOne', 0, async function (done) {
         console.info('testLineStrokeLineJoinPropOne START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineJoinPropOne);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineJoinPropOne')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLinejoin).assertEqual('bevel')
         console.info("[lineProps] get stroke-linejoin value is: " + JSON.stringify(obj.$attrs.strokeLinejoin));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineStrokeLineJoinPropTwo
      * @tc.desc      ACE
      */
     it('testLineStrokeLineJoinPropTwo', 0, async function (done) {
         console.info('testLineStrokeLineJoinPropTwo START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineJoinPropTwo);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineJoinPropTwo')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLinejoin).assertEqual('miter')
         console.info("[lineProps] get stroke-linejoin value is: " + JSON.stringify(obj.$attrs.strokeLinejoin));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineStrokeLineJoinPropThree
      * @tc.desc      ACE
      */
     it('testLineStrokeLineJoinPropThree', 0, async function (done) {
         console.info('testLineStrokeLineJoinPropThree START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineJoinPropThree);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineJoinPropThree')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLinejoin).assertEqual('round')
         console.info("[lineProps] get stroke-linejoin value is: " + JSON.stringify(obj.$attrs.strokeLinejoin));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineStrokeLineJoinPropOne
      * @tc.desc      ACE
      */
     it('testLineStrokeLineCapPropOne', 0, async function (done) {
         console.info('testLineStrokeLineCapPropOne START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineCapPropOne);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineCapPropOne')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLineCap).assertEqual('butt')
         console.info("[lineProps] get inspector stroke-lineCap value is: " + JSON.stringify(obj.$attrs.strokeLineCap));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineStrokeLineCapPropTwo
      * @tc.desc      ACE
      */
     it('testLineStrokeLineCapPropTwo', 0, async function (done) {
         console.info('testLineStrokeLineCapPropTwo START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineCapPropTwo);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineCapPropTwo')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLineCap).assertEqual('round')
         console.info("[lineProps] get inspector stroke-lineCap value is: " + JSON.stringify(obj.$attrs.strokeLineCap));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineStrokeLineCapPropThree
      * @tc.desc      ACE
      */
     it('testLineStrokeLineCapPropThree', 0, async function (done) {
         console.info('testLineStrokeLineCapPropThree START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineCapPropThree);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineCapPropThree')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLineCap).assertEqual('square')
         console.info("[lineProps] get inspector stroke-lineCap value is: " + JSON.stringify(obj.$attrs.strokeLineCap));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineStrokeMiterLimitProp
      * @tc.desc      ACE
      */
     it('testLineStrokeMiterLimitProp', 0, async function (done) {
         console.info('testLineStrokeMiterLimitProp START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeMiterLimitProp);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeMiterLimitProp')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeMiterlimit).assertEqual('10')
         console.info("[lineProps] get stroke-miterlimit value is: " + JSON.stringify(obj.$attrs.strokeMiterlimit));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineStrokeOpacityPropOne
      * @tc.desc      ACE
      */
     it('testLineStrokeOpacityPropOne', 0, async function (done) {
         console.info('testLineStrokeOpacityPropOne START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeOpacityPropOne);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeOpacityPropOne')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeOpacity).assertEqual('0.5')
         console.info("[lineProps] get inspector stroke-opacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineStrokeOpacityPropTwo
      * @tc.desc      ACE
      */
     it('testLineStrokeOpacityPropTwo', 0, async function (done) {
         console.info('testLineStrokeOpacityPropTwo START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeOpacityPropTwo);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeOpacityPropTwo')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeOpacity).assertEqual('1')
         console.info("[lineProps] get inspector stroke-opacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineStrokeOpacityPropThree
      * @tc.desc      ACE
      */
     it('testLineStrokeOpacityPropThree', 0, async function (done) {
         console.info('testLineStrokeOpacityPropThree START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeOpacityPropThree);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeOpacityPropThree')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeOpacity).assertEqual('1.5')
         console.info("[lineProps] get inspector stroke-opacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineStrokeWidthProp
      * @tc.desc      ACE
      */
     it('testLineStrokeWidthProp', 0, async function (done) {
         console.info('testLineStrokeWidthProp START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeWidthProp);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeWidthProp')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeWidth).assertEqual('6')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineTransformProp
      * @tc.desc      ACE
      */
     it('testLineTransformProp', 0, async function (done) {
         console.info('testLineTransformProp START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.transformProp);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('transformProp')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('50')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('50')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.transform).assertEqual('translate(10,5) scale(0.5 1.3) rotate(20 40 90) ' +
         'skewX(10) skewY(-15)')
         console.info("[lineProps] get inspector transform value is: " + JSON.stringify(obj.$attrs.transform));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineIdProp
      * @tc.desc      ACE
      */
     it('testLineIdProp', 0, async function (done) {
         console.info('testLineIdProp START');
         console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.idProp);
         console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('line')
         console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('idProp')
         console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.x1).assertEqual('10')
         console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
         expect(obj.$attrs.x2).assertEqual('50')
         console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
         expect(obj.$attrs.y1).assertEqual('10')
         console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
         expect(obj.$attrs.y2).assertEqual('10')
         console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
         expect(obj.$attrs.strokeWidth).assertEqual('4')
         console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         done();
     });

     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineX1Prop
      * @tc.desc      ACE
      */
      it('testLineX1Prop', 0, async function (done) {
        console.info('testLineX1Prop START');
        console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.x1Prop);
        console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('line')
        console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('x1Prop')
        console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.x1).assertEqual('10')
        console.info("[lineProps] get inspector x1 value is: " + JSON.stringify(obj.$attrs.x1));
        expect(obj.$attrs.strokeWidth).assertEqual('4')
        console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        done();
    });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineX2Prop
      * @tc.desc      ACE
      */
      it('testLineX1Prop', 0, async function (done) {
        console.info('testLineX1Prop START');
        console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.x2Prop);
        console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('line')
        console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('x2Prop')
        console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.x2).assertEqual('100')
        console.info("[lineProps] get inspector x2 value is: " + JSON.stringify(obj.$attrs.x2));
        expect(obj.$attrs.strokeWidth).assertEqual('4')
        console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineY1Prop
      * @tc.desc      ACE
      */
     it('testLineY1Prop', 0, async function (done) {
        console.info('testLineY1Prop START');
        console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.y1Prop);
        console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('line')
        console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('y1Prop')
        console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.y1).assertEqual('10')
        console.info("[lineProps] get inspector y1 value is: " + JSON.stringify(obj.$attrs.y1));
        expect(obj.$attrs.strokeWidth).assertEqual('4')
        console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testLineY2Prop
      * @tc.desc      ACE
      */
     it('testLineY2Prop', 0, async function (done) {
        console.info('testLineY2Prop START');
        console.info("[lineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.y2Prop);
        console.info("[lineProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[lineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('line')
        console.info("[lineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('y2Prop')
        console.info("[lineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.y2).assertEqual('10')
        console.info("[lineProps] get inspector y2 value is: " + JSON.stringify(obj.$attrs.y2));
        expect(obj.$attrs.strokeWidth).assertEqual('4')
        console.info("[lineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[lineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        done();
    });
 });
 