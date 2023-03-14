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
 
 
 describe('polylinePropsJsTest', function () {
 
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
         console.info('[polylinePropsJsTest] before each called')
 
         let result;
         let options = {
             uri: 'pages/polyline/prop/index'
         }
         try {
             result = router.push(options)
             console.info("push polylineProps page success " + JSON.stringify(result));
         } catch (err) {
             console.error("push polylineProps page error " + JSON.stringify(result));
         }
         await sleep(4000)
         done()
     });
 
     /**
     * run after testcase
     */
     afterAll(async function () {
         console.info('[polylinePropsJsTest] after each called')
         await backToIndex()
         await sleep(1000)
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineFillProp
      * @tc.desc      ACE
      */
     it('testPolylineFillProp', 0, async function (done) {
         console.info('testPolylineFillProp START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.fillProp);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('fillProp')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[polylineProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineFillOpacityPropOne
      * @tc.desc      ACE
      */
     it('testPolylineFillOpacityPropOne', 0, async function (done) {
         console.info('testPolylineFillOpacityPropOne START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.fillOpacityPropOne);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('fillOpacityPropOne')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[polylineProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.fillOpacity).assertEqual('0.5')
         console.info("[polylineProps] get inspector fill-opacity value is: " + JSON.stringify(obj.$attrs.fillOpacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineFillOpacityPropTwo
      * @tc.desc      ACE
      */
     it('testPolylineFillOpacityPropTwo', 0, async function (done) {
         console.info('testPolylineFillOpacityPropTwo START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.fillOpacityPropTwo);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('fillOpacityPropTwo')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[polylineProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.fillOpacity).assertEqual('1')
         console.info("[polylineProps] get inspector fill-opacity value is: " + JSON.stringify(obj.$attrs.fillOpacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineFillOpacityPropThree
      * @tc.desc      ACE
      */
     it('testPolylineFillOpacityPropThree', 0, async function (done) {
         console.info('testPolylineFillOpacityPropThree START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.fillOpacityPropThree);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('fillOpacityPropThree')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[polylineProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.fillOpacity).assertEqual('1.5')
         console.info("[polylineProps] get inspector fill-opacity value is: " + JSON.stringify(obj.$attrs.fillOpacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineFillRulePropOne
      * @tc.desc      ACE
      */
     it('testPolylineFillRulePropOne', 0, async function (done) {
         console.info('testPolylineFillRulePropOne START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.fillRulePropOne);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('fillRulePropOne')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[polylineProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.fillRule).assertEqual('evenodd')
         console.info("[polylineProps] get inspector fill-rule value is: " + JSON.stringify(obj.$attrs.fillRule));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineFillRulePropTwo
      * @tc.desc      ACE
      */
     it('testPolylineFillRulePropTwo', 0, async function (done) {
         console.info('testPolylineFillRulePropTwo START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.fillRulePropTwo);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('fillRulePropTwo')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[polylineProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.fillRule).assertEqual('nonzero')
         console.info("[polylineProps] get inspector fill-rule value is: " + JSON.stringify(obj.$attrs.fillRule));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineOpacityPropOne
      * @tc.desc      ACE
      */
     it('testPolylineOpacityPropOne', 0, async function (done) {
         console.info('testPolylineOpacityPropOne START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.opacityPropOne);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('opacityPropOne')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[polylineProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.opacity).assertEqual('0.5')
         console.info("[polylineProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.opacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineOpacityPropTwo
      * @tc.desc      ACE
      */
     it('testPolylineOpacityPropTwo', 0, async function (done) {
         console.info('testPolylineOpacityPropTwo START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.opacityPropTwo);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('opacityPropTwo')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[polylineProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.opacity).assertEqual('1')
         console.info("[polylineProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.opacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineOpacityPropThree
      * @tc.desc      ACE
      */
     it('testPolylineOpacityPropThree', 0, async function (done) {
         console.info('testPolylineOpacityPropThree START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.opacityPropThree);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('opacityPropThree')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[polylineProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.opacity).assertEqual('1.5')
         console.info("[polylineProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.opacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineStrokeProp
      * @tc.desc      ACE
      */
     it('testPolylineStrokeProp', 0, async function (done) {
         console.info('testPolylineStrokeProp START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeProp);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeProp')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[polylineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineStrokeDashArrayProp
      * @tc.desc      ACE
      */
     it('testPolylineStrokeDashArrayProp', 0, async function (done) {
         console.info('testPolylineStrokeDashArrayProp START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeDashArrayProp);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeDashArrayProp')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[polylineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeDasharray).assertEqual('2 3')
         console.info("[polylineProps] get ins dasharray value is: " + JSON.stringify(obj.$attrs.strokeDasharray));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineStrokeDashOffsetProp
      * @tc.desc      ACE
      */
     it('testPolylineStrokeDashOffsetProp', 0, async function (done) {
         console.info('testPolylineStrokeDashOffsetProp START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeDashOffsetProp);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeDashOffsetProp')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[polylineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeDashoffset).assertEqual('23')
         console.info("[polylineProps] get ins dashoffset value is: " + JSON.stringify(obj.$attrs.strokeDashoffset));
         done();
     });
 
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineStrokeLineJoinPropOne
      * @tc.desc      ACE
      */
     it('testPolylineStrokeLineJoinPropOne', 0, async function (done) {
         console.info('testPolylineStrokeLineJoinPropOne START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineJoinPropOne);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineJoinPropOne')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[polylineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLinejoin).assertEqual('bevel')
         console.info("[polylineProps] get inspector linejoin value is: " + JSON.stringify(obj.$attrs.strokeLinejoin));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineStrokeLineJoinPropTwo
      * @tc.desc      ACE
      */
     it('testPolylineStrokeLineJoinPropTwo', 0, async function (done) {
         console.info('testPolylineStrokeLineJoinPropTwo START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineJoinPropTwo);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineJoinPropTwo')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[polylineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLinejoin).assertEqual('miter')
         console.info("[polylineProps] get inspector linejoin value is: " + JSON.stringify(obj.$attrs.strokeLinejoin));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineStrokeLineJoinPropThree
      * @tc.desc      ACE
      */
     it('testPolylineStrokeLineJoinPropThree', 0, async function (done) {
         console.info('testPolylineStrokeLineJoinPropThree START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineJoinPropThree);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineJoinPropThree')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[polylineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLinejoin).assertEqual('round')
         console.info("[polylineProps] get inspector linejoin value is: " + JSON.stringify(obj.$attrs.strokeLinejoin));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineStrokeLineJoinPropOne
      * @tc.desc      ACE
      */
     it('testPolylineStrokeLineCapPropOne', 0, async function (done) {
         console.info('testPolylineStrokeLineCapPropOne START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineCapPropOne);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineCapPropOne')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[polylineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLineCap).assertEqual('butt')
         console.info("[polylineProps] get inspector lineCap value is: " + JSON.stringify(obj.$attrs.strokeLineCap));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineStrokeLineCapPropTwo
      * @tc.desc      ACE
      */
     it('testPolylineStrokeLineCapPropTwo', 0, async function (done) {
         console.info('testPolylineStrokeLineCapPropTwo START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineCapPropTwo);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineCapPropTwo')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[polylineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLineCap).assertEqual('round')
         console.info("[polylineProps] get inspector lineCap value is: " + JSON.stringify(obj.$attrs.strokeLineCap));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineStrokeLineCapPropThree
      * @tc.desc      ACE
      */
     it('testPolylineStrokeLineCapPropThree', 0, async function (done) {
         console.info('testPolylineStrokeLineCapPropThree START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineCapPropThree);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineCapPropThree')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[polylineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLineCap).assertEqual('square')
         console.info("[polylineProps] get inspector lineCap value is: " + JSON.stringify(obj.$attrs.strokeLineCap));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineStrokeMiterLimitProp
      * @tc.desc      ACE
      */
     it('testPolylineStrokeMiterLimitProp', 0, async function (done) {
         console.info('testPolylineStrokeMiterLimitProp START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeMiterLimitProp);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeMiterLimitProp')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[polylineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeMiterlimit).assertEqual('10')
         console.info("[polylineProps] get ins miterlimit value is: " + JSON.stringify(obj.$attrs.strokeMiterlimit));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineStrokeOpacityPropOne
      * @tc.desc      ACE
      */
     it('testPolylineStrokeOpacityPropOne', 0, async function (done) {
         console.info('testPolylineStrokeOpacityPropOne START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeOpacityPropOne);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeOpacityPropOne')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[polylineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeOpacity).assertEqual('0.5')
         console.info("[polylineProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineStrokeOpacityPropTwo
      * @tc.desc      ACE
      */
     it('testPolylineStrokeOpacityPropTwo', 0, async function (done) {
         console.info('testPolylineStrokeOpacityPropTwo START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeOpacityPropTwo);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeOpacityPropTwo')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[polylineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeOpacity).assertEqual('1')
         console.info("[polylineProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineStrokeOpacityPropThree
      * @tc.desc      ACE
      */
     it('testPolylineStrokeOpacityPropThree', 0, async function (done) {
         console.info('testPolylineStrokeOpacityPropThree START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeOpacityPropThree);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeOpacityPropThree')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[polylineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeOpacity).assertEqual('1.5')
         console.info("[polylineProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineStrokeWidthProp
      * @tc.desc      ACE
      */
     it('testPolylineStrokeWidthProp', 0, async function (done) {
         console.info('testPolylineStrokeWidthProp START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeWidthProp);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeWidthProp')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[polylineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeWidth).assertEqual('6')
         console.info("[polylineProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineTransformProp
      * @tc.desc      ACE
      */
     it('testPolylineTransformProp', 0, async function (done) {
         console.info('testPolylineTransformProp START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.transformProp);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('transformProp')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.transform).assertEqual('translate(10,5)scale(0.5 1.3)rotate(20 40 90)skewX(10)skewY(-15)')
         console.info("[polylineProps] get inspector transform value is: " + JSON.stringify(obj.$attrs.transform));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylineIdProp
      * @tc.desc      ACE
      */
      it('testPolylineIdProp', 0, async function (done) {
         console.info('testPolylineIdProp START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.idProp);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('idProp')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[polylineProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testPolylinePointProp
      * @tc.desc      ACE
      */
      it('testPolylinePointProp', 0, async function (done) {
         console.info('testPolylinePointProp START');
         console.info("[polylineProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.pointProp);
         console.info("[polylineProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[polylineProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('polyline')
         console.info("[polylineProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('pointProp')
         console.info("[polylineProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
         console.info("[polylineProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[polylineProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.stroke).assertEqual('blue')
         console.info("[polylineProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         done();
     });
 });
 